#include "PCH.hh"
#include "Windows.AppContext.hh"

#include "App/App.hh"
#include "Utils/Assert.hh"

namespace ct::windows
{
	namespace
	{
		LRESULT CALLBACK receiveWindowsEvents(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch(message)
			{
				case WM_CLOSE: App::get().quit(); return 0;
				default: return ::DefWindowProc(windowHandle, message, wParam, lParam);
			}
		}
	}

	AppContext::AppContext()
	{
		WNDCLASS windowClass {};
		windowClass.style		  = CS_DBLCLKS;
		windowClass.lpfnWndProc	  = receiveWindowsEvents;
		windowClass.lpszClassName = TEXT(CT_APP_NAME);
		ctEnsure(::RegisterClass(&windowClass), "Failed to register window class.");
	}

	void AppContext::pollEvents()
	{
		MSG message;
		while(::PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	}
}
