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
// Comms Data Type Definitions version 1.1
// Type info for Symbian OS Comms Data Types that can be stored in the Comms Repository
// Information held here includes
// - Field ID
// - Field Name
// - Field Value Type
// - Field Max Length (for descriptors)
// Type Definitions are version controlled.  New versions may be introduced from time to time.
// Then older versions are marked as deprecated and given a planned removal date.
// This version introduced with Release 9.1
// 
//

/**
 @file
 @publishedPartner
 @released 
*/

#if (!defined COMMSDATTYPEINFOV1_1_H)
#define       COMMSDATTYPEINFOV1_1_H

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypeinfov1_1_internal.h>
#endif

namespace CommsDat
{

	// TIds for each Comms Record Type


	// Internal
	const TMDBElementId KCDTIdGlobalSettingsRecord		 	  = 0x00800000; //< Record identifier for GlobalSettings table
	const TMDBElementId KCDTIdConnectionPrefsRecord			  = 0x01000000; //< Record identifier for ConnectionPreferences table

	const TMDBElementId KCDTIdNetworkRecord					  = 0x01800000; //< Record identifier for Network table
	const TMDBElementId KCDTIdLocationRecord				  = 0x02000000; //< Record identifier for Location table
	const TMDBElementId KCDTIdIAPRecord						  = 0x02800000; //< Record identifier for IAP table

	// WAP
	const TMDBElementId KCDTIdWAPAccessPointRecord			  = 0x03000000; //< Record identifier for WAPAccessPoint table

	// Service
	const TMDBElementId KCDTIdDialOutISPRecord				  = 0x03800000; //< Record identifier for DialOutISP table
	const TMDBElementId KCDTIdDialInISPRecord				  = 0x04000000; //< Record identifier for DialInISP table
	const TMDBElementId KCDTIdLANServiceRecord				  = 0x04800000; //< Record identifier for LANService table

	/**
	WLANServiceExtension is internal Table
	*/
	const TMDBElementId KCDTIdWLANServiceExtRecord			  = 0x05000000; //
	const TMDBElementId KCDTIdVPNServiceRecord				  = 0x05800000; //< Record identifier for VPNService table

    const TMDBElementId KCDTIdWCDMAPacketServiceRecord		  = 0x06800000; //< Record identifier for WCDMAPacketService table
	const TMDBElementId KCDTIdOutgoingGprsRecord			  = 0x0C800000; //< Record identifier for OutgoingGPRS table
	const TMDBElementId KCDTIdIncomingGprsRecord			  = 0x0D000000; //< Record identifier for IncomingGPRS table
	
	const TMDBElementId KCDTIdPANServiceExtRecord			  = 0x0D800000; //< Record identifier for PANServiceExt table

	// Service Defaults
	const TMDBElementId KCDTIdDefaultWCDMARecord	    	  = 0x07800000; //< Record identifier for DefaultWCDMA table

	// Bearer
	const TMDBElementId KCDTIdModemBearerRecord				  = 0x08000000; //< Record identifier for ModemBearer table
	const TMDBElementId KCDTIdLANBearerRecord				  = 0x08800000; //< Record identifier for LANBearer table
	const TMDBElementId KCDTIdVirtualBearerRecord			  = 0x09000000; //< Record identifier for VirtualBearer table

	const TMDBElementId KCDTIdWAPSMSBearerRecord			  = 0x09800000; //< Record identifier for WAPSMSBearer table
	const TMDBElementId KCDTIdWAPIPBearerRecord				  = 0x0A000000; //< Record identifier for WAPIPBearer table

	const TMDBElementId KCDTIdChargecardRecord				  = 0x0A800000; //< Record identifier for Chargecard table
	const TMDBElementId KCDTIdProxiesRecord					  = 0x0B000000; //< Record identifier for Proxies table

	
	const TMDBElementId KCDTIdAccessPointRecord	        	  = 0x0E000000; //< Record identifier for AccessPoint table
	const TMDBElementId KCDTIdIapPrioritySelectionPolicyRecord= 0x0E800000; //< Record identifier for IAPPrioritySelectionPolicy table

	const TMDBElementId KCDTIdUmtsR99QoSAndOnTableRecord      = 0x0F000000; //< Record identifier for UmtsR99QoS table


	const TMDBElementId KCDTIdEapSimProtocolRecord            = 0x17800000; //< Record identifier for EapSimProtocol table.
	const TMDBElementId KCDTIdEapAkaProtocolRecord            = 0x18000000; //< Record identifier for EapAkaProtocol table.

 // Comms Record type names.- 26 in total

	#define	KCDTypeNameConnectionPrefs						_S("ConnectionPreferences")
	#define	KCDTypeNameGlobalSettings						_S("GlobalSettings")

	#define	KCDTypeNameNetwork								_S("Network")
	#define	KCDTypeNameIAP									_S("IAP")
	#define	KCDTypeNameLocation								_S("Location")

	#define	KCDTypeNameWAPAccessPoint						_S("WAPAccessPoint")

	// Service
	#define	KCDTypeNameDialOutISP							_S("DialOutISP")
	#define	KCDTypeNameDialInISP							_S("DialInISP")
	#define	KCDTypeNameLANService							_S("LANService")
	#define	KCDTypeNameWLANServiceExt						_S("WLANServiceExtensionTable")
	#define	KCDTypeNameVPNService							_S("VpnService")
	#define	KCDTypeNameOutgoingWCDMA						_S("OutgoingGPRS")
	#define	KCDTypeNameIncomingWCDMA						_S("IncomingGPRS")

	#define	KCDTypeNameDefaultWCDMA							_S("DefaultGPRS")
	
	#define KCDTypeNamePANServiceExt						_S("PANServiceExtensions")

	// Bearer
	#define	KCDTypeNameModemBearer							_S("ModemBearer")
	#define	KCDTypeNameLANBearer							_S("LANBearer")
	#define	KCDTypeNameVirtualBearer						_S("VirtualBearer")

	#define	KCDTypeNameWAPSMSBearer							_S("WAPSMSBearer")
	#define	KCDTypeNameWAPIPBearer							_S("WAPIPBearer")

	// Other
	#define	KCDTypeNameChargecard							_S("Chargecard")
	#define	KCDTypeNameProxies								_S("Proxies")
	#define	KCDTypeNameSSProto								_S("SecureSocketTable")
	#define	KCDTypeNameAgentLookup							_S("AgentLookup")

	#define	KCDTypeNameService								_S("IAPService")
	#define	KCDTypeNameBearer								_S("IAPBearer")

    #define KCDTypeNameUmtsR99QoSAndOnTable                 _S("UmtsR99QoSAndOnTable")
	#define	KCDTypeNameEapSimProtocol 						_S("EapSimProtocol")
	#define	KCDTypeNameEapAkaProtocol 						_S("EapAkaProtocol")

	#define	KCDTTypeNameEAPSec								_S("EAPSecuritySettingsTable")
	#define	KCDTTypeNameTunEAP								_S("TunnelledEAPSettingsTable")
	#define	KCDTTypeNameEAPTLS								_S("EAPTLSSettingsTable")
	#define	KCDTTypeNameLEAP								_S("LEAPSettingsTable")

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	#define	KCDTTypeNamePolicySelector						_S("PolicySelectorTable")
	#define	KCDTTypeNamePolicySelector2Params				_S("PolicySelector2ParamsTable")

	#define	KCDTTypeNameGenericQoSParams					_S("GenericQoSParamsTable")
	/**
	@internalTechnology
	*/
	#define KCDTTypeNameWifiScanEngine                 		_S("WifiScanEngineTable")

#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

	#define	KCDNull											_S("")

    #define KCDTypeNameAccessPoint                          _S("AccessPoint")
    #define KCDTypeNameIapPrioritySelectionPolicy           _S("IAPPrioritySelectionPolicy")


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	#define KCDTypeNameApPrioritySelectionPolicy            _S("APPrioritySelectionPolicy")
    
    #define	KCDTypeNameTierRecord							_S("Tier")
	#define	KCDTypeNameMCprRecord							_S("MCpr")
	#define	KCDTypeNameCprRecord							_S("Cpr")
	#define	KCDTypeNameSCprRecord							_S("SCpr")
	#define	KCDTypeNameProtocolRecord						_S("Protocol")

	/**
	@InternalComponent
	*/
	#define	KCDTypeNameBearerTypeRecord						_S("BearerType")
	#define KCDTypeNameConfigAccessPointRecord				_S("ConfigAccessPoint")
	
#endif
	#define	KCDNull											_S("")




#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//
// BASE CLASS FOR ALL PARAMS RECORDS
// Contains fields common to all params records
//
	const TMDBElementId KCDTIdParamsSTypeId		                = 0x007C0000;
	
	#define	KCDTypeNameParamsSTypeId			                _S("STypeId")

#endif

//
// BASE CLASS FOR ALL SERVICE RECORDS
// Contains fields common to all service records
//
	const TMDBElementId KCDTIdServiceEnableLlmnr                = 0x007C0000;	//< Commsdat field id for field: EnableLLMNR. Field is declared in the following service tables: DialOutISP, DialInISP, VPNService, LanService and WCDMAPacketService table.
	
	#define	KCDTypeNameServiceEnableLlmnr		                _S("EnableLLMNR")


//
// BASE CLASS FOR ALL BEARER RECORDS
// Contains fields common to all bearer records
//
    const TMDBElementId KCDTIdBearerAgent		    = 0x007C0000;		//< Commsdat field id for field: BearerAgent. Field is declared in the following tables: LanBearer, VirtualBearer and ModemBearer table.
	const TMDBElementId KCDTIdBearerTechnology  	= 0x007B0000;		//< Commsdat field id for field: BearerTechnology. Field is declared in the following tables: LanBearer, VirtualBearer and ModemBearer table.

    #define	KCDTypeNameBearerAgent		            _S("Agent")
    #define	KCDTypeNameBearerTechnology            _S("BearerTechnology")
//
// 008/  GLOBAL SETTINGS RECORD
//

	// Element Type IDs
	const TMDBElementId KCDTIdMaxConnectionAttempts         = 0x00830000;	//< Commsdat field id for field: ConnectionAttempts. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdMaxRedialAttempts             = 0x00840000;	//< Commsdat field id for field: RedialAttempts. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdSMSBearer						= 0x00850000;	//< Commsdat field id for field: SMSBearer. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdSMSReceiveMode				= 0x00860000;	//< Commsdat field id for field: SMSReceiveMode. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdGPRSAttachMode                = 0x00870000;	//< Commsdat field id for field: GPRSAttachMode. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdAcceptIncomingGPRS            = 0x00880000;	//< Commsdat field id for field: AcceptIncomingGPRS. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdGPRSClassCBearer				= 0x00890000;	//< Commsdat field id for field: GPRSClassCBearer. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdModemDataFax                  = 0x008A0000;	//< Commsdat field id for field: ModemForDataAndFax. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdModemPhoneServicesSMS			= 0x008B0000;	//< Commsdat field id for field: ModemForPhoneServicesAndSMS. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdLocationDataFax               = 0x008C0000;	//< Commsdat field id for field: LocationForDataAndFax. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdLocationPhoneServicesSMS		= 0x008D0000;	//< Commsdat field id for field: LocationForPhoneServicesAndSMS. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdMaxMBufHeap					= 0x008E0000;	//< Commsdat field id for field: MaxMBufHeap. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdDefaultAgent					= 0x008F0000;	//< Commsdat field id for field: DefaultAgent. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdDefaultNetwork				= 0x00900000;	//< Commsdat field id for field: DefaultNetwork. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdBearerAvailabilityCheckTSY	= 0x00910000;	//< Commsdat field id for field: BearerAvailabilityCheckTSY. Field is declared in the GlobalSettings table.
	const TMDBElementId KCDTIdGlobalWapAcessPoint			= 0x009D0000;	//< Commsdat field id for field: GlobalWapAcessPoint. Field is declared in the GlobalSettings table.
    // #ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    // @deprecated 
    // Maps to KCDTIdDefaultAccessPoint in the Tier Table record for the 
    // default Tier (KCDTIdDefaultTier)  
	const TMDBElementId KCDTIdDefaultSnap					= 0x009E0000;	//< Commsdat field id for field: DefaultSnap. Field is declared in the GlobalSettings table.
    // #ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	// @deprecated
    // Maps to KCDTIdPromptUser in the Tier Table record for the 
    // Default Tier (KCDTIdDefaultTier)
	const TMDBElementId KCDTIdPromptForSnap					= 0x009F0000;	//< Commsdat field id for field: PromptForSnap. Field is declared in the GlobalSettings table.

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	const TMDBElementId KCDTIdDefaultTier					= 0x00A00000;
#endif

    const TMDBElementId KCDTIdRegulatoryDomain				= 0x00A30000;
	const TMDBElementId KCDTIdWmmEnabled 					= 0x00A10000;
	const TMDBElementId KCDTIdEnableCountryCode				= 0x00A20000;
	const TMDBElementId KCDTIdDefaultRegulatoryDomain		= 0x00A30000;

	// Element Type names .
	#define	KCDTypeNameMaxRedialAttempts			_S("RedialAttempts")
	#define	KCDTypeNameSMSBearer					_S("SmsBearer")
	#define	KCDTypeNameSMSReceiveMode				_S("SmsReceiveMode")
	#define	KCDTypeNameGPRSAttachMode				_S("GPRSAttachMode")
	#define	KCDTypeNameAcceptIncomingGPRS			_S("AcceptIncomingGprs")
	#define	KCDTypeNameGPRSClassCBearer				_S("GPRSClassCBearer")
	#define	KCDTypeNameMaxConnectionAttempts		_S("ConnectionAttempts")
	#define	KCDTypeNameModemDataFax					_S("ModemForDataAndFax")
	#define	KCDTypeNameModemPhoneServicesSMS		_S("ModemForPhoneServicesAndSMS")
	#define	KCDTypeNameLocationDataFax				_S("LocationForDataAndFax")
	#define	KCDTypeNameLocationPhoneServicesSMS		_S("LocationForPhoneServicesAndSMS")
	#define	KCDTypeNameMaxMBufHeap					_S("MaxMBufHeap")
	#define	KCDTypeNameDefaultAgent					_S("DefaultAgent")
	#define	KCDTypeNameDefaultNetwork				_S("DefaultNetwork")
	#define	KCDTypeNameBearerAvailabilityCheckTSY	_S("BearerAvailabilityCheckTSY")
	#define KCDTypeNameDefaultSnap					_S("DefaultSnap")	
	#define KCDTypeNamePromptForSnap			    _S("PromptForSnap")
	#define KCDTypeNameWCDMAPacketService		    _S("WCDMAPacketService")
		
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//the name of this field (KCDTypeNamePromptForSnap) depends form the actual environment
//    #define KCDTypeNamePromptForSnap			    _S("PromptUser")
	#define KCDTypeNameDefaultTier				    _S("DefaultTier")
#else
//the name of this field (KCDTypeNamePromptForSnap) depends form the actual environment
//    #define KCDTypeNamePromptForSnap			    _S("PromptForSnap")
#endif

#define KCDTypeNameRegulatoryDomain				_S("RegulatoryDomain")

	#define KCDTypeNameWmmEnabled				    _S("WmmEnabled")
	#define KCDTypeNameEnableCountryCode			_S("EnableCountryCode")
	#define KCDTypeNameDefaultRegulatoryDomain		_S("DefaultRegulatoryDomain")


//
// 010/ CONNECTION PREFERENCES RECORD
//

	// Element Type Ids
	const TMDBElementId KCDTIdRanking         = 0x01030000;		//< Commsdat field id for field: Ranking. Field is declared in the ConnectionPreferences table.
	const TMDBElementId KCDTIdDirection       = 0x01040000;		//< Commsdat field id for field: Direction. Field is declared in the ConnectionPreferences table.
	const TMDBElementId KCDTIdBearerSet       = 0x01050000;		//< Commsdat field id for field: BearerSet. Field is declared in the ConnectionPreferences table.
	const TMDBElementId KCDTIdDialogPref      = 0x01060000;		//< Commsdat field id for field: DialogPref. Field is declared in the ConnectionPreferences table.
	const TMDBElementId KCDTIdIAPLink         = 0x01070000;		//< Commsdat field id for field: IAP. Field is declared in the ConnectionPreferences table.


	// Element Type Names
	#define	KCDTypeNameRanking				_S("Ranking")
	#define	KCDTypeNameDirection			_S("Direction")
	#define	KCDTypeNameBearerSet			_S("BearerSet")
	#define	KCDTypeNameDialogPref			_S("DialogPref")






//
// 018  NETWORK RECORD
//
        // Field Type IDs
	const TMDBElementId KCDTIdHostName				  = 0x01830000;	//< Commsdat field id for field: HostName. Field is declared in the Network table.

        // NETWORK Table Field Names
	#define	KCDTypeNameHostName						_S("HostName")

//
// 020  LOCATION RECORD
//

	// Field Type IDs
	const TMDBElementId KCDTIdIntlPrefixCode				  = 0x02030000;		//< Commsdat field id for field: IntlPrefixCode. Field is declared in the Location table.
	const TMDBElementId KCDTIdNatPrefixCode					  = 0x02040000;		//< Commsdat field id for field: NatPrefixCode. Field is declared in the Location table
	const TMDBElementId KCDTIdNatCode				  		  = 0x02050000;		//< Commsdat field id for field: NatCode. Field is declared in the Location table
	const TMDBElementId KCDTIdAreaCode						  = 0x02060000;		//< Commsdat field id for field: AreaCode. Field is declared in the Location table
	const TMDBElementId KCDTIdDialOutCode					  = 0x02070000;		//< Commsdat field id for field: DialOutCode. Field is declared in the Location table
	const TMDBElementId KCDTIdDisableCallWaitingCode	      = 0x02080000;		//< Commsdat field id for field: DisableCallWaitingCode. Field is declared in the Location table
	const TMDBElementId KCDTIdMobile						  = 0x02090000;		//< Commsdat field id for field: Mobile. Field is declared in the Location table
	const TMDBElementId KCDTIdUsePulseDial					  = 0x020A0000;		//< Commsdat field id for field: UsePulseDial. Field is declared in the Location table
	const TMDBElementId KCDTIdWaitForDialTone				  = 0x020B0000;		//< Commsdat field id for field: WaitForDialTone. Field is declared in the Location table
	const TMDBElementId KCDTIdPauseAfterDialOut			      = 0x020C0000;		//< Commsdat field id for field: PauseAfterDialOut. Field is declared in the Location table


	// Location Table Field Names
	#define	KCDTypeNameIntlPrefixCode						_S("IntlPrefixCode")
	#define	KCDTypeNameNatPrefixCode						_S("NatPrefixCode")
	#define	KCDTypeNameNatCode								_S("NatCode")
	#define	KCDTypeNameAreaCode								_S("AreaCode")
	#define	KCDTypeNameDialOutCode							_S("DialOutCode")
	#define	KCDTypeNameDisableCallWaitingCode		        _S("DisableCallWaitingCode")
	#define	KCDTypeNameMobile								_S("Mobile")
	#define	KCDTypeNameUsePulseDial							_S("UsePulseDial")
	#define	KCDTypeNameWaitForDialTone						_S("WaitForDialTone")
	#define	KCDTypeNamePauseAfterDialOut		            _S("PauseAfterDialout")
	#define KCDTypeNameOPCapability	   						_S("OPCapability")
	#define	KCDTypeNameSimipMaxNumNai						_S("SimipMaxNumNai")
	#define	KCDTypeNameSimipMaxNaiLength					_S("SimipMaxNaiLength")   
	#define	KCDTypeNameSimipMaxSsLength						_S("SimipMaxSsLength")
	#define	KCDTypeNameSimipAuthSupported					_S("SimipAuthSupported")
	#define	KCDTypeNameMipMaxNumNai							_S("MipMaxNumNai ")
	#define	KCDTypeNameMipMaxNaiLength						_S("MipMaxNaiLength")
	#define	KCDTypeNameMipMaxMnAaaSsLength					_S("MipMaxMnAaaSsLength")
	#define	KCDTypeNameMipMnAaaAuthAlgorithm				_S("MipMnAaaAuthAlgorithm")
	#define	KCDTypeNameMipMaxMnHaSsLength					_S("MipMaxMnHaSsLength")

//
// 028/  IAP RECORD
//

	// FieldTIds
	const TMDBElementId KCDTIdIAPServiceType 				= 0x02830000; //< Commsdat field id for field: IAPServiceType. Type of Service record. Field is declared in the IAP table
	const TMDBElementId KCDTIdIAPService 					= 0x02840000; //< Commsdat field id for field: IAPService. Link to a Service record. Field is declared in the IAP table
	const TMDBElementId KCDTIdIAPBearerType 				= 0x02850000; //< Commsdat field id for field: IAPBearerType. Type of Bearer record. Field is declared in the IAP table.
	const TMDBElementId KCDTIdIAPBearer 					= 0x02860000; //< Commsdat field id for field: IAPBearer. Link to a Bearer record. Field is declared in the IAP table. 
	const TMDBElementId KCDTIdIAPNetwork 					= 0x02870000; //< Commsdat field id for field: IAPNetwork. Link to a Network record. Field is declared in the IAP table.
	const TMDBElementId KCDTIdIAPNetworkWeighting 			= 0x02880000; //< Commsdat field id for field: IAPNetworkWeighting. Field is declared in the IAP table.
	const TMDBElementId KCDTIdIAPLocation 					= 0x02890000; //< Commsdat field id for field: Location. Link to a Location record. Field is declared in the IAP table.
#ifdef SYMBIAN_NETWORKING_UMTSR5
	const TMDBElementId KCDTIdIAPAppSid 					= 0x028A0000; //< Commsdat field id for field: IAPAppSid.Link to a Location record. Field is declared in the IAP table.
#endif

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	// Not a real field - represents the real IAP on a VPN connection
	// If the connection is not using a VPN the value will match
	// the IAP Id (KCDTIdIAPRecord | KCDTIdRecordTag)
	const TMDBElementId KCDTIdRealIAP						= 0x02FF0000;
#endif

  // Field Names
	#define	KCDTypeNameIAPNetwork				       _S("IAPNetwork")
	#define	KCDTypeNameNetworkWeighting		           _S("IAPNetworkWeighting")
	#define	KCDTypeNameServiceType		               _S("IAPServiceType")
	#define	KCDTypeNameBearerType		               _S("IAPBearerType")
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	#define	KCDTypeNameAccessPoint		               _S("AccessPoint")
#endif
#ifdef SYMBIAN_NETWORKING_UMTSR5
	#define	KCDTypeNameAppSid		                   _S("IAPAppSid")
#endif





//
// 030/ WAP ACCESS POINT RECORD
//


	// Element Type Ids
	const TMDBElementId KCDTIdWAPCurrentBearer  = 0x03030000;		//< Commsdat field id for field: CurrentBearer. Field is declared in the WAP Access Point table.
	const TMDBElementId KCDTIdWAPStartPage      = 0x03040000;		//< Commsdat field id for field: StartPage. Field is declared in the WAP Access Point table.


	// Element Type Names
	#define	KCDTypeNameWAPCurrentBearer		      _S("CurrentBearer")
	#define	KCDTypeNameWAPStartPage		          _S("StartPage")


//
// 06/  DIAL OUT ISP SERVICE RECORD
//

	// FieldTIds

	const TMDBElementId KCDTIdDescription               = 0x03830000;	//< Commsdat field id for field: Description. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdType						= 0x03840000;	//< Commsdat field id for field: Type. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdDefaultTelNum             = 0x03850000;	//< Commsdat field id for field: DefaultTelNum. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdDialResolution            = 0x03860000;	//< Commsdat field id for field: DialResolution. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdChargecard                = 0x03870000;	//< Commsdat field id for field: ChargeCard. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdUseLoginScript            = 0x03880000;	//< Commsdat field id for field: UseLoginScript. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdLoginScript               = 0x03890000;	//< Commsdat field id for field: LoginScript. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdPromptForLogin            = 0x038A0000;	//< Commsdat field id for field: PromptForLogin. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdLoginName                 = 0x038B0000;	//< Commsdat field id for field: LoginName. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdLoginPass                 = 0x038C0000;	//< Commsdat field id for field: LoginPass. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdDisplayPct                = 0x038D0000;	//< Commsdat field id for field: DisplayPCT. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIfParams                  = 0x038F0000;	//< Commsdat field id for field: IfParams. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIfNetworks                = 0x03900000;	//< Commsdat field id for field: IfNetworks. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIfPromptForAuth           = 0x03910000;	//< Commsdat field id for field: IfPromptForAuth. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIfAuthName                = 0x03920000;	//< Commsdat field id for field: IfAuthName. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIfAuthPass                = 0x03930000;	//< Commsdat field id for field: IfAuthPass. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIfAuthRetries             = 0x03940000;	//< Commsdat field id for field: IfAuthRetries. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIfCallbackEnabled         = 0x03950000;	//< Commsdat field id for field: IfCallbackEnabled. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIfCallbackType            = 0x03960000;	//< Commsdat field id for field: IfCallbackType. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIfCallbackInfo            = 0x03970000;	//< Commsdat field id for field: IfCallbackInfo. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdCallbackTimeout           = 0x03980000;	//< Commsdat field id for field: CallbackTimeout. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIfServerMode              = 0x03990000;	//< Commsdat field id for field: IfServerMode. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIpAddrFromServer          = 0x039A0000;	//< Commsdat field id for field: IpAddrFromServer. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIpAddr					= 0x039B0000;	//< Commsdat field id for field: IpAddr. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIpNetMask                 = 0x039C0000;	//< Commsdat field id for field: IpNetmask. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIpGateway                 = 0x039D0000;	//< Commsdat field id for field: IpGateway. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIpDNSAddrFromServer       = 0x039E0000;	//< Commsdat field id for field: IpDNSAddrFromServer. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIpNameServer1             = 0x039F0000;	//< Commsdat field id for field: IpNameServer1. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIpNameServer2             = 0x03A00000;	//< Commsdat field id for field: IpNameServer2. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIp6DNSAddrFromServer      = 0x03A10000;	//< Commsdat field id for field: Ip6DNSAddrFromServer. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIp6NameServer1            = 0x03A20000;	//< Commsdat field id for field: Ip6NameServer1 . Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIp6NameServer2            = 0x03A30000;	//< Commsdat field id for field: Ip6NameServer2. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIpAddrLeaseValidFrom      = 0x03A40000;	//< Commsdat field id for field: IpAddrLeaseValidFrom. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIpAddrLeaseValidTo        = 0x03A50000;	//< Commsdat field id for field: IpAddrLeaseValidTo. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdConfigDaemonManagerName   = 0x03A60000;	//< Commsdat field id for field: ConfigDaemonManagerName. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdConfigDaemonName          = 0x03A70000;	//< Commsdat field id for field: ConfigDaemonName. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdEnableIpHeaderComp        = 0x03A80000;	//< Commsdat field id for field: EnableIpHeaderComp. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdEnableLCPExtension        = 0x03A90000;	//< Commsdat field id for field: EnableLCPExtension. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdDisablePlainTextAuth      = 0x03AA0000;	//< Commsdat field id for field: DisablePlainTextAuth. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdEnableSwComp              = 0x03AB0000;	//< Commsdat field id for field: EnableSwComp. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdBearerName                = 0x03AC0000;	//< Commsdat field id for field: BearerName. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdBearerSpeed               = 0x03AD0000;	//< Commsdat field id for field: BearerSpeed. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdBearerCe                  = 0x03AE0000;	//< Commsdat field id for field: BearerCE. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdInitString                = 0x03AF0000;	//< Commsdat field id for field: InitString. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdBearerType                = 0x03B00000;	//< Commsdat field id for field: BearerType. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdChannelCoding             = 0x03B10000;	//< Commsdat field id for field: ChannelCoding. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdAiur						= 0x03B20000;	//< Commsdat field id for field: AIUR. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdRequestedTimeSlots        = 0x03B30000;	//< Commsdat field id for field: RequestedTimeSlots. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdMaximumTimeSlots          = 0x03B40000;	//< Commsdat field id for field: MaximumTimeSlots. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdBearerService             = 0x03B50000;	//< Commsdat field id for field: BearerService. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdBearerProtocol            = 0x03B60000;	//< Commsdat field id for field: BearerProtocol. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdRlpVersion                = 0x03B70000;	//< Commsdat field id for field: RlpVersion. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdIwfToMs                   = 0x03B80000;	//< Commsdat field id for field: IwfToMs. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdMsToIwf                   = 0x03B90000;	//< Commsdat field id for field: MsToIwf. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdAckTimer                  = 0x03BA0000;	//< Commsdat field id for field: AckTimer. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdRetransmissionAttempts    = 0x03BB0000;	//< Commsdat field id for field: RetransmissionAttempts. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdResequencePeriod          = 0x03BC0000;	//< Commsdat field id for field: ResequencePeriod. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdV42Compression            = 0x03BD0000;	//< Commsdat field id for field: V42Compression. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdV42Codewords              = 0x03BE0000;	//< Commsdat field id for field: V42Codewords. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdV42MaxLength              = 0x03BF0000;	//< Commsdat field id for field: V42MaxLength. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdAsymmetry                 = 0x03C00000;	//< Commsdat field id for field: Asymmetry. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdUserInitUpgrade           = 0x03C10000;	//< Commsdat field id for field: UserInitUpgrade. Field is declared in the DialOutISP table.
	const TMDBElementId KCDTIdUseEdge                   = 0x03C20000;	//< Commsdat field id for field: UseEdge. Field is declared in the DialOutISP table.

	#define                    KCDTypeNameDescription                 _S("Description")
	#define                    KCDTypeNameType                        _S("Type")
	#define                    KCDTypeNameDefaultTelNum               _S("DefaultTelNum")
	#define                    KCDTypeNameDialResolution              _S("DialResolution")
	#define                    KCDTypeNameUseLoginScript              _S("UseLoginScript")
	#define                    KCDTypeNameLoginScript                 _S("LoginScript")
	#define                    KCDTypeNamePromptForLogin              _S("PromptForLogin")
	#define                    KCDTypeNameLoginName                   _S("LoginName")
	#define                    KCDTypeNameLoginPass                   _S("LoginPass")
	#define                    KCDTypeNameDisplayPct                  _S("DisplayPCT")
	#define                    KCDTypeNameIfParams                    _S("IfParams")
	#define                    KCDTypeNameIfNetworks                  _S("IfNetworks")
	#define                    KCDTypeNameIfPromptForAuth             _S("IfPromptForAuth")
	#define                    KCDTypeNameIfAuthName                  _S("IfAuthName")
	#define                    KCDTypeNameIfAuthPass                  _S("IfAuthPass")
	#define                    KCDTypeNameIfAuthRetries               _S("AuthRetries")
	#define                    KCDTypeNameIfCallbackEnabled           _S("IfCallbackEnabled")
	#define                    KCDTypeNameIfCallbackType              _S("IfCallbackType")
	#define                    KCDTypeNameIfCallbackInfo              _S("IfCallbackInfo")
	#define                    KCDTypeNameCallbackTimeout             _S("CallbackTimeout")
	#define                    KCDTypeNameIfServerMode                _S("IfServerMode")
	#define                    KCDTypeNameIpAddrFromServer            _S("IpAddrFromServer")
	#define                    KCDTypeNameIpAddr                      _S("IpAddr")
	#define                    KCDTypeNameIpNetMask                   _S("IpNetMask")
	#define                    KCDTypeNameIpGateway                   _S("IpGateway")
	#define                    KCDTypeNameIpDNSAddrFromServer         _S("IpDNSAddrFromServer")
	#define                    KCDTypeNameIpNameServer1               _S("IpNameServer1")
	#define                    KCDTypeNameIpNameServer2               _S("IpNameServer2")
	#define                    KCDTypeNameIp6DNSAddrFromServer        _S("Ip6DnsAddrFromServer")
	#define                    KCDTypeNameIp6NameServer1              _S("Ip6NameServer1")
	#define                    KCDTypeNameIp6NameServer2              _S("Ip6NameServer2")
	#define                    KCDTypeNameIpAddrLeaseValidFrom        _S("IpAddrLeaseValidFrom")
	#define                    KCDTypeNameIpAddrLeaseValidTo          _S("IpAddrLeaseValidTo")
	#define                    KCDTypeNameConfigDaemonManagerName     _S("ConfigDaemonManagerName")
	#define                    KCDTypeNameConfigDaemonName            _S("ConfigDaemonName")
	#define                    KCDTypeNameEnableIpHeaderComp          _S("EnableIPHeaderComp")
	#define                    KCDTypeNameEnableLcpExtensions         _S("EnableLCPExtension")
	#define                    KCDTypeNameDisablePlainTextAuth        _S("DisablePlainTextAuth")
	#define                    KCDTypeNameEnableSwComp                _S("EnableSWComp")
	#define                    KCDTypeNameBearerName                  _S("BearerName")
	#define                    KCDTypeNameBearerSpeed                 _S("BearerSpeed")
	#define                    KCDTypeNameBearerCe                    _S("BearerCE")
	#define                    KCDTypeNameInitString                  _S("InitString")
	#define	                   KCDTypeNameISPBearerType		          _S("BearerType")
	#define                    KCDTypeNameChannelCoding               _S("ChannelCoding")
	#define                    KCDTypeNameAiur                        _S("Aiur")
	#define                    KCDTypeNameRequestedTimeSlots          _S("RequestedTimeSlots")
	#define                    KCDTypeNameMaximumTimeSlots            _S("MaximumTimeSlots")
	#define                    KCDTypeNameBearerService               _S("BearerService")
	#define                    KCDTypeNameBearerProtocol              _S("BearerProtocol")
	#define                    KCDTypeNameRlpVersion                  _S("RlpVersion")
	#define                    KCDTypeNameIwfToMs                     _S("IwfToMs")
	#define                    KCDTypeNameMsToIwf                     _S("MsToIwf")
	#define                    KCDTypeNameAckTimer                    _S("AckTimer")
	#define                    KCDTypeNameRetransmissionAttempts      _S("RetransmissionAttempts")
	#define                    KCDTypeNameResequencePeriod            _S("ResequencePeriod")
	#define                    KCDTypeNameV42Compression              _S("V42Compression")
	#define                    KCDTypeNameV42Codewords                _S("V42Codewords")
	#define                    KCDTypeNameV42MaxLength                _S("V42MaxLength")
	#define                    KCDTypeNameAsymmetry                   _S("Asymmetry")
	#define                    KCDTypeNameUserInitUpgrade             _S("UserInitUpgrade")
	#define                    KCDTypeNameUseEdge                     _S("UseEdge")







//
// 040/ DIAL IN ISP RECORD
//


	// FieldTIds
	//
	const TMDBElementId KCDTIdDINDescription              = 0x04030000;	//< Commsdat field id for field: Description. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINType                     = 0x04040000;	//< Commsdat field id for field: Type. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINUseLoginScript           = 0x04050000;	//< Commsdat field id for field: UseLoginScript. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINLoginScript              = 0x04060000;	//< Commsdat field id for field: LoginScript. Field is declared in the DialnISP table.
	//
	const TMDBElementId KCDTIdDINAuthentication           = 0x04070000;	//< Commsdat field id for field: Authentication. Field is declared in the DialnISP table.
	//
	const TMDBElementId KCDTIdDINIfAuthRetries            = 0x04080000;	//< Commsdat field id for field: IfAuthRetries. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINIfParams                 = 0x04090000;	//< Commsdat field id for field: IfParams. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINIfNetworks               = 0x040A0000;	//< Commsdat field id for field: IfNetworks. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINIpAddrFromServer         = 0x040B0000;	//< Commsdat field id for field: IpAddrFromServer. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINIpAddr                   = 0x040C0000;	//< Commsdat field id for field: IpAddr. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINIpNetMask                = 0x040D0000;	//< Commsdat field id for field: IpNetMask. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINIpGateway                = 0x040E0000;	//< Commsdat field id for field: IpGateway. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINIpDNSAddrFromServer      = 0x040F0000;	//< Commsdat field id for field: IpDNSAddrFromServer. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINIpNameServer1            = 0x04100000;	//< Commsdat field id for field: IpNameServer1. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINIpNameServer2            = 0x04110000;	//< Commsdat field id for field: IpNameServer2. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINIp6DNSAddrFromServer     = 0x04120000;	//< Commsdat field id for field: Ip6DNSAddrFromServer. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINIp6NameServer1           = 0x04130000;	//< Commsdat field id for field: Ip6NameServer1. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINIp6NameServer2           = 0x04140000;	//< Commsdat field id for field: Ip6NameServer2. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINEnableIpHeaderComp       = 0x04150000;	//< Commsdat field id for field: EnableIpHeaderComp. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINEnableLCPExtensions      = 0x04160000;	//< Commsdat field id for field: EnableLCPExtensions. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINDisablePlainTextAuth     = 0x04170000;	//< Commsdat field id for field: DisablePlainTextAuth. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINEnableSwComp             = 0x04180000;	//< Commsdat field id for field: EnableSwComp. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINBearerName               = 0x04190000;	//< Commsdat field id for field: BearerName. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINBearerSpeed              = 0x041A0000;	//< Commsdat field id for field: BearerSpeed. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINBearerCe                 = 0x041B0000;	//< Commsdat field id for field: BearerCE. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINInitString               = 0x041C0000;	//< Commsdat field id for field: InitString. Field is declared in the DialnISP table.
	const TMDBElementId KCDTIdDINUseEdge                  = 0x041D0000;	//< Commsdat field id for field: UseEdge. Field is declared in the DialnISP table.
//


	#define	KCDTypeNameAuthentication		               _S("Authentication")




//
//  048/  LAN Service Record
//
//
	// FieldTIds
	const TMDBElementId KCDTIdLANIfNetworks 					= 0x04830000;	//< Commsdat field id for field: IfNetworks. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANIpNetMask 					    = 0x04840000;	//< Commsdat field id for field: IpNetMask. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANIpGateway 					    = 0x04850000;	//< Commsdat field id for field: IpGateway. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANIpAddrFromServer 				= 0x04860000;	//< Commsdat field id for field: IpAddrFromServer. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANIpAddr 						= 0x04870000;	//< Commsdat field id for field: IpAddr. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANIpDNSAddrFromServer 			= 0x04880000;	//< Commsdat field id for field: IpDNSAddrFromServer. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANIpNameServer1 				    = 0x04890000;	//< Commsdat field id for field: IpNameServer1. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANIpNameServer2 				    = 0x048A0000;	//< Commsdat field id for field: IpNameServer2. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANIp6DNSAddrFromServer			= 0x048B0000;	//< Commsdat field id for field: Ip6DNSAddrFromServer. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANIp6NameServer1				    = 0x048C0000;	//< Commsdat field id for field: Ip6NameServer1. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANIp6NameServer2 				= 0x048D0000;	//< Commsdat field id for field: Ip6NameServer2. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANIpAddrLeaseValidFrom			= 0x048E0000;	//< Commsdat field id for field: IpAddrLeaseValidFrom. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANIpAddrLeaseValidTo 			= 0x048F0000;	//< Commsdat field id for field: IpAddrLeaseValidTo. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANConfigDaemonManagerName 		= 0x04900000;	//< Commsdat field id for field: ConfigDaemonManagerName. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANConfigDaemonName 				= 0x04910000;	//< Commsdat field id for field: ConfigDaemonName. Field is declared in the LanService table.
	//
	const TMDBElementId KCDTIdLANServiceExtensionTableName 		= 0x04920000;	//< Commsdat field id for field: LANServiceExtensionTableName. Field is declared in the LanService table.
	const TMDBElementId KCDTIdLANServiceExtensionTableRecordId 	= 0x04930000;	//< Commsdat field id for field: LANServiceExtensionTableRecordId. Field is declared in the LanService table.


	#define	KCDTypeNameServiceExtensionTableName				_S("LanServiceExtensionTableName")
	#define	KCDTypeNameServiceExtensionTableRecordId		    _S("LanServiceExtensionTableRecordId")
	#define	KCDTypeNameServiceIfExternIpConfigAlwaysRejectAuth	_S("ServiceIfExternIpConfigAlwaysRejectAuth")




//
// 058/  VPN SERVICE RECORD
//


	// FieldTIds
	const TMDBElementId KCDTIdVPNPolicyName 		= 0x05830000; ///< Commsdat field id for field: Policy. Policy name. Field is declared in the VPNService table.
	const TMDBElementId KCDTIdVPNIAPRecord 			= 0x05840000; ///< Commsdat field id for field: HomeIAP. Link to a IAP record. Field is declared in the VPNService table.
	const TMDBElementId KCDTIdVPNSNAPRecord      = 0x05860000; ///< Commsdat field id for field: SNAPIAP. Link to a snap iap record. Field is declared in the VPNService table.
	const TMDBElementId KCDTIdVPNNetwork 			= 0x05850000; ///< Commsdat field id for field: HomeNetwork. Link to a Network record. Field is declared in the VPNService table.


	#define	KCDTypeNameVPNPolicyName		       _S("Policy")
	#define	KCDTypeNameVPNIAPRecord		           _S("HomeIAP")
    #define KCDTypeNameVPNSNAPRecord                   _S("HomeSNAP")
	#define	KCDTypeNameVPNNetwork		           _S("HomeNetwork")




//
// 0F0/ GPRS QoS R99 Parameters
//

//VR99/R4 QoS parameters
    const TMDBElementId KCDTIdWCDMAReqTrafficClass                     = 0x0F030000;	//< Commsdat field id for field: ReqTrafficClass. Field is declared in the UmtsR99QoSAndOn table.            
    const TMDBElementId KCDTIdWCDMAMinTrafficClass                     = 0x0F040000;	//< Commsdat field id for field: MinTrafficClass. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqDeliveryOrder                    = 0x0F050000;	//< Commsdat field id for field: ReqDeliveryOrder . Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAMinDeliveryOrder                    = 0x0F060000;	//< Commsdat field id for field: MinDeliveryOrder . Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqDeliverErroneousSDU              = 0x0F070000;	//< Commsdat field id for field: ReqDeliverErroneousSDU. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAMinDeliverErroneousSDU              = 0x0F080000;	//< Commsdat field id for field: MinDeliverErroneousSDU. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqMaxSDUSize                       = 0x0F090000;	//< Commsdat field id for field: ReqMaxSDUSize. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAMinAcceptableMaxSDUSize             = 0x0F0A0000;	//< Commsdat field id for field: MinAcceptableMaxSDUSize. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqMaxUplinkRate                    = 0x0F0B0000;	//< Commsdat field id for field: ReqMaxUplinkRate. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqMinUplinkRate                    = 0x0F0C0000;	//< Commsdat field id for field: ReqMinUplinkRate. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqMaxDownlinkRate                  = 0x0F0D0000;	//< Commsdat field id for field: ReqMaxDownlinkRate . Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqMinDownlinkRate                  = 0x0F0E0000;	//< Commsdat field id for field: ReqMinDownlinkRate. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqBER                              = 0x0F0F0000;	//< Commsdat field id for field: ReqBER. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAMaxBER                              = 0x0F100000;	//< Commsdat field id for field: MaxBER. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqSDUErrorRatio                    = 0x0F110000;	//< Commsdat field id for field: ReqSDUErrorRatio. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAMaxSDUErrorRatio                    = 0x0F120000;	//< Commsdat field id for field: MaxSDUErrorRatio. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqTrafficHandlingPriority          = 0x0F130000;	//< Commsdat field id for field: ReqTrafficHandlingPriority. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAMinTrafficHandlingPriority          = 0x0F140000;	//< Commsdat field id for field: MinTrafficHandlingPriority. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqTransferDelay                    = 0x0F150000;	//< Commsdat field id for field: ReqTransferDelay. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAMaxTransferDelay                    = 0x0F160000;	//< Commsdat field id for field: MaxTransferDelay. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqGuaranteedUplinkRate             = 0x0F170000;	//< Commsdat field id for field: ReqGuaranteedUplinkRate. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAMinGuaranteedUplinkRate             = 0x0F180000;	//< Commsdat field id for field: MinGuaranteedUplinkRate. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAReqGuaranteedDownlinkRate           = 0x0F190000;	//< Commsdat field id for field: ReqGuaranteedDownlinkRate. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAMinGuaranteedDownlinkRate           = 0x0F1A0000;	//< Commsdat field id for field: MinGuaranteedDownlinkRate. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMASignallingIndication                = 0x0F1B0000;	//< Commsdat field id for field: SignallingIndication. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMAImCmSignallingIndication            = 0x0F1C0000;	//< Commsdat field id for field: ImCnSignallingIndication. Field is declared in the UmtsR99QoSAndOn table.
    const TMDBElementId KCDTIdWCDMASourceStatisticsDescriptor          = 0x0F1D0000;	//< Commsdat field id for field: SourceStatisticsDescriptor. Field is declared in the UmtsR99QoSAndOn table.

    //Field names
    #define                    KCDTypeNameGPRSReqTrafficClass            _S("ReqTrafficClass")
    #define                    KCDTypeNameGPRSMinTrafficClass            _S("MinTrafficClass")
    #define                    KCDTypeNameGPRSReqDeliveryOrder           _S("ReqDeliveryOrder")
    #define                    KCDTypeNameGPRSMinDeliveryOrder           _S("MinDeliveryOrder")
    #define                    KCDTypeNameGPRSReqDeliverErroneousSDU     _S("ReqDeliverErroneousSDU")
    #define                    KCDTypeNameGPRSMinDeliverErroneousSDU     _S("MinDeliverErroneousSDU")
    #define                    KCDTypeNameGPRSReqMaxSDUSize              _S("ReqMaxSDUSize")
    #define                    KCDTypeNameGPRSMinAcceptableMaxSDUSize    _S("MinAcceptableMaxSDUSize")
    #define                    KCDTypeNameGPRSReqMaxUplinkRate           _S("ReqMaxUplinkRate")
    #define                    KCDTypeNameGPRSReqMinUplinkRate           _S("ReqMinUplinkRate")
    #define                    KCDTypeNameGPRSReqMaxDownlinkRate         _S("ReqMaxDownlinkRate")
    #define                    KCDTypeNameGPRSReqMinDownlinkRate         _S("ReqMinDownlinkRate")
    #define                    KCDTypeNameGPRSReqBER                     _S("ReqBER")
    #define                    KCDTypeNameGPRSMaxBER                     _S("MaxBER")
    #define                    KCDTypeNameGPRSReqSDUErrorRatio           _S("ReqSDUErrorRatio")
    #define                    KCDTypeNameGPRSMaxSDUErrorRatio           _S("MaxSDUErrorRatio")
    #define                    KCDTypeNameGPRSReqTrafficHandlingPriority _S("ReqTrafficHandlingPriority")
    #define                    KCDTypeNameGPRSMinTrafficHandlingPriority _S("MinTrafficHandlingPriority")
    #define                    KCDTypeNameGPRSReqTransferDelay           _S("ReqTransferDelay")
    #define                    KCDTypeNameGPRSMaxTransferDelay           _S("MaxTransferDelay")
    #define                    KCDTypeNameGPRSReqGuaranteedUplinkRate    _S("ReqGuaranteedUplinkRate")
    #define                    KCDTypeNameGPRSMinGuaranteedUplinkRate    _S("MinGuaranteedUplinkRat")
    #define                    KCDTypeNameGPRSReqGuaranteedDownlinkRate  _S("ReqGuaranteedDownlinkRate")
    #define                    KCDTypeNameGPRSMinGuaranteedDownlinkRate  _S("MinGuaranteedDownlinkRate")
    #define                    KCDTypeNameGPRSSignallingIndication       _S("SignallingIndication")
    #define                    KCDTypeNameGPRSImCnSignallingIndication   _S("ImCnSignallingIndication")
    #define                    KCDTypeNameGPRSSourceStatisticsDescriptor _S("SourceStatisticsDescriptor")


//
// 068/ WCDMA Packet Service Record
//

	// FieldTIds
	const TMDBElementId KCDTIdAPN						    	 = 0x00030000;	//< Commsdat field id for field: APN. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMPDPType 						 = 0x00040000;	//< Commsdat field id for field: PDPType. Field is declared in the WCDMA Packet Service table.	
	const TMDBElementId KCDTIdWCDMAPDPAddress					 = 0x00050000;	//< Commsdat field id for field: PDPAddress. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdReqPrecedence						 = 0x00060000;	//< Commsdat field id for field: ReqPrecedence. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdReqDelay							 = 0x00070000;	//< Commsdat field id for field: ReqDelay. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdReqReliability					 = 0x00080000;	//< Commsdat field id for field: ReqReliability. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdReqPeakThroughput                  = 0x00090000;	//< Commsdat field id for field: ReqPeakThroughput. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdReqMeanThroughput			         = 0x000A0000;	//< Commsdat field id for field: ReqMeanThroughput. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdMinPrecedence						 = 0x000B0000;	//< Commsdat field id for field: MinPrecedence. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdMinDelay							 = 0x000C0000;	//< Commsdat field id for field: MinDelay. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdMinReliability					 = 0x000D0000;	//< Commsdat field id for field: MinReliability. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdMinPeakThroughput				     = 0x000E0000;	//< Commsdat field id for field: MinPeakThroughput. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdMinMeanThroughput				     = 0x000F0000;	//< Commsdat field id for field: MinMeanThroughput. Field is declared in the WCDMA Packet Service table.
	//
	const TMDBElementId KCDTIdWCDMADataCompression				 = 0x00100000;	//< Commsdat field id for field: DataCompression. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAHeaderCompression  			 = 0x00110000;	//< Commsdat field id for field: HeaderCompression. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAUseEdge				         = 0x00120000;	//< Commsdat field id for field: UseEdge. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAAnonymousAccess	    		 = 0x00130000;	//< Commsdat field id for field: AnonymousAccess. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIfParams						 = 0x00140000;	//< Commsdat field id for field: IfParams. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIfNetworks					 = 0x00150000;	//< Commsdat field id for field: IfNetworks. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIfPromptForAuth				 = 0x00160000;	//< Commsdat field id for field: IfPromptForAuth. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIfAuthName					 = 0x00170000;	//< Commsdat field id for field: IfAuthName. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIfAuthPass					 = 0x00180000;	//< Commsdat field id for field: IfAuthPass. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIfAuthRetries				 = 0x00190000;	//< Commsdat field id for field: IfAuthRetries. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIPNetMask					 = 0x001A0000;	//< Commsdat field id for field: IPNetMask. Field is declared in the WCDMA Packet Service table.	
	const TMDBElementId KCDTIdWCDMAIPGateway					 = 0x001B0000;	//< Commsdat field id for field: IPGateway. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIPAddrFromServer				 = 0x001C0000;	//< Commsdat field id for field: IPAddrFromServer. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIPAddr						 = 0x001D0000;	//< Commsdat field id for field: IPAddr. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIPDNSAddrFromServer			 = 0x001E0000;	//< Commsdat field id for field: IPDNSAddrFromServer. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIPNameServer1				 = 0x001F0000;	//< Commsdat field id for field: IPNameServer1. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIPNameServer2				 = 0x00200000;	//< Commsdat field id for field: IPNameServer2. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIP6DNSAddrFromServer		     = 0x00210000;	//< Commsdat field id for field: IP6DNSAddrFromServer. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIP6NameServer1				 = 0x00220000;	//< Commsdat field id for field: IP6NameServer1. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIP6NameServer2				 = 0x00230000;	//< Commsdat field id for field: IP6NameServer2. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIPAddrLeaseValidFrom			 = 0x00240000;	//< Commsdat field id for field: IPAddrLeaseValidFrom. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAIPAddrLeaseValidTo			 = 0x00250000;	//< Commsdat field id for field: IPAddrLeaseValidTo. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAConfigDaemonManagerName	     = 0x00260000;	//< Commsdat field id for field: ConfigDaemonManagerName. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAConfigDaemonName				 = 0x00270000;	//< Commsdat field id for field: ConfigDaemonName. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMAEnableLCPExtensions			 = 0x00280000;	//< Commsdat field id for field: EnableLCPExtensions. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdWCDMADisablePlainTextAuth			 = 0x00290000;	//< Commsdat field id for field: DisablePlainTextAuth. Field is declared in the WCDMA Packet Service table.
	//
	const TMDBElementId KCDTIdAPType					  		 		= 0x002A0000;	//< Commsdat field id for field: APType. Field is declared in the WCDMA Packet Service table.
	const TMDBElementId KCDTIdQOSWarningTimeOut				    		= 0x002B0000;	//< Commsdat field id for field: QOSWarningTimeOut. Field is declared in the WCDMA Packet Service table.
	//
	const TMDBElementId KCDTIdUmtsR99QoSAndOn					= 0x002C0000;	//< Commsdat field id for field: UmtsR99QoSAndOn. Field is declared in the WCDMA Packet Service table.

	const TMDBElementId KCDTIdR5DataCompression					 = 0x002D0000;
	const TMDBElementId KCDTIdR5HeaderCompression				 = 0x002E0000;
	const TMDBElementId KCDTIdPacketFlowIdentifier				 = 0x002F0000;
	const TMDBElementId KCDTIdUmtsGprsRelease					 = 0x00300000;

	// Field Names
	#define KCDTypeNameGPRSAPN                        _S("APN")
	#define KCDTypeNameGPRSPDPType                    _S("PDPType")
	#define KCDTypeNameGPRSPDPAddress                 _S("PDPAddress")
	#define KCDTypeNameGPRSReqPrecedence              _S("ReqPrecedence")
	#define KCDTypeNameGPRSReqDelay                   _S("ReqDelay")
	#define KCDTypeNameGPRSReqReliability             _S("ReqReliability")
	#define KCDTypeNameGPRSReqPeakThroughput          _S("ReqPeakThroughput")
	#define KCDTypeNameGPRSReqMeanThroughput          _S("ReqMeanThroughput")
	#define KCDTypeNameGPRSMinPrecedence              _S("MinPrecedence")
	#define KCDTypeNameGPRSMinDelay                   _S("MinDelay")
	#define KCDTypeNameGPRSMinReliability             _S("MinReliability")
	#define KCDTypeNameGPRSMinPeakThroughput          _S("MinPeakThroughput")
	#define KCDTypeNameGPRSMinMeanThroughput          _S("MinMeanThroughput")
	#define KCDTypeNameGPRSDataCompression            _S("DataCompression")
	#define KCDTypeNameGPRSHeaderCompression          _S("HeaderCompression")
	#define KCDTypeNameGPRSUseEdge                    _S("GprsUseEdge")
	#define KCDTypeNameGPRSAnonymousAccess            _S("AnonymousAccess")
	#define KCDTypeNameGPRSIfParams                   _S("IfParams")
	#define KCDTypeNameGPRSIfPromptForAuth            _S("IfPromptForAuth")
	#define KCDTypeNameGPRSIfAuthName                 _S("IfAuthName")
	#define KCDTypeNameGPRSIfAuthPass                 _S("IfAuthPass")
	#define KCDTypeNameGPRSIfAuthRetries              _S("AuthRetries")
	#define KCDTypeNameGPRSIPNetMask                  _S("IpNetMask")
	#define KCDTypeNameGPRSIPGateway                  _S("IpGateway")
	#define KCDTypeNameGPRSIPAddrFromServer           _S("IpAddrFromServer")
	#define KCDTypeNameGPRSIPAddr                     _S("IpAddr")
	#define KCDTypeNameGPRSIPDNSAddrFromServer        _S("IpDNSAddrFromServer")
	#define KCDTypeNameGPRSIPNameServer1              _S("IpNameServer1")
	#define KCDTypeNameGPRSIPNameServer2              _S("IpNameServer2")
	#define KCDTypeNameGPRSIP6DNSAddrFromServer       _S("Ip6DNSAddrFromServer")
	#define KCDTypeNameGPRSIP6NameServer1             _S("Ip6NameServer1")
	#define KCDTypeNameGPRSIP6NameServer2             _S("Ip6NameServer2")
	#define KCDTypeNameGPRSIPAddrLeaseValidFrom       _S("IpAddrLeaseValidFrom")
	#define KCDTypeNameGPRSIPAddrLeaseValidTo         _S("IpAddrLeaseValidTo")
	#define KCDTypeNameGPRSConfigDaemonManagerName    _S("ConfigDaemonManagerName")
	#define KCDTypeNameGPRSConfigDaemonName           _S("ConfigDaemonName")
	#define KCDTypeNameGPRSEnableLCPExtensions        _S("EnableLCPExtension")
	#define KCDTypeNameGPRSDisablePlainTextAuth       _S("DisablePlainTextAuth")
	#define KCDTypeNameGPRSAPType                     _S("GprsAccessPointType")
	#define KCDTypeNameGPRSQOSWarningTimeOut          _S("QosWarningTimeout")
	#define KCDTypeNameUmtsR99QoSAndOn                _S("UmtsR99QoSAndOn")
	#define KCDTypeNameR5DataCompression			  _S("R5DataCompression")
	#define KCDTypeNameR5HeaderCompression			  _S("R5HeaderCompression")
	#define KCDTypeNamePacketFlowIdentifier           _S("PacketFlowIdentifier")
	#define KCDTypeNameUmtsGprsRelease                _S("UmtsGprsRelease")	


//
// 0D8/  PAN Service Extensions Table
//

	// FieldTIds
	const TMDBElementId KCDTIdLocalRole						    	 = 0x0D830000;	//< Commsdat field id for field: LocalRole. Field is declared in the PAN Service Extensions table.
	const TMDBElementId KCDTIdPeerRole						    	 = 0x0D840000;	//< Commsdat field id for field: PeerRole. Field is declared in the PAN Service Extensions table.
	const TMDBElementId KCDTIdPeerMACAddresses				    	 = 0x0D850000;	//< Commsdat field id for field: PeerMACAddresses. Field is declared in the PAN Service Extensions table.
	const TMDBElementId KCDTIdPromptForRemoteDevices			   	 = 0x0D860000;	//< Commsdat field id for field: PromptForRemoteDevices. Field is declared in the PAN Service Extensions table.
	const TMDBElementId KCDTIdDisableSdpQuery				    	 = 0x0D870000;	//< Commsdat field id for field: PromptForRemoteDevices. Field is declared in the PAN Service Extensions table.
	const TMDBElementId KCDTIdAllowIncoming					    	 = 0x0D880000;	//< Commsdat field id for field: DisableSdpQuery. Field is declared in the PAN Service Extensions table.
	const TMDBElementId KCDTIdPromptIfMACListFails				   	 = 0x0D890000;	//< Commsdat field id for field: PromptIfMACListFails. Field is declared in the PAN Service Extensions table.
#ifdef SYMBIAN_NETWORKING_DHCPSERVER	
	const TMDBElementId KCDTIdNapServiceEnabled				   	 	 = 0x0D8A0000;	//< Commsdat field id for field: NapServiceEnabled. Field is declared in the PAN Service Extensions table.
#endif // SYMBIAN_NETWORKING_DHCPSERVER	

	// Field Names
	#define                    KCDTypeNameLocalRole          		_S("LocalRole")
	#define                    KCDTypeNamePeerRole					_S("PeerRole")
	#define                    KCDTypeNamePeerMACAddresses			_S("PeerMACAddresses")
	#define                    KCDTypeNamePromptForRemoteDevices	_S("PromptForRemoteDevices")
	#define                    KCDTypeNameDisableSdpQuery			_S("DisableSdpQuery")
	#define                    KCDTypeNameAllowIncoming				_S("AllowIncomingConnections")
	#define                    KCDTypeNamePromptIfMACListFails		_S("PromptIfMACListFails")
#ifdef SYMBIAN_NETWORKING_DHCPSERVER 	
	#define					   KCDTypeNameNapServiceEnabled	 	    _S("NapServiceEnabled")
#endif // SYMBIAN_NETWORKING_DHCPSERVER


//
// 078/ Default WCDMA Settings Table
//

	const TMDBElementId KCDTIdDefaultGPRSUsage						     = 0x07830000;	//< Commsdat field id for field: Usage. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSAPN						     = 0x07840000;	//< Commsdat field id for field: APN. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSPDPType					     = 0x07850000;	//< Commsdat field id for field: PDPType. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSPDPAddress				    	 = 0x07860000;	//< Commsdat field id for field: PDPAddress. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSPrecedence				    	 = 0x07870000;	//< Commsdat field id for field: Precedence. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSDelay						     = 0x07880000;	//< Commsdat field id for field: Delay. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSReliability				     = 0x07890000;	//< Commsdat field id for field: Reliability. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSPeakThroughput			    	 = 0x078A0000;	//< Commsdat field id for field: PeakThroughput. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSMeanThroughput			    	 = 0x078B0000;	//< Commsdat field id for field: MeanThroughput. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSMinPrecedence				     = 0x078C0000;	//< Commsdat field id for field: MinPrecedence. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSMinDelay					     = 0x078D0000;	//< Commsdat field id for field: MinDelay. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSMinReliability			    	 = 0x078E0000;	//< Commsdat field id for field: MinReliability. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSMinPeakThroughput			     = 0x078F0000;	//< Commsdat field id for field: MinPeakThroughput. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSMinMeanThroughput			     = 0x07900000;	//< Commsdat field id for field: MinMeanThroughput. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSDataCompression			     = 0x07910000;	//< Commsdat field id for field: DataCompression. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSHeaderCompression			     = 0x07920000;	//< Commsdat field id for field: HeaderCompression. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSUseEdge					     = 0x07930000;	//< Commsdat field id for field: UseEdge. Field is declared in the Default WCDMA Settings table.
	const TMDBElementId KCDTIdDefaultGPRSAnonymousAccess			     = 0x07940000;	//< Commsdat field id for field: AnonymousAccess. Field is declared in the Default WCDMA Settings table.


	#define KCDTypeNameUsage						     _S("Usage")
	#define KCDTypeNameAPN						    	 _S("APN")
	#define KCDTypeNamePDPType					    	 _S("PDPType")
	#define KCDTypeNamePDPAddress				    	 _S("PDPAddress")
	#define KCDTypeNamePrecedence				    	 _S("Precedence")
	#define KCDTypeNameDelay						     _S("Delay")
	#define KCDTypeNameReliability				    	 _S("Reliability")
	#define KCDTypeNamePeakThroughput			    	 _S("PeakThroughput")
	#define KCDTypeNameMeanThroughput			    	 _S("MeanThroughput")
	#define KCDTypeNameMinPrecedence				     _S("MinPrecedence")
	#define KCDTypeNameMinDelay					    	 _S("MinDelay")
	#define KCDTypeNameMinReliability			    	 _S("MinReliability")
	#define KCDTypeNameMinPeakThroughput			     _S("MinPeakThroughput")
	#define KCDTypeNameMinMeanThroughput			     _S("MinMeanThroughput")
	#define KCDTypeNameDataCompression			    	 _S("DataCompression")
	#define KCDTypeNameHeaderCompression			     _S("HeaderCompression")
	#define KCDTypeNameAnonymousAccess			    	 _S("AnonymousAccess")




//
// 080/  MODEM BEARER RECORD
//

	const TMDBElementId KCDTIdNifName							  = 0x08030000;	//< Commsdat field id for field: IfName. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdPortName							  = 0x08040000;	//< Commsdat field id for field: PortName. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdTsyName							  = 0x08050000;	//< Commsdat field id for field: TSYName. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdCsyName							  = 0x08060000;	//< Commsdat field id for field: CSYName. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdDataBits							  = 0x08070000;	//< Commsdat field id for field: DataBits. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdStopBits							  = 0x08080000;	//< Commsdat field id for field: StopBits. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdParity							  = 0x08090000;	//< Commsdat field id for field: Parity. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdRate								  = 0x080A0000;	//< Commsdat field id for field: Rate. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdHandshaking						  = 0x080B0000;	//< Commsdat field id for field: Handshaking. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdSpecialRate						  = 0x080C0000;	//< Commsdat field id for field: SpecialRate. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdXonChar							  = 0x080D0000;	//< Commsdat field id for field: XonChar. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdXoffChar							  = 0x080E0000;	//< Commsdat field id for field: XoffChar. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdFaxClassPref						  = 0x080F0000;	//< Commsdat field id for field: FaxClassPref. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdSpeakerPref						  = 0x08100000;	//< Commsdat field id for field: SpeakerPref. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdSpeakerVolPref					  = 0x08110000;	//< Commsdat field id for field: SpeakerVolPref. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdModemInitString					  = 0x08120000;	//< Commsdat field id for field: ModemInitString. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdDataInitString					  = 0x08130000;	//< Commsdat field id for field: DataInitString. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdFaxInitString						  = 0x08140000;	//< Commsdat field id for field: FaxInitString. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdIspInitString						  = 0x08150000;	//< Commsdat field id for field: IspInitString	. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdDialPauseLength					  = 0x08160000;	//< Commsdat field id for field: DialPauseLength. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdCarrierTimeOut					  = 0x08170000;	//< Commsdat field id for field: CarrierTimeOut. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdAutoAnswerRingCount				  = 0x08180000;	//< Commsdat field id for field: AutoAnswerRingCount. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdSpeakerVolControlLow				  = 0x08190000;	//< Commsdat field id for field: SpeakerVolControlLow. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdSpeakerVolControlMedium			  = 0x081A0000;	//< Commsdat field id for field: SpeakerVolControlMedium. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdSpeakerVolControlHigh				  = 0x081B0000;	//< Commsdat field id for field: SpeakerVolControlHigh	. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdSpeakerAlwaysOff					  = 0x081C0000;	//< Commsdat field id for field: SpeakerAlwaysOff. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdSpeakerOnUntilCarrier				  = 0x081D0000;	//< Commsdat field id for field: SpeakerOnUntilCarrier. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdSpeakerAlwaysOn					  = 0x081E0000;	//< Commsdat field id for field: SpeakerAlwaysOn. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdSpeakerOnAfterUntilCarrier		  = 0x081F0000;	//< Commsdat field id for field: SpeakerOnAfterUntilCarrier. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdDialToneWaitModifier				  = 0x08200000;	//< Commsdat field id for field: DialToneWaitModifier. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdCallProgress1						  = 0x08210000;	//< Commsdat field id for field: CallProgress1. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdCallProgress2						  = 0x08220000;	//< Commsdat field id for field: CallProgress2. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdCallProgress3					 	  = 0x08230000;	//< Commsdat field id for field: CallProgress3. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdCallProgress4					 	  = 0x08240000;	//< Commsdat field id for field: CallProgress4. Field is declared in the ModemBearer table.	
	const TMDBElementId KCDTIdEchoOff							  = 0x08250000;	//< Commsdat field id for field: EchoOff. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdVerboseText						  = 0x08260000;	//< Commsdat field id for field: VerboseText. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdQuietOff							  = 0x08270000;	//< Commsdat field id for field: QuietOff. Field is declared in the ModemBearer table.	
	const TMDBElementId KCDTIdQuietOn							  = 0x08280000;	//< Commsdat field id for field: QuietOn. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdDialCommandStateModifier			  = 0x08290000;	//< Commsdat field id for field: DialCommandStateModifier	. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdOnLine							  = 0x082A0000;	//< Commsdat field id for field: OnLine. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdResetConfiguration				  = 0x082B0000;	//< Commsdat field id for field: ResetConfiguration. Field is declared in the ModemBearer table.	
	const TMDBElementId KCDTIdReturnToFactoryDefs				  = 0x082C0000;	//< Commsdat field id for field: ReturnToFactoryDefs. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdDcdOnDuringLink					  = 0x082D0000;	//< Commsdat field id for field: DCDOnDuringLink. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdDtrHangUp							  = 0x082E0000;	//< Commsdat field id for field: DTRHangUp. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdDsrAlwaysOn						  = 0x082F0000;	//< Commsdat field id for field: DSRAlwaysOn. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdRtsCtsHandshake					  = 0x08300000;	//< Commsdat field id for field: RTSCTSHandshake. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdXonXoffHandshake					  = 0x08310000;	//< Commsdat field id for field: XonXoffHandshake. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdEscapeCharacter					  = 0x08320000;	//< Commsdat field id for field: EscapeCharacter. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdEscapeGuardPeriod					  = 0x08330000;	//< Commsdat field id for field: EscapeGuardPeriod. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdFaxClassInterrogate				  = 0x08340000;	//< Commsdat field id for field: FaxClassInterrogate. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdFaxClass							  = 0x08350000;	//< Commsdat field id for field: FaxClass. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdNoDialTone						  = 0x08360000;	//< Commsdat field id for field: NoDialTone. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdBusy								  = 0x08370000;	//< Commsdat field id for field: Busy. Field is declared in the ModemBearer table.	
	const TMDBElementId KCDTIdNoAnswer							  = 0x08380000;	//< Commsdat field id for field: NoAnswer. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdCarrier							  = 0x08390000;	//< Commsdat field id for field: Carrier. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdConnect							  = 0x083A0000;	//< Commsdat field id for field: Connect. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdCompressionClass5					  = 0x083B0000;	//< Commsdat field id for field: CompressionClass5. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdCompressionV42bis					  = 0x083C0000;	//< Commsdat field id for field: CompressionV42bis. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdCompressionNone					  = 0x083D0000;	//< Commsdat field id for field: CompressionNone. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdProtocolLapd						  = 0x083E0000;	//< Commsdat field id for field: ProtocolLAPD. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdProtocolAlt						  = 0x083F0000;	//< Commsdat field id for field: ProtocolALT. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdProtocolAltcellular				  = 0x08400000;	//< Commsdat field id for field: ProtocolALTCELLULAR. Field is declared in the ModemBearer table.	
	const TMDBElementId KCDTIdProtocolNone						  = 0x08410000;	//< Commsdat field id for field: ProtocolNone. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdMessageCentreNumber				  = 0x08420000;	//< Commsdat field id for field: MessageCentreNumber. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdMessageValidityPeriod				  = 0x08430000;	//< Commsdat field id for field: MessageValidityPeriod. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdMessageDeliveryReport				  = 0x08440000;	//< Commsdat field id for field: MessageDeliveryReport. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdMinSignalLevel					  = 0x08450000;	//< Commsdat field id for field: MinimumSignalLevel. Field is declared in the ModemBearer table.	
	const TMDBElementId KCDTIdCommRole							  = 0x08460000;	//< Commsdat field id for field: CommRole. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdControlChannelPortName			  = 0x08470000;	//< Commsdat field id for field: ControlChannelPortName. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdSirSettings					      = 0x08480000;	//< Commsdat field id for field: SirSettings. Field is declared in the ModemBearer table.
    const TMDBElementId KCDTIdLastSocketActivityTimeout			  = 0x08490000;	//< Commsdat field id for field: LastSocketActivityTimeout. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdLastSessionClosedTimeout			  = 0x084A0000;	//< Commsdat field id for field: LastSessionClosedTimeout. Field is declared in the ModemBearer table.	
	const TMDBElementId KCDTIdLastSocketClosedTimeout			  = 0x084B0000;	//< Commsdat field id for field: LastSocketClosedTimeout. Field is declared in the ModemBearer table.
	const TMDBElementId KCDTIdBCAStack							  = 0x084C0000;	//< Commsdat field id for field: BCAStack. Field is declared in the ModemBearer table.
	


	// Field Names
	#define	KCDTypeNameNifName								_S("IfName")
	#define	KCDTypeNamePortName								_S("PortName")
	#define	KCDTypeNameTsyName								_S("TSYName")
	#define	KCDTypeNameCsyName								_S("CSYName")
	#define	KCDTypeNameSocketActivityTimeout		        _S("SocketActivityTimeout")
	#define	KCDTypeNameSessionClosedTimeout		            _S("SessionClosedTimeout")
	#define	KCDTypeNameSocketClosedTimeout		            _S("SocketClosedTimeout")
	#define	KCDTypeNameDataBits								_S("DataBits")
	#define	KCDTypeNameStopBits								_S("StopBits")
	#define	KCDTypeNameParity								_S("Parity")
	#define	KCDTypeNameRate									_S("Rate")
	#define	KCDTypeNameHandShaking							_S("Handshaking")
	#define	KCDTypeNameSpecialRate							_S("SpecialRate")
	#define	KCDTypeNameXonChar								_S("XonChar")
	#define	KCDTypeNameXoffChar								_S("XoffChar")
	#define	KCDTypeNameFaxClassPref							_S("FaxClassPref")
	#define	KCDTypeNameSpeakerPref							_S("SpeakerPref")
	#define	KCDTypeNameSpeakerVolPref						_S("SpeakerVolPref")
	#define	KCDTypeNameModemInitString						_S("ModemInitString")
	#define	KCDTypeNameDataInitString						_S("DataInitString")
	#define	KCDTypeNameFaxInitString						_S("FaxInitString")
	#define	KCDTypeNameIspInitString						_S("IspInitString")
	#define	KCDTypeNameDialPauseLength						_S("DialPauseLength")
	#define	KCDTypeNameCarrierTimeout						_S("CarrierTimeout")
	#define	KCDTypeNameAutoAnswerRingCount		            _S("AutoAnswerRingCount")
	#define	KCDTypeNameSpeakerVolControlLow		            _S("SpeakerVolContorlLow")
	#define	KCDTypeNameSpeakerVolControlMedium		        _S("SpeakerVolControlMedium")
	#define	KCDTypeNameSpeakerVolControlHigh		        _S("SpeakerVolControlHigh")
	#define	KCDTypeNameSpeakerAlwaysOff		                _S("SpeakerAlwaysOff")
	#define	KCDTypeNameSpeakerOnUntilCarrier		        _S("SpeakerOnUntilCarrier")
	#define	KCDTypeNameSpeakerAlwaysOn		                _S("SpeakerAlwaysOn")
	#define	KCDTypeNameSpeakerOnAfterUntilCarrier		    _S("SpeakerOnAfterUntilCarrier")
	#define	KCDTypeNameDialToneWaitModifier		            _S("DialToneWaitModifier")
	#define	KCDTypeNameCallProgress1						_S("CallProgress1")
	#define	KCDTypeNameCallProgress2						_S("CallProgress2")
	#define	KCDTypeNameCallProgress3						_S("CallProgress3")
	#define	KCDTypeNameCallProgress4						_S("CallProgress4")
	#define	KCDTypeNameEchoOff								_S("EchoOff")
	#define	KCDTypeNameVerboseText							_S("VerboseText")
	#define	KCDTypeNameQuietOff								_S("QuietOff")
	#define	KCDTypeNameQuietOn								_S("QuietOn")
	#define	KCDTypeNameDialCommandStateModifier		        _S("DialCommandStateModifier")
	#define	KCDTypeNameOnLine								_S("OnLine")
	#define	KCDTypeNameResetConfiguration		            _S("ResetConfiguration")
	#define	KCDTypeNameReturnToFactoryDefs		            _S("ReturnToFactoryDefs")
	#define	KCDTypeNameDcdOnDuringLink						_S("DCDOnDuringLink")
	#define	KCDTypeNameDtrHangUp							_S("DTRHangUp")
	#define	KCDTypeNameDsrAlwaysOn							_S("DSRAlwaysOn")
	#define	KCDTypeNameRtsCtsHandshake						_S("RTSCTSHandshake")
	#define	KCDTypeNameXonXoffHandshake						_S("XonXoffHandshake")
	#define	KCDTypeNameEscapeCharacter						_S("EscapeCharacter")
	#define	KCDTypeNameEscapeGuardPeriod		            _S("EscapeGuardPeriod")
	#define	KCDTypeNameFaxClassInterrogate		            _S("FaxClassInterrogate")
	#define	KCDTypeNameFaxClass								_S("FaxClass")
	#define	KCDTypeNameNoDialTone							_S("NoDialTone")
	#define	KCDTypeNameBusy									_S("Busy")
	#define	KCDTypeNameNoAnswer								_S("NoAnswer")
	#define	KCDTypeNameCarrier								_S("Carrier")
	#define	KCDTypeNameConnect								_S("Connect")
	#define	KCDTypeNameCompressionClass5		            _S("CompressionClass5")
	#define	KCDTypeNameCompressionClassV42bis		        _S("CompressionV42bis")
	#define	KCDTypeNameCompressionNone						_S("CompressionNone")
	#define	KCDTypeNameProtocolLapd							_S("ProtocolLAPD")
	#define	KCDTypeNameProtocolAlt							_S("ProtocolALT")
	#define	KCDTypeNameProtocolAltcellular		            _S("ProtocolALTCELLULAR")
	#define	KCDTypeNameProtocolNone							_S("ProtocolNone")
	#define	KCDTypeNameMessageCentreNumber		            _S("MessageCentreNumber")
	#define	KCDTypeNameMessageValidityPeriod		        _S("MessageValidityPeriod")
	#define	KCDTypeNameMessageDeliveryReport		        _S("MessageDeliveryReport")
	#define	KCDTypeNameMinSignalLevel						_S("MinimumSignalLevel")
	#define	KCDTypeNameCommRole								_S("CommRole")
	#define	KCDTypeNameControlChannelPortName		        _S("ControlChannelPortName")
	#define	KCDTypeNameSirSettings							_S("SIRSettings")
	#define	KCDTypeNameBCAStack								_S("BCAStack")



//
// 088/  LAN Bearer
//

    const TMDBElementId 	KCDTIdLANBearerNifName	             = 0x08830000;	//< Commsdat field id for field: IfName. Field is declared in the LANBearer table.
	const TMDBElementId 	KCDTIdLANBearerLddFilename    	     = 0x08840000;	//< Commsdat field id for field: LDDFilename. Field is declared in the LANBearer table.
	const TMDBElementId 	KCDTIdLANBearerLddName	             = 0x08850000;	//< Commsdat field id for field: LDDName. Field is declared in the LANBearer table.
	const TMDBElementId 	KCDTIdLANBearerPddFilename	         = 0x08860000;	//< Commsdat field id for field: PDDFilename. Field is declared in the LANBearer table.
   	const TMDBElementId     KCDTIdLANBearerPddName               = 0x08870000;	//< Commsdat field id for field: PDDName. Field is declared in the LANBearer table.
	const TMDBElementId 	KCDTIdLANBearerPacketDriverName	     = 0x08880000;	//< Commsdat field id for field: PacketDriverName. Field is declared in the LANBearer table.
	const TMDBElementId 	KCDTIdLANLastSocketActivityTimeout   = 0x08890000;	//< Commsdat field id for field: LastSocketActivityTimeout. Field is declared in the LANBearer table.
	const TMDBElementId 	KCDTIdLANLastSessionClosedTimeout    = 0x088A0000;	//< Commsdat field id for field: LastSessionClosedTimeout. Field is declared in the LANBearer table.
    const TMDBElementId 	KCDTIdLANLastSocketClosedTimeout     = 0x088B0000;	//< Commsdat field id for field: LastSocketClosedTimeout. Field is declared in the LANBearer table.

	// Field Names
	#define	KCDTypeNameLanBearerLddFilename				_S("LDDFilename")
	#define	KCDTypeNameLanBearerLddName					_S("LDDName")
	#define	KCDTypeNameLanBearerPddFilename				_S("PDDFilename")
	#define	KCDTypeNameLanBearerPddName					_S("PDDName")
	#define	KCDTypeNameLanBearerPacketDriverName        _S("PacketDriverName")



//
// 090/ VIRTUAL BEARER RECORD
//

    // FieldTIds
	const TMDBElementId KCDTIdVirtualBearerNifName		    = 0x09030000;	//< Commsdat field id for field: IfName. Field is declared in the VirtualBearer table.	
	//
	const TMDBElementId KCDTIdVBLastSocketActivityTimeout	= 0x09040000;	//< Commsdat field id for field: LastSocketActivityTimeout. Field is declared in the VirtualBearer table.
	const TMDBElementId KCDTIdVBLastSessionClosedTimeout	= 0x09050000;	//< Commsdat field id for field: LastSessionClosedTimeout. Field is declared in the VirtualBearer table.
	const TMDBElementId KCDTIdVBLastSocketClosedTimeout	    = 0x09060000;	//< Commsdat field id for field: LastSocketClosedTimeout. Field is declared in the VirtualBearer table.
	//
    

	// Field Names
	#define	KCDTypeNameVirtualBearerAgent		            _S("Agent")
	#define	KCDTypeNameVirtualBeareNIFName					_S("IfName")
	#define	KCDTypeNameLastSocketActivityTimeout		    _S("LastSocketActivityTimeout")
	#define	KCDTypeNameLastSessionClosedTimeout		        _S("LastSessionClosedTimeout")
	#define	KCDTypeNameLastSocketClosedTimeout              _S("LastSocketClosedTimeout")





//
// 098/ WAP SMS Bearer Record
//

	// Element Type Ids
	const TMDBElementId KCDTIdWAPSMSAccessPointId			= 0x09830000;	//< Commsdat field id for field: AccessPointId. Field is declared in the VirtualBearer table.
	const TMDBElementId KCDTIdWAPSMSGatewayAddress			= 0x09840000;	//< Commsdat field id for field: GatewayAddress. Field is declared in the VirtualBearer table.
	const TMDBElementId KCDTIdWAPSMSServiceCentreAddress    = 0x09850000;	//< Commsdat field id for field: ServiceCentreAddress. Field is declared in the VirtualBearer table.
	const TMDBElementId KCDTIdWAPSMSWSPOption               = 0x09860000;	//< Commsdat field id for field: WSPOption. Field is declared in the VirtualBearer table.
	const TMDBElementId KCDTIdWAPSMSSecurity                = 0x09870000;	//< Commsdat field id for field: Security. Field is declared in the VirtualBearer table.


	// Element Type Names
	#define	KCDTypeNameWAPAccessPointId		               _S("AccessPointId")
	#define	KCDTypeNameWAPGatewayAddress		           _S("GatewayAddress")
	#define	KCDTypeNameWAPServiceCentreAddress		       _S("ServiceCentreAddress")
	#define	KCDTypeNameWAPWSPOption						   _S("WSPOption")
	#define	KCDTypeNameWAPSecurity						   _S("Security")




//
// 0A0/  WAP IP BEARER
//

	const TMDBElementId  KCDTIdWAPAccessPointId       = 0x0A030000;	//< Commsdat field id for field: AccessPointId. Field is declared in the WAP IP table.
	const TMDBElementId  KCDTIdWAPGatewayAddress      = 0x0A040000;	//< Commsdat field id for field: GatewayAddress. Field is declared in the WAP IP table.
	const TMDBElementId  KCDTIdWAPChargecard          = 0x0A050000;	//< Commsdat field id though this field is not assigned a field yet. Field is declared in the WAP IP table.
	const TMDBElementId  KCDTIdWAPIAP                 = 0x0A060000;	//< Commsdat field id for field: IAP. Field is declared in the WAP IP table.
	const TMDBElementId  KCDTIdWAPWSPOption           = 0x0A070000;	//< Commsdat field id for field: WSPOption. Field is declared in the WAP IP table.
	const TMDBElementId  KCDTIdWAPSecurity            = 0x0A080000;	//< Commsdat field id for field: Security. Field is declared in the WAP IP table.
	const TMDBElementId  KCDTIdWAPProxyPort           = 0x0A090000;	//< Commsdat field id for field: ProxyPortNumber. Field is declared in the WAP IP table.
	const TMDBElementId  KCDTIdWAPProxyLoginName      = 0x0A0A0000;	//< Commsdat field id for field: ProxyLogin. Field is declared in the WAP IP table.
	const TMDBElementId  KCDTIdWAPProxyLoginPass      = 0x0A0B0000;	//< Commsdat field id for field: ProxyPassword. Field is declared in the WAP IP table.

	// Element Type Names

	#define	KCDTypeNameWAPIAP		               _S("IAP" )
	#define	KCDTypeNameWAPProxyPort		           _S("ProxyPortNumber" )
	#define	KCDTypeNameWAPProxyLoginName		   _S("ProxyLogin" )
	#define	KCDTypeNameWAPProxyLoginPass		   _S("ProxyPassword" )

//
// 0A8/  CHARGECARD RECORD
//


	// Element Type Ids
	const TMDBElementId KCDTIdAccountNo     = 0x0A830000;	//< Commsdat field id for field: AccountNumber. Field is declared in the ChargeCard table.
	const TMDBElementId KCDTIdPin           = 0x0A840000;	//< Commsdat field id for field: Pin. Field is declared in the ChargeCard table.
	const TMDBElementId KCDTIdLocalRule     = 0x0A850000;	//< Commsdat field id for field: LocalRule. Field is declared in the ChargeCard table.
	const TMDBElementId KCDTIdNatRule       = 0x0A860000;	//< Commsdat field id for field: NatRule . Field is declared in the ChargeCard table.
	const TMDBElementId KCDTIdIntlRule      = 0x0A870000;	//< Commsdat field id for field: IntlRule. Field is declared in the ChargeCard table.


	// Element Type Names
	#define	KCDTypeNameAccountNo		       _S("AccountNumber")
	#define	KCDTypeNamePin		               _S("Pin")
	#define	KCDTypeNameLocalRule		       _S("LocalRule")
	#define	KCDTypeNameNatRule		           _S("NatRule")
	#define	KCDTypeNameIntlRule		           _S("IntlRule")

	
//
// 0B0/ PROXIES RECORD
//

   // Element Type Ids
	const TMDBElementId KCDTIdISP                = 0x0B030000;	//< Commsdat field id for field: ISP. Field is declared in the Proxies table.
	const TMDBElementId KCDTIdProxyServiceType   = 0x0B040000;	//< Commsdat field id for field: ProxyServiceType. Field is declared in the Proxies table.
	const TMDBElementId KCDTIdUseProxyServer     = 0x0B050000;	//< Commsdat field id for field: UseProxyServer. Field is declared in the Proxies table.
	const TMDBElementId KCDTIdServerName         = 0x0B060000;	//< Commsdat field id for field: ProxyServerName. Field is declared in the Proxies table.
	const TMDBElementId KCDTIdProtocolName       = 0x0B070000;	//< Commsdat field id for field: ProtocolName. Field is declared in the Proxies table.
	const TMDBElementId KCDTIdPortNumber         = 0x0B080000;	//< Commsdat field id for field: PortNumber. Field is declared in the Proxies table.
	const TMDBElementId KCDTIdExceptions         = 0x0B090000;	//< Commsdat field id for field: Exceptions. Field is declared in the Proxies table.


	// Element Type Names
	#define	KCDTypeNameProxyISP				          _S("ISP")
	#define	KCDTypeNameUseProxyServer		          _S("UseProxyServer")
	#define	KCDTypeNameProxyServiceType		          _S("ProxyServiceType")
	#define	KCDTypeNameServerName		              _S("ProxyServerName")
	#define	KCDTypeNameProtocolName		              _S("ProtocolName")
	#define	KCDTypeNamePortNumber		              _S("PortNumber")
	#define	KCDTypeNameExceptions		              _S("Exceptions")






//
// 1D/ ACCESS POINT RECORD
//
	// FieldTIds
	
	const TMDBElementId KCDTIdAccessPointGID        = 0x0E030000;	//< Commsdat field id for field: AccessPointGID. Field is declared in the Access Point table.
	const TMDBElementId KCDTIdSelectionPolicy       = 0x0E040000;	//< Commsdat field id for field: AccessPointSelectionPolicy. Field is declared in the Acess Point table.

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	const TMDBElementId KCDTIdTier                  = 0x0E050000;
	const TMDBElementId KCDTIdMCpr                  = 0x0E060000;
	const TMDBElementId KCDTIdCpr                   = 0x0E070000;
	const TMDBElementId KCDTIdCprConfig             = 0x0E080000;
	const TMDBElementId KCDTIdSCpr                  = 0x0E090000;
	const TMDBElementId KCDTIdProtocol              = 0x0E0A0000;
	const TMDBElementId KCDTIdAppSID                = 0x0E0B0000;
	const TMDBElementId KCDTIdConfigAPIdList        = 0x0E0C0000;
	const TMDBElementId KCDTIdCustomSelectionPolicy = 0x0E0D0000;
	const TMDBElementId KCDTIdAccessPointPriority	= 0x0E0E0000;
#endif

	// Field Names - names used more than once stored globally

	#define	KCDTypeNameAccessPointGID               _S("AccessPointGID")
	#define	KCDTypeNameSelectionPolicy              _S("AccessPointSelectionPolicy")
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    //the name of this field (KCDTypeNameSelectionPolicy) depends from the actual environment
//	#define	KCDTypeNameSelectionPolicy               _S("SelectionPolicy")
	#define	KCDTypeNameTier                          _S("Tier")
	#define	KCDTypeNameMCpr                          _S("MCpr")
	#define	KCDTypeNameCpr                           _S("Cpr")
	#define	KCDTypeNameCprConfig                     _S("CprConfig")
	#define	KCDTypeNameSCpr                          _S("SCpr")
	#define	KCDTypeNameProtocol                      _S("Protocol")
	#define	KCDTypeNameAppSID                        _S("AppSID")
	#define KCDTypeNameConfigAPIdList                _S("ConfigAPIdList")
	#define KCDTypeNameAccessPointPriority           _S("Priority")
	#define KCDTypeNameCustomSelectionPolicy         _S("CustomSelectionPolicy")
#else
    //the name of this field (KCDTypeNameSelectionPolicy) depends from the actual environment
//	#define	KCDTypeNameSelectionPolicy              _S("AccessPointSelectionPolicy")
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//
// 1E/ IAP PRIORITY RECORD
//
	// FieldTIds
	const TMDBElementId KCDTIdIap1                  = 0x0E830000;	//< Commsdat field id for field: IAP1. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap2                  = 0x0E840000;	//< Commsdat field id for field: IAP2. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap3                  = 0x0E850000;	//< Commsdat field id for field: IAP3. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap4                  = 0x0E860000;	//< Commsdat field id for field: IAP4. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap5                  = 0x0E870000;	//< Commsdat field id for field: IAP5. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap6                  = 0x0E880000;	//< Commsdat field id for field: IAP6. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap7                  = 0x0E890000;	//< Commsdat field id for field: IAP7. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap8                  = 0x0E8A0000;	//< Commsdat field id for field: IAP8. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap9                  = 0x0E8B0000;	//< Commsdat field id for field: IAP9. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap10                 = 0x0E8C0000;	//< Commsdat field id for field: IAP10. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap11                 = 0x0E8D0000;	//< Commsdat field id for field: IAP11. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap12                 = 0x0E8E0000;	//< Commsdat field id for field: IAP12. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap13                 = 0x0E8F0000;	//< Commsdat field id for field: IAP13. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap14                 = 0x0E900000;	//< Commsdat field id for field: IAP14. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIap15                 = 0x0E910000;	//< Commsdat field id for field: IAP15. Field is declared in the IAP Priority table.
	const TMDBElementId KCDTIdIapCount              = 0x0E920000;	//< Commsdat field id for field: IAPCount. Field is declared in the IAP Priority table.

	// Field Names - names used more than once stored globally
	#define	KCDTypeNameIap1Name                     _S("IAP1")
	#define	KCDTypeNameIap2Name                     _S("IAP2")
	#define	KCDTypeNameIap3Name                     _S("IAP3")
	#define	KCDTypeNameIap4Name                     _S("IAP4")
	#define	KCDTypeNameIap5Name                     _S("IAP5")
	#define	KCDTypeNameIap6Name                     _S("IAP6")
	#define	KCDTypeNameIap7Name                     _S("IAP7")
	#define	KCDTypeNameIap8Name                     _S("IAP8")
	#define	KCDTypeNameIap9Name                     _S("IAP9")
	#define	KCDTypeNameIap10Name                    _S("IAP10")
	#define	KCDTypeNameIap11Name                    _S("IAP11")
	#define	KCDTypeNameIap12Name                    _S("IAP12")
	#define	KCDTypeNameIap13Name                    _S("IAP13")
	#define	KCDTypeNameIap14Name                    _S("IAP14")
	#define	KCDTypeNameIap15Name                    _S("IAP15")
	#define	KCDTypeNameIapCountName                 _S("IAPCOUNT")

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	//
	// AP PRIORITY RECORD
	//
	// FieldTIds	
	const TMDBElementId KCDTIdAp1                  = 0x15030000;
	const TMDBElementId KCDTIdAp2                  = 0x15040000;
	const TMDBElementId KCDTIdAp3                  = 0x15050000;
	const TMDBElementId KCDTIdAp4                  = 0x15060000;
	const TMDBElementId KCDTIdAp5                  = 0x15070000;
	const TMDBElementId KCDTIdAp6                  = 0x15080000;
	const TMDBElementId KCDTIdAp7                  = 0x15090000;
	const TMDBElementId KCDTIdAp8                  = 0x150A0000;
	const TMDBElementId KCDTIdAp9                  = 0x150B0000;
	const TMDBElementId KCDTIdAp10                 = 0x150C0000;
	const TMDBElementId KCDTIdAp11                 = 0x150D0000;
	const TMDBElementId KCDTIdAp12                 = 0x150E0000;
	const TMDBElementId KCDTIdAp13                 = 0x150F0000;
	const TMDBElementId KCDTIdAp14                 = 0x15100000;
	const TMDBElementId KCDTIdAp15                 = 0x15110000;
	const TMDBElementId KCDTIdApCount              = 0x15120000;

	// Field Names - names used more than once stored globally
	#define	KCDTypeNameAp1Name                     _S("AP1")
	#define	KCDTypeNameAp2Name                     _S("AP2")
	#define	KCDTypeNameAp3Name                     _S("AP3")
	#define	KCDTypeNameAp4Name                     _S("AP4")
	#define	KCDTypeNameAp5Name                     _S("AP5")
	#define	KCDTypeNameAp6Name                     _S("AP6")
	#define	KCDTypeNameAp7Name                     _S("AP7")
	#define	KCDTypeNameAp8Name                     _S("AP8")
	#define	KCDTypeNameAp9Name                     _S("AP9")
	#define	KCDTypeNameAp10Name                    _S("AP10")
	#define	KCDTypeNameAp11Name                    _S("AP11")
	#define	KCDTypeNameAp12Name                    _S("AP12")
	#define	KCDTypeNameAp13Name                    _S("AP13")
	#define	KCDTypeNameAp14Name                    _S("AP14")
	#define	KCDTypeNameAp15Name                    _S("AP15")
	#define	KCDTypeNameApCountName                 _S("APCOUNT")
	
//
// TIER RECORD

	// FieldTIds
	const TMDBElementId KCDTIdTierImplUid	        = 0x11870000;
	const TMDBElementId KCDTIdTierThreadName        = 0x11830000;
	const TMDBElementId KCDTIdTierManagerName       = 0x11840000;
	const TMDBElementId KCDTIdDefaultAccessPoint    = 0x11850000;
	const TMDBElementId KCDTIdPromptUser            = 0x11860000;

	// Field Names - names used more than once stored globally
	#define	KCDTypeNameTierImplUid 					_S("TierImplUid")
	#define	KCDTypeNameTierThreadName               _S("TierThreadName")
	#define	KCDTypeNameTierManagerName              _S("TierManagerName")
	#define	KCDTypeNameDefaultAccessPoint           _S("DefaultAccessPoint")
	#define	KCDTypeNamePromptUser                   _S("PromptUser")

//
// MCPR RECORD

	// FieldTIds
	const TMDBElementId KCDTIdMCprUid               = 0x12030000;

	// Field Names - names used more than once stored globally
	#define	KCDTypeNameMCprUid                      _S("MCprUid")

//
// CPR RECORD

	// FieldTIds
	const TMDBElementId KCDTIdCprUid                = 0x12830000;

	// Field Names - names used more than once stored globally
	#define	KCDTypeNameCprUid                       _S("CprUid")

//
// SCPR RECORD

	// FieldTIds
	const TMDBElementId KCDTIdSCprUid               = 0x13030000;

	// Field Names - names used more than once stored globally
	#define	KCDTypeNameSCprUid                      _S("SCprUid")

//
// PROTOCOL RECORD

	// FieldTIds
	const TMDBElementId KCDTIdProtocolUid           = 0x13830000;
	const TMDBElementId KCDTIdProtocolConfigLoaderUid = 0x13840000;
	
	// Field Names - names used more than once stored globally
	#define	KCDTypeNameProtocolUid                  _S("ProtocolUid")
	#define	KCDTypeNameProtocolConfigLoaderUid      _S("ProtocolConfigLoaderUid")


	// Draw the field names from the Access Point Record

//
// 148/ ConfigAccessPoint
//
	// FieldTIds
	const TMDBElementId KCDTIdConfigAccessPointSCpr                   = 0x14830000;
	const TMDBElementId KCDTIdConfigAccessPointProtocol               = 0x14840000;
	const TMDBElementId KCDTIdConfigAccessPointProtocolConfig         = 0x14850000;
	const TMDBElementId KCDTIdConfigAccessPointLayerBelow             = 0x14860000;
	
	// Field Names - names used more than once stored globally
        #define	KCDTypeNameConfigAccessPointSCpr                      _S("SCpr")
	#define	KCDTypeNameConfigAccessPointProtocol                  _S("Protocol")
	#define	KCDTypeNameConfigAccessPointProtocolConfig            _S("ProtocolConfig")
	#define	KCDTypeNameConfigAccessPointLayerBelow                _S("LayerBelow")

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY


//
// EAP-SIM PROTOCOL RECORD

	// FieldTIds
	const TMDBElementId KCDTIdEapSimProtocolId 	= 0x17830000;	//< Commsdat field id for field: ProtocolId. Field is declared in the EAP-SIM PROTOCOL table.
	const TMDBElementId KCDTIdEapSimPseudonym	= 0x17840000;	//< Commsdat field id for field: Pseudonym. Field is declared in the EAP-SIM PROTOCOL table.
	const TMDBElementId KCDTIdEapSimUseNaiRealm = 0x17850000;	//< Commsdat field id for field: UseNaiRealm. Field is declared in the EAP-SIM PROTOCOL table.
	const TMDBElementId KCDTIdEapSimNaiRealm	= 0x17860000;	//< Commsdat field id for field: NaiRealm. Field is declared in the EAP-SIM PROTOCOL table.
	const TMDBElementId KCDTIdEapSimTsyName		= 0x17870000;	//< Commsdat field id for field: TsyName. Field is declared in the EAP-SIM PROTOCOL table.
	const TMDBElementId KCDTIdEapSimMinRands	= 0x17880000;	

	// Field Names - names used more than once stored globally
	#define	KCDTypeNameEapSimProtocolId		_S("ProtocolId")
	#define	KCDTypeNameEapSimPseudonym		_S("Pseudonym")
	#define	KCDTypeNameEapSimUseNaiRealm	_S("UseNaiRealm")
	#define	KCDTypeNameEapSimNaiRealm		_S("NaiRealm")
	#define	KCDTypeNameEapSimTsyName		_S("TsyName")
	#define	KCDTypeNameEapSimMinRands		_S("MinRands")


//
// EAP-AKA PROTOCOL RECORD

	// FieldTIds
	const TMDBElementId KCDTIdEapAkaProtocolId 	= 0x18030000;
	const TMDBElementId KCDTIdEapAkaPseudonym	= 0x18040000;
	const TMDBElementId KCDTIdEapAkaUseNaiRealm = 0x18050000;
	const TMDBElementId KCDTIdEapAkaNaiRealm	= 0x18060000;
	const TMDBElementId KCDTIdEapAkaTsyName		= 0x18070000;

	// Field Names - names used more than once stored globally
	#define	KCDTypeNameEapAkaProtocolId		_S("ProtocolId")
	#define	KCDTypeNameEapAkaPseudonym		_S("Pseudonym")
	#define	KCDTypeNameEapAkaUseNaiRealm	_S("UseNaiRealm")
	#define	KCDTypeNameEapAkaNaiRealm		_S("NaiRealm")
	#define	KCDTypeNameEapAkaTsyName		_S("TsyName")


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//
// 24/ POLICY SELECTOR RECORD
//
	// FieldTIds
	const TMDBElementId KCDTIdPolicyId 				= 0x15830000; 
	const TMDBElementId KCDTIdAppUid 				= 0x15840000; 
	const TMDBElementId KCDTIdSrcAddress			= 0x15850000; 
	const TMDBElementId KCDTIdSrcMask 				= 0x15860000; 
	const TMDBElementId KCDTIdDstAddress			= 0x15870000;
	const TMDBElementId KCDTIdDstMask 				= 0x15880000; 
	const TMDBElementId KCDTIdSrcPort 				= 0x15890000; 
	const TMDBElementId KCDTIdDstPort 				= 0x158A0000; 
	const TMDBElementId KCDTIdSrcPortMax 			= 0x158B0000; 
	const TMDBElementId KCDTIdDstPortMax			= 0x158C0000; 
	const TMDBElementId KCDTIdProtocolId			= 0x158D0000; 
	const TMDBElementId KCDTIdIAPid 				= 0x158E0000; 
	const TMDBElementId KCDTIdPriority				= 0x158F0000; 
	

	// Field Names
	#define	KCDTypeNamePolicyId						_S("PolicyId")
	#define	KCDTypeNameAppUid						_S("AppUid")
	#define	KCDTypeNameSrcAddress                   _S("SrcAddress")
	#define	KCDTypeNameSrcMask                      _S("SrcMask")
	#define	KCDTypeNameDstAddress          			_S("DstAddress")
	#define	KCDTypeNameDstMask             			_S("DstMask")
	#define	KCDTypeNameSrcPort                      _S("SrcPort")
	#define	KCDTypeNameDstPort                    	_S("DstPort")
	#define	KCDTypeNameSrcPortMax                   _S("SrcPortMax")
	#define	KCDTypeNameDstPortMax                   _S("DstPortMax")
	#define	KCDTypeNameProtocolId                   _S("ProtocolId")
	#define	KCDTypeNameIAPid                        _S("IAPid")
	#define	KCDTypeNamePriority                     _S("Priority")
	
//
// 25 POLICY SELECTOR 2 PARAMS RECORD
//
	// FieldTIds
	const TMDBElementId KCDTIdPolicySelectorId							= 0x16030000; 
	const TMDBElementId KCDTIdParamsId 									= 0x16040000; 

	// Field Names
	#define	KCDTypeNamePolicySelectorId						_S("PolicySelectorId")
	#define	KCDTypeNameParamsId							_S("ParamsId")

//
// 26/ GENERIC QOS PARAMETER RECORD
//

	// FieldTIds
	const TMDBElementId KCDTIdDownlinkBandwidth						= 0x16830000; 
	const TMDBElementId KCDTIdUplinkBandwidth						= 0x16840000; 
	const TMDBElementId KCDTIdDownLinkMaximumBurstSize 				= 0x16850000; 
	const TMDBElementId KCDTIdUpLinkMaximumBurstSize 				= 0x16860000; 
	const TMDBElementId KCDTIdDownLinkAveragePacketSize				= 0x16870000; 
	const TMDBElementId KCDTIdUpLinkAveragePacketSize				= 0x16880000; 
	const TMDBElementId KCDTIdDownLinkMaximumPacketSize				= 0x16890000; 
	const TMDBElementId KCDTIdUpLinkMaximumPacketSize				= 0x168A0000; 
	const TMDBElementId KCDTIdDownLinkDelay							= 0x168B0000; 
	const TMDBElementId KCDTIdUpLinkDelay							= 0x168C0000; 
	const TMDBElementId KCDTIdDownLinkDelayVariation				= 0x168D0000; 
	const TMDBElementId KCDTIdUpLinkDelayVariation					= 0x168E0000; 
	const TMDBElementId KCDTIdDownLinkPriority						= 0x168F0000; 
	const TMDBElementId KCDTIdUpLinkPriority						= 0x16900000; 	
	const TMDBElementId KCDTIdHeaderMode							= 0x16910000; 
//	const TMDBElementId KCDTIdQosName								= 0x16920000; 		


	// Field Names
	#define	KCDTypeNameDownlinkBandwidth					_S("DownlinkBandwidth")
	#define	KCDTypeNameUplinkBandwidth						_S("UplinkBandwidth")
	#define	KCDTypeNameDownLinkMaximumBurstSize             _S("DownLinkMaximumBurstSize")
	#define	KCDTypeNameUpLinkMaximumBurstSize             	_S("UpLinkMaximumBurstSize")
	#define	KCDTypeNameDownLinkAveragePacketSize            _S("DownLinkAveragePacketSize")
	#define	KCDTypeNameUpLinkAveragePacketSize              _S("UpLinkAveragePacketSize")
	#define	KCDTypeNameDownLinkMaximumPacketSize            _S("DownLinkMaximumPacketSize")
	#define	KCDTypeNameUpLinkMaximumPacketSize              _S("UpLinkMaximumPacketSize")
	#define	KCDTypeNameDownLinkDelay                  		_S("DownLinkDelay")
	#define	KCDTypeNameUpLinkDelay                       	_S("UpLinkDelay")
	#define	KCDTypeNameDownLinkDelayVariation               _S("DownLinkDelayVariation")
	#define	KCDTypeNameUpLinkDelayVariation                 _S("UpLinkDelayVariation")
	#define	KCDTypeNameDownLinkPriority                     _S("DownLinkPriority")
	#define	KCDTypeNameUpLinkPriority                    	_S("UpLinkPriority")
	#define	KCDTypeNameHeaderMode              				_S("HeaderMode")
//	#define	KCDTypeNameQosName                  			_S("QosName")
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

//
// 27/ WIFI SCANENGINE RECORD
//
	/**	FieldTIds
	@internalTechnology
	*/
	const TMDBElementId KCDTIdScanPeriodMs					= 0x17030000; 
	const TMDBElementId KCDTIdRSSIMin						= 0x17040000; 
	const TMDBElementId KCDTIdRSSIMax		 				= 0x17050000; 
	
	/** Field Names
	@internalTechnology
	*/
	#define	KCDTypeNameScanPeriodMs							_S("ScanPeriodMs")
	#define	KCDTypeNameRSSIMin								_S("RSSIMin")
	#define	KCDTypeNameRSSIMax				        		_S("RSSIMax")

} // end namespace CommsDat


#endif //COMMSDATTYPEINFOV1_1_H

