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

#ifndef BTFEATURES_H
#define BTFEATURES_H

/**
@internalTechnology

Bit positions of the individual local and remote features.
@see TBTFeatures
*/
enum TBTSupportedFeaturesBits
	{
	ESupportedThreeSlotPacketsBit				= 0,  /*!< Three slot packets are supported */
	ESupportedFiveSlotPacketsBit				= 1,  /*!< Five slot packets are supported */
	ESupportedEncryptionBit						= 2,  /*!< Encryption is supported */
	ESupportedSlotOffsetBit						= 3,  /*!< Slot offset is supported */
	ESupportedTimingAccuracyBit					= 4,  /*!< Timing accuracy is supported */
	ESupportedSwitchBit							= 5,  /*!< Role switch is supported */
	ESupportedHoldModeBit						= 6,  /*!< Hold mode is supported */
	ESupportedSniffModeBit						= 7,  /*!< Sniff mode is supported */
	ESupportedParkModeBit						= 8,  /*!< Park mode is supported */
	ESupportedRSSIBit							= 9,  /*!< Receive signal strength indication is supported */
	ESupportedChannelQualityDrivenDataRateBit	= 10, /*!< Channel quality driven data rate is supported */
	ESupportedSCOLinkBit						= 11, /*!< SCO links are supported */
	ESupportedHV2PacketsBit						= 12, /*!< HV2 packets are supported */
	ESupportedHV3PacketsBit						= 13, /*!< HV3 packets are supported */
	ESupportedu_lawLogBit						= 14, /*!< SCO u-law encoding is supported */
	ESupportedA_lawLogBit						= 15, /*!< SCO A-law encoding is supported */
	ESupportedCVSDBit							= 16, /*!< SCO Continuously variable slope delta modulation is supported */
	ESupportedPagingSchemeBit					= 17, /*!< Paging scheme is supported */
	ESupportedPowerControlBit					= 18, /*!< Power control is supported */
	ESupportedTransparentSCODataBit				= 19, /*!< Transparent SCO data is supported */
	ESupportedFlowControlLagBit0Bit				= 20, /*!< Flow control lag bit 0 is supported */
	ESupportedFlowControlLagBit1Bit				= 21, /*!< Flow control lag bit 1 is supported */
	ESupportedFlowControlLagBit2Bit				= 22, /*!< Flow control lag bit 2 is supported */
	ESupportedBroadcastEncryptionBit			= 23, /*!< Broadcast encryption is supported */
	
	EEDR_ACL_2MbpsModeBit						= 25, /*!< 2Mbps EDR ACL supported */
	EEDR_ACL_3MbpsModeBit						= 26, /*!< 3Mbps EDR ACL supported */
	ESupportedEnhancedEnquiryScanBit			= 27, /*!< Enhanced enquiry scan is supported */
	ESupportedInterlacedEnquiryScanBit			= 28, /*!< Interlaced enquiry scan is supported */
	ESupportedInterlacedPageScanBit				= 29, /*!< Interlaced page scan is supported */
	ESupportedRSSIWithInquiryResultsBit			= 30, /*!< Read signal strength indication with inquiry is supported */
	ESupportedExtendedSCOLinkBit				= 31, /*!< Extended SCO links are supported */
	ESupportedEV4PacketsBit						= 32, /*!< EV4 packets are supported */
	ESupportedEV5PacketsBit						= 33, /*!< EV5 packets are supported */
	
	ESupportedAFHCapableSlaveBit				= 35, /*!< Adaptive frequency hopping in slave role is supported */
	ESupportedAFHClassificationSlaveBit			= 36, /*!< Adaptive frequency hopping classification in slave role is supported */

	ESupportedEDRThreeSlotPacketsBit			= 39, /*!< Three slot EDR packets are supported */
	ESupportedEDRFiveSlotPacketsBit				= 40, /*!< Five slot EDR packets are supported */
	ESupportedSniffSubratingBit					= 41, /*!< Sniff subrating is supported */
	ESupportedEncryptionPauseResumeBit			= 42, /*!< Encryption Pause and Resume is supported */
	ESupportedAFHCapableMasterBit				= 43, /*!< Adaptive frequency hopping in master role is supported */
	ESupportedAFHClassificationMasterBit		= 44, /*!< Adaptive frequency hopping classification in master role is supported */
	EEDR_eSCO_2MbpsModeBit						= 45, /*!< 2Mbps EDR eSCO supported */
	EEDR_eSCO_3MbpsModeBit						= 46, /*!< 3Mbps EDR eSCO supported */
	ESupportedEDReSCOThreeSlotPacketsBit		= 47, /*!< Three slot EDR eSCO packets are supported */
	ESupportedExtendedInquiryResponseBit		= 48, /*!< Extended Inquiry Response */
	
	ESupportedSecureSimplePairingBit			= 51, /*!< Secure Simple Pairing */
	ESupportedEncapsulatedPDUBit				= 52, /*!< Encapsulated PDU */
	ESupportedErroneousDataReportingBit			= 53, /*!< Erroneous Data Reporting */
	ESupportedNonflushablePacketBoundaryFlagBit	= 54, /*!< Non-flushable Packet Boundary Flag */
	
	ESupportedLinkSupervisionTimeoutChangedEventBit	= 56, /*!< Link Supervision Timeout Changed Event */
	ESupportedInquiryResponseTXPowerLevelBit	= 57, /*!< Inquiry Response TX Power Level */
	
	ESupportedExtendedFeaturesBit				= 63, /*!< Additional features are supported */
	};

/**
@internalTechnology

Bit positions of the individual local supported HCI commands.
@see TBluetoothHciCommands
*/
enum TBluetoothSupportedCommandsBits
	{
	ESupportedInquiryCommand								= 0, /*!< Inquiry command is supported */
	ESupportedInquiryCancelCommand							= 1, /*!< Inquiry Cancel command is supported */
	ESupportedPeriodicInquiryModeCommand					= 2, /*!< Periodic Inquiry Mode command is supported */
	ESupportedExitPeriodicInquiryModeCommand				= 3, /*!< Exit Periodic Inquiry Mode command is supported */
	ESupportedCreateConnectionCommand						= 4, /*!< Create Connection command is supported */
	ESupportedDisconnectCommand								= 5, /*!< Disconnect command is supported */
	ESupportedAddSCOConnectionCommand						= 6, /*!< Add SCO Connection command is supported */
	ESupportedCancelCreateConnectionCommand					= 7, /*!< Cancel Create Connection command is supported */
	ESupportedAcceptConnectionRequestCommand				= 8, /*!< Accept Connection Request command is supported */
	ESupportedRejectConnectionRequestCommand				= 9, /*!< Reject Connection Request command is supported */
	ESupportedLinkKeyRequestReplyCommand					= 10, /*!< Link Key Request Reply command is supported */
	ESupportedLinkKeyRequestNegativeReplyCommand			= 11, /*!< Link Key Request Negative Reply command is supported */
	ESupportedPINCodeRequestReplyCommand					= 12, /*!< PIN Code Request Reply command is supported */
	ESupportedPINCodeRequestNegativeReplyCommand			= 13, /*!< PIN Code Request Negative Reply command is supported */
	ESupportedChangeConnectionPacketTypeCommand				= 14, /*!< Change Connection Packet Type command is supported */
	ESupportedAuthenticationRequestCommand					= 15, /*!< Authentication Request command is supported */
	ESupportedSetConnectionEncryptionCommand				= 16, /*!< Set Connection Encryption command is supported */
	ESupportedChangeConnectionLinkKeyCommand				= 17, /*!< Change Connection Link Key command is supported */
	ESupportedMasterLinkKeyCommand							= 18, /*!< Master Link Key command is supported */
	ESupportedRemoteNameRequestCommand						= 19, /*!< Remote Name Request command is supported */
	ESupportedCancelRemoteNameRequestCommand				= 20, /*!< Cancel Remote Name Request command is supported */
	ESupportedReadRemoteSupportedFeaturesCommand			= 21, /*!< Read Remote Supported Features command is supported */
	ESupportedReadRemoteExtendedFeaturesCommand				= 22, /*!< Read Remote Extended Features command is supported */
	ESupportedReadRemoteVersionInformationCommand			= 23, /*!< Read Remote Version Information command is supported */
	ESupportedReadClockOffsetCommand						= 24, /*!< Read Clock Offset command is supported */
	ESupportedReadLMPHandleCommand							= 25, /*!< Read LMP Handle command is supported */
	ESupportedHoldModeCommand								= 33, /*!< Hold Mode command is supported */
	ESupportedSniffModeCommand								= 34, /*!< Sniff Mode command is supported */
	ESupportedExitSniffModeCommand							= 35, /*!< Exit Sniff Mode command is supported */
	ESupportedParkStateCommand								= 36, /*!< Park State command is supported */
	ESupportedExitParkStateCommand							= 37, /*!< Exit Park State command is supported */
	ESupportedQoSSetupCommand								= 38, /*!< QoS Setup command is supported */
	ESupportedRoleDiscoveryCommand							= 39, /*!< Role Discovery command is supported */
	ESupportedSwitchRoleCommand								= 40, /*!< Switch Role command is supported */
	ESupportedReadLinkPolicySettingsCommand					= 41, /*!< Read Link Policy Settings command is supported */
	ESupportedWriteLinkPolicySettingsCommand				= 42, /*!< Write Link Policy Settings command is supported */
	ESupportedReadDefaultLinkPolicySettingsCommand			= 43, /*!< Read Default Link Policy Settings command is supported */
	ESupportedWriteDefaultLinkPolicySettingsCommand			= 44, /*!< Write Default Link Policy Settings command is supported */
	ESupportedFlowSpecificationCommand						= 45, /*!< Flow Specification command is supported */
	ESupportedSetEventMarkCommand							= 46, /*!< Set Event Mark command is supported */
	ESupportedResetCommand									= 47, /*!< Reset command is supported */
	ESupportedSetEventFilterCommand							= 48, /*!< Set Event Filter command is supported */
	ESupportedFlushCommand									= 49, /*!< Flush command is supported */
	ESupportedReadPINTypeCommand							= 50, /*!< Read PIN Type command is supported */
	ESupportedWritePINTypeCommand							= 51, /*!< Write PIN Type command is supported */
	ESupportedCreateNewUnitKeyCommand						= 52, /*!< Create New Unit Key command is supported */
	ESupportedReadStoredLinkKeyCommand						= 53, /*!< Read Stored Link Key command is supported */
	ESupportedWriteStoredLinkKeyCommand						= 54, /*!< Write Stored Link Key command is supported */
	ESupportedDeleteStoredLinkKeyCommand					= 55, /*!< Delete Stored Link Key command is supported */
	ESupportedWriteLocalNameCommand							= 56, /*!< Write Local Name command is supported */
	ESupportedReadLocalNameCommand							= 57, /*!< Read Local Name command is supported */
	ESupportedReadConnectionAcceptTimeoutCommand			= 58, /*!< Read Connection Accept Timeout command is supported */
	ESupportedWriteConnectionAcceptTimeoutCommand			= 59, /*!< Write Connection Accept Timeout command is supported */
	ESupportedReadPageTimeoutCommand						= 60, /*!< Read Page Timeout command is supported */
	ESupportedWritePageTimeoutCommand						= 61, /*!< Write Page Timeout command is supported */
	ESupportedReadScanEnableCommand							= 62, /*!< Read Scan Enable command is supported */
	ESupportedWriteScanEnableCommand						= 63, /*!< Write Scan Enable command is supported */
	ESupportedReadPageScanActivityCommand					= 64, /*!< Read Page Scan Activity command is supported */
	ESupportedWritePageScanActivityCommand					= 65, /*!< Write Page Scan Activity command is supported */
	ESupportedReadInquiryScanActivityCommand				= 66, /*!< Read Inquiry Scan Activity command is supported */
	ESupportedWriteInquiryScanActivityCommand				= 67, /*!< Write Inquiry Scan Activity command is supported */
	ESupportedReadClassOfDeviceCommand						= 72, /*!< Read Class Of Device command is supported */
	ESupportedWriteClassOfDeviceCommand						= 73, /*!< Write Class Of Device command is supported */
	ESupportedReadVoiceSettingCommand						= 74, /*!< Read Voice Setting command is supported */
	ESupportedWriteVoiceSettingCommand						= 75, /*!< Write Voice Setting command is supported */
	ESupportedReadAutomaticFlushTimeoutCommand				= 76, /*!< Read Automatic Flush Timeout command is supported */
	ESupportedWriteAutomaticFlushTimeoutCommand				= 77, /*!< Write Automatic Flush Timeout command is supported */
	ESupportedReadNumBroadcastRetransmissionsCommand		= 78, /*!< Read Num Broadcast Retransmissions command is supported */
	ESupportedWriteNumBroadcastRetransmissionsCommand		= 79, /*!< Write Num Broadcast Retransmissions command is supported */
	ESupportedReadHoldModeActivityCommand					= 80, /*!< Read Hold Mode Activity command is supported */
	ESupportedWriteHoldModeActivityCommand					= 81, /*!< Write Hold Mode Activity command is supported */
	ESupportedReadTransmitPowerLevelCommand					= 82, /*!< Read Transmit Power Level command is supported */
	ESupportedReadSynchronousFlowControlEnableCommand		= 83, /*!< Read Synchronous Flow Control Enable command is supported */
	ESupportedWriteSynchronousFlowControlEnableCommand		= 84, /*!< Write Synchronous Flow Control Enable command is supported */
	ESupportedSetHostControllerToHostFlowControlCommand		= 85, /*!< Set Host Controller To Host Flow Control command is supported */
	ESupportedHostBufferSizeCommand							= 86, /*!< Host Buffer Size command is supported */
	ESupportedHostNumberOfCompletedPacketsCommand			= 87, /*!< Host Number Of Completed Packets command is supported */
	ESupportedReadLinkSupervisionTimeoutCommand				= 88, /*!< Read Link Supervision Timeout command is supported */
	ESupportedWriteLinkSupervisionTimeoutCommand			= 89, /*!< Write Link Supervision Timeout command is supported */
	ESupportedReadNumberofSupportedIACCommand				= 90, /*!< Read Number of Supported IAC command is supported */
	ESupportedReadCurrentIACLAPCommand						= 91, /*!< Read Current IAC LAP command is supported */
	ESupportedWriteCurrentIACLAPCommand						= 92, /*!< Write Current IAC LAP command is supported */
	ESupportedReadPageScanModeCommand						= 95, /*!< Read Page Scan Mode command is supported */
	ESupportedWritePageScanModeCommand						= 96, /*!< Write Page Scan Mode command is supported */
	ESupportedSetAFHChannelClassificationCommand			= 97, /*!< Set AFH Channel Classification command is supported */
	ESupportedReadInquiryScanTypeCommand					= 100, /*!< Read Inquiry Scan Type command is supported */
	ESupportedWriteInquiryScanTypeCommand					= 101, /*!< Write Inquiry Scan Type command is supported */
	ESupportedReadInquiryModeCommand						= 102, /*!< Read Inquiry Mode command is supported */
	ESupportedWriteInquiryModeCommand						= 103, /*!< Write Inquiry Mode command is supported */
	ESupportedReadPageScanTypeCommand						= 104, /*!< Read Page Scan Type command is supported */
	ESupportedWritePageScanTypeCommand						= 105, /*!< Write Page Scan Type command is supported */
	ESupportedReadAFHChannelAssessmentModeCommand			= 106, /*!< Read AFH Channel Assessment Mode command is supported */
	ESupportedWriteAFHChannelAssessmentModeCommand			= 107, /*!< Write AFH Channel Assessment Mode command is supported */
	ESupportedReadLocalVersionInformationCommand			= 115, /*!< Read Local Version Information command is supported */
	ESupportedReadLocalSupportedFeaturesCommand				= 117, /*!< Read Local Supported Features command is supported */
	ESupportedReadLocalExtendedFeaturesCommand				= 118, /*!< Read Local Extended Features command is supported */
	ESupportedReadBufferSizeCommand							= 119, /*!< Read Buffer Size command is supported */
	ESupportedReadCountryCodeCommand						= 120, /*!< Read Country Code [Deprecated] command is supported */
	ESupportedReadBDADDRCommand								= 121, /*!< Read BD ADDR command is supported */
	ESupportedReadFailedContactCountCommand					= 122, /*!< Read Failed Contact Count command is supported */
	ESupportedResetFailedContactCountCommand				= 123, /*!< Reset Failed Contact Count command is supported */
	ESupportedGetLinkQualityCommand							= 124, /*!< Get Link Quality command is supported */
	ESupportedReadRSSICommand								= 125, /*!< Read RSSI command is supported */
	ESupportedReadAFHChannelMapCommand						= 126, /*!< Read AFH Channel Map command is supported */
	ESupportedReadBDClockCommand							= 127, /*!< Read BD Clock command is supported */
	ESupportedReadLoopbackModeCommand						= 128, /*!< Read Loopback Mode command is supported */
	ESupportedWriteLoopbackModeCommand						= 129, /*!< Write Loopback Mode command is supported */
	ESupportedEnableDeviceUnderTestModeCommand				= 130, /*!< Enable Device Under Test Mode command is supported */
	ESupportedSetupSynchronousConnectionCommand				= 131, /*!< Setup Synchronous Connection command is supported */
	ESupportedAcceptSynchronousConnectionCommand			= 132, /*!< Accept Synchronous Connection command is supported */
	ESupportedRejectSynchronousConnectionCommand			= 133, /*!< Reject Synchronous Connection command is supported */
	ESupportedReadExtendedInquiryResponseCommand			= 136, /*!< Read Extended Inquiry Response command is supported */
	ESupportedWriteExtendedInquiryResponseCommand			= 137, /*!< Write Extended Inquiry Response command is supported */
	ESupportedRefreshEncryptionKeyCommand					= 138, /*!< Refresh Encryption Key command is supported */
	ESupportedSniffSubratingCommand							= 140, /*!< Sniff Subrating command is supported */
	ESupportedReadSimplePairingModeCommand					= 141, /*!< Read Simple Pairing Mode command is supported */
	ESupportedWriteSimplePairingModeCommand					= 142, /*!< Write Simple Pairing Mode command is supported */
	ESupportedReadLocalOOBDataCommand						= 143, /*!< Read Local OOB Data command is supported */
	ESupportedReadInquiryResponseTransmitPowerCommand		= 144, /*!< Read Inquiry Response Transmit Power command is supported */
	ESupportedWriteInquiryTransmitPowerLevelCommand			= 145, /*!< Write Inquiry Transmit Power Level command is supported */
	ESupportedReadDefaultErroneousDataReportingCommand		= 146, /*!< Read Default Erroneous Data Reporting command is supported */
	ESupportedWriteDefaultErroneousDataReportingCommand		= 147, /*!< Write Default Erroneous Data Reporting command is supported */
	ESupportedIOCapabilityRequestReplyCommand				= 151, /*!< IO Capability Request Reply command is supported */
	ESupportedUserConfirmationRequestReplyCommand			= 152, /*!< User Confirmation Request Reply command is supported */
	ESupportedUserConfirmationRequestNegativeReplyCommand	= 153, /*!< User Confirmation Request Negative Reply command is supported */
	ESupportedUserPasskeyRequestReplyCommand				= 154, /*!< User Passkey Request Reply command is supported */
	ESupportedUserPasskeyRequestNegativeReplyCommand		= 155, /*!< User Passkey Request Negative Reply command is supported */
	ESupportedRemoteOOBDataRequestReplyCommand				= 156, /*!< Remote OOB Data Request Reply command is supported */
	ESupportedWriteSimplePairingDebugModeCommand			= 157, /*!< Write Simple Pairing Debug Mode command is supported */
	ESupportedEnhancedFlushCommand							= 158, /*!< Enhanced Flush command is supported */
	ESupportedRemoteOOBDataRequestNegativeReplyCommand		= 159, /*!< Remote OOB Data Request Negative Reply command is supported */
	ESupportedSendKeypressNotificationCommand				= 162, /*!< Send Keypress Notification command is supported */
	ESupportedIOCapabilitiesResponseNegativeReplyCommand	= 163, /*!< IO Capabilities Response Negative Reply command is supported */
	};

#endif // BTFEATURES_H
