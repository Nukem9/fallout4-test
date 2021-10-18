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

class TESCell_CK;
class TESFormRef_CK;
class TESLayer_CK : public TESForm_CK {
private:
	CHAR _pad1[0x8];
	BSTArray<TESFormRef_CK*> _items;
	BSTArray<TESLayer_CK*> _childs;
public:
	inline DWORD GetChildsCount(VOID) const {
		return (DWORD)_childs.QSize();
	}
	inline const TESLayer_CK** GetChildsArrayConst(VOID) const {
		return (const TESLayer_CK**)_childs.QBuffer();
	}
	inline TESLayer_CK** GetChildsArray(VOID) {
		return (TESLayer_CK**)_childs.QBuffer();
	}
	inline TESLayer_CK* GetChildById(DWORD dwId) {
		return _childs[dwId];
	}
	inline DWORD GetItemsCount(VOID) const {
		return (DWORD)_items.QSize();
	}
	inline const TESFormRef_CK** GetItemsArrayConst(VOID) const {
		return (const TESFormRef_CK**)_items.QBuffer();
	}
	inline TESFormRef_CK** GetItemsArray(VOID) {
		return (TESFormRef_CK**)_items.QBuffer();
	}
	inline TESFormRef_CK* GetItemById(DWORD dwId) {
		return _items[dwId];
	}
public:
	DWORD GetItemsCountInCell(const TESCell_CK* cell) const;
public:
	inline TESFormRef_CK* operator[](DWORD dwId) {
		return _items[dwId];
	}
	inline const TESFormRef_CK* operator[](DWORD dwId) const {
		return _items[dwId];
	}
public:
	READ_PROPERTY(GetChildsCount) DWORD ChildsCount;
	READ_PROPERTY(GetChildsArrayConst) const TESLayer_CK** ChildsArrayConst;
	READ_PROPERTY(GetChildsArray) TESLayer_CK** ChildsArray;
	READ_PROPERTY(GetItemsCount) DWORD ItemsCount;
	READ_PROPERTY(GetItemsArrayConst) const TESFormRef_CK** ItemsArrayConst;
	READ_PROPERTY(GetItemsArray) TESFormRef_CK** ItemsArray;
public:
	virtual ~TESLayer_CK(VOID);
};
