/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 *
 * Copyright 2014 Marc-Andre Moreau <marcandre.moreau@gmail.com>
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

#include "shadow_surface.h"

#include "shadow_capture.h"

int shadow_capture_compare(BYTE* pData1, int nStep1, int nWidth, int nHeight, BYTE* pData2, int nStep2, RECTANGLE_16* rect)
{
	BOOL equal;
	BOOL allEqual;
	int tw, th;
	int tx, ty, k;
	int nrow, ncol;
	int l, t, r, b;
	BYTE *p1, *p2;
	BOOL rows[1024];
	BOOL cols[1024];
	BOOL grid[1024][1024];

	allEqual = TRUE;
	FillMemory(rows, sizeof(rows), 0xFF);
	FillMemory(cols, sizeof(cols), 0xFF);
	FillMemory(grid, sizeof(grid), 0xFF);
	ZeroMemory(rect, sizeof(RECTANGLE_16));

	nrow = (nHeight + 15) / 16;
	ncol = (nWidth + 15) / 16;

	l = ncol + 1;
	r = -1;

	t = nrow + 1;
	b = -1;

	for (ty = 0; ty < nrow; ty++)
	{
		th = ((ty + 1) == nrow) ? nHeight % 16 : 16;

		for (tx = 0; tx < ncol; tx++)
		{
			equal = TRUE;
			tw = ((tx + 1) == ncol) ? nWidth % 16 : 16;

			p1 = &pData1[(ty * 16 * nStep1) + (tx * 16 * 4)];
			p2 = &pData2[(ty * 16 * nStep2) + (tx * 16 * 4)];

			for (k = 0; k < th; k++)
			{
				if (memcmp(p1, p2, tw) != 0)
				{
					equal = FALSE;
					break;
				}

				p1 += nStep1;
				p2 += nStep2;
			}

			if (!equal)
			{
				grid[ty][tx] = FALSE;
				rows[ty] = FALSE;
				cols[tx] = FALSE;

				if (l > tx)
					l = tx;

				if (r < tx)
					r = tx;
			}
		}

		if (!rows[ty])
		{
			allEqual = FALSE;

			if (t > ty)
				t = ty;

			if (b < ty)
				b = ty;
		}
	}

	if (allEqual)
		return 0;

	rect->left = l * 16;
	rect->top = t * 16;
	rect->right = (r + 1) * 16;
	rect->bottom = (b + 1) * 16;

	if (0)
	{
		printf("\n");

		for (tx = 0; tx < ncol; tx++)
			printf("-");
		printf("\n");

		for (tx = 0; tx < ncol; tx++)
			printf("%s", cols[tx] ? "O" : "X");
		printf("\n");

		for (tx = 0; tx < ncol; tx++)
			printf("-");
		printf("\n");

		for (ty = 0; ty < nrow; ty++)
		{
			for (tx = 0; tx < ncol; tx++)
			{
				printf("%s", grid[ty][tx] ? "O" : "X");
			}

			printf("|%s|\n", rows[ty] ? "O" : "X");
		}
	}

	return 1;
}

rdpShadowCapture* shadow_capture_new(rdpShadowServer* server)
{
	rdpShadowCapture* capture;

	capture = (rdpShadowCapture*) calloc(1, sizeof(rdpShadowCapture));

	if (!capture)
		return NULL;

	capture->server = server;

	if (!InitializeCriticalSectionAndSpinCount(&(capture->lock), 4000))
		return NULL;

	return capture;
}

void shadow_capture_free(rdpShadowCapture* capture)
{
	if (!capture)
		return;

	DeleteCriticalSection(&(capture->lock));

	free(capture);
}

