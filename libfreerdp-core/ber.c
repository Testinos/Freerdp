/**
 * FreeRDP: A Remote Desktop Protocol Client
 * ASN.1 Basic Encoding Rules (BER)
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

#include "ber.h"

void ber_read_length(STREAM* s, int* length)
{
	uint8 byte;

	stream_read_uint8(s, byte);

	if (byte & 0x80)
	{
		byte &= ~(0x80);
		if (byte == 2)
		{
			stream_read_uint16_be(s, *length);
		}
	}
	else
	{
		*length = byte;
	}
}

/**
 * Write BER length.
 * @param s stream
 * @param length length
 */

void ber_write_length(STREAM* s, int length)
{
	if (length > 0x7F)
	{
		stream_write_uint8(s, 0x82);
		stream_write_uint16_be(s, length);
	}
	else
	{
		stream_write_uint8(s, length);
	}
}

/**
 * Read BER Universal tag.
 * @param s stream
 * @param tag BER universally-defined tag
 * @return
 */

boolean ber_read_universal_tag(STREAM* s, uint8 tag)
{
	uint8 byte;

	stream_read_uint8(s, byte);

	if (byte != (BER_CLASS_UNIV | BER_PRIMITIVE) | (BER_TAG_MASK & tag))
		return False;

	return True;
}

/**
 * Write BER Universal tag.
 * @param s stream
 * @param tag BER universally-defined tag
 */

void ber_write_universal_tag(STREAM* s, uint8 tag)
{
	stream_write_uint8(s, (BER_CLASS_UNIV | BER_PRIMITIVE) | (BER_TAG_MASK & tag));
}

/**
 * Write BER Application tag.
 * @param s stream
 * @param tag BER application-defined tag
 * @param length length
 */

void ber_write_application_tag(STREAM* s, uint8 tag, int length)
{
	if (tag > 30)
	{
		stream_write_uint8(s, (BER_CLASS_APPL | BER_CONSTRUCT) | BER_TAG_MASK);
		stream_write_uint8(s, tag);
		ber_write_length(s, length);
	}
	else
	{
		stream_write_uint8(s, (BER_CLASS_APPL | BER_CONSTRUCT) | (BER_TAG_MASK & tag));
		ber_write_length(s, length);
	}
}

boolean ber_read_application_tag(STREAM* s, uint8 tag, int* length)
{
	uint8 byte;

	if (tag > 30)
	{
		stream_read_uint8(s, byte);

		if (byte != ((BER_CLASS_APPL | BER_CONSTRUCT) | BER_TAG_MASK))
			return False;

		stream_read_uint8(s, byte);

		if (byte != tag)
			return False;

		ber_read_length(s, length);
	}
	else
	{
		stream_read_uint8(s, byte);

		if (byte != ((BER_CLASS_APPL | BER_CONSTRUCT) | (BER_TAG_MASK & tag)))
			return False;

		ber_read_length(s, length);
	}

	return True;
}

boolean ber_read_contextual_tag(STREAM* s, uint8 tag, int* length, boolean pc)
{
	uint8 byte;

	stream_read_uint8(s, byte);

	if (byte != (BER_CLASS_CTXT | BER_PC(pc)) | (BER_TAG_MASK & tag))
		return False;

	ber_read_length(s, length);

	return True;
}

boolean ber_read_sequence_of_tag(STREAM* s, int* length)
{
	uint8 byte;

	stream_read_uint8(s, byte);

	if (byte != ((BER_CLASS_UNIV | BER_CONSTRUCT) | (BER_TAG_SEQUENCE_OF)))
		return False;

	ber_read_length(s, length);

	return True;
}

/**
 * Write BER SEQUENCE OF tag.
 * @param s stream
 * @param length length
 */

void ber_write_sequence_of_tag(STREAM* s, int length)
{
	stream_write_uint8(s, (BER_CLASS_UNIV | BER_CONSTRUCT) | (BER_TAG_MASK & BER_TAG_SEQUENCE_OF));
	ber_write_length(s, length);
}

boolean ber_read_enumerated(STREAM* s, uint8* enumerated, uint8 count)
{
	int length;

	ber_read_universal_tag(s, BER_TAG_ENUMERATED);
	ber_read_length(s, &length);

	if (length == 1)
		stream_read_uint8(s, *enumerated);
	else
		return False;

	/* check that enumerated value falls within expected range */
	if (*enumerated + 1 > count)
		return False;

	return True;
}

boolean ber_read_octet_string(STREAM* s, int* length)
{
	ber_read_universal_tag(s, BER_TAG_OCTET_STRING);
	ber_read_length(s, length);

	return True;
}

/**
 * Write a BER OCTET_STRING
 * @param s stream
 * @param oct_str octet string
 * @param length string length
 */

void ber_write_octet_string(STREAM* s, uint8* oct_str, int length)
{
	ber_write_universal_tag(s, BER_TAG_OCTET_STRING);
	ber_write_length(s, length);
	stream_write(s, oct_str, length);
}

/**
 * Write a BER BOOLEAN
 * @param s
 * @param value
 */

void ber_write_boolean(STREAM* s, boolean value)
{
	ber_write_universal_tag(s, BER_TAG_BOOLEAN);
	ber_write_length(s, 1);
	stream_write_uint8(s, (value == True) ? 0xFF : 0);
}

boolean ber_read_integer64(STREAM* s, uint8* value)
{
	int length;

	ber_read_universal_tag(s, BER_TAG_INTEGER);
	ber_read_length(s, &length);

	if (length != 8)
		return False;

	stream_read(s, value, 8);

	return True;
}

boolean ber_read_integer(STREAM* s, uint32* value)
{
	int length;

	ber_read_universal_tag(s, BER_TAG_INTEGER);
	ber_read_length(s, &length);

	if (value == NULL)
	{
		stream_seek(s, length);
		return True;
	}

	if (length == 1)
		stream_read_uint8(s, *value);
	else if (length == 2)
		stream_read_uint16_be(s, *value);
	else if (length == 3)
	{
		uint8 byte;
		stream_read_uint8(s, byte);
		stream_read_uint16_be(s, *value);
		*value += (byte << 16);
	}
	else
		return False;

	return True;
}

/**
 * Write a BER INTEGER
 * @param s
 * @param value
 */

void ber_write_integer(STREAM* s, uint32 value)
{
	ber_write_universal_tag(s, BER_TAG_INTEGER);

	if (value <= 0xFF)
	{
		ber_write_length(s, 1);
		stream_write_uint8(s, value);
	}
	else if (value <= 0xFFFF)
	{
		ber_write_length(s, 2);
		stream_write_uint16_be(s, value);
	}
	else if (value <= 0xFFFFFFFF)
	{
		ber_write_length(s, 4);
		stream_write_uint32_be(s, value);
	}
}
