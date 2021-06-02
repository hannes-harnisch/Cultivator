#pragma once

#include "PCH.hh"

namespace ct
{
	class File
	{
	public:
		static std::vector<char> loadBinary(std::string_view filePath)
		{
			FILE* file;
			fopen_s(&file, filePath.data(), "rb");
			if(!file)
				return {};

			std::fseek(file, 0, SEEK_END);
			long size = std::ftell(file);
			std::rewind(file);

			std::vector<char> content(size);
			std::fread(content.data(), size, 1, file);
			std::fclose(file);
			return content;
		}
	};
}
