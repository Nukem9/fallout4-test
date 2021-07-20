#pragma once

/*

This file is part of Fallout 4 Fixes source code.

*/

#include "..\common.h"
#include "TES/BSTArray.h"

/*
==================
sub_1405B31C0

Optimized search for faster loading of large plugins
==================
*/
uint32_t FIXAPI Fix_BoostArraySearchItem(BSTArray<LPVOID>& Array, LPCVOID& Target);
uint32_t FIXAPI Fix_BoostArraySearchItemWithOffset(BSTArray<LPVOID>& Array, LPCVOID& Target, uint32_t start_index);


/*
==================
sub_1405B31C0_SSE41

Optimized search for faster loading of large plugins using SIMD instructions need SSE 4.1
==================
*/
uint32_t FIXAPI Fix_NuukemBoostArraySearchItem(BSTArray<LPVOID>& Array, LPCVOID& Target);
uint32_t FIXAPI Fix_NuukemBoostArraySearchItemWithOffset(BSTArray<LPVOID>& Array, LPCVOID& Target, uint32_t start_index);

/*
==================
sub_1405B31C0_SSE41_Ex

Optimized search to speed up the loading of large plugins using SIMD instructions, SSE 4.1 is needed.
Selection of 16 pointers at a time.
==================
*/
uint32_t FIXAPI Fix_SSE41_BoostArraySearchItem_16Pointer(BSTArray<LPVOID>& Array, LPCVOID& Target);
uint32_t FIXAPI Fix_SSE41_BoostArraySearchItemWithOffset_16Pointer(BSTArray<LPVOID>& Array, LPCVOID& Target, uint32_t start_index);