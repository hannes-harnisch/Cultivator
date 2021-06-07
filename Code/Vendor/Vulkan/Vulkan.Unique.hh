#pragma once

#include "PCH.hh"

#include "Vulkan.GPUContext.hh"

namespace ct
{
	template<typename T,
			 void (vk::Device::*Deleter)(T, vk::AllocationCallbacks const*, vk::DispatchLoaderDynamic const&) const noexcept>
	struct DeviceDeleter
	{
		using pointer = T;
		void operator()(T handle) const
		{
			(GPUContext::device().*Deleter)(handle, {}, Loader::get());
		}
	};

	template<typename T,
			 void (vk::Device::*Deleter)(T, vk::AllocationCallbacks const*, vk::DispatchLoaderDynamic const&) const noexcept>
	class DeviceOwn : public std::unique_ptr<T, DeviceDeleter<T, Deleter>>
	{
	public:
		using std::unique_ptr<T, DeviceDeleter<T, Deleter>>::unique_ptr;

		DeviceOwn& operator=(T handle)
		{
			this->reset(handle);
			return *this;
		}

		operator T() const
		{
			return this->get();
		}
	};
}
