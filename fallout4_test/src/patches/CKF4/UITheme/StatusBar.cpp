#include "..\UIBaseWindow.h"
#include "VarCommon.h"
#include "StatusBar.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace StatusBar
			{
				namespace Render
				{
					VOID WINAPI DrawBorder(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						canvas.Fill(*pRect, RGB(38, 38, 38));
						canvas.Pen.Color = GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color);
						canvas.MoveTo(pRect->left, pRect->top);
						canvas.LineTo(pRect->right - pRect->left, pRect->top);
						canvas.Pen.Color = RGB(69, 69, 69);
						canvas.MoveTo(pRect->left, pRect->top + 1);
						canvas.LineTo(pRect->right - pRect->left, pRect->top + 1);
					}

					VOID WINAPI DrawBackground(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						canvas.Fill(*pRect, GetThemeSysColor(ThemeColor::ThemeColor_Default));
					}
				}
			}
		}
	}
}