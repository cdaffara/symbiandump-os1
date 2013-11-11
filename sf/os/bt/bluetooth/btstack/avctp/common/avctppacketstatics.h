// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef AVCTPPACKETSTATICS_H
#define AVCTPPACKETSTATICS_H

#include <bluetoothav.h>
#include <bluetooth/avctptypes.h>

namespace SymbianAvctp
{

const static TUint8 KUndefinedTransactionLabel = SymbianAvctp::KMaxTransactionLabel;
		
const static TUint8 KIsValidPidMask		= 0x01;
const static TUint8 KMessageTypeMask	= 0x02;
const static TUint8 KPacketTypeMask		= 0x0C;
const static TUint8 KTransactionMask	= 0xF0;
	
const static TUint KNormalHeaderPidOffset = 1;
const static TUint KStartHeaderPidOffset  = 2;	
const static TUint KNumFragmentsOffset = 1;

const static TUint KTransactionLabelShift  = 4;	
const static TUint KPacketTypeShift  = 2;	

const static TUint KResponseMsgBit  = 2;	

/**
Types of packet, as indicated in the packet header.
Note these are the actual values used in the AVCTP v1.0
spec.
*/ 
enum TPacketType
	{
	/** An unfragmented AVCTP packet */
	ENormalPkt		= 0x0,
	/** An AVCTP fragment start packet */
	EStartFrag		= 0x1,
	/** An AVCTP fragment continue packet */
	EContinueFrag	= 0x2,
	/** An AVCTP fragment end packet */
	EEndFrag		= 0x3,
	/** This value should never be used in practise. It's only for initialisation */
	EUnknownPkt		= 0x4,
	};

/**
Offset of the data within an AVCTP packet
*/
enum TPduHeaderLength
	{
	/** For an unfragmented AVCTP packet */
	ENormalHeaderLength		= 0x3,
	/** For an AVCTP fragment start packet */
	EStartFragHeaderLength	= 0x4,
	/** For an AVCTP fragment continue or end packet */
	EOtherFragHeaderLength	= 0x1,
	/** The minimum length of the three headers */
	EMinimumHeaderLength = 0x1,
	};			

// Avctp packet parsing functions 
IMPORT_C TMessageType MessageType(const TDesC8& aData);
IMPORT_C TPacketType PacketType(const TDesC8& aData);
IMPORT_C TTransactionLabel TransactionLabel(const TDesC8& aData);
IMPORT_C TBool IsValidPid(const TDesC8& aData);
IMPORT_C TPid Pid(const TDesC8& aData);
IMPORT_C TUint8 FragmentsInSdu(const TDesC8& aData);
IMPORT_C TPduHeaderLength ConvertPacketTypeToHeaderLength(TPacketType aPktType);
	
} // end of namespace SymbianAvctp

#endif // AVCTPPACKETSTATICS_H 

