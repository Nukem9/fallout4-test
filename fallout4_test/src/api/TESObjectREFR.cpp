//////////////////////////////////////////
/*
* Copyright (c) 2020-2021 Perchik71 <email:perchik71@outlook.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
//////////////////////////////////////////

#include "TESObjectREFR.h"

// for only CK (no game)
#include "../patches/CKF4/RenderWindow.h"

NiPoint3 g_offsetNewPositionForForm;

VOID api::TESObjectREFR::hk_SetNewPosition(NiPoint3* newPos) {
	if (g_LoadType == GAME_EXECUTABLE_TYPE::GAME_FALLOUT4)
		SetNewPosition(this, newPos);
	else {
		// for only CK
		// 
		// In CK, an error was made when moving the light form along with other forms at the same time

		if (Parent->TypeID != api::TESObjectREFR::ftLight) {
			g_offsetNewPositionForForm.x = newPos->x - _position.x;
			g_offsetNewPositionForForm.y = newPos->y - _position.y;
			g_offsetNewPositionForForm.z = newPos->z - _position.z;

			SetNewPosition(this, newPos);
		}
		else {
			auto forms = RenderWindow::Picker.GetForms();
			// The coordinates is reset, but only if we have selected several objects.
			if (forms->QSize() == 1)
				SetNewPosition(this, newPos);
			else {
				for (auto i = 0; i < forms->QSize(); i++) {
					auto form = (TESObjectREFR*)(*forms).QBuffer()[i];

					// In general, light is buggy with any primitive in the form of a box, but it is unlikely that anyone will move it along with a collision, for example.

					if (form->Parent->FormID == 0x1F7) {
						_position.x += g_offsetNewPositionForForm.x;
						_position.y += g_offsetNewPositionForForm.y;
						_position.z += g_offsetNewPositionForForm.z;
						newPos->x = _position.x;
						newPos->y = _position.y;
						newPos->z = _position.z;
						return;
					}
				}

				SetNewPosition(this, newPos);
			}
		}
	}
}