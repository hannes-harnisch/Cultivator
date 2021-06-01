#include "PCH.hh"

#include "Vendor/Windows/Windows.Utils.hh"

namespace ct
{
	void showErrorBox(const std::string& title, const std::string& message)
	{
		auto wideTitle {windows::widenString(title)};
		auto wideMessage {windows::widenString(message)};
		::MessageBox(nullptr, wideMessage.data(), wideTitle.data(), MB_OK | MB_ICONERROR);
	}
}
