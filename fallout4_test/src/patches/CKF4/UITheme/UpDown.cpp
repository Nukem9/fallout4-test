#include "..\UIBaseWindow.h"
#include "VarCommon.h"
#include "UpDown.h"

#include <Uxtheme.h>
#include <vsstyle.h>

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace UpDown
			{
				namespace Render
				{
					VOID WINAPI DrawUpArrow_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF cColor, COLORREF cShadowColor)
					{
						Graphics::CRECT rc_temp(*pRect);
						rc_temp.Inflate(-((rc_temp.Width - 4) >> 1), -((rc_temp.Height - 4) >> 1));

						std::vector<POINT> p[2];

						rc_temp.Width = (rc_temp.Width & 1) ? rc_temp.Width - 1 : rc_temp.Width;
						rc_temp.Offset(0, 1);

						p[0].resize(3);
						p[0][0] = { rc_temp.Left, rc_temp.Top + (rc_temp.Height >> 1) };
						p[0][1] = { rc_temp.Right, rc_temp.Top + (rc_temp.Height >> 1) };
						p[0][2] = { rc_temp.Left + (rc_temp.Width >> 1), rc_temp.Top };
						p[1] = p[0];
						p[1][0].y++;
						p[1][1].y++;
						p[1][2].y++;

						canvas.Pen.Color = cShadowColor;
						canvas.Brush.Color = cShadowColor;
						canvas.Polygon(p[0]);
						canvas.Pen.Color = cColor;
						canvas.Brush.Color = cColor;
						canvas.Polygon(p[1]);
					}

					VOID WINAPI DrawDownArrow_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF cColor, COLORREF cShadowColor)
					{
						Graphics::CRECT rc_temp(*pRect);
						rc_temp.Inflate(-((rc_temp.Width - 4) >> 1), -((rc_temp.Height - 4) >> 1));

						std::vector<POINT> p[2];

						rc_temp.Width = (rc_temp.Width & 1) ? rc_temp.Width - 1 : rc_temp.Width;
						rc_temp.Offset(0, -1);

						p[0].resize(3);
						p[0][0] = { rc_temp.Left, rc_temp.Top };
						p[0][1] = { rc_temp.Right, rc_temp.Top };
						p[0][2] = { rc_temp.Left + (rc_temp.Width >> 1), rc_temp.Top + (rc_temp.Height >> 1) };
						p[1] = p[0];
						p[1][0].y++;
						p[1][1].y++;
						p[1][2].y++;

						canvas.Pen.Color = cShadowColor;
						canvas.Brush.Color = cShadowColor;
						canvas.Polygon(p[0]);
						canvas.Pen.Color = cColor;
						canvas.Brush.Color = cColor;
						canvas.Polygon(p[1]);
					}

					VOID WINAPI DrawUp_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF cColorBorder,
						COLORREF cColorStart, COLORREF cColorEnd, COLORREF cColor, COLORREF cShadowColor)
					{
						Graphics::CRECT rc = *pRect;

						canvas.Pen.Color = cColorBorder;
						canvas.MoveTo(rc.Left, rc.Top);
						canvas.LineTo(rc.Left, rc.Bottom);

						rc.Left++;

						if (cColorStart == cColorEnd)
							canvas.Fill(rc, cColorStart);
						else
							canvas.GradientFill(rc, cColorEnd, cColorStart, Graphics::gdVert);

						DrawUpArrow_Stylesheet(canvas, (LPRECT)&rc, cColor, cShadowColor);
					}

					VOID WINAPI DrawUp_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawUp_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color),
							GetThemeSysColor(ThemeColor::ThemeColor_Default_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Default_Gradient_End), 
							GetThemeSysColor(ThemeColor::ThemeColor_Shape),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Shadow));
					}

					VOID WINAPI DrawUp_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawUp_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Hot_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Hot_Gradient_End),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Hot),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Shadow));
					}

					VOID WINAPI DrawUp_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawUp_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Pressed_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Pressed_Gradient_End),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Pressed),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Shadow));
					}

					VOID WINAPI DrawUp_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawUp_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color_Disabled),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Disabled_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Disabled_Gradient_End),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Disabled),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Shadow_Disabled));
					}

					VOID WINAPI DrawDown_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF cColorBorder,
						COLORREF cColorStart, COLORREF cColorEnd, COLORREF cColor, COLORREF cShadowColor)
					{
						Graphics::CRECT rc = *pRect;

						canvas.Pen.Color = cColorBorder;
						canvas.MoveTo(rc.Left, rc.Top);
						canvas.LineTo(rc.Left, rc.Bottom);

						rc.Left++;

						if (cColorStart == cColorEnd)
							canvas.Fill(rc, cColorStart);
						else
							canvas.GradientFill(rc, cColorStart, cColorEnd, Graphics::gdVert);

						DrawDownArrow_Stylesheet(canvas, (LPRECT)&rc, cColor, cShadowColor);
					}

					VOID WINAPI DrawDown_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawDown_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color),
							GetThemeSysColor(ThemeColor::ThemeColor_Default_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Default_Gradient_End),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Shadow));
					}

					VOID WINAPI DrawDown_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawDown_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Hot_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Hot_Gradient_End),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Hot),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Shadow));
					}

					VOID WINAPI DrawDown_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawDown_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Pressed_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Pressed_Gradient_End),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Pressed),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Shadow));
					}

					VOID WINAPI DrawDown_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawDown_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color_Disabled),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Disabled_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Disabled_Gradient_End),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Disabled),
							GetThemeSysColor(ThemeColor::ThemeColor_Shape_Shadow_Disabled));
					}
				}

				VOID WINAPI Initialize(HWND hWindow)
				{
					OpenThemeData(hWindow, VSCLASS_SPIN);
					// This function spoils the rendering process
					// SetWindowTheme(hWindow, nullptr, nullptr);
					SetWindowSubclass(hWindow, UpDownSubclass, 0, 0);
				}

				LRESULT CALLBACK UpDownSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
				{
					LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

					if ((uMsg == WM_ENABLE) || (uMsg == WM_SETFOCUS))
					{
						InvalidateRect(hWnd, NULL, TRUE);
						UpdateWindow(hWnd);
					}
					else if (uMsg == WM_PAINT)
					{
						// redraw border
						HDC hDC = GetWindowDC(hWnd);

						Core::Classes::UI::CRECT rc;
						Core::Classes::UI::CUICanvas canvas = hDC;

						GetWindowRect(hWnd, (LPRECT)&rc);
						rc.Offset(-rc.Left, -rc.Top);
				
						if (IsWindowEnabled(hWnd))
							canvas.Frame(rc, GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color));
						else
							canvas.Frame(rc, GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color_Disabled));

						ReleaseDC(hWnd, hDC);
					}

					return result;
				}
			}
		}
	}
}