/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * FreeRDP Sample Server
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

#ifndef SFREERDP_SERVER_H
#define SFREERDP_SERVER_H

#include <freerdp/freerdp.h>
#include <freerdp/listener.h>
#include <freerdp/codec/rfx.h>
#include <freerdp/codec/nsc.h>
#include <freerdp/utils/thread.h>
#include <freerdp/channels/wtsvc.h>
#include <freerdp/server/audin.h>
#include <freerdp/server/rdpsnd.h>

struct test_peer_context
{
	rdpContext _p;

	RFX_CONTEXT* rfx_context;
	NSC_CONTEXT* nsc_context;
	STREAM* s;
	uint8* icon_data;
	uint8* bg_data;
	int icon_width;
	int icon_height;
	int icon_x;
	int icon_y;
	boolean activated;
	WTSVirtualChannelManager* vcm;
	void* debug_channel;
	freerdp_thread* debug_channel_thread;
	audin_server_context* audin;
	boolean audin_open;
	uint32 frame_id;
	rdpsnd_server_context* rdpsnd;
};
typedef struct test_peer_context testPeerContext;

#endif /* SFREERDP_SERVER_H */

