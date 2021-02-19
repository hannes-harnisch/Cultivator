#pragma once

#include "PCH.hh"

namespace ct::vulkan
{
	class Pipeline final
	{
		~Pipeline();
		Pipeline(Pipeline&& other) noexcept;
		Pipeline& operator=(Pipeline&& other) noexcept;

		inline vk::Pipeline handle() const
		{
			return PipelineHandle;
		}

	private:
		vk::Pipeline PipelineHandle;
	};
}
