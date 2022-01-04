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

using namespace api;

//#if 0
//std::unordered_map<uint64_t, TESFormArray*> FormReferenceMap;
//#else
//tbb::concurrent_unordered_map<uint64_t, TESObjectREFR*> FormReferenceMap;
//#endif

// for only CK (no game)
#include "../patches/CKF4/RenderWindow.h"

NiPoint3 g_offsetNewPositionForForm;

VOID TESObjectREFR::hk_SetNewPosition(NiPoint3* newPos) {
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

//VOID mapREFR::RemoveAllEntries(VOID) {
//	// Whatever I do, the lists are not initialized and are skipped.
//	
//	/*int64_t** rcx1;
//	int64_t* rax2;
//	int64_t** rdx3;
//	int64_t** rbx4;
//	int1_t zf5;
//	int64_t** rbx6;
//	int1_t zf7;
//
//	rcx1 = g7ff7ebd4b280;
//	if (rcx1) {
//		rax2 = *rcx1;
//		*(int32_t*)&rdx3 = 1;
//		*(int32_t*)((int64_t)&rdx3 + 4) = 0;
//		*rax2();
//	}
//	rbx4 = g7ff7ebd4b020;
//	g7ff7ebd4b280 = (int64_t**)0;
//	if (rbx4) {
//		fun_7ff7e50369dc(rbx4, 1);
//		zf5 = g7ff7e969d03c == 2;
//		if (!zf5) {
//			fun_7ff7e6fd5ad0(0x7ff7e969c580, 0x7ff7e969d03c);
//		}
//		rdx3 = rbx4;
//		fun_7ff7e6fd52d0(0x7ff7e969c580, rdx3);
//	}
//	rbx6 = g7ff7ebd4b028;
//	if (rbx6) {
//		fun_7ff7e50369dc(rbx6, rdx3);
//		zf7 = g7ff7e969d03c == 2;
//		if (!zf7) {
//			fun_7ff7e6fd5ad0(0x7ff7e969c580, 0x7ff7e969d03c);
//		}
//		fun_7ff7e6fd52d0(0x7ff7e969c580, rbx6);
//	}
//	g7ff7ebd4b028 = (int64_t**)0;
//	g7ff7ebd4b020 = (int64_t**)0;
//	return;*/
//	
//	
//	
//	
//	for (auto [k, v] : FormReferenceMap) {
//		if (v)
//			fastCall<VOID, TESObjectREFR*>(0xB19D40, v);
//	}
//
//	FormReferenceMap.clear();
//}
//
//TESObjectREFR* mapREFR::Find(uint64_t Key) {
//	auto itr = FormReferenceMap.find(Key);
//
//	if (itr != FormReferenceMap.end() && itr->second)
//		return itr->second;
//
//	return NULL;
//}
//
//VOID mapREFR::RemoveEntry(uint64_t Key) {
//	auto itr = FormReferenceMap.find(Key);
//
//	if (itr != FormReferenceMap.end() && itr->second) {
//		TESObjectREFR* ptr = itr->second;
//#if 0
//		FormReferenceMap.erase(itr);
//#else
//		FormReferenceMap.unsafe_erase(itr);
//#endif
//		if (ptr)
//			fastCall<VOID, TESObjectREFR*, INT>(0x3A4E80, ptr, 1);
//	}
//}
//
//BOOL mapREFR::Get(uint64_t Unused, uint64_t Key, TESFormArray** Value) {
//	// Function doesn't care if entry is nullptr, only if it exists
//	auto itr = FormReferenceMap.find(Key);
//
//	if (itr != FormReferenceMap.end()) {
//		*Value = itr->second;
//		return TRUE;
//	}
//
//	return FALSE;
//}