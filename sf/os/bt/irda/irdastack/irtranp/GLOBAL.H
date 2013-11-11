// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
 
#ifndef GLOBAL_H
#define GLOBAL_H

#define __CONSOLE__

#include <e32base.h>
#include <e32cons.h>

_LIT8(KTranpProductInformationString, "Symbian TranP");

// constants used by SCEP

const TUint8	KTranpMsgTypeCER	= 0x10;	// MsgType = Connection establishment request
const TUint8	KTranpMsgTypeCEC	= 0x11;	// MsgType = Connection establishment confirmation
const TUint8	KTranpMsgTypeData	= 0x20;	// Data (Command)
const TUint8	KTranpMsgTypeDisc = 0x30;	// Disconnection

const TUint8	KTranpInfTypeVer = 0x00;	// Version of MsgType
const TUint8	KTranpInfTypeNeg = 0x01;	// Negotiation Information
const TUint8	KTranpInfTypeUser = 0x03;	// UserData
const TUint8	KTranpInfTypeExt = 0x10;	// Extend in the future
const TUint8	KTranpInfTypeRsn = 0x20;	// Reason

const TUint8	KTranpInfVer = 0x10;		// Version of InfType

	// Only bit 6 and 7 count here! Use OR!
const TUint8	KTranpPduTypeReq = 0x00;		// Req PDU		- %00 000000
const TUint8	KTranpPduTypeRplAck = 0x40;		// Rpl PDU Ack	- %01 000000
const TUint8	KTranpPduTypeRplNack = 0x80;	// Rpl PDU Nack	- %10 000000
const TUint8	KTranpPduTypeAbt = 0xc0;		// Abt PDU		- %11 000000

const TUint8	KTranpCFLGi = 0x00;		// Machine can issue but not execute commands
const TUint8	KTranpCFLGie = 0x04;		// Machine can issue and execute commands

const TUint8	KTranpDFLGnseg = 0xc1;	// PDU not segmented (single PDU)
const TUint8	KTranpDFLGfPDU = 0x41;	// First segmented PDU
const TUint8	KTranpDFLGiPDU = 0x01;	// Intermediate segmented PDUs
const TUint8	KTranpDFLGlPDU = 0x81;	// Last segmented PDU
const TUint8	KTranpDFLGcint = 0xc2;	// Communication interruption
const TUint8	KTranpDFLGrcon = 0xc3;	// Reject to request connection

const TUint8	KTranpNegVer = 0x11;		// NegVer, version of NegInf

_LIT8(KTranpFR, "fr:");
_LIT8(KTranpID, "id:");
_LIT8(KTranpNM, "nm:");
_LIT8(KTranpPW, "pw:");
const TUint8 KTranpCR = 0x0d;
const TUint8 KTranpLF = 0x0a;

const TUint16	KTranpDisPDUrsnUR = 0x0000;	// Unspecified Reason
const TUint16	KTranpDisPDUrsnUD = 0x0001;	// User Disconnect
const TUint16	KTranpDisPDUrsnPD = 0x0002;	// Provider Disconnect

// bFTP

	// AttNames
_LIT8(KTranpFIL0, "FIL0");	// File name
_LIT8(KTranpLFL0, "LFL0");	// Long file name
_LIT8(KTranpTIM0, "TIM0");	// Time of creation (YYYYMMDDHHMMSS)
_LIT8(KTranpTYP0, "TYP0");	// Information on file or thumbnail
_LIT8(KTranpTMB0, "TMB0");	// The scale-down image
_LIT8(KTranpBDY0, "BDY0");	// ?
_LIT8(KTranpCMD0, "CMD0");	// Command name
_LIT8(KTranpWHT0, "WHT0");	// Category name
_LIT8(KTranpERR0, "ERR0");	// ERRCODE
_LIT8(KTranpRPL0, "RPL0");	// Stored filename

	// AttValue of AttName = "WHT0"
_LIT8(KTranpRIMG, "RIMG");	// Wants to know the data on a still image
_LIT8(KTranpRINF, "RINF");	// Wants to know details of the responder system
_LIT8(KTranpRCMD, "RCMD");	// Wants to know details on the processable commands

const TInt	KTranpUPFHeader = 384;				// Size of the UPF header

// Time and Date information offsets
const TInt KTranpDateOffset = 20;
const TInt KTranpTimeZoneOffset = 0;
const TInt KTranpYearOffset = 1;
const TInt KTranpMonthOffset = 3;
const TInt KTranpDayOffset = 4;
const TInt KTranpHourOffset = 5;
const TInt KTranpMinuteOffset = 6;
const TInt KTranpSecondOffset = 7;

// Picture Size offsets
const TInt KTranpPictureInfOffset = 286;
const TInt KTranpPictureXSizeOffset = 0;
const TInt KTranpPictureYSizeOffset = 2;

const TInt KTranpUPFEntryArea1 = 240; // Thumbnail
const TInt KTranpUPFEntryArea2 = 276; // Image
const TInt KTranpUPFEntryArea3 = 312; // Reserved 1
const TInt KTranpUPFEntryArea4 = 248; // Reserved 2
const TUint KNullImageAddress = 0xFFFFFFFF;

NONSHARABLE_CLASS(IrTranpUtil)
{
public:
	static void HexOut(const TDesC8& aBuffer);

	static void DAppend(TDes8* aBuffer, TUint16 aValue);
	static void LAppend(TDes8* aBuffer, TUint32 aValue);

	static TUint16 DExtract(const TDesC8& aBuffer, TInt aOffset);
	static TUint32 LExtract(const TDesC8& aBuffer, TInt aOffset);
};



#endif // GLOBAL_H
