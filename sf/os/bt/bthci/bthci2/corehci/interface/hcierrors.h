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
 @publishedAll
 @released
*/

#ifndef HCIERRORS_H
#define HCIERRORS_H

#include <e32def.h>

static const TInt KHCIErrorBase = -6000; /** Base error value for HCI error codes */

enum THCIErrorCode
/** HCI error codes taken from the Bluetooth code specification.
@see Bluetooth specification
*/
	{
	/** HCI OK error code - Used to indicate the absence of any error condition */
	EOK									= 0x00,

	/** HCI unknown opcode error code */
	EUnknownOpcode 						= 0x01, // Symbian OS code -6001

	/** HCI no connection error code */
	ENoConnection						= 0x02, // Symbian OS code -6002
	
	/** HCI hardware failure error code */
	EHardwareFail						= 0x03, // Symbian OS code -6003
	
	/** HCI page timeout error code */
	EPageTimedOut						= 0x04, // Symbian OS code -6004
	
	/** HCI authentication failure error code */
	EAuthenticationFailure 				= 0x05, // Symbian OS code -6005
	
	/** HCI key missing error code */
	EKeyMissing							= 0x06, // Symbian OS code -6006
	
	/** HCI key memory full error code */
	EKeyMemoryFull 						= 0x07, // Symbian OS code -6007
	
	/** HCI connection timeout error code */
	EConnectionTimeout 					= 0x08, // Symbian OS code -6008
	
	/** HCI maximum number of connections exceeded error code */
	EMaxConnections						= 0x09, // Symbian OS code -6009
	
	/** HCI maximum number of SCO connections exceeded error code */
	EMaxSCOConnections 					= 0x0a, // Symbian OS code -6010
	
	/** HCI maximum number of ACL connections exceeded error code */
	EMaxACLConnections 					= 0x0b, // Symbian OS code -6011
	
	/** HCI command disallowed error code */
	ECommandDisallowed 					= 0x0c, // Symbian OS code -6012
	
	/** HCI host resource rejection error code */ 
	EHostResourceRejection 				= 0x0d, // Symbian OS code -6013
	
	/** HCI host security rejection error code */
	EHostSecurityRejection 				= 0x0e, // Symbian OS code -6014
	
	/** HCI host personal device rejection error code */
	EHostPersonalDeviceRejection		= 0x0f, // Symbian OS code -6015 
	
	/** @deprecated HCI remote host timeout error code */
	ERemoteHostTimeout 					= 0x10, // Symbian OS code -6016

	/** HCI host timeout error code */
	EHostTimeout	 					= 0x10, // Symbian OS code -6016
	
	/** HCI unsupported parameter error code */
	EUnsupportedParameter				= 0x11, // Symbian OS code -6017
	
	/** HCI invalid HCI parameter error code */
	EInvalidHCIParameter				= 0x12, // Symbian OS code -6018
	
	/** HCI remote device user ended connection error code */
	ERemoteUserEndedConnection 			= 0x13, // Symbian OS code -6019
	
	/** HCI remote device low resources error code */
	ERemoteLowResources					= 0x14, // Symbian OS code -6020
	
	/** HCI remote device about to power off error code */
	ERemoteAboutToPowerOff 				= 0x15, // Symbian OS code -6021
	
	/** HCI terminated by local host error code */
	ETerminatedByLocalHost 				= 0x16, // Symbian OS code -6022
	
	/** HCI repeated attempts error code */
	ERepeatedAttempts					= 0x17, // Symbian OS code -6023
	
	/** HCI pairing not allowed error code */
	EPairingNotAllowed 					= 0x18, // Symbian OS code -6024
	
	/** HCI unknown LMP PDU error code */
	EUnknownLMPpdu 						= 0x19, // Symbian OS code -6025
	
	/** HCI unsupported remote LMP feature error code */
	EUnsupportedRemoteLMPFeature		= 0x1a, // Symbian OS code -6026
	
	/** HCI SCO offset rejected error code */
	ESCOOffsetRejected 					= 0x1b, // Symbian OS code -6027
	
	/** HCI SCO interval rejected error code */
	ESCOIntervalRejected				= 0x1c, // Symbian OS code -6028
	
	/** HCI SCO air mode rejected error code */
	ESCOAirModeRejected					= 0x1d, // Symbian OS code -6029
	
	/** HCI invalid LMP parameters error code */
	EInvalidLMPParameters				= 0x1e, // Symbian OS code -6030
	
	/** HCI unspecified error code */
	EUnspecifiedError					= 0x1f, // Symbian OS code -6031
	
	/** HCI unsupported LMP parameter value error code */
	EUnsupportedLMPParameterValue		= 0x20, // Symbian OS code -6032
	
	/** HCI role change not allowed error code */
	ERoleChangeNotAllowed				= 0x21, // Symbian OS code -6033
	
	/** HCI LMP response timeout error code */
	ELMPResponseTimeout					= 0x22, // Symbian OS code -6034
	
	/** HCI LMP error transaction collision error code */
	ELMPErrorTransactionCollision		= 0x23, // Symbian OS code -6035
	
	/** HCI LMP PDU not allowed error code */
	ELMPPDUNotAllowed					= 0x24, // Symbian OS code -6036
	
	/** HCI encryption mode not acceptable error code */
	EEncryptionModeNotAcceptable		= 0x25, // Symbian OS code -6037
	
	/** HCI unit key not used error code */
	EUnitKeyUsed						= 0x26, // Symbian OS code -6038
	
	/** HCI QOS not supported error code */
	EQOSIsNotSupported 					= 0x27, // Symbian OS code -6039
	
	/** HCI instant passed error code */
	EInstantPassed 						= 0x28, // Symbian OS code -6040
	
	/** HCI pairing with unit key not supported error code */
	EPairingWithUnitKeyNotSupported		= 0x29, // Symbian OS code -6041
											   
	/** HCI LMP transaction collision error code */
	EDifferentTransactionCollision		= 0x2a, // Symbian OS code -6042
	
	//Reserved by Bluetooth Spec ...	= 0x2b,
	
	/** HCI bad parameter error code */
	EQoSUnacceptableParameter			= 0x2c, // Symbian OS code -6044	
	
	/** HCI quality of service not accepted error code */
	EQoSRejected						= 0x2d, // Symbian OS code -6045
	
	/** HCI adaptive frequency hopping channel classification not supported error code */
	EChannelClassificationNotSupported	= 0x2e, // Symbian OS code -6046
	
	/** HCI insufficient security error code */
	EInsufficientSecurity				= 0x2f, // Symbian OS code -6047
	
	/** HCI parameter value outside required range error code */
	EParameterOutOfMandatoryRange		= 0x30, // Symbian OS code -6048
	
	//Reserved by Bluetooth Spec ...	= 0x31,
	
	/*!< HCI failure because a master/slave role switch is about to take place error code */
	ERoleSwitchPending 					= 0x32, // Symbian OS code -6050
	
	//Reserved by Bluetooth Spec ...	= 0x33,
	
	/** HCI use of reserved baseband packet slot error code */
	EReservedSlotViolation 				= 0x34, // Symbian OS code -6052
	
	/** HCI master/slave role switch failed error code */
	ERoleSwitchFailed					= 0x35, // Symbian OS code -6053

	/** Extended Inquiry Response too large error code */
	EExtendedInquiryResponseTooLarge	= 0x36, // Symbian OS code -6054

	/** Simple Pairing not supported by host error code */
	ESimplePairingNotSupportedByHost	= 0x37, // Symbian OS code -6055
	
	/** Host busy-pairing error code */
	EHostBusyPairing					= 0x38, // Symbian OS code -6056

	};

#endif // HCIERRORS_H
