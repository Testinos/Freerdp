/**
 * WinPR: Windows Portable Runtime
 * Serial Communication API
 *
 * Copyright 2011 O.S. Systems Software Ltda.
 * Copyright 2011 Eduardo Fiss Beloni <beloni@ossystems.com.br>
 * Copyright 2014 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 * Copyright 2014 Hewlett-Packard Development Company, L.P.
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

#ifndef _WIN32

#include <assert.h>
#include <termios.h>

#include <freerdp/utils/debug.h>

#include "comm_serial_sys.h"


/* 0: B* (Linux termios)
 * 1: CBR_* or actual baud rate
 * 2: BAUD_* (similar to SERIAL_BAUD_*)
 */
static const speed_t _SERCX_SYS_BAUD_TABLE[][3] = {
#ifdef B0
	{B0, 0, 0},	/* hang up */
#endif
#ifdef B50
	{B50, 50, 0},
#endif
#ifdef B75
	{B75, 75, BAUD_075},
#endif
#ifdef B110
	{B110, CBR_110, BAUD_110},
#endif
#ifdef  B134
	{B134, 134, 0 /*BAUD_134_5*/},
#endif
#ifdef  B150
	{B150, 150, BAUD_150},
#endif
#ifdef B200
	{B200, 200, 0},
#endif
#ifdef B300
	{B300, CBR_300, BAUD_300},
#endif
#ifdef B600
	{B600, CBR_600, BAUD_600},
#endif
#ifdef B1200
	{B1200, CBR_1200, BAUD_1200},
#endif
#ifdef B1800
	{B1800, 1800, BAUD_1800},
#endif
#ifdef B2400
	{B2400, CBR_2400, BAUD_2400},
#endif
#ifdef B4800
	{B4800, CBR_4800, BAUD_4800},
#endif
	/* {, ,BAUD_7200} */
#ifdef B9600
	{B9600, CBR_9600, BAUD_9600},
#endif
	/* {, CBR_14400, BAUD_14400},	/\* unsupported on Linux *\/ */
#ifdef B19200
	{B19200, CBR_19200, BAUD_19200},
#endif
#ifdef B38400
	{B38400, CBR_38400, BAUD_38400},
#endif
	/* {, CBR_56000, BAUD_56K},	/\* unsupported on Linux *\/ */
#ifdef  B57600
	{B57600, CBR_57600, BAUD_57600},
#endif
#ifdef B115200
	{B115200, CBR_115200, BAUD_115200},
#endif
	/* {, CBR_128000, BAUD_128K},	/\* unsupported on Linux *\/ */
	/* {, CBR_256000, BAUD_USER},	/\* unsupported on Linux *\/ */
#ifdef B230400
	{B230400, 230400, BAUD_USER},
#endif
#ifdef B460800
	{B460800, 460800, BAUD_USER},
#endif
#ifdef B500000
	{B500000, 500000, BAUD_USER},
#endif
#ifdef  B576000
	{B576000, 576000, BAUD_USER},
#endif
#ifdef B921600
	{B921600, 921600, BAUD_USER},
#endif
#ifdef B1000000
	{B1000000, 1000000, BAUD_USER},
#endif
#ifdef B1152000
	{B1152000, 1152000, BAUD_USER},
#endif
#ifdef B1500000
	{B1500000, 1500000, BAUD_USER},
#endif
#ifdef B2000000
	{B2000000, 2000000, BAUD_USER},
#endif
#ifdef B2500000
	{B2500000, 2500000, BAUD_USER},
#endif
#ifdef B3000000
	{B3000000, 3000000, BAUD_USER},
#endif
#ifdef B3500000
	{B3500000, 3500000, BAUD_USER},
#endif
#ifdef B4000000
	{B4000000, 4000000, BAUD_USER},	/* __MAX_BAUD */
#endif
};


static BOOL _get_properties(WINPR_COMM *pComm, COMMPROP *pProperties)
{
	int i;
	REMOTE_SERIAL_DRIVER* pSerialSys = SerialSys_s();

	if (!pSerialSys->get_properties(pComm, pProperties))
	{
		return FALSE;
	}

	/* override some of the inherited properties from SerialSys ... */

	pProperties->dwMaxBaud = BAUD_USER;

	pProperties->dwSettableBaud = 0;
	for (i=0; _SERCX_SYS_BAUD_TABLE[i][0]<=__MAX_BAUD; i++)
	{
		pProperties->dwSettableBaud |= _SERCX_SYS_BAUD_TABLE[i][2];
	}

	return TRUE;
}


static BOOL _set_baud_rate(WINPR_COMM *pComm, const SERIAL_BAUD_RATE *pBaudRate)
{
	int i;
	speed_t newSpeed;
	struct termios futureState;

	ZeroMemory(&futureState, sizeof(struct termios));
	if (tcgetattr(pComm->fd, &futureState) < 0) /* NB: preserves current settings not directly handled by the Communication Functions */
	{
		SetLastError(ERROR_IO_DEVICE);
		return FALSE;
	}

	for (i=0; _SERCX_SYS_BAUD_TABLE[i][0]<=__MAX_BAUD; i++)
	{
		if (_SERCX_SYS_BAUD_TABLE[i][1] == pBaudRate->BaudRate)
		{
			newSpeed = _SERCX_SYS_BAUD_TABLE[i][0];	
			if (cfsetspeed(&futureState, newSpeed) < 0)
			{
				DEBUG_WARN("failed to set speed 0x%x (%d)", newSpeed, pBaudRate->BaudRate);
				return FALSE;
			}

			assert(cfgetispeed(&futureState) == newSpeed);

			if (_comm_ioctl_tcsetattr(pComm->fd, TCSANOW, &futureState) < 0)
			{
				DEBUG_WARN("_comm_ioctl_tcsetattr failure: last-error: 0x%0.8x", GetLastError());
				return FALSE;
			}

			return TRUE;
		}
	}

	DEBUG_WARN("could not find a matching speed for the baud rate %d", pBaudRate->BaudRate);
	SetLastError(ERROR_INVALID_DATA);
	return FALSE;
}


static BOOL _get_baud_rate(WINPR_COMM *pComm, SERIAL_BAUD_RATE *pBaudRate)
{
	int i;
	speed_t currentSpeed;
	struct termios currentState;

	ZeroMemory(&currentState, sizeof(struct termios));
	if (tcgetattr(pComm->fd, &currentState) < 0)
	{
		SetLastError(ERROR_IO_DEVICE);
		return FALSE;
	}

	currentSpeed = cfgetispeed(&currentState);

	for (i=0; _SERCX_SYS_BAUD_TABLE[i][0]<=__MAX_BAUD; i++)
	{
		if (_SERCX_SYS_BAUD_TABLE[i][0] == currentSpeed)
		{
			pBaudRate->BaudRate = _SERCX_SYS_BAUD_TABLE[i][1];
			return TRUE;
		}
	}

	DEBUG_WARN("could not find a matching baud rate for the speed 0x%x", currentSpeed);
	SetLastError(ERROR_INVALID_DATA);
	return FALSE;
}

/* hard-coded in N_TTY */
#define TTY_THRESHOLD_THROTTLE		128 /* now based on remaining room */
#define TTY_THRESHOLD_UNTHROTTLE 	128

/* FIXME: mostly copied/pasted from comm_serial_sys.c, better share this code */
static BOOL _set_handflow(WINPR_COMM *pComm, const SERIAL_HANDFLOW *pHandflow)
{
	BOOL result = TRUE;
	struct termios upcomingTermios;

	/* logical XOR */
	if ((!(pHandflow->ControlHandShake & SERIAL_DTR_CONTROL) && (pHandflow->FlowReplace & SERIAL_RTS_CONTROL)) ||
	    ((pHandflow->ControlHandShake & SERIAL_DTR_CONTROL) && !(pHandflow->FlowReplace & SERIAL_RTS_CONTROL)))
	{
		DEBUG_WARN("SERIAL_DTR_CONTROL cannot be different SERIAL_RTS_CONTROL, HUPCL will be set according SERIAL_RTS_CONTROL.");
		result = FALSE; /* but keep on */
	}

	ZeroMemory(&upcomingTermios, sizeof(struct termios));
	if (tcgetattr(pComm->fd, &upcomingTermios) < 0)
	{
		SetLastError(ERROR_IO_DEVICE);
		return FALSE;
	}

	/* ControlHandShake */

	if (pHandflow->ControlHandShake & SERIAL_DTR_CONTROL)
	{
		upcomingTermios.c_cflag |= HUPCL;
	}
	else
	{
		upcomingTermios.c_cflag &= ~HUPCL;

		/* FIXME: is the DTR line also needs to be forced to a disable state? */
	}

	if (pHandflow->ControlHandShake & SERIAL_DTR_HANDSHAKE)
	{
		/* DTR/DSR flow control not supported on Linux */
		DEBUG_WARN("Attempt to use the unsupported SERIAL_DTR_HANDSHAKE feature.");
		SetLastError(ERROR_NOT_SUPPORTED);
		result = FALSE; /* but keep on */
	}

	if (pHandflow->ControlHandShake & SERIAL_CTS_HANDSHAKE)
	{
		upcomingTermios.c_cflag |= CRTSCTS;
	}
	else
	{
		upcomingTermios.c_cflag &= ~CRTSCTS;
	}

	if (pHandflow->ControlHandShake & SERIAL_DSR_HANDSHAKE)
	{
		/* DTR/DSR flow control not supported on Linux */
		DEBUG_WARN("Attempt to use the unsupported SERIAL_DSR_HANDSHAKE feature.");
		SetLastError(ERROR_NOT_SUPPORTED);
		result = FALSE; /* but keep on */
	}

	/* SERIAL_DCD_HANDSHAKE unsupported by SerCx */
	if (pHandflow->ControlHandShake & SERIAL_DCD_HANDSHAKE)
	{
		DEBUG_WARN("Attempt to set SERIAL_DCD_HANDSHAKE (not implemented)");
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		result = FALSE; /* but keep on */
	}

	/* SERIAL_DSR_SENSITIVITY unsupported by SerCx */
	if (pHandflow->ControlHandShake & SERIAL_DSR_SENSITIVITY)
	{
		DEBUG_WARN("Attempt to set SERIAL_DSR_SENSITIVITY (not implemented)");
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		result = FALSE; /* but keep on */
	}

	/* SERIAL_ERROR_ABORT unsupported by SerCx */
	if (pHandflow->ControlHandShake & SERIAL_ERROR_ABORT)
	{
		DEBUG_WARN("Attempt to set SERIAL_ERROR_ABORT (not implemented)");
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		result = FALSE; /* but keep on */
	}


	/* FlowReplace */

	/* SERIAL_AUTO_TRANSMIT unsupported by SerCx */
	if (pHandflow->FlowReplace & SERIAL_AUTO_TRANSMIT)
	{
		DEBUG_WARN("Attempt to set SERIAL_AUTO_TRANSMIT (not implemented)");
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		result = FALSE; /* but keep on */
	}


	/* SERIAL_AUTO_RECEIVE unsupported by SerCx */
	if (pHandflow->FlowReplace & SERIAL_AUTO_RECEIVE)
	{
		DEBUG_WARN("Attempt to set SERIAL_AUTO_RECEIVE (not implemented)");
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		result = FALSE; /* but keep on */
	}

	/* SERIAL_ERROR_CHAR unsupported by SerCx */
	if (pHandflow->FlowReplace & SERIAL_ERROR_CHAR)
	{
		DEBUG_WARN("Attempt to set SERIAL_ERROR_CHAR (not implemented)");
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		result = FALSE; /* but keep on */
	}

	/* SERIAL_NULL_STRIPPING unsupported by SerCx */
	if (pHandflow->FlowReplace & SERIAL_NULL_STRIPPING)
	{
		DEBUG_WARN("Attempt to set SERIAL_NULL_STRIPPING (not implemented)");
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		result = FALSE; /* but keep on */
	}

	/* SERIAL_BREAK_CHAR unsupported by SerCx */
	if (pHandflow->FlowReplace & SERIAL_BREAK_CHAR)
	{
		DEBUG_WARN("Attempt to set SERIAL_BREAK_CHAR (not implemented)");
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		result = FALSE; /* but keep on */
	}

	if (pHandflow->FlowReplace & SERIAL_RTS_CONTROL)
	{
		upcomingTermios.c_cflag |= HUPCL;
	}
	else
	{
		upcomingTermios.c_cflag &= ~HUPCL;

		/* FIXME: is the RTS line also needs to be forced to a disable state? */
	}

	if (pHandflow->FlowReplace & SERIAL_RTS_HANDSHAKE)
	{
		upcomingTermios.c_cflag |= CRTSCTS;
	}
	else
	{
		upcomingTermios.c_cflag &= ~CRTSCTS;
	}

	/* SERIAL_XOFF_CONTINUE unsupported by SerCx */
	if (pHandflow->FlowReplace & SERIAL_XOFF_CONTINUE)
	{
		DEBUG_WARN("Attempt to set SERIAL_XOFF_CONTINUE (not implemented)");
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
		result = FALSE; /* but keep on */
	}


	/* XonLimit */
	
	// FIXME: could be implemented during read/write I/O
	if (pHandflow->XonLimit != TTY_THRESHOLD_UNTHROTTLE)
	{
		DEBUG_WARN("Attempt to set XonLimit with an unsupported value: %d", pHandflow->XonLimit);
		SetLastError(ERROR_NOT_SUPPORTED);
		result = FALSE; /* but keep on */		
	}

	/* XoffChar */

	// FIXME: could be implemented during read/write I/O
	if (pHandflow->XoffLimit != TTY_THRESHOLD_THROTTLE)
	{
		DEBUG_WARN("Attempt to set XoffLimit with an unsupported value: %d", pHandflow->XoffLimit);
		SetLastError(ERROR_NOT_SUPPORTED);
		result = FALSE; /* but keep on */		
	}
	

	if (_comm_ioctl_tcsetattr(pComm->fd, TCSANOW, &upcomingTermios) < 0)
	{
		DEBUG_WARN("_comm_ioctl_tcsetattr failure: last-error: 0x0.8x", GetLastError());
		return FALSE;
	}

	return result;
}


/* FIXME: mostly copied/pasted from comm_serial_sys.c, better share this code */
static BOOL _get_handflow(WINPR_COMM *pComm, SERIAL_HANDFLOW *pHandflow)
{
	struct termios currentTermios;

	ZeroMemory(&currentTermios, sizeof(struct termios));
	if (tcgetattr(pComm->fd, &currentTermios) < 0)
	{
		SetLastError(ERROR_IO_DEVICE);
		return FALSE;
	}


	/* ControlHandShake */

	pHandflow->ControlHandShake = 0;

	if (currentTermios.c_cflag & HUPCL)
		pHandflow->ControlHandShake |= SERIAL_DTR_CONTROL;

	/* SERIAL_DTR_HANDSHAKE unsupported */
			
	if (currentTermios.c_cflag & CRTSCTS)
		pHandflow->ControlHandShake |= SERIAL_CTS_HANDSHAKE;

	/* SERIAL_DSR_HANDSHAKE unsupported */

	/* SERIAL_DCD_HANDSHAKE unsupported by SerCx */

	/* SERIAL_DSR_SENSITIVITY unsupported by SerCx */ 

	/* SERIAL_ERROR_ABORT unsupported by SerCx */


	/* FlowReplace */

	pHandflow->FlowReplace = 0;

	/* SERIAL_AUTO_TRANSMIT unsupported by SerCx */

	/* SERIAL_AUTO_RECEIVE unsupported by SerCx */

	/* SERIAL_ERROR_CHAR unsupported by SerCx */

	/* SERIAL_NULL_STRIPPING unsupported by SerCx */

	/* SERIAL_BREAK_CHAR unsupported by SerCx */

	if (currentTermios.c_cflag & HUPCL)
		pHandflow->FlowReplace |= SERIAL_RTS_CONTROL;

	if (currentTermios.c_cflag & CRTSCTS)
		pHandflow->FlowReplace |= SERIAL_RTS_HANDSHAKE;

	/* SERIAL_XOFF_CONTINUE unsupported by SerCx */


	/* XonLimit */

	pHandflow->XonLimit = TTY_THRESHOLD_UNTHROTTLE;


	/* XoffLimit */

	pHandflow->XoffLimit = TTY_THRESHOLD_THROTTLE;

	return TRUE;
}



/* specific functions only */
static REMOTE_SERIAL_DRIVER _SerCxSys = 
{
	.id		  = RemoteSerialDriverSerCxSys,
	.name		  = _T("SerCx.sys"),
	.set_baud_rate    = _set_baud_rate,
	.get_baud_rate    = _get_baud_rate,
	.get_properties   = _get_properties,
	.set_serial_chars = NULL,
	.get_serial_chars = NULL,
	.set_line_control = NULL,
	.get_line_control = NULL,
	.set_handflow     = _set_handflow,
	.get_handflow     = _get_handflow,
	.set_timeouts     = NULL,
	.get_timeouts     = NULL,
	.set_dtr          = NULL,
	.clear_dtr        = NULL,
	.set_rts          = NULL,
	.clear_rts        = NULL,
	.get_modemstatus  = NULL,
};



REMOTE_SERIAL_DRIVER* SerCxSys_s()
{
	/* _SerCxSys completed with inherited functions from SerialSys */
	REMOTE_SERIAL_DRIVER* pSerialSys = SerialSys_s();

	_SerCxSys.set_serial_chars = pSerialSys->set_serial_chars;
	_SerCxSys.get_serial_chars = pSerialSys->get_serial_chars;
	_SerCxSys.set_line_control = pSerialSys->set_line_control;
	_SerCxSys.get_line_control = pSerialSys->get_line_control;

	_SerCxSys.set_timeouts = pSerialSys->set_timeouts;
	_SerCxSys.get_timeouts = pSerialSys->get_timeouts;

	_SerCxSys.set_dtr = pSerialSys->set_dtr;
	_SerCxSys.clear_dtr = pSerialSys->clear_dtr;

	_SerCxSys.set_rts = pSerialSys->set_rts;
	_SerCxSys.clear_rts = pSerialSys->clear_rts;

	_SerCxSys.get_modemstatus = pSerialSys->get_modemstatus;

	return &_SerCxSys;
}

#endif /* _WIN32 */
