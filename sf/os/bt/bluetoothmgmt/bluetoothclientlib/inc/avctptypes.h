// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef AVCTPTYPES_H
#define AVCTPTYPES_H

#include <bt_sock.h>

//
//
// AVCTP
//
//

/**
The Symbian Avctp namespace
@internalTechnology
*/
namespace SymbianAvctp
{
	
const static TInt KErrBase = -6421;
const static TInt KErrBadAddress = KErrBase;
const static TInt KErrPacketTooShort = KErrBase - 1;
const static TInt KErrMalformedPacketFromRemote = KErrBase - 2;
const static TInt KErrUnknownPacketType = KErrBase - 3;
const static TInt KErrRemoteDeviceDecreasedMtuDuringFragmentation = KErrBase - 4;
const static TInt KErrProtocolAlreadyBound = KErrBase - 5;
const static TInt KErrRepeatConnectionAttempt = KErrBase - 6;
const static TInt KErrSynchronousMuxerError = KErrBase - 7;
const static TInt KErrInvalidTransactionLabel = KErrBase - 8;
const static TInt KErrSapAlreadyBound = KErrBase - 9;
const static TInt KErrRemoteSentTooManyIpidSdus = KErrBase - 10;
const static TInt KErrDeniedRemoteAttemptedReconnection = KErrBase - 11;
const static TInt KErrFirstChannelNotAttached = KErrBase - 12;
const static TInt KErrSecondChannelNotAttached = KErrBase - 13;
const static TInt KErrMuxerBlocked = KErrBase - 14;
const static TInt KErrMuxerShutDown = KErrBase - 15;
const static TInt KErrMuxerNotFound = KErrBase - 16;
const static TInt KErrInvalidChannel = KErrBase - 17;

// max AVCTP error is -6421 - 28

/**
The Avctp Profile ID
*/
typedef TUint16 TPid;

const static TPid KAvrcpPid = 0x110e;
const static TPid KUndefinedPid = 0x0000;	

/**
AVCTP Transaction Label
Must be < KMaxTransactionLabel.
*/
typedef TUint8 TTransactionLabel;

/**
The maximum AVCTP Transaction Label
All Transaction Labels must be < this value.
*/
const static TUint8 KMaxTransactionLabel	= 16;
							        
/**
  AVCTP Message Types
*/
enum TMessageType
	{
	EUndefined	= 0x0,		/*! Unknown */
	ECommand	= 0x1,		/*! AVCTP command */
	EResponse	= 0x2		/*! AVCTP response */
	};										        					        
		
IMPORT_C void AssertValidTransactionLabel(TTransactionLabel aLabel);
IMPORT_C TBool IsValidTransactionLabel(TTransactionLabel aLabel);
					
} // end of namespace SymbianAvctp

/**
AVCTP Socket Address.

Use this class to specify a local or remote AVCTP connection end-point,
that is: the BT device address and PID.
@internalTechnology
*/
NONSHARABLE_CLASS(TAvctpSockAddr) : public TBTSockAddr
	{
public:
	IMPORT_C TAvctpSockAddr();
	IMPORT_C TAvctpSockAddr(const TSockAddr& aSockAddr);
	IMPORT_C static TAvctpSockAddr& Cast(const TSockAddr& aSockAddr);
	IMPORT_C SymbianAvctp::TPid Pid() const;
	IMPORT_C void SetPid(SymbianAvctp::TPid aPid);
	IMPORT_C TInt Channel() const;
	IMPORT_C void SetChannel(TInt aChannel);
	};

const TInt KAvctpBaseOutboundMTU = 0;
const TInt KAvctpExtendOutboundMTU = 1;

#endif // AVCTPTYPES_H
