// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Multimode ETel API header file.
// Describes the MM ETel API - classes, methods and types.
// 
//
   
/**
 @file
 @publishedPartner
 @released
*/

#ifndef __ETELMM_H__
#define __ETELMM_H__

#include <e32base.h>
#include <s32mem.h>
#include <etel.h>
#include <etelmmcs.h>


/** 
Internal API/TSY delimiter used to pass the phonebook type
*/
#define PHBOOK_DELIMITER		_L("//")
/** The size of the phone book delimiter string.
@publishedPartner
 */
const TInt KSizeOfPhbookDelimiter=2;

/**
Names for Multimode ETel sub-sessions
*/
_LIT(KETelMeAdnPhoneBook,"S1");
_LIT(KETelMeDialledPhoneBook,"S2");
_LIT(KETelMeMissedPhoneBook,"S3");
_LIT(KETelMeReceivedPhoneBook,"S4");
_LIT(KETelCombinedAdnPhoneBook,"S5");
_LIT(KETelTaAdnPhoneBook,"S6");
_LIT(KETelIccAdnPhoneBook,"S7");
_LIT(KETelIccFdnPhoneBook,"S8");
_LIT(KETelIccSdnPhoneBook,"S9");
_LIT(KETelIccBdnPhoneBook,"S10");
_LIT(KETelIccLndPhoneBook,"S11");
_LIT(KETelIccVoiceMailBox,"S12");

_LIT(KETelMeSmsStore,"S13");
_LIT(KETelIccSmsStore,"S14");
_LIT(KETelCombinedSmsStore,"S15");

_LIT(KETelNamStore,"S16");
_LIT(KETelOwnNumberStore,"S17");
_LIT(KETelEmergencyNumberStore,"S18");

_LIT(KETelSmsMessaging,"S19");
_LIT(KETelBroadcastMessaging,"S20");
_LIT(KETelUssdMessaging,"S21");

_LIT(KETelConferenceCall,"S22");

_LIT(KETelIccMbdnPhoneBook, "S23");
_LIT(KETelIccMsisdnPhoneBook, "S24");
_LIT(KETelIccOciPhoneBook, "S25");
_LIT(KETelIccIciPhoneBook, "S26");
_LIT(KETelIccNamStore ,"S27");
_LIT(KETelIccInfoPhoneBook, "S28");

_LIT(KETelLocationServices,"S29");

// Types for Multimode ETel sub-sessions

_LIT(KEtelCdmaPhoneBookType,"T1");
_LIT(KEtelGsmPhoneBookType,"T2");
_LIT(KEtelUSimPhoneBookType,"T3");

// Types for SmartCardEap sub-sessions

/**
The prefix to all RMobileSmartCardEap sub-session names.
*/
_LIT(KETelSmartCardEapSession, "ScEap");
/**
The length of KETelSmartCardEapSession.  Used to obtain a const value
for this length at compile-time.
*/
#define SCEAP_SSN_LENGTH 5

/**
EAP-MD5 type identifier.  Value taken from section 5 of RFC 3748 Jun 2004.
This should be used as the EAP type identifier value of
RMobileSmartCardEap::Open() if standard EAP-MD5 challenges are to be used.

This value corresponds to one of the possible values returned in the type
list of TUiccApplicationEapInfoV6.

(Decimal value = 4)

@see RMobileSmartCardEap::Open()
@see RMobilePhone::TUiccApplicationEapInfoV6
*/
_LIT8(KETelSmartCardEapTypeMD5, "04");

/**
EAP-TLS type identifier.  Value taken from section 4.1 of RFC 2716 Oct 1999.
This should be used as the EAP type identifier value of
RMobileSmartCardEap::Open() if standard EAP-TLS is to be used.

This value corresponds to one of the possible values returned in the type
list of TUiccApplicationEapInfoV6.

(Decimal value = 13)

@see RMobileSmartCardEap::Open()
@see RMobilePhone::TUiccApplicationEapInfoV6
*/
_LIT8(KETelSmartCardEapTypeTLS, "0D");

/**
EAP-SIM type identifier.
This should be used as the EAP type identifier value of
RMobileSmartCardEap::Open() if standard EAP-SIM is to be used.

This value corresponds to one of the possible values returned in the type
list of TUiccApplicationEapInfoV6.

(Decimal value = 18)

@see RMobileSmartCardEap::Open()
@see RMobilePhone::TUiccApplicationEapInfoV6
*/
_LIT8(KETelSmartCardEapTypeSIM, "12");

/**
EAP-AKA type identifier.
This should be used as the EAP type identifier value of
RMobileSmartCardEap::Open() if standard EAP-AKA is to be used.

This value corresponds to one of the possible values returned in the type
list of TUiccApplicationEapInfoV6.

(Decimal value = 23)

@see RMobileSmartCardEap::Open()
@see RMobilePhone::TUiccApplicationEapInfoV6
*/
_LIT8(KETelSmartCardEapTypeAKA, "17");

/**
EAP-TTLS type identifier.
This should be used as the EAP type identifier value of
RMobileSmartCardEap::Open() if standard EAP-TTLS is to be used.

This value corresponds to one of the possible values returned in the type
list of TUiccApplicationEapInfoV6.

(Decimal value = 21)

@see RMobileSmartCardEap::Open()
@see RMobilePhone::TUiccApplicationEapInfoV6
*/
_LIT8(KETelSmartCardEapTypeTTLS, "15");

/***********************************************************************************/
//
//  Global Multimode constants and types
// 
/***********************************************************************************/

/**
Unique API identifier and Functional Unit constants
Modes: Common 
*/
enum TMultimodeETelV1Api
	{
	/** TSY supports the Multimode ETel v1.0 API.
	
	Modes: Common */
	KETelExtMultimodeV1=3000,  // 3000 is unique reference for Multimode Etel v1.0 API
	/** TSY supports the MobileSimAccess functional unit.
	
	Modes: Common */
	KETelFuncMobileIccAccess,
	/** TSY supports the MobileNetwork functional unit.
	
	Modes: Common */
	KETelFuncMobileNetwork,
	/** TSY supports the MobileIdentity functional unit.
	
	Modes: Common */
	KETelFuncMobileIdentity,
	/** TSY supports the MobilePower functional unit.
	
	Modes: Common */
	KETelFuncMobilePower,
	/** TSY supports the MobileSignal functional unit.
	
	Modes: GSM/WCDMA */
	KETelFuncMobileSignal,
	/** TSY supports the MobileIndicator functional unit.
	
	Modes: Common */
	KETelFuncMobileIndicator,
	/** TSY supports the MobileDTMF functional unit.
	
	Modes: Common */
	KETelFuncMobileDTMF,
	/** TSY supports the MobileUserNetworkAccess functional unit.
	
	Modes: Common */
	KETelFuncMobileUserNetworkAccess,
	/** TSY supports the MobileIdentityService functional unit.
	
	Modes: Common */
	KETelFuncMobileIdentityService,
	/** TSY supports the MobileCallForwarding functional unit.
	
	Modes: Common */
	KETelFuncMobileCallForwarding,
	/** TSY supports the MobileCallBarring functional unit.
	
	Modes: GSM/WCDMA */
	KETelFuncMobileCallBarring,
	/** TSY supports the MobileCallWaiting functional unit.
	
	Modes: Common */
	KETelFuncMobileCallWaiting,
	/** TSY supports the MobileCallCompletion functional unit. */
	KETelFuncMobileCallCompletion,
	/** TSY supports the MobileAlternatingCall functional unit.
	
	Modes: GSM/WCDMA */
	KETelFuncMobileAlternatingCall,
	/** TSY supports the MobileCost functional unit.
	
	Modes: GSM/WCDMA */
	KETelFuncMobileCost,
	/** TSY supports the MobileSecurity functional unit.
	
	Modes: Common */
	KETelFuncMobileSecurity,
	/** TSY supports the MobileAlternateLineService functional unit.
	
	Modes: GSM/WCDMA */
	KETelFuncMobileAlternateLineService,
	/** TSY supports the MobileMessageWaiting functional unit.
	
	Modes: Common */
	KETelFuncMobileMessageWaiting,
	/** TSY supports the MobileFixedDiallingNumbers functional unit.
	
	Modes: GSM/WCDMA */
	KETelFuncMobileFixedDiallingNumbers,
	/** TSY supports the MobileDataCall functional unit.
	
	Modes: Common */
	KETelFuncMobileDataCall,
	/** TSY supports the MobilePrivacy functional unit.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
	KETelFuncMobilePrivacy,
	/** TSY supports the MobileEmergencyCall functional unit.
	
	Modes: Common */
	KETelFuncMobileEmergencyCall,
	/** TSY supports the MobileSmsMessaging functional unit.
	
	Modes: Common */
	KETelFuncMobileSmsMessaging,
	/** TSY supports the MobileBroadcastMessaging functional unit.
	
	Modes: Common */
	KETelFuncMobileBroadcastMessaging,
	/** TSY supports the MobileUssdMessaging functional unit.
	
	Modes: GSM/WCDMA */
	KETelFuncMobileUssdMessaging,
	/** TSY supports the MobileConferenceCall functional unit.
	
	Modes: Common */
	KETelFuncMobileConferenceCall,
	/** TSY supports the MobilePhonebookStore functional unit.
	
	Modes: Common */
	KETelFuncMobilePhonebook,
	/** TSY supports the MobileSmsStore functional unit.
	
	Modes: Common */
	KETelFuncMobileSmsStore,
	/** TSY supports the MobileNamStore functional unit.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
	KETelFuncMobileNamStore,
	/** TSY supports the MobileONStore functional unit.
	
	Modes: Common */
	KETelFuncMobileOwnNumberStore,
	/** TSY supports the MobileENStore functional unit.
	
	Modes: Common */
	KETelFuncMobileEmergencyNumberStore,
	/** TSY supports the MobileMultimedia functional unit.
	
	Modes: GSM/WCDMA */
	KETelFuncMobileMulticall,
	/** TSY supports the MobileNextIncomingCall functional unit.
	
	Modes: GSM/WCDMA */
	KETelFuncMobileNextIncomingCall,
	/** TSY supports the MobileMultimediaCall functional unit.
	
	Modes: GSM/WCDMA */
	KETelFuncMobileMultimediaCall,
	/** TSY supports the MobileUserSignalling functional unit.
	
	Modes: GSM/WCDMA */
	KETelFuncMobileUserSignalling
	};

/** Unique API identifier and Functional Unit constants.
Modes: WCDMA 

*/
enum TMultimodeETelV2Api
	{
	/** TSY supports the Multimode ETel v2.0 API.
	
	Modes: WCDMA */
	KETelExtMultimodeV2=7000,  // 7000 is unique reference for Multimode Etel v2.0 API
	/** TSY supports the MobileMultimediaCallSettings functional unit.
	
	Modes: WCDMA */
	KEtelFuncMobileMultimediaCallSettings,
	/** TSY supports the MobileNetworkSecurity functional unit.
	
	Modes: WCDMA */
	KEtelFuncMobileNetworkSecurity,
	KEtelFuncMobileUSIMApplications
	};
	
/**
Enum to define the v3.0 API additions

*/
enum TMultimodeETelV3Api
	{
	/**
	Indicator that the TSY supports the Multimode ETel v3.0 API.
	*/
	KETelExtMultimodeV3 = 10000,	
	/**
	TSY supports the MobileMMSInformation functional unit.
	*/
	KEtelFuncMobileMMSInformation,       // 10001
	/**
	TSY supports the KEtelFuncMobileAirTime functional unit.
	*/
	KEtelFuncMobileAirTime,              // 10002
	/**
	TSY supports the KEtelFuncMobileAutoRedial functional unit.
	*/
	KEtelFuncMobileAutoRedial,           // 10003
	/**
	TSY supports the KEtelFuncMobilePersonalisation functional unit.
	*/
	KEtelFuncMobilePersonalisation,      // 10004
	/**
	TSY supports the KEtelMobileMailboxNumbers functional unit.
	*/
	KEtelFuncMobileMailboxNumbers,       // 10005
	/**
	TSY supports the Mobile APN Control List functional unit.
	*/
	KEtelFuncMobileAPNControlList        // 10006
	};

/**
Enum to define the v4.0 API additions

*/
enum TMultimodeETelV4Api
	{
	/**
	Indicator that the TSY supports the Multimode ETel v4.0 API.
	*/
	KETelExtMultimodeV4	= 15000
	};


/** Enum to define Release 5 enabling API and data structures
*/
enum TMultimodeEtelV5Api
	{
	/**
	Indicator that the TSY supports the Multimode ETel v5.0 API.
	*/	
	KEtelExtMultimodeV5=20000,
	/**
	Indicator that the TSY supports the IMS functional unit.
	*/	
	KEtelFuncMobileIMS,                  // 20001
	/**
	TSY supports the KEtelFuncMobileSmartCardApplications functional unit.
	*/
	KEtelFuncMobileSmartCardApplications // 20002
	};


/**
Enum to define the v6.0 API additions.
*/
enum TMultimodeEtelV6Api
	{
	/**
	Indicator that the TSY supports the Multimode ETel v6.0 API.
	*/	
	KEtelExtMultimodeV6=25000,
	/**
	Indicator that the TSY supports the EAP supporting functional unit.
	*/	
	KEtelSmartCardEap                    // 25001
	};
	
/**
Enum to define the v7.0 API additions.
*/
enum TMultimodeEtelV7Api
	{
	/**
	Indicator that the TSY supports the Multimode ETel v7.0 API.
	*/	
	KEtelExtMultimodeV7=30000,
	/**
	Indicator that the TSY supports the LocationServices supporting functional unit.
	*/	
	KEtelLocationControlServices                    // 30001
	};

/**
Enum to define the v8.0 API additions.
*/
enum TMultimodeEtelV8Api
	{
	/**
	Indicator that the TSY supports the Multimode ETel v8.0 API.
	*/	
	KEtelExtMultimodeV8=35000,
	/**
	Indicator that the TSY supports HSUPA.
	*/	
	KEtelHsupa,                   // 35001
	/**
	Indicator that the TSY supports GBA authentication functional unit
	*/
	KEtelFuncGenericBootstrapArchitecture,
	/**
	Indicator that the TSY supports MBMS authentication functional unit 
	*/
	KEtelFuncMBMS,
	/*
	Indicator that the TSY supports WLAN parameters on the USIM functional unit 
	*/
	KEtelFuncWLan
	};
			
/**
Enum to define the v9.0 API additions.
*/
enum TMultimodeEtelV9Api
	{
	/**
	Indicator that the TSY supports the Multimode ETel v9.0 API.
	*/	
	KEtelExtMultimodeV9=45000,
	/**
	Indicator that the TSY supports the Cell Information functional unit.
	*/	
	KEtelFuncCellInfo,                   // 45001
	};
		
/**
Enum to define the Etel 3rdParty V1 API additions

*/
enum TMultimodeETel3rdPartyV1Api 
	{
	/**
	Indicator that the TSY supports the Etel 3rd Party v1.0 API
	*/
	KETelExt3rdPartyV1 = 40000
	};

	
/*********************************************************/
//
// Phone based functionality (RMobilePhone)
// 
/*********************************************************/


class CMobilePhonePtrHolder;
class CMobilePhoneStoredNetworkList;
class CMobilePhoneStoredWlanSIDList;
class RMobileSmartCardEap;

class RMobilePhone : public RPhone
/**
Provides client access to mobile phone functionality provided by TSY.

Encapsulates access to a mobile phone. The functionality of RMobilePhone 
is divided into a number of functional units, for the purpose of easier documentation 
and navigation and selectable support by TSY.

Note: Some RMobilePhone function members must be supported by the TSY while 
others are part of optional "functional unit" and only need to be supported 
if the TSY supports that functional unit. When a functional unit is mandatory 
then the unit should at least support the Get...Caps() member function to 
indicate the member functions that are supported for this unit.
 
*/
	{
public:
	friend class CAsyncRetrievePhoneList;
	IMPORT_C RMobilePhone();

	// Global multimode types

	
	class TMultimodeType
	/**
	Base class for all the V1 parameter types defined within the API.
	
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TInt ExtensionId() const;
	protected:
		TMultimodeType();
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	protected:
		TInt iExtensionId;
		};
		
	/** A typedef'd packaged TMultimodeType for passing through a generic API 
	function member. */
	typedef TPckg<TMultimodeType> TMultimodeTypePckg;

	// Types used in RMobilePhone::TMobileAddress

/** Address types.

Modes: Common */
	enum TMobileTON
		{
	/** User or the network has no knowledge of the type of number.
	
	Modes: Common */
		EUnknownNumber,			// 0
	/** International number.
	
	Modes: Common */
		EInternationalNumber,	// 1
	/** National number.
	
	Modes: Common */
		ENationalNumber,		// 2
	/** Administration/service number specific to the serving network, e.g. used to 
	access an operator.
	
	Modes: Common */
		ENetworkSpecificNumber, // 3
	/** Subscriber number.
	
	Modes: Common */
		ESubscriberNumber,		// 4 - Also defined as "dedicated, short code" in GSM 04.08
	/** Alphanumeric number coded according to 3GPP TS 123 038 GSM 7-bit default alphabet.
	
	Modes: GSM/WCDMA */
		EAlphanumericNumber,	// 5
	/** Abbreviated number.
	
	Modes: Common */
		EAbbreviatedNumber		// 6
		};

/** Number Plan Indicator.

Modes: Common */
	enum TMobileNPI
		{
	/** User or the network has no knowledge of the numbering plan.
	
	Modes: Common */
		EUnknownNumberingPlan =0,
	/** ISDN/telephony numbering plan.
	
	Modes: Common */
		EIsdnNumberPlan=1,		
	/** Data numbering plan.

	Modes: Common */
		EDataNumberPlan=3,		
	/** Telex numbering plan.

	Modes: Common */
		ETelexNumberPlan=4,	
	/** Service centre specific plan used to indicate a numbering plan specific to external 
	Short Message entities attached to the SMSC. */
		EServiceCentreSpecificPlan1=5,
	/** Service centre specific plan used to indicate a numbering plan specific to external 
	Short Message entities attached to the SMSC.
	
	Modes: GSM/WCDMA */
		EServiceCentreSpecificPlan2=6,
	/** National numbering plan.
	
	Modes: GSM/WCDMA */
		ENationalNumberPlan=8,
	/** Private numbering plan.
	
	Modes: Common */
		EPrivateNumberPlan=9,
	/** ERMES numbering plan.
	
	Modes: GSM/WCDMA */
		EERMESNumberPlan=10
		};

	enum 
		{
		KMaxMobilePasswordSize=10,
		KMaxMobileNameSize=32,
		KMaxMobileTelNumberSize=100
		};

	struct TMMTableSettings
		{
		TUint32 iLocId;
		};

	typedef TPckg<TMMTableSettings> TMMTableSettingsPckg;

	

	class TMobileAddress
	/**
	Defines API abstraction of a mobile telephone number.
	
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileAddress();
			
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
			
	public:
		/** Type of number.
		
		@see TMobileTON */
		TMobileTON iTypeOfNumber;
		/** Number plan.
		
		@see TMobileNPI */
		TMobileNPI iNumberPlan;
		/** Telephone number. */
		TBuf<KMaxMobileTelNumberSize> iTelNumber;
		};

	// Mobile information location type

	/** Defines Location of service information.

	Modes: Common */
	enum TMobileInfoLocation
		{
		/** Retrieve the service information ONLY from the cache on the phone. Return KErrNotFound 
		if the cache does not exist or it is empty. */
		EInfoLocationCache,
		/** Retrieve the service information from the cache, but if this is empty or does 
		not exist then interrogate the network. */
		EInfoLocationCachePreferred,
		/** Retrieve the service information from the network (and refresh the cache if 
		there is one). */
		EInfoLocationNetwork
		};

	// Mobile call service type

	/** Applicability of request to a mobile service group.

	Modes: Common */
	enum TMobileService
		{
		/** The call service has not been specified.
		
		Modes: Common */
		EServiceUnspecified,
		/** The API request applies to voice call services.

		Modes: Common */
		EVoiceService,
		/** The API request applies to auxiliary voice call services.

		Modes: GSM/WCDMA */
		EAuxVoiceService,
		/** The API request applies to circuit switched data call services.
		
		Modes: Common */
		ECircuitDataService,
		/** The API request applies to packet data services.
	
		Modes: Common */
		EPacketDataService,
		/** The API request applies to fax call services.
	
		Modes: Common */
		EFaxService,
		/** The API request applies to short message services.
	
		Modes: Common */
		EShortMessageService,
		/** The API request applies to all mobile services.
	
		Modes: Common */
		EAllServices,
		/** All teleservices
	
		Modes: Common */
		EAllTele,
		/** Telephony
	
		Modes: Common */
		ETelephony,
		/** All data teleservices
	
		Modes: Common */
		EAllDataTele,
		/** Voice Broadcast Service (VBS) Bearer Service
	
		Modes: Common */
		EAllDataExSms,
		/** All teleservices except SMS
	
		Modes: Common */
		EAllTeleExcSms,
		/** All PLMN specific teleservices
	
		Modes: Common */
		EAllPlmnTele,
		/** PLMN specific teleservice 1
	
		Modes: Common */
		EPlmnTele1,
		/** PLMN specific teleservice 2
	
		Modes: Common */
		EPlmnTele2,
		/** PLMN specific teleservice 3
	
		Modes: Common */
		EPlmnTele3,
		/** PLMN specific teleservice 4
	
		Modes: Common */
		EPlmnTele4,
		/** PLMN specific teleservice 5

		Modes: Common */
		EPlmnTele5,
		/** PLMN specific teleservice 6

		Modes: Common */
		EPlmnTele6,
		/** PLMN specific teleservice 7
	
		Modes: Common */
		EPlmnTele7,
		/** PLMN specific teleservice 8
	
		Modes: Common */
		EPlmnTele8,
		/** PLMN specific teleservice 9
	
		Modes: Common */
		EPlmnTele9,
		/** PLMN specific teleservice 10
	
		Modes: Common */
		EPlmnTeleA,
		/** PLMN specific teleservice 11
	
		Modes: Common */
		EPlmnTeleB,
		/** PLMN specific teleservice 12
	
		Modes: Common */
		EPlmnTeleC,
		/** PLMN specific teleservice 13
	
		Modes: Common */
		EPlmnTeleD,
		/** PLMN specific teleservice 14
	
		Modes: Common */
		EPlmnTeleE,
		/** PLMN specific teleservice 15
	
		Modes: Common */
		EPlmnTeleF,
		/** All bearer services
		
		Modes: Common */
		EAllBearer,
		/** All async services
	
		Modes: Common */
		EAllAsync,
		/** All sync services
	
		Modes: Common */
		EAllSync,
		/** All data circuit sync
	
		Modes: Common */
		ESyncData,
		/** All data circuit async
	
		Modes: Common */
		EAsyncData,
		/** All packet data services
	
		Modes: Common */
		EPacketData,
		/** All pad access services

		Modes: Common */
		EPadAccess,
		/** All PLMN specific bearer services

		Modes: Common */
		EAllPlmnBearer,
		/** PLMN specific bearer service 1
	
		Modes: Common */
		EPlmnBearerServ1,
		/** PLMN specific bearer service 2
	
		Modes: Common */
		EPlmnBearerServ2,
		/** PLMN specific bearer service 3
	
		Modes: Common */
		EPlmnBearerServ3,
		/** PLMN specific bearer service 4
	
		Modes: Common */
		EPlmnBearerServ4,
		/** PLMN specific bearer service 5
	
		Modes: Common */
		EPlmnBearerServ5,
		/** PLMN specific bearer service 6
	
		Modes: Common */
		EPlmnBearerServ6,
		/** PLMN specific bearer service 7
	
		Modes: Common */
		EPlmnBearerServ7,
		/** PLMN specific bearer service 8
	
		Modes: Common */
		EPlmnBearerServ8,
		/** PLMN specific bearer service 9
	
		Modes: Common */
		EPlmnBearerServ9,
		/** PLMN specific bearer service 10
	
		Modes: Common */
		EPlmnBearerServA,
		/** PLMN specific bearer service 11
	
		Modes: Common */
		EPlmnBearerServB,
		/** PLMN specific bearer service 12
	
		Modes: Common */
		EPlmnBearerServC,
		/** PLMN specific bearer service 13
	
		Modes: Common */
		EPlmnBearerServD,
		/** PLMN specific bearer service 14
	
		Modes: Common */
		EPlmnBearerServE,
		/** PLMN specific bearer service 15
	
		Modes: Common */
		EPlmnBearerServF,
		/** Alternative tele services

		Modes: Common */
		EAltTele,
		/** Voice Group Call Service (VGCS)

		Modes: Common */
		EVoiceGroupCall,
		/** Voice Broadcast Service (VBS)

		Modes: Common */
		EVoiceBroadcast,
		/** All GPRS Bearer Services

		Modes: Common */
		EAllGprsBearer
		};

	// Mobile name type

	/** A typedef to hold the names of API sub-sessions. The names are coded as ASCII 
	characters. */
	typedef TBuf<KMaxMobileNameSize> TMobileName;

	// Mobile password type

	/** A typedef for the standard password used by mobile phones for secure access 
	to services (phone lock, SIM lock, call barring password). The password is 
	coded as Unicode characters. */
	typedef TBuf<KMaxMobilePasswordSize> TMobilePassword;

	// for use by client-side API code and TSY only

	struct TClientId
	/** This type is used within the 2-phase list retrieval classes. During both phases, 
	the client-side API code will pass down the TClientId so that TSY can use
	this information to match the first phase of the request to the second phase. */
		{
		/** The handle to the underlying RTelServer session. */
		TInt iSessionHandle;
		/** The handle to the sub-session to which this API request relates. */
		TInt iSubSessionHandle;
		};

	/** Phone air interface capabilities. */
	enum TMobilePhoneModeCaps
		{
		/** Phone can operate in GSM mode on 900/1800/1900 MHz bands. */
		KCapsGsmSupported=0x00000001,
		/** Phone can operate in GPRS mode on 900/1800/1900 MHz bands. */
		KCapsGprsSupported=0x00000002,
		/** Phone can operate in AMPS mode on 800MHz band. */
		KCapsAmpsSupported=0x00000004,
		/** 
		  Phone can operate in CDMA (IS-95) mode on 800/1900 MHz bands. 
		   @deprecated 9.5
		  */
		KCapsCdma95Supported=0x00000008,
		/** 
		 Phone can operate in CDMA (cdma2000) mode on 800/1900 MHz bands.
		  @deprecated 9.5
		 */
		KCapsCdma2000Supported=0x00000010,
		/** Phone can operate in W-CDMA (UTRA Frequency Division Duplex (FDD)) mode. */
		KCapsWcdmaSupported=0x00000020,
		/** Phone can operate in TDMA/CDMA (UTRA Time Division Duplex (TDD)) mode. */
		KCapsTdcdmaSupported=0x00000040,
		/** 
		Phone can access Smart Card functionality required for an EAP. 
		@deprecated 9.3
		*/
		KCapsEapSupported=0x00000080
		};

	/** Version number of the multimode ETel API. */
	 enum TMultimodeEtelAPIVersion
		{
		/** Multimode Etel API version 1. */
		 TMultimodeETelApiV1
		};

	 //
	 //  API/TSY internal type
	 //
	
	 struct TPhoneStoreNameAndIccType
	/** Structure that holds the phone store name and ICC type.

	Mode: Common */
		{
		/** This parameter is used to set the name of the storage. */
		TName iStoreName;
		/** This parameter is used to set the type of the ICC card. */
		TName iMode;
		};

	IMPORT_C TInt GetMultimodeAPIVersion(TInt& aVersion) const;

	IMPORT_C TInt GetMultimodeCaps(TUint32& aCaps) const;

	IMPORT_C void GetPhoneStoreInfo(TRequestStatus& aReqStatus, TDes8& aInfo, const TDesC& aStoreName) const;

	IMPORT_C void GetPhoneStoreInfo(TRequestStatus& aStatus, TDes8& aInfo, const TDesC& aStoreName, const TDesC& aMode) const;

	// explicit phone initialisation
	IMPORT_C void InitialiseMM(TRequestStatus& aReqStatus, TDes8& aTableSettings) const; 

	/***********************************************************************************/
	//
	// MobilePhoneIccAccess functional unit
	//
	/***********************************************************************************/
	
/** ICC access capabilities.

Modes: GSM */
	enum TMobilePhoneIccCaps
		{
	/** Phone has a SIM and it currently supports SIM access by clients.
	
	Modes: GSM */
		KCapsSimAccessSupported=0x00000001,
	/** Phone has a R-UIM and it currently supports R-UIM access by clients.
	
	Modes: CDMA 
	 @deprecated 9.5
	*/
		KCapsRUimAccessSupported=0x00000002,
	/** Phone has a USIM and it currently supports USIM access by clients.
	
	Modes: WCDMA */
		KCapsUSimAccessSupported=0x00000004
		};
		

	enum 
	{
	/**
	Maximum size of an ICC identity. 
	
	NOTE: TS102.221 defines the maximum size of the ICC Identity as held in EF_ICCID as 10 bytes, however
	TS 51.011 mentions that network operators may issue a SIM with an identification number of 20 bytes.  
	Therefore to cover this possibility the higher figure of 20 bytes has been chosen as the maximum size of the ICC identity.
	*/
	KMaxSizeIccIdentity	= 20
	};
	
	/** A typedef to hold the ICC identity.*/
	typedef TBuf8<KMaxSizeIccIdentity> TIccIdentity; 

	IMPORT_C TInt GetIccAccessCaps(TUint32& aCaps) const;
	IMPORT_C void NotifyIccAccessCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const;
	IMPORT_C void GetIccIdentity(TRequestStatus& aReqStatus, TIccIdentity& aIdentity) const;

/** The Call Transfer/Call Forward options that are supported by the CSP.

Modes: WCDMA/GSM */
	enum TCspCallOffering
		{
	/** Indicates that "Call Transfer" is supported by the CSP. */
		KCspCT=0x08,
	/** Indicates that "Call Forwarding on Not Reachable" is supported by the CSP. */
		KCspCFNRc=0x10,
	/** Indicates that "Call Forwarding on No Reply" is supported by the CSP. */
		KCspCFNRy=0x20,
	/** Indicates that "Call Forwarding on Busy" is supported by the CSP. */
		KCspCFB=0x40,
	/** Indicates that "Call Forwarding Unconditional" is supported by the CSP. */
		KCspCFU=0x80
		};

/** The call restrictions supported by the CSP.

Modes: GSM/WCDMA */
	enum TCspCallRestriction
		{
	/** Indicates that "Barring of Incoming Calls when roaming outside the home PLMN 
	country" is supported by CSP. */
		KCspBICRoam=0x08,
	/** Indicates that "Barring of All Incoming Calls" is supported by CSP. */
		KCspBAIC=0x10,
	/** Indicates that "Barring of Outgoing International Calls except those directed 
	to the home PLMN country" is supported by CSP. */
		KCspBOICexHC=0x20,
	/** Indicates that "Barring of Outgoing International Calls" is supported by CSP. */
		KCspBOIC=0x40,
	/** Indicates that "Barring of All Outgoing Calls" is supported by CSP. */
		KCspBOAC=0x80
		};

/** Other services supported by the CSP.

Modes: GSM/WCDMA */
	enum TCspOtherSuppServices
		{
	/** Indicates that the Outgoing Access option of the Closed User Group service is 
	supported by the CSP. */
		KCspCUGOA=0x08,
	/** Indicates that the Preferential CUG option of the Closed User Group service 
	is supported by the CSP. */
		KCspPrefCUG=0x10,
	/** Indicates that the Advice of Charge service is supported by the CSP. */
		KCspAoC=0x20,
	/** Indicates that the Closed User Group service is supported by the CSP. */
		KCspCUG=0x40,
	/** Indicates that the Multiparty call service is supported by the CSP */
		KCspMPTY=0x80,
		};

/** Call completion modes supported by this CSP.

Modes: GSM/WCDMA */
	enum TCspCallCompletion
		{
	/** Indicates that the Call Completion to Busy Subscriber service is supported by 
	the CSP. */
		KCspCCBS=0x20,
	/** Indicates that the Call Waiting service is supported by the CSP. */
		KCspCW=0x40,
	/** Indicates that the Call Hold service is supported by the CSP. */
		KCspHOLD=0x80,
		};

/** SMS Teleservices supported by the CSP.

Modes: GSM/WCDMA */
	enum TCspTeleservices
		{
	/** Indicates that display of Validity Period menu options for SMS is supported 
	by the CSP. */
		KCspValidityPeriod=0x02,
	/** Indicates that display of Protocol ID menu options for SMS is supported by the 
	CSP. */
		KCspProtocolID=0x04,
	/** Indicates that SMS Delivery Confirmation is supported by the CSP. */
		KCspDelConf=0x08,
	/** Indicates that display of Reply Path menu options for outgoing SMS is supported 
	by the CSP. */
		KCspReplyPath=0x10,
	/** Indicates that the Cell Broadcast service is supported by the CSP. */
		KCspSMCB=0x20,
	/** Indicates mobile originated Short Messages are supported by CSP. */
		KCspSMMO=0x40,
	/** Indicates mobile terminated Short Messages are supported by CSP. */
		KCspSMMT=0x80,
		};

/** Alternate Line Services */
	enum TCspCPHSTeleservices
		{
	/** Indicates that Alternate Line Service is supported by the CSP. */
		KCspALS=0x80
		};

/** Currently not used. */
	enum TCspCPHSFeatures
		{
	/** This bit value is reserved for SST in Phase 1 CPHS. */
		KCspReservedSST=0x80
		};

/** Number Identification option supported by the CSP.

Modes: GSM/WCDMA */
	enum TCspNumberIdentification
		{
	/** Indicates that Calling Line Identity Presentation blocking per call (CLI sent 
	by default) is supported by the CSP. */
		KCspCLIBlock=0x01,
	/** Indicates that Calling Line Identity Presentation per call (CLI blocked by default) 
	is supported by the CSP. */
		KCspCLISend=0x02,
	/** Indicates that Connected Line Identification Presentation is supported by the 
	CSP. */
		KCspCOLP=0x10,
	/** Indicates that Connected Line Identification Restriction is supported by the 
	CSP. */
		KCspCOLR=0x20,
	/** Indicates that Calling Line Identification Presentation is supported by the CSP. */
		KCspCLIP=0x80,
		};

/** Phase 2+ services supported by the CSP.

Modes: GSM/WCDMA */
	enum TCspPhase2PlusServices
		{
	/** Indicates that Multiple Band roaming is supported by CSP. */
		KCspMultipleband=0x04,
	/** Indicates that Multiple Subscriber Profile is supported by CSP.
	
	v1.0 of Multimode ETel API does not support this feature */
		KCspMSP=0x08,
	/** Indicates that Voice Broadcast is supported by CSP.
	
	v1.0 of Multimode ETel API does not support this feature. */
		KCspVoiceBroadcast=0x10,
	/** Indicates that Voice Group Call is supported by CSP. 
	
	v1.0 of Multimode ETel API does not support this feature. */
		KCspVoiceGroupCall=0x20,
	/** Indicates that High Speed Circuit Switched Data is supported by CSP. */
		KCspHscsd=0x40,
	/** Indicates that General Packet Radio Service is supported by CSP. */
		KCspGprs=0x80
		};

/** Value added services supported by this CSP.

Modes: GSM/WCDMA */
	enum TCspValueAdded
		{
	/** Indicates that display of language selection menus is supported by the CSP.
	
	v1.0 of Multimode ETel API does not support this feature. */
		KCspLanguage=0x01,
	/** Indicates that display of data call option menu is supported by the CSP. */
		KCspData=0x04,
	/** Indicates that display of fax call option menu is supported by the CSP. */
		KCspFax=0x08,
	/** Indicates that display of the menu option to send a Short Message of type Email 
	is supported by the CSP. */
		KCspSMMOEmail=0x10,
	/** Indicates that display of the menu option to send a Short Message of type Paging 
	is supported by the CSP. */
		KCspSMMOPaging=0x20,
	/** Indicates that display of the menu option for manual PLMN selection is supported 
	by the CSP. */
		KCspPLMNMode=0x80,
		};

	

	class TMobilePhoneCspFileV1 : public TMultimodeType
	/** Defines contents of the CSP (Customer Service Profile) on the SIM. The profile 
	consists of the sum of the respective enums.

	@see GetCustomerServiceProfile()
	@publishedPartner
	@released*/
		{
	public:
	/** Constructor. */
		IMPORT_C TMobilePhoneCspFileV1();

	/** Sum of the TCspCallOffering. */
		TUint8	iCallOfferingServices;
	/** Sum of the TCspCallRestriction. */
		TUint8	iCallRestrictionServices;
	/** Sum of the TCspOtherSuppServices. */
		TUint8	iOtherSuppServices;
	/** Sum of the TCspCallCompletion. */
		TUint8	iCallCompletionServices;
	/** Sum of the TCspTeleservices. */
		TUint8  iTeleservices;
	/** Sum of the TCspCPHSTeleservices. */
		TUint8	iCphsTeleservices;
	/** Sum of the TCspCPHSFeatures. */
		TUint8	iCphsFeatures;
	/** Sum of the TCspNumberIdentification. */
		TUint8	iNumberIdentServices;
	/** Sum of the TCspPhase2PlusServices. */
		TUint8	iPhase2PlusServices;
	/** Sum of the TCspValueAdded. */
		TUint8	iValueAddedServices;
		};

/** A typedef'd packaged TMobilePhoneCspFileV1 for passing through a generic API 
function member. */
	typedef TPckg<TMobilePhoneCspFileV1> TMobilePhoneCspFileV1Pckg;

	IMPORT_C void GetCustomerServiceProfile(TRequestStatus& aReqStatus, TDes8& aCsp) const;

/** SIM/USIM/CDMA Service Table.

Modes: Common */
	enum TSSTServices1To8
		{
	/** Indicates that disabling PIN1 is supported by the ICC. */
		KSstPin1Disable=0x01,
	/** Indicates that storage of Abbreviated Dialling Numbers within this ICC is supported. */
		KSstADN=0x02,
	/** Indicates that the Fixed Dialling Numbers service and the storage of FDN numbers
	within this ICC is supported. */
		KSstFDN=0x04,
	/** Indicates that the Short Message Service and the storage of short messages within 
	this ICC is supported.
	
	Modes: Common */
		KSstSMS=0x08,
	/** Indicates that the Advice of Charge service is supported by this ICC.
	
	Modes: GSM/WCDMA */
		KSstAoC=0x10,
	/** Indicates that the storage of Configuration Capability Parameters associated 
	with an AND, FDN, MSISDN, LND or SDN is supported by this ICC.
	
	Modes: GSM/WCDMA */
		KSstCCP=0x20,
	/** Indicates that storage of a preferred network list is supported by this ICC.
	
	v1.0 of Multimode ETel API does not support this feature
	
	Modes: GSM/WCDMA */
		KSstPLMNSelector=0x40
		};

/** SIM/USIM/CDMA Service Table (continued).

Modes: GSM/WCDMA */
	enum TSSTServices9To16
		{
	/** Indicates that storage of MSISDN (own numbers) numbers within this ICC is supported.
	
	Modes: GSM/WCDMA */
		KSstMSISDN=0x01,
	/** Indicates that storage of extended (greater than 20 digits) ADN, LND and MSISDN
	numbers within this ICC is supported.
	
	Modes: Common */
		KSstExt1=0x02,
	/** Indicates that storage of extended (greater than 20 digits) FDN numbers within 
	this ICC is supported.
	
	Modes: Common */
		KSstExt2=0x04,
	/** Indicates that storage of SMS parameter records within this ICC is supported.
	
	Modes: Common */
		KSstSMSP=0x08,
	/** Indicates that storage of Last Numbers Dialled within this ICC is supported.
	
	Modes: Common */
		KSstLND=0x10,
	/** Indicates that the storage of Cell Broadcast message identifiers is supported 
	within this ICC.
	
	Modes: GSM/WCDMA */
		KSstCBMI=0x20,
	/** Indicates that access to the GID1 file is supported within this ICC.
	
	v1.0 of Multimode ETel API does not support this feature
	
	Modes: GSM/WCDMA */
		KSstGID1=0x40,
	/** Indicates that access to the GID2 file is supported within this ICC.
	
	v1.0 of Multimode ETel API does not support this feature
	
	Modes: GSM/WCDMA */
		KSstGID2=0x80
		};
	
/** SIM/USIM/CDMA Service Table (continued).

v1.0 of Multimode ETel API does not support this feature

Modes: Common */
	enum TSSTServices17To24
		{
	/** Indicates that access to the Service Provider Name within ICC is supported.
	
	v1.0 of Multimode ETel API does not support this feature
	
	Modes: Common */
		KSstSPName=0x01,
	/** Indicates that storage of Service Dialling Numbers within ICC is supported.
	
	Modes: Common */
		KSstSDN=0x02,
	/** Indicates that storage of extended (greater than 20 digits) SDN numbers within 
	this ICC/RUIM is supported.
	
	Modes: Common */
		KSstExt3=0x04,
	/** Indicates that access to the VGCS group identifier list within ICC is supported.
	
	v1.0 of Multimode ETel API does not support this feature
	
	Modes: GSM/WCDMA */
		KSstVGCSList=0x10,
	/** Indicates that access to the VBS group identifier list within ICC is supported.
	
	v1.0 of Multimode ETel API does not support this feature

	Modes: GSM/WCDMA */
		KSstVBSList=0x20,
	/** Indicates that the enhanced Multi-Level Precedence and Pre-emption service is 
	supported by ICC
	
	v1.0 of Multimode ETel API does not support this feature
	
	Modes: GSM/WCDMA */
		KSsteMLPP=0x40,
	/** Indicates that automatic answer for eMLPP is supported by ICC
	
	v1.0 of Multimode ETel API does not support this feature
	
	Modes: GSM/WCDMA */
		KSstAnswereMLPP=0x80
		};

/** SIM/USIM/CDMA Service Table (continued).

Modes: GSM/WCDMA */
	enum TSSTServices25To32
		{
	/** Indicates that SAT SMS-CB data download is supported by ICC. */
		KSstSmsCbDataDownload=0x01,
	/** Indicates that SAT SMS-PP data download is supported by ICC. */
		KSstSmsPpDataDownload=0x02,
	/** Indicates that SAT menu selection is supported by ICC.
	
	SAT ETel API provides this feature. */
		KSstMenuSelection=0x04,
	/** Indicates that SAT call control is supported by ICC.
	
	SAT ETel API provides this feature. */
		KSstCallControl=0x08,
	/** Indicates that SAT pro-active SIM is supported by ICC.
	
	SAT ETel API provides this feature. */
		KSstProactiveSim=0x10,
	/** Indicates ICC storage of ranges of Cell Broadcast Message Identifiers is supported. */
		KSstCBMIRanges=0x20,
	/** Indicates ICC storage of Barred Dialling Numbers is supported. */
		KSstBDN=0x40,
	/** Indicates ICC storage of extended (greater than 20 digits) BDN numbers is supported. */
		KSstExt4=0x80
		};

/** SIM/USIM/CDMA Service Table (continued).

Modes: GSM/WCDMA */
	enum TSSTServices33To40
		{
	/** Indicates depersonalisation control keys are supported by ICC.
	
	v1.0 of Multimode ETel API does not support this feature. */
		KSstDepersonalisationKeys=0x01,
	/** Indicates co-operative network list are supported by this ICC.
	
	v1.0 of Multimode ETel API does not support this feature. */
		KSstCooperativeNetworks=0x02,
	/** Indicates Short Message Status Reports are supported by ICC. */
		KSstSMStatusReports=0x04,
	/** Indicates network's indication of alerting is supported by ICC.
	
	v1.0 of Multimode ETel API does not support this feature. */
		KSstNetworkIndAlerting=0x08,
	/** Indicates outgoing SM control by SAT is supported by ICC.
	
	SAT ETel API provides this feature. */
		KSstMoSmControlBySim=0x10,
	/** Indicates that GPRS is supported by this ICC.
	
	GPRS ETel API provides GPRS functionality. */
		KSstGprs=0x20,
	/** Indicates that Image files are supported by this ICC.
	
	SAT ETel API provides this feature. */
		KSstImage=0x40,
	/** Indicates that SoLSA in supported by this ICC.
	
	v1.0 of Multimode ETel API does not support this feature */
		KSstSoLSA=0x80
		};

/** SIM/USIM/CDMA Service Table (continued).

Modes: GSM/WCDMA */
	enum TSSTServices41To48
		{
	/** Indicates that USSD string data is supported in SAT Call Control by this ICC.
	
	SAT ETel API provides this feature. */
		KSstUssdStringInCallControl=0x01,
	/** Indicates that RUN AT COMMAND is supported in SAT by this ICC.
	
	v1.0 of Multimode ETel API does not support this feature */
		KSstRunATCommand=0x02,
	/** Indicates that PLMN selector list with access technology is supported by this 
	ICC.
	
	v1.0 of Multimode ETel API does not support this feature. */
		KSstPlmnSelectorListWithAccessTechnology=0x04,
	/** Indicates that OPLMN selector list with access technology is supported by this
	ICC.
	
	v1.0 of Multimode ETel API does not support this feature. */
		KSstOplmnSelectorListWithAccessTechnology=0x08,
	/** Indicates that HPLMN access technology is supported by this ICC.
	
	v1.0 of Multimode ETel API does not support this feature. */
		KSstHplmnAccessTechnology=0x10,
	/** Indicates that CPBCCH information is supported by this ICC.
	
	v1.0 of Multimode ETel API does not support this feature. */
		KSstCpbcchInformation=0x20,
	/** Indicates that Investigation Scan is supported by this ICC.
	
	v1.0 of Multimode ETel API does not support this feature. */
		KSstInvestigationScan=0x40,
	/** Indicates that extended capability configuration parameters are supported by 
	this ICC.
	
	v1.0 of Multimode ETel API does not support this feature. */
		KSstExtendedCcp=0x80
		};

/** SIM/USIM/CDMA Service Table (continued).

Modes: GSM/WCDMA */
	enum TSSTServices49To56
		{
	/** Indicates Mobile Execution Environment services are supported by this ICC */
		KSstMExE=0x01,
	/** Indicates that RPLMN last used access technology is supported by this ICC.
	
	v1.0 of Multimode ETel API does not support this feature. */
		KSstRplmnLastUsedAccessTechnology=0x02
		};

/** USIM service table. */
	enum TUSTServices1To8
		{
	/** Indicates that the local phonebook is supported by the USIM. Local phonebook 
	means a phonebook that is specific to a USIM Application, as opposed to the 
	global phonebook which is located at the DF Telecom level on the UICC and 
	not attached to a USIM Application. */
		KUstLocalPhBk=0x01,
	/** Indicates that the Fixed Dialling Numbers (FDN) service and the storage of 
	FDN numbers within this USIM is supported. */
		KUstFDN=0x02,
	/** Indicates that the Extension 2 is supported by the USIM. Extension 2 provides 
	extension data for Fixed Dialling Numbers. */
		KUstExt2=0x04,
	/** Indicates that the Service Dialling Numbers are supported by the USIM. */
		KUstSDN=0x08,
	/** Indicates that the Extension 3 is supported by the USIM. Extension 3 provides 
	extension data for Service Dialling Numbers. */
		KUstExt3=0x10,
	/** Indicates that the Barred Dialling Numbers are supported by the USIM. */
		KUstBDN=0x20,
	/** Indicates that the Extension 4 is supported by the USIM. Extension 4 provides 
	extension data for Barred Dialling Numbers. */
		KUstExt4=0x40,
	/** Indicates that the outgoing call information (OCI and OCT) are supported by 
	the USIM.
	
	Note: v2.0 of Multimode ETel API does not support this feature. */
		KUstOugoingCallInfo=0x80
		};
	
/** USIM service table (continued). */
	enum TUSTServices9To16
		{
	/** Indicates that the incoming call information (ICI and ICT) are supported by 
	the USIM
	
	Note: v2.0 of Multimode ETel API does not support this feature. */
		KUstIncomingCallInfo=0x01,
	/** Indicates that Short Message Storage is supported by the USIM. */
		KUstSMS=0x02,
	/** Indicates that Short Message Status Reports are supported by the USIM. */
		KUstSMSR=0x04,
	/** Indicates that Short Message Service Parameters are supported by the USIM. */
		KUstSMSP=0x08,
	/** Indicates that Advice Of Charge is supported by the USIM. */
		KUstAoC=0x10,
	/** Indicates that Capability Configuration Parameters are supported by the USIM. */
		KUstCCP=0x20,
	/** Indicates that Cell Broadcast Message Identifier is supported by the USIM. */
		KUstCBMI=0x40,
	/** Indicates that Cell Broadcast Message Identifier Ranges are supported by the 
	USIM. */
		KUstCBMIRanges=0x80
		};

/** USIM service table (continued). */
	enum TUSTServices17To24
		{
	/** Indicates that Group Identifier Level 1 is supported by the USIM.
	
	Note: v2.0 of Multimode ETel API does not support this feature. */
		KUstGID1=0x01,
	/** Indicates that Group Identifier Level 2 is supported by the USIM.
	
	Note: v2.0 of Multimode ETel API does not support this feature. */
		KUstGID2=0x02,
	/** Indicates that Service Provider Name is supported by the USIM. */
		KUstSPN=0x04,
	/** Indicates that user controlled PLMN selector with Access Technology is supported
	by the USIM.
	
	Note: v2.0 of Multimode ETel API does not support this feature. */
		KUstPLMNSelAccessTech=0x08,
	/** Indicates that MSISDN is supported by the USIM. */
		KUstMSISDN=0x10,
	/** Indicates that Image is supported by the USIM.
	
	Note: (U)SAT ETel API provides this feature. This enum value is reserved for 
	future API use. */
		KUstIMG=0x20,
		//0x40 is a reserved value
	/** Indicates that enhanced Multi Level Precedence and Pre-emption Service is supported 
	by the USIM.
	
	Note: v2.0 of Multimode ETel API does not support this feature. */
		KUsteMLPP=0x80
		};

/** USIM service table (continued). */
	enum TUSTServices25To32
		{
	/** Indicates that Automatic Answer for eMLPP is supported by the USIM.
	
	Note: v1.0 and v2.0 of Multimode ETel API do not support this feature. */
		KUstAnswereMLPP=0x01,
		//0x02 is a reserved value
	/** Indicates that GSM Access is supported by the USIM */
		KUstGSMAccess=0x04,
	/** Indicates that Data download via SMS-PP is supported by the USIM.
	
	Note: (U)SAT ETel API provides this feature. */
		KUstSMSPpDataDownload=0x08,
	/** Indicates that Data download via SMS-CB is supported by the USIM.
	
	Note: (U)SAT ETel API provides this feature. */
		KUstSMSCbDataDownload=0x10,
	/** Indicates that Call Control by USIM is supported by the USIM.
	
	Note: (U)SAT ETel API provides this feature. */
		KUstCallControl=0x20,
	/** Indicates that MO-SMS Control by USIM is supported by the USIM.
	
	Note: (U)SAT ETel API provides this feature. */
		KUstMoSmControl=0x40,
	/** Indicates that the proactive command RUN AT COMMAND is supported by the USIM.
	
	Note: (U)SAT ETel API provides this feature. */
		KUstPCmdRunAtCommand=0x80		
		};

/** USIM service table (continued). */
	enum TUSTServices33To40
		{
	/** Should always be present as specified by 3 GPP spec. */
		KUstAlways=0x10,
	/** Indicates that the Enabled Service Table is supported by the USIM. */
		KUstEST=0x02,
	/** Indicates that APN Control List is supported by the USIM.
	
	Note: v2.0 of Multimode ETel API does not support this feature. */
		KUstACL=0x04,
	/** Indicates that the Depersonalisation Control Keys are supported by the USIM.
	
	Note: v2.0 of Multimode ETel API does not support this feature. */
		KIstDepersonalisationKeys =0x08,
	/** Indicates that the Co-operative Network List is supported by the USIM.

	Note: v2.0 of Multimode ETel API does not support this feature. */
		KUstCooperativeNetwork=0x10,
	/** Indicates that GSM Security Context is supported by the USIM. */
		KUstGSMSecurityContext=0x20,
	/** Indicates that CPBCCH Information is supported by the USIM.
	
	Note: v2.0 of Multimode ETel API do not support this feature. */
		KUstCPBCCHInfo=0x40,
	/** Indicates that Investigation Scan is supported by the USIM.
	
	Note:v2.0 of Multimode ETel API do not support this feature. */
		KUstInvestigationScan=0x80
		};

/** USIM service table (continued). */
	enum TUSTServices41To48
		{
	/** Indicates that MExE is supported by the USIM.
	
	Note: v2.0 of Multimode ETel API do not support this feature. */
		KUstMExE=0x01,
	/** Indicates that Operator controlled PLMN selector with Access Technology is 
	supported by the USIM.
	
	N.B .v2.0 of Multimode ETel API do not support this feature. */
		KUstOperatorPLMNSelector=0x02,
	/** Indicates that HPLMN selector with Access Technology is supported by the USIM.
	
	Note: v2.0 of Multimode ETel API do not support this feature. */
		KUstHPLMNSelAccessTech=0x04,
	/** Indicates that Extension 5 is supported by the USIM. Provides support for extension 
	data for MSISDN. */
		KUstExt5=0x08,
	/** Indicates that PLMN Network Name is supported by the USIM.
	
	Note: v2.0 of Multimode ETel API do not support this feature. */
		KUstPLMNNetworkName=0x10,
	/** Indicates that Operator PLMN List is supported by the USIM.
	
	Note: v2.0 of Multimode ETel API do not support this feature. */
		KUstOperatorPLMNList=0x20,
	/** Indicates that Mailbox Dialling Numbers are supported by the USIM. */
		KUstMDN=0x40,
	/** Indicates that Message Waiting Indication Status is supported by the USIM. */
		KUstMWISo=0x80
		};

/** USIM service table (continued). */
	enum TUSTServices49To56
		{
	/** Indicates that Call forwarding indication status is supported by the USIM.
	
	Note: v2.0 of Multimode ETel API do not support this feature. */
		KUstCFIS=0x01,
	/** Indicates that RPLMN last-used access technology is supported by the USIM (Release 5).
	From Release 6 this service is "Reserved and shall be ignored".
	
	Note: v2.0 of Multimode ETel API do not support this feature. */
		KUstRPLMN=0x02,
	/** Indicates that Service provider display information is supported by the USIM.
	
	Note: v2.0 of Multimode ETel API do not support this feature. */
		KUstSPDI=0x04,
	    KUstMMS=0x08,
	    KUstExt8=0x10,
	    KUstCConGPRS=0x20,
	    KUstMMSUCP=0x40
		};


/** USIM service table (continued). */
	enum TUSTServices57To64
		{
	/** Indicates that VGCS Group Identifier List ( VGCS and VGCSS) is supported by the USIM.
	
	Note: v8.0 of Multimode ETel API does not support this feature. */
		KUstVGCSGroupIdList=0x01,
	/** Indicates that VBS Group Identifier List ( VBS and VBSS ) is supported by the USIM.
	
	Note: v8.0 of Multimode ETel API does not support this feature. */
		KUstVBSGroupIdList=0x02,
	/** Indicates that Pseudonym  is supported by the USIM.*/
		KUstPseudonym=0x04,
	/** Indicates that User controlled PLMN selector for WLAN Access is supported by the USIM. */	
	    KUstUserPLMNSelectorWLAN=0x08,
	/** Indicates that Operator controlled PLMN selector for WLAN Access is supported by the USIM.*/    
	    KUstOperatorPLMNSelectorWLAN =0x10,
	/** Indicates that User controlled WLAN specific identifier list  is supported by the USIM.*/    
	    KUstUserWSIDList=0x20,
	/** Indicates that Opertor controlled WLAN specific identifier list  is supported by the USIM. */    
  	    KUstOperatorWSIDList=0x40,
	/** Indicates that Voice Group Call Service Ciphering Algorithm is supported by the USIM.
	
	Note: v8.0 of Multimode ETel API does not support this feature. */  
	    KUstVGCSSecurity=0x80
		};

/** USIM service table (continued). */
	enum TUSTServices65To72
		{
	/** Indicates that Voice Broadcast Service Ciphering Algorithm is supported by the USIM.
	
	 Note: v8.0 of Multimode ETel API does not support this feature. */  
		KUstVBSSecurity=0x01,
	/** Indicates that the WLAN Reauthentication Identity is supported by the USIM.*/
		KUstWLANReAuthenId=0x02,
	/** Indicates that Multimedia Message Storage is supported by the USIM.
	
	Note: v8.0 of Multimode ETel API does not support this feature. */  
		KUstMMStorage=0x04,
	/** Indicates that Generic Bootstrapping Architecture (GBA) is supported by the USIM */  
			
	    KUstGBA=0x08,
	/** Indicates that Multimedia Broadcast Multicast Service (MBMS) Security is supported by the USIM.*/  
	
	    KUstMBMSSecurity =0x10,
	 /** Indicates that USSD Data Dowload feature is supported by the USIM.
	
	Note: v8.0 of Multimode ETel API does not support this feature. */  
	
	    KUstUSSDDataDownload=0x20,
	/** Indicates that Additional TERMINAL PROFILE after UICC activation is supported by the USIM */  
	    KUstTerminalProfileUICCActivation=0x80
		};

/** Enabled Services Table. */
	enum TESTServices1To8
		{
	/** Indicates that FDN service is activated. */
		KEstFDN=0x01,
	/** Indicates that BDN service is activated. */
		KEstBDN=0x02,
	/** Indicates that the APN Control List service is activated.
	
	Note: v2.0 of Multimode ETel API do not support this feature. */
		KEstACL=0x04
		};

 /** ISIM Service Table*/
	enum TISTServices1To8
		{
	/** Indicates that EFP-CSCF is present in the ISIM. */	
		KIstPCSCF=0x01,
	/** Indicates that the Generic Bootstrapping Architecture (GBA) is supported by the ISIM. */
		KIstGBA=0x02,
	/** Indicates that the ISIM supports operations in HTTP-Digest security context. */
		KIstHTTPDigest=0x04
		};


/** Get Service Table.

Modes: GSM/WCDMA */
	enum TMobilePhoneServiceTable
		{
	/** Retrieve SIM service table on SIM (or from GSM application on UICC).
	
	Modes: GSM/WCDMA */
		ESIMServiceTable,
	/** Retrieve USIM service table from active USIM application on UICC.
	
	Modes: WCDMA */
		EUSIMServiceTable,
	/** Retrieve CDMA service table on R-UIM.
	
	Modes: CDMA 
	@deprecated 9.5	
	*/
		ECDMAServiceTable,
	/** USIM Enabled Services Table to be used in conjunction with the USIM Service 
	Table.
	
	Modes: WCDMA */
		EUSIMEnabledServiceTable,
	/** ISIM Service Table (IST) for the currently active ISIM application on the UICC is selected.
	
	Modes: WCDMA */	
		EISIMServiceTable
		};

	
	class TMobilePhoneServiceTableV1 : public TMultimodeType
	/** Defines contents of the specified Service Table on the ICC.

	@see GetServiceTable()
	@publishedPartner
	@released
	 */
		{
	public:
		IMPORT_C TMobilePhoneServiceTableV1();

	/** Sum of the TSSTServices1To8 capability constants. */
		TUint8 iServices1To8;
	/** Sum of the TSSTServices9To16 capability constants. */
		TUint8 iServices9To16;
	/** Sum of the TSSTServices17To24 capability constants. */
		TUint8 iServices17To24;
	/** Sum of the TSSTServices25To32 capability constants. */
		TUint8 iServices25To32;
	/** Sum of the TSSTServices33To40 capability constants. */
		TUint8 iServices33To40;
	/** Sum of the TSSTServices41To48 capability constants. */
		TUint8 iServices41To48;
	/** Sum of the TSSTServices49To56 capability constants. */
		TUint8 iServices49To56;
		};

/** A typedef'd packaged TMobilePhoneServiceTableV1 passing through a generic API 
function member. */
	typedef TPckg<TMobilePhoneServiceTableV1> TMobilePhoneServiceTableV1Pckg;

	enum { KAIDSize=16 };

/** A typedef'd buffer to hold the application identifier (AID) that identifies 
the USIM application in a UICC. The AID consists of a Registered application 
provider IDentifier (RID) of 5Â bytes and a Proprietary application Identifier 
eXtension (PIX) of up to 11 bytes. */
	typedef TBuf8<KAIDSize> TAID; 

	class TMobilePhoneServiceTableV2 : public TMobilePhoneServiceTableV1
/** Defines contents of the specified Service Table on the ICC for the v2.0 version 
of the API. 
*/
		{
	public:
		IMPORT_C TMobilePhoneServiceTableV2();
	public:
	/** The application identifier. */
		TAID   iAID;
		};

/** A typedef'd packaged TMobilePhoneServiceTableV2 passing through a generic API 
function member. */
	typedef TPckg<TMobilePhoneServiceTableV2> TMobilePhoneServiceTableV2Pckg;
	
	class TMobilePhoneServiceTableV8 : public TMobilePhoneServiceTableV2
	/** Defines contents of the specified Service Table on the UICC for the v8.0 version 
	of the API.
	See section 4.2.8 of 3GPP TS 31.102 v6.18.0 of the API. 
	*/	
		{
	public:
		IMPORT_C TMobilePhoneServiceTableV8();	
	public:
	/** Sum of the TSSTServices57To64 capability constants. */
		TUint8 	iServices57To64;
	/** Sum of the TSSTServices65To72 capability constants. */		
		TUint8	iServices65To72;	
		};
	
/** A typedef'd packaged TMobilePhoneServiceTableV8 passing through a generic API 
function member. */	
	typedef TPckg<TMobilePhoneServiceTableV8> TMobilePhoneServiceTableV8Pckg;	
 
	IMPORT_C void GetServiceTable(TRequestStatus& aReqStatus, TMobilePhoneServiceTable aTable, TDes8& aTableData) const;
	
	
	/***********************************************************************************/
	//
	// MobilePhonePower functional unit
	//
	/***********************************************************************************/

/** Phone supported battery information.

Modes: Common */
	enum TMobilePhoneBatteryCaps
		{
	/** Phone supports requests to get the current battery information. */
		KCapsGetBatteryInfo=0x00000001,
	/** Phone supports requests for notification of change in battery information. */
		KCapsNotifyBatteryInfoChange=0x00000002
		};

	IMPORT_C TInt GetBatteryCaps(TUint32& aCaps) const; 

/** The mobile phone battery status.

Modes: Common */
	enum TMobilePhoneBatteryStatus
		{
	/** The TSY can not determine the phone's current power status. */
		EPowerStatusUnknown,
	/** The phone is currently powered by a battery. */
		EPoweredByBattery,
	/** A battery is connected, but the phone is externally powered. */
		EBatteryConnectedButExternallyPowered,
	/** No battery is connected. */
		ENoBatteryConnected,
	/** Power fault. */
		EPowerFault
		};

	class TMobilePhoneBatteryInfoV1 : public TMultimodeType
/** Defines contents of the battery status of the phone.

@see GetBatteryInfo()
@see NotifyBatteryInfoChange()
*/
		{
	public:
		IMPORT_C TMobilePhoneBatteryInfoV1();
	public:
	/** The power and battery status.
	
	@see TMobilePhoneBatteryStatus */
		TMobilePhoneBatteryStatus iStatus;
	/** The percentage battery charge level. */
		TUint iChargeLevel;
		};

	IMPORT_C void GetBatteryInfo(TRequestStatus& aReqStatus, TMobilePhoneBatteryInfoV1& aInfo) const;
	IMPORT_C void NotifyBatteryInfoChange(TRequestStatus& aReqStatus, TMobilePhoneBatteryInfoV1& aInfo) const;

	
	/***********************************************************************************/
	//
	// MobilePhoneSignal functional unit
	//	
	/***********************************************************************************/

/** The mobile phone signal strength capabilities.

Modes: Common */
	enum TMobilePhoneSignalCaps
		{
	/** Phone supports requests to get the current signal strength. */
		KCapsGetSignalStrength=0x00000001,
	/** Phone supports requests for notification of change in signal strength. */
		KCapsNotifySignalStrengthChange=0x00000002
		};

	IMPORT_C TInt GetSignalCaps(TUint32& aCaps) const; 
	IMPORT_C void GetSignalStrength(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar) const;
	IMPORT_C void NotifySignalStrengthChange(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar) const;

	/***********************************************************************************/
	//
	// MobilePhoneIndicator functional unit
	//
	/***********************************************************************************/
	
/** The mobile phone indicator capabilities supported.

Modes: Common */
	enum TMobilePhoneIndicatorCaps
		{
	/** Phone supports requests to get the current value of all supported indicators. */
		KCapsGetIndicator=0x00000001,
	/** Phone supports requests for notification of change in the supported indicators. */
		KCapsNotifyIndicatorChange=0x00000002
		};

/** The mobile phone indicators.

Modes Common */
	enum TMobilePhoneIndicators
		{
	/** If bit-flag is set to '1' indicates that the battery charger is connected to 
	the phone. If bit-flag is set to '0' indicates that the battery charger is 
	disconnected. */
		KIndChargerConnected=0x00000001,
	/** If bit-flag is set to '1' indicates that network service is available. If bit-flag 
	is set to '0' indicates that network service is unavailable. */
		KIndNetworkAvailable=0x00000002,
	/** If bit-flag is set to '1' indicates that a call is in progress. If set to '0' 
	indicates that a call is not in progress. */
		KIndCallInProgress=0x00000004
		};

	IMPORT_C TInt GetIndicatorCaps(TUint32& aActionCaps, TUint32& aIndCaps) const; 
	IMPORT_C void GetIndicator(TRequestStatus& aReqStatus, TUint32& aIndicator) const;
	IMPORT_C void NotifyIndicatorChange(TRequestStatus& aReqStatus, TUint32& aIndicator) const;

	
	/***********************************************************************************/
	//
	// MobilePhoneIdentity functional unit
	//
	/***********************************************************************************/
	
/** Defines Identity Capabilities.

Modes: Common */
	enum TMobilePhoneIdentityCaps
		{
	/** Phone can return the identity of its manufacturer. */
		KCapsGetManufacturer=0x00000001,
	/** Phone can return the identity of its model. */
		KCapsGetModel=0x00000002,
	/** Phone can return the identity of its revision. */
		KCapsGetRevision=0x00000004,
	/** Phone can return the identity of its serial number. */
		KCapsGetSerialNumber=0x00000008,
	/** Phone can return the identity of its subscriber. */
		KCapsGetSubscriberId=0x00000010
		};

	IMPORT_C TInt GetIdentityCaps(TUint32& aCaps) const;

	enum {	KPhoneManufacturerIdSize=50	};
	enum {	KPhoneModelIdSize=50 };
	enum {	KPhoneRevisionIdSize=50	};
	enum {	KPhoneSerialNumberSize=50 };
	
	class TMobilePhoneIdentityV1 : public TMultimodeType
/** Defines the mobile phone identity.

@see GetPhoneId() */
		{
	public:
		IMPORT_C TMobilePhoneIdentityV1();
	public:
	/** Phone manufacturer identification, in character string format. */
		TBuf<KPhoneManufacturerIdSize> iManufacturer;
	/** Phone model identification, in character string format. */
		TBuf<KPhoneModelIdSize> iModel;
	/** Phone revision identification, in character string format. */
		TBuf<KPhoneRevisionIdSize> iRevision;
	/** Phone serial number (IMEI or ESN), in character string format. */
		TBuf<KPhoneSerialNumberSize> iSerialNumber;
		};

	IMPORT_C void GetPhoneId(TRequestStatus& aReqStatus, TMobilePhoneIdentityV1& aId) const;

	enum {	KIMSISize = 15 };

/** A typedef'd buffer to hold the mobile phone subscriber id. */
	typedef TBuf<KIMSISize> TMobilePhoneSubscriberId;

	IMPORT_C void GetSubscriberId(TRequestStatus& aReqStatus, TMobilePhoneSubscriberId& aId) const;

	/***********************************************************************************/
	//
	// MobilePhoneDTMF functional unit
	//
	/***********************************************************************************/
	
/** DTMF Capabilities.

Modes: Common */
	enum TMobilePhoneDTMFCaps
		{
	/** Capability to send a string of DTMF tones, containing one or more tone(s). */
		KCapsSendDTMFString=0x00000001,
	/** Capability to send one DTMF tone, using start and stop DTMF tone function member. */
		KCapsSendDTMFSingleTone=0x00000002,
		};
		
	/**
	DTMF tone operation event.
	*/
	enum TMobilePhoneDTMFEvent
		{
		/**
		 Start sending of a Dtmf tone operation has been acknowledged from the MSC.
		*/
		EStartDtmfTone,
		/**
		 Stop sending of a Dtmf tone operation has been acknowledged from the MSC.
		*/
		EStopDtmfTone,
		};
		

	IMPORT_C TInt GetDTMFCaps(TUint32& aCaps) const; 
	IMPORT_C void NotifyDTMFCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const;

	IMPORT_C void SendDTMFTones(TRequestStatus& aReqStatus, const TDesC& aTones) const;
	IMPORT_C void ReadDTMFTones(TRequestStatus& aReqStatus, TDes& aTones) const;
	IMPORT_C TInt StartDTMFTone(TChar aTone) const;
	IMPORT_C TInt StopDTMFTone() const;

	IMPORT_C void NotifyStopInDTMFString(TRequestStatus& aRequestStatus) const;
	IMPORT_C TInt ContinueDTMFStringSending(TBool aContinue) const;
	IMPORT_C void NotifyDTMFEvent(TRequestStatus& aReqStatus, TMobilePhoneDTMFEvent& aEvent) const;

	/***********************************************************************************/
	//
	// MobilePhoneNetwork functional unit
	//
	/***********************************************************************************/
	
/** Defines Network Capabilities.

Modes: Common */
	enum TMobilePhoneNetworkCaps
		{
	/** The phone supports retrieval of current registration status. */
		KCapsGetRegistrationStatus=0x00000001,
	/** The phone supports notifications of change in registration status. */
		KCapsNotifyRegistrationStatus=0x00000002,
	/** The phone supports retrieval of current mode. */
		KCapsGetCurrentMode=0x00000004,
	/** The phone supports notification of change in mode. */
		KCapsNotifyMode=0x00000008,
	/** The phone supports retrieval of current network information. */
		KCapsGetCurrentNetwork=0x00000010,
	/** The phone supports notification of change of current network. */
		KCapsNotifyCurrentNetwork=0x00000020,
	/** The phone supports retrieval of home network information. */
		KCapsGetHomeNetwork=0x00000040,
	/** The phone supports retrieval of a list of detected networks. */
		KCapsGetDetectedNetworks=0x00000080,
	/** The phone supports manual network selection mode. */
		KCapsManualNetworkSelection=0x00000100,
	/** The phone supports retrieval of time and date information (NITZ) received from 
	the network. */
		KCapsGetNITZInfo=0x00000200,
	/** The phone supports notification of new updates of time & date information (NITZ) 
	received from the network. */
		KCapsNotifyNITZInfo=0x00000400
		};

	IMPORT_C TInt GetNetworkCaps(TUint32& aCaps) const;

/** Defines Current Network Modes.

Modes: Common */
	enum TMobilePhoneNetworkMode
		{
	/** Network mode is unknown.
	
	Modes: Common */
		ENetworkModeUnknown,
	/** ME is not registered.
	
	Modes: Common */
		ENetworkModeUnregistered,
	/** GSM/GPRS or DCS1800 network. */
		ENetworkModeGsm,
	/** AMPS network.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ENetworkModeAmps,
	/** CDMA (IS-95) network.
	
	Modes: CDMA
	 @deprecated 9.5
	  */
		ENetworkModeCdma95,
	/** CDMA (cdma2000) network.
	
	Modes: CDMA
	@deprecated 9.5 */
		ENetworkModeCdma2000,
	/** WCDMA  (UTRA Frequency Division Duplex (FDD)) network.
	
	Modes: WCDMA */
		ENetworkModeWcdma,
	/** TD-CDMA (UTRA Time Division Duplex (TDD)) network.
	
	Modes: WCDMA */	
		ENetworkModeTdcdma
		};

	IMPORT_C TInt GetCurrentMode(TMobilePhoneNetworkMode& aNetworkMode) const;
	IMPORT_C void NotifyModeChange(TRequestStatus& aReqStatus, TMobilePhoneNetworkMode& aNetworkMode) const;

/** Phone network status.

Modes: Common */
	enum TMobilePhoneNetworkStatus
		{
	/** Status is unknown. */
		ENetworkStatusUnknown,
	/** A network that the ME is allowed to register to. */
		ENetworkStatusAvailable,
	/** The currently registered network. */
		ENetworkStatusCurrent,
	/** A network that the ME is not allowed to register to. */
		ENetworkStatusForbidden
		};

/** Mobile phone network band information.

Modes: CDMA

@deprecated 9.5

 */
	enum TMobilePhoneNetworkBandInfo
		{
	/** The current band and band class is unknown. */
		EBandUnknown,
	/** The network operates at 800MHz on Band A. */
		E800BandA,
	/** The network operates at 800MHz on Band B. */
		E800BandB,
	/** The network operates at 800MHz on Band C. */
		E800BandC,
	/** The network operates at 1900MHz on Band A. */
		E1900BandA,
	/** The network operates at 1900MHz on Band B. */
		E1900BandB,
	/** The network operates at 1900MHz on Band C. */
		E1900BandC,
	/** The network operates at 1900MHz on Band D. */
		E1900BandD,
	/** The network operates at 1900MHz on Band E. */
		E1900BandE,
	/** The network operates at 1900MHz on Band F. */
		E1900BandF
		};

/** A typedef'd buffer to hold the network display tag */
	typedef TBuf<30> TMobilePhoneNetworkDisplayTag;
/** A typedef'd buffer to hold the network long name. */
	typedef TBuf<20> TMobilePhoneNetworkLongName;		
/** A typedef'd buffer to hold the network short name. */
	typedef TBuf<10> TMobilePhoneNetworkShortName;

/** A typedef'd buffer to hold the network country code. */
	typedef TBuf<4> TMobilePhoneNetworkCountryCode;		// MCC in GSM and CDMA
/** A typedef'd buffer to hold the network identity. */
	typedef TBuf<8> TMobilePhoneNetworkIdentity;		// MNC in GSM and SID or NID in CDMA 

	

	class TMobilePhoneNetworkInfoV1 : public TMultimodeType
	/** Defines information related to a mobile phone network.

	@see GetCurrentNetwork()
	@see NotifyCurrentNetworkChange()
	@see GetHomeNetwork()
	@publishedPartner
	@released */
		{
	public:
		IMPORT_C TMobilePhoneNetworkInfoV1();
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
	/** Mode of the network.
	
	Modes: Common
	
	@see TMobilePhoneNetworkMode */
		TMobilePhoneNetworkMode iMode;
	/** Status of the network
	
	Modes: Common
	
	@see TMobilePhoneNetworkStatus */
		TMobilePhoneNetworkStatus iStatus;
	/** The band and band class of the CDMA network operator.

	Modes: CDMA
	
	@see TMobilePhoneNetworkBandInfo
	
	@deprecated 9.5 */
		TMobilePhoneNetworkBandInfo iBandInfo;
	/** The MCC of the network.
	
	Modes: Common
	
	@see TMobilePhoneNetworkCountryCode */
		TMobilePhoneNetworkCountryCode iCountryCode;
		
	/** The system identity (SID) of the CDMA network.
	
	Modes: CDMA
	
	@see TMobilePhoneNetworkIdentity 
	@deprecated 9.5
	*/
		TMobilePhoneNetworkIdentity iCdmaSID;
	/** The system identity (SID) of the AMPS network.
	
	Modes: CDMA
	
	@see TMobilePhoneNetworkIdentity
	TMobilePhoneNetworkIdentity
	@deprecated 9.5
	*/
		TMobilePhoneNetworkIdentity iAnalogSID;
	/** The network identity (NID in CDMA and MNC in GSM).
	
	Modes: Common
	
	@see TMobilePhoneNetworkIdentity */
		TMobilePhoneNetworkIdentity iNetworkId;
	/** The alpha-tag displayed when this is the serving network.
	
	Modes: Common
	
	@see TMobilePhoneNetworkDisplayTag */
		TMobilePhoneNetworkDisplayTag iDisplayTag;
	/** The short name (up to 10 characters) of the network operator.
	
	Modes: GSM/WCDMA
	
	@see TMobilePhoneNetworkShortName */
		TMobilePhoneNetworkShortName iShortName;
	/** The long name (up to 20 characters) of the network operator.
	
	Modes: GSM/WCDMA
	
	@see TMobilePhoneNetworkLongName */
		TMobilePhoneNetworkLongName iLongName;
		};
	
/** A typedef'd packaged TMobilePhoneNetworkInfoV1 for passing through a generic 
API function member. */
	typedef TPckg<TMobilePhoneNetworkInfoV1>  TMobilePhoneNetworkInfoV1Pckg;
	
/** The access technology that the network is based on.

Modes: Common */
	enum TMobilePhoneNetworkAccess
		{
	/** This is used when there is no network activity and therefore no RAT active.
	
	Modes: Common */
		ENetworkAccessUnknown,
	/** The access technology is GSM.
	
	Modes: GSM */
		ENetworkAccessGsm,
	/** The access technology is GSM COMPACT. However GSM COMPACT systems which use 
	GSM frequency bands but with the CBPCCH broadcast channel are considered as 
	a separate access technology from GSM.
	
	Modes: GSM */
		ENetworkAccessGsmCompact,
	/** The access technology is UTRAN (UMTS Network).
	
	Modes: WCDMA */
		ENetworkAccessUtran,					
	/** Analogue Cellular 
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ENetworkAccessAmpsCellular,
	/** CDMA Cellular - Standard channels 

	Modes: CDMA
	@deprecated 9.5
	 */
		 ENetworkAccessCdmaCellularStdChannel,
	/** CDMA Cellular - Custom channels 

	Modes: CDMA 
	@deprecated 9.5
	*/
		ENetworkAccessCdmaCellularCustomChannel,
	/** CDMA/Analogue Cellular (cdma first then analogue)

	Modes: CDMA 
	@deprecated 9.5
	*/
		ENetworkAccessCdmaAmpsCellular,
	/** CDMA PCS - Using blocks 
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ENetworkAccessCdmaPcsUsingBlocks,
	/** CDMA PCS - Using channels 
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ENetworkAccessCdmaPcsUsingNetworkAccessChannels,
	/** CDMA JTACS - Standard channels 
	
	Modes: CDMA 
	@deprecated 9.5 
	*/
		ENetworkAccessJTacsStdChannels,
	/** CDMA JTACS - Custom channels 
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ENetworkAccessJTacsCustomChannels,
	/** CDMA 2GHz band - Using channels 
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ENetworkAccess2GHzBandUsingChannels,
	/** Generic Acquisition Record for IS-2000 and IS-95 
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ENetworkAccessGenericAcqRecord2000and95,
	/** Generic Acquisition Record for IS-856
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ENetworkAccessGenericAcqRecord856,
	/** The access technology is GSM and UTRAN (UMTS Network).
		
	Modes: GSM and WCDMA */
		ENetworkAccessGsmAndUtran
		};

	

	class TMobilePhoneNetworkInfoV2 : public TMobilePhoneNetworkInfoV1
	/** The access technology that the network is based on in version v2.0 of the API. 
	@publishedPartner
	@released*/
		{
	public:
	/** Default constructor. Network access is set to ENetworkAccessUnknown and iExtensionId 
	is set to KETelExtMultimodeV2. */
		IMPORT_C TMobilePhoneNetworkInfoV2();
	
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	
	public:
	/** The type of network access.
	
	@see TMobilePhoneNetworkAccess */
		TMobilePhoneNetworkAccess iAccess;
		};
	
/** A typedef'd packaged TMobilePhoneNetworkInfoV2 for passing through a generic 
API function member. */
	typedef TPckg<TMobilePhoneNetworkInfoV2>  TMobilePhoneNetworkInfoV2Pckg;


	class TMobilePhoneNetworkInfoV5 : public TMobilePhoneNetworkInfoV2
	/** The access technology that the network is based on in version v2.0 of the API. 
	@publishedPartner
	@released*/
		{
	public:
		IMPORT_C TMobilePhoneNetworkInfoV5();
		
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
		
	public:
	/** HSDPA availability indicator. This information is for display 
	indication only and shall not be used for any other purpose.
		
	Modes: WCDMA */
		TBool iHsdpaAvailableIndicator;

	/** EGPRS availability indicator. This information is for display
	indication only and shall not be used for any other purpose.
		
	Modes: GPRS */
		TBool iEgprsAvailableIndicator;
		};

/** A typedef'd packaged TMobilePhoneNetworkInfoV5 for passing through a generic 
API function member. */		
	typedef TPckg<TMobilePhoneNetworkInfoV5> TMobilePhoneNetworkInfoV5Pckg;
	
		
	class TMobilePhoneNetworkInfoV8 : public TMobilePhoneNetworkInfoV5
	/** The access technology that the network is based on in version v8.0 of the API. 
	@publishedPartner
	@released*/
		{
	public:
		IMPORT_C TMobilePhoneNetworkInfoV8();
		
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
		
	public:
	/** HSUPA availability indicator. This information is for display 
	indication only and shall not be used for any other purpose.
		
	Modes: WCDMA */
		TBool iHsupaAvailableIndicator;

		};
		
/** A typedef'd packaged TMobilePhoneNetworkInfoV8 for passing through a generic 
API function member. */		
	typedef TPckg<TMobilePhoneNetworkInfoV8> TMobilePhoneNetworkInfoV8Pckg;

	class TMobilePhoneLocationAreaV1 : public TMultimodeType
/** Defines the mobile phone location area.

@see GetCurrentNetwork()
@see NotifyCurrentNetworkChange() */
		{
	public:
	/** Constructor. */
		IMPORT_C TMobilePhoneLocationAreaV1();
	public:
	/** Boolean indicating whether the location area is known.
	
	Modes: Common */
		TBool	iAreaKnown;
	/** Location area code.
	
	Modes: GSM/WCDMA */
		TUint	iLocationAreaCode;
	/** Cell identity code.
	
	Modes: GSM/WCDMA */
		TUint	iCellId;
		};

	IMPORT_C void GetCurrentNetwork(TRequestStatus& aReqStatus, TDes8& aNetworkInfo, TMobilePhoneLocationAreaV1& aArea) const;
	IMPORT_C void NotifyCurrentNetworkChange(TRequestStatus& aReqStatus, TDes8& aNetworkInfo, TMobilePhoneLocationAreaV1& aArea) const;
	IMPORT_C void GetCurrentNetwork(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const;
	IMPORT_C void NotifyCurrentNetworkChange(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const;

	IMPORT_C void GetHomeNetwork(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const;
	IMPORT_C void GetHomeNetworkSearchPeriod(TRequestStatus& aReqStatus, TInt& aSearchIntv) const;
	IMPORT_C void GetLastUsedAccessTechnology(TRequestStatus& aReqStatus, TMobilePhoneNetworkAccess& aAccessTech) const;
	
	/** Cell mode */
	enum TCellMode
		{
		/** Unknown mode */
		ECellModeUnknown,
		/** GSM mode */
		ECellModeGsm,
		/** WCDMA mode */
		ECellModeWcdma
		};

	/** Cell Information Status 
	@publishedPartner	
	@released */
	enum TCellInfoStatus
		{
		/** Unknown status: not known if Cell ID is current or not */
		ECellInfoStatusUnknown,
		/** Not current status: the Cell ID value is the ID of the last known cell */
		ECellInfoStatusNotCurrent,
		/** Current Status: the Cell ID value is the ID of the present cell */
		ECellInfoStatusCurrent
		};
	
	/** Defined type to represent the Timing Advance. See 3GPP TS 04.18, section 10.5.2.40. 
	 * Valid values for the Timing Advance are 0-255. A negative value indicates that
	 * no Timing Advance information is available. */
	typedef TInt16 TGsmTimingAdvance;
	
	/**	Encapsulates cell specific information.	
	@publishedPartner	
	@released */
	class TMobilePhoneCellInfoV9 : public TMultimodeType
		{
	public:
		IMPORT_C TMobilePhoneCellInfoV9();
		
	public:
		/** The cell mode 
		@see TCellMode */
		TCellMode iMode;
		/** The current cell ID */
		TUint iCellId;
		/** The Timing Advance. Relevant only if iMode is ECellModeGsm.
		 @see TGsmTimingAdvance */
		TGsmTimingAdvance iTimingAdvance;
		/** The status of the cell information. Indicates whether the Cell ID is
		 * up-to-date.
		 @see TCellInfoStatus */
		TCellInfoStatus iStatus;
		};				

	/** Packaged TMobilePhoneCellInfoV9 data structure. 
	@see TMobilePhoneCellInfoV9 */
	typedef TPckg<TMobilePhoneCellInfoV9> TMobilePhoneCellInfoV9Pckg;
	
	IMPORT_C void GetCellInfo(TRequestStatus& aReqStatus, TDes8& aCellInfo) const;
	IMPORT_C void NotifyCellInfoChange(TRequestStatus& aReqStatus, TDes8& aCellInfo) const;

	
/** The registration status of the mobile phone.

Modes: WCDMA */
	enum TMobilePhoneRegistrationStatus
		{
	/** Registration status is unknown. */
		ERegistrationUnknown,
	/** Not registered. The ME can not detect any other networks and is not currently 
	searching a new operator to register to. */
		ENotRegisteredNoService,
	/** Not registered. The ME can detect other networks on which it is possible to 
	make emergency calls only. */
		ENotRegisteredEmergencyOnly,
	/** Not registered, but the ME is currently searching a new operator to register to. */
		ENotRegisteredSearching,
	/** Registered, network busy.
	
	Modes: WCDMA */
		ERegisteredBusy,
	/** Registered on home network. */
		ERegisteredOnHomeNetwork,
	/** Registration denied. */
		ERegistrationDenied,
	/** Registered, roaming. */
		ERegisteredRoaming
		};

	IMPORT_C void GetNetworkRegistrationStatus(TRequestStatus& aReqStatus, TMobilePhoneRegistrationStatus& aStatus) const;
	IMPORT_C void NotifyNetworkRegistrationStatusChange(TRequestStatus& aReqStatus, TMobilePhoneRegistrationStatus& aStatus) const;

/** The network selection function member.

Modes: Common */
	enum TMobilePhoneSelectionMethod
		{
	/** Network selection function member is unknown.
	
	Modes: Common */
		ENetworkSelectionUnknown,
	/** Network is selected automatically by phone.
	
	Modes: Common */
		ENetworkSelectionAutomatic,
	/** Network is selected manually by user.
	
	Modes: GSM/WCDMA */
		ENetworkSelectionManual,
	/** Network selection is restricted to home network only
	
	Modes: CDMA 
	@deprecated 9.5 
	*/
		ENetworkSelectionHomeOnly
		};

/** The mobile phone band class preferences.

Modes: CDMA 
@deprecated 9.5 
*/
	enum TMobilePhoneBandClass
		{
	/** Band class selection is unknown. */
		ENetworkBandClassUnknown,
	/** Phone will select band class A only. */
		ENetworkBandClassAOnly,
	/** Phone will select band class B only. */
		ENetworkBandClassBOnly,
	/** Phone will prefer to select band class A. */
		ENetworkBandClassAPreferred,
	/** Phone will prefer to select band class B. */
		ENetworkBandClassBPreferred
		};

/** The mobile phone network operation mode preferences.

Modes: CDMA
@deprecated 9.5 */
	enum TMobilePhoneOperation
		{
	/** Network operation mode is unknown. */
		ENetworkOperationUnknown,
	/** Phone will select analog operation only. */
		ENetworkOperationAnalogOnly,
	/** Phone will select digital/CDMA operation only. */
		ENetworkOperationDigitalOnly,
	/** Phone will prefer to select analog operation. */
		ENetworkOperationAnalogPreferred,
	/** Phone will prefer to select digital/CDMA operation. */
		ENetworkOperationDigitalPreferred
		};

	class TMobilePhoneNetworkSelectionV1 : public TMultimodeType
/** The mobile phone network selection preferences.

@see GetNetworkSelectionSetting()
@see SetNetworkSelectionSetting()
@see NotifyNetworkSelectionSettingChange()
@publishedPartner
@released */
		{
	public:
		IMPORT_C TMobilePhoneNetworkSelectionV1();
	public:
	/** Phone's network selection function member.
	
	Modes: Common
	
	@see TMobilePhoneSelectionMethod */
		TMobilePhoneSelectionMethod	iMethod;
	/** Phone's band class setting.
	
	Modes: CDMA
	
	@see TMobilePhoneBandClass
	@deprecated 9.5
	 */
		TMobilePhoneBandClass		iBandClass;
	/** Phone's CDMA operation mode.
	
	Modes: CDMA
	
	@see TMobilePhoneOperation 
		@deprecated 9.5
		*/
		TMobilePhoneOperation		iOperationMode;
		};

/** A typedef'd packaged TMobilePhoneNetworkSelectionV1 for passing through a generic 
API function member. */
	typedef TPckg<TMobilePhoneNetworkSelectionV1>  TMobilePhoneNetworkSelectionV1Pckg;

	IMPORT_C TInt GetNetworkSelectionSetting(TDes8& aSetting) const;
	IMPORT_C void SetNetworkSelectionSetting(TRequestStatus& aReqStatus, const TDes8& aSetting) const;
	IMPORT_C void NotifyNetworkSelectionSettingChange(TRequestStatus& aReqStatus, TDes8& aSetting) const;
	
	struct TMobilePhoneNetworkManualSelection
/** The mobile phone manual network selection parameters.

Modes: GSM/WCDMA

@see TMobilePhoneNetworkCountryCode */
		{
	/** The MCC of the network
	
	@see TMobilePhoneNetworkCountryCode */
		TMobilePhoneNetworkCountryCode iCountry;
	/** The MNC of the network
	
	@see TMobilePhoneNetworkIdentity */
		TMobilePhoneNetworkIdentity iNetwork;
		};
	
	IMPORT_C void SelectNetwork(TRequestStatus& aReqStatus, TBool aIsManual, const TMobilePhoneNetworkManualSelection& aManualSelection) const;

	

	 // Used to indicate which TMobilePhoneNITZ fields are currently available
/** The Phone Network Time Zone Capabilities. */
	 enum TMobilePhoneNITZCaps
		{
	/** Universal Time is available in the current network mode. */
		KCapsTimeAvailable      = 0x00000001,
	/** Local time zone is available in the current network mode. */
		KCapsTimezoneAvailable  = 0x00000002,
	/** DST is available in the current network mode. */
		KCapsDSTAvailable       = 0x00000004,
	/** Short name is available in the current network mode. */
		KCapsShortNameAvailable = 0x00000008,
	/** Long name is available in the current network mode. */
		KCapsLongNameAvailable  = 0x00000010
		};

	class TMobilePhoneNITZ : public TDateTime
/** Defines time and date information received from a mobile phone network.

@see GetNITZInfo()
@see NotifyNITZInfoChange()
@publishedPartner
@released */
		{
	public:
		IMPORT_C TMobilePhoneNITZ();
		IMPORT_C TMobilePhoneNITZ(TInt aYear, TMonth aMonth, TInt aDay, TInt aHour, TInt aMinute, TInt aSecond, TInt aMicroSecond);		
	public:
	/** Bitwise sum of TMobilePhoneNITZCaps constants used to indicate which NITZ fields 
	are available.
	
	Modes: Common */
		TInt32	                     iNitzFieldsUsed;
	/** Time zone, expressed in +/- 15 minute offsets from GMT.
	
	Modes: Common */
		TInt                         iTimeZone;
	/** Daylight Saving Time adjustment for summer time. The adjustment for DST 
	can be +1h or +2h.
	
	Modes: GSM/WCDMA */
		TInt	                     iDST;
	/** Network name in "short" representation.
	
	Modes: GSM/WCDMA
	
	@see TMobilePhoneNetworkShortName */
		TMobilePhoneNetworkShortName iShortNetworkId;
	/** Network name in "long" representation.
	
	Modes: GSM/WCDMA
	
	@see TMobilePhoneNetworkLongName */
		TMobilePhoneNetworkLongName	 iLongNetworkId;
		};

	IMPORT_C TInt GetNITZInfo(TMobilePhoneNITZ& aNITZInfo) const;
	IMPORT_C void NotifyNITZInfoChange(TRequestStatus& aReqStatus, TMobilePhoneNITZ& aNITZInfo) const;

	enum { KMaxSPNameSize=16 };
	enum { KMaxPLMNFieldSize=251 };

	enum TDisplayRequirements
		{
		KDisplayPLMNNotRequired	= 0x00000001,
		KDisplayPLMNRequired	= 0x00000002,
		KDisplaySPNRequired		= 0x00000004,
		KDisplaySPNNotRequired	= 0x00000008
		};

	typedef TUint32 TDisplayRequirementsFlags;

	class TMobilePhoneServiceProviderNameV2 : public TMultimodeType
/** Used to retrieve the Service Provider Name from the ICC. The 
SP is actually the entity that provides the service. The SP could have contractual 
agreements with several home PLMN that the ME can register to. The display 
preference of the registered PLMN and/or the SPN on the ME can be defined 
on the UICC.
@publishedPartner
@released */
		{
	public:
		IMPORT_C TMobilePhoneServiceProviderNameV2();
	public:
	/** This parameter specifies if the display of the registered PLMN is required. 
	Can contain a number of flags from RMobilePhone::TDisplayRequirements.
	*/
		TDisplayRequirementsFlags	iDisplayReq;
	/** The name of the service provider. */
		TBuf<KMaxSPNameSize>	iSPName;
		TBuf<KMaxPLMNFieldSize>	iPLMNField;
		}; 

/** A typedef'd packaged TMobilePhoneServiceProviderNameV2 for passing through 
a generic API function member. */
	typedef TPckg<TMobilePhoneServiceProviderNameV2> TMobilePhoneServiceProviderNameV2Pckg;

	IMPORT_C void GetServiceProviderName(TRequestStatus& aReqStatus, TDes8& aName) const;
	
	/**
	Network investigation scan settings.
	*/
	enum TMobilePhoneInvestigationScan
		{
		/**
		Network investigation setting in limited service mode.
		*/
		KNetworkInvLimited = 0x01,
		/**
		Network investigation setting after successful PLMN selection.
		*/
		KNetworkInvAfterPlmnSelection=0x02		
		};
		
	/**
	Network investigation scan events.
	*/
	enum TMobilePhoneInvestigationScanEvent
		{
		/**
		No investigation scan events available.
		*/
		ENetworkInvestigationNoEvent,
		/**
		A higher priority network has been detected, but it does not offer CS Voice.
		*/
		ENetworkInvestigationHighPriorityPlmn
		};
	
	IMPORT_C TInt GetNetworkInvScanSetting(TMobilePhoneInvestigationScan & aSetting) const;
	IMPORT_C void NotifyNetworkInvScanChange(TRequestStatus& aReqStatus, TMobilePhoneInvestigationScan& aSetting) const;
	IMPORT_C void NotifyNetworkInvScanEvent(TRequestStatus& aReqStatus, TMobilePhoneInvestigationScanEvent& aEvent) const;


	/** 251 bytes is the maximum number of octets in an L3 message as defined in3GPP TS 44.006. */
	enum { KMaxNetworkNameFieldSize = 251 };
	
	class TMobilePhoneNetworkNameV3 : public TMultimodeType
	/**
	Class containing short, long network names and location information for a PLMN.
	*/
		{
	public:
		IMPORT_C TMobilePhoneNetworkNameV3();
	public:
		/**
		The long name (up to 20 characters) of the network operator.
		*/
		TMobilePhoneNetworkLongName	 iLongName;
		/**
		The short name (up to 10 characters) of the network operator.
		*/
		TMobilePhoneNetworkShortName	 iShortName;
		/**
		This buffer contains other long and short names for the registered PLMN coded as TLV objects.
		*/
		TBuf<KMaxNetworkNameFieldSize>	 iOtherNames;
		};
		
	/**
 	Packaging typedef for TMobilePhoneNetworkNameV3 class.
 	*/	
	typedef TPckg<TMobilePhoneNetworkNameV3>  TMobilePhoneNetworkNameV3Pckg;
	
	
	class  TMobilePhoneOPlmnV3 : public TMultimodeType
	/**
	Class containing network name information.
	*/
		{
	public:
		IMPORT_C TMobilePhoneOPlmnV3();
	public:
		/**
		The MCC of the registered PLMN, with "wild" values specified for the particular digits of the code.
		*/
		TMobilePhoneNetworkCountryCode iCountryCode;
		/**
		The MNC of the registered PLMN, with "wild" values specified for the particular digits of the code.
		*/
		TMobilePhoneNetworkIdentity iNetworkId;
		/**
		The id of the Network Name to be displayed for the (following) location area codes.
		*/
		TUint32 iPNNid;
		/**
		First Location area code for defining the entire range of LACs for the registered PLMN.
		*/
		TUint iFirstLocationAreaCode;
		/**
		Last Location area code defining the entire range of LACs for the registered PLMN.
		*/
		TUint iLastLocationAreaCode;
		
		};
		
	/**
 	Packaging typedef for TMobilePhoneOPlmnV3 class.
 	*/	
	typedef TPckg<TMobilePhoneOPlmnV3>  TMobilePhoneOPlmnV3Pckg;
	
	IMPORT_C void GetCurrentNetworkName(TRequestStatus& aReqStatus, TDes8& aNetworkName, TDes8& aLocInfo) const;
	
	/**
	Identifiers for different access technologies.
	*/
	enum TMobilePhoneNetworkAccessCaps
		{
		/**
		No network activity.
		*/
		KNetworkAccessUnknown=0x0000,
		/**
		Access technology is GSM.
		*/
		KNetworkAccessGsm=0x8000,
		/**
		Access technology is GSM COMPACT.
		*/
		KNetworkAccessGsmCompact=0x4000,
		/**
		Access technology is UTRAN (UMTS only).
		*/
		KNetworkAccessUtran=0x0080,
		/** 
		Indicates that a particular PLMN entry is a WLAN entry used for WLAN PLMN selection.
		*/
		KNetworkAccessWLAN=0x0040
		};

	typedef TUint32 TMobilePhoneNetworkAccessCapsFlags;
		
	class TMobilePreferredNetworkEntryV3 : public TMultimodeType
	/**
	Defines information for a preferred network.
	*/
		{
	public:
		IMPORT_C TMobilePreferredNetworkEntryV3();
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		/**
		Access Technology supported.
		Can contain a number of flags from RMobilePhone::TMobilePhoneNetworkAccessCaps.
		*/
		TMobilePhoneNetworkAccessCapsFlags iAccess;
		/**
		Editable preferences in the User-controlled Networks List with Access Technology.
		*/
		TBool iUserDefined;
		/**
		The MCC of the network.
		*/
		TMobilePhoneNetworkCountryCode iCountryCode;
		/**
		The MNC of the network.
		*/
		TMobilePhoneNetworkIdentity iNetworkId;
		};
		
	IMPORT_C void StorePreferredNetworksListL(TRequestStatus& aReqStatus, CMobilePhoneStoredNetworkList* aList) const;
	IMPORT_C void NotifyPreferredNetworksListChange(TRequestStatus& aReqStatus) const;
		
	/***********************************************************************************/		
	//
	// Mobile Multimedia Call Settings functional unit
	//
	/***********************************************************************************/
	
/** An enumerated list of settings for multimedia calls. */
	enum TMobilePhoneMultimediaSettings
		{
	/** Accept all incoming multimedia calls with the fallback option enabled. (default). */
		EAcceptMMCallsVoiceFallback,
	/** Accept all incoming multimedia calls, but do not accept voice fallback. */
		EAcceptMMCallsNoFallback,
	/** Reject all incoming multimedia calls. */
		ERejectMMCalls,
	/** Reject all incoming multimedia calls, but drop to voice instead. */
		EAnswerMMCallsAsVoice
		};

	IMPORT_C void SetMultimediaCallPreference(TRequestStatus& aReqStatus, TMobilePhoneMultimediaSettings aMmSettings) const;
	IMPORT_C TInt GetMultimediaCallPreference(TMobilePhoneMultimediaSettings& aMmSettings) const;
	IMPORT_C void NotifyMultimediaCallPreferenceChange(TRequestStatus& aReqStatus, TMobilePhoneMultimediaSettings& aMmSettings);

	/***********************************************************************************/	
	//
	// MobilePrivacy functional unit
	//
	/***********************************************************************************/
	
/** The mobile phone privacy settings.

Modes: CDMA
@deprecated 9.5 */
	enum TMobilePhonePrivacy
		{
	/** The privacy setting is unspecified. */
		EPrivacyUnspecified,
	/** The privacy setting is set to ON. */
		EPrivacyOn,
	/** The privacy setting is set to OFF. */
		EPrivacyOff
		};

	IMPORT_C TInt GetDefaultPrivacy(TMobilePhonePrivacy& aSetting) const;
	IMPORT_C void SetDefaultPrivacy(TRequestStatus& aReqStatus, TMobilePhonePrivacy aSetting) const;
	IMPORT_C void NotifyDefaultPrivacyChange(TRequestStatus& aReqStatus, TMobilePhonePrivacy& aSetting) const;

	/***********************************************************************************/
	//
	// TSY Capabilities for supplementary call services
	// 
	/***********************************************************************************/
	
/** The call services.

Modes: GSM/WCDMA */
	enum TMobilePhoneCallServiceCaps
		{
	/** Phone supports retrieval of call forwarding status from a cache on the phone.
	
	Modes: GSM/WCDMA */
		KCapsGetCFStatusCache				=0x00000001,
	/** Phone supports retrieval of call forwarding status from the network.
	
	Modes: GSM/WCDMA */
		KCapsGetCFStatusNetwork				=0x00000002,
	/** Phone supports setting of call forwarding status (and registered information) 
	in the network. */
		KCapsSetCFStatus					=0x00000004,
	/** Phone supports notification of change in status of any call forwarding service.
	
	Modes: GSM/WCDMA */
		KCapsNotifyCFStatus					=0x00000008,
	/** Phone supports retrieval of CLIP status from either a cache or the network.
	
	Modes: GSM/WCDMA */
		KCapsGetClipStatus					=0x00000010,
	/** Phone supports retrieval of CLIR status from either a cache or the network.
	
	Modes: GSM/WCDMA */
		KCapsGetClirStatus					=0x00000020,
	/** Phone supports retrieval of COLP status from either a cache or the network.
	
	Modes: GSM/WCDMA */
		KCapsGetColpStatus					=0x00000040,
	/** Phone supports retrieval of COLR status from either a cache or the network.
	
	Modes: GSM/WCDMA */
		KCapsGetColrStatus					=0x00000080,
	/** Phone supports retrieval of CNAP status from either a cache or the network.

	Modes: GSM/WCDMA */
		KCapsGetCnapStatus					=0x00000100,
	/** Phone supports retrieval of call barring status from a cache on the phone.
	
	Modes: Common */
		KCapsGetCBStatusCache				=0x00000200,
	/** Phone supports retrieval of call barring status from the network.
	
	Modes: GSM/WCDMA */
		KCapsGetCBStatusNetwork				=0x00000400,
	/** Phone supports setting of call barring status in the network.
	
	Modes: Common */
		KCapsSetCBStatus					=0x00000800,
	/** Phone supports notification of change in call barring status for any service.
	
	Modes: Common */
		KCapsNotifyCBStatus					=0x00001000,
	/** Phone supports setting of a password for call barring service.
	
	Modes: Common */
		KCapsChangeCBPassword				=0x00002000,
	/** Phone supports BAIC call barring service.
	
	Modes: Common */
		KCapsBarAllIncoming					=0x00004000,
	/** Phone supports BIC-Roam call barring service.
	
	Modes: Common */
		KCapsBarIncomingRoaming				=0x00008000,
	/** Phone supports BAOC call barring service.
	
	Modes: Common */
		KCapsBarAllOutgoing					=0x00010000,
	/** Phone supports BOIC call barring service.
	
	Modes: Common */
		KCapsBarOutgoingInternational		=0x00020000,
	/** Phone supports BOIC-ExHC call barring service.
	
	Modes: Common */
		KCapsBarOutgoingInternationalExHC	=0x00040000,
	/** Phone supports barring all cases at once.
	
	Modes: Common */
		KCapsBarAllCases					=0x00080000,
	/** Phone supports retrieval of call waiting status from a cache on the phone.
	
	Modes: GSM/WCDMA */
		KCapsGetCWStatusCache				=0x00100000,
	/** Phone supports retrieval of call waiting status from the network.
	
	Modes: GSM/WCDMA */
		KCapsGetCWStatusNetwork				=0x00200000,
	/** Phone supports setting of call waiting status in the network.
	
	Modes: Common */
		KCapsSetCWStatus					=0x00400000,
	/** Phone supports notification of change in call waiting status for any service.
	
	Modes: GSM/WCDMA */
		KCapsNotifyCWStatus					=0x00800000,
	/** Phone supports retrieval of call completion (CCBS) status from a cache on the 
	phone.
	
	Modes: GSM/WCDMA */
		KCapsGetCCBSStatusCache				=0x01000000,
	/** Phone supports retrieval of call completion (CCBS) status from the network.
	
	Modes: GSM/WCDMA */
		KCapsGetCCBSStatusNetwork			=0x02000000,
	/** Phone supports deactivation of all CCBS requests at once.
	
	Modes: GSM/WCDMA */
		KCapsDeactivateAllCCBS				=0x04000000,
		KCapsDeactivateCCBS					=0x08000000,
	/** Phone supports retrieval of a list of active CCBS requests.
	
	Modes: GSM/WCDMA */
		KCapsRetrieveActiveCCBS				=0x10000000,
	/** The phone supports programming and retrieval of feature code against a CDMA 
	network service.
	
	Modes: CDMA
	@deprecated 9.5
	 */
		KCapsFeatureCode					=0x20000000,
	/** The phone supports sending of generic network service request strings.
	
	Modes: Common */
		KCapsNetworkServiceRequest			=0x40000000,
	/** The phone supports retrieval of called line identification status.
	
	Modes: GSM/WCDMA */
		KCapsGetCdipStatus					=0x80000000
		};

	IMPORT_C TInt GetCallServiceCaps(TUint32& aCaps) const;
	IMPORT_C void NotifyCallServiceCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const;

	/***********************************************************************************/
	//
	// MobilePhoneUserNetworkAccess functional unit
	//
	/***********************************************************************************/

/** Mobile Phone network services.

Modes: CDMA 
@deprecated 9.5
*/
	enum TMobilePhoneNetworkService
		{
	/** Network service is unspecified. */
		ENetworkServiceUnspecified,
	/** Feature code applies to call forwarding unconditional service. */
		ECFUService,
	/** Feature code applies to call forwarding on busy service. */
		ECFBService,
	/** Feature code applies to call forwarding on no reply service. */
		ECFNRyService,
	/** Feature code applies to call forwarding on not reachable service. */
		ECFNRcService,
	/** Feature code applies to user selective call forwarding service - when the 
	user wishes to forward incoming call to voicemail. */
		EDeflectToVoicemail,
	/** Feature code applies to user selective call forwarding service - when the
	user wishes to forward incoming call to a number supplied within the deflect 
	request. */
		EDeflectToNumber,
	/** Feature code applies to user selective call forwarding service - when the 
	user wishes to forward incoming call to a number pre-registered within the 
	network. */
		EDeflectToRegisteredNumber,
	/** Feature code applies to call waiting service. */
		ECWService,
		ENextCallShowCLI,
	/** Feature code applies to caller ID restriction service - when user wishes to 
	hide their ID for the next call they make. */
		ENextCallHideCLI
		};

/** Mobile phone service actions.

Modes: Common */
	enum TMobilePhoneServiceAction
		{
	/** The action is unspecified. */
		EServiceActionUnspecified,
	/** The client is registering new service information. If the service was not active 
	then this action also activates the service. */
		EServiceActionRegister,
	/** The client is activating the service. */
		EServiceActionActivate,
	/** The client is invoking the service. */
		EServiceActionInvoke,
	/** The client is deactivating the service. */
		EServiceActionDeactivate,
	/** The client is erasing the currently registered service information. If the service 
	was active then this action also deactivates the service. */
		EServiceActionErase
		};
		
	/**
	Defines the type of SS operation
	*/
	enum TMobilePhoneNotifySendSSOperation
		{
		/**
		SS Invoke operation.
		*/
		ESendSSInvoke,
		/**
		SS ReturnResult operation.
		*/
		ESendSSReturnResult,
		/**
		SS ReturnError operation.
		*/
		ESendSSReturnError,
		/**
		SS Reject operation.
		*/
		ESendSSReject		
		};
	
	/** Size of additional info buffer. */	
	enum{ KAdditionalInfoSize = 244 };
	
	class TMobilePhoneSendSSRequestV3 : public TMultimodeType
	/**
	Contains the operation code and buffer to hold any additional information when receiving a notification 
	about a network service request.
	*/
		{
	public:
		IMPORT_C TMobilePhoneSendSSRequestV3();
	public:
		/**
		This is the Operation Code for a Send SS Invoke or Return result events.
		This is the Error or Problem Code for a Send SS Return Error or Reject events.
		*/
		TUint8	iOpCode;
		/**
		The additional information (parameters) for a Send SS Invoke or Return result or Return Error events.
		This is not used for a Send SS Reject event
		*/
		TBuf<KAdditionalInfoSize>	iAdditionalInfo;
		};
	
	/**
 	Packaging typedef for TMobilePhoneSendSSRequestV3 class.
 	*/	
	typedef TPckg<TMobilePhoneSendSSRequestV3> TMobilePhoneSendSSRequestV3Pckg;

	// API/TSY internal type

	struct TNetworkServiceAndAction
		{
		TMobilePhoneNetworkService iService;
		TMobilePhoneServiceAction iAction;
		};
	
	IMPORT_C void ProgramFeatureCode(TRequestStatus& aReqStatus, const TDesC& aFCString, TMobilePhoneNetworkService aService, TMobilePhoneServiceAction aAction) const;
	IMPORT_C void GetFeatureCode(TRequestStatus& aReqStatus, TDes& aFCString, TMobilePhoneNetworkService aService, TMobilePhoneServiceAction aAction) const;

	IMPORT_C void SendNetworkServiceRequest(TRequestStatus& aReqStatus, const TDesC& aServiceString) const;
	IMPORT_C void SendNetworkServiceRequestNoFdnCheck(TRequestStatus& aReqStatus, const TDesC& aServiceString) const;
	IMPORT_C void NotifySendNetworkServiceRequest(TRequestStatus& aReqStatus, const TMobilePhoneNotifySendSSOperation aOperation, TDes8& aRequestComplete) const;
	IMPORT_C void NotifyAllSendNetworkServiceRequest(TRequestStatus& aReqStatus, TMobilePhoneNotifySendSSOperation& aOperation, TDes8& aRequestComplete) const;

	/***********************************************************************************/
	//
	// MobilePhoneCallForwarding functional unit
	// 
	/***********************************************************************************/

/** The Call Forwarding flavours.

Modes: GSM/WCDMA */
	enum TMobilePhoneCFCondition
		{
	/** The call forwarding service is unspecified. */
		ECallForwardingUnspecified,
	/** All calls to this subscriber are forwarded. */
		ECallForwardingUnconditional,
	/** Calls are forwarded when this subscriber is busy. */
		ECallForwardingBusy,
	/** Calls are forwarded when this subscriber does not reply within a time-out period. */
		ECallForwardingNoReply,
	/** Calls are forwarded when this subscriber is unreachable. */
		ECallForwardingNotReachable,
	/** All of above CF services. Can be used to check all 4 call forwarding Fs 
	at once. */
		ECallForwardingAllCases,			// combination of all four above cases
	/** All the conditional (busy, no reply, not reachable) CF services. Can be used 
	to check all 3 conditional call forwarding conditions at once. */
		ECallForwardingAllConditionalCases	// combination of CFB, CFNRy and CFNRc
		};

/** The Call Forwarding status

Modes: GSM/WCDMA */
	enum TMobilePhoneCFStatus
		{
	/** The CF service is currently active and operative. */
		ECallForwardingStatusActive,
	/** The CF service is currently deactivated. */
		ECallForwardingStatusNotActive,
	/** The CF service is not registered. */
		ECallForwardingStatusNotRegistered,
	/** The CF service is not provisioned. */
		ECallForwardingStatusNotProvisioned,
	/** The CF service is not available in the serving network. */
		ECallForwardingStatusNotAvailable,
	/** The phone can not determine CF service status. */
		ECallForwardingStatusUnknown,
	/** The Call Forwarding service is active and the presentation indicator is set 
	to allowed */
		ECallForwardingStatusActivePIAllowed,
	/** The Call Forwarding service is active and the presentation indicator is set 
	to not available */
		ECallForwardingStatusActivePINotAvailable,
	/** The Call Forwarding service is active and the presentation indicator is set 
	to restricted
	
	The Call Forwarding service is active and the presentation indicator is set 
	to restricted and screening returns "User provided, verified and passed". */
		ECallForwardingStatusActivePIClir,
	/** The Call Forwarding service is active and the presentation indicator is set 
	to restricted and screening returns "User provided, not screened". */
		ECallForwardingStatusActivePIClirSIVerified,
	/** The Call Forwarding service is active and the presentation indicator is set 
	to "restricted" and screening returns "Network provided". */
		ECallForwardingStatusActivePIClirSINotScreened,
	/** The Call Forwarding service is active and the presentation indicator is set 
	to "restricted" and screening returns "Network provided". */
		ECallForwardingStatusActivePIClirSINetwork,
	/** The CF service is currently in the quiescent state. */	
		ECallForwardingStatusQuiescent
		};
		
	

	class TMobilePhoneCFInfoEntryV1 : public TMultimodeType
/** Defines information about the call forwarding service.

Modes: GSM/WCDMA

@see CMobilePhoneCFList
@publishedPartner
@released 
*/
		{
	public:
		IMPORT_C TMobilePhoneCFInfoEntryV1();
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
	/** The CF service of this list entry.
	
	@see TMobilePhoneCFCondition */
		TMobilePhoneCFCondition iCondition;
	/** The basic service group associated to this CF service list entry.
	
	@see TMobileService */
		TMobileService iServiceGroup;
	/** The current status of the call forwarding condition.
	
	@see TMobilePhoneCFStatus */
		TMobilePhoneCFStatus iStatus;
	/** The "forward-to" telephone number registered for the call forwarding condition. 
	An empty string if CF service is not registered, not provisioned or not available.
	
	@see TMobileAddress */
		TMobileAddress iNumber;
	/** The "No Reply" time-out (in seconds) registered for the call forwarding no reply 
	condition.
	
	Equals to -1 if this value is not applicable. In GSM mode, will be between 
	5 and 30 and in steps of 5 if this value is applicable. */
		TInt iTimeout; // valid for CFRNy only
		};

	IMPORT_C void NotifyCallForwardingStatusChange(TRequestStatus& aReqStatus, TMobilePhoneCFCondition& aCondition) const;

	class TMobilePhoneCFChangeV1 : public TMultimodeType
/** Defines the call forward information.

@see SetCallForwardingStatus()
@publishedPartner
@released 
*/
		{
	public:
		IMPORT_C TMobilePhoneCFChangeV1();
	public:
	/** The basic service group(s) to apply the CF status change to.
	
	@see TMobileService */
		TMobileService iServiceGroup;
	/** The service action for call forwarding.
	
	@see TMobilePhoneServiceAction */
		TMobilePhoneServiceAction iAction;
	/** A new "forward-to" number to be registered against the CF condition. This is 
	applicable if iAction=EServiceActionRegister. In other cases, it is set to
	an empty string.
	
	@see TMobileAddress */
		TMobileAddress iNumber;
	/** The new "No Reply" time-out in seconds to be registered. This is applicable 
	if the CF condition is CFNRy and iAction=EServiceActionRegister. Equals -1 
	if this value is not applicable. If applicable, in GSM mode will be between 
	5 and 30 and in steps of 5. */
		TInt iTimeout;
		};

	IMPORT_C void SetCallForwardingStatus(TRequestStatus& aReqStatus, TMobilePhoneCFCondition aCondition, const TMobilePhoneCFChangeV1& aInfo) const;

/** Call Forwarding Active Notification

Modes: Common */
	enum TMobilePhoneCFActive
		{
	/** CF Unconditional is currently active. */
		ECFUnconditionalActive,
	/** CF Busy, CF No Reply or CF Not Reachable is currently active. */
		ECFConditionalActive
		};

	IMPORT_C void NotifyCallForwardingActive(TRequestStatus& aReqStatus, TMobileService& aServiceGroup, TMobilePhoneCFActive& aActiveType) const;
	
	/**
	Current status indicators for unconditional call forwarding.
	*/
	enum TCFUIndicatorStatus
		{
		/**
		CFU Indicator status unknown.
		*/
		KCFUIndicatorUnknown = 0x0,
		/**
		CFU Indicator status for Voice (TMobileServiceGroup EVoice).
		*/
		KCFUIndicatorVoice=0x01,
		/**
		CFU Indicator status for Fax (TMobileServiceGroup EFax).
		*/
		KCFUIndicatorFax=0x02,
		/**
		CFU Indicator status for Data Teleservices(TMobileServiceGroup EData).
		*/
		KCFUIndicatorData=0x04,
		/**
		CFU Indicator status for Sms (TMobileServiceGroup ESms).
		*/
		KCFUIndicatorSms=0x08,
		/**
		CFU Indicator status for Data Bearer Services(TMobileServiceGroup EDataBearer).
		*/
		KCFUIndicatorDataBearer=0x10,
		/**
		CFU Indicator status for Auxiliary Voice (TMobileServiceGroup EAuxVoiceService).
		*/
		KCFUIndicatorAuxVoice=0x20
		};

	typedef TUint32 TCFUIndicatorStatusFlags;
		
	class TMobilePhoneCFUIndicatorV3 : public TMultimodeType
	/**
	Contains indicator parameters for unconditional call forwarding.
	*/
		{
	public:
		IMPORT_C TMobilePhoneCFUIndicatorV3();
	public:
		/**
		Current setting for indicator status.
		Can contain a number of flags from RMobilePhone::TCFUIndicatorStatus.
		*/
		TCFUIndicatorStatusFlags iIndicator;		
		};
		
	typedef TPckg<TMobilePhoneCFUIndicatorV3> TMobilePhoneCFUIndicatorV3Pckg;

	/**
	The MSP number contains the Profile Identity of the subscriber profile.
	The Profile Identity shall be any of the following enlisted.
	@see 3GPP TS 31.102 v7.4.1 section 4.2.64
	@see 3GPP TS 23.097 [36]
	*/
	enum TMultipleSubscriberProfileID
		{
		/**
		Profile Identity Unknown
		*/
		KProfileIdentityUnknown = 0x0,
		/**
		Profile Identity 1
		*/
		KProfileIdentityOne = 0x01,
		/**
		Profile Identity 2
		*/
		KProfileIdentityTwo = 0x02,
		/**
		Profile Identity 3
		*/
		KProfileIdentityThree = 0x04,
		/**
		Profile Identity 4
		*/
		KProfileIdentityFour = 0x08
		};

	class TMobilePhoneCFUIndicatorV6 : public TMobilePhoneCFUIndicatorV3
	/**
	Contains the indicator parameters for unconditional call forwarding.
	The TMobilePhoneCFUIndicatorV6 supports MSP number(Multiple Subscriber Profile) and Call Forwarding Number.
	Upto four different profiles can be provisioned against a subscriber using the MSP feature.
	@see 3GPP TS 31.102 v7.4.1 section 4.2.64
	*/
		{
	public:
		IMPORT_C TMobilePhoneCFUIndicatorV6();
	public:
		/**
		@see RMoblie::TMobileAddress
		*/
		TMobileAddress iCFNumber;
		/**
		The profile IDs allow unambiguous identification of each profile. 
		This will allow the subscriber to select the preferred profile for outgoing calls and for subscriber actions. 
		For terminating calls the profile ID shall be part of the notification of the profile.
		*/
		TMultipleSubscriberProfileID iMultipleSubscriberProfileID;
		};

	typedef TPckg<TMobilePhoneCFUIndicatorV6> TMobilePhoneCFUIndicatorV6Pckg;

	IMPORT_C void GetCallForwardingIndicator(TRequestStatus& aReqStatus, TDes8& aIndicator) const;

	/***********************************************************************************/
	//
	// Mobile Identity Service functional unit
	// 
	/***********************************************************************************/

/** Phone ID services (CLIP/CNIP/CLIR).

Modes: GSM/WCDMA */
	enum TMobilePhoneIdService
		{
	/** The identity service is unspecified */
		EIdServiceUnspecified,
	/** The caller's ID is presented to the called party. */
		EIdServiceCallerPresentation,
	/** The caller's ID is restricted to the called party. */
		EIdServiceCallerRestriction,
	/** The connected party's ID is presented to the calling party. */
		EIdServiceConnectedPresentation,
	/** The connected party's ID is restricted to the calling party. */
		EIdServiceConnectedRestriction,
	/** The caller's name is presented to the called party. */
		EIdServiceCallerName,
	/** The called party is presented with the caller's ID. This command enables a 
	called subscriber to get the called line identification of the called party 
	when receiving a mobile terminated call. - maps to +CDIP. */
		EIdServiceCalledPresentation
		};

/** Service status of the Phone ID services (CLIP/CLIR/CNIP).

Modes: GSM/WCDMA */
	enum TMobilePhoneIdServiceStatus
		{
	/** The interrogated Identity service is provisioned and active. This status is 
	used for all identity services and in the case of the CLIR service means that 
	it is permanently active with no subscription option set. */
		EIdServiceActivePermanent,
	/** This status is used for the CLIR service only and means that the service is 
	provisioned and active.
	
	The CLIR subscription option is set to "Temporary, default restricted", therefore 
	identity is always restricted unless over-ridden by the user during a call 
	set-up. */
		EIdServiceActiveDefaultRestricted,
	/** This status is used for the CLIR service only and means that the service is 
	provisioned and active.
	
	The CLIR subscription option is set to "Temporary, default allowed", therefore 
	identity is always allowed unless over-ridden by the user during a call set-up. */
		EIdServiceActiveDefaultAllowed,
	/** The interrogated Identity service is not provisioned. */
		EIdServiceNotProvisioned,
	/** The status of the interrogated Identity service is unknown. */
		EIdServiceUnknown
		};

	// for use by client-side API code and TSY only

	struct TIdServiceAndLocation
		{
		TMobilePhoneIdService iService;
		TMobileInfoLocation iLocation;
		};

	/** This function member retrieves the current status of the identity service specified.
	
	The phone will retrieve this service status information from the location 
	specified by the client.
	
	If the client has requested EInfoLocationCache but there is no valid status 
	information in the cache then the request will return KErrNotFound.
	
	If the client has requested EInfoLocationCachePreferred, the network will 
	be interrogated if the cache is empty. Whenever the network is interrogated, 
	the cache will be refreshed with the information from the network.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetIdentityServiceStatus) 
	to cancel a previously placed asynchronous GetIdentityServiceStatus() request.
	
	@param aReqStatus On return, KErrNone if successful.
	@param aService The identity service to be queried.
	@param aStatus On return, the service status.
	@param aLocation The location to use. */
	IMPORT_C void GetIdentityServiceStatus(TRequestStatus& aReqStatus, TMobilePhoneIdService aService, TMobilePhoneIdServiceStatus& aStatus, TMobileInfoLocation aLocation = EInfoLocationCachePreferred) const;
	
	/**
	Identifies default service status for provisioned identity services.
	*/
	enum TMobilePhoneIdServiceSetting
		{
		/** Sets the default setting to network default (unknown). */
		EIdServiceNetworkDefault,
		/** Sets the default setting to presentation restricted. */
		EIdServicePresentationRestricted,
		/** Sets the default setting to presentation allowed. */
		EIdServicePresentationAllowed
		};
		
	IMPORT_C void SetIdentityServiceStatus(TRequestStatus& aReqStatus, const TMobilePhoneIdService aService, const TMobilePhoneIdServiceSetting aSetting) const;
	IMPORT_C void NotifyIdentityServiceStatus(TRequestStatus& aReqStatus, const TMobilePhoneIdService aService, TMobilePhoneIdServiceStatus& aStatus) const;
	IMPORT_C void NotifyIdentitySuppressionRejected(TRequestStatus& aReqStatus) const;

	/***********************************************************************************/
	//
	// Mobile Call Barring Functional Unit
	//
	/***********************************************************************************/

/** Mobile Phone Call Baring Conditions.

Modes: Common */
	enum TMobilePhoneCBCondition
		{
	/** The barring program is unspecified */
		EBarUnspecified,
	/** All incoming calls to this subscriber are barred (BAIC). */
		EBarAllIncoming,
	/** All incoming calls to this subscriber are barred when roaming outside the home 
	PLMN country (BIC-roam). */
		EBarIncomingRoaming,
	/** All outgoing calls by this subscriber are barred (BAOC). */
		EBarAllOutgoing,
	/** All outgoing international calls by this subscriber are barred (BOIC). */
		EBarOutgoingInternational,
	/** All outgoing international calls except those directed to the home PLMN country 
	by this subscriber are barred (BOIC-ExHC). */
		EBarOutgoingInternationalExHC,
	/** All of above CB services. */
		EBarAllCases,
		EBarAllOutgoingServices,
		EBarAllIncomingServices
		};

/** Mobile Phone Call Barring Status and Information.

Modes: Common */
	enum TMobilePhoneCBStatus
		{
	/** The CB service is currently active and operative. */
		ECallBarringStatusActive,
	/** The CB service is currently deactivated or quiescent. */
		ECallBarringStatusNotActive,
	/** In GSM/WCDMA mode, the CB service is not provisioned. In CDMA mode, this value 
	has no meaning. */
		ECallBarringStatusNotProvisioned,
	/** In GSM/WCDMA mode, the CB service is not available in serving network. In CDMA
	mode, the CB service is not available in the phone. */
		ECallBarringStatusNotAvailable,
	/** The phone can not determine CB service status. */
		ECallBarringStatusUnknown
		};

	

	class TMobilePhoneCBInfoEntryV1 : public TMultimodeType
/** Defines information about the call barring service.

@see CMobilePhoneCBList
@publishedPartner
@released 
*/
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		IMPORT_C TMobilePhoneCBInfoEntryV1();
	public:
	/** The CB service of this list entry.
	
	Modes: Common
	
	@see TMobilePhoneCBCondition */
		TMobilePhoneCBCondition iCondition;
	/** The service group associated to this list entry.
	
	Modes: Common
	
	@see TMobileService */
		TMobileService iServiceGroup;
	/** The CB service status of this list entry.
	
	Modes: Common
	
	@see TMobilePhoneCBStatus */
		TMobilePhoneCBStatus iStatus;
		};
		
	class TMobilePhoneCBInfoEntryV3 : public TMobilePhoneCBInfoEntryV1
	/**
	V3 class extending TMobilePhoneCBInfoEntryV1 to add a comparison method parameter.
	*/
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		IMPORT_C TMobilePhoneCBInfoEntryV3();
	public:
		/**
		The comparison method associated with this CB entry.
		*/
		TUint8 iCompMethod;
		};
		
	class TMobilePhoneCBChangeV1 : public TMultimodeType
/** Mobile Phone Call Barring Change.

@see SetCallBarringStatus() 
*/
		{
	public:
		IMPORT_C TMobilePhoneCBChangeV1();
	public:
	/** The basic service group(s) to apply the CB status change to.
	
	@see TMobileService */
		TMobileService iServiceGroup;
	/** The service action for call barring.
	
	@see TMobilePhoneServiceAction */
		TMobilePhoneServiceAction iAction;
	/** Password required to perform the call barring service action.
	
	@see TMobilePassword */
		TMobilePassword iPassword;
		};

	IMPORT_C void SetCallBarringStatus(TRequestStatus& aReqStatus, TMobilePhoneCBCondition aCondition, const TMobilePhoneCBChangeV1& aInfo) const;
	IMPORT_C void NotifyCallBarringStatusChange(TRequestStatus& aReqStatus, TMobilePhoneCBCondition& aCondition) const;
	
	class TMobilePhonePasswordChangeV1 : public TMultimodeType
/** Mobile phone password change.

@see SetCallBarringPassword()
@see ChangeSecurityCode()
@publishedPartner
@released 
*/
		{
	public:
		IMPORT_C TMobilePhonePasswordChangeV1();
	public:
	/** Old password.
	
	Modes: Common
	
	@see TMobilePassword */
		TMobilePassword iOldPassword;
	/** New password.
	
	Modes: Common
	
	@see TMobilePassword */
		TMobilePassword iNewPassword;
		};

	IMPORT_C void SetCallBarringPassword(TRequestStatus& aReqStatus, const TMobilePhonePasswordChangeV1& aPassword) const;

	class TMobilePhonePasswordChangeV2 : public TMobilePhonePasswordChangeV1
/** Mobile phone password change version 2. 
*/
	{
	public:
		IMPORT_C TMobilePhonePasswordChangeV2();
	public:
	/** Verified password.
	
	Modes: Common
	
	@see TMobilePassword */
		TMobilePassword iVerifiedPassword;
	};

/** A typedef'd packaged TMobilePhonePasswordChangeV2 passing through a generic 
API function member. */
	typedef TPckg<TMobilePhonePasswordChangeV2> TMobilePhonePasswordChangeV2Pckg;

	IMPORT_C void SetSSPassword(TRequestStatus& aReqStatus, const TDesC8& aPassword, const TInt aService) const;
	
	IMPORT_C void GetCompMethodName(TRequestStatus& aReqStatus, TDes& aAlphaTag, const TUint8 aCmi) const;

	/***********************************************************************************/
	//
	// Mobile Call Waiting Functional Unit
	//
	/***********************************************************************************/

/** The mobile phone call waiting status.

Modes: GSM/WCDMA */
	enum TMobilePhoneCWStatus
		{
	/** The CW service is currently active and operative. */
		ECallWaitingStatusActive,
	/** The CW service is currently deactivated or quiescent. */
		ECallWaitingStatusNotActive,
	/** The CW service is not provisioned. */
		ECallWaitingStatusNotProvisioned,
	/** The CW service is not available in the serving network. */
		ECallWaitingStatusNotAvailable,
	/** The phone can not determine CW service status. */
		ECallWaitingStatusUnknown
		};

	

	class TMobilePhoneCWInfoEntryV1 : public TMultimodeType
/** Defines information about the call waiting service.

@see CMobilePhoneCWList 
*/
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		IMPORT_C TMobilePhoneCWInfoEntryV1();
	public:
	/** The service group associated to this CW service list entry.
	
	Modes: GSM/WCDMA
	
	@see TMobileService */
		TMobileService iServiceGroup;
	/** The CW service status of this list entry.
	
	Modes: GSM/WCDMA
	
	@see TMobilePhoneCWStatus */
		TMobilePhoneCWStatus iStatus;
		};

/** A typedef'd packaged TMobilePhoneCWInfoEntryV1 for passing through a generic 
API function member. */
	typedef TPckg<TMobilePhoneCWInfoEntryV1> TMobilePhoneCWInfoEntryV1Pckg;

	IMPORT_C void SetCallWaitingStatus(TRequestStatus& aReqStatus, TMobileService aServiceGroup, TMobilePhoneServiceAction aAction) const;
	IMPORT_C void NotifyCallWaitingStatusChange(TRequestStatus& aReqStatus, TDes8& aCWStatus) const;

	/***********************************************************************************/
	//
	// Mobile Call Completion Unit
	//
	/***********************************************************************************/

/** The Call Completion Busy Subscriber status of the phone.

Modes: GSM/WCDMA */
	enum TMobilePhoneCCBSStatus
		{
	/** User has CCBS provisioned and there is at least 1 (and up to 5) active CCBS 
	requests. */
		ECcbsActive,
	/** User has CCBS provisioned but there are currently no active CCBS requests. */
		ECcbsNotActive,
	/** User has not subscribed to the CCBS service. */
		ECcbsNotProvisioned,
	/** The CCBS service is not available in the serving network. */
		ECcbsNotAvailable,
	/** MS is unable to determine status of the CCBS service. */
		ECcbsUnknown
		};

/** CCBS recall events.

@see RMobilePhone::TMobilePhoneCCBSEntryV2 */
	enum TCCBSRecallEvent
		{
	/** Recall received. */
		ECcbsRecallReceived,
	/** Recall accepted. */
		ECcbsRecallAccepted,
	/** Recall refused. */
		ECcbsRecallRefused,
	/** Recall reason unspecified. */
		ECcbsRecallUnspecified
		};

	

	class TMobilePhoneCCBSEntryV1 : public TMultimodeType
/** Defines information about the call completion (CCBS) service.

@see CMobilePhoneCcbsList
@publishedPartner
@released 
*/
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		IMPORT_C TMobilePhoneCCBSEntryV1();
	public:
	/** The CCBS index - a number allocated by the network.
	
	Modes: GSM/WCDMA */
		TInt			iCcbsIndex;
	/** The basic service group (voice/data/fax) this CCBS request is applicable to.
	
	Modes: GSM/WCDMA
	
	@see TMobileService */
		TMobileService	iServiceGroup;
	/** The phone number of the remote party to whom the CCBS request is aimed.
	
	Modes: GSM/WCDMA
	
	@see TMobileAddress */
		TMobileAddress	iDestination;

		};

/** A typedef'd packaged TMobilePhoneCCBSEntryV1 for passing through a generic API 
function member. */
	typedef TPckg<TMobilePhoneCCBSEntryV1> TMobilePhoneCCBSEntryV1Pckg;


	class TMobilePhoneCCBSEntryV2 : public TMobilePhoneCCBSEntryV1
/** Defines information about the call completion (CCBS) service version 2. 
*/
		{
	public:
		IMPORT_C	TMobilePhoneCCBSEntryV2();
	public:
		TCCBSRecallEvent	iEvent;
		}; 
	
/** A typedef'd packaged TMobilePhoneCCBSEntryV2 for passing through a generic API 
function member. */
	typedef TPckg<TMobilePhoneCCBSEntryV2> TMobilePhoneCCBSEntryV2Pckg;
	
	IMPORT_C void GetCCBSStatus(TRequestStatus& aReqStatus, TMobilePhoneCCBSStatus& aCcbsStatus, TMobileInfoLocation aLocation = EInfoLocationCachePreferred) const;
	IMPORT_C void NotifyCCBSStatusChange(TRequestStatus& aReqStatus, TMobilePhoneCCBSStatus& aCcbsStatus) const;
	IMPORT_C void DeactivateCCBS(TRequestStatus& aReqStatus, TInt aIndex) const;
	IMPORT_C void NotifyCCBSRecall(TRequestStatus& aReqStatus, TDes8& aCCBSEntry) const;
	IMPORT_C void AcceptCCBSRecall(TRequestStatus& aReqStatus, TInt aIndex, TName& aCallName) const;
	IMPORT_C TInt RefuseCCBSRecall(TInt aIndex) const;

	/***********************************************************************************/
	//
	// Mobile Alternating Call Function Unit
	//
	/***********************************************************************************/

/** Alternating Call Capabilities

Modes: GSM */
	enum TMobilePhoneAlternatingCallCaps
		{
	/** Phone supports outgoing Voice/Data calls
	
	Modes: GSM */
		KCapsMOVoiceData = 0x00000001,
	/** Phone supports outgoing Voice Then Data calls.
	
	Modes: GSM */
		KCapsMOVoiceThenData = 0x00000002,
	/** Phone supports outgoing Voice/Fax calls.
	
	Modes: GSM/WCDMA */
		KCapsMOVoiceFax	= 0x00000004,
	/** Phone supports incoming Voice/Data calls.
	
	Modes: GSM */
		KCapsMTVoiceData = 0x00000008,
	/** Phone supports incoming Voice Then Data calls.
	
	Modes: GSM */
		KCapsMTVoiceThenData = 0x00000010,
	/** Phone supports incoming Voice/Fax calls

	Modes: GSM/WCDMA */
		KCapsMTVoiceFax = 0x00000020
		};

	IMPORT_C TInt GetAlternatingCallCaps(TUint32& aCaps) const;
	IMPORT_C void NotifyAlternatingCallCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const;

/** Alternating Call Mode.

Modes: Common */
	enum TMobilePhoneAlternatingCallMode
		{
	/** The alternating call mode is unspecified
	
	Modes: Common */
		EAlternatingModeUnspecified,
	/** Next call will use a single bearer service - default setting
	
	Modes: Common */
		EAlternatingModeSingle,
	/** Next call will use voice/data alternating bearer service. 
	
	Modes: GSM */
		EAlternatingModeVoiceData,
	/** Next call will use voice then data alternating bearer service.
	
	Modes: GSM */
		EAlternatingModeVoiceThenData,
	/** Next call will use voice/fax alternating bearer service. 
	
	Modes: GSM/WCDMA */
		EAlternatingModeVoiceFax
		};

	IMPORT_C TInt GetAlternatingCallMode(TMobilePhoneAlternatingCallMode& aMode, TMobileService& aFirstService) const;
	IMPORT_C void SetAlternatingCallMode(TRequestStatus& aReqStatus, TMobilePhoneAlternatingCallMode aMode, TMobileService aFirstService) const;
	IMPORT_C void NotifyAlternatingCallModeChange(TRequestStatus& aReqStatus, TMobilePhoneAlternatingCallMode& aMode,TMobileService& aFirstService) const;

	/***********************************************************************************/
	//
	// Mobile Alternate Line Service Functional Unit
	//
	/***********************************************************************************/

/** Alternate Line Primary settings.

Modes: GSM */
	enum TMobilePhoneALSLine
		{
	/** Current ALS line selection is the "Primary" voice line. */
		EAlternateLinePrimary,
	/** Current ALS line selection is the "Auxiliary" voice line. */
		EAlternateLineAuxiliary,
	/** ALS is supported by ME and SIM, but the TSY can not determine which line is 
	currently selected. */
		EAlternateLineUnknown,
	/** The ME, SIM or serving network does not support ALS. */
		EAlternateLineNotAvailable
		};

	IMPORT_C TInt GetALSLine(TMobilePhoneALSLine& aALSLine) const;
	IMPORT_C void SetALSLine(TRequestStatus& aReqStatus, TMobilePhoneALSLine aALSLine) const;
	IMPORT_C void NotifyALSLineChange(TRequestStatus& aReqStatus, TMobilePhoneALSLine& aALSLine) const;

	/***********************************************************************************/	
	//
	// Mobile Network Security Functional Unit
	//
	/***********************************************************************************/

/** Enumerated network security types.

Mode: Common */
	enum TMobilePhoneNetworkSecurity
		{
	/** The encryption level is NONE.
	
	Mode: Common */
		ECipheringOff,
	/** The encryption level is GSM.(standard encryption algorithms for 2nd Generation 
	Mobile networks).
	
	Mode: GSM */
		ECipheringGSM,
	/** The encryption level is WCDMA.(standard encryption algorithms for 3rd Generation 
	Mobile networks).
	
	Mode: WCDMA */
		ECipheringWCDMA,
	/** The encryption level is CDMA.(standard encryption algorithms for 3rd Generation 
	Mobile networks).
	
	Mode: CDMA 
	@deprecated 9.5
	*/
		ECipheringCDMA
		};

	IMPORT_C void GetNetworkSecurityLevel(TRequestStatus& aReqStatus, TMobilePhoneNetworkSecurity& aSecurity) const;
	IMPORT_C void NotifyNetworkSecurityLevelChange(TRequestStatus& aReqStatus, TMobilePhoneNetworkSecurity& aSecurity) const;
	
	/**
	Indicators for whether encryption status of the current call should be displayed.
	*/
	enum TMobileCallCipheringIndicator
		{
		/**
		The ME indication displaying the encryption status of the current call should be off.
		*/
		ECipheringDisplayOff,
		/**
		The ME indication displaying the encryption status of the current call should be on.
		*/
		ECipheringDisplayOn 
		};
		
    IMPORT_C void GetCipheringIndicatorStatus(TRequestStatus& aReqStatus, TMobileCallCipheringIndicator& aIndicatorStatus) const;
    IMPORT_C void NotifyCipheringIndicatorStatus(TRequestStatus& aReqStatus, TMobileCallCipheringIndicator& aIndicatorStatus) const;

	/***********************************************************************************/
	//
	// Mobile Cost Functional Unit
	//
	/***********************************************************************************/

/** Call Cost Capabilities.

Modes: GSM/WCDMA */
	enum TMobilePhoneCostCaps
		{
	/** Indicates that the phone supports calculation and display of current and accumulated 
	call cost on an information-only basis. (The AoCI service in GSM) 
	
	Modes: GSM/WCDMA */
		KCapsCostInformation = 0x00000001,
	/** Indicates that the phone supports calculation and display of current and accumulated 
	call cost in order to support the charging service. (The AoCC service in GSM). 


	Modes: Common */
		KCapsCostCharging = 0x00000002,
	/** Indicates that the phone supports clearing of cost meters (ACM or CCM) to zero.
	
	Modes: Common */
		KCapsClearCost = 0x00000004,
	/** Indicates that the phone supports setting the maximum allowed cost (ACMmax) 
	to a non-zero value. 
	
	Modes: Common */
		KCapsSetMaxCost = 0x00000008,
	/** Indicates that the phone supports setting the currency and price per unit.
	
	Modes: GSM/WCDMA */
		KCapsSetPuct = 0x00000010,
	/** Indicates that the phone currently has call cost information available that 
	the client can retrieve.
	
	Modes: Common */
		KCapsGetCost = 0x00000020,
	/** Indicates that the phone can notify the client when the call cost information 
	changes. 
	
	Modes: Common */
		KCapsNotifyCostChange = 0x00000040
		};

	IMPORT_C TInt GetCostCaps(TUint32& aCaps) const;
	IMPORT_C void NotifyCostCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const;

/** Clear Cost Meter.

Modes: Common */
	enum TMobilePhoneCostMeters
		{
	/** Clear the Current Call Meter. */
		EClearCCM,
	/** Clear the Accumulated Charge Meter. */
		EClearACM,
	/** Clear all cost meters */
		EClearAll
		};

	IMPORT_C void ClearCostMeter(TRequestStatus& aReqStatus, TMobilePhoneCostMeters aMeter) const;
	IMPORT_C void SetMaxCostMeter(TRequestStatus& aReqStatus, TUint aUnits) const;

	
	
	class  TMobilePhonePuctV1 : public TMultimodeType
/** Defines the contents of the price/unit and currency table (PUCT) on the SIM.

@see TMobilePhoneCostInfoV1
@see SetPuct() 
*/
		{
	public:
		IMPORT_C TMobilePhonePuctV1();
	public:
	/** The Price per Unit. This floating point value is used to adjust the pricing 
	units supplied by the network into real currency values. 
	
	Modes: GSM/WCDMA */
		TReal   iPricePerUnit;
	/** The three character currency indicator.
	
	Modes: GSM/WCDMA */
		TBuf<4> iCurrencyName;
		};

/** A typedef'd packaged TMobilePhonePuctV1 for passing through a generic API function 
member. */
	typedef TPckg<TMobilePhonePuctV1> TMobilePhonePuctV1Pckg;
	
	IMPORT_C void SetPuct(TRequestStatus& aReqStatus, const TDesC8& aPuct) const;
	
/** Cost Information

Modes: GSM/WCDMA */
	enum TMobilePhoneCostService
		{
	/** Phone supports call cost service but no calls are currently in progress so the 
	network support and user subscription is unknown. */
		ECostServiceUnknown,
	/** The network or the phone does not support any of the call cost services. The 
	call can be either in progress or not. If the user is subscribed to AoCI they 
	will be able to continue to make calls but will not see any charging information. 
	If the user is subscribed to AoCC then they will be prevented from making 
	chargeable calls. */
		ECostServiceNotAvailable,
	/** Call is in progress and a call cost service is active but the phone is unable 
	to determine whether it is an AoCI or AoCC service. */
		ECostServiceAvailable,
	/** Call is in progress and the call cost information (AoCI) service is currently 
	operating.
	
	Modes: GSM/WCDMA */
		ECostServiceInformation,
	/** Call is in progress and the call cost charging (AoCC) service is currently operating.
	
	Modes: GSM/WCDMA */
		ECostServiceCharging
		};

	

	class TMobilePhoneCostInfoV1 : public TMultimodeType
/** Defines the information related to current billing costs.

@see GetCostInfo()
@see NotifyCostInfoChange() 
*/
		{
	public:
		IMPORT_C TMobilePhoneCostInfoV1();
	public:
	/** The current status and type of call cost service.
	
	@see TMobilePhoneCostService */
		TMobilePhoneCostService iService;
	/** The current number of charging units in the Current Call Meter. This is set 
	to zero if reset by the user or at the start of call set-up attempt. */
		TUint iCCM;
	/** The current number of charging units in the Accumulated Call Meter. This equals 
	the accumulation of previous and current calls. */
		TUint iACM;
	/** The maximum number of charging units that the ACM can rise to before chargeable 
	calls are stopped. Equals zero if Service != ECostServiceCharging. */
		TUint iACMmax;
	/** The Price per Unit and Currency Table.
	
	@see TMobilePhonePuctV1 */
		TMobilePhonePuctV1 iPuct;
		};

/** A typedef'd packaged TMobilePhoneCostInfoV1 for passing through a generic API
function member. */
	typedef TPckg<TMobilePhoneCostInfoV1> TMobilePhoneCostInfoV1Pckg;

	IMPORT_C void GetCostInfo(TRequestStatus& aReqStatus, TDes8& aCostInfo) const;
	IMPORT_C void NotifyCostInfoChange(TRequestStatus& aReqStatus, TDes8& aCostInfo) const;

	/***********************************************************************************/
	//
	// Mobile Security Functional Unit
	//
	/***********************************************************************************/

/** Security Capabilities.

Modes: Common */
	enum TMobilePhoneSecurityCaps
		{
	/** The phone supports an overall "phone device lock" which can be enabled or disabled. 
	If enabled, user must enter phone password before the phone can be used. When 
	this capability is set the phone will support get lock info, notify lock info 
	change and set lock setting for phone device lock.
	
	Modes: Common */
		KCapsLockPhone = 0x0000001,
	/** The phone supports locking the ICC (using the PIN1 security code), which can 
	be enabled or disabled. If enabled, the user must enter PIN1 code before the 
	ICC can be used. When this capability is set the phone will support get lock 
	info, notify lock info change and set lock setting for ICC lock. 
	
	Modes: Common */
		KCapsLockICC = 0x00000002,
	/** The phone supports the "phone to ICC lock" which can be enabled or disabled. 
	If enabled, at power-up the phone will check whether current ICC is the one 
	the phone is locked to or not. If not, then the user must enter the phone
	password before phone can be used with this ICC. When this capability is set 
	the phone will support get lock info, notify lock info change and set lock 
	setting for Phone-ICC lock. 
	
	Modes: Common */
		KCapsLockPhoneToICC = 0x00000004,
	/** The phone supports the "phone to first ICC lock" which can be enabled or disabled. 
	If enabled, at power-up the phone will check whether current ICC is the one 
	the phone is first locked to or not. If not, then user must enter the phone 
	password before the phone can be used with this ICC. When this capability 
	is set the phone will support get lock info, notify lock info change and set 
	lock setting for Phone-First ICC lock.

	Modes: Common */
		KCapsLockPhoneToFirstICC = 0x00000008,
	/** The phone supports the "Over The Air" programming lock which can be enabled 
	or disabled. If enabled, the phone will prevent OTA programming by the network. 
	If disabled, the phone will allow OTA programming by the network. When this 
	capability is set the phone will support get lock info, notify lock info change 
	and set lock setting for OTA lock. 
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		KCapsLockOTA = 0x00000010,
	/** The phone supports access to the PIN 1 security code. When this capability is 
	set the phone will support verification of PIN1, unblocking PIN1, changing 
	PIN1 and notification of PIN1 required. 
	
	Modes: Common */
		KCapsAccessPin1 = 0x00000020,
	/** The phone supports access to the PIN 2 security code. When this capability is 
	set the phone will support verification of PIN2, unblocking PIN2, changing 
	PIN2 and notification of PIN2 required.
	
	Modes: GSM/WCDMA */
		KCapsAccessPin2 = 0x00000040,
	/** The phone supports access to the phone password security code. When this capability 
	is set the phone will support verification of the phone password, changing 
	the phone password and notification if the phone password is required. 
	
	Modes: Common */
		KCapsAccessPhonePassword = 0x00000080,
	/** The phone supports access to the SPC (Service Programming Code) security code. 
	This code may need to be verified before OTA can commence.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		KCapsAccessSPC = 0x00000100,
	/** The phone supports access to the hidden key. When this capability is set the
	phone will support verification of HiddenKey. 
	
	Modes: WCDMA */
		KCapsAccessHiddenKey=0x00000200,
	/** The phone supports access to the USIM Application PIN. An Application PIN allows 
	access to any file on the UICC where it is referenced in the access rules.
	
	Modes: WCDMA */
		KCapsAccessUSIMAppPin=0x00000400,
	/** The phone supports access to the second USIM Application PIN. The second Application 
	PIN allows access to files (where it is referenced in the access rules) within 
	one USIM Application on the UICC. This is a local PIN.
	
	Modes: WCDMA */
		KCapsAccessUSIMAppSecondPin =0x00000800,
	/** The phone supports access to the Universal PIN. A Universal PIN is a PIN that 
	is used in a multi-application UICC environment to allow several USIM applications 
	to share one common PIN.
	
	Modes: WCDMA */
		KCapsAccessUniversalPin =0x00001000
		};

	IMPORT_C TInt GetSecurityCaps(TUint32& aCaps) const;
	IMPORT_C void NotifySecurityCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const;

/** Lock Information.

Modes: Common */
	enum TMobilePhoneLock
		{
	/** Lock the whole device. Phone lock password is required to unlock when this lock 
	is enabled. 

	Modes: Common */
		ELockPhoneDevice,
	/** Lock PIN1 on the ICC. PIN1 security code is required to unlock when this lock 
	is enabled. 
	
	Modes: Common */
		ELockICC,
	/** Lock the phone to the current ICC inserted in the phone Phone lock password 
	is required to unlock when this lock is enabled.
	
	Modes: Common */
		ELockPhoneToICC,
	/** Lock the phone to the first ICC inserted in the phone. Phone lock password is 
	required to unlock when this lock is enabled.
	
	Modes: Common */
		ELockPhoneToFirstICC,
	/** Lock the OTA facility on the phone. SPC security code may be required to unlock 
	when this lock is enabled.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ELockOTA,
	/** Lock PIN2 on the ICC. PIN2 security code is required to unlock when this lock 
	is enabled.
	
	Modes: Common */
		ELockPin2,
	/** Lock the phonebook hidden entries on the phone. Hidden key security code may 
	be required to unlock when this lock is enabled.
	
	Modes: WCDMA */
		ELockHiddenKey,
	/** Lock the associated USIM Application. USIM Application PIN is required to unlock 
	when this lock is enabled.
	
	Modes: WCDMA */
		ELockUSimApp,
	/** Lock the associated USIM Application. The second USIM Application PIN is required 
	to unlock when this lock is enabled.
	
	Modes: WCDMA */
		ELockSecondUSimApp,
	/** Lock the associated USIM Application(s) that share this common lock. Universal 
	Pin is required to unlock when this lock is enabled.
	
	Modes: WCDMA */
		ELockUniversalPin,
	/** Lock for the NAM.  The SPC is required to unlock.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ELockNam
		};

/** Phone lock status.

Modes: Common */
	enum TMobilePhoneLockStatus
		{
	/** The status of the lock is unknown. */
		EStatusLockUnknown,
	/** Lock is closed - user can not access functionality governed by this lock until 
	user correctly enters associated security code. */
		EStatusLocked,
	/** Lock is open - user can access functionality governed by this lock */
		EStatusUnlocked,
		EStatusBlocked
		};

/** The phone lock setting.

Modes: Common */
	enum TMobilePhoneLockSetting
		{
	/** The setting of the lock is unknown
	
	Modes: Common */
		ELockSetUnknown,
	/** Lock is enabled - associated security code will be required to unlock the 
	lock, the next time the lock's status is EStatusLocked. 
	
	Modes: Common */
		ELockSetEnabled,
	/** Lock is disabled - so its status is always EStatusUnlocked.
	
	Modes: Common */
		ELockSetDisabled,
	/** Lock of the current Application is replaced by the Universal PIN. Only the 
	Universal PIN is allowed as a replacement.

	Modes: WCDMA */
		ELockReplaced
		};

	

	class TMobilePhoneLockInfoV1 : public TMultimodeType
/** Defines the status of a phone lock.

@see GetLockInfo()
@see NotifyLockInfoChange()
@publishedPartner
@released
*/
		{
	public:
		IMPORT_C TMobilePhoneLockInfoV1();
	public:
	/** The current status of the lock.
	
	Modes: Common
	
	@see TMobilePhoneLockStatus */
		TMobilePhoneLockStatus iStatus;
	/** The current setting of the lock.
	
	Modes: Common
	
	@see TMobilePhoneLockSetting */
		TMobilePhoneLockSetting iSetting;
		};

/** A typedef'd packaged TMobilePhoneLockInfoV1 for passing through a generic API 
function member. */
	typedef TPckg<TMobilePhoneLockInfoV1> TMobilePhoneLockInfoV1Pckg;

	IMPORT_C void GetLockInfo(TRequestStatus& aReqStatus, TMobilePhoneLock aLock, TDes8& aLockInfo) const;
	IMPORT_C void NotifyLockInfoChange(TRequestStatus& aReqStatus, TMobilePhoneLock& aLock, TDes8& aLockInfo) const;
	IMPORT_C void SetLockSetting(TRequestStatus& aReqStatus, TMobilePhoneLock aLock, TMobilePhoneLockSetting aSetting) const;

/** Change Security Code.

Modes: Common */
	enum TMobilePhoneSecurityCode
		{
	/** PIN 1 code (between 4-8 digits).

	Modes: Common */
		ESecurityCodePin1,
	/** PIN 2 code (between 4-8 digits)
	
	Modes: GSM/WCDMA */
		ESecurityCodePin2,
	/** PIN 1 unblocking code (always 8 digits).
	
	Modes: Common */
		ESecurityCodePuk1,
	/** PIN 2 unblocking code (always 8 digits)
	
	Modes: GSM/WCDMA */
		ESecurityCodePuk2,
	/** Password used for Phone-SIM Lock and Phone Device Lock (up to 8 digits) 
	
	Modes: Common */
		ESecurityCodePhonePassword,
	/** Service Programming Code (SPC)
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ESecurityCodeSPC,
	/** Phonebook key to display the hidden entries.
	
	Modes: WCDMA */
		ESecurityHiddenKey,
	/** USIM Application PIN.
	
	Modes: WCDMA */
		ESecurityUSIMAppPin,
	/** Second USIM Application PIN.
	
	Modes: WCDMA */
		ESecuritySecondUSIMAppPin,
	/** Universal PIN.
	
	Modes: WCDMA */
		ESecurityUniversalPin,
	/** Universal PIN unblocking code (always 8 digits).
	
	Modes: Common */
		ESecurityUniversalPuk
		};

	IMPORT_C void ChangeSecurityCode(TRequestStatus& aReqStatus, TMobilePhoneSecurityCode aType, const TMobilePhonePasswordChangeV1& aChange) const;

	/** Defines the status of a security code.

	@see GetSecurityCodeInfo()
	@see NotifySecurityCodeInfoChange()
	@publishedPartner
	@released
	*/
	class TMobilePhoneSecurityCodeInfoV5 : public TMultimodeType
		{
	public:
		IMPORT_C TMobilePhoneSecurityCodeInfoV5();
	public:
	/** Number of remaining security code entry attempts before the corresponding lock gets blocked
	
	Modes: Common
	*/
		TInt iRemainingEntryAttempts;
		};
	
	/** A typedef'd packaged TMobilePhoneSecurityCodeInfoV5 for passing through a generic API
	function member. */
	typedef TPckg<TMobilePhoneSecurityCodeInfoV5> TMobilePhoneSecurityCodeInfoV5Pckg;
	
	IMPORT_C void GetSecurityCodeInfo(TRequestStatus& aReqStatus, TMobilePhoneSecurityCode aSecurityCode, TDes8& aSecurityCodeInfo) const;
	IMPORT_C void NotifySecurityCodeInfoChange(TRequestStatus& aReqStatus, TMobilePhoneSecurityCode& aSecurityCode, TDes8& aSecurityCodeInfo) const;

/** Notify Security Events.

Modes: Common */
	enum TMobilePhoneSecurityEvent
		{
	/** The phone has detected that there is no ICC present. 
	
	Modes: Common */
		ENoICCFound,
	/** The phone has detected that the ICC is unusable. 
	
	Modes: Common */
		EICCTerminated,
	/** The phone requires PIN1 to be entered. 
	
	Modes: Common */
		EPin1Required,
	/** The phone requires PUK1 to be entered.
	
	Modes: Common */
		EPuk1Required,
	/** The phone requires PIN2 to be entered.
	
	Modes: GSM/WCDMA */
		EPin2Required,
	/** The phone requires PUK2 to be entered.
	
	Modes: GSM/WCDMA */
		EPuk2Required,
	/** The phone requires the Phone Password to be entered. 
	
	Modes: Common */
		EPhonePasswordRequired,
	/** The phone requires the SPC to be entered. 
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ESPCRequired,
	/** The PIN1 verification procedure has passed. 
	
	Modes: Common */
		EPin1Verified,
	/** The PIN2 verification procedure has passed.

	Modes: GSM/WCDMA */
		EPin2Verified,
	/** The PUK1 unblocking procedure has passed.
	
	Modes: Common */
		EPuk1Verified,
	/** The PUK2 unblocking procedure has passed. 
	
	Modes: GSM/WCDMA */
		EPuk2Verified,
	/** The Phone Password has been verified. 
	
	Modes: Common */
		EPhonePasswordVerified,
	/** The SPC verification procedure has passed.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ESPCVerified,
	/** The phone requires the phonebook hidden key to be entered.
	
	Modes: WCDMA */
		EHiddenKeyRequired,
	/** The phonebook hidden key procedure has passed.
	
	Modes: WCDMA */
		EHiddenKeyVerified,
	/** The phone requires the currently active USIM Application PIN to be entered.
	
	Modes: WCDMA */
		EUSIMAppPinRequired,
	/** The currently active USIM Application PIN procedure has passed.
	
	Modes: WCDMA */
		EUSIMAppPinVerified,
	/** The phone requires the currently active USIM Application second PIN to be entered.
	
	Modes: WCDMA */
		ESecondUSIMAppPinRequired,
	/** The currently active USIM Application second PIN procedure has passed.
	
	Modes: WCDMA */
		ESecondUSIMAppPinVerified,
	/** The phone requires the Universal PIN to be entered.
	
	Modes: WCDMA */
		EUniversalPinRequired,
	/** The currently active Universal PIN procedure has passed.
	
	Modes: WCDMA */
		EUniversalPinVerified,
	/** The Service Programming Code has been changed.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		ESPCChanged,
	/** The phone requires Universal PUK to be entered.
	
	Modes: WCDMA */
		EUniversalPukRequired,
	/** The Universal PUK unblocking procedure has passed.
	
	Modes: WCDMA */
		EUniversalPukVerified
		};

	IMPORT_C void NotifySecurityEvent(TRequestStatus& aReqStatus, TMobilePhoneSecurityEvent& aEvent) const;

	// for use by client-side API code and TSY only

	struct TCodeAndUnblockCode
		{
		TMobilePassword iCode;
		TMobilePassword iUnblockCode;
		};	

	IMPORT_C void VerifySecurityCode(TRequestStatus& aReqStatus, TMobilePhoneSecurityCode aType,
		const TMobilePassword& aCode, const TMobilePassword& aUnblockCode) const;
	IMPORT_C TInt AbortSecurityCode(TMobilePhoneSecurityCode aType) const;

	/***********************************************************************************/	
	// 
	// MobileMessageWaiting
	//
	/***********************************************************************************/

/** Display indicators. */
	enum TMobilePhoneIndicatorDisplay
		{
	/** Display voicemail active. */
		KDisplayVoicemailActive = 0x01,
	/** Display fax active. */
		KDisplayFaxActive = 0x02,
	/** Display E-mail active. */
		KDisplayEmailActive = 0x04,
	/** Display other active. */
		KDisplayOtherActive = 0x08,
	/** Display Aux voicemail active. */
		KDisplayAuxVoicemailActive = 0x10,
	/** Display data active. */
		KDisplayDataActive = 0x20,
	/** Display Videomail active. */
		KDisplayVideomailActive = 0x40
		};

	class TMobilePhoneMessageWaitingV1 : public TMultimodeType
/** Message Waiting Indicators. See 3GPP TS 31 102.

@see GetIccMessageWaitingIndicators()
@see SetIccMessageWaitingIndicators()
@see NotifyIccMessageWaitingIndicatorsChange() 
*/
		{
	public:
		IMPORT_C TMobilePhoneMessageWaitingV1();
	public:
	/** The flags for each indicator from TMobilePhoneIndicatorDisplay to specify whether 
	the indicator should be displayed or not. E.g. Messages of the specified type are waiting.

	Modes: Common */
		TUint8	iDisplayStatus;
	/** The number of voicemail messages waiting for the "main" voice line.
	
	Modes: Common */
		TUint8	iVoiceMsgs;
	/** The number of voicemail messages waiting for the "auxiliary" voice line.
	
	Modes: GSM */
		TUint8	iAuxVoiceMsgs;
	/** The number of messages waiting for the data line.
	
	Modes: Common */
		TUint8	iDataMsgs;
	/** The number of messages waiting for the fax line.
	
	Modes: Common */
		TUint8	iFaxMsgs;
	/** The number of email messages waiting.
	
	Modes: WCDMA */
		TUint8	iEmailMsgs;
	/** Contains the number of "unspecified" messages waiting.
	
	Modes: WCDMA */
		TUint8	iOtherMsgs;
		};

/** A typedef'd packaged TMobilePhoneMessageWaitingV1 for passing through a generic 
API function member. */
	typedef TPckg<TMobilePhoneMessageWaitingV1> TMobilePhoneMessageWaitingV1Pckg;
	
	class TMobilePhoneMessageWaitingV8 : public TMobilePhoneMessageWaitingV1
/** Message Waiting Indicators. See Section 4.2.63 of 3GPP TS 31 102 V6.18.0 .
@see GetIccMessageWaitingIndicators()
@see SetIccMessageWaitingIndicators()
@see NotifyIccMessageWaitingIndicatorsChange() 
*/
		{
	public:
		IMPORT_C TMobilePhoneMessageWaitingV8();
	public:
	/** 
	Contains the number of video messages waiting.
	
	Modes: Common */
		TUint8	iVideoMsgs;
		};

/** A typedef'd packaged TMobilePhoneMessageWaitingV8 for passing through a generic 
API function member. */
	typedef TPckg<TMobilePhoneMessageWaitingV8> TMobilePhoneMessageWaitingV8Pckg;

	IMPORT_C void GetIccMessageWaitingIndicators(TRequestStatus& aReqStatus, TDes8& aMsgIndicators) const;
	IMPORT_C void SetIccMessageWaitingIndicators(TRequestStatus& aReqStatus, const TDesC8& aMsgIndicators) const;
	IMPORT_C void NotifyIccMessageWaitingIndicatorsChange(TRequestStatus& aReqStatus, TDes8& aMsgIndicators) const;

	IMPORT_C void NotifyMessageWaiting(TRequestStatus& aReqStatus, TInt& aCount) const;

	/***********************************************************************************/
	//
	// Mobile Fixed Dialling Numbers Functional Unit
	//
	/***********************************************************************************/

/** Fixed Dialling Numbers Status.

Modes: Common */
	enum TMobilePhoneFdnStatus
		{
	/** Fixed Dialling Number mode is supported but is not currently active. */
		EFdnNotActive,
	/** Fixed Dialling Number mode is supported and is currently active. Deactivating 
	FDN mode is supported. */
		EFdnActive,
	/** Fixed Dialling Number mode is supported and is permanently active. Deactivating 
	FDN mode is NOT supported with this SIM. */
		EFdnPermanentlyActive,
	/** Fixed Dialling Number mode is not supported by ME and/or SIM. */
		EFdnNotSupported,
	/** ME is currently unable to determine the status of FDN mode. */
		EFdnUnknown
		};

	IMPORT_C TInt GetFdnStatus(TMobilePhoneFdnStatus& aFdnStatus) const;
	IMPORT_C void GetFdnStatus(TRequestStatus& aReqStatus, TMobilePhoneFdnStatus& aFdnStatus) const;

/** Fixed Dialling Numbers Setting

Modes: Common */
	enum TMobilePhoneFdnSetting
		{
	/** Activate the FDN service. */
		EFdnSetOn,
	/** Deactivate the FDN service. */
		EFdnSetOff
		};

	IMPORT_C void SetFdnSetting(TRequestStatus& aReqStatus, TMobilePhoneFdnSetting aFdnSetting) const;
	IMPORT_C void NotifyFdnStatusChange(TRequestStatus& aReqStatus, TMobilePhoneFdnStatus& aFdnStatus) const;

	/***********************************************************************************/
	//
	// Multicall bearer settings
	//
	/***********************************************************************************/

	class TMobilePhoneMulticallSettingsV1 : public TMultimodeType
/** Multi call settings for version v1.0.

@see GetMulticallParams()
@see NotifyMulticallParamsChange() 
@publishedPartner
@released*/
		{
	public:
		IMPORT_C TMobilePhoneMulticallSettingsV1();
	public:
	/** Maximum number of simultaneous CS bearers allowed - defined by the user. 
	
	Modes: WCDMA */
		TInt iUserMaxBearers;
	/** Maximum number of simultaneous bearers allowed - defined by the service provider.
	
	Modes: WCDMA */
		TInt iServiceProviderMaxBearers;
	/** Maximum number of simultaneous CS bearers supported by the network.
	
	Modes: WCDMA */
		TInt iNetworkSupportedMaxBearers;
	/** Maximum number of simultaneous CS bearers supported by the UE.
	
	Modes: WCDMA */
		TInt iUESupportedMaxBearers;
		};

/** A typedef'd packaged TMobilePhoneMulticallSettingsV1 for passing through a generic 
API function member. */
	typedef TPckg<TMobilePhoneMulticallSettingsV1> TMobilePhoneMulticallSettingsV1Pckg;

	IMPORT_C void GetMulticallParams(TRequestStatus& aReqStatus, TDes8& aMulticallParams) const;
	IMPORT_C void SetMulticallParams(TRequestStatus& aReqStatus, TInt aUserMaxBearers) const;
	IMPORT_C void NotifyMulticallParamsChange(TRequestStatus& aReqStatus, TDes8& aMulticallParams) const;
	
	enum TMobilePhoneMulticallIndicator
	/**
	Indicators for exceeding allowed multiple active bearers.
	*/
		{
		/** The maximum number of multiple active bearers allowed by the service provider has been exceeded. */
		EBearerNetworProviderExceeded,
		/** The maximum number of multiple active bearers allowed by the mobile user  has been exceeded. */
		EBearerUserExceeded
		};
		
	IMPORT_C void NotifyMulticallIndicatorChange(TRequestStatus& aReqStatus, TMobilePhoneMulticallIndicator& aMulticallIndicator) const;

	/***********************************************************************************/
	//
	// MobileNextIncomingCall Functional Unit
	//
	/***********************************************************************************/

/** Incoming Call Types.

Modes: Common */
	enum TMobilePhoneIncomingCallType
		{
	/** MS will receive any type of incoming call.
	
	Modes: Common */
		EIncomingTypeNotSpecified,
	/** MS will request voice bearer for incoming calls.
	
	Modes: Common */
		EIncomingVoice,
	/** MS will request fax bearer for incoming calls 
	
	Modes: Common */
		EIncomingFax,
	/** MS will request data bearer for incoming calls
	
	Modes: Common */
		EIncomingData,
	/** MS will request a data bearer for use by a Multimedia CS call (voice fall back 
	is acceptable) for incoming calls.
	
	Modes: GSM/WCDMA */
		EIncomingMultimediaVoiceFallback,
	/** MS will request a data bearer for use by a Multimedia CS call (voice fall back 
	NOT acceptable) for incoming calls 
	
	Modes: GSM/WCDMA */
		EIncomingMultimediaNoFallback,
	/** The single numbering scheme call mode is alternating voice to fax.
	
	Modes: GSM/WCDMA */
		EIncomingAlternatingVoiceFax,
	/** The single numbering scheme call mode is alternating voice to data.
	
	Modes: GSM */
		EIncomingAlternatingVoiceData,
	/** The single numbering scheme call mode is alternating fax to voice.
	
	Modes: GSM */
		EIncomingAlternatingFaxVoice,
	/** The single numbering scheme call mode is alternating data to voice.
	
	Modes: GSM */
		EIncomingAlternatingDataVoice,
	/** The single numbering scheme call mode is alternating voice then data.
	
	Modes: GSM */
		EIncomingAlternatingVoiceThenData
		};

	IMPORT_C void GetIncomingCallType(TRequestStatus& aReqStatus, TMobilePhoneIncomingCallType& aCallType, TDes8& aDataParams) const;
	IMPORT_C void SetIncomingCallType(TRequestStatus& aReqStatus, TMobilePhoneIncomingCallType aCallType, TDes8& aDataParams) const;
	IMPORT_C void NotifyIncomingCallTypeChange(TRequestStatus& aReqStatus, TMobilePhoneIncomingCallType& aCallType, TDes8& aDataParams) const;

	/***********************************************************************************/
	//
	// User-To-User Signalling Functional Unit
	//
	/***********************************************************************************/

/** UUS Settings Requests.

Modes: GSM/WCDMA */
	enum TMobilePhoneUUSSetting         // UUS settings of the phone
		{
	/** UUS is not specified. */
		EIncomingUUSNotSpecified,
	/** MS will accept incoming UUS activation request. */
		EIncomingUUSAccepted,
	/** MS will reject incoming UUS activation request. */
		EIncomingUUSRejected
		};

	IMPORT_C void GetUUSSetting(TRequestStatus& aReqStatus, TMobilePhoneUUSSetting& aSetting) const;
	IMPORT_C void SetUUSSetting(TRequestStatus& aReqStatus, TMobilePhoneUUSSetting aSetting) const;
	IMPORT_C void NotifyUUSSettingChange(TRequestStatus& aReqStatus, TMobilePhoneUUSSetting& aSetting) const;
	
	/***********************************************************************************/
	//
	// Mobile Smart Card Applications Functional Unit
	//
	/***********************************************************************************/
	
	
	enum { KSmartCardApplicationLabelMaxSize=32 };
	
	/** Smart card application info version v5.0. 
	Modes: Common

	@see GetSmartCardApplicationInfo()
	@publishedPartner
	@released
	*/
	class TSmartCardApplicationInfoV5 : public TMultimodeType
		{
	public:
		IMPORT_C TSmartCardApplicationInfoV5();
	public:
		/** 
		Smart card application identifier.
		*/
		TAID iAID;
		/** 
		Smart card application label. 16 bit USC2 characters string provided by the application 
		provider to be shown to the user for information, e.g. operator name. It is 
		recommended by ETSI/3GPP spec that the application label does not exceed 32 
		bytes.
		
		The application label is optional. If no label is provided, the buffer should 
		be NULL.
		 */
		TBuf8<KSmartCardApplicationLabelMaxSize> iLabel;
		};

	/**
	A typedef'd packaged TSmartCardApplicationInfoV5 for passing through a generic API 
	function member. 
	*/	
	typedef TPckg<TSmartCardApplicationInfoV5> TSmartCardApplicationInfoV5Pckg;

	/**
	Specifies the max length of the buffer used to retrieve the smart
	card application's supported EAP method list.  Value 255 since EAP
	type list length fits into one byte (see table 5.1 of ETSI TS
	102.310 v6.2.0).

	@see RMobilePhone::GetSmartCardApplicationInfo()
	@see RMobilePhone::TUiccApplicationEapInfoV6::iEapTypeList
	*/
	enum { KEapSupportedTypeListSize = 255 };

	/**
	UICC application info version v6.0.  Is a
	TSmartCardApplicationInfoV5 object, but has an additional buffer
	(iEapTypeList) for retrieving a list supported EAP methods on the
	application.

	Modes: GSM/WCDMA

	@see RMobilePhone::GetSmartCardApplicationInfo()
	@publishedPartner
	@released
	*/
	class TUiccApplicationEapInfoV6 : public TSmartCardApplicationInfoV5
		{
	public:
		IMPORT_C TUiccApplicationEapInfoV6();

	public:
		/**
		Application EAP support type list, defining the EAP methods
		supported by the UICC application (see RFC 3748 and
		http://www.iana.org/assignments/eap-numbers).
		
		As reference, also worth seeing section 5.2 of ETSI TS 102.310
		v6.2.0 and section 6.2 of RFC3748.

		@see KETelSmartCardEapTypeMD5
		@see KETelSmartCardEapTypeTLS
		@see KETelSmartCardEapTypeSIM
		@see KETelSmartCardEapTypeAKA
		@see KETelSmartCardEapTypeTTLS
		*/
		TBuf8<KEapSupportedTypeListSize> iEapTypeList;	
		};

	/**
	A typedef'd packaged TEapUiccApplicationInfoV6 for passing through
	a generic API function member.

	@see RMobilePhone::GetSmartCardApplicationInfo()
	*/	
	typedef TPckg<TUiccApplicationEapInfoV6> TUiccApplicationEapInfoV6Pckg;

	/**
	Smart card application notification events
	
	Modes: Common
	*/	
	enum TSmartCardApplicationEvent
		{
		/**
		Smart card application has been installed.
		*/
		EScApplicationInstalled,
		/**
		Smart card application has been removed.
		*/
		EScApplicationRemoved,
		/**
		Smart card application has been activated.
		*/
		EScApplicationActivated,
		/**
		Smart card application has been terminated.
		*/
		EScApplicationTerminated
		};

	/**
	Smart card application status.

	Modes: Common
	*/
	enum TSmartCardApplicationAction
		{
		/**
		Smart card application will be activated.
		*/
		EScApplicationActivate,
		/** 
		Smart card application will be terminated.
		*/
		EScTerminate
		};

	/**
	Smart card application file types. For definitions see TS 102.221.
	
	Modes: Common
	*/		
	enum TScFileType
		{
		/**
		Smart card file type is unknown.
		*/
		EUnknown,
		/**
		Smart card file type is linear fixed. 
		
		This is a record-based file.
		*/
		ELinearFixed,
		/**
		Smart card file type is cyclic. 
		
		This is a record-based file.
		*/
		ECyclic,
		/**
		Smart card file type is transparent. 
		
		This is a binary file.
		*/
		ETransparent
		};

	/**
	Typedef to hold a 2-byte file identifier.
	*/
	typedef TUint16 TScFileID;

	/** Smart card application file info version v5.0.
	Modes: Common

	@see GetScFileInfo()
	@publishedPartner
	@released
	*/
	class TScFileInfoV5 : public TMultimodeType
		{
	public:
		IMPORT_C TScFileInfoV5();
	public:
		/**
		Smart card file identifier.
		*/
		TScFileID iFileID;
		/**
		The type of the smart card file. Options are
		Linear Fixed, Cyclic or Transparent.
		*/
		TScFileType iType;
		/**
		Total length of the file.  As specified in TS 102.221 the maximum length of an EF is 0xFFF bytes.
		*/
		TInt iTotalLength;
		/**
		If the file is a record based file (e.g. file type is ELinearFixed or ECyclic) then this gives the fixed length of a record in the file.
		If file type is ETransparent then this value is unused and will be set to 0. 
		
		From TS 102.221 the maximum length of a record in an EF is 255 bytes.
		*/
		TInt iRecordLength;
		/**
		If the file is a record based file (e.g. file type is ELinearFixed or ECyclic) then this gives the number of records in the file.
		If file type is ETransparent then this value is unused and will be set to 0. 
		
		From TS 102.221 the maximum number of records in an EF is 254.
		*/
		TInt iNumberRecords;
		};
	
	
	/** A typedef'd packaged TScFileInfoV5 for passing through a generic API 
	function member. 
	*/
	typedef TPckg<TScFileInfoV5> TScFileInfoV5Pckg;
	
	/**
	This based on the maximum number of bytes in a command PDU 
	as defined in TS 102.221 however the actual path string is very unlikely to ever be this big.
	
	Modes: Common
	*/
	enum{ KMaxScFilePathLength = 256};
	
	/**
	A typedef'd buffer to hold the path to an EF file.  
	*/
	typedef TBuf8<KMaxScFilePathLength> TScFilePathReference;
	
	
	 /** 
	 Class that holds a smart card applications AID and associated file path string.

	 Modes: Common

	 @see GetScFileInfo()
	 @publishedPartner
	 @released
	 */
	 class TScFilePath
		{
	 public:
		IMPORT_C TScFilePath();
     public:
     	/** 
     	AID of application. This may be empty implying the iPath is a path reference starting at the master file (MF).
     	*/
		TAID iAID;
		/**
		Full path reference to file.  
		
		The iPath must define the file path using full path referencing. This is because the client does not know 
		what is the currently selected EF or DF and it is therefore necessary to select the file by full
		path referencing from the master file.  
		
		If iAID value is empty then the iPath reference starts from the MF file but should not include the file ID of the MF (3F00) itself.
		E.g. To reference the EF_DIR - iAID would be empty and iPath would be set to "2F00".
		E.g. To refefence the EF_SMS under DF_TELECOM on a 2G SIM ICC - iAID would be empty and iPath would be set to "7F106F3C".
		
		If iAID is non-empty then it refers to the AID of the application and iPath is a path reference to a file in that application,
		In this situation the path reference may start with the implicit FID of the application DF (7FFF).  
		E.g.  The path to reference to the EF_SMS file in a USIM application using implicit ADF FID would 
		be "7FFF6F3C".
		
		See TS102.221 section 8.4.2 for more information.  
		*/
		TScFilePathReference iPath;
		};
		
	/** Smart card application file identifier and access offsets.  This class is used to identify a file and the 
	location in a file to either read or write. Inherits attributes from TScFilePath which provide the 
	parent AID and full path reference.
	
	Modes: Common

	@see TScFilePath
	@see ReadScFile()
	@see UpdateScFile()
	@publishedPartner
	@released
	*/
	class TScFilePathWithAccessOffsets : public TScFilePath
		{
	public:
		IMPORT_C TScFilePathWithAccessOffsets();
	public:
		/**
		The type of the smart card file. Options are
		Linear Fixed, Cyclic or Transparent.
		*/
		TScFileType iType;
		/**
		If file type is ELinearFixed or ECyclic then this is the starting record index to read or update.
		This must be less than the number of records in the file.

		If file type is ETransparent then this is the offset (in bytes) from the start of the transparent file to read or update.
		This must be less than the total file size.
		*/
		TUint iStartOffset;
		/**
		If the file type is ELinearFixed or ECyclic then this is unused.
		NOTE: A call to ReadScFile or UpdateScFile for a record based file type will only read or update
		one record as described in TS 102.221 READ RECORD and UPDATE RECORD.

		If file type is ETransparent then this is the number of bytes to read or update.
		*/
		TUint iCount;
		};

	IMPORT_C void EnumerateSmartCardApplications(TRequestStatus& aReqStatus, TUint& aCount) const;
	IMPORT_C void GetSmartCardApplicationInfo(TRequestStatus& aReqStatus, TUint aAppIndex, TDes8& aInfo) const;
	IMPORT_C void NotifySmartCardApplicationInfoChange(TRequestStatus& aReqStatus, TAID& aAID, TSmartCardApplicationEvent& aEvent) const;
	IMPORT_C void SetSmartCardApplicationStatus(TRequestStatus& aReqStatus, const TAID& aAID, TSmartCardApplicationAction aAppAction) const;
	IMPORT_C void GetScFileInfo(TRequestStatus& aReqStatus, const TScFilePath& aFilePathId, TDes8& aInfo) const;
	IMPORT_C void ReadScFile(TRequestStatus& aReqStatus, const TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8& aReadBuffer) const;
	IMPORT_C void UpdateScFile(TRequestStatus& aReqStatus, const TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8& aUpdateBuffer) const;
	
	/***********************************************************************************/
	//
	// Mobile USIM Applications Functional Unit
	//
	/***********************************************************************************/

	enum { KUSimApplicationLabelMaxSize=32 };

	IMPORT_C void EnumerateUSimApplications(TRequestStatus& aReqStatus, TInt& aCount, TAID& aActiveAID) const; //deprecated 9.1

	class TUSimApplicationInfoV2 : public TMultimodeType
	/** USIM application information for version v2.0. 
		
	@deprecated 9.1 Use @see TSmartCardApplicationInfoV5 instead.
	*/
		{
	public:
		IMPORT_C TUSimApplicationInfoV2();
	public:
	/** USIM Application identifier.

	Modes: WCDMA */
		TAID								iAID;
	/** USIM Application label. 16 bit USC2 characters string provided by the application 
	provider to be shown to the user for information, e.g. operator name. It is 
	recommended by ETSI/3GPP spec that the application label does not exceed 32 
	bytes.
	
	The application label is optional. If no label is provided, the buffer should 
	be NULL.
	
	Modes: WCDMA */
		TBuf8<KUSimApplicationLabelMaxSize> iLabel; 
	};

	/** A typedef'd packaged TUSimApplicationInfoV2 for passing through a generic API 
	function member. 

	@deprecated 9.1 Use @see TSmartCardApplicationInfoV5 instead.
	*/
	typedef TPckg<TUSimApplicationInfoV2> TUSimApplicationInfoV2Pckg;

	IMPORT_C void GetUSimApplicationInfo(TRequestStatus& aReqStatus, TInt aUSimAppIndex, TDes8& aInfo) const; //deprecated 9.1
	IMPORT_C void NotifyUSimApplicationsInfoChange(TRequestStatus& aReqStatus, TInt& aCount, TAID& aActiveAID) const; //deprecated 9.1

	/** USIM Application Status.

	Modes: WCDMA 

	@deprecated 9.1 Use @see TSmartCardApplicationAction instead.
	*/
	enum TUSimAppAction
		{
	/** USIM application will be activated.
	
	Modes: WCDMA */
		EUSimActivate,
	/** USIM application will be terminated.
	
	Modes: WCDMA */
		EUSimTerminate
		};

	IMPORT_C void SetUSimApplicationStatus(TRequestStatus& aReqStatus, const TAID aAID, TUSimAppAction aUsimAppAction) const; //deprecated 9.1

	/** USIM Application selection mode.

	Mode: WCDMA */
	enum TUSimSelectionMode
		{
	/** USIM application is selected automatically by phone.
	
	Mode: WCDMA */
		EUSimSelectionAutomatic,
	/** USIM application is selected manually by user.
	
	Mode: WCDMA */
		EUSimSelectionManual
		};

	IMPORT_C TInt SetUSimAppsSelectionMode(TUSimSelectionMode aUSimSelectionMode) const;
	IMPORT_C TInt GetUSimAppsSelectionMode(TUSimSelectionMode& aUSimSelectionMode) const;
	IMPORT_C void NotifyUSimAppsSelectionModeChange(TRequestStatus& aReqStatus, TUSimSelectionMode& aUSimSelectionMode) const;
	IMPORT_C void GetCurrentActiveUSimApplication(TRequestStatus& aReqStatus, TAID& aAID) const;

	/***********************************************************************************/
	//
	// Mobile MMS Functional Unit
	// 
	/***********************************************************************************/
	
	/** max size of MMS notification buffer. */
	
	enum{KMaxMmsNotification = 253};
	
	/**
	Identifies current status of MMS.
	*/
	enum TMmsStatus
		{
		/**
		The MMS has not been retrieved.
		*/
		EMmsNotRetrieved,
		/**
	    The MMS has been retrieved.
	    */
		EMmsRetrieved,
		/**
		The MMS has been rejected.
		*/
		EMmsRejected,
		/**
		The MMS has been forwarded.
		*/
		EMmsforwarded
		};

	/** 
	MMS Implementation.
	*/
	enum TMmsImplementation
		{
		/**
		Unknown implementation for MMS.
		*/
		KMmsImplUnknwown=0x00,
		/**
		Wap implementation for MMS.
		*/
		KWapImplementation=0x01
		};
	
	/**
	MMS Connectivity settings.
	*/
	enum TMmsConnParams
		{
		/**
		MMS User Preferences.
		*/
		EUserPreferences,
		/**
		MMS Issuer Connectivity Parameters.
		*/
		EIssuerConnParams,
		/**
		MMS User Connectivity Parameters.
		*/
		EUserConnParams
		};

	/**
	MMS field Id tags.
	*/
	enum TMobileMMSFieldTags
		{
		/**
		Tag id for MMS connectivity parameters.
		*/
		ETagMmsConnParams=0xAB,
		/**
		Tag id for MMS Implementation.
		*/
		ETagMmsImpl=0x80,
		/**
		Tag id for MMS Relay/Server.
		*/
		ETagMmsServer=0x81,
		/**
		Tag id for Interface to Core Network and Bearer.
		*/
		ETagMmsBearerIE=0x82,
		/**
		Tag id for Gateway.
		*/
		ETagMmsGateway=0x83 	  
		};
		
	class TMmsNotificationV3 : public TMultimodeType
	/**
	Class containing MMS notification paramters.
	*/
		{
		public :
			IMPORT_C TMmsNotificationV3();
		public :
			/**
			Free/Used space.
			*/
			TBool iSpace;
			/**
			Notification read/not read.
			*/
			TBool iNotification;
			/**
			MMS status.
			*/
			TMmsStatus iMmsStatus;
			/**
			MMS implementation.
			*/
			TMmsImplementation iImplementation;
			/**
			MMS notification buffer.
			*/
			TBuf8<KMaxMmsNotification> iNotify;
		};
	
	typedef TPckg<TMmsNotificationV3> TMmsNotificationV3Pckg;

	//	MMS notification 	
	IMPORT_C void NotifyMmsUpdate(TRequestStatus& aReqStatus, TDes8& aNotification) const;
	IMPORT_C void SetMmsUserConnParams(TRequestStatus& aReqStatus, TDes8& aConnectivity) const;
	IMPORT_C void SetMmsUserPreferences(TRequestStatus& aReqStatus, TDes8& aPreferences) const;
	IMPORT_C void GetMmsConfig(TRequestStatus& aReqStatus, const TMmsConnParams aType, TDes8& aConnectivity) const;
	IMPORT_C void NotifyMmsConfig(TRequestStatus& aReqStatus, const TMmsConnParams aType, TDes8& aConnectivity) const;
	
	/***********************************************************************************/
	//
	// Mobile MailBox Numbers Functional Unit 
	// 
	/***********************************************************************************/

	class TMobilePhoneVoicemailIdsV3 : public TMultimodeType
	/**
	Contains mailbox numbers.
	*/
		{
		public :
			IMPORT_C TMobilePhoneVoicemailIdsV3();
		public :
			/**
			Phonebook entry for "voicemail" mailbox number.
			*/
			TUint8	iVoice;
			/** 
			Phonebook entry for "data" mailbox number.
			*/
			TUint8	iData;
			/**
			Phonebook entry for "fax" mailbox number.
			*/
			TUint8	iFax;
			/**
			Phonebook entry for "other" mailbox number.
			*/
			TUint8	iOther;
		};

/** A typedef'd packaged TMobilePhoneVoicemailIdsV3 for passing through a generic 
	API function member. */
	typedef TPckg<TMobilePhoneVoicemailIdsV3> TMobilePhoneVoicemailIdsV3Pckg;

	class TMobilePhoneVoicemailIdsV8 : public TMobilePhoneVoicemailIdsV3
	/**
	Contains mailbox numbers see section 4.2.62 of 3GPP TS 31.102 V6.18.0.
	*/
		{
		public :
			IMPORT_C TMobilePhoneVoicemailIdsV8();
		public :
			/**
			Phonebook entry for "videomail" mailbox number.
			*/
			TUint8	 iVideo;
		};
		
	/** A typedef'd packaged TMobilePhoneVoicemailIdsV8 for passing through a generic 
	API function member. */
	typedef TPckg<TMobilePhoneVoicemailIdsV8> TMobilePhoneVoicemailIdsV8Pckg;

	IMPORT_C void GetMailboxNumbers(TRequestStatus& aReqStatus, TDes8& aMailBox) const;
	IMPORT_C void NotifyMailboxNumbersChange(TRequestStatus& aReqStatus, TDes8& aMailBox) const;

	/***********************************************************************************/
	//
	// Mobile APN Control List Functional Unit
	// 
	/***********************************************************************************/	
	
	enum { KMaxApnName = 100 };
	
	class TAPNEntryV3 : public TMultimodeType
	/**
	Contains parameters for an Access Point Name (APN) entry contained 
	within the Access Control List EFacl file in the UICC.
	*/
		{
	public:
		IMPORT_C TAPNEntryV3();
	public:
		/**
		GGSN DNS name.
		*/
		TBuf8<KMaxApnName> iApn;		
		};
		
	typedef TPckg<TAPNEntryV3> TAPNEntryV3Pckg;
	
	IMPORT_C void EnumerateAPNEntries(TRequestStatus& aReqStatus, TUint32 &aIndex) const;
	IMPORT_C void GetAPNname(TRequestStatus& aReqStatus, const TUint32 aIndex, TDes8& aAPN) const;
	IMPORT_C void AppendAPNName(TRequestStatus& aReqStatus, const TDes8& aAPN) const;
	IMPORT_C void DeleteAPNName(TRequestStatus& aReqStatus, TUint32 aIndex) const; 
	IMPORT_C void NotifyAPNListChanged(TRequestStatus& aReqStatus) const;

	enum TAPNControlListServiceStatus {
		/**
		The APN control list service is disabled.
		*/
		EAPNControlListServiceDisabled,
		/**
		The APN control list service is enabled.
		*/
		EAPNControlListServiceEnabled
	};

	IMPORT_C void SetAPNControlListServiceStatus(TRequestStatus& aReqStatus, TAPNControlListServiceStatus aAPNControlListServiceStatus) const;
	IMPORT_C void GetAPNControlListServiceStatus(TRequestStatus& aReqStatus, TAPNControlListServiceStatus& aAPNControlListServiceStatus) const;
	IMPORT_C void NotifyAPNControlListServiceStatusChange(TRequestStatus& aReqStatus, TAPNControlListServiceStatus& aAPNControlListServiceStatus) const;

	/***********************************************************************************/
	//
	// Retrieve Total Air Time Duration Functional Unit
	//
	/***********************************************************************************/
	IMPORT_C TInt GetAirTimeDuration(TTimeIntervalSeconds& aTime) const;
	IMPORT_C void NotifyAirTimeDurationChange(TRequestStatus& aReqStatus,TTimeIntervalSeconds& aTime) const;
	/***********************************************************************************/
	//
	// Terminate All Calls
	//
	/***********************************************************************************/
	IMPORT_C void TerminateAllCalls(TRequestStatus& aReqStatus) const;
	
	/***********************************************************************************/
	//
	// Auto Redial Functional Unit
	//
	/***********************************************************************************/
	IMPORT_C void ClearBlacklist(TRequestStatus& aReqStatus) const;

	/***********************************************************************************/
	//
	// Mobile Personalisation Functional Unit
	//
	/***********************************************************************************/
	
	/**
	Indentifies the personalisation scheme capabilities of the UICC/ME.
	*/
	enum TMobilePhonePersonalisationCaps
		{
		/**
		UICC/ME personalisation based on network (MCC, MNC) information is possible.
		*/
		KCapsPersonalisedNetwork=0x01,
		/**
		UICC/ME personalisation based on network (MCC, MNC, IMSI network code group) information is possible.
		*/
		KCapsPersonalisedNetworkSub=0x02,
		/**
		UICC/ME personalisation based on network (MCC, MNC, EFGID1, EFCNL) information is possible.
		*/
		KCapsPersonalisedServiceProvider=0x04,
		/**
		UICC/ME personalisation based on network (MCC, MNC, EFGID1, EFGID2, EFCNL) information is possible.
		*/
		KCapsPersonalisedCorporate=0x08,
		/**
		UICC/ME personalisation based on (IMSI SIM/USIM code group) information is possible.
		*/
		KCapsPersonalisedUSim=0x10
		};
		
	/**
	Identifies the current personalisation schemes that are active. 
	*/
	enum TMobilePhonePersonalisationScheme
		{
		/**
		The ME is personalised under the network scheme.
		*/
		KPersonalisedNetwork=0x01,
		/**
		The ME is personalised under the network sub scheme.
		*/
		KPersonalisedNetworkSub=0x02,
		/**
		The ME is personalised under the service provider scheme.
		*/
		KPersonalisedServiceProvider=0x04,
		/**
		The ME is personalised under the corprorate scheme.
		*/
		KPersonalisedCorporate=0x08,
		/**
		The ME is personalised under the  SIM/USIM scheme.
		*/
		KPersonalisedUSim=0x10	
		};

	/**
	Identifies the personalisation scheme to be set.
	*/
	enum TMobilePhonePersonalisation
		{
		/**
		Personalise the ME using the network scheme.
		*/
		EPersonaliseNetwork,
		/**
		Personalise the ME using the network sub scheme.
		*/
		EPersonaliseNetworkSub,
		/**
		Personalise the ME using the service provider scheme.
		*/
		EPersonaliseServiceProvider,
		/**
		Personalise the ME using the corporate scheme.
		*/
		EPersonaliseCorporate,
		/**
		Personalise the ME using the SIM/USIM scheme.
		*/
		EPersonaliseUsim
		};
		
	IMPORT_C TInt GetPersonalisationCaps(TUint32& aCaps) const;
	IMPORT_C void GetPersonalisationStatus(TRequestStatus& aReqStatus, TUint32& aPersSchemes) const;
	IMPORT_C void SetPersonalisationStatus(TRequestStatus& aReqStatus, const TMobilePhonePersonalisation aPersSchemes, const TDes& aPersCode) const;
	
	/***********************************************************************************/
	//
	// Etel 3rd Party v1.0 Parameter classes
	//
	/***********************************************************************************/
	
	class TEtel3rdPartyMobileNetworkInfoV1 : public TMultimodeType
	/**
	Defines the parameters used to set-up of a call originated from an ETel 3rd Party client.

	TEtel3rdPartyMobileNetworkInfoV1 was used to give an indication to the TSY of where the
	request was originating from; i.e. if the request is from an application using the ISV
	API or ETel API, which allowed the TSY to make a decision based on the origin.  This
	data type also provided further protection by not including any location information
	parameters.  With PlatSec being introduced in v9.0, there was no need to use this data
	structure, as each API is policed with the adequate PlatSec capabilities.  Therefore,
	in v9.0 TEtel3rdPartyMobileNetworkInfoV1 was deprecated and removed from the ETel ISV
	(ETel3rdParty) implementation.

	TEtel3rdPartyMobileNetworkInfoV1 was only used internally by the ETel ISV API when calls
	are made to RMobilePhone::GetCurrentNetwork or RMobilePhone::NotifyCurrentNetworkChange.
	The TSY no longer needs to support this as ETel ISV now uses the TMobilePhoneNetworkInfoV2
	data structure and no distinction is required for ISV Apps.

	@publishedPartner
	@deprecated 9.0 This class has been deprecated from 9.0 and should no longer be used.
	*/
		{
	public:
		IMPORT_C TEtel3rdPartyMobileNetworkInfoV1();
	public:
	/** Mode of the network.
	Modes: Common
	
	@see TMobilePhoneNetworkMode */
		TMobilePhoneNetworkMode iMode;
	/** Status of the network
	
	Modes: Common
	
	@see TMobilePhoneNetworkStatus */
		TMobilePhoneNetworkStatus iStatus;
	/** The band and band class of the CDMA network operator.
	
	Modes: CDMA
	
	@see TMobilePhoneNetworkBandInfo 
	@deprecated 9.5 */
		TMobilePhoneNetworkBandInfo iBandInfo;
	/** The MCC of the network.
	
	Modes: Common
	
	@see TMobilePhoneNetworkCountryCode */
		TMobilePhoneNetworkCountryCode iCountryCode;
	/** The system identity (SID) of the CDMA network or the system identity (SID) of the AMPS network.
	
	Modes: CDMA

	@see TMobilePhoneNetworkIdentity 
	@deprecated 9.5 
	*/
		TMobilePhoneNetworkIdentity iCdmaSID;
	/** The network identity (NID in CDMA and MNC in GSM).
	
	Modes: Common
	
	@see TMobilePhoneNetworkIdentity */
		TMobilePhoneNetworkIdentity iNetworkId;
	/** The alpha-tag displayed when this is the serving network.
	
	Modes: Common
	
	@see TMobilePhoneNetworkDisplayTag */
		TMobilePhoneNetworkDisplayTag iDisplayTag;
	/** The short name (up to 10 characters) of the network operator.
	
	Modes: GSM/WCDMA
	
	@see TMobilePhoneNetworkShortName */
		TMobilePhoneNetworkShortName iShortName;
	/** The long name (up to 20 characters) of the network operator.
	
	Modes: GSM/WCDMA
	
	@see TMobilePhoneNetworkLongName */
		TMobilePhoneNetworkLongName iLongName;
	/** The type of network access.
	
	@see TMobilePhoneNetworkAccess */
		TMobilePhoneNetworkAccess iAccess;
		};

/** A typedef'd packaged TEtel3rdPartyMobileNetworkInfoV1 for passing through a generic
API function member.

@deprecated 9.0
*/
	typedef TPckg<TEtel3rdPartyMobileNetworkInfoV1>  TEtel3rdPartyMobileNetworkInfoV1Pckg;

	/** IMS Authenticate constants,in accordance to 3GPP TS 33.102,
	section 6.3.7, and RFC 3310*/
	enum TMobilePhoneAuthenticate
		{
		/** length of autn is 128bits - 16octets*/
		KAutnLength=16,
		/** length of RAND is 16 octets */
		KRandLength=16,
		/** length of RES is 16 octets*/
		KResLength=16,
		/** length of IK is 16 octets*/
		KIkLength=16,
		/** length of CK is 16 octets*/
		KCkLength=16,
		/** length of Kc (GSM cipher key) is 8 octets*/
		KKcLength=8,
		/** length of is 112 bits - 14 octets */
		KAutsLength=14,
		};

	/** Maximum size of the record in an EF is 0xff( 255 bytes) according to TS 102.221, section 8.2.2.2
	*/
	enum TMobilePhoneRelease5EFMaxSize
	{
	 KRelease5EFMaxSize=255,
	};
	
	/** typedefs for encapsulation of the buffers that hold authorization data stored on the ISIM/USIM*/
	/** typedef for Internet Multimedia Public Identity*/	
	typedef TBuf8<KRelease5EFMaxSize> TIMPU;
	/** typedef for Internet Multimedia Private Identity*/
	typedef TBuf8<KRelease5EFMaxSize> TIMPI;
	/** typedef for Home Network Domain Name*/
	typedef TBuf8<KRelease5EFMaxSize> THNDN;


	/** IMS Authenticate API*/
	/** Class encapsulates data needed as an input/output for
	ISIM/USIM AUTHENTICATE command
	
	@publishedPartner
	@released
	*/
	class TImsAuthenticateDataV5: public TMultimodeType
		{
	public:
		IMPORT_C TImsAuthenticateDataV5();
		/** holds value of network challenge AUTN */
		TBuf8<KAutnLength> iAUTN;
		/** holds value of network challenge RAND */
		TBuf8<KRandLength> iRAND;
		/** holds result RES of AUTHENTICATE command when it's successfull*/
		TBuf8<KResLength> iRES;
		/** holds value of session key Integrity Key */
		TBuf8<KIkLength> iIK;
		/** holds value of session key Cipher Key */
		TBuf8<KCkLength> iCK;
		/** holds value of Authentication Token when  AUTHENTICATE fails*/
		TBuf8<KAutsLength> iAUTS;
		};

	/** packaged version of TImsAuthenticateDataV5, used when this structure has to be passeed via IPC*/
	typedef TPckg<TImsAuthenticateDataV5> TImsAuthenticateDataV5Pckg;
	friend class CAsyncRetrieveAuthorizationInfo;

	/** API that execute ISIM/USIM AUTHENTICATE command*/
	IMPORT_C void ImsAuthenticate (TRequestStatus& aReqStatus,TDes8& aAuthenticateDataPckgd) const;

	IMPORT_C void NotifyImsAuthorizationInfoChange(TRequestStatus& aReqStatus) const;

	/**
	Source location of IMS Authorization data.
	*/
	enum TAuthorizationDataSource
		{
		/** Authorization data source is USIM. */
		EFromUSIM,
		/** Authorization data source is ISIM. */
		EFromISIM
		};

	/** 
	Class encapsulates IMS authorization data stored in the ISIM or USIM

	@publishedPartner
	@released
	*/
	class CImsAuthorizationInfoV5 : public CBase	
		{
	public:
		IMPORT_C ~CImsAuthorizationInfoV5();
		IMPORT_C static CImsAuthorizationInfoV5* NewL();
		IMPORT_C virtual void ExternalizeL(HBufC8*& aBuffer);
		IMPORT_C virtual void InternalizeL(const TDesC8& aBuffer);
		IMPORT_C TUint ExtensionId() const;
	public:
		/**Internet Multimedia Private Identity stored on the ISIM/USIM*/
		RMobilePhone::TIMPI iIMPI;
		/**Internet Multimedia Public Identity(ies) stored on the ISIM/USIM*/
		RArray<RMobilePhone::TIMPU> iIMPUArray;
		/**Home Network Domain Name stored on the ISIM/USIM*/
		RMobilePhone::THNDN iHomeNetworkDomainName;
		/**indicates whether authorization data has been retrieved from the ISIM*/
		TAuthorizationDataSource iAuthenticationDataSource;
	protected:
		CImsAuthorizationInfoV5();
		/** version of the data supported*/
		TUint iExtensionId;
		};
		
	/******************************************************************************************************/
	//
	//           UICC Authentication 
	//
	/******************************************************************************************************/		
	
	/**	
	Defines the input as well as the ouput parameters used when performing the
	(U)ICC AUTHENTICATE command.

	@publishedPartner
	@released
    */
	class TSmartCardAuthenticateDataV6 : public TImsAuthenticateDataV5
		{
	public:
		IMPORT_C TSmartCardAuthenticateDataV6();

	public:
		TBuf8<KKcLength> iKc;
		};

	/**
	Packaged version of TSmartCardAuthenticateDataV6, used when this
	structure has to be passeed via IPC.
	*/
	typedef TPckg<TSmartCardAuthenticateDataV6> TSmartCardAuthenticateDataV6Pckg;

	IMPORT_C void SmartCardAuthenticate(TRequestStatus& aReqStatus, TDes8& aAuthDataPckgd) const;
	IMPORT_C void SmartCardAuthenticate(TRequestStatus& aReqStatus, const TAID& aAID, TDes8& aAuthDataPckgd) const;
	
	/******************************************************************************************************/
	//
	//     Generic Bootstrapping Architecture (GBA_U) Authentication 
	//
	/******************************************************************************************************/
	// used to set iExtensionId 
	/** The mobile phone GBA and MBMS authentication extension IDs.

	Modes: Common */
	enum TMobilePhoneAuthDataExtId
  		{
		/** The iExtensionId contents indicate that the packaged class is a TGbaBootstrapAuthDataV8. */
		KETelMobileGbaBootstrapAuthDataV8 = KEtelExtMultimodeV8,
		/** The iExtensionId contents indicate that the packaged class is a TGbaNafAuthDataV8. */
		KETelMobileGbaNafAuthDataV8,
		/** The iExtensionId contents indicate that the packaged class is a TMbmsMskUpdateDataV8. */
		KETelMobileMbmsMskUpdateDataV8,
		/** The iExtensionId contents indicate that the packaged class is a TMbmsMtkGenerateDataV8. */
		KETelMobileMbmsMtkGenerateDataV8,
		/** The iExtensionId contents indicate that the packaged class is a TMbmsMskDeleteDataV8. */
		KETelMobileMbmsMskDeleteDataV8,
		/** The iExtensionId contents indicate that the packaged class is a TMbmsMukDeleteDataV8. */
		KETelMobileMbmsMukDeleteDataV8,
		/** The iExtensionId contents indicate that the packaged class is a TMbmsUserKeyV8. */
		KETelMobileMbmsUserKeyV8,
		/** The iExtensionId contents indicate that the packaged class is a TGbaBootstrapParamsV8. */
		KETelMobileGbaBootstrapParamsV8
  		};
		
	class TGbaBootstrapAuthDataV8 : public TImsAuthenticateDataV5
	/**
	The class is identical to its base class except for its GBA Bootstrapping specific V8 extension id.
	Contains parameters to hold the input as well as the output parameters for the GBA authentication 
	in the Bootstrapping mode. The parameters are used as follows: 

	Input: RAND,AUTN
	Output: 
	RES (if the authentication procedure is successful) 
	or
	AUTS (contains the failure response in case of unsuccessful authentication procedure)
	See section 7.1.1.4 and 7.1.2.3 of 3GPP TS 31.102 v6.18.0
	 
	@see TImsAuthenticateDataV5
	 
    @see RMobilePhone::SmartCardAuthenticate request
	 
    @publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TGbaBootstrapAuthDataV8();
		};
	
	/**
	Packaged version of TGbaBootstrapAuthDataV8, used when this structure has to be passed via IPC.
	
	@publishedPartner
	@released
	*/	
	typedef TPckg<TGbaBootstrapAuthDataV8> TGbaBootstrapAuthDataV8Pckg;
	
	
	enum 
		{ 
		/** The Maximum permitted size of Bootstrapping Transaction Identifier(B-TID) of 
		the GBA_U bootstapped key 
		see section 4.2.79 of 3GPP TS 31.102 v6.18.0
		*/
		KBtidLength = 255,
		/** The Maximum permitted size of lifetime of the GBA_U bootstrapped keys 
		see section 4.2.79 of 3GPP TS 31.102 v6.18.0
		*/
		KKeyLifetimeLength = 255
		};
	
	class TBsfDataV8 : public TMultimodeType
	/**
	 Contains parameters to store B-TID and the Key lifetime that are supplied by the the BSF(EF_GBABP).
	 See section 4.2.79 of 3GPP TS 31.102 v6.18.0	
	 
	 @see RMobilePhone::SetGbaBootstrapParams request
	 
	 @publishedPartner
	 @released
	*/
		{
	 public:
	    IMPORT_C TBsfDataV8();	
	 public: 
	 	/** The Bootstrapping Transaction Identifier (B-TID) of the GBA_U bootstrapped key. */  	
	 	TBuf8<KBtidLength>			iBtid;
	 	/** The lifetime of the GBA_U bootstrapped keys. */  		
	 	TBuf8<KKeyLifetimeLength>	iKeyLifetime;
		};
		
	/**
	Packaged version of TBsfDataV8, used when this structure has to be passeed via IPC.
	
	@publishedPartner
	@released
	*/
	typedef TPckg<TBsfDataV8> TBsfDataV8Pckg;
	
	IMPORT_C void SetGbaBootstrapParams(TRequestStatus& aReqStatus, const TAID& aAID, const TDesC8& aParams) const;

	enum 
		{ 
		/**  The Maximum permitted size of NAF_ID of the Network Application Function(NAF) 
		used in the GBA_U NAF derivation procedure 
		see section 7.1.2.4 of 3GPP TS 31.102 v6.18.0
		*/
		KNafIdLength = 255,
		/**  The Maximum permitted size of the private user identity of the GBA_U NAF
		see section 7.1.2.4 of 3GPP TS 31.102 v6.18.0
		*/
		KImpiLength = 255,
		/**  The The Maximum permitted size of the Ks_ext_Naf key returned by the UICC application 
		see section 7.1.2.4 of 3GPP TS 31.102 v6.18.0
		*/
		KKsExtNafLength = 255
		};
	 	
	class TGbaNafAuthDataV8 : public TMultimodeType
	/**
	 Contains parameters to hold the input as well as the output parameters for the GBA authentication 
	 in the NAF(Network Apllication Function) Derivation mode.
	 See section 7.1.1.5 and 7.1.2.4 of 3GPP TS 31.102 v6.18.0
	 
	 @see RMobilePhone::SmartCardAuthenticate request
	 
	 @publishedPartner
  	 @released
	*/
		{
	 public:
	 	IMPORT_C TGbaNafAuthDataV8();
	 public:
	 	/** Input Parameter: The NAF_ID of the Network Application Function (NAF) used in the 
	 	GBA_U NAF Derivation procedure. */  	
	 	TBuf8<KNafIdLength>	iNafId;	
	  	/** Input Parameter: The Private user identity. */  			
	 	TBuf8<KImpiLength>	iImpi;
		/** Output Parameter: The Ks_ext_NAF key returned by the UICC app.  */  		 	
	 	TBuf8<KKsExtNafLength> iKsExtNaf;
		};
		
	/**
	Packaged version of TGbaNafAuthDataV8, used when this structure has to be passeed via IPC.
	
	@publishedPartner
	@released
	*/
	typedef TPckg <TGbaNafAuthDataV8> TGbaNafAuthDataV8Pckg;
	
	class TGbaBootstrapParamsV8 : public TMultimodeType
	/**
	 Contains parameters for the GBA Bootstrapping Parametres (EF_GBABP).
	 See section 4.2.79 of 3GPP TS 31.102 v6.18.0	
	 
	 @see RMobilePhone::GetAuthenticationParams request
	 
	 @publishedPartner
	 @released
	*/
		{
	public:
		IMPORT_C TGbaBootstrapParamsV8();
	public:
		/** The RAND (AKA Random challenge) value used in the GBA_U bootstrapping procedure. */  		
		TBuf8<KRandLength>		iRand;
		/** The Bootstrapping Transaction Identifier (B-TID) associated with the GBA_U bootstrapped 
		keys. The client requesting the GBA Bootstrapping procedure receives this value from the network 
		(i.e. BSF) and it then stores it on the UICC. */  		
		TBuf8<KBtidLength>		iBtid;
		/** The lifetime of the GBA_U bootstrapped keys. The client requesting the GBA Bootstrapping
		 procedure receives this value from the network (i.e. BSF) and it then stores it on the UICC. */  		
		TBuf8<KKeyLifetimeLength>	iKeyLifetime;
		};
	
	/**
	Packaged version of TGbaBootstrapParamsV8, used when this structure has to be passeed via IPC.
	
	@publishedPartner
	@released
	*/
	typedef TPckg<TGbaBootstrapParamsV8> TGbaBootstrapParamsV8Pckg;
	
	enum TAuthData
	/**
	The type of the authentication related data that has been modified as a result of the 
	UICC authentication command.
	
	@see RMobilePhone::GetAuthenticationParams
	
	@publishedPartner
	@released
	*/	
		{
		/** The GBA Bootstrapping parameters. 
		The GBA Bootstrapping parameters are stored under EF_GBABP of the UICC app. */
		EGbaBootstrapParams,
		/** The list of the GBA Network Application Function (NAF) IDs and the Bootstrapping
		Transaction Identifiers (B-TIDs), associated with the previous GBA NAF derivation procedure.
		The list is stored under EF_GBANL of the UICC app.  */
		EGbaNafIdList,
		/** The list of MBMS Service Keys (MSKs) and corresponding time stamp counters, which are 
		related to an MBMS Key Domain. 
		This list is stored in the EF_MSK file of the UICC app. */
		EMbmsMskIdList,
		/** The identifier of the MBMS User Key (MUK) that is used to protect the transfer of MBMS 
		Service Keys (MSK). 
		The MUK ID is stored in the EF_MUK file of the UICC app.*/
		EMbmsMuk
		};
		
	class TAuthInfoV8 : public TMultimodeType
	/**
	 Contains parameters for Authentication related information
	 */	
		{
	public:
		IMPORT_C TAuthInfoV8();
	public:
		/** Application identifier (AID) of the UICC application performing the authentication. */
		TAID  	iAID;
		/** The type of the authentication related data that has been modified as a result of the 
		UICC authentication command*/
		TAuthData	iData;
		};
	
	/**
	Packaged version of TAuthInfoV8 
	*/	
	typedef TPckg<TAuthInfoV8>  TAuthInfoV8Pckg;
	
	IMPORT_C void NotifyAuthenticateDataChange(TRequestStatus& aReqstatus, TDes8& aAuthInfo) const;
	IMPORT_C void GetAuthenticationParams(TRequestStatus& aReqstatus, const TDesC8& aAuthInfo, TDes8& aParams) const;
	
	
	class TGbaNafEntryV8 : public TMultimodeType
	/**
	Contains parameters for the GBA NAF entry(EF_GBANL)
	See section 4.2.83 of 3GPP TS 31.102 v6.18.0
		
	@publishedPartner
	@released
	*/	
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		IMPORT_C TGbaNafEntryV8();
	public:
		/** The identifier of the Network Application Function (NAF) used in the GBA_U NAF 
		Derivation procedure. */  	
		TBuf8<KNafIdLength>		iNafId;
		/** The Bootstrapping Transaction Identifier (B-TID) associated with the GBA_U 
		bootstrapped keys. The client requesting the GBA Bootstrapping procedure receives 
		this value from the network (i.e. BSF) and it then stores it on the UICC.  */  
		TBuf8<KBtidLength>		iBtid;
		};
	
	/**
	Packaged version of TGbaNafEntryV8, used when this structure has to be passeed via IPC.
	
	@publishedPartner
	@released
	*/	
	typedef TPckg<TGbaNafEntryV8> TGbaNafEntryV8Pckg;
	
	/************************************************************************************************/
    //
 	//    Multimedia Broadcast Multicast Service (MBMS) Authentication
 	//  
	/************************************************************************************************/
	
	/** The Maximum permitted size of multimedia internet Keying message 
	see section 7.1.2.5 of 3GPP TS 31.102 v6.18.0
	*/
	enum { KMikeyLength = 255 };
	typedef TBuf8<KMikeyLength> TMikey;
	
	class TMbmsMskUpdateDataV8 : public TMultimodeType
	/**
	Contains parameters to hold the input as well as the output parameters for the MBMS 
	authentication in the MSK Update mode.  
	See section 7.1.1.6 and 7.1.2.5 of 3GPP TS 31.102 v6.18.0
	
	@see RMobilePhone::SmartCardAuthenticate request
	
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMbmsMskUpdateDataV8();
	public:
	    /** Input Parameter:MIKEY (Multimedia Internet Keying) message containing the MSK update 
	    message from the network.  */
		TMikey  iInputMikey;
		/** Output Parameter: The output MIKEY (Multimedia Internet Keying) message containing 
		the MSK Verification message from the UICC as the command response.
		This is an optional parameter and it is only returned if the V-bit in the HDR field of 
		the received MIKEY message is set. */
		TMikey  iOutputMikey;
		};
		
	/**
	Packaged version of TMbmsMskUpdateDataV8, used when this structure has to be passeed via IPC.
	
	@publishedPartner
	@released
	*/		
	typedef TPckg<TMbmsMskUpdateDataV8> TMbmsMskUpdateDataV8Pckg;
	
	/**
	The Maximum permitted size of MTK length.
	see section 7.1.2.5 of 3GPP TS 31.102 v6.18.0.
	*/
	enum {KMtkLength = 255};
	
	/**
	The Salt key length.
	see section 6.4.5.3 of 3GPP TS 33.246 v6.11.0.
	*/
	enum {KSaltLength = 14};
	
	class TMbmsMtkGenerateDataV8 : public TMultimodeType
	/**
	Contains parameters to hold the input as well as the output parameters for the MBMS 
	authentication in MTK Generation mode.  
	See section 7.1.1.7 and 7.1.2.5 of 3GPP TS 31.102 v6.18.0  
	
	@see RMobilePhone::SmartCardAuthenticate request
	
	@publishedPartner
	@released 
	*/
		{
	public:
		IMPORT_C TMbmsMtkGenerateDataV8();
	public:
		/** Input Parameter: The input MIKEY message containing an MTK and a Salt key (if Salt key is available). */
		TMikey	iMikey;
		/** Output Parameter: The MBMS Traffic Key.  */
		TBuf8<KMtkLength>	iMtk;
		/** Output Parameter: A random or pseudo-random string used to protect against some off-line 
		pre computation attacks on the underlying security protocol.
		This is an optional parameter. */
		TBuf8<KSaltLength>	iSaltKey;
		};
	
	/**
	Packaged version of TMbmsMtkGenerateDataV8, used when this structure has to be passeed via IPC.
	
	@publishedPartner
	@released 
	*/		
	typedef TPckg<TMbmsMtkGenerateDataV8> TMbmsMtkGenerateDataV8Pckg;
	
	/**
	The Key domain id length.
	see section 6.3.2.1. 3GPP TS 33.246 v6.11.0
	*/
	enum {KKeyDomainIdLength = 3};
	
	/**
	The Key group length.
	see section 6.3.2.1. 3GPP TS 33.246 v6.11.0
	*/
	enum {KKeyGroupLength = 2};
	
	class TMbmsMskDeleteDataV8 : public TMultimodeType
	/**
	Contains parameters to hold the input as well as the output parameters for the MBMS authentication
	in the MSK Deletion mode.  
	See section 7.1.1.8 and 7.1.2.5 of 3GPP TS 31.102 v6.18.0 
	
	@see RMobilePhone::SmartCardAuthenticate request
	
	@publishedPartner
	@released 
	*/	
		{
	public:
		IMPORT_C TMbmsMskDeleteDataV8();
	public:
		/** Input parameter : Every MSK is uniquely identifiable by its Key Domain ID and MSK ID. 
		Key Domain ID = MCC || MNC and is 3 bytes long. */
		TBuf8<KKeyDomainIdLength>	iKeyDomainId;
		/** Input Parameter: The Key Group part is used to group keys together in order to allow
		redundant MSKs to be deleted. 
		The Key group part is contained within bytes 0 and 1 of the MSK ID. */
		TBuf8<KKeyGroupLength>	iMskIdKeyGroup;
		};
	
	/**
	Packaged version of TMbmsMskDeleteDataV8, used when this structure has to be passeed via IPC.
	
	@publishedPartner
	@released 
	*/		
	typedef TPckg<TMbmsMskDeleteDataV8> TMbmsMskDeleteDataV8Pckg;
	
	/**
	The Maximum permitted size of MUK ID TLV length.
	see section 6.4.5 of 3GPP TS 33.246 v6.11.0
	*/
	enum {KMukIdTlvLength = 255};
	
	class TMbmsMukDeleteDataV8 : public TMultimodeType
	/**
	Contains parameters to hold the input as well as the output parameters for the MBMS 
	authentication in the MUK Deletion mode.  
	See section 7.1.1.9 and 7.1.2.5 of 3GPP TS 31.102 v6.18.0  
	
	@see RMobilePhone::SmartCardAuthenticate request
	
	@publishedPartner
	@released 
	*/	
		{
	public:
		IMPORT_C TMbmsMukDeleteDataV8();
	public:
		/** Input parameter : The MUK ID TLV containing the MUK IDr and MUK IDi. 
		This field will contain the MUK ID as a TLV object containing the MUK IDr and MUK IDi.*/
		TBuf8<KMukIdTlvLength> iMukIdTlv;
		};

	/**
	Packaged version of TMbmsMukDeleteDataV8, used when this structure has to be passeed via IPC.
	
	@publishedPartner
	@released 
	*/		
	typedef TPckg<TMbmsMukDeleteDataV8> TMbmsMukDeleteDataV8Pckg;
		
	/**
	The Maximum permitted size of MUK IDi length.
	see section 6.4.5 of 3GPP TS 33.246 v6.11.0
	*/
	enum { KMukIdiLength = 255 };
	
	/**
	The Maximum permitted size of MUK IDr length.
	see section 6.4.5 of 3GPP TS 33.246 v6.11.0
	*/
	enum { KMukIdrLength = 255 };
	
	/**
	The time stamp counter length.
	see section 6.4.3 of 3GPP TS 33.246 v6.11.0.
	*/
	enum { KTimeStampCounterLength = 4 };
	
	class TMbmsUserKeyV8 : public TMultimodeType
	/**
	Contains parameters for MBMS User Key(MUK)(EF_MUK).
	See section 4.2.81 of 3GPP TS 31.102 v6.18.0.  
	
	@see RMobilePhone::GetAuthenticationParams
	
	@publishedPartner
	@released  
	*/	
		{
	public:
		IMPORT_C TMbmsUserKeyV8();
	public:
		/** The IDi part of MBMS User Key (MUK). */
		TBuf8<KMukIdiLength>	iMukIdi;
		/** The IDr part of MBMS User Key (MUK). */
		TBuf8<KMukIdrLength>	iMukIdr;
		/** The counter for MIKEY replay protection in MSK delivery. The counter is associated 
		with the particular MUK. */
		TBuf8<KTimeStampCounterLength>	iTimeStampCounter;	
		};
		
	/**
	Packaged version of TMbmsUserKeyV8, used when this structure has to be passeed via IPC.
	
	@publishedPartner
	@released
	*/		
	typedef TPckg<TMbmsUserKeyV8> TMbmsUserKeyV8Pckg;
	
	/**
	The MSK id length.
	see section 6.3.2.1. TS 33.246 v6.11.0
	*/
	enum { KMskIdLength =4 };

	class TMskEntryV8 : public TMultimodeType
	/**
	contains Parameters for MBMS MSK ID Entry (EF_MSK).
	See section 4.2.80 of 3GPP TS 31.102 v6.18.0. 
	 
    @publishedPartner
	@released  
	*/		
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		IMPORT_C TMskEntryV8();
	public:
		/** Identifier of the Domain of the BM-SC providing MBMS service */
		TBuf8<KKeyDomainIdLength>	iKeyDomainId;
		/** Identifier of MBMS Service Key(MSK) with in a paricular Key Domain */
		TBuf8<KMskIdLength>		iMskId;
		/** Counter for MIKEY replay protection in MTK delivery.Each counter is
		 associated with a particular MSK */
		TBuf8<KTimeStampCounterLength>	iTimeStampCounter;
		};

	/*************************************************************************************/
	//
	//    WLAN Parameters 
	//
	/*************************************************************************************/	
	
	/**
	This type indicates which of the following fields, within the WLAN data structure,
	contain valid data. 
	*/
	enum TUsimWlanDataFlags
		{
		/** The iPseudonym field is valid in the Wlan data class */
		KWlanPseudonym  =0x00000001,
		/** The iReauthenticationId field is valid in the Wlan data class */
		KWlanReauthenticationId  = 0x00000002,
		/** The iMasterKey field is valid in the Wlan data class */
		KWlanMasterKey   = 0x00000004,
		/** The iCounter field is valid in the Wlan data class */
		KWlanCounter     = 0x00000008
		};	
		
	/**
	Holds a Maximum permitted size of the temporary user Identity or 
	pseudonym stored in EF_Pseudo of the currently active USIM.
	see section 4.4.5.6 of 3GPP TS 31.102 v6.18.0
	*/
	enum {	KWlanUserIdentity = 255 };
	typedef TBuf8 <KWlanUserIdentity> TWlanUserIdentity;

	/**
	Holds a Maximum permitted size of re-authentication identity stored
	in EF_WRI of the currently active USIM.
	see section 4.4.5.6 of 3GPP TS 31.102 v6.18.0.
	*/
	enum { KWlanMasterKeyLength = 255 };

	/**
	Holds a Maximum permitted size of master key used in the process of re-authentication. 
	see section 4.4.5.6 of 3GPP TS 31.102 v6.18.0.
	*/
	enum { KCounterLength = 255 };

	/**
	Contains Parameters for Wlan Pseudonym(EF_Pseudo) and Reauthentication Id(EF_WRI).
	See section 4.4.5.1 and 4.4.5.6 of 3GPP TS 31.102 v6.18.0. 
	@see RMobilePhone::SetWlanData
	@see RMobilePhone::GetWlanData
	@see RMobilePhone::NotifyWlanDataChange

	@publishedPartner
	@released  
	*/
	class TUsimWlanDataV8 : public TMultimodeType
		{
	public:
		IMPORT_C TUsimWlanDataV8();
	public:
		/** A bit-wise sum of TUsimWlanDataCaps constants indicating which of the following 
		fields, within the WLAN data structure, contain valid data. */
		TUint32 	iFieldsUsed;  
		/** Holds a buffer containing the temporary user Identity or pseudonym stored in EF_Pseudo 
		of the currently active USIM. */
		TWlanUserIdentity iPseudonym;
		/** Holds a buffer containing the re-authentication identity stored in EF_WRI of the 
		currently active USIM. */
		TWlanUserIdentity	iReauthenticationId;
		/** Holds the master key used in the process of re-authentication. */
		TBuf8<KWlanMasterKeyLength>	iMasterKey;
		/** A counter value to be used in the fast re-authentication. This value is provided as a 
		part of a previous authentication sequence. */ 
		TBuf8<KCounterLength>	iCounter; 
		};
			
	/** Packaged version of TUsimWlanDataV8, used when this structure has to be passeed via IPC.
	
	@publishedPartner
	@released
	*/	
	typedef TPckg<TUsimWlanDataV8> TUsimWlanDataV8Pckg;

	IMPORT_C void SetWlanData(TRequestStatus& aReqStatus, const TDesC8& aData) const;
	IMPORT_C void GetWlanData(TRequestStatus& aReqStatus, TDes8& aData) const;
	IMPORT_C void NotifyWlanDataChange(TRequestStatus& aReqStatus, TDes8& aData) const;

	/**
	Holds a Maximum permitted size of Wlan Specific Identifier.
	see section 4.4.5.4 of 31.102 v6.18.0. 
	*/	
	enum { KWlanSIDLength = 255 };

	/**
	Contains Parameters for User controlled WLAN Specific IDentifier List (EF_UWSIDL) and Operator 
	controlled WLAN Specific IDentifier List(EF_OWSIDL).
	See section 4.4.5.4 and 4.4.5.5 of 3GPP TS 31.102 v6.18.0.
	 
	@see RMobilePhone::StorePreferredWlanSIDListL
	@see RMobilePhone::NotifyPreferredWlanSIDListChange
	
	@publishedPartner
	@released 
	*/
	class TWlanSIDV8 : public TMultimodeType	
		{
	public:
		IMPORT_C TWlanSIDV8();
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		/** Holds a buffer containing WLAN Specific Identifier (SID)*/
		TBuf8<KWlanSIDLength>	iWSID;
		/** Editable preferences in the User-controlled WLAN Specific IDentifier List */
		TBool	iUserDefined;
		};

	/** Packaged version of TWlanSIDV8, used when this structure has to be passeed via IPC.
	
	@publishedPartner
	@released
	*/	
	typedef TPckg<TWlanSIDV8> TWlanSIDV8Pckg;

	IMPORT_C void StorePreferredWlanSIDListL(TRequestStatus& aReqStatus, CMobilePhoneStoredWlanSIDList* aList) const;
	IMPORT_C void NotifyPreferredWlanSIDListChange(TRequestStatus& aReqStatus) const;	

	/***********************************************************************************/
    //
    // Terminate Active Calls
    //
    /***********************************************************************************/
    IMPORT_C void TerminateActiveCalls(TRequestStatus& aReqStatus) const;

private:
	CMobilePhonePtrHolder* iMmPtrHolder;
	RMobilePhone(const RMobilePhone& aPhone);

protected:
	IMPORT_C void ConstructL();
	IMPORT_C void Destruct();
	};

/*********************************************************/
//
// Phone Storage functionality (RMobilePhoneStore)
//
/*********************************************************/



class CMobilePhoneStorePtrHolder;
class RMobilePhoneStore : public RTelSubSessionBase
/**
Abstract class used to define common phone storage actions and types

Clients open one of the specialised phone store classes that are derived from RMobilePhoneStore
*/
	{
public:
	friend class CAsyncRetrieveStoreList;

/** Types of store.

Modes: Common */
	enum TMobilePhoneStoreType
		{
	/** Store type is unknown.
	
	Modes: Common */
		EPhoneStoreTypeUnknown,
	/** Store contains SMS messages.
	
	Modes: Common */
		EShortMessageStore,
	/** Store contains NAM entries.
	
	Modes: CDMA 
	@deprecated 9.5 */
		ENamStore,
	/** Store contains PhoneBook entries.
	
	Modes: Common */
		EPhoneBookStore,
	/** Store contains emergency number entries.
	
	Modes: Common */
		EEmergencyNumberStore,
	/** Store contains own number entries.

	Modes: Common */
		EOwnNumberStore
		};

/** Store capabilities. */
	enum TMobilePhoneStoreCaps
		{
	/** The store can be accessed as a whole using the ReadAll() and StoreAll() member
	functions. */
		KCapsWholeStore			= 0x80000000,
	/** The store can be accessed one entry at a time using the RMobilePhoneStore::Read(), 
	RMobilePhoneStore::Write() and RMobilePhoneStore::Delete() member functions. */
		KCapsIndividualEntry	= 0x40000000,
	/** The store can be accessed in order to read entries. */
		KCapsReadAccess			= 0x20000000,
	/** The store can be accessed in order to write or delete entries. */
		KCapsWriteAccess		= 0x10000000,
	/** The TSY supports deleting all the store entries at once. */
		KCapsDeleteAll			= 0x08000000,
	/** The TSY supports notification of store events such as entries added, deleted
	or store becoming full. */
		KCapsNotifyEvent		= 0x04000000
		};

/** The type of store information. As defined in TMobilePhoneStoreInfoV1::iExtensionId

Modes: Common

@see TMobilePhoneStoreInfoV1
@see RMobilePhone::TMultimodeType */
	enum TMobilePhoneStoreInfoExtId
		{
	/** The iExtensionId contents indicate that the packaged class is a RMobilePhoneStore::TMobilePhoneStoreInfoV1.
	Modes: Common */
		KETelMobilePhoneStoreV1=KETelExtMultimodeV1,
	/** The iExtensionId contents indicate that the packaged class is a RMobilePhoneBookStore::TMobilePhoneBookInfoV1.
	Modes: Common */
		KETelMobilePhonebookStoreV1,
	/** Reserved for SMS store specific information class.
	Modes: Common */
		KETelMobileSmsStoreV1,
	/** The iExtensionId contents indicate that the packaged class is a RMobileNamStore::TMobileNamStoreInfoV1.
	Modes: CDMA 
	@deprecated 9.5
	*/
		KETelMobileNamStoreV1,
	/** The iExtensionId contents indicate that the packaged class is a RMobileONStore::TMobileONStoreInfoV1.
	Modes: Common */
		KETelMobileONStoreV1,
	/** Reserved for Emergency Number store specific infoprmation class.
	Modes: Common */
		KETelMobileENStoreV1,
	/** The iExtensionId contents indicate that the packaged class is a TMobilePhoneBookInfoV2.
	Modes: Common */
		KETelMobilePhonebookStoreV2=KETelExtMultimodeV2,
		
	/** The iExtensionId contents indicate that the packaged class is a TMobilePhoneBookInfoV2.
 	Modes: Common */
 		KETelMobilePhonebookStoreV5=KEtelExtMultimodeV5
		};



	class TMobilePhoneStoreInfoV1 : public RMobilePhone::TMultimodeType
/** Defines information and capabilities of a phone-side store.
@publishedPartner
@released */
		{
	public:
		IMPORT_C TMobilePhoneStoreInfoV1();

	/** The type of entry in the phone-side store. 
	
	Modes: Common

	@see TMobilePhoneStoreType */
		TMobilePhoneStoreType		iType;
	/** Indicates the total number of entries that may be held in this store. */
		TInt						iTotalEntries;
	/** Indicates the current number of entries held in this store.
	
	Is equal to KIndexNotUsed if the TSY is unable to determine current number 
	of entries.
	
	Modes: Common */
		TInt						iUsedEntries;
	/** Indicates the capabilities of the store. It is the sum of caps constants defined
	in TMobilePhoneStoreCaps and those defined for a specialised phone store.
	
	Modes: Common */
		TUint32						iCaps;
	/** The name of the phone-side store. A valid list of names is given in the description 
	of each specialised store. 
	
	Modes: Common

	@see RMobilePhone::TMobileName */
		RMobilePhone::TMobileName	iName;
		};

/** A typedef'd packaged TMobilePhoneStoreInfoV1 for passing through a generic
API member function. */
	typedef TPckg<TMobilePhoneStoreInfoV1> TMobilePhoneStoreInfoV1Pckg;

	

	class TMobilePhoneStoreEntryV1 : public RMobilePhone::TMultimodeType
/** Defines common contents of an entry in a phone-side store. 
*/
		{
	public:

		enum
			{
			KIndexNotUsed = -1
			};

	protected:
		TMobilePhoneStoreEntryV1();
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
	/** The slot location of the entry within phone-side storage.
	
	Modes: Common */
		TInt  iIndex;
		};

	IMPORT_C void GetInfo(TRequestStatus& aReqStatus, TDes8& aInfo) const;

	IMPORT_C void Read(TRequestStatus& aReqStatus, TDes8& aEntry) const;
	IMPORT_C void Write(TRequestStatus& aReqStatus, TDes8& aEntry) const;

	IMPORT_C void Delete(TRequestStatus& aReqStatus, TInt aIndex) const;
	IMPORT_C void DeleteAll(TRequestStatus& aReqStatus) const;

/** The store events. It is possible for more than one event to occur at the same 
time, therefore the events are a bitmask and should be tested as a bitmask.

Modes: Common

@see NotifyStoreEvent() */
	enum TMobileStoreEvent
		{
	/** The store has become full. This event will trigger after the last possible 
	entry in a store has been written. */
		KStoreFull =0x00000001,
	/** The store was full but now has some space. This event will trigger after one 
	or more entries have been deleted from a full store. */
		KStoreHasSpace =0x00000002,
	/** The store is empty. This event will trigger after the last entry in a store 
	has been deleted or if all entries are deleted at once. */
		KStoreEmpty =0x00000004,
	/** The store has a new entry, placed in the slot indicated by the aIndex parameter. */
		KStoreEntryAdded =0x00000008,
	/** The store has deleted the entry that was in the slot indicated by the aIndex 
	parameter. */
		KStoreEntryDeleted =0x00000010,
	/** The store has changed the entry that was in the slot indicated by the aIndex 
	parameter. */
		KStoreEntryChanged =0x00000020,
	/** The store contents have substantially changed and the TSY is recommending that 
	the client performs a refresh by reading all the contents again. */
		KStoreDoRefresh =0x00000040
		};

	IMPORT_C void NotifyStoreEvent(TRequestStatus& aReqStatus, TUint32& aEvent, TInt& aIndex) const;

protected:
	CMobilePhoneStorePtrHolder* iStorePtrHolder;

protected:
	RMobilePhoneStore();
	IMPORT_C void BaseConstruct(CMobilePhoneStorePtrHolder* aPtrHolder);
	IMPORT_C void Destruct();
	};


/*********************************************************/
//
// Call based functionality (RMobileCall)
// 
/*********************************************************/



class CMobileCallPtrHolder;

class RMobileCall : public RCall
/** Provides access to the functionality associated with a specific mobile call.

RMobileCall encapsulates access to a mobile phone. The functionality of RMobileCall 
is divided into a number of functional units, for the purpose of easier documentation 
and navigation and selectable support by TSY. 

Note: Some RMobileCall function members must be supported by the TSY while 
others are part of optional "functional unit" and only need to be supported
if the TSY supports that functional unit. When a functional unit is mandatory 
then the TSY should at least support the Get...Caps() member function to indicate 
the member functions that are supported for this unit. 
 */
	{
public:
	IMPORT_C RMobileCall();
	/***********************************************************************************/
	//
	//  Mobile call parameters - used within Dial/Answer API
	//
	/***********************************************************************************/

	// used to set iExtensionId in RCall::TCallParams
/** The mobile call parameter extension IDs.

Modes: Common */
	enum TMobileCallParamsExtensionId
		{
	/** The iExtensionId contents indicate that the packaged class is a TMobileCallParamsV1. */
		KETelMobileCallParamsV1=KETelExtMultimodeV1,
	/** The iExtensionId contents indicate that the packaged class is a TMobileDataCallParamsV1.
	
	Modes: Common */
		KETelMobileDataCallParamsV1,
	/** The iExtensionId contents indicate that the packaged class is a TMobileHscsdCallParamsV1.
	
	Modes: GSM/WCDMA */
		KETelMobileHscsdCallParamsV1,
	/** The iExtensionId contents indicate that the packaged class is a TMobileCallParamsV2.
	
	Modes: Common */
		KETelMobileCallParamsV2=KETelExtMultimodeV2,
	/** The iExtensionId contents indicate that the packaged class is a TMobileDataCallParamsV2.
	
	Modes: Common */
		KETelMobileDataCallParamsV2,
	/** The iExtensionId contents indicate that the packaged class is a TMobileHscsdCallParamsV2.

	Modes: GSM/WCDMA */
		KETelMobileHscsdCallParamsV2,
	/** The iExtensionId contents indicate that the packaged class is a TMobileCallParamsV7.

	Modes: GSM/WCDMA */ 
		KETelMobileCallParamsV7=KEtelExtMultimodeV7,
	/** The iExtensionId contents indicate that the packaged class is a TMobileHscsdCallParamsV7.

	Modes: GSM/WCDMA */
		KETelMobileHscsdCallParamsV7,
		
	/** The iExtensionId contents indicate that the packaged class is a TMobileHscsdCallParamsV8.

	Modes: GSM/WCDMA */
		KETelMobileHscsdCallParamsV8 = KEtelExtMultimodeV8,

	/** The iExtensionId contents indicate that the packaged class is a TMobileDataCallParamsV8.

	Modes: GSM/WCDMA */
		KETelMobileDataCallParamsV8,
		
	/** The iExtensionId contents indicate that the packaged class is a TEtel3rdPartyMobileCallParamsV1. 
			
	Modes: Common */	
	KETel3rdPartyCallParamsV1=KETelExt3rdPartyV1
		
		};

/** Caller Id restriction settings.

Modes: Common

@see TMobileCallParamsV1 */
	enum TMobileCallIdRestriction
		{
	/** The default setting should be used if the user has not explicitly requested 
	their identity to be restricted/allowed. */
		EIdRestrictDefault,
	/** The user has explicitly requested their identity to be sent for this call. */
		ESendMyId,
	/** The user has explicitly requested their identity not to be sent for this call. */
		EDontSendMyId
		};

	class TMobileCallCugV1 : public RMobilePhone::TMultimodeType
/** This class is used to set up a CUG (Closed User Group) call. 

TMobileCallCugV1() is valid in GSM/WCDMA mode only. The client application 
should set the CUG settings for the call depending on the user's choice. If
the user has not explicitly requested a CUG call then this parameter should 
indicate that explicit CUG invoke is false. The TSY ignores this parameter 
if in CDMA mode. 
*/
		{
	public:
		IMPORT_C TMobileCallCugV1();
	public:
	/** A boolean indicating whether Closed User Group is being explicitly invoked by 
	the client or not. If this parameter is false, the other entries in this structure 
	will be ignored. */
		TBool	iExplicitInvoke;
	/** The Closed User Group index that the user has specified for this call (if no
	index has been specified then this element will equal -1). */
		TInt	iCugIndex;
	/** A boolean indicating whether the user has suppressed their Outgoing Access option 
	or not. */
		TBool	iSuppressOA;
	/** A boolean indicating whether the user has suppressed the use of their preferential 
	Closed User Group or not. */
		TBool	iSuppressPrefCug;
		};
	
/** Multicall Operation set-up.

Mode: WCDMA */
	enum TMobileCallMulticallBearerMode
		{
	/** Multicall is not supported. */
		EMulticallNotSupported,
	/** MS will accept incoming call and establish a connection using an existing bearer. */
		EMulticallShareBearer,
	/** MS will accept incoming call and establish a connection by creating a new bearer. */
		EMulticallNewBearer
		};

	/** 
	 The originator of a call.
	 
	 @see RSat::TCallParamOrigin
	 */
	enum TCallParamOrigin
		{
		/** The call originator is unknown. */
		EOriginatorUnknown,
		/** The call originated/originates from the SIM. */
		EOriginatorSIM,
		/** The call originated/originates from an Etel Client. */
		EOriginatorEtelClient,
		/** The call originated/originates from an external device connected to the modem. */
		EOriginatorExternal 
		}; 

	/** The maximum size of a sub-address.

	 @see TSubAddress

	 @see RSat::KSubAddressMaxSize*/
	enum {KSubAddressMaxSize=21};

	/** A buffer to hold the sub-address of a telephone number. 
	 
	 @see KSubAddressMaxSize
	 
	 @see RSat::TSubAddress
	 */
	typedef TBuf<KSubAddressMaxSize> TSubAddress;	

	/** The maximum size of the 8-bit capability configuration parameter buffer.
	 @see TCcp
	 
	 @see RSat::KCcpMaxSize
	 */
	enum {KCcpMaxSize=15};

	/** TCcp ( Capability configuration parameters ) is an 8-bit data buffer that
	contains the bearer capabilities as coded in ETSI 24.008(Ver 04.11 2003-06) and 
	defined in ETSI 31.111. It is 15 bytes long as it does not include the IEI (Information 
	Element Identity).
	
	@see TMobileCallParamsV7
	
	@see RSat::TCcp
	*/
	typedef TBuf8<KCcpMaxSize> TCcp;

	/** Bearer Capability Repeat Indicator.
	 
	 @see TMobileCallParamsV7
	 
	 @see RSat::TBCRepeatIndicator
	 */
	enum TBCRepeatIndicator
	        {
	        /** The alternate mode of the Bearer Capability Repeat Indicator. */
	        EBCAlternateMode,
	        /** Support of fallback - mode 1 preferred, mode 2 selected if setup of mode 1 fails. */
	        EBCFallbackMode,
	        /** Service change and fallback - mode 1 alternate mode 2, mode 1 preferred. */
	        EBCServiceChangeAndFallbackMode
	        };

	/** Describes the state of the icon.
	    
	@see TIconId
	
	@see RSat::TIconQualifier
	*/
	enum TIconQualifier
	        {
	        /** Icon qualifier is not set. */
	        EIconQualifierNotSet,
	        /** Icon is not present. */
	        ENoIconId,
	        /** Icon is present; It is self explanatory. 
	         Display instead of the alpha ID or text string. */
	        ESelfExplanatory,
	        /** Icon is present; It is not self explanatory. 
	         Display both the icon and the alpha ID or text string. */
	        ENotSelfExplanatory
	        };

	/** Holds details about an icon.
	
	@see TMobileCallParamsV7
	@see TMobileCallInfoV7
	
	@see RSat::TIconId
	*/
	struct TIconId
	        {
	        /** Describes the state of the icon. */
	        TIconQualifier iQualifier;
	        /** Icon identifier */
	        TUint8 iIdentifier;
	        };

	/** The maximum size of the alpha ID buffer.

	@see TAlphaIdBuf
	
	@see RSat::KAlphaIdMaxSize
	*/
	enum {KAlphaIdMaxSize=254};

	/**
	A buffer to hold an alpha identifier. 
	
	@see TMobileCallParamsV7
	@see TMobileCallInfoV7
	
	@see RSat::TAlphaIdBuf
	*/
	typedef TBuf<KAlphaIdMaxSize> TAlphaIdBuf;
	
	 
	class TMobileCallParamsV1 : public RCall::TCallParams
/** Defines the parameters used for set-up of a call of any type. 
*/
		{
	public:
		IMPORT_C TMobileCallParamsV1();
	public:
	/** Call Id restriction setting to be used for this call.

	@see TMobileCallIdRestriction */
		TMobileCallIdRestriction iIdRestrict;
	/** Closed User Group setting to be used for this call.
	
	@see TMobileCallCugV1 */
		TMobileCallCugV1 iCug;
	/** Specifies whether this call attempt is user initiated (EFalse) or a client initiated 
	redial (ETrue). */
		TBool iAutoRedial;
		};

	/** A typedef'd packaged TMobileCallParamsV1 for passing through a generic API function 
	member. */
	typedef TPckg<TMobileCallParamsV1> TMobileCallParamsV1Pckg;

	
 
 	class TMobileCallParamsV2 : public TMobileCallParamsV1
 	/** Defines the parameters used for set-up of a call of any type 
	for version v2.0 of the API.
	A new parameter (iBearerMode) is required to support Multicall.
	
	@publishedPartner
	@released 
 	*/
 		{
 	public:
 		IMPORT_C TMobileCallParamsV2();
 	public:
	/** The bearer mode. */
 		TMobileCallMulticallBearerMode iBearerMode;
 		};
 
 	/** A typedef'd packaged TMobileCallParamsV1 for passing through a generic API 
	function member. */
 	typedef TPckg<TMobileCallParamsV2> TMobileCallParamsV2Pckg;
 

 	class TMobileCallParamsV7 : public TMobileCallParamsV2
 	/** Defines the parameters used for set-up of a call of any type 
	for version v7.0 of the API.

	@publishedPartner
	@released 
 	*/
 		{        
 	public:
 		IMPORT_C TMobileCallParamsV7();
 	public:
 		/**
 		 The origin of the call.
 		 */
 		TCallParamOrigin iCallParamOrigin;        
 		/**
 		 The sub-address of the remote party. 
 		 */
 		TSubAddress iSubAddress;
 		/**
 		 Capability configuration parameters for the first bearer.
 		 */
 		TCcp iBearerCap1;
 		/**
 		 Capability configuration parameters for the second bearer. 		  
 		 */
 		TCcp iBearerCap2;
 		/**
 		 The bearer capability repeat indicator. 
 		 */
 		TBCRepeatIndicator iBCRepeatIndicator;
 		/**
 		 The icon identifier of the icon to display.  
 		 This can be used, for example, to provide 
 		 graphical notification that the call is taking place.
 		 
 		 @see TIconQualifier
 		 */	 	
 		TIconId iIconId;
 		/**
 		 Buffer to hold the alpha identifier.  This is used to describe
 		 the icon stored in iIconId if iIconId.iQualifier is set to
 		 RMobileCall::ENotSelfExplanatory, 
 		 RMobileCall::EIconQualifierNotSet or 
 		 RMobileCall::ENoIconId.
 		 */
	 	TAlphaIdBuf iAlphaId;
 		};
 	
 	/** A typedef'd packaged TMobileCallParamsV7 for passing through a generic API 
	function member. */
 	typedef TPckg<TMobileCallParamsV7> TMobileCallParamsV7Pckg;
 	
	/***********************************************************************************/
	//
	// Mobile Call Data Functional Unit
	//
	/***********************************************************************************/

/** In GSM mode, maps to list of supported speeds returned in response to AT+CBST=?. 
In CDMA mode, maps to data rates supported by supported service options.

@see TMobileCallDataCapsV1 */
	enum TMobileCallDataSpeedCaps
		{
	/** Call can support autobauding. */
		KCapsSpeedAutobauding = 0x00000001,
	/** Call can support a speed of 2400bps. */
		KCapsSpeed2400  = 0x00000002,
	/** Call can support a speed of 4800bps. */
		KCapsSpeed4800  = 0x00000004,
	/** Call can support a speed of 9600bps. */
		KCapsSpeed9600  = 0x00000008,
	/** Call can support a speed of 14400bps. */
		KCapsSpeed14400 = 0x00000010,
	/** Call can support a speed of 19200bps. */
		KCapsSpeed19200 = 0x00000020,
	/** Call can support a speed of 28800bps. */
		KCapsSpeed28800 = 0x00000040,
	/** Call can support a speed of 32000bps. */
		KCapsSpeed32000 = 0x00000080,
	/** Call can support a speed of 33600bps. */
		KCapsSpeed33600 = 0x00000100,
	/** Call can support a speed of 38400bps. */
		KCapsSpeed38400 = 0x00000200,
	/** Call can support a speed of 43200bps. */
		KCapsSpeed43200 = 0x00000400,
	/** Call can support a speed of 48000bps. */
		KCapsSpeed48000 = 0x00000800,
	/** Call can support a speed of 56000bps. */
		KCapsSpeed56000 = 0x00001000,
	/** Call can support a speed of 57600bps. */
		KCapsSpeed57600 = 0x00002000,
	/** Call can support a speed of 64000bps. */
		KCapsSpeed64000 = 0x00004000,
	/** Call can support extended data speed capabilities. This flag is reserved for 
	future expansion of API and should always be set to zero. */
		KCapsSpeedExtended = 0x80000000		
		};

/** List of supported values of the <speed> parameter returned in response to AT+CBST=?

Not applicable to CDMA mode

@see TMobileDataCallParamsV1 */
	enum TMobileCallDataProtocolCaps
		{
	/** Call can support V.22bis standard
	
	Modes: COMMON */
		KCapsProtocolV22bis = 0x00000001,
	/** Call can support V.32 standard.
	
	Modes: COMMON */
		KCapsProtocolV32 = 0x00000002,
	/** Call can support V.34 standard.
	
	Modes: COMMON */
		KCapsProtocolV34 = 0x00000004,
	/** Call can support V.110 standard.
	
	Modes: GSM/WCDMA */
		KCapsProtocolV110 = 0x00000008,
	/** Call can support V.120 standard.
	
	Modes: GSM/WCDMA */
		KCapsProtocolV120 = 0x00000010,
	/** Call can support bit transparent service.
	
	Modes: GSM/WCDMA */
		KCapsProtocolBitTransparent = 0x00000020,
	/** Call can support X31 flag stuffing standard.
	
	Modes: GSM/WCDMA */
		KCapsProtocolX31FlagStuffing = 0x00000040,
	/** Call can support both PIAF standards (32000bps and 64000bps).
	
	Modes: GSM/WCDMA */
		KCapsProtocolPIAFS = 0x00000080,
	/** Call can support request for Multimedia protocol (3G-324M) over a call to PSTN
	with voice fallback.
	
	Modes: WCDMA */
		KCapsPstnMultimediaVoiceFallback = 0x00000100,
	/** Call can support request for Multimedia protocol (3G-324M) over a call to PSTN
	without voice fallback
	
	Modes: WCDMA */
		KCapsPstnMultimedia = 0x00000200,
	/** Call can support request for Multimedia protocol (3G-324M) over a call to ISDN 
	(voice fallback not possible).
	
	Modes: WCDMA */
		KCapsIsdnMultimedia = 0x00000400,
	/** Call can support extended data carrier capabilities. This flag is reserved for 
	future expansion and should always be set to zero for v1.0 of this API.
	
	Modes: COMMON */
		KCapsProtocolExtended = 0x80000000
		};

/** In GSM mode, maps to list of supported <name> returned in response to AT+CBST=? 
In CDMA mode, maps to supported service options.

Modes: COMMON

@see TMobileCallDataCapsV1 */
	enum TMobileCallDataServiceCaps
		{
		KCapsDataCircuitAsynchronous = 0x00000001,
		KCapsDataCircuitAsynchronousRDI = 0x00000002,
		KCapsDataCircuitSynchronous = 0x00000004,
		KCapsDataCircuitSynchronousRDI = 0x00000008,
	/** Call can support "PAD Access (asynchronous) (UDI)".
	
	Modes: GSM/WCDMA */
		KCapsPADAsyncUDI = 0x00000010,
	/** Call can support "PAD Access (asynchronous) (RDI)"

	Modes: GSM/WCDMA */
		KCapsPADAsyncRDI = 0x00000020,
	/** Call can support "Packet Access (synchronous) (UDI)" 
	
	Modes: GSM/WCDMA */
		KCapsPacketAccessSyncUDI = 0x00000040,
	/** Call can support "Packet Access (synchronous) (RDI)".

	Modes: GSM/WCDMA */
		KCapsPacketAccessSyncRDI = 0x00000080,
	/** Call can support extended data service capabilities. This flag is reserved for 
	future expansion of this API and should always be set to zero for v1.0 of 
	this API.
	
	Modes: COMMON */
		KCapsServiceExtended = 0x80000000
		};

/** Maps to list of supported <ce> returned in response to AT+CBST=?

Not applicable to CDMA mode.

@see TMobileCallDataCapsV1 */
	enum TMobileCallDataQoSCaps
		{
	/** Call can support a request for "transparent only" connection. */
		KCapsTransparent = 0x00000001,
	/** Call can support a request for "non-transparent only" connection. */
		KCapsNonTransparent = 0x00000002,
	/** Call can support a request for "transparent preferred" connection. */
		KCapsTransparentPreferred = 0x00000004,
	/** Call can support a request for "non-transparent preferred" connection. */
		KCapsNonTransparentPreferred = 0x00000008
		};

/** Supported Air Interface User Rates. */
	enum TMobileCallAiurCodingCaps
		{
	/** Air interface user rate of 4.8k is supported. */
		KCapsAiurCoding48 = 0x01,
	/** Air interface user rate of 9.6k is supported. */
		KCapsAiurCoding96 = 0x04,
	/** Air interface user rate of 14.4k is supported. */
		KCapsAiurCoding144 = 0x08
		};

/** HSCSD coding capabilities.

Modes: GSM/WCDMA */
	enum TMobileCallTchCodingsCaps
		{
	/** Call supports HSCSD coding on 4.8K full rate data traffic channel. */
		KCapsTchCoding48  = 0x00000001,
	/** Call supports HSCSD coding on 9.6K full rate data traffic channel. */
		KCapsTchCoding96  = 0x00000004,
	/** Call supports HSCSD coding on 14.4K full rate data traffic channel. */
		KCapsTchCoding144 = 0x00000008,
	/** Call supports HSCSD coding on 28.8K full rate data traffic channel (only possible 
	when 14.4K is supported). */
		KCapsTchCoding288 = 0x00000010,
	/** Call supports HSCSD coding on 32.0K full rate data traffic channel (only possible 
	in a two-timeslot configuration). */
		KCapsTchCoding320 = 0x00000020,
	/** Call supports HSCSD coding on 43.2K full rate data traffic channel (only possible 
	when 14.4K is supported). */
		KCapsTchCoding432 = 0x00000040
		};

/** The call asymmetry capabilities.

Modes: GSM/WCDMA

@see TMobileCallDataCapsV1 */
	enum TMobileCallAsymmetryCaps
		{
	/** Call supports "No Preference" option for asymmetry bias of a Non-Transparent
	ECSD call. */
		KCapsAsymmetryNoPreference= 0x00000001,
	/** Call supports "Downlink bias" option for asymmetry bias of a Non-Transparent 
	ECSD call. 
	
	This means 8-PSK modulation is preferred downlink and GMSK modulation uplink. */
		KCapsAsymmetryDownlink = 0x00000002,
	/** Call supports "Uplink bias" option for asymmetry bias of a Non-Transparent ECSD 
	call. 
	
	This means 8-PSK modulation is preferred uplink and GMSK modulation downlink. */
		KCapsAsymmetryUplink = 0x00000004
		};

/** The supported Call RLP versions.

Modes: GSM/WCDMA

@see TMobileCallDataCapsV1 */
	enum TMobileCallRLPVersionCaps
		{
	/** Call supports the single link, basic RLP version 0. */
		KCapsRLPSingleLinkVersion0 = 0x00000001,
	/** Call supports the single link, extended RLP version 1. */
		KCapsRLPSingleLinkVersion1 = 0x00000002,
	/** Call supports the multi link, RLP version 2. */
		KCapsRLPMultiLinkVersion2  = 0x00000004
		};

/** The supported V42bis options.

Modes: GSM/WCDMA

@see TMobileCallDataCapsV1 */
	enum TMobileCallV42bisCaps
		{
	/** Call supports V42bis compression in initiator-responder direction. */
		KCapsV42bisTxDirection = 0x00000001,
	/** Call supports V42bis compression in responder-initiator direction. */
		KCapsV42bisRxDirection = 0x00000002,
	/** Call supports V42bis compression in both directions. */
		KCapsV42bisBothDirections = 0x00000004
		};

	

	class TMobileCallDataCapsV1 : public RMobilePhone::TMultimodeType
	/** Defines the capabilities of a circuit switched data call. 
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileCallDataCapsV1();
	public:
	/** Data speed capabilities - a bit wise sum of the caps defined in TMobileCallDataSpeedCaps. */
		TUint32	iSpeedCaps;
	/** Data protocol capabilities - a bit wise sum of the caps defined in TMobileCallDataProtocolCaps. */
		TUint32	iProtocolCaps;
	/** Data service capabilities - a bit wise sum of the caps defined in TMobileCallDataServiceCaps. */
		TUint32	iServiceCaps;
	/** Data QoS capabilities - a bit wise sum of the caps defined in TMobileCallDataQoSCaps.
	
	Note: Only available in GSM/WCDMA mode. */
		TUint32	iQoSCaps;
	/** A boolean indicating whether or not the MS supports HSCSD for this call. If 
	this attribute equals EFalse all the following attributes are invalid.

	Note: Only available in GSM/WCDMA mode. */
		TBool	iHscsdSupport;
	/** For HSCSD: The multi-slot class supported. */
		TInt	iMClass;
	/** For HSCSD: The maximum supported number of time-slots in the receive (down-link) 
	direction. */
		TInt	iMaxRxTimeSlots;
	/** For HSCSD: The maximum supported number of time-slots in the transmit (up-link) 
	direction. */
		TInt	iMaxTxTimeSlots;
	/** For HSCSD: The maximum supported total number of receive and transmit time-slots 
	that can be used at same time. */
		TInt	iTotalRxTxTimeSlots;
	/** For HSCSD: A bit-wise sum of the supported air interface channel codings defined 
	in TMobileCallAiurCodingCaps. */
		TUint32	iCodingCaps;
	/** For HSCSD: A bit-wise sum of the supported asymmetry bias defined in TMobileCallAsymmetryCaps. */
		TUint32 iAsymmetryCaps;
	/** For HSCSD: A boolean indicating whether or not the MS supports automatic user 
	initiated service level upgrading. */
		TBool 	iUserInitUpgrade;
	/** For HSCSD and CSD: A bit-wise sum of the supported Radio Link Protocol versions 
	defined in TMobileCallRLPVersionCaps. */
		TUint32	iRLPVersionCaps;
	/** For HSCSD and CSD: A bit-wise sum of the supported V42bis compression capabilities 
	defined in TMobileCallV42bisCaps . */
		TUint32	iV42bisCaps;
		};

/** A typedef'd packaged TMobileCallDataCapsV1 for passing through a generic API 
function member. */
	typedef TPckg<TMobileCallDataCapsV1> TMobileCallDataCapsV1Pckg;

	IMPORT_C TInt GetMobileDataCallCaps(TDes8& aCaps) const;
	IMPORT_C void NotifyMobileDataCallCapsChange(TRequestStatus& aReqStatus, TDes8& aCaps) const;

/** The call setup requested data speed.

@see TMobileDataCallParamsV1 */
	enum TMobileCallDataSpeed
		{
	/** Call set-up will request speed depending on data carrier chosen. */
		ESpeedUnspecified,
	/** Call set-up will request autobauding. */
		ESpeedAutobauding,
	/** Call set-up will request 2400bps. */
		ESpeed2400,
	/** Call set-up will request 4800bps. */
		ESpeed4800,
	/** Call set-up will request 9600bps. */
		ESpeed9600,
	/** Call set-up will request 14400bps. */
		ESpeed14400,
	/** Call set-up will request 19200bps. */
		ESpeed19200,
	/** Call set-up will request 28800bps. */
		ESpeed28800,
	/** Call set-up will request 32000bps. */
		ESpeed32000,
	/** Call set-up will request 33600bps. */
		ESpeed33600,
	/** Call set-up will request 38400bps. */
		ESpeed38400,
	/** Call set-up will request 43200bps. */
		ESpeed43200,
	/** Call set-up will request 48000bps. */
		ESpeed48000,
	/** Call set-up will request 56000bps. */
		ESpeed56000,
	/** Call set-up will request 57600bps. */
		ESpeed57600,
	/** Call set-up will request 64000bps. */
		ESpeed64000
		};

/** The call set-up requested data protocol.

Modes: GSM/WCDMA */
	enum TMobileCallDataProtocol
		{
	/** Call set-up will request carrier depending on data speed chosen. */
		EProtocolUnspecified,
	/** Call set-up will request the V.22bis standard. */
		EProtocolV22bis,
	/** Call set-up will request the V.32 standard. */
		EProtocolV32,
	/** Call set-up will request the V.34 standard. */
		EProtocolV34,
	/** Call set-up will request the V.110 standard. */
		EProtocolV110,
	/** Call set-up will request the V.120 standard. */
		EProtocolV120,
	/** Call set-up will request the X31FlagStuffing standard. */
		EProtocolX31FlagStuffing,
	/** Call set-up will request the PIAFS standard. */
		EProtocolPIAFS,
	/** Call set-up will request the bit transparent service. */
		EProtocolBitTransparent,
	/** Call set-up will request a PSTN multimedia call with voice fallback if multimedia 
	fails.
	
	WCDMA mode only. */
		EProtocolPstnMultimediaVoiceFallback,
	/** Call set-up will request a PSTN multimedia call.
	
	WCDMA mode only. */
		EProtocolPstnMultimedia,
	/** Call set-up will request a ISDN multimedia call.
	
	WCDMA mode only. */
		EProtocolIsdnMultimedia
		};

/** The call setup requested data services.

Modes: Common

@see TMobileDataCallParamsV1 */
	enum TMobileCallDataService
		{
	/** Call set-up will request its default service type.
	
	Modes: Common */
		EServiceUnspecified,
	/** Call set-up will request:
	
	GSM mode - Data circuit asynchronous (UDI or 3.1 KHz, ex PLMN) service 

	In CDMA mode - Async Data service 
	
	Modes: Common */
		EServiceDataCircuitAsync,
	/** Call set-up will request Data circuit asynchronous (RDI) service.
	
	Modes: GSM/WCDMA */
		EServiceDataCircuitAsyncRdi,
	/** Call set-up will request Data circuit synchronous (UDI or 3.1 KHz, ex PLMN) 
	service.
	
	Modes: GSM/WCDMA */
		EServiceDataCircuitSync,
	/** Call set-up will request Data circuit synchronous (RDI) service.
	
	Modes: GSM/WCDMA */
		EServiceDataCircuitSyncRdi,
	/** Call set-up will request PAD asynchronous (UDI) service.
	
	Modes: GSM/WCDMA */
		EServicePADAsyncUDI,
	/** Call set-up will request PAD asynchronous (RDI) service.
	
	Modes: GSM/WCDMA */
		EServicePADAsyncRDI,
	/** Call set-up will request Packet Access synchronous (UDI) service.
	
	Modes: GSM/WCDMA */
		EServicePacketAccessSyncUDI,
	/** Call set-up will request Packet Access synchronous (RDI) service.
	
	Modes: GSM/WCDMA */
		EServicePacketAccessSyncRDI
		};

/** The call set-up requested quality of service (QoS).

Modes: GSM/WCDMA

@see TMobileDataCallParamsV1 */
	enum TMobileCallDataQoS
		{
	/** Call set-up will request its default connection. */
		EQoSUnspecified,
	/** Call set-up will request a transparent connection. */
		EQoSTransparent,
	/** Call set-up will request a non-transparent connection. */
		EQoSNonTransparent,
	/** Call set-up will prefer a transparent connection. */
		EQosTransparentPreferred,
	/** Call set-up will prefer a non-transparent connection. */
		EQosNonTransparentPreferred
		};

/** The call set-up requested RLP version.

@see TMobileDataCallParamsV1 */
	enum TMobileCallDataRLPVersion
		{
	/** Call set-up will request no RLP version. */
		ERLPNotRequested,
	/** Call set-up will request single link basic RLP (version 0). */
		ERLPSingleLinkVersion0,
	/** Call set-up will request single link extended RLP (version 1). */
		ERLPSingleLinkVersion1,
	/** Call set-up will prefer multi link RLP (version 2). */
		ERLPMultiLinkVersion2
		};

/** The V.42bis parameters used in the call set-up of a V.42bis data call.

@see TMobileDataCallParamsV1 */
	enum TMobileCallDataV42bis
		{
	/** Call set-up will not request V.42bis compression. */
		EV42bisNeitherDirection,
	/** Call set-up will request V.42bis compression in initiator-responder direction 
	only. */
		EV42bisTxDirection,
	/** Call set-up will request V.42bis compression in responder-initiator direction 
	only. */
		EV42bisRxDirection,
	/** Call set-up will request V.42bis compression in both directions. */
		EV42bisBothDirections
		};

	
	

	class TMobileDataCallParamsV1 : public TMobileCallParamsV1
/** Defines the parameters used for setup of a circuit switched data call. 
*/
		{
	public:
		IMPORT_C TMobileDataCallParamsV1();
	public:
	/** The type of data service.
	
	@see TMobileCallDataService */
		TMobileCallDataService iService;
	/** The data transfer rate.

	@see TMobileCallDataSpeed */
		TMobileCallDataSpeed iSpeed;
	/** The ITU-T standard modulation protocol or higher application protocol. */
		TMobileCallDataProtocol iProtocol;
	/** The quality of data service.
	
	@see TMobileCallDataQoS */
		TMobileCallDataQoS iQoS;
	/** The version of Radio Link Protocol.
	
	Modes: GSM/WCDMA
	
	@see TMobileCallDataRLPVersion */
		TMobileCallDataRLPVersion iRLPVersion;
	/** The IWF to MS windows size.
	
	Modes: GSM/WCDMA */
		TInt iModemToMSWindowSize;
	/** The MS window to IWF size.
	
	Modes: GSM/WCDMA */
		TInt iMSToModemWindowSize;
	/** The acknowledgement timer.
	
	Modes: GSM/WCDMA */
		TInt iAckTimer;
	/** The number of retransmission attempts.
	
	Modes: GSM/WCDMA */
		TInt iRetransmissionAttempts;
	/** The resequencing period in integer format.
	
	Modes: GSM/WCDMA */
		TInt iResequencingPeriod;
	/** The V42bis compression request parameter.
	
	@see TMobileCallDataV42bis */
		TMobileCallDataV42bis iV42bisReq;
	/** The number of codewords in the algorithm. */
		TInt iV42bisCodewordsNum;
	/** The maximum encodable data string length. */
		TInt iV42bisMaxStringLength;
		TBool iUseEdge; // True for ECSD
		};

/** A typedef'd packaged TMobileDataCallParamsV1 for passing through a generic API 
function member. */
	typedef TPckg<TMobileDataCallParamsV1> TMobileDataCallParamsV1Pckg;

	class TMobileDataCallParamsV2 : public TMobileDataCallParamsV1
/** Defines the parameters used for setup of a circuit switched data call for version 
v2.0 of the API. 
*/
		{
	public:
		IMPORT_C TMobileDataCallParamsV2();
	public:
	/** The bearer mode. */
		TMobileCallMulticallBearerMode iBearerMode;
		};

/** A typedef'd packaged TMobileDataCallParamsV2 for passing through a generic 
API function member. */
	typedef TPckg<TMobileDataCallParamsV2> TMobileDataCallParamsV2Pckg;

	class TMobileDataCallParamsV8 : public TMobileDataCallParamsV2
	/** Defines the parameters used for setup of a circuit switched data call for version 
	v8.0 of the API. 
	*/
		{
	public:
		IMPORT_C TMobileDataCallParamsV8();
	public:
	/** The origin of the call.
		
		@see TCallParamOrigin */
		TCallParamOrigin iCallParamOrigin;
	
	/** The sub-address of the remote party.
	 	
	 	@see TSubAddress */
		TSubAddress iSubAddress;
	
	/** Capability configuration parameters for the first bearer. 	 	
		
		@see TCcp */
		TCcp iBearerCap1;
	
	/** Capability configuration parameters for the second bearer.
		
		@see TCcp */
		TCcp iBearerCap2;
	
	/** The bearer capability repeat indicator. 
		
		@see TBCRepeatIndicator */
		TBCRepeatIndicator iBCRepeatIndicator;
	
	/**	The icon identifier of the icon to display.  
		This can be used, for example, to provide 
		graphical notification that the call is taking place.
		 
		@see TIconQualifier
		*/	 	
		TIconId iIconId;
		
	/** Buffer to hold the alpha identifier.  This is used to describe
		the icon stored in iIconId if iIconId.iQualifier is set to
		RMobileCall::ENotSelfExplanatory, 
		RMobileCall::EIconQualifierNotSet or 
		*/
	 	TAlphaIdBuf iAlphaId;
		};

/** A typedef'd packaged TMobileDataCallParamsV8 for passing through a generic 
API function member. */
	typedef TPckg<TMobileDataCallParamsV8> TMobileDataCallParamsV8Pckg;
	
/** The Air interface user rate.

Modes: GSM/WCDMA. */
	enum TMobileCallAiur
		{
	/** Air interface user rate is not set. */
		EAiurBpsUnspecified,
	/** Air interface user rate of 9.6Kbps. */
		EAiurBps9600,
	/** Air interface user rate of 14.4Kbps. */
		EAiurBps14400,
	/** Air interface user rate of 19.2Kbps. */
		EAiurBps19200,
	/** Air interface user rate of 28.8Kbps. */
		EAiurBps28800,
	/** Air interface user rate of 38.4Kbps. */
		EAiurBps38400,
	/** Air interface user rate of 43.2Kbps. */
		EAiurBps43200,
	/** Air interface user rate of 57.6Kbps. */
		EAiurBps57600
		};

/** Call Asymmetry preferences.

Modes: GSM/WCDMA

@see TMobileHscsdCallParamsV1 */
	enum TMobileCallAsymmetry
		{
	/** There is no preference for Asymmetry Bias. */
		EAsymmetryNoPreference,
	/** Downlink Bias is preferred. */
		EAsymmetryDownlink,
	/** Uplink Bias is preferred. */
		EAsymmetryUplink
		};

/** The call channel coding used.

Modes: GSM/WCDMA */
	enum TMobileCallTchCoding
		{
	/** Channel coding is unspecified. */
		ETchCodingUnspecified,
	/** 4.8K full rate data traffic channel is used. */
		ETchCoding48,
	/** 9.6K full rate data traffic channel is used. */
		ETchCoding96,
	/** 14.4K full rate data traffic channel is used. */
		ETchCoding144,
	/** 28.8K full rate data traffic channel is used. */
		ETchCoding288,
	/** 32.0K full rate data traffic channel is used. */
		ETchCoding320,
	/** 43.2K full rate data traffic channel is used. */
		ETchCoding432
		};

	

	class TMobileDataRLPRangesV1 : public RMobilePhone::TMultimodeType
/** Defines the parameters for minimum and maximum RLP parameter ranges.

Modes: GSM/WCDMA 

*/
		{
	public:
		IMPORT_C TMobileDataRLPRangesV1();
	public:
	/** Maximum modem to MS window size value. */
		TInt  iIWSMax;
	/** Minimum modem to MS window size value. */
		TInt  iIWSMin;
	/** Maximum MS to modem window size value. */
		TInt  iMWSMax;
	/** Minimum MS to modem window size value. */
		TInt  iMWSMin;
	/** Maximum acknowledgement timer value. */
		TInt  iT1Max;
	/** Minimum acknowledgement timer value. */
		TInt  iT1Min;
	/** Maximum retransmission attempts value. */
		TInt  iN2Max;
	/** Minimum retransmission attempts value. */
		TInt  iN2Min;
	/** Maximum resequencing period value. */
		TInt  iT4Max;
	/** Minimum resequencing period value. */
		TInt  iT4Min;
		};

/** A typedef'd packaged TMobileDataRLPRangesV1 for passing through a generic API 
function member. */
	typedef TPckg<TMobileDataRLPRangesV1> TMobileDataRLPRangesV1Pckg;

	IMPORT_C void GetMobileDataCallRLPRange(TRequestStatus& aReqStatus, TInt aRLPVersion, TDes8& aRLPRange) const;
	
	class TMobileHscsdCallParamsV1 : public TMobileDataCallParamsV1
/** The High Speed Circuit Switched Data parameters. 
*/
		{
	public:
		IMPORT_C TMobileHscsdCallParamsV1();
	public:
	/** The wanted Air Interface Rate for the HSCSD call being set-up.
	
	Modes: GSM/WCDMA

	@see TMobileCallAiur */
		TMobileCallAiur	iWantedAiur;
	/** The wanted number of receive timeslots for the HSCSD call being set-up.
	
	Modes: GSM/WCDMA */
		TInt iWantedRxTimeSlots;
		TInt iMaxTimeSlots;
	/** Bit-wise sum of acceptable air interface channel codings TMobileCallAiurCodingCaps 
	for the HSCSD call being set-up.
	
	Modes: GSM/WCDMA */
		TUint iCodings;
	/** The wanted asymmetry bias for the HSCSD call being set-up.
	
	Modes: GSM/WCDMA
	
	@see TMobileCallAsymmetry */
		TMobileCallAsymmetry  iAsymmetry;
	/** A boolean indicating whether or not automatic user initiated service level upgrading 
	is active for the HSCSD call being set-up. ETrue means enabled, EFalse means 
	disabled.
	
	Modes: GSM/WCDMA */
		TBool iUserInitUpgrade;
		};

/** A typedef'd packaged TMobileHscsdCallParamsV1 for passing through a generic 
API function member. */
	typedef TPckg<TMobileHscsdCallParamsV1> TMobileHscsdCallParamsV1Pckg;

	class TMobileHscsdCallParamsV2 : public TMobileHscsdCallParamsV1
/** Call parameters for High Speed Circuit Switched Data (HSCSD) for version v2.0 
of the API. 

*/
		{
	public:
		IMPORT_C TMobileHscsdCallParamsV2();
	public:
	/** The bearer mode. */
		TMobileCallMulticallBearerMode iBearerMode;
		};

/** A typedef'd packaged TMobileHscsdCallParamsV2 for passing through a generic 
API function member. */
	typedef TPckg<TMobileHscsdCallParamsV2> TMobileHscsdCallParamsV2Pckg;

	
	class TMobileHscsdCallParamsV7 : public TMobileHscsdCallParamsV2
	/** Call parameters for High Speed Circuit Switched Data (HSCSD) for version v7.0
	of the API. 
	*/
			{
		public:
			IMPORT_C TMobileHscsdCallParamsV7();
		public:
			/** The originator of the call. */
			TCallParamOrigin iCallParamOrigin;
	 		/**
	 		 The icon identifier of the icon to display.  
	 		 This can be used, for example, to provide 
	 		 graphical notification that the call is taking place.
	 		 
	 		 @see TIconQualifier
	 		 */	 	
	 		TIconId iIconId;
	 		/**
	 		 Buffer to hold the alpha identifier.  This is used to describe
	 		 the icon stored in iIconId if iIconId.iQualifier is set to
	 		 RMobileCall::ENotSelfExplanatory, 
	 		 RMobileCall::EIconQualifierNotSet or 
	 		 RMobileCall::ENoIconId.
	 		 */
		 	TAlphaIdBuf iAlphaId;
			};

	/** A typedef'd packaged TMobileHscsdCallParamsV7 for passing through a generic 
	API function member. */
	typedef TPckg<TMobileHscsdCallParamsV7> TMobileHscsdCallParamsV7Pckg;
	
	class TMobileHscsdCallParamsV8 : public TMobileHscsdCallParamsV7
		/** Call parameters for High Speed Circuit Switched Data (HSCSD) for version v8.0
		of the API. 
		*/
			{
		public:
			IMPORT_C TMobileHscsdCallParamsV8();
		public:
		
			/** The sub-address of the remote party.
				
			@see TSubAddress */
			TSubAddress iSubAddress;
			
	 		/** Capability configuration parameters for the first bearer. 
			
			@see TCcp */
	 		TCcp iBearerCap1;
	 		
	 		/** Capability configuration parameters for the second bearer. 	
	 		
	 		@see TCcp */
	 		TCcp iBearerCap2;
	 		
	 		/** The bearer capability repeat indicator. 
	 		
	 		@see TBCRepeatIndicator */
		 	TBCRepeatIndicator iBCRepeatIndicator;
			};

	/** A typedef'd packaged TMobileHscsdCallParamsV8 for passing through a generic 
	API function member. */
	typedef TPckg<TMobileHscsdCallParamsV8> TMobileHscsdCallParamsV8Pckg;
	
	IMPORT_C void SetDynamicHscsdParams(TRequestStatus& aReqStatus, TMobileCallAiur aAiur, TInt aRxTimeslots) const;

	class TMobileCallHscsdInfoV1 : public RMobilePhone::TMultimodeType
/** The HSCSD settings.

Modes: GSM/WCDMA 

*/
		{
	public:
		IMPORT_C TMobileCallHscsdInfoV1();
	public:
	/** Current AIUR for the HSCSD call.
	
	@see TMobileCallAiur */
		TMobileCallAiur	iAiur;
	/** Current number of receive timeslots for the HSCSD call. */
		TInt iRxTimeSlots;
	/** Current number of transmit time slots for the HSCSD call. */
		TInt iTxTimeSlots;
	/** Current channel coding for the HSCSD call. */
		TMobileCallTchCoding iCodings;
		};

/** A typedef'd packaged TMobileCallHscsdInfoV1 for passing through a generic API 
function member. */
	typedef TPckg<TMobileCallHscsdInfoV1> TMobileCallHscsdInfoV1Pckg;

	class TMobileCallHscsdInfoV7 : public TMobileCallHscsdInfoV1
	/** Version 7 of the HSCSD settings.

	Modes: GSM/WCDMA 
	*/
			{
		public:
			IMPORT_C TMobileCallHscsdInfoV7();
		public:
			/** The originator of the call. */
			TCallParamOrigin iCallParamOrigin;
	    	/**
	    	The icon identifier which can be used for indication that the 
	    	call is being made.
	     	*/
	    	TIconId iIconId;
	    	/**
	    	The alpha identifier to describe the icon.
	     	*/
	    	TAlphaIdBuf iAlphaId;
	    	/**
	    	An indication of whether the call parameters have been modified.  
	    	Set to ETrue if they have been modified, EFalse otherwise.
	     	*/
	    	TBool iParamsCallControlModified;
			};

	/** A typedef'd packaged TMobileCallHscsdInfoV7 for passing through a generic API 
function member. */
	typedef TPckg<TMobileCallHscsdInfoV7> TMobileCallHscsdInfoV7Pckg;		
	
	class TMobileCallHscsdInfoV8 : public TMobileCallHscsdInfoV7
	/** Version 8 of the HSCSD settings.

	Modes: GSM/WCDMA 
	*/
			{
		public:
			IMPORT_C TMobileCallHscsdInfoV8();
		public:
		
			/** The sub-address of the remote party.
				
			@see TSubAddress */
			TSubAddress iSubAddress;
			
	 		/** Capability configuration parameters for the first bearer. 
			
			@see TCcp */
	 		TCcp iBearerCap1;
	 		
	 		/** Capability configuration parameters for the second bearer. 	
	 		
	 		@see TCcp */
	 		TCcp iBearerCap2;
	 		
	 		/** The bearer capability repeat indicator. 
	 		
	 		@see TBCRepeatIndicator */
		 	TBCRepeatIndicator iBCRepeatIndicator;
			};

	/** A typedef'd packaged TMobileCallHscsdInfoV8 for passing through a generic 
	API function member. */
	typedef TPckg<TMobileCallHscsdInfoV8> TMobileCallHscsdInfoV8Pckg;	
	
	
	IMPORT_C TInt GetCurrentHscsdInfo(TDes8& aHSCSDInfo) const;
	IMPORT_C void NotifyHscsdInfoChange(TRequestStatus& aReqStatus, TDes8& aHSCSDInfo) const;

	/***********************************************************************************/
	//
	// Multimedia Calls
	//
	/***********************************************************************************/

	IMPORT_C void AnswerMultimediaCallAsVoice(TRequestStatus& aReqStatus, const TDesC8& aCallParams, TName& aCallName) const;
	IMPORT_C void NotifyVoiceFallback(TRequestStatus& aReqStatus, TName& aCallName) const;

	//
	// Mobile Alternating Call Functional Unit
	//

	IMPORT_C void SwitchAlternatingCall(TRequestStatus& aReqStatus) const;
	IMPORT_C void NotifyAlternatingCallSwitch(TRequestStatus& aReqStatus) const;

	/***********************************************************************************/
	//
	// MobileCallControl functional unit
	//
	/***********************************************************************************/

/** Mobile call control capabilites.

Modes: Common */
	enum TMobileCallControlCaps 
		{
		//KCapsData=0x00000001, // taken from etel.h
		//KCapsFax=0x00000002,
		//KCapsVoice=0x00000004,
		//KCapsDial=0x00000008,
		//KCapsConnect=0x00000010,
		//KCapsHangUp=0x00000020,
		//KCapsAnswer=0x00000040,
		//KCapsLoanDataPort=0x00000080, 
		//KCapsRecoverDataPort=0x00000100
	/** Indicates that this call can be put on hold. This implies that the call is currently 
	active and that there is no other held call. */
		KCapsHold = 0x00000200,
	/** Indicates that this call can be resumed. This implies that the call is currently 
	on hold and that there is no other active call. 
	
	Modes: Common */
		KCapsResume = 0x00000400,
	/** Indicates that this call's state can be swapped to the opposite state. This
	implies that this call is either active or held. There may be another call 
	in the opposite state and if this is the case then both calls will be simultaneously 
	swapped to their opposite state. 
	
	Modes: Common */
		KCapsSwap = 0x00000800,
	/** Indicates that this incoming call can be deflected to another destination. 
	
	Modes: GSM/WCDMA */
		KCapsDeflect = 0x00001000,
	/** Indicates that this call can be transferred to the remote party of another call. 
	For this capability to be set there must be one active call and one held or 
	dialing call.
	
	Modes: GSM/WCDMA */
		KCapsTransfer = 0x00002000,
	/** Indicates that this call can be joined either to another single call to form 
	a conference call or to an existing conference call.
	
	Modes: Common */
		KCapsJoin = 0x00004000,
	/** Indicates that this call (within a conference call) can go into One to One mode. 
	This action would put the rest of the conference call on hold. 
	
	Modes: GSM/WCDMA */
		KCapsOneToOne = 0x00008000,
	/** Indicates that the user can activate a CCBS request on this failed call set-up. 
	
	Modes: GSM/WCDMA */
		KCapsActivateCCBS = 0x00010000,
	/** Indicates that this call is part of an alternating call and the client can call 
	SwitchAlternatingCall() on this call in order to switch it to its other mode.
	
	Modes: GSM/WCDMA */
		KCapsSwitchAlternatingCall = 0x00020000,
	/** Indicates that this call can use/create a new bearer. This means that multiple 
	calls (multicall) can be active at the same time using no more that 7 bearers 
	simultaneously. The other option is to swap with an active call, therefore 
	it becomes held and shares the same bearer.
	
	Modes: WCDMA */
		KCapsMulticall = 0x00040000
		};


/** Mobile call events capabilities.

Modes: Common */
	enum TMobileCallEventCaps
		{
	/** Indicates that local hold notifications are supported.
	
	Modes: Common */
		KCapsLocalHold = 0x00000001,
	/** Indicates that local resume notifications are supported. 
	
	Modes: Common */
		KCapsLocalResume = 0x00000002,
	/** Indicates that local deflect transfer notifications are supported. 
	
	Modes: GSM/WCDMA */
		KCapsLocalDeflectCall = 0x00000004,
	/** Indicates that local explicit transfer notifications are supported. 
	
	Modes: GSM/WCDMA */
		KCapsLocalTransfer = 0x00000008,
	/** Indicates that remote hold notifications are supported. 

	Modes: GSM/WCDMA */
		KCapsRemoteHold = 0x00000010,
	/** Indicates that remote resume notifications are supported. 
	
	Modes: GSM/WCDMA */
		KCapsRemoteResume = 0x00000020,
	/** Indicates that remote termination notifications are supported. 
	
	Modes: Common */
		KCapsRemoteTerminate = 0x00000040,
	/** Indicates that remote conference invocation notifications are supported.
	
	Modes: GSM/WCDMA */
		KCapsRemoteConferenceCreate = 0x00000080
		};

	

	class TMobileCallCapsV1 : public RMobilePhone::TMultimodeType
/** Defines the dynamic capabilities of a mobile call. 
*/
		{
	public:
		IMPORT_C TMobileCallCapsV1();
	public:
	/** Call control capability information - a bit wise sum of TMobileCallControlCaps 
	constants */
		TUint32 iCallControlCaps;
	/** Call event capability information - a bit wise sum of TMobileCallEventCaps constants. */
		TUint32 iCallEventCaps;
		};

/** A typedef'd packaged TMobileCallCapsV1 for passing through a generic API function 
member. */
	typedef TPckg<TMobileCallCapsV1> TMobileCallCapsV1Pckg;

	IMPORT_C TInt GetMobileCallCaps(TDes8& aCaps) const;
	IMPORT_C void NotifyMobileCallCapsChange(TRequestStatus& aReqStatus, TDes8& aCaps) const;

	IMPORT_C void Hold(TRequestStatus& aReqStatus) const;
	IMPORT_C void Resume(TRequestStatus& aReqStatus) const;
	IMPORT_C void Swap(TRequestStatus& aReqStatus) const;
	IMPORT_C void DialISV(TRequestStatus& aStatus,const TDesC8& aCallParams,const TTelNumberC& aTelNumber);
	IMPORT_C void AnswerIncomingCallISV(TRequestStatus& aStatus,const TDesC8& aCallParams);

/** The type of call deflection requested.

@see Deflect() */
	enum TMobileCallDeflect
		{
	/** Destination is not specified. */
		EDeflectUnspecified,
	/** Call will be deflected to voicemail. */
		EDeflectVoicemail,
	/** Call will be deflected to the pre-registered number for the call forwarding 
	service. */
		EDeflectRegisteredNumber,
	/** Call will be deflected to the number supplied in the aDestination parameter. */
		EDeflectSuppliedNumber
		};

	IMPORT_C void Deflect(TRequestStatus& aReqStatus, TMobileCallDeflect aDeflectType, const RMobilePhone::TMobileAddress& aDestination) const;
	IMPORT_C void Transfer(TRequestStatus& aReqStatus) const;
	IMPORT_C void GoOneToOne(TRequestStatus& aReqStatus) const;

/** The mobile call events.

Modes: Common */
	enum TMobileCallEvent
		{
	/** The call has been placed on hold as a result of a local action. 
	
	Modes: Common */
		ELocalHold,
	/** The call has been resumed as a result of a local action. 
	
	Modes: Common */
		ELocalResume,
	/** The call has been deflected to another remote party as a result of a local action. 
	
	
	Modes: GSM/WCDMA */
		ELocalDeflectCall,
	/** The call has been transferred to another remote party as a result of a local 
	action. 
	
	Modes: Common */
		ELocalTransfer,
	/** The call has been placed on hold by the remote connected party. 
	
	Modes: GSM/WCDMA */
		ERemoteHold,
	/** The call has been resumed by the remote connected party. 
	
	Modes: GSM/WCDMA */
		ERemoteResume,
	/** The call has been terminated by the remote party. 
	
	Modes: Common */
		ERemoteTerminated,
	/** The call has been joined by the remote connected party to other call(s) to create/add 
	to a conference call. 
	
	Modes: GSM/WCDMA */
		ERemoteConferenceCreate,
		ERemoteTransferring,
		ERemoteTransferAlerting,
	/** The outgoing call has been barred by the remote party. 
	
	Modes: GSM/WCDMA */
		ERemoteBarred,
	/** The call is being forwarded by the remote party. 
	
	Modes: GSM/WCDMA */
		ERemoteForwarding,
	/** The call is waiting at the remote end. 
	
	Modes: GSM/WCDMA */
		ERemoteWaiting,
	/** The outgoing call has been barred by the local party. 
	
	Modes: GSM/WCDMA */
		ELocalBarred
		};

	IMPORT_C void NotifyCallEvent(TRequestStatus& aReqStatus, TMobileCallEvent& aEvent) const;

/** Describes the possible call states. */
	enum TMobileCallStatus
		{
	/** Indicates that the call status is unknown. */
		EStatusUnknown,			// same as RCall::EStatusUnknown
	/** Indicates that the call is idle. */
		EStatusIdle,			// same as RCall::EStatusIdle
		EStatusDialling,		// same as RCall::EStatusDialling
	/** Indicates that the MT call is ringing but not answered yet by the local user. */
		EStatusRinging,			// same as RCall::EStatusRinging
	/** Indicates that the local user has answered the MT call but the network has not 
	acknowledged the call connection yet. */
		EStatusAnswering,		// same as RCall::EStatusAnswering
	/** MO Call: the network notifies to the MS that the remote party is now ringing. */
		EStatusConnecting,		// same as RCall::EStatusConnecting
	/** Indicates that call is connected and active. */
		EStatusConnected,		// same as RCall::EStatusConnected
	/** Indicates that call is disconnecting. */
		EStatusDisconnecting,	// same as RCall::EStatusHangingUp
	/** Indicates that the call is disconnecting with inband data (to enable the network 
	to send an audio tone), signifying that the call is not to be released until 
	the user terminates the call. */
		EStatusDisconnectingWithInband,
	/** Indicates that call is undergoing temporary channel loss and it may or may not 
	be reconnected. */
		EStatusReconnectPending,
	/** Indicates that the call is connected but on hold. */
		EStatusHold,
	/** Indicates that the call is the non-active half of an alternating call. This 
	call is waiting for its active half or the remote end to switch alternating 
	call mode. */
		EStatusWaitingAlternatingCallSwitch,
		EStatusTransferring,
		EStatusTransferAlerting
		};

	IMPORT_C TInt GetMobileCallStatus(TMobileCallStatus& aStatus) const;
	IMPORT_C void NotifyMobileCallStatusChange(TRequestStatus& aReqStatus, TMobileCallStatus& aStatus) const;

	IMPORT_C void DialNoFdnCheck(TRequestStatus& aStatus,const TDesC& aTelNumber) const;
	IMPORT_C void DialNoFdnCheck(TRequestStatus& aStatus,const TDesC8& aCallParams,const TDesC& aTelNumber) const;
	
	IMPORT_C void DialCallControl(TRequestStatus& aStatus, const TDesC8& aCallParams, const TTelNumberC& aTelNumber) const;
	
	/**
	Contains audio tone events.
	*/
	enum TMobileCallAudioToneEvent
		{
		/**
		Stop audio tone.
		*/
		EAudioStop,
		/**
		Start audio tone.
		*/
		EAudioStart		
		};
		
	/**
	List of 3GPP defined supervisory audio tones taken from technical specification 3GPP TS 22.001 v4.0.0 Annex F.

	This enumeration is based on the signal information element specified in Table 10.5.130/3GPP TS 24.008 v5.15.0
	*/
	enum TMobileCallAudioTones	
		{
		/** 
		No tone.  Not defined in specification used for initialisation of 
		iTone class member variable during TAudioToneV3 construction.
		*/
		ENoTone,
		/**
		Dial tone.
		*/
		EDialTone,
		/**
		Subscriber busy tone.
		*/
		ESubscriberBusyTone,
		/**
		Congestion tone.
		*/
		ECongestionTone,
		/**
		Radio path acknowledgement tone.
		*/
		ERadioPathAcknowledgement,
		/**
		Radio path not available tone.
		*/
		ERadioPathNotAvailable,
		/**
		Call dropped (Mobile Originated only).
		*/
 		EMOCallDropped,
 		/**
		Error special information.
		*/
 		EErrorSpecialInformation,
 		/**
		Number unobtainable.
		*/
 		ENumberUnobtainable,
 		/**
		Authentication failure.
		*/
 		EAuthenticationFailure,
 		/**
		Call waiting.
		*/
 		ECallWaiting,
 		/**
		Ringback tone.
		*/
		ERingback
		};

	class TAudioToneV3 :  public RMobilePhone::TMultimodeType
	/**
	Audio tone version 1 class. Contains information relating to the  call control playing of audio tones. 
	Tones maybe generated by the network (in-band) or signalled to be played locally by the ME.
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TAudioToneV3();
		
	public:
		/**
		Indicates whether the tone is starting or stopping playing.
		
		@see TMobileCallAudioToneEvent
		*/
		TMobileCallAudioToneEvent iEvent;
		/**
		Indicates which tone is to be played.
		
		@see TMobileCallAudioTones
		*/
		TMobileCallAudioTones	iTone;
		/**
		Indicates whether the tone is played by the network or is to be played locally by the ME.
		*/
		TBool iPlayedLocally; 
		};

	/**
	Packaging typedef for TAudioToneV3 class.
	*/
	typedef TPckg<TAudioToneV3> TAudioToneV3Pckg;
	
	IMPORT_C void NotifyAudioToneEvent(TRequestStatus& aReqStatus, TDes8& aToneInfo) const;

	/***********************************************************************************/
	//
	// MobilePrivacy functional unit
	//
	/***********************************************************************************/

	IMPORT_C TInt SetPrivacy(RMobilePhone::TMobilePhonePrivacy aPrivacySetting) const;
	IMPORT_C void NotifyPrivacyConfirmation(TRequestStatus& aReqStatus, RMobilePhone::TMobilePhonePrivacy& aPrivacySetting) const;

	/***********************************************************************************/
	//
	// MobileTrafficChannel function unit
	//
	/***********************************************************************************/

/** Call traffic channels.

Modes: CDMA 
@deprecated 9.5
*/
	enum TMobileCallTch
		{
	/** The traffic channel of the call is unknown. */
		ETchUnknown,
	/** The call has a digital traffic channel */
		ETchDigital,
	/** The call has an analog traffic channel */
		ETchAnalog
		};

	IMPORT_C TInt SetTrafficChannel(TMobileCallTch aTchRequest) const;
	IMPORT_C void NotifyTrafficChannelConfirmation(TRequestStatus& aReqStatus, TMobileCallTch& aTchType) const;

	/***********************************************************************************/
	//
	// MobileCallInformation functional unit
	//
	/***********************************************************************************/

	/**
	Remote party Identity status.

	Modes: Common

	@see TMobileCallRemotePartyInfoV1
	*/
	enum TMobileCallRemoteIdentityStatus
		{
		/**
		The remote party's identity can not be determined.  Calling
		party's Binary Coded Decimal (BCD) number is not available.
		Cause of No CLI is "unavailable".
		*/
		ERemoteIdentityUnknown,

		/**
		The remote party's identity, or BCD number, is available.
		However, cause of No CLI has not been set.
		*/
		ERemoteIdentityAvailable,

		/**
		The remote party has suppressed the transmission of its
		identity; i.e. their BCD number is not available.  Cause of No
		CLI is "reject by user".
		*/
		ERemoteIdentitySuppressed,
		/**
		The remote party's identity, or BCD number, is available.  The
		cause of No CLI is "reject by user".

		To be used where the network has specified that displaying of
		CLI information of the remote party has been rejected by the
		caller but the TMobileCallRemotePartyInfoV1::iRemoteNumber
		parameter has been populated with some other string which may
		be of use to the client.
		*/
		ERemoteIdentityAvailableNoCliRejectedByUser,

		/**
		The remote party's identity is available; a BCD number is
		available.  The cause of No CLI is "Interaction with other
		service".

		To be used where the network has specified that CLI information
		of the remote party is not available due to ineraction with
		another service.  However, the
		TMobileCallRemotePartyInfoV1::iRemoteNumber parameter has been
		populated with some other string which may be of use to the
		client.
		*/
		ERemoteIdentityAvailableNoCliInteractionWithOtherService,
		/**
		The remote party's identity, or BCD number, is unavailable.
		Cause of No CLI is "interaction with other service".
		*/
		ERemoteIdentityUnavailableNoCliInteractionWithOtherService,

		/**
		The remote party's identity, or BCD number, is available.
		Cause of No CLI is "Coin Line/ Payphone".

		To be used where the network has specified that CLI information
		of the remote party is not available as the caller is calling
		from a coin/ payphone.  However, the
		TMobileCallRemotePartyInfoV1::iRemoteNumber parameter has been
		populated with some other string which may be of use to the
		client.
		*/
		ERemoteIdentityAvailableNoCliCoinOrPayphone,
		/**
		The remote party's identity, or BCD number, is not available.
		Cause of No CLI is "Coin Line/ Payphone".
		*/
		ERemoteIdentityUnavailableNoCliCoinOrPayphone,

		/**
		The remote party's identity, or BCD number, is available.
		Cause of No CLI is "unavailable".

		To be used where the network has specified that the CLI
		information of the remote party is not available but the
		TMobileCallRemotePartyInfoV1::iRemoteNumber parameter has been
		populated with some other string which may be of use to the
		client.
		*/
		ERemoteIdentityAvailableNoCliUnavailable
		};


	/**
	The direction of the mobile call.

	Modes: Common

	@see TMobileCallRemotePartyInfoV1
	*/
	enum TMobileCallDirection
		{
	/** The direction of the call is unknown. */
		EDirectionUnknown,
	/** The call was originated by this phone, i.e. it is an outgoing call. */
		EMobileOriginated,
	/** The call was terminated by this phone, i.e. it is an incoming call. */
		EMobileTerminated
		};

	enum { KCallingNameSize=80 };

	

	class TMobileCallRemotePartyInfoV1 : public RMobilePhone::TMultimodeType
/** Defines information about the remote party of a mobile call. 
*/
		{
	public:
		IMPORT_C TMobileCallRemotePartyInfoV1();
	public:
	/** Indicates whether the remote party information in the rest of this structure 
	is valid or not.
	
	Modes: Common
	
	@see TMobileCallRemoteIdentityStatus */
		TMobileCallRemoteIdentityStatus iRemoteIdStatus;
	/** The direction of the call and hence the role of the remote party, i.e. if the 
	call is mobile originated then the remote party is the called party whereas 
	if the call is mobile terminated then the remote party is the calling party.
	
	Modes: Common
	
	@see TMobileCallDirection */
		TMobileCallDirection iDirection;
	/** The phone number of the remote party if available.
	
	Modes: Common
	
	@see RMobilePhone::TMobileAddress */
		RMobilePhone::TMobileAddress iRemoteNumber;
	/** Calling party name available through the CNAP supplementary service (if provisioned). 
	
	
	Modes: Common */
		TBuf<KCallingNameSize> iCallingName;
		};

/** A typedef'd packaged TMobileCallRemotePartyInfoV1 for passing through a generic 
API function member. */
	typedef TPckg<TMobileCallRemotePartyInfoV1> TMobileCallRemotePartyInfoV1Pckg;

	IMPORT_C void NotifyRemotePartyInfoChange(TRequestStatus& aReqStatus, TDes8& aRemotePartyInfo) const;

/** Determines which fields in the class are valid.

@see TMobileCallInfoV1 */
	enum TMobileCallInfoFlags
		{
	/** The iStartTime field is valid in the call info class.
	
	Modes: COMMON */
		KCallStartTime		= 0x00000001,
	/** The iDuration field is valid in the call info class.
	
	Modes: COMMON */
		KCallDuration		= 0x00000002,
	/** The iCallId field is valid in the call info class.
	
	Modes: COMMON */
		KCallId				= 0x00000004,
	/** The iRemoteParty field is valid in the call info class.
	
	Modes: COMMON */
		KCallRemoteParty	= 0x00000008,
	/** The iDialledParty field is valid in the call info class.
	
	Modes: COMMON */
		KCallDialledParty	= 0x00000010,
	/** The iExitCode field is valid in the call info class.
	
	Modes: COMMON */
		KCallExitCode		= 0x00000020,
	/** The iEmergency field is valid in the call info class.
	
	Modes: COMMON */
		KCallEmergency		= 0x00000040,
	/** The iForwarded field is valid in the call info class.
	
	Modes: COMMON */
		KCallForwarded		= 0x00000080,
	/** The iPrivacy field is valid in the call info class.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		KCallPrivacy		= 0x00000100,
	/** The iTch field is valid in the call info class.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		KCallTch			= 0x00000200,
	/** The iAlternatingCall field is valid in the call info class.
	
	Modes: GSM/WCDMA */
		KCallAlternating	= 0x00000400,
	/** The iSecurity field is valid in the call info class.
	
	Modes: GSM/WCDMA */		
		KCallSecurity		= 0x00000800,
	/** The iCallParamOrigin field is valid in the call info class.
	
	Modes: GSM/WCDMA */		
		KCallParamOrigin	= 0x00001000,
	/** The iIconId field is valid in the call info class.
	
	Modes: GSM/WCDMA */		
		KCallIconId			= 0x00002000,
	/** The iAlphaId field is valid in the call info class.
	
	Modes: GSM/WCDMA */		
		KCallAlphaId		= 0x00004000,
	/** The iParamsCallControlModified field is valid in the call 
	info class.
	
	Modes: GSM/WCDMA */		
		KCallParamsCallControlModified	= 0x00008000,	
		
	/** The iSubAddress field is valid in the call 
	info class.
					
	Modes: GSM/WCDMA */		
		KCallSubAddress	= 0x0010000,	
				
	/** The iBearerCap1 field is valid in the call 
	info class.
							
	Modes: GSM/WCDMA */		
		KCallBearerCap1	= 0x0020000,	
			
	/** The iBearerCap2 field is valid in the call 
	info class.
									
	Modes: GSM/WCDMA */		
		KCallBearerCap2	= 0x0040000,	
			
	/** The iBCRepeatIndicator field is valid in the call 
	info class.
										
	Modes: GSM/WCDMA */		
		KCallBCRepeatIndicator	= 0x0080000,
				
		};
	

	class TMobileCallInfoV1 : public RMobilePhone::TMultimodeType
/** Defines general information about a mobile call. 
*/
		{
	public:
		IMPORT_C TMobileCallInfoV1();
	public:
	/** A bit-wise sum of TMobileCallInfoFlags indicating which of the following optional 
	fields contain valid data. */
		TUint32 iValid;
	/** The call service (voice, data, fax, aux. voice) that this call provides.
	
	@see RMobilePhone::TMobileService */
		RMobilePhone::TMobileService iService;
	/** The mobile call status.
	
	@see TMobileCallStatus */
		TMobileCallStatus iStatus;
	/** The call ID number assigned to an ongoing call. Will equal -1 if the call does 
	not have an allocated ID. */
		TInt iCallId;
	/** The reason for termination of a finished call. Will equal KErrNone if the call 
	ended normally and KErrNotFound if the call has not ended. */
		TInt iExitCode;
	/** ETrue if the call is an emergency call or EFalse if it is not. */
		TBool iEmergency;
	/** For a MT call, this attribute equals ETrue if the call has been forwarded to 
	this phone or EFalse if it has not. For a MO call, this attribute equals ETrue 
	if the call has been forwarded to another destination or EFalse if it has 
	not. If the phone can not determine whether the call has been forwarded or 
	not then the attribute will return EFalse. */
		TBool iForwarded;
	/** This attribute indicates whether privacy is on or off for this call.
	
	@see RMobilePhone::TMobilePhonePrivacy */
		RMobilePhone::TMobilePhonePrivacy iPrivacy;
	/** This attribute indicates whether this call is an alternating call or not.
	
	@see RMobilePhone::TMobilePhoneAlternatingCallMode */
		RMobilePhone::TMobilePhoneAlternatingCallMode iAlternatingCall;
	/** The current duration of the call.
	
	@see TTimeIntervalSeconds */
		TTimeIntervalSeconds iDuration;
	/** This attribute indicates whether a digital or analog traffic channel is being 
	used for this call.
	
	@see TMobileCallTch */
		TMobileCallTch iTch;
	/** Information about remote party of a connected call.
	
	@see TMobileCallRemotePartyInfoV1 */
		TMobileCallRemotePartyInfoV1 iRemoteParty;
	/** The original number (including DTMF) dialled for an outgoing call.
	
	@see RMobilePhone::TMobileAddress */
		RMobilePhone::TMobileAddress iDialledParty;
	/** The time and date the call started.
	
	@see TDateTime */
		TDateTime iStartTime;
	/** The name of the call.
	
	@see TName */
		TName iCallName;
	/** The name of the line on which the call is open.
	
	@see TName */
		TName iLineName;
		};

/** A typedef'd packaged TMobileCallInfoV1 for passing through a generic API function 
member. */
	typedef TPckg<TMobileCallInfoV1> TMobileCallInfoV1Pckg;
	
	class TMobileCallInfoV3 : public TMobileCallInfoV1
	/**
	Extends the TMobileCallInfoV1 claass to include extra call info parameter relating to network security.
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileCallInfoV3();
	public:
		/**
		Indicates whether Ciphering Status of a call is enabled or disabled. 
		*/
		RMobilePhone::TMobilePhoneNetworkSecurity  iSecurity;
		};
		
	typedef TPckg<TMobileCallInfoV3> TMobileCallInfoV3Pckg; //< Package descriptor to hold a TMobileCallInfoV3.

	
	class TMobileCallInfoV7 : public TMobileCallInfoV3
	/**
	Extends the TMobileCallInfoV3 claass to include extra parameters
	to indicate the origin of the call, an icon and identifier to 
	describe the call and an indication of whether the call parameters
	have been modified.
	
	@publishedPartner
	@released
	*/
    	{        
    public:
    	IMPORT_C TMobileCallInfoV7();
    public:
    	/**
    	The origin of the call.
    	*/
    	TCallParamOrigin iCallParamOrigin;
    	/**
    	The icon identifier which can be used for indication that the 
    	call is being made.
     	*/
    	TIconId iIconId;
    	/**
    	The alpha identifier to describe the icon.
     	*/
    	TAlphaIdBuf iAlphaId;
    	/**
    	An indication of whether the call parameters have been modified.  
    	Set to ETrue if they have been modified, EFalse otherwise.
     	*/
    	TBool iParamsCallControlModified;
    };
	
    typedef TPckg<TMobileCallInfoV7> TMobileCallInfoV7Pckg; //< Package descriptor to hold a TMobileCallInfoV7.
	
    class TMobileCallInfoV8 : public TMobileCallInfoV7
    	/**
    	Extends the TMobileCallInfoV7 claass to include extra parameters
    	to indicate the bearer capabilities, repeat indicator and sub address. 
        	
    	@publishedPartner
    	@released
    	*/
        {        
    public:
       	IMPORT_C TMobileCallInfoV8();
    public:
        /** The sub-address of the remote party. 
    	
    	@see TSubAddress */
        TSubAddress iSubAddress;
        
        /** Capability configuration parameters for the first bearer. 
        
        @see TCcp */
       	TCcp iBearerCap1;
        
        /** Capability configuration parameters for the second bearer. 	 	
       	
        @see TCcp */
       	TCcp iBearerCap2;
       	
        /** The bearer capability repeat indicator. 
       	
        @see TBCRepeatIndicator */
        TBCRepeatIndicator iBCRepeatIndicator;
        };
    	
    typedef TPckg<TMobileCallInfoV8> TMobileCallInfoV8Pckg; //< Package descriptor to hold a TMobileCallInfoV8.
      
	IMPORT_C TInt GetMobileCallInfo(TDes8& aCallInfo) const;

	/***********************************************************************************/
	//
	// MobileCallEmergency functional unit
	//
	/***********************************************************************************/

	IMPORT_C void DialEmergencyCall(TRequestStatus& aReqStatus, const TDesC& aNumber) const;

	/***********************************************************************************/
	//
	// MobileCallCompletion
	//
	/***********************************************************************************/

	IMPORT_C void ActivateCCBS(TRequestStatus& aReqStatus, TInt& aIndex) const;
	IMPORT_C TInt RejectCCBS() const;
		//
	// User-To-User Signalling Functional Unit
	//
	/***********************************************************************************/

/** The UUS capabilities of the call.

Modes: GSM/WCDMA. */
	enum TMobileCallUUSCaps			// UUS capabilities of the call
		{
	/** Indicates that MS supports UUS1 implicit request. */
		KCapsSetupUUS1Implicit=0x00000001,
	/** Indicates that MS supports UUS1 explicit request. */
		KCapsSetupUUS1Explicit=0x00000002,
	/** Indicates that MS supports UUS2 request. */
		KCapsSetupUUS2=0x00000004,
	/** Indicates that MS supports UUS3 request. */
		KCapsSetupUUS3=0x00000008,
	/** Indicates that MS supports activating more than one UUS request at a time. */
		KCapsSetupMultipleUUS=0x00000010,
	/** Indicates that UUS1 is active for this call. */
		KCapsActiveUUS1=0x00000020,
	/** Indicates that UUS2 is active for this call. */
		KCapsActiveUUS2=0x00000040,
	/** Indicates that UUS3 is active for this call. */
		KCapsActiveUUS3=0x00000080
		};

	IMPORT_C TInt GetUUSCaps(TUint32& aCaps) const;
	IMPORT_C void NotifyUUSCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const;

/** UUS Service requests.

Modes: GSM/WCDMA */
	enum TMobileCallUUSReqs			// UUS Service requests
		{
	/** UUS1 is implicitly requested. */
		KUUS1Implicit=0x00000001,
	/** UUS1 is explicitly requested, but it is not mandatory for this call to proceed. */
		KUUS1ExplicitRequested=0x00000002,
	/** UUS1 is explicitly requested and it is mandatory for this call to proceed. */
		KUUS1ExplicitRequired=0x00000004,
	/** UUS2 is (explicitly) requested, but it is not mandatory for this call to proceed. */
		KUUS2Requested=0x00000008,
	/** UUS2 is (explicitly) requested and it is mandatory for this call to proceed. */
		KUUS2Required=0x00000010,
	/** UUS3 is (explicitly) requested, but it is not mandatory for this call to proceed. */
		KUUS3Requested=0x00000020,
	/** UUS3 is (explicitly) requested and it is mandatory for this call to proceed. */
		KUUS3Required=0x00000040
		};

	enum 
		{
		KMaxUUISize = 129,
		};

/** Buffer for the UUI element. */
	typedef TBuf<KMaxUUISize> TMobileCallUUI;

	class  TMobileCallUUSRequestV1 : public RMobilePhone::TMultimodeType
/** The mobile Call User-to-User Signaling request. 
*/
		{
	public:
		IMPORT_C  TMobileCallUUSRequestV1();
	public:
	/** This field indicates whether the specified service is requested or required 
	for the call. */
		TUint             iServiceReq;
	/** The UUI element. This field only contains data if iServiceReq indicates UUS1.
	
	@see TMobileCallUUI */
		TMobileCallUUI	  iUUI;
		};
	
/** A typedef'd packaged TMobileCallUUSRequestV1 for passing through a generic API 
function member. */
	typedef TPckg<TMobileCallUUSRequestV1> TMobileCallUUSRequestV1Pckg;

	IMPORT_C void ActivateUUS(TRequestStatus& aReqStatus, const TDesC8& aUUSRequest) const;
	IMPORT_C void SendUUI(TRequestStatus& aReqStatus, TBool aMore, const TMobileCallUUI& aUUI) const;
	IMPORT_C void ReceiveUUI(TRequestStatus& aReqStatus, TMobileCallUUI& aUUI) const;
	IMPORT_C void HangupWithUUI(TRequestStatus& aReqStatus, const TMobileCallUUI& aUUI) const;
	IMPORT_C void AnswerIncomingCallWithUUI(TRequestStatus& aReqStatus, const TDesC8& aCallParams, const TMobileCallUUI& aUUI) const;
	
	/***********************************************************************************/
	//
	// Etel 3rd Party v1.0 Parameter classes
	//
	/***********************************************************************************/
	
	class TEtel3rdPartyMobileCallParamsV1 : public RCall::TCallParams
/** Defines the parameters used to set-up of a call originated from an ETel 3rd Party client. 
@publishedPartner
@released
*/
		{
	public:
		IMPORT_C TEtel3rdPartyMobileCallParamsV1();
	public:
	/** Call Id restriction setting to be used for this call.
	
	@see TMobileCallIdRestriction */
		TMobileCallIdRestriction iIdRestrict;
	/** Specifies whether this call attempt is user initiated (EFalse) or a client initiated 
	redial (ETrue). */
		TBool iAutoRedial;		
		};
		
/** A typedef'd packaged TEtel3rdPartyMobileCallParamsV1 for passing through a generic API 
function member. */
	typedef TPckg<TEtel3rdPartyMobileCallParamsV1>TEtel3rdPartyMobileCallParamsV1Pckg;	
	
private:
	RMobileCall(const RMobileCall& aCall);
	CMobileCallPtrHolder* iMmPtrHolder;
protected:
	IMPORT_C void ConstructL();
	IMPORT_C void Destruct();
	};


/*********************************************************/
//
// Line based functionality (RMobileLine)
// 
/*********************************************************/



class CMobileLinePtrHolder;

class RMobileLine : public RLine
/** Provides access to the functionality associated with a specific mobile line. 
*/
	{
public:
	IMPORT_C RMobileLine();

	/***********************************************************************************/
	//
	// MobileLineStatus functional unit
	// 
	/***********************************************************************************/

	IMPORT_C TInt GetMobileLineStatus(RMobileCall::TMobileCallStatus& aStatus) const;
	IMPORT_C void NotifyMobileLineStatusChange(TRequestStatus& aReqStatus, RMobileCall::TMobileCallStatus& aStatus) const;

	//
	// Additional Caps
	//
/** Enumerated auxiliary capabilities. */
	enum TMobileLineAdditionalCaps
		{
	/** Indicates whether the line is a Primary or Auxiliary voice line offset added 
	so that core Etel and additional line caps can be returned in one TUint. */
		KCapsAuxVoice=0x00000008	
		};

private:
	RMobileLine(const RMobileLine& aLine);
	CMobileLinePtrHolder* iMmPtrHolder;
protected:
	IMPORT_C void ConstructL();
	IMPORT_C void Destruct();
	};


/*********************************************************/
//
// SMS Messaging (RMobileSmsMessaging)
// 
/*********************************************************/



class CMobilePhoneSmspList;
class CSmsMessagingPtrHolder;

class RMobileSmsMessaging : public RTelSubSessionBase
/** Provides client access to SMS messaging functionality provided by TSY.
@publishedPartner
@released 
*/
	{
public:

	friend class CRetrieveMobilePhoneSmspList;

	IMPORT_C RMobileSmsMessaging();

	IMPORT_C TInt Open(RMobilePhone& aPhone);
	IMPORT_C void Close();

/** Defines capabilities of SMS messaging.

Modes: GSM/WCDMA */
	enum TMobileSmsModeCaps
		{
	/** SMS is supported in GSM and WCDMA modes. SMS are sent across MM ETel API in 
	a GSM 03.40 TPDU format.
	
	Modes: GSM/WCDMA */
		KCapsGsmSms=0x00000001,
	/** SMS is supported in CDMA mode. SMS are sent across MM ETel API in a IS-637 
	Teleservice message PDU format. 
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		KCapsCdmaSms=0x00000002
		};

/** Defines SMS control capabilities.

Modes: Common */
	enum TMobileSmsControlCaps
		{
	/** TSY supports receiving incoming SMS that have been stored and acknowledged 
	by the phone prior to forwarding to the TSY. If this flag is set then KCapsReceiveEither 
	will not be set and the client can receive stored messages only while the 
	TSY-phone link is in "receive stored" mode.
	
	Modes: Common */
		KCapsReceiveStored=0x00000001,
	/** TSY supports receiving incoming SMS that have been acknowledged by the phone 
	but have NOT already been stored. It is the client's responsibility to do 
	the storage. If this flag is set then KCapsReceiveEither will not be set and 
	the client can receive unstored messages only while the TSY-phone link is 
	in "receive unstored with phone ack" mode.
	
	Modes: Common */
		KCapsReceiveUnstoredPhoneAck=0x00000002,
	/** TSY supports receiving incoming SMS that have NOT already been stored and acknowledged
	by the phone. It is the client's responsibility to do both the storage and 
	the acknowledgment or rejection. If this flag is set then KCapsReceiveEither 
	will not be set and the client can receive unstored messages only while the 
	TSY-phone link is in "receive unstored with client ack" mode.
	
	Modes: Common */
		KCapsReceiveUnstoredClientAck=0x00000004,
	/** TSY supports receiving either stored or unstored incoming SMS at any time. 
	If this flag is set then any other KCapsReceive... flags will not be set and 
	the client can receive stored or unstored messages while the TSY-phone link 
	is in "receive either" mode.
	
	Modes: Common */
		KCapsReceiveEither=0x00000008,
	/** TSY supports creating a default acknowledgment or rejection TPDU using only 
	a "cause code" supplied by the client. If this flag is not set then the client 
	must supply the full ack or nack TPDU if it calls RMobileSmsMessaging::AckSmsStored() 
	or RMobileSmsMessaging::NackSmsStored().
	
	Modes: Common */
		KCapsCreateAck=0x00000010,
	/** TSY supports sending SMS messages but does not support returning the service 
	centre's acknowledgment of the submitted message to the client.
	
	Modes: Common */
		KCapsSendNoAck=0x00000020,
	/** TSY supports sending SMS messages and also supports returning the service centre's 
	acknowledgment of the submitted message to the client.
	
	Modes: GSM/WCDMA */
		KCapsSendWithAck=0x00000040,
	/** TSY supports retrieval of the SMS parameter list.
	
	Modes: GSM/WCDMA */
		KCapsGetSmspList=0x00000080,
	/** TSY supports storage of the SMS parameter list
	
	Modes: GSM/WCDMA */
		KCapsSetSmspList=0x00000100
		};

	class TMobileSmsCapsV1 : public RMobilePhone::TMultimodeType
/** Defines capabilities of SMS messaging.
*/
		{
	public:
		IMPORT_C TMobileSmsCapsV1();

	/** Sum of TMobileSmsModeCaps constants.
	
	Modes: Common */
		TUint32 iSmsMode;
	/** Sum of TMobileSmsControlCaps constants.
	
	Modes: Common */
		TUint32 iSmsControl;
		};

/** A typedef'd packaged TMobileSmsCapsV1 for passing through a generic API member 
function. */
	typedef TPckg<TMobileSmsCapsV1> TMobileSmsCapsV1Pckg;

	IMPORT_C TInt GetCaps(TDes8& aCaps) const;

	// Definitions for sizes of TPDU and User Data fields
	enum 
		{ 
		KGsmTpduSize = 165, // 140 bytes user data + 25 bytes TPDU header
		/*
		 * @deprecated 9.5
		 * */
		KCdmaTpduSize  = 256	// Max size of Bearer Data in Transport Layer message
		};

/** A typedef'd buffer for GSM or WCDMA message data. */
	typedef TBuf8<KGsmTpduSize>			TMobileSmsGsmTpdu;
/** A typedef'd buffer for CDMA message data. 
 @deprecated 9.5 
 */
	typedef TBuf8<KCdmaTpduSize>		TMobileSmsCdmaTpdu;

	/***********************************************************************************/
	//
	// Enum used by TSY to distinguish which SMS attribute class is used by client
	//
	/***********************************************************************************/

/** Used by TSY to distinguish which SMS attribute class is used by client.

Modes: Common

@see RMobileSmsMessaging::TMobileSmsSendAttributesV1 */
	enum TMobileSmsAttributeExtensionId
		{
	/** The iExtensionId contents indicate that the packaged class is a TMobileSmsAttributesV1. */
		KETelMobileSmsAttributesV1=KETelExtMultimodeV1,
	/** The iExtensionId contents indicate that the packaged class is a TMobileSmsReceiveAttributesV1. */
		KETelMobileSmsReceiveAttributesV1,
	/** The iExtensionId contents indicate that the packaged class is a TMobileSmsSendAttributesV1. */
		KETelMobileSmsSendAttributesV1,
	/** The iExtensionId contents indicate that the packaged class is a TMobileSmsCdmaSendAttributesV4. 
	 @deprecated 9.5 
	 */
		KETelMobileSmsCdmaSendAttributesV4=KETelExtMultimodeV4
		};

/** Defines common attributes of all SMS messages.

Modes: CDMA

@see TMobileSmsAttributesV1 
@deprecated 9.5
*/
	enum TMobileSmsAttributeFlags
		{
	/** The iCdmaTeleservice field is valid in the attribute class.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		KCdmaTeleservice = 0x00000001,
	/** The iCdmaServiceCategory field is valid in the attribute class.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		KCdmaServiceCategory = 0x00000002,
	/** The iGsmServiceCentre field is valid in the attribute class.
	
	Modes: GSM/WCDMA */
		KGsmServiceCentre = 0x00000004,
	/** The iDataFormat field is valid in the attribute class.
	
	Modes: Common */
		KSmsDataFormat = 0x00000008,	
	/** The iOriginator or iDestination field is valid in the attribute class.
	
	Modes: Common */
		KRemotePartyInfo = 0x00000010,
	/** The iStatus field is valid in the attribute class.
	
	Modes: Common */
		KIncomingStatus = 0x00000020,
	/** The iStore and iStoreIndex fields are valid in the attribute class.
	
	Modes: Common */
		KStorageLocation = 0x00000040,
	/** The iMsgRef field is valid in the attribute class.

	Modes: Common */
		KMessageReference = 0x00000080,
	/** The iSubmitReport field is valid in the attribute class.
	
	Modes: GSM/WCDMA */
		KGsmSubmitReport = 0x00000100,
	/** The iMore field is valid in the attribute class.
	
	Modes: Common */
		KMoreToSend = 0x00000200,
	/** Indicates to the TSY that it should keep the traffic channel open since there is
	    another message ready to be sent immediately after this one (typically used when
	    sending messages to multiple recipients). TSY should then keep the channel open 
	    until a message is sent with this bit cleared.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		KCdmaKeepChannelOpen = 0x00000400,
	/** Indicates to the TSY that it must wait for the Transport Layer Acknowledgement from the network
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		KCdmaTLAckRequired = 0x00000800,
	/** The iAddressAuthentication field is valid in the attribute class.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		KCdmaAddressAuthentication = 0x00001000
		};

/** Defines the SMS data format.

Modes: Common */
	enum TMobileSmsDataFormat
		{
	/** The format is not specified.

	Modes: Common */
		EFormatUnspecified,
	/** The message data format complies to a SMS TPDU coded as octets according to 
	GSM 03.40.
	
	Modes: GSM/WCDMA */
		EFormatGsmTpdu,
	/** The message data format complies to a SMS Teleservice PDU coded as octets according 
	to IS-637.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		EFormatCdmaTpdu
		};

	class TMobileSmsAttributesV1 : public RMobilePhone::TMultimodeType
	/** Defines common attributes of all SMS messages. 
	@publishedPartner
	@released
	*/
		{
	protected:
		TMobileSmsAttributesV1();
	public:
	/** The bit-mask flags from TMobileSmsAttributeFlags indicating which attributes 
	are present in this instance.
	
	Modes: Common */
		TUint32 iFlags;
	/** The format of the message data buffer .
	
	Modes: Common
	
	@see TMobileSmsDataFormat */
		TMobileSmsDataFormat iDataFormat;
	/** The teleservice this message belongs to, coded according to SMS teleservice 
	identifier values table in TIA/EIA-41-D.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		TInt iCdmaTeleservice;
	/** The service category this message belongs to.
	
	Modes: CDMA 
	@deprecated 9.5
		*/
		TInt iCdmaServiceCategory;
	/** The GSM service centre used for this SMS.
	
	Modes: GSM/WCDMA
	
	@see RMobilePhone::TMobileAddress */
		RMobilePhone::TMobileAddress iGsmServiceCentre;
		};

/** A typedef'd packaged TMobileSmsAttributesV1 for passing through a generic 
API member function. */
	typedef TPckg<TMobileSmsAttributesV1> TMobileSmsAttributesV1Pckg;

/** Defines attributes of incoming SMS messages.

Modes: Common */
	enum TMobileSmsIncomingStatus
		{
	/** The status of the incoming SMS is unknown. */
		EMtMessageUnknownStatus,
	/** The incoming SMS is not stored phone-side but has already been acknowledged 
	by the phone. */
		EMtMessageUnstoredPhoneAck,
	/** The incoming SMS is not stored phone-side and needs to be acknowledged by the 
	client. */
		EMtMessageUnstoredClientAck,
	/** The incoming SMS is stored phone-side. */
		EMtMessageStored
		};

	class TMobileSmsReceiveAttributesV1 : public TMobileSmsAttributesV1
	/** Defines attributes of incoming SMS messages. 
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileSmsReceiveAttributesV1();
	public:
	/** Indicates the status of the incoming message -whether it is stored phone-side 
	or has been acknowledged by the phone.
	
	Modes: Common
	
	@see TMobileSmsIncomingStatus */
		TMobileSmsIncomingStatus	 iStatus;	// indicates if MT message is stored phone-side
	/** If the message is stored phone-side, indicates the index (within the phone-side 
	store) where the message is stored.
	
	Modes: Common */
		TInt						 iStoreIndex;// used if MT message is stored phone-side
	/** If the message is stored phone-side, indicates the name of the appropriate 
	phone-side store.
	
	Modes: Common
	
	@see RMobilePhone::TMobileName */
		RMobilePhone::TMobileName	 iStore;		// used if MT message is stored phone-side
	/** The address of the originator of the SMS.

	Modes: Common
	
	@see RMobilePhone::TMobileAddress */
		RMobilePhone::TMobileAddress iOriginator;
		};

/** A typedef'd packaged TMobileSmsReceiveAttributesV1 for passing through a generic 
API member function. */
	typedef TPckg<TMobileSmsReceiveAttributesV1> TMobileSmsReceiveAttributesV1Pckg;

	

	class TMobileSmsSendAttributesV1 : public TMobileSmsAttributesV1
	/** Defines attributes of sent SMS messages.
	@publishedPartner
	@released */
		{
	public:
		IMPORT_C TMobileSmsSendAttributesV1();
	public:
	/** The message reference assigned to a sent message.
	
	Modes: Common */
		TUint16						 iMsgRef;
	/** A boolean indicating whether the client is going to send another SMS immediately 
	after this one. Equals ETrue if this is the case, EFalse if it is not.
	
	Modes: Common */
		TBool						 iMore;
	/** The SMS-SUBMIT-REPORT TPDU received from network in response to a sent message.
	
	Modes: GSM/WCDMA
	
	@see TMobileSmsGsmTpdu */
		TMobileSmsGsmTpdu			 iSubmitReport;
	/** The address of the destination of the SMS.
	
	Modes: Common
	
	@see RMobilePhone::TMobileAddress */
		RMobilePhone::TMobileAddress iDestination;
		};
	
/** A typedef'd packaged TMobileSmsSendAttributesV1 for passing through a generic 
API member function. */
	typedef TPckg<TMobileSmsSendAttributesV1> TMobileSmsSendAttributesV1Pckg;

	class TMobileSmsCdmaSendAttributesV4 : public TMobileSmsAttributesV1
	/**
	Defines attributes of sent SMS messages (CDMA only).

	@publishedPartner
	@released
	@deprecated 9.5
	*/
		{
	public:
		IMPORT_C TMobileSmsCdmaSendAttributesV4();
	public:
		/** 
		The message id allocated by the SMS stack. The TSY should
		retrieve the value from iCdmaTeleserviceId to determine
		which teleservice this message id corresponds to. 
		*/
		TUint16 iMsgRef;
		/** Indicates whether the client is going to send another 
		PDU immediately after this one (typically used when sending
		messages of more than one PDU). Equals ETrue if at least 
		one more PDU is needed to complete the message and EFalse
		otherwise.
		*/
		TBool iMore;
		/** TPDU received from network in response to a sent message */
		TMobileSmsCdmaTpdu iTLAck;
		/** The address of the destination of the SMS. */
		RMobilePhone::TMobileAddress iDestination;
		/** Transport layer authentication parameter */
		TUint32 iAddressAuthentication;
		};

/** A typedef'd packaged TMobileSmsCdmaSendAttributesV4 for passing through a generic 
API member function. */
	typedef TPckg<TMobileSmsCdmaSendAttributesV4> TMobileSmsCdmaSendAttributesV4Pckg;

	//
	// Setting up the storage & acknowledgement mode
	//

/** Defines the SMS receive modes.

Modes: Common */
	enum TMobileSmsReceiveMode
		{
	/** Receive mode is unspecified and phone will use its default. */
		EReceiveModeUnspecified,
	/** Phone will acknowledge but NOT store all incoming messages before passing them 
	onto TSY. It is client's responsibility to store the message. */
		EReceiveUnstoredPhoneAck,	// client stores but phone acknowledges message
	/** Phone will NOT acknowledge or store any incoming messages before passing them 
	onto TSY. It is client's responsibility to attempt storage and then either
	acknowledge or reject the message. */
		EReceiveUnstoredClientAck,	// client acknowledges and stores message
	/** Phone will store and acknowledge all incoming messages before passing them 
	onto TSY. */
		EReceiveStored,				// phone acknowledges and store message
	/** Phone may pass on either stored or unstored messages to TSY. It is client's 
	responsibility to check RMobileSmsMessaging::TMobileSmsReceiveAttributesV1::iStatus 
	to determine what action(s) the client needs to take for a message. */
		EReceiveEither				// client deals with stored & unstored messages
		};
		
	IMPORT_C void SetReceiveMode(TRequestStatus& aReqStatus, TMobileSmsReceiveMode aReceiveMode) const;
	IMPORT_C TInt GetReceiveMode(TMobileSmsReceiveMode& aReceiveMode) const;
	IMPORT_C void NotifyReceiveModeChange(TRequestStatus& aStatus, TMobileSmsReceiveMode& aReceiveMode);

	/***********************************************************************************/
	//
	// Incoming SMS
	//
	/***********************************************************************************/

	IMPORT_C void ReceiveMessage(TRequestStatus& aReqStatus, TDes8& aMsgData, TDes8& aMsgAttributes) const;

	/***********************************************************************************/
	//
	// Responding to incoming SMS
	//
	/***********************************************************************************/

	IMPORT_C void AckSmsStored(TRequestStatus& aReqStatus, const TDesC8& aMsgData, TBool aFull=EFalse) const;
	IMPORT_C void NackSmsStored(TRequestStatus& aReqStatus, const TDesC8& aMsgData, TInt aRpCause) const;
	IMPORT_C void ResumeSmsReception(TRequestStatus& aReqStatus) const;

	/***********************************************************************************/
	//
	// Outgoing SMS
	//
	/***********************************************************************************/

	IMPORT_C void SendMessage(TRequestStatus& aReqStatus, const TDesC8& aMsgData, TDes8& aMsgAttributes) const;
	IMPORT_C void SendMessageNoFdnCheck(TRequestStatus& aReqStatus, const TDesC8& aMsgData, TDes8& aMsgAttributes) const;

/** Defines the SMS bearer.

Modes: GSM */
	enum TMobileSmsBearer
		{
	/** SMS messages will only be sent over a packet-switched (GPRS) network. */
		ESmsBearerPacketOnly,
	/** SMS messages will only be sent over a circuit-switched (GSM) network. */
		ESmsBearerCircuitOnly,
	/** SMS messages will be sent over the packet-switched (GPRS) network if possible, 
	otherwise over circuit-switched (GSM) network. */
		ESmsBearerPacketPreferred,
	/** SMS messages will be sent over the circuit-switched (GSM) network if possible, 
	otherwise over packet-switched (GPRS) network. */
		ESmsBearerCircuitPreferred
		};

	IMPORT_C void SetMoSmsBearer(TRequestStatus& aReqStatus, TMobileSmsBearer aBearer) const;
	IMPORT_C TInt GetMoSmsBearer(TMobileSmsBearer& aBearer) const;
	IMPORT_C void NotifyMoSmsBearerChange(TRequestStatus& aReqStatus, TMobileSmsBearer& aBearer);

	/***********************************************************************************/
	//
	// Get information on phone-side SMS storage
	//
	/***********************************************************************************/

	IMPORT_C TInt EnumerateMessageStores(TInt& aCount) const;
	IMPORT_C void GetMessageStoreInfo(TRequestStatus& aReqStatus, TInt aIndex, TDes8& aInfo) const;

	/***********************************************************************************/
	//
	// Read/Write SMS parameters to phone-side storage
	//
	/***********************************************************************************/

	enum { KMaxSmspTextSize=30 };

/** Defines the valid SMS parameters store.

Modes: GSM/WCDMA

@see TMobileSmspEntryV1 */
	enum TMobileSmspStoreValidParams
		{
	/** The entry includes a valid iDestination field. */
		KDestinationIncluded=0x00000001,
	/** The entry includes a valid iServiceCentre field. */
		KSCAIncluded=0x00000002,
	/** The entry includes a valid iProtocolId field. */
		KProtocolIdIncluded=0x00000004,
	/** The entry includes a valid iDcs field. */
		KDcsIncluded=0x00000008,
	/** The entry includes a valid iValidityPeriod field. */
		KValidityPeriodIncluded=0x00000010
 		};

	class TMobileSmspEntryV1 : public RMobilePhone::TMultimodeType

	/** Defines a set of SMS parameters. 
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileSmspEntryV1();
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
	/** The slot index where this SMSP entry is stored in the SIM's SMSP file. */
		TInt iIndex;
	/** A bit-wise sum of the constants defined in TMobileSmspStoreValidParams, that 
	specifies which parameters are valid within this entry. */
		TUint32	iValidParams;
	/** Default protocol ID. Coded according to GSM 03.40. */
		TUint8 iProtocolId;
	/** Default data coding scheme. Coded according to GSM 03.40. */
		TUint8 iDcs;
	/** Default validity period, in relative format and coded on 8 bits as defined 
	by GSM 03.40. */
		TUint8 iValidityPeriod;
	/** Reserved for future use. */
		TUint8 iReservedFiller;
	/** Default destination for outgoing SMS.
	
	@see RMobilePhone::TMobileAddress */
		RMobilePhone::TMobileAddress iDestination;
	/** Default service centre for outgoing SMS.
	
	@see RMobilePhone::TMobileAddress */
		RMobilePhone::TMobileAddress iServiceCentre;
	/** The alpha-tag associated with this SMSP entry, expressed as Unicode characters. */
		TBuf<KMaxSmspTextSize> iText;
		};

/** A typedef'd packaged TMobileSmspEntryV1 for passing through a generic API member
function. */
	typedef TPckg<TMobileSmspEntryV1> TMobileSmspEntryV1Pckg;
	IMPORT_C void StoreSmspListL(TRequestStatus& aReqStatus, CMobilePhoneSmspList* aSmspList) const;
	IMPORT_C void NotifySmspListChange(TRequestStatus& aReqStatus) const;

private:
	RMobileSmsMessaging(const RMobileSmsMessaging&);
	CSmsMessagingPtrHolder* iSmsMessagingPtrHolder;
protected:
	IMPORT_C void ConstructL();
	IMPORT_C void Destruct();
	};


/*********************************************************/
//
// Broadcast Messaging (RMobileBroadcastMessaging)
// 
/*********************************************************/



class CMobilePhoneBroadcastIdList;

class CCbsMessagingPtrHolder;
class RMobileBroadcastMessaging : public RTelSubSessionBase
/** This sub-session provides access to the broadcast message services provided 
by GSM/WCDMA and CDMA networks.

Clients will open a RMobileBroadcastMessaging sub-session and then wait for 
incoming broadcast messages. The broadcast messages received can depend upon 
a "message filter" which defines the languages and/or identifiers of acceptable 
(or unacceptable) messages. This sub-session and the function members within 
this section must be supported if the TSY indicates that it supports the MobileBroadcastMessaging
functional unit. 

@publishedPartner
@released*/
	{
public:
	
	friend class CRetrieveMobilePhoneBroadcastIdList;

	IMPORT_C RMobileBroadcastMessaging();

	IMPORT_C TInt Open(RMobilePhone& aPhone);
	IMPORT_C void Close();

	/***********************************************************************************/
	//
	// Broadcast messaging capabilities
	//
	/***********************************************************************************/

/** Broadcast capabilities.

Modes:GSM/WCDMA */
	enum TMobileBroadcastModeCaps
		{
	/** GSM 03.41 cell broadcast messages are supported.
	
	Modes:GSM/WCDMA */
		KCapsGsmTpduFormat = 0x00000001,
	/** IS-637-A broadcast messages are supported.
	
	Modes:CDMA 
	@deprecated 9.5
	*/
		KCapsCdmaTpduFormat = 0x00000002,
	/** 3GPP 25.324 Release 4 cell broadcast messages are supported.
	
	Modes: WCDMA */
		KCapsWcdmaTpduFormat = 0x00000004
		};

/** Broadcast message filter capabilities.

Modes: Common

@see RMobileBroadcastMessaging::GetLanguageFilter() */
	enum TBroadcastMessagingFilterCaps
		{
	/** Phone supports switching broadcast messages on or off. This means the EbroadcastAcceptNone 
	and EBroadcastAcceptAll settings of TMobilePhoneBroadcastFilter are supported.
	
	Modes: Common */
		KCapsSimpleFilter = 0x00000001,
	/** Phone supports filtering of broadcast messages depending upon their language. 
	The language filter is a list of languages used only for broadcast filtering.
	
	Modes: Common
	
	@see RMobileBroadcastMessaging::GetLanguageFilter() */
		KCapsLangFilter = 0x00000002,
	/** Phone supports filtering of broadcast messages depending upon their subject. 
	The subject of a message is identified in GSM/WCDMA mode by the Cell Broadcast 
	Message Identifier (CBMI) and in CDMA mode by the Service Category.
	
	Modes: Common */
		KCapsIdFilter = 0x00000004
		};

	

	class TMobileBroadcastCapsV1 : public RMobilePhone::TMultimodeType
	/** Defines capabilities of Broadcast messaging. 
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileBroadcastCapsV1();
	public:
	/** Sum of TMobileBroadcastModeCaps constants.
	
	Modes: GSM/WCDMA */
		TUint32 iModeCaps;
	/** Sum of TBroadcastMessagingFilterCaps constants.
	
	Modes: GSM/WCDMA */
		TUint32 iFilterCaps;
		};

/** A typedef'd packaged TMobileBroadcastCapsV1 for passing through a generic API 
function member. */
	typedef TPckg<TMobileBroadcastCapsV1> TMobileBroadcastCapsV1Pckg;

	IMPORT_C TInt GetCaps(TDes8& aCaps) const;

/** Defines the broadcast attributes.

Modes: Common */
	enum TMobileBroadcastAttributeFlags
		{
	/** The iFormat field is valid in the attribute class.
	
	Modes: Common */
		KBroadcastDataFormat = 0x00000001,
	/** The iServiceCategory field is valid in the attribute class.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		KCdmaServiceCategory = 0x00000002
		};

/** The mobile broadcast data format, used byTMobileBroadcastAttributesV1.

Modes: Common */
	enum TMobileBroadcastDataFormat
		{
	/** The message data format is not specified.
	
	Modes: Common */
		EFormatUnspecified,
	/** The message data format complies to a Cell Broadcast TPDU coded as 88 octets 
	(6 for header and 82 for message data) according to GSM 03.41.
	
	Modes: GSM/WCDMA */
		EFormatGsmTpdu,
	/** The message data format complies to IS-637-A encoding of the Bearer Data parameter 
	within the Transport Layer's SMS-Broadcast message.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		EFormatCdmaTpdu,
	/** The message data format complies to a Cell Broadcast TPDU coded as 1252 octets 
	(6 for header and a maximum of 1246 for message data) according to 3GPP 25.324.
	
	Modes: WCDMA */
		EFormatWcdmaTpdu
		};

	

	class TMobileBroadcastAttributesV1 : public RMobilePhone::TMultimodeType
	/** Defines attributes of a Broadcast message.
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileBroadcastAttributesV1();
	public:
	/** The bit-mask flags indicating which attributes are present in this instance.
	
	Modes: Common
	
	@see TMobileBroadcastAttributeFlags */
		TUint32	iFlags;
	/** Format of the message data buffer.
	
	Modes: Common
	
	@see TMobileBroadcastDataFormat */
		TMobileBroadcastDataFormat	iFormat;
	/** Service category of the message.
	
	Modes: CDMA 
	@deprecated 9.5 
	*/
		TInt iServiceCategory;
		};

/** A typedef'd packaged TMobileBroadcastAttributesV1 for passing through a generic 
API function member. */
	typedef TPckg<TMobileBroadcastAttributesV1> TMobileBroadcastAttributesV1Pckg;


	

	class TMobileBroadcastAttributesV2 : public TMobileBroadcastAttributesV1
	/** Defines attributes of a UMTS Broadcast message for version v2.0 of the API. 
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileBroadcastAttributesV2();
	public:
	/** The number of pages that the current broadcast message consists of. The maximum 
	number of pages for WCDMA Broadcast messages is 15. */
		TInt	 iNumberOfPages;
	/** This parameter is used to store the message type for this broadcast message. */
		TUint8	 iMessageType;
	/** This parameter is used to store the message id for this broadcast message.
	
	This parameter is used to store the serial number for this broadcast message.
	
	This parameter is used to store the data coding scheme for this broadcast 
	message. */
		TUint16	 iMessageId;
	/** This parameter is used to store the serial number for this broadcast message. */
		TUint16	 iSerialNum;
	/** This parameter is used to store the data coding scheme for this broadcast message. */
		TUint8	 iDCS;
		};

/** A typedef'd packaged TMobileBroadcastAttributesV1 for passing through a generic 
API function member. */
	typedef TPckg<TMobileBroadcastAttributesV2> TMobileBroadcastAttributesV2Pckg;

	/***********************************************************************************/
	//
	// Receiving broadcast messages
 	//
	/***********************************************************************************/

	/** A constant which defines the length of the buffer to be passed to ReceiveMessage.	
	
	This constant should be used for all message types: GSM, CDMA and WCDMA.*/
	enum
		{
		KBroadcastPageSize = 88	
		};
	
	/** Buffer for received messages. 
	
	This buffer should be used for all message types: GSM, CDMA and WCDMA.*/
	typedef TBuf8<KBroadcastPageSize> TBroadcastPageData;

	/** Old buffer sizes.  KBroadcastPageSize should be used instead. */
	enum 
		{
		KGsmBroadcastDataSize = KBroadcastPageSize,
		/*
		 @deprecated 9.5
		 */
		KCdmaBroadcastDataSize = KBroadcastPageSize,
		KWCdmaBroadcastPageSize = KBroadcastPageSize
		};

	/** Buffer for GSM messages.  TBroadcastPageData should be used instead. */
	typedef TBuf8<KGsmBroadcastDataSize>  TGsmBroadcastMessageData;
	/** Buffer for CDMA messages.  TBroadcastPageData should be used instead. 
	 @deprecated 9.5
	  */
	typedef TBuf8<KCdmaBroadcastDataSize> TCdmaBroadcastMessageData;
	/** Buffer for WCDMA messages.  TBroadcastPageData should be used instead. */
	typedef TBuf8<KWCdmaBroadcastPageSize> TWCdmaBroadcastPageData;

	IMPORT_C void ReceiveMessage(TRequestStatus& aReqStatus, TDes8& aMsgData, TDes8& aMsgAttributes) const;

	/***********************************************************************************/
	//
	// Filters
	//
	/***********************************************************************************/

/** The filter settings of the mobile phone.

Modes: Common */
	enum TMobilePhoneBroadcastFilter
		{
	/** The phone does not have a filter setting. */
		EBroadcastFilterUnspecified,
	/** The phone is not accepting any broadcast messages. */
		EBroadcastAcceptNone,
	/** The phone is accepting all broadcast messages. */
		EBroadcastAcceptAll,
	/** The phone is accepting those broadcast messages included within the language 
	and identifier filters. */
		EBroadcastAcceptFilter,
	/** The phone is rejecting those broadcast messages included within the language 
	or identifier filters. */
		EBroadcastRejectFilter
		};

	IMPORT_C TInt GetFilterSetting(TMobilePhoneBroadcastFilter& aSetting) const;
	IMPORT_C void SetFilterSetting(TRequestStatus& aReqStatus, TMobilePhoneBroadcastFilter aSetting) const;
	IMPORT_C void NotifyFilterSettingChange(TRequestStatus& aReqStatus, TMobilePhoneBroadcastFilter& aSetting) const;

	IMPORT_C void GetLanguageFilter(TRequestStatus& aReqStatus, TDes16& aLangFilter) const;
	IMPORT_C void SetLanguageFilter(TRequestStatus& aReqStatus, const TDesC16& aLangFilter) const;
	IMPORT_C void NotifyLanguageFilterChange(TRequestStatus& aReqStatus, TDes16& aLangFilter) const;

	class TMobileBroadcastIdEntryV1 : public RMobilePhone::TMultimodeType
	/** In GSM - defines a Cell Broadcast Message Identifier (CBMI) list entry. In 
	CDMA - defines a Service Category list entry. 
	@publishedPartner
	@released
	*/
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
		IMPORT_C TMobileBroadcastIdEntryV1();
	public:
	/** The value of the message identifier. */
		TUint16	iId;
		};

/** Defines the broadcast types. */
	enum TMobileBroadcastIdType
		{
	/** GSM broadcast. */
		EGsmBroadcastId,
	/** CDMA broadcast.
	 @deprecated 9.5
	  */
		ECdmaBroadcastId
		};

	IMPORT_C void StoreBroadcastIdListL(TRequestStatus& aReqStatus, CMobilePhoneBroadcastIdList* aIdList, TMobileBroadcastIdType aIdType);
	IMPORT_C void NotifyBroadcastIdListChange(TRequestStatus& aReqStatus) const;

private:
	RMobileBroadcastMessaging(const RMobileBroadcastMessaging&);
	CCbsMessagingPtrHolder* iCbsMessagingPtrHolder;
protected:
	IMPORT_C void ConstructL();
	IMPORT_C void Destruct();
	};

/*********************************************************/
//
// USSD Messaging (RMobileUssdMessaging)
// 
/*********************************************************/



class CUssdMessagingPtrHolder;
class RMobileUssdMessaging : public RTelSubSessionBase
/** Provides client access to USSD functionality provided by TSY. 
*/
	{
public:
	IMPORT_C RMobileUssdMessaging();

	IMPORT_C TInt Open(RMobilePhone& aPhone);
	IMPORT_C void Close();

/** Defines the USSD Messaging Format Capabilities.

Modes: GSM/WCDMA */
	enum TMobileUssdFormatCaps
		{
	/** USSD messages coded as a packed string within 160 octets, as defined for a 
	ussd-String within GSM 04.80 and GSM 03.38.
	
	Modes: GSM/WCDMA */
		KCapsPackedString=0x00000001
		};

/** Defines the USSD Messaging Type Capabilities.

Modes: GSM/WCDMA */
	enum TMobileUssdTypeCaps
		{
	/** Outgoing USSD messages are supported.
	
	Modes: GSM/WCDMA */
		KCapsMOUssd=0x00000001,
	/** Incoming USSD messages are supported.
	
	Modes: GSM/WCDMA */
		KCapsMTUssd=0x00000002
		};

	

	class TMobileUssdCapsV1 : public RMobilePhone::TMultimodeType
	/** Defines capabilities of USSD messaging. 
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileUssdCapsV1();
	/** Sum of TMobileUssdFormatCaps constants.
	
	Modes: GSM/WCDMA

	@see TMobileUssdFormatCaps */
		TUint32 iUssdFormat;
	/** Sum of TMobileUssdTypeCaps constants.
	
	Modes: GSM/WCDMA

	@see TMobileUssdTypeCaps */
		TUint32 iUssdTypes;
		};

/** A typedef'd packaged TMobileUssdCapsV1 for passing through a generic API member 
function. */
	typedef TPckg<TMobileUssdCapsV1> TMobileUssdCapsV1Pckg;

	IMPORT_C TInt GetCaps(TDes8& aCaps) const;

/** Defines the USSD Messaging Attributes.

Modes: GSM/WCDMA

@see TMobileUssdAttributesV1 */
	enum TMobileUssdAttributeFlags
		{
	/** The iFormat field is valid in the attribute class. */
		KUssdDataFormat = 0x00000001,
	/** The iType field is valid in the attribute class. */
		KUssdMessageType = 0x00000002,
	/** The iDcs field is valid in the attribute class. */
		KUssdMessageDcs = 0x00000004
		};

/** Defines the USSD Data Formats.

Modes: Common */
	enum TMobileUssdDataFormat
		{
	/** The message data format is unspecified.
	
	Modes: Common */
		EFormatUnspecified,
	/** The message data format complies to a USSD coded as 160 octets as defined for 
	a ussd-String within GSM 04.80 and GSM 03.38.
	
	Modes: GSM/WCDMA */
		EFormatPackedString
		};

/** Defines the USSD Message Types.

Modes: GSM/WCDMA */
	enum TMobileUssdMessageType
		{
	/** Message is an unknown type. */
		EUssdUnknown,
	/** Message is a mobile originated USSD request. The MS expects the network to 
	send a USSD MT reply. */
		EUssdMORequest,
	/** Message is a reply to a previously received network initiated USSD request. 
	It is a USSD MO reply. */
		EUssdMOReply,
	/** Message is a one-off network initiated USSD notification. The network expects the MS to
	send a USSD MO acknowledgement with empty message content. */
		EUssdMTNotify,
	/** Message is a network initiated USSD request. The network expects
	the MS to send a USSD MO reply. */
		EUssdMTRequest,
	/** Message is a network initiated reply to a previously sent MO USSD request. */
		EUssdMTReply,
	/** For acknowledging MT USSD notifications (empty message content). */
		EUssdMOAcknowledgement
		};

	

	class TMobileUssdAttributesV1 : public RMobilePhone::TMultimodeType
	/** Defines attributes of a USSD message. 
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileUssdAttributesV1();
	public:
	/** The TMobileUssdAttributeFlags bit-mask flags indicating which attributes are
	present in this instance.
	
	Modes: GSM/WCDMA

	@see TMobileUssdAttributeFlags */
		TUint32 iFlags;
	/** Format of the message data buffer.
	
	Modes: GSM/WCDMA
	
	@see TMobileUssdDataFormat */
		TMobileUssdDataFormat iFormat;
	/** The type of USSD message.
	
	Modes: GSM/WCDMA
	
	@see TMobileUssdMessageType */
		TMobileUssdMessageType iType;
	/** The Data Coding Scheme of the USSD message.
	
	Modes: GSM/WCDMA */
		TUint8 iDcs;
		};
	
/** A typedef'd packaged TMobileUssdAttributesV1 for passing through a generic 
API member function. */
	typedef TPckg<TMobileUssdAttributesV1> TMobileUssdAttributesV1Pckg;

	/***********************************************************************************/
	//
	// Receiving USSD messages
 	//
	/***********************************************************************************/

	enum 
		{
		KGsmUssdDataSize = 160
		};

/** A typedef'd buffer to hold the message data. */
	typedef TBuf8<KGsmUssdDataSize> TGsmUssdMessageData;

	IMPORT_C void ReceiveMessage(TRequestStatus& aReqStatus, TDes8& aMsgData, TDes8& aMsgAttributes) const;

	/***********************************************************************************/
	//
	// Sending USSD messages
	//
	/***********************************************************************************/

	IMPORT_C void SendMessage(TRequestStatus& aReqStatus, const TDesC8& aMsgData, const TDesC8& aMsgAttributes) const;
	IMPORT_C void SendMessageNoFdnCheck(TRequestStatus& aReqStatus, const TDesC8& aMsgData, const TDesC8& aMsgAttributes) const;
	
	IMPORT_C void SendRelease(TRequestStatus& aReqStatus, TDes8& aReturnResult) const;
	IMPORT_C void NotifyNetworkRelease(TRequestStatus& aReqStatus, TDes8& aReturnResult) const;
	IMPORT_C void NotifyNetworkRelease(TRequestStatus& aReqStatus, TDes8& aMsgData, TDes8& aMsgAttributes) const;

private:
	RMobileUssdMessaging(const RMobileUssdMessaging&);
	CUssdMessagingPtrHolder* iUssdMessagingPtrHolder;
protected:
	IMPORT_C void ConstructL();
	IMPORT_C void Destruct();
	};



/*********************************************************/
//
// SMS Message Storage (RMobileSmsStore)
//
/*********************************************************/



class CSmsStorePtrHolder;
class CMobilePhoneSmsList;
class RMobileSmsStore : public RMobilePhoneStore
/** Provides client access to SMS storage functionality provided by TSY.
@publishedPartner
@released
*/
	{
public:

	IMPORT_C RMobileSmsStore();
	IMPORT_C TInt Open(RMobileSmsMessaging& aMessaging, const TDesC& aStoreName);
	IMPORT_C void Close();

/** Defines the SMS Store Capabilities.

Modes: Common */
	enum TMobileSmsStoreCaps
		{
	/** The store contains unread, incoming SMS entries.
	
	Modes: Common */
		KCapsUnreadMessages = 0x00000001,
	/** The store contains read, incoming SMS entries.
	
	Modes: Common */
		KCapsReadMessages = 0x00000002,
	/** The store contains sent SMS entries.
	
	Modes: Common */
		KCapsSentMessages = 0x00000004,
	/** The store contains un sent SMS entries. */
		KCapsUnsentMessages = 0x00000008,
	/** The store contains GSM SMS message entries â€“ so TMobileGsmSmsEntryV1 class 
	should be used.
	
	Modes: GSM/WCDMA */
		KCapsGsmMessages = 0x00000010,
	/** The store contains CDMA SMS message entries â€“ so TMobileCdmaSmsEntryV1 class 
	should be used.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		KCapsCdmaMessages = 0x00000020
		};

/** Defines contents of a fixed-size, stored SMS entry.

Modes: Common */
	enum TMobileSmsStoreStatus
		{
	/** The status of the SMS is unknown. */
		EStoredMessageUnknownStatus,
	/** The SMS is stored phone-side. It is an incoming message that has not been read 
	yet. */
		EStoredMessageUnread,
	/** The SMS is stored phone-side. It is an incoming message that has already been 
	read. */
		EStoredMessageRead,
	/** The SMS is stored phone-side. It is an outgoing message that has not been sent 
	yet. */
		EStoredMessageUnsent,
	/** The SMS is stored phone-side. It is an outgoing message that has already been 
	sent but a delivery/status report has either not been received yet or was 
	not requested in the first place. */
		EStoredMessageSent,
	/** The SMS is stored phone-side. It is an outgoing message that has already been 
	sent and a delivery/status report has been received */
		EStoredMessageDelivered
		};

	//
	// Enum used by TSY to distinguish which SMS entry class is used by client
	//

/** Used by TSY to distinguish which SMS entry class is used by client. */
	enum TMobileSmsEntryExtensionId
		{
	/** Type is unknown. */
		KETelMobileSmsEntryV1=KETelExtMultimodeV1,
	/** The iExtensionId contents indicate that the packaged class is a TMobileGsmSmsEntryV1.
	
	Also used to indicate that GSM SMS entries will be retrieved by CRetrieveMobilePhoneSmsList. */
		KETelMobileGsmSmsEntryV1,
	/** The iExtensionId contents indicate that the packaged class is a TMobileCdmaSmsEntryV1.
	
	Also used to indicate that CDMA SMS entries will be retrieved by CRetrieveMobilePhoneSmsList. 
	@deprecated 9.5
	*/
		KETelMobileCdmaSmsEntryV1
		};

	class TMobileSmsEntryV1 : public RMobilePhoneStore::TMobilePhoneStoreEntryV1
	/** Defines contents of a fixed-size, stored SMS entry.
	@publishedPartner
	@released 
	*/
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	protected:
		TMobileSmsEntryV1();
	public:
	/** The status of the stored message.
	
	Modes: Common
	
	@see TMobileSmsStoreStatus */
		TMobileSmsStoreStatus	iMsgStatus;	
		};

/** A typedef'd packaged TMobileSmsEntryV1 for passing through a generic API 
member function. */
	typedef TPckg<TMobileSmsEntryV1> TMobileSmsEntryV1Pckg;
	
	class TMobileGsmSmsEntryV1 : public TMobileSmsEntryV1
/** Defines contents of a fixed-size, stored GSM SMS entry.
@publishedPartner
@released */
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		IMPORT_C TMobileGsmSmsEntryV1();
	public:
	/** The service centre to use (or used) for the message.
	
	Modes: GSM/WCDMA
	
	@see RMobilePhone::TMobileAddress */
		RMobilePhone::TMobileAddress iServiceCentre;
	/** The GSM 03.40 TPDU.
	
	Modes: GSM/WCDMA
	
	@see RMobileSmsMessaging::TMobileSmsGsmTpdu */
		RMobileSmsMessaging::TMobileSmsGsmTpdu	iMsgData;	
		};

/** A typedef'd packaged TMobileGsmSmsEntryV1 for passing through a generic API 
member function. */
	typedef TPckg<TMobileGsmSmsEntryV1> TMobileGsmSmsEntryV1Pckg;

	class TMobileCdmaSmsEntryV1 : public TMobileSmsEntryV1
	/** Defines contents of a fixed-size, stored CDMA SMS entry. 
	@publishedPartner
	@released
	
	@deprecated 9.5
	*/
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		IMPORT_C TMobileCdmaSmsEntryV1();
	public:
	/** The teleservice identifier of the stored message.
	
	Modes: CDMA
	@deprecated 9.5
	 */
		TInt iTeleservice;
	/** The service category of the stored message.
	
	Modes: CDMA */
		TInt iServiceCategory;
	/** The remote party (destination or originator) of the stored message.
	
	Modes: CDMA
	
	@see RMobilePhone::TMobileAddress */
		RMobilePhone::TMobileAddress iRemoteParty;
	/** The teleservice layer bearer data of the stored message.
	
	Modes: CDMA
	
	@see RMobileSmsMessaging::TMobileSmsCdmaTpdu */
		RMobileSmsMessaging::TMobileSmsCdmaTpdu iMsgData;	
		};

/** A typedef'd packaged TMobileCdmaSmsEntryV1 for passing through a generic API 
member function.
@deprecated 9.5 
*/
	typedef TPckg<TMobileCdmaSmsEntryV1> TMobileCdmaSmsEntryV1Pckg;

protected:
	IMPORT_C void ConstructL();
private:
	RMobileSmsStore(const RMobileSmsStore&);
	};

/*********************************************************/
//
// NAM Storage (RMobileNamStore)
//
/*********************************************************/



class CNamStorePtrHolder;
class CMobilePhoneNamList;
class CMobilePhoneNamListV4;

class RMobileNamStore : public RMobilePhoneStore
/**
Provides client access to NAM storage functionality provided by TSY

	@deprecated 9.5

*/
	{
public:
	IMPORT_C RMobileNamStore();
	IMPORT_C TInt Open(RMobilePhone& aPhone);
	IMPORT_C void Close();



	class TMobileNamStoreInfoV1 : public RMobilePhoneStore::TMobilePhoneStoreInfoV1
	/**
	Defines information about a NAM store
	
	@publishedPartner
    @released
	*/
		{
	public:
		IMPORT_C TMobileNamStoreInfoV1();
	public:
	/** The number of enabled NAMs within the NAM store.
	
	Modes: CDMA */
		TInt iNamCount;
	/** The index of the NAM that is currently the active NAM, will be between 0 and 
	iNamCount-1.
	
	Modes: CDMA */
		TInt iActiveNam;
		};

/** A typedef'd packaged TMobileNamStoreInfoV1 for passing through a generic API 
member function. */
	typedef TPckg<TMobileNamStoreInfoV1> TMobileNamStoreInfoV1Pckg;

	IMPORT_C void SetActiveNam(TRequestStatus& aReqStatus, TInt aNamId) const;

	enum
		{
		/* The original maximum NAM parameter size for a TMobileNamEntryV1 */
		KMaxNamParamSize = 64,
		/* Updated maximum NAM parameter size for TMobileNamEntryV4 */
		KMaxNamParamSizeV4 = 256
		};

	class TMobileNamEntryV1 : public RMobilePhoneStore::TMobilePhoneStoreEntryV1
	/**
	Defines contents of a NAM store entry
	
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileNamEntryV1();
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
	/** The NAM that is to be accessed, will be between 0 and TMobileNamStoreInfoV1::iNamCount-1.
	
	Modes: CDMA */
		TInt iNamId;
	/** The identifier of the NAM parameter to be accessed.
	
	TSY must define values of parameter identifiers.
	
	Modes: CDMA */
		TInt iParamIdentifier;
	/** The data contents of the selected parameter.
	
	Modes: CDMA
	
	@see KMaxNamParamSize */
		TBuf8<KMaxNamParamSize> iData;
		};

	/** A typedef'd packaged TMobileNamEntryV1 for passing through a generic API member 
	function. */
	typedef TPckg<TMobileNamEntryV1> TMobileNamEntryV1Pckg;

	/** Enumeration for the standard NAM parameters (see 3GPP2 C.S0005-A appendix F.3)
	Modes: CDMA */
	enum TStandardNamParameters
		{
		/** The 64-bit pattern stored in the Mobile Station and 
			Home Location Register/Authentication Centre used to generate/update the 
			mobile station’s Shared Secret Data
			Corresponds to NAM Indicator 'A_KEY' */
		EAKey,
		/** 64-bit key used for authentication
			Corresponds to NAM Indicator 'SSD_A(s-p)' */
		ESharedSecretDataA,
		/** 64-bit key used as input to generate encryption mask and long code
			Corresponds to NAM Indicator 'SSD_B(s-p)' */
		ESharedSecretDataB,
		/** Modulo-64 event counter maintained by the mobile station and Authentication Center 
			used for clone detection
			Corresponds to NAM Indicator 'COUNT(s-p)' */
		EParameterUpdateReceivedCount,
		/** Class of the International Mobile Station Identity (IMSI) containing a MIN as 
			the lower 10 digits 
			Corresponds to NAM Indicator 'IMSI_M_CLASS(p)' */
		EMinBasedImsiClass,
		/** The class of the International Mobile Station Identity (IMSI) not containing a MIN as 
			the lower 10 digits 
			Corresponds to NAM Indicator 'IMSI_T_CLASS(p)' */
		ETrueImsiClass,
		/** 34-bit number derived from the IMSI M used as input for authentication
			Corresponds to NAM Indicator 'IMSI_M_S(p)' */
		EMinBasedImsiS,
		/** 34-bit number derived from the IMSI T used as input for authentication
			Corresponds to NAM Indicator 'IMSI_T_S(p)' */
		ETrueImsiS,
		/** Number of digits in the IMSI M minus 4
			Corresponds to NAM Indicator 'IMSI_M_ADDR_NUM(p)' */
		EMinBasedImsiAddressNum,
		/** The number of digits in the IMSI T minus 4
			Corresponds to NAM Indicator 'IMSI_T_ADDR_NUM(p)' */
		ETrueImsiAddressNum,
		/** The 11th and 12th digits in the IMSI M
			Corresponds to NAM Indicator 'IMSI_M_11_12(p)' */
		EMinBasedImsiDigits11and12,
		/** The 11th and 12th digits in the IMSI T
			Corresponds to NAM Indicator 'IMSI_T_11_12(p)' */
		ETrueImsiDigits11and12,
		/** The country code for the IMSI M
			Corresponds to NAM Indicator 'MCC_M(p)' */
		EMinBasedMobileCountryCode,
		/** The country code for the IMSI T
			Corresponds to NAM Indicator 'MCC_T(p)' */
		ETrueMobileCountryCode,
		/** Up to 15 digit dialable number associated with the mobile station through a service 
			subscription
			Corresponds to NAM Indicator 'MDN(p)' */
		EMobileDirectoryNumber,
		/** 4-bit number giving the length of the assigning Temporary Mobile Station Identity (TMSI) zone 
			(can range from 1 to 8 octets)
			Corresponds to NAM Indicator 'ASSIGNING_TMSI_ZONE_LEN(s-p)' */
		EAssigningTmsiZoneLen,
		/** Temporary Mobile Station Identity (TMSI) zone is an arbitrary set of base stations 
			for the administrative assignment of TMSI
			Corresponds to NAM Indicator 'ASSIGNING_TMSI_ZONE(s-p)' */
		EAssigningTmsiZone,
		/** The uniquely assigned number to a mobile station inside a TMSI zone
			Corresponds to NAM Indicator 'TMSI_CODE(s-p)' */
		ETmsiCode,
		/** The expiration time used to automatically delete the assigned TMSI
			Corresponds to NAM Indicator 'TMSI_EXP_TIME(s-p)' */
		ETmsiExpTime,
		/** Mobile station's home System Id for 800MHz analog operation
			Corresponds to NAM Indicator 'HOME_SID(p)' */
		EHomeSystemIdAnalog,
		/** Mobile station's home System Id
			Corresponds to NAM Indicator 'SID(p)' */
		EHomeSystemId,
		/** Mobile station's home Network Id
			Corresponds to NAM Indicator 'NID(p)' */
		EHomeNetworkId,
		/** Mobile station is configured to receive mobile station terminated calls when 
			using a home (SID, NID) pair
			Corresponds to NAM Indicator 'MOB_TERM_HOME(p)' */
		EReceiveCallsHome,
		/** The mobile station is configured to receive mobile station terminated calls when 
			it is a foreign SID roamer
			Corresponds to NAM Indicator 'MOB_TERM_FOR_SID(p)' */
		EReceiveCallsForeignSystem,
		/** The mobile station is configured to receive mobile station terminated calls when 
			it is a foreign NID roamer
			Corresponds to NAM Indicator 'MOB_TERM_FOR_NID(p)' */
		EReceiveCallsForeignNetwork,
		/**	A TSY may use this as the first TSY-defined non-standard NAM parameter */
		ENumStandardNamParameters
		};

	class TMobileNamEntryV4 : public RMobilePhoneStore::TMobilePhoneStoreEntryV1
	/**
	Defines contents of a NAM store entry

	The differences between the original TMobileNamEntryV1, and updated TMobileNamEntryV4 are:
	- the maximum mobile NAM parameter size is KMaxNamParamSize(64) for TMobileNamEntryV1
	  and KMaxNamParamSizeV4(256) for TMobileNamEntryV4
	- TMobileNamEntryV4 defines a set of standard NAM parameters for the iParamIdentifier as
	  described in 3GPP2 C.S0005-A appendix F.3
	
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileNamEntryV4();
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
	/** The NAM that is to be accessed, will be between 0 and TMobileNamStoreInfo::iNamCount-1.
	
	Modes: CDMA */
		TInt iNamId;

	/** The identifier of the NAM parameter to be accessed.
	iParamIdentifier will be a TStandardNamParameters value if a standard NAM parameter is being accessed, 
	however a TSY may support and define NAM parameters in addition to those given by TStandardNamParameters	
	Modes: CDMA */
		TInt iParamIdentifier;

	/** The data contents of the selected parameter.
	
	Modes: CDMA
	
	@see KMaxNamParamSizeV4 */
		TBuf8<KMaxNamParamSizeV4> iData;
		};

	/** A typedef'd packaged TMobileNamEntryV4 for passing through a generic API member 
	function. */
	typedef TPckg<TMobileNamEntryV4> TMobileNamEntryV4Pckg;

	IMPORT_C void StoreAllL(TRequestStatus& aReqStatus, TInt aNamId, CMobilePhoneNamList* aNamList) const;
	IMPORT_C void StoreAllL(TRequestStatus& aReqStatus, TInt aNamId, CMobilePhoneNamListV4* aNamList) const;

protected:
	IMPORT_C void ConstructL();
private:
	RMobileNamStore(const RMobileNamStore&);
	};


/*********************************************************/
//
// Own Number Storage (RMobileONStore)
//
/*********************************************************/



class CONStorePtrHolder;
class CMobilePhoneONList;
	
class RMobileONStore : public RMobilePhoneStore
/**
Provides client access to Own Number storage functionality provided by TSY

*/
	{
public:
	IMPORT_C RMobileONStore();
	IMPORT_C TInt Open(RMobilePhone& aPhone);
	IMPORT_C void Close();

	

	class TMobileONStoreInfoV1 : public RMobilePhoneStore::TMobilePhoneStoreInfoV1
	/**
	Defines information about an Own Number store
	
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileONStoreInfoV1();
	public:
	/** The maximum length of the own phone number can be. */
		TInt iNumberLen;
	/** The maximum length the text field can be. */
		TInt iTextLen;
		};

/** A typedef'd packaged TMobileONStoreInfoV1 for passing through a generic API 
member function. */
	typedef TPckg<TMobileONStoreInfoV1> TMobileONStoreInfoV1Pckg;

	enum
		{
		KOwnNumberTextSize = 20
		};

	

	class TMobileONEntryV1 : public RMobilePhoneStore::TMobilePhoneStoreEntryV1
	/**
	Defines contents of a Own Number store entry
	
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileONEntryV1();
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
	/** Network mode. */
		RMobilePhone::TMobilePhoneNetworkMode iMode;
	/** Mobile call service type.
	
	@see RMobilePhone::TMobileService */
		RMobilePhone::TMobileService iService;
	/** Own number.
	
	@see RMobilePhone::TMobileAddress */
		RMobilePhone::TMobileAddress iNumber;
	/** The name-tag associated with own number (e.g. "Voice Number"). */
		TBuf<KOwnNumberTextSize> iText;
		};

/** A typedef'd packaged TMobileONEntryV1 for passing through a generic API member 
function. */
	typedef TPckg<TMobileONEntryV1> TMobileONEntryV1Pckg;

	IMPORT_C void StoreAllL(TRequestStatus& aReqStatus, CMobilePhoneONList* aONList) const;

protected:
	IMPORT_C void ConstructL();
private:
	RMobileONStore(const RMobileONStore&);
	};

/*********************************************************/
//
// Emergency Number Storage (RMobileENStore)
//
/*********************************************************/



class CMobilePhoneENList;

class RMobileENStore : public RMobilePhoneStore
/**
Provides client access to Emergency Number storage functionality provided by TSY

*/
	{
public:
	IMPORT_C RMobileENStore();
	IMPORT_C TInt Open(RMobilePhone& aPhone);
	IMPORT_C void Close();

	enum 
		{
		KEmergencyNumberSize  = 6,
		KEmergencyAlphaTagSize = 20
		};

/** Enumerated list of Emergency Number entries.

Mode: WCDMA */
	enum TMobilePhoneENServiceCategory
		{
	/** This number provides emergency care by the police department. */
		KServiceCatPolice =0x0001,
	/** This number provides emergency care by the ambulance department. */
		KServiceCatAmbulance =0x0002,
	/** This number provides emergency care by the fire brigade department. */
		KServiceCatFireBrigade =0x0004,
	/** This number provides emergency care by the marine guard department. */
		KServiceCatMarineGuard =0x0008,
	/** This number provides emergency care by the mountain rescue department. */
		KServiceCatMountainRescue =0x0010
		// Reserved types: 0x0020,0x0040,
		};

	

/** Buffer to hold the emergency number in. */
	typedef TBuf<KEmergencyNumberSize> TEmergencyNumber;

	class TMobileENEntryV1 : public RMobilePhoneStore::TMobilePhoneStoreEntryV1
	/**
	Defines contents of a Emergency Number store entry
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TMobileENEntryV1();
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
	/** Indicates whether the emergency number is applicable in all situations (=EFalse) 
	or only when the MS is registered to a specific GSM or CDMA network (=ETrue). */
		TBool iNetworkSpecific;
	/** The network mode. Only applicable if iNetworkSpecific equals ETrue.

	Modes: Common
	
	@see RMobilePhone::TMobilePhoneNetworkMode */
		RMobilePhone::TMobilePhoneNetworkMode iMode;
	/** Country code. Only applicable if iNetworkSpecific equals ETrue.
	
	Modes: Common
	
	@see RMobilePhone::TMobilePhoneNetworkCountryCode */
		RMobilePhone::TMobilePhoneNetworkCountryCode iCountryCode;
	/** Network identity. Only applicable if iNetworkSpecific equals ETrue.
	
	Modes: Common
	
	@see RMobilePhone::TMobilePhoneNetworkIdentity */
		RMobilePhone::TMobilePhoneNetworkIdentity iIdentity;
	/** Emergency number.
	
	Modes: Common
	
	@see TEmergencyNumber */
		TEmergencyNumber             iNumber;
	/** Emergency call number type. It is used to display information about the dialed 
	emergency number (e.g. "Police", "Ambulance", "Fire brigade", etc.)
	
	Modes: WCDMA */
		TBuf<KEmergencyAlphaTagSize> iAlphaId;
		//TInt should map to TMobilePhoneENServiceCategory.
		//It was not changed to maintain source compatibility
		
	/** Emergency call type indicator, which contains information to be sent to the 
	network indicating the type of emergency call.
	
	The field is for future reference only and it is set to RFU (Reserved for
	Future Use) as it is currently not being used, as specified in section 4.2.21 
	TS 31.102 v3.3.0 Rel 99.
	
	Modes: WCDMA */
		TInt	                     iCallType; 
		};

	typedef TPckg<TMobileENEntryV1> TMobileENEntryV1Pckg;

protected:
	IMPORT_C void ConstructL();
private:
	RMobileENStore(const RMobileENStore&);
	};


/*********************************************************/
//
// RMobilePhoneBookStore 
//
/*********************************************************/



class CPhoneBookStorePtrHolder;
class CMobilePhoneBookList;

class RMobilePhoneBookStore : public RMobilePhoneStore
/** Provides member functions for clients to access phone 
books that are stored in ICC memory or in non-volatile memory on the phone 
itself. This sub-session and the member functions within this section must 
be supported if the TSY indicates that it supports the MobilePhonebookStore 
functional unit.
@publishedPartner
@released 
*/
	{
public:
	IMPORT_C RMobilePhoneBookStore();
	IMPORT_C TInt Open(RMobilePhone& aPhone, const TDesC& aStore);
	IMPORT_C TInt Open(RMobilePhone& aPhone, const TDesC& aStore, const TDesC& aMode);
	IMPORT_C void Close();

/** Describes the specialized phone book capabilities.

Modes: Common */
	enum TMobilePhoneBookCaps
		{
	/** New entries can only be written to (or deleted from) the phone book with the 
	PIN2 password or equivalent.
	
	Modes: Common */
		KCapsRestrictedWriteAccess = 0x00000001,
	/** The phonebook supports the second name field.
	
	Modes: WCDMA */
		KCapsSecondNameUsed        = 0x00000002,
	/** The phonebook supports the additional number (with optional Text, TON, NPI 
	and Bearer Capability) field.
	
	Modes: WCDMA */
		KCapsAdditionalNumUsed     = 0x00000004,
	/** The phonebook supports the group name field.
	
	Modes: WCDMA */
		KCapsGroupingUsed		   = 0x00000008,
	/** The phonebook supports entry control and hidden flag fields.

	Modes: WCDMA */
		KCapsEntryControlUsed      = 0x00000010,
	/** The phonebook supports the email address field.
	
	Modes: WCDMA */
		KCapsEmailAddressUsed      = 0x00000020,
	/** The phonebook supports the bearer capability IE field.
	
	Modes: WCDMA */
		KCapsBearerCapUsed		   = 0x00000040,
	/** The phonebook supports retrieval of the phonebook's PBID and Change Counter 
	- and for each entry its UID field.
	
	Modes: WCDMA */
		KCapsSynchronisationUsed   = 0x00000080
		};

/** Described the location of the phone book.

Modes: Common for all */
	enum TMobilePhoneBookLocation
		{
	/** The phonebook's location is unknown. */
		ELocationUnknown,
	/** The phonebook is located in ICC (SIM/R-UIM) memory. */
		ELocationIccMemory,
	/** The phonebook is located in Phone (ME) memory. */
		ELocationPhoneMemory,
	/** The phonebook is located in an external memory. */
		ELocationExternalMemory,
	/** The phonebook entries are dispersed over a combination of memory locations. */
		ELocationCombinedMemory
		};

	enum
		{
		KMaxPBIDSize=15
		};

/** Defines a modifiable buffer descriptor to contain the phone book identity. 
Used in TMobilePhoneBookInfoV1. */
	typedef TBuf8<KMaxPBIDSize> TMobilePhoneBookIdentity;

	

	class TMobilePhoneBookInfoV1 : public RMobilePhoneStore::TMobilePhoneStoreInfoV1
	/**
	Defines information about a Phonebook store.
	@publishedPartner
	@released
	*/
		{
	public:
	/** Trivial constructor. Initializes iMaxNumLength=-1; iMaxTextLength=-1; iLocation=ELocationUnknown; 
	iChangeCounter=0; iExtensionId=KETelMobilePhonebookStoreV1; */
		IMPORT_C TMobilePhoneBookInfoV1();
	public:
	/** The maximum number of characters for the number in a phonebook entry.
	
	Modes: Common */
		TInt    iMaxNumLength;
	/** The maximum number of characters for the text tag in a phonebook entry.
	
	Modes: Common */
		TInt    iMaxTextLength;
	/** The memory location of this phonebook.
	
	Modes: Common
	
	@see TMobilePhoneBookLocation */
		TMobilePhoneBookLocation iLocation;
	/** The contents of the Change Counter file in a USIM phonebook.
	
	Modes: WCDMA */
		TUint16 iChangeCounter;
	/** A unique identity for this type of phonebook.
	
	If this is a SIM or R-UIM phonebook then it will equal the ICCID of the card.
	
	If this is a USIM phonebook then it will equal the PBID of the phonebook. 
	
	Modes: Common
	
	@see TMobilePhoneBookIdentity */
		TMobilePhoneBookIdentity iIdentity; 
		};

/** Packages the TMobilePhoneBookInfoV1 into a TMobilePhoneBookInfoV1Pckg. */
	typedef TPckg<TMobilePhoneBookInfoV1> TMobilePhoneBookInfoV1Pckg;

	
	class TMobilePhoneBookInfoV2 : public TMobilePhoneBookInfoV1
/** Defines information about a Phonebook store for version v2.0 of the API.
@publishedPartner
@released */
		{
	public:
		IMPORT_C TMobilePhoneBookInfoV2();
	public:
	/** The type of the phonebook which relates to its location (ICC/UICC/CDMA).
	
	Mode: Common */
		TName iPhBkMode; 
		};

/** Packages the TMobilePhoneBookInfoV2 into a TMobilePhoneBookInfoV1Pckg. */
	typedef TPckg<TMobilePhoneBookInfoV2> TMobilePhoneBookInfoV2Pckg;


	/** Defines information about a Phonebook store for version v5.0 of the API.

	Extends the v2.0 API to allow additional information about the phonebook
	(Maximum number of second name fields, Maximum number of additional number fields,
	Maximum number of additional group name fields, Maximum number of email address fields)
	to be retieved where available. 

	@publishedPartner
	@released*/
	class TMobilePhoneBookInfoV5 : public TMobilePhoneBookInfoV2
		{
	public:
		IMPORT_C TMobilePhoneBookInfoV5();
	public:
		/** The maximum number of Second Name fields that can be used. */
		TInt iMaxSecondNames;
		
		/** The maximum text length for Second Name fields. */
		TInt iMaxTextLengthSecondName;

		/** The maximum number of additional number fields that can be used. */
		TInt iMaxAdditionalNumbers;
		
		/** The maximum number length for Additional Number fields. */
		TInt iMaxNumLengthAdditionalNumber;
		
		/** The maximum text length for Additonal Number fields. */
		TInt iMaxTextLengthAdditionalNumber;

		/** The maximum number of additional Group Name fields that can be used. */
		TInt iMaxGroupNames;
		
		/** The maximum text length for Group Name fields. */
		TInt iMaxTextLengthGroupName;
		
		/** The maximum number of additional E-Mail address fields that can be used. */
		TInt iMaxEmailAddr;
		
		/** The maximum text length for Email Address fields. */
		TInt iMaxTextLengthEmailAddr;
		};

	/** Packages the TMobilePhoneBookInfoV5 into a TMobilePhoneBookInfoV5Pckg. */
	typedef TPckg<TMobilePhoneBookInfoV5> TMobilePhoneBookInfoV5Pckg;


	// check these fields - not sure all are correct
/** The following Tag IDs are used to encode/decode the phonebook entries to/from 
the TLV format. Each field will have a unique identifier and will be followed 
by the field length. The type of data associated with the field is also specified 
below. 

Some fields do not have any data and just serve as separators within the buffer 
- in these cases the Data Type column is set to not applicable (n/a).

Modes: Common */
	enum TMobilePBFieldTags
		{
	/** Tag id for the beginning of a new phonebook entry.
	
	Data type: n/a.
	
	Modes: Common */
		ETagPBNewEntry		=0xA0,
	/** Tag id for the Unique ID field.
	
	Data type: TUint16

	Modes: WCDMA */
		ETagPBUniqueId		=0xB0,
	/** Tag id for the Index within the ADN record.
	
	Data type: TUint16

	Modes: Common */
		ETagPBAdnIndex		=0xC0,
	/** Tag id for a Text field within the ADN/ANR record.
	
	Data type: TDes16
	
	Modes: Common */
		ETagPBText			=0xC1,
	/** Tag id for a Number field within the ADN/ANR record.
	
	Data type: TDes16
	
	Modes: Common */
		ETagPBNumber		=0xC2,
	/** Tag id for a TON/NPI byte within the ADN/ANR record.
	
	Data type: TUint8
	
	Modes: Common */
		ETagPBTonNpi		=0xC3,
	/** Tag id for a Bearer Capability IE associated to the ADN/ANR record.
	
	Data type: TDes8
	
	Modes: GSM/WCDMA */
		ETagPBBearerCap		=0xC4,
	/** Tag id for the beginning of an Additional Number entry within the phonebook 
	entry.
	
	Data type: n/a
	
	Modes: WCDMA */
		ETagPBAnrStart		=0xC5,
	/** Tag id for the Second Name associated to the ADN record.
	
	Data type: TDes16
	
	Modes: WCDMA */
		ETagPBSecondName	=0xC6,
	/** Tag id for a Group Name associated to the ADN record.
	
	Data type:TDes16
	
	Modes: WCDMA */
		ETagPBGroupName		=0xC7,
	/** Tag id for an Email Address associated to the ADN record.
	
	Data type: TDes16
	
	Modes: WCDMA */
		ETagPBEmailAddress	=0xC8,
	/** Tag id for the Entry Control byte associated to the ADN record.
	
	Data type: TUint8
	
	Modes: WCDMA */
		ETagPBEntryControl	=0xC9,
	/** Tag id for the Hidden Information byte associated to the ADN record.
	
	Data type: TUint8
	
	Modes: WCDMA */
		ETagPBHiddenInfo	=0xCA,
		ETagPBDateTime		=0xCB,
		ETagPBDuration		=0xCC,
		ETagPBCallStatus	=0xCD,
		ETagPBEntryStatus	=0xCE
		};


	// API/TSY internal type
	struct TPBIndexAndNumEntries
/** A structure to hold the phone book index and number of entries. */
		{
	/** Index for the phone book. */
		TInt iIndex;
		TInt iNumSlots;
		};

	IMPORT_C void Read(TRequestStatus& aReqStatus, TInt aIndex, TInt aNumSlots, TDes8& aPBData) const;
	IMPORT_C void Write(TRequestStatus& aReqStatus, const TDesC8& aPBData, TInt& aIndex) const;

protected:
	IMPORT_C void ConstructL();
private:
	RMobilePhoneBookStore(const RMobilePhoneBookStore&);
	};

/*********************************************************/
//
// RMobileConferenceCall
//
/*********************************************************/



class CMobileConferenceCallPtrHolder;

class RMobileConferenceCall : public RTelSubSessionBase
/** Provides access to conference call functionality provided by TSY.

Conference calls allow users to conduct multi-connection voice calls, for example
simultaneous communication to more than one remote party. All participants 
within a conference call can hear and speak to each other. There is a controlling 
party that initiates and manages the conference.

Conference calling is possible in both GSM and CDMA mode and the actions to 
initiate a 3-way conference are the same in either mode. However, GSM conference 
calls offer more functionality and the number of remote parties is limited 
in CDMA mode to 2 and in GSM mode to 5.

The Multimode ETel API uses a new sub-session called RMobileConferenceCall 
as a model for a conference call. The conference call object will be an aggregation 
of all the RMobileCall objects that are part of the conference call. It will 
be possible for clients to manipulate both the conference call as a whole 
and individual calls within a conference call depending on the dynamic capabilities 
of each of these objects.

Please note that RMobileConferenceCall is replacing the RCallGroup abstraction 
used in the GSM API. This was an optimisation decision. The call group abstraction 
placed a large burden on a TSY to create call groups, monitor the calls in 
each group and trigger notification when calls were added or moved, probably 
even when a conference call was not ongoing. Clients will probably only open 
a RMobileConferenceCall when the user has decided to make a conference call 
and by having just one object replace three call groups it will also reduce 
the number of notifications that would trigger in response to conference call 
events. With a view to the future, without call groups there are also no longer 
any restrictions to the allowed types of active calls (simultaneous, separate 
voice and data calls are possible).

The methods within this section must be supported if the TSY supports MobileConferenceCall 
functional unit. 

*/
	{
public:
	IMPORT_C RMobileConferenceCall();
	IMPORT_C TInt Open(RMobilePhone& aPhone);
	IMPORT_C void Close();

/** Conference call capabilities. */
	enum TMobileConferenceCallCaps
		{
	/** Indicates that a conference call can be created. */
		KCapsCreate = 0x00000001,
	/** Indicates that a conference call can be terminated. */
		KCapsHangUp = 0x00000002,
	/** Indicates that a conference call exists and can be swapped to the opposite 
	state (active or hold). */
		KCapsSwap = 0x00000004
		};

	IMPORT_C TInt GetCaps(TUint32& aCaps) const;
	IMPORT_C void NotifyCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const;

	IMPORT_C void CreateConference(TRequestStatus& aReqStatus) const;
	IMPORT_C void AddCall(TRequestStatus& aReqStatus, const TName& aCallName) const;
	IMPORT_C void Swap(TRequestStatus& aReqStatus) const;
	IMPORT_C void HangUp(TRequestStatus& aReqStatus) const;
	IMPORT_C TInt EnumerateCalls(TInt& aCount) const;
	IMPORT_C TInt GetMobileCallInfo(TInt aIndex, TDes8& aCallInfo) const;

/** Conference call status. */
	enum TMobileConferenceStatus
		{
	/** The conference call is in the idle state. */
		EConferenceIdle,
	/** The conference call is in the active, connected state. */
		EConferenceActive,
	/** The conference call is in the held, connected state. */
		EConferenceHold
		};
	
	IMPORT_C TInt GetConferenceStatus(TMobileConferenceStatus& aStatus) const;
	IMPORT_C void NotifyConferenceStatusChange(TRequestStatus& aReqStatus, TMobileConferenceStatus& aStatus) const;

/** Conference events. */
	enum TMobileConferenceEvent
		{
	/** A call has been added to the conference. */
		EConferenceCallAdded,		// Name of the call added returned in aCallName
	/** A call has been removed from the conference. */
		EConferenceCallRemoved,		// Name of the call terminated returned in aCallName
	/** Names of the two calls used to build the conference call returned in aCallName. */
		EConferenceBuilt,		// Names of the two calls used to build the conference call returned in aCallName
	/** No name provided */
		EConferenceTerminated,	// No name provided
	/** No name provided. */
		EConferenceSwapped,		// No name provided
	/** Name of the call being seperated. */
		EConferenceSplit		// Name of the call being seperated
		};

	IMPORT_C void NotifyConferenceEvent(TRequestStatus& aReqStatus, TMobileConferenceEvent& aEvent, TName& aCallName) const;

private:
	CMobileConferenceCallPtrHolder* iMmPtrHolder;
	RMobileConferenceCall(const RMobileConferenceCall&);
protected:
	IMPORT_C void ConstructL();
	IMPORT_C void Destruct();
	};


//
//
// EAP-SIM Authentication functional unit
//
//

class CMobileSmartCardEapPtrHolder;

/**
This sub-session opens under RMobilePhone.

RMobileSmartCardEap provides the client with access to a Smart Card
Application's EAP-capability (if one exists).

(See ETSI TS 102.310 v6.2.0 and RFC3748)

To submit authentication challenges or requests, a client must use
CAsyncSmartCardEapAuthentication in conjunction with this sub-session.

@see CAsyncSmartCardEapAuthentication
@publishedPartner
@released
*/
class RMobileSmartCardEap : public RTelSubSessionBase
	{
	friend class CAsyncSmartCardEapAuthentication;

public:
	/**
	Class encapsulates EAP request packet data for authentication.

	@see CAsyncSmartCardEapAuthentication

	@publishedPartner
	@released
	*/
	class CEapAuthenticateRequestDataV6 : public CBase
		{
	public:
		IMPORT_C static CEapAuthenticateRequestDataV6* NewL();
		IMPORT_C virtual void ExternalizeL(TPtr8& aBuffer);
		IMPORT_C virtual void InternalizeL(const TDesC8& aBuffer);
		IMPORT_C TUint VersionId() const;
		IMPORT_C TPtr8 GetEapReqPacket() const;
		IMPORT_C void SetEapReqPacketL(const TDesC8& aData);
		IMPORT_C ~CEapAuthenticateRequestDataV6();

	private:
		CEapAuthenticateRequestDataV6();
		void ConstructL();

	protected:
		/**
		ETelMM's supported version.
		*/
		TUint iVersionId;

	public:
		/**
		EAP-request packet in a flat buffer.
		*/
		CBufFlat* iEapReqPacket;
		};

	IMPORT_C RMobileSmartCardEap();

	/**
	EAP types are represented by one byte for standard EAP methods, and
	eight bytes for extended methods.  Each byte specified in the
	TEapType buffer should represent one semi-octet.  Therefore, a
	maximum of 16-chars are needed for representing any possible EAP
	type.  (See section 5.2 of ETSI TS 102.310 v6.2.0.)

	@see RMobileSmartCardEap::TEapType

	@publishedPartner
	@released
	*/
	enum { KEapTypeSize = 16 };

	/**
	A typedef'd buffer to hold the EAP type for the subsequent
	authentication that will be carried out on the Smart Card
	Application.

	The value specified must correspond to the pre-allocated type
	identifiers for various EAPs (see
	http://www.iana.org/assignments/eap-numbers).  Some known values
	are given in etelmm.h.

	The type must be specified in hexadecimal format, where each
	character represents one semi-octet.

	@see KETelSmartCardEapTypeMD5
	@see KETelSmartCardEapTypeTLS
	@see KETelSmartCardEapTypeSIM
	@see KETelSmartCardEapTypeAKA
	@see KETelSmartCardEapTypeTTLS
	@see RMobileSmartCardEap::Open()

	@publishedPartner
	@released
	*/
	typedef TBuf8<KEapTypeSize> TEapType;

	IMPORT_C TInt Open(RMobilePhone& aPhone, const RMobilePhone::TAID& aAID, const TEapType& aEapType);
	IMPORT_C void InitialiseEapMethod(TRequestStatus& aReqStatus);
	IMPORT_C void Close();

	/**
	Authentication status of the EAP supporting UICC application (See
	section 7.2 of ETSI TS 102.310 v6.2.0).  One of these values is
	returned on completion of an
	RMobileSmartCardEap::GetAuthenticationStatus() request.

	@see RMobileSmartCardEap::GetAuthenticationStatus()

	@publishedPartner
	@released
	*/
	enum TEapAuthStatus
		{
		/**
		No authentication started
		*/
		ENoAuthStarted,    // 0
		/**
		Authenticating
		*/
		EAuthenticating,   // 1
		/**
		Authentication complete
		*/
		EAuthenticated,    // 2
		/**
		Held (authentication failure)
		*/
		EHeld              // 3
		};

	IMPORT_C void GetAuthenticationStatus(TRequestStatus& aReqStatus, TEapAuthStatus& aAuthStatus);

	/**
	TEapUserIdType should be used to request an identity from EF_PUId
	or EF_Ps, when making an RMobileSmartCardEap::GetUserIdentity()
	request.
	(See sections 7.3 and 7.4, respectively, of ETSI TS	102.310 v6.2.0,
	and RFC2486 - The Network Access Identifier).

	@see RMobileSmartCardEap::GetUserIdentity()
	@see RMobileSmartCardEap::TEapUserIdentityV6

	@publishedPartner
	@released
	*/
	enum TEapUserIdType
		{
		/** Identity is permanent type */
		EPermanentIdentity,   // 0
		/** Identity is pseudonym type */
		EPseudonymIdentity    // 1
		};

	/**
	Maximum permitted size of identity data.
	*/
	enum { KEapIdentityMaxSize = 255 };

	/**
	EAP User Identity data.  Ids are stored in EF_PUId or EF_Ps
	(specified in sections 7.3 and 7.4, respectively, of ETSI TS
	102.310 v6.2.0).

	@see RMobileSmartCardEap::GetUserIdentity()

	@publishedPartner
	@released
	*/
	class TEapUserIdentityV6 : public RMobilePhone::TMultimodeType
		{
	public:
		IMPORT_C TEapUserIdentityV6();
	public:
		/**
		EAP-IDENTITY Data.
		*/
		TBuf8<KEapIdentityMaxSize> iEapId;
		};

	/**
	A typedef'd packaged TEapUserIdentityV6 for passing through a
	generic API method.

	@publishedPartner
	@released
	*/
	typedef TPckg<TEapUserIdentityV6> TEapUserIdentityV6Pckg;

	IMPORT_C void GetUserIdentity(TRequestStatus& aReqStatus,
	                              TEapUserIdType aRequestedIdType,
	                              TDes8& aUserId);

	/**
	TEapKeyV6 should be used to request one of (currently) two keys
	available on the EF_EAPKEYS of the UICC application (see section
	7.1 of ETSI TS 102.310 v6.2.0).  This enumeration type should be
	used in RMobileSmartCardEap::GetEapKey() to specify the key to be
	retrieved.

	@see RMobileSmartCardEap::TEapKeyV6
	@see RMobileSmartCardEap::GetEapKey()

	@publishedPartner
	@released
	*/
	enum TEapKeyTag
		{
		/**
		Used to request Master Session Key.
		*/
		EEapKeyMSK =  0x80,
		/**
		Used to request Extended Master Session Key.
		*/
		EEapKeyEMSK = 0x81
		};

	/**
	Maximum size of an EAP Key stored on the DF_EAP's EF_EAPKEYS.
	255 as each key has a length that can be specified by exactly one
	byte (see section 7.1 of ETSI TS 102.310 v6.2.0).

	@publishedPartner
	@released
	*/
	enum { KEapKeyMaxSize = 255 };

	/** 
	EAP authentication key data.

	@see RMobileSmartCardEap::GetEapKey()

	@publishedPartner
	@released
	*/
	class TEapKeyV6 : public RMobilePhone::TMultimodeType
		{
	public:
		IMPORT_C TEapKeyV6();
	public:
		/**
		EAP-KEY Data.
		*/
		TBuf8<KEapKeyMaxSize> iEapKey;
		};

	/**
	A typedef'd packaged TEapKeyV6 for passing through a generic API
	method.

	@publishedPartner
	@released
	*/
	typedef TPckg<TEapKeyV6> TEapKeyV6Pckg;

	IMPORT_C void GetEapKey(TRequestStatus& aReqStatus, const TEapKeyTag aRequestedKey, TDes8& aKey);

	/**
	Status of the DF_EAP this subsession refers to.  The status is
	basically an indication of whether the DF is in use by another sub-
	session client instance.

	NotifyEapMethodAccessStatusChange() will give a notification when
	the status changes.

	Status will change when the first client calls
	InitialiseEapMethod() on this sub-session.  When the same client
	calls ReleaseEapMethod() (or Close()), the status will change
	again.  This allows mutually exclusive access to the DF_EAP.  All
	other RMobileSmartCardEap hanles will get an error if they attempt
	to make requests that access the same DF_EAP.

	@see RMobileSmartCardEap::NotifyEapMethodAccessStatusChange()
	@see RMobileSmartCardEap::ReleaseEapMethod()
	@see RMobileSmartCardEap::InitialiseEapMethod()

	@publishedPartner
	@released
	*/
	enum TEapMethodAccessStatus
		{
		/**
		AID/DF_EAP has been reset, and the DF_EAP has not been
		initialised by any other instance of RMobileSmartCardEap.
		*/
		EEapMethodAvailable,
		/**
		Another instance of RMobileSmartCardEap has initialised first
		and taken ownership of the DF_EAP.  The DF_EAP is currently
		active and EAP requests can be made.
		*/
		EEapMethodInUseApplicationActive,
		/**
		This instance of RMobileSmartCardEap still owns the lock on the
		DF_EAP, but the application has been deactivated elsewhere.
		The client should re-initialise before making further EAP
		requests.

		@see RMobileSmartCardEap::InitialiseEapMethod()
		*/
		EEapMethodInUseApplicationInactive,
		/**
		Lock on the DF_EAP has been released, but another DF_EAP method
		is in use under the same AID, thus, cannot reset/initialise
		this subsessions EAP method.  Client can only post a
		notification and wait till status changes to
		EEapMethodAvailable.

		@see RMobileSmartCardEap::NotifyEapMethodAccessStatusChange()
		*/
		EEapMethodUnableToInitialise
		};

	IMPORT_C TInt ReleaseEapMethod();
	IMPORT_C TInt GetEapMethodAccessStatus(TEapMethodAccessStatus& aEapMethodStatus);
	IMPORT_C void NotifyEapMethodAccessStatusChange(TRequestStatus& aReqStatus, TEapMethodAccessStatus& aEapMethodStatus);
	IMPORT_C TBool IsEapMethodOwner() const;

protected:
	IMPORT_C void ConstructL();
	IMPORT_C void Destruct();

private:
	RMobileSmartCardEap(const RMobileSmartCardEap&);

	TChar SeptChar(TInt aDigit);
	void ConvertBinToText(const TDesC8& aBinData, TDes& aText);

private:
	/**
	Pointer Holder for the RMobileSmartCardEap sub-session requests.
	*/
	CMobileSmartCardEapPtrHolder* iMmPtrHolder;
	/**
	True if this object is the first to request	InitialiseEapMethod()
	on its <AID,EAPType> when the status is EEapMethodAvailable.
	I.e. True only for the instance of RMobileSmartCardEap that
	successfully passes the Wait() on iSemaphore.
	*/
	TBool iOwnsEapMethodLock;
	/**
	Semaphore is actually owned by TSY, and used by all instances of
	RMobileSmartCardEap	to stop multiple access to the same EAP method
	on any one application.
	*/
	RSemaphore iSemaphore;

	};	// RMobileSmartCardEap

//
//
// LCS-MO-LR AND LCS-MT-LR
//
//
class CMobileLocationServicesPtrHolder;

/**
This sub-session opens under RMobilePhone.

A new RMobileLocationServices class is used to add the ETelMM Support for Mobile Terminating Location Request,
Mobile Originating Location Requests, A-Gps assistance data for the LBS Framework.

RMobileLocationServices is derived from RTelSubSessionBase.RMobileLocationServices lists the 
APIs,which provided the support for Mtlr, Molr, A-Gps assistance data requests.
*/
class RMobileLocationServices : public RTelSubSessionBase
	{
public:
	
	IMPORT_C RMobileLocationServices();
    IMPORT_C TInt Open(RMobilePhone& aPhone);
    IMPORT_C void Close();
 	/** 
 	Maximum length of the array used for TLcsRequestorIdString.
 	*/
	enum { KLcsMaxRequestorIdStringSize = 256 };
	/** 
	Maximum length of the array used for TLcsClientBufferName.
 	*/
	enum { KLcsMaxClientNameBufferSize = 256 };
	/** 
	Maximum length of the array used for TLcsClientExternalID.
	*/
	enum { KLcsMaxClientExternalIdSize = 64 };
	/** 
	Maximum length of the array used for TLcsLocationEstimate.
	*/
	enum { KLcsMaxLocationEstimateSize = 64 };
	/** 
	Maximum length of the array used for TLcsVelocityEstimate.
	*/
	enum { KLcsMaxVelocityEstimateSize = 64 };
	/** 
	Maximum length of the array used for TLcsGpsAssistanceData.
	*/
	enum { KLcsMaxGpsAssistanceDataSize = 64 };
	/** 
	A typedef'd buffer which holds the External ClientName Buffer.
	@see KLcsMaxClientNameBufferSize
	*/
	typedef TBuf8<KLcsMaxClientNameBufferSize> TLcsClientNameBuffer;
	/** 
	A typedef'd buffer which holds the RequestorId String.
	@see KLcsMaxRequestorIdStringSize
	*/
	typedef TBuf8<KLcsMaxRequestorIdStringSize> TLcsRequestorIdString;
	/**
	A typedef'd buffer which holds the Gps Assistance Data.
	@see KLcsMaxGpsAssistanceDataSize
	*/
	typedef TBuf8<KLcsMaxGpsAssistanceDataSize> TGpsAssistanceData;
	/**
	A typedef'd buffer which holds the External ClientId.
	@see KLcsMaxClientExternalIdSize
	*/
	typedef TBuf8<KLcsMaxClientExternalIdSize> TLcsClientExternalID;
	/**
	A typedef'd TUint8 which holds the Data Coding Scheme information.
	*/
	typedef TUint8 TLcsDataCodingScheme;
	/**
	A typedef'd buffer which holds the Location Estimate.
	@see KLcsMaxLocationEstimateSize
 	*/
	typedef TBuf8<KLcsMaxLocationEstimateSize> TLcsLocationEstimate;
	/**
	A typedef'd buffer which holds the Velocity Estimate.
	@see KLcsMaxVelocityEstimateSize
	*/
	typedef TBuf8<KLcsMaxVelocityEstimateSize> TLcsVelocityEstimate;

	typedef TUint8 TTransactionIdentifier;
	typedef TUint8 TGPSI;
 	 /**
     Note:Refer to 3GPP TS 23.271(Functional stage 2 description of Location Services (LCS)),
     3GPP TS 24.030(Location Services (LCS); Supplementary service operations - Stage 3) and
     3GPP TS 24.080(Mobile radio interface layer 3 supplementary services specification; Formats and coding)
     for further details of MTLR and MOLR member data.
     */
 /*********************************************************************/ 
 //                  MTLR                                             //
 /*********************************************************************/   
    /**
    TNotificationType lists the various types of Notification
    that can be expected from any MTLR-Location Notification coming from the Network,
    These types depend on the Subscription Profile of the UE. 
    
    One of the TNotificationType is expected to be retrieved from the Network,
    when a UE places a RMobileLocationServices::NotifyMtlr() request on the TSY.
    @publishedPartner
    */
    enum TNotificationType
		{
		/**
		Notification Type Unknown
		*/
		ENotificationTypeUnknown,
		/**
		The Location Request is accepted by default
		*/
		ENotifyLocationAllowed,
		/**
        This enum is used to indicate to the client, that the user should be notified of the incoming MTLR request, 
        and that the user is expected to respond by granting/denying the request. By defualt, if the user 
        does not provide a response, the request is granted
		*/
		ENotifyAndVerify_LocationAllowedIfNoResponse,
		/**
        This enum is used to indicate to the client, that the user should be notified of the incoming MTLR request, 
        and that the user is expected to respond by granting/denying the request. By defualt, if the 
        user does not provide a response, the request is denied.
		*/
		ENotifyAndVerify_LocationNotAllowedIfNoResponse,
        /**
        The network sends the information to the UE that the external client has privacy overide capability,
        This indicator is received when an authenticated or emergency client had sent the request.
		*/     
        ELocationPrivacyOverride,  
        /**
        This enum is used to notify the UE that an incoming MTLR request was denied by the network for any of various 
        reasons.
		*/
		ELocationNotAllowed                                      
        };

    
    /**
    TLocationEstimateType lists the various types of LocationEstimate
    that can be expected from any MTLR-Location Notification request coming from the Network,
    These types depend on the Subscription Profile of the UE.
    
    One of the TLocationEstimateType is expected to be retrieved from the Network,
    when a UE places a RMobileLocationServices::NotifyMtlr() request on the TSY.
    @publishedPartner
    */
    enum TLocationEstimateType
		{
		/**
		Location Estimate Type Unknown.
		*/
		ELocationEstimateTypeUnknown,
		/**
		The External Client has requested for the Current Location of
		the UE.
		*/
		ECurrentLocation,
		/**
		The External Client has requested for the Current Location of
		the UE.Incase,if the request is accepted,But still it fails to
		generate the Location report because of Measurement Control Failure.
		The Client receives the Last Known Location.
		*/
		ECurrentOrLastKnownLocation,
		/**
		The External Client has requested for the initial location of the UE. The current 
		location of the UE may or may not be the initial location.
		*/
		EInitialLocation
		};	
    
    /**
    TLcsClientNameV1 holds the details of the name of the External client, 
    who is making the request via Network.
    TLcsClientNameV1 is used by RMobileLocationServices::NotifyMtlr() request.
    @see TLcsDataCodingScheme
    @see TLcsClientNameBuffer
    @publishedPartner
    */
    class TLcsClientNameV1
    	{
    public:
    	/**
    	TLcsDataCodingScheme holds the Data Coding Scheme information which is used
    	to extract the ExternalClient name.
    	*/
    	TLcsDataCodingScheme iLcsDataCodingScheme;
    	/**
    	TLcsClientNameBuffer holds the Client name 		
    	*/
    	TLcsClientNameBuffer       iLcsClientNameBuffer;	
    	};
    
    /**
    TLcsRequestorIDV1 holds the details of the RequestorId i.e the details of the 
    Network,Which is carrying the request of External Client to UE
    TLcsRequestorIDV1 is used by RMobileLocationServices::NotifyMtlr() request.
    @see TLcsDataCodingScheme
    @see TLcsRequestorIdString 
    @publishedPartner
    */
    class TLcsRequestorIDV1
    	{
    public:
        /**
    	TLcsDataCodingScheme holds the Data Coding Scheme information which is used
    	to extract the RequestorId String.
    	*/
    	TLcsDataCodingScheme        iLcsDataCodingScheme;
    	/**
    	TLcsRequestorIdString holds the RequestorId String 		
    	*/
    	TLcsRequestorIdString       iLcsRequestorIdString;	
    	};
    
    /**
    TLocationExternalInfoV1 holds the details of the External client and the Requestor
    TLocationExternalInfoV1 is used by RMobileLocationServices::NotifyMtlr() request
    @see TLcsClientExternalID
    @see TLcsClientNameV1
    @see TLcsRequestorIDV1
    @publishedPartner
    */
    class TLocationExternalInfoV1
		{
	public:
		/**
		TLcsClientExternalID gives the id of the External client.
		*/
		TLcsClientExternalID     iLcsClientID;
		/**
		TLcsClientNameV1 holds the name of the External Client.
		*/
		TLcsClientNameV1           iLcsClientName;
		/**
		TLcsRequestorIDV1 gives the details of requestor id
		*/
		TLcsRequestorIDV1          iLcsRequestorID;
		};
	
	/**
	TNotifyMtlrV7 holds the details of the MTLR request made by the External client.
	TNotifyMtlrV7 is used by RMobileLocationServices::NotifyMtlr() request
	@see TNotificationType
	@see TLocationEstimateType
	@see TLocationExternalInfoV1
	@publishedPartner
	*/
	class TNotifyMtlrV7: public RMobilePhone::TMultimodeType 
		{
	public:
		IMPORT_C TNotifyMtlrV7();
	public:
		/**
		TNotificationType holds the type of notification request being made
		*/
		TNotificationType          iNotificationType;
		/**
		TLocationEstimateType holds the type of LocationEstimate is requested 
		*/
		TLocationEstimateType      iMtlrLocationEstimateType;
		/**
		TLocationExternalInfoV7  holds the details of the External client. 
		*/
		TLocationExternalInfoV1      iLocationInfo;
        };
	
   	/**
	A typedef'd packaged TNotifyMtlrV7 for passing through a
	generic API method.

	@publishedPartner
	*/
	typedef TPckg<TNotifyMtlrV7> TNotifyMtlrV7Pckg;
	
 //MTLR-RESPONSE
 	/**
 	TMtlrResponse is used by the OverLoaded RMobileLocationServices::SendMtlrResponse() API
 	It holds the type of response being sent back for the MTLR request made.
 	@publishedPartner
 	*/
	
	enum TMtlrResponse
		{
		/**
		Mtlr Response Unknown.
		*/
		EMtlrResponseUnknown,
		/**
		The UE has denied the Location Request  of the External client.
		@see RMobileLocationServices::SendMtlrResponse()
		*/
		EPermissionDenied ,
		/**
		The UE has granted  the Location Request  of the External client.
		@see RMobileLocationServices::SendMtlrResponse()
		*/
		EPermissionGranted
		};
	
	/**
 	TMtlrError is used by the Overloaded RMobileLocationServices::SendMtlrResponse() API
 	It holds the type of Error response being sent back for the MTLR request made.
 	@publishedPartner
 	*/
 	enum TMtlrError
		{
		/**
		Mtlr Error Unknown.
		*/
		EMtlrErrorUnknown,
		/**
		This response is sent back to the network when System has failed to 
		do the privacy check due to some Miscellaneous reasons
		*/
		ESystemFailure ,
		/**
		The response is sent when the privacy verification has failed because of Unexpected data 
		values retrieved from the Location Notification request generated by the Network.
		This generates an Error-Prone response from UE
		*/
		EUnexpectedDataValue	
		};
	
	/**
 	TMtlrReject is used by the Overloaded RMobileLocationServices::SendMtlrResponse() API
 	It holds the details of the Response being rejected due to one of the problems reported to the UE
 	for the MTLR request being made.
 	@publishedPartner
 	*/
 	enum TMtlrReject
		{
		/**
		Mtlr Reject Unknown.
		*/
		EMtlrRejectUnknown,
		/**
		This indication is given under a scenario where the UE wants to reject a request,
		It is done by invoking Duplicate ID's.
		*/
		EDuplicateInvokeID,
		/**
		This indication is given under a scenario where the UE wants to reject a request, 
		It is done by saying the operation is not recognized
		*/
		EUnrecognizedOperation,
		/**
		This invokes a problem indicating Resource Limitation.
		*/
		EResourceLimitation,
		/**
		This Invokes a problem initiating release.
		*/
		EInitiatingRelease,
		/**
		This invokes a problem indicating that  the ID linked with the Client information 
		is unrecognized
		*/
		EUnrecognizedLinkedID,
		/**
		This invokes a problem of Linked Response Unexpected
		*/
		ELinkedResponseUnexpected,
		/**
		This invokes a problem of Unexpected Linked Operation
		*/
		EUnexpectedLinkedOperation,
		/**
		This invokes a problem of Mistyped Parameter.
		*/
		EMistypedParameter	
		};
	
   /**************************************************************/
   //                  MOLR                                      //
   /**************************************************************/
 
 	/**
 	TMolrType is used by the SendMolr() API.
 	This gives an indication of type of Molr request the UE wants to invoke
 	@publishedPartner
 	*/
 	enum TMolrType
		{
		/**
		MolrType UnKnown
		*/
		EMolrTypeUnknown,
		/**
		UE is requesting the Network for its Location Estimate  
		*/
		ELocationEstimate,
		/**
		UE is requesting the Network for  GPS Assistance data
		*/
		EAssistanceData,
		/**
		UE is requesting the Network for  DecipheringKeys
		*/
		EDecipheringKeys
		};

 	/**
 	TResponseTime is used by SendMolr() API
 	This enum holds the various types of delay that an UE is expecting.
 	@publishedPartner
 	*/
 	enum TResponseTime
  		{
  		/**
  		Response Time Unknown.
		*/
  		EResponseTimeunknown,
  		/**
  		This enum value indicates that the UE expects a Less Time Delay
  		from Network in responding to its request
  		*/
  		ELowDelay,
  		/**
  		This enum value indicates that the UE expects a Long Time Delay
  		from Network in responding to its request and is tolerant to it.
  		*/
  		EDelayTolerant		
  		};
 	
 	/**
 	TLocationMethod is used by SendMolr() API
 	This is an enum which lists out all different types of Location methods that an UE can request
 	@publishedPartner
 	*/
 	enum TLocationMethod
  		{
  		/**
  		Location Method Unknown
		*/
  		ELocationMethodunknown,
  		/**
  		This indication is given under a scenario where the UE wants a MS (Mobile Station) Based Location Fix.
  		This can be termed as UE Assisted Location Fix(EOTD-Enhanced observed time difference).
  		*/
  		EMSBasedEOTD,
  		/**
  		This indication is given under a scenario where the UE wants a MS (Mobile Station) Assisted Location Fix.
  		This can be termed out as UE based location fix.
  		*/
  		EMSAssistedEOTD,
  		/**
  		This indication is given under a scenario where the UE wants a MS (Mobile Station) Assisted GPS data.
  		*/
  		EMSAssistedGPS,
  		/**
  		This indication is given under a scenario where the UE wants a MS (Mobile Station) based location fix 
  		(OTDOA-Observed time difference of arrival).
  		*/
  		EMSBasedOTDOA
  		}; 
   
    /**
    TQosV1 is used by SendMolr() API
    This is a class with details of accuracy of Position information, Response Time, 
    expected to be received from the network.
    @see TResponseTime
    @publishedPartner
    */
 	class TQosV1
   		{
   	public:
    	/**
    	This gives an information of the amount of Horizontal accuracy that the UE is expecting.
    	*/
    	TUint8 iHorizontalAccuracy;
    	/**
    	This gives an information of the amount of Vertical accuracy that the UE is expecting. 
    	*/
    	TUint8 iVerticalAccuracy;
    	/**
    	This enum indicates the Response Time expected by the UE to receive a response for its MOlr request.
    	*/
    	TResponseTime iResponseTime;
    	};
    
    /**
    TMolrErrors is used by SendMolr() API
    This is an enum which lists out all the possible errors that can be 
    reported to UE by an External Client or the Network.
    @publishedPartner
    */
   	enum TMolrErrors
    	{
    	/**
    	Molr Error Unknown
		*/
    	EMolrErrorUnknown,
    	/**
    	This indication is given to the UE when the Location Report is unable to reach the 
    	External Client due to some failure in the Network Connections.
    	*/
        EMolrSystemFailure, 
        /**
        This indication is given back to the UE by the Network to the UE or by the 
        External Client to the UE,where the UE generated MOLR has some unexpected values. 
        */
        EMolrUnexpectedDataValue,  
        /**
        This indication is given back to the UE by the Network to the UE or by the 
        External Client to the UE,where the UE generated MOLR had a data discrepancy
        */
        EMolrDataMissing, 
        /**
        This indication is given by the Network to the UE, 
        when the UE is not given a facility to make a MOLR request
        */
        EMolrFacilityNotSupported, 
        /**
        This indication is given by the Network to the UE, 
        when the subscription profile of the UE doesn't match with the requests made by it.
        */
        EMolrSS_SubscriptionViolation, 
        /**
        This indication is given by the Network to the UE, 
        when the UE or the Network failed to generate the Location estimate
        */ 
        EMolrPositionMethodFailure 
        };

	/**
	TStartMolrV7 is used by SendMolr() API
	This class lists the details send by UE to Network when a Molr request is being made.
	@see TMolrType
	@see TQosV1
	@see TLocationEstimateType
	@see TLcsClientExternalID
	@see TGpsAssistanceData
	@see TLocationMethod
	@publishedPartner
	*/
	class TStartMolrV7: public RMobilePhone::TMultimodeType
		{
	public:
		IMPORT_C TStartMolrV7();
	public:
		/**
		TMolrType holds the type of Molr request being made
		*/
		TMolrType             iMolrType;
		/**
		TQosV1 lists out the Quality of Service expected by UE
		*/
		TQosV1                iQos;
		/**
		Type of Location Estimate requested
		*/
		TLocationEstimateType iMolrLocationEstimateType;
		/**
		Id of the External client
		*/
		TLcsClientExternalID  iLcsClientExternalID;
		/**
		Type of GpsAssistanceData
		*/
		TGpsAssistanceData    iGpsAssistanceData;
		/**
		Type of Location method requested
		*/
		TLocationMethod       iLocationMethod ;
		/**
		Type of Service requested
		*/
		TUint8                 iLcsServiceTypeID;
		};
	/**
	A typedef'd packaged TStartMolrV7 for passing through a
	generic API method.

	@publishedPartner
	*/
	typedef TPckg<TStartMolrV7> TStartMolrV7Pckg;
	
	/**
	TMolrReturnResultV7 is used by SendMolr() API
	This class lists all the Molr return results that can be expected for the Molr request
	being made.
	@see TLcsLocationEstimate 
	@see TLcsVelocityEstimate
	@see TMolrErrors
	@publishedPartner
	*/
    class TMolrReturnResultV7: public RMobilePhone::TMultimodeType
    	{
    public:
		IMPORT_C TMolrReturnResultV7();
    public:
    	/**
    	This gives the Location Estimate of the UE
    	*/
    	TLcsLocationEstimate    iLocationEstimate;
    	/**
    	This gives the Velocity Estimate of the UE
    	*/
    	TLcsVelocityEstimate	iVelocityEstimate;
    	/**
    	This gives the indication of the type of error occured
    	*/
    	TMolrErrors             iMolrErrors;
    	};

	/**
	A typedef'd packaged TMolrReturnResultV7 for passing through a
	generic API method.

	@publishedPartner
	*/
	typedef TPckg<TMolrReturnResultV7> TMolrReturnResultV7Pckg;
	
	//**************************************//
	// Measurement Control Data structures	//
	//**************************************//

	enum 
		{
		/**Maximum number of monitored cells RACH*/
		KMaxMonitoredCells = 8,
		/**Maximum number of satellites*/
		KMaxSat = 16,
		/**GlobalHealth size. 364 bits =46 bytes (Rounded) */
		KGlobalHealthSize	= 46
		};

	/**This enum contains the possible types of measurement commands.*/
	enum TMeasurementCmd
		{
		/**Measurement Command Unknown*/
		EMeasCommandUnknown,
		/**Setup a new measurement*/
		ESetup,
		/**Modify a previously defined measurement, e.g. to change the reporting criteria*/
		EModify,
		/**Indicates to stop a measurement and clear all information in the UE that are related to that measurement */
		ERelease,
		};

	/**This enum contains the possible positioning method types*/
	enum TUePosMethodType
		{
		/**Positioning Method Type unknown*/
		EPosMethodTypeUnknown,
		/**UE Assisted : The network computes the location of the UE.*/
		EUeAssisted,
		/**UE Based : The UE computes the location of itself*/
		EUeBased,
		};

	/**This enum contains the possible positioning methods*/
	enum TPosMethod
		{
		/**Positioning Method unknown*/
		EPosMethodUnknown,
		/**OTDOA may be selected by the SRNC as the method to be used on receipt of an LCS positioning request from the CN */
		EOtdoa,
		/**These methods make use of UEs, which are equipped with radio receivers capable of receiving GPS signals*/
		EGps,
		/**Use either OTDOA or GPS positioning method.*/
		EOtdoaOrGps,
		/**
		In the cell ID based (i.e. cell coverage) method, the position of an UE is 
		estimated with the knowledge of its serving Node B. The information about the serving 
		Node B and cell may be obtained by paging,locating area update, cell update, URA update, or routing area update.
		*/
		ECellID
		};

	/**
	This class contains the information which is part of the Navigation Model.
	This can be requested by the UE as part of additional request.
	*/
	class TSatDataV1
		{
	public:
		/**Satellite identity*/
		TUint8		iSatID;
		/**Issue of Data Ephemeris for SatID*/
		TUint8		iIode;
		};
	
	/**
	This class contains the additional information that can be requested by the UE.
	*/
	class TNavModelAddlDataReqV1	
		{
	public:
		/**GPS time in weeks*/
		TUint16		iGpsWeek;
		/**GPS time of ephemeris in hours of the latest ephemeris set contained by the UE.*/
		TUint8		iGpsToe;
		/**Ephemeris age tolerance of the UE to UTRAN in hours.*/
		TUint8		iTToeLimit;
		/**Satellite Identities
		@see TSatDataV1*/
		TSatDataV1	iSatData[KMaxSat];
		};

	/**This class contains the indicators to request for additional assistance data from the network.*/
	class TGpsAddlAssistDataReqV1
		{
	public:
		/**ETrue indicates Almanac Data is requested
		@see	TAlmanacV1*/
		TBool	iAlmanacRequest;
		/**ETrue indicates UTC model Data is requested
		@see	TUtcModelV1*/
		TBool	iUtcModelRequest;
		/**ETrue indicates Ionospheric Model Data is requested
		@see	TUtcModelV1*/
		TBool	iIonosphericModelRequest;
		/**ETrue indicates Navigation model Data is requested
		@see	TIonosphericModelV1*/
		TBool	iNavigationModelRequest;
		/**ETrue indicates DGPS corrections Data is requested
		@see	TNavigationModelSatInfoV1*/
		TBool	iDgpsCorrectionsRequest;
		/**ETrue indicates Reference location Data is requested
		@see	TDgpsCorrectionsV1*/
		TBool	iReferenceLocationRequest;
		/**ETrue indicates Reference time Data is requested
		@see	TReferenceLocationV1*/
		TBool	iReferenceTimeRequest;
		/**ETrue indicates Acquisition assistance Data is requested
		@see	TReferenceTimeV1*/
		TBool	iAcquisitionAssistanceReq;
		/**ETrue indicates Realtime integrity Data is requested
		@see	TAcquisitionAssistanceV1*/
		TBool	iRealTimeIntegrityRequest;
		/**This member is included only if Navigagion Model Data is requested (iNavigationModelRequest is ETrue).
		@see	TNavModelAddlDataReqV1*/
		TNavModelAddlDataReqV1	iNavModelAddlDataReq;
		};

	/**This class contains the UE positioning reporting quantity*/
	class TUePosReportingQuantityV1
		{
	public:
		/**Holds the positioning method type*/
		TUePosMethodType	 iUePosMethodType;
		/**Holds the positioning method*/
		TPosMethod 	iPosMethod;
		/**Holds the horizontal accuracy*/
		TInt8	iHorzAccuracy;
		/**Holds the vertical accuracy*/
		TInt8	iVertAccuracy;
		/**If true the SRNC wants the UE to report the SFN-GPS timing of the reference cell.*/ 
		TBool 	iGpsTimingOfCellWanted;
		/**ETrue indicates that the UE is requested to send the IE "Additional assistance Data Request" 
		when the IE "UE positioning Error" is present in the UE positioning measured results.
		EFalse indicates that the UE shall use the assistance data available.
		*/
		TBool	iAddlAssistanceDataReq;
		};

	/**This enum contains the states in which measurements are valid*/
	enum TMeasurementValidity
		{
		/**Measurement Validity Unknown*/
		EMeasValidityUnknown,
		/**Indicates measurement is valid in CellDCH state*/
		ECellDCH,
		/**Indicates measurement is valid except in CellDCH state*/
		EAllButCellDCH,
		/**Indicates measurement is valid in all state*/
		EAllStates
		};

	/**This enum list the SfnTow uncertainity*/
	enum TSfnTowUncertainity
		{
		/**Sfn Tow Uncertainity unknown*/
		ESfnTowUncertainityUnknown,
		/**SfnTowUncertainity < 10*/
		ELessThan10,
		/**SfnTowUncertainity > 10*/
		EMoreThan10
		};
	
	/**This enum lists the different GPS drift rates*/
	enum TUtranGpsDriftRate
		{
		/**GPS Drift rate unknown*/
		EDriftRateUnknown,
		/**Drift = 0*/
		EUtranGpsDrift0,
		/**Drift = 1*/
		EUtranGpsDrift1,
		/**Drift = 2*/
		EUtranGpsDrift2,
		/**Drift = 5*/
		EUtranGpsDrift5,
		/**Drift = 10*/
		EUtranGpsDrift10,
		/**Drift = 15*/
		EUtranGpsDrift15,
		/**Drift = 25*/
		EUtranGpsDrift25,
		/**Drift = 50*/
		EUtranGpsDrift50,
		/**Drift = -1*/
		EUtranGpsDriftMinus1,
		/**Drift = -2*/
		EUtranGpsDriftMinus2,
		/**Drift = -5*/
		EUtranGpsDriftMinus5,
		/**Drift = -10*/
		EUtranGpsDriftMinus10,
		/**Drift = -15*/
		EUtranGpsDriftMinus15,
		/**Drift = -25*/
		EUtranGpsDriftMinus25,
		/**Drift = -50*/
		EUtranGpsDriftMinus50,
		};

	/**This enum lists the different Udre values*/
	enum TDiffCorrectionsStatus
		{
		/**Diff Corrections Status unknown*/
		ECorrectionStatusUnknown,
		/**UDRE 1.0 */
		EUdre1Point0,
		/**UDRE 0.75 */
		EUdre0Point75,
		/**UDRE 0.5 */
		EUdre0Point5,
		/**UDRE 0.3 */
		EUdre0Point3,
		/**UDRE 0.2 */
		EUdre0Point2,
		/**UDRE 0.1 */
		EUdre0Point1,
		/**UDRE No Data */
		ENoData,
		/**UDRE Invalid Data */
		EInvalidData
		};
	
	/**This class reserves members for future use*/
	class TSubFrame1ReservedV1
		{
	public:
		/**Reserve member 1*/
		TUint	iReserved1;
		/**Reserve member 2*/
		TUint	iReserved2;
		/**Reserve member 3*/
		TUint	iReserved3;
		/**Reserve member 4*/
		TUint	iReserved4;
		};

	/**This class contains information for GPS ephemeris and clock correction.*/
	class TEphemerisParameterV1
		{
	public:
		/**Code(s) on L2 Channel */
		TUint8	iCodeOnL2;
		/**User Range Accuracy */
		TUint8	iUraIndex;
		/**Health of Satellite*/
		TUint8	iSatHealth;
		/**Issue of Data, Clock */
		TUint16	iIodc;
		/**L2 channel flag*/
		TUint8	iL2Flag;
		TSubFrame1ReservedV1	iSubFrame1Reserved;
		/**Estimated group delay differential */
		TUint8	iTGd;
		/**Apparent clock correction*/
		TUint16	iTOc;
		/**Apparent clock correction*/
		TUint8	iAf2;
		/**Apparent clock correction*/
		TUint16	iAf1;
		/**Apparent clock correction*/
		TUint	iAf0;
		/**Amplitude of the Sine Harmonic Correction Term to the Orbit Radius (meters)*/
		TUint16	iCRs;
		/**Mean Motion Difference From Computed Value (semi-circles/sec)*/
		TUint16	iDeltaN;
		/**Mean Anomaly at Reference Time (semi-circles)*/
		TUint	iM0;
		/**Amplitude of the Cosine Harmonic Correction Term To The Argument Of Latitude (radians)*/
		TUint16	iCUc;
		/**Eccentricity*/
		TUint	iE;
		/**Amplitude of the Sine Harmonic Correction Term To The Argument Of Latitude (radians)*/
		TUint16	iCUs;
		/**Semi-Major Axis in squareroot(meters) */
		TUint	iASqrt;
		/**Reference Time Ephemeris */
		TUint16	iTOe;
		/**
		Fit interval flag
		@see ICD-GPS-200
		*/
		TUint8	iFitInterval;
		/**Age Of Data Offset */
		TUint8	iAodo;
		/**Amplitude of the Cosine Harmonic Correction Term To The Angle Of Inclination (radians)*/
		TUint16	iCIc;
		/**Longitude of Ascending Node of Orbit Plane at Weekly Epoch (semi-circles) */
		TUint	iOmega0;
		/**Amplitude of the Sine Harmonic Correction Term To The Angle Of Inclination (radians) */
		TUint16	iCIs;
		/**Inclination Angle at Reference Time (semi-circles) */
		TUint	i0;
		/**Amplitude of the Cosine Harmonic Correction Term to the Orbit Radius (meters) */
		TUint16	iCRc;
		/**Argument of Perigee (semi-circles) */
		TUint	iOmega;
		/**Longitude of Ascending Node of Orbit Plane at Weekly Epoch (semi-circles/sec) */
		TUint	iOmegaDot;
		/**Rate of Inclination Angle (semi-circles/sec) */
		TUint16	iDot;
		};

	/**This enum specifies the possible status of a satellite*/
	enum TSatelliteStatus
		{
		/**Satellite Status unknown*/
		ESatStatusUnknown,
		/**New satellite, new Navigation Model*/
		ENsNnU,
		/**Existing satellite, same Navigation Model*/
		EEsSn,
		/**Existing satellite, new Navigation Model*/
		EEsNnU,
		/**Reserved*/
		ERev2,
		/**Reserved*/
		ERev
		};

	/**This class contains the navigation model satellite information*/
	class TNavigationModelSatInfoV1
		{
	public:
		/**Satellite identification*/
		TUint8				iSatID;
		/**Status of the satellite*/
		TSatelliteStatus	iSatelliteStatus;
		/**Contains information for GPS ephemeris and clock correction.*/
		TEphemerisParameterV1	iEphemerisParameter;
		};

	/**
	This class contains the ionospheric model information needed to model
	the propagation delays of the GPS signals through the ionosphere
	*/
	class TIonosphericModelV1
		{
	public:
		/**
		The parameters *n are the coefficients of a cubic equation 
		representing the amplitude of the vertical delay.
		@see ICD-GPS-200
		*/
		/**Alfa = 0*/
		TUint8	iAlfa0;
		/**Alfa = 1*/
		TUint8	iAlfa1;
		/**Alfa = 2*/
		TUint8	iAlfa2;
		/**Alfa = 3*/
		TUint8	iAlfa3;
		/**
		The parameters *n are the coefficients of a cubic equation 
		representing the period of the ionospheric model 
		@see ICD-GPS-200
		*/
		/**Beta = 0*/
		TUint8	iBeta0;
		/**Beta = 1*/
		TUint8	iBeta1;
		/**Beta = 2*/
		TUint8	iBeta2;
		/**Beta = 3*/
		TUint8	iBeta3;
		};
		
	/**
	This class contains the UTC model information needed to relate GPS time to Universal Time Coordinate (UTC)
	@see ICD-GPS-200
	*/
	class TUtcModelV1
		{
	public:
		/**
		Time in seconds. 
		@see ICD-GPS-200.
		*/
		TUint	iA1;
		/**
		Time in seconds. 
		@see ICD-GPS-200.
		*/
		TUint	iA2;
		/**Time in seconds
		@see ICD-GPS-200
		*/
		TUint8	iTOt;
		/**weeks
		@see ICD-GPS-200
		*/
		TUint8	iWnT;
		/**Time in seconds
		@see ICD-GPS-200
		*/
		TUint8	iDeltaTLs;
		/**Weeks
		@see ICD-GPS-200
		*/
		TUint8	iWnLsf;
		/**Days
		@see ICD-GPS-200
		*/
		TUint8	iDn;
		/**Seconds
		@see ICD-GPS-200
		*/
		TUint8	iDeltaTLsf;
		};

	/**This class contains a reduced-precision subset of the ephemeris and clock correction parameters.*/
	class TAlmanacSatInfoV1
		{
	public:
		/**Data identification*/
		TUint8	iDataID;
		/**Eccentricity*/
		TUint16	iE;
		/**Reference Time of Almanac */
		TUint8 	iTOa;
		/**@see ICD-GPS-200*/
		TUint16	iDeltaI;
		/**Longitude of Ascending Node of Orbit Plane at Weekly Epoch (semi-circles/sec) */
		TUint16	iOmegaDot;
		/**Satellite health*/
		TUint8	iSatHealth;
		/**Semi-Major Axis in squareroot(meters)*/
		TUint	iASqrt;
		/**Longitude of Ascending Node of Orbit Plane at Weekly Epoch (semi-circles) */
		TUint	iOmega0;
		/**Mean Anomaly at Reference Time (semi-circles) */
		TUint	iM0;
		/**Argument of Perigee (semi-circles) */
		TUint	iOmega;
		/**Apparent clock correction */
		TUint16	iAf0;
		/**Apparent clock correction */
		TUint16	iAf1;
		};

	/**This class holds the coarse, long-term model of the satellite positions and clocks.*/
	class TAlmanacV1
		{
	public:
		/**Almanac Reference Week */
		TUint8	iWnA;
		TAlmanacSatInfoV1	iAlmanacSatInfoList[KMaxSat]; 
		/**This enables GPS time recovery and possibly extended GPS correlation intervals.*/
		TUint16	iSvGlobalHealth[KGlobalHealthSize]; 
		};

	/**This union contains the information specific to mode - FDD/TDD*/
	union TModeSpecificInfoV1
		{
		/**Used in FDD mode*/
		TUint16	iPrimaryScramblingCode;
		/**Used in TDD mode*/
		TUint16	iCellParametersID;
		};

	/**This class contains parameters that enable fast acquisition of the GPS 
	   signals in UE-assisted GPS positioning.*/
	class TAcquisitionAssistanceV1
		{
	public:
		/**Specifies GPS reference time*/
		TUint				iGpsReferenceTime;
		/**Specifies GPS reference time specific to mode - FDD/TDD*/
		TModeSpecificInfoV1	iUtranGpsReferenceTime;
		/**ETrue indicates iUtranGpsReferenceTime is iPrimaryScramblingCode. 
		EFalse indicates iUtranGpsReferenceTime is iCellParametersID*/
		TBool	iUtranGpsRefTimeStatus;
		};

	/**
	The UDRE provides an estimate of the uncertainty (1-*) in the corrections for 
	the particular satellite. The value in this field shall be multiplied by the 
	UDRE Scale Factor in the common Corrections Status/Health field to determine the 
	final UDRE estimate for the particular satellite. 
	@see 3GPP TS 25.305
	*/
	enum TUdre
		{
		/**Udre unknown*/
		EUdreUnknown,
		/** UDRE < 1*/
		ELessThan1,
		/** 1> UDRE < 4*/
		EBtw1And4,
		/** 1> UDRE < 8*/
		EBtw1And8,
		/** UDRE > 8*/
		EOver8
		};

	/**This class contains DGPS corrections to be used by the UE*/
	class TDgpsCorrectionSatInfoV1
		{
	public:
		/**Satellite identification*/
		TUint8	iSatID;
		/**Issue of Data Ephemeris for SatID.*/
		TUint8	iIode;
		/**
		The UDRE provides an estimate of the uncertainty (1-*) in the corrections for 
		the particular satellite. The value in this field shall be multiplied by the 
		UDRE Scale Factor in the common Corrections Status/Health field to determine the 
		final UDRE estimate for the particular satellite. 
		@see 3GPP TS 25.305
		*/
		TUdre	iUdre;
		/**The PRC indicates the correction to the pseudorange for the particular satellite at the GPS Reference Time*/
		TInt16	iPrc;
		/**
		This information indicates the rate-of-change of the pseudorange correction for the particular satellite,
		using the satellite ephemeris identified by the IODE IE
		*/
		TInt8	iRrc;
		/**For future use*/
		TInt8	iDummy1;
		/**For future use*/
		TInt8	iDummy2;
		};

	/**This class contains the DGPS corrections information*/
	class TDgpsCorrectionsV1
		{
	public:
		/**GPS time of week*/
		TUint16				iGpsTow;
		/**This information indicates the status of the differential corrections contained in the message.*/
		TDiffCorrectionsStatus	iStatusHealth;
		/**This member data contains DGPS corrections of the satellites to be used by the UE*/
		TDgpsCorrectionSatInfoV1	iDgpsCorrectionSatInfoList[KMaxSat];
		};

	/**This enum containst the possible signs of latitude*/
	enum TLatitudeSign
		{
		/**Latitude Sign Unknown*/
		ELatitudeSignUnknown,
		/**Latitude sign is north*/
		ENorth,
		/**Latitude sign is south*/
		ESouth
		};
	
	/**This enum contains the possible directions of altitudes for reference location*/
	enum TAltDirection
		{
		/**Altitude Direction Unknown*/
		EAltDirectionUnknown,
		/**Indicates Altitude direction is height-wise*/
		EHeight,
		/**Indicates Altitude direction is depth-wise*/
		EDepth
		};

	/**This class contains a 3-D location (with uncertainty)*/
	class TReferenceLocationV1
		{
	public:
		/**Specifies the sign - North/South*/
		TLatitudeSign	iLatitudeSign;
		/**Degree of latitude*/
		TUint16			iLatitude;
		/**Degree of longitude*/
		TInt16			iLongitude;
		/**Direction of altitude*/
		TAltDirection	iAltDirection;
		/**Degree of altitude*/
		TUint16			iAltitude;
		/**Uncertainty semi major axis*/
		TUint8			iUncertainitySemiMajor;
		/**Uncertainty semi minor axis*/
		TUint8			iUncertainitySemiMinor;
		/**Orientation of major axis*/
		TUint8			iOrientationMajorAxis;
		/**The uncertainty in altitude, h, expressed in metres*/
		TUint8			iUncertainityAltitude;
		/**The confidence by which the position of a target entity is known to be within the shape description*/
		TUint8			iConfidence;
		};

	/**
	Contains several fields in the Telemetry (TLM) Word and Handover Word (HOW) 
	that are currently being broadcast by the respective GPS satellites. Combining this information 
	with GPS TOW helps the UE with time-recovery needed to predict satellite signal.
	*/
	class TGpsTowAssistV1
		{
	public:
		/**Satellite identification*/
		TUint8				iSatID;
		/**
		Contains a 14-bit value representing the Telemetry Message (TLM) 
		being broadcast by the GPS satellite identified by the particular SatID, 
		with the MSB occurring first in the satellite transmission.
		*/
		TUint16				iTlmMsg;
		/**
		Contains the two reserved bits in the TLM Word 
		being broadcast by the GPS satellite identified by SatID, 
		with the MSB occurring first in the satellite transmission.
		*/
		TUint8				iTlmReserved;
		/**Contain the Alert flags that are being broadcast by the GPS satellite identified by SatID.*/
		TBool				iAlert;
		/**Contain the Anti-Spoof that are being broadcast by the GPS satellite identified by SatID.*/
		TBool				iAntiSpoof;
		};

	/**This class contains information that may be used to provide a mapping between UTRAN and GPS time*/
	class TUtranGpsRefTimeV1
		{
	public:
		/**UE-GPS timing of cell ms-part*/
		TUint16 			iMsPart;
		/**UE-GPS timing of cell ls-part*/
		TUint16				iLsPart;
		/**System frame number*/
		TUint16				iSfn;
		/**Specifies GPS reference time specific to mode - FDD/TDD*/
		TModeSpecificInfoV1	iModeSpecificInfo;
		/**ETrue indicates iUtranGpsReferenceTime is iPrimaryScramblingCode. 
		EFalse indicates iUtranGpsReferenceTime is iCellParametersID*/
		TBool	iModeSpecificInfoStatus;
		};

	/**
	This class contains several fields in the Telemetry (TLM) Word and Handover Word (HOW) 
	that are currently being broadcast by the respective GPS satellites. Combining this information 
	with GPS TOW helps the UE with time-recovery needed to predict satellite signal.
	*/
	class TReferenceTimeV1
		{
	public:
		/**GPS time in weeks*/
		TUint16				iGpsWeek;
		/**GPS Time of Week in milliseconds */
		TUint16				iGpsTowOneMsec;
		/**GPS reference time may be used to provide a mapping between UTRAN and GPS time*/
		TUtranGpsRefTimeV1	iUtranGpsRefTime;
		/**
		This field indicates the uncertainty of the relation GPS TOW/SFN. 
		ELessThan10 means the relation is accurate to at least 10 ms.
		*/
		TSfnTowUncertainity	iSfnTowUncertainity;
		/**Drift rate of the NODE B clock relative to GPS time.*/
		TUtranGpsDriftRate	iUtranGpsDriftRate;
		/**
		Contains several fields in the Telemetry (TLM) Word and Handover Word (HOW) 
		that are currently being broadcast by the respective GPS satellites. Combining this information 
		with GPS TOW helps the UE with time-recovery needed to predict satellite signal.
		*/
		TGpsTowAssistV1		iGpsTowAssistList[KMaxSat];
		};

	/**This class contains the GPS Assistance Data received from network*/
	class TUePosGpsAssistanceDataV1
		{
	public:
		/**GPS reference time may be used to provide a mapping between UTRAN and GPS time.*/
		TReferenceTimeV1 			iReferencTime;
		/**The Reference Location contains a 3-D location (with uncertainty)*/
		TReferenceLocationV1		iReferenceLocation;
		/**
		Holds information to allow a UE to estimate its position more accurate,
		biases in the pseudorange measurements may be provided to the UE.
		*/
		TDgpsCorrectionsV1			iDgpsCorrections;
		/**Holds navigation model satellite information*/
		TNavigationModelSatInfoV1	iNavigationModel;
		/**
		The Ionospheric Model contains information needed to model
		the propagation delays of the GPS signals through the ionosphere.
		*/
		TIonosphericModelV1			iIonosphericModel;
		/**UTC parameters may be used to provide Coordinated Universal Time to the UE.*/
		TUtcModelV1					iUtcModel;
		/**Holds the coarse, long-term model of the satellite positions and clocks.*/
		TAlmanacV1					iAlmanac;
		/**Holds the acquistions assistance information of the UE*/
		TAcquisitionAssistanceV1	iAcquisitionAssistance;
		/**Holds the list of bad satellites*/
		TUint8						iBadSatList[KMaxSat];
		/**
		This member specifies which of the above additional data structures are populated.
		*/
		TGpsAddlAssistDataReqV1 iGpsAddlDataStatus;
		};

	/**This class contains the GPS assistance data required to compute the UE Postioning information*/
	class TUePosMeasurementV1
		{
	public:
		/**
		The purpose of the element is to express the allowed/required location method(s), 
		and to provide information desired QoS.
		*/
		TUePosReportingQuantityV1	iUePosReportingQuantity;
		/**This member data defines in which UE states the measurement is valid.*/
		TMeasurementValidity		iMeasurementValidity;
		/**This member data holds the GPS Assistance Data sent by the network*/
		TUePosGpsAssistanceDataV1	iUePosGpsAssistanceData;
		};

	/**This holds the possible types of measurement commands that can be specified*/
	union TMeasurementCommandV1
		{
		/**Setup a new measurement*/
		TUePosMeasurementV1	iSetup;
		/**Modify a previously defined measurement*/
		TUePosMeasurementV1	iModify;
		/**Stop the measurement and clear all information in the UE that are related to that measurement.*/
		TBool	iRelease;
		};

	/**This enum specifies the mode the UE shall use to transmit the measurement report*/
	enum TMeasReportTransferMode
		{
		/**Transfer mode unknown*/
		ETransferModeUnknown,
		/**The UE shall transmit the measurement report using Acknowledged RLC mode*/
		EAckModeRlc,
		/**The UE shall transmit the measurement report using Unacknowledged RLC mode*/
		EUnAckModeRlc
		};

	/**
	This class contains the GPS assistance data needed by the UE to compute the location 
	of the UE. The UE computes the location once it receives all required information.
	*/
	class TMeasurementControlV7 : public RMobilePhone::TMultimodeType
		{
	public:
		IMPORT_C TMeasurementControlV7();
	public:
		/**
		A reference number that should be used by the UTRAN when setting up, 
		modifying or releasing the measurement and by the UE in the measurement report.
		*/
		TUint8	iMeasurementIdentity;
		/**Specifies the Measurement Setup / Modify / Release*/
		TMeasurementCommandV1	iMeasurementCommand;
		/**This specifies whether the UE shall transmit the measurement report using AM or UM RLC.*/
		TMeasReportTransferMode iMeasReportTransferMode;
		/**ETrue indicates velocity estimate is requested. EFalse indicates velocity estimate is not requested*/
		TBool	iVelocityRequested;
		/**Holds the type of measurement command being used*/
		TMeasurementCmd	iMeasurementCmdStatus;
		};
   	/**
	A typedef'd packaged TMeasurementControlV7 for passing through a
	generic API method.

	@publishedPartner
	*/
	typedef TPckg<TMeasurementControlV7> TMeasurementControlV7Pckg;
	
	//**************************************//
	// Measurement Report Data structures	//
	//**************************************//

	
	/**
	This enum contains the indicators thta specify which 
	union members of TMeasurementQuantityV1 are included
	*/
	enum TMeasQnty
		{
		/**Measurement Quantity unknown*/
		EMeasQntyUnknown,
		/**Indicates that CpichEcN0 is present*/
		EMeasQntyCpichEcN0Present,
		/**Indicates that CpichRscp is present*/
		EMeasQntyCpichRscpPresent,
		/**Indicates that path loss is present*/
		EMeasQntyPathLossPresent,
		};

	/**
	This enum contains the indicators that specify which 
	union members of TPosEstimateRefTime and  TPositionEstimateV1 are included
	*/
	enum TPosEstimateInfo
		{
		/**Position Estimate Info Unknown*/
		EPosEstimateInfoUnknown,
		/**
		NOTE: The following states correspond to TUePosPositionEstimateInfoV1/TPosEstimateRefTime
		Indicates that UE positioning estimate reference time is populated
		*/
		EUtranGpsRefTimeResultPresent,
		/**Indicates that GPS reference time is populated*/
		EGpsRefTimeOnlyPresent,
		/**Indicates that cell timing is populated*/
		ECellTimingPresent,
		/**
		NOTE: The following states correspond to TUePosPositionEstimateInfoV1/TPositionEstimateV1.
		Indicates EllipsoidPoint is populated
		*/
		EEllipsoidPointPresent,
		/**Indicates EllipsoidPoint with uncertainty circle is populated*/
		EEllipsoidPointUncertCirclePresent,
		/**Indicates EllipsoidPoint with uncertainty ellipse is populated*/
		EEllipsoidPointUncertEllipsePresent,
		/**Indicates EllipsoidPoint with altitude is populated*/
		EEllipsoidPointAltitudePresent,
		/**Indicates EllipsoidPoint with altitude and ellipse is populated*/
		EEllipsoidPointAltEllipsePresent,
		};

	/**This enum contains the possible types of velocity estimates*/
	enum TVelEstimate
		{
		/**Velocity Estimate Unknown*/
		EVelEstimateUnknown,
		/**Velocity Estimate is not present*/
		EVelEstimateNotPresent,
		/**Horizontal velocity is included*/
		EHorzVelPresent,
		/**Horizontal and vertical velocity is included*/
		EHorzWithVertVelPresent,
		/**Horizontal velocity with uncertainty is included*/
		EHorzVelWithUncertPresent,
		/**Horizontal and vertical velocity with uncertainty is included*/
		EHorzWithVertVelAndUncertPresent,
		};

	/**This contains the measurement quantities of the UE*/
	union TMeasurementQuantityV1
		{
	public:
		/**
		@see 3GPP TS 25.133.
		@see 3GPP TS 25.123.
		*/
		TUint8	iCpichEcN0;
		/**
		@see 3GPP TS 25.133.
		@see 3GPP TS 25.123.
		*/
		TUint8	iCpichRscp;
		/**Measurement path loss in dB.*/
		TUint8	iPathLoss;
		/**Extra for future use*/
		TBool	iSpare;
		};

	/**This class contains the current cell informations in TDD mode*/
	class TTddV1
		{
	public:
		/**
		Time slot Interference on Signal Code Power
		@see 3GPP TS 25.123
		*/
		TUint8	iTimeSlotIscp;
		/**Primary Common Control Physical CHannel received signal code power*/
		TUint8	iPrimaryCcpchRscp;
		};

	/**This class contains the information in FDD mode*/
	class TFddV1
		{
	public:
		/**Holds the measurement quantities of the UE*/
		TMeasurementQuantityV1	iMeasurementQuantity;
		/**Holds the field that would be populated in iMeasurementQuantity*/
		TMeasQnty	iMeasQntyStatus;
		};

	/**This contains the current cell information in specific modes - FDD/TDD*/
	union TCurrentCellV1 
		{
		/**Current cell information in FDD mode*/
		TFddV1	iFdd;
		/**Current cell information in TDD mode*/
		TTddV1	iTdd;
		};

	/**This contains the type of sfn-sfn observed time difference*/
	union TSfnSfnObsTimeDifferenceV1
		{
		/**Sfn-Sfn observed time difference -type1*/
		TUint16	iSfnSfnObsTimeDiff1;
		/**Sfn-Sfn observed time difference -type2*/
		TUint16	iSfnSfnObsTimeDiff2;
		};

	/**This class contains the FDD mode information of monitored cells*/
	class TMonitoredCellFddV1
		{
	public:
		/**Primary Common Control Physical CHannel information*/
		TUint16		iPrimaryCpichInfo;
		/**Holds the measurement quantities of the UE*/
		TMeasurementQuantityV1 iMeasurementQuantity;
		/**Holds the field that would be populated in iMeasurementQuantity*/
		TMeasQnty	iMeasQntyStatus;
		};

	/**This class contains the TDD mode information of monitored cells*/
	class TMonitoredCellTddV1
		{
	public:
		/**Cell identification*/
		TUint8	iCellParametersID;
		/**Primary Common Control Physical CHannel received signal code power*/
		TUint8	iPrimaryCcpchRscp;
		};

	/**This contains the monitored cells information in specific mode -  FDD/TDD*/
	union TMonitoredCellModeSpecificInfoV1
		{
		/**Monitored Cell information in FDD mode*/
		TMonitoredCellFddV1	iMonitoredCellFdd;
		/**Monitored Cell information in TDD mode*/
		TMonitoredCellTddV1	iMonitoredCellTdd;
		};

	/**This class contains the measured results for monitored cells on Random Access Channel*/
	class TMonitoredCellRachResultV1
		{
	public:
		/**Holds the sfn-sfn observed time difference*/
		TSfnSfnObsTimeDifferenceV1 iSfnSfnObsTimeDiff;
		/**Holds the monitored cells specific info mode - FDD/TDD*/
		TMonitoredCellModeSpecificInfoV1 iMonitoredCellModeSpecificInfo;
		/**ETrue indicates iMonitoredCellModeSpecificInfo is iMonitoredCellFdd.
		EFalse indicates iMonitoredCellModeSpecificInfo is iMonitoredCellTdd*/
		TBool	iSfnSfnObsTimeDiffStatus;
		};
	/**
	Horizontal velocity is characterised by the horizontal speed and bearing. 
	The horizontal speed gives the magnitude of the horizontal component of the velocity of a UE. 
	The bearing provides the direction of the horizontal component of velocity taken clockwise from North.
	@see 3GPP TS 23.032
	*/
	class THorzVelocityV1
		{
	public:
		/**The direction of movement is given in degrees where '0' represents North, '90' represents East, etc.*/
		TUint16		iBearing;
		/**The horizontal speed gives the magnitude of the horizontal component of the velocity of a UE*/
		TUint16		iHorzSpeed;
		};

	/**This enum holds vertical speed direction*/
	enum TVertSpeedDirection
		{
		/**Speed direction unknown*/
		ESpeedDirectionUnknown,
		/**Upward direction*/
		EUpward,
		/**Downward direction*/
		EDownward
		};

	/**
	Horizontal velocity is characterised by the horizontal speed and bearing. 
	The horizontal speed gives the magnitude of the horizontal component of the velocity of a UE. 
	The vertical speed and direction provides the component of velocity of a UE in a vertical direction.
	The bearing provides the direction of the horizontal component of velocity taken clockwise from North.
	@see 3GPP TS 23.032
	*/
	class THorzWithVertVelocityV1
		{
	public:
		/**Direction of vertical speed*/
		TVertSpeedDirection	iVertSpeedDirection;
		/**The direction of movement is given in degrees where '0' represents North, '90' represents East, etc.*/
		TUint16		iBearing;
		/**The horizontal speed gives the magnitude of the horizontal component of the velocity of a UE*/
		TUint16		iHorzSpeed;
		/**The vertical speed and direction provides the component of velocity of a UE in a vertical direction.*/
		TUint8		iVertSpeed;
		};

	/**
	Horizontal velocity with uncertainty is characterised by a horizontal speed and bearing,
	giving a horizontal velocity vector V, and an uncertainty speed s.
	@see 3GPP TS 23.032
	*/
	class THorzVelocityWithUncertaintyV1
		{
	public:
		/**	The direction of movement is given in degrees where '0' represents North, '90' represents East, etc.*/
		TUint16		iBearing;
		/**The horizontal speed gives the magnitude of the horizontal component of the velocity of a UE*/
		TUint16		iHorzSpeed;
		/**
		Horizontal speed uncertainty.
		*/
		TUint8		iHorzSpeedUncertainty;
		};

	/**
	Horizontal and vertical velocity with uncertainty is characterised by a horizontal speed and bearing, 
	giving a horizontal velocity vector Vx,y, a vertical speed and direction giving a vertical velocity component Vz,
	and uncertainty speeds s1 and s2. 
	@see 3GPP TS 23.032
	*/
	class THorzWithVertVelocityAndUncertaintyV1
		{	
	public:
		/**
		The direction of movement is given in degrees where '0' represents North, '90' represents East, etc.
		*/
		TUint16		iBearing;
		/**The horizontal speed gives the magnitude of the horizontal component of the velocity of a UE*/
		TUint16		iHorzSpeed;
		/**The vertical speed and direction provides the component of velocity of a UE in a vertical direction.*/
		TUint8		iVertSpeed;
		/**Horizontal speed uncertainty.*/
		TUint8		iHorzSpeedUncertainty;
		};

	/**
	This class contains the possible ways of velocity estimates
	@see 3GPP TS 23.032
	*/
	union TVelocityEstimateV1
		{
		/**Horizontal velocity*/
		THorzVelocityV1					iHorzVelocity;
		/**Horizontal velockity and vertical velocity*/
		THorzWithVertVelocityV1			iHorzWithVertVelocity;
		/**Horizontal velocity with uncertainty*/
		THorzVelocityWithUncertaintyV1	iHorzVelocityWithUncertainty;
		/**Horizontal and vertical velocity with uncertainty*/
		THorzWithVertVelocityAndUncertaintyV1	iHorzWithVertVelocityAndUncertainty;
		};

	/**This class contains the measured results on Random Access Channel*/
	class TMeasuredResultsOnRachV1
		{
	public:
		/**Current cell information*/
		TCurrentCellV1	iCurrentCell;
		/**Monitored Cell information*/
		TMonitoredCellRachResultV1	iMonitoredCellsRachList[KMaxMonitoredCells];
		/**ETrue indicates iCurrentCell is iFdd.
		EFalse indicates iCurrentCell is iTdd*/
		TBool	iCurrentCellStatus;
		};

	/**
	Identifies the channel in combination with the Midamble shift and slot number.
	It is not used in 1.28 Mcps TDD and may be set to either value and should be ignored by the receiver.
	*/
	enum TBurstType
		{
		/**Burst type 1*/
		EType1,
		/**Burst type 2*/
		EType2
		};

	/**This class contains UE positioning estimates specific to TDD*/
	class TPosEstimateTddV1
		{
	public:
		/**Identifies the channel in combination with the Midamble shift and slot number.*/
		TBurstType	iBurstType;
		/**Midamble shift*/
		TUint8	iMidambleShift;
		/**
		This member data is present only if no IPDL scheme is configured in the reference cell.
		Otherwise the slot is defined by the IPDL configuration.
		*/
		TUint8	iTimeSlotNumber;
		/**
		Cell identification
		*/
		TUint8	iCellParametersID;
		};

	/**This class contains UE positioning estimates specific to FDD/TDD*/
	union TPosEstimateModeSpecificInfoV1
		{
		/**Identifies the reference cell for the GPS TOW-SFN relationship.*/
		TUint16		iPrimaryCpichInfo;
		/**Holds the cell and channel identification*/
		TPosEstimateTddV1	iCellAndChannelID;
		};

	/**This class contains the cell timing information*/
	class TCellTimingV1
		{
	public:
		/**
		SFN during which the position was calculated.
		*/
		TUint16		iSfn;
		/**Holds the UE positioning estimates specific to mode - FDD or TDD*/
		TPosEstimateModeSpecificInfoV1	iPosEstimateModeSpecificInfo;
		/**ETrue indicates iPosEstimateModeSpecificInfo is iPrimaryCpichInfo.
		EFalse indicates iPosEstimateModeSpecificInfo is iCellAndChannelID.*/
		TBool	iPosEstimateModeStatus;
		};

	/**This class captures the GPS measurement specific to FDD or TDD*/
	union TGpsMeasModeSpecificInfoV1
		{
		/**Frequency Division Duplex identification*/
		TUint16	iFddRefID;
		/**Time Division Duplex identification*/
		TUint8	iTddRefID;
		};

	/**This class contains results of the GPS reference time*/
	class TUtranGpsRefTimeResultV1
		{
	public:
		/**UE-GPS timing of cell ms-part*/
		TUint16		iMsPart;
		/**UE-GPS timing of cell ls-part*/
		TUint16		iLsPart;
		/**Holds the GPS measurement specific to FDD or TDD*/
		TGpsMeasModeSpecificInfoV1	iGpsMeasModeSpecificInfo;
		/**Cell System Frame Number*/
		TUint16		iSfn;
		/**ETrue indicates iGpsMeasModeSpecificInfo is iFddRefID. 
		 EFalse indicates iGpsMeasModeSpecificInfo is iTddRefID*/
		TBool	iGpsMeasModeStatus;
		};

	/**This union contains the UE positioning reference time*/
	union TPosReferenceTimeV1
		{
		/**Contains the results of GPS reference time*/
		TUtranGpsRefTimeResultV1	iUtranGpsRefTimeResult;
		/**Contains the GPS reference time*/
		TUint16		iGpsRefTimeOnly;
		};
	
	/**
	This class contains the information of ellipsoid.
	It consists of a latitude and a longitude.
	@see 3GPP TS 23.032
	*/
	class TEllipsoidPointV1
		{
	public:
		/**Specifies the sign - North/South*/
		TLatitudeSign	iLatitudeSign;
		/**
		A north/south angular measurement of position relative to the equator, 
		in the meridian plane which contains the earth's rotation axis
		*/
		TUint16	iLatitude;
		/**
		An east/west angular measurement of position in relation to the Prime Meridian. 
		The angle between the two great circles, one being the Prime (or Greenwich) Meridian 
		and the other a meridian passing through the point of interest.
		*/
		TInt16	iLongitude;
		};

	/**
	It is characterised by the co-ordinates of an ellipsoid point (the origin) and a distance r
	@see 3GPP TS 23.032
	*/
	class TEllipsoidPointUncertCircleV1
		{
	public:
		/**Specifies the sign - North/South*/
		TLatitudeSign	iLatitudeSign;
		/**
		A north/south angular measurement of position relative to the equator, 
		in the meridian plane which contains the earth's rotation axis
		*/
		TUint16	iLatitude;
		/**
		An east/west angular measurement of position in relation to the Prime Meridian. 
		The angle between the two great circles, one being the Prime (or Greenwich) Meridian 
		and the other a meridian passing through the point of interest.
		*/
		TInt16	iLongitude;
		/**Uncertainty code*/
		TUint8	iUncertaintyCode;
		};

	/**
	It is characterised by the co-ordinates of an ellipsoid point (the origin), 
	distances r1 and r2 and an angle of orientation A
	@see 3GPP TS 23.032
	*/
	class TEllipsoidPointUncertEllipseV1
		{
	public:
		/**Specifies the sign - North/South*/
		TLatitudeSign	iLatitudeSign;
		/**
		A north/south angular measurement of position relative to the equator, 
		in the meridian plane which contains the earth's rotation axis
		*/
		TUint16	iLatitude;
		/**
		An east/west angular measurement of position in relation to the Prime Meridian. 
		The angle between the two great circles, one being the Prime (or Greenwich) Meridian 
		and the other a meridian passing through the point of interest.
		*/
		TInt16	iLongitude;
		/**Uncertainty semi major axis*/
		TUint8	iUncertaintySemiMajor;
		/**Uncertainty semi minor axis*/
		TUint8	iUncertaintySemiMinor;
		/**Orientation of major axis*/
		TUint8	iOrientationMajorAxis;
		/**The confidence by which the position of a target entity is known to be within the shape description*/
		TUint8	iConfidence;
		};

	/**
	The description of an ellipsoid point with altitude is that of a point at a specified distance 
	above or below a point on the earth's surface. This is defined by an ellipsoid point 
	with the given longitude and latitude and the altitude above or below the ellipsoid point.
	@see 3GPP TS 23.032
	*/
	class TEllipsoidPointAltitudeV1
		{
	public:
		/**Specifies the sign - North/South*/
		TLatitudeSign	iLatitudeSign;
		/**
		A north/south angular measurement of position relative to the equator, 
		in the meridian plane which contains the earth's rotation axis
		*/
		TUint16	iLatitude;
		/**
		An east/west angular measurement of position in relation to the Prime Meridian. 
		The angle between the two great circles, one being the Prime (or Greenwich) Meridian 
		and the other a meridian passing through the point of interest.
		*/
		TInt16	iLongitude;
		/**Direction of altitude*/
		TAltDirection	iAltDirection;
		/**Degree of altitude*/
		TUint16	iAltitude;
		};
	
	/**
	characterised by the co-ordinates of an ellipsoid point with altitude, distances r1 (the "semi-major uncertainty"),
	r2 (the "semi-minor uncertainty") and r3 (the "vertical uncertainty") and an angle of orientation A 
	(the "angle of the major axis")
	@see 3GPP TS 23.032
	*/
	class TEllipsoidPointAltitudeEllipseV1
		{
	public:
		/**Specifies the sign - North/South*/
		TLatitudeSign	iLatitudeSign;
		/**
		A north/south angular measurement of position relative to the equator, 
		in the meridian plane which contains the earth's rotation axis
		*/
		TUint16	iLatitude;
		/**
		An east/west angular measurement of position in relation to the Prime Meridian. 
		The angle between the two great circles, one being the Prime (or Greenwich) Meridian 
		and the other a meridian passing through the point of interest.
		*/
		TInt16	iLongitude;
		/**Direction of altitude*/
		TAltDirection	iAltDirection;
		/**Degree of latitude*/
		TUint16	iAltitude;
		/**Uncertainty semi major axis*/
		TUint8	iUncertaintySemiMajor;
		/**Uncertainty semi minor axis*/
		TUint8	iUncertaintySemiMinor;
		/**Orientation of major axis*/
		TUint8	iOrientationMajorAxis;
		/**The confidence by which the position of a target entity is known to be within the shape description*/
		TUint8	iConfidence;
		};

	/**This union holds the possible forms of position estimate of the UE that can be captured.*/
	union TPositionEstimateV1
		{
		/**
		The description of an ellipsoid point is that of a point
		on the surface of the ellipsoid, and consists of a latitude and a longitude.
		@see 3GPP TS 23.032
		*/
		TEllipsoidPointV1					iEllipsoidPoint;
		/**
		It is characterised by the co-ordinates of an ellipsoid point (the origin) and a distance r
		@see 3GPP TS 23.032
		*/
		TEllipsoidPointUncertCircleV1		iEllipsoidPointUncertCircle;
		/**
		It is characterised by the co-ordinates of an ellipsoid point (the origin), 
		distances r1 and r2 and an angle of orientation A
		@see 3GPP TS 23.032
		*/
		TEllipsoidPointUncertEllipseV1		iEllipsoidPointUncertEllipse;
		/**
		The description of an ellipsoid point with altitude is that of a point at a specified distance 
		above or below a point on the earth's surface. This is defined by an ellipsoid point 
		with the given longitude and latitude and the altitude above or below the ellipsoid point.
		@see 3GPP TS 23.032
		*/
		TEllipsoidPointAltitudeV1			iEllipsoidPointAltitude;
		/**
		characterised by the co-ordinates of an ellipsoid point with altitude, distances r1 (the "semi-major uncertainty"),
		r2 (the "semi-minor uncertainty") and r3 (the "vertical uncertainty") and an angle of orientation A 
		(the "angle of the major axis")
		@see 3GPP TS 23.032
		*/
		TEllipsoidPointAltitudeEllipseV1	iEllipsoidPointAltitudeEllipsoide;
		};

	/**This contains the positioning estimate reference time*/
	union TPosEstimateRefTime
		{
		/**Contains results of the GPS reference time*/
		TUtranGpsRefTimeResultV1	iUtranGpsRefTimeResult;
		/**Contains the GPS reference time */
		TUint16		iGpsRefTimeOnly;
		/**Contains the cell timing*/
		TCellTimingV1	iCellTiming;
		};

	/**
	Provides the position estimate from the UE to the network, 
	if the UE is capable of determining its own position.
	*/
	class TUePosPositionEstimateInfoV1
		{
	public:
		/**UE positioning estimate reference time*/
		TPosEstimateRefTime	iPosEstimateRefTime;
		/**Holds the UE's position estimate
		@see TPositionEstimateV1
		*/
		TPositionEstimateV1	iPostionEstimate;
		/**Holds the field that would be populated in iPosEstimateRefTime*/
		TPosEstimateInfo	iPosEstimateRefTimeStatus;
		/**Holds the field that would be populated in iPosEstimate*/
		TPosEstimateInfo	iPosEstimateStatus;
		};
	
	/**This enum contains the multipath indicators*/
	enum TMultipathIndicator
		{
		/**
		Multipath indicator unknown
		This is an invalid indicator. 
		*/
		EMPUnknown,
		/**Not measured*/
		ENm,
		/**MP error < 5m*/
		ELow,
		/**5m < MP error < 43m*/
		EMedium,
		/**MP error > 43m*/
		EHigh
		};

	/**This class contains the GPS measurement parameters of the UE*/
	class TGpsMeasMeasurementParamV1
		{
	public:
		/**Satellite Identification*/
		TUint8		iSatID;
		/**
		The estimate of the carrier-to-noise ratio of the received signal from the particular 
		satellite used in the measurement. It is given in units of dB-Hz (typical levels will 
		be in the range of 20 - 50 dB-Hz).
		*/
		TUint8		iCN0;
		/**
		It is the instantaneous frequency difference between the receiver's internal oscillator 
		and the received carrier from the satellite. Hz, scale factor 0.2.
		*/
		TInt16		iDoppler;
		/**
		Unit in GPS chips.
		Whole value of the UE GPS code-phase measurement, where increasing binary values of the field 
		signify increasing measured pseudoranges.
		*/
		TUint16		iWholeGpsChips;
		/**
		The UE GPS code-phase measurement is divided into the fields "Whole GPS Chips" and "Fractional GPS Chips".
		Scale factor 2-10 Fractional value of the UE GPS code-phase measurement.
		*/
		TUint16		iFractionalGpsChips;
		/**
		Contains the possible multipath indicators. Multipath error is usually caused by one path being bounced or 
		reflected. The impact on a pseudo-range measurement may be up to a few metres. In the case of carrier phase,
		this is of the order of a few centimetres.
		*/
		TMultipathIndicator	iMultipathIndicator;
		/**
		A distance measurement based on the correlation of a satellite's transmitted code (may be the C/A-Code or 
		the encrypted P-Code) and the local receiver's reference code (for that PRN satellite number), 
		that has not been corrected for errors in synchronisation between the transmitter's clock and the receiver's clock.
		Hence a pseudo-range measurement is a time-error biased distance measurement.
		*/
		TUint8		iPseudorangeRmsError;
		};

	/**This class contains the GPS measurement parameters and the positioning reference time*/
	class TUePosGpsMeasurementResultsV1
		{
	public:
		/**UE positioning reference time*/
		TPosReferenceTimeV1	iPosGpsMeasRefTime;
		/**Holds the GPS measurement parameters from all the active satellites*/
		TGpsMeasMeasurementParamV1	iGpsMeasurementParamList[KMaxSat];
		/**ETrue indicates TPosReferenceTimeV1::iUtranGpsRefTimeResult is populated. 
		 EFalse indicates TPosReferenceTimeV1::iGpsRefTimeOnly is populated*/
		TBool	iPosGpsMeasRefTimeStatus;
		};

	/**
	This enum lists all the possible errors that UE can report to the network
	*/
	enum TPosErrorCause
		{
		/**
		Error Cause Unknown. This is an invalid error cause and is set if the UE does not set with 
		any of the underlying valid error causes
		*/
		EErrorCauseUnknown,
		/**Indicates that enought number of GPS satellites are healthy and active.*/
		ENotEnoughGpsSatellites,
		/**Assistance data required to compute location of the UE is missing in the measurement control.
		If the Additional data request flag is true in the measurement control only then additional assistance
		data can be requested otherwise measurements need to be done using the existing assistance data
		@see TGpsAddlAssistDataReqV1
		@see TUePosReportingQuantityV1
		 */
		EAssistanceDataMissing,
		/**
		Cell Frames Timings are not accomplished
		*/
		ENotAccomplishedGpsTimingOfCellFrames,
		/**
		UE can set this error in case other than the above mentioned is encountered 
		*/
		EUndefinedError,
		/**
		UE denies Location information
		*/
		ERequestDeniedByUser,
		};

	/**
	This class contains the positioning errors that might have encountered during 
	measurement of location of the UE.
	*/
	class TUePosError
		{
	public:
		/**Indicates the error cause encountered while computing GPS Assistance data*/
		TPosErrorCause		iPosErrorCause;
		/**If error cause is "Assistance Data Missing" and iAddlAssistanceDataReq is set to ETrue then
		then this member shall indicate what additional assistance data is needed.
		If iAddlAssistanceDataReq is set to EFalse then existing assistance data is used to compute location
		information of the UE.
		@see TUePosReportingQuantityV1::iAddlAssistanceDataReq*/
		TGpsAddlAssistDataReqV1	iGpsAddlAssistDataReq;
		};

	/**
	This class contains the measurement results computed by the UE.
	*/
	class TUePosMeasuredResultsV1
		{
	public:
		/**The purpose of this is to provide the position estimate from the UE to the network, 
		if the UE is capable of determining its own position.*/
		TUePosPositionEstimateInfoV1	iUePosPositionEstimateInfo;
		/**Contains the GPS measurement parameters and the positioning reference time*/
		TUePosGpsMeasurementResultsV1	iUePosGpsMeasurement;
		/**Indicates the error cause and/or additional assistance data is required*/
		TUePosError						iUePosError;
		/**
		ETrue indicates some error has encountered. iUePosError will be populated.
		EFalse indicates Gps Assistance Data is received and no error has encountered while processing.
		Hence iUePosPositionEstimateInfo and iUePosGpsMeasurement will be populated
		*/
		TBool	iPosErrorStatus;
		};

	/**Contains the measured location information of the UE*/
	union TMeasuredResultsV1
		{
		/**
		This member either contains the measurement results or the error that might have 
		encountered during measurements.
		*/
		TUePosMeasuredResultsV1		iTUePosMeasuredResults;
		/**
		A spare member for future use.
		*/
		TBool						iSpare;
		};

	/**
	This class contains the measurement report expected as response to the measurement control sent by the 
	network. Veloctiy estimates are included if requested in measurement control.
	*/
	class TMeasurementReportV7 : public RMobilePhone::TMultimodeType
		{
	public:
		IMPORT_C TMeasurementReportV7();
	public:
		/**A reference number that should be used by the UTRAN when setting up, 
		modifying or releasing the measurement and by the UE in the measurement report.
		*/
		TUint8 iMeasurementIdentity;
		/**Contains the measured location information of the UE*/
		TMeasuredResultsV1 iMeasuredResults;
		/**Contains the measured results on Random Access Channel*/
		TMeasuredResultsOnRachV1	iMeasuredResultsOnRach;
		/**Contains the velocity estimates of the UE*/
		TVelocityEstimateV1		iVelocityEstimate;
		/**
		Indicates the populated member in iVelocityEstimate if velocity estimate is computed.
		Otherwise indicates that velocity estimate is not computed.
		*/
		TVelEstimate	iVelEstimateStatus;
		};
	/**
	A typedef'd packaged TMeasurementReportV7 for passing through a
	generic API method.

	@publishedPartner
	*/
	typedef TPckg<TMeasurementReportV7> TMeasurementReportV7Pckg;
	
	//*********************************************//	
	//  Measurement Control Failure data structure //
	//*********************************************//

	/**
	This enum contains the list of errors that the UE can specify.
	The Measurement control failure is caused only when the the UE 
	cannot initiate a measurement as instructed by the network.
	*/
	enum TMeasurementControlFailure
		{
		/**
		Unknown Measurement Control Failure.
		This is an invalid failure state whic will be set if the UE does not set with the 
		underlying valid error reasons.
		*/
		EControlFailureUnknown,
		/**Indicates ASN1.0 encoding is incorrect*/
		EAsn1ViolationOrEncoding,
		/**Messsgae type specified is not existent*/
		EMsgTypeNonExistent,
		/**Message is not compatible*/
		EMsgNotCompatibleWithReceiverState,
		/**Information sent cannot be understood*/
		EIeValueNotComprehend,
		/**Information expected is not present*/
		EInformationElementMissing,
		/*Message extension cannot be understood*/
		EMsgExtnNotComprehend
		}; 
		
	/**
	This enum contains the possible domains used by MOLR 
	*/
	enum TDomain
   		{
		/**Packet switch domain is the default domain that will be used by Molr*/
   		EPacketSwitchedDomain,
		/**UE shall set the domain to Circuit switch whenever Molr is intended to use it*/
  	 	ECircuitSwitchedDomain
   		};

	/**
	The UE Positioning Technology value, as defined in 3GPP TS 34.109 V7.2.0 Section 6.10
	@publishedPartner
	@released
	*/
	enum TUePosTechnology
		{
		/** A-GPS */
		KUePosTechnologyAGPS = 0
		};

	IMPORT_C void NotifyMtlr(TRequestStatus& aReqStatus,TDes8& aNotifyMtlr);
	IMPORT_C void SendMtlrResponse(TRequestStatus& aReqStatus,const TMtlrResponse aMtlrResponse);
	IMPORT_C void SendMtlrResponse(TRequestStatus& aReqStatus,const TMtlrError aMtlrResponse);
	IMPORT_C void SendMtlrResponse(TRequestStatus& aReqStatus,const TMtlrReject aMtlrResponse);
    IMPORT_C void SendMolr(TRequestStatus& aReqStatus,const TDesC8& aStartMolr,TDes8& aMolrReturnResult);
	IMPORT_C void NotifyMeasurementControl(TRequestStatus& aReqStatus,TDes8& aMeasurementControl);
	IMPORT_C void SendMeasurementReport(TRequestStatus& aReqStatus,const TDesC8& aMeasurementReport);
	IMPORT_C void SendMeasurementReport(TRequestStatus& aReqStatus,const TMeasurementControlFailure aMeasurementControlFailure);
	IMPORT_C TInt SetLcsDomain(const TDomain aDomain);	
	IMPORT_C void NotifyResetUePositioningInformation(TRequestStatus& aReqStatus, TUePosTechnology& aUePosTechnology) const;

private:
	void ConstructL();
	void Destruct();
private:
	/**
	Pointer Holder for the RMobileLocationServices sub-session requests.
	*/
	CMobileLocationServicesPtrHolder* iMmPtrHolder;
	};

#endif // __ETELMM_H__
