/**
 * WinPR: Windows Portable Runtime
 * Serial Communication API
 *
 * Copyright 2014 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 * Copyright 2014 Hewlett-Packard Development Company, L.P.
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

#ifndef WINPR_COMM_PRIVATE_H
#define WINPR_COMM_PRIVATE_H

#ifndef _WIN32

#include <winpr/comm.h>

#include "../handle/handle.h"

/**
 * IOCTLs table according the remote serial driver:
 * http://msdn.microsoft.com/en-us/library/windows/hardware/dn265347%28v=vs.85%29.aspx
 */
typedef enum _REMOTE_SERIAL_DRIVER_ID
{
	RemoteSerialDriverUnknown = 0,
	RemoteSerialDriverSerialSys,
	RemoteSerialDriverSerCxSys,
	RemoteSerialDriverSerCx2Sys /* default fallback */
} REMOTE_SERIAL_DRIVER_ID;

struct winpr_comm
{
	WINPR_HANDLE_DEF();

	int fd;
	REMOTE_SERIAL_DRIVER_ID remoteSerialDriverId;

	/* NB: CloseHandle() has to free resources */
};
typedef struct winpr_comm WINPR_COMM;


#endif /* _WIN32 */

#endif /* WINPR_COMM_PRIVATE_H */
