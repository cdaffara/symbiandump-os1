/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/




/**
 @file
 @publishedPartner
 @released
*/
#ifndef _MmTsy_Defaults_H_
#define _MmTsy_Defaults_H_

//  INCLUDES
#include <et_phone.h>
#include <etelmm.h>
#include <etelpckt.h>

//PHONE FACTORY related definitions
// CONSTANTS
/** Defines network type */
const RTelServer::TNetworkType KMmTsyNetworkType 
    = RTelServer::ENetworkTypeMobileDigital; 

//PHONE related definitions
/** Phone core capabilities */
const TUint KPhoneCoreCaps 
    = RPhone::KCapsVoice 
    | RPhone::KCapsData
    | RPhone::KCapsEventModemDetection
    | RPhone::KCapsStealCommPort;

//VOICE call related definitions
/** This is always the Emergency call transaction id. Do not change this. */
const TUint8 KEmergencyNumberTransactionId = 0x00;
/** Maximum Call ID value */
const TInt KMaxCallIdValue = 0x6F;

/** Maximum number of active calls */
const TUint8 KMaxNumberOfCalls = 7;

//DTMF related definitions
/** Maximum length of DTMF string (defined in call server specification) */
const TInt KDtmfStringMaxLength = 123;

//SS related definitions
/** Maximum length of call barring password */
const TInt KMmTsyMaxLengthOfSSPassword = 4;

// GSM SPECIFIC!!
/** Initial call service capabilities */
const TUint32 KMultimodeTsyGsmCallServiceCaps 
        = RMobilePhone::KCapsGetCFStatusNetwork
        | RMobilePhone::KCapsSetCFStatus
        | RMobilePhone::KCapsNotifyCFStatus
        | RMobilePhone::KCapsGetClipStatus
        | RMobilePhone::KCapsGetClirStatus
        | RMobilePhone::KCapsGetColpStatus
        | RMobilePhone::KCapsGetColrStatus
        | RMobilePhone::KCapsGetCnapStatus
        | RMobilePhone::KCapsGetCBStatusNetwork
        | RMobilePhone::KCapsSetCBStatus
        | RMobilePhone::KCapsNotifyCBStatus
        | RMobilePhone::KCapsChangeCBPassword
        | RMobilePhone::KCapsBarAllIncoming
        | RMobilePhone::KCapsBarIncomingRoaming
        | RMobilePhone::KCapsBarAllOutgoing
        | RMobilePhone::KCapsBarOutgoingInternational
        | RMobilePhone::KCapsBarOutgoingInternationalExHC
        | RMobilePhone::KCapsBarAllCases
        | RMobilePhone::KCapsGetCWStatusNetwork
        | RMobilePhone::KCapsSetCWStatus
        | RMobilePhone::KCapsNotifyCWStatus
        | RMobilePhone::KCapsNetworkServiceRequest;

//NET related definitions
/** NITZ Capabilities */
const TInt32 KMmTsyNITZCaps 
        = RMobilePhone::KCapsTimeAvailable 
        | RMobilePhone::KCapsTimezoneAvailable
        | RMobilePhone::KCapsDSTAvailable;

/** Signal Capabilities */
const TInt32 KMmTsySignalCaps 
        = RMobilePhone::KCapsGetSignalStrength
        | RMobilePhone::KCapsNotifySignalStrengthChange;

//SECURITY related definitions
/** Data buffer length of code changing msg */
const TInt KMmTsyMaxLengthOfSecChangeData = 13;
/** Data buffer length of sec state setting and query msg */
const TInt KMmTsyMaxLengthOfSecStateData = 24;
/** Data buffer length of code cancelation msg */
const TInt KMmTsyMaxLengthOfSecCancelData = 2;
/** Maximum length of PIN and PIN2 codes */
const TInt KMmTsyMaxPinLength = 8;
/** Minimum length of PIN and PIN2 codes */
const TInt KMmTsyMinPinLength = 4;
/** Length of PUK and PUK2 codes */
const TInt KMmTsyPukLength = 8;
/** Maximum length of security code */
const TInt KMmTsyMaxSecCodeLength = 10;
/** Minimum length of security code */
const TInt KMmTsyMinSecCodeLength = 5;

/** Initial security caps */
const TUint32 KMultimodeTsyGsmSecurityInitCaps 
        = RMobilePhone::KCapsLockPhone
        | RMobilePhone::KCapsLockICC
        | RMobilePhone::KCapsLockPhoneToICC
        | RMobilePhone::KCapsAccessPin1
        | RMobilePhone::KCapsAccessPin2
        | RMobilePhone::KCapsAccessPhonePassword;


// SIM & STORAGE related definitions
/** ADN Phonebook Caps */
const TUint32 KPBTypeADNCaps 
        = RMobilePhoneStore::KCapsReadAccess
        | RMobilePhoneStore::KCapsWriteAccess
        | RMobilePhoneStore::KCapsDeleteAll
        | RMobilePhoneStore::KCapsNotifyEvent;

/** VMBX Phonebook Caps */
const TUint32 KPBTypeVMBXCaps 
        = RMobilePhoneStore::KCapsReadAccess
        | RMobilePhoneStore::KCapsWriteAccess
        | RMobilePhoneStore::KCapsDeleteAll
        | RMobilePhoneStore::KCapsNotifyEvent;

/** FDN Phonebook Caps */
const TUint32 KPBTypeFDNCaps 
        = RMobilePhoneStore::KCapsReadAccess
        | RMobilePhoneStore::KCapsWriteAccess
        | RMobilePhoneStore::KCapsDeleteAll
        | RMobilePhoneStore::KCapsNotifyEvent
        | RMobilePhoneBookStore::KCapsRestrictedWriteAccess;

/** SDN Phonebook Caps */
const TUint32 KPBTypeSDNCaps 
        = RMobilePhoneStore::KCapsReadAccess;

/** BDN Phonebook Caps */
const TUint32 KPBTypeBDNCaps = 0;

/** MBDN Phonebook Caps */
const TUint32 KPBTypeMBDNCaps 
		= RMobilePhoneStore::KCapsReadAccess
        | RMobilePhoneStore::KCapsWriteAccess;

/** ONStore caps */
const TUint32 KONStoreCaps 
        = ( TUint32 )( RMobilePhoneStore::KCapsIndividualEntry
        | RMobilePhoneStore::KCapsReadAccess
        | RMobilePhoneStore::KCapsWriteAccess
        | RMobilePhoneStore::KCapsDeleteAll
        | RMobilePhoneStore::KCapsNotifyEvent
        | RMobilePhoneStore::KCapsWholeStore );

/** ENStore caps */
const TUint32 KENStoreCaps 
        = ( TUint32 )( RMobilePhoneStore::KCapsReadAccess );


//BATTERY related definitions
// Battery voltage limits. These values are used in CMmPhoneTsy::CompleteBatteryInfo().

/** Minimum voltage for batteries used with phone. Value is used in CMmPhoneTsy::CompleteBatteryInfo() */
const TUint16 KBatteryMinVoltage = 0x0CE4; // 3300mV

/** Maximum voltage for batteries used with phone. Value is used in CMmPhoneTsy::CompleteBatteryInfo() */
const TUint16 KBatteryMaxVoltage = 0x1068; // 4200mV

/** Maximum voltage for Lithium phone batteries. Value is used in CMmPhoneTsy::CompleteBatteryInfo() */
const TUint16 KBatteryType4v1LithiumMaxVoltage = 0x1004; // 4100mV

/** Supported battery information capabilities. The supported 
* capabilities are returned to the client when it requests them by calling
* CMmPhoneTsy::GetBatteryCaps() 
*/
const TUint32 KBatteryCaps 
        = RMobilePhone::KCapsGetBatteryInfo 
        | RMobilePhone::KCapsNotifyBatteryInfoChange;

//INFO related definitions
// Misc (Em and Info) related constants
const TUint8 KSerialNumberLength = 0x0F;
const TUint8 KManufacturerDataLength = 0x02;
const TUint8 KProductCodeLength = 0x08;

/** Supported identity information capabilities of the phone.
* The supported capabilities are returned to the client when it requests
* them by using the CMmPhoneTsy::GetIdentityCaps() method.
*/
const TUint32 KIdentityCaps 
        = RMobilePhone::KCapsGetManufacturer 
        | RMobilePhone::KCapsGetModel
        | RMobilePhone::KCapsGetRevision
        | RMobilePhone::KCapsGetSerialNumber
        | RMobilePhone::KCapsGetSubscriberId;

// MMPACKET constants
/** Maximum number of simultaneous contexts supported by the phone */
const TInt KMmMaxNumberOfContexts = 11;

/** Maximum Mobile Station Class supported by the phone */
const RPacketService::TMSClass KMaxMsClass =
    RPacketService::EMSClassDualMode; // ( MS Class A )

/** Max total length of context authentication information (username + password) */
const TInt KMaxAuthenticationInfoLength = 50;

/** Initial packet service static capabilities */
const TUint KMmPacketServiceStaticCaps
        = RPacketService::KCapsSuspendSupported
        //| RPacketService::KCapsAASupported
        | RPacketService::KCapsNetworkAvailabilitySupported
        | RPacketService::KCapsSetDefaultContextSupported
        | RPacketService::KCapsChangeAttachModeSupported
        | RPacketService::KCapsGetDataTransferredSupported
        //| RPacketService::KCapsNotifyDataTransferredSupported
        | RPacketService::KCapsPreferredBearerSupported
        | RPacketService::KCapsPdpDataCompSupported
        | RPacketService::KCapsPdpHeaderCompSupported
        | RPacketService::KCapsMSClassSupported
        | RPacketService::KCapsNotifyMSClassSupported;



//USSD related definitions
const TInt KMaxLengthOfUssdMessage = 161;


//SMS related definitions
/** Maximum size of SMS location buffers */
const TUint8 KMaxSimSmsLocations = 255;

//TF Log message buffer size for phonet messages
#define KLogSizeOfBuffer 130

#endif      // _MmTsy_Defaults_H_   
            
// End of File
