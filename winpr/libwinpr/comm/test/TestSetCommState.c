/**
 * WinPR: Windows Portable Runtime
 * Serial Communication API
 *
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

#include <stdio.h>

#include <winpr/comm.h>
#include <winpr/crt.h>

#include "../comm.h"

static BOOL test_fParity(HANDLE hComm)
{
	DCB dcb;
	BOOL result;

	ZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	result = GetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "GetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	/* test 1 */
	dcb.fParity = TRUE;
	result = SetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "SetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	ZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	result = GetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "GetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	if (!dcb.fParity)
	{
		fprintf(stderr, "unexpected fParity: %d instead of TRUE\n", dcb.fParity);
		return FALSE;
	}

	/* test 2 */
	dcb.fParity = FALSE;
	result = SetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "SetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	ZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	result = GetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "GetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	if (dcb.fParity)
	{
		fprintf(stderr, "unexpected fParity: %d instead of FALSE\n", dcb.fParity);
		return FALSE;
	}

	/* test 3 (redo test 1) */
	dcb.fParity = TRUE;
	result = SetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "SetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	ZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	result = GetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "GetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	if (!dcb.fParity)
	{
		fprintf(stderr, "unexpected fParity: %d instead of TRUE\n", dcb.fParity);
		return FALSE;
	}

	return TRUE;
}


static BOOL test_SerialSys(HANDLE hComm)
{
	DCB dcb;
	BOOL result;

	ZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	result = GetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "GetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	/* Test 1 */
	dcb.BaudRate = SERIAL_BAUD_115200;
	result = SetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "SetCommState failure: 0x%0.8x\n", GetLastError());
		return FALSE;
	}

	ZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	result = GetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "GetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}
	if (dcb.BaudRate != SERIAL_BAUD_115200)
	{
		fprintf(stderr, "SetCommState failure: could not set BaudRate=%d (SERIAL_BAUD_115200)\n", SERIAL_BAUD_115200);
		return FALSE;
	}

	/* Test 2 using a defferent baud rate */

	dcb.BaudRate = SERIAL_BAUD_57600;
	result = SetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "SetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	ZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	result = GetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "GetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}
	if (dcb.BaudRate != SERIAL_BAUD_57600)
	{
		fprintf(stderr, "SetCommState failure: could not set BaudRate=%d (SERIAL_BAUD_57600)\n", SERIAL_BAUD_57600);
		return FALSE;
	}

	/* Test 3 using an unsupported baud rate  on Linux */
	dcb.BaudRate = SERIAL_BAUD_128K;
	result = SetCommState(hComm, &dcb);
	if (result)
	{
		fprintf(stderr, "SetCommState failure: unexpected support of BaudRate=%d (SERIAL_BAUD_128K)\n", SERIAL_BAUD_128K);
		return FALSE;
	}

	return TRUE;
}

static BOOL test_SerCxSys(HANDLE hComm)
{
	DCB dcb;
	BOOL result;

	ZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	result = GetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "GetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	/* Test 1 */
	dcb.BaudRate = CBR_115200;
	result = SetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "SetCommState failure: 0x%0.8x\n", GetLastError());
		return FALSE;
	}

	ZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	result = GetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "GetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}
	if (dcb.BaudRate != CBR_115200)
	{
		fprintf(stderr, "SetCommState failure: could not set BaudRate=%d (CBR_115200)\n", CBR_115200);
		return FALSE;
	}

	/* Test 2 using a defferent baud rate */

	dcb.BaudRate = CBR_57600;
	result = SetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "SetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	ZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	result = GetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "GetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}
	if (dcb.BaudRate != CBR_57600)
	{
		fprintf(stderr, "SetCommState failure: could not set BaudRate=%d (CBR_57600)\n", CBR_57600);
		return FALSE;
	}

	/* Test 3 using an unsupported baud rate  on Linux */
	dcb.BaudRate = CBR_128000;
	result = SetCommState(hComm, &dcb);
	if (result)
	{
		fprintf(stderr, "SetCommState failure: unexpected support of BaudRate=%d (CBR_128000)\n", CBR_128000);
		return FALSE;
	}

	return TRUE;
}


static BOOL test_SerCx2Sys(HANDLE hComm)
{
	/* as of today there is no difference */
	return test_SerCxSys(hComm);
}

static BOOL test_generic(HANDLE hComm)
{
	DCB dcb, dcb2;
	BOOL result;
	
	ZeroMemory(&dcb, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	result = GetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "GetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	/* Checks whether we get the same information before and after SetCommState */
	memcpy(&dcb2, &dcb, sizeof(DCB));
	result = SetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "SetCommState failure: 0x%0.8x\n", GetLastError());
		return FALSE;
	}

	result = GetCommState(hComm, &dcb);
	if (!result)
	{
		fprintf(stderr, "GetCommState failure: 0x%x\n", GetLastError());
		return FALSE;
	}

	if (memcmp(&dcb, &dcb2, sizeof(DCB)) != 0)
	{
		fprintf(stderr, "DCB is different after SetCommState() whereas it should have not changed\n");
		return FALSE;
	}

	// TODO: a more complete and generic test using GetCommProperties()

	/* TMP: TODO: fBinary tests */

	/* fParity tests */
	if (!test_fParity(hComm))
	{
		fprintf(stderr, "test_fParity failure\n");
		return FALSE;
	}

	return TRUE;
}

int TestSetCommState(int argc, char* argv[])
{
	BOOL result;
	HANDLE hComm;

	// TMP: FIXME: check if we can proceed with tests on the actual device, skip and warn otherwise but don't fail
	result = DefineCommDevice("COM1", "/dev/ttyS0");
	if (!result)
	{
		fprintf(stderr, "DefineCommDevice failure: 0x%x\n", GetLastError());
		return EXIT_FAILURE;
	}

	hComm = CreateFile("COM1",
			   GENERIC_READ | GENERIC_WRITE,
			   0, NULL, OPEN_EXISTING, 0, NULL);
	if (hComm == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "CreateFileA failure: 0x%x\n", GetLastError());
		return EXIT_FAILURE;
	}

	if (!test_generic(hComm))
	{
		fprintf(stderr, "test_generic failure (RemoteSerialDriverUnknown)\n");
		return EXIT_FAILURE;
	}

	_comm_setRemoteSerialDriver(hComm, RemoteSerialDriverSerialSys);
	if (!test_generic(hComm))
	{
		fprintf(stderr, "test_generic failure (RemoteSerialDriverSerialSys)\n");
		return EXIT_FAILURE;
	}
	if (!test_SerialSys(hComm))
	{
		fprintf(stderr, "test_SerialSys failure\n");
		return EXIT_FAILURE;
	}


	_comm_setRemoteSerialDriver(hComm, RemoteSerialDriverSerCxSys);
	if (!test_generic(hComm))
	{
		fprintf(stderr, "test_generic failure (RemoteSerialDriverSerCxSys)\n");
		return EXIT_FAILURE;
	}
	if (!test_SerCxSys(hComm))
	{
		fprintf(stderr, "test_SerCxSys failure\n");
		return EXIT_FAILURE;
	}

	_comm_setRemoteSerialDriver(hComm, RemoteSerialDriverSerCx2Sys);
	if (!test_generic(hComm))
	{
		fprintf(stderr, "test_generic failure (RemoteSerialDriverSerCx2Sys)\n");
		return EXIT_FAILURE;
	}
	if (!test_SerCx2Sys(hComm))
	{
		fprintf(stderr, "test_SerCx2Sys failure\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
