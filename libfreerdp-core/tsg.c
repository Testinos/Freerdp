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

HRESULT TsProxyCloseChannel(PCHANNEL_CONTEXT_HANDLE_NOSERIALIZE* context)
{
	return 0;
}

HRESULT TsProxyCloseTunnel(PTUNNEL_CONTEXT_HANDLE_SERIALIZE* context)
{
	return 0;
}

DWORD TsProxySetupReceivePipe(byte pRpcMessage[])
{
	return 0;
}

DWORD TsProxySendToServer(byte pRpcMessage[])
{
	return 0;
}

boolean tsg_connect(rdpTsg* tsg, const char* hostname, uint16 port)
{
	uint8* data;
	uint32 length;
	int status = -1;
	rdpRpc* rpc = tsg->rpc;
	rdpTransport* transport = tsg->transport;

	if (!rpc_attach(rpc, transport->tcp_in, transport->tcp_out, transport->tls_in, transport->tls_out))
	{
		printf("rpc_attach failed!\n");
		return false;
	}

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

	tsg->tunnelContext = xmalloc(16);
	memcpy(tsg->tunnelContext, data + 0x91c, 16);

#ifdef WITH_DEBUG_TSG
	printf("TSG tunnelContext:\n");
	freerdp_hexdump(tsg->tunnelContext, 16);
	printf("\n");
#endif

	memcpy(tsg_packet2 + 4, tsg->tunnelContext, 16);

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

	memcpy(tsg_packet3 + 4, tsg->tunnelContext, 16);

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

	memcpy(tsg_packet4 + 4, tsg->tunnelContext, 16);
	memcpy(tsg_packet4 + 38, &port, 2);

	STREAM* s_p4 = stream_new(60 + dest_addr_unic_len + 2);
	stream_write(s_p4, tsg_packet4, 48);
	stream_write_uint32(s_p4, (dest_addr_unic_len / 2) + 1); /* MaximumCount */
	stream_write_uint32(s_p4, 0x00000000); /* Offset */
	stream_write_uint32(s_p4, (dest_addr_unic_len / 2) + 1);/* ActualCount */
	stream_write(s_p4, dest_addr_unic, dest_addr_unic_len);
	stream_write_uint16(s_p4,0x0000); /* unicode zero to terminate hostname string */

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

	tsg->channelContext = xmalloc(16);
	memcpy(tsg->channelContext, data + 4, 16);

#ifdef WITH_DEBUG_TSG
	printf("TSG channelContext:\n");
	freerdp_hexdump(tsg->channelContext, 16);
	printf("\n");
#endif

	memcpy(tsg_packet5 + 4, tsg->channelContext, 16);

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

uint8 pp[8] =
{
	0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00
};

int tsg_write(rdpTsg* tsg, uint8* data, uint32 length)
{
	STREAM* s;
	uint8* tsg_pkg;
	int status = -1;

	uint16 opnum = 9;
	uint32 tsg_length = length + 16 + 4 + 12 + 8;
	uint32 totalDataBytes = length + 4;

	s = stream_new(12);
	stream_write_uint32_be(s, totalDataBytes);
	stream_write_uint32_be(s, 0x01);
	stream_write_uint32_be(s, length);

	tsg_pkg = xmalloc(tsg_length);
	memset(tsg_pkg, 0, 4);
	memcpy(tsg_pkg + 4, tsg->channelContext, 16);
	memcpy(tsg_pkg + 20, s->data, 12);
	memcpy(tsg_pkg + 32, data, length);

	memcpy(tsg_pkg + 32 + length, pp, 8);

	status = rpc_tsg_write(tsg->rpc, tsg_pkg, tsg_length, opnum);

	xfree(tsg_pkg);
	stream_free(s);

	if (status <= 0)
	{
		printf("rpc_write failed!\n");
		return -1;
	}

	return length;
}

int tsg_read(rdpTsg* tsg, uint8* data, uint32 length)
{
	int status;

	status = rpc_read(tsg->rpc, data, length);

	return status;
}

rdpTsg* tsg_new(rdpSettings* settings)
{
	rdpTsg* tsg;
	tsg = (rdpTsg*) xzalloc(sizeof(rdpTsg));

	tsg->settings = settings;
	tsg->rpc = rpc_new(settings);

	return tsg;
}

void tsg_free(rdpTsg* tsg)
{

}
