/**
 * FreeRDP: A Remote Desktop Protocol Client
 * Terminal Server Gateway (TSG)
 *
 * Copyright 2012 Fujitsu Technology Solutions GmbH
 * Copyright 2012 Dmitrij Jasnov <dmitrij.jasnov@ts.fujitsu.com>
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freerdp/utils/sleep.h>
#include <freerdp/utils/stream.h>
#include <freerdp/utils/memory.h>
#include <freerdp/utils/hexdump.h>
#include <freerdp/utils/unicode.h>

#include "tsg.h"

/**
 * RPC Functions: http://msdn.microsoft.com/en-us/library/windows/desktop/aa378623/
 * Remote Procedure Call: http://msdn.microsoft.com/en-us/library/windows/desktop/aa378651/
 * RPC NDR Interface Reference: http://msdn.microsoft.com/en-us/library/windows/desktop/hh802752/
 */

uint8 tsg_packet1[108] =
{
	0x43, 0x56, 0x00, 0x00, 0x43, 0x56, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x52, 0x54, 0x43, 0x56,
	0x04, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00,
	0x8A, 0xE3, 0x13, 0x71, 0x02, 0xF4, 0x36, 0x71, 0x01, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x02, 0x40, 0x28, 0x00, 0xDD, 0x65, 0xE2, 0x44, 0xAF, 0x7D, 0xCD, 0x42, 0x85, 0x60, 0x3C, 0xDB,
	0x6E, 0x7A, 0x27, 0x29, 0x01, 0x00, 0x03, 0x00, 0x04, 0x5D, 0x88, 0x8A, 0xEB, 0x1C, 0xC9, 0x11,
	0x9F, 0xE8, 0x08, 0x00, 0x2B, 0x10, 0x48, 0x60, 0x02, 0x00, 0x00, 0x00
};

/**
	TsProxyCreateTunnel

	0x43, 0x56, 0x00, 0x00, packetId (TSG_PACKET_TYPE_VERSIONCAPS)

	TSG_PACKET
	0x43, 0x56, 0x00, 0x00, SwitchValue (TSG_PACKET_TYPE_VERSIONCAPS)

	0x00, 0x00, 0x02, 0x00, NdrPtr

	0x52, 0x54, componentId
	0x43, 0x56, packetId

	0x04, 0x00, 0x02, 0x00, NdrPtr TsgCapsPtr
	0x01, 0x00, 0x00, 0x00, numCapabilities

	0x01, 0x00, MajorVersion
	0x01, 0x00, MinorVersion
	0x00, 0x00, QuarantineCapabilities

	0x00, 0x00, alignment pad?

	0x01, 0x00, 0x00, 0x00, MaximumCount
	0x01, 0x00, 0x00, 0x00, TSG_CAPABILITY_TYPE_NAP

	0x01, 0x00, 0x00, 0x00, SwitchValue (TSG_NAP_CAPABILITY_QUAR_SOH)
	0x1F, 0x00, 0x00, 0x00, idle value in minutes?

	0x8A, 0xE3, 0x13, 0x71, 0x02, 0xF4, 0x36, 0x71, 0x01, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x02, 0x40, 0x28, 0x00, 0xDD, 0x65, 0xE2, 0x44, 0xAF, 0x7D, 0xCD, 0x42, 0x85, 0x60, 0x3C, 0xDB,
	0x6E, 0x7A, 0x27, 0x29, 0x01, 0x00, 0x03, 0x00, 0x04, 0x5D, 0x88, 0x8A, 0xEB, 0x1C, 0xC9, 0x11,
	0x9F, 0xE8, 0x08, 0x00, 0x2B, 0x10, 0x48, 0x60, 0x02, 0x00, 0x00, 0x00
 */

uint8 tsg_packet2[112] =
{
	0x00, 0x00, 0x00, 0x00, 0x6A, 0x78, 0xE9, 0xAB, 0x02, 0x90, 0x1C, 0x44, 0x8D, 0x99, 0x29, 0x30,
	0x53, 0x6C, 0x04, 0x33, 0x52, 0x51, 0x00, 0x00, 0x52, 0x51, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00,
	0x61, 0x00, 0x62, 0x00, 0x63, 0x00, 0x2D, 0x00, 0x4E, 0x00, 0x48, 0x00, 0x35, 0x00, 0x37, 0x00,
	0x30, 0x00, 0x2E, 0x00, 0x43, 0x00, 0x53, 0x00, 0x4F, 0x00, 0x44, 0x00, 0x2E, 0x00, 0x6C, 0x00,
	0x6F, 0x00, 0x63, 0x00, 0x61, 0x00, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/**
	TsProxyAuthorizeTunnel

	0x00, 0x00, 0x00, 0x00, 0x6A, 0x78, 0xE9, 0xAB, 0x02, 0x90, 0x1C, 0x44, 0x8D, 0x99, 0x29, 0x30,
	0x53, 0x6C, 0x04, 0x33, 0x52, 0x51, 0x00, 0x00,

	0x52, 0x51, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00,
	0x61, 0x00, 0x62, 0x00, 0x63, 0x00, 0x2D, 0x00, 0x4E, 0x00, 0x48, 0x00, 0x35, 0x00, 0x37, 0x00,
	0x30, 0x00, 0x2E, 0x00, 0x43, 0x00, 0x53, 0x00, 0x4F, 0x00, 0x44, 0x00, 0x2E, 0x00, 0x6C, 0x00,
	0x6F, 0x00, 0x63, 0x00, 0x61, 0x00, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
 */

uint8 tsg_packet3[40] =
{
	0x00, 0x00, 0x00, 0x00, 0x6A, 0x78, 0xE9, 0xAB, 0x02, 0x90, 0x1C, 0x44, 0x8D, 0x99, 0x29, 0x30,
	0x53, 0x6C, 0x04, 0x33, 0x01, 0x00, 0x00, 0x00, 0x52, 0x47, 0x00, 0x00, 0x52, 0x47, 0x00, 0x00,
	0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00
};

/**
	TsProxyMakeTunnelCall

	0x00, 0x00, 0x00, 0x00, 0x6A, 0x78, 0xE9, 0xAB, 0x02, 0x90, 0x1C, 0x44, 0x8D, 0x99, 0x29, 0x30,
	0x53, 0x6C, 0x04, 0x33, 0x01, 0x00, 0x00, 0x00,

	0x52, 0x47, 0x00, 0x00,
	0x52, 0x47, 0x00, 0x00,
	0x00, 0x00, 0x02, 0x00,
	0x01, 0x00, 0x00, 0x00
 */

uint8 tsg_packet4[48] =
{
	0x00, 0x00, 0x00, 0x00, 0x6A, 0x78, 0xE9, 0xAB, 0x02, 0x90, 0x1C, 0x44, 0x8D, 0x99, 0x29, 0x30,
	0x53, 0x6C, 0x04, 0x33, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00
};

/**
	TsProxyCreateChannel

	0x00, 0x00, 0x00, 0x00, 0x6A, 0x78, 0xE9, 0xAB, 0x02, 0x90, 0x1C, 0x44, 0x8D, 0x99, 0x29, 0x30,
	0x53, 0x6C, 0x04, 0x33, 0x00, 0x00, 0x02, 0x00,

	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00
 */

uint8 tsg_packet5[20] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};

void Opnum0NotUsedOnWire(handle_t IDL_handle)
{

}

HRESULT TsProxyCreateTunnel(PTSG_PACKET tsgPacket, PTSG_PACKET* tsgPacketResponse,
		PTUNNEL_CONTEXT_HANDLE_SERIALIZE* tunnelContext, unsigned long* tunnelId)
{
	return 0;
}

HRESULT TsProxyAuthorizeTunnel(PTUNNEL_CONTEXT_HANDLE_NOSERIALIZE tunnelContext,
		PTSG_PACKET tsgPacket, PTSG_PACKET* tsgPacketResponse)
{
	return 0;
}

HRESULT TsProxyMakeTunnelCall(PTUNNEL_CONTEXT_HANDLE_NOSERIALIZE tunnelContext,
		unsigned long procId, PTSG_PACKET tsgPacket, PTSG_PACKET* tsgPacketResponse)
{
	return 0;
}

HRESULT TsProxyCreateChannel(PTUNNEL_CONTEXT_HANDLE_NOSERIALIZE tunnelContext,
		PTSENDPOINTINFO tsEndPointInfo, PCHANNEL_CONTEXT_HANDLE_SERIALIZE* channelContext, unsigned long* channelId)
{
	return 0;
}

void Opnum5NotUsedOnWire(handle_t IDL_handle)
{

}

HRESULT TsProxyCloseChannel(PCHANNEL_CONTEXT_HANDLE_NOSERIALIZE* context)
{
	return 0;
}

HRESULT TsProxyCloseTunnel(PTUNNEL_CONTEXT_HANDLE_SERIALIZE* context)
{
	return 0;
}

DWORD TsProxySetupReceivePipe(handle_t IDL_handle, byte pRpcMessage[])
{
	return 0;
}

DWORD TsProxySendToServer(handle_t IDL_handle, byte pRpcMessage[], uint32 count, uint32* lengths)
{
	STREAM* s;
	int status;
	int length;
	rdpTsg* tsg;
	byte* buffer1;
	byte* buffer2;
	byte* buffer3;
	uint32 buffer1Length;
	uint32 buffer2Length;
	uint32 buffer3Length;
	uint32 numBuffers = 0;
	uint32 totalDataBytes = 0;

	tsg = (rdpTsg*) IDL_handle;
	buffer1Length = buffer2Length = buffer3Length = 0;

	if (count > 0)
	{
		numBuffers++;
		buffer1 = &pRpcMessage[0];
		buffer1Length = lengths[0];
		totalDataBytes += lengths[0] + 4;
	}

	if (count > 1)
	{
		numBuffers++;
		buffer2 = &pRpcMessage[1];
		buffer2Length = lengths[1];
		totalDataBytes += lengths[1] + 4;
	}

	if (count > 2)
	{
		numBuffers++;
		buffer3 = &pRpcMessage[2];
		buffer3Length = lengths[2];
		totalDataBytes += lengths[2] + 4;
	}

	s = stream_new(28 + totalDataBytes);

	/* PCHANNEL_CONTEXT_HANDLE_NOSERIALIZE_NR (20 bytes) */
	stream_write_uint32(s, 0); /* ContextType (4 bytes) */
	stream_write(s, tsg->ChannelContext, 16); /* ContextUuid (4 bytes) */

	stream_write_uint32_be(s, totalDataBytes); /* totalDataBytes (4 bytes) */
	stream_write_uint32_be(s, numBuffers); /* numBuffers (4 bytes) */

	if (buffer1Length > 0)
		stream_write_uint32_be(s, buffer1Length); /* buffer1Length (4 bytes) */
	if (buffer2Length > 0)
		stream_write_uint32_be(s, buffer2Length); /* buffer2Length (4 bytes) */
	if (buffer3Length > 0)
		stream_write_uint32_be(s, buffer3Length); /* buffer3Length (4 bytes) */

	if (buffer1Length > 0)
		stream_write(s, buffer1, buffer1Length); /* buffer1 (variable) */
	if (buffer2Length > 0)
		stream_write(s, buffer2, buffer2Length); /* buffer2 (variable) */
	if (buffer3Length > 0)
		stream_write(s, buffer3, buffer3Length); /* buffer3 (variable) */

	stream_seal(s);

	length = s->size;
	status = rpc_tsg_write(tsg->rpc, s->data, s->size, 9);

	stream_free(s);

	if (status <= 0)
	{
		printf("rpc_tsg_write failed!\n");
		return -1;
	}

	return length;
}

boolean tsg_connect(rdpTsg* tsg, const char* hostname, uint16 port)
{
	uint8* data;
	uint32 length;
	int status = -1;
	rdpRpc* rpc = tsg->rpc;

	if (!rpc_connect(rpc))
	{
		printf("rpc_connect failed!\n");
		return false;
	}

	DEBUG_TSG("rpc_connect success");

	/**
	 * OpNum = 1
	 *
	 * HRESULT TsProxyCreateTunnel(
	 * [in, ref] PTSG_PACKET tsgPacket,
	 * [out, ref] PTSG_PACKET* tsgPacketResponse,
	 * [out] PTUNNEL_CONTEXT_HANDLE_SERIALIZE* tunnelContext,
	 * [out] unsigned long* tunnelId
	 * );
	 */

	DEBUG_TSG("TsProxyCreateTunnel");
	status = rpc_tsg_write(rpc, tsg_packet1, sizeof(tsg_packet1), 1);

	if (status <= 0)
	{
		printf("rpc_write opnum=1 failed!\n");
		return false;
	}

	length = 0x8FFF;
	data = xmalloc(length);
	status = rpc_read(rpc, data, length);

	if (status <= 0)
	{
		printf("rpc_recv failed!\n");
		return false;
	}

	memcpy(tsg->TunnelContext, data + 2300, 16);

#ifdef WITH_DEBUG_TSG
	printf("TSG tunnelContext:\n");
	freerdp_hexdump(tsg->TunnelContext, 16);
	printf("\n");
#endif

	memcpy(tsg_packet2 + 4, tsg->TunnelContext, 16);

	/**
	 * OpNum = 2
	 *
	 * HRESULT TsProxyAuthorizeTunnel(
	 * [in] PTUNNEL_CONTEXT_HANDLE_NOSERIALIZE tunnelContext,
	 * [in, ref] PTSG_PACKET tsgPacket,
	 * [out, ref] PTSG_PACKET* tsgPacketResponse
	 * );
	 *
	 */

	DEBUG_TSG("TsProxyAuthorizeTunnel");
	status = rpc_tsg_write(rpc, tsg_packet2, sizeof(tsg_packet2), 2);

	if (status <= 0)
	{
		printf("rpc_write opnum=2 failed!\n");
		return false;
	}

	status = rpc_read(rpc, data, length);

	if (status <= 0)
	{
		printf("rpc_recv failed!\n");
		return false;
	}

	memcpy(tsg_packet3 + 4, tsg->TunnelContext, 16);

	/**
	 * OpNum = 3
	 *
	 * HRESULT TsProxyMakeTunnelCall(
	 * [in] PTUNNEL_CONTEXT_HANDLE_NOSERIALIZE tunnelContext,
	 * [in] unsigned long procId,
	 * [in, ref] PTSG_PACKET tsgPacket,
	 * [out, ref] PTSG_PACKET* tsgPacketResponse
	 * );
	 */

	DEBUG_TSG("TsProxyMakeTunnelCall");
	status = rpc_tsg_write(rpc, tsg_packet3, sizeof(tsg_packet3), 3);

	if (status <= 0)
	{
		printf("rpc_write opnum=3 failed!\n");
		return false;
	}
	status = -1;

	UNICONV* tsg_uniconv = freerdp_uniconv_new();
	uint32 dest_addr_unic_len;
	uint8* dest_addr_unic = (uint8*) freerdp_uniconv_out(tsg_uniconv, hostname, (size_t*) &dest_addr_unic_len);
	freerdp_uniconv_free(tsg_uniconv);

	memcpy(tsg_packet4 + 4, tsg->TunnelContext, 16);
	memcpy(tsg_packet4 + 38, &port, 2);

	STREAM* s_p4 = stream_new(60 + dest_addr_unic_len + 2);
	stream_write(s_p4, tsg_packet4, 48);
	stream_write_uint32(s_p4, (dest_addr_unic_len / 2) + 1); /* MaximumCount */
	stream_write_uint32(s_p4, 0x00000000); /* Offset */
	stream_write_uint32(s_p4, (dest_addr_unic_len / 2) + 1);/* ActualCount */
	stream_write(s_p4, dest_addr_unic, dest_addr_unic_len);
	stream_write_uint16(s_p4, 0x0000); /* unicode zero to terminate hostname string */

	/**
	 * OpNum = 4
	 *
	 * HRESULT TsProxyCreateChannel(
	 * [in] PTUNNEL_CONTEXT_HANDLE_NOSERIALIZE tunnelContext,
	 * [in, ref] PTSENDPOINTINFO tsEndPointInfo,
	 * [out] PCHANNEL_CONTEXT_HANDLE_SERIALIZE* channelContext,
	 * [out] unsigned long* channelId
	 * );
	 */

	DEBUG_TSG("TsProxyCreateChannel");
	status = rpc_tsg_write(rpc, s_p4->data, s_p4->size, 4);

	if (status <= 0)
	{
		printf("rpc_write opnum=4 failed!\n");
		return false;
	}
	xfree(dest_addr_unic);

	status = rpc_read(rpc, data, length);

	if (status < 0)
	{
		printf("rpc_recv failed!\n");
		return false;
	}

	memcpy(tsg->ChannelContext, data + 4, 16);

#ifdef WITH_DEBUG_TSG
	printf("TSG ChannelContext:\n");
	freerdp_hexdump(tsg->ChannelContext, 16);
	printf("\n");
#endif

	memcpy(tsg_packet5 + 4, tsg->ChannelContext, 16);

	/**
	 * OpNum = 8
	 *
	 * DWORD TsProxySetupReceivePipe(
	 * [in, max_is(32767)] byte pRpcMessage[]
	 * );
	 */

	DEBUG_TSG("TsProxySetupReceivePipe");
	status = rpc_tsg_write(rpc, tsg_packet5, sizeof(tsg_packet5), 8);

	if (status <= 0)
	{
		printf("rpc_write opnum=8 failed!\n");
		return false;
	}

	return true;
}

int tsg_read(rdpTsg* tsg, uint8* data, uint32 length)
{
	int status;

	status = rpc_read(tsg->rpc, data, length);

	return status;
}

int tsg_write(rdpTsg* tsg, uint8* data, uint32 length)
{
	return TsProxySendToServer((handle_t) tsg, data, 1, &length);
}

rdpTsg* tsg_new(rdpTransport* transport)
{
	rdpTsg* tsg;

	tsg = xnew(rdpTsg);

	if (tsg != NULL)
	{
		tsg->transport = transport;
		tsg->settings = transport->settings;
		tsg->rpc = rpc_new(tsg->transport);
	}

	return tsg;
}

void tsg_free(rdpTsg* tsg)
{
	if (tsg != NULL)
	{
		rpc_free(tsg->rpc);
		xfree(tsg);
	}
}
