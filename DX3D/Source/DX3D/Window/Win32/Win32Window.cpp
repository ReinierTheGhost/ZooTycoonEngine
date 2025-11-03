#include <DX3D/Window/Window.h>
#include <Windows.h>
#include <stdexcept>

static constexpr const wchar_t* kIconPath = L"assets\\icon.ico"; // ICON: path to your icon

static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

dx3d::Window::Window(const WindowDesc& desc) : Base(desc.base), m_size(desc.size)
{

	auto registerWindowClassFunction = []()
		{
			WNDCLASSEX wc{};
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = CS_HREDRAW | CS_VREDRAW;               // ICON: nice to have
			wc.lpszClassName = L"DX3DWindow";
			wc.lpfnWndProc = &WindowProcedure;
			wc.hInstance = GetModuleHandle(nullptr);              // ICON: set instance
			wc.hCursor = LoadCursor(nullptr, IDC_ARROW);        // ICON: standard cursor
			wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);            // ICON: background brush

			// ICON: load icons for the class (Alt-Tab / taskbar)
			wc.hIcon = (HICON)LoadImage(
				nullptr, kIconPath, IMAGE_ICON,
				64, 64, LR_LOADFROMFILE
			);

			return RegisterClassEx(&wc);
		};

	
	static const auto windowClassId = std::invoke(registerWindowClassFunction);

	if (!windowClassId) 
		DX3DLogThrowError("RegisterClassEx failed.");
	

	RECT rc{0, 0, m_size.width, m_size.height};
	AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, false);

	// ICON: pass a valid hInstance (recommended)
	HINSTANCE hInst = GetModuleHandle(nullptr);


	m_handle = CreateWindowEx(NULL, MAKEINTATOM(windowClassId), L"Zoo Tycoon 2.2", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInst, NULL);


	if (!m_handle) 
		DX3DLogThrowError("CreateWindowEx failed.");
		
	 // ICON: also set icons on this specific window instance
    {
        HWND hwnd = static_cast<HWND>(m_handle);

        HICON hBigIcon = (HICON)LoadImage(
            nullptr, kIconPath, IMAGE_ICON,
            0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE
        );
        if (hBigIcon) {
            SendMessage(hwnd, WM_SETICON, ICON_BIG,   (LPARAM)hBigIcon);
        }

        HICON hSmallIcon = (HICON)LoadImage(
            nullptr, kIconPath, IMAGE_ICON,
            16, 16, LR_LOADFROMFILE
        );
        if (hSmallIcon) {
            SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hSmallIcon);
        }
    }
		

	ShowWindow(static_cast<HWND>(m_handle), SW_SHOW);
}


dx3d::Window::~Window()
{
	DestroyWindow(static_cast<HWND>(m_handle));
}
