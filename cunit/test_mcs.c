/**
 * FreeRDP: A Remote Desktop Protocol Client
 * T.125 Multipoint Communication Service (MCS) Protocol Unit Tests
 *
 * Copyright 2011 Marc-Andre Moreau <marcandre.moreau@gmail.com>
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

#include "mcs.h"

#include <freerdp/freerdp.h>
#include <freerdp/utils/hexdump.h>
#include <freerdp/utils/stream.h>

#include "test_mcs.h"

int init_mcs_suite(void)
{
	return 0;
}

int clean_mcs_suite(void)
{
	return 0;
}

int add_mcs_suite(void)
{
	add_test_suite(mcs);

	add_test_function(mcs_write_connect_initial);

	return 0;
}

/*
7F 65 82 01 94 04 01 01 04 01 01 01 01 FF 30 19
02 01 22 02 01 02 02 01 00 02 01 01 02 01 00 02
01 01 02 02 FF FF 02 01 02 30 19 02 01 01 02 01
01 02 01 01 02 01 01 02 01 00 02 01 01 02 02 04
20 02 01 02 30 1C 02 02 FF FF 02 02 FC 17 02 02
FF FF 02 01 01 02 01 00 02 01 01 02 02 FF FF 02
01 02 04 82 01 33
*/

uint8 gcc_CCrq[307] =
	"\x00\x05\x00\x14\x7C\x00\x01\x81\x2A\x00\x08\x00\x10\x00\x01\xC0"
	"\x00\x44\x75\x63\x61\x81\x1c\x01\xc0\xd8\x00\x04\x00\x08\x00\x00"
	"\x05\x00\x04\x01\xCA\x03\xAA\x09\x04\x00\x00\xCE\x0E\x00\x00\x45"
	"\x00\x4c\x00\x54\x00\x4f\x00\x4e\x00\x53\x00\x2d\x00\x44\x00\x45"
	"\x00\x56\x00\x32\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04"
	"\x00\x00\x00\x00\x00\x00\x00\x0c\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\xCA\x01\x00\x00"
	"\x00\x00\x00\x18\x00\x07\x00\x01\x00\x36\x00\x39\x00\x37\x00\x31"
	"\x00\x32\x00\x2d\x00\x37\x00\x38\x00\x33\x00\x2d\x00\x30\x00\x33"
	"\x00\x35\x00\x37\x00\x39\x00\x37\x00\x34\x00\x2d\x00\x34\x00\x32"
	"\x00\x37\x00\x31\x00\x34\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04"
	"\xC0\x0C\x00\x0D\x00\x00\x00\x00\x00\x00\x00\x02\xC0\x0C\x00\x1B"
	"\x00\x00\x00\x00\x00\x00\x00\x03\xC0\x2C\x00\x03\x00\x00\x00\x72"
	"\x64\x70\x64\x72\x00\x00\x00\x00\x00\x80\x80\x63\x6c\x69\x70\x72"
	"\x64\x72\x00\x00\x00\xA0\xC0\x72\x64\x70\x73\x6e\x64\x00\x00\x00"
	"\x00\x00\xc0";

void test_mcs_write_connect_initial(void)
{
	STREAM* s;
	rdpMcs* mcs;
	STREAM* user_data;

	mcs = mcs_new((rdpTransport*) NULL);

	user_data = stream_new(0);
	user_data->data = gcc_CCrq;
	user_data->p = user_data->data + sizeof(gcc_CCrq);

	s = stream_new(512);
	mcs_write_connect_initial(s, mcs, user_data);

	/* get expected value to compare with */
	CU_ASSERT(1 == 1);
}
