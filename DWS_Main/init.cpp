#include "init.h"
#include "resource.h"
#include "GUIFuncs.h"

mWindowSize DWSWindowSize;
HINSTANCE _hInstance;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL RegisterClass(HINSTANCE hInstance)
{
	_hInstance = hInstance;
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MAINICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = CreateSolidBrush(RGB(255,255,255));
	wndclass.lpszMenuName = MAKEINTRESOURCE(DWS_MAINMENU);
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MAINICON));

	if (RegisterClassEx(&wndclass) == 0) return FALSE;
	return TRUE;
}

UINT _TestPluginItem = 0;

VOID OutLog(HWND hWndLog, String _str)
{
	SendMessage(hWndLog, EM_REPLACESEL, TRUE, (LPARAM)_str.c_str());
	std::cout << _str.c_str();
}

HWND textHwnd;

BOOL CreateComponents(HWND hWnd)
{

	HFONT BtnDestroySpyingFont = CreateFont(34, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));
	HWND btnHwnd = CreateButton(hWnd, 10, 10, 410, 50, MainButton, TEXT("Destroy Windows Spy now"));
	SendMessage(btnHwnd, WM_SETFONT, (WPARAM)BtnDestroySpyingFont, TRUE);

	//----------------------------------------------------------------------------------------

	HFONT _Text_Font = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));
	String BuildDate = String(TEXT("Build: ")) + String(TEXT(__DATE__));
	textHwnd = CreateStatic(hWnd, BuildDate.c_str(), 300, 420, 110, 18, StatusBar);
	SetTextAlignment(textHwnd, TextPosition::RIGHT);
	SendMessage(textHwnd, WM_SETFONT, (WPARAM)_Text_Font, TRUE);

	//----------------------------------------------------------------------------------------

	HFONT _Log_Font = CreateFont(12, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("consolas"));
	HWND LogHwnd = CreateLogBox(hWnd, 10, 85, 410, 320, LogBox);
	SendMessage(LogHwnd, WM_SETFONT, (WPARAM)_Log_Font, TRUE);

	//grab full log!
	DWSKernel::addOutputGrabber([LogHwnd](String _str) {
		OutLog(LogHwnd, _str);
	});
#pragma message ("sergius-dart:Dont remove this!")
	DWSKernel::initKernel();
#ifdef _DEBUG
	//ñîçäàäèì ïðèíóäèòåëüíî êîíñîëü äëÿ âûâîäà
	RedirectIOToConsole();
#endif
	//à ýòî ìîæåøü óäàëÿòü. Íî ïîòîì.
	_main();
	
	//----------------------------------------------------------------------------------------
	
	// EXAMPLE ADD PLUGIN MENU!
	HMENU MenuHWND = LoadMenu(_hInstance, MAKEINTRESOURCE(DWS_MAINMENU));
	HMENU PluginsMenu = GetSubMenu(MenuHWND, 2);
	HMENU TestPluginItem = CreateMenu();
	_TestPluginItem = (UINT)TestPluginItem;
	AppendMenu(PluginsMenu, MF_STRING, (UINT)TestPluginItem, TEXT("TEST PLUGIN"));
	SetMenu(hWnd, MenuHWND);

	//----------------------------------------------------------------------------------------

	return TRUE;
}

BOOL CreateMainWindow(HINSTANCE hInstance, int iCmdShow)
{
	DWSWindowSize._MinSize.x = 450;
	DWSWindowSize._MinSize.y = 500;
	DWSWindowSize._MaxSize.x = 450;
	DWSWindowSize._MaxSize.y = 500;


	HWND m_hWnd;

	/* register window class */
	if (!RegisterClass(hInstance))
		return FALSE;

	RECT rect;
	rect.left = (GetSystemMetrics(SM_CXSCREEN) - DWSWindowSize._MinSize.x) / 2;
	rect.top = (GetSystemMetrics(SM_CYSCREEN) - DWSWindowSize._MinSize.y) / 2;

	m_hWnd = CreateWindow(
		szAppName, // window class name
		DWSCation.c_str(), // window caption
		WS_CAPTION | WS_SYSMENU, // window style
		rect.left, // initial x position
		rect.top, // initial y position
		DWSWindowSize._MinSize.x, // initial x size
		DWSWindowSize._MinSize.y, // initial y size
		NULL, // parent window handle
		NULL, // window menu handle
		hInstance, // program instance handle
		NULL
		); // creation parameters
	ShowWindow(m_hWnd, iCmdShow);
	UpdateWindow(m_hWnd);
	return TRUE;
}

// ÒÎ×ÊÀ ÂÕÎÄÀ Â ÏÐÎÃÐÀÌÌÓ
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(szCmdLine);
	_iCmdShow = iCmdShow;
	MSG msg;
	if(!CreateMainWindow(hInstance, iCmdShow))
	{
		MessageBox(NULL, TEXT("ERROR CREATE WINDOW"), TEXT("FATAL ERROR"), 0);
		return -1;
	}
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

POINT MouseDlgPos;

INT_PTR CALLBACK AboutDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		switch(LOWORD(wParam))
		{
		case IDOK_ABOUT_DLG:
			EndDialog(hDlg, 0);
			break;
		}
		return TRUE;
	}
	case WM_MOUSEMOVE:
		MouseDlgPos = GET_POINT_LPARAM(lParam);
		return TRUE;
	case WM_LBUTTONDOWN:
		// Down Mouse
		return TRUE;
	case WM_LBUTTONUP:
		// Up Mouse
		RECT _staticpos = GetLocalCoordinates(GetDlgItem(hDlg, ID_WEBSITE_STATIC));
		if(POINT_LOC_IN_RECT(_staticpos, MouseDlgPos))
			ShellExecute(NULL, TEXT("open"), TEXT("http://nullptr.space/"),	NULL, NULL, SW_SHOWNORMAL);
		return TRUE;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	case WM_DESTROY:
		return TRUE;
	}
	return FALSE;
}

// ÎÁÐÀÁÎÒÊÀ ÑÎÎÁÙÅÍÈÉ
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rect;

	bool DrawHelloText = false;
	switch (iMsg)
	{
		case WM_CREATE:
		{
			CreateComponents(hWnd);
			return 0;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_EXIT:
					PostQuitMessage(0);
					break;
				case ID_TEST_SHOWMSGBOX:
					DrawHelloText = !DrawHelloText;

					//DWSKernel::ShowMessageBox(hWnd, TEXT("TEST MESSAGE"), TEXT("TEST"));
					break;
				case  ID_ABOUT:
				{
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTDLG),
						hWnd, (DLGPROC)AboutDialogProc);
					break;
				}
				case Elements::MainButton:
				{
					using namespace DWSKernel;
					for (BaseFixer* _ptr : BaseFixer::getAllFixers())
					{
						if (!_ptr->isApply())
						{
							_ptr->exec();
							wcout() << TEXT("::") << _ptr->getName() << TEXT(" == ") << _ptr->isApply() << TEXT("::") << "\r\n";
						}
					}
					break;
				}
			}
			default:
	#pragma todo("Add callback for plugins menu")
				break;
			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			if ((HWND)lParam == textHwnd)
			{
				SetBkColor((HDC)wParam, RGB(255, 255, 255));
				SetTextColor((HDC)wParam, RGB(0, 162, 232));
				return (BOOL)GetSysColorBrush(COLOR_MENU);
			}
			break;
		}
		case WM_GETMINMAXINFO:
		{

			MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
			if (!mmi)
				return 0;
			mmi->ptMinTrackSize = DWSWindowSize._MinSize;
			mmi->ptMaxTrackSize = DWSWindowSize._MaxSize;
			return 0;
		}
		case WM_PAINT:
		{
			HDC hDC = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}