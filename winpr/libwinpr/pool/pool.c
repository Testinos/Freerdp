/**
 * WinPR: Windows Portable Runtime
 * Thread Pool API (Pool)
 *
 * Copyright 2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
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
#include <winpr/pool.h>

#if (!(defined _WIN32 && (_WIN32_WINNT < 0x0600)))

PTP_POOL CreateThreadpool(PVOID reserved)
{
	return NULL;
}

VOID CloseThreadpool(PTP_POOL ptpp)
{

}

VOID SetThreadpoolThreadMaximum(PTP_POOL ptpp, DWORD cthrdMost)
{

}

BOOL SetThreadpoolThreadMinimum(PTP_POOL ptpp, DWORD cthrdMic)
{
	return FALSE;
}

#endif

void winpr_pool_dummy()
{

}
