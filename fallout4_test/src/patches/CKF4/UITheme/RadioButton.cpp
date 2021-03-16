#include "..\UIBaseWindow.h"
#include "VarCommon.h"
#include "RadioButton.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace RadioButton
			{
				namespace Render
				{
					VOID WINAPI DrawRadioButton_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF clColor)
					{
						Core::Classes::UI::CRECT rc_temp[2];

						rc_temp[0] = *pRect;
						rc_temp[0].Inflate(-3, -3);
						rc_temp[1] = rc_temp[0];
						rc_temp[1].Inflate(-1, -1);

						canvas.GradientFill(rc_temp[0], GetThemeSysColor(ThemeColor::ThemeColor_CheckBox_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_CheckBox_Gradient_End), Core::Classes::UI::gdVert);
						canvas.Fill(rc_temp[1], clColor);
					}

					VOID WINAPI DrawRadioButton_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawRadioButton_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Shape));
					}

					VOID WINAPI DrawRadioButton_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawRadioButton_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Shape_Hot));
					}

					VOID WINAPI DrawRadioButton_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawRadioButton_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Shape_Pressed));
					}

					VOID WINAPI DrawRadioButton_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawRadioButton_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Shape_Disabled));
					}
				}
			}
		}
	}
}