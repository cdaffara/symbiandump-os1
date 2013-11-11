// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Utility functions for Rfcomm
// 
//

#ifndef RFCOMMUTIL_H
#define RFCOMMUTIL_H

_LIT(KRFCOMMPanicName, "RFCOMM Panic");

enum TRFCOMMPanic
	{
	ERfcommUnexpectedEvent = 0,
	ERfcommNotBound = 1,
	ERfcommProtocolOpenedAfterClose = 2,
	ERfcommErrorSettingAddress = 3,
	ERfcommPacketReassemblyError = 4,
	ERfcommPacketLengthError = 5,
	ERfcommConnectWithData = 6,
	ERfcommPacketHasFCSSet = 7,
	ERfcommCantCloseMuxChannel = 8,
	ERfcommDataPacketTooLong = 9,
	ERfcommTestDataTooLong = 10,
	ERfcommInvalidPriority = 11,
	ERfcommUnexpectedCanSend = 12,
	ERfcommAddSAPInBadState = 13,
	ERfcommInvalidRPNLength = 14,
	ERfcommNextPacketAlreadySet = 15,
	ERfcommDisconnectDataNotSupported = 16,
	ERfcommReadForTooMuchData = 17,
	ERfcommDataTooLong = 18,
	ERfcommSendingWhileBlocked = 19,
	ERfcommIdleTimeoutWhenNotIdle = 20,
	ERfcommInvalidDLCI = 21,
	ERfcommL2CAPMTUtooBig = 22,
	ERfcommBadFrameCtrlField = 23,
	ERfcommBadFrameAddrField = 24,
	ERfcommChannelIdleTimeout = 25,
	ERfcommChannelError = 26,
	ERfcommSAPTwoIoctls = 27,
	ERfcommNewDataCallBack = 28,
	ERfcommIncomingNotSupported = 29,
	ERfcommMismatchedAddressAndSAP = 30,
	ERfcommNullSAPForListening = 31,
	ERfcommNullSAPForMuxer = 32,
	ERfcommBadDLCIForClone = 33,
	ERfcommBadStateForCloning = 34,
	ERfcommDequeingBadListeningSAP = 35,
	ERfcommRequeuingListeningSAP = 36,
	ERfcommBadCalculatedMTU = 37,
	ERfcommUnknownChildConnectionNotified = 38,
	ERfcommBadStateForDestruction = 39,
	ERfcommNullMux = 40,
	ERfcommUnexpectedSecurityRequesterCallback = 41,
	ERfcommNoSecurityRequester = 42,
	ERfcommUnknownChildSecurityFailed = 43,
	ERfcommInvalidMuxInSAP = 44,
	ERfcommStateDisconnectingWithNoData = 45,
	ERfcommNullSAPForCallback = 46,
	ERfcommFrameBeforeMuxUp = 47,
	ERfcommBadOptionForRFCOMM = 48,
	ERfcommMuxChannelOpeningAndClosing = 49,
	ERfcommChannelUnhandledEvent = 50,
	ERfcommPNMismatchedDLCI = 51,
	ERfcommMuxChannelStateChangeToSelf = 52,
	ERfcommUnexpectedCBFCCredit = 53,
	ERfcommCBFCProxyForRemoteCreditGoingNegative = 54,
	ERfcommCBFCLocalCreditGoingNegative = 55,
	ERfcommNoPacket = 56,
	ERfcommUnhandledIoctl = 57,
	ERfcommInvalidSAPIoctlCancel =58,
	ERfcommStateClosedWithData = 59,
	ERfcommSAPAllocatedBetweenBindAndListen = 60,
	ERfcommRequeuingBoundSAP = 61,
	ERfcommDequeuingBadBoundSAP = 62,
	ERfCommFlowStrategyOutOfBounds = 63,
	ERfCommMuxerStateOutOfBounds = 64, 
	ERfCommStateOutOfBounds = 65,
	ERfCommStateBufferFull = 66,
	ERfCommNothingToHandleResponse = 67,
	ERfCommDiscSentOnMuxDlci = 68,
	ERfCommUnexpectedCommandOnMuxChannel = 69,
	};

/**
Panics raised by state machine default behaviour are multiplied by this
factor.  The less signifigant digits are then available to carry a state
index, allowing more information when fixing defects.

For example, 'RFCOMM Panic 1008' is a UnexpectedEvent panic in the 
'Listenning' state.
*/
const TInt KStatePanicMultiplier = 10000;

/**
A delimiter to prevent a panic code of 0 creating ambigious panics.
Without this panic code 0 in state 8 and panic code 8 (not in the state 
machine) would be identical.
*/
const TInt KStatePanicDelimiter = 1000; // '1' is the delimiter

/**
If for any reason the current state cannot be found, this value is placed
in the state field of a panic.
*/
const TInt KUnknownState = KStatePanicDelimiter - 1;

void Panic(TRFCOMMPanic aPanic);

void Panic(TRFCOMMPanic aPanic, TInt aState);

#endif
