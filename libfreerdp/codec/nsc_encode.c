/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * NSCodec Encoder
 *
 * Copyright 2012 Vic Lee
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include <winpr/crt.h>

#include <freerdp/codec/nsc.h>

#include "nsc_types.h"
#include "nsc_encode.h"

static void nsc_context_initialize_encode(NSC_CONTEXT* context)
{
	int i;
	UINT32 length;
	UINT32 tempWidth;
	UINT32 tempHeight;

	tempWidth = ROUND_UP_TO(context->width, 8);
	tempHeight = ROUND_UP_TO(context->height, 2);

	/* The maximum length a decoded plane can reach in all cases */
	length = tempWidth * tempHeight + 16;

	if (length > context->priv->PlaneBuffersLength)
	{
		for (i = 0; i < 5; i++)
			context->priv->PlaneBuffers[i] = (BYTE*) realloc(context->priv->PlaneBuffers[i], length);

		context->priv->PlaneBuffersLength = length;
	}

	if (context->nsc_stream.ChromaSubSamplingLevel > 0)
	{
		context->OrgByteCount[0] = tempWidth * context->height;
		context->OrgByteCount[1] = tempWidth * tempHeight / 4;
		context->OrgByteCount[2] = tempWidth * tempHeight / 4;
		context->OrgByteCount[3] = context->width * context->height;
	}
	else
	{
		context->OrgByteCount[0] = context->width * context->height;
		context->OrgByteCount[1] = context->width * context->height;
		context->OrgByteCount[2] = context->width * context->height;
		context->OrgByteCount[3] = context->width * context->height;
	}
}

static void nsc_encode_argb_to_aycocg(NSC_CONTEXT* context, BYTE* bmpdata, int rowstride)
{
	UINT16 x;
	UINT16 y;
	UINT16 rw;
	BYTE ccl;
	BYTE* src;
	BYTE* yplane;
	BYTE* coplane;
	BYTE* cgplane;
	BYTE* aplane;
	INT16 r_val;
	INT16 g_val;
	INT16 b_val;
	BYTE a_val;
	UINT32 tempWidth;
	UINT32 tempHeight;

	tempWidth = ROUND_UP_TO(context->width, 8);
	tempHeight = ROUND_UP_TO(context->height, 2);
	rw = (context->nsc_stream.ChromaSubSamplingLevel > 0 ? tempWidth : context->width);
	ccl = context->nsc_stream.ColorLossLevel;
	yplane = context->priv->PlaneBuffers[0];
	coplane = context->priv->PlaneBuffers[1];
	cgplane = context->priv->PlaneBuffers[2];
	aplane = context->priv->PlaneBuffers[3];

	for (y = 0; y < context->height; y++)
	{
		src = bmpdata + (context->height - 1 - y) * rowstride;
		yplane = context->priv->PlaneBuffers[0] + y * rw;
		coplane = context->priv->PlaneBuffers[1] + y * rw;
		cgplane = context->priv->PlaneBuffers[2] + y * rw;
		aplane = context->priv->PlaneBuffers[3] + y * context->width;

		for (x = 0; x < context->width; x++)
		{
			switch (context->pixel_format)
			{
				case RDP_PIXEL_FORMAT_B8G8R8A8:
					b_val = *src++;
					g_val = *src++;
					r_val = *src++;
					a_val = *src++;
					break;

				case RDP_PIXEL_FORMAT_R8G8B8A8:
					r_val = *src++;
					g_val = *src++;
					b_val = *src++;
					a_val = *src++;
					break;

				case RDP_PIXEL_FORMAT_B8G8R8:
					b_val = *src++;
					g_val = *src++;
					r_val = *src++;
					a_val = 0xFF;
					break;

				case RDP_PIXEL_FORMAT_R8G8B8:
					r_val = *src++;
					g_val = *src++;
					b_val = *src++;
					a_val = 0xFF;
					break;

				case RDP_PIXEL_FORMAT_B5G6R5_LE:
					b_val = (INT16) (((*(src + 1)) & 0xF8) | ((*(src + 1)) >> 5));
					g_val = (INT16) ((((*(src + 1)) & 0x07) << 5) | (((*src) & 0xE0) >> 3));
					r_val = (INT16) ((((*src) & 0x1F) << 3) | (((*src) >> 2) & 0x07));
					a_val = 0xFF;
					src += 2;
					break;

				case RDP_PIXEL_FORMAT_R5G6B5_LE:
					r_val = (INT16) (((*(src + 1)) & 0xF8) | ((*(src + 1)) >> 5));
					g_val = (INT16) ((((*(src + 1)) & 0x07) << 5) | (((*src) & 0xE0) >> 3));
					b_val = (INT16) ((((*src) & 0x1F) << 3) | (((*src) >> 2) & 0x07));
					a_val = 0xFF;
					src += 2;
					break;

				case RDP_PIXEL_FORMAT_P4_PLANER:
					{
						int shift;
						BYTE idx;

						shift = (7 - (x % 8));
						idx = ((*src) >> shift) & 1;
						idx |= (((*(src + 1)) >> shift) & 1) << 1;
						idx |= (((*(src + 2)) >> shift) & 1) << 2;
						idx |= (((*(src + 3)) >> shift) & 1) << 3;
						idx *= 3;
						r_val = (INT16) context->palette[idx];
						g_val = (INT16) context->palette[idx + 1];
						b_val = (INT16) context->palette[idx + 2];
						if (shift == 0)
							src += 4;
					}
					a_val = 0xFF;
					break;

				case RDP_PIXEL_FORMAT_P8:
					{
						int idx = (*src) * 3;

						r_val = (INT16) context->palette[idx];
						g_val = (INT16) context->palette[idx + 1];
						b_val = (INT16) context->palette[idx + 2];
						src++;
					}
					a_val = 0xFF;
					break;

				default:
					r_val = g_val = b_val = a_val = 0;
					break;
			}

			*yplane++ = (BYTE) ((r_val >> 2) + (g_val >> 1) + (b_val >> 2));

			/* Perform color loss reduction here */
			*coplane++ = (BYTE) ((r_val - b_val) >> ccl);
			*cgplane++ = (BYTE) ((-(r_val >> 1) + g_val - (b_val >> 1)) >> ccl);
			*aplane++ = a_val;
		}

		if (context->nsc_stream.ChromaSubSamplingLevel > 0 && (x % 2) == 1)
		{
			*yplane = *(yplane - 1);
			*coplane = *(coplane - 1);
			*cgplane = *(cgplane - 1);
		}
	}

	if (context->nsc_stream.ChromaSubSamplingLevel > 0 && (y % 2) == 1)
	{
		CopyMemory(yplane + rw, yplane, rw);
		CopyMemory(coplane + rw, coplane, rw);
		CopyMemory(cgplane + rw, cgplane, rw);
	}
}

static void nsc_encode_subsampling(NSC_CONTEXT* context)
{
	UINT16 x;
	UINT16 y;
	BYTE* co_dst;
	BYTE* cg_dst;
	INT8* co_src0;
	INT8* co_src1;
	INT8* cg_src0;
	INT8* cg_src1;
	UINT32 tempWidth;
	UINT32 tempHeight;

	tempWidth = ROUND_UP_TO(context->width, 8);
	tempHeight = ROUND_UP_TO(context->height, 2);

	for (y = 0; y < tempHeight >> 1; y++)
	{
		co_dst = context->priv->PlaneBuffers[1] + y * (tempWidth >> 1);
		cg_dst = context->priv->PlaneBuffers[2] + y * (tempWidth >> 1);
		co_src0 = (INT8*) context->priv->PlaneBuffers[1] + (y << 1) * tempWidth;
		co_src1 = co_src0 + tempWidth;
		cg_src0 = (INT8*) context->priv->PlaneBuffers[2] + (y << 1) * tempWidth;
		cg_src1 = cg_src0 + tempWidth;

		for (x = 0; x < tempWidth >> 1; x++)
		{
			*co_dst++ = (BYTE) (((INT16) *co_src0 + (INT16) *(co_src0 + 1) +
				(INT16) *co_src1 + (INT16) *(co_src1 + 1)) >> 2);
			*cg_dst++ = (BYTE) (((INT16) *cg_src0 + (INT16) *(cg_src0 + 1) +
				(INT16) *cg_src1 + (INT16) *(cg_src1 + 1)) >> 2);
			co_src0 += 2;
			co_src1 += 2;
			cg_src0 += 2;
			cg_src1 += 2;
		}
	}
}

void nsc_encode(NSC_CONTEXT* context, BYTE* bmpdata, int rowstride)
{
	nsc_encode_argb_to_aycocg(context, bmpdata, rowstride);

	if (context->nsc_stream.ChromaSubSamplingLevel > 0)
	{
		nsc_encode_subsampling(context);
	}
}

static UINT32 nsc_rle_encode(BYTE* in, BYTE* out, UINT32 originalSize)
{
	UINT32 left;
	UINT32 runlength = 1;
	UINT32 planeSize = 0;

	left = originalSize;
	/**
	 * We quit the loop if the running compressed size is larger than the original.
	 * In such cases data will be sent uncompressed.
	 */
	while (left > 4 && planeSize < originalSize - 4)
	{
		if (left > 5 && *in == *(in + 1))
		{
			runlength++;
		}
		else if (runlength == 1)
		{
			*out++ = *in;
			planeSize++;
		}
		else if (runlength < 256)
		{
			*out++ = *in;
			*out++ = *in;
			*out++ = runlength - 2;
			runlength = 1;
			planeSize += 3;
		}
		else
		{
			*out++ = *in;
			*out++ = *in;
			*out++ = 0xFF;
			*out++ = (runlength & 0x000000FF);
			*out++ = (runlength & 0x0000FF00) >> 8;
			*out++ = (runlength & 0x00FF0000) >> 16;
			*out++ = (runlength & 0xFF000000) >> 24;
			runlength = 1;
			planeSize += 7;
		}
		in++;
		left--;
	}

	if (planeSize < originalSize - 4)
		CopyMemory(out, in, 4);

	planeSize += 4;

	return planeSize;
}

static void nsc_rle_compress_data(NSC_CONTEXT* context)
{
	UINT16 i;
	BYTE* rle;
	UINT32 planeSize;
	UINT32 originalSize;

	rle = context->nsc_stream.Planes;

	for (i = 0; i < 4; i++)
	{
		originalSize = context->OrgByteCount[i];

		if (originalSize == 0)
		{
			planeSize = 0;
		}
		else
		{
			planeSize = nsc_rle_encode(context->priv->PlaneBuffers[i], context->priv->PlaneBuffers[4], originalSize);

			if (planeSize < originalSize)
				CopyMemory(context->priv->PlaneBuffers[i], context->priv->PlaneBuffers[4], planeSize);
			else
				planeSize = originalSize;
		}

		context->nsc_stream.PlaneByteCount[i] = planeSize;
	}
}

UINT32 nsc_compute_byte_count(NSC_CONTEXT* context, UINT32* ByteCount, UINT32 width, UINT32 height)
{
	UINT32 tempWidth;
	UINT32 tempHeight;
	UINT32 maxPlaneSize;

	tempWidth = ROUND_UP_TO(width, 8);
	tempHeight = ROUND_UP_TO(height, 2);

	maxPlaneSize = tempWidth * tempHeight + 16;

	if (context->nsc_stream.ChromaSubSamplingLevel > 0)
	{
		ByteCount[0] = tempWidth * height;
		ByteCount[1] = tempWidth * tempHeight / 4;
		ByteCount[2] = tempWidth * tempHeight / 4;
		ByteCount[3] = width * height;
	}
	else
	{
		ByteCount[0] = width * height;
		ByteCount[1] = width * height;
		ByteCount[2] = width * height;
		ByteCount[3] = width * height;
	}

	return maxPlaneSize;
}

NSC_MESSAGE* nsc_encode_messages(NSC_CONTEXT* context, BYTE* data, int x, int y,
		int width, int height, int scanline, int* numMessages, int maxDataSize)
{
	int step;
	int i, j;
	UINT32 ByteCount[4];
	UINT32 MaxPlaneSize;
	UINT32 MaxMessageSize;
	NSC_MESSAGE* messages;

	MaxPlaneSize = nsc_compute_byte_count(context, (UINT32*) ByteCount, width, height);
	MaxMessageSize = MaxPlaneSize + 20;

	maxDataSize -= 1024; /* reserve enough space for headers */

	if (MaxMessageSize > maxDataSize)
		*numMessages = MaxMessageSize / maxDataSize;

	if (*numMessages < 1)
		*numMessages = 1;

	messages = (NSC_MESSAGE*) malloc(sizeof(NSC_MESSAGE) * (*numMessages));
	ZeroMemory(messages, sizeof(sizeof(NSC_MESSAGE) * (*numMessages)));

	if (width > height)
	{
		/**
		 * Horizontal Split
		 */

		step = height / *numMessages;
		step += (step % 4);

		for (i = 0; i < *numMessages; i++)
		{
			messages[i].x = x;
			messages[i].width = width;
			messages[i].data = data;
			messages[i].scanline = scanline;

			messages[i].y = y + (i * step);
			messages[i].height = height - (i * step);

			messages[i].MaxPlaneSize = nsc_compute_byte_count(context,
					(UINT32*) messages[i].OrgByteCount, messages[i].width, messages[i].height);
		}
	}
	else
	{
		/**
		 * Vertical Split
		 */

		step = width / *numMessages;
		step += (step % 4);

		for (i = 0; i < *numMessages; i++)
		{
			messages[i].y = y;
			messages[i].height = height;
			messages[i].data = data;
			messages[i].scanline = scanline;

			messages[i].x = x + (i * step);
			messages[i].width = width - (i * step);

			messages[i].MaxPlaneSize = nsc_compute_byte_count(context,
					(UINT32*) messages[i].OrgByteCount, messages[i].width, messages[i].height);
		}
	}

	for (i = 0; i < *numMessages; i++)
	{
		for (j = 0; j < 4; j++)
		{
			messages[i].PlaneBuffers[j] = NULL;
		}
	}

	return messages;
}

void nsc_compose_message(NSC_CONTEXT* context, wStream* s, BYTE* data, int width, int height, int scanline)
{
	int i;

	context->width = width;
	context->height = height;
	nsc_context_initialize_encode(context);

	/* ARGB to AYCoCg conversion, chroma subsampling and colorloss reduction */
	PROFILER_ENTER(context->priv->prof_nsc_encode);
	context->encode(context, data, scanline);
	PROFILER_EXIT(context->priv->prof_nsc_encode);

	/* RLE encode */
	PROFILER_ENTER(context->priv->prof_nsc_rle_compress_data);
	nsc_rle_compress_data(context);
	PROFILER_EXIT(context->priv->prof_nsc_rle_compress_data);

	/* Assemble the NSCodec message into stream */
	Stream_EnsureRemainingCapacity(s, 20);
	Stream_Write_UINT32(s, context->nsc_stream.PlaneByteCount[0]); /* LumaPlaneByteCount (4 bytes) */
	Stream_Write_UINT32(s, context->nsc_stream.PlaneByteCount[1]); /* OrangeChromaPlaneByteCount (4 bytes) */
	Stream_Write_UINT32(s, context->nsc_stream.PlaneByteCount[2]); /* GreenChromaPlaneByteCount (4 bytes) */
	Stream_Write_UINT32(s, context->nsc_stream.PlaneByteCount[3]); /* AlphaPlaneByteCount (4 bytes) */
	Stream_Write_UINT8(s, context->nsc_stream.ColorLossLevel); /* ColorLossLevel (1 byte) */
	Stream_Write_UINT8(s, context->nsc_stream.ChromaSubSamplingLevel); /* ChromaSubsamplingLevel (1 byte) */
	Stream_Write_UINT16(s, 0); /* Reserved (2 bytes) */

	for (i = 0; i < 4; i++)
	{
		if (context->nsc_stream.PlaneByteCount[i] > 0)
		{
			Stream_EnsureRemainingCapacity(s, (int) context->nsc_stream.PlaneByteCount[i]);
			Stream_Write(s, context->priv->PlaneBuffers[i], context->nsc_stream.PlaneByteCount[i]);
		}
	}
}
