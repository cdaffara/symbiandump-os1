// Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LINKUTIL_H
#define LINKUTIL_H

_LIT(KLinkPanicName, "LinkMgr Panic");

enum TLinkPanic
	{
	ELinkEntryNotFound					=0,
	ELinkMgrPacketMTUUseBeforeSet		=1,
	EHCIBufferMgrBadState				=2,
	EHostResolverTwoRequests			=3,
	ELinkMgrFIFOLevelZeroOrBelowZero	=4,
	EHCICommandCreditsAreZero			=5,
	EHCICommandCouldNotBeCommited		=6,
	EHCIACLDataControllerDataCreditsZeroBeforeIssue			=7,
	EHCIACLDataControllerConnectionCouldNotBeFound			=8,
	EHCIACLDataControllerDataCreditsNotConsistentAfterReset	=9,
	EHCIACLDataControllerDroppedConnectionCouldNotBeFound	=10,
	EHCIACLDataControllerMaxAllowedDataCreditsExceeded		=11,
	EHCIACLDataControllerConnectionListConnectionNotFound	=12,
	EHCIACLDataControllerFlushCompleteError	=13,
	ELinkMgrFIFOLevelAboveCeiling			=14,
	EHCIFIFOLevelBelowZero					=15,
	ELinkMgrNoSuchFlowControlMode			=16,
	ELinkMgrNoFlowControlSetInReleaseBuild	=17,
	ELinkMgrDataQBufferIsNotSufficient		=18,
	ELinkMgrClientHasOutstandingQueueObject	=19,
	ELinkMgrBadFlowControlSetInReleaseBuild	=20,
	ELinkMgrOverflowedHostController		=21,
	ELinkMgrBadSCOState						=22,
	EBTConnectionUnknownLowPowerMode		=23,
	EBTLinkMgrConnectionEventInWrongSAP		=24,
	ELinkMgrBadBasebandArray				=25,
	ELinkMgrBadDeviceChangeArray			=26,
	EBTLinkMgrBasebandModelInvalid			=27,
	EBTConnectionPINRequestedTwice			=28,
	EBTConnectionBadDeviceAddress			=29,
	EBTConnectionListenerUsingRegistry		=30,
	EBTBaseSAPUnimplementedEvent			=31,
	EBTProxySAPUnexpectedEvent				=32,
	EBTProxySAPBadCanClose					=33,
	EBTBasebandSAPTwoPhysicalLinks			=34,
	EBTACLSAPUnexpectedEvent				=35,
	EBTACLAlreadyBoundToPhysicalLink		=36,
	EBTACLLogicalLinkBadDebind				=37,
	EBTPHYWithoutACLLogicalLinkBound		=38,
	EBTSCOAlreadyBoundToPhysicalChannel		=39,
	EBTSCOLogicalLinkBadDebind				=40,
	EBTUnknownLogicalLink					=41,
	EBTConnectionMgrCountBelowZero			=42,
	EBTACLSAPUnsupportedQueSize				=43,
	EBTConnectionRequestWithNoListeners		=44,
	EBTConnectionMgrBadListener				=45,
	EBTACLSAPChildlessParent				=46,
	EBTACLSAPParentlessChild				=47,
	EBTACLSAPDataForWrongSAP				=48,
	EBTACLSAPNullSocket						=49,
	EBTACLSAPWrongPhysicalLink				=50,
	EBTSCOSAPWrongPhysicalLink				=51,
	EBTSCOSAPExistsWhenParking				=52,
	EBTSCOSAPChildlessParent				=53,
	EBTSCOSAPParentlessChild				=54,
	EBTSCOSAPDataForWrongSAP				=55,
	EBTSCOSAPNullSocket						=56,
	EBTSCOSAPMaxDataLengthExceeded			=57,
	EBTProxySAPCannotFindPhysicalLink		=58,
	EBTProxyUserAttemptingToTransmitL2CAPDirectData	=59,
	EBTProxySAPNotReadyToSendRawData		=60,
	EBTProxySAPNoRawConduit					=61,
	EBTProxySAPRawConduitAlreadyExists		=62,
	EBTProxySAPInvalidCreate				=63,
	EBTProxySAPInvalidIoctl					=64,
	EBTProxySAPInvalidEventMask				=65,
	EBTACLRawConduitError					=66,
	EBTACLSAPDidntAcceptInTime				=67,
	EBTACLSAPListenerSettingName			=68,
	EBTACLSAPCorruptInboundStore			=69,
	EBTSCOSAPUnexpectedEvent				=70,
	EBTSCOSAPWrongSCOSAP					=71,
	EBTProxySAPInvalidTerminate				=72,
	EBTProxySAPNullCallback					=73,
	EBTPhysicalLinkBadPort					=74,
	EBTBasebandSAPBadChild					=75,
	EBTACLSAPBadFlagsOnWrite				=76,
	EBTACLSAPNotSupportedFeature			=77,
	EBTBasebandModelBadRecord				=78,
	EBTACLSAPToldRoleSwitchWhenParked		=79,
	EBTACLSAPDoesNotSupportBroadcastL2CAP	=80,
	EBTACLSAPDoesNotSupportPiconetBroadcast	=81,
	EVendorSAPBadCall						=82,
	EVendorTwoIoctls						=83,
	EBTPhysicalLinksManagerConnectingToNullAddress		=84,
	ELinkMgrHostControllerHasOverflowedHost				=85,
	ELinkMgrCouldNotSendHostNumberOfCompletedPackets	=86,
	EBTPhysicalLinkNotAuthenticated			=87,
	EBTPhysicalLinksInvalidAddress			=88,
	EBTNonSyncConnectInSyncConnectFunc		=89,
	EHCIDirectAccessSAPBadCall				=90,
	EHCIDirectAccessTwoIoctls				=91,
	EHCIDirectAccessBadIoctlCompletion		=92,
	EHCIDirectAccessBadIoctlCancel			=93,
	EBTPhysicalLinkModeChangeErrorLeavingSniffMode	=94,
	EHCICommandNullItem						=95,
	EBTBasebandInvalidLinkState				=96,
	ERoleSwitcherInvalidState				=97,
	ERoleSwitcherStateMachineInvalidEvent	=98,
	ERoleSwitchersNotDeleted				=99,
	EBTACLSAPWriteDataItemWithNullFrame		=100,
	EAclSapChildStillPresentAtDestruction	=101,
	EPendingConnectionNotCleared			=102,
	EPairedBDAddrGetterUnexpectedState		=103,
	EBTPairingsCacheRemovalOfUnpairedDevice	=104,
	EBTPairingsCacheDuplicatePairingInCache	=105,
	EBTPairingsCacheBadState				=106,
	EPhysicalLinkNoConnectionPending		=107,
	EBTPairingsCacheNotEmpty				=108,
	EHostResolverBadHWState					=109,
	//Panic code values between 110 and 159 are reserved
	//to distinguish older code.
	//Please do not use these values in future upgrades.
	EACLLinkStateOutOfBounds				=160,
	ESCOInboundPacketTooLarge				=161,
	EVendorSpecificCommandIncorrectSize		=162,
	EEncryptionEnforcerBadState				=163,
	EBTInvalidEncryptionDisableRequest		=164,
	EBTProxySAPAlreadySubscribed			=165,
	EBTProxySAPSubscribedToBroadcastHandle	=166,
	EVendorSAPBadIoctlCancel				=167,
	EDisconnectRequestedNotOnACLLink		=168,
	EInvalidLinkStateDuringDisconnect		=169,
	EHCIControllerInitialisation			=170,
	EHCIIncompleteInterfaces				=171,
	EHCIMultipleCoreHciImplementations		=172,
	EHCIUnknownDataEvent					=173,
	EHCIUnknownCommandEvent					=174,
	EHCIUnknownUnsolicitedEvent				=175,
	EHCIUnknownCommandCompleteOpcode		=176,
	EHCICmdQNotInitialising					=177,
	EHCICommandBadArgument					=178,
	EHCICtrlrInitFailedToRemoveCmd			=179,
	EHCICtrlrInitNoOutstandingCmds			=180,
	EHCICtrlrInitCmdNotFound				=181,
	EHCIUnknownHardResetState				=182,
	EHCIUnknownPowerState					=183,
	EBTPhysicalLinksInvalidArgument			=184,
	EHCIPowerStateError						=185,
	EIncorrectStateOnPreResetCallback		=186,
	EIncorrectStateOnPostResetCallback		=187,
	EBTNameLookupResultNotFound				=188,
	EBTInvalidInquiryMode					=189,
	EBTInvalidInquiryLogEntry				=190,
	EBTUnexpectedLinkKeyType				=191,
	EBTInvalidDataInPhysicalLinkMetric		=192,
	EBTHostControllerQueueNotEmpty			=193,
	EBTCommandQueueAlreadyExist				=194,
	EBTCommandQueueNotFound					=195,
	EBTUnexpectedIAC						=196,
	ETRegistryDevBeingModUnexpectedCompleted=197,
	EDiscCtrlUnexpectedConnectionHandle		=198,
	EDiscCtrlUnmatchedCommandEvent			=199,
	EDiscCtrlUnexpectedCommandEvent			=200,
	EHCIUnmatchedInquiryEvent				=201,
	EBTInquiryMgrMismatchedStatusEvent		=202,
	EBTInquiryMgrUnmatchedEvent				=203,
	EBTPhysicalLinkPinHandlerStillPresent	=204,
	EBTPrefetchManagerReplyForNoRequest		=205,
	EHCIUnexpectedEvent						=206,
	EUnexpectedIOCapability					=207,
	EEIRPublisherUpdateNameTooLong			=208,
	EBTSSPModeChangedDuringConnection		=209,
	EHCICtrlrInitAddingInitialisationCommandInBadState	=210,
	EHCICtrlrInitOnlyOneResetCmdAllowed		=211,
	EInvalidDisconnectReason				=212,

	//Please add new panic codes common to all versions of linkmgr here.
	EVendorSAPBadVendorEvent				=250,
	EBTACLSAPIndeterminateInitiator			=251,
	EBTACLSAPUnexpectedSecurityResult		=252,
	ELinkMgrFlowControlUnexpectedUpdate		=253,
	ELinkMgrFlowControlChangeOfMind			=254,
	EEncryptionKeyRefresherInvalidState		=255,
	EEncryptionKeyRefresherStateMachineInvalidEvent = 256,
	};

/**
Panics raised by state machine default behaviour are multiplied by this
factor.  The less signifigant digits are then available to carry a state
index, allowing more information when fixing defects.

For example, a bad event passed to the ACL state machine may raise panic
'Link Panic 351008' if it occurs in the 'Open' state.
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

void Panic(TLinkPanic aPanic);

void Panic(TLinkPanic aPanic, TInt aState);



#endif
