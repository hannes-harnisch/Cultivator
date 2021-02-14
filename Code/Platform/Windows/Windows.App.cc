#include "PCH.hh"
#include "App.hh"

#include "Assert.hh"

namespace ct
{
	::LRESULT CALLBACK receiveWindowsEvents(::HWND windowHandle, ::UINT message, ::WPARAM wParam, ::LPARAM lParam)
	{
		switch(message)
		{
			case WM_CLOSE: App::get().quit(); return 0;
			default: return ::DefWindowProc(windowHandle, message, wParam, lParam);
		}
	}

	void App::initializePlatform()
	{
		::WNDCLASS windowClass {};
		windowClass.style		  = CS_DBLCLKS;
		windowClass.lpfnWndProc	  = receiveWindowsEvents;
		windowClass.lpszClassName = TEXT(CT_APP_NAME);
		ctEnsure(::RegisterClass(&windowClass), "Failed to register window class.");
	}

	void App::pollEvents()
	{
		::MSG message;
		while(::PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	}
}
