#include "../../common.h"
#include "TESForm_CK.h"

TESForm_CK *TESForm_CK::GetFormByNumericID(uint32_t SearchID)
{
	return ((TESForm_CK *(__fastcall *)(uint32_t))OFFSET(0x853220, 0))(SearchID);
}