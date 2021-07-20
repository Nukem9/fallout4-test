#pragma once

/*

This file is part of Fallout 4 Fixes source code.

*/

#include "../common.h"

struct z_stream_s
{
	LPCVOID next_in;
	uint32_t avail_in;
	uint32_t total_in;
	LPVOID next_out;
	uint32_t avail_out;
	uint32_t total_out;
	LPCSTR msg;
	struct internal_state* state;
};
static_assert_offset(z_stream_s, state, 0x28);

INT32 FIXAPI hk_inflateInit(z_stream_s* Stream, LPCSTR Version, INT32 Mode);
INT32 FIXAPI hk_inflate(z_stream_s* Stream, INT32 Flush);