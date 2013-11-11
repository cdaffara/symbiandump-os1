// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Utility functions for AVDTP
// 
//

/**
 @file
 @internalComponent
*/


#ifndef AVDTPUTIL_H
#define AVDTPUTIL_H

#include <bluetoothav.h>

#define __SYMBIAN_AVDTP_HIDE_MUX 

#ifndef __SYMBIAN_AVDTP_HIDE_MUX
#pragma message("Note: MUX support is untested")
#endif

using namespace SymbianBluetoothAV;

_LIT(KAvdtpPanicName, "AVDTP Panic");

enum TAvdtpPanic
	{
	EAVDTPBadSessionCount,
	EAVDTPBadSessionSet,
	EAVDTPBadDropSession,
	EAVDTPBadRemoveStream,
	EAvdtpStreamAlreadyExists,
	EAVDTPCorruptMessage,
	EAVDTPBadSessionDetachFromTransportChannel,
	EAVDTPBadSessionAttachToTransportChannel,
	EAVDTPBadMuxConfiguration,
	EAVDTPBadMuxSetConfigPacket,
	EAVDTPBadStreamConstitution,
	EAVDTPSAPInitiatedServiceCompleteNoIoctlOutstanding,
	EAvdtpInvalidResponseInUse,
	EAvdtpBadIoctl,
	EAvdtpBadSEPCount,
	ERGAVDPSequenceFailure,
	EAvdtpSAPIndicationEngineFailure,
	EAvdtpBadErrorCase,
	EAvdtpBadSignallingUserQueue,
	EAvdtpBadLocalSEPUpdate,
	EAvdtpBadSessionAttachingToStream,
	EAvdtpSearchingForDirectChannel,
	EAvdtpIncorrectlyFoundDirectChannel,
	EAvdtpProtocolUsingWrongListeningMechanism,
	EAvdtpGavdpHasNotCheckedSEID,
	EAvdtpConfirmAddressedToWrongRequester,
	EAvdtpStackIssuedBadSEID,
	EAvdtpIndicationResult_Accepted_NotChecked,
	EAvdtpOutboundMessageNewDataCalled,
	EAvdtpTransactionLabelUnionMisused,
	EAvdtpBadLocalSEPInstructingStreamRelease,
	EAvdtpProtocolDyingWithTransportChannelsOnQueue,
	EAvdtpProtocolDyingWithSignallingChannelsOnQueue,
	EAvdtpProtocolDyingWithStreamsOnQueue,
	EAvdtpSignallingChannelShouldExist,
	EAvdtpProtocolReceivingControlFromUnexpectedProtocol,
	EAvdtpProtocolReceivingControlFromUnexpectedPort,
	EAvdtpProtocolReceivingBadlyFormedControlMessage,
	EAvdtpProtocolAskedForSecondarySAPWhenNonExist,
	EAvdtpConfiguratorsMultiplexingUnexpectedlySet,
	EAvdtpSEIDHasWrongDomain,
	EAvdtpReconfigurationCapabilitiesNotChecked,
	EAvdtpLogicalChannelFactoryJobHasNoLogicalChannels,
	EAvdtpLogicalChannelFactoryBadClaimLogicalChannel,
	EAvdtpLogicalChannelFactoryBadSequenceNumber,
	EAvdtpUnexpectedErrorFromL2CAP,
	EAvdtpBadlyFormattedOutboundSignallingMessage,
	EAvdtpGotBadDataFromL2CAP,
	EAvdtpSignallingChannelDrainingFaulty,
	EAvdtpSignallingChannelDyingWithDraftMessagesOnQueue,
	EAvdtpSignallingChannelDyingWithTransactionsOnQueue,
	EAvdtpSignallingChannelDyingWithMessagesOnQueue,
	EAvdtpSignallingChannelGotBadSyncResponseFromSignallingSession,
	EAvdtpSignallingChannelLogicalChannelNotReady,
	EAvdtpSignallingMessageResponseNoTrailingDataExpected,
	EAvdtpSignallingMessageResponseTrailingDataExpected,
	EAvdtpSignallingSessionReceivedBadIndication,
	EAvdtpSignallingSessionFoundWrongStream,
	EAvdtpTransportSessionPacketPoolNotZeroSize,
	EAvdtpTransportSessionPacketBalkingNotSet,
	EAvdtpBadTransportSessionUpcallFromTransportChannel,
	EAvdtpBadSessionAccessingMuxChannel,
	EAvdtpTransportSessionBaseNotCheckStream,
	EAvdtpStreamStateMachineFailed,
	EAvdtpUnknownPostSendAction,
	EAvdtpSignallingMessageHandlerLeft,
	EAvdtpProtocolToldToRemoveUnknownTransportChannel,
	EAvdtpSignallingChannelNotifiedOfWrongJob,
	EAvdtpProtocolDeletionWhilstIdling,
	EAvdtpInvalidTimeout,
	EAvdtpLogicalChannelArrayMismatchesIndexAndSequence,
	EAvdtpUnexpectedAwaitLogicalChannelsEvent,
	EAvdtpUnexpectedLogicalChannelsAvailableEvent,
	EAvdtpUnexpectedWatchdogFiredEvent,
	EAvdtpUnexpectedConfiguredEvent,
	EAvdtpUnexpectedSetConfigurationEvent,
	EAvdtpUnexpectedAddSessionEvent,
	EAvdtpUnexpectedDropSession,
	EAvdtpUnexpectedStartedEvent,
	EAvdtpUnexpectedSuspendedEvent,
	EAvdtpPassingSapOwnershipToChannelThatAlreadyHasASap,
	EAvdtpJobCompleteForUnknownCloseChannelJob,
	EAvdtpTransferSapCalledForMuxChannel,
	EAvdtpManagedChannelToldToShutdownWithNoSAP,
	EAvdtpStartedListeningAgain,
	EAvdtpNotListening,
	EAvdtpStillListening,
	EAvdtpInconsistentPendingConfigurationState,
	EAvdtpInconsistentPendingReconfigurationState,
	EAvdtpPassingNullSapOwnershipToChannel,
	EAvdtpMuxerStateOutOfBounds,
	EAvdtpInvalidLogicalChannel,
	EAvdtpNotifyCompleteCancelledRequest,
	EAvdtpUnexpectedIndicationsInQueue,
	EAvdtpLogicalChannelFactoryDeletionWhileJobPending,
	EUnexpectedEncryptionKeyRefreshPrevent,
	EAvdtpInvalidReservedValueInOutboundSignallingMessage
	};

/**
Panics raised by state machine default behaviour are multiplied by this
factor.  The less signifigant digits are then available to carry a state
index, allowing more information when fixing defects.

For example, the Avdtp stream state machine failing may raise 
panic 'AVDTP Panic 631000' if it occurs in the 'Idle' state.
*/
const TInt KAvdtpStatePanicMultiplier = 10000;

/**
A delimiter to prevent a panic code of 0 creating ambigious panics.
Without this panic code 0 in state 8 and panic code 8 (not in the state 
machine) would be identical.
*/
const TInt KAvdtpStatePanicDelimiter = 1000; // '1' is the delimiter

/**
If for any reason the current state cannot be found, this value is placed
in the state field of a panic.
*/
const TInt KAvdtpUnknownState = KAvdtpStatePanicDelimiter - 1;
	
void Panic(TAvdtpPanic aPanic);
void Panic(TAvdtpPanic aPanic, TInt aState);

#endif
