//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
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

#include <stdint.h>

class BSTArrayHeapAllocator
{
	friend class __BSTArrayCheckOffsets;

private:
	void *m_Buffer;
	uint32_t m_AllocSize;

public:
	BSTArrayHeapAllocator() : m_Buffer(nullptr), m_AllocSize(0) {}

	void *QBuffer() const {
		return m_Buffer;
	}

	uint32_t QAllocSize() const {
		return m_AllocSize;
	}
};

class BSTArrayBase
{
	friend class __BSTArrayCheckOffsets;

private:
	uint32_t m_Size;

public:
	BSTArrayBase() : m_Size(0) {}

	uint32_t QSize() const {
		return m_Size;
	}

	bool QEmpty() const {
		return m_Size == 0;
	}
};

template <class _Ty, class _Alloc = BSTArrayHeapAllocator>
class BSTArray : public _Alloc, public BSTArrayBase
{
	friend class __BSTArrayCheckOffsets;

public:
	using value_type = _Ty;
	using allocator_type = _Alloc;
	using reference = _Ty&;
	using const_reference = const _Ty&;
	using size_type = uint32_t;

	BSTArray(void) {
	}

	reference operator[](const size_type Pos) {
		return (this->_Myfirst()[Pos]);
	}

	const_reference operator[](const size_type Pos) const {
		return (this->_const_Myfirst()[Pos]);
	}

	reference at(const size_type Pos) {
		AssertMsg(Pos >= 0 && Pos < QSize(), "Exceeded array bounds");

		return (this->_Myfirst()[Pos]);
	}

	const_reference at(const size_type Pos) const {
		AssertMsg(Pos >= 0 && Pos < QSize(), "Exceeded array bounds");

		return (this->_Myfirst()[Pos]);
	}
	
	reference front(void) {
		return (*this->_Myfirst());
	}

	const_reference const_front(void) const {
		return (*this->_const_Myfirst());
	}

	reference back(void) {
		return (this->_Mylast()[-1]);
	}

	const_reference const_back(void) const {
		return (this->_const_Mylast()[-1]);
	}

private:
	_Ty *_Myfirst(void) {
		return (_Ty *)QBuffer();
	}

	_Ty *_Mylast(void) {
		return ((_Ty *)QBuffer()) + QSize();
	}

	const _Ty* _const_Myfirst(void) const {
		return (_Ty*)QBuffer();
	}

	const _Ty* _const_Mylast(void) const {
		return ((_Ty*)QBuffer()) + QSize();
	}
};

class __BSTArrayCheckOffsets
{
	static_assert_offset(BSTArrayHeapAllocator, m_Buffer, 0x0);
	static_assert_offset(BSTArrayHeapAllocator, m_AllocSize, 0x8);

	static_assert_offset(BSTArrayBase, m_Size, 0x0);

	static_assert_offset(BSTArray<int>, m_Buffer, 0x0);
	static_assert_offset(BSTArray<int>, m_AllocSize, 0x8);
	static_assert_offset(BSTArray<int>, m_Size, 0x10);
};