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
 @internalComponent
 @released 
*/

#ifndef __COMMSDATTYPEINFOV1_1_INTERNAL_H__
#define __COMMSDATTYPEINFOV1_1_INTERNAL_H__

namespace CommsDat
{

/**
SecureSocketProto is Internal Table
@internalComponent
*/
const TMDBElementId KCDTIdSSProtoRecord					  = 0x0B800000; //
/**
Agent Lookup is Internal Table
@internalComponent
*/
const TMDBElementId KCDTIdAgentLookupRecord				  = 0x0C000000; //

/**
EAPSecSettings is internal Table
@internalComponent
*/
const TMDBElementId KCDTIdEAPSecRecord					= 0x0F800000; //

/**
TunEAPSettings is internal Table
@internalComponent
*/
const TMDBElementId KCDTIdTunEAPRecord					= 0x10000000; //

/**
EAPTLSSetings is internal Table
@internalComponent
*/
const TMDBElementId KCDTIdEAPTLSRecord					= 0x10800000; //

/**
LEAPSetting is internal Table
@internalComponent
*/
const TMDBElementId KCDTIdLEAPRecord					= 0x11000000; //

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

/**
@internalTechnology
*/
const TMDBElementId KCDTIdTierRecord        	          = 0x11800000; //
/**
@internalTechnology
*/	
const TMDBElementId KCDTIdMCprRecord        			  = 0x12000000; //
/**
@internalTechnology
*/
const TMDBElementId KCDTIdCprRecord        	              = 0x12800000; //
/**
@internalTechnology
*/
const TMDBElementId KCDTIdSCprRecord                      = 0x13000000; //
/**
@internalTechnology
*/
const TMDBElementId KCDTIdProtocolRecord                  = 0x13800000; //

/**
@internalComponent
*/
const TMDBElementId KCDTIdBearerTypeRecord                = 0x14000000; //
const TMDBElementId KCDTIdConfigAccessPointRecord         = 0x14800000;
const TMDBElementId KCDTIdApPrioritySelectionPolicyRecord = 0x15000000; //
const TMDBElementId KCDTIdPolicySelectorRecord               = 0x15800000; //
const TMDBElementId KCDTIdPolicySelector2ParamsRecord        = 0x16000000; //

const TMDBElementId KCDTIdGenericQosRecord                = 0x16800000; //

const TMDBElementId KCDTIdWifiScanEngineRecord            = 0x17000000; //

#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY


//
// 050/ WLAN SERVICE EXTENSION RECORD
//

	// FieldTIds
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANProfile	 					= 0x05030000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANSSID	 					= 0x05040000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANEncryptionStatus	 			= 0x05050000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANEncryptionType	 			= 0x05060000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANDefaultWepKey  	 			= 0x05070000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANEncryptKey1Valid	 			= 0x05080000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANWepKey1	 				= 0x05090000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANEncryptKey2Valid	 			= 0x050A0000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANWepKey2	 				= 0x050B0000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANEncryptKey3Valid	 			= 0x050C0000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANWepKey3	 				= 0x050D0000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANEncryptKey4Valid	 			= 0x050E0000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANWepKey4	 				= 0x050F0000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANRanking 	 					= 0x05100000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANDialogPref	 				= 0x05110000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANNetworkType	 				= 0x05120000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANFragThres	 					= 0x05130000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANRTSThres	 					= 0x05140000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANDesTransRate	 				= 0x05150000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANBeaconInt	 					= 0x05160000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANChannelId	 				= 0x05170000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANAuthenticationMode	 		= 0x05180000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANPowerSaveMode	 				= 0x05190000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANAllowUnencryptMode	 		= 0x051A0000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANAllowShortPre	 				= 0x051B0000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANPreambleType	 				= 0x051C0000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANTxPowerLevel	 				= 0x051D0000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANEapPresharedKey	 			= 0x051E0000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANRegDomain	 					= 0x051F0000;
	/**
	@internalComponent
	*/
	const TMDBElementId	KCDTIdWLANSecData	 					= 0x05200000;


	// Field Names
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANProfile							_S("Wlan_Profile")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANSSID							_S("Wlan_SSID")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANEncryptionStatus					_S("Wlan_Encryption_Status")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANEncryptionType					_S("Wlan_Encryption_Type")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANDefaultWepKey  				_S("Wlan_Wep_Default_Key")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANEncryptKey1Valid					_S("Wlan_Key1_Valid")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANWepKey1						_S("Wlan_Wep_Key1")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANEncryptKey2Valid					_S("Wlan_Key2_Valid")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANWepKey2						_S("Wlan_Wep_Key2")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANEncryptKey3Valid					_S("Wlan_Key3_Valid")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANWepKey3						_S("Wlan_Wep_Key3")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANEncryptKey4Valid					_S("Wlan_Key4_Valid")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANWepKey4						_S("Wlan_Wep_Key4")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANRanking 							_S("Wlan_Ranking")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANDialogPref						_S("Wlan_Dialog_Pref")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANNetworkType						_S("Wlan_Network_Type")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANFragThres						_S("Wlan_Frag_Threshold")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANRTSThres							_S("Wlan_RTS_Threshold")
	/**	
	@internalComponent
	*/
	#define	KCDTypeNameWLANDesTransRate						_S("Wlan_Desired_Transmit_Rate")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANBeaconInt						_S("Wlan_Beacon_Int")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANChannelId						_S("Wlan_Channel_ID")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANAuthenticationMode			_S("Wlan_Authentication_Mode")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANPowerSaveMode					_S("Wlan_Power_Save_Mode")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANAllowUnencryptMode				_S("Wlan_Allow_UnEncrypt_Mode")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANAllowShortPre					_S("Wlan_Allow_Short_Preamble")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANPreambleType						_S("Wlan_Preamble_Type")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANTxPowerLevel						_S("Wlan_TX_Power_Level")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANEapPresharedKey					_S("Wlan_EAP_Preshared_Key")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANRegDomain						_S("Wlan_Reglutory_Domain")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameWLANSecData							_S("Wlan_Security_Data")
	
	
	//
	// 0B8/ SECURE SOCKET PROTO RECORD
	//

	// Field Type Ids
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdSSProtoName		 = 0x0B830000;
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdSSProtoLibrary 	 = 0x0B840000;


	/**
	@internalComponent
	*/
	#define	KCDTypeNameSSProtoName		           _S("ProtocolName")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameSSProtoLibrary		       _S("ProtoLibrary")
		
		
	//
	// 17/ AGENTLOOKUPTABLE RECORD
	//


	// FieldTIds
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdAgentFriendlyName 	 = 0x0C030000;
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdAgentFileName 		 = 0x0C040000;


	// Field Names - names used more than once stored globally
	/**
	@internalComponent
	*/
	#define	KCDTypeNameAgentFileName		               _S("AgentFilename")
	/**
	@internalComponent
	*/
	#define	KCDTypeNameAgentFriendlyName		           _S("AgentFriendlyName")


	
	//
	// 14/ BearerType 
	//
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdBearerTypeTier        = 0x14030000;
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdBearerTypeMCpr        = 0x14040000;
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdBearerTypeCpr         = 0x14050000;
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdBearerTypeSCpr        = 0x14060000;
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdBearerTypeProtocol    = 0x14070000;
	
	
	//
	// 20/  EAP SECURITY SETTINGS RECORD
	//
	/**
	@internalComponent
	*/
		const TMDBElementId KCDTIdEAPSecOuterEAPType    = 0x0F830000;
	/**
	@internalComponent
	*/
		const TMDBElementId KCDTIdEAPSecId       		= 0x0F840000;
	/**
	@internalComponent
	*/
		const TMDBElementId KCDTIdEAPSecPassword       	= 0x0F850000;
	/**
	@internalComponent
	*/
		const TMDBElementId KCDTIdEAPSecConfigId      	= 0x0F870000;
	/**
	@internalComponent
	*/
		const TMDBElementId KCDTIdEAPSecData       		= 0x0F860000;

	/**
	@internalComponent
	*/
	    const TMDBElementId KCDTIdEAPSecVendorId   		= 0x0F880000;
	    
	/**
	@internalComponent
	*/
	    const TMDBElementId KCDTIdEAPSecVendorType 		= 0x0F890000;    


	/**
	@internalComponent
	*/
		#define	KCDTypeNameEAPSecOuterEAPType	 	_S("OuterEapMethodType")
	/**
	@internalComponent
	*/
		#define	KCDTypeNameEapId  					_S("EapId")
	/**
	@internalComponent
	*/
		#define	KCDTypeNameEAPSecPassword  			_S("EapPassword")
	/**
	@internalComponent
	*/
		#define	KCDTypeNameEAPSecConfigId  			_S("ConfigId")

	/**
	@internalComponent
	*/
		#define	KCDTypeNameEAPSecData 				_S("EAP_Method_Data")

	/**
	@internalComponent
	*/
		#define	KCDTypeNameEAPSecVendorId			_S("EapVendorId")

	/**
	@internalComponent
	*/
		#define	KCDTypeNameEAPSecVendorType			_S("EapVendorType")

	//
	// 21/  TUNNELLED EAP SETTINGS RECORD
	//
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdTUNEAPInnerType       = 0x10030000;
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdTUNEAPData       		= 0x10040000;



	/**
	@internalComponent
	*/
	#define	KCDTypeNameTUNEAPInnerType	 	_S("InnerEapMethodType")
	/**
	@internalComponent
	*/
	#define KCDTypeNameTUNEAPData 			_S("Tun_EAP_Method_Data")



	//
	// 22/  EAP-TLS SETTINGS RECORD
	//
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdEAPTLSClientCertName    	= 0x10830000;
	/**
	@internalComponent
	*/
	#define	KCDTypeNameEAPTLSClientCertName	 			_S("ClientCertificateName")



	//
	// 23/  LEAP SETTINGS RECORD
	//
	/**
	@internalComponent
	*/
	const TMDBElementId KCDTIdLEAPTimeout    	= 0x11030000;
	/**
	@internalComponent
	*/
	#define	KCDTypeNameLEAPTimeout	 			_S("LeapTimeout")
	
} // namespace CommsDat

#endif // __COMMSDATTYPEINFOV1_1_INTERNAL_H__

