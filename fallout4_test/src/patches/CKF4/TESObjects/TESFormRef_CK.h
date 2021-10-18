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

#include "TESForm_CK.h"
#include "TESCell_CK.h"

/* 
	The class of objects that are not something, but have a parent object.
	Such objects are used in Cells and Layers.
*/
class TESFormRef_CK : public TESBaseForm_CK {
private:
	CHAR _pad1[0xA0];			// 0x28
	TESCell_CK* _parentcell;	// 0xC8
	CHAR _pad2[0x20];			// 0xD0
	TESForm_CK* _parent;		// 0xF0
public:
	inline const TESCell_CK* GetParentCellConst(VOID) const {
		return _parentcell;
	}
	inline const TESForm_CK* GetParentConst(VOID) const {
		return _parent;
	}
	inline TESCell_CK* GetParentCell(VOID) {
		return _parentcell;
	}
	inline TESForm_CK* GetParent(VOID) {
		return _parent;
	}
	std::string GetEditID(VOID) const;
	CHAR GetTypeID(VOID) const;
public:
	READ_PROPERTY(GetEditID) std::string EditID;
	READ_PROPERTY(GetTypeID) CHAR TypeID;
	READ_PROPERTY(GetParentCell) TESCell_CK* ParentCell;
	READ_PROPERTY(GetParent) TESForm_CK* Parent;
	READ_PROPERTY(GetParentCellConst) const TESCell_CK* ParentCellConst;
	READ_PROPERTY(GetParentConst) const TESForm_CK* ParentConst;
};