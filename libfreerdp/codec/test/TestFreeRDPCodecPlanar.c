
#include <freerdp/freerdp.h>
#include <freerdp/codec/color.h>
#include <freerdp/codec/bitmap.h>

/**
 * [MS-RDPEGDI] Test Bitmap 32x32 (16bpp)
 */

const BYTE TEST_RLE_UNCOMPRESSED_BITMAP_16BPP[2048] =
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84"
	"\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x99\xD6\xFF\xFF"
	"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
	"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x08\x42"
	"\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42"
	"\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\xFF\xFF"
	"\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42"
	"\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42"
	"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
	"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
	"\xFF\xFF\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84"
	"\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x08\x42"
	"\x08\x42\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84"
	"\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x99\xD6\xFF\xFF"
	"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
	"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x08\x42"
	"\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42"
	"\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\xFF\xFF"
	"\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42"
	"\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42"
	"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
	"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
	"\xFF\xFF\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84"
	"\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x08\x42"
	"\x08\x42\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x00\x00\x00\x00\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00"
	"\x99\xD6\x99\xD6\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x99\xD6"
	"\x99\xD6\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x00\x00"
	"\x00\x00\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00"
	"\x99\xD6\x99\xD6\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x99\xD6"
	"\x99\xD6\x00\x00\x00\x00\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x00\x00\x00\x00\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x00\x00\x00\x00\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\xFF\xFF"
	"\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6"
	"\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x10\x84\x08\x42"
	"\x08\x42\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84"
	"\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x10\x84\x99\xD6\xFF\xFF"
	"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
	"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x08\x42"
	"\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42"
	"\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\x08\x42\xFF\xFF";

/**
 * [MS-RDPEGDI] Test Bitmap 32x32 (RLE Encoded, not RDP6)
 */

const BYTE TEST_RLE_COMPRESSED_BITMAP[220] =
	"\x85\xFF\xFF\x99\xD6\x99\xD6\x99\xD6\x99\xD6\x06\x8B\x99\xD6\x99"
	"\xD6\x99\xD6\x10\x84\x08\x42\x08\x42\x10\x84\x99\xD6\x99\xD6\x99"
	"\xD6\x99\xD6\x06\x84\x99\xD6\x99\xD6\x99\xD6\xFF\xFF\x16\x69\x99"
	"\xD6\x06\x69\x99\xD6\x04\xCC\x89\x52\x03\x6E\xFF\xFF\x02\x6E\x08"
	"\x42\x01\x70\x08\x42\x71\xFF\xFF\xCE\x18\xC6\x01\x81\x08\x42\xCE"
	"\x66\x29\x02\xCD\x89\x52\x03\x88\x10\x84\x99\xD6\x99\xD6\x99\xD6"
	"\x00\x00\x00\x00\x00\x00\x00\x00\xD8\x99\xD6\x03\xF8\x01\x00\x00"
	"\x00\x00\xF0\x66\x99\xD6\x05\x6A\x99\xD6\x00\xC4\xCC\x89\x52\x03"
	"\x6E\xFF\xFF\x02\x6E\x08\x42\x01\x70\x08\x42\x71\xFF\xFF\xCE\x18"
	"\xC6\x01\x81\x08\x42\xCE\x66\x29\x02\xCD\x89\x52\x03\x00\x04\xD6"
	"\x99\xD6\xC3\x80\x61\x00\xA5\x80\x40\xEC\x52\x00\x5A\x00\x2D\x00"
	"\x24\x00\x12\x00\x24\x00\x12\x00\x5A\x00\x2D\x00\xA5\x80\x52\x00"
	"\xC3\x80\x61\x00\x00\x00\x00\x00\xCC\x89\x52\x03\x6E\xFF\xFF\x02"
	"\xCB\x18\xC6\x84\x08\x42\x08\x42\x08\x42\xFF\xFF";

const BYTE TEST_RLE_SCANLINE_UNCOMPRESSED[12] =
	"AAAABBCCCCCD";

/**
 * [MS-RDPEGDI] 3.1.9.2.1 Encoding Run-Length Sequences
 */

/* Scanline Absolute Values */

const BYTE TEST_RDP6_SCANLINES_ABSOLUTE[3][6] =
{
	{  255,  255,  255,  255,  254,  253 },
	{  254,  192,  132,   96,   75,   25 },
	{  253,  140,   62,   14,  135,  193 }
};

/* Scanline Delta Values */

const int TEST_RDP6_SCANLINES_DELTA[3][6] =
{
	{  255, 255,  255,  255,  254,  253 },
	{   -1, -63, -123, -159, -179, -228 },
	{   -1, -52,  -70,  -82,   60,  168 }
};

/* Scanline Delta Values (1-byte two's complement) */

const char TEST_RDP6_SCANLINES_DELTA_2C[3][6] =
{
	{   -1,  -1,   -1,   -1,   -2,   -3 },
	{   -1, -63, -123,   97,   77,   28 },
	{   -1, -52,  -70,  -82,   60,  -88 }
};

/* Scanline Delta Values (1-byte two's complement, encoded) */

const char TEST_RDP6_SCANLINES_DELTA_2C_ENCODED[3][6] =
{
	{   -1,  -1,   -1,   -1,   -2,   -3 },
	{   -1, 125,   11,  -62, -102,   56 },
	{    1, 103, -117,  -93,  120,  -81 }
};

/* Scanline Delta Values (1-byte two's complement, encoded, unsigned) */

const BYTE TEST_RDP6_SCANLINES_DELTA_2C_ENCODED_UNSIGNED[3][6] =
{
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFD },
	{ 0x01, 0x7D, 0xF5, 0xC2, 0x9A, 0x38 },
	{ 0x01, 0x67, 0x8B, 0xA3, 0x78, 0xAF }
};

#include "../planar.h"

int TestFreeRDPCodecPlanar(int argc, char* argv[])
{
	int dstSize;
	UINT32 format;
	HCLRCONV clrconv;
	BYTE* srcBitmap32;
	BYTE* srcBitmap16;

	clrconv = freerdp_clrconv_new(0);
	srcBitmap16 = (BYTE*) TEST_RLE_UNCOMPRESSED_BITMAP_16BPP;

	srcBitmap32 = freerdp_image_convert(srcBitmap16, NULL, 32, 32, 16, 32, clrconv);

	format = FREERDP_PIXEL_FORMAT(32, FREERDP_PIXEL_FORMAT_TYPE_ARGB, FREERDP_PIXEL_FLIP_NONE);

	//freerdp_bitmap_compress_planar(srcBitmap32, format, 32, 32, 32 * 4, NULL, &dstSize);

	//freerdp_bitmap_planar_compress_plane_rle((BYTE*) TEST_RLE_SCANLINE_UNCOMPRESSED, 12, 1, NULL);

	//freerdp_bitmap_planar_delta_encode_scanlines((BYTE*) TEST_RDP6_SCANLINES_ABSOLUTE, 6, 3, NULL);

	freerdp_bitmap_planar_compress_plane_rle((BYTE*) TEST_RDP6_SCANLINES_DELTA_2C_ENCODED_UNSIGNED, 6, 3, NULL);

	freerdp_clrconv_free(clrconv);
	free(srcBitmap32);

	return 0;
}
