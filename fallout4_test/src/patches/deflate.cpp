#include <libdeflate/libdeflate.h>

#include "deflate.h"

/*

This file is part of Fallout 4 Fixes source code.

*/

#pragma comment(lib, "libdeflate.lib")

INT32 FIXAPI hk_inflateInit(z_stream_s* Stream, LPCSTR Version, INT32 Mode)
{
	// Force inflateEnd to error out and skip frees
	Stream->state = NULL;

	return 0;
}

INT32 FIXAPI hk_inflate(z_stream_s* Stream, INT32 Flush)
{
	size_t outBytes = 0;
	libdeflate_decompressor* decompressor = libdeflate_alloc_decompressor();

	libdeflate_result result = libdeflate_zlib_decompress(decompressor, Stream->next_in, Stream->avail_in, Stream->next_out, Stream->avail_out, &outBytes);
	libdeflate_free_decompressor(decompressor);

	if (result == LIBDEFLATE_SUCCESS)
	{
		Assert(outBytes < std::numeric_limits<uint32_t>::max());

		Stream->total_in = Stream->avail_in;
		Stream->total_out = (uint32_t)outBytes;

		return 1;
	}

	if (result == LIBDEFLATE_INSUFFICIENT_SPACE)
		return -5;

	return -2;
}