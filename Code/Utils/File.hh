#pragma once

#include "PCH.hh"
#include "Utils/Array.hh"

namespace ct
{
	class File
	{
	public:
		inline static Array<char> loadBinary(const std::string& filePath)
		{
			FILE* file;
			fopen_s(&file, filePath.data(), "r");
			if(!file)
				return Array<char>();

			std::fseek(file, 0, SEEK_END);
			long size {std::ftell(file)};
			std::rewind(file);

			Array<char> content(size);
			std::fread(content.data(), size, 1, file);
			std::fclose(file);
			return content;
		}
	};
}
