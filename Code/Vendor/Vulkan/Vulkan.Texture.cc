#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.GPUContext.hh"
#include "Vendor/Vulkan/Vulkan.Utils.hh"
#include "Vulkan.Texture.hh"

namespace ct::vulkan
{
	Texture::Texture(Rectangle size) :
		image(makeImage(size)), memory(allocateMemory()), sampler(makeSampler()), imageView(makeImageView())
	{}

	vk::Image Texture::makeImage(Rectangle size)
	{
		auto imageInfo = vk::ImageCreateInfo()
							 .setImageType(vk::ImageType::e2D)
							 .setFormat(vk::Format::eR8G8B8A8Srgb)
							 .setExtent({size.Width, size.Height, 1})
							 .setMipLevels(1)
							 .setArrayLayers(1)
							 .setSamples(vk::SampleCountFlagBits::e1)
							 .setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
		auto [res, handle] = GPUContext::device().createImage(imageInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan texture.");
		return handle;
	}

	vk::DeviceMemory Texture::allocateMemory()
	{
		auto memRequirements = GPUContext::device().getImageMemoryRequirements(image, Loader::get());
		uint32_t typeIndex	 = findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

		auto allocInfo	   = vk::MemoryAllocateInfo().setAllocationSize(memRequirements.size).setMemoryTypeIndex(typeIndex);
		auto [res, handle] = GPUContext::device().allocateMemory(allocInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to allocate texture memory.");

		ctAssertResult(GPUContext::device().bindImageMemory(image, handle, 0, Loader::get()),
					   "Failed to bind memory to Vulkan texture.");
		return handle;
	}

	vk::Sampler Texture::makeSampler()
	{
		auto samplerInfo = vk::SamplerCreateInfo()
							   .setMagFilter(vk::Filter::eLinear)
							   .setMinFilter(vk::Filter::eLinear)
							   .setAddressModeU(vk::SamplerAddressMode::eClampToBorder)
							   .setAddressModeV(vk::SamplerAddressMode::eClampToBorder)
							   .setAddressModeW(vk::SamplerAddressMode::eClampToBorder)
							   .setAnisotropyEnable(false)
							   .setMaxAnisotropy(1.0f)
							   .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
							   .setUnnormalizedCoordinates(false)
							   .setCompareEnable(false)
							   .setCompareOp(vk::CompareOp::eAlways);
		auto [res, handle] = GPUContext::device().createSampler(samplerInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create sampler.");
		return handle;
	}

	vk::ImageView Texture::makeImageView()
	{
		auto subresourceRange =
			vk::ImageSubresourceRange().setAspectMask(vk::ImageAspectFlagBits::eColor).setLevelCount(1).setLayerCount(1);
		auto imageViewInfo = vk::ImageViewCreateInfo()
								 .setImage(image)
								 .setViewType(vk::ImageViewType::e2D)
								 .setFormat(vk::Format::eR8G8B8A8Srgb)
								 .setSubresourceRange(subresourceRange);
		auto [res, handle] = GPUContext::device().createImageView(imageViewInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan image view.");
		return handle;
	}
}
