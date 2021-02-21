#pragma once

namespace ct::vulkan
{
	class RenderPass final
	{
	public:
		~RenderPass();
		RenderPass(RenderPass&& other) noexcept;
		RenderPass& operator=(RenderPass&& other) noexcept;

		inline vk::RenderPass handle() const
		{
			return Pass;
		}

	private:
		vk::RenderPass Pass;
	};
}
