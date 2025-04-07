#include "AutomatonRenderer.hpp"

#include "graphics/vulkan/Util.hpp"
#include "util/Util.hpp"

namespace cltv {

AutomatonRenderer::AutomatonRenderer(const DeviceContext* ctx, Window& window, RectSize size, const char* shader_path) :
	_ctx(ctx),
	_window_size(window.get_viewport()),
	_simulation_pass(*_ctx, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
	_presentation_pass(*_ctx, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
	_swap_chain(*_ctx, _window_size, window, _presentation_pass),
	_front_target(*_ctx, size, _simulation_pass),
	_back_target(*_ctx, size, _simulation_pass),
	_vertex_shader(create_shader_module("ScreenQuad.vert.spv")),
	_simulation_fragment_shader(create_shader_module(shader_path)),
	_presentation_fragment_shader(create_shader_module("Presentation.frag.spv")),
	_descriptor_set_layout(create_descriptor_set_layout()),
	_pipeline_layout(create_pipeline_layout()),
	_simulation_pipeline(*_ctx, _vertex_shader, _simulation_fragment_shader, _pipeline_layout, _simulation_pass),
	_presentation_pipeline(*_ctx, _vertex_shader, _presentation_fragment_shader, _pipeline_layout, _presentation_pass),
	_sampler(create_sampler()) {
}

AutomatonRenderer::~AutomatonRenderer() {
	_ctx->lib.vkDestroySampler(_ctx->device(), _sampler, nullptr);

	_presentation_pipeline.destroy(*_ctx);
	_simulation_pipeline.destroy(*_ctx);

	_ctx->lib.vkDestroyPipelineLayout(_ctx->device(), _pipeline_layout, nullptr);
	_ctx->lib.vkDestroyDescriptorSetLayout(_ctx->device(), _descriptor_set_layout, nullptr);
	_ctx->lib.vkDestroyShaderModule(_ctx->device(), _presentation_fragment_shader, nullptr);
	_ctx->lib.vkDestroyShaderModule(_ctx->device(), _simulation_fragment_shader, nullptr);
	_ctx->lib.vkDestroyShaderModule(_ctx->device(), _vertex_shader, nullptr);

	_back_target.destroy(*_ctx);
	_front_target.destroy(*_ctx);
	_swap_chain.destroy(*_ctx);
	_presentation_pass.destroy(*_ctx);
	_simulation_pass.destroy(*_ctx);
}

void AutomatonRenderer::draw_frame() {
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

} // namespace cltv
