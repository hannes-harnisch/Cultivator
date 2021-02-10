#include "Windows/Windows.API.hh"
#include "Windows/Windows.Utils.hh"
#include <string>

namespace ct
{
	void showErrorWindow(const std::string& title, const std::string& message)
	{
		auto wideTitle {widenString(title)};
		auto wideMessage {widenString(message)};
		::MessageBox(nullptr, wideMessage.data(), wideTitle.data(), MB_OK | MB_ICONERROR);
	}
}
