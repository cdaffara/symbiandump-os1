//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description: This include file contains the global definitions for the CSY.
//

// CsyGlobals.h

/** @file CsyGlobals.h
 *
 */

#ifndef _CSYGLOBALS_H_
#define _CSYGLOBALS_H_

#include <e32std.h>


#if defined DSAMPLE || defined P2SAMPLE || defined H2
#define _27010ADVANCEOPTION
_LIT8(KATInitialised,"AT-Command Interpreter ready");
_LIT8(KATCmdSetup,"ATE0Q0V1%CPRI=0\r");  // echo off, quiet off, verbose on
#else
_LIT8(KATInitialised,"OK");
_LIT8(KATCmdSetup,"ATE0Q0V1\r");  // echo off, quiet off, verbose on
#endif


// CSY name
_LIT(KCSY_Gsm0710ServerName, "CSY27010");
_LIT(KCSY_Description, "Csy27010::CSY");

// Version information.
const TUint KCSY_Gsm0710MajorVersionNumber = 1;
const TUint KCSY_Gsm0710MinorVersionNumber = 0;
const TUint KCSY_Gsm0710BuildVersionNumber = 0;  // MAF put mainline num here on release

// Serial Info
const TUint KCSY_Gsm0710LowUnit =	1;
const TUint KCSY_Gsm0710HighUnit =	100;

const TInt	KCsyDefaultMaxDLCChannels = 10;

// Buffer size for LDD
const TUint KDefaultTxRxBufSize = 512;


// LDD/PDD configuration
#if !defined P2SAMPLE
_LIT8(KCsyDefaultPDDNameComm1, "EUART1");
_LIT8(KCsyDefaultPDDNameComm2, "EUART2");
#else
_LIT8(KCsyDefaultPDDNameComm1, "EUARTCSMI");
_LIT8(KCsyDefaultPDDNameComm2, "EUART1");
#endif

// AT Command need to put the baseband into Mux mode
#ifdef _27010ADVANCEOPTION
_LIT8(KCsyDefaultATEnterMuxModeCmd, "AT+CMUX=1,0,5\r");
#else
_LIT8(KCsyDefaultATEnterMuxModeCmd, "AT+CMUX=0,0,5\r");
#endif

// C32 Port number offsets
const TInt KCOMMP_CSD_PPP_NUMBER = 6;  // 6 - reserved for CSD PPP
const TInt KCOMMP_IP_NIF_OFFSET  = 16; // 16 and above - Raw IP

// Factor by which to increase the size of the receive buffer in the LDD.
const TInt KCSY_ReceiveBufferSizeIncreaseFactor = 10;

// Basic sizes
const TUint KMaxFrameSize    = 127;
const TUint KMaxIpPacketSize = 1502;

const TUint KMaxAdvFrameSize = KMaxFrameSize + 8;

// --------------------------------------------------------

// Constants for enforcing when the modem is allowed to send to the CSY

// FC enforced when we are starting to run out of frames to put data in
// or when a particular client is slow at reading

// Maximum frames and flow control thresholds
const TUint KMaxFreeFrames = 75;    // <- should be enough for 3 contexts at max throughput each way
const TUint KStopDataDlcsThreshold = 10;
const TUint KStartDataDlcsThreshold = 40;

// Slow read by client
const TUint KMaxPacketsOutstandingForC32Client = 5;

// At Response timeout
const TInt	KAtResponseTimeoutLimit = 5;

// --------------------------------------------------------

// active object priorities
const TInt KFrameReaderAoPriority = 100;
const TInt KFrameWriterAoPriority = KFrameReaderAoPriority;

// Magic numbers
const TUint8 KSingleFrame      = 0x03;	// Only Frame of Message
const TUint8 KStartMultiFrame  = 0x01;	// Start  MultiFrame
const TUint8 KMiddleMultiFrame = 0x00;	// Middle MultiFrame
const TUint8 KEndMultiFrame    = 0x02;	// Ending MultiFrame

const TInt KAdvOptionNumOfNonDataOctets = 4;
const TInt KAdvOptionType4StartOfMessageData = 3;
const TInt KAdvOptionType4FrameControl = 2;

// Ignoring start/end flags ..
// Advanced format: address | control | data | checksum
const TInt KAdvOptionHeaderSize	= 2;
const TInt KAdvOptionControl = 1;
const TInt KAdvOptionAddress = 0;

const TInt KChecksumSize = 1;

const TInt KBasicOptionHeaderSize	= 3;
const TInt KBasicOptionLength		= 2;
const TInt KBasicOptionControl		= 1;
const TInt KBasicOptionAddress		= 0;

// Ctrl(dlc 0) commands
// adv option:   address | control | CtrlType   | LengthData | Value(s)
// basic option: address | control | length | CtrlType  | LengthData | Value(s)
const TInt KBasicOptionCtrlStart = KBasicOptionHeaderSize;
const TInt KAdvOptionCtrlStart   = KAdvOptionHeaderSize;

// CtrlType  | LengthData | Value(s)
const TInt KPositionOfValueFieldInCtrlData  = 2;
const TInt KPositionOfLengthFieldInCtrlData = 1;

const TUint8 KCtrlChannelDlcNum = 0;

// Timeouts
const TInt KOneSecond  = 1000000;
const TInt KTwoSeconds = 2000000;


// Frame Start/Stop Flag


const TUint8 KCsy0710StartEndFlag	= 0x7E;
const TUint8 KCsy0710EscapeByte		= 0x7D;

const TUint8 KPNFrameType			= 0x00;
const TUint8 KPNClBits				= 0x03;
const TUint8 KPNDlcPriority			= 0x07;
const TUint8 KPNAckTimer			= 0x0A; // 10ms resolution
const TUint8 KPNMaxFrameSize		= 0x00ff & KMaxFrameSize;
const TUint8 KPNMaxRetransmissions	= 0x03;
const TUint8 KPNWindowSize			= 0x00;


// Control Field Constants
const TUint8 KCsy0710CTLSABM		= 0x2F;    
const TUint8 KCsy0710CTLUA			= 0x63;	 
const TUint8 KCsy0710CTLDM			= 0x0F;
const TUint8 KCsy0710CTLDISC		= 0x43;
const TUint8 KCsy0710CTLUIH			= 0xEF;
const TUint8 KCsy0710CTLUI			= 0x03;

const TUint KCsy0710EA				= 0x01;
const TUint KCsy0710CRBit			= 0x02;
const TUint KCsy0710PollFinal		= 0x10;

// Message Types for UIH frames received by the CSY on DLC 0 (control channel)
//     The EA (bit 0) and C/R (bit 1) bits are both assumed to be set
const TUint8 KCsy0710CTLUIH_DlcParamNegotiate   = 0x83;
const TUint8 KCsy0710CTLUIH_DlcParamNegotiateRsp = 0x81;
const TUint8 KCsy0710CTLUIH_PowerSaveCtl        = 0x43;
const TUint8 KCsy0710CTLUIH_MultCloseDown       = 0xC3;
const TUint8 KCsy0710CTLUIH_TestCmd             = 0x23;
const TUint8 KCsy0710CTLUIH_FlowControlOn       = 0xA3;
const TUint8 KCsy0710CTLUIH_FlowControlOff      = 0x63;
const TUint8 KCsy0710CTLUIH_ModemStatusCmd      = 0xE3;
const TUint8 KCsy0710CTLUIH_ModemStatusRsp      = 0xE1;
const TUint8 KCsy0710CTLUIH_NonSupportedCmdResp = 0x13;
const TUint8 KCsy0710CTLUIH_RemotePortNegotiate = 0x93;
const TUint8 KCsy0710CTLUIH_RemoteLineStatus    = 0x53;
const TUint8 KCsy0710CTLUIH_ServiceNegotiate    = 0xD3;


// The CRC Table as defined in 27.010
const TUint8 crctable[256] = 
	{ //reversed, 8-bit, poly=0x07
	0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98,0xEA, 0x7B,
	0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 0x12, 0x83, 0xF1, 0x60, 0x15, 0x84,0xF6, 0x67,
	0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0,0xD2, 0x43,
	0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC,0xCE, 0x5F,
	0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05, 0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8,0x9A, 0x0B,
	0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19, 0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4,0x86, 0x17,
	0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0,0xA2, 0x33,
	0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC,0xBE, 0x2F,
	0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78,0x0A, 0x9B,
	0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64,0x16, 0x87,
	0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD, 0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40,0x32, 0xA3,
	0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1, 0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C,0x2E, 0xBF,
	0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08,0x7A, 0xEB,
	0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 0x82, 0x13, 0x61, 0xF0, 0x85, 0x14,0x66, 0xF7,
	0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30,0x42, 0xD3,
	0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C,0x5E, 0xCF
	};

//v24 signals
const TUint KV24SignalRTC = 0x04;
const TUint KV24SignalRTR = 0x08;
const TUint KV24SignalIC  = 0x40;
const TUint KV24SignalDV  = 0x80;

// Logging

_LIT(KCsyLogDir,  "TRP\\CSY" );
_LIT(KCsyLogFile, "csy27010.txt");
_LIT(KCsyLogLine, "---------- CSY 3GPP 27.010 Log ----------");


#endif  // _CSYGLOBALS_H_
