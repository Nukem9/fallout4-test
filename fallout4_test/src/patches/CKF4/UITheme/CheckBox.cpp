#include "..\UIBaseWindow.h"
#include "VarCommon.h"
#include "CheckBox.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace CheckBox
			{
				namespace Render
				{
					VOID WINAPI DrawCheck_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF clShadow, COLORREF clColor)
					{
						Graphics::CRECT rc_temp = *pRect;

						POINT ps[6] = {
							{ 3 + rc_temp.Left, 5 + rc_temp.Top },
							{ 5 + rc_temp.Left, 7 + rc_temp.Top },
							{ 10 + rc_temp.Left, 2 + rc_temp.Top },
							{ 11 + rc_temp.Left, 3 + rc_temp.Top },
							{ 5 + rc_temp.Left, 9 + rc_temp.Top },
							{ 2 + rc_temp.Left, 6 + rc_temp.Top }
						};

						POINT p[6] = {
							{ 3 + rc_temp.Left, 6 + rc_temp.Top },
							{ 5 + rc_temp.Left, 8 + rc_temp.Top },
							{ 10 + rc_temp.Left, 3 + rc_temp.Top },
							{ 11 + rc_temp.Left, 4 + rc_temp.Top },
							{ 5 + rc_temp.Left, 10 + rc_temp.Top },
							{ 2 + rc_temp.Left, 7 + rc_temp.Top }
						};

						canvas.Pen.Color = clShadow;
						canvas.Brush.Color = clShadow;
						canvas.Polygon(ps, 6);

						canvas.Pen.Color = clColor;
						canvas.Brush.Color = clColor;
						canvas.Polygon(p, 6);
					}

					VOID WINAPI DrawCheck_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawCheck_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Shape_Shadow),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape));
					}

					VOID WINAPI DrawCheck_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawCheck_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Shape_Shadow),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Hot));
					}

					VOID WINAPI DrawCheck_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawCheck_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Shape_Shadow),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Pressed));
					}

					VOID WINAPI DrawCheck_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawCheck_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Shape_Shadow_Disabled),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Disabled));
					}
				}
			}
		}
	}
}