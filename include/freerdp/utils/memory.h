/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * Memory Utils
 *
 * Copyright 2009-2011 Jay Sorg
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

#ifndef __MEMORY_UTILS_H
#define __MEMORY_UTILS_H

#include <stddef.h>
#include <ctype.h>
#include <wchar.h>
#include <freerdp/api.h>

FREERDP_API void* xzalloc(size_t size);

FREERDP_API char* xstrdup(const char* str);
FREERDP_API char* xstrtoup(const char* str);
FREERDP_API wchar_t* xwcsdup(const wchar_t* wstr);

#define xnew(_type) (_type*)xzalloc(sizeof(_type))

#define ARRAY_SIZE(_x) (sizeof(_x)/sizeof(*(_x)))

#endif /* __MEMORY_UTILS_H */
