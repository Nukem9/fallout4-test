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

#include "TESForm.h"
#include "TESCell.h"

#include "NiMain/NiTypes.h"

#pragma pack(push, 1)

namespace api {
	class TESObjectREFR : public TESForm {
	private:
		enum { eTypeID = ftReference };
	private:
		CHAR _pad1[0xA0];			// 0x28
		TESCell* _parentCell;		// 0xC8
		NiPoint3 _rotate;			// C0, C4, C8
		FLOAT _unkCC;
		NiPoint3 _position;			// D0, D4, D8
		FLOAT _unkDC;
		TESForm* _baseForm;			// E0
		LPVOID _unkE8;				// E8
	public:
		virtual ~TESObjectREFR(VOID) = 0;
	public:
		inline const TESCell* GetParentCellConst(VOID) const { return _parentCell; }
		inline const TESForm* GetParentConst(VOID) const { return _baseForm; }
		inline TESCell* GetParentCell(VOID) { return _parentCell; }
		inline TESForm* GetParent(VOID) { return _baseForm; }
	public:
		READ_PROPERTY(GetParentCell) TESCell* ParentCell;
		READ_PROPERTY(GetParent) TESForm* Parent;
		READ_PROPERTY(GetParentCellConst) const TESCell* ParentCellConst;
		READ_PROPERTY(GetParentConst) const TESForm* ParentConst;
	};
}

#pragma pack(pop)