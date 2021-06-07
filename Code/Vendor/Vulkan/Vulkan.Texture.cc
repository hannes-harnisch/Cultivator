#include "PCH.hh"

#include "Vulkan.GPUContext.hh"
#include "Vulkan.Texture.hh"
#include "Vulkan.Utils.hh"

namespace ct
{
	Texture::Texture(Rectangle size) : image(makeImage(size)), memory(allocateMemory()), imgView(makeImageView())
	{}

	vk::Image Texture::makeImage(Rectangle size)
	{
		auto imageInfo = vk::ImageCreateInfo()
							 .setImageType(vk::ImageType::e2D)
							 .setFormat(vk::Format::eB8G8R8A8Srgb)
							 .setExtent({size.width, size.height, 1})
							 .setMipLevels(1)
							 .setArrayLayers(1)
							 .setSamples(vk::SampleCountFlagBits::e1)
							 .setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
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

	vk::ImageView Texture::makeImageView()
	{
		auto subresourceRange =
			vk::ImageSubresourceRange().setAspectMask(vk::ImageAspectFlagBits::eColor).setLevelCount(1).setLayerCount(1);
		auto imageViewInfo = vk::ImageViewCreateInfo()
								 .setImage(image)
								 .setViewType(vk::ImageViewType::e2D)
								 .setFormat(vk::Format::eB8G8R8A8Srgb)
								 .setSubresourceRange(subresourceRange);
		auto [res, handle] = GPUContext::device().createImageView(imageViewInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan image view.");
		return handle;
	}
}
