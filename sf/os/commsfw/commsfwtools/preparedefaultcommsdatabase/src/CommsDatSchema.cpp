// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Comms Data Schema Definitions (version 1.1)
// Definitions of Symbian OS Comms Database Schema for Version 1.1.
// This gives details of the value and value types in each field.
// This information is used to validate data entered into the database.
// The data schema is version controlled.  New versions may be introduced from time to time.
// Then older versions are marked as deprecated and given a planned removal date.
// This version (1.1) was introduced with Release 9.1
// 
//

/**
 @file
*/



// Internal headers
#include <e32def.h>
#include <metadatabase.h>
#include <commsdattypeinfov1_1.h>
#include <commsdattypesv1_1.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_internal.h>
#include <commsdat_partner.h>
#endif

#include "Commsdat_Log.h"
#include "CommsDatInternalDefs.h"

#include <comms-infras/commsdatschema.h>



using namespace CommsDat;


/********************************************************************************************
CONTENTS OF THIS FILE

  1/ Schema of field info for each table
		Linking field name, field value type and constraints for the value

  2/ Utilities to validate meaning of element ids

  3/ Type Validation Functions
		Lookup for type by id and
		Function to check container type matches data type
*/




/*********************************************************************************************
1/ Schema of type info for each field in each table
*****************************************************

See MetaDatabase.h for this definition:

enum TFieldTypeAttributes
**
To express type information for all MetaDatabase fields
@publishedAll
@released
**
	{
	ENoAttrs,
	ENotNull,
	};

typedef struct
**
To express type information for all MetaDatabase fields
@publishedAll
@released
**
	{
		const TMDBElementId	        iTypeId;
		const TInt                  iValType;
	    const TFieldTypeAttributes	iTypeAttr;
        const TText * const         iTypeName;

	} SRecordTypeInfo;


*/

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	const SRecordTypeInfo CommsDatSchemaV1_1::iParamsRecordBaseInfo[] = {
    	{ KCDTIdParamsSTypeId,		EInt,	ENotNull,     KCDTypeNameParamsSTypeId },
		{ 0, 0, ENoAttrs, KCDNull }
	};
#endif

	const SRecordTypeInfo CommsDatSchemaV1_1::iServiceRecordBaseInfo[] = {
    	// CCDRecordBase class
    	{ KCDTIdRecordTag,	            EInt,   ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	            EText,	ENotNull,   KCDTypeNameRecordName },
		// this class
        { KCDTIdServiceEnableLlmnr,     EBool,  ENoAttrs,   KCDTypeNameServiceEnableLlmnr },
		{ 0, 0, ENoAttrs, KCDNull }
	};


	const SRecordTypeInfo CommsDatSchemaV1_1::iBearerRecordBaseInfo[] = {
    	// CCDRecordBase class
    	{ KCDTIdRecordTag,	    EInt,	ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	    EText,	ENotNull,   KCDTypeNameRecordName },
		// this class
        {KCDTIdBearerAgent,     EText,	ENotNull,     KCDTypeNameBearerAgent },
		{ 0, 0, ENoAttrs, KCDNull }
	};


    // 02/  Connection Prefs Record
    const SRecordTypeInfo CommsDatSchemaV1_1::iConnectionPrefsRecordInfo[] =
    {
    	// CCDRecordBase class
    	{ KCDTIdRecordTag,	EInt,		        ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	EText,		        ENotNull,   KCDTypeNameRecordName },
		// this class
		{ KCDTIdRanking,	EUint32,	        ENoAttrs,   KCDTypeNameRanking },
		{ KCDTIdDirection,	EInt,		        ENoAttrs,   KCDTypeNameDirection },
		{ KCDTIdBearerSet,	EUint32,	        ENoAttrs,   KCDTypeNameBearerSet },
		{ KCDTIdDialogPref, EUint32,	        ENoAttrs,   KCDTypeNameDialogPref },
		{ KCDTIdIAPLink, 	ELinkIAPRecord,  	ENoAttrs,   KCDTypeNameIAP },
	     { 0, 0, ENoAttrs, KCDNull }
    };


    // 03  Global Settings
	const SRecordTypeInfo CommsDatSchemaV1_1::iGlobalSettingsRecordInfo[] =
    {
    	// CCDRecordBase class
    	{ KCDTIdRecordTag,	EInt,		        ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	EText,		        ENotNull,   KCDTypeNameRecordName },
		// this class
		{ KCDTIdGlobalWapAcessPoint,   		ELinkWAPAPRecord,       ENoAttrs,   KCDTypeNameWAPAccessPoint },
		{ KCDTIdMaxConnectionAttempts,	    EInt,	                ENoAttrs,   KCDTypeNameMaxConnectionAttempts },
		{ KCDTIdMaxRedialAttempts,		    EInt,	                ENoAttrs,   KCDTypeNameMaxRedialAttempts },
		{ KCDTIdSMSBearer,			        EInt,	                ENoAttrs,   KCDTypeNameSMSBearer },
		{ KCDTIdSMSReceiveMode,		        EInt,	                ENoAttrs,   KCDTypeNameSMSReceiveMode },
		{ KCDTIdGPRSAttachMode,	        	EInt,	                ENoAttrs,   KCDTypeNameGPRSAttachMode },
		{ KCDTIdAcceptIncomingGPRS,	        EInt,	                ENoAttrs,   KCDTypeNameAcceptIncomingGPRS },
		{ KCDTIdGPRSClassCBearer,	        EInt,	                ENoAttrs,   KCDTypeNameGPRSClassCBearer },
		{ KCDTIdModemDataFax,	        	ELinkModemBearerRecord,	ENoAttrs,   KCDTypeNameModemDataFax },
		{ KCDTIdModemPhoneServicesSMS,	    ELinkModemBearerRecord,	ENoAttrs,   KCDTypeNameModemPhoneServicesSMS },
		{ KCDTIdLocationDataFax,	        ELinkLocationRecord,	ENoAttrs,   KCDTypeNameLocationDataFax },
		{ KCDTIdLocationPhoneServicesSMS,	ELinkLocationRecord,	ENoAttrs,   KCDTypeNameLocationPhoneServicesSMS },
		{ KCDTIdMaxMBufHeap,	            EInt,	                ENoAttrs,   KCDTypeNameMaxMBufHeap },
		{ KCDTIdDefaultAgent,	 	        ELinkAgentLookupRecord,	ENoAttrs,   KCDTypeNameDefaultAgent },
		{ KCDTIdDefaultNetwork,	     	    ELinkNetworkRecord,	    ENoAttrs,   KCDTypeNameDefaultNetwork },
		{ KCDTIdBearerAvailabilityCheckTSY, EText,                  ENoAttrs,   KCDTypeNameBearerAvailabilityCheckTSY },
		{ KCDTIdDefaultSnap,				ELinkAPRecord,	        ENoAttrs,   KCDTypeNameDefaultSnap },
 		//Name is env dependendent... :((
		{ KCDTIdPromptForSnap,				EBool,	                ENoAttrs,   KCDTypeNamePromptForSnap },
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		{ KCDTIdDefaultTier,				ELinkTierRecord,	    ENoAttrs,   KCDTypeNameDefaultTier },
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		{ KCDTIdRegulatoryDomain,			EInt,					ENoAttrs,   KCDTypeNameRegulatoryDomain },
		{ KCDTIdWmmEnabled,					EBool,	ENoAttrs,   KCDTypeNameWmmEnabled },
		{ KCDTIdEnableCountryCode,			EBool,	ENoAttrs,   KCDTypeNameEnableCountryCode },
		{ KCDTIdDefaultRegulatoryDomain,	EInt,	ENoAttrs,   KCDTypeNameDefaultRegulatoryDomain },
        { 0, 0, ENoAttrs, KCDNull }
    };


    // 04/  Network Record
    const SRecordTypeInfo CommsDatSchemaV1_1::iNetworkRecordInfo[] =
    {
    	// CCDRecordBase class
    	{ KCDTIdRecordTag,	EInt,		        ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	EText,		        ENotNull,   KCDTypeNameRecordName },
		// this class
		{ KCDTIdHostName,	EText,	ENoAttrs,   KCDTypeNameHostName },
	    { 0, 0, ENoAttrs, KCDNull }
    };


    // 05/  Location record
    const SRecordTypeInfo CommsDatSchemaV1_1::iLocationRecordInfo[] =
    {
    	// CCDRecordBase class
		{ KCDTIdRecordTag,	            EInt,		ENoAttrs,   KCDTypeNameRecordTag },
	    { KCDTIdRecordName,	            EText,		ENotNull,   KCDTypeNameRecordName },
		// this class
		{ KCDTIdIntlPrefixCode, 		EText,		ENoAttrs,   KCDTypeNameIntlPrefixCode },
	    { KCDTIdNatPrefixCode, 			EText,		ENoAttrs,   KCDTypeNameNatPrefixCode },
    	{ KCDTIdNatCode, 				EText,		ENoAttrs,   KCDTypeNameNatCode },
    	{ KCDTIdAreaCode, 				EText,		ENoAttrs,   KCDTypeNameAreaCode },
    	{ KCDTIdDialOutCode, 			EText,		ENoAttrs,   KCDTypeNameDialOutCode },
    	{ KCDTIdDisableCallWaitingCode, EText,		ENoAttrs,   KCDTypeNameDisableCallWaitingCode },
    	{ KCDTIdMobile, 				EBool,		ENotNull,   KCDTypeNameMobile },
    	{ KCDTIdUsePulseDial, 			EBool,		ENotNull,   KCDTypeNameUsePulseDial },
    	{ KCDTIdWaitForDialTone, 		EBool,		ENotNull,   KCDTypeNameWaitForDialTone },
    	{ KCDTIdPauseAfterDialOut, 		EUint32,	ENotNull,   KCDTypeNamePauseAfterDialOut },
        { 0, 0, ENoAttrs, KCDNull }
    };


    // 06/  IAP Record
	const SRecordTypeInfo CommsDatSchemaV1_1::iIAPRecordInfo[] =
    {
    	// CCDRecordBase class
    	{ KCDTIdRecordTag,	EInt,		        ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	EText,		        ENotNull,   KCDTypeNameRecordName },
		// this class
        { KCDTIdIAPServiceType, 		EText,		        ENotNull,   KCDTypeNameServiceType },
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    	{ KCDTIdIAPService, 			ELink,		        ENotNull,   KCDTypeNameService },
#else
        { KCDTIdIAPService, 			EInt,		        ENotNull,   KCDTypeNameService },
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    	{ KCDTIdIAPBearerType, 			EText,		        ENotNull,   KCDTypeNameBearerType },
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    	{ KCDTIdIAPBearer, 				ELink,		        ENotNull,   KCDTypeNameBearer },
#else
        { KCDTIdIAPBearer, 				EInt,		        ENotNull,   KCDTypeNameBearer },
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    	{ KCDTIdIAPNetwork, 			ELinkNetworkRecord,	ENotNull,   KCDTypeNameIAPNetwork },
    	{ KCDTIdIAPNetworkWeighting,	EUint32,	        ENotNull,   KCDTypeNameNetworkWeighting },
    	{ KCDTIdIAPLocation, 			ELinkLocationRecord,ENotNull,   KCDTypeNameLocation },
#ifdef SYMBIAN_NETWORKING_UMTSR5
     	{ KCDTIdIAPAppSid,	            EUint32,	        ENotNull,   KCDTypeNameAppSid },
#endif
	    { 0, 0, ENoAttrs, KCDNull }
    };


  	const SRecordTypeInfo CommsDatSchemaV1_1::iWAPAccessPointRecordInfo[] =
  	{
    	// CCDRecordBase class
		{ KCDTIdRecordTag,	        EInt,		ENoAttrs,   KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     	EText,		ENotNull,   KCDTypeNameRecordName },
		// this class
		{ KCDTIdWAPCurrentBearer,	EText,		ENotNull,   KCDTypeNameWAPCurrentBearer },
		{ KCDTIdWAPStartPage,	    ELongText,  ENoAttrs,   KCDTypeNameWAPStartPage },
  		{ 0, 0, ENoAttrs, KCDNull }
  	};

  	const SRecordTypeInfo CommsDatSchemaV1_1::iDialOutISPRecordInfo[] =
  	{
    	// CCDRecordBase class
		{ KCDTIdRecordTag,	        	EInt,		ENoAttrs,    KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     		EText,		ENotNull,    KCDTypeNameRecordName },
		// this class
		{ KCDTIdDescription,	 		EText,		ENoAttrs,    KCDTypeNameDescription },
		{ KCDTIdType,	 				EInt,		ENoAttrs,    KCDTypeNameType },
		{ KCDTIdDefaultTelNum,	 		EText,		ENoAttrs,    KCDTypeNameDefaultTelNum },
		{ KCDTIdDialResolution,	 		EBool,		ENotNull,    KCDTypeNameDialResolution },
		{ KCDTIdChargecard,	 			ELinkChargecardRecord,		ENoAttrs,    KCDTypeNameChargecard },
		{ KCDTIdUseLoginScript,	 		EBool,		ENotNull,    KCDTypeNameUseLoginScript },
		{ KCDTIdLoginScript,	 		ELongText,  ENoAttrs,    KCDTypeNameLoginScript },
		{ KCDTIdPromptForLogin,	 		EBool,		ENotNull,    KCDTypeNamePromptForLogin },
		{ KCDTIdLoginName,	 			EMedText,		ENoAttrs,    KCDTypeNameLoginName },
		{ KCDTIdLoginPass,	       		EMedText,		ENoAttrs,    KCDTypeNameLoginPass },
		{ KCDTIdDisplayPct,	        	EBool,		ENoAttrs,    KCDTypeNameDisplayPct },
		{ KCDTIdIfParams,	 			EText,		ENoAttrs,    KCDTypeNameIfParams },
		{ KCDTIdIfNetworks,	 			EText,		ENoAttrs,    KCDTypeNameIfNetworks },
		{ KCDTIdIfPromptForAuth,		EBool,		ENotNull,    KCDTypeNameIfPromptForAuth },
		{ KCDTIdIfAuthName,	 			EMedText,		ENoAttrs,    KCDTypeNameIfAuthName },
		{ KCDTIdIfAuthPass,	 			EMedText,		ENoAttrs,    KCDTypeNameIfAuthPass },
		{ KCDTIdIfAuthRetries,	 		EInt,		ENoAttrs,    KCDTypeNameIfAuthRetries },
		{ KCDTIdIfCallbackEnabled,		EBool,		ENoAttrs,    KCDTypeNameIfCallbackEnabled },
		{ KCDTIdIfCallbackType,	 		EInt,		ENoAttrs,    KCDTypeNameIfCallbackType },
		{ KCDTIdIfCallbackInfo,	 		EDesC8,		ENoAttrs,    KCDTypeNameIfCallbackInfo },
		{ KCDTIdCallbackTimeout,		EInt,		ENoAttrs,    KCDTypeNameCallbackTimeout },
		{ KCDTIdIfServerMode,	 		EBool,		ENoAttrs,    KCDTypeNameIfServerMode },
		{ KCDTIdIpAddrFromServer,		EBool,		ENotNull,    KCDTypeNameIpAddrFromServer },
		{ KCDTIdIpAddr,	 				EText,		ENoAttrs,    KCDTypeNameIpAddr },
		{ KCDTIdIpNetMask,	 			EText,		ENoAttrs,    KCDTypeNameIpNetMask },
		{ KCDTIdIpGateway,	 			EText,		ENoAttrs,    KCDTypeNameIpGateway },
		{ KCDTIdIpDNSAddrFromServer,	EBool,		ENotNull,    KCDTypeNameIpDNSAddrFromServer },
		{ KCDTIdIpNameServer1,	 		EText,		ENoAttrs,    KCDTypeNameIpNameServer1 },
		{ KCDTIdIpNameServer2,	 		EText,		ENoAttrs,    KCDTypeNameIpNameServer2 },
		{ KCDTIdIp6DNSAddrFromServer,	EBool,		ENoAttrs,    KCDTypeNameIp6DNSAddrFromServer },
		{ KCDTIdIp6NameServer1,	 		EText,		ENoAttrs,    KCDTypeNameIp6NameServer1 },
		{ KCDTIdIp6NameServer2,	 		EText,		ENoAttrs,    KCDTypeNameIp6NameServer2 },
		{ KCDTIdIpAddrLeaseValidFrom,	EText,		ENoAttrs,    KCDTypeNameIpAddrLeaseValidFrom },
		{ KCDTIdIpAddrLeaseValidTo,	 	EText,		ENoAttrs,    KCDTypeNameIpAddrLeaseValidTo },
		{ KCDTIdConfigDaemonManagerName,EText,		ENoAttrs,    KCDTypeNameConfigDaemonManagerName },
		{ KCDTIdConfigDaemonName,	 	EText,		ENoAttrs,    KCDTypeNameConfigDaemonName },
		{ KCDTIdEnableIpHeaderComp,	 	EBool,		ENoAttrs,    KCDTypeNameEnableIpHeaderComp },
		{ KCDTIdEnableLCPExtension,		EBool,		ENoAttrs,    KCDTypeNameEnableLcpExtensions  },
		{ KCDTIdDisablePlainTextAuth,   EBool,		ENoAttrs,    KCDTypeNameDisablePlainTextAuth },
		{ KCDTIdEnableSwComp,	 		EBool,		ENoAttrs,    KCDTypeNameEnableSwComp },
		{ KCDTIdBearerName,	 			EInt,		ENoAttrs,    KCDTypeNameBearerName },
		{ KCDTIdBearerSpeed,	 		EInt,		ENoAttrs,    KCDTypeNameBearerSpeed },
		{ KCDTIdBearerCe,	 			EInt,		ENoAttrs,    KCDTypeNameBearerCe },
		{ KCDTIdInitString,	 			EDesC8,		ENoAttrs,    KCDTypeNameInitString },
		{ KCDTIdBearerType,	 			EInt,		ENoAttrs,    KCDTypeNameISPBearerType },
		{ KCDTIdChannelCoding,	 		EInt,		ENoAttrs,    KCDTypeNameChannelCoding },
		{ KCDTIdAiur,	 				EInt,		ENoAttrs,    KCDTypeNameAiur },
		{ KCDTIdRequestedTimeSlots,	 	EInt,		ENoAttrs,    KCDTypeNameRequestedTimeSlots },
		{ KCDTIdMaximumTimeSlots,	 	EInt,		ENoAttrs,    KCDTypeNameMaximumTimeSlots },
		{ KCDTIdBearerService,	 		EInt,		ENoAttrs,    KCDTypeNameBearerService },
		{ KCDTIdBearerProtocol,	 		EInt,		ENoAttrs,    KCDTypeNameBearerProtocol },
		{ KCDTIdRlpVersion,	 			EInt,		ENoAttrs,    KCDTypeNameRlpVersion },
		{ KCDTIdIwfToMs,	 			EInt,		ENoAttrs,    KCDTypeNameIwfToMs },
		{ KCDTIdMsToIwf,	 			EInt,		ENoAttrs,    KCDTypeNameMsToIwf },
		{ KCDTIdAckTimer,	 			EInt,		ENoAttrs,    KCDTypeNameAckTimer },
		{ KCDTIdRetransmissionAttempts,	EInt,		ENoAttrs,    KCDTypeNameRetransmissionAttempts },
		{ KCDTIdResequencePeriod,	 	EInt,		ENoAttrs,    KCDTypeNameResequencePeriod },
		{ KCDTIdV42Compression,	 		EInt,		ENoAttrs,    KCDTypeNameV42Compression },
		{ KCDTIdV42Codewords,	 		EInt,		ENoAttrs,    KCDTypeNameV42Codewords },
		{ KCDTIdV42MaxLength,	 		EInt,		ENoAttrs,    KCDTypeNameV42MaxLength },
		{ KCDTIdAsymmetry,	 			EInt,		ENoAttrs,    KCDTypeNameAsymmetry },
		{ KCDTIdUserInitUpgrade,	 	EBool,		ENoAttrs,    KCDTypeNameUserInitUpgrade },
		{ KCDTIdUseEdge,	 			EBool,		ENoAttrs,    KCDTypeNameUseEdge },
		/**** add new fields above this comment ****/
        // service base class
		{ KCDTIdServiceEnableLlmnr,     EBool,      ENoAttrs,    KCDTypeNameServiceEnableLlmnr },
  		{ 0, 0, ENoAttrs, KCDNull }
  	};


    // 09/  DIAL IN ISP RECORD

  	const SRecordTypeInfo CommsDatSchemaV1_1::iDialInISPRecordInfo[] =
  	{
    	// CCDRecordBase class
		{ KCDTIdRecordTag,	        	    EInt,		ENoAttrs,    KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     		    EText,		ENotNull,    KCDTypeNameRecordName },
		// this class
		{ KCDTIdDINDescription,	 		    EText,		ENoAttrs,    KCDTypeNameDescription },
		{ KCDTIdDINType,	 				EInt,		ENoAttrs,    KCDTypeNameType },
		{ KCDTIdDINUseLoginScript,	 		EBool,		ENoAttrs,    KCDTypeNameUseLoginScript },
		{ KCDTIdDINLoginScript,	 		    ELongText,	ENoAttrs,    KCDTypeNameLoginScript },
		{ KCDTIdDINAuthentication,	 		EInt,		ENoAttrs,    KCDTypeNameAuthentication },
		{ KCDTIdDINIfAuthRetries,	 		EInt,		ENoAttrs,    KCDTypeNameIfAuthRetries },
		{ KCDTIdDINIfParams,	 			EText,		ENoAttrs,    KCDTypeNameIfParams },
		{ KCDTIdDINIfNetworks,	 			EText,		ENoAttrs,    KCDTypeNameIfNetworks },
		{ KCDTIdDINIpAddrFromServer,		EBool,		ENotNull,    KCDTypeNameIpAddrFromServer },
		{ KCDTIdDINIpAddr,	 				EText,		ENoAttrs,    KCDTypeNameIpAddr },
		{ KCDTIdDINIpNetMask,				EText,		ENoAttrs,    KCDTypeNameIpNetMask },
		{ KCDTIdDINIpGateway,	 			EText,		ENoAttrs,    KCDTypeNameIpGateway },
		{ KCDTIdDINIpDNSAddrFromServer,	    EBool,		ENotNull,    KCDTypeNameIpDNSAddrFromServer },
		{ KCDTIdDINIpNameServer1,	 		EText,		ENoAttrs,    KCDTypeNameIpNameServer1 },
		{ KCDTIdDINIpNameServer2,	 		EText,		ENoAttrs,    KCDTypeNameIpNameServer2 },
		{ KCDTIdDINIp6DNSAddrFromServer,	EBool,		ENoAttrs,    KCDTypeNameIp6DNSAddrFromServer },
		{ KCDTIdDINIp6NameServer1,		 	EText,		ENoAttrs,    KCDTypeNameIp6NameServer1 },
		{ KCDTIdDINIp6NameServer2,	 		EText,		ENoAttrs,    KCDTypeNameIp6NameServer2 },
		{ KCDTIdDINEnableIpHeaderComp,	 	EBool,		ENoAttrs,    KCDTypeNameEnableIpHeaderComp },
		{ KCDTIdDINEnableLCPExtensions,		EBool,		ENoAttrs,    KCDTypeNameEnableLcpExtensions },
		{ KCDTIdDINDisablePlainTextAuth,	EBool,		ENoAttrs,    KCDTypeNameDisablePlainTextAuth },
		{ KCDTIdDINEnableSwComp,	 		EBool,		ENoAttrs,    KCDTypeNameEnableSwComp },
		{ KCDTIdDINBearerName,	 			EInt,		ENoAttrs,    KCDTypeNameBearerName },
		{ KCDTIdDINBearerSpeed,	 		    EInt,		ENoAttrs,    KCDTypeNameBearerSpeed },
		{ KCDTIdDINBearerCe,	 			EInt,		ENoAttrs,    KCDTypeNameBearerCe },
		{ KCDTIdDINInitString,	 			EDesC8,		ENoAttrs,    KCDTypeNameInitString },
		{ KCDTIdDINUseEdge,	 			    EBool,		ENoAttrs,    KCDTypeNameUseEdge },
        /**** add new fields above this comment ****/
        // service base class
        { KCDTIdServiceEnableLlmnr,         EBool,      ENoAttrs,    KCDTypeNameServiceEnableLlmnr },
  		{ 0, 0, ENoAttrs, KCDNull }
  	};


    //  0A/  LAN Service Record

  	const SRecordTypeInfo CommsDatSchemaV1_1::iLANServiceRecordInfo[] =
  	{
    	// CCDRecordBase class
		{ KCDTIdRecordTag,	        			    EInt,     ENoAttrs,    KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     				    EText,    ENotNull,    KCDTypeNameRecordName },
		// this class
		{ KCDTIdLANIfNetworks,	 					EText,    ENoAttrs,    KCDTypeNameIfNetworks },
		{ KCDTIdLANIpNetMask,	 					EText,    ENoAttrs,    KCDTypeNameIpNetMask },
		{ KCDTIdLANIpGateway,	 					EText,    ENoAttrs,    KCDTypeNameIpGateway },
		{ KCDTIdLANIpAddrFromServer,	 			EBool,    ENotNull,    KCDTypeNameIpAddrFromServer },
		{ KCDTIdLANIpAddr,	 						EText,    ENoAttrs,    KCDTypeNameIpAddr },
		{ KCDTIdLANIpDNSAddrFromServer,	 		    EBool,    ENotNull,    KCDTypeNameIpDNSAddrFromServer },
		{ KCDTIdLANIpNameServer1,	 				EText,    ENoAttrs,    KCDTypeNameIpNameServer1 },
		{ KCDTIdLANIpNameServer2,	 				EText,    ENoAttrs,    KCDTypeNameIpNameServer2 },
		{ KCDTIdLANIp6DNSAddrFromServer,	 		EBool,    ENoAttrs,    KCDTypeNameIp6DNSAddrFromServer },
		{ KCDTIdLANIp6NameServer1,	 				EText,    ENoAttrs,    KCDTypeNameIp6NameServer1 },
		{ KCDTIdLANIp6NameServer2,	 				EText,    ENoAttrs,    KCDTypeNameIp6NameServer2 },
		{ KCDTIdLANIpAddrLeaseValidFrom,	 		EText,    ENoAttrs,    KCDTypeNameIpAddrLeaseValidFrom },
		{ KCDTIdLANIpAddrLeaseValidTo,	 			EText,    ENoAttrs,    KCDTypeNameIpAddrLeaseValidTo },
		{ KCDTIdLANConfigDaemonManagerName,		    EText,    ENoAttrs,    KCDTypeNameConfigDaemonManagerName },
		{ KCDTIdLANConfigDaemonName,	 			EText,    ENoAttrs,    KCDTypeNameConfigDaemonName },
		{ KCDTIdLANServiceExtensionTableName,		EText,    ENoAttrs,    KCDTypeNameServiceExtensionTableName },
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		{ KCDTIdLANServiceExtensionTableRecordId,	ELink,    ENoAttrs,    KCDTypeNameServiceExtensionTableRecordId },
#else
        { KCDTIdLANServiceExtensionTableRecordId,	EInt,    ENoAttrs,    KCDTypeNameServiceExtensionTableRecordId },
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        /**** add new fields above this comment ****/
        // service base class
        { KCDTIdServiceEnableLlmnr,                 EBool,    ENoAttrs,    KCDTypeNameServiceEnableLlmnr },
  		{ 0, 0, ENoAttrs, KCDNull }
  	};

  	const SRecordTypeInfo CommsDatSchemaV1_1::iWLANServiceExtRecordInfo[] =
  	{
    	// CCDRecordBase class
  		{ KCDTIdRecordTag,	        		EInt,   ENoAttrs,   KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     			EText,  ENotNull,   KCDTypeNameRecordName },
		// This class
		{ KCDTIdWLANProfile, 				EText,	ENotNull,	KCDTypeNameWLANProfile },
		{ KCDTIdWLANSSID,					EText,	ENotNull,	KCDTypeNameWLANSSID },
		{ KCDTIdWLANEncryptionStatus,		EInt,	ENoAttrs,	KCDTypeNameWLANEncryptionStatus },
		{ KCDTIdWLANEncryptionType,			EInt,	ENoAttrs,	KCDTypeNameWLANEncryptionType },
		{ KCDTIdWLANDefaultWepKey ,			EInt,	ENoAttrs,	KCDTypeNameWLANDefaultWepKey   },
		{ KCDTIdWLANEncryptKey1Valid,		EInt,	ENoAttrs,	KCDTypeNameWLANEncryptKey1Valid },
		{ KCDTIdWLANWepKey1,				EText,	ENoAttrs,	KCDTypeNameWLANWepKey1 },
		{ KCDTIdWLANEncryptKey2Valid, 		EInt,	ENoAttrs,	KCDTypeNameWLANEncryptKey2Valid },
		{ KCDTIdWLANWepKey2,				EText,	ENoAttrs,	KCDTypeNameWLANWepKey2 },
		{ KCDTIdWLANEncryptKey3Valid,		EInt,	ENoAttrs,	KCDTypeNameWLANEncryptKey3Valid },
		{ KCDTIdWLANWepKey3,				EText,	ENoAttrs,	KCDTypeNameWLANWepKey3 },
		{ KCDTIdWLANEncryptKey4Valid,		EInt,	ENoAttrs,	KCDTypeNameWLANEncryptKey4Valid },
		{ KCDTIdWLANWepKey4,				EText,	ENoAttrs,	KCDTypeNameWLANWepKey4 },
		{ KCDTIdWLANRanking,				EInt,	ENoAttrs,	KCDTypeNameWLANRanking  },
		{ KCDTIdWLANDialogPref,				EInt,	ENoAttrs,	KCDTypeNameWLANDialogPref },
		{ KCDTIdWLANNetworkType,			EInt,	ENoAttrs,	KCDTypeNameWLANNetworkType },
		{ KCDTIdWLANFragThres,				EInt,	ENoAttrs,	KCDTypeNameWLANFragThres },
		{ KCDTIdWLANRTSThres, 				EInt,	ENoAttrs,	KCDTypeNameWLANRTSThres },
		{ KCDTIdWLANDesTransRate,			EInt,	ENoAttrs,	KCDTypeNameWLANDesTransRate },
		{ KCDTIdWLANBeaconInt,				EInt,	ENoAttrs,	KCDTypeNameWLANBeaconInt },
		{ KCDTIdWLANChannelId,				EInt,	ENoAttrs,	KCDTypeNameWLANChannelId },
		{ KCDTIdWLANAuthenticationMode,		EInt,	ENoAttrs,	KCDTypeNameWLANAuthenticationMode },
		{ KCDTIdWLANPowerSaveMode,			EInt,	ENoAttrs,	KCDTypeNameWLANPowerSaveMode },
		{ KCDTIdWLANAllowUnencryptMode,		EInt,	ENoAttrs,	KCDTypeNameWLANAllowUnencryptMode },
		{ KCDTIdWLANAllowShortPre,			EInt,	ENoAttrs,	KCDTypeNameWLANAllowShortPre },
		{ KCDTIdWLANPreambleType,			EInt,	ENoAttrs,	KCDTypeNameWLANPreambleType },
		{ KCDTIdWLANTxPowerLevel,			EInt,	ENoAttrs,	KCDTypeNameWLANTxPowerLevel },
		{ KCDTIdWLANEapPresharedKey,		EMedText,	ENoAttrs,	KCDTypeNameWLANEapPresharedKey },
		{ KCDTIdWLANRegDomain,				EInt,	ENoAttrs,	KCDTypeNameWLANRegDomain },
		{ KCDTIdWLANSecData,				ELinkEAPSecRecord,	ENoAttrs,	KCDTypeNameWLANSecData },
		{ 0, 0, ENoAttrs, KCDNull }
  	};

  	const SRecordTypeInfo CommsDatSchemaV1_1::iVPNServiceRecordInfo[] =
  	{
    	// CCDRecordBase class
    	{ KCDTIdRecordTag,	EInt,		        ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	EText,		        ENotNull,   KCDTypeNameRecordName },
		// This class
		{ KCDTIdVPNPolicyName,	 	EText,              ENotNull,    KCDTypeNameVPNPolicyName },
		{ KCDTIdVPNIAPRecord, 		ELinkIAPRecord,	    ENoAttrs,    KCDTypeNameVPNIAPRecord },
		{ KCDTIdVPNSNAPRecord,      ELinkAPRecord,            ENoAttrs,    KCDTypeNameVPNSNAPRecord },
		{ KCDTIdVPNNetwork,	  		ELinkNetworkRecord, ENoAttrs,    KCDTypeNameVPNNetwork },
		/**** add new fields above this comment ****/
        // service base class
		{ KCDTIdServiceEnableLlmnr,	EBool,  ENoAttrs,    KCDTypeNameServiceEnableLlmnr },
  		{ 0, 0, ENoAttrs, KCDNull }
  	};

  
  	const SRecordTypeInfo CommsDatSchemaV1_1::iUmtsR99QoSAndOnTableRecordInfo[] =
	{
    	// CCDRecordBase class
		{ KCDTIdRecordTag,						 EInt,	  ENoAttrs,     KCDTypeNameRecordTag },
		{ KCDTIdRecordName,						 EText,   ENotNull,     KCDTypeNameRecordName },
		// this class
		{ KCDTIdWCDMAReqTrafficClass, 			 EUint32, ENoAttrs,     KCDTypeNameGPRSReqTrafficClass },
		{ KCDTIdWCDMAMinTrafficClass, 			 EUint32, ENoAttrs,     KCDTypeNameGPRSMinTrafficClass },
		{ KCDTIdWCDMAReqDeliveryOrder, 			 EUint32, ENoAttrs,	    KCDTypeNameGPRSReqDeliveryOrder },
		{ KCDTIdWCDMAMinDeliveryOrder	, 		 EUint32, ENoAttrs,	    KCDTypeNameGPRSMinDeliveryOrder },
		{ KCDTIdWCDMAReqDeliverErroneousSDU,	 EUint32, ENoAttrs,	    KCDTypeNameGPRSReqDeliverErroneousSDU },
		{ KCDTIdWCDMAMinDeliverErroneousSDU,	 EUint32, ENoAttrs,	    KCDTypeNameGPRSMinDeliverErroneousSDU },
		{ KCDTIdWCDMAReqMaxSDUSize, 			 EInt,    ENoAttrs,	    KCDTypeNameGPRSReqMaxSDUSize },
		{ KCDTIdWCDMAMinAcceptableMaxSDUSize,	 EInt,    ENoAttrs,	    KCDTypeNameGPRSMinAcceptableMaxSDUSize },
		{ KCDTIdWCDMAReqMaxUplinkRate, 			 EInt,    ENoAttrs,     KCDTypeNameGPRSReqMaxUplinkRate },
		{ KCDTIdWCDMAReqMinUplinkRate, 			 EInt,    ENoAttrs,	    KCDTypeNameGPRSReqMinUplinkRate },
		{ KCDTIdWCDMAReqMaxDownlinkRate, 		 EInt,    ENoAttrs,	    KCDTypeNameGPRSReqMaxDownlinkRate },
		{ KCDTIdWCDMAReqMinDownlinkRate, 		 EInt,    ENoAttrs,	    KCDTypeNameGPRSReqMinDownlinkRate },
		{ KCDTIdWCDMAReqBER, 					 EUint32, ENoAttrs,	    KCDTypeNameGPRSReqBER },
		{ KCDTIdWCDMAMaxBER, 					 EUint32, ENoAttrs,	    KCDTypeNameGPRSMaxBER },
		{ KCDTIdWCDMAReqSDUErrorRatio, 			 EUint32, ENoAttrs,	    KCDTypeNameGPRSReqSDUErrorRatio },
		{ KCDTIdWCDMAMaxSDUErrorRatio, 			 EUint32, ENoAttrs,	    KCDTypeNameGPRSMaxSDUErrorRatio },
		{ KCDTIdWCDMAReqTrafficHandlingPriority, EUint32, ENoAttrs,	    KCDTypeNameGPRSReqTrafficHandlingPriority },
		{ KCDTIdWCDMAMinTrafficHandlingPriority, EUint32, ENoAttrs,	    KCDTypeNameGPRSMinTrafficHandlingPriority },
		{ KCDTIdWCDMAReqTransferDelay, 			 EInt,    ENoAttrs,     KCDTypeNameGPRSReqTransferDelay },
		{ KCDTIdWCDMAMaxTransferDelay, 			 EInt,    ENoAttrs,	    KCDTypeNameGPRSMaxTransferDelay },
		{ KCDTIdWCDMAReqGuaranteedUplinkRate, 	 EInt,    ENoAttrs,	    KCDTypeNameGPRSReqGuaranteedUplinkRate },
		{ KCDTIdWCDMAMinGuaranteedUplinkRate, 	 EInt,    ENoAttrs,	    KCDTypeNameGPRSMinGuaranteedUplinkRate },
		{ KCDTIdWCDMAReqGuaranteedDownlinkRate,  EInt,    ENoAttrs,	    KCDTypeNameGPRSReqGuaranteedDownlinkRate },
		{ KCDTIdWCDMAMinGuaranteedDownlinkRate,  EInt,    ENoAttrs,	    KCDTypeNameGPRSMinGuaranteedDownlinkRate },
		{ KCDTIdWCDMASignallingIndication,		 EBool,   ENoAttrs,	    KCDTypeNameGPRSSignallingIndication },
		{ KCDTIdWCDMAImCmSignallingIndication,	 EBool,   ENoAttrs,	    KCDTypeNameGPRSImCnSignallingIndication },
		{ KCDTIdWCDMASourceStatisticsDescriptor, EUint32, ENoAttrs,	    KCDTypeNameGPRSSourceStatisticsDescriptor },
		{ 0, 0, ENoAttrs, KCDNull }

	};


  	const SRecordTypeInfo CommsDatSchemaV1_1::iWCDMAPacketServiceRecordInfo[] =
    {
    	// CCDRecordBase class
		{ KCDTIdRecordTag,						EInt,		ENoAttrs,    KCDTypeNameRecordTag },
		{ KCDTIdRecordName,						EText,      ENotNull,    KCDTypeNameRecordName },
		// This class
		{ KCDTIdAPN,	 							EMedText,   ENoAttrs,    KCDTypeNameGPRSAPN },
		{ KCDTIdWCDMPDPType,					EInt,		ENoAttrs,    KCDTypeNameGPRSPDPType },
		{ KCDTIdWCDMAPDPAddress,				EText,      ENoAttrs,    KCDTypeNameGPRSPDPAddress },
		{ KCDTIdReqPrecedence,	 				EInt,		ENoAttrs,    KCDTypeNameGPRSReqPrecedence },
		{ KCDTIdReqDelay,	 						EInt,		ENoAttrs,    KCDTypeNameGPRSReqDelay },
		{ KCDTIdReqReliability,	 					EInt,		ENoAttrs,    KCDTypeNameGPRSReqReliability  },
		{ KCDTIdReqPeakThroughput,				EInt,		ENoAttrs,    KCDTypeNameGPRSReqPeakThroughput },
		{ KCDTIdReqMeanThroughput,				EInt,		ENoAttrs,    KCDTypeNameGPRSReqMeanThroughput },
		{ KCDTIdMinPrecedence,	 				EInt,		ENoAttrs,    KCDTypeNameGPRSMinPrecedence },
		{ KCDTIdMinDelay,	 						EInt,		ENoAttrs,    KCDTypeNameGPRSMinDelay },
		{ KCDTIdMinReliability,	 					EInt,		ENoAttrs,    KCDTypeNameGPRSMinReliability },
		{ KCDTIdMinPeakThroughput,  				EInt,		ENoAttrs,    KCDTypeNameGPRSMinPeakThroughput },
		{ KCDTIdMinMeanThroughput,  				EInt,		ENoAttrs,    KCDTypeNameGPRSMinMeanThroughput },
		{ KCDTIdWCDMADataCompression, 			EBool,      ENoAttrs,    KCDTypeNameGPRSDataCompression },
		{ KCDTIdWCDMAHeaderCompression,			EBool,      ENoAttrs,    KCDTypeNameGPRSHeaderCompression },
		{ KCDTIdWCDMAUseEdge,	 				EBool,      ENoAttrs,    KCDTypeNameGPRSUseEdge },
		{ KCDTIdWCDMAAnonymousAccess,			EBool,      ENoAttrs,    KCDTypeNameGPRSAnonymousAccess },
		{ KCDTIdWCDMAIfParams,					EText,      ENoAttrs,    KCDTypeNameGPRSIfParams },
		{ KCDTIdWCDMAIfNetworks,	 				EText,      ENoAttrs,    KCDTypeNameIfNetworks },
		{ KCDTIdWCDMAIfPromptForAuth,			EBool,      ENoAttrs,    KCDTypeNameGPRSIfPromptForAuth },
		{ KCDTIdWCDMAIfAuthName,	 			EText,      ENoAttrs,    KCDTypeNameGPRSIfAuthName },
		{ KCDTIdWCDMAIfAuthPass,	 			EText,      ENoAttrs,    KCDTypeNameGPRSIfAuthPass },
		{ KCDTIdWCDMAIfAuthRetries,	 			EInt,       ENoAttrs,    KCDTypeNameGPRSIfAuthRetries },
		{ KCDTIdWCDMAIPNetMask,					EText,      ENoAttrs,    KCDTypeNameGPRSIPNetMask },
		{ KCDTIdWCDMAIPGateway,	 				EText,      ENoAttrs,    KCDTypeNameGPRSIPGateway },
		{ KCDTIdWCDMAIPAddrFromServer,			EBool,      ENoAttrs,    KCDTypeNameGPRSIPAddrFromServer },
		{ KCDTIdWCDMAIPAddr,	 				EText,      ENoAttrs,    KCDTypeNameGPRSIPAddr },
		{ KCDTIdWCDMAIPDNSAddrFromServer,		EBool,      ENoAttrs,    KCDTypeNameGPRSIPDNSAddrFromServer  },
		{ KCDTIdWCDMAIPNameServer1,	 			EText,      ENoAttrs,    KCDTypeNameGPRSIPNameServer1 },
		{ KCDTIdWCDMAIPNameServer2,	 			EText,      ENoAttrs,    KCDTypeNameGPRSIPNameServer2 },
		{ KCDTIdWCDMAIP6DNSAddrFromServer,		EBool,      ENoAttrs,    KCDTypeNameGPRSIP6DNSAddrFromServer },
		{ KCDTIdWCDMAIP6NameServer1,	 		EText,      ENoAttrs,    KCDTypeNameGPRSIP6NameServer1 },
		{ KCDTIdWCDMAIP6NameServer2,	 		EText,      ENoAttrs,    KCDTypeNameGPRSIP6NameServer2 },
		{ KCDTIdWCDMAIPAddrLeaseValidFrom,		EText,      ENoAttrs,    KCDTypeNameGPRSIPAddrLeaseValidFrom },
		{ KCDTIdWCDMAIPAddrLeaseValidTo,			EText,      ENoAttrs,    KCDTypeNameGPRSIPAddrLeaseValidTo },
		{ KCDTIdWCDMAConfigDaemonManagerName,   	EText,      ENoAttrs,    KCDTypeNameGPRSConfigDaemonManagerName },
		{ KCDTIdWCDMAConfigDaemonName,		    	EText,      ENoAttrs,    KCDTypeNameGPRSConfigDaemonName },
		{ KCDTIdWCDMAEnableLCPExtensions,	    		EBool,      ENoAttrs,    KCDTypeNameGPRSEnableLCPExtensions  },
		{ KCDTIdWCDMADisablePlainTextAuth,	    		EBool,      ENoAttrs,    KCDTypeNameGPRSDisablePlainTextAuth },
		{ KCDTIdAPType,	 				        	EInt,       ENoAttrs,    KCDTypeNameGPRSAPType },
		{ KCDTIdQOSWarningTimeOut,		        	EInt,       ENoAttrs,    KCDTypeNameGPRSQOSWarningTimeOut },
		{ KCDTIdUmtsR99QoSAndOn, 				ELinkUMTSR99QoSRecord,	    ENoAttrs,    KCDTypeNameUmtsR99QoSAndOn },
        /**** add new fields above this comment ****/
        // service base class
		{ KCDTIdServiceEnableLlmnr,		        	EBool,      ENoAttrs,    KCDTypeNameServiceEnableLlmnr },
		{ KCDTIdR5DataCompression,	 		   	EInt,       ENoAttrs,    KCDTypeNameR5DataCompression },
		{ KCDTIdR5HeaderCompression,	 	   	        EInt,       ENoAttrs,    KCDTypeNameR5HeaderCompression },
		{ KCDTIdPacketFlowIdentifier,	 	   	        EInt,       ENoAttrs,    KCDTypeNamePacketFlowIdentifier },
		{ KCDTIdUmtsGprsRelease,	 		   	EInt,       ENoAttrs,    KCDTypeNameUmtsGprsRelease },


		{ 0, 0, ENoAttrs, KCDNull }
    };


    //
    const SRecordTypeInfo CommsDatSchemaV1_1::iDefaultWCDMARecordInfo[] =
    {
		// CCDRecordBase class
		{  KCDTIdRecordTag,						EInt,		ENoAttrs,    KCDTypeNameRecordTag },
    	{  KCDTIdRecordName,					EText,		ENotNull,    KCDTypeNameRecordName },
		// This class
    	{  KCDTIdDefaultGPRSUsage,     			EUint32,    ENoAttrs,    KCDTypeNameUsage },
		{  KCDTIdDefaultGPRSAPN,     			EMedText,	ENoAttrs,    KCDTypeNameAPN },
		{  KCDTIdDefaultGPRSPDPType,    		EUint32,	ENotNull,    KCDTypeNamePDPType },
		{  KCDTIdDefaultGPRSPDPAddress,    		EText,		ENoAttrs,    KCDTypeNamePDPAddress },
		{  KCDTIdDefaultGPRSPrecedence,			EUint32,	ENoAttrs,    KCDTypeNamePrecedence },
		{  KCDTIdDefaultGPRSDelay,     			EUint32,	ENoAttrs,    KCDTypeNameDelay },
		{  KCDTIdDefaultGPRSReliability, 		EUint32,	ENoAttrs,    KCDTypeNameReliability },
		{  KCDTIdDefaultGPRSPeakThroughput,		EUint32,	ENoAttrs,    KCDTypeNamePeakThroughput },
		{  KCDTIdDefaultGPRSMeanThroughput,		EUint32,	ENoAttrs,    KCDTypeNameMeanThroughput },
		{  KCDTIdDefaultGPRSMinPrecedence,		EUint32,	ENoAttrs,    KCDTypeNameMinPrecedence },
		{  KCDTIdDefaultGPRSMinDelay,     		EUint32,	ENoAttrs,    KCDTypeNameMinDelay },
		{  KCDTIdDefaultGPRSMinReliability,		EUint32,	ENoAttrs,    KCDTypeNameMinReliability },
		{  KCDTIdDefaultGPRSMinPeakThroughput,	EUint32,	ENoAttrs,    KCDTypeNameMinPeakThroughput },
		{  KCDTIdDefaultGPRSMinMeanThroughput,	EUint32,	ENoAttrs,    KCDTypeNameMinMeanThroughput },
		{  KCDTIdDefaultGPRSDataCompression,	EBool,		ENoAttrs,    KCDTypeNameDataCompression },
		{  KCDTIdDefaultGPRSHeaderCompression,	EBool,		ENoAttrs,    KCDTypeNameHeaderCompression },
		{  KCDTIdDefaultGPRSUseEdge,            EBool,      ENoAttrs,    KCDTypeNameGPRSUseEdge },
		{  KCDTIdDefaultGPRSAnonymousAccess,	EBool,		ENoAttrs,    KCDTypeNameAnonymousAccess },
		{ 0, 0, ENoAttrs, KCDNull }
    };

	
	const SRecordTypeInfo CommsDatSchemaV1_1::iModemBearerRecordInfo[] = {
		// CCDRecordBase class
		{ KCDTIdRecordTag,					EInt,	ENoAttrs,    KCDTypeNameRecordTag },
		{ KCDTIdRecordName,					EText,  ENotNull,    KCDTypeNameRecordName },
		// This class
		{ KCDTIdNifName,					EText,  ENoAttrs,    KCDTypeNameNifName },
		{ KCDTIdPortName,					EText,  ENoAttrs,    KCDTypeNamePortName },
		{ KCDTIdTsyName,					EText,  ENoAttrs,    KCDTypeNameTsyName },
		{ KCDTIdCsyName,					EText,  ENoAttrs,    KCDTypeNameCsyName },
		{ KCDTIdLastSocketActivityTimeout,	EInt,	ENoAttrs,    KCDTypeNameLastSocketActivityTimeout },
		{ KCDTIdLastSessionClosedTimeout,	EInt,	ENoAttrs,    KCDTypeNameLastSessionClosedTimeout },
		{ KCDTIdLastSocketClosedTimeout,	EInt,	ENoAttrs,    KCDTypeNameLastSocketClosedTimeout },
		{ KCDTIdDataBits,					EInt,	ENoAttrs,    KCDTypeNameDataBits },
		{ KCDTIdStopBits,					EInt,	ENoAttrs,    KCDTypeNameStopBits },
		{ KCDTIdParity,						EInt,	ENoAttrs,    KCDTypeNameParity },
		{ KCDTIdRate,						EInt,	ENoAttrs,    KCDTypeNameRate },
		{ KCDTIdHandshaking,				EInt,	ENoAttrs,    KCDTypeNameHandShaking },
		{ KCDTIdSpecialRate,				EInt,	ENoAttrs,    KCDTypeNameSpecialRate },
		{ KCDTIdXonChar,					EInt,	ENoAttrs,    KCDTypeNameXonChar },
		{ KCDTIdXoffChar,					EInt,	ENoAttrs,    KCDTypeNameXoffChar },
		{ KCDTIdFaxClassPref,				EInt,	ENoAttrs,    KCDTypeNameFaxClassPref },
		{ KCDTIdSpeakerPref,				EInt,	ENoAttrs,    KCDTypeNameSpeakerPref },
		{ KCDTIdSpeakerVolPref,				EInt,	ENoAttrs,    KCDTypeNameSpeakerVolPref },
		{ KCDTIdModemInitString,			EDesC8,  ENoAttrs,    KCDTypeNameModemInitString },
		{ KCDTIdDataInitString,				EDesC8,  ENoAttrs,    KCDTypeNameDataInitString },
		{ KCDTIdFaxInitString,				EDesC8,  ENoAttrs,    KCDTypeNameFaxInitString },
		{ KCDTIdIspInitString,				EDesC8,  ENoAttrs,    KCDTypeNameIspInitString },
		{ KCDTIdDialPauseLength,			EText,  ENoAttrs,    KCDTypeNameDialPauseLength },
		{ KCDTIdCarrierTimeOut,				EText,  ENoAttrs,    KCDTypeNameCarrierTimeout },
		{ KCDTIdAutoAnswerRingCount,		EText,  ENoAttrs,    KCDTypeNameAutoAnswerRingCount },
		{ KCDTIdSpeakerVolControlLow,		EText,  ENoAttrs,    KCDTypeNameSpeakerVolControlLow },
		{ KCDTIdSpeakerVolControlMedium,	EText,  ENoAttrs,    KCDTypeNameSpeakerVolControlMedium },
		{ KCDTIdSpeakerVolControlHigh,		EText,  ENoAttrs,    KCDTypeNameSpeakerVolControlHigh },
		{ KCDTIdSpeakerAlwaysOff,			EText,  ENoAttrs,    KCDTypeNameSpeakerAlwaysOff },
		{ KCDTIdSpeakerOnUntilCarrier,		EText,  ENoAttrs,    KCDTypeNameSpeakerOnUntilCarrier },
		{ KCDTIdSpeakerAlwaysOn,			EText,  ENoAttrs,    KCDTypeNameSpeakerAlwaysOn },
		{ KCDTIdSpeakerOnAfterUntilCarrier,	 EText,  ENoAttrs,    KCDTypeNameSpeakerOnAfterUntilCarrier },
		{ KCDTIdDialToneWaitModifier,		EText,  ENoAttrs,    KCDTypeNameDialToneWaitModifier },
		{ KCDTIdCallProgress1,				EText,  ENoAttrs,    KCDTypeNameCallProgress1 },
		{ KCDTIdCallProgress2,				EText,  ENoAttrs,    KCDTypeNameCallProgress2 },
		{ KCDTIdCallProgress3,				EText,  ENoAttrs,    KCDTypeNameCallProgress3 },
		{ KCDTIdCallProgress4,				EText,  ENoAttrs,    KCDTypeNameCallProgress4 },
		{ KCDTIdEchoOff,					EText,  ENoAttrs,    KCDTypeNameEchoOff },
		{ KCDTIdVerboseText,				EText,  ENoAttrs,    KCDTypeNameVerboseText },
		{ KCDTIdQuietOff,					EText,  ENoAttrs,    KCDTypeNameQuietOff },
		{ KCDTIdQuietOn,					EText,  ENoAttrs,    KCDTypeNameQuietOn },
		{ KCDTIdDialCommandStateModifier,	EText,  ENoAttrs,    KCDTypeNameDialCommandStateModifier },
		{ KCDTIdOnLine,						EText,  ENoAttrs,    KCDTypeNameOnLine },
		{ KCDTIdResetConfiguration,			EText,  ENoAttrs,    KCDTypeNameResetConfiguration },
		{ KCDTIdReturnToFactoryDefs,		EText,  ENoAttrs,    KCDTypeNameReturnToFactoryDefs },
		{ KCDTIdDcdOnDuringLink,			EText,  ENoAttrs,    KCDTypeNameDcdOnDuringLink },
		{ KCDTIdDtrHangUp,					EText,  ENoAttrs,    KCDTypeNameDtrHangUp },
		{ KCDTIdDsrAlwaysOn,				EText,  ENoAttrs,    KCDTypeNameDsrAlwaysOn },
		{ KCDTIdRtsCtsHandshake,			EText,  ENoAttrs,    KCDTypeNameRtsCtsHandshake },
		{ KCDTIdXonXoffHandshake,			EText,  ENoAttrs,    KCDTypeNameXonXoffHandshake },
		{ KCDTIdEscapeCharacter,			EText,  ENoAttrs,    KCDTypeNameEscapeCharacter },
		{ KCDTIdEscapeGuardPeriod,			EText,  ENoAttrs,    KCDTypeNameEscapeGuardPeriod },
		{ KCDTIdFaxClassInterrogate,		EText,  ENoAttrs,    KCDTypeNameFaxClassInterrogate },
		{ KCDTIdFaxClass,					EText,  ENoAttrs,    KCDTypeNameFaxClass },
		{ KCDTIdNoDialTone,					EText,  ENoAttrs,    KCDTypeNameNoDialTone },
		{ KCDTIdBusy,						EText,  ENoAttrs,    KCDTypeNameBusy },
		{ KCDTIdNoAnswer,					EText,  ENoAttrs,    KCDTypeNameNoAnswer },
		{ KCDTIdCarrier,					EText,  ENoAttrs,    KCDTypeNameCarrier },
		{ KCDTIdConnect,					EText,  ENoAttrs,    KCDTypeNameConnect },
		{ KCDTIdCompressionClass5,			EText,  ENoAttrs,    KCDTypeNameCompressionClass5 },
		{ KCDTIdCompressionV42bis,			EText,  ENoAttrs,    KCDTypeNameCompressionClassV42bis },
		{ KCDTIdCompressionNone,			EText,  ENoAttrs,    KCDTypeNameCompressionNone },
		{ KCDTIdProtocolLapd,				EText,	ENoAttrs,    KCDTypeNameProtocolLapd  },
		{ KCDTIdProtocolAlt,				EText,	ENoAttrs,    KCDTypeNameProtocolAlt },
		{ KCDTIdProtocolAltcellular,		EText,	ENoAttrs,    KCDTypeNameProtocolAltcellular },
		{ KCDTIdProtocolNone,				EText,	ENoAttrs,    KCDTypeNameProtocolNone },
		{ KCDTIdMessageCentreNumber,		EText,	ENoAttrs,    KCDTypeNameMessageCentreNumber },
		{ KCDTIdMessageValidityPeriod,		EInt,	ENoAttrs,    KCDTypeNameMessageValidityPeriod },
		{ KCDTIdMessageDeliveryReport,		EBool,	ENoAttrs,    KCDTypeNameMessageDeliveryReport },
		{ KCDTIdMinSignalLevel,				EInt,	ENoAttrs,    KCDTypeNameMinSignalLevel },
		{ KCDTIdCommRole,					EInt,	ENoAttrs,    KCDTypeNameCommRole },
		{ KCDTIdControlChannelPortName,		EText,	ENoAttrs,    KCDTypeNameControlChannelPortName  },
		{ KCDTIdSirSettings,				EInt,	ENoAttrs,    KCDTypeNameSirSettings },
		{ KCDTIdBCAStack,					EText,  ENoAttrs,    KCDTypeNameBCAStack },
		{ KCDTIdBearerTechnology,			EInt,	ENoAttrs,    KCDTypeNameBearerTechnology	},
		/**** add new fields above this comment ****/
        // bearer base class
        { KCDTIdBearerAgent,				EText,  ENoAttrs,    KCDTypeNameBearerAgent },
		{ 0, 0, ENoAttrs, KCDNull }

	};


    const SRecordTypeInfo CommsDatSchemaV1_1::iLANBearerRecordInfo[] = {
		// CCDRecordBase class
    	{ KCDTIdRecordTag,					    EInt,	ENoAttrs,    KCDTypeNameRecordTag },
		{ KCDTIdRecordName,					    EText,	ENotNull,    KCDTypeNameRecordName },
		// This class
		{ KCDTIdLANBearerNifName,			    EText,	ENotNull,    KCDTypeNameNifName },
		{ KCDTIdLANBearerLddFilename,		    EText,	ENoAttrs,    KCDTypeNameLanBearerLddFilename },
		{ KCDTIdLANBearerLddName,			    EText,	ENotNull,    KCDTypeNameLanBearerLddName },
		{ KCDTIdLANBearerPddFilename,		    EText,	ENoAttrs,    KCDTypeNameLanBearerPddFilename },
		{ KCDTIdLANBearerPddName,			    EText,	ENotNull,    KCDTypeNameLanBearerPddName },
		{ KCDTIdLANBearerPacketDriverName,	    EText,	ENoAttrs,    KCDTypeNameLanBearerPacketDriverName },
		{ KCDTIdLANLastSocketActivityTimeout,	EInt,	ENoAttrs,    KCDTypeNameLastSocketActivityTimeout },
		{ KCDTIdLANLastSessionClosedTimeout,	EInt,	ENoAttrs,    KCDTypeNameLastSessionClosedTimeout },
		{ KCDTIdLANLastSocketClosedTimeout,	    EInt,	ENoAttrs,    KCDTypeNameLastSocketClosedTimeout },
		{ KCDTIdBearerTechnology,				EInt,	ENoAttrs,    KCDTypeNameBearerTechnology	},
        /**** add new fields above this comment ****/
        // bearer base class
        { KCDTIdBearerAgent,				    EText,	ENotNull,    KCDTypeNameBearerAgent },
	    { 0, 0, ENoAttrs, KCDNull }
	};


    const SRecordTypeInfo CommsDatSchemaV1_1::iVirtualBearerRecordInfo[] = {
		// CCDRecordBase class
    	{KCDTIdRecordTag,	                EInt,	ENoAttrs,    KCDTypeNameRecordTag },
		{KCDTIdRecordName,	                EText,	ENotNull,    KCDTypeNameRecordName },
		// This class
		{KCDTIdVirtualBearerNifName,        EText,	ENotNull,    KCDTypeNameNifName },
    	{KCDTIdVBLastSocketActivityTimeout, EInt,	ENoAttrs,    KCDTypeNameLastSocketActivityTimeout },
     	{KCDTIdVBLastSessionClosedTimeout,  EInt,	ENoAttrs,    KCDTypeNameLastSessionClosedTimeout },
        {KCDTIdVBLastSocketClosedTimeout,	EInt,	ENoAttrs,    KCDTypeNameLastSocketClosedTimeout },
        {KCDTIdBearerTechnology,			EInt,	ENoAttrs,    KCDTypeNameBearerTechnology	},
    	/**** add new fields above this comment ****/
        // bearer base class
        {KCDTIdBearerAgent,                 EText,	ENotNull,    KCDTypeNameBearerAgent },
    	{ 0, 0, ENoAttrs, KCDNull }
	};


	const SRecordTypeInfo CommsDatSchemaV1_1::iWAPSMSBearerRecordInfo[] =
    {
		// CCDRecordBase class
		{ KCDTIdRecordTag,	                EInt,	ENoAttrs,    KCDTypeNameRecordTag },
		{ KCDTIdRecordName,         	    EText,	ENotNull,    KCDTypeNameRecordName },
		// This class
		{ KCDTIdWAPSMSAccessPointId,	    ELinkWAPAPRecord,	ENotNull,    KCDTypeNameWAPAccessPointId },
		{ KCDTIdWAPSMSGatewayAddress,	    EText,  ENoAttrs,    KCDTypeNameWAPGatewayAddress },
		{ KCDTIdWAPSMSServiceCentreAddress,	EText,	ENoAttrs,    KCDTypeNameWAPServiceCentreAddress },
		{ KCDTIdWAPSMSWSPOption,	        EInt,	ENoAttrs,    KCDTypeNameWAPWSPOption },
		{ KCDTIdWAPSMSSecurity,	            EBool,	ENoAttrs,    KCDTypeNameWAPSecurity },
		{ 0, 0, ENoAttrs, KCDNull }
    };

    const SRecordTypeInfo CommsDatSchemaV1_1::iWAPIPBearerRecordInfo[] = {
		// CCDRecordBase class
    	{ KCDTIdRecordTag,	         EInt,  ENoAttrs,    KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	         EText, ENotNull,    KCDTypeNameRecordName },
		// This class
    	{ KCDTIdWAPAccessPointId,	 ELinkWAPAPRecord,	ENotNull,    KCDTypeNameWAPAccessPointId },
		{ KCDTIdWAPGatewayAddress,	 EText,             ENoAttrs,    KCDTypeNameWAPGatewayAddress },
		{ KCDTIdWAPIAP,	 			 ELinkIAPRecord,	ENoAttrs,    KCDTypeNameWAPIAP },
		{ KCDTIdWAPWSPOption,	     EInt,	            ENoAttrs,    KCDTypeNameWAPWSPOption },
		{ KCDTIdWAPSecurity,	     EBool,             ENoAttrs,    KCDTypeNameWAPSecurity },
		{ KCDTIdWAPProxyPort,	 	 EInt,	            ENotNull,    KCDTypeNameWAPProxyPort },
		{ KCDTIdWAPProxyLoginName,	 EText,	            ENoAttrs,    KCDTypeNameWAPProxyLoginName },
		{ KCDTIdWAPProxyLoginPass,	 EText,             ENoAttrs,    KCDTypeNameWAPProxyLoginPass },
		{ 0, 0, ENoAttrs, KCDNull }
	};


    const SRecordTypeInfo CommsDatSchemaV1_1::iChargecardRecordInfo[] = {
		// CCDRecordBase class
		{ KCDTIdRecordTag,		EInt,	ENoAttrs,    KCDTypeNameRecordTag },
		{ KCDTIdRecordName,		EText,	ENotNull,    KCDTypeNameRecordName },
		// This class
		{ KCDTIdAccountNo,		EText,	ENoAttrs,    KCDTypeNameAccountNo },
		{ KCDTIdPin,			EText,	ENoAttrs,    KCDTypeNamePin },
		{ KCDTIdLocalRule,		EText,	ENoAttrs,    KCDTypeNameLocalRule },
		{ KCDTIdNatRule,		EText,  ENoAttrs,    KCDTypeNameNatRule },
		{ KCDTIdIntlRule,		EText,  ENoAttrs,    KCDTypeNameIntlRule },
        { 0, 0, ENoAttrs, KCDNull }
	};

    const SRecordTypeInfo CommsDatSchemaV1_1::iProxiesRecordInfo[] = {
		// CCDRecordBase class
    	{ KCDTIdRecordTag,	EInt,		        ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	EText,		        ENotNull,   KCDTypeNameRecordName },
		// This class
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    	{ KCDTIdISP,				ELink,		ENoAttrs,    KCDTypeNameProxyISP },
#else
        { KCDTIdISP,				EInt,		ENoAttrs,    KCDTypeNameProxyISP },
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		{ KCDTIdProxyServiceType,	EText,		ENoAttrs,    KCDTypeNameProxyServiceType },
		{ KCDTIdUseProxyServer,		EBool,		ENotNull,    KCDTypeNameUseProxyServer },
		{ KCDTIdServerName,			ELongText,	ENoAttrs,    KCDTypeNameServerName },
		{ KCDTIdProtocolName,		EText,		ENotNull,    KCDTypeNameProtocolName },
		{ KCDTIdPortNumber,			EInt,		ENoAttrs,    KCDTypeNamePortNumber },
		{ KCDTIdExceptions,			ELongText,  ENoAttrs,    KCDTypeNameExceptions },
	    { 0, 0, ENoAttrs, KCDNull }
	};

	const SRecordTypeInfo CommsDatSchemaV1_1::iSecureSocketRecordInfo[] = {
		// CCDRecordBase class
    	{ KCDTIdRecordTag,	     EInt,	    ENoAttrs,    KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	     EText,	    ENotNull,    KCDTypeNameRecordName },
		// This class
		{ KCDTIdSSProtoName,     EText,	    ENotNull,    KCDTypeNameSSProtoName },
	    { KCDTIdSSProtoLibrary,  EText,	    ENotNull,    KCDTypeNameSSProtoLibrary },
	    { 0, 0, ENoAttrs, KCDNull }
	};


    const SRecordTypeInfo CommsDatSchemaV1_1::iAgentLookupRecordInfo[] = {
		// CCDRecordBase class
    	{ KCDTIdRecordTag,	        EInt,		ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	        EText,		ENotNull,   KCDTypeNameRecordName },
		// This class
		{ KCDTIdAgentFriendlyName,  EText,		ENotNull,   KCDTypeNameAgentFriendlyName },
    	{ KCDTIdAgentFileName,      EText,		ENotNull,   KCDTypeNameAgentFileName },
		{ 0, 0, ENoAttrs, KCDNull }
	};

    const SRecordTypeInfo CommsDatSchemaV1_1::iPANServiceExtRecordInfo[] = {
		// CCDRecordBase class
    	{ KCDTIdRecordTag,	        	EInt,		ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	        	EText,		ENotNull,   KCDTypeNameRecordName },
		// This class
    	{ KCDTIdLocalRole,	        	EInt,		ENotNull,   KCDTypeNameLocalRole },
		{ KCDTIdPeerRole,	        	EInt,		ENotNull,   KCDTypeNamePeerRole },
		{ KCDTIdPeerMACAddresses,  		EText,		ENotNull,   KCDTypeNamePeerMACAddresses },
    	{ KCDTIdPromptForRemoteDevices,	EBool,		ENotNull,   KCDTypeNamePromptForRemoteDevices },
    	{ KCDTIdDisableSdpQuery,		EBool,		ENotNull,   KCDTypeNameDisableSdpQuery },
		{ KCDTIdAllowIncoming,			EBool,		ENotNull,   KCDTypeNameAllowIncoming },
    	{ KCDTIdPromptIfMACListFails,	EBool,		ENotNull,   KCDTypeNamePromptIfMACListFails },
#ifdef SYMBIAN_NETWORKING_DHCPSERVER
		{ KCDTIdNapServiceEnabled,			EBool,		ENotNull,   KCDTypeNameNapServiceEnabled },
#endif // SYMBIAN_NETWORKING_DHCPSERVER    	
		/**** add new fields above this comment ****/
        // service base class [BTW it is not correct that this inherits from Service Record base]
        { KCDTIdServiceEnableLlmnr,		        EBool,      ENoAttrs,    KCDTypeNameServiceEnableLlmnr },
		{ 0, 0, ENoAttrs, KCDNull }
	};

    const SRecordTypeInfo CommsDatSchemaV1_1::iAccessPointRecordInfo[] = {
		// CCDRecordBase class
    	{ KCDTIdRecordTag,	EInt,		        ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	EText,		        ENotNull,   KCDTypeNameRecordName },
		// This class
   		{ KCDTIdAccessPointGID,	    EInt,		            ENotNull,   KCDTypeNameAccessPointGID },
		{ KCDTIdSelectionPolicy,    ELink,		            ENotNull,   KCDTypeNameSelectionPolicy },
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    	{ KCDTIdTier,                   ELinkTierRecord,		ENotNull,   KCDTypeNameTier },
		{ KCDTIdMCpr,                   ELinkMCprRecord,		ENotNull,   KCDTypeNameMCpr },
		{ KCDTIdCpr,                    ELinkCprRecord,		    ENotNull,   KCDTypeNameCpr },
		{ KCDTIdSCpr,                   ELinkSCprRecord,		ENotNull,   KCDTypeNameSCpr },
		{ KCDTIdProtocol,               ELinkProtocolRecord,    ENotNull,   KCDTypeNameProtocol },
		{ KCDTIdCprConfig,              EInt,		            ENotNull,   KCDTypeNameCprConfig },
		{ KCDTIdAppSID,                 EInt,		            ENoAttrs,   KCDTypeNameAppSID },
		{ KCDTIdConfigAPIdList,         EText,                  ENoAttrs,   KCDTypeNameConfigAPIdList },
		{ KCDTIdCustomSelectionPolicy,  EInt,		            ENoAttrs,   KCDTypeNameCustomSelectionPolicy },
		{ KCDTIdAccessPointPriority,    EUint32,		ENoAttrs,	KCDTypeNameAccessPointPriority },
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	    { 0, 0, ENoAttrs, KCDNull }
	};

	const SRecordTypeInfo CommsDatSchemaV1_1::iIAPPrioritySelectionPolicyRecordInfo[] = {
		// CCDRecordBase class
   		{ KCDTIdRecordTag,	EInt,		ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	EText,		ENotNull,   KCDTypeNameRecordName },
		// This class
   		{ KCDTIdIap1,       ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap1Name },
		{ KCDTIdIap2,       ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap2Name },
		{ KCDTIdIap3,       ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap3Name },
		{ KCDTIdIap4,       ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap4Name },
		{ KCDTIdIap5,       ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap5Name },
		{ KCDTIdIap6,       ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap6Name },
		{ KCDTIdIap7,       ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap7Name },
		{ KCDTIdIap8,       ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap8Name },
		{ KCDTIdIap9,       ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap9Name },
		{ KCDTIdIap10,      ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap10Name },
		{ KCDTIdIap11,      ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap11Name },
		{ KCDTIdIap12,      ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap12Name },
		{ KCDTIdIap13,      ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap13Name },
		{ KCDTIdIap14,      ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap14Name },
		{ KCDTIdIap15,      ELinkIAPRecord,		ENoAttrs,   KCDTypeNameIap15Name },
		{ KCDTIdIapCount,   EInt,		ENoAttrs,   KCDTypeNameIapCountName },
		{ 0, 0, ENoAttrs, KCDNull }
	};
    
    
	const SRecordTypeInfo CommsDatSchemaV1_1::iEAPSecRecordInfo[] = {
		// CCDRecordBase class
	  	{ KCDTIdRecordTag,	        	EInt,   ENoAttrs,   KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     		EText,  ENotNull,   KCDTypeNameRecordName },
		// This class
		{ KCDTIdEAPSecOuterEAPType,		EInt,	ENoAttrs,	KCDTypeNameEAPSecOuterEAPType },
		{ KCDTIdEAPSecId,				EMedText,	ENoAttrs,	KCDTypeNameEapId },
		{ KCDTIdEAPSecPassword,			EMedText,	ENoAttrs,	KCDTypeNameEAPSecPassword },	
		{ KCDTIdEAPSecConfigId,			EUint32,ENoAttrs,	KCDTypeNameEAPSecConfigId },
		{ KCDTIdEAPSecData,				EInt,	ENoAttrs,	KCDTypeNameEAPSecData },
        { KCDTIdEAPSecVendorId,			EText,  ENoAttrs,	KCDTypeNameEAPSecVendorId },
        { KCDTIdEAPSecVendorType,		EUint32,ENoAttrs,	KCDTypeNameEAPSecVendorType },
		{ 0, 0, ENoAttrs, KCDNull }
  	};

	const SRecordTypeInfo CommsDatSchemaV1_1::iTunEAPRecordInfo[] = {
		// CCDRecordBase class
	  	{ KCDTIdRecordTag,	       		EInt,   ENoAttrs,   KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,    			EText,  ENotNull,   KCDTypeNameRecordName },
		// This class
		{ KCDTIdTUNEAPInnerType,		EInt,	ENoAttrs,	KCDTypeNameTUNEAPInnerType },
		{ KCDTIdTUNEAPData,				EInt,	ENoAttrs,	KCDTypeNameTUNEAPData },
		{ 0, 0, ENoAttrs, KCDNull }
  	};

	const SRecordTypeInfo CommsDatSchemaV1_1::iEAPTLSRecordInfo[] = {
		// CCDRecordBase class
	  	{ KCDTIdRecordTag,	       		EInt,   ENoAttrs,   KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,    			EText,  ENotNull,   KCDTypeNameRecordName },
		// This class
		{ KCDTIdEAPTLSClientCertName,	EText,	ENotNull,	KCDTypeNameEAPTLSClientCertName },
		{ 0, 0, ENoAttrs, KCDNull }
  	};

	const SRecordTypeInfo CommsDatSchemaV1_1::iLEAPRecordInfo[] = {
		// CCDRecordBase class
	  	{ KCDTIdRecordTag,	       		EInt,   ENoAttrs,   KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,    			EText,  ENotNull,   KCDTypeNameRecordName },
		// This class
		{ KCDTIdLEAPTimeout,			EInt,	ENoAttrs,	KCDTypeNameLEAPTimeout },
		{ 0, 0, ENoAttrs, KCDNull }
  	};

	 const SRecordTypeInfo CommsDatSchemaV1_1::iEAPSIMRecordInfo[] = {
 		// CCDRecordBase class
 		{ KCDTIdRecordTag, EInt, ENoAttrs, KCDTypeNameRecordTag },
   		{ KCDTIdRecordName, EText, ENotNull, KCDTypeNameRecordName },
		// This class
   		{ KCDTIdEapSimProtocolId, EInt, ENotNull, KCDTypeNameEapSimProtocolId },
   		{ KCDTIdEapSimPseudonym, EDesC8, ENotNull, KCDTypeNameEapSimPseudonym },
		{ KCDTIdEapSimUseNaiRealm, EBool, ENotNull, KCDTypeNameEapSimUseNaiRealm },
		{ KCDTIdEapSimNaiRealm, EDesC8, ENotNull, KCDTypeNameEapSimNaiRealm },
		{ KCDTIdEapSimMinRands, EInt, ENotNull, KCDTypeNameEapSimMinRands },
		{ 0, 0, ENoAttrs, KCDNull }
	};
	
    const SRecordTypeInfo CommsDatSchemaV1_1::iEAPAKARecordInfo[] = {
		// CCDRecordBase class
    	{ KCDTIdRecordTag,	EInt,		        ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	EText,		        ENotNull,   KCDTypeNameRecordName },
		// This class
 		{ KCDTIdEapAkaProtocolId,           EInt,   ENotNull, KCDTypeNameEapAkaProtocolId },
   		{ KCDTIdEapAkaPseudonym,            EDesC8, ENotNull, KCDTypeNameEapAkaPseudonym },
		{ KCDTIdEapAkaUseNaiRealm,          EBool,  ENotNull, KCDTypeNameEapAkaUseNaiRealm },
		{ KCDTIdEapAkaNaiRealm,             EDesC8, ENotNull, KCDTypeNameEapAkaNaiRealm },
	    { 0, 0, ENoAttrs, KCDNull }
    };
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    const SRecordTypeInfo CommsDatSchemaV1_1::iAPPrioritySelectionPolicyRecordInfo[] = {
		// CCDRecordBase class
    	{ KCDTIdRecordTag,	EInt,		        ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	EText,		        ENotNull,   KCDTypeNameRecordName },
		// This class
   		{ KCDTIdAp1,        ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp1Name },
		{ KCDTIdAp2,        ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp2Name },
		{ KCDTIdAp3,        ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp3Name },
		{ KCDTIdAp4,        ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp4Name },
		{ KCDTIdAp5,        ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp5Name },
		{ KCDTIdAp6,        ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp6Name },
		{ KCDTIdAp7,        ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp7Name },
		{ KCDTIdAp8,        ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp8Name },
		{ KCDTIdAp9,        ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp9Name },
		{ KCDTIdAp10,       ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp10Name },
		{ KCDTIdAp11,       ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp11Name },
		{ KCDTIdAp12,       ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp12Name },
		{ KCDTIdAp13,       ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp13Name },
		{ KCDTIdAp14,       ELinkAPRecord,	    ENoAttrs,   KCDTypeNameAp14Name },
		{ KCDTIdAp15,       ELinkAPRecord,		ENoAttrs,   KCDTypeNameAp15Name },
		{ KCDTIdApCount,   EInt,		ENoAttrs,   KCDTypeNameApCountName },
		{ 0, 0, ENoAttrs, KCDNull }
	};

    const SRecordTypeInfo CommsDatSchemaV1_1::iTierRecordInfo[] = {
		// CCDRecordBase class
   		{ KCDTIdRecordTag,	        EInt,		ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	        EText,		ENotNull,   KCDTypeNameRecordName },
		// This class
		{ KCDTIdTierImplUid,		EInt,		ENotNull,   KCDTypeNameTierImplUid },
    	{ KCDTIdTierThreadName,     EText,		ENotNull,   KCDTypeNameTierThreadName },
    	{ KCDTIdTierManagerName,    EText,		ENotNull,   KCDTypeNameTierManagerName },
    	{ KCDTIdDefaultAccessPoint, ELinkAPRecord,	ENotNull,   KCDTypeNameDefaultAccessPoint },
    	{ KCDTIdPromptUser,         EInt,		ENotNull,   KCDTypeNamePromptUser },
		{ 0, 0, ENoAttrs, KCDNull }
	};

    const SRecordTypeInfo CommsDatSchemaV1_1::iMCprRecordInfo[] = {
		// CCDRecordBase class
   		{ KCDTIdRecordTag,	        EInt,		ENoAttrs,   KCDTypeNameRecordTag },
   		{ KCDTIdRecordName,	        EText,		ENotNull,   KCDTypeNameRecordName },
		// This class
   		{ KCDTIdMCprUid,            EInt,		ENotNull,   KCDTypeNameMCprUid },
		{ 0, 0, ENoAttrs, KCDNull }
	};

    const SRecordTypeInfo CommsDatSchemaV1_1::iCprRecordInfo[] = {
		// CCDRecordBase class
   		{ KCDTIdRecordTag,	        EInt,		ENoAttrs,   KCDTypeNameRecordTag },
   		{ KCDTIdRecordName,	        EText,		ENotNull,   KCDTypeNameRecordName },
		// This class
   		{ KCDTIdCprUid,             EInt,		ENotNull,   KCDTypeNameCprUid },
		{ 0, 0, ENoAttrs, KCDNull }
	};

    const SRecordTypeInfo CommsDatSchemaV1_1::iSCprRecordInfo[] = {
		// CCDRecordBase class
   		{ KCDTIdRecordTag,	        EInt,		ENoAttrs,   KCDTypeNameRecordTag },
   		{ KCDTIdRecordName,	        EText,		ENotNull,   KCDTypeNameRecordName },
		// This class
   		{ KCDTIdSCprUid,            EInt,		ENotNull,   KCDTypeNameSCprUid },
		{ 0, 0, ENoAttrs, KCDNull }
	};

    const SRecordTypeInfo CommsDatSchemaV1_1::iProtocolRecordInfo[] = {
		// CCDRecordBase class
   		{ KCDTIdRecordTag,	        EInt,		ENoAttrs,   KCDTypeNameRecordTag },
   		{ KCDTIdRecordName,	        EText,		ENotNull,   KCDTypeNameRecordName },
		// This class
   		{ KCDTIdProtocolUid,        EInt,		ENotNull,   KCDTypeNameProtocolUid },
   		{ KCDTIdProtocolConfigLoaderUid, EInt,          ENoAttrs,   KCDTypeNameProtocolConfigLoaderUid },
		{ 0, 0, ENoAttrs, KCDNull }
	};

    const SRecordTypeInfo CommsDatSchemaV1_1::iBearerTypeRecordInfo[] = {
		// CCDRecordBase class
    	{ KCDTIdRecordTag,	EInt,		        ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	EText,		        ENotNull,   KCDTypeNameRecordName },
		// This class
   		{ KCDTIdBearerTypeTier,     ELinkTierRecord,	    ENotNull,   KCDTypeNameTier },
		{ KCDTIdBearerTypeMCpr,     ELinkMCprRecord,        ENotNull,   KCDTypeNameMCpr },
		{ KCDTIdBearerTypeCpr,      ELinkCprRecord,         ENotNull,   KCDTypeNameCpr  },
		{ KCDTIdBearerTypeSCpr,     ELinkSCprRecord,	    ENotNull,   KCDTypeNameSCpr },
		{ KCDTIdBearerTypeProtocol, ELinkProtocolRecord,    ENotNull,   KCDTypeNameProtocol },
	    { 0, 0, ENoAttrs, KCDNull }
	};
		
	const SRecordTypeInfo CommsDatSchemaV1_1::iConfigAccessPointRecordInfo[] = {
		// CCDRecordBase class
        { KCDTIdRecordTag,	        EInt,		ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	        EText,		ENotNull,   KCDTypeNameRecordName },
		// This class
        { KCDTIdConfigAccessPointSCpr,              ELinkSCprRecord,    ENotNull,   KCDTypeNameConfigAccessPointSCpr },
		{ KCDTIdConfigAccessPointProtocol,          ELinkProtocolRecord,ENotNull,   KCDTypeNameConfigAccessPointProtocol },
		{ KCDTIdConfigAccessPointProtocolConfig,    EInt,               ENotNull,   KCDTypeNameConfigAccessPointProtocolConfig },
		{ KCDTIdConfigAccessPointLayerBelow,        ELinkCAPRecord,     ENoAttrs,   KCDTypeNameConfigAccessPointLayerBelow },
		{ 0, 0, ENoAttrs, KCDNull }
     };

 	const SRecordTypeInfo CommsDatSchemaV1_1::iPolicySelectorRecordInfo[] = {
 		{ KCDTIdRecordTag,	  		EInt,		ENoAttrs,	KCDTypeNameRecordTag },
 		{ KCDTIdRecordName,	  		EText,		ENotNull,	KCDTypeNameRecordName },
 		{ KCDTIdPolicyId,			EInt,		ENotNull,	KCDTypeNamePolicyId },
 		{ KCDTIdAppUid,				EInt,		ENotNull,	KCDTypeNameAppUid },
 		{ KCDTIdSrcAddress,			EText,		ENotNull,	KCDTypeNameSrcAddress },
 		{ KCDTIdSrcMask,			EText,		ENotNull,	KCDTypeNameSrcMask },
 		{ KCDTIdDstAddress,			EText,		ENotNull,	KCDTypeNameDstAddress },
 		{ KCDTIdDstMask,			EText,		ENotNull,	KCDTypeNameDstMask },
 		{ KCDTIdSrcPort,			EInt,		ENotNull,	KCDTypeNameSrcPort },
 		{ KCDTIdDstPort,			EInt,		ENotNull,	KCDTypeNameDstPort },
 		{ KCDTIdSrcPortMax,			EInt,		ENotNull,	KCDTypeNameSrcPortMax },
 		{ KCDTIdDstPortMax,			EInt,		ENotNull,	KCDTypeNameDstPortMax },
 		{ KCDTIdProtocolId,			EUint32,	ENotNull,	KCDTypeNameProtocolId },
 		{ KCDTIdIAPid,				EUint32,	ENotNull,	KCDTypeNameIAPid },
 		{ KCDTIdPriority,			EInt,		ENotNull,	KCDTypeNamePriority },
 		{ 0, 0, ENoAttrs, KCDNull }
     };
 
 	const SRecordTypeInfo CommsDatSchemaV1_1::iPolicySelector2ParamsRecordInfo[] = {
		{ KCDTIdRecordTag,	        		EInt,		ENoAttrs,   KCDTypeNameRecordTag },
 		{ KCDTIdRecordName,	        		EText,		ENotNull,   KCDTypeNameRecordName },
   		{ KCDTIdPolicySelectorId,	  		EInt,		ENotNull,	KCDTypeNamePolicySelectorId },
 		{ KCDTIdParamsId,			EInt,		ENotNull,	KCDTypeNameParamsId },
 		{ 0, 0, ENoAttrs, KCDNull }
     };

 	const SRecordTypeInfo CommsDatSchemaV1_1::iGenericQosRecordInfo[] = {
    		{ KCDTIdRecordTag,	        		EInt,		ENoAttrs,   KCDTypeNameRecordTag },
 		{ KCDTIdRecordName,	        		EText,		ENotNull,   KCDTypeNameRecordName },
     	{ KCDTIdUplinkBandwidth,     		EInt,		ENotNull,   KCDTypeNameUplinkBandwidth},
     	{ KCDTIdDownLinkMaximumBurstSize,   EInt,		ENotNull,   KCDTypeNameDownLinkMaximumBurstSize},
     	{ KCDTIdUpLinkMaximumBurstSize,     EInt,		ENotNull,   KCDTypeNameUpLinkMaximumBurstSize},
     	{ KCDTIdDownLinkAveragePacketSize,  EInt,		ENotNull,   KCDTypeNameDownLinkAveragePacketSize},
     	{ KCDTIdUpLinkAveragePacketSize,    EInt,		ENotNull,   KCDTypeNameUpLinkAveragePacketSize},
     	{ KCDTIdDownLinkMaximumPacketSize,  EInt,		ENotNull,   KCDTypeNameDownLinkMaximumPacketSize},
     	{ KCDTIdUpLinkMaximumPacketSize,    EInt,		ENotNull,   KCDTypeNameUpLinkMaximumPacketSize},
     	{ KCDTIdDownLinkDelay,     			EInt,		ENotNull,   KCDTypeNameDownLinkDelay},
     	{ KCDTIdUpLinkDelay,     			EInt,		ENotNull,   KCDTypeNameUpLinkDelay},
     	{ KCDTIdDownLinkDelayVariation,     EInt,		ENotNull,   KCDTypeNameDownLinkDelayVariation},
     	{ KCDTIdUpLinkDelayVariation,     	EInt,		ENotNull,   KCDTypeNameUpLinkDelayVariation},
     	{ KCDTIdDownLinkPriority,     		EInt,		ENotNull,   KCDTypeNameDownLinkPriority},
     	{ KCDTIdUpLinkPriority,     		EInt,		ENotNull,   KCDTypeNameUpLinkPriority},
     	{ KCDTIdHeaderMode, 				EBool,	    ENotNull,   KCDTypeNameHeaderMode },
 //    	{ KCDTIdQosName,         			EText,		ENotNull,   KCDTypeNameQosName },
 		{ 0, 0, ENoAttrs, KCDNull }
 	};	
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

    //
    // Lookup table to associate table name with Table Id
    //  

	const SRecordTypeInfo CommsDatSchemaV1_1::iWifiScanEngineRecordInfo[] = {
   		{ KCDTIdRecordTag,	        		EInt,		ENoAttrs,   KCDTypeNameRecordTag },
		{ KCDTIdRecordName,	        		EText,		ENotNull,   KCDTypeNameRecordName },
		{ KCDTIdScanPeriodMs,  				EInt,		ENotNull,	KCDTypeNameScanPeriodMs },
		{ KCDTIdRSSIMin,					EInt,		ENotNull,	KCDTypeNameRSSIMin },
		{ KCDTIdRSSIMax,					EInt,		ENotNull,	KCDTypeNameRSSIMax },
		{ 0, 0, ENoAttrs, KCDNull }
    };

    const STableLookup CommsDatSchemaV1_1::iTableLookup[] = {
	{KCDTIdConnectionPrefsRecord,				KCDTypeNameConnectionPrefs,				CommsDatSchemaV1_1::iConnectionPrefsRecordInfo },
	{KCDTIdNetworkRecord,						KCDTypeNameNetwork,						CommsDatSchemaV1_1::iNetworkRecordInfo },
	{KCDTIdIAPRecord,							KCDTypeNameIAP,							CommsDatSchemaV1_1::iIAPRecordInfo },
	{KCDTIdWAPAccessPointRecord,				KCDTypeNameWAPAccessPoint,				CommsDatSchemaV1_1::iWAPAccessPointRecordInfo },
	{KCDTIdDialOutISPRecord,					KCDTypeNameDialOutISP,					CommsDatSchemaV1_1::iDialOutISPRecordInfo },
	{KCDTIdDialInISPRecord,						KCDTypeNameDialInISP,					CommsDatSchemaV1_1::iDialInISPRecordInfo },
	{KCDTIdLANServiceRecord,					KCDTypeNameLANService,					CommsDatSchemaV1_1::iLANServiceRecordInfo },
	{KCDTIdWLANServiceExtRecord,				KCDTypeNameWLANServiceExt,				CommsDatSchemaV1_1::iWLANServiceExtRecordInfo },
	{KCDTIdVPNServiceRecord,					KCDTypeNameVPNService,					CommsDatSchemaV1_1::iVPNServiceRecordInfo },
	{KCDTIdWCDMAPacketServiceRecord,			KCDTypeNameWCDMAPacketService,			CommsDatSchemaV1_1::iWCDMAPacketServiceRecordInfo },
	{KCDTIdOutgoingGprsRecord,					KCDTypeNameOutgoingWCDMA,				CommsDatSchemaV1_1::iWCDMAPacketServiceRecordInfo },
	{KCDTIdIncomingGprsRecord,					KCDTypeNameIncomingWCDMA,				CommsDatSchemaV1_1::iWCDMAPacketServiceRecordInfo },
	{KCDTIdDefaultWCDMARecord,					KCDTypeNameDefaultWCDMA,				CommsDatSchemaV1_1::iDefaultWCDMARecordInfo },
	{KCDTIdModemBearerRecord,					KCDTypeNameModemBearer,					CommsDatSchemaV1_1::iModemBearerRecordInfo },
	{KCDTIdLANBearerRecord,						KCDTypeNameLANBearer,					CommsDatSchemaV1_1::iLANBearerRecordInfo },
	{KCDTIdVirtualBearerRecord,					KCDTypeNameVirtualBearer,				CommsDatSchemaV1_1::iVirtualBearerRecordInfo },
	{KCDTIdWAPSMSBearerRecord,					KCDTypeNameWAPSMSBearer,				CommsDatSchemaV1_1::iWAPSMSBearerRecordInfo },
	{KCDTIdWAPIPBearerRecord,					KCDTypeNameWAPIPBearer,					CommsDatSchemaV1_1::iWAPIPBearerRecordInfo },
	{KCDTIdChargecardRecord,					KCDTypeNameChargecard,					CommsDatSchemaV1_1::iChargecardRecordInfo },
	{KCDTIdProxiesRecord,						KCDTypeNameProxies,						CommsDatSchemaV1_1::iProxiesRecordInfo },
	{KCDTIdSSProtoRecord,						KCDTypeNameSSProto,						CommsDatSchemaV1_1::iSecureSocketRecordInfo},
	{KCDTIdAgentLookupRecord,					KCDTypeNameAgentLookup,					CommsDatSchemaV1_1::iAgentLookupRecordInfo },
	{KCDTIdPANServiceExtRecord,					KCDTypeNamePANServiceExt,				CommsDatSchemaV1_1::iPANServiceExtRecordInfo },
	{KCDTIdGlobalSettingsRecord,				KCDTypeNameGlobalSettings,				CommsDatSchemaV1_1::iGlobalSettingsRecordInfo },
	{KCDTIdLocationRecord,						KCDTypeNameLocation,					CommsDatSchemaV1_1::iLocationRecordInfo },
    {KCDTIdAccessPointRecord,	        	    KCDTypeNameAccessPoint,					CommsDatSchemaV1_1::iAccessPointRecordInfo },
    {KCDTIdIapPrioritySelectionPolicyRecord,    KCDTypeNameIapPrioritySelectionPolicy,	CommsDatSchemaV1_1::iIAPPrioritySelectionPolicyRecordInfo },
    {KCDTIdUmtsR99QoSAndOnTableRecord,          KCDTypeNameUmtsR99QoSAndOnTable,		CommsDatSchemaV1_1::iUmtsR99QoSAndOnTableRecordInfo },
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    {KCDTIdTierRecord,        	                KCDTypeNameTierRecord,					CommsDatSchemaV1_1::iTierRecordInfo },
    {KCDTIdMCprRecord,        			        KCDTypeNameMCprRecord,					CommsDatSchemaV1_1::iMCprRecordInfo },
    {KCDTIdCprRecord,        	                KCDTypeNameCprRecord,					CommsDatSchemaV1_1::iCprRecordInfo },
    {KCDTIdSCprRecord,                          KCDTypeNameSCprRecord,					CommsDatSchemaV1_1::iSCprRecordInfo },
    {KCDTIdProtocolRecord,                      KCDTypeNameProtocolRecord,				CommsDatSchemaV1_1::iProtocolRecordInfo },
    {KCDTIdBearerTypeRecord,                    KCDTypeNameBearerTypeRecord,			CommsDatSchemaV1_1::iBearerTypeRecordInfo },
    {KCDTIdConfigAccessPointRecord,             KCDTypeNameConfigAccessPointRecord,		CommsDatSchemaV1_1::iConfigAccessPointRecordInfo },
    {KCDTIdApPrioritySelectionPolicyRecord,     KCDTypeNameApPrioritySelectionPolicy,	CommsDatSchemaV1_1::iAPPrioritySelectionPolicyRecordInfo },
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    {KCDTIdEapSimProtocolRecord,                KCDTypeNameEapSimProtocol,				CommsDatSchemaV1_1::iEAPSIMRecordInfo },
    {KCDTIdEapAkaProtocolRecord,                KCDTypeNameEapAkaProtocol,				CommsDatSchemaV1_1::iEAPAKARecordInfo },
	{KCDTIdEAPSecRecord,						KCDTTypeNameEAPSec,			            CommsDatSchemaV1_1::iEAPSecRecordInfo },
	{KCDTIdTunEAPRecord,						KCDTTypeNameTunEAP,			            CommsDatSchemaV1_1::iTunEAPRecordInfo },
	{KCDTIdEAPTLSRecord,						KCDTTypeNameEAPTLS,			            CommsDatSchemaV1_1::iEAPTLSRecordInfo },
	{KCDTIdLEAPRecord,							KCDTTypeNameLEAP,			            CommsDatSchemaV1_1::iLEAPRecordInfo },
	{0,											NULL,			NULL } // stop marker
	};

/*****************************************************************************************
2/ Static functions to interpret element ids
**********************************************/


TBool CommsDatSchema::IsNode(TMDBElementId aElementId)
/*
[ C == 7f ]
[ R == ff ]
Identifies Node as opposed to Field
Understands new table, column or record request to mean Node too
*/
    {
    return ( (aElementId & KCDMaskShowRecordId) == KCDMaskShowRecordId ||
			 (aElementId & KCDMaskShowFieldType) == KCDMaskShowFieldType  );
    }

TBool CommsDatSchema::IsTable(TMDBElementId aElementId)
/*
[ 0 < T < ff ][ C == 7f ][ R == ff ]
*/
    {
    return ( (aElementId & KCDMaskShowColumnTypeAndRecordId) == KCDMaskShowColumnTypeAndRecordId &&
			 HasValidTableId(aElementId) );
    }


TBool CommsDatSchema::IsColumn(TMDBElementId aElementId)
/*
[ 0 < T <= 7f8 ][ 0 < C < ff][ R == ff]
*/
    {
    return ( (aElementId & KCDMaskShowRecordId) == KCDMaskShowRecordId &&
			 HasValidColumnId(aElementId) &&
			 HasValidTableId(aElementId) );
    }


TBool CommsDatSchema::IsRecord(TMDBElementId aElementId)
/*
[ 0 < T <= 7f8 ][ C == 7f ][ R < ff ]
*/
    {
    return ( (aElementId & KCDMaskShowFieldType) == KCDMaskShowFieldType &&
			 HasValidRecordId(aElementId) &&
			 HasValidTableId(aElementId) );
    }


TBool CommsDatSchema::IsGenericRecord(TMDBElementId aElementId)
/*
[ 5F8 < T <= 7f8 ][ C == ff][ R == ff ]
*/
    {
    const TMDBElementId tableType = (aElementId & KCDMaskShowRecordType);
    return ( ( tableType >= KCDInitialUDefRecordType || tableType == 0 ) ||
			 IsNewTableRequest(aElementId) ); // But don't support new table request currently
    }


TBool CommsDatSchema::IsTemplate(TMDBElementId aElementId)
/*
0 record id in real table
[ 0 < T < 7f8 ][ 0 < C < ff ][ R == 0 ]
*/
    {
    return ( (aElementId & KCDMaskShowRecordId) == KCDDefaultRecord &&
			 HasValidTableId(aElementId) );
    }


TBool CommsDatSchema::HasValidTableId(TMDBElementId aElementId)
	{
	const TMDBElementId recordType = (aElementId & KCDMaskShowRecordType);
	return ( recordType > 0  &&
		     recordType <= KCDMaskShowRecordType );
	}


TBool CommsDatSchema::HasValidColumnId(TMDBElementId aElementId)
/*
[ 0 <= C < ff ]
*/
	{
	const TMDBElementId fieldType = (aElementId & KCDMaskShowFieldType);
	return ( fieldType > 0  &&
			 fieldType < KCDMaskShowFieldType );
	}


TBool CommsDatSchema::HasValidRecordId(TMDBElementId aElementId)
/*
[ 0 <= R < ff ]
*/
	{
	return (aElementId & KCDMaskShowRecordId) < KCDMaskShowRecordId;
	}


TBool CommsDatSchema::IsNewTableRequest(TMDBElementId aElementId)
/*
<T==0><C==7f><R==ff>
*/
	{
	return (aElementId & KCDMaskHideAttrAndRes) == KCDNewTableRequest;
	}


TBool CommsDatSchema::IsNewColumnRequest(TMDBElementId aElementId)
/*
0xnn00ff01
[ 0 < T < ff ][ C == 0 ][ R == ff ][ LSB == 1 ]
*/	{
    return ( ( aElementId & ~( KCDMaskShowAttributes | KCDUtilityFlag ) ) ==
             ( KCDNewColumnRequest | (aElementId & KCDMaskShowFieldType) ) );
	}


TBool CommsDatSchema::IsNewRecordRequest(TMDBElementId aElementId)
/*
0x8n7f0001
[ U == 1 ][ 0 < T < ff ][ C == 7f ][ R == 00 ][ LSB == 1 ]
*/
	{
	return (aElementId & KCDMaskHideAttrAndRes) ==
              ( (KCDNewRecordRequest & KCDMaskHideAttrAndRes) | (aElementId & KCDMaskShowRecordType) );

	}


TBool CommsDatSchema::IsDeprecatedRecord(
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                                         TMDBElementId aElementId
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                                        )
/*
Doesn't matter what version of the data schema is in use.  If a 
record has been deprecated it is never stored in the database
*/
    {
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    switch (aElementId)
        {
        case KCDTIdIapPrioritySelectionPolicyRecord :
            {
            return ETrue;
            }
        }
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

    return EFalse;
    }




/*****************************************************************************************
* 3/ Functions to check validity of container being used to store or modify data
*/



const SRecordTypeInfo* CommsDatSchemaV1_1::GetRecordTypeInfo(TMDBElementId aId)
/*
Retrieve the schema for this table
Will Return NULL if someone looks up a table that doesn'exist
@internalComponent
*/
    {
    const TMDBElementId recordTypeId = aId & KCDMaskShowRecordType;

    const STableLookup* tableInfoPtr = iTableLookup;

    while( tableInfoPtr && tableInfoPtr->iTableId != 0)
       {
        if (tableInfoPtr->iTableId == recordTypeId)
            {
            return tableInfoPtr->iFields;
            }
        
        ++tableInfoPtr;
        }

	__FLOG_STATIC1(KLogComponent, KCDErrLog, _L("CommsDatSchema::GetRecordTypeInfo failed to find type info for Element %08x"), aId);

	// CHECK - Don't leave or panic as client may have meant to search for generic record?

	ASSERT(0);

	return NULL;
	}


EXPORT_C TInt CommsDatSchemaV1_1::GetFieldTypeInfoL(TMDBElementId aId) 
/*
Return the value type of a field that is known to be in this record
@internalcomponent
*/
    {
    const SRecordTypeInfo* recordInfo = GetRecordTypeInfo(aId);
   
    if (recordInfo)
        {
        while( recordInfo->iTypeId != 0)
            {
            TMDBElementId temp = aId & KCDMaskShowFieldType;
            if ((recordInfo->iTypeId & KCDMaskShowFieldType) == temp)//(aId & KCDMaskShowFieldType))
               {
               return recordInfo->iValType; // return the full val type info here (not just the basic type)
               }
            ++recordInfo;
            }
        }
          
    User::Leave(KErrNotFound);
    
    return KErrNone; // never get here
    }

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
EXPORT_C const STableLookup* CommsDatSchemaV1_1::GetTableInfoL(TMDBElementId aId)
/*
Return a full tableinfo based on a tableId
@internalcomponent
*/
    {
    TInt tableID = aId & KCDMaskShowRecordType;
    
    for (TInt i = 0; 
         ( 0 != CommsDat::CommsDatSchemaV1_1::iTableLookup[i].iTableId );
         ++i)
        {
        if ( tableID == CommsDat::CommsDatSchemaV1_1::iTableLookup[i].iTableId )
            {
            //OK, got the tableId
            return &CommsDat::CommsDatSchemaV1_1::iTableLookup[i];
            }
        }
    
    User::Leave(KErrNotFound);
    
    //this is dummy return statement here - never reached
    return NULL;
    }
#endif

TBool CommsDatSchemaV1_1::IsEnabledForcedLinkedTypeResolving(TMDBElementId aId)
    {
    /**
    Forced link resolving is enabled for the:
    - AccessPointRecord
    - IAPRecord
    - ProxiesRecord
    - LANServiceRecord
    */
    return ( KCDTIdAccessPointRecord == aId );
    }
    
    
TMDBElementId CommsDatSchemaV1_1::ForcedFindLinkedIdL(TMDBElementId aTableId, TMDBElementId aLinkingFieldId)
    {
    const SRecordTypeInfo* recordInfo = GetRecordTypeInfo(aTableId);

    if (recordInfo)
    {
    while( recordInfo->iTypeId != 0)
        {
        if (recordInfo->iTypeId == aLinkingFieldId)
           {
           return recordInfo->iValType;
           }
        ++recordInfo;
        }
    }
          
    User::Leave(KErrNotFound);
    
    return 0; // never get here
    }


TMDBElementId CommsDatSchemaV1_1::LookupTableId(TDesC& tableName)
    {
    const STableLookup* tableInfoPtr = iTableLookup;
   
    while( tableInfoPtr && tableInfoPtr->iTableId != 0)
        {
        TPtrC tableNameFromId(tableInfoPtr->iTableName);

        if (tableNameFromId.CompareF(tableName) == 0)
            {
            return tableInfoPtr->iTableId;
            }
            
        ++tableInfoPtr;
        }
    
    return 0;     
    }



//
//  CommsDatSchema Functions
//

TMDBElementId CommsDatSchema::GetLegacyLinkTableIdL(CMDBSession& aSession, TMDBElementId aLinkingFieldId, CMDBElement* aLinkingRecord)
    {
    // for some legacy fields validate that the link is correct wrt another field

    CMDBField<TDesC>* tableNameField = NULL;
    TInt tmp = (aLinkingFieldId & KCDMaskShowType);
    
    switch (tmp)
        {
        case KCDTIdIAPService :
            {         
            if (aLinkingRecord)
                {
                CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(aLinkingRecord);
                tableNameField = &iapRecord->iServiceType;
                if (! tableNameField->IsNull())
                    {
                    return CommsDatSchemaV1_1::LookupTableId(tableNameField->GetL());
                    }
                }
            // create new field if linkingRecord is NULL or has no data in the qualifying field
            tableNameField = new (ELeave) CMDBField<TDesC>(KCDTIdIAPServiceType);
            break;
            }
        case KCDTIdIAPBearer :
            {
            if (aLinkingRecord)
                {
                tableNameField = &(static_cast<CCDIAPRecord&>(*aLinkingRecord)).iBearerType;
                if (! tableNameField->IsNull())
                    {
                    return CommsDatSchemaV1_1::LookupTableId(tableNameField->GetL());
                    }
                }
           
            // create new field if linkingRecord is NULL or has no data in the qualifying field
            tableNameField = new (ELeave) CMDBField<TDesC>(KCDTIdIAPBearerType);
            break;
            }
        case KCDTIdISP :
            {
            if (aLinkingRecord)
                {
                tableNameField = &(static_cast<CCDProxiesRecord&>(*aLinkingRecord)).iServiceType;
                if (! tableNameField->IsNull())
                    {
                    return CommsDatSchemaV1_1::LookupTableId(tableNameField->GetL());
                    }
                }
           
            // create new field if linkingRecord is NULL or has no data in the qualifying field
            tableNameField = new (ELeave) CMDBField<TDesC>(KCDTIdProxyServiceType);
            break;
            }
        case KCDTIdLANServiceExtensionTableRecordId :
            {
            if (aLinkingRecord)
                {
                tableNameField = &(static_cast<CCDLANServiceRecord&>(*aLinkingRecord)).iServiceExtensionTableName;
                if (! tableNameField->IsNull())
                    {
                    return CommsDatSchemaV1_1::LookupTableId(tableNameField->GetL());
                    }
                }

            // create new field if linkingRecord is NULL or has no data in the qualifying field            
            tableNameField = new (ELeave) CMDBField<TDesC>(KCDTIdLANServiceExtensionTableName);
            break;
            }
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        case KCDTIdSelectionPolicy :
            {
            return KCDTIdApPrioritySelectionPolicyRecord;
            }
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        default :
            {
            //all other linking records should be identified by full element ids so
            __FLOG_STATIC1(KLogComponent, KCDErrLog, 
                _L("CommsDatSchema::GetLegacyLinkTableIdL() - Don't have algorithm to find table id for Field <%08x>"), aLinkingFieldId);

            return 0; // shouldn't be in this function in the first place
            }
        }


    CleanupStack::PushL(tableNameField);
    
    
    if (aLinkingFieldId & KCDMaskShowRecordId)
    	{ //if the linkingField has a valid reocrdID...
    	tableNameField->SetRecordId((aLinkingFieldId & KCDMaskShowRecordId) >> 8);
    
	    tableNameField->LoadL(aSession);
	            
	    if ( ! tableNameField->IsNull() )
	        {
	        tmp = CommsDatSchemaV1_1::LookupTableId(tableNameField->GetL());
	        }
    	}
    else
    	{
    	/* Notifying the caller code that the resolving of the legacy link field was not successful.
    	 * Some other actions need to be taken...
    	 */
    	
    	tmp = 0;
    	}
    
    CleanupStack::PopAndDestroy(tableNameField);
    
    return tmp;
    }


TMDBElementId CommsDatSchema::ConstructLinkFromLegacyValueL(CMDBSession& aSession, CMDBElement* aLegacyField, TInt aLegacyValue)
    {
    TMDBElementId linkedTableId = 0;
    TMDBElementId serviceFieldId = 0;
    TInt temp = aLegacyField->ElementId() & KCDMaskShowType;
        
    switch (temp)
    {
    case KCDTIdIAPService :
        {         
        serviceFieldId = KCDTIdIAPServiceType;
        break;
        }
    case KCDTIdIAPBearer :
        {
        serviceFieldId = KCDTIdIAPBearerType;
        break;
        }
    case KCDTIdISP :
        {
        serviceFieldId = KCDTIdProxyServiceType;
        break;
        }
    case KCDTIdLANServiceExtensionTableRecordId :
        {
        serviceFieldId = KCDTIdLANServiceExtensionTableName;
        break;
        }
    default :
        {
        //all other linking records should be identified by full element ids so
        __FLOG_STATIC1(KLogComponent, KCDErrLog, 
            _L("CommsDatSchema::GetLegacyLinkTableIdL() - Don't have algorithm to find table id for Field <%08x>"), aLegacyField);

        return 0; // shouldn't be in this function in the first place
        }
    }
    
    //create an empty filed for loading the serviceType field for the given service...
    CMDBField<TDesC>* serviceField = new(ELeave) CMDBField<TDesC>(serviceFieldId);
    CleanupStack::PushL(serviceField);
    
    serviceField->SetRecordId(aLegacyValue);
    
    serviceField->LoadL(aSession);
    
    linkedTableId = CommsDatSchemaV1_1::LookupTableId(serviceField->GetL());
    
    if (linkedTableId !=0)
        {
        linkedTableId |= (aLegacyValue << 8);
        }
    
    CleanupStack::PopAndDestroy(serviceField);
    
    return linkedTableId;
    }
        


TMDBElementId CommsDatSchema::IdTagResolverL(CMDBSession& aSession, TMDBElementId aLinkedTypeId, TMDBElementId aTargetValue)
    {
    TMDBElementId linkedElementId = 0;
    
    CMDBField<TUint32>* tagField = new (ELeave) CMDBField<TUint32>(aLinkedTypeId | KCDTIdRecordTag);
    CleanupStack::PushL(tagField);

    *tagField = (aTargetValue & ~KLinkableTag);

    if (tagField->FindL(aSession))
        {
        linkedElementId = (tagField->ElementId() & KCDMaskShowRecordTypeAndId);
        }
    else
        {        
        User::Leave(KErrArgument);
        }
    
    CleanupStack::PopAndDestroy(tagField);
    
    return linkedElementId;
    }

TMDBElementId CommsDatSchema::GetLinkIdL(CMDBSession& aSession, TMDBElementId aLinkingFieldId, TMDBElementId aLinkingFieldValue, TMDBElementId aLinkedTableId, CMDBElement* aLinkingRecord)
/**
    Get or calculate the id of the linked record if enough information is available

    @param  aSession the session to use if need to lookup in the database
    @param  The id of the table that is linked to.  This can be NULL, but must be given
            if the link type is a base class and link field value does not specify the table being linked to.

    @leave  KErrArgument if aKnownLinkType does not match current field value
    @leave  System wide error codes if database action fails unexpectedly for some reason.

    @pre    The linking field value must be set correctly for this operation to succeed.
            
    @post   The value of this field may be updated to reflect the correct location of the linked record in the database
            The caller will have enough information to create the linked record if they need to
                
	@internal component
    @prototype
*/
    {
    __FLOG_STATIC2(KLogComponent, KCDInfoLog, 
       	                        _L("CommsDatSchema::GetLinkIdL() - aLinkingFieldId: <%08x>, aLinkingFieldValue: <%08x>."), aLinkingFieldId, aLinkingFieldValue);
    
    TMDBElementId linkedElementId(0);
    TMDBElementId linkedTypeId(aLinkedTableId & KCDMaskShowRecordType);
    TMDBElementId targetValue(aLinkingFieldValue);


    if ( (linkedTypeId & KCDMaskShowRecordType) == 0 )
        {
        if ( (targetValue & KLinkableTag) != 0          ||   // Linkable tag is set
             (targetValue & KCDMaskShowRecordType) == 0   )  // target value does not contain table type info
            {
            // lookup the record type in the value of this field
            linkedTypeId = CommsDatSchemaV1_1::GetFieldTypeInfoL(aLinkingFieldId) & KCDMaskShowRecordType;
        
            if ((linkedTypeId & KCDMaskShowRecordType) == 0)
                {
                // This is the 3rd stage of the legacy link resolver - the BRUTE FORCE...
               linkedTypeId = ForcedLegacyLinkFindL(aSession, aLinkingFieldId, aLinkingFieldValue, aLinkingRecord);

               if ((linkedTypeId & KCDMaskShowRecordType) == 0)
            	   {
                  	// Can't establish the table id for this link. Link is not valid
                   __FLOG_STATIC2(KLogComponent, KCDErrLog, 
                		   _L("CommsDatSchema::GetLinkIdL() - Can't identify the table linked to by field <%08x>, value <%08x>.  Link not usable"), aLinkingFieldId, targetValue);

                   User::Leave(KErrArgument);
                 	}
                }
            }
        else 
            {
            linkedTypeId = (targetValue & KCDMaskShowRecordType);
            }
        }
    
    // At this point the target table is known

    if (KLinkableTag & targetValue)
        {
        // this is a link by tag so need to do a find for the iTag field in the linked table
        
        linkedElementId = IdTagResolverL(aSession, linkedTypeId, targetValue);
        
        }
    else if (targetValue <= KCDMaxRecords)
        {
        // targetValue is just a simple record ID not in elementId form
        //the record id is greater than 0. It's still not sure that the link is correct...
        linkedElementId = linkedTypeId | (targetValue << 8); 
        }
   	else 
        {
        // targetValue should be an element id
        if ( (targetValue & KCDMaskShowRecordType) == (linkedTypeId & KCDMaskShowRecordType) ||
             (linkedTypeId & KCDMaskShowRecordType) == 0 )
            {   
            //tableid and recordid both set explicitly in targetValue;
            linkedElementId = targetValue & KCDMaskShowRecordTypeAndId;
            }
        else if( (targetValue & KCDMaskShowRecordType) == 0 )
            {
            // hmmm quite unusual. linkValue does not specify a table but looks like 
            // an element id.  So just add in the table id from aTargetTableId 
            linkedElementId = (linkedTypeId | (targetValue & KCDMaskShowRecordId));
            }
        else
            {
            __FLOG_STATIC3(KLogComponent, KCDErrLog, 
                _L("TMDBRecordLinkVisitor GetLinkIdL() - value <%08x> given in link field <%08x> and type of link <%08x> do not match. Link not usable"), (targetValue & KCDMaskShowRecordType), aLinkingFieldId & KCDMaskHideAttrAndRes, linkedTypeId);
            
            User::Leave(KErrArgument);
            }      
        }

    __FLOG_STATIC3(KLogComponent, KCDInfoLog, 
    _L("TMDBRecordLinkVisitor GetLinkIdL() - resolved link field <%08x> with value <%08x> to link value <%08x>"), aLinkingFieldId & KCDMaskHideAttrAndRes, aLinkingFieldValue, linkedElementId);
 
    return linkedElementId;
    }

//We are here because the client entered a number (possibly record id) for a linked field. CommsDat has to resolve
//in this case the linked elementId.
TMDBElementId CommsDatSchema::ForcedLegacyLinkFindL(CMDBSession& aSession, TMDBElementId aLinkingFieldId, TMDBElementId aLinkingFieldValue, CMDBElement* aElement)
	{
	TMDBElementId tableId = aLinkingFieldId & KCDMaskShowRecordType;
	TMDBElementId linkedId = 0;
	switch (tableId)
		{
		case KCDTIdIAPRecord:
			{
			linkedId = ForcedIAPFindL(aSession, aLinkingFieldId, aLinkingFieldValue, aElement);
			__FLOG_STATIC0(KLogComponent, KCDInfoLog, 
			   	                        _L("CommsDatSchema::ForcedLegacyLinkFindL() - IAP"));
			break;
			}
		case KCDTIdProxiesRecord:
			{
			linkedId = ForcedProxiesFindL(aSession, aLinkingFieldId, aLinkingFieldValue, aElement);
			__FLOG_STATIC0(KLogComponent, KCDInfoLog, 
			   	                        _L("CommsDatSchema::ForcedLegacyLinkFindL() - Proxies"));
			break;
			}
		case KCDTIdLANServiceRecord:
			{
			linkedId = ForcedLanServiceFindL(aSession, aLinkingFieldId, aLinkingFieldValue, aElement);
			__FLOG_STATIC0(KLogComponent, KCDInfoLog, 
			   	                        _L("CommsDatSchema::ForcedLegacyLinkFindL() - LanService"));
			break;
			}
		default:
			__FLOG_STATIC2(KLogComponent, KCDErrLog, 
			                        _L("CommsDatSchema::ForcedLegacyLinkFindL() - Can't identify the table linked to by field <%08x>, value <%08x>.  Link not usable"), aLinkingFieldId, aLinkingFieldValue);

			User::Leave(KErrArgument);
			break;
		}
	return linkedId;
	}

TMDBElementId CommsDatSchema::ForcedLanServiceFindL(CMDBSession& aSession, TMDBElementId aLinkingFieldId, TMDBElementId aLinkingFieldValue, CMDBElement* aElement)
	{

	/*CMDBField<TDesC>* serviceExtension = NULL;
	TMDBElementId linkedTableId = 0;*/
	
	TBuf<50> serviceExtension;
	TMDBElementId linkedTableId = 0;
	TBool isRecord = EFalse;
	
	if ((NULL!=aElement) && (CommsDat::CommsDatSchema::IsRecord(aElement->ElementId())))
		{
		isRecord = ETrue;
		}
	
	if (0 != (aLinkingFieldId & KCDMaskShowRecordId))
		{
		//there is a record id set on the container. Let's try to resolve the link from this
		CCDLANServiceRecord* lanServiceRec = static_cast<CCDLANServiceRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
		CleanupStack::PushL(lanServiceRec);
		lanServiceRec->SetRecordId((aLinkingFieldId & KCDMaskShowRecordId) >> 8);
		lanServiceRec->LoadL(aSession);
		
		serviceExtension = lanServiceRec->iServiceExtensionTableName.GetL();
		
		linkedTableId = CommsDatSchemaV1_1::LookupTableId(serviceExtension);
		CleanupStack::PopAndDestroy(lanServiceRec);
		}
	else if (isRecord)
		{
		RArray<TUint32> ids;
		
		CleanupClosePushL(ids);
			    
        TUint32 mask = KCDMaskShowRecordType | KCDMaskShowFieldType | aSession.iMDBSessionImpl->GetReadAttributeMask() | KCDUtilityFlag;
        TUint32 id = aElement->ElementId() & KCDMaskHideAttrAndRes;
        
        aSession.iMDBSessionImpl->StorageL()->FindL(id, mask, ids);
		
		TBool found = EFalse;
		TInt i = 0;
		TUint fieldValue = 0;
		
		CCDLANServiceRecord* lanServiceRec = NULL;
		lanServiceRec = static_cast<CCDLANServiceRecord*>(aElement);
		
		TInt fieldExists = KErrNotFound;
		
		while (!found && i<ids.Count())	
			{
			fieldValue = 0;
			
			if ( (IsRecord(ids[i])) && (KCDTIdLANServiceRecord == (ids[i] & KCDMaskShowRecordType)) )
				{
				//check whether the lanserviceext field is filled/changed...
				if (KCDChangedFlag == (lanServiceRec->iServiceExtensionTableName.ElementId() & KCDChangedFlag))
					{
					//the given field has changed, let's check it
					TInt cleanID = ((ids[i] & KCDMaskShowRecordType) | KCDTIdLANServiceExtensionTableName) | (ids[i] & KCDMaskShowRecordId);
					CMDBField<TDesC>* lanServiceExtName = new(ELeave) CMDBField<TDesC>(cleanID);
					CleanupStack::PushL(lanServiceExtName);
					//This field is not mandatory so it's possible that the given record doesn't contain it...
					TRAP(fieldExists, lanServiceExtName->LoadL(aSession));
					if (KErrNone == fieldExists)
						{
					    TPtrC serviceTypeFromCurrRec(*lanServiceExtName);
					    TPtrC serviceTypeFromOrigRec(lanServiceRec->iServiceExtensionTableName);
					    if (serviceTypeFromCurrRec == serviceTypeFromOrigRec)
					        {
					        //ok, found the primed record
							cleanID = ((ids[i] & KCDMaskShowRecordType) | KCDTIdLANServiceExtensionTableRecordId) | (ids[i] & KCDMaskShowRecordId);
							CMDBRecordLink<CCDServiceRecordBase>* lanServiceExtRecId = new(ELeave) CMDBRecordLink<CCDServiceRecordBase>(cleanID);
							CleanupStack::PushL(lanServiceExtRecId);
							
							TRAP(fieldExists, lanServiceExtRecId->LoadL(aSession));
							if (KErrNone == fieldExists)
								{
								fieldValue = *lanServiceExtRecId;
								serviceExtension = (*lanServiceExtName);
								}
							
							CleanupStack::PopAndDestroy(lanServiceExtRecId);
					        }
						}
						CleanupStack::PopAndDestroy(lanServiceExtName);
					}
				else
					{
					TInt cleanID = ((ids[i] & KCDMaskShowRecordType) | KCDTIdLANServiceExtensionTableRecordId) | (ids[i] & KCDMaskShowRecordId);
					CMDBRecordLink<CCDServiceRecordBase>* lanServiceExtRecId = new(ELeave) CMDBRecordLink<CCDServiceRecordBase>(cleanID);
					CleanupStack::PushL(lanServiceExtRecId);
					
					TRAPD(fieldExists, lanServiceExtRecId->LoadL(aSession));
					if (KErrNone == fieldExists)
						{
						cleanID = ((ids[i] & KCDMaskShowRecordType) | KCDTIdLANServiceExtensionTableName) | (ids[i] & KCDMaskShowRecordId);
						CMDBField<TDesC>* lanServiceExtName = new(ELeave) CMDBField<TDesC>(cleanID);
						CleanupStack::PushL(lanServiceExtName);
						
						TRAP(fieldExists, lanServiceExtName->LoadL(aSession));
						if (KErrNone == fieldExists)
							{
							fieldValue = *lanServiceExtRecId;
							serviceExtension = (*lanServiceExtName);
							}
						
						CleanupStack::PopAndDestroy(lanServiceExtName);
						}
					
					CleanupStack::PopAndDestroy(lanServiceExtRecId);
					}
				
				//Note: the legacy link values are already in the expected decimal recordID format. So no need to shift
				//the value to left...
				if ( fieldValue == aLinkingFieldValue )
					{
				    linkedTableId = CommsDatSchemaV1_1::LookupTableId(serviceExtension);
					found = true;
					}
				++i;
				}
			}
		
		CleanupStack::PopAndDestroy(&ids);
		}
	
	__FLOG_STATIC1(KLogComponent, KCDInfoLog, 
	   	                        _L("CommsDatSchema::ForcedLanServiceFindL() - linkedTableId: <%08x>."), linkedTableId);
	
	return linkedTableId;
	}

TMDBElementId CommsDatSchema::ForcedProxiesFindL(CMDBSession& aSession, TMDBElementId aLinkingFieldId, TMDBElementId aLinkingFieldValue, CMDBElement* aElement)
	{
	TBuf<50> serviceType;
	TMDBElementId linkedTableId = 0;
	TBool isRecord = EFalse;
	
	if ((NULL!=aElement) && (CommsDat::CommsDatSchema::IsRecord(aElement->ElementId())))
		{
		isRecord = ETrue;
		}
	
	if (0 != (aLinkingFieldId & KCDMaskShowRecordId))
		{
		//there is a record id set on the container. Let's true to resolve the link from this
		CCDProxiesRecord* proxiesRec = static_cast<CCDProxiesRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
		CleanupStack::PushL(proxiesRec);
		proxiesRec->SetRecordId((aLinkingFieldId & KCDMaskShowRecordId) >> 8);
		proxiesRec->LoadL(aSession);
		
		serviceType = proxiesRec->iServiceType.GetL();
		
		linkedTableId = CommsDatSchemaV1_1::LookupTableId(serviceType);
		CleanupStack::PopAndDestroy(proxiesRec);		
		}
	else if (isRecord)
		{
		RArray<TUint32> ids;
		
		CleanupClosePushL(ids);
			    
        TUint32 mask = KCDMaskShowRecordType | KCDMaskShowFieldType | aSession.iMDBSessionImpl->GetReadAttributeMask() | KCDUtilityFlag;
        TUint32 id = aElement->ElementId() & KCDMaskHideAttrAndRes;
        
        aSession.iMDBSessionImpl->StorageL()->FindL(id, mask, ids);
		
		TBool found = EFalse;
		TInt i = 0;
		TUint fieldValue = 0;
		
		CCDProxiesRecord* proxiesRec = NULL;
		proxiesRec = static_cast<CCDProxiesRecord*>(aElement);
		
		while (!found && i<ids.Count())	
			{
			fieldValue = 0;
			
			if ( (IsRecord(ids[i])) && (KCDTIdProxiesRecord == (ids[i] & KCDMaskShowRecordType)) )
				{
				//check whether the proxyServiceType field is filled/changed...
				if (KCDChangedFlag == (proxiesRec->iServiceType.ElementId() & KCDChangedFlag))
					{
					//the given field has changed, let's check it
					TInt cleanID = ((ids[i] & KCDMaskShowRecordType) | KCDTIdProxyServiceType) | (ids[i] & KCDMaskShowRecordId);
					CMDBField<TDesC>* proxyServiceType = new(ELeave) CMDBField<TDesC>(cleanID);
					CleanupStack::PushL(proxyServiceType);
					proxyServiceType->LoadL(aSession);
					
				    TPtrC serviceTypeFromCurrRec(*proxyServiceType);
				    TPtrC serviceTypeFromOrigRec(proxiesRec->iServiceType);
				    if (serviceTypeFromCurrRec == serviceTypeFromOrigRec)
				        {
				        //ok, found the primed record
						cleanID = ((ids[i] & KCDMaskShowRecordType) | KCDTIdISP) | (ids[i] & KCDMaskShowRecordId);
						CMDBRecordLink<CCDServiceRecordBase>* proxyISP = new(ELeave) CMDBRecordLink<CCDServiceRecordBase>(cleanID);
						CleanupStack::PushL(proxyISP);
						proxyISP->LoadL(aSession);
						
						fieldValue = *proxyISP;
						serviceType = (*proxyServiceType);
						
						CleanupStack::PopAndDestroy(proxyISP);
				        }
				    
				    CleanupStack::PopAndDestroy(proxyServiceType);
					}
				else
					{
					//the IAPService/BearerType field is not filled which means that were searcing for the 1st IAP
					//record with the given service/bearer value.
					TInt cleanID = ((ids[i] & KCDMaskShowRecordType) | KCDTIdISP) | (ids[i] & KCDMaskShowRecordId);
					CMDBRecordLink<CCDServiceRecordBase>* proxyISP = new(ELeave) CMDBRecordLink<CCDServiceRecordBase>(cleanID);
					CleanupStack::PushL(proxyISP);
					
					cleanID = ((ids[i] & KCDMaskShowRecordType) | KCDTIdProxyServiceType) | (ids[i] & KCDMaskShowRecordId);
					CMDBField<TDesC>* proxyServiceType = new(ELeave) CMDBField<TDesC>(cleanID);
					CleanupStack::PushL(proxyServiceType);
					
					proxyServiceType->LoadL(aSession);
					
					proxyISP->LoadL(aSession);
					
					fieldValue = *proxyISP;
					
					serviceType = (*proxyServiceType);
					
					CleanupStack::PopAndDestroy(proxyServiceType);
					
					CleanupStack::PopAndDestroy(proxyISP);
					}
				}

				//Note: the legacy link values are already in the expected decimal recordID format. So no need to shift
				//the value to left...
				if ( fieldValue == aLinkingFieldValue )
					{
				    linkedTableId = CommsDatSchemaV1_1::LookupTableId(serviceType);
					found = true;
					}
			++i;
			}
		
		CleanupStack::PopAndDestroy(&ids);
		}
	
	__FLOG_STATIC1(KLogComponent, KCDInfoLog, 
	   	                        _L("CommsDatSchema::ForcedProxiesFindL() - linkedTableId: <%08x>."), linkedTableId);
	
	return linkedTableId;
	}

TMDBElementId CommsDatSchema::ForcedIAPFindL(CMDBSession& aSession, TMDBElementId aLinkingFieldId, TMDBElementId aLinkingFieldValue, CMDBElement* aElement)
	{
	//According to the schema definition the IAP service/bearer type cannot be longer than 50 character
	TBuf<50> serviceOrBearerType;
	TMDBElementId linkedTableId = 0;
	TMDBElementId tempField = 0;
	TBool isRecord = EFalse;
	
	if ((NULL!=aElement) && (CommsDat::CommsDatSchema::IsRecord(aElement->ElementId())))
		{
		isRecord = ETrue;
		}
	
	if (0 != (aLinkingFieldId & KCDMaskShowRecordId))
		{
		//there is a record id set on the container. Let's try to resolve the link from this
		CCDIAPRecord* iapRec = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
		CleanupStack::PushL(iapRec);
		iapRec->SetRecordId((aLinkingFieldId & KCDMaskShowRecordId) >> 8);
		iapRec->LoadL(aSession);
		
		tempField = aLinkingFieldId & (KCDMaskShowRecordType | KCDMaskShowFieldType);
			
		if (KCDTIdIAPService == tempField)
			{ //searcing for the service...
			//serviceOrBearerType.Set(iapRec->iServiceType.GetL()); 
			serviceOrBearerType = (iapRec->iServiceType.GetL());
			}
		else
			{ //searching for the bearer...
			serviceOrBearerType = (iapRec->iBearerType.GetL());
			}
		
		linkedTableId = CommsDatSchemaV1_1::LookupTableId(serviceOrBearerType);
		
		CleanupStack::PopAndDestroy(iapRec);
		}
	else if (isRecord)
		{
		RArray<TUint32> ids;
		CleanupClosePushL(ids);
			    
        TUint32 mask = KCDMaskShowRecordType | KCDMaskShowFieldType | aSession.iMDBSessionImpl->GetReadAttributeMask() | KCDUtilityFlag;
        TUint32 id = aElement->ElementId() & KCDMaskHideAttrAndRes;
        
        aSession.iMDBSessionImpl->StorageL()->FindL(id, mask, ids);
		
		TBool found = EFalse;
		TInt i = 0;
		TUint fieldValue = 0;
		
		CCDIAPRecord* iapRec = NULL;
		iapRec = static_cast<CCDIAPRecord*>(aElement);
		
		tempField = aLinkingFieldId & (KCDMaskShowRecordType | KCDMaskShowFieldType);
		
		TInt iapServiceOrBearerTypeId = 0;
		TInt iapServiceOrBearerId = 0;
		TInt origIapServiceOrBearerType = 0;
		TBuf<50> serviceOrbearerTypeFromOrigRec(iapRec->iServiceType);
		
		if (KCDTIdIAPService == tempField)
			{ //searcing for the service...
			iapServiceOrBearerTypeId = KCDTIdIAPServiceType;
			iapServiceOrBearerId = KCDTIdIAPService;
			origIapServiceOrBearerType = iapRec->iServiceType.ElementId();
			serviceOrbearerTypeFromOrigRec = iapRec->iServiceType;
			}
		else
			{ //searching for the bearer...
			iapServiceOrBearerTypeId = KCDTIdIAPBearerType;
			iapServiceOrBearerId = KCDTIdIAPBearer;
			origIapServiceOrBearerType = iapRec->iBearerType.ElementId();
			serviceOrbearerTypeFromOrigRec = iapRec->iBearerType;
			}
		
		while (!found && i<ids.Count())	
			{
			fieldValue = 0;
			
			if ( (IsRecord(ids[i])) && (KCDTIdIAPRecord == (ids[i] & KCDMaskShowRecordType)) )
				{
				//check whether the IAPService/BearerType field is filled/changed...
				if (KCDChangedFlag == (origIapServiceOrBearerType & KCDChangedFlag))
					{
					//the given field has changed, let's check it
					TInt cleanID = ((ids[i] & KCDMaskShowRecordType) | iapServiceOrBearerTypeId) | (ids[i] & KCDMaskShowRecordId);
					CMDBField<TDesC>* serviceOrbearerType = new(ELeave) CMDBField<TDesC>(cleanID);
					CleanupStack::PushL(serviceOrbearerType);
					serviceOrbearerType->LoadL(aSession);
					
				    TPtrC serviceTypeFromCurrRec(*serviceOrbearerType);
				    //TPtrC serviceTypeFromOrigRec(iapRec->iServiceType);
				    //if (serviceTypeFromCurrRec == serviceTypeFromOrigRec)
				    if (serviceTypeFromCurrRec == serviceOrbearerTypeFromOrigRec)
				        {
				        //ok, found the primed record
						cleanID = ((ids[i] & KCDMaskShowRecordType) | iapServiceOrBearerId) | (ids[i] & KCDMaskShowRecordId);
						CMDBRecordLink<CCDServiceRecordBase>* serviceOrbearer = new(ELeave) CMDBRecordLink<CCDServiceRecordBase>(cleanID);
						CleanupStack::PushL(serviceOrbearer);
						serviceOrbearer->LoadL(aSession);
						
						fieldValue = *serviceOrbearer;
						serviceOrBearerType = (*serviceOrbearerType);
						
						CleanupStack::PopAndDestroy(serviceOrbearer);
				        }
				    
				    CleanupStack::PopAndDestroy(serviceOrbearerType);
					}
				else
					{
					//the IAPService/BearerType field is not filled which means that were searcing for the 1st IAP
					//record with the given service/bearer value.
					TInt cleanID = ((ids[i] & KCDMaskShowRecordType) | iapServiceOrBearerId) | (ids[i] & KCDMaskShowRecordId);
					CMDBRecordLink<CCDServiceRecordBase>* serviceOrbearer = new(ELeave) CMDBRecordLink<CCDServiceRecordBase>(cleanID);
					CleanupStack::PushL(serviceOrbearer);
					
					cleanID = ((ids[i] & KCDMaskShowRecordType) | iapServiceOrBearerTypeId) | (ids[i] & KCDMaskShowRecordId);
					CMDBField<TDesC>* serviceOrbearerType = new(ELeave) CMDBField<TDesC>(cleanID);
					CleanupStack::PushL(serviceOrbearerType);
					
					serviceOrbearerType->LoadL(aSession);
					
					serviceOrbearer->LoadL(aSession);
					
					fieldValue = *serviceOrbearer;
					
					serviceOrBearerType = (*serviceOrbearerType);
					
					CleanupStack::PopAndDestroy(serviceOrbearerType);
					
					CleanupStack::PopAndDestroy(serviceOrbearer);
					}
					}

				//Note: the legacy link values are already in the expected decimal recordID format. So no need to shift
				//the value to left...
				if ( fieldValue == aLinkingFieldValue )
					{
				    linkedTableId = CommsDatSchemaV1_1::LookupTableId(serviceOrBearerType);
					found = true;
					}
			++i;
			}
		
		CleanupStack::PopAndDestroy(&ids);
		}
	
	__FLOG_STATIC1(KLogComponent, KCDInfoLog, 
	   	                        _L("CommsDatSchema::ForcedIAPFindL() - linkedTableId: <%08x>."), linkedTableId);
	
	return linkedTableId;
	}

void CommsDatSchema::ChangeLegacyLinkFieldValues(CMDBElement*& aLegacyField)
 	{
 	//only do the check if there is a legacy table...
     if( ( (aLegacyField->ElementId() & KCDMaskShowRecordType ) < KCDTIdAccessPointRecord    &&
           (aLegacyField->ElementId() & KCDMaskShowRecordType ) != KCDTIdWLANServiceExtRecord  ) )
     	{
     	//this is why we accept only fields in this function...
 		//CMDBNumFieldBase* field = static_cast<CMDBNumFieldBase*>(aLegacyField);
 		
 		CMDBField<TInt>* field = static_cast<CMDBField<TInt>* >(aLegacyField);
 		
 		TMDBElementId tableId = field->ElementId() & KCDMaskShowRecordType;
 		
 	    const STableLookup* tableInfoPtr = CommsDatSchemaV1_1::iTableLookup;
 	    TInt i = 0;
 	    
 	    while( tableInfoPtr && tableInfoPtr->iTableId != 0 && (tableInfoPtr->iTableId) != tableId )
 	        {
 	        ++i;        
 	        ++tableInfoPtr;
 	        }
 	        
 	    /* At this point we know that what is the table of the field.
 	       Now we should search for the field in the table and get the position of it.
 	       it is important because as the mapping here is 1:1 this position will be the 
 	       position of the mapped field either.
 	       */
 	    const CommsDat::SRecordTypeInfo* fieldInfoPtr = CommsDatSchemaV1_1::iTableLookup[i].iFields;
 	    TMDBElementId fieldId = field->ElementId() & KCDMaskShowType;
 	    TInt fieldPosition = 0;
 	    
 	    //Skip the tagId and the name fields...
 	    ++fieldInfoPtr;
 	    ++fieldInfoPtr;
 	    
 	    while( fieldInfoPtr && fieldInfoPtr->iTypeId != 0 && (fieldInfoPtr->iTypeId) != fieldId)
 		    {
 		    ++fieldPosition;        
 		    ++fieldInfoPtr;
 		    }
 	    
 	    if (fieldInfoPtr->iTypeId != 0)
 	    //The field is found...
 	    	{
 	    	if ( (fieldInfoPtr->iValType & ELink) == ELink )
 	    	//Found a link. Remember that this function is called if the input container is a field and
 	    	//the type of the field is integer... So if we are here this means that somebody tries to read 
 	    	//linked field with a CMDBField<TInt>. Conclusion is that this can be a legacy field where the
 	    	//value of the field needs to be chenged to contain only the record number...
 	    		{
 	    		//field->iValue = ((field->iValue & KCDMaskShowRecordId) >> 8);
 	    		*field = (*field & KCDMaskShowRecordId) >> 8;
 	    		}
 	    	}	    	
     	}
 	}


using namespace CommsDat;

void CommsDatSchema::ValidateFieldTypeL(CMDBSession& aSession, CMDBElement& aElement)
/* checks the validity of the container being used for this element id
 *
 	EText,          // 50 length
	EDesC8,         // binary
	EUint32,
	EInt,
	EBool,
	EMedText,       // 256 length
	ELongText,		// 1024 length
    ELink			// an integer field that is a link

  only looks up field types, not node types
 */
	{
	TMDBElementId elementId = aElement.ElementId();

	/*
	Don't try to validate nodes or user defined fields here  (CHECK - perhaps would be a good idea though??)
	*/
	if ( IsNode(elementId) || IsGenericRecord(elementId | KCDMaskShowColumnTypeAndRecordId) )
		{
		return;
		}

	// Lookup type of element in the database
	const SRecordTypeInfo* pRecordTypeInfo = CommsDatSchemaV1_1::GetRecordTypeInfo(elementId);

	while ( pRecordTypeInfo &&
		    (pRecordTypeInfo->iTypeId & KCDMaskShowFieldType) != (elementId & KCDMaskShowFieldType ) )
		{
		++pRecordTypeInfo;
		}

	if (pRecordTypeInfo == 0)
		{
		// CHECK - how about looking for generic record info too?
		__FLOG_STATIC1(KLogComponent, KCDErrLog, _L("MMetaDatabase::ValidateFieldTypeL failed for unexpectedly for Element <%08x> because the type info for this element was not found"), aElement.ElementId());                        
		User::Leave(KErrArgument);
		}

	TInt correctType = pRecordTypeInfo->iValType;

	// read the type of the element stored with NetMeta
	TInt containerType = aElement.Type();

	switch (containerType)
		{
		// note - cases ordered with most common types first
		case EInt :
			{
			switch (correctType & KCDShowBasicTypeInfo)
                {
                case EInt :
                case EUint32 :
                    {
                    // everything ok
                    return; 
                    }
                case EBool :
				    {
				    CMDBField<TInt>& boolField = static_cast<CMDBField<TInt>&>(aElement);
				    TInt value = boolField;

				    if ( value == FALSE || value == TRUE)
					    {
                        // everything ok
                        return;
                        }
           
                    break;
					}
                case ELink : 
                    {
                    CMDBNumFieldBase& numElement = static_cast<CMDBNumFieldBase&>(aElement);
                    numElement = GetLinkIdL(aSession, numElement.ElementId(), numElement.GetL(), 0/*, NULL*/);
                    
                    return; // everything ok
                    }
                }
            break;
            }
        case ELink : 
            {
            if ( (correctType & KCDShowBasicTypeInfo) == ELink )
                {
                CMDBNumFieldBase& numElement = static_cast<CMDBNumFieldBase&>(aElement);
                numElement = GetLinkIdL(aSession, numElement.ElementId(), numElement.GetL(), 0/*, NULL*/);
                return;
                }
            else
                {
                break;
                }
            }
		case EText :
			{
			CMDBField<TDesC>& textField = static_cast<CMDBField<TDesC>&>(aElement);
			TDesC value = textField;

			// Note - Checking the real length of the data not the max length in the container
			if (correctType == EText)
				{
				if ( value.Length() <= KMaxTextLength)
					{
					return;
					}
				}
			else if (correctType == EMedText)
				{
				if ( value.Length() <= KMaxMedTextLength)
					{
					return;
					}
				}
			else if (correctType == ELongText)
				{
				if ( value.Length() <= KMaxLongTextLength)
					{
					return;
					}
				}

			break; // The string in the container is too long to be correct or the correct type is not text
			}
		case EDesC8 :
			{
			if (correctType == EDesC8)
				{
				return;
				}
			break;
			}
		default :
			{
			// don't need to check any other types so just return
			// useful to log an error though as really should not get here
			__FLOG_STATIC3(KLogComponent, KCDErrLog, _L("CommsDatSchema::ValidateTypeL.  Unexpectedly, the container type is %d.  ElementId is %d.  Correct type is %d. No validation attempted."), containerType, elementId, correctType);

			ASSERT(0);

			return;
			}
		}


	// The types are not compatible so log and leave

    __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("CommsDatSchema::ValidateTypeL.  Container of type %d cannot be used to access element id %08x that has type %d"), containerType, elementId, correctType);

	User::Leave(KErrArgument);
	}



//EOF
