#pragma once

#include <Windows.h>

#include "common.h"

DWORD WINAPI DumpWriterThread(LPVOID Arg);
LONG WINAPI DumpExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo);