/*
 * Generated by asn1c-0.9.22.1409 (http://lionet.info/asn1c)
 * From ASN.1 module "SPNEGO"
 * 	found in "spnego.asn1"
 * 	`asn1c -fnative-types -fskeletons-copy -fcompound-names`
 */

#include "asn_internal.h"

#include "NegTokenInit2.h"

static asn_TYPE_member_t asn_MBR_NegTokenInit2_1[] = {
	{ ATF_POINTER, 5, offsetof(struct NegTokenInit2, mechTypes),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_MechTypeList,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"mechTypes"
		},
	{ ATF_POINTER, 4, offsetof(struct NegTokenInit2, reqFlags),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ContextFlags,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"reqFlags"
		},
	{ ATF_POINTER, 3, offsetof(struct NegTokenInit2, mechToken),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"mechToken"
		},
	{ ATF_POINTER, 2, offsetof(struct NegTokenInit2, negHints),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_NegHints,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"negHints"
		},
	{ ATF_POINTER, 1, offsetof(struct NegTokenInit2, mechListMIC),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"mechListMIC"
		},
};
static ber_tlv_tag_t asn_DEF_NegTokenInit2_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_NegTokenInit2_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* mechTypes at 37 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* reqFlags at 38 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* mechToken at 39 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* negHints at 40 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 } /* mechListMIC at 41 */
};
static asn_SEQUENCE_specifics_t asn_SPC_NegTokenInit2_specs_1 = {
	sizeof(struct NegTokenInit2),
	offsetof(struct NegTokenInit2, _asn_ctx),
	asn_MAP_NegTokenInit2_tag2el_1,
	5,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_NegTokenInit2 = {
	"NegTokenInit2",
	"NegTokenInit2",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_NegTokenInit2_tags_1,
	sizeof(asn_DEF_NegTokenInit2_tags_1)
		/sizeof(asn_DEF_NegTokenInit2_tags_1[0]), /* 1 */
	asn_DEF_NegTokenInit2_tags_1,	/* Same as above */
	sizeof(asn_DEF_NegTokenInit2_tags_1)
		/sizeof(asn_DEF_NegTokenInit2_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_NegTokenInit2_1,
	5,	/* Elements count */
	&asn_SPC_NegTokenInit2_specs_1	/* Additional specs */
};

