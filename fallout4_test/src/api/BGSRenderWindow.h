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

#pragma once

#include "NiMain/NiTypes.h"
#include "BSTArray.h"
#include "TESCell.h"

#pragma pack(push, 1)

namespace api {

	class BGSRenderWindow;
	class BGSRenderComponent;

	// 8
	class BGSRenderComponent {
	public:
		virtual ~BGSRenderComponent(VOID);
	private:
		BGSRenderWindow* render;
	public:
		inline BGSRenderWindow* GetRenderWindow(VOID) const { return render; }
	public:
		READ_PROPERTY(GetRenderWindow) BGSRenderWindow* RenderWindow;
	};

	// 38
	class BGSRenderObjectPickerData : public BGSRenderComponent {
	private:
		LPVOID UnkPtr10;
		LPVOID UnkPtr18;
		NiPoint3 MouseWorldCoord;
		NiPoint3 UnkPoint3f;
	};

	class BGSRenderObjectPicker : public BGSRenderComponent {
	public:
		virtual ~BGSRenderObjectPicker(VOID);
		virtual VOID Unk08(VOID);
	private:
		BGSRenderObjectPickerData* pikerData;
		LPVOID* Unk18;							// extended ??? in code exists this+18
	public:
		
	};

	class BGSRenderWindow {
	public:
		BGSRenderWindow(VOID) = default;
		virtual ~BGSRenderWindow(VOID);
	private:
		LPVOID lpUnk08;
		BSTArray<LPVOID> UnkArray10;
		CHAR pad28[0x80];
		BGSRenderObjectPicker* objPicker;
		LPVOID UnkClassB0;
		LPVOID UnkClassB8;
		LPVOID UnkClassC0;
		LPVOID UnkClassC8;
		LPVOID UnkClassD0;
		LPVOID UnkClassD8;
		LPVOID UnkClassE0;
		LPVOID UnkPointerE8;
		LPVOID UnkClassF0;
		TESCell* currentCell;
		struct SizeInfo {
			DWORD dwUnk100;
			NiPoint2T<DWORD> pWindowSize;
			NiPoint2T<DWORD> pMouseOffset;
			NiPoint2T<DWORD> pMouseOffsetScaled;
			DWORD dwUnk11C;
		} sizeInfo;
		CHAR pad120[0x18];
		DWORD dwFlags;					// flags??? rsi+138 hmmm..
		DWORD dwUnk13C;					// the value jumps like crazy
		DWORD dwUnk140;					// ^---
		BSTArray<DWORD> UnkArray148;	// the array is chock-full of values similar to FormID
	public:
		inline TESCell* GetCurrentCell(VOID) const { return currentCell; }
		inline DWORD GetRenderWindowClientWidth(VOID) const { return sizeInfo.pWindowSize.x; }
		inline DWORD GetRenderWindowClientHeight(VOID) const { return sizeInfo.pWindowSize.y; }
		inline NiPoint2T<DWORD> GetRenderWindowMouseOffset(VOID) const { return sizeInfo.pMouseOffset; }
		inline NiPoint2T<DWORD> GetRenderWindowMouseOffsetScaled(VOID) const { return sizeInfo.pMouseOffsetScaled; }
	};
}

#pragma pack(pop)

