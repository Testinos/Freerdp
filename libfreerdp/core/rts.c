/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * Request To Send (RTS) PDUs
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rts.h"

/**
 *                                      Connection Establishment\n
 *
 *     Client                  Outbound Proxy           Inbound Proxy                 Server\n
 *        |                         |                         |                         |\n
 *        |-----------------IN Channel Request--------------->|                         |\n
 *        |---OUT Channel Request-->|                         |<-Legacy Server Response-|\n
 *        |                         |<--------------Legacy Server Response--------------|\n
 *        |                         |                         |                         |\n
 *        |---------CONN_A1-------->|                         |                         |\n
 *        |----------------------CONN_B1--------------------->|                         |\n
 *        |                         |----------------------CONN_A2--------------------->|\n
 *        |                         |                         |                         |\n
 *        |<--OUT Channel Response--|                         |---------CONN_B2-------->|\n
 *        |<--------CONN_A3---------|                         |                         |\n
 *        |                         |<---------------------CONN_C1----------------------|\n
 *        |                         |                         |<--------CONN_B3---------|\n
 *        |<--------CONN_C2---------|                         |                         |\n
 *        |                         |                         |                         |\n
 *
 */

/**
 * [MS-RPCH]: Remote Procedure Call over HTTP Protocol Specification:
 * http://msdn.microsoft.com/en-us/library/cc243950/
 */

BOOL rts_connect(rdpRpc* rpc)
{
	int status;
	RTS_PDU rts_pdu;
	HttpResponse* http_response;

	if (!rpc_ntlm_http_out_connect(rpc))
	{
		printf("rpc_out_connect_http error!\n");
		return FALSE;
	}

	if (!rts_send_CONN_A1_pdu(rpc))
	{
		printf("rpc_send_CONN_A1_pdu error!\n");
		return FALSE;
	}

	if (!rpc_ntlm_http_in_connect(rpc))
	{
		printf("rpc_in_connect_http error!\n");
		return FALSE;
	}

	if (!rts_send_CONN_B1_pdu(rpc))
	{
		printf("rpc_send_CONN_B1_pdu error!\n");
		return FALSE;
	}

	/* Receive OUT Channel Response */
	http_response = http_response_recv(rpc->tls_out);

	if (http_response->StatusCode != 200)
	{
		printf("rts_connect error! Status Code: %d\n", http_response->StatusCode);
		http_response_print(http_response);
		http_response_free(http_response) ;
		return FALSE;
	}

	http_response_print(http_response);

	http_response_free(http_response);

	/* Receive CONN_A3 RTS PDU */
	status = rts_recv_pdu(rpc, &rts_pdu);

	/* Receive CONN_C2 RTS PDU */
	status = rts_recv_pdu(rpc, &rts_pdu);

	return TRUE;
}

#ifdef WITH_DEBUG_RTS

static const char* const RTS_CMD_STRINGS[] =
{
	"ReceiveWindowSize",
	"FlowControlAck",
	"ConnectionTimeout",
	"Cookie",
	"ChannelLifetime",
	"ClientKeepalive",
	"Version",
	"Empty",
	"Padding",
	"NegativeANCE",
	"ANCE",
	"ClientAddress",
	"AssociationGroupId",
	"Destination",
	"PingTrafficSentNotify"
};

#endif

void rts_pdu_header_init(rdpRpc* rpc, RTS_PDU_HEADER* header)
{
	header->rpc_vers = rpc->rpc_vers;
	header->rpc_vers_minor = rpc->rpc_vers_minor;
	header->packed_drep[0] = rpc->packed_drep[0];
	header->packed_drep[1] = rpc->packed_drep[1];
	header->packed_drep[2] = rpc->packed_drep[2];
	header->packed_drep[3] = rpc->packed_drep[3];
}

void rts_receive_window_size_command_read(rdpRpc* rpc, STREAM* s)
{
	stream_seek_UINT32(s); /* ReceiveWindowSize (4 bytes) */
}

void rts_receive_window_size_command_write(STREAM* s, UINT32 ReceiveWindowSize)
{
	stream_write_UINT32(s, RTS_CMD_RECEIVE_WINDOW_SIZE); /* CommandType (4 bytes) */
	stream_write_UINT32(s, ReceiveWindowSize); /* ReceiveWindowSize (4 bytes) */
}

void rts_flow_control_ack_command_read(rdpRpc* rpc, STREAM* s)
{
	/* Ack (24 bytes) */
	stream_seek_UINT32(s); /* BytesReceived (4 bytes) */
	stream_seek_UINT32(s); /* AvailableWindow (4 bytes) */
	stream_seek(s, 16); /* ChannelCookie (16 bytes) */
}

void rts_flow_control_ack_command_write(STREAM* s, UINT32 BytesReceived, UINT32 AvailableWindow, BYTE* ChannelCookie)
{
	stream_write_UINT32(s, RTS_CMD_FLOW_CONTROL_ACK); /* CommandType (4 bytes) */

	/* Ack (24 bytes) */
	stream_write_UINT32(s, BytesReceived); /* BytesReceived (4 bytes) */
	stream_write_UINT32(s, AvailableWindow); /* AvailableWindow (4 bytes) */
	stream_write(s, ChannelCookie, 16); /* ChannelCookie (16 bytes) */
}

void rts_connection_timeout_command_read(rdpRpc* rpc, STREAM* s)
{
	stream_seek_UINT32(s); /* ConnectionTimeout (4 bytes) */
}

void rts_connection_timeout_command_write(STREAM* s, UINT32 ConnectionTimeout)
{
	stream_write_UINT32(s, RTS_CMD_CONNECTION_TIMEOUT); /* CommandType (4 bytes) */
	stream_write_UINT32(s, ConnectionTimeout); /* ConnectionTimeout (4 bytes) */
}

void rts_cookie_command_read(rdpRpc* rpc, STREAM* s)
{
	stream_seek(s, 16); /* Cookie (16 bytes) */
}

void rts_cookie_command_write(STREAM* s, BYTE* Cookie)
{
	stream_write_UINT32(s, RTS_CMD_COOKIE); /* CommandType (4 bytes) */
	stream_write(s, Cookie, 16); /* Cookie (16 bytes) */
}

void rts_channel_lifetime_command_read(rdpRpc* rpc, STREAM* s)
{
	stream_seek_UINT32(s); /* ChannelLifetime (4 bytes) */
}

void rts_channel_lifetime_command_write(STREAM* s, UINT32 ChannelLifetime)
{
	stream_write_UINT32(s, RTS_CMD_CHANNEL_LIFETIME); /* CommandType (4 bytes) */
	stream_write_UINT32(s, ChannelLifetime); /* ChannelLifetime (4 bytes) */
}

void rts_client_keepalive_command_read(rdpRpc* rpc, STREAM* s)
{
	stream_seek_UINT32(s); /* ClientKeepalive (4 bytes) */
}

void rts_client_keepalive_command_write(STREAM* s, UINT32 ClientKeepalive)
{
	stream_write_UINT32(s, RTS_CMD_CLIENT_KEEPALIVE); /* CommandType (4 bytes) */
	stream_write_UINT32(s, ClientKeepalive); /* ClientKeepalive (4 bytes) */
}

void rts_version_command_read(rdpRpc* rpc, STREAM* s)
{
	stream_seek_UINT32(s); /* Version (4 bytes) */
}

void rts_version_command_write(STREAM* s)
{
	stream_write_UINT32(s, RTS_CMD_VERSION); /* CommandType (4 bytes) */
	stream_write_UINT32(s, 1); /* Version (4 bytes) */
}

void rts_empty_command_read(rdpRpc* rpc, STREAM* s)
{

}

void rts_empty_command_write(STREAM* s)
{
	stream_write_UINT32(s, RTS_CMD_EMPTY); /* CommandType (4 bytes) */
}

void rts_padding_command_read(rdpRpc* rpc, STREAM* s)
{
	UINT32 ConformanceCount;

	stream_read_UINT32(s, ConformanceCount); /* ConformanceCount (4 bytes) */
	stream_seek(s, ConformanceCount); /* Padding (variable) */
}

void rts_padding_command_write(STREAM* s, UINT32 ConformanceCount)
{
	stream_write_UINT32(s, ConformanceCount); /* ConformanceCount (4 bytes) */
	stream_write_zero(s, ConformanceCount); /* Padding (variable) */
}

void rts_negative_ance_command_read(rdpRpc* rpc, STREAM* s)
{

}

void rts_negative_ance_command_write(STREAM* s)
{
	stream_write_UINT32(s, RTS_CMD_NEGATIVE_ANCE); /* CommandType (4 bytes) */
}

void rts_ance_command_read(rdpRpc* rpc, STREAM* s)
{

}

void rts_ance_command_write(STREAM* s)
{
	stream_write_UINT32(s, RTS_CMD_ANCE); /* CommandType (4 bytes) */
}

void rts_client_address_command_read(rdpRpc* rpc, STREAM* s)
{
	UINT32 AddressType;

	stream_read_UINT32(s, AddressType); /* AddressType (4 bytes) */

	if (AddressType == 0)
	{
		stream_seek(s, 4); /* ClientAddress (4 bytes) */
	}
	else
	{
		stream_seek(s, 16); /* ClientAddress (16 bytes) */
	}

	stream_seek(s, 12); /* padding (12 bytes) */
}

void rts_client_address_command_write(STREAM* s, UINT32 AddressType, BYTE* ClientAddress)
{
	stream_write_UINT32(s, RTS_CMD_CLIENT_ADDRESS); /* CommandType (4 bytes) */
	stream_write_UINT32(s, AddressType); /* AddressType (4 bytes) */

	if (AddressType == 0)
	{
		stream_write(s, ClientAddress, 4); /* ClientAddress (4 bytes) */
	}
	else
	{
		stream_write(s, ClientAddress, 16); /* ClientAddress (16 bytes) */
	}

	stream_write_zero(s, 12); /* padding (12 bytes) */
}

void rts_association_group_id_command_read(rdpRpc* rpc, STREAM* s)
{
	stream_seek(s, 16); /* AssociationGroupId (16 bytes) */
}

void rts_association_group_id_command_write(STREAM* s, BYTE* associationGroupId)
{
	stream_write_UINT32(s, RTS_CMD_ASSOCIATION_GROUP_ID); /* CommandType (4 bytes) */
	stream_write(s, associationGroupId, 16); /* AssociationGroupId (16 bytes) */
}

void rts_destination_command_read(rdpRpc* rpc, STREAM* s)
{
	stream_seek_UINT32(s); /* Destination (4 bytes) */
}

void rts_destination_command_write(STREAM* s, UINT32 Destination)
{
	stream_write_UINT32(s, RTS_CMD_DESTINATION); /* CommandType (4 bytes) */
	stream_write_UINT32(s, Destination); /* Destination (4 bytes) */
}

void rts_ping_traffic_sent_notify_command_read(rdpRpc* rpc, STREAM* s)
{
	stream_seek_UINT32(s); /* PingTrafficSent (4 bytes) */
}

void rts_ping_traffic_sent_notify_command_write(STREAM* s, UINT32 PingTrafficSent)
{
	stream_write_UINT32(s, RTS_CMD_PING_TRAFFIC_SENT_NOTIFY); /* CommandType (4 bytes) */
	stream_write_UINT32(s, PingTrafficSent); /* PingTrafficSent (4 bytes) */
}

void rts_generate_cookie(BYTE* cookie)
{
	RAND_pseudo_bytes(cookie, 16);
}

BOOL rts_send_CONN_A1_pdu(rdpRpc* rpc)
{
	STREAM* s;
	RTS_PDU_HEADER header;
	UINT32 ReceiveWindowSize;
	BYTE* OUTChannelCookie;
	BYTE* VirtualConnectionCookie;

	rts_pdu_header_init(rpc, &header);

	header.ptype = PTYPE_RTS;
	header.pfc_flags = PFC_FIRST_FRAG | PFC_LAST_FRAG;
	header.frag_length = 76;
	header.auth_length = 0;
	header.call_id = 0;
	header.flags = 0;
	header.numberOfCommands = 4;

	DEBUG_RPC("Sending CONN_A1 RTS PDU");

	s = stream_new(header.frag_length);

	rts_generate_cookie((BYTE*) &(rpc->VirtualConnection->Cookie));
	rts_generate_cookie((BYTE*) &(rpc->VirtualConnection->DefaultOutChannelCookie));

	VirtualConnectionCookie = (BYTE*) &(rpc->VirtualConnection->Cookie);
	OUTChannelCookie = (BYTE*) &(rpc->VirtualConnection->DefaultOutChannelCookie);
	ReceiveWindowSize = rpc->VirtualConnection->DefaultOutChannel->ReceiveWindow;

	stream_write(s, ((BYTE*) &header), 20); /* RTS Header (20 bytes) */
	rts_version_command_write(s); /* Version (8 bytes) */
	rts_cookie_command_write(s, VirtualConnectionCookie); /* VirtualConnectionCookie (20 bytes) */
	rts_cookie_command_write(s, OUTChannelCookie); /* OUTChannelCookie (20 bytes) */
	rts_receive_window_size_command_write(s, ReceiveWindowSize); /* ReceiveWindowSize (8 bytes) */
	stream_seal(s);

	rpc_out_write(rpc, s->data, s->size);

	stream_free(s);

	return TRUE;
}

BOOL rts_send_CONN_B1_pdu(rdpRpc* rpc)
{
	STREAM* s;
	RTS_PDU_HEADER header;
	BYTE* INChannelCookie;
	BYTE* AssociationGroupId;
	BYTE* VirtualConnectionCookie;

	rts_pdu_header_init(rpc, &header);

	header.ptype = PTYPE_RTS;
	header.pfc_flags = PFC_FIRST_FRAG | PFC_LAST_FRAG;
	header.frag_length = 104;
	header.auth_length = 0;
	header.call_id = 0;
	header.flags = 0;
	header.numberOfCommands = 6;

	DEBUG_RPC("Sending CONN_B1 RTS PDU");

	s = stream_new(header.frag_length);

	rts_generate_cookie((BYTE*) &(rpc->VirtualConnection->DefaultInChannelCookie));
	rts_generate_cookie((BYTE*) &(rpc->VirtualConnection->AssociationGroupId));

	VirtualConnectionCookie = (BYTE*) &(rpc->VirtualConnection->Cookie);
	INChannelCookie = (BYTE*) &(rpc->VirtualConnection->DefaultInChannelCookie);
	AssociationGroupId = (BYTE*) &(rpc->VirtualConnection->AssociationGroupId);

	stream_write(s, ((BYTE*) &header), 20); /* RTS Header (20 bytes) */
	rts_version_command_write(s); /* Version (8 bytes) */
	rts_cookie_command_write(s, VirtualConnectionCookie); /* VirtualConnectionCookie (20 bytes) */
	rts_cookie_command_write(s, INChannelCookie); /* INChannelCookie (20 bytes) */
	rts_channel_lifetime_command_write(s, 0x40000000); /* ChannelLifetime (8 bytes) */
	rts_client_keepalive_command_write(s, 0x000493E0); /* ClientKeepalive (8 bytes) */
	rts_association_group_id_command_write(s, AssociationGroupId); /* AssociationGroupId (20 bytes) */
	stream_seal(s);

	rpc_in_write(rpc, s->data, s->size);

	stream_free(s);

	return TRUE;
}

BOOL rts_send_keep_alive_pdu(rdpRpc* rpc)
{
	STREAM* s;
	RTS_PDU_HEADER header;

	rts_pdu_header_init(rpc, &header);

	header.ptype = PTYPE_RTS;
	header.pfc_flags = PFC_FIRST_FRAG | PFC_LAST_FRAG;
	header.frag_length = 28;
	header.auth_length = 0;
	header.call_id = 0;
	header.flags = 2;
	header.numberOfCommands = 1;

	DEBUG_RPC("Sending Keep-Alive RTS PDU");

	s = stream_new(header.frag_length);
	stream_write(s, ((BYTE*) &header), 20); /* RTS Header (20 bytes) */
	rts_client_keepalive_command_write(s, 0x00007530); /* ClientKeepalive (8 bytes) */
	stream_seal(s);

	rpc_in_write(rpc, s->data, s->size);

	stream_free(s);

	return TRUE;
}

BOOL rts_send_flow_control_ack_pdu(rdpRpc* rpc)
{
	STREAM* s;
	RTS_PDU_HEADER header;
	UINT32 BytesReceived;
	UINT32 AvailableWindow;
	BYTE* ChannelCookie;

	rts_pdu_header_init(rpc, &header);

	header.ptype = PTYPE_RTS;
	header.pfc_flags = PFC_FIRST_FRAG | PFC_LAST_FRAG;
	header.frag_length = 56;
	header.auth_length = 0;
	header.call_id = 0;
	header.flags = 2;
	header.numberOfCommands = 2;

	DEBUG_RPC("Sending FlowControlAck RTS PDU");

	BytesReceived = rpc->VirtualConnection->DefaultOutChannel->BytesReceived;
	AvailableWindow = rpc->VirtualConnection->DefaultOutChannel->ReceiverAvailableWindow;
	ChannelCookie = (BYTE*) &(rpc->VirtualConnection->DefaultOutChannelCookie);

	s = stream_new(header.frag_length);
	stream_write(s, ((BYTE*) &header), 20); /* RTS Header (20 bytes) */
	rts_destination_command_write(s, FDOutProxy); /* Destination Command (8 bytes) */

	/* FlowControlAck Command (28 bytes) */
	rts_flow_control_ack_command_write(s, BytesReceived, AvailableWindow, ChannelCookie);

	stream_seal(s);

	rpc_in_write(rpc, s->data, s->size);

	stream_free(s);

	return TRUE;
}

BOOL rts_send_ping_pdu(rdpRpc* rpc)
{
	STREAM* s;
	RTS_PDU_HEADER header;

	rts_pdu_header_init(rpc, &header);

	header.ptype = PTYPE_RTS;
	header.pfc_flags = PFC_FIRST_FRAG | PFC_LAST_FRAG;
	header.frag_length = 20;
	header.auth_length = 0;
	header.call_id = 0;
	header.flags = 1;
	header.numberOfCommands = 0;

	DEBUG_RPC("Sending Ping RTS PDU");

	s = stream_new(header.frag_length);
	stream_write(s, ((BYTE*) &header), 20); /* RTS Header (20 bytes) */
	stream_seal(s);

	rpc_in_write(rpc, s->data, s->size);

	stream_free(s);

	return TRUE;
}

int rts_recv_pdu_commands(rdpRpc* rpc, RTS_PDU* rts_pdu)
{
	int i;
	STREAM* s;
	UINT32 CommandType;

	DEBUG_RTS("numberOfCommands:%d", rts_pdu->header.numberOfCommands);

	if (rts_pdu->header.flags & RTS_FLAG_PING)
	{
		rts_send_keep_alive_pdu(rpc);
		return 0;
	}

	s = stream_new(0);
	stream_attach(s, rts_pdu->content, rts_pdu->header.frag_length);

	for (i = 0; i < rts_pdu->header.numberOfCommands; i++)
	{
		stream_read_UINT32(s, CommandType); /* CommandType (4 bytes) */

		DEBUG_RTS("CommandType: %s (0x%08X)", RTS_CMD_STRINGS[CommandType % 14], CommandType);

		switch (CommandType)
		{
			case RTS_CMD_RECEIVE_WINDOW_SIZE:
				rts_receive_window_size_command_read(rpc, s);
				break;

			case RTS_CMD_FLOW_CONTROL_ACK:
				rts_flow_control_ack_command_read(rpc, s);
				break;

			case RTS_CMD_CONNECTION_TIMEOUT:
				rts_connection_timeout_command_read(rpc, s);
				break;

			case RTS_CMD_COOKIE:
				rts_cookie_command_read(rpc, s);
				break;

			case RTS_CMD_CHANNEL_LIFETIME:
				rts_channel_lifetime_command_read(rpc, s);
				break;

			case RTS_CMD_CLIENT_KEEPALIVE:
				rts_client_keepalive_command_read(rpc, s);
				break;

			case RTS_CMD_VERSION:
				rts_version_command_read(rpc, s);
				break;

			case RTS_CMD_EMPTY:
				rts_empty_command_read(rpc, s);
				break;

			case RTS_CMD_PADDING:
				rts_padding_command_read(rpc, s);
				break;

			case RTS_CMD_NEGATIVE_ANCE:
				rts_negative_ance_command_read(rpc, s);
				break;

			case RTS_CMD_ANCE:
				rts_ance_command_read(rpc, s);
				break;

			case RTS_CMD_CLIENT_ADDRESS:
				rts_client_address_command_read(rpc, s);
				break;

			case RTS_CMD_ASSOCIATION_GROUP_ID:
				rts_association_group_id_command_read(rpc, s);
				break;

			case RTS_CMD_DESTINATION:
				rts_destination_command_read(rpc, s);
				break;

			case RTS_CMD_PING_TRAFFIC_SENT_NOTIFY:
				rts_ping_traffic_sent_notify_command_read(rpc, s);
				break;

			default:
				printf("Error: Unknown RTS Command Type: 0x%x\n", CommandType);
				stream_detach(s) ;
				stream_free(s) ;
				return -1;
				break;
		}
	}

	stream_detach(s);
	stream_free(s);

	return 0;
}

int rts_recv_pdu(rdpRpc* rpc, RTS_PDU* rts_pdu)
{
	int status;
	int length;
	rdpTls* tls_out = rpc->tls_out;

	/* read first 20 bytes to get RTS PDU Header */
	status = tls_read(tls_out, (BYTE*) &(rts_pdu->header), 20);

	if (status <= 0)
	{
		printf("rts_recv_pdu error\n");
		return status;
	}

	length = rts_pdu->header.frag_length - 20;
	rts_pdu->content = (BYTE*) malloc(length);

	status = tls_read(tls_out, rts_pdu->content, length);

	if (status < 0)
	{
		printf("rts_recv_pdu error\n");
		return status;
	}

	if (rts_pdu->header.ptype != PTYPE_RTS)
	{
		printf("rts_recv_pdu error: unexpected ptype: %d\n", rts_pdu->header.ptype);
		return -1;
	}

#ifdef WITH_DEBUG_RTS
	printf("rts_recv_pdu: length: %d\n", length);
	freerdp_hexdump(rts_pdu->content, length);
	printf("\n");
#endif

	rts_recv_pdu_commands(rpc, rts_pdu);

	return rts_pdu->header.frag_length;
}
