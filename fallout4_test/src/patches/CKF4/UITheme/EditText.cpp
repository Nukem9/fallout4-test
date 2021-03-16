#include "..\UIBaseWindow.h"
#include "VarCommon.h"
#include "EditText.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace EditText
			{
				namespace Render
				{
					VOID WINAPI DrawEditText_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF clColor, 
						COLORREF clColorDividerStart, COLORREF clColorDividerEnd)
					{
						Core::Classes::UI::CRECT rc_temp(*pRect);
						rc_temp.Inflate(-1, -1);

						if (clColorDividerStart == clColorDividerEnd)
							canvas.Fill(*pRect, clColorDividerStart);
						else
							canvas.GradientFill(*pRect, clColorDividerStart, clColorDividerEnd, Core::Classes::UI::gdVert);
						canvas.Fill(rc_temp, clColor);
					}

					VOID WINAPI DrawEditText_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawEditText_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Edit_Color),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Gradient_Start), 
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Gradient_End));
					}

					VOID WINAPI DrawEditText_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawEditText_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Edit_Color),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Hot_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Hot_Gradient_End));
					}
					VOID WINAPI DrawEditText_Focused(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawEditText_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Edit_Color),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Pressed),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Pressed));
					}

					VOID WINAPI DrawEditText_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawEditText_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Edit_Color_Disabled),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Disabled_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Disabled_Gradient_End));
					}
				}

				INT_PTR WINAPI OnCtlColorEdit(HDC hDC)
				{
					SetTextColor(hDC, GetThemeSysColor(ThemeColor_Text_4));
					SetBkColor(hDC, GetThemeSysColor(ThemeColor_Edit_Color));
					return reinterpret_cast<INT_PTR>(Comctl32GetSysColorBrushEx(COLOR_WINDOW));
				}
			}
		}
	}
}