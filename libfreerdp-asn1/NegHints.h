/*
 * Generated by asn1c-0.9.22.1409 (http://lionet.info/asn1c)
 * From ASN.1 module "SPNEGO"
 * 	found in "spnego.asn1"
 * 	`asn1c -fnative-types -fskeletons-copy -fcompound-names`
 */

#ifndef	_NegHints_H_
#define	_NegHints_H_


#include "asn_application.h"

/* Including external dependencies */
#include "GeneralString.h"
#include "OCTET_STRING.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* NegHints */
typedef struct NegHints {
	GeneralString_t	*hintName	/* OPTIONAL */;
	OCTET_STRING_t	*hintAddress	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} NegHints_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_NegHints;

#ifdef __cplusplus
}
#endif

#endif	/* _NegHints_H_ */
