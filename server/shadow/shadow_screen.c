/**
 * FreeRDP: A Remote Desktop Protocol Implementation
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "shadow_screen.h"

rdpShadowScreen* shadow_screen_new(rdpShadowServer* server)
{
	rdpShadowScreen* screen;

	screen = (rdpShadowScreen*) calloc(1, sizeof(rdpShadowScreen));

	if (!screen)
		return NULL;

	screen->server = server;

	return screen;
}

void shadow_screen_free(rdpShadowScreen* screen)
{
	if (!screen)
		return;
}

