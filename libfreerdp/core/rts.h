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

#ifndef FREERDP_CORE_RTS_H
#define FREERDP_CORE_RTS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

typedef struct _rts_pdu RTS_PDU;
typedef struct _rts_pdu_header RTS_PDU_HEADER;

#include "rpc.h"

#include <freerdp/api.h>
#include <freerdp/types.h>
#include <freerdp/utils/debug.h>
#include <freerdp/utils/stream.h>

#define RTS_FLAG_NONE				0x0000
#define RTS_FLAG_PING				0x0001
#define RTS_FLAG_OTHER_CMD			0x0002
#define RTS_FLAG_RECYCLE_CHANNEL		0x0004
#define RTS_FLAG_IN_CHANNEL			0x0008
#define RTS_FLAG_OUT_CHANNEL			0x0010
#define RTS_FLAG_EOF				0x0020
#define RTS_FLAG_ECHO				0x0040

#define RTS_CMD_RECEIVE_WINDOW_SIZE		0x00000000
#define RTS_CMD_FLOW_CONTROL_ACK		0x00000001
#define RTS_CMD_CONNECTION_TIMEOUT		0x00000002
#define RTS_CMD_COOKIE				0x00000003
#define RTS_CMD_CHANNEL_LIFETIME		0x00000004
#define RTS_CMD_CLIENT_KEEPALIVE		0x00000005
#define RTS_CMD_VERSION	 			0x00000006
#define RTS_CMD_EMPTY				0x00000007
#define RTS_CMD_PADDING				0x00000008
#define RTS_CMD_NEGATIVE_ANCE			0x00000009
#define RTS_CMD_ANCE				0x0000000A
#define RTS_CMD_CLIENT_ADDRESS			0x0000000B
#define RTS_CMD_ASSOCIATION_GROUP_ID		0x0000000C
#define RTS_CMD_DESTINATION			0x0000000D
#define RTS_CMD_PING_TRAFFIC_SENT_NOTIFY	0x0000000E

#define FDClient				0x00000000
#define FDInProxy				0x00000001
#define FDServer				0x00000002
#define FDOutProxy				0x00000003

struct _rts_pdu_header
{
	DEFINE_RPC_COMMON_FIELDS();

	UINT16 flags;
	UINT16 numberOfCommands;
};

struct _rts_pdu
{
	RTS_PDU_HEADER header;
	BYTE* content;
};

BOOL rts_connect(rdpRpc* rpc);

void rts_receive_window_size_command_read(rdpRpc* rpc, STREAM* s);
int rts_receive_window_size_command_write(BYTE* buffer, UINT32 ReceiveWindowSize);

void rts_flow_control_ack_command_read(rdpRpc* rpc, STREAM* s);
int rts_flow_control_ack_command_write(BYTE* buffer, UINT32 BytesReceived, UINT32 AvailableWindow, BYTE* ChannelCookie);

void rts_connection_timeout_command_read(rdpRpc* rpc, STREAM* s);
int rts_connection_timeout_command_write(BYTE* buffer, UINT32 ConnectionTimeout);

void rts_cookie_command_read(rdpRpc* rpc, STREAM* s);
int rts_cookie_command_write(BYTE* buffer, BYTE* Cookie);

void rts_channel_lifetime_command_read(rdpRpc* rpc, STREAM* s);
int rts_channel_lifetime_command_write(BYTE* buffer, UINT32 ChannelLifetime);

void rts_client_keepalive_command_read(rdpRpc* rpc, STREAM* s);
int rts_client_keepalive_command_write(BYTE* buffer, UINT32 ClientKeepalive);

void rts_version_command_read(rdpRpc* rpc, STREAM* s);
int rts_version_command_write(BYTE* buffer);

void rts_empty_command_read(rdpRpc* rpc, STREAM* s);
int rts_empty_command_write(BYTE* buffer);

void rts_padding_command_read(rdpRpc* rpc, STREAM* s);
int rts_padding_command_write(BYTE* buffer, UINT32 ConformanceCount);

void rts_negative_ance_command_read(rdpRpc* rpc, STREAM* s);
int rts_negative_ance_command_write(BYTE* buffer);

void rts_ance_command_read(rdpRpc* rpc, STREAM* s);
int rts_ance_command_write(BYTE* buffer);

void rts_client_address_command_read(rdpRpc* rpc, STREAM* s);
int rts_client_address_command_write(BYTE* buffer, UINT32 AddressType, BYTE* ClientAddress);

void rts_association_group_id_command_read(rdpRpc* rpc, STREAM* s);
int rts_association_group_id_command_write(BYTE* buffer, BYTE* AssociationGroupId);

void rts_destination_command_read(rdpRpc* rpc, STREAM* s);
int rts_destination_command_write(BYTE* buffer, UINT32 Destination);

void rts_ping_traffic_sent_notify_command_read(rdpRpc* rpc, STREAM* s);
int rts_ping_traffic_sent_notify_command_write(BYTE* buffer, UINT32 PingTrafficSent);

BOOL rts_send_CONN_A1_pdu(rdpRpc* rpc);
BOOL rts_send_CONN_B1_pdu(rdpRpc* rpc);
BOOL rts_send_keep_alive_pdu(rdpRpc* rpc);
BOOL rts_send_flow_control_ack_pdu(rdpRpc* rpc);
BOOL rts_send_ping_pdu(rdpRpc* rpc);

int rts_recv_pdu(rdpRpc* rpc, RTS_PDU* rts_pdu);

#ifdef WITH_DEBUG_TSG
#define WITH_DEBUG_RTS
#endif

#ifdef WITH_DEBUG_RTS
#define DEBUG_RTS(fmt, ...) DEBUG_CLASS(RTS, fmt, ## __VA_ARGS__)
#else
#define DEBUG_RTS(fmt, ...) DEBUG_NULL(fmt, ## __VA_ARGS__)
#endif

#endif /* FREERDP_CORE_RTS_H */
