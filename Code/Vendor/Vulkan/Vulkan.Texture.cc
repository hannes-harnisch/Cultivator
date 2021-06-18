#include "PCH.hh"

#include "Vulkan.GPUContext.hh"
#include "Vulkan.Texture.hh"
#include "Vulkan.Utils.hh"

namespace ct
{
	Texture::Texture(Rectangle size) : texSize(size)
	{
		img = makeImage(size);
		memory = allocateMemory();
		imgView = makeImageView();
	}

	vk::Image Texture::makeImage(Rectangle size)
	{
		vk::ImageCreateInfo info;
		info.imageType	 = vk::ImageType::e2D;
		info.format		 = vk::Format::eB8G8R8A8Srgb;
		info.extent		 = vk::Extent3D(size.width, size.height, 1);
		info.mipLevels	 = 1;
		info.arrayLayers = 1;
		info.samples	 = vk::SampleCountFlagBits::e1;
		info.usage =
			vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;

		auto [res, handle] = GPUContext::device().createImage(info);
		ctAssertResult(res, "Failed to create Vulkan texture.");
		return handle;
	}

	vk::DeviceMemory Texture::allocateMemory()
	{
		auto memRequirements = GPUContext::device().getImageMemoryRequirements(img);
		uint32_t typeIndex	 = findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

		vk::MemoryAllocateInfo info;
		info.allocationSize	 = memRequirements.size;
		info.memoryTypeIndex = typeIndex;
		auto [res, handle]	 = GPUContext::device().allocateMemory(info);
		ctAssertResult(res, "Failed to allocate texture memory.");

		ctAssertResult(GPUContext::device().bindImageMemory(img, handle, 0), "Failed to bind memory to Vulkan texture.");
		return handle;
	}

	vk::ImageView Texture::makeImageView()
	{
		vk::ImageSubresourceRange range;
		range.aspectMask = vk::ImageAspectFlagBits::eColor;
		range.levelCount = 1;
		range.layerCount = 1;

		vk::ImageViewCreateInfo info;
		info.image			  = img;
		info.viewType		  = vk::ImageViewType::e2D;
		info.format			  = vk::Format::eB8G8R8A8Srgb;
		info.subresourceRange = range;

		auto [res, handle] = GPUContext::device().createImageView(info);
		ctAssertResult(res, "Failed to create Vulkan image view.");
		return handle;
	}
}
