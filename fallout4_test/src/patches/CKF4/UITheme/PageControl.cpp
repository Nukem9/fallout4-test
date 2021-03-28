#include "..\UIBaseWindow.h"
#include "VarCommon.h"
#include "PageControl.h"

#include <Uxtheme.h>
#include <vsstyle.h>

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace PageControl
			{
				namespace Render
				{
					VOID WINAPI DrawBorder(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						canvas.Frame(*pRect, GetThemeSysColor(ThemeColor_Divider_Highlighter_Ver2));
						Graphics::CRECT rc = *pRect;
						rc.Height = 2;
						canvas.Fill(rc, GetThemeSysColor(ThemeColor_Border_Window));
					}

					VOID WINAPI DrawButton_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF cColor)
					{
						canvas.Fill(*pRect, cColor);
					}

					VOID WINAPI DrawButton_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawButton_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor_Default));
					}

					VOID WINAPI DrawButton_SelectedAndFocused(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawButton_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor_Border_Window));
					}

					VOID WINAPI DrawButton_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawButton_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor_Divider_Highlighter_Ver2));
					}

					VOID WINAPI DrawButton_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawButton_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor_Divider_Color_Disabled_Ver2));
					}
				}

				namespace Event
				{
					VOID WINAPI OnBeforeDrawText(Graphics::CUICanvas& canvas, DWORD& flags)
					{
						flags |= DT_END_ELLIPSIS;
						canvas.ColorText = GetThemeSysColor(ThemeColor_Caption_Text);
					}
				}

				VOID WINAPI Initialize(HWND hWnd)
				{
					LONG_PTR style = GetWindowLongPtrA(hWnd, GWL_STYLE);
					if ((style & TCS_BUTTONS) == TCS_BUTTONS)
						SetWindowLongPtrA(hWnd, GWL_STYLE, (style & ~TCS_BUTTONS) | TCS_TABS);

					OpenThemeData(hWnd, VSCLASS_TAB);
				}
			}
		}
	}
}