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
// Utility functions for AVCTP
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVCTPUTILS_H
#define AVCTPUTILS_H


/*
@internalTechnology
*/
namespace SymbianAvctp
{

_LIT(KAvctpPanicName, "AvctpPrtPanic");

/**
AVCTP panic codes
*/
enum TPanic
	{
	/** ESock has done something unexpected */
	EUnexpectedEsockEvent = 0,
	/** Unable to set remote address on lower protocol */
	EErrorSettingAddress = 1,
	/** We don't support (dis)connection data */
	EDisconnectDataNotSupported = 2,
	/** We don't support the graceful close of the lower protocol Sap */
	ELowerProtocolSapCanClose = 3,
	/** We don't support Active Open calls since AVCTP is connectionless*/
	EActiveOpenNotSupported = 4,
	/** We don't support Passive Open calls since AVCTP is connectionless*/
	EPassiveOpenNotSupported = 5,
	/** The protocol hasn't been bound to a lower protocol */
	ENotBound = 6,
	/** Unknown packet type */
	EUnknownPacketType = 7,
	/** The idle timer has expired but we're not idle */
	EIdleTimeoutWhenNotIdle = 8,
	/** The lower protocol SAP's remote address and the passed address don't match */
	EMismatchedAddressAndSap = 9,
	/** There must be at least one data client pending a join in the join async callback */
	EDataClientsPendingJoinWasZero = 10,
	/** We've unexpectedly found a null TPid */
	ENullPid = 11,
	/** We've unexpectedly found a null TBTDevAddr */
	ENullTBTDevAddr = 12,
	/** We've should've had a null TBTDevAddr at this point*/
	ETBTDevAddrNotNull = 13,
	/** L2CAP shouldn't have given us an error */
	EUnexpectedErrorFromL2CAP = 14,
	/** We're expecting a Lower Protocol SAP we could use at this point*/
	ENullLowerProtocolSap = 15,
	/** Expected a usable Muxer at this point */
	ENullMuxer = 15,
	/** Unexpected Muxer Event*/
	EUnexpectedMuxerEvent = 16,
	/** The muxer shouldn't have any data clients ! */
	EMuxerHasADataClient = 17,
	/** A sap is being asked to bind to a Pid when it's already bound to one */	
	ESapAlreadyBound = 18,
	/** A muxer is being asked to bind to a remote address when it's already bound to one */	
	EMuxerAlreadyBound = 19, 
	/** A sap Link Manager is being asked to bind to a Pid when it's already bound to one */	
	ESapLinksMgrAlreadyBound = 20,
	/** No data clients are allowed on a CAvctpTransport */
	EUnexpectedDataClient = 21,
	/** When the Control EConnectRspAccept is called on the SapLinksMgr, 
	the muxer that is addressed by the Control must be Connected */
	EMuxerNotConnected = 22,
	/** We should've had a muxer at this point */
	EMuxerNotFound = 23,
	/** We've received an unexpected Avctp Sub Conn Event*/
	EUnexpectedAvctpSubConnEvent = 24,
	/** The protocol has received a badly formed message in control plane*/
	EProtocolReceivingBadlyFormedControlMessage = 25,
	/** The protocol has received a message from a protocol it was not expecting*/
	EProtocolReceivingControlFromUnexpectedProtocol = 26,
	/** A Muxer isn't clear to send when it should be */
	EMuxerBlocked = 27,	
	/** We expected to have a proper packet */
	ENullPacket = 28,
	/** Avctp is connectionless so a valid sockAddr must be supplied to the Sap */
	ENullSockAddr = 29,
	/** We expected a usable Avctp Packet Manager at this point */
	ENullPacketMgr = 30,
	/** We expected a null packet mgr pointer */
	ENonNullPacketMgr = 31,
	/** We've some how ended up with the wrong BT address */
	EWrongBTAddress = 32,
	/** The muxer must be blocked if the packet manager has is still working on a 
	partial packet when a sap wants to write to it*/	    
	EMuxerNotBlocked = 33,
	/** A packet is in the wrong state */
	EInvalidPacketState = 34,
	/** The Q Count doesn't match what we thought it was */
	EOutboundQCountIncorrect = 35,
	/** Avctp doesn't support Ioctls */
	EIoctlsNotSupported = 36,
	/** Avctp doesn't support parameters being set on it's subconnections */
	EParameterSettingNotSupported = 37,
	/** Someone has tried to get a non Mtu Option */
	ENonMtuOption = 38,
	/** Stub Panic for exported Avctp functions */
	EAvctpStubUsedInANonAvBuild = 39,
	/** We expected to find a SapLinksMgr */
	ESapLinksMgrNotFound = 40,
	/** No events for this address */
	EMuxEventsNotFound = 41,
	/** DeliverEvent has been called, but there is no request from the client outstanding
	so this event would be dropped by ESock */
	EAttemptToDeliverEventWithNoNotificationOutstanding = 42,
	EUnexpectedErrorCode = 43,
	/**
	An unexpected comms-framework message has been received by a node
	This panic code should never happen
	*/
	EUnexpectedMessage = 43,
	
	/** 
	 An attempt was made to use an RNodeInterface who's communication channel was closed 
	 This panic code should never happen
	 */
	ENodeInterfaceUnusable = 44,
	EAvctpMuxerStateOutOfBounds = 45,
	EAvctpTransportNotFound = 46,
	EAvctpInvalidChannelNotify = 47,
	EAvctpInvalidChannel = 48,
	EAvctpUnexpectedIoctlCompletion = 49,
	EUnexpectedIdleMuxer = 50, 
	ENewDataNotSupported = 51,
	EAvctpClientNotFound = 52,
	EAvctpInvalidAddress = 53,
	/** SetOption called on an CServProviderBase returned an error*/
	ESetOptionError = 54,
	EGetOptionError = 55,
	EAvctpInvalidFragmenterState = 56,
	EAvctpEmptyIndicationQueue = 57,
	EAvctpInvalidSecChannelRefCount = 58,
	ESetRemNameError = 59,
	EAvctpFragmenterNonEmptyPdu = 60,
	EAvctpFragmenterEmptyPdu = 61,
	EAvctpFragmenterInvalidHeaderType = 62,
	EAvctpControlSAPOpetationNotSupported = 63,
	EAvctpDataSAPNullSocket = 64,
	EAvctpRMBufChainSplitError = 65,
	EAvctpInvalidChannelIoctl = 66,
	EAvctpSapClosed = 67,
	ENullAvctpProtocol = 68,
	};

/**
Panics raised by state machine default behaviour are multiplied by this
factor.  The less signifigant digits are then available to carry a state
index, allowing more information when fixing defects.

For example, a bad event passed to the Avctp muxer state machine may raise 
panic 'AvctpPrtPanic 161001' if it occurs in the 'Link Pending' state.
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

const TInt KBTAddressLength = 12;	// used to allocate a TBuf<> needed to get the readable BTDevAddress
void Panic(TPanic aPanic);

void Panic(TPanic aPanic, TInt aState);

} // end of namespace SymbianAvctp

#endif // AVCTPUTILS_H

