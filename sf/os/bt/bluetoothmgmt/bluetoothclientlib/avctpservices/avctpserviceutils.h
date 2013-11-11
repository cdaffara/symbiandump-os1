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
// Utility functions for AvctpServices
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVCTPSERVICEUTILS_H
#define AVCTPSERVICEUTILS_H

/**
Avctp Services panic codes
*/
enum TAvctpServicesPanic
	{
	/** Invalid message type */
	EAvctpInvalidMessageType = 0,
	/** A null TBTDevAddr has been given*/
	EAvctpNullBTDevAddr = 1,
	/** You must call Open on RAvctp before calling any other method*/
	EAvctpRAvctpNotOpen = 2,
	/** You must call not Open RAvctp if it is already open*/
	EAvctpRAvctpAlreadyOpen = 3,
	/** We've got into an unrecognised state */
	EAvctpUnrecognisedState = 4,
	/** Incorrect state transition in CAvctpReceiver */
	EAvctpRecvBadStateTransition = 5,
	/** At the RAvctp level all packet types must be normal*/
	EAvctpPacketTypeNotNormal = 6,
	/** We've got a wrong PID somehow*/
	EAvctpIncorrectPid = 7,
	/** We are setting the notifier that is already assigned*/
	EAvctpSecondaryChannelNotifyAlreadyAssigned = 8,
	/** RAvctp::CloseGracefully doesn't support a Cancel */
	EAvctpCloseGracefullyCancelled = 9,
	/** The same device address occurs more than once in the remote device array*/
	EAvctpRemoteAddressOccursMultipleTimes = 10,
	EAvctpSecondChannelNotPresent = 11,
	EAvctpRemoteDeviceNotConnected = 12,
	EAvctpInvalidChannelState = 13,
	EAvctpSenderAlreadyAllocated = 14,
	EAvctpReceiverAlreadyAllocated = 15,
	EAvctpInvalidChannel = 16,
	EAvctpSecondaryChannelUnexpected = 17,
	EAvctpUnexpectedErrorCode = 18,
	EAvctpNullControllerChannel = 19,
	EDetachIndicateForSecondChannelWithNoConsumer = 20,
	EAvctpUnexpectedLeave = 21,
	};

void Panic(TAvctpServicesPanic aPanic);

class TBTDevAddr;
TBool IsNullAddress(const TBTDevAddr& aAddr);

class TBTDevAddr;	// forward declaration

const TInt KBTAddressLength = 12;	// used to allocate a TBuf<> needed to get the readable BTDevAddress

#endif // AVCTPSERVICEUTILS_H

