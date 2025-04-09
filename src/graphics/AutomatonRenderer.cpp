#include "AutomatonRenderer.hpp"

#include "graphics/vulkan/Buffer.hpp"
#include "graphics/vulkan/Util.hpp"
#include "util/Util.hpp"

namespace cltv {

AutomatonRenderer::AutomatonRenderer(const DeviceContext* ctx, Window& window, const RendererParams& params) :
	_ctx(ctx),
	_window_size(window.get_viewport()),
	_delay_milliseconds(params.delay_milliseconds),
	_simulation_pass(_ctx, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
	_presentation_pass(_ctx, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
	_swap_chain(_ctx, _window_size, window, _presentation_pass),
	_front_target(_ctx, params.universe_size, _simulation_pass),
	_back_target(_ctx, params.universe_size, _simulation_pass),
	_vertex_shader(create_shader_module("ScreenQuad.vert.spv")),
	_simulation_fragment_shader(create_shader_module(params.simulation_shader_path)),
	_presentation_fragment_shader(create_shader_module("Presentation.frag.spv")),
	_descriptor_set_layout(create_descriptor_set_layout()),
	_pipeline_layout(create_pipeline_layout()),
	_simulation_pipeline(_ctx, _vertex_shader, _simulation_fragment_shader, _pipeline_layout, _simulation_pass),
	_presentation_pipeline(_ctx, _vertex_shader, _presentation_fragment_shader, _pipeline_layout, _presentation_pass),
	_sampler(create_sampler()),
	_descriptor_pool(create_descriptor_pool()),
	_descriptor_set_front(create_descriptor_set(_front_target)),
	_descriptor_set_back(create_descriptor_set(_back_target)),
	_frame_fences {create_fence(), create_fence()},
	_img_release_semaphores {create_semaphore(), create_semaphore()},
	_img_acquire_semaphores {create_semaphore(), create_semaphore()},
	_image_pending_fences(_swap_chain.get_image_count(), VK_NULL_HANDLE) {
	for (size_t i = 0; i < _swap_chain.get_image_count(); ++i) {
		_cmd_lists.emplace_back(_ctx);
	}

	prepare_render_targets(params.initial_live_cell_incidence);
}

AutomatonRenderer::~AutomatonRenderer() {
	VkResult result = _ctx->lib.vkDeviceWaitIdle(_ctx->device());
	require_vk_result(result, "failed to wait for device idle state");

	for (VkSemaphore semaphore : _img_acquire_semaphores) {
		_ctx->lib.vkDestroySemaphore(_ctx->device(), semaphore, nullptr);
	}
	for (VkSemaphore semaphore : _img_release_semaphores) {
		_ctx->lib.vkDestroySemaphore(_ctx->device(), semaphore, nullptr);
	}
	for (VkFence fence : _frame_fences) {
		_ctx->lib.vkDestroyFence(_ctx->device(), fence, nullptr);
	}

	_ctx->lib.vkDestroyDescriptorPool(_ctx->device(), _descriptor_pool, nullptr);
	_ctx->lib.vkDestroySampler(_ctx->device(), _sampler, nullptr);

	_ctx->lib.vkDestroyPipelineLayout(_ctx->device(), _pipeline_layout, nullptr);
	_ctx->lib.vkDestroyDescriptorSetLayout(_ctx->device(), _descriptor_set_layout, nullptr);
	_ctx->lib.vkDestroyShaderModule(_ctx->device(), _presentation_fragment_shader, nullptr);
	_ctx->lib.vkDestroyShaderModule(_ctx->device(), _simulation_fragment_shader, nullptr);
	_ctx->lib.vkDestroyShaderModule(_ctx->device(), _vertex_shader, nullptr);
}

void AutomatonRenderer::draw_frame() {
	VkFence frame_fence			  = _frame_fences[_current_frame];
	VkSemaphore acquire_semaphore = _img_acquire_semaphores[_current_frame];
	VkSemaphore release_semaphore = _img_release_semaphores[_current_frame];

	VkResult result = _ctx->lib.vkWaitForFences(_ctx->device(), 1, &frame_fence, VK_TRUE, UINT64_MAX);
	require_vk_result(result, "failed to wait for current frame fence");

	uint32_t image_index = _swap_chain.get_next_image_index(acquire_semaphore);
	record_commands(image_index);

	VkFence image_pending_fence = _image_pending_fences[image_index];
	if (image_pending_fence != VK_NULL_HANDLE) {
		result = _ctx->lib.vkWaitForFences(_ctx->device(), 1, &image_pending_fence, VK_TRUE, UINT64_MAX);
		require_vk_result(result, "failed to wait for image-pending fence");
	}

	_image_pending_fences[image_index] = frame_fence;
	_ctx->lib.vkResetFences(_ctx->device(), 1, &frame_fence);

	VkCommandBuffer cmd_buffer = _cmd_lists[image_index].get();
	_ctx->submit_to_queue(_ctx->graphics_queue, cmd_buffer, acquire_semaphore, release_semaphore, frame_fence);

	_swap_chain.present(image_index, release_semaphore);
	_current_frame = (_current_frame + 1) % MaxFrames;

	std::this_thread::sleep_for(std::chrono::milliseconds(_delay_milliseconds));
}

void AutomatonRenderer::prepare_render_targets(uint32_t initial_live_cell_incidence) {
	::srand(static_cast<unsigned>(::time(nullptr)));

	const size_t size = 4 * static_cast<size_t>(_back_target.get_size().area());
	Buffer staging_buffer(_ctx, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* staging_target;
	VkResult result = _ctx->lib.vkMapMemory(_ctx->device(), staging_buffer.get_memory(), 0, size, 0, &staging_target);
	require_vk_result(result, "failed to map memory");

	uint32_t* pixels = reinterpret_cast<uint32_t*>(staging_target);
	for (size_t i = 0; i < size / sizeof(uint32_t); ++i) {
		*pixels++ = static_cast<uint32_t>(::rand()) % initial_live_cell_incidence == 0 ? 0xFFFFFFFF : 0;
	}
	_ctx->lib.vkUnmapMemory(_ctx->device(), staging_buffer.get_memory());

	CommandList cmd(_ctx);
	cmd.begin();
	cmd.transition_render_target(_back_target, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	cmd.end();
	_ctx->submit_to_queue_blocking(_ctx->graphics_queue, cmd.get());

	cmd.begin();
	cmd.copy_buffer_to_render_target(staging_buffer, _back_target);
	cmd.end();
	_ctx->submit_to_queue_blocking(_ctx->graphics_queue, cmd.get());

	cmd.begin();
	cmd.transition_render_target(_back_target, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	cmd.end();
	_ctx->submit_to_queue_blocking(_ctx->graphics_queue, cmd.get());
}

void AutomatonRenderer::record_commands(uint32_t image_index) {
	static bool x = false;

	CommandList& cmd	 = _cmd_lists[image_index];
	RenderTarget& target = _current_frame == 0 ? _front_target : _back_target;

	cmd.begin();
	cmd.transition_render_target(target, x ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED,
								 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	x = true;

	cmd.begin_render_pass(target.get_size(), _simulation_pass, target.get_framebuffer());
	cmd.bind_viewport(target.get_size());
	cmd.bind_scissor(target.get_size());
	cmd.bind_descriptor_set(_pipeline_layout, _current_frame == 0 ? _descriptor_set_back : _descriptor_set_front);
	cmd.bind_pipeline(_simulation_pipeline);
	cmd.draw(3);
	cmd.end_render_pass();

	cmd.transition_render_target(target, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	cmd.begin_render_pass(_window_size, _presentation_pass, _swap_chain.get_framebuffer(image_index));
	cmd.bind_viewport(_window_size);
	cmd.bind_scissor(_window_size);
	cmd.bind_descriptor_set(_pipeline_layout, _current_frame == 0 ? _descriptor_set_front : _descriptor_set_back);
	cmd.bind_pipeline(_presentation_pipeline);
	cmd.draw(3);
	cmd.end_render_pass();

	cmd.end();
}

VkShaderModule AutomatonRenderer::create_shader_module(const char* path) const {
	std::optional<std::vector<char>> bytecode = get_all_file_bytes(path);
	require(bytecode.has_value(), "shader file '" + std::string(path) + "' not found");

	VkShaderModuleCreateInfo shader_info {
		.sType	  = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext	  = nullptr,
		.flags	  = 0,
		.codeSize = bytecode->size(),
		.pCode	  = reinterpret_cast<uint32_t*>(bytecode->data()),
	};

	VkShaderModule shader_module;
	VkResult result = _ctx->lib.vkCreateShaderModule(_ctx->device(), &shader_info, nullptr, &shader_module);
	require_vk_result(result, "failed to create Vulkan shader module");
	return shader_module;
}

VkDescriptorSetLayout AutomatonRenderer::create_descriptor_set_layout() const {
	VkDescriptorSetLayoutBinding binding {
		.binding			= 0,
		.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount	= 1,
		.stageFlags			= VK_SHADER_STAGE_FRAGMENT_BIT,
		.pImmutableSamplers = nullptr,
	};
	VkDescriptorSetLayoutCreateInfo layout_info {
		.sType		  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext		  = nullptr,
		.flags		  = 0,
		.bindingCount = 1,
		.pBindings	  = &binding,
	};

	VkDescriptorSetLayout descriptor_set_layout;
	VkResult result = _ctx->lib.vkCreateDescriptorSetLayout(_ctx->device(), &layout_info, nullptr, &descriptor_set_layout);
	require_vk_result(result, "failed to create Vulkan descriptor set layout");
	return descriptor_set_layout;
}

VkPipelineLayout AutomatonRenderer::create_pipeline_layout() const {
	VkPipelineLayoutCreateInfo pipeline_layout_info {
		.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext					= nullptr,
		.flags					= 0,
		.setLayoutCount			= 1,
		.pSetLayouts			= &_descriptor_set_layout,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges	= nullptr,
	};

	VkPipelineLayout pipeline_layout;
	VkResult result = _ctx->lib.vkCreatePipelineLayout(_ctx->device(), &pipeline_layout_info, nullptr, &pipeline_layout);
	require_vk_result(result, "failed to create Vulkan pipeline layout");
	return pipeline_layout;
}

VkSampler AutomatonRenderer::create_sampler() const {
	VkSamplerCreateInfo sampler_info {
		.sType					 = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext					 = nullptr,
		.flags					 = 0,
		.magFilter				 = VK_FILTER_NEAREST,
		.minFilter				 = VK_FILTER_NEAREST,
		.mipmapMode				 = VK_SAMPLER_MIPMAP_MODE_NEAREST,
		.addressModeU			 = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		.addressModeV			 = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		.addressModeW			 = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		.mipLodBias				 = 0,
		.anisotropyEnable		 = VK_TRUE,
		.maxAnisotropy			 = 16.0f,
		.compareEnable			 = VK_FALSE,
		.compareOp				 = VK_COMPARE_OP_ALWAYS,
		.minLod					 = 0,
		.maxLod					 = 0,
		.borderColor			 = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE,
	};

	VkSampler sampler;
	VkResult result = _ctx->lib.vkCreateSampler(_ctx->device(), &sampler_info, nullptr, &sampler);
	require_vk_result(result, "failed to create Vulkan sampler");
	return sampler;
}

VkDescriptorPool AutomatonRenderer::create_descriptor_pool() const {
	VkDescriptorPoolSize pool_size {
		.type			 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount = 2,
	};
	VkDescriptorPoolCreateInfo descriptor_pool_info {
		.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext		   = nullptr,
		.flags		   = 0,
		.maxSets	   = 2,
		.poolSizeCount = 1,
		.pPoolSizes	   = &pool_size,
	};

	VkDescriptorPool descriptor_pool;
	VkResult result = _ctx->lib.vkCreateDescriptorPool(_ctx->device(), &descriptor_pool_info, nullptr, &descriptor_pool);
	require_vk_result(result, "failed to create Vulkan descriptor pool");
	return descriptor_pool;
}

VkDescriptorSet AutomatonRenderer::create_descriptor_set(const RenderTarget& render_target) const {
	VkDescriptorSetAllocateInfo alloc_info {
		.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext				= nullptr,
		.descriptorPool		= _descriptor_pool,
		.descriptorSetCount = 1,
		.pSetLayouts		= &_descriptor_set_layout,
	};
	VkDescriptorSet descriptor_set;
	VkResult result = _ctx->lib.vkAllocateDescriptorSets(_ctx->device(), &alloc_info, &descriptor_set);
	require_vk_result(result, "failed to create Vulkan descriptor set");

	VkDescriptorImageInfo image_info {
		.sampler	 = _sampler,
		.imageView	 = render_target.get_image_view(),
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
	};
	VkWriteDescriptorSet write {
		.sType			  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.pNext			  = nullptr,
		.dstSet			  = descriptor_set,
		.dstBinding		  = 0,
		.descriptorCount  = 1,
		.descriptorType	  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.pImageInfo		  = &image_info,
		.pBufferInfo	  = nullptr,
		.pTexelBufferView = nullptr,
	};
	_ctx->lib.vkUpdateDescriptorSets(_ctx->device(), 1, &write, 0, nullptr);

	return descriptor_set;
}

VkFence AutomatonRenderer::create_fence() const {
	VkFenceCreateInfo fence_info {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};
	VkFence fence;
	VkResult result = _ctx->lib.vkCreateFence(_ctx->device(), &fence_info, nullptr, &fence);
	require_vk_result(result, "failed to create Vulkan fence");
	return fence;
}

VkSemaphore AutomatonRenderer::create_semaphore() const {
	VkSemaphoreCreateInfo semaphore_info {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
	};
	VkSemaphore semaphore;
	VkResult result = _ctx->lib.vkCreateSemaphore(_ctx->device(), &semaphore_info, nullptr, &semaphore);
	require_vk_result(result, "failed to create Vulkan semaphore");
	return semaphore;
}

} // namespace cltv
