#include "PCH.hh"
#include "Vulkan.Texture.hh"

#include "Vendor/Vulkan/Vulkan.GraphicsContext.hh"
#include "Vendor/Vulkan/Vulkan.Utils.hh"

namespace ct::vulkan
{
	Texture::Texture(Rectangle size) : Image {createImage(size)}, Memory {allocateMemory()}
	{
		auto res {GraphicsContext::device().bindImageMemory(Image, Memory, 0, Loader::get())};
		ctAssertResult(res, "Failed to bind memory to Vulkan texture.");
	}

	Texture::~Texture()
	{
		auto device {GraphicsContext::device()};
		device.destroyImage(Image, {}, Loader::get());
		device.freeMemory(Memory, {}, Loader::get());
	}

	Texture::Texture(Texture&& other) noexcept :
		Image {std::exchange(other.Image, nullptr)}, Memory {std::exchange(other.Memory, nullptr)}
	{}

	Texture& Texture::operator=(Texture&& other) noexcept
	{
		std::swap(Image, other.Image);
		std::swap(Memory, other.Memory);
		return *this;
	}

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
		auto [res, image] {GraphicsContext::device().createImage(imageInfo, nullptr, Loader::get())};
		ctAssertResult(res, "Failed to create Vulkan texture.");
		return image;
	}

	vk::DeviceMemory Texture::allocateMemory()
	{
		auto memRequirements {GraphicsContext::device().getImageMemoryRequirements(Image, Loader::get())};
		uint32_t typeIndex {findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)};

		auto allocInfo {vk::MemoryAllocateInfo().setAllocationSize(memRequirements.size).setMemoryTypeIndex(typeIndex)};
		auto [res, memory] {GraphicsContext::device().allocateMemory(allocInfo, nullptr, Loader::get())};
		ctAssertResult(res, "Failed to allocate texture memory.");
		return memory;
	}
}
