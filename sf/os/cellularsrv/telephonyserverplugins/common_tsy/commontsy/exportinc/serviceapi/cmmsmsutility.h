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
#ifndef CMMSMSUTILITY_H
#define CMMSMSUTILITY_H

// INCLUDES
#include <etelmm.h>
#include <ctsy/serviceapi/mmtsy_defaults.h>

// CONSTANTS
/**  Type-of-number constants */
const TUint8 KTonUnknown                    = 0x00;
const TUint8 KInternationalNumber           = 0x01;
const TUint8 KNationalNumber                = 0x02;
const TUint8 KNetworkSpecificNumber         = 0x03;
const TUint8 KSubscriberNumber              = 0x04;
const TUint8 KAlphanumeric                  = 0x05;
const TUint8 KAbbreviatedNumber             = 0x06;

/**  Numbering-plan-identification */
const TUint8 KNpiUnknown = 0x00;
const TUint8 KIsdnTelephoneNumPlan = 0x01;
const TUint8 KDataNumPlan = 0x03;
const TUint8 KTelexNumPlan = 0x04;
const TUint8 KServiceCentreSpecificPlan1 = 0x05;
const TUint8 KServiceCentreSpecificPlan2 = 0x06;
const TUint8 KNationalNumPlan = 0x08;
const TUint8 KPrivateNumPlan = 0x09;
const TUint8 KErmesNumPlan = 0x0A;

/**  Masks for type of number and numbering plan identification handling */
const TUint8 KTONMask = 0x07;
const TUint8 KNPIMask = 0x0F;

/**  CB Language List Storage can contains 4 language */
const TUint8 KCbLanguageListStorageMaxSize = 0x04;

/**  CB Identifier List can contains 48 valid subject */
const TUint8 KCbmiListStorageMaxSize = 0x30;

/**  SIM or R-UIM based SMS message store */
const TUint8 KSmsStoreNumber = 1;

/**  Max number of CBMI entries */
const TUint8 KMaxNumberEntries = 48;

/**  Service center address buffer */
const TUint8 KMaxAddressBufferSize = 22;

/**  Maximum amount of digits in address data */
const TUint8 KMaxAmountOfDigits = 20;

/**  Message Status*/
const TUint8 KSimSmsNotPresent = 0x00;
const TUint8 KSimSmsMtRead = 0x01; 
const TUint8 KSimSmsMtNotRead = 0x03;
const TUint8 KSimSmsMoSent = 0x05;
const TUint8 KSimSmsMoNotSent = 0x07;
const TUint8 KSimSmsStatusReportReqNotReceived = 0x0D;
const TUint8 KSimSmsStatusReportReqReceivedNotStored = 0x15; 
const TUint8 KSimSmsStatusReportReqReceivedStored = 0x1D;

/**  SMS_GSM_PARAMETERS */
const TUint8 KDestinationAddress =   0x01;
const TUint8 KServiceCentreAddress =   0x02;
const TUint8 KProtocolID =   0x04;
const TUint8 KDataCodingScheme =   0x08;
const TUint8 KValidityPeriod =   0x10;
const TUint8 KSmsGsmParametersIndMask =   0x1F;

/**  Routing command */
const TUint8 KSmsRoutingActivated = 0x1;
const TUint8 KSmsRoutingDeactivated = 0x00;

/**  Caps */
const TUint32 KSmsGsmModeCaps = RMobileSmsMessaging::KCapsGsmSms;

const TUint32 KSmsMessageStoreInfoCaps = 
    static_cast< TUint32 >( RMobilePhoneStore::KCapsIndividualEntry 
    | RMobilePhoneStore::KCapsDeleteAll
    | RMobilePhoneStore::KCapsReadAccess
    | RMobilePhoneStore::KCapsWriteAccess
    | RMobilePhoneStore::KCapsNotifyEvent
    | RMobilePhoneStore::KCapsWholeStore );

const TUint32 KSmsStoreInfoCaps = 
    static_cast< TUint32 >( RMobilePhoneStore::KCapsWholeStore
    | RMobilePhoneStore::KCapsIndividualEntry
    | RMobilePhoneStore::KCapsReadAccess
    | RMobilePhoneStore::KCapsWriteAccess
    | RMobilePhoneStore::KCapsDeleteAll
    | RMobilePhoneStore::KCapsNotifyEvent )
    | static_cast< TUint32 > ( RMobileSmsStore::KCapsUnreadMessages
    | RMobileSmsStore::KCapsReadMessages
    | RMobileSmsStore::KCapsSentMessages
    | RMobileSmsStore::KCapsUnsentMessages );

const TUint32 KSmsControlCaps = 
    ( RMobileSmsMessaging::KCapsReceiveUnstoredClientAck |
    RMobileSmsMessaging::KCapsSendWithAck |
    RMobileSmsMessaging::KCapsGetSmspList |
    RMobileSmsMessaging::KCapsSetSmspList );

const TUint32 KUssdTypeCaps = 
    ( RMobileUssdMessaging::KCapsMOUssd | 
    RMobileUssdMessaging::KCapsMTUssd );

/**  LITERALS */
_LIT( KSmsParamStoreName,"SMSP_STORE" );

/**  Used to store CBMI & language list to the TSY */
struct TCbsCbmiAndLanglist
    {   
    /** CBMI Storage max size is 48 */
    TBuf16<KMaxNumberEntries> iCbmiStorage;

    /** CBMI Temporary storage max size is 48 */
    TBuf16<KMaxNumberEntries> iCbmiTempStorage;

    /** Language storage max size is 4 */
    TBuf16<KCbLanguageListStorageMaxSize> iLanguageStorage;

    /** Temporary language max size is 4 */
    TBuf16<KCbLanguageListStorageMaxSize> iLanguageTempStorage;
    };

/**  Used to buffer incoming SMSes to the TSY */
struct TSmsMsg
    {
    /*
     * The size of MT Undefined SMS
     */     
    TBuf8<231> iSmsMsg; 
    
    /*
     * Service Centre buffer (3GPP TS 24.011 and 3GPP TS 23.040)
     */
    TBuf<KMaxAddressBufferSize> iServiceCentre; 

    /*
     * Service Center Type-of-Number
     */

    RMobilePhone::TMobileTON iMobileScTON;  

    /*
     * Type of Service Center Numbering-Plan-Indentification
     */
    RMobilePhone::TMobileNPI iMobileScNPI;

    /*
     * MessageStatus
     */
    TUint8 iMessageStatus;
    
    /*
     * Location
     */    
    TInt iLocation;
    
    /*
     * indicator iDeleteAfterClientAck
     */    
    TBool iDeleteAfterClientAck;
    
    /*
     * iSmsClass2
     */    
	TBool iSmsClass2;
    };

struct TCbsCbmiAndLangAndFilter 
    {

    /*
     * Filter setting
     */ 
    RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter iSetting;

    /*
     * CBMI Storage max size is 48
     */ 
    TBuf16<KMaxNumberEntries> iCbmiStorage;

    /*
     * Language storage max size is 4
     */ 
    TBuf16<KCbLanguageListStorageMaxSize> iLanguageStorage;
    };

struct TSendSmsDataAndAttributes 
    {
    /*
     * Send attributes
     */ 
    RMobileSmsMessaging::TMobileSmsSendAttributesV1* iAttributes;

    /*
     * Data
     */ 
    TDes8* iMsgData;

    /*
     * IPC number
     */ 
    TInt iIpc;
    };

/** Cell broadcast message */
/** GSM */
struct TGsmCbsMsg 
    {
    /*
     * Broadcast message GSM
     */ 
    TBuf8<RMobileBroadcastMessaging::KBroadcastPageSize> iCbsMsg;
    };

struct TWcdmaCbsMsg
	{
    /*
     * Broadcast message WCDMA
     */ 	
    TBuf8<RMobileBroadcastMessaging::KBroadcastPageSize> iWcdmaCbsData;

    /*
     * Number of SubBlocks in the CBS message
     */ 		
	TUint iSbNumber;

    /*
     * Number of Pages
     */
	TInt iNumberOfPages;

    /*
     * Message Type
     */ 			
	TUint8 iMessageType;

    /*
     * Message ID
     */ 		
	TUint16 iMessageId;

    /*
     * Serial Number
     */
	TUint16 iSerialNum;
	
    /*
     * Data Coding Scheme
     */
	TUint8 iDCS;

    /*
     * Info length
     */
	TUint8 iInfoLength;
	};

/** SMS parameters */
struct TSmsParameters
    {
    /*
     * LocationNumber
     */    
    TUint8 iLocationNumber;

    /*
     * Parameter Indicator
     */    
    TUint8 iParameterIndicator;

    /*
     * ProtocolId
     */    
    TUint8 iProtocolId;

    /*
     * Data Coding Scheme
     */    
    TUint8 iDataCodingScheme;

    /*
     * Validity Period
     */    
    TUint8 iValidityPeriod;

    /*
     * Service Center Address
     */    
    TBuf<KMaxAddressBufferSize> iServiceCenterAddress;

    /*
     * MobileSc TON
     */    
    RMobilePhone::TMobileTON iMobileScTON;

    /*
     * MobileSc NPI
     */    
    RMobilePhone::TMobileNPI iMobileScNPI;

    /*
     * Destination Address
     */    
    TBuf<KMaxAddressBufferSize> iDestinationAddress;

    /*
     * Mobile De TON
     */    
    RMobilePhone::TMobileTON iMobileDeTON;

    /*
     * Mobile De NPI
     */    
    RMobilePhone::TMobileNPI iMobileDeNPI;

    /*
     * Alpha Tag Data
     */
    TBuf<RMobileSmsMessaging::KMaxSmspTextSize> iAlphaTagData;

    /*
     * Alpha Tag Present
     */    
    TBool iAlphaTagPresent;
    };

#endif  // CMMSMSUTILITY_H

// End of File


