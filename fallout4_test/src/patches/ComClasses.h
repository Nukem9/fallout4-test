#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <atomic>
#include <string>
#include <vector>

#define PROPERTY(read_func, write_func) __declspec(property(get = read_func, put = write_func))
#define READ_PROPERTY(read_func) __declspec(property(get = read_func))
