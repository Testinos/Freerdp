/**
 * FreeRDP: A Remote Desktop Protocol Client
 * FreeRDP Mac OS X Server
 *
 * Copyright 2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 * Copyright 2012 Corey Clayton <can.of.tuna@gmail.com>
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

#ifndef MF_INTERFACE_H
#define MF_INTERFACE_H

#include <pthread.h>

#include <freerdp/codec/nsc.h>
#include <freerdp/listener.h>
#include <freerdp/utils/stream.h>

#ifdef WITH_SERVER_CHANNELS
#include <freerdp/channels/wtsvc.h>
#endif

#ifdef CHANNEL_RDPSND_SERVER
#include <freerdp/server/rdpsnd.h>
#include "mf_rdpsnd.h"
#endif

#ifdef CHANNEL_AUDIN_SERVER
#include "mf_audin.h"
#endif

typedef struct mf_info mfInfo;
typedef struct mf_peer_context mfPeerContext;

struct mf_peer_context
{
	rdpContext _p;
    
    mfInfo* info;
	STREAM* s;
	BOOL activated;
	UINT32 frame_id;
	BOOL audin_open;
	RFX_CONTEXT* rfx_context;
	NSC_CONTEXT* nsc_context;
    
#ifdef WITH_SERVER_CHANNELS
	WTSVirtualChannelManager* vcm;
#endif
#ifdef CHANNEL_AUDIN_SERVER
	audin_server_context* audin;
#endif
    
#ifdef CHANNEL_RDPSND_SERVER
	rdpsnd_server_context* rdpsnd;
#endif
};


struct mf_info
{
	//STREAM* s;
    
	//screen and monitor info
	int screenID;
	int virtscreen_width;
	int virtscreen_height;
	int servscreen_width;
	int servscreen_height;
	int servscreen_xoffset;
	int servscreen_yoffset;
    
	//int frame_idx;
	int bitsPerPixel;
	//HDC driverDC;
	int peerCount;
	int activePeerCount;
	//void* changeBuffer;
	int framesPerSecond;
	//LPTSTR deviceKey;
	//TCHAR deviceName[32];
	freerdp_peer** peers;
	//BOOL mirrorDriverActive;
	unsigned int framesWaiting;
    
	//HANDLE snd_mutex;
	//BOOL snd_stop;
    
	RFX_RECT invalid;
	pthread_mutex_t mutex;
	//BOOL updatePending;
	//HANDLE updateEvent;
	//HANDLE updateThread;
	//HANDLE updateSemaphore;
	//RFX_CONTEXT* rfx_context;
	//unsigned long lastUpdate;
	//unsigned long nextUpdate;
	//SURFACE_BITS_COMMAND cmd;
    
	BOOL input_disabled;
	BOOL force_all_disconnect;
};

#endif