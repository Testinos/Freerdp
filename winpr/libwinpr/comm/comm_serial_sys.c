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
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>

#include <freerdp/utils/debug.h>

#include "comm_serial_sys.h"

#include <winpr/crt.h>

/*
 * Linux, Windows speeds
 * 
 */
static const speed_t _SERIAL_SYS_BAUD_TABLE[][2] = {
#ifdef B0
	{B0, 0},	/* hang up */
#endif
/* #ifdef B50 */
/* 	{B50, },	/\* undefined by serial.sys *\/  */
/* #endif */
#ifdef B75
	{B75, SERIAL_BAUD_075},
#endif
#ifdef B110
	{B110, SERIAL_BAUD_110},
#endif
/* #ifdef  B134 */
/* 	{B134, SERIAL_BAUD_134_5}, /\* TODO: might be the same? *\/ */
/* #endif */
#ifdef  B150
	{B150, SERIAL_BAUD_150},
#endif
/* #ifdef B200 */
/* 	{B200, },	/\* undefined by serial.sys *\/ */
/* #endif */
#ifdef B300
	{B300, SERIAL_BAUD_300},
#endif
#ifdef B600
	{B600, SERIAL_BAUD_600},
#endif
#ifdef B1200
	{B1200, SERIAL_BAUD_1200},
#endif
#ifdef B1800
	{B1800, SERIAL_BAUD_1800},
#endif
#ifdef B2400
	{B2400, SERIAL_BAUD_2400},
#endif
#ifdef B4800
	{B4800, SERIAL_BAUD_4800},
#endif
	/* {, SERIAL_BAUD_7200} /\* undefined on Linux *\/ */
#ifdef B9600
	{B9600, SERIAL_BAUD_9600},
#endif
	/* {, SERIAL_BAUD_14400} /\* undefined on Linux *\/ */
#ifdef B19200
	{B19200, SERIAL_BAUD_19200},
#endif
#ifdef B38400
	{B38400, SERIAL_BAUD_38400},
#endif
/* 	{, SERIAL_BAUD_56K},	/\* undefined on Linux *\/ */
#ifdef  B57600
	{B57600, SERIAL_BAUD_57600},
#endif
	/* {, SERIAL_BAUD_128K} /\* undefined on Linux *\/ */
#ifdef B115200
	{B115200, SERIAL_BAUD_115200},	/* _SERIAL_MAX_BAUD */
#endif
/* undefined by serial.sys:
#ifdef B230400
	{B230400, },
#endif
#ifdef B460800
	{B460800, },
#endif
#ifdef B500000
	{B500000, },
#endif
#ifdef  B576000
	{B576000, },
#endif
#ifdef B921600
	{B921600, },
#endif
#ifdef B1000000
	{B1000000, },
#endif
#ifdef B1152000
	{B1152000, },
#endif
#ifdef B1500000
	{B1500000, },
#endif
#ifdef B2000000
	{B2000000, },
#endif
#ifdef B2500000
	{B2500000, },
#endif
#ifdef B3000000
	{B3000000, },
#endif
#ifdef B3500000
	{B3500000, },
#endif
#ifdef B4000000
	{B4000000, },	 __MAX_BAUD
#endif
*/
};

#define _SERIAL_MAX_BAUD  B115200


static BOOL _get_properties(WINPR_COMM *pComm, COMMPROP *pProperties)
{
	int i;

	/* http://msdn.microsoft.com/en-us/library/windows/hardware/jj680684%28v=vs.85%29.aspx
	 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa363189%28v=vs.85%29.aspx
	 */

	/* FIXME: properties should be better probe. The current
	 * implementation just relies on the Linux' implementation.
	 */

	// TMP: TODO:


	if (pProperties->dwProvSpec1 != COMMPROP_INITIALIZED)
	{
		ZeroMemory(pProperties, sizeof(COMMPROP));
		pProperties->wPacketLength = sizeof(COMMPROP);
	}

	pProperties->wPacketVersion = 2;

	pProperties->dwServiceMask = SERIAL_SP_SERIALCOMM;

	/* pProperties->Reserved1; not used */

	// TMP: FIXME: related to the UART's FIFO ? 
	/* pProperties->MaxTxQueue; */
	/* pProperties->MaxRxQueue; */

	pProperties->dwMaxBaud = SERIAL_BAUD_115200; /* _SERIAL_MAX_BAUD */

	/* FIXME: what about PST_RS232? */
	pProperties->dwProvSubType = PST_UNSPECIFIED;

	/* TMP: TODO: to be finalized */
	pProperties->dwProvCapabilities = 
		/*PCF_16BITMODE | PCF_DTRDSR | PCF_INTTIMEOUTS |*/ PCF_PARITY_CHECK | /*PCF_RLSD | */
		PCF_RTSCTS | PCF_SETXCHAR | /*PCF_SPECIALCHARS | PCF_TOTALTIMEOUTS |*/ PCF_XONXOFF;

	/* TMP: TODO: double check SP_RLSD */
	pProperties->dwSettableParams = SP_BAUD | SP_DATABITS | SP_HANDSHAKING | SP_PARITY | SP_PARITY_CHECK | /*SP_RLSD |*/ SP_STOPBITS;

	pProperties->dwSettableBaud = 0;
	for (i=0; _SERIAL_SYS_BAUD_TABLE[i][0]<=_SERIAL_MAX_BAUD; i++)
	{
		pProperties->dwSettableBaud |= _SERIAL_SYS_BAUD_TABLE[i][1];
	}

	pProperties->wSettableData = DATABITS_5 | DATABITS_6 | DATABITS_7 | DATABITS_8 /*| DATABITS_16 | DATABITS_16X*/;

	pProperties->wSettableStopParity = STOPBITS_10 | /*STOPBITS_15 |*/ STOPBITS_20 | PARITY_NONE | PARITY_ODD | PARITY_EVEN | PARITY_MARK | PARITY_SPACE;

	// TMP: FIXME: related to the UART's FIFO ? 
	/* pProperties->CurrentTxQueue; */
	/* pProperties->CurrentRxQueue; */

	/* pProperties->ProvSpec1; see above */
	/* pProperties->ProvSpec2; ignored */
	/* pProperties->ProvChar[1]; ignored */

	return TRUE;
}

static BOOL _set_baud_rate(WINPR_COMM *pComm, const SERIAL_BAUD_RATE *pBaudRate)
{
	int i;
	speed_t newSpeed;
	struct termios upcomingTermios;

	ZeroMemory(&upcomingTermios, sizeof(struct termios));
	if (tcgetattr(pComm->fd, &upcomingTermios) < 0)
	{
		SetLastError(ERROR_IO_DEVICE);
		return FALSE;
	}

	for (i=0; _SERIAL_SYS_BAUD_TABLE[i][0]<=_SERIAL_MAX_BAUD; i++)
	{
		if (_SERIAL_SYS_BAUD_TABLE[i][1] == pBaudRate->BaudRate)
		{
			newSpeed = _SERIAL_SYS_BAUD_TABLE[i][0];
			if (cfsetspeed(&upcomingTermios, newSpeed) < 0)
			{
				DEBUG_WARN("failed to set speed %d (%d)", newSpeed, pBaudRate->BaudRate);
				return FALSE;
			}

			if (_comm_ioctl_tcsetattr(pComm->fd, TCSANOW, &upcomingTermios) < 0)
			{
				DEBUG_WARN("_comm_ioctl_tcsetattr failure: last-error: 0x0.8x", GetLastError());
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

	for (i=0; _SERIAL_SYS_BAUD_TABLE[i][0]<=_SERIAL_MAX_BAUD; i++)
	{
		if (_SERIAL_SYS_BAUD_TABLE[i][0] == currentSpeed)
		{
			pBaudRate->BaudRate = _SERIAL_SYS_BAUD_TABLE[i][1];
			return TRUE;
		}
	}

	DEBUG_WARN("could not find a matching baud rate for the speed 0x%x", currentSpeed);
	SetLastError(ERROR_INVALID_DATA);
	return FALSE;
}

/**
 * NOTE: Only XonChar and XoffChar are plenty supported with the Linux
 *       N_TTY line discipline.
 *
 * ERRORS:
 *   ERROR_IO_DEVICE
 *   ERROR_INVALID_PARAMETER when Xon and Xoff chars are the same;
 *   ERROR_NOT_SUPPORTED
 */
static BOOL _set_serial_chars(WINPR_COMM *pComm, const SERIAL_CHARS *pSerialChars)
{
	BOOL result = TRUE;
	struct termios upcomingTermios;

	ZeroMemory(&upcomingTermios, sizeof(struct termios));
	if (tcgetattr(pComm->fd, &upcomingTermios) < 0)
	{
		SetLastError(ERROR_IO_DEVICE);
		return FALSE;
	}

	if (pSerialChars->XonChar == pSerialChars->XoffChar)
	{
		/* http://msdn.microsoft.com/en-us/library/windows/hardware/ff546688%28v=vs.85%29.aspx */
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	/* termios(3): (..) above symbolic subscript values are all
         * different, except that VTIME, VMIN may have the same value
         * as VEOL, VEOF, respectively. In noncanonical mode the
         * special character meaning is replaced by the timeout
         * meaning.
	 *
	 * EofChar and c_cc[VEOF] are not quite the same, prefer to
	 * don't use c_cc[VEOF] at all.
	 *
	 * FIXME: might be implemented during read/write I/O
	 */
	if (pSerialChars->EofChar != '\0')
	{
		DEBUG_WARN("EofChar='%c' cannot be set\n", pSerialChars->EofChar);
		SetLastError(ERROR_NOT_SUPPORTED);
		result = FALSE; /* but keep on */
	}

	/* According the Linux's n_tty discipline, charaters with a
	 * parity error can only be let unchanged, replaced by \0 or
	 * get the prefix the prefix \377 \0 
	 */

	/* FIXME: see also: _set_handflow() */
	if (pSerialChars->ErrorChar != '\0')
	{
		DEBUG_WARN("ErrorChar='%c' (0x%x) cannot be set (unsupported).\n", pSerialChars->ErrorChar, pSerialChars->ErrorChar);
		SetLastError(ERROR_NOT_SUPPORTED);
		result = FALSE; /* but keep on */
	}

	/* FIXME: see also: _set_handflow() */
	if (pSerialChars->BreakChar != '\0')
	{
		DEBUG_WARN("BreakChar='%c' (0x%x) cannot be set (unsupported).\n", pSerialChars->BreakChar, pSerialChars->BreakChar);
		SetLastError(ERROR_NOT_SUPPORTED);
		result = FALSE; /* but keep on */
	}

	/* TMP: FIXME: Didn't find anything similar yet on Linux */
	if (pSerialChars->EventChar != '\0')
	{
		DEBUG_WARN("EventChar='%c' (0x%x) cannot be set\n", pSerialChars->EventChar, pSerialChars->EventChar);
		SetLastError(ERROR_NOT_SUPPORTED);
		result = FALSE; /* but keep on */
	}

	upcomingTermios.c_cc[VSTART] = pSerialChars->XonChar;

	upcomingTermios.c_cc[VSTOP] = pSerialChars->XoffChar;


	if (_comm_ioctl_tcsetattr(pComm->fd, TCSANOW, &upcomingTermios) < 0)
	{
		DEBUG_WARN("_comm_ioctl_tcsetattr failure: last-error: 0x0.8x", GetLastError());
		return FALSE;
	}

	return result;
}


static BOOL _get_serial_chars(WINPR_COMM *pComm, SERIAL_CHARS *pSerialChars)
{
	struct termios currentTermios;

	ZeroMemory(&currentTermios, sizeof(struct termios));
	if (tcgetattr(pComm->fd, &currentTermios) < 0)
	{
		SetLastError(ERROR_IO_DEVICE);
		return FALSE;
	}

	ZeroMemory(pSerialChars, sizeof(SERIAL_CHARS));
	
	/* EofChar unsupported */

	/* ErrorChar unsupported */

	/* BreakChar unsupported */
	
	/* TMP: FIXME: see also: _set_serial_chars() */
	/* EventChar */

	pSerialChars->XonChar = currentTermios.c_cc[VSTART];

	pSerialChars->XoffChar = currentTermios.c_cc[VSTOP];

	return TRUE;
}


static BOOL _set_line_control(WINPR_COMM *pComm, const SERIAL_LINE_CONTROL *pLineControl)
{
	BOOL result = TRUE;
	struct termios upcomingTermios;


	/* http://msdn.microsoft.com/en-us/library/windows/desktop/aa363214%28v=vs.85%29.aspx
	 *
	 * The use of 5 data bits with 2 stop bits is an invalid
	 * combination, as is 6, 7, or 8 data bits with 1.5 stop bits.
	 *
	 * FIXME: prefered to let the underlying driver to deal with
	 * this issue. At least produce a warning message?
	 */


	ZeroMemory(&upcomingTermios, sizeof(struct termios));
	if (tcgetattr(pComm->fd, &upcomingTermios) < 0)
	{
		SetLastError(ERROR_IO_DEVICE);
		return FALSE;
	}

	/* FIXME: use of a COMMPROP to validate new settings? */

	switch (pLineControl->StopBits)
	{
		case STOP_BIT_1:
			upcomingTermios.c_cflag &= ~CSTOPB;
			break;

		case STOP_BITS_1_5:
			DEBUG_WARN("Unsupported one and a half stop bits.");
			break;
			
		case STOP_BITS_2:
			upcomingTermios.c_cflag |= CSTOPB;
			break;

		default:
			DEBUG_WARN("unexpected number of stop bits: %d\n", pLineControl->StopBits);
			result = FALSE; /* but keep on */
			break;
	} 


	switch (pLineControl->Parity)
	{
		case NO_PARITY:
			upcomingTermios.c_cflag &= ~(PARENB | PARODD | CMSPAR);
			break;

		case ODD_PARITY:
			upcomingTermios.c_cflag &= ~CMSPAR;
			upcomingTermios.c_cflag |= PARENB | PARODD;
			break;

		case EVEN_PARITY:
			upcomingTermios.c_cflag &= ~(PARODD | CMSPAR);
			upcomingTermios.c_cflag |= PARENB;
			break;

		case MARK_PARITY:
			upcomingTermios.c_cflag |= PARENB | PARODD | CMSPAR;
			break;

		case SPACE_PARITY:
			upcomingTermios.c_cflag &= ~PARODD;
			upcomingTermios.c_cflag |= PARENB | CMSPAR;
			break;

		default:
			DEBUG_WARN("unexpected type of parity: %d\n", pLineControl->Parity);
			result = FALSE; /* but keep on */
			break;
	}

	switch (pLineControl->WordLength)
	{
		case 5:
			upcomingTermios.c_cflag &= ~CSIZE;
			upcomingTermios.c_cflag |= CS5;
			break;

		case 6:
			upcomingTermios.c_cflag &= ~CSIZE;
			upcomingTermios.c_cflag |= CS6;
			break;

		case 7:
			upcomingTermios.c_cflag &= ~CSIZE;
			upcomingTermios.c_cflag |= CS7;
			break;

		case 8:
			upcomingTermios.c_cflag &= ~CSIZE;
			upcomingTermios.c_cflag |= CS8;
			break;

		default:
			DEBUG_WARN("unexpected number od data bits per character: %d\n", pLineControl->WordLength);
			result = FALSE; /* but keep on */
			break;
	}

	if (_comm_ioctl_tcsetattr(pComm->fd, TCSANOW, &upcomingTermios) < 0)
	{
		DEBUG_WARN("_comm_ioctl_tcsetattr failure: last-error: 0x0.8x", GetLastError());
		return FALSE;
	}

	return result;
}


static BOOL _get_line_control(WINPR_COMM *pComm, SERIAL_LINE_CONTROL *pLineControl)
{
	struct termios currentTermios;

	ZeroMemory(&currentTermios, sizeof(struct termios));
	if (tcgetattr(pComm->fd, &currentTermios) < 0)
	{
		SetLastError(ERROR_IO_DEVICE);
		return FALSE;
	}

	pLineControl->StopBits = (currentTermios.c_cflag & CSTOPB) ? STOP_BITS_2 : STOP_BIT_1;

	if (!(currentTermios.c_cflag & PARENB))
	{
		pLineControl->Parity = NO_PARITY;
	}
	else if (currentTermios.c_cflag & CMSPAR)
	{
		pLineControl->Parity = (currentTermios.c_cflag & PARODD) ? MARK_PARITY : SPACE_PARITY;
	}
	else
	{
		/* PARENB is set */
		pLineControl->Parity = (currentTermios.c_cflag & PARODD) ?  ODD_PARITY : EVEN_PARITY;
	}

	switch (currentTermios.c_cflag & CSIZE)
	{
		case CS5:
			pLineControl->WordLength = 5;
			break;
		case CS6:
			pLineControl->WordLength = 6;
			break;
		case CS7:
			pLineControl->WordLength = 7;
			break;
		default:
			pLineControl->WordLength = 8;
			break;
	}

	return TRUE;
}


/* hard-coded in N_TTY */
#define TTY_THRESHOLD_THROTTLE		128 /* now based on remaining room */
#define TTY_THRESHOLD_UNTHROTTLE 	128
#define N_TTY_BUF_SIZE			4096

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

	if (pHandflow->ControlHandShake & SERIAL_DCD_HANDSHAKE)
	{
		/* DCD flow control not supported on Linux */
		DEBUG_WARN("Attempt to use the unsupported SERIAL_DCD_HANDSHAKE feature.");
		SetLastError(ERROR_NOT_SUPPORTED);
		result = FALSE; /* but keep on */
	}

	// TMP: FIXME: could be implemented during read/write I/O
	if (pHandflow->ControlHandShake & SERIAL_DSR_SENSITIVITY)
	{
		/* DSR line control not supported on Linux */
		DEBUG_WARN("Attempt to use the unsupported SERIAL_DSR_SENSITIVITY feature.");
		SetLastError(ERROR_NOT_SUPPORTED);
		result = FALSE; /* but keep on */
	}

	// TMP: FIXME: could be implemented during read/write I/O
	if (pHandflow->ControlHandShake & SERIAL_ERROR_ABORT)
	{
		/* Aborting operations on error not supported on Linux */
		DEBUG_WARN("Attempt to use the unsupported SERIAL_ERROR_ABORT feature.");
		SetLastError(ERROR_NOT_SUPPORTED);
		result = FALSE; /* but keep on */
	}

	/* FlowReplace */

	if (pHandflow->FlowReplace & SERIAL_AUTO_TRANSMIT)
	{
		upcomingTermios.c_iflag |= IXON;
	}
	else
	{
		upcomingTermios.c_iflag &= ~IXON;
	}

	if (pHandflow->FlowReplace & SERIAL_AUTO_RECEIVE)
	{
		upcomingTermios.c_iflag |= IXOFF;
	}
	else
	{
		upcomingTermios.c_iflag &= ~IXOFF;
	}

	// TMP: FIXME: could be implemented during read/write I/O
	if (pHandflow->FlowReplace & SERIAL_ERROR_CHAR)
	{
		DEBUG_WARN("Attempt to use the unsupported SERIAL_ERROR_CHAR feature. A character with a parity error or framing error will be read as \0");

		/* errors will be replaced by the character '\0' */
		upcomingTermios.c_iflag &= ~IGNPAR;
	}
	else
	{
		upcomingTermios.c_iflag |= IGNPAR;
	}

	if (pHandflow->FlowReplace & SERIAL_NULL_STRIPPING)
	{
		upcomingTermios.c_iflag |= IGNBRK;
	}
	else
	{
		upcomingTermios.c_iflag &= ~IGNBRK;
	}

	// TMP: FIXME: could be implemented during read/write I/O
	if (pHandflow->FlowReplace & SERIAL_BREAK_CHAR)
	{
		DEBUG_WARN("Attempt to use the unsupported SERIAL_BREAK_CHAR feature.");
		SetLastError(ERROR_NOT_SUPPORTED);
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


	// FIXME: could be implemented during read/write I/O
	if (pHandflow->FlowReplace & SERIAL_XOFF_CONTINUE)
	{
		/* not supported on Linux */
		DEBUG_WARN("Attempt to use the unsupported SERIAL_XOFF_CONTINUE feature.");
		SetLastError(ERROR_NOT_SUPPORTED);
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

	/* SERIAL_DCD_HANDSHAKE unsupported */

	/* SERIAL_DSR_SENSITIVITY unsupported */ 

	/* SERIAL_ERROR_ABORT unsupported */


	/* FlowReplace */

	pHandflow->FlowReplace = 0;

	if (currentTermios.c_iflag & IXON)
		pHandflow->FlowReplace |= SERIAL_AUTO_TRANSMIT;

	if (currentTermios.c_iflag & IXOFF)
		pHandflow->FlowReplace |= SERIAL_AUTO_RECEIVE;

	if (!(currentTermios.c_iflag & IGNPAR))
		pHandflow->FlowReplace |= SERIAL_ERROR_CHAR;

	if (currentTermios.c_iflag & IGNBRK)
		pHandflow->FlowReplace |= SERIAL_NULL_STRIPPING;

	/* SERIAL_BREAK_CHAR unsupported */

	if (currentTermios.c_cflag & HUPCL)
		pHandflow->FlowReplace |= SERIAL_RTS_CONTROL;

	if (currentTermios.c_cflag & CRTSCTS)
		pHandflow->FlowReplace |= SERIAL_RTS_HANDSHAKE;

	/* SERIAL_XOFF_CONTINUE unsupported */


	/* XonLimit */

	pHandflow->XonLimit = TTY_THRESHOLD_UNTHROTTLE;


	/* XoffLimit */

	pHandflow->XoffLimit = TTY_THRESHOLD_THROTTLE;

	return TRUE;
}

static BOOL _set_timeouts(WINPR_COMM *pComm, const SERIAL_TIMEOUTS *pTimeouts)
{
	/* NB: timeouts are applied on system during read/write I/O */

	/* http://msdn.microsoft.com/en-us/library/windows/hardware/hh439614%28v=vs.85%29.aspx */
	if ((pTimeouts->ReadIntervalTimeout == MAXULONG) && (pTimeouts->ReadTotalTimeoutConstant == MAXULONG))
	{
		DEBUG_WARN("ReadIntervalTimeout and ReadTotalTimeoutConstant cannot be both set to MAXULONG");
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	pComm->timeouts.ReadIntervalTimeout         = pTimeouts->ReadIntervalTimeout;
	pComm->timeouts.ReadTotalTimeoutMultiplier  = pTimeouts->ReadTotalTimeoutMultiplier;
	pComm->timeouts.ReadTotalTimeoutConstant    = pTimeouts->ReadTotalTimeoutConstant;
	pComm->timeouts.WriteTotalTimeoutMultiplier = pTimeouts->WriteTotalTimeoutMultiplier;
	pComm->timeouts.WriteTotalTimeoutConstant   = pTimeouts->WriteTotalTimeoutConstant;

	return TRUE;
}

static BOOL _get_timeouts(WINPR_COMM *pComm, SERIAL_TIMEOUTS *pTimeouts)
{
	pTimeouts->ReadIntervalTimeout         = pComm->timeouts.ReadIntervalTimeout;
	pTimeouts->ReadTotalTimeoutMultiplier  = pComm->timeouts.ReadTotalTimeoutMultiplier;
	pTimeouts->ReadTotalTimeoutConstant    = pComm->timeouts.ReadTotalTimeoutConstant;
	pTimeouts->WriteTotalTimeoutMultiplier = pComm->timeouts.WriteTotalTimeoutMultiplier;
	pTimeouts->WriteTotalTimeoutConstant   = pComm->timeouts.WriteTotalTimeoutConstant;

	return TRUE;
}


static BOOL _set_lines(WINPR_COMM *pComm, UINT32 lines)
{
	if (ioctl(pComm->fd, TIOCMBIS, &lines) < 0)
	{
		DEBUG_WARN("TIOCMBIS ioctl failed, lines=0x%0.4X, errno=[%d] %s", lines, errno, strerror(errno));
		SetLastError(ERROR_IO_DEVICE);
		return FALSE;
	}

	return TRUE;
}


static BOOL _clear_lines(WINPR_COMM *pComm, UINT32 lines)
{
	if (ioctl(pComm->fd, TIOCMBIC, &lines) < 0)
	{
		DEBUG_WARN("TIOCMBIC ioctl failed, lines=0x%0.4X, errno=[%d] %s", lines, errno, strerror(errno));
		SetLastError(ERROR_IO_DEVICE);
		return FALSE;
	}

	return TRUE;
}


static BOOL _set_dtr(WINPR_COMM *pComm)
{
	SERIAL_HANDFLOW handflow;
	if (!_get_handflow(pComm, &handflow))
		return FALSE;

	/* SERIAL_DTR_HANDSHAKE not supported as of today */
	assert((handflow.ControlHandShake & SERIAL_DTR_HANDSHAKE) == 0);

	if (handflow.ControlHandShake & SERIAL_DTR_HANDSHAKE)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	return _set_lines(pComm, TIOCM_DTR);
}

static BOOL _clear_dtr(WINPR_COMM *pComm)
{
	SERIAL_HANDFLOW handflow;
	if (!_get_handflow(pComm, &handflow))
		return FALSE;

	/* SERIAL_DTR_HANDSHAKE not supported as of today */
	assert((handflow.ControlHandShake & SERIAL_DTR_HANDSHAKE) == 0);

	if (handflow.ControlHandShake & SERIAL_DTR_HANDSHAKE)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	return _clear_lines(pComm, TIOCM_DTR);
}

static BOOL _set_rts(WINPR_COMM *pComm)
{
	// TMP: really required?
	/* SERIAL_HANDFLOW handflow; */
	/* if (!_get_handflow(pComm, &handflow)) */
	/* 	return FALSE; */

	/* if (handflow.FlowReplace & SERIAL_RTS_HANDSHAKE) */
	/* { */
	/* 	SetLastError(ERROR_INVALID_PARAMETER); */
	/* 	return FALSE; */
	/* } */

	return _set_lines(pComm, TIOCM_RTS);
}

static BOOL _clear_rts(WINPR_COMM *pComm)
{
	// TMP: really required?
	/* SERIAL_HANDFLOW handflow; */
	/* if (!_get_handflow(pComm, &handflow)) */
	/* 	return FALSE; */

	/* if (handflow.FlowReplace & SERIAL_RTS_HANDSHAKE) */
	/* { */
	/* 	SetLastError(ERROR_INVALID_PARAMETER); */
	/* 	return FALSE; */
	/* } */

	return _clear_lines(pComm, TIOCM_RTS);
}



static BOOL _get_modemstatus(WINPR_COMM *pComm, ULONG *pRegister)
{
	UINT32 lines=0;
	if (ioctl(pComm->fd, TIOCMGET, &lines) < 0)
	{
		DEBUG_WARN("TIOCMGET ioctl failed, errno=[%d] %s", errno, strerror(errno));
		SetLastError(ERROR_IO_DEVICE);
		return FALSE;
	}

	ZeroMemory(pRegister, sizeof(ULONG));

	/* TODO: FIXME: how to get a direct access from the user space
	 * to the MSR register in order to complete the 4 first bits?
	 */

	/* #define SERIAL_MSR_DCTS     0x01 */
	/* #define SERIAL_MSR_DDSR     0x02 */
	/* #define SERIAL_MSR_TERI     0x04 */
	/* #define SERIAL_MSR_DDCD     0x08 */

	if (lines & TIOCM_CTS)
		*pRegister |= SERIAL_MSR_CTS;
	if (lines & TIOCM_DSR)
		*pRegister |= SERIAL_MSR_DSR;
	if (lines & TIOCM_RI)
		*pRegister |= SERIAL_MSR_RI;
	if (lines & TIOCM_CD)
		*pRegister |= SERIAL_MSR_DCD;

	return TRUE;
}

/* http://msdn.microsoft.com/en-us/library/windows/hardware/hh439605%28v=vs.85%29.aspx */
static const ULONG _SERIAL_SYS_SUPPORTED_EV_MASK = 
	SERIAL_EV_RXCHAR   |
	SERIAL_EV_RXFLAG   |
	SERIAL_EV_TXEMPTY  |
	SERIAL_EV_CTS      |
	SERIAL_EV_DSR      |  
	SERIAL_EV_RLSD     |
	SERIAL_EV_BREAK    |
	SERIAL_EV_ERR      |
	SERIAL_EV_RING     |
	/* SERIAL_EV_PERR     | */
	SERIAL_EV_RX80FULL /*|
	SERIAL_EV_EVENT1   |
	SERIAL_EV_EVENT2*/;


static BOOL _set_wait_mask(WINPR_COMM *pComm, const ULONG *pWaitMask)
{
	ULONG possibleMask;

	if (*pWaitMask == 0)
	{
		/* clearing pending events */

		// TMP: TODO:

		if (ioctl(pComm->fd, TIOCGICOUNT, &(pComm->counters)) < 0)
		{
			DEBUG_WARN("TIOCGICOUNT ioctl failed, errno=[%d] %s", errno, strerror(errno));
			SetLastError(ERROR_IO_DEVICE);
			return FALSE;
		}

		pComm->pendingEvents = 0;
	}
	
	// TMP: TODO:
	// pending wait_on_mask must be stopped with STATUS_SUCCESS
	// http://msdn.microsoft.com/en-us/library/ff546805%28v=vs.85%29.aspx


	possibleMask = *pWaitMask & _SERIAL_SYS_SUPPORTED_EV_MASK;

	if (possibleMask != *pWaitMask)
	{
		DEBUG_WARN("Not all wait events supported (Serial.sys), requested events= 0X%0.4X, possible events= 0X%0.4X", *pWaitMask, possibleMask);

		/* FIXME: shall we really set the possibleMask and return FALSE? */
		pComm->waitMask = possibleMask;
		return FALSE;
	}

	pComm->waitMask = possibleMask;
	return TRUE;
}


static BOOL _get_wait_mask(WINPR_COMM *pComm, ULONG *pWaitMask)
{
	*pWaitMask = pComm->waitMask;
	return TRUE;
}


static BOOL _wait_on_mask(WINPR_COMM *pComm, ULONG *pOutputMask)
{
	assert(*pOutputMask == 0);


	/* TMP: TODO: while (TRUE)  */
	{
		int nbBytesToBeRead = 0;
		int nbBytesToBeWritten = 0;
		struct serial_icounter_struct currentCounters; 
		ULONG tiocmiwaitMask = 0; /* TIOCMIWAIT can wait for the 4 lines: TIOCM_RNG/DSR/CD/CTS */

		if (ioctl(pComm->fd, TIOCINQ, &nbBytesToBeRead) < 0)
		{
			DEBUG_WARN("TIOCINQ ioctl failed, errno=[%d] %s", errno, strerror(errno));
			SetLastError(ERROR_IO_DEVICE);
			return FALSE;
		}

		if (ioctl(pComm->fd, TIOCOUTQ, &nbBytesToBeWritten) < 0)
		{
			DEBUG_WARN("TIOCOUTQ ioctl failed, errno=[%d] %s", errno, strerror(errno));
			SetLastError(ERROR_IO_DEVICE);
			return FALSE;
		}

		ZeroMemory(&currentCounters, sizeof(struct serial_icounter_struct));
		if (ioctl(pComm->fd, TIOCGICOUNT, &currentCounters) < 0)
		{
			DEBUG_WARN("TIOCGICOUNT ioctl failed, errno=[%d] %s", errno, strerror(errno));
			SetLastError(ERROR_IO_DEVICE);
			return FALSE;
		}

		/* NB: preferred below "currentCounters.* != pComm->counters.*" over "currentCounters.* > pComm->counters.*" thinking the counters can loop */


		/* events */

		if (pComm->waitMask & SERIAL_EV_RXCHAR)
		{
			if (nbBytesToBeRead > 0)
			{
				/* at least one character is pending to be read */
				*pOutputMask |= SERIAL_EV_RXCHAR;
			}
		}
		
		if (pComm->waitMask & SERIAL_EV_RXFLAG)
		{
			if (pComm->pendingEvents & SERIAL_EV_RXFLAG) // TMP: to be done in the ReadThread
			{
				/* the event character was received FIXME: is the character supposed to be still in the input buffer? */
				
				/* event consumption */
				pComm->pendingEvents &= ~SERIAL_EV_RXFLAG;
				*pOutputMask |= SERIAL_EV_RXFLAG;
			}
		}

		if (pComm->waitMask & SERIAL_EV_TXEMPTY)
		{
			if (nbBytesToBeWritten == 0)
			{
				/* NB: as of today CommWriteFile still blocks and uses the same thread than CommDeviceIoControl, 
				 *  it should be enough to just check nbBytesToBeWritten 
				 */

				/* the output buffer is empty */
				*pOutputMask |= SERIAL_EV_TXEMPTY; 
			}
		}

		if (pComm->waitMask & SERIAL_EV_CTS)
		{
			tiocmiwaitMask |= TIOCM_CTS;
		}

		if (pComm->waitMask & SERIAL_EV_DSR)
		{
			tiocmiwaitMask |= TIOCM_DSR;
		}

		if (pComm->waitMask & SERIAL_EV_RLSD)
		{
			tiocmiwaitMask |= TIOCM_CD;
		}

		if (pComm->waitMask & SERIAL_EV_BREAK)
		{
			if (currentCounters.brk != pComm->counters.brk)
			{
				*pOutputMask |= SERIAL_EV_BREAK;

				/* event consumption */
				pComm->counters.brk = currentCounters.brk;
			}
		}
			 
		if (pComm->waitMask & SERIAL_EV_ERR)
		{
			if ((currentCounters.frame != pComm->counters.frame) ||
			    (currentCounters.overrun != pComm->counters.overrun) ||
			    (currentCounters.parity != pComm->counters.parity))
			{
				*pOutputMask |= SERIAL_EV_ERR;

				/* event consumption */
				pComm->counters.frame = currentCounters.frame;
				pComm->counters.overrun = currentCounters.overrun;
				pComm->counters.parity = currentCounters.parity;
			}
		}

		if (pComm->waitMask & SERIAL_EV_RING)
		{
			tiocmiwaitMask |= TIOCM_RNG;
		}

		if (pComm->waitMask & SERIAL_EV_RX80FULL)
		{
			if (nbBytesToBeRead > (0.8 * N_TTY_BUF_SIZE))
				*pOutputMask |= SERIAL_EV_RX80FULL;
		}

		if ((*pOutputMask == 0) && /* don't need to wait more if at least an event already occured */
		    (tiocmiwaitMask > 0))
		{
			if ((pComm->waitMask & SERIAL_EV_CTS) && currentCounters.cts != pComm->counters.cts)
			{
				*pOutputMask |= SERIAL_EV_CTS;

				/* event consumption */
				pComm->counters.cts = currentCounters.cts;
			}

			if ((pComm->waitMask & SERIAL_EV_DSR) && currentCounters.dsr != pComm->counters.dsr)
			{
				*pOutputMask |= SERIAL_EV_DSR;

				/* event consumption */
				pComm->counters.dsr = currentCounters.dsr;
			}

			if ((pComm->waitMask & SERIAL_EV_RLSD) && currentCounters.dcd != pComm->counters.dcd)
			{
				*pOutputMask |= SERIAL_EV_RLSD;

				/* event consumption */
				pComm->counters.dcd = currentCounters.dcd;
			}

			if ((pComm->waitMask & SERIAL_EV_RING) && currentCounters.rng != pComm->counters.rng)
			{
				*pOutputMask |= SERIAL_EV_RING;

				/* event consumption */
				pComm->counters.rng = currentCounters.rng;
			}


			/* FIXME: TIOCMIWAIT could be possible if _wait_on_mask gets its own thread */
			/* if (*pOutputMask == 0) */
			/* { */
			/* 	if (ioctl(pComm->fd, TIOCMIWAIT, &tiocmiwaitMask) < 0) */
			/* 	{ */
			/* 		DEBUG_WARN("TIOCMIWAIT ioctl failed, errno=[%d] %s", errno, strerror(errno)); */
			/* 		SetLastError(ERROR_IO_DEVICE); */
			/* 		return FALSE; */
			/* 	} */
			/* 	/\* TODO: check counters again after TIOCMIWAIT *\/ */
			/* } */
		}

		if (*pOutputMask != 0)
		{
			/* at least an event occurred */
			return TRUE;
		}
	}

	DEBUG_WARN("_wait_on_mask pending on events:0X%0.4X", pComm->waitMask);
	SetLastError(ERROR_IO_PENDING); /* see: WaitCommEvent's help */
	return FALSE;
}


static REMOTE_SERIAL_DRIVER _SerialSys = 
{
	.id		  = RemoteSerialDriverSerialSys,
	.name		  = _T("Serial.sys"),
	.set_baud_rate	  = _set_baud_rate,
	.get_baud_rate	  = _get_baud_rate,
	.get_properties   = _get_properties,
	.set_serial_chars = _set_serial_chars,
	.get_serial_chars = _get_serial_chars,
	.set_line_control = _set_line_control,
	.get_line_control = _get_line_control,
	.set_handflow     = _set_handflow,
	.get_handflow     = _get_handflow,
	.set_timeouts     = _set_timeouts,
	.get_timeouts     = _get_timeouts,
	.set_dtr          = _set_dtr,
	.clear_dtr        = _clear_dtr,
	.set_rts          = _set_rts,
	.clear_rts        = _clear_rts,
	.get_modemstatus  = _get_modemstatus,
	.set_wait_mask    = _set_wait_mask,
	.get_wait_mask    = _get_wait_mask,
	.wait_on_mask     = _wait_on_mask,
};


REMOTE_SERIAL_DRIVER* SerialSys_s()
{
	return &_SerialSys;
}

#endif /* _WIN32 */
