/**
 * FreeRDP: A Remote Desktop Protocol client.
 * Clipboard Virtual Channel
 *
 * Copyright 2009-2011 Jay Sorg
 * Copyright 2010-2011 Vic Lee
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
#include <stdlib.h>
#include <string.h>
#include <freerdp/constants.h>
#include <freerdp/types.h>
#include <freerdp/utils/memory.h>
#include <freerdp/utils/svc_plugin.h>

#include "cliprdr_constants.h"
#include "cliprdr_main.h"
#include "cliprdr_format.h"

#define CFSTR_HTML      "H\0T\0M\0L\0 \0F\0o\0r\0m\0a\0t\0\0"
#define CFSTR_PNG       "P\0N\0G\0\0"
#define CFSTR_JPEG      "J\0F\0I\0F\0\0"
#define CFSTR_GIF       "G\0I\0F\0\0"

void cliprdr_process_format_list_event(cliprdrPlugin* cliprdr, FRDP_CB_FORMAT_LIST_EVENT* cb_event)
{
	STREAM* data_out;
	int i;

	data_out = cliprdr_packet_new(CB_FORMAT_LIST, 0, 36 * cb_event->num_formats);

	for (i = 0; i < cb_event->num_formats; i++)
	{
		stream_write_uint32(data_out, cb_event->formats[i]);
		switch (cb_event->formats[i])
		{
			case CB_FORMAT_HTML:
				memcpy(stream_get_tail(data_out), CFSTR_HTML, sizeof(CFSTR_HTML));
				break;
			case CB_FORMAT_PNG:
				memcpy(stream_get_tail(data_out), CFSTR_PNG, sizeof(CFSTR_PNG));
				break;
			case CB_FORMAT_JPEG:
				memcpy(stream_get_tail(data_out), CFSTR_JPEG, sizeof(CFSTR_JPEG));
				break;
			case CB_FORMAT_GIF:
				memcpy(stream_get_tail(data_out), CFSTR_GIF, sizeof(CFSTR_GIF));
				break;
		}
		stream_seek(data_out, 32);
	}

	cliprdr_packet_send(cliprdr, data_out);
}
