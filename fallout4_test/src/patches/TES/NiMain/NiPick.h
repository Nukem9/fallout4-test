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

#include "NiPoint.h"

class NiPick
{
public:
	enum PickType
	{
		FIND_ALL = 0,
		FIND_FIRST = 1,
	};

	enum SortType
	{
		SORT = 0,
		NO_SORT = 1,
	};

	enum IntersectType
	{
		BOUND_INTERSECT = 0,
		TRIANGLE_INTERSECT = 1,
	};

	enum CoordinateType
	{
		MODEL_COORDINATES = 0,
		WORLD_COORDINATES = 1,
	};

	PickType m_ePickType;
	SortType m_eSortType;
	char _pad0[0x4];
	CoordinateType m_eCoordinateType;
	bool m_bFrontOnly;
	char _pad1[0x38];
	bool m_bReturnNormal;

	PickType GetPickType() const
	{
		return m_ePickType;
	}

	SortType GetSortType() const
	{
		return m_eSortType;
	}

	CoordinateType GetCoordinateType() const
	{
		return m_eCoordinateType;
	}

	bool GetFrontOnly() const
	{
		return m_bFrontOnly;
	}

	bool GetReturnNormal() const
	{
		return m_bReturnNormal;
	}

	class Record
	{
	public:
		void *m_Object;// NiPointer<NiAVObject *>
		NiPoint3 m_Intersect;
		NiPoint3 m_Normal;
		float m_Distance;

		inline void SetIntersection(const NiPoint3& Intersect)
		{
			m_Intersect = Intersect;
		}

		inline void SetNormal(const NiPoint3& Normal)
		{
			m_Normal = Normal;
		}

		inline void SetDistance(float Distance)
		{
			m_Distance = Distance;
		}
	};
	static_assert(sizeof(Record) == 0x28);
	static_assert_offset(Record, m_Object, 0x0);
	static_assert_offset(Record, m_Intersect, 0x8);
	static_assert_offset(Record, m_Normal, 0x14);
	static_assert_offset(Record, m_Distance, 0x20);
};
static_assert_offset(NiPick, m_ePickType, 0x0);
static_assert_offset(NiPick, m_eSortType, 0x4);
static_assert_offset(NiPick, m_eCoordinateType, 0xC);
static_assert_offset(NiPick, m_bFrontOnly, 0x10);
static_assert_offset(NiPick, m_bReturnNormal, 0x49);