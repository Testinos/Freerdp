/**
 * FreeRDP: A Remote Desktop Protocol Client
 * RDP Security
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

#ifndef __SECURITY_H
#define __SECURITY_H

#include "rdp.h"
#include "crypto.h"

#include <freerdp/freerdp.h>
#include <freerdp/utils/stream.h>

void security_salted_hash(uint8* salt, uint8* input, int length, uint8* client_random, uint8* server_random, uint8* output);
void security_premaster_hash(uint8* input, int length, uint8* premaster_secret, uint8* client_random, uint8* server_random, uint8* output);
void security_master_secret(uint8* premaster_secret, uint8* client_random, uint8* server_random, uint8* output);
void security_master_hash(uint8* input, int length, uint8* master_secret, uint8* client_random, uint8* server_random, uint8* output);
void security_session_key_blob(uint8* master_secret, uint8* client_random, uint8* server_random, uint8* output);
void security_mac_salt_key(uint8* session_key_blob, uint8* client_random, uint8* server_random, uint8* output);
void security_licensing_encryption_key(uint8* session_key_blob, uint8* client_random, uint8* server_random, uint8* output);

#endif /* __SECURITY_H */
