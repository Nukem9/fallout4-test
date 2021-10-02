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

#if FALLOUT4_CK64_BSTARRAY_DBG
#include "CKF4/LogWindow.h"
#include <chrono>
#define F64_NOW std::chrono::steady_clock::now()
#endif

#pragma warning(disable:4533)

#define QSIMD_INVALID_INDEX 0xFFFFFFFF

namespace Experimental {

	DWORD FIXAPI QSIMDFastSearchArrayItemOffsetDWORD(BSTArray<DWORD>& _array, DWORD& _target, DWORD _start_index) {
		DWORD index = _start_index;
		const DWORD size_array = _array.QSize();

		if (size_array <= _start_index)
			return QSIMD_INVALID_INDEX;

#if FALLOUT4_CK64_BSTARRAY_DBG
		auto start = F64_NOW;
#endif

		DWORD* data = (DWORD*)_array.QBuffer() + index;

		if ((size_array - _start_index) < 50) {
			for (; index < size_array; index++) {
				if (data[index] == _target) {
#if FALLOUT4_CK64_BSTARRAY_DBG
					auto end = F64_NOW;
					std::chrono::duration<double> elapsed_seconds = end - start;
					_MESSAGE_FMT("items %i elapsed time: %f", (size_array - _start_index), elapsed_seconds.count());
#endif
					return index;
				}
			}

#if FALLOUT4_CK64_BSTARRAY_DBG
			auto end = F64_NOW;
			std::chrono::duration<double> elapsed_seconds = end - start;
			_MESSAGE_FMT("items %i elapsed time: %f", (size_array - _start_index), elapsed_seconds.count());
#endif

			return QSIMD_INVALID_INDEX;
		}

		DWORD res = QSIMD_INVALID_INDEX;

		//
		// Compare 16 indexes per iteration - use SIMD instructions to generate a bit mask. Set
		// bit 0 if 'array[i + 0]'=='target', set bit 1 if 'array[i + 1]'=='target', set bit X...
		const DWORD comparesPerIter = 16;
		const DWORD vectorizedIterations = (size_array - index) / comparesPerIter;
		const __m128i target = _mm_set_epi32(_target, _target, _target, _target);
		UINT32 bsr_v;
		UINT32 mask;
		std::size_t iter = 0;
		__m128i* ebp = (__m128i*)(data + index);

		for (; iter < vectorizedIterations; ebp += 4, iter++) {
			if ((mask = (_mm_movemask_ps(_mm_castsi128_ps(_mm_cmpeq_epi32(target, _mm_loadu_si128(ebp))))) |
				((_mm_movemask_ps(_mm_castsi128_ps(_mm_cmpeq_epi32(target, _mm_loadu_si128(ebp + 1))))) << 4) |
				((_mm_movemask_ps(_mm_castsi128_ps(_mm_cmpeq_epi32(target, _mm_loadu_si128(ebp + 2))))) << 8) |
				((_mm_movemask_ps(_mm_castsi128_ps(_mm_cmpeq_epi32(target, _mm_loadu_si128(ebp + 3))))) << 12))) {
				_BitScanForward((unsigned long*)&bsr_v, mask);

				index += comparesPerIter * iter;
				//_MESSAGE_FMT("items m%i bsf%i (%i == %i)", mask, bsr_v, data[index + bsr_v], _target);

				return index + bsr_v;
			}
		}

		index += comparesPerIter * iter;

		// Scan the rest 1-by-1
		for (; index < size_array; index++) {
			if (data[index] == _target) {
				res = index;
				break;
			}
		}

#if FALLOUT4_CK64_BSTARRAY_DBG
		auto end = F64_NOW;
		std::chrono::duration<double> elapsed_seconds = end - start;
		_MESSAGE_FMT("items %i elapsed time: %f", (size_array - _start_index), elapsed_seconds.count());
#endif

		return res;
	}

	DWORD FIXAPI QSIMDFastSearchArrayItemOffsetQWORD(BSTArray<UINT64>& _array, UINT64& _target, DWORD _start_index) {
		DWORD index = _start_index;
		const DWORD size_array = _array.QSize();

		if (size_array <= _start_index)
			return QSIMD_INVALID_INDEX;

#if FALLOUT4_CK64_BSTARRAY_DBG
		auto start = F64_NOW;
#endif

		PUINT64 data = (PUINT64)_array.QBuffer() + index;

		if ((size_array - _start_index) < 50) {
			for (; index < size_array; index++) {
				if (data[index] == _target) {
#if FALLOUT4_CK64_BSTARRAY_DBG
					auto end = F64_NOW;
					std::chrono::duration<double> elapsed_seconds = end - start;
					_MESSAGE_FMT("items %i elapsed time: %f", (size_array - _start_index), elapsed_seconds.count());
#endif
					return index;
				}
			}

#if FALLOUT4_CK64_BSTARRAY_DBG
			auto end = F64_NOW;
			std::chrono::duration<double> elapsed_seconds = end - start;
			_MESSAGE_FMT("items %i elapsed time: %f", (size_array - _start_index), elapsed_seconds.count());
#endif

			return QSIMD_INVALID_INDEX;
		}

		DWORD res = QSIMD_INVALID_INDEX;

		//
		// Compare 16 pointers per iteration - use SIMD instructions to generate a bit mask. Set
		// bit 0 if 'array[i + 0]'=='target', set bit 1 if 'array[i + 1]'=='target', set bit X...
		const DWORD comparesPerIter = 16;
		const DWORD vectorizedIterations = (size_array - index) / comparesPerIter;
		const __m128i target = _mm_set_epi64x(_target, _target);
		UINT32 bsr_v;
		UINT64 mask;
		std::size_t iter = 0;
		__m128i* ebp = (__m128i*)(data + index);

		for (; iter < vectorizedIterations; ebp += 8, iter++) {
			if ((mask = ((UINT64)_mm_movemask_pd(_mm_castsi128_pd(_mm_cmpeq_epi64(target, _mm_loadu_si128(ebp))))) |
				(((UINT64)_mm_movemask_pd(_mm_castsi128_pd(_mm_cmpeq_epi64(target, _mm_loadu_si128(ebp + 1))))) << 2) |
				(((UINT64)_mm_movemask_pd(_mm_castsi128_pd(_mm_cmpeq_epi64(target, _mm_loadu_si128(ebp + 2))))) << 4) |
				(((UINT64)_mm_movemask_pd(_mm_castsi128_pd(_mm_cmpeq_epi64(target, _mm_loadu_si128(ebp + 3))))) << 6) |
				(((UINT64)_mm_movemask_pd(_mm_castsi128_pd(_mm_cmpeq_epi64(target, _mm_loadu_si128(ebp + 4))))) << 8) |
				(((UINT64)_mm_movemask_pd(_mm_castsi128_pd(_mm_cmpeq_epi64(target, _mm_loadu_si128(ebp + 5))))) << 10) |
				(((UINT64)_mm_movemask_pd(_mm_castsi128_pd(_mm_cmpeq_epi64(target, _mm_loadu_si128(ebp + 6))))) << 12) |
				(((UINT64)_mm_movemask_pd(_mm_castsi128_pd(_mm_cmpeq_epi64(target, _mm_loadu_si128(ebp + 7))))) << 14))) {
				_BitScanForward64((unsigned long*)&bsr_v, mask);

				index += comparesPerIter * iter;
			//	_MESSAGE_FMT("items m%i bsf%i (%i == %i)", mask, bsr_v, data[index + bsr_v], _target);

				return index + bsr_v;
			}
		}

		index += comparesPerIter * iter;

		// Scan the rest 1-by-1
		for (; index < size_array; index++) {
			if (data[index] == _target) {
				res = index;
				break;
			}
		}

#if FALLOUT4_CK64_BSTARRAY_DBG
		auto end = F64_NOW;
		std::chrono::duration<double> elapsed_seconds = end - start;
		_MESSAGE_FMT("items %i elapsed time: %f", (size_array - _start_index), elapsed_seconds.count());
#endif

		return res;
	}
}

#pragma warning(default:4533)