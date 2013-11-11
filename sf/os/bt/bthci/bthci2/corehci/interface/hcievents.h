// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef HCIEVENTS_H
#define HCIEVENTS_H

enum THCIEventCode
	{
	EInquiryCompleteEvent							= 0x01,
	EInquiryResultEvent								= 0x02,
	EConnectionCompleteEvent						= 0x03,
	EConnectionRequestEvent							= 0x04,
	EDisconnectionCompleteEvent						= 0x05,
	EAuthenticationCompleteEvent					= 0x06,
	ERemoteNameReqCompleteEvent						= 0x07,
	EEncryptionChangeEvent							= 0x08,
	EChangeLinkKeyEvent								= 0x09,
	EMasterLinkKeyEvent								= 0x0a,
	EReadRemSuppFeatCompleteEvent					= 0x0b,
	EReadRemVerInfoCompleteEvent					= 0x0c,
	EQOSSetupCompleteEvent							= 0x0d,
	ECommandCompleteEvent							= 0x0e,
	ECommandStatusEvent								= 0x0f,
	EHardwareErrorEvent								= 0x10,
	EFlushOccurredEvent								= 0x11,
	ERoleChangeEvent								= 0x12,
	ENumberOfCompletedPacketsEvent					= 0x13,
	EModeChangeEvent								= 0x14,
	EReturnLinkKeysEvent							= 0x15,
	EPINCodeRequestEvent							= 0x16,
	ELinkKeyRequestEvent							= 0x17,
	ELinkKeyNotificationEvent						= 0x18,
	ELoopbackCommandEvent							= 0x19,
	EDataBufferOverflowEvent						= 0x1a,
	EMaxSlotsChangeEvent							= 0x1b,
	EReadClockOffsetEvent							= 0x1c,
	EConnectionPacketTypeChangedEvent				= 0x1d,
	EQOSViolationEvent								= 0x1e,
	EPageScanModeChangeEvent						= 0x1f,
	EPageScanRepetitionModeChangeEvent				= 0x20,
	EFlowSpecificationCompleteEvent					= 0x21,
	EInquiryResultwithRSSIEvent						= 0x22,
	EReadRemoteExtendedFeaturesCompleteEvent		= 0x23,
	ESynchronousConnectionCompleteEvent				= 0x2c,
	ESynchronousConnectionChangedEvent				= 0x2d,
	ESniffSubratingEvent							= 0x2e,
	EExtendedInquiryResultEvent						= 0x2f,
	EEncryptionKeyRefreshCompleteEvent				= 0x30,
	EIOCapabilityRequestEvent						= 0x31,
	EIOCapabilityResponseEvent						= 0x32,
	EUserConfirmationRequestEvent					= 0x33,
	EUserPasskeyRequestEvent						= 0x34,
	ERemoteOOBDataRequestEvent						= 0x35,
	ESimplePairingCompleteEvent						= 0x36,
	ELinkSupervisionTimeoutChangedEvent				= 0x38,
	EEnhancedFlushCompleteEvent						= 0x39,
	EUserPasskeyNotificationEvent					= 0x3b,
	EKeypressNotificationEvent						= 0x3c,
	ERemoteHostSupportedFeaturesNotificationEvent	= 0x3d,
	EBTLogoTestingEvent								= 0xfe,
	EVendorDebugEvent								= 0xff
	};

#endif // HCIEVENTS_H
