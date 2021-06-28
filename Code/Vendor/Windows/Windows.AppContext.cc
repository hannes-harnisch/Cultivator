#include "PCH.hh"

#include "App/App.hh"
#include "Utils/Assert.hh"
#include "Windows.AppContext.hh"

namespace ct::windows
{
	namespace
	{
		LRESULT CALLBACK receiveWindowsEvents(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch(message)
			{
				case WM_CLOSE: App::quit(); return 0;
				default: return ::DefWindowProc(windowHandle, message, wParam, lParam);
			}
		}
	}

	AppContext::AppContext() : appHandle(::GetModuleHandle(nullptr))
	{
		WNDCLASS windowClass {
			.style		   = CS_DBLCLKS,
			.lpfnWndProc   = receiveWindowsEvents,
			.hInstance	   = appHandle,
			.lpszClassName = WindowClassName,
		};
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
