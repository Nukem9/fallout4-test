#include "VarCommon.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			const Core::Classes::UI::CUIFont ThemeFont("Microsoft Sans Serif", 8, {}, Core::Classes::UI::fqClearTypeNatural, Core::Classes::UI::fpVariable);

			constexpr COLORREF szConstDark[56]
			{
				RGB(52, 52, 52),											// ThemeColor_Default
				RGB(32, 32, 32),											// ThemeColor_ListView_Color
				RGB(32, 32, 32),											// ThemeColor_TreeView_Color
				RGB(32, 32, 32),											// ThemeColor_Edit_Color
				RGB(37, 37, 37),											// ThemeColor_Edit_Color_Disabled
				RGB(137, 137, 137),											// ThemeColor_Text_1
				RGB(156, 156, 156),											// ThemeColor_Text_2
				RGB(200, 200, 200),											// ThemeColor_Text_3
				RGB(220, 220, 220),											// ThemeColor_Text_4
				RGB(16, 16, 16),											// ThemeColor_MDIWindow
				RGB(78, 78, 78),											// ThemeColor_Default_Gradient_Start
				RGB(61, 61, 61),											// ThemeColor_Default_Gradient_End
				RGB(52, 52, 52),											// ThemeColor_Divider_Highlighter_Disabled_Gradient_Start
				RGB(64, 64, 64),											// ThemeColor_Divider_Highlighter_Disabled_Gradient_End
				RGB(20, 20, 20),											// ThemeColor_Divider_Color
				RGB(38, 38, 38),											// ThemeColor_Divider_Color_Disabled
				RGB(105, 105, 105),											// ThemeColor_Divider_Highlighter
				RGB(125, 125, 125),											// ThemeColor_Divider_Highlighter_Hot
				RGB(38, 38, 38),											// ThemeColor_Divider_Color_Ver2
				RGB(45, 45, 45),											// ThemeColor_Divider_Color_Disabled_Ver2
				RGB(73, 73, 73),											// ThemeColor_Divider_Highlighter_Ver2
				RGB(61, 61, 61),											// ThemeColor_Divider_Highlighter_Disabled_Ver2
				RGB(52, 52, 52),											// ThemeColor_Divider_Highlighter_Gradient_Start
				RGB(72, 72, 72),											// ThemeColor_Divider_Highlighter_Gradient_End
				RGB(52, 52, 52),											// ThemeColor_Divider_Highlighter_Hot_Gradient_Start
				RGB(80, 95, 114),											// ThemeColor_Divider_Highlighter_Hot_Gradient_End
				RGB(74, 102, 150),											// ThemeColor_Divider_Highlighter_Pressed
				RGB(38, 38, 38),											// ThemeColor_Button_Pressed_Gradient_Start
				RGB(29, 29, 29),											// ThemeColor_Button_Pressed_Gradient_End
				RGB(98, 98, 98),											// ThemeColor_Button_Hot_Gradient_Start
				RGB(78, 78, 78),											// ThemeColor_Button_Hot_Gradient_End
				RGB(31, 31, 31),											// ThemeColor_Button_Pressed_Divider
				RGB(79, 79, 79),											// ThemeColor_Button_Light_Disabled_Divider
				RGB(65, 65, 65),											// ThemeColor_Button_Disabled_Gradient_Start
				RGB(57, 57, 57),											// ThemeColor_Button_Disabled_Gradient_End
				RGB(51, 51, 51),											// ThemeColor_CheckBox_Gradient_Start
				RGB(20, 20, 20),											// ThemeColor_CheckBox_Gradient_End
				RGB(39, 39, 39),											// ThemeColor_ScrollBar_Gradient_Start
				RGB(27, 27, 27),											// ThemeColor_ScrollBar_Gradient_End
				RGB(190, 190, 190),											// ThemeColor_Shape
				RGB(131, 162, 194),											// ThemeColor_Shape_Hot
				RGB(220, 220, 220),											// ThemeColor_Shape_Pressed
				RGB(110, 110, 110),											// ThemeColor_Shape_Disabled
				RGB(20, 20, 20),											// ThemeColor_Shape_Shadow
				RGB(38, 38, 38),											// ThemeColor_Shape_Shadow_Disabled
				RGB(70, 149, 205),											// ThemeColor_Progress_Fill_Gradient_Start
				RGB(42, 101, 143),											// ThemeColor_Progress_Fill_Gradient_End
				RGB(87, 159, 209),											// ThemeColor_Progress_Fill_Highlighter
				RGB(103, 175, 225),											// ThemeColor_Progress_Fill_Highlighter_Up
				RGB(0, 122, 204),											// ThemeColor_Border_Window,
				RGB(255, 255, 255),											// ThemeColor_StatusBar_Text
				RGB(255, 255, 255),											// ThemeColor_Caption_Text
				RGB(34, 34, 34),											// ThemeColor_Header_Normal_Gradient_Start
				RGB(27, 27, 27),											// ThemeColor_Header_Normal_Gradient_End
				RGB(44, 44, 44),											// ThemeColor_Header_Hot_Gradient_Start
				RGB(37, 37, 37)												// ThemeColor_Header_Hot_Gradient_End
			};

			HBRUSH hThemeEditBackBrush;
			static HBRUSH szConstDarkBrush[7]
			{
				CreateSolidBrush(szConstDark[ThemeColor_Default - 1]),
				CreateSolidBrush(szConstDark[ThemeColor_Text_3 - 1]),
				CreateSolidBrush(szConstDark[ThemeColor_Edit_Color - 1]),
				CreateSolidBrush(szConstDark[ThemeColor_Divider_Color - 1]),
				CreateSolidBrush(szConstDark[ThemeColor_Text_4 - 1]),
				CreateSolidBrush(szConstDark[ThemeColor_Border_Window - 1]),
				CreateSolidBrush(szConstDark[ThemeColor_Divider_Color - 1])
			};

			static Theme generalCurentTheme = Theme_Dark;

			VOID WINAPI SetTheme(Theme theme)
			{
				generalCurentTheme = theme;

				// ???
				hThemeEditBackBrush = CreateSolidBrush(GetThemeSysColor(ThemeColor_Edit_Color));
			}

			COLORREF WINAPI GetThemeSysColor(const ThemeColor color)
			{
				return szConstDark[color - 1];
			}

			DWORD WINAPI Comctl32GetSysColor(INT nIndex)
			{
				switch (nIndex)
				{
				case COLOR_BTNFACE: return GetThemeSysColor(ThemeColor_Default);
				case COLOR_BTNTEXT: return GetThemeSysColor(ThemeColor_Text_3);
				case COLOR_WINDOW: return GetThemeSysColor(ThemeColor_Edit_Color);
				case COLOR_WINDOWFRAME: return GetThemeSysColor(ThemeColor_Divider_Color);
				case COLOR_WINDOWTEXT: return GetThemeSysColor(ThemeColor_Text_4);
				case COLOR_ACTIVEBORDER: return GetThemeSysColor(ThemeColor_Border_Window);
				case COLOR_INACTIVEBORDER: return GetThemeSysColor(ThemeColor_Divider_Color);
				default:
					return ::GetSysColor(nIndex);
				}
			}

			HBRUSH WINAPI Comctl32GetSysColorBrushEx(INT nIndex, BOOL reCreate)
			{
				if (!reCreate)
					return Comctl32GetSysColorBrush(nIndex);

				return CreateSolidBrush(Comctl32GetSysColor(nIndex));
			}

			HBRUSH WINAPI Comctl32GetSysColorBrush(INT nIndex)
			{
				switch (nIndex)
				{
				case COLOR_BTNFACE: return szConstDarkBrush[0];
				case COLOR_BTNTEXT: return szConstDarkBrush[1];
				case COLOR_WINDOW: return szConstDarkBrush[2];
				case COLOR_WINDOWFRAME: return szConstDarkBrush[3];
				case COLOR_WINDOWTEXT: return szConstDarkBrush[4];
				case COLOR_ACTIVEBORDER: return szConstDarkBrush[5];
				case COLOR_INACTIVEBORDER: return szConstDarkBrush[6];
				default:
					return ::GetSysColorBrush(nIndex);
				}
			}
		}
	}
}