/**
 * WinPR: Windows Portable Runtime
 * makecert replacement
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <winpr/crt.h>
#include <winpr/file.h>
#include <winpr/path.h>
#include <winpr/cmdline.h>
#include <winpr/sysinfo.h>

#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>

#ifdef _WIN32
#include <openssl/applink.c>
#endif

#include "makecert.h"

struct _MAKECERT_CONTEXT
{
	int argc;
	char** argv;

	BIO* bio;
	RSA* rsa;
	X509* x509;
	EVP_PKEY* pkey;

	BOOL live;
	BOOL silent;

	char* output_file;
	char* default_name;
};

COMMAND_LINE_ARGUMENT_A args[] =
{
	/* Custom Options */

	{ "rdp", COMMAND_LINE_VALUE_FLAG, NULL, NULL, NULL, -1, NULL,
			"Generate certificate with required options for RDP usage."
	},
	{ "silent", COMMAND_LINE_VALUE_FLAG, NULL, NULL, NULL, -1, NULL,
			"Silently generate certificate without verbose output."
	},
	{ "live", COMMAND_LINE_VALUE_FLAG, NULL, NULL, NULL, -1, NULL,
			"Generate certificate live in memory when used as a library."
	},

	/* Basic Options */

	{ "n", COMMAND_LINE_VALUE_REQUIRED, "<name>", NULL, NULL, -1, NULL,
			"Specifies the subject's certificate name. This name must conform to the X.500 standard. "
			"The simplest method is to specify the name in double quotes, preceded by CN=; for example, -n \"CN=myName\"."
	},
	{ "pe", COMMAND_LINE_VALUE_FLAG, NULL, NULL, NULL, -1, NULL,
			"Marks the generated private key as exportable. This allows the private key to be included in the certificate."
	},
	{ "sk", COMMAND_LINE_VALUE_REQUIRED, "<keyname>", NULL, NULL, -1, NULL,
			"Specifies the subject's key container location, which contains the private key. "
			"If a key container does not exist, it will be created."
	},
	{ "sr", COMMAND_LINE_VALUE_REQUIRED, "<location>", NULL, NULL, -1, NULL,
			"Specifies the subject's certificate store location. location can be either currentuser (the default) or localmachine."
	},
	{ "ss", COMMAND_LINE_VALUE_REQUIRED, "<store>", NULL, NULL, -1, NULL,
			"Specifies the subject's certificate store name that stores the output certificate."
	},
	{ "#", COMMAND_LINE_VALUE_REQUIRED, "<number>", NULL, NULL, -1, NULL,
			"Specifies a serial number from 1 to 2,147,483,647. The default is a unique value generated by Makecert.exe."
	},
	{ "$", COMMAND_LINE_VALUE_REQUIRED, "<authority>", NULL, NULL, -1, NULL,
			"Specifies the signing authority of the certificate, which must be set to either commercial "
			"(for certificates used by commercial software publishers) or individual (for certificates used by individual software publishers)."
	},

	/* Extended Options */

	{ "a", COMMAND_LINE_VALUE_REQUIRED, "<algorithm>", NULL, NULL, -1, NULL,
			"Specifies the signature algorithm. algorithm must be md5, sha1 (the default), sha256, sha384, or sha512."
	},
	{ "b", COMMAND_LINE_VALUE_REQUIRED, "<mm/dd/yyyy>", NULL, NULL, -1, NULL,
			"Specifies the start of the validity period. Defaults to the current date."
	},
	{ "crl", COMMAND_LINE_VALUE_FLAG, NULL, NULL, NULL, -1, NULL,
			"Generates a certificate relocation list (CRL) instead of a certificate."
	},
	{ "cy", COMMAND_LINE_VALUE_REQUIRED, "<certType>", NULL, NULL, -1, NULL,
			"Specifies the certificate type. Valid values are end for end-entity and authority for certification authority."
	},
	{ "e", COMMAND_LINE_VALUE_REQUIRED, "<mm/dd/yyyy>", NULL, NULL, -1, NULL,
			"Specifies the end of the validity period. Defaults to 12/31/2039 11:59:59 GMT."
	},
	{ "eku", COMMAND_LINE_VALUE_REQUIRED, "<oid[,oid…]>", NULL, NULL, -1, NULL,
			"Inserts a list of comma-separated, enhanced key usage object identifiers (OIDs) into the certificate."
	},
	{ "h", COMMAND_LINE_VALUE_REQUIRED, "<number>", NULL, NULL, -1, NULL,
			"Specifies the maximum height of the tree below this certificate."
	},
	{ "ic", COMMAND_LINE_VALUE_REQUIRED, "<file>", NULL, NULL, -1, NULL,
			"Specifies the issuer's certificate file."
	},
	{ "ik", COMMAND_LINE_VALUE_REQUIRED, "<keyName>", NULL, NULL, -1, NULL,
			"Specifies the issuer's key container name."
	},
	{ "iky", COMMAND_LINE_VALUE_REQUIRED, "<keyType>", NULL, NULL, -1, NULL,
			"Specifies the issuer's key type, which must be one of the following: "
			"signature (which indicates that the key is used for a digital signature), "
			"exchange (which indicates that the key is used for key encryption and key exchange), "
			"or an integer that represents a provider type. "
			"By default, you can pass 1 for an exchange key or 2 for a signature key."
	},
	{ "in", COMMAND_LINE_VALUE_REQUIRED, "<name>", NULL, NULL, -1, NULL,
			"Specifies the issuer's certificate common name."
	},
	{ "ip", COMMAND_LINE_VALUE_REQUIRED, "<provider>", NULL, NULL, -1, NULL,
			"Specifies the issuer's CryptoAPI provider name. For information about the CryptoAPI provider name, see the –sp option."
	},
	{ "ir", COMMAND_LINE_VALUE_REQUIRED, "<location>", NULL, NULL, -1, NULL,
			"Specifies the location of the issuer's certificate store. location can be either currentuser (the default) or localmachine."
	},
	{ "is", COMMAND_LINE_VALUE_REQUIRED, "<store>", NULL, NULL, -1, NULL,
			"Specifies the issuer's certificate store name."
	},
	{ "iv", COMMAND_LINE_VALUE_REQUIRED, "<pvkFile>", NULL, NULL, -1, NULL,
			"Specifies the issuer's .pvk private key file."
	},
	{ "iy", COMMAND_LINE_VALUE_REQUIRED, "<type>", NULL, NULL, -1, NULL,
			"Specifies the issuer's CryptoAPI provider type. For information about the CryptoAPI provider type, see the –sy option."
	},
	{ "l", COMMAND_LINE_VALUE_REQUIRED, "<link>", NULL, NULL, -1, NULL,
			"Links to policy information (for example, to a URL)."
	},
	{ "len", COMMAND_LINE_VALUE_REQUIRED, "<number>", NULL, NULL, -1, NULL,
			"Specifies the generated key length, in bits."
	},
	{ "m", COMMAND_LINE_VALUE_REQUIRED, "<number>", NULL, NULL, -1, NULL,
			"Specifies the duration, in months, of the certificate validity period."
	},
	{ "nscp", COMMAND_LINE_VALUE_FLAG, NULL, NULL, NULL, -1, NULL,
			"Includes the Netscape client-authorization extension."
	},
	{ "r", COMMAND_LINE_VALUE_FLAG, NULL, NULL, NULL, -1, NULL,
			"Creates a self-signed certificate."
	},
	{ "sc", COMMAND_LINE_VALUE_REQUIRED, "<file>", NULL, NULL, -1, NULL,
			"Specifies the subject's certificate file."
	},
	{ "sky", COMMAND_LINE_VALUE_REQUIRED, "<keyType>", NULL, NULL, -1, NULL,
			"Specifies the subject's key type, which must be one of the following: "
			"signature (which indicates that the key is used for a digital signature), "
			"exchange (which indicates that the key is used for key encryption and key exchange), "
			"or an integer that represents a provider type. "
			"By default, you can pass 1 for an exchange key or 2 for a signature key."
	},
	{ "sp", COMMAND_LINE_VALUE_REQUIRED, "<provider>", NULL, NULL, -1, NULL,
			"Specifies the subject's CryptoAPI provider name, which must be defined in the registry subkeys of "
			"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider. If both –sp and –sy are present, "
			"the type of the CryptoAPI provider must correspond to the Type value of the provider's subkey."
	},
	{ "sv", COMMAND_LINE_VALUE_REQUIRED, "<pvkFile>", NULL, NULL, -1, NULL,
			"Specifies the subject's .pvk private key file. The file is created if none exists."
	},
	{ "sy", COMMAND_LINE_VALUE_REQUIRED, "<type>", NULL, NULL, -1, NULL,
			"Specifies the subject's CryptoAPI provider type, which must be defined in the registry subkeys of "
			"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider Types. If both –sy and –sp are present, "
			"the name of the CryptoAPI provider must correspond to the Name value of the provider type subkey."
	},
	{ "tbs", COMMAND_LINE_VALUE_REQUIRED, "<file>", NULL, NULL, -1, NULL,
			"Specifies the certificate or CRL file to be signed."
	},

	/* Help */

	{ "?", COMMAND_LINE_VALUE_FLAG | COMMAND_LINE_PRINT_HELP, NULL, NULL, NULL, -1, "help", "print help" },
	{ "!", COMMAND_LINE_VALUE_FLAG | COMMAND_LINE_PRINT_HELP, NULL, NULL, NULL, -1, "help-ext", "print extended help" },
	{ NULL, 0, NULL, NULL, NULL, -1, NULL, NULL }
};

int makecert_print_command_line_help(int argc, char** argv)
{
	char* str;
	int length;
	COMMAND_LINE_ARGUMENT_A* arg;

	printf("Usage: %s [options] [output file]\n", argv[0]);
	printf("\n");

	arg = args;

	do
	{
		if (arg->Flags & COMMAND_LINE_VALUE_FLAG)
		{
			printf("    %s", "-");
			printf("%-20s", arg->Name);
			printf("\t%s\n", arg->Text);
		}
		else if ((arg->Flags & COMMAND_LINE_VALUE_REQUIRED) || (arg->Flags & COMMAND_LINE_VALUE_OPTIONAL))
		{
			printf("    %s", "-");

			if (arg->Format)
			{
				length = strlen(arg->Name) + strlen(arg->Format) + 2;
				str = malloc(length + 1);
				sprintf_s(str, length + 1, "%s %s", arg->Name, arg->Format);
				printf("%-20s", str);
				free(str);
			}
			else
			{
				printf("%-20s", arg->Name);
			}

			printf("\t%s\n", arg->Text);
		}
	}
	while ((arg = CommandLineFindNextArgumentA(arg)) != NULL);

	return 1;
}

int x509_add_ext(X509* cert, int nid, char* value)
{
	X509V3_CTX ctx;
	X509_EXTENSION* ext;

	X509V3_set_ctx_nodb(&ctx);

	X509V3_set_ctx(&ctx, cert, cert, NULL, NULL, 0);
	ext = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);

	if (!ext)
		return 0;

	X509_add_ext(cert, ext, -1);
	X509_EXTENSION_free(ext);

	return 1;
}

char* x509_name_parse(char* name, char* txt, int* length)
{
	char* p;
	char* entry;

	p = strstr(name, txt);

	if (!p)
		return NULL;

	entry = p + strlen(txt) + 1;

	p = strchr(entry, '=');

	if (!p)
		*length = strlen(entry);
	else
		*length = p - entry;

	return entry;
}

char* x509_get_default_name()
{
	DWORD nSize = 0;
	char* ComputerName;

	GetComputerNameExA(ComputerNameNetBIOS, NULL, &nSize);
	ComputerName = (char*) malloc(nSize);
	GetComputerNameExA(ComputerNameNetBIOS, ComputerName, &nSize);

	return ComputerName;
}

int command_line_pre_filter(MAKECERT_CONTEXT* context, int index, int argc, LPCSTR* argv)
{
	if (index == (argc - 1))
	{
		if (argv[index][0] != '-')
			context->output_file = (char*) argv[index];

		return 1;
	}

	return 0;
}

int makecert_context_parse_arguments(MAKECERT_CONTEXT* context, int argc, char** argv)
{
	int status;
	DWORD flags;
	COMMAND_LINE_ARGUMENT_A* arg;

	/**
	 * makecert -r -pe -n "CN=%COMPUTERNAME%" -eku 1.3.6.1.5.5.7.3.1 -ss my -sr LocalMachine
	 * -sky exchange -sp "Microsoft RSA SChannel Cryptographic Provider" -sy 12
	 */

	CommandLineClearArgumentsA(args);

	flags = COMMAND_LINE_SEPARATOR_SPACE | COMMAND_LINE_SIGIL_DASH;
	status = CommandLineParseArgumentsA(argc, (const char**) argv, args, flags, context,
			(COMMAND_LINE_PRE_FILTER_FN_A) command_line_pre_filter, NULL);

	if (status & COMMAND_LINE_STATUS_PRINT_HELP)
	{
		makecert_print_command_line_help(argc, argv);
		return 0;
	}

	arg = args;

	do
	{
		if (!(arg->Flags & COMMAND_LINE_VALUE_PRESENT))
			continue;

		CommandLineSwitchStart(arg)

		/* Basic Options */

		CommandLineSwitchCase(arg, "silent")
		{
			context->silent = TRUE;
		}
		CommandLineSwitchCase(arg, "live")
		{
			context->live = TRUE;
		}

		CommandLineSwitchDefault(arg)
		{

		}

		CommandLineSwitchEnd(arg)
	}
	while ((arg = CommandLineFindNextArgumentA(arg)) != NULL);


	return 1;
}

int makecert_context_set_output_file_name(MAKECERT_CONTEXT* context, char* name)
{
	context->output_file = _strdup(name);
	return 1;
}

int makecert_context_output_certificate_file(MAKECERT_CONTEXT* context, char* path)
{
	FILE* fp;
	int length;
	char* filename;
	char* fullpath;

	if (!context->output_file)
		context->output_file = context->default_name;

	/*
	 * Output Certificate File
	 */

	length = strlen(context->output_file);
	filename = malloc(length + 8);
	strcpy(filename, context->output_file);
	strcpy(&filename[length], ".crt");

	if (path)
		fullpath = GetCombinedPath(path, filename);
	else
		fullpath = _strdup(filename);

	fp = fopen(fullpath, "w+");

	if (fp)
	{
		PEM_write_X509(fp, context->x509);
		fclose(fp);
	}

	free(filename);
	free(fullpath);

	return 1;
}

int makecert_context_output_private_key_file(MAKECERT_CONTEXT* context, char* path)
{
	FILE* fp;
	int length;
	char* filename;
	char* fullpath;

	if (!context->output_file)
		context->output_file = context->default_name;

	/**
	 * Output Private Key File
	 */

	length = strlen(context->output_file);
	filename = malloc(length + 8);
	strcpy(filename, context->output_file);
	strcpy(&filename[length], ".key");
	length = strlen(filename);

	if (path)
		fullpath = GetCombinedPath(path, filename);
	else
		fullpath = _strdup(filename);

	fp = fopen(fullpath, "w+");

	if (fp)
	{
		PEM_write_PrivateKey(fp, context->pkey, NULL, NULL, 0, NULL, NULL);
		fclose(fp);
	}

	free(filename);
	free(fullpath);

	return 1;
}

int makecert_context_process(MAKECERT_CONTEXT* context, int argc, char** argv)
{
	int length;
	char* entry;
	int key_length;
	long serial = 0;
	X509_NAME* name = NULL;
	const EVP_MD* md = NULL;
	COMMAND_LINE_ARGUMENT_A* arg;

	if (makecert_context_parse_arguments(context, argc, argv) < 1)
		return 0;

	context->default_name = x509_get_default_name();

	CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
	context->bio = BIO_new_fp(stderr, BIO_NOCLOSE);

	if (!context->pkey)
		context->pkey = EVP_PKEY_new();

	if (!context->pkey)
		return -1;

	if (!context->x509)
		context->x509 = X509_new();

	if (!context->x509)
		return -1;

	key_length = 2048;

	arg = CommandLineFindArgumentA(args, "len");

	if (arg->Flags & COMMAND_LINE_VALUE_PRESENT)
	{
		key_length = atoi(arg->Value);
	}

	context->rsa = RSA_generate_key(key_length, RSA_F4, NULL, NULL);

	if (!EVP_PKEY_assign_RSA(context->pkey, context->rsa))
		return -1;

	context->rsa = NULL;

	X509_set_version(context->x509, 2);

	arg = CommandLineFindArgumentA(args, "#");

	if (arg->Flags & COMMAND_LINE_VALUE_PRESENT)
		serial = atoi(arg->Value);
	else
		serial = (long) GetTickCount64();

	ASN1_INTEGER_set(X509_get_serialNumber(context->x509), serial);

	X509_gmtime_adj(X509_get_notBefore(context->x509), 0);
	X509_gmtime_adj(X509_get_notAfter(context->x509), (long) 60 * 60 * 24 * 365);
	X509_set_pubkey(context->x509, context->pkey);

	name = X509_get_subject_name(context->x509);

	arg = CommandLineFindArgumentA(args, "n");

	if (arg->Flags & COMMAND_LINE_VALUE_PRESENT)
	{
		entry = x509_name_parse(arg->Value, "C", &length);

		if (entry)
			X509_NAME_add_entry_by_txt(name, "C", MBSTRING_UTF8, (const unsigned char*) entry, length, -1, 0);

		entry = x509_name_parse(arg->Value, "ST", &length);

		if (entry)
			X509_NAME_add_entry_by_txt(name, "ST", MBSTRING_UTF8, (const unsigned char*) entry, length, -1, 0);

		entry = x509_name_parse(arg->Value, "L", &length);

		if (entry)
			X509_NAME_add_entry_by_txt(name, "L", MBSTRING_UTF8, (const unsigned char*) entry, length, -1, 0);

		entry = x509_name_parse(arg->Value, "O", &length);

		if (entry)
			X509_NAME_add_entry_by_txt(name, "O", MBSTRING_UTF8, (const unsigned char*) entry, length, -1, 0);

		entry = x509_name_parse(arg->Value, "OU", &length);

		if (entry)
			X509_NAME_add_entry_by_txt(name, "OU", MBSTRING_UTF8, (const unsigned char*) entry, length, -1, 0);

		entry = x509_name_parse(arg->Value, "CN", &length);

		if (!entry)
		{
			entry = context->default_name;
			length = strlen(entry);
		}

		X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_UTF8, (const unsigned char*) entry, length, -1, 0);
	}
	else
	{
		entry = context->default_name;
		length = strlen(entry);

		X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_UTF8, (const unsigned char*) entry, length, -1, 0);
	}

	X509_set_issuer_name(context->x509, name);

	x509_add_ext(context->x509, NID_ext_key_usage, "serverAuth");
	x509_add_ext(context->x509, NID_key_usage, "keyEncipherment,dataEncipherment");

	arg = CommandLineFindArgumentA(args, "a");

	md = EVP_sha1();

	if (arg->Flags & COMMAND_LINE_VALUE_PRESENT)
	{
		if (strcmp(arg->Value, "md5") == 0)
			md = EVP_md5();
		else if (strcmp(arg->Value, "sha1") == 0)
			md = EVP_sha1();
		else if (strcmp(arg->Value, "sha256") == 0)
			md = EVP_sha256();
		else if (strcmp(arg->Value, "sha384") == 0)
			md = EVP_sha384();
		else if (strcmp(arg->Value, "sha512") == 0)
			md = EVP_sha512();
	}

	if (!X509_sign(context->x509, context->pkey, md))
		return -1;

	/**
	 * Print certificate
	 */

	if (!context->silent)
		X509_print_fp(stdout, context->x509);

	/**
	 * Output certificate and private key to files
	 */

	if (!context->live)
	{
		makecert_context_output_certificate_file(context, NULL);
		makecert_context_output_private_key_file(context, NULL);
	}

	return 0;
}

MAKECERT_CONTEXT* makecert_context_new()
{
	MAKECERT_CONTEXT* context = NULL;

	context = (MAKECERT_CONTEXT*) malloc(sizeof(MAKECERT_CONTEXT));

	if (context)
	{
		ZeroMemory(context, sizeof(MAKECERT_CONTEXT));
	}

	return context;
}

void makecert_context_free(MAKECERT_CONTEXT* context)
{
	if (context)
	{
		X509_free(context->x509);
		EVP_PKEY_free(context->pkey);

		free(context->default_name);

		CRYPTO_cleanup_all_ex_data();

		CRYPTO_mem_leaks(context->bio);
		BIO_free(context->bio);

		free(context);
	}
}
