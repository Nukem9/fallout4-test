#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace PopupMenu
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawBackground_NonClientArray(Graphics::CUICanvas& canvas);
					VOID WINAPI DrawItem_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawItem_Focused(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawItem_Divider(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawItem_Checkbox(Graphics::CUICanvas& canvas, LPCRECT pRect, BOOL bSelected);
					VOID WINAPI DrawItem_Arrow(Graphics::CUICanvas& canvas, LPCRECT pRect, BOOL bSelected);
				}

				BOOL WINAPI IsSystemPopupMenu(HWND hWindow, HMENU hMenu);
				BOOL WINAPI IsSystemPopupMenuBlindly(HWND hWindow);

				VOID WINAPI Initialize(HWND hWindow);
				LRESULT CALLBACK PopupMenuSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

				namespace Event
				{
					VOID WINAPI OnInitPopupMenu(HWND hWindow, HMENU hMenu);
					VOID WINAPI OnDrawNoClientPopupMenu(HWND hWindow, HDC hDC);
					VOID WINAPI OnMeasureItem(HWND hWindow, LPMEASUREITEMSTRUCT lpMeasureItem);
					VOID WINAPI OnDrawItem(HWND hWindow, LPDRAWITEMSTRUCT lpDrawItem);
					LRESULT WINAPI OnNcCalcSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
				}
			}
		}
	}
}