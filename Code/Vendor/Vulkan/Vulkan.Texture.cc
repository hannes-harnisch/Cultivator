#include "PCH.hh"

#include "Vulkan.GPUContext.hh"
#include "Vulkan.Texture.hh"
#include "Vulkan.Utils.hh"

namespace ct
{
	Texture::Texture(Rectangle size) : texSize(size)
	{
		img		= makeImage(size);
		memory	= allocateMemory();
		imgView = makeImageView();
	}

	vk::Image Texture::makeImage(Rectangle size)
	{
		vk::ImageCreateInfo info {
			.imageType	 = vk::ImageType::e2D,
			.format		 = vk::Format::eB8G8R8A8Srgb,
			.extent		 = vk::Extent3D(size.width, size.height, 1),
			.mipLevels	 = 1,
			.arrayLayers = 1,
			.samples	 = vk::SampleCountFlagBits::e1,
			.usage		 = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled |
					 vk::ImageUsageFlagBits::eTransferDst,
		};
		auto [res, handle] = GPUContext::device().createImage(info);
		ctAssertResult(res, "Failed to create Vulkan texture.");
		return handle;
	}

	vk::DeviceMemory Texture::allocateMemory()
	{
		auto memRequirements = GPUContext::device().getImageMemoryRequirements(img);
		uint32_t typeIndex	 = findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

		vk::MemoryAllocateInfo info {
			.allocationSize	 = memRequirements.size,
			.memoryTypeIndex = typeIndex,
		};
		auto [res, handle] = GPUContext::device().allocateMemory(info);
		ctAssertResult(res, "Failed to allocate texture memory.");

		ctAssertResult(GPUContext::device().bindImageMemory(img, handle, 0), "Failed to bind memory to Vulkan texture.");
		return handle;
	}

	vk::ImageView Texture::makeImageView()
	{
		vk::ImageSubresourceRange range {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.levelCount = 1,
			.layerCount = 1,
		};
		vk::ImageViewCreateInfo info {
			.image			  = img,
			.viewType		  = vk::ImageViewType::e2D,
			.format			  = vk::Format::eB8G8R8A8Srgb,
			.subresourceRange = range,
		};
		auto [res, handle] = GPUContext::device().createImageView(info);
		ctAssertResult(res, "Failed to create Vulkan image view.");
		return handle;
	}
}
