/**
 * FreeRDP: A Remote Desktop Protocol Client
 * FreeRDP Windows Server
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

#ifndef WFREERDP_H
#define WFREERDP_H

//#define WITH_WIN8	1
//#define WITH_DOUBLE_BUFFERING	1

#include <freerdp/freerdp.h>
#include <freerdp/codec/rfx.h>
#include "wf_info.h"

struct wf_peer_context
{
	rdpContext _p;

	STREAM* s;
	wfInfo* info;
	boolean activated;
	RFX_CONTEXT* rfx_context;
};
typedef struct wf_peer_context wfPeerContext;

#endif /* WFREERDP_H */
