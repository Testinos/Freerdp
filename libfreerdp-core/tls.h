/**
 * FreeRDP: A Remote Desktop Protocol Client
 * Transport Layer Security
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

#ifndef __TLS_H
#define __TLS_H

#include "crypto.h"

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <freerdp/types.h>
#include <freerdp/utils/stream.h>

typedef struct rdp_tls rdpTls;
typedef boolean (*TlsConnect) (rdpTls* tls);
typedef boolean (*TlsAccept) (rdpTls* tls, const char* cert_file, const char* privatekey_file);
typedef boolean (*TlsDisconnect) (rdpTls* tls);

struct rdp_tls
{
	SSL* ssl;
	int sockfd;
	SSL_CTX* ctx;
	TlsConnect connect;
	TlsAccept accept;
	TlsDisconnect disconnect;
};

boolean tls_connect(rdpTls* tls);
boolean tls_accept(rdpTls* tls, const char* cert_file, const char* privatekey_file);
boolean tls_disconnect(rdpTls* tls);
int tls_read(rdpTls* tls, uint8* data, int length);
int tls_write(rdpTls* tls, uint8* data, int length);
CryptoCert tls_get_certificate(rdpTls* tls);
boolean tls_print_error(char* func, SSL* connection, int value);
boolean tls_verify_cert(CryptoCert cert);
rdpTls* tls_new();
void tls_free(rdpTls* tls);

#endif /* __TLS_H */
