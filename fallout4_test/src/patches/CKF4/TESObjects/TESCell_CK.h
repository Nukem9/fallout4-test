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

class TESFormRef_CK;
class TESCell_CK : public TESForm_CK {
public:
	struct TESMaxHeightData {
		DWORD unk00;			// 00
		DWORD unk04;			// 04
		CHAR* maxHeightBlock;	// 08
	};
private:
	enum {
		cfInterior = 1,
		cfExterior = 2,
	};
private:
	CHAR _pad1[0x22];
	UINT16 _cell_flags;
	CHAR _pad2[0x6];
	LPVOID _extradata;
	TESMaxHeightData* _maxheightdata;
	TESForm_CK* _landspace;
	CHAR _pad3[0x8];
	BSTArray<TESForm_CK*> *_navmeshes;
	BSTArray<TESFormRef_CK*> _items;
public:
	inline const TESFormRef_CK** GetItemsConst(VOID) const { return (const TESFormRef_CK**)_items.QBuffer(); }
	inline TESFormRef_CK** GetItems(VOID) { return (TESFormRef_CK**)_items.QBuffer(); }
	inline DWORD GetItemsCount(VOID) const { return (DWORD)_items.QSize(); }
	inline const TESForm_CK* GetLandspaceConst(VOID) const { return _landspace; }
	inline TESForm_CK* GetLandspace(VOID) { return _landspace; }
	inline BOOL IsNavMesh(VOID) const { return _navmeshes != NULL; }
	inline DWORD GetNavMeshesCount(VOID) const { return (DWORD)_navmeshes->QSize(); }
	inline const TESForm_CK** GetNavMeshesConst(VOID) const { return (const TESForm_CK**)_navmeshes->QBuffer(); }
	inline TESForm_CK** GetNavMeshes(VOID) { return (TESForm_CK**)_navmeshes->QBuffer(); }
	inline BOOL IsMaxHeightData(VOID) const { return _maxheightdata != NULL; }
	inline const TESMaxHeightData* GetMaxHeightDataConst(VOID) const { return _maxheightdata; }
	inline TESMaxHeightData* GetMaxHeightData(VOID) { return _maxheightdata; }
	inline BOOL IsInterior(VOID) const { return _cell_flags == cfInterior; }
	inline BOOL IsExterior(VOID) const { return _cell_flags == cfExterior; }
public:
	READ_PROPERTY(GetNavMeshes) TESForm_CK** NavMeshes;
	READ_PROPERTY(GetNavMeshesConst) const TESForm_CK** NavMeshesConst;
	READ_PROPERTY(GetNavMeshesCount) DWORD NavMeshesCount;
	READ_PROPERTY(GetItems) TESFormRef_CK** Items;
	READ_PROPERTY(GetItemsConst) const TESFormRef_CK** ItemsConst;
	READ_PROPERTY(GetItemsCount) DWORD ItemsCount;
	READ_PROPERTY(GetLandspaceConst) const TESForm_CK* LandspaceConst;
	READ_PROPERTY(GetLandspace) TESForm_CK* Landspace;
	READ_PROPERTY(GetMaxHeightDataConst) const TESMaxHeightData* MaxHeightDataConst;
	READ_PROPERTY(GetMaxHeightData) TESMaxHeightData* MaxHeightData;
public:
	virtual ~TESCell_CK(VOID);
};