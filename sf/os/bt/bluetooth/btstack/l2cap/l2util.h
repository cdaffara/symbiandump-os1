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
// Utility functions for L2CAP
// 
//

#ifndef L2UTIL_H
#define L2UTIL_H

_LIT(KL2CAPPanicName, "L2CAP Panic");

enum TL2CAPPanic
	{
	EL2CapPDUInvalidLength = 0,
	EL2CapPDUInvalidFrameType = 1,
	EL2CAPLinkDownWhenNotUp = 2,
	EL2CAPMuxAlreadyExists = 3,
	EL2CAPLinkDownConnectComplete = 4,
	EL2CAPProtocolOpenedAfterClose = 5,
	EL2CAPClonesExistDuringSAPDestructor = 6,
	EL2CAPSDUQueueStillExistsDuringSAPDestructor = 7,
	EL2CAPCloneHasNoReferenceToListeningSAP = 8,
	EL2SAPTwoIoctls = 9,
	EL2SAPStrayIoctlCancel = 10,
	EL2CAPSAPOpenWithConnectionDataNotSupported = 11,
	EL2CAPShutdownWithDisconnectionDataNotSupported = 12,
	EL2CAPPauseDataPlaneCalledWhenDataPlaneDoesNotExist = 13,
	EL2CAPAttemptToAsignCloneToSecondListeningSAP = 14,
	EL2CAPLinkUpWhenNoLinkRequested = 15,
	EL2CAPCouldNotSetBasebandAddress = 16,
	EL2CAPMuxDeleteWithResourcesStillRegistered = 17,
	EL2CAPPacketCompleteFailedToConsumeIncomingPDU = 18,
	EL2CAPUnexpectedSocketEvent = 19,
	EL2CAPUnexpectedSAPSignalHandlerEvent = 20,
	EL2CAPUnexpectedDataControllerEvent = 21,
	EL2CAPUnexpectedSecurityManagerEvent = 22,
	EL2CAPStateClosedWithData = 23,
	EL2CAPSDUPositionNegativeDuringSegmentation = 24,
	EL2CAPPDUBeingSentAfterFragmentationComplete = 25,
	EL2CAPInvalidHFragmentedPDUSenderStatusCode = 26,
	EL2CAPUnexpectedSocketUpcall = 27,
	EL2CAPRequestToResendAPDUThatIsAlreadyInTheResendQueue = 28,
	EL2CAPAttemptToChangeSegmentationForPartiallySentSDU = 29,
	EL2CAPBadDataPlaneConfiguration = 30,
	EL2CAPInvalidDataControllerTimerState = 31,
	EL2CAPIFrameOrSFrameHandledByBasicController = 32,
	EL2CAPUnexpectedSAPEvent = 33,
	
	EL2CAPInvalidConfigResponseCodeGenerated = 35,
	EL2CAPInvalidPeerConfigCodeGenerated = 36,
	EL2CAPInvalidCallToAppendPayloadToBuffer = 37,
	EL2CAPPDUSendPendingCalledTwice = 38,
	EL2CAPFragmentSenderDeletedWhileSending = 39,
	EL2CAPRequestToFragmentWhileBufferContainsData = 40,
	EL2CAPDeleteCFrameWhileContainingCommands = 41,
	EL2CAPAckTimerExpiryWithoutPDUToSupervise = 42,
	EL2CAPSigHandlerDeletedWithResources = 43,
	EL2CAPSAPSignalHandlerRegisteredTwice = 44,
	EL2CAPSSHTimerExpiredWhileInIdleState = 45,
	EL2CAPNoResponseSignalHandler = 46,
	EL2CAPAttemptToStartRTxTimerForResponseCommand = 47,
	EL2CAPAttemptToRegisterEntityConfigTwice = 48,
	EL2CAPAttemptToCreateCommandLongerThanSigMTU = 49,
	EL2CAPInvalidLinkMode = 50,
	EL2CAPUpdatePDUTimerDebugFailed = 51,
	EL2CAPInvalidDebugFlushCounterEvent = 52,
	EL2CAPInvalidPDUSAR = 53,
	EL2CAPInvalidSAPSHTimerState = 54,
	EL2CAPInvalidCommandRejectReason = 55,
	EL2CAPInvalidSAPShutdownType = 56,
	EL2CAPInvalidConfigOptionState = 57,
	EL2CAPAttemptToRestartFlushTimer = 58,
	EL2CAPGetOptionFailed =59,
	EL2CAPMuxerDeletedUnexpectedly = 60,
	EL2CAPMuxerStateOutOfBounds = 61,
	EL2CAPSAPMuxerStateOutOfBounds = 62,
	EL2CAPOptimalFragmentSizeTooBigForACLBuffer = 63,
	EL2CAPUnexpectedFragmentation = 64,
	EL2CAPACLBufferTooSmall = 65,
	EL2CAPReceivedPDUsDuringSRejSequenceError = 66,
	EL2CAPFecReconfigAttemptAfterInitialConfig = 68,
	EL2CAPFecConfigAttemptWithoutPeerInfo = 69,
	EL2CAPAsymmetricChannelModes = 70,
	EL2CAPUnexpectedGetPduLeave = 71,
	EL2CAPCompletedPDUOnAckedQ = 72,
	EL2CAPFecNegotiatorWasNotSetUp = 73,
//	EL2CAPFecNegotiatorSetUpCalledTwice = 74,
	EL2CAPUnknownChannelMode = 75,
	EL2CAPUnexpectedMonitorTimeout = 76,
//	EL2CAPStrayCallToProcessFinal = 77,
	EL2CAPFrameNotOnMissingList = 78,
	EL2CAPOldestSRejedFrameNotOnMissingList = 79,
	EL2CAPFrameNotOnMissingListNorDuplicate = 80,
	EL2CAPOldestUnackedPduTxSeqNotMatchingExpectedAck = 81,
	EL2CAPUnacknowledgedPdusMissingFromList = 82,
	EL2CAPPollFrameNumberTransmitIsZero = 83,
	EL2CAPPollFrameNumberTransmitIsNotZero = 84,
	EL2CAPMonitorTimerNotRunning = 85,
	EL2CAPIFramePutOnSduQOutOfSequence = 86,
	EL2CAPDataFrameNotPassedOnEntryToSRejSent = 87,
	EL2CAPLeftWithUnconsumedPdusAfterReceivingAllSRejedFrames = 88,
	EL2CAPExpectedTxSeqMismatchAfterReceivingAllSRejedFrames = 89,
	EL2CAPSduQNotExisitantInConnectComplete = 90,
	EL2CAPTryingToDequeueSRejedTxSeqFromEmptyQ = 91,
	EL2CAPGetNewIFrameCalledDuringRetransmission = 92,
	EL2CAPAckTimerRunningWhenRemoteBusy = 93,
	EL2CAPSRejReqSeqNotOnUnackedList = 94,
	EL2CAPSFrameQueuedForSendTwice = 95,
	EL2CAPAckedTxSeqFoundOnUnackedList = 96,
	EL2CAPNewTxSeqFoundOnUnackedList = 97,
	EL2CAPInRejSentAndLocalBusy = 98,
	EL2CAPInSRejSentAndLocalBusy = 99,
	EL2CAPCaughtInSRejSentWithNoMissingTxSeqs = 100,
	EL2CAPStrayPendingLocalBusy = 101,
//	EL2CAPErtmReceiveStateUnexpectedLeave = 102,
	EL2CAPNextExpectedTxSeqCalledWhenNoneAreExpected = 103,
	EL2CapSduQAndLocalBusyStateInconsistent = 104,
	EL2CAPRejSentEnteredWhenSduQFull = 105,
	EL2CAPNotInLocalBusyAfterTransitionToRecvFromSrejSentWithSduQFull = 106,
	EL2CapDuplicateIFramePassedToIncomingQ = 107,
	EL2CAPLocalBusyUnderIllegalConditions = 108,
	EL2CAPWindowInformationInconsistentWhenExitingSduQFull = 109,
	EL2CAPRetransmitPDUCalledForNormalTransmit = 110,
	EL2CAPNormalReceiveStateWhenSRejBufferIsNotEmpty = 111,
	EL2CAPNormalReceiveStateWhenMissingSRejFramesListIsNotEmpty = 112,
	EL2CAPZeroAclMtuInTryToSend = 113,
	EL2CAPFixedCIDIFrame = 114,
	EL2CAPReqSeqSetOnFrameAlreadyQueuedForSend = 115,
	EL2CAPSendSFrameWithAckCalledForSRejP0 = 116,
	EL2CAPEnterLocalBusyCalledWhileDelayTimerStillRunning = 117,
	EL2CAPBufferSeqNotEqualToExpectedTxSeqWhenInRecvAndSduQNotFull = 118,
	EL2CAPLocalBusyDelayTimerNotRunningWhenSduQFullButNotInLB = 119,
	EL2CAPStrayCallToDataControllerDeliveredOutgoingData = 120,
	EL2CAPWaitAckPendingInWaitAck = 121,
	EL2CAPRejSentEnteredWithBufferedFrames = 122,
	EL2CAPMultiSRejPathHitWhenNotSoConfigured = 123,
	EL2CAPUnownedPduOnMuxQ = 124,
	EL2CAPPutIFrameCalledWhenSDUQFull = 125,
	EL2CAPReceivedTxSeqNotMatchingSRejSentSubcase = 126,
	EL2CAPPreviouslyAcceptedFecValueSuddenlyUnacceptable = 127,
	EL2CAPPeerAndPreferredChannelModesNotEqual = 128,
	EL2CAPFlushingNotReallySupported = 129,
	EL2CAPUnackedTxSeqNotFoundOnUnackedList = 130,
	EL2CAPHaveSRejsToResendEvenThoughReceivedFirstInOrder = 131,
	EL2CAPLastTxSeqCalledWhenNoneAreExpected = 132,
	EL2CAPGivenMoreSRejedTxSeqsToTraceThanCanStore = 133,
	EL2CAPStrayCallToGetTxSeqForResend = 134,
	EL2CAPNextTxSeqExpectedBySduQNotEqualToBufferSeqWhenNotInSRejSent = 135,
	EL2CAPAttemptToEnterWaitFWhenAlreadyInAWaitState = 136,
	EL2CAPAttemptToEnterWaitAckWhenAlreadyInAWaitState = 137,
	EL2CAPTryingToAppendSameTxSeqIFrameTwice = 138,
	EL2CAPTryingToRemoveUnackedIFrameWithSameTxSeqButDifferentAddress = 139,
	EL2CAPFlushingIncomingIFramesInStateOtherThanRecv = 140,
	EL2CAPExpectedTxSeqNotEqualToBufferSeqWhenIncomingQEmpty = 141,
	EL2CAPWindowInformationInconsistentAfterMovingBufferSeqWhenSduQFull = 142,
	EL2CAPExpectedTxSeqAndBufferSeqAndTxSeqExpectedBySduQNotEqualWhenIncomingQEmptyAndInRecvState = 143,
	EL2CAPTxSeqExpectedBySduQNotWithinBufferSeqAndExpectedTxSeq = 144,
	EL2CAPUpdateLocalConfigCalledWithoutPeerEntityConfig = 145,
	EL2CAPOpenChannelRequestCalledTwice = 146,
	EL2CAPIFrameSentInWaitAck = 147,
	EL2CAPNullOwnerSupplied = 148,
	EL2CAPTryingToStealOwnedPdu = 149,
	EL2CapConstructingPositiveConfigResponseWithUnresolvedOptionStatus = 200,
	EL2CapReferencingUnackedIFrameWhenNoRetransmissionGoing = 274,
	EL2CapAttemptToStartConfigTimerWhenItIsRunning = 275,
	};

/**
Panics raised by state machine default behaviour are multiplied by this
factor.  The less signifigant digits are then available to carry a state
index, allowing more information when fixing defects.

For example, 'L2CAP Panic 331008' is a UnexpectedSAPEvent panic in the 
'Open' state.
*/
const TInt KL2StatePanicMultiplier = 10000;

/**
A delimiter to prevent a panic code of 0 creating ambigious panics.
Without this panic code 0 in state 8 and panic code 8 (not in the state 
machine) would be identical.
*/
const TInt KL2StatePanicDelimiter = 1000; // '1' is the delimiter

/**
If for any reason the current state cannot be found, this value is placed
in the state field of a panic.
*/
const TInt KL2UnknownState = KL2StatePanicDelimiter - 1;

void Panic(TL2CAPPanic aPanic);

void Panic(TL2CAPPanic aPanic, TInt aState);

#endif

