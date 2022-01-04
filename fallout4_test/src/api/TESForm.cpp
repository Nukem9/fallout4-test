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

#include "../StdAfx.h"
#include "TESForm.h"

using namespace api;

#if 0
std::unordered_set<TESForm*> AlteredFormListShadow;
#else
tbb::concurrent_unordered_set<TESForm*> AlteredFormListShadow;
#endif

VOID TESForm::LoadForm(TESFile* file) {
	thisVirtualCall<VOID>(0xD0, this, file);
}

VOID TESForm::SaveForm(TESFile* file) {
	thisVirtualCall<VOID>(0xD8, this, file);
}

TESForm::FormType TESForm::GetType(VOID) const {
	return thisVirtualCall<TESForm::FormType>(0x120, this);
}

VOID TESForm::DebugInfo(LPSTR buffer, DWORD dwSize) const {
	thisVirtualCall<VOID>(0x128, this, buffer, dwSize);
}

VOID TESForm::MarkAsDeleted(BOOL state) {
	thisVirtualCall<VOID>(0x198, this, state);
}

VOID TESForm::MarkAsChanged(BOOL state) {
	thisVirtualCall<VOID>(0x1A0, this, state);
}

DWORD TESForm::GetEditIDLength(VOID) const {
	return thisVirtualCall<DWORD>(0x230, this);
}

LPCSTR TESForm::GetEditID(VOID) const {
	return thisVirtualCall<LPCSTR>(0x238, this);
}

BSString TESForm::GetID(VOID) const {
	if (!GetEditIDLength())
		return "";
	else
		return GetEditID();
}

TESForm* api::GetFormByNumericID(const DWORD SearchID) {
	return fastCall<TESForm*>(0x853220, SearchID);
}

LPVOID TESForm::AlteredFormList_Create(TESFormArray* Array, uint32_t Unknown) {
	AlteredFormListShadow.clear();

	return fastCall<LPVOID, TESFormArray*, uint32_t>(0x62EB80, Array, Unknown);
}

VOID TESForm::AlteredFormList_RemoveAllEntries(TESFormArray* Array) {
	AlteredFormListShadow.clear();

	fastCall<VOID, TESFormArray*>(0x2007E70, Array);
}

VOID TESForm::AlteredFormList_Insert(TESFormArray* Array, TESForm*& Entry, uint64_t Unknow1, uint32_t Unknow2) {
	AlteredFormListShadow.insert(Entry);

	fastCall<VOID, TESFormArray*, TESForm*&>(0x20078B0, Array, Entry, Unknow1, Unknow2);
}

VOID TESForm::AlteredFormList_RemoveEntry(TESFormArray* Array, uint32_t Index, uint32_t Unknown) {
#if 0
	AlteredFormListShadow.erase(Array->at(Index));
#else
	AlteredFormListShadow.unsafe_erase(Array->at(Index));
#endif

	fastCall<VOID, TESFormArray*, uint32_t, uint32_t>(0x7FC280, Array, Index, Unknown);
}

BOOL TESForm::AlteredFormList_ElementExists(TESFormArray* Array, TESForm*& Entry) {
	return AlteredFormListShadow.count(Entry) > 0;
}