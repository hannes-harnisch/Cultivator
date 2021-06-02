#include "PCH.hh"

#include "Vendor/Windows/Windows.Utils.hh"

namespace ct
{
	void showErrorBox(std::string const& title, std::string const& message)
	{
		auto wideTitle	 = windows::widenString(title);
		auto wideMessage = windows::widenString(message);
		::MessageBox(nullptr, wideMessage.data(), wideTitle.data(), MB_OK | MB_ICONERROR);
	}
}
