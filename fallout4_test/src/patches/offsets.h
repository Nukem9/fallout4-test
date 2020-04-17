#pragma once

#include <array>

#define OFFSET(RelOffset, Version) (g_ModuleBase + RelOffset)
#define OFFSET_ENTRY_KEY(RelOffset, Version) (((uint64_t)(Version) << 32) | (RelOffset))
#define OFFSET_ENTRY(RelOffset, Version, Signature, SigAdjustment, TranslatedOffset) { RelOffset, Version, Signature, SigAdjustment, (uint32_t)(TranslatedOffset) },

namespace Offsets
{
	struct OffsetEntry
	{
		uint32_t RelOffset;
		uint32_t Version;
		const char *Signature;
		int SigAdjustment;
		uint32_t TranslatedOffset;
	};

	uintptr_t Resolve(uint32_t RelOffset, uint32_t Version);
	bool CanResolve(uint32_t RelOffset, uint32_t Version);

	void BuildTableForCKF4Version(uint32_t Version);
	void BuildTableForCKSSEVersion(uint32_t Version);
	void BuildTableForGameVersion(uint32_t Version);
	void BuildTable(const OffsetEntry *Table, size_t Count, bool CurrentVersion);
	void ValidateTable(const OffsetEntry *Table, size_t Count);
	void DumpLoadedTable(const char *FilePath);
}