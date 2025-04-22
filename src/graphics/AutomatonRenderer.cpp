#include "AutomatonRenderer.hpp"

#include "graphics/vulkan/Buffer.hpp"
#include "graphics/vulkan/Util.hpp"
#include "util/Util.hpp"

namespace cltv {

AutomatonRenderer::AutomatonRenderer(const DeviceContext* ctx, const Window* window, const RendererParams& params) :
	ctx_(ctx),
	delay_milliseconds_(params.delay_milliseconds),
	simulation_pass_(ctx_, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
	presentation_pass_(ctx_, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
	swap_chain_(ctx_, window, presentation_pass_),
	front_target_(ctx_, params.universe_size, simulation_pass_),
	back_target_(ctx_, params.universe_size, simulation_pass_),
	vertex_shader_(create_shader_module("ScreenQuad.vert.spv")),
	simulation_fragment_shader_(create_shader_module(params.simulation_shader_path)),
	presentation_fragment_shader_(create_shader_module("Presentation.frag.spv")),
	descriptor_set_layout_(create_descriptor_set_layout()),
	pipeline_layout_(create_pipeline_layout()),
	simulation_pipeline_(ctx_, vertex_shader_, simulation_fragment_shader_, pipeline_layout_, simulation_pass_),
	presentation_pipeline_(ctx_, vertex_shader_, presentation_fragment_shader_, pipeline_layout_, presentation_pass_),
	sampler_(create_sampler()),
	descriptor_pool_(create_descriptor_pool()),
	descriptor_set_front_(create_descriptor_set(front_target_)),
	descriptor_set_back_(create_descriptor_set(back_target_)),
	frame_fences_ {create_fence(), create_fence()},
	img_release_semaphores_ {create_semaphore(), create_semaphore()},
	img_acquire_semaphores_ {create_semaphore(), create_semaphore()},
	image_pending_fences_(swap_chain_.get_image_count(), VK_NULL_HANDLE) {
	// prepare as many command lists as there are swap chain images
	for (size_t i = 0; i < swap_chain_.get_image_count(); ++i) {
		cmd_lists_.emplace_back(ctx_);
	}

	prepare_render_targets(params.initial_live_cell_incidence);
}

AutomatonRenderer::~AutomatonRenderer() {
	VkResult result = ctx_->lib.vkDeviceWaitIdle(ctx_->device());
	require_vk_result(result, "failed to wait for device idle state");

	for (VkSemaphore semaphore : img_acquire_semaphores_) {
		ctx_->lib.vkDestroySemaphore(ctx_->device(), semaphore, nullptr);
	}
	for (VkSemaphore semaphore : img_release_semaphores_) {
		ctx_->lib.vkDestroySemaphore(ctx_->device(), semaphore, nullptr);
	}
	for (VkFence fence : frame_fences_) {
		ctx_->lib.vkDestroyFence(ctx_->device(), fence, nullptr);
	}

	ctx_->lib.vkDestroyDescriptorPool(ctx_->device(), descriptor_pool_, nullptr);
	ctx_->lib.vkDestroySampler(ctx_->device(), sampler_, nullptr);

	ctx_->lib.vkDestroyPipelineLayout(ctx_->device(), pipeline_layout_, nullptr);
	ctx_->lib.vkDestroyDescriptorSetLayout(ctx_->device(), descriptor_set_layout_, nullptr);
	ctx_->lib.vkDestroyShaderModule(ctx_->device(), presentation_fragment_shader_, nullptr);
	ctx_->lib.vkDestroyShaderModule(ctx_->device(), simulation_fragment_shader_, nullptr);
	ctx_->lib.vkDestroyShaderModule(ctx_->device(), vertex_shader_, nullptr);
}

void AutomatonRenderer::draw_frame() {
	VkFence frame_fence			  = frame_fences_[current_frame_];
	VkSemaphore acquire_semaphore = img_acquire_semaphores_[current_frame_];
	VkSemaphore release_semaphore = img_release_semaphores_[current_frame_];

	VkResult result = ctx_->lib.vkWaitForFences(ctx_->device(), 1, &frame_fence, VK_TRUE, UINT64_MAX);
	require_vk_result(result, "failed to wait for current frame fence");

	std::optional<uint32_t> img_index_result = swap_chain_.get_next_image_index(acquire_semaphore);
	if (!img_index_result.has_value()) {
		return;
	}
	const uint32_t image_index = img_index_result.value();

	record_commands(image_index);

	VkFence image_pending_fence = image_pending_fences_[image_index];
	if (image_pending_fence != VK_NULL_HANDLE) {
		result = ctx_->lib.vkWaitForFences(ctx_->device(), 1, &image_pending_fence, VK_TRUE, UINT64_MAX);
		require_vk_result(result, "failed to wait for image-pending fence");
	}

	image_pending_fences_[image_index] = frame_fence;
	ctx_->lib.vkResetFences(ctx_->device(), 1, &frame_fence);

	VkCommandBuffer cmd_buffer = cmd_lists_[image_index].get();
	ctx_->submit_to_queue(ctx_->graphics_queue, cmd_buffer, acquire_semaphore, release_semaphore, frame_fence);

	swap_chain_.present(image_index, release_semaphore);
	current_frame_ = (current_frame_ + 1) % MaxFrames;

	std::this_thread::sleep_for(std::chrono::milliseconds(delay_milliseconds_));
	first_frame_done_ = true;
}

void AutomatonRenderer::record_commands(uint32_t image_index) {
	CommandList& cmd = cmd_lists_[image_index];
	RenderTarget& rt = current_frame_ == 0 ? front_target_ : back_target_;

	cmd.begin();
	if (first_frame_done_) {
		cmd.transition_render_target(rt, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}

	RectSize simulation_size = rt.get_size();
	cmd.begin_render_pass(simulation_size, simulation_pass_, rt.get_framebuffer());
	cmd.bind_viewport(simulation_size);
	cmd.bind_scissor(simulation_size);
	cmd.bind_descriptor_set(pipeline_layout_, current_frame_ == 0 ? descriptor_set_back_ : descriptor_set_front_);
	cmd.bind_pipeline(simulation_pipeline_);
	cmd.draw(3);
	cmd.end_render_pass();

	RectSize window_size = swap_chain_.get_size();
	cmd.begin_render_pass(window_size, presentation_pass_, swap_chain_.get_framebuffer(image_index));
	cmd.bind_viewport(window_size);
	cmd.bind_scissor(window_size);
	cmd.bind_descriptor_set(pipeline_layout_, current_frame_ == 0 ? descriptor_set_front_ : descriptor_set_back_);
	cmd.bind_pipeline(presentation_pipeline_);
	cmd.draw(3);
	cmd.end_render_pass();

	cmd.end();
}

void AutomatonRenderer::prepare_render_targets(uint32_t initial_live_cell_incidence) {
	::srand(static_cast<unsigned>(::time(nullptr)));

	const size_t size = sizeof(uint32_t) * static_cast<uint32_t>(back_target_.get_size().area());
	Buffer staging_buffer(ctx_, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* staging_target;
	VkResult result = ctx_->lib.vkMapMemory(ctx_->device(), staging_buffer.get_memory(), 0, size, 0, &staging_target);
	require_vk_result(result, "failed to map memory");

	uint32_t* pixels = reinterpret_cast<uint32_t*>(staging_target);
	for (size_t i = 0; i < size / sizeof(uint32_t); ++i) {
		*pixels++ = static_cast<uint32_t>(::rand()) % initial_live_cell_incidence == 0 ? 0xFFFFFFFF : 0;
	}
	ctx_->lib.vkUnmapMemory(ctx_->device(), staging_buffer.get_memory());

	CommandList cmd(ctx_);
	cmd.begin();
	cmd.transition_render_target(front_target_, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	cmd.transition_render_target(back_target_, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	cmd.copy_buffer_to_render_target(staging_buffer, back_target_);
	cmd.transition_render_target(back_target_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	cmd.end();
	ctx_->submit_to_queue_blocking(ctx_->graphics_queue, cmd.get());
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
	VkResult result = ctx_->lib.vkCreateShaderModule(ctx_->device(), &shader_info, nullptr, &shader_module);
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
	VkResult result = ctx_->lib.vkCreateDescriptorSetLayout(ctx_->device(), &layout_info, nullptr, &descriptor_set_layout);
	require_vk_result(result, "failed to create Vulkan descriptor set layout");
	return descriptor_set_layout;
}

VkPipelineLayout AutomatonRenderer::create_pipeline_layout() const {
	VkPipelineLayoutCreateInfo pipeline_layout_info {
		.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext					= nullptr,
		.flags					= 0,
		.setLayoutCount			= 1,
		.pSetLayouts			= &descriptor_set_layout_,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges	= nullptr,
	};

	VkPipelineLayout pipeline_layout;
	VkResult result = ctx_->lib.vkCreatePipelineLayout(ctx_->device(), &pipeline_layout_info, nullptr, &pipeline_layout);
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
	VkResult result = ctx_->lib.vkCreateSampler(ctx_->device(), &sampler_info, nullptr, &sampler);
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
	VkResult result = ctx_->lib.vkCreateDescriptorPool(ctx_->device(), &descriptor_pool_info, nullptr, &descriptor_pool);
	require_vk_result(result, "failed to create Vulkan descriptor pool");
	return descriptor_pool;
}

VkDescriptorSet AutomatonRenderer::create_descriptor_set(const RenderTarget& render_target) const {
	VkDescriptorSetAllocateInfo alloc_info {
		.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext				= nullptr,
		.descriptorPool		= descriptor_pool_,
		.descriptorSetCount = 1,
		.pSetLayouts		= &descriptor_set_layout_,
	};
	VkDescriptorSet descriptor_set;
	VkResult result = ctx_->lib.vkAllocateDescriptorSets(ctx_->device(), &alloc_info, &descriptor_set);
	require_vk_result(result, "failed to create Vulkan descriptor set");

	VkDescriptorImageInfo image_info {
		.sampler	 = sampler_,
		.imageView	 = render_target.get_image_view(),
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
	};
	VkWriteDescriptorSet write {
		.sType			  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.pNext			  = nullptr,
		.dstSet			  = descriptor_set,
		.dstBinding		  = 0,
		.dstArrayElement  = 0,
		.descriptorCount  = 1,
		.descriptorType	  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.pImageInfo		  = &image_info,
		.pBufferInfo	  = nullptr,
		.pTexelBufferView = nullptr,
	};
	ctx_->lib.vkUpdateDescriptorSets(ctx_->device(), 1, &write, 0, nullptr);

	return descriptor_set;
}

VkFence AutomatonRenderer::create_fence() const {
	VkFenceCreateInfo fence_info {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};
	VkFence fence;
	VkResult result = ctx_->lib.vkCreateFence(ctx_->device(), &fence_info, nullptr, &fence);
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
	VkResult result = ctx_->lib.vkCreateSemaphore(ctx_->device(), &semaphore_info, nullptr, &semaphore);
	require_vk_result(result, "failed to create Vulkan semaphore");
	return semaphore;
}

} // namespace cltv
