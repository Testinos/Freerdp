/**
 * WinPR: Windows Portable Runtime
 * NTLM Security Package
 *
 * Copyright 2011-2014 Marc-Andre Moreau <marcandre.moreau@gmail.com>
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

#include <time.h>
#include <openssl/des.h>
#include <openssl/md4.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/engine.h>

#include <winpr/crt.h>
#include <winpr/sspi.h>
#include <winpr/print.h>
#include <winpr/sysinfo.h>
#include <winpr/registry.h>

#include "ntlm.h"
#include "../sspi.h"

#include "ntlm_message.h"

char* NTLM_PACKAGE_NAME = "NTLM";

int ntlm_SetContextWorkstation(NTLM_CONTEXT* context, char* Workstation)
{
	int status;
	DWORD nSize = 0;
	char* ws = Workstation;

	if (!Workstation)
	{
		GetComputerNameExA(ComputerNameNetBIOS, NULL, &nSize);
		
		ws = (char*) malloc(nSize);

		if (!ws)
			return -1;

		if (!GetComputerNameExA(ComputerNameNetBIOS, ws, &nSize))
			return 0;
	}

	context->Workstation.Buffer = NULL;
	status = ConvertToUnicode(CP_UTF8, 0, ws, -1, &context->Workstation.Buffer, 0);
	free(ws);

	if (status <= 0)
		return -1;

	context->Workstation.Length = (USHORT) (status - 1);
	context->Workstation.Length *= 2;

	if (!Workstation)
		free(Workstation);

	return 1;
}

int ntlm_SetContextServicePrincipalNameW(NTLM_CONTEXT* context, LPWSTR ServicePrincipalName)
{
	if (!ServicePrincipalName)
	{
		context->ServicePrincipalName.Buffer = NULL;
		context->ServicePrincipalName.Length = 0;
		return 1;
	}

	context->ServicePrincipalName.Length = _wcslen(ServicePrincipalName) * 2;
	context->ServicePrincipalName.Buffer = (PWSTR) malloc(context->ServicePrincipalName.Length + 2);

	if (!context->ServicePrincipalName.Buffer)
		return -1;

	CopyMemory(context->ServicePrincipalName.Buffer, ServicePrincipalName, context->ServicePrincipalName.Length + 2);

	return 1;
}

int ntlm_SetContextServicePrincipalNameA(NTLM_CONTEXT* context, char* ServicePrincipalName)
{
	int status;

	context->ServicePrincipalName.Buffer = NULL;

	status = ConvertToUnicode(CP_UTF8, 0, ServicePrincipalName, -1, &context->ServicePrincipalName.Buffer, 0);

	if (status <= 0)
		return -1;

	context->ServicePrincipalName.Length = (USHORT) ((status - 1) * 2);

	return 1;
}

int ntlm_SetContextTargetName(NTLM_CONTEXT* context, char* TargetName)
{
	int status;
	DWORD nSize = 0;
	char* name = TargetName;

	if (!TargetName)
	{
		if (!GetComputerNameExA(ComputerNameDnsHostname, NULL, &nSize))
			return -1;

		name = (char*) malloc(nSize);

		if (!name)
			return -1;

		if (!GetComputerNameExA(ComputerNameDnsHostname, name, &nSize))
			return -1;

		CharUpperA(TargetName);
	}

	context->TargetName.pvBuffer = NULL;
	status = ConvertToUnicode(CP_UTF8, 0, name, -1, (LPWSTR*) &context->TargetName.pvBuffer, 0);

	if (status <= 0)
		return -1;

	context->TargetName.cbBuffer = (USHORT) ((status - 1) * 2);

	if (!TargetName)
		free(name);

	return 1;
}

NTLM_CONTEXT* ntlm_ContextNew()
{
	HKEY hKey;
	LONG status;
	DWORD dwType;
	DWORD dwSize;
	DWORD dwValue;
	NTLM_CONTEXT* context;

	context = (NTLM_CONTEXT*) calloc(1, sizeof(NTLM_CONTEXT));

	if (!context)
		return NULL;

	context->NTLMv2 = TRUE;
	context->UseMIC = FALSE;
	context->SendVersionInfo = TRUE;
	context->SendSingleHostData = FALSE;
	context->SendWorkstationName = TRUE;
	context->NegotiateKeyExchange = TRUE;

	status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\WinPR\\NTLM"), 0, KEY_READ | KEY_WOW64_64KEY, &hKey);

	if (status == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, _T("NTLMv2"), NULL, &dwType, (BYTE*) &dwValue, &dwSize) == ERROR_SUCCESS)
			context->NTLMv2 = dwValue ? 1 : 0;

		if (RegQueryValueEx(hKey, _T("UseMIC"), NULL, &dwType, (BYTE*) &dwValue, &dwSize) == ERROR_SUCCESS)
			context->UseMIC = dwValue ? 1 : 0;

		if (RegQueryValueEx(hKey, _T("SendVersionInfo"), NULL, &dwType, (BYTE*) &dwValue, &dwSize) == ERROR_SUCCESS)
			context->SendVersionInfo = dwValue ? 1 : 0;

		if (RegQueryValueEx(hKey, _T("SendSingleHostData"), NULL, &dwType, (BYTE*) &dwValue, &dwSize) == ERROR_SUCCESS)
			context->SendSingleHostData = dwValue ? 1 : 0;

		if (RegQueryValueEx(hKey, _T("SendWorkstationName"), NULL, &dwType, (BYTE*) &dwValue, &dwSize) == ERROR_SUCCESS)
			context->SendWorkstationName = dwValue ? 1 : 0;

		if (RegQueryValueEx(hKey, _T("WorkstationName"), NULL, &dwType, NULL, &dwSize) == ERROR_SUCCESS)
		{
			char* workstation = (char*) malloc(dwSize + 1);

			if (!workstation)
				return NULL;

			status = RegQueryValueExA(hKey, "WorkstationName", NULL, &dwType, (BYTE*) workstation, &dwSize);
			workstation[dwSize] = '\0';

			if (ntlm_SetContextWorkstation(context, workstation) < 0)
				return NULL;

			free(workstation);
		}

		RegCloseKey(hKey);
	}

	/*
	 * Extended Protection is enabled by default in Windows 7,
	 * but enabling it in WinPR breaks TS Gateway at this point
	 */
	context->SuppressExtendedProtection = FALSE;

	status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Control\\LSA"), 0, KEY_READ | KEY_WOW64_64KEY, &hKey);

	if (status == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, _T("SuppressExtendedProtection"), NULL, &dwType, (BYTE*) &dwValue, &dwSize) == ERROR_SUCCESS)
			context->SuppressExtendedProtection = dwValue ? 1 : 0;

		RegCloseKey(hKey);
	}

	context->NegotiateFlags = 0;
	context->LmCompatibilityLevel = 3;
	context->state = NTLM_STATE_INITIAL;
	FillMemory(context->MachineID, sizeof(context->MachineID), 0xAA);

	if (context->NTLMv2)
		context->UseMIC = TRUE;

	return context;
}

void ntlm_ContextFree(NTLM_CONTEXT* context)
{
	if (!context)
		return;

	sspi_SecBufferFree(&context->NegotiateMessage);
	sspi_SecBufferFree(&context->ChallengeMessage);
	sspi_SecBufferFree(&context->AuthenticateMessage);
	sspi_SecBufferFree(&context->ChallengeTargetInfo);
	sspi_SecBufferFree(&context->TargetName);
	sspi_SecBufferFree(&context->NtChallengeResponse);
	sspi_SecBufferFree(&context->LmChallengeResponse);

	free(context->ServicePrincipalName.Buffer);
	free(context->Workstation.Buffer);
	free(context);
}

SECURITY_STATUS SEC_ENTRY ntlm_AcquireCredentialsHandleW(SEC_WCHAR* pszPrincipal, SEC_WCHAR* pszPackage,
		ULONG fCredentialUse, void* pvLogonID, void* pAuthData, SEC_GET_KEY_FN pGetKeyFn,
		void* pvGetKeyArgument, PCredHandle phCredential, PTimeStamp ptsExpiry)
{
	SSPI_CREDENTIALS* credentials;
	SEC_WINNT_AUTH_IDENTITY* identity;

	if ((fCredentialUse != SECPKG_CRED_OUTBOUND) &&
		(fCredentialUse != SECPKG_CRED_INBOUND) &&
		(fCredentialUse != SECPKG_CRED_BOTH))
	{
		return SEC_E_INVALID_PARAMETER;
	}

	credentials = sspi_CredentialsNew();

	if (!credentials)
		return SEC_E_INTERNAL_ERROR;

	credentials->fCredentialUse = fCredentialUse;
	credentials->pGetKeyFn = pGetKeyFn;
	credentials->pvGetKeyArgument = pvGetKeyArgument;

	identity = (SEC_WINNT_AUTH_IDENTITY*) pAuthData;

	if (identity)
		sspi_CopyAuthIdentity(&(credentials->identity), identity);

	sspi_SecureHandleSetLowerPointer(phCredential, (void*) credentials);
	sspi_SecureHandleSetUpperPointer(phCredential, (void*) NTLM_PACKAGE_NAME);

	return SEC_E_OK;
}

SECURITY_STATUS SEC_ENTRY ntlm_AcquireCredentialsHandleA(SEC_CHAR* pszPrincipal, SEC_CHAR* pszPackage,
		ULONG fCredentialUse, void* pvLogonID, void* pAuthData, SEC_GET_KEY_FN pGetKeyFn,
		void* pvGetKeyArgument, PCredHandle phCredential, PTimeStamp ptsExpiry)
{
	SSPI_CREDENTIALS* credentials;
	SEC_WINNT_AUTH_IDENTITY* identity;

	if ((fCredentialUse != SECPKG_CRED_OUTBOUND) &&
		(fCredentialUse != SECPKG_CRED_INBOUND) &&
		(fCredentialUse != SECPKG_CRED_BOTH))
	{
		return SEC_E_INVALID_PARAMETER;
	}

	credentials = sspi_CredentialsNew();

	if (!credentials)
		return SEC_E_INTERNAL_ERROR;

	credentials->fCredentialUse = fCredentialUse;
	credentials->pGetKeyFn = pGetKeyFn;
	credentials->pvGetKeyArgument = pvGetKeyArgument;

	identity = (SEC_WINNT_AUTH_IDENTITY*) pAuthData;

	if (identity)
		sspi_CopyAuthIdentity(&(credentials->identity), identity);

	sspi_SecureHandleSetLowerPointer(phCredential, (void*) credentials);
	sspi_SecureHandleSetUpperPointer(phCredential, (void*) NTLM_PACKAGE_NAME);

	return SEC_E_OK;
}

SECURITY_STATUS SEC_ENTRY ntlm_FreeCredentialsHandle(PCredHandle phCredential)
{
	SSPI_CREDENTIALS* credentials;

	if (!phCredential)
		return SEC_E_INVALID_HANDLE;

	credentials = (SSPI_CREDENTIALS*) sspi_SecureHandleGetLowerPointer(phCredential);

	if (!credentials)
		return SEC_E_INVALID_HANDLE;

	sspi_CredentialsFree(credentials);

	return SEC_E_OK;
}

SECURITY_STATUS SEC_ENTRY ntlm_QueryCredentialsAttributesW(PCredHandle phCredential, ULONG ulAttribute, void* pBuffer)
{
	if (ulAttribute == SECPKG_CRED_ATTR_NAMES)
	{
		return SEC_E_OK;
	}

	return SEC_E_UNSUPPORTED_FUNCTION;
}

SECURITY_STATUS SEC_ENTRY ntlm_QueryCredentialsAttributesA(PCredHandle phCredential, ULONG ulAttribute, void* pBuffer)
{
	return ntlm_QueryCredentialsAttributesW(phCredential, ulAttribute, pBuffer);
}

/**
 * @see http://msdn.microsoft.com/en-us/library/windows/desktop/aa374707
 */
SECURITY_STATUS SEC_ENTRY ntlm_AcceptSecurityContext(PCredHandle phCredential, PCtxtHandle phContext,
		PSecBufferDesc pInput, ULONG fContextReq, ULONG TargetDataRep, PCtxtHandle phNewContext,
		PSecBufferDesc pOutput, PULONG pfContextAttr, PTimeStamp ptsTimeStamp)
{
	NTLM_CONTEXT* context;
	SECURITY_STATUS status;
	SSPI_CREDENTIALS* credentials;
	PSecBuffer input_buffer;
	PSecBuffer output_buffer;

	context = (NTLM_CONTEXT*) sspi_SecureHandleGetLowerPointer(phContext);

	if (!context)
	{
		context = ntlm_ContextNew();

		if (!context)
			return SEC_E_INSUFFICIENT_MEMORY;

		context->server = TRUE;

		if (fContextReq & ASC_REQ_CONFIDENTIALITY)
			context->confidentiality = TRUE;

		credentials = (SSPI_CREDENTIALS*) sspi_SecureHandleGetLowerPointer(phCredential);
		context->credentials = credentials;

		ntlm_SetContextTargetName(context, NULL);

		sspi_SecureHandleSetLowerPointer(phNewContext, context);
		sspi_SecureHandleSetUpperPointer(phNewContext, (void*) NTLM_PACKAGE_NAME);
	}

	if (context->state == NTLM_STATE_INITIAL)
	{
		context->state = NTLM_STATE_NEGOTIATE;

		if (!pInput)
			return SEC_E_INVALID_TOKEN;

		if (pInput->cBuffers < 1)
			return SEC_E_INVALID_TOKEN;

		input_buffer = sspi_FindSecBuffer(pInput, SECBUFFER_TOKEN);

		if (!input_buffer)
			return SEC_E_INVALID_TOKEN;

		if (input_buffer->cbBuffer < 1)
			return SEC_E_INVALID_TOKEN;

		status = ntlm_read_NegotiateMessage(context, input_buffer);

		if (context->state == NTLM_STATE_CHALLENGE)
		{
			if (!pOutput)
				return SEC_E_INVALID_TOKEN;

			if (pOutput->cBuffers < 1)
				return SEC_E_INVALID_TOKEN;

			output_buffer = sspi_FindSecBuffer(pOutput, SECBUFFER_TOKEN);

			if (!output_buffer->BufferType)
				return SEC_E_INVALID_TOKEN;

			if (output_buffer->cbBuffer < 1)
				return SEC_E_INSUFFICIENT_MEMORY;

			return ntlm_write_ChallengeMessage(context, output_buffer);
		}

		return SEC_E_OUT_OF_SEQUENCE;
	}
	else if (context->state == NTLM_STATE_AUTHENTICATE)
	{
		if (!pInput)
			return SEC_E_INVALID_TOKEN;

		if (pInput->cBuffers < 1)
			return SEC_E_INVALID_TOKEN;

		input_buffer = sspi_FindSecBuffer(pInput, SECBUFFER_TOKEN);

		if (!input_buffer)
			return SEC_E_INVALID_TOKEN;

		if (input_buffer->cbBuffer < 1)
			return SEC_E_INVALID_TOKEN;

		status = ntlm_read_AuthenticateMessage(context, input_buffer);

		if (pOutput)
		{
			ULONG i;

			for (i = 0; i < pOutput->cBuffers; i++)
			{
				pOutput->pBuffers[i].cbBuffer = 0;
				pOutput->pBuffers[i].BufferType = SECBUFFER_TOKEN;
			}
		}

		return status;
	}

	return SEC_E_OUT_OF_SEQUENCE;
}

SECURITY_STATUS SEC_ENTRY ntlm_ImpersonateSecurityContext(PCtxtHandle phContext)
{
	return SEC_E_OK;
}

SECURITY_STATUS SEC_ENTRY ntlm_InitializeSecurityContextW(PCredHandle phCredential, PCtxtHandle phContext,
		SEC_WCHAR* pszTargetName, ULONG fContextReq, ULONG Reserved1, ULONG TargetDataRep,
		PSecBufferDesc pInput, ULONG Reserved2, PCtxtHandle phNewContext,
		PSecBufferDesc pOutput, PULONG pfContextAttr, PTimeStamp ptsExpiry)
{
	NTLM_CONTEXT* context;
	SECURITY_STATUS status;
	SSPI_CREDENTIALS* credentials;
	PSecBuffer input_buffer = NULL;
	PSecBuffer output_buffer = NULL;
	PSecBuffer channel_bindings = NULL;

	context = (NTLM_CONTEXT*) sspi_SecureHandleGetLowerPointer(phContext);

	if (!context)
	{
		context = ntlm_ContextNew();

		if (!context)
			return SEC_E_INSUFFICIENT_MEMORY;

		if (fContextReq & ISC_REQ_CONFIDENTIALITY)
			context->confidentiality = TRUE;

		credentials = (SSPI_CREDENTIALS*) sspi_SecureHandleGetLowerPointer(phCredential);
		context->credentials = credentials;

		if (context->Workstation.Length < 1)
		{
			if (ntlm_SetContextWorkstation(context, NULL) < 0)
				return SEC_E_INTERNAL_ERROR;
		}

		if (ntlm_SetContextServicePrincipalNameW(context, pszTargetName) < 0)
			return SEC_E_INTERNAL_ERROR;

		sspi_SecureHandleSetLowerPointer(phNewContext, context);
		sspi_SecureHandleSetUpperPointer(phNewContext, (void*) NTLM_PACKAGE_NAME);
	}

	if ((!pInput) || (context->state == NTLM_STATE_AUTHENTICATE))
	{
		if (!pOutput)
			return SEC_E_INVALID_TOKEN;

		if (pOutput->cBuffers < 1)
			return SEC_E_INVALID_TOKEN;

		output_buffer = sspi_FindSecBuffer(pOutput, SECBUFFER_TOKEN);

		if (!output_buffer)
			return SEC_E_INVALID_TOKEN;

		if (output_buffer->cbBuffer < 1)
			return SEC_E_INVALID_TOKEN;

		if (context->state == NTLM_STATE_INITIAL)
			context->state = NTLM_STATE_NEGOTIATE;

		if (context->state == NTLM_STATE_NEGOTIATE)
			return ntlm_write_NegotiateMessage(context, output_buffer);

		return SEC_E_OUT_OF_SEQUENCE;
	}
	else
	{
		if (pInput->cBuffers < 1)
			return SEC_E_INVALID_TOKEN;

		input_buffer = sspi_FindSecBuffer(pInput, SECBUFFER_TOKEN);

		if (!input_buffer)
			return SEC_E_INVALID_TOKEN;

		if (input_buffer->cbBuffer < 1)
			return SEC_E_INVALID_TOKEN;

		channel_bindings = sspi_FindSecBuffer(pInput, SECBUFFER_CHANNEL_BINDINGS);

		if (channel_bindings)
		{
			context->Bindings.BindingsLength = channel_bindings->cbBuffer;
			context->Bindings.Bindings = (SEC_CHANNEL_BINDINGS*) channel_bindings->pvBuffer;
		}

		if (context->state == NTLM_STATE_CHALLENGE)
		{
			status = ntlm_read_ChallengeMessage(context, input_buffer);

			if (!pOutput)
				return SEC_E_INVALID_TOKEN;

			if (pOutput->cBuffers < 1)
				return SEC_E_INVALID_TOKEN;

			output_buffer = sspi_FindSecBuffer(pOutput, SECBUFFER_TOKEN);

			if (!output_buffer)
				return SEC_E_INVALID_TOKEN;

			if (output_buffer->cbBuffer < 1)
				return SEC_E_INSUFFICIENT_MEMORY;

			if (context->state == NTLM_STATE_AUTHENTICATE)
				return ntlm_write_AuthenticateMessage(context, output_buffer);
		}

		return SEC_E_OUT_OF_SEQUENCE;
	}

	return SEC_E_OUT_OF_SEQUENCE;
}

/**
 * @see http://msdn.microsoft.com/en-us/library/windows/desktop/aa375512%28v=vs.85%29.aspx
 */
SECURITY_STATUS SEC_ENTRY ntlm_InitializeSecurityContextA(PCredHandle phCredential, PCtxtHandle phContext,
		SEC_CHAR* pszTargetName, ULONG fContextReq, ULONG Reserved1, ULONG TargetDataRep,
		PSecBufferDesc pInput, ULONG Reserved2, PCtxtHandle phNewContext,
		PSecBufferDesc pOutput, PULONG pfContextAttr, PTimeStamp ptsExpiry)
{
	SECURITY_STATUS status;
	SEC_WCHAR* pszTargetNameW = NULL;

	if (pszTargetName)
	{
		if (ConvertToUnicode(CP_UTF8, 0, pszTargetName, -1, &pszTargetNameW, 0) <= 0)
			return SEC_E_INTERNAL_ERROR;
	}

	status = ntlm_InitializeSecurityContextW(phCredential, phContext, pszTargetNameW, fContextReq,
		Reserved1, TargetDataRep, pInput, Reserved2, phNewContext, pOutput, pfContextAttr, ptsExpiry);

	if (pszTargetNameW)
		free(pszTargetNameW);
	
	return status;
}

SECURITY_STATUS SEC_ENTRY ntlm_CompleteAuthToken(PCtxtHandle phContext, PSecBufferDesc pToken)
{
	NTLM_CONTEXT* context;
	SECURITY_STATUS status = SEC_E_OK;

	context = (NTLM_CONTEXT*) sspi_SecureHandleGetLowerPointer(phContext);

	if (!context)
		return SEC_E_INVALID_HANDLE;

	if (context->server)
	{
		status = ntlm_server_AuthenticateComplete(context);
	}

	return status;
}

/* http://msdn.microsoft.com/en-us/library/windows/desktop/aa375354 */

SECURITY_STATUS SEC_ENTRY ntlm_DeleteSecurityContext(PCtxtHandle phContext)
{
	NTLM_CONTEXT* context;

	context = (NTLM_CONTEXT*) sspi_SecureHandleGetLowerPointer(phContext);

	if (!context)
		return SEC_E_INVALID_HANDLE;

	ntlm_ContextFree(context);

	return SEC_E_OK;
}

/* http://msdn.microsoft.com/en-us/library/windows/desktop/aa379337/ */

SECURITY_STATUS SEC_ENTRY ntlm_QueryContextAttributesW(PCtxtHandle phContext, ULONG ulAttribute, void* pBuffer)
{
	NTLM_CONTEXT* context;

	if (!phContext)
		return SEC_E_INVALID_HANDLE;

	if (!pBuffer)
		return SEC_E_INSUFFICIENT_MEMORY;

	context = (NTLM_CONTEXT*) sspi_SecureHandleGetLowerPointer(phContext);

	if (ulAttribute == SECPKG_ATTR_SIZES)
	{
		SecPkgContext_Sizes* ContextSizes = (SecPkgContext_Sizes*) pBuffer;

		ContextSizes->cbMaxToken = 2010;
		ContextSizes->cbMaxSignature = 16;
		ContextSizes->cbBlockSize = 0;
		ContextSizes->cbSecurityTrailer = 16;

		return SEC_E_OK;
	}
	else if (ulAttribute == SECPKG_ATTR_AUTH_IDENTITY)
	{
		int status;
		char* UserA = NULL;
		char* DomainA = NULL;
		SSPI_CREDENTIALS* credentials;
		SecPkgContext_AuthIdentity* AuthIdentity = (SecPkgContext_AuthIdentity*) pBuffer;

		credentials = context->credentials;
		ZeroMemory(AuthIdentity, sizeof(SecPkgContext_AuthIdentity));

		UserA = AuthIdentity->User;
		status = ConvertFromUnicode(CP_UTF8, 0, (WCHAR*) credentials->identity.User,
				credentials->identity.UserLength,
				&UserA, 256, NULL, NULL);

		if (status <= 0)
			return SEC_E_INTERNAL_ERROR;

		DomainA = AuthIdentity->Domain;
		status = ConvertFromUnicode(CP_UTF8, 0, (WCHAR*) credentials->identity.Domain,
				credentials->identity.DomainLength,
				&DomainA, 256, NULL, NULL);

		if (status <= 0)
			return SEC_E_INTERNAL_ERROR;

		return SEC_E_OK;
	}

	return SEC_E_UNSUPPORTED_FUNCTION;
}

SECURITY_STATUS SEC_ENTRY ntlm_QueryContextAttributesA(PCtxtHandle phContext, ULONG ulAttribute, void* pBuffer)
{
	return ntlm_QueryContextAttributesW(phContext, ulAttribute, pBuffer);
}

SECURITY_STATUS SEC_ENTRY ntlm_SetContextAttributesW(PCtxtHandle phContext, ULONG ulAttribute, void* pBuffer, ULONG cbBuffer)
{
	NTLM_CONTEXT* context;

	if (!phContext)
		return SEC_E_INVALID_HANDLE;

	if (!pBuffer)
		return SEC_E_INVALID_PARAMETER;

	context = (NTLM_CONTEXT*) sspi_SecureHandleGetLowerPointer(phContext);

	if (ulAttribute == SECPKG_ATTR_AUTH_NTLM_HASH)
	{
		SecPkgContext_AuthNtlmHash* AuthNtlmHash = (SecPkgContext_AuthNtlmHash*) pBuffer;

		if (cbBuffer < sizeof(SecPkgContext_AuthNtlmHash))
			return SEC_E_INVALID_PARAMETER;

		CopyMemory(context->NtlmHash, AuthNtlmHash->NtlmHash, 16);

		return SEC_E_OK;
	}
	else if (ulAttribute == SECPKG_ATTR_AUTH_NTLM_MESSAGE)
	{
		SecPkgContext_AuthNtlmMessage* AuthNtlmMessage = (SecPkgContext_AuthNtlmMessage*) pBuffer;

		if (cbBuffer < sizeof(SecPkgContext_AuthNtlmMessage))
			return SEC_E_INVALID_PARAMETER;

		if (AuthNtlmMessage->type == 1)
		{
			sspi_SecBufferFree(&context->NegotiateMessage);
			sspi_SecBufferAlloc(&context->NegotiateMessage, AuthNtlmMessage->length);
			CopyMemory(context->NegotiateMessage.pvBuffer, AuthNtlmMessage->buffer, AuthNtlmMessage->length);
		}
		else if (AuthNtlmMessage->type == 2)
		{
			sspi_SecBufferFree(&context->ChallengeMessage);
			sspi_SecBufferAlloc(&context->ChallengeMessage, AuthNtlmMessage->length);
			CopyMemory(context->ChallengeMessage.pvBuffer, AuthNtlmMessage->buffer, AuthNtlmMessage->length);
		}
		else if (AuthNtlmMessage->type == 3)
		{
			sspi_SecBufferFree(&context->AuthenticateMessage);
			sspi_SecBufferAlloc(&context->AuthenticateMessage, AuthNtlmMessage->length);
			CopyMemory(context->AuthenticateMessage.pvBuffer, AuthNtlmMessage->buffer, AuthNtlmMessage->length);
		}

		return SEC_E_OK;
	}
	else if (ulAttribute == SECPKG_ATTR_AUTH_NTLM_TIMESTAMP)
	{
		SecPkgContext_AuthNtlmTimestamp* AuthNtlmTimestamp = (SecPkgContext_AuthNtlmTimestamp*) pBuffer;

		if (cbBuffer < sizeof(SecPkgContext_AuthNtlmTimestamp))
			return SEC_E_INVALID_PARAMETER;

		if (AuthNtlmTimestamp->ChallengeOrResponse)
			CopyMemory(context->ChallengeTimestamp, AuthNtlmTimestamp->Timestamp, 8);
		else
			CopyMemory(context->Timestamp, AuthNtlmTimestamp->Timestamp, 8);

		return SEC_E_OK;
	}
	else if (ulAttribute == SECPKG_ATTR_AUTH_NTLM_CLIENT_CHALLENGE)
	{
		SecPkgContext_AuthNtlmClientChallenge* AuthNtlmClientChallenge = (SecPkgContext_AuthNtlmClientChallenge*) pBuffer;

		if (cbBuffer < sizeof(SecPkgContext_AuthNtlmClientChallenge))
			return SEC_E_INVALID_PARAMETER;

		CopyMemory(context->ClientChallenge, AuthNtlmClientChallenge->ClientChallenge, 8);

		return SEC_E_OK;
	}
	else if (ulAttribute == SECPKG_ATTR_AUTH_NTLM_SERVER_CHALLENGE)
	{
		SecPkgContext_AuthNtlmServerChallenge* AuthNtlmServerChallenge = (SecPkgContext_AuthNtlmServerChallenge*) pBuffer;

		if (cbBuffer < sizeof(SecPkgContext_AuthNtlmServerChallenge))
			return SEC_E_INVALID_PARAMETER;

		CopyMemory(context->ServerChallenge, AuthNtlmServerChallenge->ServerChallenge, 8);

		return SEC_E_OK;
	}

	return SEC_E_UNSUPPORTED_FUNCTION;
}

SECURITY_STATUS SEC_ENTRY ntlm_SetContextAttributesA(PCtxtHandle phContext, ULONG ulAttribute, void* pBuffer, ULONG cbBuffer)
{
	return ntlm_SetContextAttributesW(phContext, ulAttribute, pBuffer, cbBuffer);
}

SECURITY_STATUS SEC_ENTRY ntlm_RevertSecurityContext(PCtxtHandle phContext)
{
	return SEC_E_OK;
}

SECURITY_STATUS SEC_ENTRY ntlm_EncryptMessage(PCtxtHandle phContext, ULONG fQOP, PSecBufferDesc pMessage, ULONG MessageSeqNo)
{
	int index;
	int length;
	void* data;
	UINT32 SeqNo;
	HMAC_CTX hmac;
	BYTE digest[16];
	BYTE checksum[8];
	BYTE* signature;
	ULONG version = 1;
	NTLM_CONTEXT* context;
	PSecBuffer data_buffer = NULL;
	PSecBuffer signature_buffer = NULL;

	SeqNo = MessageSeqNo;
	context = (NTLM_CONTEXT*) sspi_SecureHandleGetLowerPointer(phContext);

	for (index = 0; index < (int) pMessage->cBuffers; index++)
	{
		if (pMessage->pBuffers[index].BufferType == SECBUFFER_DATA)
			data_buffer = &pMessage->pBuffers[index];
		else if (pMessage->pBuffers[index].BufferType == SECBUFFER_TOKEN)
			signature_buffer = &pMessage->pBuffers[index];
	}

	if (!data_buffer)
		return SEC_E_INVALID_TOKEN;

	if (!signature_buffer)
		return SEC_E_INVALID_TOKEN;

	/* Copy original data buffer */
	length = data_buffer->cbBuffer;
	data = malloc(length);

	if (!data)
		return SEC_E_INSUFFICIENT_MEMORY;

	CopyMemory(data, data_buffer->pvBuffer, length);

	/* Compute the HMAC-MD5 hash of ConcatenationOf(seq_num,data) using the client signing key */
	HMAC_CTX_init(&hmac);
	HMAC_Init_ex(&hmac, context->SendSigningKey, 16, EVP_md5(), NULL);
	HMAC_Update(&hmac, (void*) &(SeqNo), 4);
	HMAC_Update(&hmac, (void*) data, length);
	HMAC_Final(&hmac, digest, NULL);
	HMAC_CTX_cleanup(&hmac);

	/* Encrypt message using with RC4, result overwrites original buffer */

	if (context->confidentiality)
		RC4(&context->SendRc4Seal, length, (BYTE*) data, (BYTE*) data_buffer->pvBuffer);
	else
		CopyMemory(data_buffer->pvBuffer, data, length);

#ifdef WITH_DEBUG_NTLM
	fprintf(stderr, "Data Buffer (length = %d)\n", length);
	winpr_HexDump(data, length);
	fprintf(stderr, "\n");

	fprintf(stderr, "Encrypted Data Buffer (length = %d)\n", (int) data_buffer->cbBuffer);
	winpr_HexDump(data_buffer->pvBuffer, data_buffer->cbBuffer);
	fprintf(stderr, "\n");
#endif

	free(data);

	/* RC4-encrypt first 8 bytes of digest */
	RC4(&context->SendRc4Seal, 8, digest, checksum);

	signature = (BYTE*) signature_buffer->pvBuffer;

	/* Concatenate version, ciphertext and sequence number to build signature */
	CopyMemory(signature, (void*) &version, 4);
	CopyMemory(&signature[4], (void*) checksum, 8);
	CopyMemory(&signature[12], (void*) &(SeqNo), 4);
	context->SendSeqNum++;

#ifdef WITH_DEBUG_NTLM
	fprintf(stderr, "Signature (length = %d)\n", (int) signature_buffer->cbBuffer);
	winpr_HexDump(signature_buffer->pvBuffer, signature_buffer->cbBuffer);
	fprintf(stderr, "\n");
#endif

	return SEC_E_OK;
}

SECURITY_STATUS SEC_ENTRY ntlm_DecryptMessage(PCtxtHandle phContext, PSecBufferDesc pMessage, ULONG MessageSeqNo, PULONG pfQOP)
{
	int index;
	int length;
	void* data;
	UINT32 SeqNo;
	HMAC_CTX hmac;
	BYTE digest[16];
	BYTE checksum[8];
	UINT32 version = 1;
	NTLM_CONTEXT* context;
	BYTE expected_signature[16];
	PSecBuffer data_buffer = NULL;
	PSecBuffer signature_buffer = NULL;

	SeqNo = (UINT32) MessageSeqNo;
	context = (NTLM_CONTEXT*) sspi_SecureHandleGetLowerPointer(phContext);

	for (index = 0; index < (int) pMessage->cBuffers; index++)
	{
		if (pMessage->pBuffers[index].BufferType == SECBUFFER_DATA)
			data_buffer = &pMessage->pBuffers[index];
		else if (pMessage->pBuffers[index].BufferType == SECBUFFER_TOKEN)
			signature_buffer = &pMessage->pBuffers[index];
	}

	if (!data_buffer)
		return SEC_E_INVALID_TOKEN;

	if (!signature_buffer)
		return SEC_E_INVALID_TOKEN;

	/* Copy original data buffer */
	length = data_buffer->cbBuffer;
	data = malloc(length);

	if (!data)
		return SEC_E_INSUFFICIENT_MEMORY;

	CopyMemory(data, data_buffer->pvBuffer, length);

	/* Decrypt message using with RC4, result overwrites original buffer */

	if (context->confidentiality)
		RC4(&context->RecvRc4Seal, length, (BYTE*) data, (BYTE*) data_buffer->pvBuffer);
	else
		CopyMemory(data_buffer->pvBuffer, data, length);

	/* Compute the HMAC-MD5 hash of ConcatenationOf(seq_num,data) using the client signing key */
	HMAC_CTX_init(&hmac);
	HMAC_Init_ex(&hmac, context->RecvSigningKey, 16, EVP_md5(), NULL);
	HMAC_Update(&hmac, (void*) &(SeqNo), 4);
	HMAC_Update(&hmac, (void*) data_buffer->pvBuffer, data_buffer->cbBuffer);
	HMAC_Final(&hmac, digest, NULL);
	HMAC_CTX_cleanup(&hmac);

#ifdef WITH_DEBUG_NTLM
	fprintf(stderr, "Encrypted Data Buffer (length = %d)\n", length);
	winpr_HexDump(data, length);
	fprintf(stderr, "\n");

	fprintf(stderr, "Data Buffer (length = %d)\n", (int) data_buffer->cbBuffer);
	winpr_HexDump(data_buffer->pvBuffer, data_buffer->cbBuffer);
	fprintf(stderr, "\n");
#endif

	free(data);

	/* RC4-encrypt first 8 bytes of digest */
	RC4(&context->RecvRc4Seal, 8, digest, checksum);

	/* Concatenate version, ciphertext and sequence number to build signature */
	CopyMemory(expected_signature, (void*) &version, 4);
	CopyMemory(&expected_signature[4], (void*) checksum, 8);
	CopyMemory(&expected_signature[12], (void*) &(SeqNo), 4);
	context->RecvSeqNum++;

	if (memcmp(signature_buffer->pvBuffer, expected_signature, 16) != 0)
	{
		/* signature verification failed! */
		fprintf(stderr, "signature verification failed, something nasty is going on!\n");

		fprintf(stderr, "Expected Signature:\n");
		winpr_HexDump(expected_signature, 16);
		fprintf(stderr, "Actual Signature:\n");
		winpr_HexDump((BYTE*) signature_buffer->pvBuffer, 16);

		return SEC_E_MESSAGE_ALTERED;
	}

	return SEC_E_OK;
}

SECURITY_STATUS SEC_ENTRY ntlm_MakeSignature(PCtxtHandle phContext, ULONG fQOP, PSecBufferDesc pMessage, ULONG MessageSeqNo)
{
	return SEC_E_UNSUPPORTED_FUNCTION;
}

SECURITY_STATUS SEC_ENTRY ntlm_VerifySignature(PCtxtHandle phContext, PSecBufferDesc pMessage, ULONG MessageSeqNo, PULONG pfQOP)
{
	return SEC_E_UNSUPPORTED_FUNCTION;
}

const SecurityFunctionTableA NTLM_SecurityFunctionTableA =
{
	1, /* dwVersion */
	NULL, /* EnumerateSecurityPackages */
	ntlm_QueryCredentialsAttributesA, /* QueryCredentialsAttributes */
	ntlm_AcquireCredentialsHandleA, /* AcquireCredentialsHandle */
	ntlm_FreeCredentialsHandle, /* FreeCredentialsHandle */
	NULL, /* Reserved2 */
	ntlm_InitializeSecurityContextA, /* InitializeSecurityContext */
	ntlm_AcceptSecurityContext, /* AcceptSecurityContext */
	ntlm_CompleteAuthToken, /* CompleteAuthToken */
	ntlm_DeleteSecurityContext, /* DeleteSecurityContext */
	NULL, /* ApplyControlToken */
	ntlm_QueryContextAttributesA, /* QueryContextAttributes */
	ntlm_ImpersonateSecurityContext, /* ImpersonateSecurityContext */
	ntlm_RevertSecurityContext, /* RevertSecurityContext */
	ntlm_MakeSignature, /* MakeSignature */
	ntlm_VerifySignature, /* VerifySignature */
	NULL, /* FreeContextBuffer */
	NULL, /* QuerySecurityPackageInfo */
	NULL, /* Reserved3 */
	NULL, /* Reserved4 */
	NULL, /* ExportSecurityContext */
	NULL, /* ImportSecurityContext */
	NULL, /* AddCredentials */
	NULL, /* Reserved8 */
	NULL, /* QuerySecurityContextToken */
	ntlm_EncryptMessage, /* EncryptMessage */
	ntlm_DecryptMessage, /* DecryptMessage */
	ntlm_SetContextAttributesA, /* SetContextAttributes */
};

const SecurityFunctionTableW NTLM_SecurityFunctionTableW =
{
	1, /* dwVersion */
	NULL, /* EnumerateSecurityPackages */
	ntlm_QueryCredentialsAttributesW, /* QueryCredentialsAttributes */
	ntlm_AcquireCredentialsHandleW, /* AcquireCredentialsHandle */
	ntlm_FreeCredentialsHandle, /* FreeCredentialsHandle */
	NULL, /* Reserved2 */
	ntlm_InitializeSecurityContextW, /* InitializeSecurityContext */
	ntlm_AcceptSecurityContext, /* AcceptSecurityContext */
	ntlm_CompleteAuthToken, /* CompleteAuthToken */
	ntlm_DeleteSecurityContext, /* DeleteSecurityContext */
	NULL, /* ApplyControlToken */
	ntlm_QueryContextAttributesW, /* QueryContextAttributes */
	ntlm_ImpersonateSecurityContext, /* ImpersonateSecurityContext */
	ntlm_RevertSecurityContext, /* RevertSecurityContext */
	ntlm_MakeSignature, /* MakeSignature */
	ntlm_VerifySignature, /* VerifySignature */
	NULL, /* FreeContextBuffer */
	NULL, /* QuerySecurityPackageInfo */
	NULL, /* Reserved3 */
	NULL, /* Reserved4 */
	NULL, /* ExportSecurityContext */
	NULL, /* ImportSecurityContext */
	NULL, /* AddCredentials */
	NULL, /* Reserved8 */
	NULL, /* QuerySecurityContextToken */
	ntlm_EncryptMessage, /* EncryptMessage */
	ntlm_DecryptMessage, /* DecryptMessage */
	ntlm_SetContextAttributesA, /* SetContextAttributes */
};

const SecPkgInfoA NTLM_SecPkgInfoA =
{
	0x00082B37, /* fCapabilities */
	1, /* wVersion */
	0x000A, /* wRPCID */
	0x00000B48, /* cbMaxToken */
	"NTLM", /* Name */
	"NTLM Security Package" /* Comment */
};

WCHAR NTLM_SecPkgInfoW_Name[] = { 'N','T','L','M','\0' };

WCHAR NTLM_SecPkgInfoW_Comment[] =
{
	'N','T','L','M',' ',
	'S','e','c','u','r','i','t','y',' ',
	'P','a','c','k','a','g','e','\0'
};

const SecPkgInfoW NTLM_SecPkgInfoW =
{
	0x00082B37, /* fCapabilities */
	1, /* wVersion */
	0x000A, /* wRPCID */
	0x00000B48, /* cbMaxToken */
	NTLM_SecPkgInfoW_Name, /* Name */
	NTLM_SecPkgInfoW_Comment /* Comment */
};
