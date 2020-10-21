#pragma once

#include "../TES/BSTArray.h"

class TESForm_CK
{
private:
	char _pad[0x8];
	uint32_t FormFlags;
	uint32_t FormID;
public:
	inline bool GetActive(void) const { return (FormFlags & 2) != 0; }
	inline uint32_t GetFormID() const { return FormID; }
public:
	__declspec(property(get = GetActive)) bool Active;
	__declspec(property(get = GetFormID)) uint32_t FormID;
public:
	using Array = BSTArray<TESForm_CK *>;

	virtual ~TESForm_CK();
	static TESForm_CK *GetFormByNumericID(uint32_t SearchID);
};