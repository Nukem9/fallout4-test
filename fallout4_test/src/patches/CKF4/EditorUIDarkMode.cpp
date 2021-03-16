#include "../../common.h"

#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_vector.h>
#include <unordered_set>
#include <vssym32.h>
#include <Richedit.h>

#include "EditorUIDarkMode.h"
#include "UIBaseWindow.h"
#include "MainWindow.h"

/// THEMES

#include "UITheme/VarCommon.h"
#include "UITheme/MDIClient.h"
#include "UITheme/PopupMenu.h"
#include "UITheme/ListView.h"
#include "UITheme/TreeView.h"
#include "UITheme/ToolBar.h"
#include "UITheme/PushButton.h"
#include "UITheme/CheckBox.h"
#include "UITheme/RadioButton.h"
#include "UITheme/GroupBox.h"
#include "UITheme/EditText.h"
#include "UITheme/ComboBox.h"
#include "UITheme/StatusBar.h"
#include "UITheme/ScrollBar.h"
#include "UITheme/TrackBar.h"
#include "UITheme/UpDown.h"
#include "UITheme/ListBox.h"
#include "UITheme/ProgressBar.h"
#include "UITheme/ColorBox.h"

#include <fstream>

namespace Theme = Core::UI::Theme;

namespace EditorUIDarkMode
{
	std::ofstream of;

	typedef struct UITimeOfDayComponentsTag
	{
		Core::Classes::UI::CUIToolWindow hWndToolBar;
		Core::Classes::UI::CUIBaseControl hWndLabel, hWndTrackBar, hWndEdit;
	} UITimeOfDayComponents, *LPUITimeOfDayComponents, *PUITimeOfDayComponents;

	static UITimeOfDayComponents OldUITimeOfDayComponents;
	static UITimeOfDayComponents NewUITimeOfDayComponents;

	enum class ThemeType
	{
		None,
		ScrollBar,
		StatusBar,
		MDIClient,
		Static,
		Edit,
		RichEdit,
		Button,
		ComboBox,
		Header,
		ListBox,
		ListView,
		TreeView,
		TabControl,
		ToolBar,
		TrackBar,
		ProgressBar,
		PopupMenu,
		Spin
	};

	struct string_equal_to
	{
		inline bool operator()(const std::string_view& lhs, const std::string_view& rhs) const
		{
			return !_stricmp(lhs.data(), rhs.data());
		}
	};

	const std::unordered_map<std::string_view, ThemeType, std::hash<std::string_view>, string_equal_to> TargetWindowThemes
	{
		{ STATUSCLASSNAMEA, ThemeType::StatusBar },
		{ "mdiclient", ThemeType::MDIClient },
		{ WC_STATICA, ThemeType::Static },
		{ UPDOWN_CLASS, ThemeType::Spin },
		{ WC_EDITA, ThemeType::Edit },
		{ RICHEDIT_CLASSA, ThemeType::RichEdit },
		{ "RICHEDIT50W", ThemeType::RichEdit },
		{ PROGRESS_CLASSA, ThemeType::ProgressBar },
		{ WC_BUTTONA, ThemeType::Button },
		{ WC_LISTBOXA, ThemeType::ListBox },
		{ WC_COMBOBOXA, ThemeType::ComboBox },
		{ WC_COMBOBOXEXA, ThemeType::ComboBox },
		{ WC_HEADERA, ThemeType::Header },
		{ WC_LISTVIEWA, ThemeType::ListView },
		{ WC_TREEVIEWA, ThemeType::TreeView },
		{ WC_TABCONTROLA, ThemeType::TabControl },
		{ TOOLBARCLASSNAMEA, ThemeType::ToolBar },
		{ TRACKBAR_CLASSA, ThemeType::TrackBar },
		{ "#32768", ThemeType::PopupMenu },
	};

	const std::unordered_set<std::string_view> PermanentWindowSubclasses
	{
		"Creation Kit",
		"ActivatorClass",
		"AlchemyClass",
		"ArmorClass",
		"CreatureClass",
		"LockPickClass",
		"NPCClass",
		"WeaponClass",
		"FaceClass",
		"PlaneClass",
		"MonitorClass",
		"ViewerClass",
		"SpeakerClass",
		"LandClass",
		"RenderWindow",
		"#32770",
		// "BABYGRID",
		// "NiTreeCtrl",
	};

	tbb::concurrent_unordered_map<HTHEME, ThemeType> ThemeHandles;
	BOOL EnableThemeHooking;

	static WNDPROC OldPopupMenuWndClass = NULL;

	VOID Initialize(VOID)
	{
		EnableThemeHooking = TRUE;

		of.open("test.log");
	}

	VOID InitializeThread(VOID)
	{
		if (EnableThemeHooking)
			SetWindowsHookExA(WH_CALLWNDPROC, CallWndProcCallback, nullptr, GetCurrentThreadId());
	}

	BOOL IsUIDarkMode(VOID)
	{
		return EnableThemeHooking;
	}

	LRESULT WINAPI TimeOfDayClassWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HDC hdc;

		static NMHDR hdr;

		switch (uMsg)
		{
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLORSTATIC:
		{
			if (hdc = reinterpret_cast<HDC>(wParam); hdc)
			{
				SetTextColor(hdc, Theme::GetThemeSysColor(Theme::ThemeColor_Text_3));
				SetBkColor(hdc, Theme::GetThemeSysColor(Theme::ThemeColor_Default));
			}

			return reinterpret_cast<INT_PTR>(Theme::Comctl32GetSysColorBrushEx(COLOR_BTNFACE));
		case WM_HSCROLL:
			switch (LOWORD(wParam)) {
			case SB_ENDSCROLL:
			case SB_LEFT:
			case SB_RIGHT:
			case SB_LINELEFT:
			case SB_LINERIGHT:
			case SB_PAGELEFT:
			case SB_PAGERIGHT:
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
				if (NewUITimeOfDayComponents.hWndTrackBar.Handle == (HWND)lParam)
				{
					hdr.code = NM_RELEASEDCAPTURE;
					hdr.hwndFrom = (HWND)OldUITimeOfDayComponents.hWndTrackBar.Handle;
					hdr.idFrom = 0x16D3;

					LONG lPos = (LONG)SendMessageA((HWND)lParam, TBM_GETPOS, 0, 0);
					OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_SETPOS, TRUE, lPos);

					// fake change time of day
					MainWindow::GetWindowObj().Perform(WM_NOTIFY, 0x16D3, (LPARAM)&hdr);
					MainWindow::GetWindowObj().Perform(WM_NOTIFY, NM_RELEASEDCAPTURE, (LPARAM)&hdr);

					POINT Range = {
						(LONG)OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_GETRANGEMIN, 0, 0),
						(LONG)OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_GETRANGEMAX, 0, 0)
					};

					CHAR szBuf[35] = { 0 };
					FLOAT v = (24.0 * lPos) / (Range.y - Range.x);
					sprintf_s(szBuf, "%.2f", v);
					NewUITimeOfDayComponents.hWndEdit.Caption = szBuf;
				}
				break;
			}
			break;
		}
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		}

		return 0;
	}

	HWND WINAPI Comctl32CreateToolbarEx_1(HWND hwnd, DWORD ws, UINT wID, INT nBitmaps, HINSTANCE hBMInst, UINT_PTR wBMID, LPCTBBUTTON lpButtons,
		INT iNumButtons, INT dxButton, INT dyButton, INT dxBitmap, INT dyBitmap, UINT uStructSize)
	{
		HIMAGELIST hImageList = ImageList_LoadImageA(g_hModule, MAKEINTRESOURCE(IDB_BITMAP5), 16, 0, RGB(56, 56, 56), IMAGE_BITMAP, LR_COLOR | LR_VGACOLOR);
		HWND ret = CreateToolbarEx(hwnd, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, wID, nBitmaps, 
			NULL, NULL, lpButtons, iNumButtons - 2 /*delete two divider*/, dxButton, dyButton, dxBitmap, dyBitmap, uStructSize);

		SendMessageA(ret, TB_SETIMAGELIST, 0, (LPARAM)hImageList);
		SendMessageA(ret, TB_SETBITMAPSIZE, 0, MAKELPARAM(16, 16));
		ShowWindow(ret, SW_SHOWNORMAL);

		OldUITimeOfDayComponents.hWndToolBar = ret;
		NewUITimeOfDayComponents.hWndToolBar = ret;

		WNDCLASSEXA wc = { 0 };

		wc.cbSize = sizeof(wc);
		wc.hbrBackground = (HBRUSH)Theme::Comctl32GetSysColorBrushEx(COLOR_BTNFACE);
		wc.hInstance = hBMInst;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.hCursor = LoadCursorA(hBMInst, IDC_ARROW);
		wc.lpszClassName = "TimeOfDayClass";
		wc.lpfnWndProc = (WNDPROC)&TimeOfDayClassWndProc;

		Assert(RegisterClassExA(&wc));

		// To organize the normal operation of another list of components, 
		// you need to group them into a separate window with the processing of messages, 
		// I don't know what the developers were thinking, but not for nothing was the behavior of this thing ambiguous, 
		// not to mention the rendering of the component itself

		HWND hPanel = CreateWindowExA(0, wc.lpszClassName, "", WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 1280, 2, 250, 24, ret, (HMENU)NULL, hBMInst, NULL);

		NewUITimeOfDayComponents.hWndLabel = CreateWindowExA(0, WC_STATICA, "Time of day", WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 5, 90, 20, hPanel, (HMENU)0x16D2, hBMInst, NULL);
		NewUITimeOfDayComponents.hWndTrackBar = CreateWindowExA(0, TRACKBAR_CLASSA, "", WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS, 90, 3, 110, 18, hPanel, (HMENU)0x16D3, hBMInst, NULL);
		NewUITimeOfDayComponents.hWndEdit = CreateWindowExA(0, WC_STATICA, "0.00", WS_CHILD | WS_VISIBLE | ES_CENTER | ES_READONLY, 200, 5, 50, 20, hPanel, (HMENU)0x16D4, hBMInst, NULL);

		Theme::ThemeFont.Apply(NewUITimeOfDayComponents.hWndLabel.Handle);
		Theme::ThemeFont.Apply(NewUITimeOfDayComponents.hWndEdit.Handle);

		return ret; 
	}

	HIMAGELIST WINAPI Comctl32ImageList_LoadImageA_1(HINSTANCE hi, LPCSTR lpbmp, INT cx, INT cGrow, COLORREF crMask, UINT uType, UINT uFlags)
	{
		return ImageList_LoadImageA(g_hModule, MAKEINTRESOURCE(IDB_BITMAP6), cx, cGrow, crMask, IMAGE_BITMAP, LR_COLOR | LR_VGACOLOR);
	}

	HWND WINAPI Comctl32CreateWindowEx_1(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, INT x, INT y,
		INT nWidth, INT nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
	{
		HWND ret = CreateWindowExA(dwExStyle, lpClassName, lpWindowName, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

		return ret;
	}

	VOID WINAPI HideOldTimeOfDayComponents(VOID)
	{
		// I will hide the old ones, but I will rely on them when sending messages, however, in the end, I will fake the event to change the time of day

		OldUITimeOfDayComponents.hWndLabel = GetDlgItem(OldUITimeOfDayComponents.hWndToolBar.Handle, 0x16D2);
		OldUITimeOfDayComponents.hWndTrackBar = GetDlgItem(OldUITimeOfDayComponents.hWndToolBar.Handle, 0x16D3);
		OldUITimeOfDayComponents.hWndEdit = GetDlgItem(OldUITimeOfDayComponents.hWndToolBar.Handle, 0x16D4);

		ShowWindow(OldUITimeOfDayComponents.hWndLabel.Handle, SW_HIDE);
		ShowWindow(OldUITimeOfDayComponents.hWndTrackBar.Handle, SW_HIDE);
		ShowWindow(OldUITimeOfDayComponents.hWndEdit.Handle, SW_HIDE);

		POINT Range = {
			(LONG)OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_GETRANGEMIN, 0, 0),
			(LONG)OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_GETRANGEMAX, 0, 0)
		};

		NewUITimeOfDayComponents.hWndTrackBar.Perform(TBM_SETRANGE, (WPARAM)TRUE, MAKELPARAM(Range.x, Range.y));
		NewUITimeOfDayComponents.hWndTrackBar.Perform(TBM_SETPOS, (WPARAM)TRUE, (LONG)((10 * (Range.y - Range.x)) / 24));
		NewUITimeOfDayComponents.hWndEdit.Caption = "10.00";
	}

	LRESULT CALLBACK CallWndProcCallback(INT nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HC_ACTION)
		{
			auto messageData = reinterpret_cast<CWPSTRUCT*>(lParam);

			switch (messageData->message)
			{
			case WM_CREATE:
			{
				CHAR className[MAX_PATH] = { 0 };
				GetClassNameA(messageData->hwnd, className, MAX_PATH - 1);

				of << "WM_CREATE: " << className << std::endl;

				SetWindowSubclass(messageData->hwnd, WindowSubclass, 0, reinterpret_cast<DWORD_PTR>(WindowSubclass));
			}
			break;

			case WM_INITDIALOG:
			{
				SetWindowSubclass(messageData->hwnd, DialogWindowSubclass, 0, reinterpret_cast<DWORD_PTR>(DialogWindowSubclass));
			}
			break;
			}
		}

		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}
	
	LRESULT CALLBACK WindowSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		switch (uMsg)
		{
		case WM_CTLCOLOREDIT:
		{
			if (HDC hdc = reinterpret_cast<HDC>(wParam); hdc)
				return Theme::EditText::OnCtlColorEdit(hdc);
		}
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		{
			if (HDC hdc = reinterpret_cast<HDC>(wParam); hdc)
			{
				SetTextColor(hdc, Theme::GetThemeSysColor(Theme::ThemeColor_Text_3));
				SetBkColor(hdc, Theme::GetThemeSysColor(Theme::ThemeColor_Default));
			}

			return reinterpret_cast<INT_PTR>(Theme::Comctl32GetSysColorBrushEx(COLOR_BTNFACE));
		}
		case WM_CTLCOLORLISTBOX:
		{
			if (HDC hdc = reinterpret_cast<HDC>(wParam); hdc)
				return Theme::ListBox::OnCtlColorListBox(hWnd, hdc);
		}
		}

		LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

		switch (uMsg)
		{
		case WM_INITDIALOG:
		case WM_CREATE:
		{
			// Theme settings are initialized after WM_CREATE is processed
			auto themeType = ThemeType::None;
			HTHEME scrollBarTheme = nullptr;

			CHAR className[256] = { 0 };
			GetClassNameA(hWnd, className, ARRAYSIZE(className));
			UINT uStyles = GetWindowLongPtrA(hWnd, GWL_STYLE);
			UINT uStylesEx = GetWindowLongPtrA(hWnd, GWL_EXSTYLE);

			if (auto itr = TargetWindowThemes.find(className); itr != TargetWindowThemes.end())
				themeType = itr->second;

			switch (themeType)
			{
			case ThemeType::MDIClient:
				Theme::MDIClient::Initialize(hWnd);
				break;

			case ThemeType::PopupMenu:
				Theme::PopupMenu::Initialize(hWnd);
				break;

			case ThemeType::RichEdit:
			{
				CHARFORMATA format = { 0 };
				format.cbSize = sizeof(CHARFORMATA);
				format.dwMask = CFM_COLOR;
				format.crTextColor = Theme::GetThemeSysColor(Theme::ThemeColor_Text_4);
				SendMessageA(hWnd, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&format);
				SendMessageA(hWnd, EM_SETBKGNDCOLOR, FALSE, Theme::GetThemeSysColor(Theme::ThemeColor_Edit_Color));
			}
			break;

			case ThemeType::Button:
				if (((uStylesEx & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE) && ((uStylesEx & WS_EX_STATICEDGE) == WS_EX_STATICEDGE))
				{
					// Imaged
				}
				break;

			case ThemeType::ProgressBar:
				Theme::ProgressBar::Initialize(hWnd);
				break;

			case ThemeType::ToolBar:
				Theme::ToolBar::Initialize(hWnd);
			break;

			case ThemeType::Spin:
				Theme::UpDown::Initialize(hWnd);
				break;

			case ThemeType::ListBox:
				scrollBarTheme = Theme::ListBox::Initialize(hWnd);
				break;

			case ThemeType::ListView:
				scrollBarTheme = Theme::ListView::Initialize(hWnd);
				break;

			case ThemeType::TreeView:
				scrollBarTheme = Theme::TreeView::Initialize(hWnd);
				break;

			case ThemeType::TabControl:
				SetWindowLongPtrA(hWnd, GWL_STYLE, (GetWindowLongPtrA(hWnd, GWL_STYLE) & ~TCS_BUTTONS) | TCS_TABS);
				SetWindowTheme(hWnd, nullptr, nullptr);
				break;
			default:
				if ((uStyles & SS_SUNKEN) == SS_SUNKEN)
					Theme::ColorBox::Initialize(hWnd, (uStyles & SS_OWNERDRAW) == SS_OWNERDRAW);
				else if (((uStyles & WS_CAPTION) == WS_CAPTION) && ((uStyles & WS_CHILD) == WS_CHILD))
				{
					// Window
				}
				break;
			}

			if (scrollBarTheme)
			{
				ThemeHandles.emplace(scrollBarTheme, ThemeType::ScrollBar);

				// TODO: This is a hack...the handle should be valid as long as at least one window is still open
				// PS: This kills the scroll rendering in the Cell View (perchik71) (first draw)
				// CloseThemeData(scrollBarTheme);
			}

			if (HTHEME windowTheme = GetWindowTheme(hWnd); windowTheme)
				ThemeHandles.emplace(windowTheme, themeType);

			if (!PermanentWindowSubclasses.count(className))
				RemoveWindowSubclass(hWnd, WindowSubclass, 0);
		}
		break;
		
		case WM_INITMENUPOPUP:
		{
			// The message about the initialization of the pop-up menu, set all its elements as owner draw
			Theme::PopupMenu::OnInitPopupMenu(hWnd, (HMENU)wParam);
		}
		return S_OK;

		case WM_MEASUREITEM:
		{
			LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;
			
			if (lpmis->CtlType == ODT_MENU)
				// Calc size menu item
				Theme::PopupMenu::OnMeasureItem(hWnd, lpmis);
		}
		return TRUE;

		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;

			if (lpdis->CtlType == ODT_MENU)
				// Paint menu item
				Theme::PopupMenu::OnDrawItem(hWnd, lpdis);
		}
		return TRUE;

		case WM_NOTIFY:
		{
			LPNMHDR nmhdr = (LPNMHDR)lParam;

			// Custom drawing (ToolBar)
			if (nmhdr->code == NM_CUSTOMDRAW)
			{
				auto themeType = ThemeType::None;

				CHAR className[256] = { 0 };
				GetClassNameA(nmhdr->hwndFrom, className, ARRAYSIZE(className));

				if (auto itr = TargetWindowThemes.find(className); itr != TargetWindowThemes.end())
					themeType = itr->second;

				if (themeType == ThemeType::ToolBar)
					return Theme::ToolBar::OnCustomDraw(hWnd, (LPNMTBCUSTOMDRAW)lParam);
			}
		}
		break;
		}

		return result;
	}

	LRESULT CALLBACK DialogWindowSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		LRESULT result = WindowSubclass(hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);

		switch (uMsg)
		{
		case WM_PAINT:
		{
			/// The following code completely kills draw the dialog for selecting a color and saving a file and other standard Windows OS dialogs (perchik71)
			/// maybe WM_NCPAINT ???

			// Special override for DialogBoxIndirectParam (MessageBox) since the bottom half doesn't get themed correctly. ReactOS
			// says this is MSGBOX_IDTEXT.
		/*	if (GetDlgItem(hWnd, 0xFFFF))
			{
				if (HDC hdc = GetDC(hWnd); hdc)
				{
					RECT windowArea;
					GetClientRect(hWnd, &windowArea);

					FillRect(hdc, &windowArea, Comctl32GetSysColorBrush(COLOR_BTNFACE));
					ReleaseDC(hWnd, hdc);
				}
			}*/
		}
		break;
		}

		return result;
	}

	DWORD WINAPI Comctl32GetSysColor(INT nIndex)
	{
		return Theme::Comctl32GetSysColor(nIndex);
	}

	HBRUSH WINAPI Comctl32GetSysColorBrush(INT nIndex)
	{
		return Theme::Comctl32GetSysColorBrush(nIndex);
	}

	HRESULT WINAPI Comctl32DrawThemeText(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCWSTR pszText, INT cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect)
	{
		SetBkMode(hdc, TRANSPARENT);

		auto themeType = ThemeType::None;
		if (auto itr = ThemeHandles.find(hTheme); itr != ThemeHandles.end())
			themeType = itr->second;

		switch (themeType)
		{
		case ThemeType::StatusBar:
			dwTextFlags |= DT_CENTER;
			SetTextColor(hdc, Theme::GetThemeSysColor(Theme::ThemeColor_Text_2));
			break;
		case ThemeType::Button:
			if (iStateId == PBS_DISABLED)
				SetTextColor(hdc, Theme::GetThemeSysColor(Theme::ThemeColor_Text_1));
			else
				SetTextColor(hdc, Theme::GetThemeSysColor(Theme::ThemeColor_Text_4));
			break;
		default:
			SetTextColor(hdc, Theme::GetThemeSysColor(Theme::ThemeColor_Text_4));
			break;
		}			

		RECT temp = *pRect;
		DrawTextW(hdc, pszText, cchText, &temp, dwTextFlags);

		return S_OK;
	}

	HRESULT WINAPI Comctl32DrawThemeBackground(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, LPCRECT pClipRect)
	{
		auto themeType = ThemeType::None;

		if (auto itr = ThemeHandles.find(hTheme); itr != ThemeHandles.end())
			themeType = itr->second;

		Core::Classes::UI::CUICanvas Canvas(hdc);

		if (themeType == ThemeType::ScrollBar)
		{
			switch (iPartId)
			{
			case SBP_THUMBBTNHORZ:	// Horizontal drag bar
			{
				if (iStateId == SCRBS_HOT || iStateId == SCRBS_PRESSED)
					Theme::ScrollBar::Render::DrawSliderHorz_Focused(Canvas, pRect);
				else
					Theme::ScrollBar::Render::DrawSliderHorz_Normal(Canvas, pRect);
			}
			return S_OK;
			case SBP_THUMBBTNVERT:	// Vertical drag bar
			{
				if (iStateId == SCRBS_HOT || iStateId == SCRBS_PRESSED)
					Theme::ScrollBar::Render::DrawSliderVert_Focused(Canvas, pRect);
				else
					Theme::ScrollBar::Render::DrawSliderVert_Normal(Canvas, pRect);
			}
			return S_OK;

			case SBP_LOWERTRACKHORZ:// Horizontal background
			case SBP_UPPERTRACKHORZ:// Horizontal background
			{
				Theme::ScrollBar::Render::DrawBackgroundHorz(Canvas, pRect);
			}
			return S_OK;

			case SBP_LOWERTRACKVERT:// Vertical background
			case SBP_UPPERTRACKVERT:// Vertical background
			{
				Theme::ScrollBar::Render::DrawBackgroundVert(Canvas, pRect);
			}
			return S_OK;

			case SBP_ARROWBTN:		// Arrow button
			{
				// Assume the perspective of the arrow pointing upward ( /\ ) in GDI coordinates. NOTE: (0, 0) is the
				// top left corner of the screen. Awful code, but it works.
				const INT arrowWidth = std::ceil(std::abs(pRect->left - pRect->right) * 0.4f);
				const INT arrowHeight = std::ceil(std::abs(pRect->top - pRect->bottom) * 0.35f);

				std::array<DWORD, 6> counts{ 2, 2, 2, 2, 2, 2 };
				std::array<POINT, 12> verts
				{ {
						// Left segment
						{ 0, -0 },
						{ (arrowWidth / 2) + 1, -arrowHeight + 2 },

						{ 0, -1 },
						{ (arrowWidth / 2) + 1, -arrowHeight + 1 },

						{ 0, -2 },
						{ (arrowWidth / 2) + 1, -arrowHeight + 0 },

						// Right segment (final vertex Y adjusted to avoid a stray pixel)
						{ arrowWidth - 1, -0 },
						{ arrowWidth / 2, -arrowHeight + 2 },

						{ arrowWidth - 1, -1 },
						{ arrowWidth / 2, -arrowHeight + 1 },

						{ arrowWidth - 1, -2 },
						{ arrowWidth / 2, -arrowHeight + 1 },
					} };

				BOOL isHot = FALSE;
				BOOL isDisabled = FALSE;
				BOOL isHorz = FALSE;

				for (auto& vert : verts)
				{
					switch (iStateId)
					{
					case ABS_UPHOT:// Up
					case ABS_UPPRESSED:
						isHot = TRUE;
					case ABS_UPDISABLED:
						isDisabled = TRUE;
					case ABS_UPNORMAL:
					case ABS_UPHOVER:
						vert.x += pRect->left + arrowHeight - 1;
						vert.y += pRect->bottom - arrowHeight;
						break;

					case ABS_DOWNHOT:// Down
					case ABS_DOWNPRESSED:
						isHot = TRUE;
					case ABS_DOWNDISABLED:
						isDisabled = TRUE;
					case ABS_DOWNNORMAL:
					case ABS_DOWNHOVER:
						vert.x += pRect->left + arrowHeight - 1;
						vert.y = -vert.y + pRect->top + arrowHeight - 1;
						break;

					case ABS_LEFTHOT:// Left
					case ABS_LEFTPRESSED:
						isHot = TRUE;
					case ABS_LEFTDISABLED:
						isDisabled = TRUE;
					case ABS_LEFTNORMAL:
					case ABS_LEFTHOVER:
						isHorz = TRUE;
						std::swap(vert.x, vert.y);
						vert.x += pRect->right - arrowHeight;
						vert.y += pRect->top + arrowHeight - 1;
						break;

					case ABS_RIGHTHOT:// Right
					case ABS_RIGHTPRESSED:
						isHot = TRUE;
					case ABS_RIGHTDISABLED:
						isDisabled = TRUE;
					case ABS_RIGHTNORMAL:
					case ABS_RIGHTHOVER:
						isHorz = TRUE;
						std::swap(vert.x, vert.y);
						vert.x = -vert.x + pRect->left + arrowHeight - 1;
						vert.y += pRect->top + arrowHeight - 1;
						break;
					}
				}

				if (isHot)
				{
					Theme::ScrollBar::Render::DrawButton_Hot(Canvas, pRect);

					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape);
				}
				else if (isDisabled)
				{
					Theme::ScrollBar::Render::DrawButton_Disabled(Canvas, pRect);

					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape_Disabled);
				}
				else
				{
					if (isHorz)
						Theme::ScrollBar::Render::DrawBackgroundHorz(Canvas, pRect);
					else
						Theme::ScrollBar::Render::DrawBackgroundVert(Canvas, pRect);

					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape);
				}

				if (isDisabled)
					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape_Shadow_Disabled);
				else
					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape_Shadow);

				PolyPolyline(Canvas.Handle, verts.data(), counts.data(), counts.size());
				
				if (isHot)
					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape);
				else if (isDisabled)
					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape_Disabled);
				else
					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape);

				for (auto i = verts.begin(); i != verts.end(); i++)
					(*i).y++;

				PolyPolyline(Canvas.Handle, verts.data(), counts.data(), counts.size());
			}
			return S_OK;

			case SBP_GRIPPERHORZ:	// Horizontal resize scrollbar
			case SBP_GRIPPERVERT:	// Vertical resize scrollbar
				break;
			case SBP_SIZEBOX:		// Resize box, bottom right
			case SBP_SIZEBOXBKGND:	// Resize box, background, unused
				Canvas.Fill(*pRect, Theme::GetThemeSysColor(Theme::ThemeColor_Default));
				return S_OK;
			}
		}
		else if (themeType == ThemeType::StatusBar)
		{
			switch (iPartId)
			{
			case 0:
			{
				// Outside border (top, right)
				Theme::StatusBar::Render::DrawBorder(Canvas, pRect);
			}
			return S_OK;

			case SP_PANE:
			case SP_GRIPPERPANE:
			case SP_GRIPPER:
			{
				// Everything else
				Theme::StatusBar::Render::DrawBackground(Canvas, pRect);
			}
			return S_OK;
			}
		}
		else if (themeType == ThemeType::Spin)
		{
			switch (iPartId)
			{
				case SPNP_UP:
				{
					switch (iStateId)
					{
					case DNS_HOT:
						Theme::UpDown::Render::DrawUp_Hot(Canvas, pRect);
						break;
					case DNS_PRESSED:
						Theme::UpDown::Render::DrawUp_Pressed(Canvas, pRect);
						break;
					case DNS_DISABLED:
						Theme::UpDown::Render::DrawUp_Disabled(Canvas, pRect);
						break;
					default:
						Theme::UpDown::Render::DrawUp_Normal(Canvas, pRect);
						break;
					}
				}
				return S_OK;

				case SPNP_DOWN:
				{
					switch (iStateId)
					{
					case UPS_HOT:
						Theme::UpDown::Render::DrawDown_Hot(Canvas, pRect);
						break;
					case UPS_PRESSED:
						Theme::UpDown::Render::DrawDown_Pressed(Canvas, pRect);
						break;
					case UPS_DISABLED:
						Theme::UpDown::Render::DrawDown_Disabled(Canvas, pRect);
						break;
					default:
						Theme::UpDown::Render::DrawDown_Normal(Canvas, pRect);
						break;
					}
				}
				return S_OK;
				
			}
		}
		else if (themeType == ThemeType::Edit)
		{
			switch (iPartId)
			{
			case EP_EDITBORDER_NOSCROLL:
			{
				switch (iStateId)
				{
				case ETS_DISABLED:
					Theme::EditText::Render::DrawEditText_Disabled(Canvas, pRect);
					break;
				case ETS_SELECTED:
				case ETS_FOCUSED:
					Theme::EditText::Render::DrawEditText_Focused(Canvas, pRect);
					break;
				case ETS_HOT:
					Theme::EditText::Render::DrawEditText_Hot(Canvas, pRect);
					break;
				default:
					Theme::EditText::Render::DrawEditText_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case EP_EDITBORDER_VSCROLL:
			{
				switch (iStateId)
				{
				case EPSV_DISABLED:
					Theme::EditText::Render::DrawEditText_Disabled(Canvas, pRect);
					break;
				case EPSV_FOCUSED:
					Theme::EditText::Render::DrawEditText_Focused(Canvas, pRect);
					break;
				case EPSV_HOT:
					Theme::EditText::Render::DrawEditText_Hot(Canvas, pRect);
					break;
				default:
					Theme::EditText::Render::DrawEditText_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case EP_EDITBORDER_HSCROLL:
			{
				switch (iStateId)
				{
				case EPSH_DISABLED:
					Theme::EditText::Render::DrawEditText_Disabled(Canvas, pRect);
					break;
				case EPSH_FOCUSED:
					Theme::EditText::Render::DrawEditText_Focused(Canvas, pRect);
					break;
				case EPSH_HOT:
					Theme::EditText::Render::DrawEditText_Hot(Canvas, pRect);
					break;
				default:
					Theme::EditText::Render::DrawEditText_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case EP_EDITBORDER_HVSCROLL:
			{
				switch (iStateId)
				{
				case EPSHV_DISABLED:
					Theme::EditText::Render::DrawEditText_Disabled(Canvas, pRect);
					break;
				case EPSHV_FOCUSED:
					Theme::EditText::Render::DrawEditText_Focused(Canvas, pRect);
					break;
				case EPSHV_HOT:
					Theme::EditText::Render::DrawEditText_Hot(Canvas, pRect);
					break;
				default:
					Theme::EditText::Render::DrawEditText_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;
			}
		}
		else if (themeType == ThemeType::ProgressBar)
		{
			switch (iPartId)
			{
			case PP_BAR:
			case PP_TRANSPARENTBAR:
			{
				Theme::ProgressBar::Render::DrawBar(Canvas, pRect);
			}
			return S_OK;

			case PP_FILL:
			{
				Theme::ProgressBar::Render::DrawFill(Canvas, pRect);
			}
			return S_OK;
			}

			// skip other
			return S_OK;
		}
		else if (themeType == ThemeType::Button)
		{
			switch (iPartId)
			{
			case BP_PUSHBUTTON:
			{
				switch (iStateId)
				{
				case PBS_HOT:
					Theme::PushButton::Render::DrawPushButton_Hot(Canvas, pRect);
					break;
				case PBS_DISABLED:
					Theme::PushButton::Render::DrawPushButton_Disabled(Canvas, pRect);
					break;
				case PBS_PRESSED:
					Theme::PushButton::Render::DrawPushButton_Pressed(Canvas, pRect);
					break;
				default:
					Theme::PushButton::Render::DrawPushButton_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case BP_GROUPBOX:
			{
				switch (iStateId)
				{
				case GBS_DISABLED:
					Theme::GroupBox::Render::DrawGroupBox_Disabled(Canvas, pRect);
					break;
				default:
					Theme::GroupBox::Render::DrawGroupBox_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case BP_CHECKBOX:
			{
				switch (iStateId)
				{
				case CBS_MIXEDDISABLED:
				case CBS_IMPLICITDISABLED:
				case CBS_EXCLUDEDDISABLED:
				case CBS_UNCHECKEDDISABLED:
					Theme::PushButton::Render::DrawPushButton_Disabled(Canvas, pRect);
					break;
				case CBS_UNCHECKEDNORMAL:
					Theme::PushButton::Render::DrawPushButton_Normal(Canvas, pRect);
					break;
				case CBS_UNCHECKEDHOT:
					Theme::PushButton::Render::DrawPushButton_Hot(Canvas, pRect);
					break;
				case CBS_UNCHECKEDPRESSED:
					Theme::PushButton::Render::DrawPushButton_Pressed(Canvas, pRect);
					break;
				case CBS_CHECKEDDISABLED:
					Theme::PushButton::Render::DrawPushButton_Disabled(Canvas, pRect);
					Theme::CheckBox::Render::DrawCheck_Disabled(Canvas, pRect);
					break;
				case CBS_CHECKEDNORMAL:
					Theme::PushButton::Render::DrawPushButton_Normal(Canvas, pRect);
					Theme::CheckBox::Render::DrawCheck_Normal(Canvas, pRect);
					break;
				case CBS_CHECKEDHOT:
					Theme::PushButton::Render::DrawPushButton_Hot(Canvas, pRect);
					Theme::CheckBox::Render::DrawCheck_Hot(Canvas, pRect);
					break;
				case CBS_CHECKEDPRESSED:
					Theme::PushButton::Render::DrawPushButton_Pressed(Canvas, pRect);
					Theme::CheckBox::Render::DrawCheck_Pressed(Canvas, pRect);
					break;
				default:
					return DrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
				}
			}
			return S_OK;

			case BP_RADIOBUTTON:
			{
				switch (iStateId)
				{
				case CBS_MIXEDDISABLED:
				case CBS_IMPLICITDISABLED:
				case CBS_EXCLUDEDDISABLED:
				case CBS_UNCHECKEDDISABLED:
					Theme::PushButton::Render::DrawPushButton_Disabled(Canvas, pRect);
					break;
				case CBS_UNCHECKEDNORMAL:
					Theme::PushButton::Render::DrawPushButton_Normal(Canvas, pRect);
					break;
				case CBS_UNCHECKEDHOT:
					Theme::PushButton::Render::DrawPushButton_Hot(Canvas, pRect);
					break;
				case CBS_UNCHECKEDPRESSED:
					Theme::PushButton::Render::DrawPushButton_Pressed(Canvas, pRect);
					break;
				case CBS_CHECKEDDISABLED:
					Theme::PushButton::Render::DrawPushButton_Disabled(Canvas, pRect);
					Theme::RadioButton::Render::DrawRadioButton_Disabled(Canvas, pRect);
					break;
				case CBS_CHECKEDNORMAL:
					Theme::PushButton::Render::DrawPushButton_Normal(Canvas, pRect);
					Theme::RadioButton::Render::DrawRadioButton_Normal(Canvas, pRect);
					break;
				case CBS_CHECKEDHOT:
					Theme::PushButton::Render::DrawPushButton_Hot(Canvas, pRect);
					Theme::RadioButton::Render::DrawRadioButton_Hot(Canvas, pRect);
					break;
				case CBS_CHECKEDPRESSED:
					Theme::PushButton::Render::DrawPushButton_Pressed(Canvas, pRect);
					Theme::RadioButton::Render::DrawRadioButton_Pressed(Canvas, pRect);
					break;
				default:
					return DrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
				}
			}
			return S_OK;
			}
		}
		else if (themeType == ThemeType::TrackBar)
		{
			switch (iPartId)
			{
			case TKP_TRACK:
			case TKP_TRACKVERT:
			{
				Theme::TrackBar::Render::DrawTrack(Canvas, pRect);
			}
			return S_OK;
			case TKP_THUMB:
			case TKP_THUMBVERT:
			case TKP_THUMBBOTTOM:
			case TKP_THUMBTOP:
			case TKP_THUMBLEFT:
			case TKP_THUMBRIGHT:
			{
				switch (iStateId)
				{
				case TUS_HOT:
					Theme::TrackBar::Render::DrawSlider_Hot(Canvas, pRect);
					break;
				case TUS_PRESSED:
					Theme::TrackBar::Render::DrawSlider_Pressed(Canvas, pRect);
					break;
				case TUS_FOCUSED:
					Theme::TrackBar::Render::DrawSlider_Focused(Canvas, pRect);
					break;
				case TUS_DISABLED:
					Theme::TrackBar::Render::DrawSlider_Disabled(Canvas, pRect);
					break;
				default:
					Theme::TrackBar::Render::DrawSlider_Normal(Canvas, pRect);
					break;
				}
			}
			}

			return S_OK;
		}
		else if (themeType == ThemeType::ComboBox)
		{
			switch (iPartId)
			{
			case CP_READONLY:			// Main control
			{
				switch (iStateId)
				{
				case CBRO_HOT:
					Theme::PushButton::Render::DrawPushButton_Hot(Canvas, pRect);
					break;
				case CBRO_DISABLED:
					Theme::PushButton::Render::DrawPushButton_Disabled(Canvas, pRect);
					break;
				case CBRO_PRESSED:
					Theme::PushButton::Render::DrawPushButton_Pressed(Canvas, pRect);
					break;
				default:
					Theme::PushButton::Render::DrawPushButton_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case CP_BORDER:				// Main control with text edit
			{
				// Special case: dropdown arrow needs to be drawn
				DrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

				switch (iStateId)
				{
				case CBB_HOT:
					Theme::EditText::Render::DrawEditText_Hot(Canvas, pRect);
					break;
				case CBB_DISABLED:
					Theme::EditText::Render::DrawEditText_Disabled(Canvas, pRect);
					break;
				case CBB_FOCUSED:
					Theme::EditText::Render::DrawEditText_Focused(Canvas, pRect);
					break;
				default:
					Theme::EditText::Render::DrawEditText_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case CP_DROPDOWNBUTTONRIGHT:// Dropdown arrow
			case CP_DROPDOWNBUTTONLEFT:	// Dropdown arrow
				if (iStateId == CBXS_DISABLED)
					Theme::ComboBox::Render::DrawArrow_Disabled(Canvas, pRect);
				else
					Theme::ComboBox::Render::DrawArrow_Normal(Canvas, pRect);
				return S_OK;

			case CP_DROPDOWNBUTTON:
			case CP_BACKGROUND:
			case CP_TRANSPARENTBACKGROUND:
			case CP_CUEBANNER:
			case CP_DROPDOWNITEM:
				return S_OK;
			}
		}
		else if (themeType == ThemeType::Header)
		{
			static HBRUSH headerFill = CreateSolidBrush(RGB(77, 77, 77));

			switch (iPartId)
			{
			case 0:
			case HP_HEADERITEM:
			case HP_HEADERITEMLEFT:
			case HP_HEADERITEMRIGHT:
			{
				HGDIOBJ oldPen = SelectObject(hdc, GetStockObject(DC_PEN));

				for (int i = 0; i < 2; i++)
				{
					if (i == 0)
						SetDCPenColor(hdc, RGB(65, 65, 65));
					else
						SetDCPenColor(hdc, RGB(29, 38, 48));

					std::array<DWORD, 2> counts{ 2, 2 };
					std::array<POINT, 4> verts
					{ {
							// Right border
							{ pRect->right - 2 + i, pRect->top },
							{ pRect->right - 2 + i, pRect->bottom },

							// Bottom border
							{ pRect->left - 1, pRect->bottom - 2 + i },
							{ pRect->right - 2, pRect->bottom - 2 + i },
						} };

					PolyPolyline(hdc, verts.data(), counts.data(), counts.size());
				}

				// Fill background on hover (1px padding for border shadow)
				if ((iPartId == 0 && iStateId == HIS_HOT) ||
					(iPartId == HP_HEADERITEM && iStateId == HIS_HOT) ||
					(iPartId == HP_HEADERITEMLEFT && iStateId == HILS_HOT) ||
					(iPartId == HP_HEADERITEMRIGHT && iStateId == HIRS_HOT))
				{
					RECT padded = { pRect->left - 1, pRect->top, pRect->right - 1, pRect->bottom - 1 };

					FillRect(hdc, &padded, headerFill);
				}

				SelectObject(hdc, oldPen);
			}
			return S_OK;
			}
		}
		else if (themeType == ThemeType::TabControl)
		{
			static HBRUSH tabControlButtonBorder = CreateSolidBrush(RGB(130, 135, 144));// RGB(83, 83, 83)
			static HBRUSH tabControlButtonFill = CreateSolidBrush(RGB(56, 56, 56));

			if (iPartId == TABP_PANE)
				return S_OK;

			switch (iPartId)
			{
			case TABP_TABITEM:				// TCS_MULTILINE middle buttons
			case TABP_TABITEMLEFTEDGE:		// TCS_MULTILINE leftmost button
			case TABP_TABITEMRIGHTEDGE:		// TCS_MULTILINE rightmost button
			case TABP_TABITEMBOTHEDGE:		// TCS_MULTILINE ???
			case TABP_TOPTABITEM:			// Middle buttons
			case TABP_TOPTABITEMLEFTEDGE:	// Leftmost button
			case TABP_TOPTABITEMRIGHTEDGE:	// Rightmost button
			case TABP_TOPTABITEMBOTHEDGE:	// ???
			{
				RECT paddedRect = *pRect;
				RECT insideRect = { pRect->left + 1, pRect->top + 1, pRect->right - 1, pRect->bottom - 1 };

				bool isHover = (iPartId == TABP_TABITEM && iStateId == TIS_HOT) ||
					(iPartId == TABP_TABITEMLEFTEDGE && iStateId == TILES_HOT) ||
					(iPartId == TABP_TABITEMRIGHTEDGE && iStateId == TIRES_HOT) ||
					(iPartId == TABP_TABITEMBOTHEDGE && iStateId == TIBES_HOT) ||
					(iPartId == TABP_TOPTABITEM && iStateId == TTIS_HOT) ||
					(iPartId == TABP_TOPTABITEMLEFTEDGE && iStateId == TTILES_HOT) ||
					(iPartId == TABP_TOPTABITEMRIGHTEDGE && iStateId == TTIRES_HOT) ||
					(iPartId == TABP_TOPTABITEMBOTHEDGE && iStateId == TTIBES_HOT);

				if ((iPartId == TABP_TABITEM && iStateId == TIS_SELECTED) ||
					(iPartId == TABP_TABITEMLEFTEDGE && iStateId == TILES_SELECTED) ||
					(iPartId == TABP_TABITEMRIGHTEDGE && iStateId == TIRES_SELECTED) ||
					(iPartId == TABP_TABITEMBOTHEDGE && iStateId == TIBES_SELECTED) ||
					(iPartId == TABP_TOPTABITEM && iStateId == TTIS_SELECTED) ||
					(iPartId == TABP_TOPTABITEMLEFTEDGE && iStateId == TTILES_SELECTED) ||
					(iPartId == TABP_TOPTABITEMRIGHTEDGE && iStateId == TTIRES_SELECTED) ||
					(iPartId == TABP_TOPTABITEMBOTHEDGE && iStateId == TTIBES_SELECTED))
				{
					paddedRect.top += 1;
					paddedRect.bottom -= 2;

					// Allow the rect to overlap so the bottom border outline is removed
					insideRect.top += 1;
					insideRect.bottom += 1;
				}

				FrameRect(hdc, &paddedRect, tabControlButtonBorder);
				FillRect(hdc, &insideRect, isHover ? tabControlButtonBorder : tabControlButtonFill);
			}
			return S_OK;
			}
		}

		return DrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
	}
}