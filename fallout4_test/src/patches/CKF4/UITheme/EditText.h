#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace EditText
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawEditText_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF clColor, 
						COLORREF clColorDividerStart, COLORREF clColorDividerEnd);

					VOID WINAPI DrawEditText_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawEditText_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawEditText_Focused(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawEditText_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}

				INT_PTR WINAPI OnCtlColorEdit(HDC hDC);
			}
		}
	}
}