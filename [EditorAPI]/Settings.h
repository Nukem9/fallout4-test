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

#pragma once
#pragma pack(push, 1)

#include "..\[Common]\StdAfx.h"
#include "BSString.h"
#include "BSTList.h"

// class Setting`: VTable [0x000000000380EA80, 0x0000000000000000 offset, 2 functions]
class Setting {
public:
	virtual ~Setting(VOID);
public:
	// See https://github.com/shadeMe/Construction-Set-Extender/blob/master/EditorAPI/Setting.h
	enum SettingTypes
	{
		kSetting_Bool			= 0x0,	// 'b' size 1
		kSetting_Char			= 0x1,	// 'c' size 1
		kSetting_UnsignedChar	= 0x2,	// 'h' size 1
		kSetting_SignedInt		= 0x3,	// 'i' size 4
		kSetting_UnsignedInt	= 0x4,	// 'u' size 4
		kSetting_Float			= 0x5,	// 'f' size 4
		kSetting_String			= 0x6,	// 'S'/'s' size indet. See note below^^
		kSetting_RGB			= 0x7,	// 'r' size 4. alpha byte set to 255 (?)
		kSetting_RGBA			= 0x8,	// 'a' size 4
		kSetting__MAX			= 0x9
	};
	// union for various value types
	union SettingValue
	{
		BOOL	b;
		CHAR	c;
		UCHAR	h;
		INT32	i;
		DWORD	u;
		FLOAT	f;
		LPSTR	s;
		struct color {
			// alpha is least sig. byte, *opposite* standard windows order
			UCHAR	alpha;
			UCHAR	blue;
			UCHAR	green;
			UCHAR	red;
		} rgba;
	};
private:
	/* 000 vtable */
	/* 008 */ SettingValue _Value;
	/* 010 */ LPSTR _Name;
public:
	static Setting* FIXAPI CreateInstance(LPCSTR name, const SettingValue& value);
	VOID DeleteInstance(VOID);

	VOID UpdateLinks(VOID);
public:
	UCHAR GetValueType(VOID) const;
	
	VOID SetBool(BOOL value);
	VOID SetChar(CHAR value);
	VOID SetUnsignedChar(UCHAR value);
	VOID SetSignedInt(INT32 value);
	VOID SetUnsignedInt(DWORD value);
	VOID SetFloat(FLOAT value);
	VOID SetString(LPCSTR value);
	VOID SetString(const BSString& value);
	VOID SetString(const std::string& value);
	VOID SetRGB(UCHAR r, UCHAR g, UCHAR b);
	VOID SetRGBA(UCHAR r, UCHAR g, UCHAR b, UCHAR a);

	INLINE BOOL GetBool(VOID) const { return _Value.b; }
	INLINE CHAR GetChar(VOID) const { return _Value.c; }
	INLINE UCHAR GetUnsignedChar(VOID) const { return _Value.h; }
	INLINE INT32 GetSignedInt(VOID) const { return _Value.i; }
	INLINE DWORD GetUnsignedInt(VOID) const { return _Value.u; }
	INLINE FLOAT GetFloat(VOID) const { return _Value.f; }
	INLINE LPSTR GetString(VOID) const { return _Value.s; }
	INLINE SettingValue::color GetRGB(VOID) const { return _Value.rgba; }
	INLINE SettingValue::color GetRGBA(VOID) const { return _Value.rgba; }
public:
	INLINE LPCSTR GetName(VOID) const { return _Name; }
public:
	READ_PROPERTY(GetName) LPCSTR Name;
	READ_PROPERTY(GetValueType) UCHAR ValueType;
};
static_assert(sizeof(Setting) == 0x18);

// class SettingCollection<class Setting>
// VTable [0x000000000380EAA8, 0x0000000000000000 offset, 11 functions]
// Abstract class for settings
template <class _Ty>
class SettingCollection {
public:
	/* 000 */ virtual ~SettingCollection(VOID);
	/* 008 */ virtual VOID AddItem(_Ty* setting) = 0;
	/* 010 */ virtual VOID Unknown10(VOID) = 0;
	/* 018 */ virtual DWORD ItemCount(VOID) = 0;
	/* 020 */ virtual BOOL WriteItem(_Ty* setting, LPVOID VTable /* unknown why use */) = 0;
	/* 028 */ virtual BOOL ReadItem(_Ty* setting, LPVOID VTable /* unknown why use */) = 0;
	/* 030 */ virtual BOOL Open(VOID);
	/* 038 */ virtual BOOL Close(VOID);
	/* 040 */ virtual VOID Unknown40(VOID);
	/* 048 */ virtual VOID LockWriteItems(VOID) = 0;
	/* 050 */ virtual VOID LockReadItems(VOID) = 0;
protected:
	/* 008 */ CHAR filename[MAX_PATH];		// full path of ini file for ini settings, ignored for other collections
	/* 10C */ CHAR pad0[4];					
	/* 110 */ LPVOID fileObject;			// actual type depends on class, but must be nonzero when file is open
											// TESFile* for gmst, HKEY for RegSetting, this* pointer for INISetting
};
static_assert(sizeof(SettingCollection<Setting>) == 0x118);

// class SettingCollectionList<class Setting>
// VTable[0x000000000380EB28, 0x0000000000000000 offset, 11 functions]
// Another class is still not full-fledged, but contains a list of options
template <class _Ty>
class SettingCollectionList : public SettingCollection<_Ty> {
public:
	typedef BSSimpleList<_Ty> SettingListT;
protected:
	/* 118 */ SettingListT settingList;
public:
	Setting* LookupByName(LPCSTR Name) {
		for (SettingListT::Iterator Itr = settingList.Begin(); Itr.End() == FALSE && Itr.Get(); ++Itr) {
			Setting* Current = Itr.Get();
			Assert(Current);

			if (!_stricmp(Name, Current->Name))
				return Current;
		}

		return NULL;
	}
public:
	/* 008 */ virtual VOID AddItem(_Ty* setting);
	/* 010 */ virtual VOID Unknown10(VOID);
	/* 018 */ virtual DWORD ItemCount(VOID);
	/* 048 */ virtual VOID LockWriteItems(VOID);
	/* 050 */ virtual VOID LockReadItems(VOID);
public:
	Setting* AddItemCustomType(LPCSTR name, Setting::SettingValue value) {
		auto setting = LookupByName(name);
		if (setting)
			return NULL;

		setting = Setting::CreateInstance(name, value);
		if (!setting)
			return NULL;

		AddItem(setting);
		return setting;
	}

	Setting* AddItemBool(LPCSTR name, BOOL default_value) {
		Setting::SettingValue settingValue;
		settingValue.b = default_value;

		return AddItemCustomType(name, settingValue);
	}

	Setting* AddItemChar(LPCSTR name, CHAR default_value) {
		Setting::SettingValue settingValue;
		settingValue.c = default_value;

		return AddItemCustomType(name, settingValue);
	}

	Setting* AddItemUnsignedChar(LPCSTR name, UCHAR default_value) {
		Setting::SettingValue settingValue;
		settingValue.h = default_value;

		return AddItemCustomType(name, settingValue);
	}

	Setting* AddItemSignedInt(LPCSTR name, INT32 default_value) {
		Setting::SettingValue settingValue;
		settingValue.i = default_value;

		return AddItemCustomType(name, settingValue);
	}

	Setting* AddItemUnsignedInt(LPCSTR name, UINT32 default_value) {
		Setting::SettingValue settingValue;
		settingValue.u = default_value;

		return AddItemCustomType(name, settingValue);
	}

	Setting* AddItemFloat(LPCSTR name, float default_value) {
		Setting::SettingValue settingValue;
		settingValue.f = default_value;

		return AddItemCustomType(name, settingValue);
	}

	Setting* AddItemString(LPCSTR name, LPCSTR default_value) {
		Setting::SettingValue settingValue;
		settingValue.s = const_cast<LPSTR>(default_value);

		return AddItemCustomType(name, settingValue);
	}

	Setting* AddItemString(LPCSTR name, const std::string& default_value) {
		Setting::SettingValue settingValue;
		settingValue.s = default_value.data();

		return AddItemCustomType(name, settingValue);
	}

	Setting* AddItemString(LPCSTR name, const BSString& default_value) {
		Setting::SettingValue settingValue;
		settingValue.s = const_cast<LPSTR>(*default_value);

		return AddItemCustomType(name, settingValue);
	}

	Setting* AddItemRGB(LPCSTR name, UCHAR default_r_value, UCHAR default_g_value, UCHAR default_b_value) {
		Setting::SettingValue settingValue;
		settingValue.rgba = { default_r_value, default_g_value, default_b_value, 0xFF };

		return AddItemCustomType(name, settingValue);
	}

	Setting* AddItemRGBA(LPCSTR name, UCHAR default_r_value, UCHAR default_g_value, UCHAR default_b_value, UCHAR default_a_value) {
		Setting::SettingValue settingValue;
		settingValue.rgba = { default_r_value, default_g_value, default_b_value, default_a_value };

		return AddItemCustomType(name, settingValue);
	}
};
static_assert(sizeof(SettingCollectionList<Setting>) == 0x128);

// class INISettingCollection: 
// VTable[0x000000000380EBA8, 0x0000000000000000 offset, 11 functions]
// Reading the .INI files
class INISettingCollection : public SettingCollectionList<Setting> {
public:
	// fileName holds the ini file name
	// fileObject holds the this* pointer when file is "open", and is zero otherwise
	// file is not actually opened; all i/o uses Get/WritePrivateProfileString()

	// no additional members
public:
	static INISettingCollection* FIXAPI CreateInstance(LPCSTR fileName);
public:
	BOOL OpenFile(LPCSTR name);
public:
	/* 018 */ INLINE DWORD ItemCount(VOID) const { return thisVirtualCall<DWORD>(0x18, this); }
	/* 020 */ INLINE BOOL WriteItem(Setting* setting) const {
		return thisVirtualCall<BOOL, Setting*, UINT_PTR>(0x20, this, setting, OFFSET(0x380EBA8, 0) /* unknown why use */);
	}
	/* 028 */ INLINE BOOL ReadItem(Setting* setting) const {
		return thisVirtualCall<BOOL, Setting*, UINT_PTR>(0x28, this, setting, OFFSET(0x380EBA8, 0) /* unknown why use */);
	}
	/* 030 */ INLINE BOOL Open(VOID) const { return thisVirtualCall<BOOL>(0x30, this); }
	/* 038 */ INLINE BOOL Close(VOID) const { return thisVirtualCall<BOOL>(0x38, this); }
};
static_assert(sizeof(INISettingCollection) == 0x128);

// Own F4CKFixes
class CreationKitSettingCollections {
public:
	static INISettingCollection* FIXAPI GetInstance(VOID);
	static INISettingCollection* FIXAPI GetInstancePrefs(VOID);
};

#pragma pack(pop)