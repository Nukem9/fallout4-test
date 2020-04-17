#pragma once

#include "../TES/BSTArray.h"

class TESForm_CK
{
public:
	using Array = BSTArray<TESForm_CK *>;

	virtual ~TESForm_CK();
	static TESForm_CK *GetFormByNumericID(uint32_t SearchID);
};