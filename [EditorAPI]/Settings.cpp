//////////////////////////////////////////
/*
* Copyright (c) 2022 Perchik71 <email:perchik71@outlook.com>
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

#include "Settings.h"
#include "NiMemoryManager.h"

// Setting

UCHAR Setting::GetValueType(VOID) const {
	return fastCall<UCHAR, LPCSTR>(0x26036C0, _Name);
}

Setting* FIXAPI Setting::CreateInstance(LPCSTR name, const SettingValue& value) {
	auto len = strlen(name);
	if (!len)
		return NULL;

	Setting* setting = (Setting*)NiMemoryManager::Alloc(NULL, sizeof(Setting));
	if (!setting)
		return NULL;

	// set vtable
	(*(UINT_PTR*)setting) = OFFSET(0x380EA80, 0);

	setting->_Name = (LPSTR)NiMemoryManager::Alloc(NULL, len + 1);
	strcpy(setting->_Name, name);

	if (setting->ValueType == kSetting_String) {
		len = strlen(value.s);
		if (!len) {
			setting->_Value.s = (LPSTR)NiMemoryManager::Alloc(NULL, 1);
			*setting->_Value.s = 0;
		}
		else {
			setting->_Value.s = (LPSTR)NiMemoryManager::Alloc(NULL, len + 1);
			strcpy(setting->_Value.s, value.s);
		}
	}
	else
		setting->_Value = value;

	return setting;
}

VOID Setting::DeleteInstance(VOID) {
	if (ValueType == kSetting_String)
		NiMemoryManager::Free(NULL, (LPVOID)_Value.s);
	
	NiMemoryManager::Free(NULL, (LPVOID)_Name);
	NiMemoryManager::Free(NULL, (LPVOID)this);
}

VOID Setting::UpdateLinks(VOID) {
	thisCall<VOID>(0x26031D0, this);
}

VOID Setting::SetBool(BOOL value) {
	_Value.b = value;
}

VOID Setting::SetChar(CHAR value) {
	_Value.c = value;
}

VOID Setting::SetUnsignedChar(UCHAR value) {
	_Value.h = value;
}

VOID Setting::SetSignedInt(INT32 value) {
	_Value.i = value;
}

VOID Setting::SetUnsignedInt(DWORD value) {
	_Value.u = value;
}

VOID Setting::SetFloat(FLOAT value) {
	_Value.f = value;
}

VOID Setting::SetString(LPCSTR value) {
	if (ValueType == kSetting_String && _Value.s)
		NiMemoryManager::Free(NULL, (LPVOID)_Value.s);

	auto len = strlen(value);
	if (!len) {
		_Value.s = (LPSTR)NiMemoryManager::Alloc(NULL, 1);
		*_Value.s = 0;
	}
	else {
		_Value.s = (LPSTR)NiMemoryManager::Alloc(NULL, len + 1);
		strcpy(_Value.s, value);
	}
}

VOID Setting::SetString(const BSString& value) {
	SetString(*value);
}

VOID Setting::SetString(const std::string& value) {
	SetString(value.c_str());
}

VOID Setting::SetRGB(UCHAR r, UCHAR g, UCHAR b) {
	_Value.rgba.red = r;
	_Value.rgba.red = g;
	_Value.rgba.red = b;
	_Value.rgba.red = 0xFF;
}

VOID Setting::SetRGBA(UCHAR r, UCHAR g, UCHAR b, UCHAR a) {
	_Value.rgba.red = r;
	_Value.rgba.red = g;
	_Value.rgba.red = b;
	_Value.rgba.red = a;
}

// INISettingCollection

INISettingCollection* FIXAPI INISettingCollection::CreateInstance(LPCSTR fileName) {
	auto collection = (INISettingCollection*)fastCall<INISettingCollection*>(0xAC610);

	if (collection) {
		collection->OpenFile(fileName);

		if (!collection->fileObject) {
			// impossible

			collection->Close();
			collection->~INISettingCollection();

			return NULL;
		}

		return collection;
	}

	return NULL;
}

BOOL INISettingCollection::OpenFile(LPCSTR name) {
	strcpy_s(filename, MAX_PATH, name);
	return Open();
}

// CreationKitSettingCollections

INISettingCollection* FIXAPI CreationKitSettingCollections::GetInstance(VOID) {
	return *(INISettingCollection**)OFFSET(0x9D38A28, 0);
}

INISettingCollection* FIXAPI CreationKitSettingCollections::GetInstancePrefs(VOID) {
	return *(INISettingCollection**)OFFSET(0x6EC1BC8, 0);
}