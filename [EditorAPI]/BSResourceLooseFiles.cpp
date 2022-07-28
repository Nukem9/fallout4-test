//////////////////////////////////////////
/*
* Copyright (c) 2022 Perchik71 <email:perchik71@outlook.com>
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

#include "BSString.h"
#include "BSResourceLooseFiles.h"

#include "..\[Common]\StdAfx.h"

///////////////////////////////////////////////////////////////////////
///	class BSResource::LooseFileStream
///////////////////////////////////////////////////////////////////////

VOID FIXAPI BSResource::LooseFileStream::CreateInstance(LPCSTR FileName, LooseFileStream** Instance)
{
	if (!FileName || !(*Instance))
		return;

	//_MESSAGE_FMT("DEBUG: Load loose file: %s (%X)", FileName, *Instance);

	auto stream = *Instance;
	stream->Initialize64BitData(FileName);
}

VOID BSResource::LooseFileStream::Initialize64BitData(LPCSTR FileName)
{
	dw64FileSize = 0;
	dw64Position = 0;

	auto path = BSString::Utils::GetApplicationPath() + FileName;
	if (BSString::Utils::FileExists(path))
		dw64FileSize = std::filesystem::file_size(*path);
	else
		dw64FileSize = dwFileSize;
}

///////////////////////////////////////////////////////////////////////