/**
 * WinPR: Windows Portable Runtime
 * Security Support Provider Interface (SSPI)
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

#ifndef WINPR_SSPI_PRIVATE_H
#define WINPR_SSPI_PRIVATE_H

#include "config.h"

#include <winpr/sspi.h>

struct _CREDENTIALS
{
	SEC_WINNT_AUTH_IDENTITY identity;
};
typedef struct _CREDENTIALS CREDENTIALS;

CREDENTIALS* sspi_CredentialsNew();
void sspi_CredentialsFree(CREDENTIALS* credentials);

SecHandle* sspi_SecureHandleAlloc();
void sspi_SecureHandleInit(SecHandle* handle);
void sspi_SecureHandleInvalidate(SecHandle* handle);
void* sspi_SecureHandleGetLowerPointer(SecHandle* handle);
void sspi_SecureHandleSetLowerPointer(SecHandle* handle, void* pointer);
void* sspi_SecureHandleGetUpperPointer(SecHandle* handle);
void sspi_SecureHandleSetUpperPointer(SecHandle* handle, void* pointer);
void sspi_SecureHandleFree(SecHandle* handle);

void sspi_SetAuthIdentity(SEC_WINNT_AUTH_IDENTITY* identity, char* user, char* domain, char* password);

enum SecurityFunctionTableIndex
{
	EnumerateSecurityPackagesIndex = 1,
	Reserved1Index = 2,
	QueryCredentialsAttributesIndex = 3,
	AcquireCredentialsHandleIndex = 4,
	FreeCredentialsHandleIndex = 5,
	Reserved2Index = 6,
	InitializeSecurityContextIndex = 7,
	AcceptSecurityContextIndex = 8,
	CompleteAuthTokenIndex = 9,
	DeleteSecurityContextIndex = 10,
	ApplyControlTokenIndex = 11,
	QueryContextAttributesIndex = 12,
	ImpersonateSecurityContextIndex = 13,
	RevertSecurityContextIndex = 14,
	MakeSignatureIndex = 15,
	VerifySignatureIndex = 16,
	FreeContextBufferIndex = 17,
	QuerySecurityPackageInfoIndex = 18,
	Reserved3Index = 19,
	Reserved4Index = 20,
	ExportSecurityContextIndex = 21,
	ImportSecurityContextIndex = 22,
	AddCredentialsIndex = 23,
	Reserved8Index = 24,
	QuerySecurityContextTokenIndex = 25,
	EncryptMessageIndex = 26,
	DecryptMessageIndex = 27,
	SetContextAttributesIndex = 28
};

#endif /* WINPR_SSPI_PRIVATE_H */
