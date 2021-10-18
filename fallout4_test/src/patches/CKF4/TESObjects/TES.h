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

#include "TESCellViewScene_CK.h"
#include "TESDataFileHandler_CK.h"
#include "TESFormRef_CK.h"
#include "TESLayer_CK.h"
#include "TESForm_CK.h"
#include "TESFile_CK.h"
#include "TESCell_CK.h"

/*
==================
TESUnknownClass01_CK

A poorly studied class, I suspect that there is a lot of useful information there
==================
*/
class TESUnknownClass01_CK {
private:
	CHAR _pad1[0xF60];
	TESCell_CK** m_BeginCells;
	CHAR _pad2[0x1];
	WORD m_CountCell;
public:
	// IMPORTANT: In other words, the total CELL can be no more than 65355 :O
	inline WORD GetCellCount(VOID) const { return m_CountCell; }
	// Returns the usual C array of all cells
	inline TESCell_CK** GetCells(VOID) const { return m_BeginCells; }
};


// A common platform for the studied global classes
namespace TES {
	inline TESDataFileHandler_CK* FileManager(VOID) { 
		return FileHandler; 
	}

	inline TESCellViewScene_CK* Scene(VOID) {
		return TESCellViewScene_CK::GetCellViewScene();
	}

	inline TESUnknownClass01_CK* UnknownClass01(VOID) { 
		return *(TESUnknownClass01_CK**)(OFFSET(0x6D37F88, 0));
	}

	inline TESBaseForm_CK* GetFormByNumericID(UINT32 SearchID) {
		return (TESBaseForm_CK*)TESForm_CK::GetFormByNumericID(SearchID);
	}
}