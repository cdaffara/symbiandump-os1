// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <e32base.h>
#include <etelqos.h>


// Constants for IPv4 protocol interface
_LIT8(KDescIp, "ip");
_LIT8(KDescIcmp, "icmp");

_LIT(KIPv4ProtocolIfName, "Bttint4");

// Constants for IPv6 protocol interface
_LIT8(KDescIp6, "ip6");
_LIT(KIPv6ProtocolIfName, "Bttint6");

_LIT(KNifName, "rawip2nif");

// constants for diagnostic logging
_LIT8(KNifSubDir, "RawIp2"); 
_LIT8(KRefFile, "RawIP2"); 


enum TRawIP2NifPanic
    {
	KNifUnknownInitState,
	KNifUnknownShutDownState,
	KNifProtocolInUse,
	/** Bca in unkonwn State */
	KBcaUnkownState,
	/** BCA not exist*/
	KBcaNotExist 	
	};


const TUint16 KIp4FrameType = 0x21;
const TUint16 KIp6FrameType = 0x57;

const TUint KDefaultSpeedMetric = 1;
const TUint KDefaultMtu = 1500;

// Max size of IP packet 
const TInt KMaxIPPacket = 1500;
const TInt KIPTagHeaderLength = 2;
const TInt KMaxIPPacketAndHeader = KMaxIPPacket + KIPTagHeaderLength;

#endif	//CONSTANTS_H
