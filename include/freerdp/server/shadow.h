/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * Session Shadowing
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

#ifndef FREERDP_SERVER_SHADOW_H
#define FREERDP_SERVER_SHADOW_H

#include <freerdp/api.h>
#include <freerdp/types.h>

#include <freerdp/listener.h>

typedef struct rdp_shadow_client rdpShadowClient;
typedef struct rdp_shadow_server rdpShadowServer;
typedef struct rdp_shadow_screen rdpShadowScreen;
typedef struct rdp_shadow_surface rdpShadowSurface;
typedef struct rdp_shadow_encoder rdpShadowEncoder;

struct rdp_shadow_client
{
	rdpContext context;

	void* ext;
	HANDLE thread;
	rdpShadowServer* server;
};

struct rdp_shadow_server
{
	void* ext;
	HANDLE thread;
	rdpShadowScreen* screen;
	rdpShadowSurface* surface;
	rdpShadowEncoder* encoder;

	DWORD port;
	freerdp_listener* listener;
};

#endif /* FREERDP_SERVER_SHADOW_H */

