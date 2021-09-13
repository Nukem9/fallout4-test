//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
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

#include "boost_search_array.h"

#include <smmintrin.h>
#include <functional>

#define _mm_cmp1_epi128(targets, iptr) (_mm_movemask_pd(_mm_castsi128_pd(_mm_or_si128(_mm_cmpeq_epi64(targets, _mm_loadu_si128((__m128i*)iptr)), _mm_cmpeq_epi64(targets, _mm_loadu_si128((__m128i*)(iptr + 2)))))))

#if FALLOUT4_CK64_BSTARRAY_DBG
#include "CKF4/LogWindow.h"
#include <chrono>
#define F64_NOW std::chrono::steady_clock::now()
#endif

#pragma warning(disable:4533)

namespace Experimental {
	uint32_t FIXAPI BSTArraySearchItem(BSTArray<LPVOID>& Array, LPCVOID& Target) {
		return BSTArraySearchItemWithOffset(Array, Target, 0);
	}

	uint32_t FIXAPI BSTArraySearchItemWithOffset(BSTArray<LPVOID>& Array, LPCVOID& Target, uint32_t start_index)
	{
#if FALLOUT4_CK64_BSTARRAY_DBG
		auto start = F64_NOW;
#endif
		uint32_t i, res = 0xFFFFFFFF, count = Array.QSize();
		for (i = start_index; i < count; i++)
		{
			if (Array[i] == Target) {
				res = i;
				break;
			}
		}
#if FALLOUT4_CK64_BSTARRAY_DBG
		auto end = F64_NOW;
		std::chrono::duration<double> elapsed_seconds = end - start;
		LogWindow::Log("items %i elapsed time: %f", count, elapsed_seconds.count());
#endif
		return res;
	}

	uint32_t FIXAPI BSTArraySIMDSearchItem(BSTArray<LPVOID>& Array, LPCVOID& Target) {
		return BSTArraySIMDSearchItemWithOffset(Array, Target, 0);
	}

	uint32_t FIXAPI BSTArraySIMDSearchItemWithOffset(BSTArray<LPVOID>& Array, LPCVOID& Target, uint32_t start_index) {
		uint32_t index = start_index, res = 0xFFFFFFFF;
		
		if (Array.QSize() <= start_index)
			goto EndSearchSIMD;

		if ((Array.QSize() - start_index) < 50)
			return BSTArraySearchItemWithOffset(Array, Target, start_index);

#if FALLOUT4_CK64_BSTARRAY_DBG
		auto start = F64_NOW;
#endif
		
		PINT64 data = (PINT64)Array.QBuffer();

		const uint32_t comparesPerIter = 4;
		const uint32_t vectorizedIterations = (Array.QSize() - index) / comparesPerIter;

		//
		// Compare 4 pointers per iteration - use SIMD instructions to generate a bit mask. Set
		// bit 0 if 'array[i + 0]'=='target', set bit 1 if 'array[i + 1]'=='target', set bit X...
		const __m128i targets = _mm_set1_epi64x((INT64)Target);

		for (uint32_t iter = 0; iter < vectorizedIterations; iter++) {
			__m128i test1 = _mm_cmpeq_epi64(targets, _mm_loadu_si128((__m128i*)&data[index + 0]));
			__m128i test2 = _mm_cmpeq_epi64(targets, _mm_loadu_si128((__m128i*)&data[index + 2]));

			INT32 mask = _mm_movemask_pd(_mm_castsi128_pd(_mm_or_si128(test1, test2)));

			// if (target pointer found) { break into the remainder loop to get the index }
			if (mask != 0)
				break;

			index += comparesPerIter;
		}

		// Scan the rest 1-by-1
		for (; index < Array.QSize(); index++) {
			if (data[index] == (INT64)Target) {
				res = index;
				break;
			}
		}
#if FALLOUT4_CK64_BSTARRAY_DBG
		auto end = F64_NOW;
		std::chrono::duration<double> elapsed_seconds = end - start;
		LogWindow::Log("items %i elapsed time: %f", (Array.QSize() - start_index), elapsed_seconds.count());
#endif
		EndSearchSIMD:
		return res;
	}

	uint32_t FIXAPI BSTArraySIMD2SearchItem(BSTArray<LPVOID>& Array, LPCVOID& Target) {
		return BSTArraySIMD2SearchItemWithOffset(Array, Target, 0);
	}

	uint32_t FIXAPI BSTArraySIMD2SearchItemWithOffset(BSTArray<LPVOID>& Array, LPCVOID& Target, uint32_t start_index) {
		uint32_t index = start_index, res = 0xFFFFFFFF;
		
		if (Array.QSize() <= start_index)
			goto EndSearchSIMD2;

		if ((Array.QSize() - start_index) < 50)
			return BSTArraySearchItemWithOffset(Array, Target, start_index);

#if FALLOUT4_CK64_BSTARRAY_DBG
		auto start = F64_NOW;
#endif

		PINT64 data = (PINT64)Array.QBuffer() + index;

		if ((Array.QSize() > 0) && (index < Array.QSize())) {
			//
			// Compare 16 pointers per iteration - use SIMD instructions to generate a bit mask. Set
			// bit 0 if 'array[i + 0]'=='target', set bit 1 if 'array[i + 1]'=='target', set bit X...
			const uint32_t comparesPerIter = 16;
			const uint32_t vectorizedIterations = (Array.QSize() - index) / comparesPerIter;
			const __m128i target = _mm_set1_epi64x((INT64)Target);

			for (uint32_t iter = 0; iter < vectorizedIterations; iter++) {
				if (_mm_cmp1_epi128(target, &data[index]) || _mm_cmp1_epi128(target, &data[index + 4]) ||
					_mm_cmp1_epi128(target, &data[index + 8]) || _mm_cmp1_epi128(target, &data[index + 12]))
					break;

				index += comparesPerIter;
			}

			// Scan the rest 1-by-1
			for (; index < Array.QSize(); index++) {
				if (data[index] == (INT64)Target) {
					res = index;
					break;
				}
			}
		}

#if FALLOUT4_CK64_BSTARRAY_DBG
		auto end = F64_NOW;
		std::chrono::duration<double> elapsed_seconds = end - start;
		LogWindow::Log("items %i elapsed time: %f", (Array.QSize() - start_index), elapsed_seconds.count());
#endif
	EndSearchSIMD2:
		return res;
	}
}

#pragma warning(default:4533)