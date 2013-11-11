/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#ifndef __ZEROCOPY_LOOPBACK_VAR_H__
#define __ZEROCOPY_LOOPBACK_VAR_H__

#include <in_sock.h>
/**
some port number not likely to be used by anyone else
@internalTechnology
*/
const TUint KZeroCopyLoopbackCmdPort = 0xAB;

/**
@internalTechnology
*/
enum TZeroCopyLoopbackCommands
{
	KForceReconnect = 0xFE,
	KSendNotification = 0xFD,
	KForceDisconnect = 0xFC,
	KForceFinishedSelection = 0xFB,
	KForceBinderError = 0xFA,
	KColourDataByLinkTierAccessPointId = 0xF9
};


/** Range of ports reflected through the loopback driver with the addresses swapped, ports intact [suitable for TCP to talk to self]
 */
const TUint KDriverReflectionRangeStartPort = 5000;
const TUint KDriverReflectionRangeEndPort = 5099;

/** Range of ports reflected through the DummyNif bearer protocol with the addresses swapped, ports intact [suitable for TCP to talk to self]
 */
const TUint KBearerReflectionRangeStartPort = 5100;
const TUint KBearerReflectionRangeEndPort = 5199;


/**
@internalTechnology
*/
// address ranges for different nifs (all variants of dummy)
const TUint32 KZeroCopyLoopbackLocalAddressBase				= INET_ADDR(10,1,1,0);


/**
@internalTechnology
*/
// Class C host id that the Dummy/Hungry Nifs will never allocate as a local address
// (10.1.1.4 and 10.2.1.4).  These addresses are guaranteed not to match the (randomly
// allocated) local address of the Dummy/Hungry Nifs and hence will avoid the situation
// where the TCP/IP stack does a "short-cut" loopback of packets that avoid the Nifs.
const TUint32 KZeroCopyLoopbackReservedHostId		= 4;

// return value from CNifIfLink::Send()
const TInt KZeroCopyLoopbackSendOkay = 1;


//
// DummyNif specific Control options (Level KCOLInterface)
//

/** Base constant for options that set Nifman Idle Timeouts. Used only to define other options. 
@internalTechnology */
const TInt KTestSoZeroCopyLoopbackSetNifmanTimeout(125); 

/** Set Short (LastSessionClosed) Nifman Idle timeout. Level KCOLInterface.
@internalTechnology */
const TInt KTestSoZeroCopyLoopbackNifSetLastSessionClosedTimeout( (KTestSoZeroCopyLoopbackSetNifmanTimeout + 1) | KConnReadUserDataBit);

/** Set Medium (LastSocketClosed) Nifman Idle Timeout. Level KCOLInterface.
@internalTechnology */
const TInt KTestSoZeroCopyLoopbackSetLastSocketClosedTimeout(  (KTestSoZeroCopyLoopbackSetNifmanTimeout + 2) | KConnReadUserDataBit);

/** Set Long (LastSocketActivity) Nifman Idle Timeout. Level KCOLInterface.
@internalTechnology */
const TInt KTestSoZeroCopyLoopbackSetLastSocketActivityTimeout((KTestSoZeroCopyLoopbackSetNifmanTimeout + 3) | KConnReadUserDataBit);

#endif // __ZEROCOPY_LOOPBACK_VAR_H__
