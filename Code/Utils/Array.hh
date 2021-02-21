#pragma once

#include "Utils/Assert.hh"

namespace ct
{
	template<typename T> class Array final
	{
	public:
		inline Array() = default;

		inline Array(size_t size, T data[]) : Size {size}, Data {data}
		{}

		inline Array(size_t size) : Array {size, new T[size]}
		{}

		inline Array(const Array& other) : Array {other.Size}
		{
			auto src {other.begin()};
			for(auto& element : *this)
				element = *src++;
		}

		inline Array(Array&& other) noexcept : Array {other.Size, std::exchange(other.Data, nullptr)}
		{}

		inline ~Array()
		{
			delete[] Data;
		}

		inline Array& operator=(const Array& other)
		{
			delete[] Data;
			Size = other.Size;
			Data = new T[Size];

			auto src {other.begin()};
			for(auto& element : *this)
				element = *src++;

			return *this;
		}

		inline Array& operator=(Array&& other) noexcept
		{
			Size = other.Size;
			std::swap(Data, other.Data);
			return *this;
		}

		inline T& operator[](size_t index)
		{
			ctAssertPure(index < Size, "Array index out of range: " + std::to_string(index));
			return *(Data + index);
		}

		inline const T& operator[](size_t index) const
		{
			ctAssertPure(index < Size, "Array index out of range: " + std::to_string(index));
			return *(Data + index);
		}

		inline size_t size() const
		{
			return Size;
		}

		inline T* data()
		{
			return Data;
		}

		inline const T* data() const
		{
			return Data;
		}

		template<typename V> class Iterator
		{
			friend Array;

		public:
			// These aliases are necessary to be seen as a contiguous iterator in the eyes of the compiler.
			using iterator_concept = std::contiguous_iterator_tag;
			using value_type	   = V;
			using size_type		   = size_t;
			using difference_type  = ptrdiff_t;
			using pointer		   = V*;
			using const_pointer	   = const V*;
			using reference		   = V&;
			using const_reference  = const V&;

			inline Iterator() = default;

			inline V& operator*() const
			{
				return *Position;
			}

			inline V* operator->() const
			{
				return Position;
			}

			inline bool operator==(Iterator other) const
			{
				return Position == other.Position;
			}

			inline bool operator!=(Iterator other) const
			{
				return Position != other.Position;
			}

			inline bool operator>(Iterator other) const
			{
				return Position > other.Position;
			}

			inline bool operator>=(Iterator other) const
			{
				return Position >= other.Position;
			}

			inline bool operator<(Iterator other) const
			{
				return Position < other.Position;
			}

			inline bool operator<=(Iterator other) const
			{
				return Position <= other.Position;
			}

			inline Iterator& operator++()
			{
				incrementPosition(1);
				return *this;
			}

			inline const Iterator& operator++() const
			{
				incrementPosition(1);
				return *this;
			}

			inline Iterator operator++(int) const
			{
				Iterator old {*this};
				incrementPosition(1);
				return old;
			}

			inline Iterator& operator--()
			{
				decrementPosition(1);
				return *this;
			}

			inline const Iterator& operator--() const
			{
				decrementPosition(1);
				return *this;
			}

			inline Iterator operator--(int) const
			{
				Iterator old {*this};
				decrementPosition(1);
				return old;
			}

			inline Iterator& operator+=(ptrdiff_t offset)
			{
				incrementPosition(offset);
				return *this;
			}

			inline const Iterator& operator+=(ptrdiff_t offset) const
			{
				incrementPosition(offset);
				return *this;
			}

			inline Iterator operator+(ptrdiff_t offset) const
			{
				Iterator copy {*this};
				return copy += offset;
			}

			inline friend Iterator operator+(ptrdiff_t offset, Iterator iterator)
			{
				return iterator + offset;
			}

			inline Iterator& operator-=(ptrdiff_t offset)
			{
				decrementPosition(offset);
				return *this;
			}

			inline const Iterator& operator-=(ptrdiff_t offset) const
			{
				decrementPosition(offset);
				return *this;
			}

			inline Iterator operator-(ptrdiff_t offset) const
			{
				Iterator copy {*this};
				return copy -= offset;
			}

			inline ptrdiff_t operator-(Iterator other) const
			{
				return Position - other.Position;
			}

			inline V& operator[](size_t index) const
			{
				return *(*this + index);
			}

		private:
			mutable V* Position {};

#if CT_DEBUG
			V* Begin {};
			V* End {};

			inline Iterator(V* pos, V* begin, V* end) : Position {pos}, Begin {begin}, End {end}
			{}
#else
			inline Iterator(V* pos) : Position {pos}
			{}
#endif

			inline void incrementPosition(ptrdiff_t offset) const
			{
				ctAssertPure(Position < End, "Cannot increment iterator past end.");
				Position += offset;
			}

			inline void decrementPosition(ptrdiff_t offset) const
			{
				ctAssertPure(Begin < Position, "Cannot decrement iterator before begin.");
				Position -= offset;
			}
		};

		inline Iterator<T> begin()
		{
#if CT_DEBUG
			return {Data, Data, Data + Size};
#else
			return {Data};
#endif
		}

		inline Iterator<const T> begin() const
		{
#if CT_DEBUG
			return {Data, Data, Data + Size};
#else
			return {Data};
#endif
		}

		inline Iterator<T> end()
		{
#if CT_DEBUG
			T* endPos {Data + Size};
			return {endPos, Data, endPos};
#else
			return {Data + Size};
#endif
		}

		inline Iterator<const T> end() const
		{
#if CT_DEBUG
			T* endPos {Data + Size};
			return {endPos, Data, endPos};
#else
			return {Data + Size};
#endif
		}

		void* operator new(size_t)	= delete;
		void operator delete(void*) = delete;

	private:
		size_t Size {};
		T* Data {};
	};
}
