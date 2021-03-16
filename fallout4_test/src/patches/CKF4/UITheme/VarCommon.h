#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			extern const Core::Classes::UI::CUIFont ThemeFont;

			enum Theme
			{
				Theme_Light,
				Theme_Gray,
				Theme_DarkGray,
				Theme_Dark
			};

			inline VOID WINAPI SetTheme(Theme theme);

			enum ThemeColor
			{
				ThemeColor_Default = 1,
				ThemeColor_ListView_Color,
				ThemeColor_TreeView_Color,
				ThemeColor_Edit_Color,
				ThemeColor_Edit_Color_Disabled,
				ThemeColor_Text_1,
				ThemeColor_Text_2,
				ThemeColor_Text_3,
				ThemeColor_Text_4,
				ThemeColor_MDIWindow,
				ThemeColor_Default_Gradient_Start,
				ThemeColor_Default_Gradient_End,
				ThemeColor_Divider_Highlighter_Disabled_Gradient_Start,
				ThemeColor_Divider_Highlighter_Disabled_Gradient_End,
				ThemeColor_Divider_Color,
				ThemeColor_Divider_Color_Disabled,
				ThemeColor_Divider_Highlighter,
				ThemeColor_Divider_Highlighter_Hot,
				ThemeColor_Divider_Color_Ver2,
				ThemeColor_Divider_Color_Disabled_Ver2,
				ThemeColor_Divider_Highlighter_Ver2,
				ThemeColor_Divider_Highlighter_Disabled_Ver2,
				ThemeColor_Divider_Highlighter_Gradient_Start,
				ThemeColor_Divider_Highlighter_Gradient_End,
				ThemeColor_Divider_Highlighter_Hot_Gradient_Start,
				ThemeColor_Divider_Highlighter_Hot_Gradient_End,
				ThemeColor_Divider_Highlighter_Pressed,
				ThemeColor_Button_Pressed_Gradient_Start,
				ThemeColor_Button_Pressed_Gradient_End,
				ThemeColor_Button_Hot_Gradient_Start,
				ThemeColor_Button_Hot_Gradient_End,
				ThemeColor_Button_Pressed_Divider,
				ThemeColor_Button_Light_Disabled_Divider,
				ThemeColor_Button_Disabled_Gradient_Start,
				ThemeColor_Button_Disabled_Gradient_End,
				ThemeColor_CheckBox_Gradient_Start,
				ThemeColor_CheckBox_Gradient_End,
				ThemeColor_ScrollBar_Gradient_Start,
				ThemeColor_ScrollBar_Gradient_End,
				ThemeColor_Shape,
				ThemeColor_Shape_Hot,
				ThemeColor_Shape_Pressed,
				ThemeColor_Shape_Disabled,
				ThemeColor_Shape_Shadow,
				ThemeColor_Shape_Shadow_Disabled,
				ThemeColor_Progress_Fill_Gradient_Start,
				ThemeColor_Progress_Fill_Gradient_End,
				ThemeColor_Progress_Fill_Highlighter,
				ThemeColor_Progress_Fill_Highlighter_Up
			};

			inline COLORREF WINAPI GetThemeSysColor(const ThemeColor color);

			DWORD WINAPI Comctl32GetSysColor(INT nIndex);
			HBRUSH WINAPI Comctl32GetSysColorBrush(INT nIndex);
			HBRUSH WINAPI Comctl32GetSysColorBrushEx(INT nIndex, BOOL reCreate = TRUE);
		}
	}
}