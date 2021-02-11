#include "Platform/Windows/Windows.API.hh"
#include "Platform/Windows/Windows.Utils.hh"
#include <string>

namespace ct
{
	void showErrorBox(const std::string& title, const std::string& message)
	{
		auto wideTitle {widenString(title)};
		auto wideMessage {widenString(message)};
		::MessageBox(nullptr, wideMessage.data(), wideTitle.data(), MB_OK | MB_ICONERROR);
	}
}
