// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Constants for the SDP protocol.
// Version for the SDP server
// 
//

#ifndef SDPCONSTS_H
#define SDPCONSTS_H

#include <btsdp.h>

// The PSM on which SDP resides
static const TInt KSDPPSM = 0x01;

// Idle timer default value (seconds)
static const TInt KSDPIdleTimeout = 5;

// Client timeouts
static const TInt KSDPClientTimeout = 120; // seconds

// PDU Size
static const TInt KSdpMaxPduSize = 250;

//PDU Fields
static const TInt KSdpPduHeaderSize = 5;
static const TInt KSdpPduIdOffset   = 0;
static const TInt KSdpPduTransIdOffset = 1;
static const TInt KSdpPduParamLengthOffset = 3;

// PDU Types
enum TSdpPduId
	{
	EErrorResponse					= 0x01,
	EServiceSearchRequest			= 0x02,
	EServiceSearchResponse			= 0x03,
	EServiceAttributeRequest		= 0x04,
	EServiceAttributeResponse		= 0x05,
	EServiceSearchAttributeRequest	= 0x06,
	EServiceSearchAttributeResponse	= 0x07
	};

// Errors
enum TSdpErrorCodes							// EPOC error that'll generate each code
	{
	EErrorReserved				= 0x00,
	EInvalidSdpVersion			= 0x01,		// KErrNotSupported
	EInvalidServiceRecordHandle = 0x02,		// KErrBadHandle 
	EInvalidRequestSyntax		= 0x03,		// KErrCorrupt, KErrArgument
	EInvalidPduSize				= 0x04,		// KErrOverflow, KErrUnderflow, KErrTooBig
	EInvalidContinuationState	= 0x05,		// KErrNotReady, KErrUnknown, KErrLocked
	EInsufficientResources		= 0x06,		// KErrNoMemory, KErrHardwareNotAvailable
	};



// Data Element stuff

static const TInt KErrSdpParserInvalidSizeForType = -3000;
static const TInt KErrSdpParserInvalidSizeForParentList = -3001;
static const TInt KErrSdpBadAttributeId = -3002;

static const TInt KSdpRecordHandleSize = 4;
static const TInt KSdpAttributeIdSize = 2;
static const TSdpServRecordHandle KSdpMaxServRecordHandle = 0xffffffff;
static const TSdpServRecordHandle KSdpMinServRecordHandle = 0x10000;

static const TInt KSdpContinuationStateMaxLength = 16;

enum TSdpElementSizeDesc
	{
	ESizeOneByte		= 0,
	ESizeTwoBytes		= 1,
	ESizeFourBytes		= 2,
	ESizeEightBytes		= 3,
	ESizeSixteenBytes	= 4,
	ESizeOneAdditional	= 5,
	ESizeTwoAdditional	= 6,
	ESizeFourAdditional	= 7
	};

// Element header fields
static const TInt KSdpElemHdrTypeShift = 3;
static const TInt KSdpElemHdrSizeMask = 0x07;

// Data Element Stuff
static const TUint8 KSdpElementValidSizes[] = 
	{		// Type		// Sizes
	0x01,	//	00		-------0
	0x1f,	//	01		---43210
	0x1f,	//	02		---43210
	0x16,	//	03		---4-21-
	0xe0,	//	04		765-----
	0x01,	//	05		-------0
	0xe0,	//	06		765-----
	0xe0,	//	07		765-----
	0xe0	//	08		765-----
	};


#endif
