/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * NTLM Security Package
 *
 * Copyright 2011-2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
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

#ifndef FREERDP_AUTH_NTLM_PRIVATE_H
#define FREERDP_AUTH_NTLM_PRIVATE_H

#include <freerdp/auth/sspi.h>
#include <freerdp/auth/credssp.h>

#include "../sspi.h"

enum _NTLM_STATE
{
	NTLM_STATE_INITIAL,
	NTLM_STATE_NEGOTIATE,
	NTLM_STATE_CHALLENGE,
	NTLM_STATE_AUTHENTICATE,
	NTLM_STATE_FINAL
};
typedef enum _NTLM_STATE NTLM_STATE;

struct _NTLM_CONTEXT
{
	boolean ntlm_v2;
	NTLM_STATE state;
	uint32 NegotiateFlags;
	SEC_BUFFER NegotiateMessage;
	SEC_BUFFER ChallengeMessage;
	SEC_BUFFER AuthenticateMessage;
};
typedef struct _NTLM_CONTEXT NTLM_CONTEXT;

#endif /* FREERDP_AUTH_NTLM_PRIVATE_H */
