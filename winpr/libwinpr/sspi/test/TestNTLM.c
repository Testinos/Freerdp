
#include <winpr/crt.h>
#include <winpr/sspi.h>
#include <winpr/print.h>

#define TEST_SSPI_INTERFACE SSPI_INTERFACE_WINPR

static const char* TEST_NTLM_USER = "Username";
static const char* TEST_NTLM_DOMAIN = "Domain";
static const char* TEST_NTLM_PASSWORD = "P4ss123!";

static const char* TEST_NTLM_HASH_STRING = "d5922a65c4d5c082ca444af1be0001db";

static const BYTE TEST_NTLM_HASH[16] =
	{ 0xd5, 0x92, 0x2a, 0x65, 0xc4, 0xd5, 0xc0, 0x82, 0xca, 0x44, 0x4a, 0xf1, 0xbe, 0x00, 0x01, 0xdb };

struct _TEST_NTLM_CLIENT
{
	CtxtHandle context;
	ULONG cbMaxToken;
	ULONG fContextReq;
	ULONG pfContextAttr;
	TimeStamp expiration;
	PSecBuffer pBuffer;
	SecBuffer inputBuffer[2];
	SecBuffer outputBuffer[2];
	BOOL haveContext;
	BOOL haveInputBuffer;
	LPTSTR ServicePrincipalName;
	SecBufferDesc inputBufferDesc;
	SecBufferDesc outputBufferDesc;
	CredHandle credentials;
	BOOL confidentiality;
	SecPkgInfo* pPackageInfo;
	SecurityFunctionTable* table;
	SEC_WINNT_AUTH_IDENTITY identity;
	SecPkgContext_Sizes ContextSizes;
};
typedef struct _TEST_NTLM_CLIENT TEST_NTLM_CLIENT;

int test_ntlm_client_init(TEST_NTLM_CLIENT* ntlm, const char* user, const char* domain, const char* password)
{
	SECURITY_STATUS status;

	SecInvalidateHandle(&(ntlm->context));

	ntlm->table = InitSecurityInterfaceEx(TEST_SSPI_INTERFACE);

	sspi_SetAuthIdentity(&(ntlm->identity), user, domain, password);

	status = ntlm->table->QuerySecurityPackageInfo(NTLMSP_NAME, &ntlm->pPackageInfo);

	if (status != SEC_E_OK)
	{
		fprintf(stderr, "QuerySecurityPackageInfo status: %s (0x%04X)\n",
				GetSecurityStatusString(status), status);
		return -1;
	}

	ntlm->cbMaxToken = ntlm->pPackageInfo->cbMaxToken;

	status = ntlm->table->AcquireCredentialsHandle(NULL, NTLMSP_NAME,
			SECPKG_CRED_OUTBOUND, NULL, &ntlm->identity, NULL, NULL, &ntlm->credentials, &ntlm->expiration);

	if (status != SEC_E_OK)
	{
		fprintf(stderr, "AcquireCredentialsHandle status: %s (0x%04X)\n",
				GetSecurityStatusString(status), status);
		return -1;
	}

	ntlm->haveContext = FALSE;
	ntlm->haveInputBuffer = FALSE;
	ZeroMemory(&ntlm->inputBuffer, sizeof(SecBuffer));
	ZeroMemory(&ntlm->outputBuffer, sizeof(SecBuffer));
	ZeroMemory(&ntlm->ContextSizes, sizeof(SecPkgContext_Sizes));

	ntlm->fContextReq = 0;

#if 0
	/* HTTP authentication flags */
	ntlm->fContextReq |= ISC_REQ_CONFIDENTIALITY;
#endif

	/* NLA authentication flags */
	ntlm->fContextReq |= ISC_REQ_MUTUAL_AUTH;
	ntlm->fContextReq |= ISC_REQ_CONFIDENTIALITY;
	ntlm->fContextReq |= ISC_REQ_USE_SESSION_KEY;

	return 1;
}

void test_ntlm_client_uninit(TEST_NTLM_CLIENT* ntlm)
{
	if (!ntlm)
		return;

	free(ntlm->identity.User);
	free(ntlm->identity.Domain);
	free(ntlm->identity.Password);
	free(ntlm->ServicePrincipalName);

	if (ntlm->table)
	{
		ntlm->table->FreeCredentialsHandle(&ntlm->credentials);
		ntlm->table->FreeContextBuffer(ntlm->pPackageInfo);
		ntlm->table->DeleteSecurityContext(&ntlm->context);
	}
}

/**
 *                                        SSPI Client Ceremony
 *
 *                                           --------------
 *                                          ( Client Begin )
 *                                           --------------
 *                                                 |
 *                                                 |
 *                                                \|/
 *                                      -----------+--------------
 *                                     | AcquireCredentialsHandle |
 *                                      --------------------------
 *                                                 |
 *                                                 |
 *                                                \|/
 *                                    -------------+--------------
 *                 +---------------> / InitializeSecurityContext /
 *                 |                 ----------------------------
 *                 |                               |
 *                 |                               |
 *                 |                              \|/
 *     ---------------------------        ---------+-------------            ----------------------
 *    / Receive blob from server /      < Received security blob? > --Yes-> / Send blob to server /
 *    -------------+-------------         -----------------------           ----------------------
 *                /|\                              |                                |
 *                 |                               No                               |
 *                Yes                             \|/                               |
 *                 |                   ------------+-----------                     |
 *                 +---------------- < Received Continue Needed > <-----------------+
 *                                     ------------------------
 *                                                 |
 *                                                 No
 *                                                \|/
 *                                           ------+-------
 *                                          (  Client End  )
 *                                           --------------
 */

int test_ntlm_client_authenticate(TEST_NTLM_CLIENT* ntlm)
{
	SECURITY_STATUS status;

	if (ntlm->outputBuffer[0].pvBuffer)
	{
		free(ntlm->outputBuffer[0].pvBuffer);
		ntlm->outputBuffer[0].pvBuffer = NULL;
	}

	ntlm->outputBufferDesc.ulVersion = SECBUFFER_VERSION;
	ntlm->outputBufferDesc.cBuffers = 1;
	ntlm->outputBufferDesc.pBuffers = ntlm->outputBuffer;
	ntlm->outputBuffer[0].BufferType = SECBUFFER_TOKEN;
	ntlm->outputBuffer[0].cbBuffer = ntlm->cbMaxToken;
	ntlm->outputBuffer[0].pvBuffer = malloc(ntlm->outputBuffer[0].cbBuffer);

	if (!ntlm->outputBuffer[0].pvBuffer)
		return -1;

	if (ntlm->haveInputBuffer)
	{
		ntlm->inputBufferDesc.ulVersion = SECBUFFER_VERSION;
		ntlm->inputBufferDesc.cBuffers = 1;
		ntlm->inputBufferDesc.pBuffers = ntlm->inputBuffer;
		ntlm->inputBuffer[0].BufferType = SECBUFFER_TOKEN;
	}

	if ((!ntlm) || (!ntlm->table))
	{
		fprintf(stderr, "ntlm_authenticate: invalid ntlm context\n");
		return -1;
	}

	status = ntlm->table->InitializeSecurityContext(&ntlm->credentials,
			(ntlm->haveContext) ? &ntlm->context : NULL,
			(ntlm->ServicePrincipalName) ? ntlm->ServicePrincipalName : NULL,
			ntlm->fContextReq, 0, SECURITY_NATIVE_DREP,
			(ntlm->haveInputBuffer) ? &ntlm->inputBufferDesc : NULL,
			0, &ntlm->context, &ntlm->outputBufferDesc,
			&ntlm->pfContextAttr, &ntlm->expiration);

	if ((status == SEC_I_COMPLETE_AND_CONTINUE) || (status == SEC_I_COMPLETE_NEEDED))
	{
		if (ntlm->table->CompleteAuthToken)
			ntlm->table->CompleteAuthToken(&ntlm->context, &ntlm->outputBufferDesc);

		if (ntlm->table->QueryContextAttributes(&ntlm->context, SECPKG_ATTR_SIZES, &ntlm->ContextSizes) != SEC_E_OK)
		{
			fprintf(stderr, "QueryContextAttributes SECPKG_ATTR_SIZES failure status: %s (0x%04X)\n",
				GetSecurityStatusString(status), status);
			return -1;
		}

		if (status == SEC_I_COMPLETE_NEEDED)
			status = SEC_E_OK;
		else if (status == SEC_I_COMPLETE_AND_CONTINUE)
			status = SEC_I_CONTINUE_NEEDED;
	}

	if (ntlm->haveInputBuffer)
	{
		free(ntlm->inputBuffer[0].pvBuffer);
	}

	ntlm->haveInputBuffer = TRUE;
	ntlm->haveContext = TRUE;

	return (status == SEC_I_CONTINUE_NEEDED) ? 1 : 0;
}

TEST_NTLM_CLIENT* test_ntlm_client_new()
{
	TEST_NTLM_CLIENT* ntlm;

	ntlm = (TEST_NTLM_CLIENT*) calloc(1, sizeof(TEST_NTLM_CLIENT));

	if (!ntlm)
		return NULL;

	return ntlm;
}

void test_ntlm_client_free(TEST_NTLM_CLIENT* ntlm)
{
	if (!ntlm)
		return;

	test_ntlm_client_uninit(ntlm);

	free(ntlm);
}

struct _TEST_NTLM_SERVER
{
	CtxtHandle context;
	ULONG cbMaxToken;
	ULONG fContextReq;
	ULONG pfContextAttr;
	TimeStamp expiration;
	PSecBuffer pBuffer;
	SecBuffer inputBuffer[2];
	SecBuffer outputBuffer[2];
	BOOL haveContext;
	BOOL haveInputBuffer;
	LPTSTR ServicePrincipalName;
	SecBufferDesc inputBufferDesc;
	SecBufferDesc outputBufferDesc;
	CredHandle credentials;
	BOOL confidentiality;
	SecPkgInfo* pPackageInfo;
	SecurityFunctionTable* table;
	SEC_WINNT_AUTH_IDENTITY identity;
	SecPkgContext_Sizes ContextSizes;
};
typedef struct _TEST_NTLM_SERVER TEST_NTLM_SERVER;

void SEC_ENTRY test_ntlm_server_get_key(void* pArg, void* pPrincipal, ULONG KeyVer, void** ppKey, SECURITY_STATUS* pStatus)
{
	char* User = NULL;
	char* Domain = NULL;
	TEST_NTLM_SERVER* ntlm;
	SEC_WINNT_AUTH_IDENTITY* identity;
	SECURITY_STATUS status = SEC_E_NO_CREDENTIALS;

	if (!pPrincipal || !ppKey)
	{
		*pStatus = SEC_E_INVALID_PARAMETER;
		return;
	}

	ntlm = (TEST_NTLM_SERVER*) pArg;
	identity = (SEC_WINNT_AUTH_IDENTITY*) *ppKey;

	if (!ntlm || !identity)
	{
		*pStatus = SEC_E_INVALID_PARAMETER;
		return;
	}

	if (strcmp((char*) pPrincipal, "NTLM") != 0)
	{
		*pStatus = SEC_E_UNSUPPORTED_FUNCTION;
		return;
	}

	ConvertFromUnicode(CP_UTF8, 0, (WCHAR*) identity->User, identity->UserLength, &User, 0, NULL, NULL);

	if (identity->Domain)
		ConvertFromUnicode(CP_UTF8, 0, (WCHAR*) identity->Domain, identity->DomainLength, &Domain, 0, NULL, NULL);

	if (KeyVer == 0) /* plaintext password */
	{
#if 0
		char* password;

		if (strcmp(User, TEST_NTLM_USER) == 0)
		{
			password = _strdup(TEST_NTLM_PASSWORD);
			*ppKey = (void*) password;
			status = SEC_E_OK;
		}
#endif
	}
	else if (KeyVer == 1) /* NTLMv1 Hash */
	{
		BYTE* hash;

		status = SEC_E_NO_CREDENTIALS;

		hash = (BYTE*) calloc(1, 16);

		if (!hash)
		{
			*pStatus = SEC_E_INTERNAL_ERROR;
			return;
		}

		if (strcmp(User, TEST_NTLM_USER) == 0)
		{
			CopyMemory(hash, TEST_NTLM_HASH, 16);
			*ppKey = (void*) hash;
			status = SEC_E_OK;
		}
	}
	else if (KeyVer == 2) /* NTLMv2 Hash */
	{
		status = SEC_E_NO_CREDENTIALS;
	}
	else
	{
		/* unknown */
		status = SEC_E_UNSUPPORTED_FUNCTION;
	}

	fprintf(stderr, "SecGetKey %s\\%s status: %s (0x%04X)\n",
				Domain, User, GetSecurityStatusString(status), status);

	*pStatus = status;
}

int test_ntlm_server_init(TEST_NTLM_SERVER* ntlm)
{
	SECURITY_STATUS status;

	SecInvalidateHandle(&(ntlm->context));

	ntlm->table = InitSecurityInterfaceEx(TEST_SSPI_INTERFACE);

	status = ntlm->table->QuerySecurityPackageInfo(NTLMSP_NAME, &ntlm->pPackageInfo);

	if (status != SEC_E_OK)
	{
		fprintf(stderr, "QuerySecurityPackageInfo status: %s (0x%04X)\n",
				GetSecurityStatusString(status), status);
		return -1;
	}

	ntlm->cbMaxToken = ntlm->pPackageInfo->cbMaxToken;

	status = ntlm->table->AcquireCredentialsHandle(NULL, NTLMSP_NAME,
			SECPKG_CRED_INBOUND, NULL, NULL,
			test_ntlm_server_get_key, (void*) ntlm,
			&ntlm->credentials, &ntlm->expiration);

	if (status != SEC_E_OK)
	{
		fprintf(stderr, "AcquireCredentialsHandle status: %s (0x%04X)\n",
				GetSecurityStatusString(status), status);
		return -1;
	}

	ntlm->haveContext = FALSE;
	ntlm->haveInputBuffer = FALSE;
	ZeroMemory(&ntlm->inputBuffer, sizeof(SecBuffer));
	ZeroMemory(&ntlm->outputBuffer, sizeof(SecBuffer));
	ZeroMemory(&ntlm->ContextSizes, sizeof(SecPkgContext_Sizes));

	ntlm->fContextReq = 0;

	/* NLA authentication flags */
	ntlm->fContextReq |= ASC_REQ_MUTUAL_AUTH;
	ntlm->fContextReq |= ASC_REQ_CONFIDENTIALITY;
	ntlm->fContextReq |= ASC_REQ_CONNECTION;
	ntlm->fContextReq |= ASC_REQ_USE_SESSION_KEY;
	ntlm->fContextReq |= ASC_REQ_REPLAY_DETECT;
	ntlm->fContextReq |= ASC_REQ_SEQUENCE_DETECT;
	ntlm->fContextReq |= ASC_REQ_EXTENDED_ERROR;

	return 1;
}

void test_ntlm_server_uninit(TEST_NTLM_SERVER* ntlm)
{
	if (!ntlm)
		return;

	free(ntlm->identity.User);
	free(ntlm->identity.Domain);
	free(ntlm->identity.Password);
	free(ntlm->ServicePrincipalName);

	if (ntlm->table)
	{
		ntlm->table->FreeCredentialsHandle(&ntlm->credentials);
		ntlm->table->FreeContextBuffer(ntlm->pPackageInfo);
		ntlm->table->DeleteSecurityContext(&ntlm->context);
	}
}

int test_ntlm_server_authenticate(TEST_NTLM_SERVER* ntlm)
{
	SECURITY_STATUS status;

	ntlm->inputBufferDesc.ulVersion = SECBUFFER_VERSION;
	ntlm->inputBufferDesc.cBuffers = 1;
	ntlm->inputBufferDesc.pBuffers = ntlm->inputBuffer;
	ntlm->inputBuffer[0].BufferType = SECBUFFER_TOKEN;

	ntlm->outputBufferDesc.ulVersion = SECBUFFER_VERSION;
	ntlm->outputBufferDesc.cBuffers = 1;
	ntlm->outputBufferDesc.pBuffers = &ntlm->outputBuffer[0];
	ntlm->outputBuffer[0].BufferType = SECBUFFER_TOKEN;
	ntlm->outputBuffer[0].cbBuffer = ntlm->cbMaxToken;
	ntlm->outputBuffer[0].pvBuffer = malloc(ntlm->outputBuffer[0].cbBuffer);

	status = ntlm->table->AcceptSecurityContext(&ntlm->credentials,
		ntlm->haveContext? &ntlm->context: NULL,
		&ntlm->inputBufferDesc, ntlm->fContextReq, SECURITY_NATIVE_DREP, &ntlm->context,
		&ntlm->outputBufferDesc, &ntlm->pfContextAttr, &ntlm->expiration);

	if ((status == SEC_I_COMPLETE_AND_CONTINUE) || (status == SEC_I_COMPLETE_NEEDED))
	{
		if (ntlm->table->CompleteAuthToken)
			ntlm->table->CompleteAuthToken(&ntlm->context, &ntlm->outputBufferDesc);

		if (status == SEC_I_COMPLETE_NEEDED)
			status = SEC_E_OK;
		else if (status == SEC_I_COMPLETE_AND_CONTINUE)
			status = SEC_I_CONTINUE_NEEDED;
	}

	if ((status != SEC_E_OK) && (status != SEC_I_CONTINUE_NEEDED))
	{
		fprintf(stderr, "AcceptSecurityContext status: %s (0x%04X)\n",
				GetSecurityStatusString(status), status);
		return -1; /* Access Denied */
	}

	ntlm->haveContext = TRUE;

	return (status == SEC_I_CONTINUE_NEEDED) ? 1 : 0;
}

TEST_NTLM_SERVER* test_ntlm_server_new()
{
	TEST_NTLM_SERVER* ntlm;

	ntlm = (TEST_NTLM_SERVER*) calloc(1, sizeof(TEST_NTLM_SERVER));

	if (!ntlm)
		return NULL;

	return ntlm;
}

void test_ntlm_server_free(TEST_NTLM_SERVER* ntlm)
{
	if (!ntlm)
		return;

	test_ntlm_server_uninit(ntlm);

	free(ntlm);
}

int TestNTLM(int argc, char* argv[])
{
	int status;
	PSecBuffer pSecBuffer;
	TEST_NTLM_CLIENT* client;
	TEST_NTLM_SERVER* server;

	/**
	 * Client Initialization
	 */

	client = test_ntlm_client_new();

	status = test_ntlm_client_init(client, TEST_NTLM_USER, TEST_NTLM_DOMAIN, TEST_NTLM_PASSWORD);

	if (status < 0)
	{
		printf("test_ntlm_client_init failure\n");
		return -1;
	}

	/**
	 * Server Initialization
	 */

	server = test_ntlm_server_new();

	status = test_ntlm_server_init(server);

	if (status < 0)
	{
		printf("test_ntlm_server_init failure\n");
		return -1;
	}

	/**
	 * Client -> Negotiate Message
	 */

	status = test_ntlm_client_authenticate(client);

	if (status < 0)
	{
		printf("test_ntlm_client_authenticate failure\n");
		return -1;
	}

	pSecBuffer = &(client->outputBuffer[0]);

	fprintf(stderr, "NTLM_NEGOTIATE (length = %d):\n", pSecBuffer->cbBuffer);
	winpr_HexDump((BYTE*) pSecBuffer->pvBuffer, pSecBuffer->cbBuffer);

	/**
	 * Server <- Negotiate Message
	 * Server -> Challenge Message
	 */

	server->haveInputBuffer = TRUE;
	server->inputBuffer[0].BufferType = SECBUFFER_TOKEN;
	server->inputBuffer[0].pvBuffer = pSecBuffer->pvBuffer;
	server->inputBuffer[0].cbBuffer = pSecBuffer->cbBuffer;

	status = test_ntlm_server_authenticate(server);

	if (status < 0)
	{
		printf("test_ntlm_server_authenticate failure\n");
		return -1;
	}

	pSecBuffer = &(server->outputBuffer[0]);

	fprintf(stderr, "NTLM_CHALLENGE (length = %d):\n", pSecBuffer->cbBuffer);
	winpr_HexDump((BYTE*) pSecBuffer->pvBuffer, pSecBuffer->cbBuffer);

	/**
	 * Client <- Challenge Message
	 * Client -> Authenticate Message
	 */

	client->haveInputBuffer = TRUE;
	client->inputBuffer[0].BufferType = SECBUFFER_TOKEN;
	client->inputBuffer[0].pvBuffer = pSecBuffer->pvBuffer;
	client->inputBuffer[0].cbBuffer = pSecBuffer->cbBuffer;

	status = test_ntlm_client_authenticate(client);

	if (status < 0)
	{
		printf("test_ntlm_client_authenticate failure\n");
		return -1;
	}

	pSecBuffer = &(client->outputBuffer[0]);

	fprintf(stderr, "NTLM_AUTHENTICATE (length = %d):\n", pSecBuffer->cbBuffer);
	winpr_HexDump((BYTE*) pSecBuffer->pvBuffer, pSecBuffer->cbBuffer);

	/**
	 * Server <- Authenticate Message
	 */

	server->haveInputBuffer = TRUE;
	server->inputBuffer[0].BufferType = SECBUFFER_TOKEN;
	server->inputBuffer[0].pvBuffer = pSecBuffer->pvBuffer;
	server->inputBuffer[0].cbBuffer = pSecBuffer->cbBuffer;

	status = test_ntlm_server_authenticate(server);

	if (status < 0)
	{
		printf("test_ntlm_server_authenticate failure\n");
		return -1;
	}

	/**
	 * Cleanup & Termination
	 */

	test_ntlm_client_free(client);
	test_ntlm_server_free(server);

	return 0;
}
