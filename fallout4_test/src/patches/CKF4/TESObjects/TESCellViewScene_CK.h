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

#include "..\..\..\common.h"
#include "TESForm_CK.h"
#include "TESCell_CK.h"

/*
==================
TESCellViewScene_CK

As I understood the structure, but it can also be a class (without a description?). 
The object itself is permanently located at RAV+6D54CF8.
I was able to identify two structures in it, one containing information about the Cell, 
the other containing information about fog, light, whether the sky is on, etc.
==================
*/
class TESCellViewScene_CK
{
public:
	/*
	==================
	TESCellViewSceneRenderInfo_CK

	The object contains the current render state, as long as the sky is interested.
	==================
	*/
	class TESRenderInfo_CK
	{
	public:
		BOOL IsSky(VOID) const;
	};
private:
	CHAR _pad1[0x58];
	TESCell_CK* cell_int_info;
	CHAR _pad2[0x38];
	TESRenderInfo_CK* render_info;
public:
	BOOL IsEmpty(VOID) const;
	inline BOOL IsLocationCell(VOID) const { return cell_int_info == NULL; }
	inline BOOL IsInteriorsCell(VOID) const { return cell_int_info != NULL; }
	inline TESCell_CK* GetInterios(VOID) const { return cell_int_info; }
	inline TESRenderInfo_CK* GetRenderInfo(VOID) const { return render_info; }
public:
	static TESCellViewScene_CK* GetCellViewScene(VOID); 
public:
	~TESCellViewScene_CK(VOID);
public:
	READ_PROPERTY(GetInterios) TESCell_CK* Interios;
	READ_PROPERTY(GetRenderInfo) TESRenderInfo_CK* RenderInfo;
}; 