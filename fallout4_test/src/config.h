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
#pragma once
//////////////////////////////////////////
// need definitions for VC++
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
//////////////////////////////////////////
// optional
#define FALLOUT4_CREATIONKIT_ONLY	0	// Only build code related to the Creation Kit hooks
#define FALLOUT4_LAZ_UNICODE_PLUGIN 1	// Support unicode string (utf-8) convert to ansi and back
#define FALLOUT4_STUDY_CK64_INIFILE 0	// Output of variables from the ini file and their values to the log
#define FALLOUT4_CK64_BSTARRAY_DBG  0   // Debugging SIMD instructions to speed up the search
#define FALLOUT4_CK64_ENB_FIXABLE   1   // Enabling the loader implementation in .dll and installation f4ck_loader.exe
#define FALLOUT4_DEVELOPER_MODE		0   // Enabling developer mode
//////////////////////////////////////////
// required value set 1
#define FALLOUT4_MINI_PROJECT		1	// uses https://github.com/pulzed/mINI