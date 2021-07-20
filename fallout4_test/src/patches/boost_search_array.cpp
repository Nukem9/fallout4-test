#include "boost_search_array.h"

#include <smmintrin.h>

/*

This file is part of Fallout 4 Fixes source code.

*/

/*
==================
sub_1405B31C0

Optimized search for faster loading of large plugins
==================
*/
uint32_t FIXAPI Fix_BoostArraySearchItem(BSTArray<LPVOID>& Array, LPCVOID& Target)
{
	return Fix_BoostArraySearchItemWithOffset(Array, Target, 0);
}


uint32_t FIXAPI Fix_BoostArraySearchItemWithOffset(BSTArray<LPVOID>& Array, LPCVOID& Target, uint32_t start_index)
{
	for (uint32_t i = start_index; i < Array.QSize(); i++)
	{
		if (Array[i] == Target)
			return i;
	}

	return 0xFFFFFFFF;
}


/*
==================
sub_1405B31C0_SSE41

Optimized search for faster loading of large plugins using SIMD instructions need SSE 4.1
==================
*/

uint32_t FIXAPI Fix_NuukemBoostArraySearchItem(BSTArray<LPVOID> & Array, LPCVOID & Target)
{
	return Fix_NuukemBoostArraySearchItemWithOffset(Array, Target, 0);
}


uint32_t FIXAPI Fix_NuukemBoostArraySearchItemWithOffset(BSTArray<LPVOID>& Array, LPCVOID& Target, uint32_t start_index)
{
	uint32_t index = start_index;
	PINT64 data = (PINT64)Array.QBuffer();

	const uint32_t comparesPerIter = 4;
	const uint32_t vectorizedIterations = (Array.QSize() - index) / comparesPerIter;

	//
	// Compare 4 pointers per iteration - use SIMD instructions to generate a bit mask. Set
	// bit 0 if 'array[i + 0]'=='target', set bit 1 if 'array[i + 1]'=='target', set bit X...
	//
	// AVX: mask = _mm256_movemask_pd(_mm256_castsi256_pd(_mm256_cmpeq_epi64(targets, _mm256_loadu_si256((__m256i *)&data[i]))));
	//
	const __m128i targets = _mm_set1_epi64x((INT64)Target);

	for (uint32_t iter = 0; iter < vectorizedIterations; iter++)
	{
		__m128i test1 = _mm_cmpeq_epi64(targets, _mm_loadu_si128((__m128i*) & data[index + 0]));
		__m128i test2 = _mm_cmpeq_epi64(targets, _mm_loadu_si128((__m128i*) & data[index + 2]));

		INT32 mask = _mm_movemask_pd(_mm_castsi128_pd(_mm_or_si128(test1, test2)));

		// if (target pointer found) { break into the remainder loop to get the index }
		if (mask != 0)
			break;

		index += comparesPerIter;
	}

	// Scan the rest 1-by-1
	for (; index < Array.QSize(); index++)
	{
		if (data[index] == (INT64)Target)
			return index;
	}

	return 0xFFFFFFFF;
}


/*
==================
sub_1405B31C0_SSE41_Ex

Optimized search to speed up the loading of large plugins using SIMD instructions, SSE 4.1 is needed.
Selection of 16 pointers at a time.
==================
*/

#define _mm_cmp1_epi128(targets, iptr) (_mm_movemask_pd(_mm_castsi128_pd(_mm_or_si128(_mm_cmpeq_epi64(targets, _mm_loadu_si128((__m128i*)iptr)), _mm_cmpeq_epi64(targets, _mm_loadu_si128((__m128i*)(iptr + 2)))))))

uint32_t FIXAPI Fix_SSE41_BoostArraySearchItem_16Pointer(BSTArray<LPVOID> & Array, LPCVOID & Target)
{
	return Fix_SSE41_BoostArraySearchItemWithOffset_16Pointer(Array, Target, 0);
}

uint32_t FIXAPI Fix_SSE41_BoostArraySearchItemWithOffset_16Pointer(BSTArray<LPVOID> & Array, LPCVOID & Target, uint32_t start_index)
{
	uint32_t index = start_index;
	PINT64 data = (PINT64)Array.QBuffer() + index;

	if ((Array.QSize() > 0) && (index < Array.QSize()))
	{
		//
		// Compare 16 pointers per iteration - use SIMD instructions to generate a bit mask. Set
		// bit 0 if 'array[i + 0]'=='target', set bit 1 if 'array[i + 1]'=='target', set bit X...
		const uint32_t comparesPerIter = 16;
		const uint32_t vectorizedIterations = (Array.QSize() - index) / comparesPerIter;
		const __m128i target = _mm_set1_epi64x((INT64)Target);

		for (uint32_t iter = 0; iter < vectorizedIterations; iter++)
		{
			if (_mm_cmp1_epi128(target, &data[index]) || _mm_cmp1_epi128(target, &data[index + 4]) ||
				_mm_cmp1_epi128(target, &data[index + 8]) || _mm_cmp1_epi128(target, &data[index + 12]))
				break;

			index += comparesPerIter;
		}

		// Scan the rest 1-by-1
		for (; index < Array.QSize(); index++)
		{
			if (data[index] == (INT64)Target)
				return index;
		}
	}

	return 0xFFFFFFFF;
}