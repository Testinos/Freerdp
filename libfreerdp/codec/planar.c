/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * RDP6 Planar Codec
 *
 * Copyright 2013 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <winpr/crt.h>
#include <winpr/print.h>

#include <freerdp/codec/bitmap.h>

#include "planar.h"

static int freerdp_bitmap_planar_decompress_plane_rle(BYTE* inPlane, int width, int height, BYTE* outPlane, int srcSize)
{
	int k;
	int x, y;
	BYTE* srcp;
	BYTE* dstp;
	UINT32 pixel;
	int scanline;
	int cRawBytes;
	int nRunLength;
	int deltaValue;
	BYTE controlByte;
	BYTE* currentScanline;
	BYTE* previousScanline;

	k = 0;

	srcp = inPlane;
	dstp = outPlane;
	scanline = width * 4;
	previousScanline = NULL;

	y = 0;

	while (y < height)
	{
		pixel = 0;
		dstp = (outPlane + height * scanline) - ((y + 1) * scanline);
		currentScanline = dstp;

		x = 0;

		while (x < width)
		{
			controlByte = *srcp;
			srcp++;

			if ((srcp - inPlane) > srcSize)
			{
				printf("freerdp_bitmap_planar_decompress_plane_rle: error reading input buffer\n");
				return -1;
			}

			nRunLength = PLANAR_CONTROL_BYTE_RUN_LENGTH(controlByte);
			cRawBytes = PLANAR_CONTROL_BYTE_RAW_BYTES(controlByte);

			//printf("CONTROL(%d, %d)\n", cRawBytes, nRunLength);

			if (nRunLength == 1)
			{
				nRunLength = cRawBytes + 16;
				cRawBytes = 0;
			}
			else if (nRunLength == 2)
			{
				nRunLength = cRawBytes + 32;
				cRawBytes = 0;
			}

#if 0
			printf("y: %d cRawBytes: %d nRunLength: %d\n", y, cRawBytes, nRunLength);

			printf("RAW[");

			for (k = 0; k < cRawBytes; k++)
			{
				printf("0x%02X%s", srcp[k],
						((k + 1) == cRawBytes) ? "" : ", ");
			}

			printf("] RUN[%d]\n", nRunLength);
#endif

			if (((dstp + (cRawBytes + nRunLength)) - currentScanline) > width * 4)
			{
				printf("freerdp_bitmap_planar_decompress_plane_rle: too many pixels in scanline\n");
				return -1;
			}

			if (!previousScanline)
			{
				/* first scanline, absolute values */

				while (cRawBytes > 0)
				{
					pixel = *srcp;
					srcp++;

					*dstp = pixel;
					dstp += 4;
					x++;
					cRawBytes--;
				}

				while (nRunLength > 0)
				{
					*dstp = pixel;
					dstp += 4;
					x++;
					nRunLength--;
				}
			}
			else
			{
				/* delta values relative to previous scanline */

				while (cRawBytes > 0)
				{
					deltaValue = *srcp;
					srcp++;

					if (deltaValue & 1)
					{
						deltaValue = deltaValue >> 1;
						deltaValue = deltaValue + 1;
						pixel = -deltaValue;
					}
					else
					{
						deltaValue = deltaValue >> 1;
						pixel = deltaValue;
					}

					deltaValue = previousScanline[x * 4] + pixel;
					*dstp = deltaValue;
					dstp += 4;
					x++;
					cRawBytes--;
				}

				while (nRunLength > 0)
				{
					deltaValue = previousScanline[x * 4] + pixel;
					*dstp = deltaValue;
					dstp += 4;
					x++;
					nRunLength--;
				}
			}
		}

		previousScanline = currentScanline;
		y++;
	}

	return (int) (srcp - inPlane);
}

static int freerdp_bitmap_planar_decompress_plane_raw(BYTE* srcData, int width, int height, BYTE* dstData, int size)
{
	int x, y;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			dstData[(((height - y - 1) * width) + x) * 4] = srcData[((y * width) + x)];
		}
	}

	return (width * height);
}

int freerdp_bitmap_planar_decompress(BYTE* srcData, BYTE* dstData, int width, int height, int size)
{
	BYTE* srcp;
	int dstSize;
	BYTE FormatHeader;

	srcp = srcData;

	FormatHeader = *srcp;
	srcp++;

	/* AlphaPlane */

	if (!(FormatHeader & PLANAR_FORMAT_HEADER_NA))
	{
		if (FormatHeader & PLANAR_FORMAT_HEADER_RLE)
		{
			dstSize = freerdp_bitmap_planar_decompress_plane_rle(srcp, width, height, dstData + 3, size - (srcp - srcData));

			if (dstSize < 0)
				return -1;

			srcp += dstSize;
		}
		else
		{
			dstSize = freerdp_bitmap_planar_decompress_plane_raw(srcp, width, height, dstData + 3, size - (srcp - srcData));
			srcp += dstSize;
		}
	}

	if (FormatHeader & PLANAR_FORMAT_HEADER_RLE)
	{
		/* LumaOrRedPlane */

		dstSize = freerdp_bitmap_planar_decompress_plane_rle(srcp, width, height, dstData + 2, size - (srcp - srcData));

		if (dstSize < 0)
			return -1;

		srcp += dstSize;

		/* OrangeChromaOrGreenPlane */

		dstSize = freerdp_bitmap_planar_decompress_plane_rle(srcp, width, height, dstData + 1, size - (srcp - srcData));

		if (dstSize < 0)
			return -1;

		srcp += dstSize;

		/* GreenChromeOrBluePlane */

		dstSize = freerdp_bitmap_planar_decompress_plane_rle(srcp, width, height, dstData + 0, size - (srcp - srcData));

		if (dstSize < 0)
			return -1;

		srcp += dstSize;
	}
	else
	{
		/* LumaOrRedPlane */

		dstSize = freerdp_bitmap_planar_decompress_plane_raw(srcp, width, height, dstData + 2, size - (srcp - srcData));
		srcp += dstSize;

		/* OrangeChromaOrGreenPlane */

		dstSize = freerdp_bitmap_planar_decompress_plane_raw(srcp, width, height, dstData + 1, size - (srcp - srcData));
		srcp += dstSize;

		/* GreenChromeOrBluePlane */

		dstSize = freerdp_bitmap_planar_decompress_plane_raw(srcp, width, height, dstData + 0, size - (srcp - srcData));
		srcp += dstSize;
		srcp++;
	}

	return (size == (srcp - srcData)) ? 0 : -1;
}

int freerdp_split_color_planes(BYTE* data, UINT32 format, int width, int height, int scanline, BYTE* planes[4])
{
	int bpp;
	int i, j, k;

	k = 0;
	bpp = FREERDP_PIXEL_FORMAT_BPP(format);

	if (bpp == 32)
	{
		UINT32* pixel;

		for (i = height - 1; i >= 0; i--)
		{
			pixel = (UINT32*) &data[scanline * i];

			for (j = 0; j < width; j++)
			{
				GetARGB32(planes[0][k], planes[1][k], planes[2][k], planes[3][k], *pixel);
				pixel++;
				k++;
			}
		}
	}
	else if (bpp == 24)
	{
		UINT32* pixel;

		for (i = height - 1; i >= 0; i--)
		{
			pixel = (UINT32*) &data[scanline * i];

			for (j = 0; j < width; j++)
			{
				GetRGB32(planes[1][k], planes[2][k], planes[3][k], *pixel);
				planes[0][k] = 0xFF; /* A */
				pixel++;
				k++;
			}
		}
	}

	return 0;
}

struct _PLANAR_RLE_CONTEXT
{
	int width;
	int height;
	BYTE* output;
	int nRunLength;
	int cRawBytes;
	BYTE* rawValues;
	BYTE* rawScanline;
	BYTE* outPlane;
	int outPlaneSize;
	int outSegmentSize;
	int nRunLengthPrev;
};
typedef struct _PLANAR_RLE_CONTEXT PLANAR_RLE_CONTEXT;

int freerdp_bitmap_planar_compress_plane_rle_segment(PLANAR_RLE_CONTEXT* rle)
{
#if 0
	{
		int k;

		printf("RAW(%d)[", rle->cRawBytes);

		for (k = 0; k < rle->cRawBytes; k++)
		{
			printf("%02x%s", rle->rawValues[k],
					((k + 1) == rle->cRawBytes) ? "" : " ");
		}

		printf("] RUN[%d] offset: 0x%04x\n", rle->nRunLength,
				rle->output - rle->outPlane);
	}
#endif

	while ((rle->cRawBytes != 0) || (rle->nRunLength != 0))
	{
		//printf("|| cRawBytes: %d nRunLength: %d\n", rle->cRawBytes, rle->nRunLength);

		if (rle->nRunLength < 3)
		{
			rle->cRawBytes += rle->nRunLength;
			rle->nRunLength = 0;
		}

		if ((rle->rawValues - rle->rawScanline) > rle->width)
		{
			printf("rawValues overflow! %d\n", rle->rawValues - rle->rawScanline);
			return 0;
		}

		if (rle->cRawBytes > 15)
		{
			rle->outSegmentSize = 1 + 15;

			if (((rle->output - rle->outPlane) + rle->outSegmentSize) > rle->outPlaneSize)
			{
				printf("overflow: %d > %d\n", ((rle->output - rle->outPlane) + rle->outSegmentSize), rle->outPlaneSize);
				return -1;
			}

			*rle->output = PLANAR_CONTROL_BYTE(0, 15);
			rle->output++;

			CopyMemory(rle->output, rle->rawValues, 15);
			rle->cRawBytes -= 15;
			rle->rawValues += 15;
			rle->output += 15;

			/* continue */
		}
		else if (rle->cRawBytes == 0)
		{
			if (rle->nRunLength > 47)
			{
				rle->outSegmentSize = 1;

				if (((rle->output - rle->outPlane) + rle->outSegmentSize) > rle->outPlaneSize)
				{
					printf("overflow: %d > %d\n", ((rle->output - rle->outPlane) + rle->outSegmentSize), rle->outPlaneSize);
					return -1;
				}

				*rle->output = PLANAR_CONTROL_BYTE(2, 15);
				rle->nRunLength -= 47;
				rle->output++;

				/* continue */
			}
			else if (rle->nRunLength > 31)
			{
				rle->outSegmentSize = 1;

				if (((rle->output - rle->outPlane) + rle->outSegmentSize) > rle->outPlaneSize)
				{
					printf("overflow: %d > %d\n", ((rle->output - rle->outPlane) + rle->outSegmentSize), rle->outPlaneSize);
					return -1;
				}

				*rle->output = PLANAR_CONTROL_BYTE(2, (rle->nRunLength - 32));
				rle->output++;

				rle->rawValues += (rle->cRawBytes + rle->nRunLength);
				rle->output += rle->cRawBytes;

				rle->cRawBytes = 0;
				rle->nRunLength = 0;

				return 0; /* finish */
			}
			else if (rle->nRunLength > 15)
			{
				rle->outSegmentSize = 1;

				if (((rle->output - rle->outPlane) + rle->outSegmentSize) > rle->outPlaneSize)
				{
					printf("overflow: %d > %d\n", ((rle->output - rle->outPlane) + rle->outSegmentSize), rle->outPlaneSize);
					return -1;
				}

				*rle->output = PLANAR_CONTROL_BYTE(1, (rle->nRunLength - 16));
				rle->output++;

				rle->rawValues += (rle->cRawBytes + rle->nRunLength);
				rle->output += rle->cRawBytes;

				rle->cRawBytes = 0;
				rle->nRunLength = 0;

				return 0; /* finish */
			}
			else
			{
				rle->outSegmentSize = 1 + rle->cRawBytes;

				if (((rle->output - rle->outPlane) + rle->outSegmentSize) > rle->outPlaneSize)
				{
					printf("overflow: %d > %d\n", ((rle->output - rle->outPlane) + rle->outSegmentSize), rle->outPlaneSize);
					return -1;
				}

				*rle->output = PLANAR_CONTROL_BYTE(rle->nRunLength, rle->cRawBytes);
				rle->output++;

				rle->rawValues += (rle->cRawBytes + rle->nRunLength);
				rle->output += rle->cRawBytes;

				rle->cRawBytes = 0;
				rle->nRunLength = 0;

				return 0; /* finish */
			}
		}
		else if (rle->cRawBytes < 16)
		{
			if (rle->nRunLength > 15)
			{
				rle->outSegmentSize = 1 + rle->cRawBytes;

				if (((rle->output - rle->outPlane) + rle->outSegmentSize) > rle->outPlaneSize)
				{
					printf("overflow: %d > %d\n", ((rle->output - rle->outPlane) + rle->outSegmentSize), rle->outPlaneSize);
					return -1;
				}

				if (rle->nRunLength < 18)
				{
					*rle->output = PLANAR_CONTROL_BYTE(13, rle->cRawBytes);
					rle->output++;

					CopyMemory(rle->output, rle->rawValues, rle->cRawBytes);
					rle->rawValues += (rle->cRawBytes + 13);
					rle->output += rle->cRawBytes;

					rle->nRunLength -= 13;
					rle->cRawBytes = 0;
				}
				else
				{
					*rle->output = PLANAR_CONTROL_BYTE(15, rle->cRawBytes);
					rle->output++;

					CopyMemory(rle->output, rle->rawValues, rle->cRawBytes);
					rle->rawValues += (rle->cRawBytes + 15);
					rle->output += rle->cRawBytes;

					rle->nRunLength -= 15;
					rle->cRawBytes = 0;
				}

				/* continue */
			}
			else
			{
				rle->outSegmentSize = 1 + rle->cRawBytes;

				if (((rle->output - rle->outPlane) + rle->outSegmentSize) > rle->outPlaneSize)
				{
					printf("overflow: %d > %d\n", ((rle->output - rle->outPlane) + rle->outSegmentSize), rle->outPlaneSize);
					return -1;
				}

				*rle->output = PLANAR_CONTROL_BYTE(rle->nRunLength, rle->cRawBytes);
				rle->output++;

				CopyMemory(rle->output, rle->rawValues, rle->cRawBytes);
				rle->rawValues += (rle->cRawBytes + rle->nRunLength);
				rle->output += rle->cRawBytes;

				rle->cRawBytes = 0;
				rle->nRunLength = 0;

				return 0; /* finish */
			}
		}
	}

	return 0;
}

BYTE* freerdp_bitmap_planar_compress_plane_rle(BYTE* inPlane, int width, int height, BYTE* outPlane, int* dstSize)
{
	int i, j;
	int bSymbolMatch;
	int bSequenceEnd;
	PLANAR_RLE_CONTEXT rle_s;
	PLANAR_RLE_CONTEXT* rle = &rle_s;

	if (!outPlane)
	{
		rle->outPlaneSize = width * height * 2;
		outPlane = malloc(rle->outPlaneSize);
	}
	else
	{
		rle->outPlaneSize = *dstSize;
	}

	rle->output = outPlane;
	rle->outPlane = outPlane;
	rle->width = width;
	rle->height = height;

	for (i = 0; i < height; i++)
	{
		rle->nRunLength = 0;
		rle->cRawBytes = 1;

		rle->rawScanline = &inPlane[i * width];
		rle->rawValues = rle->rawScanline;

		rle->nRunLengthPrev = 0;

		//winpr_HexDump(rle->rawScanline, width);

		for (j = 1; j <= width; j++)
		{
			bSymbolMatch = FALSE;
			bSequenceEnd = (j == width) ? TRUE : FALSE;

			if (!bSequenceEnd)
			{
				if (rle->rawScanline[j] == rle->rawValues[rle->cRawBytes - 1])
				{
					bSymbolMatch = TRUE;
				}
				else
				{
					//printf("mismatch: nRunLength: %d cRawBytes: %d\n", rle->nRunLength, rle->cRawBytes);

					if (rle->nRunLength < 3)
					{
						rle->cRawBytes += rle->nRunLength;
						rle->nRunLength = 0;
					}
					else
					{
						bSequenceEnd = TRUE;
					}
				}
			}

			if (bSymbolMatch)
			{
				rle->nRunLength++;
			}

			//printf("j: %d [0x%02X] cRawBytes: %d nRunLength: %d bSymbolMatch: %d bSequenceEnd: %d\n",
			//		j, rle->rawScanline[j], rle->cRawBytes, rle->nRunLength, bSymbolMatch, bSequenceEnd);

			if (bSequenceEnd)
			{
				int nRunLengthPrev;

				if (rle->nRunLength < 3)
				{
					rle->cRawBytes += rle->nRunLength;
					rle->nRunLength = 0;
				}

				if ((rle->cRawBytes == 1) && (*rle->rawValues == 0))
				{
					if (!rle->nRunLengthPrev)
					{
						rle->cRawBytes = 0;
						rle->nRunLength++;
					}
				}

				nRunLengthPrev = rle->nRunLength;

				if (freerdp_bitmap_planar_compress_plane_rle_segment(rle) < 0)
					return NULL;

				rle->nRunLengthPrev = nRunLengthPrev;

				rle->nRunLength = 0;
				rle->cRawBytes = 1;
			}
			else
			{
				if (!bSymbolMatch)
				{
					rle->cRawBytes++;
				}
			}
		}
	}

	*dstSize = (rle->output - outPlane);

	return outPlane;
}

int freerdp_bitmap_planar_compress_planes_rle(BYTE* inPlanes[4], int width, int height, BYTE* outPlanes, int* dstSizes, BOOL skipAlpha)
{
	int outPlanesSize = width * height * 4;

	/* AlphaPlane */

	if (skipAlpha)
	{
		dstSizes[0] = 0;
	}
	else
	{
		dstSizes[0] = outPlanesSize;

		if (!freerdp_bitmap_planar_compress_plane_rle(inPlanes[0], width, height, outPlanes, &dstSizes[0]))
			return 0;

		outPlanes += dstSizes[0];
		outPlanesSize -= dstSizes[0];
	}

	/* LumaOrRedPlane */

	dstSizes[1] = outPlanesSize;

	if (!freerdp_bitmap_planar_compress_plane_rle(inPlanes[1], width, height, outPlanes, &dstSizes[1]))
		return 0;

	outPlanes += dstSizes[1];
	outPlanesSize -= dstSizes[1];

	/* OrangeChromaOrGreenPlane */

	dstSizes[2] = outPlanesSize;

	if (!freerdp_bitmap_planar_compress_plane_rle(inPlanes[2], width, height, outPlanes, &dstSizes[2]))
		return 0;

	outPlanes += dstSizes[2];
	outPlanesSize -= dstSizes[2];

	/* GreenChromeOrBluePlane */

	dstSizes[3] = outPlanesSize;

	if (!freerdp_bitmap_planar_compress_plane_rle(inPlanes[3], width, height, outPlanes, &dstSizes[3]))
		return 0;

	outPlanes += dstSizes[3];
	outPlanesSize -= dstSizes[3];

	return 1;
}

BYTE* freerdp_bitmap_planar_delta_encode_plane(BYTE* inPlane, int width, int height, BYTE* outPlane)
{
	char s2c;
	BYTE u2c;
	int delta;
	int i, j, k;

	if (!outPlane)
	{
		outPlane = (BYTE*) malloc(width * height);
	}

	k = 0;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (i < 1)
			{
				delta = inPlane[j];

				s2c = (delta >= 0) ? (char) delta : (char) (~((BYTE) (delta * -1)) + 1);
			}
			else
			{
				delta = inPlane[(i * width) + j] - inPlane[((i - 1) * width) + j];

				s2c = (delta >= 0) ? (char) delta : (char) (~((BYTE) (delta * -1)) + 1);

				s2c = (s2c >= 0) ? (s2c << 1) : (char) (((~((BYTE) s2c) + 1) << 1) - 1);
			}

			u2c = (BYTE) s2c;

			outPlane[(i * width) + j] = u2c;

			k++;
		}
	}

	return outPlane;
}

int freerdp_bitmap_planar_delta_encode_planes(BYTE* inPlanes[4], int width, int height, BYTE* outPlanes[4])
{
	freerdp_bitmap_planar_delta_encode_plane(inPlanes[0], width, height, outPlanes[0]);
	freerdp_bitmap_planar_delta_encode_plane(inPlanes[1], width, height, outPlanes[1]);
	freerdp_bitmap_planar_delta_encode_plane(inPlanes[2], width, height, outPlanes[2]);
	freerdp_bitmap_planar_delta_encode_plane(inPlanes[3], width, height, outPlanes[3]);

	return 0;
}

BYTE* freerdp_bitmap_compress_planar(BITMAP_PLANAR_CONTEXT* context, BYTE* data, UINT32 format,
		int width, int height, int scanline, BYTE* dstData, int* dstSize)
{
	int size;
	BYTE* dstp;
	int planeSize;
	int dstSizes[4];
	BYTE FormatHeader = 0;

	if (context->AllowSkipAlpha)
		FormatHeader |= PLANAR_FORMAT_HEADER_NA;

	planeSize = width * height;

	freerdp_split_color_planes(data, format, width, height, scanline, context->planes);

	if (context->AllowRunLengthEncoding)
	{
		freerdp_bitmap_planar_delta_encode_planes(context->planes, width, height, context->deltaPlanes);

		if (freerdp_bitmap_planar_compress_planes_rle(context->deltaPlanes, width, height,
				context->rlePlanesBuffer, (int*) &dstSizes, context->AllowSkipAlpha) > 0)
		{
			int offset = 0;

			FormatHeader |= PLANAR_FORMAT_HEADER_RLE;

			context->rlePlanes[0] = &context->rlePlanesBuffer[offset];
			offset += dstSizes[0];

			context->rlePlanes[1] = &context->rlePlanesBuffer[offset];
			offset += dstSizes[1];

			context->rlePlanes[2] = &context->rlePlanesBuffer[offset];
			offset += dstSizes[2];

			context->rlePlanes[3] = &context->rlePlanesBuffer[offset];
			offset += dstSizes[3];

			//printf("R: [%d/%d] G: [%d/%d] B: [%d/%d]\n",
			//		dstSizes[1], planeSize, dstSizes[2], planeSize, dstSizes[3], planeSize);
		}
	}

	if (!dstData)
	{
		size = 1;

		if (!(FormatHeader & PLANAR_FORMAT_HEADER_NA))
		{
			if (FormatHeader & PLANAR_FORMAT_HEADER_RLE)
				size += dstSizes[0];
			else
				size += planeSize;
		}

		if (FormatHeader & PLANAR_FORMAT_HEADER_RLE)
			size += (dstSizes[1] + dstSizes[2] + dstSizes[3]);
		else
			size += (planeSize * 3);

		if (!(FormatHeader & PLANAR_FORMAT_HEADER_RLE))
			size++;

		dstData = malloc(size);
		*dstSize = size;
	}

	dstp = dstData;

	*dstp = FormatHeader; /* FormatHeader */
	dstp++;

	/* AlphaPlane */

	if (!(FormatHeader & PLANAR_FORMAT_HEADER_NA))
	{
		if (FormatHeader & PLANAR_FORMAT_HEADER_RLE)
		{
			CopyMemory(dstp, context->rlePlanes[0], dstSizes[0]); /* Alpha */
			dstp += dstSizes[0];
		}
		else
		{
			CopyMemory(dstp, context->planes[0], planeSize); /* Alpha */
			dstp += planeSize;
		}
	}

	/* LumaOrRedPlane */

	if (FormatHeader & PLANAR_FORMAT_HEADER_RLE)
	{
		CopyMemory(dstp, context->rlePlanes[1], dstSizes[1]); /* Red */
		dstp += dstSizes[1];
	}
	else
	{
		CopyMemory(dstp, context->planes[1], planeSize); /* Red */
		dstp += planeSize;
	}

	/* OrangeChromaOrGreenPlane */

	if (FormatHeader & PLANAR_FORMAT_HEADER_RLE)
	{
		CopyMemory(dstp, context->rlePlanes[2], dstSizes[2]); /* Green */
		dstp += dstSizes[2];
	}
	else
	{
		CopyMemory(dstp, context->planes[2], planeSize); /* Green */
		dstp += planeSize;
	}

	/* GreenChromeOrBluePlane */

	if (FormatHeader & PLANAR_FORMAT_HEADER_RLE)
	{
		CopyMemory(dstp, context->rlePlanes[3], dstSizes[3]); /* Blue */
		dstp += dstSizes[3];
	}
	else
	{
		CopyMemory(dstp, context->planes[3], planeSize); /* Blue */
		dstp += planeSize;
	}

	/* Pad1 (1 byte) */

	if (!(FormatHeader & PLANAR_FORMAT_HEADER_RLE))
	{
		*dstp = 0;
		dstp++;
	}

	size = (dstp - dstData);
	*dstSize = size;

	return dstData;
}

BITMAP_PLANAR_CONTEXT* freerdp_bitmap_planar_context_new(DWORD flags, int maxWidth, int maxHeight)
{
	BITMAP_PLANAR_CONTEXT* context;

	context = (BITMAP_PLANAR_CONTEXT*) malloc(sizeof(BITMAP_PLANAR_CONTEXT));

	if (!context)
		return NULL;

	ZeroMemory(context, sizeof(BITMAP_PLANAR_CONTEXT));

	if (flags & PLANAR_FORMAT_HEADER_NA)
		context->AllowSkipAlpha = TRUE;

	if (flags & PLANAR_FORMAT_HEADER_RLE)
		context->AllowRunLengthEncoding = TRUE;

	if (flags & PLANAR_FORMAT_HEADER_CS)
		context->AllowColorSubsampling = TRUE;

	context->ColorLossLevel = flags & PLANAR_FORMAT_HEADER_CLL_MASK;

	if (context->ColorLossLevel)
		context->AllowDynamicColorFidelity = TRUE;

	context->maxWidth = maxWidth;
	context->maxHeight = maxHeight;
	context->maxPlaneSize = context->maxWidth * context->maxHeight;

	context->planesBuffer = malloc(context->maxPlaneSize * 4);
	context->planes[0] = &context->planesBuffer[context->maxPlaneSize * 0];
	context->planes[1] = &context->planesBuffer[context->maxPlaneSize * 1];
	context->planes[2] = &context->planesBuffer[context->maxPlaneSize * 2];
	context->planes[3] = &context->planesBuffer[context->maxPlaneSize * 3];

	context->deltaPlanesBuffer = malloc(context->maxPlaneSize * 4);
	context->deltaPlanes[0] = &context->deltaPlanesBuffer[context->maxPlaneSize * 0];
	context->deltaPlanes[1] = &context->deltaPlanesBuffer[context->maxPlaneSize * 1];
	context->deltaPlanes[2] = &context->deltaPlanesBuffer[context->maxPlaneSize * 2];
	context->deltaPlanes[3] = &context->deltaPlanesBuffer[context->maxPlaneSize * 3];

	context->rlePlanesBuffer = malloc(context->maxPlaneSize * 4);

	return context;
}

void freerdp_bitmap_planar_context_free(BITMAP_PLANAR_CONTEXT* context)
{
	if (!context)
		return;

	free(context->planesBuffer);
	free(context->deltaPlanesBuffer);
	free(context->rlePlanesBuffer);

	free(context);
}
