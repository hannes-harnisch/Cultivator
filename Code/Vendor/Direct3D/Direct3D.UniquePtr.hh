#pragma once

#include "PCH.hh"

namespace ct::direct3d
{
	template<typename I> class UniquePtr final
	{
	public:
		UniquePtr() : Interface {nullptr}
		{}

		UniquePtr(UniquePtr&& other) noexcept : Interface {std::exchange(other.Interface, nullptr)}
		{}

		~UniquePtr()
		{
			if(Interface)
				Interface->Release();
		}

		UniquePtr& operator=(UniquePtr&& other) noexcept
		{
			std::swap(Interface, other.Interface);
			return *this;
		}

		bool operator==(const UniquePtr& other) const
		{
			return Interface == other.Interface;
		}

		bool operator!=(const UniquePtr& other) const
		{
			return Interface != other.Interface;
		}

		I* operator->()
		{
			return Interface;
		}

		const I* operator->() const
		{
			return Interface;
		}

		I& operator*()
		{
			return *Interface;
		}

		const I& operator*() const
		{
			return *Interface;
		}

		I** operator&()
		{
			return &Interface;
		}

		I* const* operator&() const
		{
			return &Interface;
		}

		operator I*()
		{
			return Interface;
		}

		operator const I*() const
		{
			return Interface;
		}

		operator bool() const
		{
			return Interface != nullptr;
		}

	private:
		I* Interface;
	};
}
