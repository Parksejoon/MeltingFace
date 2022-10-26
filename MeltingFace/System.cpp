#include "System.h"

namespace MF
{
	System::System()
	{
		m_input = nullptr;
		m_graphics = nullptr;
	}

	System::System(const System& other)
	{
	}

	System::~System()
	{
	}

	bool System::Initialize()
	{
		int screenWidth = 0;
		int screenHeight = 0;

		InitializeWindows(screenWidth, screenHeight);

		m_input = new Input;
		if (!m_input)
		{
			return false;
		}

		m_input->Initialize();

		m_graphics = new Graphics;
		if (!m_graphics)
		{
			return false;
		}

		return m_graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	}

	void System::Shutdown()
	{
		if (m_graphics)
		{
			m_graphics->Shutdown();
			delete m_graphics;
			m_graphics = nullptr;
		}

		if (m_input)
		{
			delete m_input;
			m_input = nullptr;
		}

		ShutdownWindows();
	}

	void System::Run()
	{
		MSG msg;
		bool done;
		bool result;

		ZeroMemory(&msg, sizeof(MSG));

		done = false;
		while (!done)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (msg.message == WM_QUIT)
			{
				done = true;
			}
			else
			{
				result = Frame();
				if (!result)
				{
					done = true;
				}
			}
		}
	}

	LRESULT System::MessageHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
			case WM_KEYDOWN:
			{
				m_input->KeyDown((unsigned int)wParam);
				return 0;
			}

			case WM_KEYUP:
			{
				m_input->KeyUp((unsigned int)wParam);
				return 0;
			}

			default:
			{
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
		}
	}

	bool System::Frame()
	{
		if (m_input->IsKeyDown(VK_ESCAPE))
		{
			return false;
		}

		return m_graphics->Frame();
	}

	void System::InitializeWindows(int& screenWidth, int& screenHeight)
	{
		WNDCLASSEX wc;
		DEVMODE dmScreenSettings;
		int posX;
		int posY;

		ApplicationHandle = this;

		m_hinstance = GetModuleHandle(nullptr);
		m_applicationName = L"Engine";

		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hinstance;
		wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = m_applicationName;
		wc.cbSize = sizeof(WNDCLASSEX);

		RegisterClassEx(&wc);

		screenWidth = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);

		if (Graphics::FULL_SCREEN)
		{
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
			dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			posX = 0;
			posY = 0;
		}
		else
		{
			screenWidth = 800;
			screenHeight = 600;

			posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
			posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
		}

		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
						WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
								posX, posY, screenWidth, screenHeight,
						nullptr, nullptr, m_hinstance, nullptr);

		ShowWindow(m_hwnd, SW_SHOW);
		SetForegroundWindow(m_hwnd);
		SetFocus(m_hwnd);

		ShowCursor(false);
	}

	void System::ShutdownWindows()
	{
		ShowCursor(true);

		if (m_graphics->FULL_SCREEN)
		{
			ChangeDisplaySettings(nullptr, 0);
		}

		DestroyWindow(m_hwnd);
		m_hwnd = nullptr;

		UnregisterClass(m_applicationName, m_hinstance);
		m_hinstance = nullptr;

		ApplicationHandle = nullptr;
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
			case WM_DESTROY:
			case WM_CLOSE:
			{
				PostQuitMessage(0);
				return 0;
			}

			default:
			{
				return ApplicationHandle->MessageHandler(hwnd, uMsg, wParam, lParam);
			}
		}
	}
}
