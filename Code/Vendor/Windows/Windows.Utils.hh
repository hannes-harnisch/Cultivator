#pragma once

#include "PCH.hh"

namespace ct::windows
{
	inline std::wstring widenString(std::string const& narrow)
	{
		int length = ::MultiByteToWideChar(CP_UTF8, 0, narrow.data(), -1, nullptr, 0);
		std::wstring wide(length, '\0');
		::MultiByteToWideChar(CP_UTF8, 0, narrow.data(), -1, wide.data(), length);
		return wide;
	}

	inline std::string narrowString(std::wstring const& wide)
	{
		int length = ::WideCharToMultiByte(CP_UTF8, 0, wide.data(), -1, nullptr, 0, nullptr, nullptr);
		std::string narrow(length, '\0');
		::WideCharToMultiByte(CP_UTF8, 0, wide.data(), -1, narrow.data(), length, nullptr, nullptr);
		return narrow;
	}
}
