#include "PCH.hh"

#include "Vulkan.GPUContext.hh"
#include "Vulkan.Texture.hh"
#include "Vulkan.Utils.hh"

namespace ct
{
	Texture::Texture(Rectangle size) : texSize(size), img(makeImage(size)), memory(allocateMemory()), imgView(makeImageView())
	{}

	vk::Image Texture::makeImage(Rectangle size)
	{
		vk::ImageCreateInfo imageInfo;
		imageInfo.imageType	  = vk::ImageType::e2D;
		imageInfo.format	  = vk::Format::eB8G8R8A8Srgb;
		imageInfo.extent	  = vk::Extent3D(size.width, size.height, 1);
		imageInfo.mipLevels	  = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.samples	  = vk::SampleCountFlagBits::e1;
		imageInfo.usage =
			vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;

		auto [res, handle] = GPUContext::device().createImage(imageInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan texture.");
		return handle;
	}

	vk::DeviceMemory Texture::allocateMemory()
	{
		auto memRequirements = GPUContext::device().getImageMemoryRequirements(img, Loader::get());
		uint32_t typeIndex	 = findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

		vk::MemoryAllocateInfo allocInfo;
		allocInfo.allocationSize  = memRequirements.size;
		allocInfo.memoryTypeIndex = typeIndex;
		auto [res, handle]		  = GPUContext::device().allocateMemory(allocInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to allocate texture memory.");

		ctAssertResult(GPUContext::device().bindImageMemory(img, handle, 0, Loader::get()),
					   "Failed to bind memory to Vulkan texture.");
		return handle;
	}

	vk::ImageView Texture::makeImageView()
	{
		vk::ImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		subresourceRange.levelCount = 1;
		subresourceRange.layerCount = 1;

		vk::ImageViewCreateInfo imageViewInfo;
		imageViewInfo.image			   = img;
		imageViewInfo.viewType		   = vk::ImageViewType::e2D;
		imageViewInfo.format		   = vk::Format::eB8G8R8A8Srgb;
		imageViewInfo.subresourceRange = subresourceRange;

		auto [res, handle] = GPUContext::device().createImageView(imageViewInfo, nullptr, Loader::get());
		ctAssertResult(res, "Failed to create Vulkan image view.");
		return handle;
	}
}
