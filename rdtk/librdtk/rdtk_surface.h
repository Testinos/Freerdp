/**
 * RdTk: Remote Desktop Toolkit
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

#ifndef RDTK_SURFACE_PRIVATE_H
#define RDTK_SURFACE_PRIVATE_H

#include <rdtk/rdtk.h>

struct rdtk_surface
{
	int width;
	int height;
	int scanline;
	BYTE* data;
	BOOL owner;
};

#ifdef __cplusplus
extern "C" {
#endif

rdtkSurface* rdtk_surface_new(BYTE* data, int width, int height, int scanline);
void rdtk_surface_free(rdtkSurface* surface);

#ifdef __cplusplus
}
#endif

#endif /* RDTK_SURFACE_PRIVATE_H */

