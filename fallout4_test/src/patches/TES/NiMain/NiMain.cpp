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

#include "../../../common.h"
#include "NiColor.h"
#include "NiPoint.h"

const NiColor NiColor::BLACK{ 0.0f, 0.0f, 0.0f };
const NiColor NiColor::WHITE{ 1.0f, 1.0f, 1.0f };

const NiColorA NiColorA::BLACK{ 0.0f, 0.0f, 0.0f, 1.0f };
const NiColorA NiColorA::WHITE{ 1.0f, 1.0f, 1.0f, 1.0f };
const NiColorA NiColorA::RED{ 1.0f, 0.0f, 0.0f, 1.0f };
const NiColorA NiColorA::GREEN{ 0.0f, 1.0f, 0.0f, 1.0f };
const NiColorA NiColorA::BLUE{ 0.0f, 0.0f, 1.0f, 1.0f };

const NiPoint3 NiPoint3::ZERO{ 0.0f, 0.0f, 0.0f };