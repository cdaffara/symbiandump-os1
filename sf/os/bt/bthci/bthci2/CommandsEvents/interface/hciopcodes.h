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
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef HCIOPCODES_H
#define HCIOPCODES_H

#include <e32base.h>

/** Definition of the THCIOpcode type */
typedef TUint16 THCIOpcode;

// HCI Command opcodes are built up as follows:
//
//		lower 10 bit OCF | upper 6 bit OGF
//
// The OGF distinguishes the command subgroup and the OCF 
// determines the command within the subgroup.	Here
// individual opcodes are constructed by shifting the OGF 
// up by 10 and or'ing with the command OCF.

// ************* HCI Command Opcodes ***********************
const THCIOpcode KOGFMask									=0xFC00; /*!< Opcode Group Field Mask of the top six bits of opcode */

// NOP No OPeration opcode.
const THCIOpcode KNopOpcode									=0x0000; /*!< No Operation Opcode */

// SUBGROUP 0x01 - LINK CONTROL
const THCIOpcode KLinkControlOGF							=0x0001<<10; /*!< Link Control Opcode Group Field */
const THCIOpcode KInquiryOpcode 							=0x0001|KLinkControlOGF; /*!< Inquiry Opcode */
const THCIOpcode KInquiryCancelOpcode						=0x0002|KLinkControlOGF; /*!< InquiryCancel Opcode*/
const THCIOpcode KPeriodicInquiryModeOpcode 				=0x0003|KLinkControlOGF; /*!< Periodic Inquiry Mode Opcode */
const THCIOpcode KExitPeriodicInquiryModeOpcode 			=0x0004|KLinkControlOGF; /*!< Exit Periodic Inquiry Mode Opcode */
const THCIOpcode KCreateACLConnectionOpcode 				=0x0005|KLinkControlOGF; /*!< Create ACL Connection Opcode */
const THCIOpcode KDisconnectOpcode							=0x0006|KLinkControlOGF; /*!< Disconnect Opcode */
const THCIOpcode KAddSCOConnectionOpcode					=0x0007|KLinkControlOGF; /*!< Add SCO Connection Opcode */
const THCIOpcode KCreateConnectionCancelOpcode				=0x0008|KLinkControlOGF; /*!< Create Connection Cancel Opcode */
const THCIOpcode KAcceptConnectionRequestOpcode 			=0x0009|KLinkControlOGF; /*!< Accept Connection Request Opcode */
const THCIOpcode KRejectConnectionRequestOpcode 			=0x000a|KLinkControlOGF; /*!< Reject Connection Request Opcode */
const THCIOpcode KLinkKeyRequestReplyOpcode 				=0x000b|KLinkControlOGF; /*!< Link Key Request Reply Opcode */		 
const THCIOpcode KLinkKeyRequestReplyNegativeOpcode 		=0x000c|KLinkControlOGF; /*!< Link Key Request Reply Negative Opcode */
const THCIOpcode KPINCodeRequestReplyOpcode 				=0x000d|KLinkControlOGF; /*!< PIN Request Reply Opcode */		
const THCIOpcode KPINCodeRequestReplyNegativeOpcode 		=0x000e|KLinkControlOGF; /*!< PIN Request Reply Negative Opcode */
const THCIOpcode KChangeConnectionPacketTypeOpcode			=0x000f|KLinkControlOGF; /*!< Change Connection Packet Type Opcode */
const THCIOpcode KAuthenticationRequestedOpcode 			=0x0011|KLinkControlOGF; /*!< Authentication Requested Opcode */
const THCIOpcode KSetConnectionEncryptionOpcode 			=0x0013|KLinkControlOGF; /*!< Set Connection Encryption Opcode */
const THCIOpcode KChangeLinkKeyOpcode						=0x0015|KLinkControlOGF; /*!< Change Link Key Opcode */
const THCIOpcode KMasterLinkKeyOpcode 						=0x0017|KLinkControlOGF; /*!< Master Link Key Opcode */
const THCIOpcode KRemoteNameRequestOpcode					=0x0019|KLinkControlOGF; /*!< Remote Name Request Opcode */
const THCIOpcode KRemoteNameRequestCancelOpcode				=0x001a|KLinkControlOGF; /*!< Remote Name Request Cancel Opcode */
const THCIOpcode KReadRemoteSupportedFeaturesOpcode 		=0x001b|KLinkControlOGF; /*!< Read Remote Supported Features Opcode */
const THCIOpcode KReadRemoteExtendedFeaturesOpcode 			=0x001c|KLinkControlOGF; /*!< Read Remote Extended Features Opcode */
const THCIOpcode KReadRemoteVersionInfoOpcode				=0x001d|KLinkControlOGF; /*!< Read Remote Version Info Opcode */
const THCIOpcode KReadClockOffsetOpcode 					=0x001f|KLinkControlOGF; /*!< Read Clock Offset Opcode */
const THCIOpcode KReadLMPHandleOpcode						=0x0020|KLinkControlOGF; /*!< Read LMP Handle Opcode */
const THCIOpcode KSetupSynchronousConnectionOpcode			=0x0028|KLinkControlOGF; /*!< Read Clock Offset Opcode */
const THCIOpcode KAcceptSynchronousConnectionRequestOpcode 	=0x0029|KLinkControlOGF; /*!< Accept Synchronous Connection Request Opcode */
const THCIOpcode KRejectSynchronousConnectionRequestOpcode 	=0x002A|KLinkControlOGF; /*!< Reject Synchronous Connection Request Opcode */
const THCIOpcode KIOCapabilityRequestReplyOpcode				=0x002B|KLinkControlOGF; /*!< IO Capability Request Reply Opcode */
const THCIOpcode KUserConfirmationRequestReplyOpcode			=0x002C|KLinkControlOGF; /*!< User Confirmation Request Reply Opcode */
const THCIOpcode KUserConfirmationRequestNegativeReplyOpcode	=0x002D|KLinkControlOGF; /*!< User Confirmation Request Negative Reply Opcode */
const THCIOpcode KUserPasskeyRequestReplyOpcode					=0x002E|KLinkControlOGF; /*!< User Passkey Request Reply Opcode */
const THCIOpcode KUserPasskeyRequestNegativeReplyOpcode			=0x002F|KLinkControlOGF; /*!< User Passkey Request Negative Reply Opcode */
const THCIOpcode KRemoteOOBDataRequestReplyOpcode				=0x0030|KLinkControlOGF; /*!< Remote OOB Data Request Reply Opcode */
const THCIOpcode KRemoteOOBDataRequestNegativeReplyOpcode		=0x0033|KLinkControlOGF; /*!< Remote OOB Data Request Negative Reply Opcode */
const THCIOpcode KIOCapabilityRequestNegativeReplyOpcode		=0x0034|KLinkControlOGF; /*!< IO Capability Request Negative Reply Opcode */

// SUBGROUP 0x02 - LINK POLICY
const THCIOpcode KLinkPolicyOGF 							=0x0002<<10; /*!< Link Policy Opcode Group Field */
const THCIOpcode KHoldModeOpcode							=0x0001|KLinkPolicyOGF; /*!< Hold Mode Opcode */
const THCIOpcode KSniffModeOpcode							=0x0003|KLinkPolicyOGF; /*!< Sniff Mode Opcode */
const THCIOpcode KExitSniffModeOpcode						=0x0004|KLinkPolicyOGF; /*!< Exit Sniff Mode Opcode */
const THCIOpcode KParkModeOpcode							=0x0005|KLinkPolicyOGF; /*!< Park Mode Opcode */
const THCIOpcode KExitParkModeOpcode						=0x0006|KLinkPolicyOGF; /*!< Exit Park Mode Opcode */
const THCIOpcode KQOSSetupOpcode							=0x0007|KLinkPolicyOGF; /*!< QOS Setup Opcode */
const THCIOpcode KRoleDiscoveryOpcode						=0x0009|KLinkPolicyOGF; /*!< Role Discovery Opcode */
const THCIOpcode KSwitchRoleOpcode							=0x000b|KLinkPolicyOGF; /*!< Switch Role Opcode */
const THCIOpcode KReadLinkPolicySettingsOpcode				=0x000c|KLinkPolicyOGF; /*!< Read Link Policy Settings Opcode */
const THCIOpcode KWriteLinkPolicySettingsOpcode 			=0x000d|KLinkPolicyOGF; /*!< Write Link Policy Settings Opcode */
const THCIOpcode KReadDefaultLinkPolicySettingsOpcode 		=0x000e|KLinkPolicyOGF; /*!< Read Default Link Policy Settings Opcode */
const THCIOpcode KWriteDefaultLinkPolicySettingsOpcode 		=0x000f|KLinkPolicyOGF; /*!< Write Default Link Policy Settings Opcode */
const THCIOpcode KFlowSpecificationOpcode 					=0x0010|KLinkPolicyOGF; /*!< Flow Specification Opcode */
const THCIOpcode KSniffSubratingOpcode						=0x0011|KLinkPolicyOGF; /*!< Sniff Subrating Opcode */

// SUBGROUP 0x03 - HOST CONTROLLER AND BASEBAND
const THCIOpcode KHCBasebandOGF 							=0x0003<<10; /*!< HC Baseband Opcode Group Field */
const THCIOpcode KSetEventMaskOpcode						=0x0001|KHCBasebandOGF; /*!< Set Event Mask Opcode */
const THCIOpcode KResetOpcode								=0x0003|KHCBasebandOGF; /*!< Reset Opcode */
const THCIOpcode KSetEventFilterOpcode						=0x0005|KHCBasebandOGF; /*!< Set Event Filter Opcode */
const THCIOpcode KFlushOpcode								=0x0008|KHCBasebandOGF; /*!< Flush Opcode */
const THCIOpcode KReadPINTypeOpcode 						=0x0009|KHCBasebandOGF; /*!< Read PIN Type Opcode */
const THCIOpcode KWritePINTypeOpcode						=0x000a|KHCBasebandOGF; /*!< Write PIN Type Opcode */
const THCIOpcode KCreateNewUnitKeyOpcode					=0x000b|KHCBasebandOGF; /*!< Create New Unit Key Opcode */
const THCIOpcode KReadStoredLinkKeyOpcode					=0x000d|KHCBasebandOGF; /*!< Read Stored Link Key Opcode */
const THCIOpcode KWriteStoredLinkKeyOpcode					=0x0011|KHCBasebandOGF; /*!< Write Stored Link Key Opcode */
const THCIOpcode KDeleteStoredLinkKeyOpcode 				=0x0012|KHCBasebandOGF; /*!< Delete Stored Link Key Opcode */
const THCIOpcode KWriteLocalNameOpcode	 					=0x0013|KHCBasebandOGF; /*!< Write Local Name Opcode */
const THCIOpcode KReadLocalNameOpcode						=0x0014|KHCBasebandOGF; /*!< Read Local Name Opcode */
const THCIOpcode KReadConnectionAcceptTimeoutOpcode 		=0x0015|KHCBasebandOGF; /*!< Read Connection Accept Timeout Opcode */
const THCIOpcode KWriteConnectionAcceptTimeoutOpcode		=0x0016|KHCBasebandOGF; /*!< Write Connection Accept Timeout Opcode */
const THCIOpcode KReadPageTimeoutOpcode 					=0x0017|KHCBasebandOGF; /*!< Read Page Timeout Opcode */
const THCIOpcode KWritePageTimeoutOpcode					=0x0018|KHCBasebandOGF; /*!< Write Page Timeout Opcode */
const THCIOpcode KReadScanEnableOpcode						=0x0019|KHCBasebandOGF; /*!< Read Scan Enable Opcode */
const THCIOpcode KWriteScanEnableOpcode 					=0x001a|KHCBasebandOGF; /*!< Write Scan Enable Opcode */
const THCIOpcode KReadPageScanActivityOpcode				=0x001b|KHCBasebandOGF; /*!< Read Page Scan Activity Opcode */
const THCIOpcode KWritePageScanActivityOpcode				=0x001c|KHCBasebandOGF; /*!< Write Page Scan Activity Opcode */
const THCIOpcode KReadInquiryScanActivityOpcode 			=0x001d|KHCBasebandOGF; /*!< Read Inquiry Scan Activity Opcode */
const THCIOpcode KWriteInquiryScanActivityOpcode			=0x001e|KHCBasebandOGF; /*!< Write Inquiry Scan Activity Opcode */
const THCIOpcode KReadAuthenticationEnableOpcode			=0x001f|KHCBasebandOGF; /*!< Read Authentication Enable Opcode */
const THCIOpcode KWriteAuthenticationEnableOpcode			=0x0020|KHCBasebandOGF; /*!< Write Authentication Enable Opcode */
const THCIOpcode KReadEncryptionModeOpcode					=0x0021|KHCBasebandOGF; /*!< Read Encryption Mode Opcode */
const THCIOpcode KWriteEncryptionModeOpcode 				=0x0022|KHCBasebandOGF; /*!< Write Encryption Mode Opcode */
const THCIOpcode KReadClassOfDeviceOpcode					=0x0023|KHCBasebandOGF; /*!< Read Class Of Device Opcode */
const THCIOpcode KWriteClassOfDeviceOpcode					=0x0024|KHCBasebandOGF; /*!< Write Class Of Device Opcode */
const THCIOpcode KReadVoiceSettingOpcode					=0x0025|KHCBasebandOGF; /*!< Read Voice Setting Opcode */
const THCIOpcode KWriteVoiceSettingOpcode					=0x0026|KHCBasebandOGF; /*!< Write Voice Setting Opcode */
const THCIOpcode KReadAutomaticFlushTimeoutOpcode			=0x0027|KHCBasebandOGF; /*!< Read Automatic Flush Timeout Opcode */
const THCIOpcode KWriteAutomaticFlushTimeoutOpcode			=0x0028|KHCBasebandOGF; /*!< Write Automatic Flush Timeout Opcode */
const THCIOpcode KReadNumBroadcastRetransmissionsOpcode 	=0x0029|KHCBasebandOGF; /*!< Read Num Broadcast Retransmissions Opcode */
const THCIOpcode KWriteNumBroadcastRetransmissionsOpcode	=0x002A|KHCBasebandOGF; /*!< Write Num Broadcast Retransmissions Opcode */
const THCIOpcode KReadHoldModeActivityOpcode				=0x002B|KHCBasebandOGF; /*!< Read Hold Mode Activity Opcode */
const THCIOpcode KWriteHoldModeActivityOpcode				=0x002C|KHCBasebandOGF; /*!< Write Hold Mode Activity Opcode */
const THCIOpcode KReadTransmitPowerLevelOpcode				=0x002D|KHCBasebandOGF; /*!< Read Transmit Power Level Opcode */
const THCIOpcode KReadSynchronousFlowControlEnableOpcode	=0x002E|KHCBasebandOGF; /*!< Read SCO Flow Control Enable Opcode */
const THCIOpcode KWriteSynchronousFlowControlEnableOpcode	=0x002F|KHCBasebandOGF; /*!< Write SCO Flow Control Enable Opcode */
const THCIOpcode KSetControllerToHostFlowControlOpcode 		=0x0031|KHCBasebandOGF; /*!< Set Controller To Host Flow Control Opcode */
const THCIOpcode KHostBufferSizeOpcode						=0x0033|KHCBasebandOGF; /*!< Host Buffer Size Opcode */
const THCIOpcode KHostNumberOfCompletedPacketsOpcode		=0x0035|KHCBasebandOGF; /*!< Host Number Of Completed Packets Opcode */
const THCIOpcode KReadLinkSupervisionTimeoutOpcode			=0x0036|KHCBasebandOGF; /*!< Read Link Supervision Timeout Opcode */
const THCIOpcode KWriteLinkSupervisionTimeoutOpcode 		=0x0037|KHCBasebandOGF; /*!< Write Link Supervision Timeout Opcode */
const THCIOpcode KReadNumberOfSupportedIACOpcode			=0x0038|KHCBasebandOGF; /*!< Read Number Of Supported Inquiry Access Code Opcode */
const THCIOpcode KReadCurrentIACLAPOpcode					=0x0039|KHCBasebandOGF; /*!< Read Current Inquiry Access Code Lower Address Part Opcode */
const THCIOpcode KWriteCurrentIACLAPOpcode					=0x003A|KHCBasebandOGF; /*!< Write Current Inquiry Access Code Lower Address Part Opcode */
const THCIOpcode KReadPageScanPeriodOpcode					=0x003B|KHCBasebandOGF; /*!< Read Page Scan Period Opcode */
const THCIOpcode KWritePageScanPeriodOpcode 				=0x003C|KHCBasebandOGF; /*!< Write Page Scan Period Opcode */
const THCIOpcode KReadPageScanOpcode						=0x003D|KHCBasebandOGF; /*!< Read Page Scan Opcode */
const THCIOpcode KWritePageScanOpcode						=0x003E|KHCBasebandOGF; /*!< Write Page Scan Opcode */
const THCIOpcode KSetAFHHostChannelClassificationOpcode 	=0x003F|KHCBasebandOGF; /*!< AFH Host Channel Classification Opcode */
const THCIOpcode KReadInquiryScanTypeOpcode					=0x0042|KHCBasebandOGF; /*!< Read Inquiry Scan Type Opcode */
const THCIOpcode KWriteInquiryScanTypeOpcode				=0x0043|KHCBasebandOGF; /*!< Write Inquiry Scan Type Opcode */
const THCIOpcode KReadInquiryModeOpcode						=0x0044|KHCBasebandOGF; /*!< Read Inquiry Mode Opcode */
const THCIOpcode KWriteInquiryModeOpcode					=0x0045|KHCBasebandOGF; /*!< Write Inquiry Mode Opcode */
const THCIOpcode KReadPageScanTypeOpcode					=0x0046|KHCBasebandOGF; /*!< Read Page Scan Type Opcode */
const THCIOpcode KWritePageScanTypeOpcode					=0x0047|KHCBasebandOGF; /*!< Read Page Scan Type Opcode */
const THCIOpcode KReadAFHChannelAssessmentModeOpcode		=0x0048|KHCBasebandOGF; /*!< Read AFH Channel Assessment Mode Scan Opcode */
const THCIOpcode KWriteAFHChannelAssessmentModeOpcode		=0x0049|KHCBasebandOGF; /*!< Write AFH Channel Assessment Mode Scan Opcode */
const THCIOpcode KReadExtendedInquiryResponseOpcode			=0x0051|KHCBasebandOGF; /*!< Read Extended Inquiry Response Opcode */
const THCIOpcode KWriteExtendedInquiryResponseOpcode		=0x0052|KHCBasebandOGF; /*!< Write Extended Inquiry Response Opcode */
const THCIOpcode KRefreshEncryptionKeyOpcode				=0x0053|KHCBasebandOGF; /*!< Refresh Encryption Key Opcode */
const THCIOpcode KReadSimplePairingModeOpcode				=0x0055|KHCBasebandOGF; /*!< Read Simple Pairing Mode Opcode */
const THCIOpcode KWriteSimplePairingModeOpcode				=0x0056|KHCBasebandOGF; /*!< Write Simple Pairing Mode Opcode */
const THCIOpcode KReadLocalOOBDataOpcode					=0x0057|KHCBasebandOGF; /*!< Read Local OOB Data Opcode */
const THCIOpcode KReadInquiryResponseTransmitPowerLevelOpcode	=0x0058|KHCBasebandOGF; /*!< Read Inquiry Response Transmit Power Level Opcode */
const THCIOpcode KWriteInquiryResponseTransmitPowerLevelOpcode	=0x0059|KHCBasebandOGF; /*!< Write Inquiry Response Transmit Power Level Opcode */
const THCIOpcode KSendKeypressNotificationOpcode			=0x0060|KHCBasebandOGF; /*!< Send Keypress Notification Opcode */
const THCIOpcode KReadDefaultErroneousDataReportingOpcode	=0x005A|KHCBasebandOGF; /*!< Read Default Erroneous Data Reporting Opcode */
const THCIOpcode KWriteDefaultErroneousDataReportingOpcode	=0x005B|KHCBasebandOGF; /*!< Write Default Erroneous Data Reporting Opcode */
const THCIOpcode KEnhancedFlushOpcode						=0x005F|KHCBasebandOGF; /*!< Enhanced Flush Opcode */

// SUBGROUP 0x04 - INFORMATIONAL
const THCIOpcode KInfoOGF									=0x0004<<10; /*!< Info Opcode Group Field */
const THCIOpcode KReadLocalVersionInfoOpcode				=0x0001|KInfoOGF; /*!< Read Local Version Info Opcode */
const THCIOpcode KReadLocalSupportedCommandsOpcode			=0x0002|KInfoOGF; /*!< Read Local Supported Commands Opcode */
const THCIOpcode KReadLocalSupportedFeaturesOpcode			=0x0003|KInfoOGF; /*!< Read Local Supported Features Opcode */
const THCIOpcode KReadLocalExtendedFeaturesOpcode			=0x0004|KInfoOGF; /*!< Read Local Extended Features Opcode */
const THCIOpcode KReadBufferSizeOpcode						=0x0005|KInfoOGF; /*!< Read Buffer Size Opcode */
const THCIOpcode KReadCountryCodeOpcode 					=0x0007|KInfoOGF; /*!< Read Country Code Opcode */
const THCIOpcode KReadBdaddrOpcode							=0x0009|KInfoOGF; /*!< Read Bdaddr Opcode */

// SUBGROUP 0x05 - STATUS
const THCIOpcode KStatusOGF 								=0x0005<<10; /*!< Status Opcode Group Field */
const THCIOpcode KReadFailedContactCounterOpcode			=0x0001|KStatusOGF; /*!< Read Failed Contact Counter Opcode */
const THCIOpcode KResetFailedContactCounterOpcode			=0x0002|KStatusOGF; /*!< Reset Failed Contact Counter Opcode */
const THCIOpcode KReadLinkQualityOpcode						=0x0003|KStatusOGF; /*!< Read Link Quality Opcode */
const THCIOpcode KReadRSSIOpcode							=0x0005|KStatusOGF; /*!< Read RSSI Opcode */				  
const THCIOpcode KReadAFHChannelMapOpcode					=0x0006|KStatusOGF; /*!< Read AFH Channel Map Opcode */
const THCIOpcode KReadClockOpcode							=0x0007|KStatusOGF; /*!< Read Clock Opcode */ 

// SUBGROUP 0x06 - TESTING
const THCIOpcode KTestingOGF								=0x0006<<10; /*!< Testing Opcode Group Field */
const THCIOpcode KReadLoopbackModeOpcode					=0x0001|KTestingOGF; /*!< Read Loopback Mode Opcode */
const THCIOpcode KWriteLoopbackModeOpcode					=0x0002|KTestingOGF; /*!< Write Loopback Mode Opcode */
const THCIOpcode KEnableDeviceUnderTestModeOpcode			=0x0003|KTestingOGF; /*!< Enable Device Under Test Mode Opcode */
const THCIOpcode KWriteSimplePairingDebugModeOpcode			=0x0004|KTestingOGF; /*!< Write Simple Pairing Debug Mode Opcode */

// SUBGROUP 0x3F - Vendor Specific Commands
const THCIOpcode KVendorDebugOGF							=0x003f<<10; /*!< VendorDebug Opcode Group Field */
const THCIOpcode KSomeVendorSpecificCommand 				=0x0001|KVendorDebugOGF; /*!< Deprecated: an example of a Vendor Specific Command. Usage of this definition is deprecated */

/** @deprected No longer required as HCI v1 no longer present **/
const THCIOpcode KChangeLocalNameOpcode = KWriteLocalNameOpcode;
const THCIOpcode KSetHostControllerToHostFlowOpcode = KSetControllerToHostFlowControlOpcode;
const THCIOpcode KAcceptSynchronousConnectionOpcode = KAcceptSynchronousConnectionRequestOpcode;
const THCIOpcode KReadSCOFlowControlEnableOpcode = KReadSynchronousFlowControlEnableOpcode;
const THCIOpcode KChangeMasterLinkKeyOpcode = KMasterLinkKeyOpcode;
const THCIOpcode KWriteSCOFlowControlEnableOpcode = KWriteSynchronousFlowControlEnableOpcode;
const THCIOpcode KGetLinkQualityOpcode = KReadLinkQualityOpcode;
const THCIOpcode KRejectSynchronousConnectionOpcode = KRejectSynchronousConnectionRequestOpcode;

#endif // HCIOPCODES_H 
