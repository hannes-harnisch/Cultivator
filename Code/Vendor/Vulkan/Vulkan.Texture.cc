#include "PCH.hh"
#include "Vulkan.Texture.hh"

#include "Vulkan.GraphicsContext.hh"

namespace ct::vulkan
{
	Texture::Texture()
	{
		auto result {GraphicsContext::device().bindImageMemory(Image, Memory, 0, Loader::get())};
		ctAssertResult(result, "Failed to bind memory to Vulkan texture.");
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
}
