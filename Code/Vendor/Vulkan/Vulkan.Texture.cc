#include "PCH.hh"

#include "Vendor/Vulkan/Vulkan.GPUContext.hh"
#include "Vendor/Vulkan/Vulkan.Utils.hh"
#include "Vulkan.Texture.hh"

namespace ct::vulkan
{
	Texture::Texture(Rectangle size) : Image {createImage(size)}, Memory {allocateMemory()}, ImageView {createImageView()}
	{}

	vk::Image Texture::createImage(Rectangle size)
	{
		auto imageInfo {vk::ImageCreateInfo()
							.setImageType(vk::ImageType::e2D)
							.setFormat(vk::Format::eR8G8B8A8Srgb)
							.setExtent({size.Width, size.Height, 1})
							.setMipLevels(1)
							.setArrayLayers(1)
							.setSamples(vk::SampleCountFlagBits::e1)
							.setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)};
		auto [res, image] {GPUContext::device().createImage(imageInfo, nullptr, Loader::get())};
		ctAssertResult(res, "Failed to create Vulkan texture.");
		return image;
	}

	vk::DeviceMemory Texture::allocateMemory()
	{
		auto memRequirements {GPUContext::device().getImageMemoryRequirements(Image, Loader::get())};
		uint32_t typeIndex {findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)};

		auto allocInfo {vk::MemoryAllocateInfo().setAllocationSize(memRequirements.size).setMemoryTypeIndex(typeIndex)};
		auto [res, memory] {GPUContext::device().allocateMemory(allocInfo, nullptr, Loader::get())};
		ctAssertResult(res, "Failed to allocate texture memory.");

		ctAssertResult(GPUContext::device().bindImageMemory(Image, memory, 0, Loader::get()),
					   "Failed to bind memory to Vulkan texture.");
		return memory;
	}

	vk::ImageView Texture::createImageView()
	{
		auto subresourceRange {
			vk::ImageSubresourceRange().setAspectMask(vk::ImageAspectFlagBits::eColor).setLevelCount(1).setLayerCount(1)};
		auto imageViewInfo {vk::ImageViewCreateInfo()
								.setImage(Image)
								.setViewType(vk::ImageViewType::e2D)
								.setFormat(vk::Format::eR8G8B8A8Srgb)
								.setSubresourceRange(subresourceRange)};
		auto [res, imageView] {GPUContext::device().createImageView(imageViewInfo, nullptr, Loader::get())};
		ctAssertResult(res, "Failed to create Vulkan image view.");
		return imageView;
	}
}
