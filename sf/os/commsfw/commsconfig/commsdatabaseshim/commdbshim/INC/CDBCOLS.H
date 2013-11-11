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
// - General definitions for client use
// - Table and column names,
// - Enums and constants use in tables,
// - UIDs for notifications
//
//

/**
 @file
 @publishedAll
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/


#ifndef CDBCOLS_H
#define CDBCOLS_H

#include <e32std.h>
// Database table names. The name (a string) of each table is defined by a macro.
#define NETWORK											_S("Network")
/** Internet access point table identifier.*/
#define IAP												_S("IAP")
/** Identifier of default dial-out ISP table.

Field type: TUint32 */
#define DIAL_OUT_ISP									_S("DialOutISP")
/** Dial-in Internet Service Provider.
*/
#define DIAL_IN_ISP										_S("DialInISP")
/** Outgoing WCDMA table.
*/
#define OUTGOING_WCDMA									_S("OutgoingGPRS")
/**
Not used in v7.0 */
#define INCOMING_WCDMA									_S("IncomingGPRS")
#define LAN_SERVICE										_S("LANService")	// test use only
#define PAN_SERVICE_EXTENSIONS							_S("PANServiceExtensions")
#define WLAN_SERVICE_EXTENSIONS							_S("WLANServiceExtensions")
/**
Identifier of modem settings table.
In v7.0s, this table changed its name from MODEM to MODEM_BEARER.
In v6.1, it is possible to define only a single record for this table. */
#define MODEM_BEARER									_S("ModemBearer")
#define LAN_BEARER										_S("LANBearer")
/** Identifier of the default location table.
In 6.1, it is possible to define only a single record for this table.
Field type: TUint32 */
#define LOCATION										_S("Location")
/** Identifier of the default chargecard table.

Field type: TUint32 */
#define CHARGECARD										_S("Chargecard")
/** Identifier of proxy server settings table.*/
#define PROXIES											_S("Proxies")
/** Identifier of default WAP Access Point table.

Field type: TUint32 */
#define WAP_ACCESS_POINT								_S("WAPAccessPoint")
/** WAP IP bearer. */
#define WAP_IP_BEARER									_S("WAPIPBearer")
/** WAP SMS bearer. */
#define WAP_SMS_BEARER									_S("WAPSMSBearer")
/** GPRS default settings.

It is possible to define only one record for  this table.
*/
#define DEFAULT_GPRS									_S("DefaultGPRS")
/** Bluetooth device table. */
#define BT_DEVICE_TABLE									_S("BTDeviceTable")
/** Bluetooth default table. */
#define BT_DEFAULT_TABLE								_S("BTDefaultTable")
/** Bluetooth security table. */
#define BT_SECURITY_TABLE								_S("BTSecurityTable")
/** Bluetooth persist table. */
#define BT_PERSIST_TABLE								_S("BTPersistTable")
/** Identifier of the sockets table.*/
#define SS_PROTO_TABLE									_S("SecureSocketTable")
/** Agent lookup table identifier*/
#define AGENT_LOOKUP									_S("AgentLookup")
#define VIRTUAL_BEARER									_S("VirtualBearer")
#define VPN_SERVICE										_S("VpnService")
#define WLAN_SERVICE_EXTENSION_TABLE					_S("WLANServiceExtensionTable")

#define ACCESS_POINT_TABLE								_S("AccessPointTable")
#define IAP_PRIORITY_SELECTION_POLICY_TABLE				_S("IAPPrioritySelectionPolicyTable")
#define XML_ACCESS_POINT_TABLE							_S("AccessPoint")


#define AP_PRIORITY_SELECTION_POLICY_TABLE				_S("APPrioritySelectionPolicyTable")
#define TIER_TABLE										_S("TierTable")
#define MCPR_TABLE										_S("MCprTable")
#define CPR_TABLE										_S("CprTable")
#define SCPR_TABLE										_S("SCprTable")
#define PROTOCOL_TABLE									_S("ProtocolTable")
#define BEARER_TYPE_TABLE								_S("BearerTypeTable")
#define CONFIG_ACCESS_POINT_TABLE                                                       _S("ConfigAccessPointTable")

#define XML_AP_PRIORITY_SELECTION_POLICY_TABLE			_S("APPrioritySelectionPolicy")
#define XML_TIER_TABLE									_S("Tier")
#define XML_MCPR_TABLE									_S("MCpr")
#define XML_CPR_TABLE									_S("Cpr")
#define XML_SCPR_TABLE									_S("SCpr")
#define XML_PROTOCOL_TABLE								_S("Protocol")
#define XML_BEARER_TYPE_TABLE							_S("BearerType")
#define XML_CONFIG_ACCESS_POINT_TABLE                                                       _S("ConfigAccessPoint")


#define	EAP_SEC_TABLE									_S("EAPSecuritySettingsTable")
#define	TUN_EAP_TABLE									_S("TunnelledEAPSettingsTable")
#define	EAP_TLS_TABLE									_S("EAPTLSSettingsTable")
#define	LEAP_TABLE										_S("LEAPSettingsTable")
#define EAPSIM_PROTOCOL_TABLE							_S("EapSimProtocolTable")
#define EAPAKA_PROTOCOL_TABLE							_S("EapAkaProtocolTable")


#define POLICY_SELECTOR_TABLE                     		_S("PolicySelectorTable")
#define XML_POLICY_SELECTOR_TABLE                 		_S("PolicySelector")

#define POLICYSELECTOR2QOSPARAMETERS_TABLE 				_S("PolicySelector2QosParametersTable")
#define XML_POLICYSELECTOR2QOSPARAMETERS_TABLE 			_S("PolicySelector2QosParameters")

#define GENERIC_QOS_TABLE 								_S("GenericQosTable")
#define XML_GENERIC_QOS_TABLE 							_S("GenericQos")

/**
@internalTechnology
*/
#define WIFI_SCANENGINE_TABLE                     		_S("WifiScanEngineTable")



//DEPRECATED : MODEM has been reassigned so as to maintain backwards compatibility
/** Identifier of default modem.

Field type: TUint32
@deprecated Unsupported since v7.0 */
#define MODEM											MODEM_BEARER
// Just in case any applications use the literal pre-7.0s string for the ModemBearer table.
/** @deprecated Unsupported since v7.0 */
#define OLD_MODEM_TABLE									_S("Modem")


//
// Global settings.
// The following macros define values for the Global table fields.
// Note that Global table fields are accessed through CCommsDatabase.
//
/** Number of times to attempt redial when the ISP number is busy.

Field type: TUint32 */
#define REDIAL_ATTEMPTS									_S("RedialAttempts")
/** The bearer for sending SMS.

Values for this are defined by the enum RMobileSmsMessaging::TMobileSmsBearer

Field type: TUint32 */
#define SMS_BEARER										_S("SmsBearer")				// Takes the values of RGprs::TSmsBearer from ETEL
/** The SMS receive mode, taken from RMobileSmsMessaging::TMobileSmsReceiveMode.

Field type: TUint32 */
#define SMS_RECEIVE_MODE								_S("SmsReceiveMode")		// Takes the values of RMobileSmsMessaging::TMobileSmsReceiveMode from ETel MM
/** The GPRS attach mode, i.e. whether to attach at the boot-up of the device or
at a later time.

Values for this are defined by the enum RPacketService::TAttachMode
Field type: TUint32 */
#define GPRS_ATTACH_MODE								_S("GPRSAttachMode")				// Takes the values of RGprs::TGprsAttachMode from ETEL
/** Whether or not to allow incoming requests for PDP context activation to be automatically
accepted (when there is a waiting application).

Use a TBool to specify the value.

Field type: TUint32 */
#define ACCEPT_INCOMING_GPRS							_S("AcceptIncomingGprs")			// Takes a TBool value - ETrue = accept incoming requests, EFalse = reject incoming requests
/** The preferred bearer when device is forced into GPRS Class C operation.

The values for this field are defined by TCommDbGprsClassCBearer.

Field type: TUint32

@see TCommDbGprsClassCBearer */
#define GPRS_CLASS_C_BEARER								_S("GPRSClassCBearer")				// Takes the values of TCommDbGprsClassCBearer
/** The number of connection attempts (preferences) to make.

This is limited to a maximum of 2.

Field type: TUint32 */
#define CONNECTION_ATTEMPTS								_S("ConnectionAttempts")			// Number of connection preferences, with incremental rank, to try
/** Specifies the ID of the modem record to be used for data transactions.

Field type: TUint32 */
#define MODEM_DATA_FAX									_S("ModemForDataAndFax")
/** Specifies the ID of the modem record to be used for phone services such as SMS.

Field type: TUint32 */
#define MODEM_PHONE_SERVICES_SMS						_S("ModemForPhoneServicesAndSMS")
/** Specifies the location to be used with the default modem for data transactions.

Field type: TUint32 */
#define LOCATION_DATA_FAX								_S("LocationForDataAndFax")
/** Specifies the location to be used with default modem for phone services such
as SMS.

Field type: TUint32 */
#define LOCATION_PHONE_SERVICES_SMS						_S("LocationForPhoneServicesAndSMS")
/** Maximum RMBuf heap size.

Field type: TUint32 */
#define MAXMBUF_HEAP									_S("MaxMBufHeap")
#define DEFAULT_AGENT									_S("DefaultAgent")
#define DEFAULT_NETWORK									_S("DefaultNetwork")
#define BEARER_AVAILABILITY_CHECK_TSY					_S("BearerAvailabilityCheckTSY")

// The following fields define the specific CDMA parameters provisioned through OTA
// and  defined in TIA-683B section 3.5.8

/** Operation capability bitmap to indicate which operations are supported by the MS.
Bit allocation:

bit 0 - SimpleIP supported

bit 1 - MobileIP supported

bit 2 - MobileIP with simple IP fallback supported

bits 3-7 - reserved

Field type: TUint32 */
#define CDMA_OP_CAPABILITY								_S("CDMAOperationCapability")
/** Maximum number of SimpleIP NAI entries.

Field type: TUint32 */
#define CDMA_SIMIP_MAX_NUM_NAI							_S("CDMASimpleIpMaxNumNai")
/** Maximum SimpleIP NAI length (min 72 octets).

Field type: TUint32 */
#define CDMA_SIMIP_MAX_NAI_LENGTH						_S("CDMASimpleIpMaxNaiLength")
/** Maximum length of SimpleIP shared secret data.

Field type: TUint32 */
#define CDMA_SIMIP_MAX_SS_LENGTH						_S("CDMASimpleIpMaxSsLength")
/** SimpleIP authentication algorithm bitmap supported by the MS.
Bit allocation:

bit 0 - CHAP supported

bit 1 - PAP supported

bits 2-7 - reserved

Field type: TUint32 */
#define CDMA_SIMIP_AUTH_SUPPORTED						_S("CDMASimpleIpAuthSupported")
/** Maximum number of mobile IP NAI entries.

Field type: TUint32 */
#define CDMA_MIP_MAX_NUM_NAI							_S("CDMAMobileIpMaxNumNai")
/** Maximum MobileIP NAI length (min 72 octets).

Field type: TUint32 */
#define CDMA_MIP_MAX_NAI_LENGTH							_S("CDMAMobileIpMaxNAILength")
/** Maximum length of the shared secret data (MN-AAA)

Field type: TUint32 */
#define CDMA_MIP_MAX_MN_AAA_SS_LENGTH					_S("CDMAMobileIpMaxMnAaaSsLength")
/** Authentication algorithm bitmap (MN-AAA) supported by the mobile station.
Bit allocation:

bit 0 - MD5 authentication supported (RFC 3012)

bits 1-7 - reserved

Field type: TUint32 */
#define CDMA_MIP_MN_AAA_AUTH_ALGORITHM					_S("CDMAMobileIpMnAaaAuthAlgorithm")
/** Maximum length of the shared secret data (MN-HA)

Field type: TUint32 */
#define CDMA_MIP_MAX_MN_HA_SS_LENGTH					_S("CDMAMobileIpMaxMnHaSsLength")
/** Authentication algorithm bitmap (MN-HA) supported by the mobile station.
Bit allocation:

bit 0 - MD5 authentication supported (RFC 3012)

bits 1-7 - reserved

Field type: TUint32 */
#define CDMA_MIP_MN_HA_AUTH_ALGORITHM					_S("CDMAMobileIpMnHaAuthAlgorithm")

/** Prompt for an Access Point if used.

Field type: TBool*/
#define PROMPT_FOR_AP									_S("PromptForSnap")

/** If default Access Point is defined, use default Access Point. Otherwise use connection preferences.

Field type: TUint32 */
#define DEFAULT_AP										_S("DefaultSnap")

/** This field differentiate the Bravo selection from the 399 selection
(needed for implicit scenarios where prefs are not available).

Field type: TUint32 */
#define DEFAULT_SNAP									_S("DefaultSnap")

/** Default tier.

Field type: TUint32 */
#define DEFAULT_TIER	      							_S("DefaultTier")

/** Prompt the User.

Field type: TBool*/
#define PROMPT_USER									    _S("PromptUser")

/** This field  defines the default or current IEEE 802.11 regulatory domain or country code.
Field type: TCommsDatWlanRegDomain */
#define REGULATORY_DOMAIN						_S("RegulatoryDomain")

#define	WMM_ENABLED										_S("WmmEnabled")

/** This field determines whether to use the country code information or not.
 *
 * Field type: TBool */
#define ENABLE_COUNTRYCODE								_S("EnableCountryCode")

/** This field  defines the default regulatory domain or country code.
 *
 * Field type: TDesC */
#define DEFAULT_REGULATORY_DOMAIN						_S("DefaultRegulatoryDomain")

/**
Maintained for backwards compatibility.
Specifies whether to show the connection dialog.
Field type: TUint32 (Boolean)
@deprecated v7.0 */
#define ASK_USER_BEFORE_DIAL							_S("AskUserBeforeDial")
/** Identifier of default dial-out IAP.

Field type: TUint32
@deprecated v7.0 */
#define DIAL_OUT_IAP									_S("DialOutIAP")
#define DIAL_IN_IAP										_S("DialInIAP")
/** Identifier of (dial out) ISP in this IAP.
Field type: TUint32. This field may not be left as NULL.
@deprecated v7.0 */
#define IAP_ISP											_S("IAPISP")
/** Time to stay online when all clients have closed.

Field type: TUint32
@deprecated v7.0 */
#define CLIENT_TIMEOUT									_S("ClientTimout")
/** Time to stay online when idle.

Field type: TUint32
@deprecated v7.0 */
#define ROUTE_TIMEOUT									_S("RouteTimeout")
/** Outgoing GPRS table.
*/
#define OUTGOING_GPRS									OUTGOING_WCDMA
/** Incoming GPRS table.
*/
#define INCOMING_GPRS									INCOMING_WCDMA
#define ECommDbBearerUnknown							KCommDbBearerUnknown
#define ECommDbBearerCSD								KCommDbBearerCSD
#define ECommDbBearerWcdma								KCommDbBearerWcdma
#define ECommDbBearerLAN								KCommDbBearerLAN
#define ECommDbBearerCdma2000							KCommDbBearerCdma2000
#define ECommDbBearerPSD								KCommDbBearerPSD
#define ECommDbBearerVirtual							KCommDbBearerVirtual
#define ECommDbBearerPAN								KCommDbBearerPAN
#define ECommDbBearerWLAN								KCommDbBearerWLAN
/** Avoids the need for lots of search & replace in migrations from v6.1 */
#define ECommDbBearerGPRS                               ECommDbBearerWcdma

// Columns common to all tables
/**Unique ID assigned by the server for identification of records by the server.

Field type: TUint32 */
#define COMMDB_ID										_S("Id")			// Id of the row
/**Name of the record for easy identification by the user.

This is not present in the some tables.
Field type: Text. */
#define COMMDB_NAME										_S("Name")			// Name of the row

// COMMDB_UNUSED_NAME with COMMDB_ID appended, is inserted in the name column of entries that
// have been deleted, but are required to be inserted by ceddump, inserted and subsequently
// deleted by ced so that the COMMDB_ID indices are preserved. Such entries may be modified
// but ideally should not be deleted as this will require all the indices to be recalculated.
_LIT(COMMDB_UNUSED_NAME, "__UNUSED_ENTRY_DO_NOT_DELETE__");

// Columns for `IAP` table
// The following macros define values for the IAP table fields.
// An IAP table allows a group of related records for service information (including service
// type), and chargecard (optional) to be identified.
/** Identifier of the dialog preference in this IAP.

Field type: TUint32. This field may be left as NULL. */
#define IAP_DIALOG_PREF									_S("DialogPref")
/** Identifier of the service in this IAP.

Field type: TUint32. This field may not be left as NULL. */
#define IAP_SERVICE										_S("IAPService")
/** Name of the service table (e.g. DIAL_OUT_ISP, OUTGOING_GPRS)
in this IAP.

Field type: Text. This field may not be left as NULL. */
#define IAP_SERVICE_TYPE								_S("IAPServiceType")
/** Identifier of the bearer in this IAP.

Field type: TUint32. This field may be left as NULL. */
#define IAP_BEARER										_S("IAPBearer")
/** Identifier of the type of the bearer in this IAP.

Field type: Text. This field may be left as NULL. */
#define IAP_BEARER_TYPE									_S("IAPBearerType")
/** Identifier of the network in this IAP.

Field type: TUint32. This field may be left as NULL. */
#define IAP_NETWORK										_S("IAPNetwork")
/** Identifier of the Application secure ID in this IAP.

Field type: TUint32. This field may be left as NULL. */
#define IAP_APPSID                                      _S("IAPAppSid")

/** Identifier of the weighting for networks in this IAP.

Field type: TUint32. This field may be left as NULL. */
#define IAP_NETWORK_WEIGHTING							_S("IAPNetworkWeighting")
/** Identifier of the location in this IAP.

Field type: TUint32. This field may be left as NULL. */
#define IAP_LOCATION									_S("Location")
//#define IAP_CHARGECARD									_S("Chargecard")

#define BCA_STACK										_S("BCAStack") 						///< Comma separated list of BCAs, eg. C32BCA 

// Columns common to all bearer tables using IP and PPP
#define IF_NAME											_S("IfName")						//< Interface name, e.g. "PPP" or "SLIP"

// Columns common to all service tables using IP and PPP
#define SERVICE_IF_PARAMS								_S("IfParams")						//< Interface parameter string
#define SERVICE_IF_NETWORKS								_S("IfNetworks")					//< Comma separated list of network protocols, e.g. "PPP"


/**  When using external IP configuration (MobileIP or DHCP), always reject PPP peer authentication requests.
If external IP configuration is used and this field is set to TRUE then peer authentication requests are always rejected.
If external IP configuration is used and this field is set to FALSE then PPP allows authentication to proceed (although it may still fail later).
If this field is not set or absent, the result is the same as when it is set to TRUE. */
#define SERVICE_IF_EXTERN_IP_CONFIG_ALWAYS_REJECT_AUTH	_S("IfExternIpConfigAlwaysRejectAuth")

#define SERVICE_IF_PROMPT_FOR_AUTH						_S("IfPromptForAuth")				//< Prompt user for authentication name and password?
#define SERVICE_IF_AUTH_NAME							_S("IfAuthName")					//< Authentication username used by PPP
#define SERVICE_IF_AUTH_PASS							_S("IfAuthPass")					//< Authentication password used by PPP
#define SERVICE_IF_AUTH_RETRIES							_S("AuthRetries")					//< No of times to retry authentication if it fails
#define SERVICE_IF_CALLBACK_ENABLED						_S("IfCallbackEnabled")				//< Callback enabled?
#define SERVICE_IF_CALLBACK_TYPE						_S("IfCallbackType")				//< Type of callback if enabled
#define SERVICE_IF_CALLBACK_INFO						_S("IfCallbackInfo")				//< Info for callback request if enabled
#define SERVICE_CALLBACK_TIMEOUT						_S("CallbackTimeout")				//< time to wait for callback
#define SERVICE_IF_SERVER_MODE							_S("IfServerMode")					//< PPP in server mode?

#define SERVICE_IP_ADDR_FROM_SERVER						_S("IpAddrFromServer")				//< Get IP address from server?
#define SERVICE_IP_ADDR									_S("IpAddr")						//< IP Address of Interface
#define SERVICE_IP_NETMASK								_S("IpNetMask")						//< IP Netmask of Interface
#define SERVICE_IP_GATEWAY								_S("IpGateway")						//< IP Address of Gateway
#define SERVICE_IP_DNS_ADDR_FROM_SERVER					_S("IpDNSAddrFromServer")			//< Get DNS addresses from server?
#define SERVICE_IP_NAME_SERVER1							_S("IpNameServer1")					//< IP Address of primary name server reachable through this interface
#define SERVICE_IP_NAME_SERVER2							_S("IpNameServer2")					//< IP Address of secondary name server reachable through this interface

#define SERVICE_IP6_DNS_ADDR_FROM_SERVER					_S("Ip6DNSAddrFromServer")			//< Get DNS addresses from server?
#define SERVICE_IP6_NAME_SERVER1							_S("Ip6NameServer1")					//< IP Address of primary name server reachable through this interface
#define SERVICE_IP6_NAME_SERVER2							_S("Ip6NameServer2")					//< IP Address of secondary name server reachable through this interface

#define SERVICE_ENABLE_IP_HEADER_COMP	 				_S("EnableIPHeaderComp")			//< Enable IP header compression
#define SERVICE_ENABLE_LCP_EXTENSIONS					_S("EnableLCPExtension")			//< Enable LCP extension
#define SERVICE_DISABLE_PLAIN_TEXT_AUTH					_S("DisablePlainTextAuth")			//< Disable plain text authentication
#define SERVICE_ENABLE_SW_COMP							_S("EnableSWComp")					//< Enable S/W compression

#define SERVICE_IP_ADDR_LEASE_VALID_FROM				_S("IpAddrLeaseValidFrom")			//< Start of address lease (.e.g. a DHCP assigned ip address)
#define SERVICE_IP_ADDR_LEASE_VALID_TO					_S("IpAddrLeaseValidTo")			//< End of address lease (e.g. a DHCP assigned IP address)
#define SERVICE_CONFIG_DAEMON_MANAGER_NAME				_S("ConfigDaemonManagerName")		//< Name of the ECOM daemon manager plug-in for NIFMAN used to load a specific configuration daemon (see the next field)
#define SERVICE_CONFIG_DAEMON_NAME						_S("ConfigDaemonName")				//< Name of the daemon used for address configuration .e.g Dhcp, MobileIp

/** Enable link-local multicast name resolution

Field type: TBool. This field may be left as NULL. */
#define SERVICE_ENABLE_LLMNR						_S("EnableLLMNR")
#define ISP_SERVICE_ENABLE_LLMNR					SERVICE_ENABLE_LLMNR
#define GPRS_SERVICE_ENABLE_LLMNR					SERVICE_ENABLE_LLMNR
#define LAN_SERVICE_ENABLE_LLMNR					SERVICE_ENABLE_LLMNR
#define CDMA_SERVICE_ENABLE_LLMNR					SERVICE_ENABLE_LLMNR
#define VPN_SERVICE_ENABLE_LLMNR					SERVICE_ENABLE_LLMNR

// Columns of the `DIAL_OUT_ISP` and `DIAL_IN_ISP` service tables.
// The following macros define values for the dial out/in ISP table fields.
/** Description of ISP (for use by applications etc.)

Field type: Text. This field may be left as NULL.*/
#define ISP_DESCRIPTION									_S("Description")					// text field for app to use to describe ISP
/** TCommsDbIspType value indicating ISP type, e.g. Internet, LAN, etc.

Field type: TUint32. This field may be left as NULL. */
#define ISP_TYPE										_S("Type")							// int field for ISP "type" e.g. Internet, LAN, etc

/** Default phone number.

Field type: Text. This field may be left as NULL. */
#define ISP_DEFAULT_TEL_NUM								_S("DefaultTelNum")					// The default telephone number to be used if the location does not match those below
/** Perform dialling resolution for default phone number?

Field type: TBool. This field may not be left as NULL. */
#define ISP_DIAL_RESOLUTION								_S("DialResolution")				// Perform Dialing Resolution for default phone number?
/** Use login script?

Field type: TBool. This field may not be left as NULL. */
#define ISP_USE_LOGIN_SCRIPT							_S("UseLoginScript")				// Use login script?
/** Login script (used if ISP_USE_LOGIN_SCRIPT is ETrue).

Type Text. This field may be left as NULL. */
#define ISP_LOGIN_SCRIPT								_S("LoginScript")					// Login Script
/** Prompt user for username and password?

Field type: TBool. This field may not be left as NULL. */
#define ISP_PROMPT_FOR_LOGIN							_S("PromptForLogin")				// Prompt user for username and password?
/** Login name

Field type: Text. This field may be left as NULL. */
#define ISP_LOGIN_NAME									_S("LoginName")						// Login name
/** Login Password

Field type: Text. This field may be left as NULL. */
#define ISP_LOGIN_PASS									_S("LoginPass")						// Login password
/** Identifier of the chargecard used for this ISP (optional).

Field type: TUint32. This field may be left as NULL. */
#define ISP_CHARGECARD									_S("Chargecard")
/** Display post connection terminal (PCT) (without scanning script for READ command)?

Field type: TBool. This field may be left as NULL. */
#define ISP_DISPLAY_PCT									_S("DisplayPCT")					// Display PCT (do not scan script for READ)?
//#define ISP_IF_NAME									SERVICE_IF_NAME
/** Interface parameter string

Field type: Text. This field may be left as NULL. */
#define ISP_IF_PARAMS									SERVICE_IF_PARAMS
/** Comma separated list of network protocols, e.g. "IP"

Field type: Text. This field may be left as NULL. */
#define ISP_IF_NETWORKS									SERVICE_IF_NETWORKS

/** Prompt user for authentication username and password?

Field type: TBool. This field may not be left as NULL. */
#define ISP_IF_PROMPT_FOR_AUTH							SERVICE_IF_PROMPT_FOR_AUTH
/** Authentication username used by PPP

Field type: Text. This field may be left as NULL. */
#define ISP_IF_AUTH_NAME								SERVICE_IF_AUTH_NAME
/** Authentication password used by PPP

Field type: Text. This field may be left as NULL. */
#define ISP_IF_AUTH_PASS								SERVICE_IF_AUTH_PASS
/** Number of retries if authentication fails.

Field type: TUint32. This field may be left as NULL. */
#define ISP_IF_AUTH_RETRIES								SERVICE_IF_AUTH_RETRIES
/** Callback enabled?

Field type: TBool. This field may be left as NULL. */
#define ISP_IF_CALLBACK_ENABLED							SERVICE_IF_CALLBACK_ENABLED
/** TCallbackAction value indicating type of callback (if enabled)

Field type: TUint32. This field may be left as NULL. */
#define ISP_IF_CALLBACK_TYPE							SERVICE_IF_CALLBACK_TYPE
/** Callback information, e.g. phone number to call back to.

Field type: 8-Bit Text. This field may be left as NULL. */
#define ISP_IF_CALLBACK_INFO							SERVICE_IF_CALLBACK_INFO
/** Time to wait for callback in microseconds (if enabled)

Field type: TUint32. This field may be left as NULL. */
#define ISP_CALLBACK_TIMEOUT							SERVICE_CALLBACK_TIMEOUT
/** PPP in server mode?

Field type: TBool. This field may be left as NULL. */
#define ISP_IF_SERVER_MODE								SERVICE_IF_SERVER_MODE

/** Get IP address from server?

Field type: TBool. This field may not be left as NULL. */
#define ISP_IP_ADDR_FROM_SERVER							SERVICE_IP_ADDR_FROM_SERVER
/** IP address of interface

Field type: Text. This field may be left as NULL. */
#define ISP_IP_ADDR										SERVICE_IP_ADDR
/** IP net mask of interface

Field type: Text. This field may be left as NULL. */
#define ISP_IP_NETMASK									SERVICE_IP_NETMASK
/** IP address of gateway

Field type: Text. This field may be left as NULL. */
#define ISP_IP_GATEWAY									SERVICE_IP_GATEWAY
/** Get DNS addresses from server?

Field type: TBool. This field may not be left as NULL. */
#define ISP_IP_DNS_ADDR_FROM_SERVER						SERVICE_IP_DNS_ADDR_FROM_SERVER
/** IP address of primary name server

Field type: Text. This field may be left as NULL. */
#define ISP_IP_NAME_SERVER1								SERVICE_IP_NAME_SERVER1
/** IP address of secondary name server

Field type: Text. This field may be left as NULL. */
#define ISP_IP_NAME_SERVER2								SERVICE_IP_NAME_SERVER2

#define ISP_IP6_DNS_ADDR_FROM_SERVER					SERVICE_IP6_DNS_ADDR_FROM_SERVER
#define ISP_IP6_NAME_SERVER1							SERVICE_IP6_NAME_SERVER1
#define ISP_IP6_NAME_SERVER2							SERVICE_IP6_NAME_SERVER2

#define ISP_IP_ADDR_LEASE_VALID_FROM					SERVICE_IP_ADDR_LEASE_VALID_FROM
#define ISP_IP_ADDR_LEASE_VALID_TO						SERVICE_IP_ADDR_LEASE_VALID_TO

/** Name of the ECOM configuration daemon manager component.
This component interfaces with the server identified in ISP_CONFIG_DAEMON_NAME.
If specified, ISP_CONFIG_DAEMON_NAME should also be specified.

Field type: Text. This field may be left as NULL.  */
#define ISP_CONFIG_DAEMON_MANAGER_NAME					SERVICE_CONFIG_DAEMON_MANAGER_NAME
/** Name of the configuration daemon server. This server is used to
provide further configuration for a connection, e.g. dynamic IP
address assignment. If specified, ISP_CONFIG_DAEMON_MANAGER_NAME
should also be specified.

Field type: Text. This field may be left as NULL.  */
#define ISP_CONFIG_DAEMON_NAME							SERVICE_CONFIG_DAEMON_NAME

/** Enable IP header compression

Field type: TBool. This field may be left as NULL. */
#define ISP_ENABLE_IP_HEADER_COMP		 				SERVICE_ENABLE_IP_HEADER_COMP
/** Whether to enable LCP extensions.

Field type: TBool. This field may be left as NULL. */
#define ISP_ENABLE_LCP_EXTENSIONS						SERVICE_ENABLE_LCP_EXTENSIONS
/** Disable plain text authentication?

Field type: TBool. This field may be left as NULL. */
#define ISP_DISABLE_PLAIN_TEXT_AUTH						SERVICE_DISABLE_PLAIN_TEXT_AUTH
/** Enable software compression (e.g. Microsoft, STAC or Predictor compression)?

Field type: TBool. This field may be left as NULL. */
#define ISP_ENABLE_SW_COMP								SERVICE_ENABLE_SW_COMP

/** The name of the bearer (RMobileCall::TMobileCallDataProtocol value)

Field type: TUint32. This field may be left as NULL. */
#define ISP_BEARER_NAME									_S("BearerName")					// Bearer Name (for GSM)
/** Connection speed if this is an ISDN, HSCSD or other connection (RMobileCall::TMobileCallDataSpeed
value).

Field type: TUint32. This field may be left as NULL. */
#define ISP_BEARER_SPEED								_S("BearerSpeed")					// Bearer Speed (for GSM)
/** The bearer CE (RMobileCall::TMobileCallTchCoding value)

Field type: TUint32. This field may be left as NULL. */
#define ISP_BEARER_CE									_S("BearerCE")						// Bearer CE (for GSM)
/** Modem initialization string to be used when this ISP is in use. This string
will be copied into the MODEM_ISP_INIT_STRING field by RGenericAgent.

Field type: TUint32. This field may be left as NULL. */
#define ISP_INIT_STRING									_S("InitString")					// Init string specific to ISP, to be written into the MODEM_ISP_INIT_STRING by NetDial

/** TCommsDbBearerType value indicating connection type, e.g. Standard GSM, HSCSD.

Field type: TUint32. This field may be left as NULL. */
#define ISP_BEARER_TYPE									_S("BearerType")					// HSCSD or CSD?
/** RMobileCall::TMobileCallTchCoding value specifying HSCSD channel coding.

Field type: TUint32. This field may be left as NULL. */
#define ISP_CHANNEL_CODING								_S("ChannelCoding")					// Channel coding if HSCSD connection
/** RMobileCall::TMobileCallAiur value specifying AIUR for HSCSD.

Field type: TUint32. This field may be left as NULL. */
#define ISP_AIUR										_S("AIUR")							// AIUR is HSCSD connection
/** Requested number of time slots for HSCSD.

Field type: TUint32. This field may be left as NULL. */
#define ISP_REQUESTED_TIME_SLOTS						_S("RequestedTimeSlots")			// Requested time slots if HSCSD connection
/** Maximum number of time slots for HSCSD which could be requested during this
connection.

Field type: TUint32. This field may be left as NULL. */
#define ISP_MAXIMUM_TIME_SLOTS							_S("MaximumTimeSlots")				// Maximum time slots if HSCSD connection

/** Reserved for future use.

Field type: Text. This field may be left as NULL. */
#define ISP_AUTHENTICATION								_S("Authentication")				// Authentication table/group (dial in only)

/** Enum of type RMobileCall::TMobileCallDataService.

Field type: TUint32. This field may be left as NULL. */
#define ISP_BEARER_SERVICE								_S("BearerService")
/** Enum of type RMobileCall::TMobileCallDataProtocol.

Field type: TUint32. This field may be left as NULL. */
#define ISP_BEARER_PROTOCOL								_S("BearerProtocol")
/** Enum of type RMobileCall::TMobileCallDataRLPVersion.

Field type: TUint32. This field may be left as NULL. */
#define ISP_RLP_VERSION									_S("RlpVersion")
/** Integer specifying network to MS window size.

Field type: TUint32. This field may be left as NULL. */
#define ISP_IWF_TO_MS									_S("IwfToMs")
/** Integer specifying MS to network window size.

Field type: TUint32. This field may be left as NULL. */
#define ISP_MS_TO_IWF									_S("MsToIwf")
/** Integer specifying rlp ack timeout in microseconds.

Field type: TUint32. This field may be left as NULL. */
#define ISP_ACK_TIMER									_S("AckTimer")
/** Integer specifying rlp retransmission attempts.

Field type: TUint32. This field may be left as NULL. */
#define ISP_RETRANSMISSION_ATTEMPTS						_S("RetransmissionAttempts")
/** Integer specifying resequencing period.

Field type: TUint32. This field may be left as NULL. */
#define ISP_RESEQUENCE_PERIOD							_S("ResequencePeriod")
/** Enum of type RMobileCall::TMobileCallDataV42bis.

Field type: TUint32. This field may be left as NULL. */
#define ISP_V42_COMPRESSION								_S("V42Compression")
/** Integer specifying V42bis codewords.

Field type: TUint32. This field may be left as NULL. */
#define ISP_V42_CODEWORDS								_S("V42Codewords")
/** Integer specifying V42bis maximum string length.

Field type: TUint32. This field may be left as NULL. */
#define ISP_V42_MAX_LENGTH								_S("V42MaxLength")
/** Enum of type RMobileCall::TMobileCallAsymmetry for HSCSD.

Field type: TUint32. This field may be left as NULL. */
#define ISP_ASYMMETRY									_S("Asymmetry")
/** HSCSD parameter - user init upgrade?

Field type: TBool. This field may be left as NULL. */
#define ISP_USER_INIT_UPGRADE							_S("UserInitUpgrade")
/** Use edge?

Field type: TBool. This field may be left as NULL. */
#define ISP_USE_EDGE									_S("UseEdge")



// Columns for `OUTGOING_WCDMA` and `INCOMING_WCDMA` tables.
// The following macros define values for the outgoing and incoming WCDMA table fields.
// Both tables have the same set of fields.
/** Access point name.

Field type: Text. This field may be left as NULL. */
#define GPRS_APN										_S("APN")

/** PDP (Packet Data Protocol, e.g. IP) type.

Field type: TUint32. This field may not be left as NULL. */
#define GPRS_PDP_TYPE									_S("PDPType")
/** PDP address of the phone.

Field type: Text. This field may be left as NULL. */
#define GPRS_PDP_ADDRESS								_S("PDPAddress")		// Phone IP address

/** Requested quality of service precedence class.

Field type: TUint32 from RPacketQoS::TQoSPrecedence. This field may be left as NULL. */
#define GPRS_REQ_PRECEDENCE								_S("ReqPrecedence")
/** Requested quality of service delay class.

Field type: TUint32 from RPacketQoS::TQoSDelay. This field may be left as NULL. */
#define GPRS_REQ_DELAY									_S("ReqDelay")
/** Requested quality of service reliability class.

Field type: TUint32 from RPacketQoS::TQoSReliability. This field may be left as NULL.*/
#define GPRS_REQ_RELIABILITY							_S("ReqReliability")
/** Requested quality of service peak throughput.

Field type: TUint32 from RPacketQoS::TQoSPeakThroughput. This field may be left as NULL. */
#define GPRS_REQ_PEAK_THROUGHPUT						_S("ReqPeakThroughput")
/** Requested quality of service mean throughput class.

Field type: TUint32 from RPacketQoS::TQoSMeanThroughput. This field may be left as NULL. */
#define GPRS_REQ_MEAN_THROUGHPUT						_S("ReqMeanThroughput")

/** Minimum quality of service precedence class.

Field type: TUint32 from RPacketQoS::TQoSPrecedence. This field may be left as NULL. */
#define GPRS_MIN_PRECEDENCE								_S("MinPrecedence")
/** Minimum quality of service delay class.

Field type: TUint32 from RPacketQoS::TQoSDelay. This field may be left as NULL. */
#define GPRS_MIN_DELAY									_S("MinDelay")
/** Minimum quality of service reliability class.

Field type: TUint32 from RPacketQoS::TQoSReliability. This field may be left as NULL.*/
#define GPRS_MIN_RELIABILITY							_S("MinReliability")
/** Minimum quality of service peak throughput class.

Field type: TUint32 from RPacketQoS::TQoSMeanThroughput. This field may be left as NULL. */
#define GPRS_MIN_PEAK_THROUGHPUT						_S("MinPeakThroughput")
/** Minimum quality of service mean throughput class.

Field type: TUint32 from RPacketQoS::TQoSMeanThroughput. This field may be left as NULL. */
#define GPRS_MIN_MEAN_THROUGHPUT						_S("MinMeanThroughput")

/** Whether data compression is on.

Field type: TBool. This field may be left as NULL. */
#define GPRS_DATA_COMPRESSION							_S("DataCompression")
/** Whether IP header compression is on.

Field type: TBool. This field may be left as NULL. */
#define GPRS_HEADER_COMPRESSION							_S("HeaderCompression")

/** Whether data compression is on and what type of compression should be used when using UMTS R5
Field type: TDataCompression. This field may be left as NULL. */
#define GPRS_R5_DATA_COMPRESSION						_S("R5DataCompression")

/** Whether IP header compression is on and what type of compression should be used when using UMTS R5
Field type: THeaderCompression. This field may be left as NULL. */
#define GPRS_R5_HEADER_COMPRESSION						_S("R5HeaderCompression")

/** Packet flow indicator - See table 10.5.161 of TS 24.008
Field type: TPacketFlowIdentifier. This field may be left as NULL. */
#define GPRS_R4_PACKET_FLOW_IDENTIFIER                  _S("PacketFlowIdentifier")

/** Identifies the current UMTS/GPRS release in use.
Field type: TUmtsGprsRelease. This field may be left as NULL. */
#define GPRS_UMTS_GPRS_RELEASE                          _S("UmtsGprsRelease")

/** Whether use of anonymous access is on.

Field type: TBool. This field may be left as NULL. */
#define GPRS_ANONYMOUS_ACCESS							_S("AnonymousAccess")

/** Use edge?

Field type: TBool. This field may be left as NULL. */
#define GPRS_USE_EDGE									_S("GprsUseEdge")

//#define GPRS_IF_NAME									SERVICE_IF_NAME
/** Interface parameter string.

Field type: Text. This field may be left as NULL. */
#define GPRS_IF_PARAMS									SERVICE_IF_PARAMS
/** Comma separated list of network protocols, e.g. "PPP".

Field type: Text. This field may be left as NULL. */
#define GPRS_IF_NETWORKS								SERVICE_IF_NETWORKS
/** Whether to prompt user for authentication username and password.

Field type: TBool. This field may not be left as NULL. */
#define GPRS_IF_PROMPT_FOR_AUTH							SERVICE_IF_PROMPT_FOR_AUTH
/** Authentication username used by PPP.

Field type: Text. This field may be left as NULL. */
#define GPRS_IF_AUTH_NAME								SERVICE_IF_AUTH_NAME
/** Authentication password used by PPP.

Field type: Text. This field may be left as NULL. */
#define GPRS_IF_AUTH_PASS								SERVICE_IF_AUTH_PASS
/** Number of times to retry authentication if it fails.

Field type: TUint32. This field may be left as NULL. */
#define GPRS_IF_AUTH_RETRIES							SERVICE_IF_AUTH_RETRIES

/** IP net mask of interface.

Field type: Text. This field may be left as NULL. */
#define GPRS_IP_NETMASK									SERVICE_IP_NETMASK
/** IP address of the gateway.

Field type: Text. This field may be left as NULL. */
#define GPRS_IP_GATEWAY									SERVICE_IP_GATEWAY
/** Whether to get IP addresses (for the Symbian phone) from the server.

Field type: TBool. This field may not be left as NULL. */
#define GPRS_IP_ADDR_FROM_SERVER						SERVICE_IP_ADDR_FROM_SERVER
/** IP address of the Symbian phone

Field type: Text. This field may be left as NULL. */
#define GPRS_IP_ADDR									SERVICE_IP_ADDR						// EPOC IP address
/** Whether to get DNS addresses from the server.

Field type: TBool. This field may not be left as NULL. */
#define GPRS_IP_DNS_ADDR_FROM_SERVER					SERVICE_IP_DNS_ADDR_FROM_SERVER
/** IP address of the primary name server.

Field type: Text. This field may be left as NULL. */
#define GPRS_IP_NAME_SERVER1							SERVICE_IP_NAME_SERVER1
/** IP address of the secondary name server.

Field type: Text. This field may be left as NULL. */
#define GPRS_IP_NAME_SERVER2							SERVICE_IP_NAME_SERVER2

#define GPRS_IP6_DNS_ADDR_FROM_SERVER					SERVICE_IP6_DNS_ADDR_FROM_SERVER
#define GPRS_IP6_NAME_SERVER1							SERVICE_IP6_NAME_SERVER1
#define GPRS_IP6_NAME_SERVER2							SERVICE_IP6_NAME_SERVER2

#define GPRS_IP_ADDR_LEASE_VALID_FROM					SERVICE_IP_ADDR_LEASE_VALID_FROM
#define GPRS_IP_ADDR_LEASE_VALID_TO						SERVICE_IP_ADDR_LEASE_VALID_TO

/** Name of the ECOM configuration daemon manager component.
This component interfaces with the server identified in GPRS_CONFIG_DAEMON_NAME.
If specified, GPRS_CONFIG_DAEMON_NAME should also be specified.

Field type: Text. This field may be left as NULL.  */
#define GPRS_CONFIG_DAEMON_MANAGER_NAME					SERVICE_CONFIG_DAEMON_MANAGER_NAME
/** Name of the configuration daemon server. This server is used to
provide further configuration for a connection, e.g. dynamic IP
address assignment. If specified, GPRS_CONFIG_DAEMON_MANAGER_NAME
should also be specified.

Field type: Text. This field may be left as NULL.  */
#define GPRS_CONFIG_DAEMON_NAME							SERVICE_CONFIG_DAEMON_NAME

/** Whether to enable LCP extensions.

Field type: TBool. This field may be left as NULL. */
#define GPRS_ENABLE_LCP_EXTENSIONS						SERVICE_ENABLE_LCP_EXTENSIONS
/** Whether to disable plain text authentication.

Field type: TBool. This field may be left as NULL. */
#define GPRS_DISABLE_PLAIN_TEXT_AUTH					SERVICE_DISABLE_PLAIN_TEXT_AUTH

/** Service supports Internet only, WAP only or both.

Field type: TUint32. This field may be left as NULL. */
#define	GPRS_AP_TYPE									_S("GprsAccessPointType")

/** If the requested QOS can not be satisfied warn the user after this time in microseconds.
Set to 0xffffffff to disable.

Field type: TUint32. This field may be left as NULL. */
#define GPRS_QOS_WARNING_TIMEOUT						_S("QosWarningTimeout")


/**
Link to the Gprs R99 Parameters Table
Field type: TUint32. This field may be left as NULL
*/
#define QOS_UMTS_R99_AND_ON_TABLE 						_S("UmtsR99QoSAndOn")

//
// Columns of the `QOS R99 Parameters` Table.
//
#define GPRS_QOS_REQ_TRAFFIC_CLASS              _S("ReqTrafficClass")
#define GPRS_QOS_MIN_TRAFFIC_CLASS              _S("MinTrafficClass")
#define GPRS_QOS_REQ_DELIVERY_ORDER	            _S("ReqDeliveryOrder")
#define GPRS_QOS_MIN_DELIVERY_ORDER	            _S("MinDeliveryOrder")
#define GPRS_QOS_REQ_DELIVER_ERRONEOUS_SDU      _S("ReqDeliverErroneousSDU")
#define GPRS_QOS_MIN_DELIVER_ERRONEOUS_SDU      _S("MinDeliverErroneousSDU")
#define GPRS_QOS_REQ_MAX_SDUSIZE                _S("ReqMaxSDUSize")
#define GPRS_QOS_MIN_ACCEPTABLE_MAX_SDU_SIZE    _S("MinAcceptableMaxSDUSize")
#define GPRS_QOS_REQ_MAX_UPLINK_RATE            _S("ReqMaxUplinkRate")
#define GPRS_QOS_REQ_MIN_UPLINK_RATE            _S("ReqMinUplinkRate")
#define GPRS_QOS_REQ_MAX_DOWNLINK_RATE          _S("ReqMaxDownlinkRate")
#define GPRS_QOS_REQ_MIN_DOWNLINK_RATE          _S("ReqMinDownlinkRate")
#define GPRS_QOS_REQ_BER                        _S("ReqBER")
#define GPRS_QOS_MAX_BER                        _S("MaxBER")
#define GPRS_QOS_REQ_SDU_ERROR_RATIO            _S("ReqSDUErrorRatio")
#define GPRS_QOS_MAX_SDU_ERROR_RATIO            _S("MaxSDUErrorRatio")
#define GPRS_QOS_REQ_TRAFFIC_HANDLING_PRIORITY  _S("ReqTrafficHandlingPriority")
#define GPRS_QOS_MIN_TRAFFIC_HANDLING_PRIORITY  _S("MinTrafficHandlingPriority")
#define GPRS_QOS_REQ_TRANSFER_DELAY             _S("ReqTransferDelay")
#define GPRS_QOS_MAX_TRANSFER_DELAY             _S("MaxTransferDelay")
#define GPRS_QOS_REQ_GUARANTEED_UPLINK_RATE     _S("ReqGuaranteedUplinkRate")
#define GPRS_QOS_MIN_GUARANTEED_UPLINK_RATE     _S("MinGuaranteedUplinkRat")
#define GPRS_QOS_REQ_GUARANTEED_DOWNLINK_RATE   _S("ReqGuaranteedDownlinkRate")
#define GPRS_QOS_MIN_GUARANTEED_DOWNLINK_RATE   _S("MinGuaranteedDownlinkRate")
#define GPRS_QOS_SIGNALLING_INDICATION          _S("SignallingIndication")
#define GPRS_QOS_IM_CN_SIGNALLING_INDICATOR     _S("IMCNSignallingIndication")
#define GPRS_QOS_SOURCE_STATISTICS_DESCRIPTOR   _S("SourceStatisticsDescriptor")


//
// Columns of the `LAN_SERVICE` Table.
//
#define LAN_IF_NETWORKS									SERVICE_IF_NETWORKS
#define LAN_IP_NETMASK									SERVICE_IP_NETMASK
#define LAN_IP_GATEWAY									SERVICE_IP_GATEWAY
#define LAN_IP_ADDR_FROM_SERVER							SERVICE_IP_ADDR_FROM_SERVER
#define LAN_IP_ADDR										SERVICE_IP_ADDR
#define LAN_IP_DNS_ADDR_FROM_SERVER						SERVICE_IP_DNS_ADDR_FROM_SERVER
#define LAN_IP_NAME_SERVER1								SERVICE_IP_NAME_SERVER1
#define LAN_IP_NAME_SERVER2								SERVICE_IP_NAME_SERVER2
#define LAN_IP6_DNS_ADDR_FROM_SERVER					SERVICE_IP6_DNS_ADDR_FROM_SERVER
#define LAN_IP6_NAME_SERVER1							SERVICE_IP6_NAME_SERVER1
#define LAN_IP6_NAME_SERVER2							SERVICE_IP6_NAME_SERVER2

#define LAN_IP_ADDR_LEASE_VALID_FROM					SERVICE_IP_ADDR_LEASE_VALID_FROM
#define LAN_IP_ADDR_LEASE_VALID_TO						SERVICE_IP_ADDR_LEASE_VALID_TO
#define LAN_CONFIG_DAEMON_MANAGER_NAME					SERVICE_CONFIG_DAEMON_MANAGER_NAME
#define LAN_CONFIG_DAEMON_NAME							SERVICE_CONFIG_DAEMON_NAME

/** used when Bluetooth PAN profile or WLAN is in use */
#define LAN_SERVICE_EXTENSION_TABLE_NAME				_S("LanServiceExtensionTableName")
#define LAN_SERVICE_EXTENSION_TABLE_RECORD_ID			_S("LanServiceExtensionTableRecordId")

//
// Columns of the `PAN_SERVICE_EXTENSION` Table.
//

/** PAN role that the local device will act in. To dynamically select a role, use ECommDbPanRoleUnknown.
Field type: TCommDbBluetoothPanRole.*/
#define PAN_LOCAL_ROLE									_S("LocalRole")
/** PAN role that the remote device will act in. To dynamically select a role, use ECommDbPanRoleUnknown.
Field type: TCommDbBluetoothPanRole.*/
#define PAN_PEER_ROLE									_S("PeerRole")
/** A list of Bluetooth MAC addresses.  If one address is specified, then PAN agent will connect to it.  If more than one device address is specified (in a comma seperated list), then PAN agent will discover whether any of the devices are in range, and connect to one of them.
Field type: Comma-separated list of Bluetooth MAC addresses, with no "0x" preamble and no colons separating the digits*/
#define PAN_PEER_MAC_ADDRESSES							_S("PeerMACAddresses")
/** Whether the agent should prompt the user to select the remote devices to which we will connect.
Field type: TBool*/
#define PAN_PROMPT_FOR_REMOTE_DEVICES					_S("PromptForRemoteDevices")
/** Whether the remote role defined above should be accepted without performing an SDP query to ensure the remote device supports the specified role.  Not recommended for use.
Field type: TBool*/
#define PAN_DISABLE_SDP_QUERY							_S("DisableSdpQuery")
/** Whether the PAN agent should support incoming connections.
Field type: TBool*/
#define PAN_ALLOW_INCOMING								_S("AllowIncomingConnections")
/** Whether the agent should prompt the user to select the remote device to which we will connect if we cannot connect to any device specified in the MAC list.
Field type: TBool*/
#define PAN_PROMPT_IF_MAC_LIST_FAILS					_S("PromptIfMACListFails")
/** Whether the device is enabled for NAP service (to be used as a modem) for a particular connection
Field type: TBool
*/
#define PAN_NAP_SERVICE_ENABLED							_S("NapServiceEnabled")

/** Columns of all bearer tables */
#define AGENT_NAME								_S("Agent")
#define LAST_SOCKET_ACTIVITY_TIMEOUT				_S("LastSocketActivityTimeout")
#define LAST_SESSION_CLOSED_TIMEOUT				_S("LastSessionClosedTimeout")
#define LAST_SOCKET_CLOSED_TIMEOUT				_S("LastSocketClosedTimeout")

// Columns of the `MODEM BEARER` table.
// The following macros define values for the Modem Bearer table fields.
// Note that in v7.0s, this table changed its name from MODEM to MODEM_BEARER.
// In v6.1 there can be only a single record in this table.
/** Agent identifier.

Field type: TUint32. This field may be left as NULL. */
#define MODEM_AGENT										AGENT_NAME
/** NIF name.

Field type: Text. This field may be left as NULL. */
#define MODEM_NIF_NAME									IF_NAME
/** BCA (Baseband Channel Adaptor) Stack.

Field type: Text. This field may be left as NULL. */
#define MODEM_BCA_STACK		    						BCA_STACK
/** Comm port name.

Field type: Text. This field may be left as NULL. */
#define MODEM_PORT_NAME									_S("PortName")
/** The modem control channel port name.

Field type: Text. This field may be left as NULL. */
#define MODEM_CONTROL_CHANNEL_PORT_NAME					_S("ControlChannelPortName")
/** TSY name.

Field type: Text. This field may be left as NULL. */
#define MODEM_TSY_NAME									_S("TSYName")
/** CSY name.

Field type: Text. This field may be left as NULL. */
#define MODEM_CSY_NAME									_S("CSYName")
/** Data bits (TDataBits value).

Field type: TUint8. This field may not be left as NULL. */
#define	MODEM_DATA_BITS									_S("DataBits")						// Data Bits to use for ISP or Modem
/** Stop bits.

This is a TStopBits value.

Field type: TUint8. This field may not be left as NULL. */
#define	MODEM_STOP_BITS									_S("StopBits")						// Stop Bits to use for ISP or Modem
/** Parity.

This is a TParity value.

Field type: TUint8. This field may not be left as NULL. */
#define	MODEM_PARITY									_S("Parity")						// Parity Type to use for ISP or Modem
/** Baud rate.

This is a TBps value.

Field type: TUint32. This field may not be left as NULL. */
#define MODEM_RATE										_S("Rate")
/** Handshaking.

Field type: TUint32. This field may be left as NULL. */
#define MODEM_HANDSHAKING								_S("Handshaking")
/** Special rate.

Field type: TUint32. This field may be left as NULL. */
#define MODEM_SPECIAL_RATE								_S("SpecialRate")
/** Xon character.

Field type: TUint8. This field may be left as NULL. */
#define MODEM_XON_CHAR									_S("XonChar")
/** Xoff Character.

Field type: TUint8. This field may be left as NULL. */
#define MODEM_XOFF_CHAR									_S("XoffChar")

/** Preferred fax class.

This is a TFaxClass value.

Field type: TUint8. This field may not be left as NULL. */
#define MODEM_FAX_CLASS_PREF							_S("FaxClassPref")
/** Preferred speaker mode.

This is a TCommsDbModemSpeakerSetting value.

Field type: TUint8. This field may not be left as NULL. */
#define MODEM_SPEAKER_PREF								_S("SpeakerPref")
/** Preferred speaker volume.

This is a TCommsDbModemSpeakerVolume value.

Field type: TUint8. This field may not be left as NULL. */
#define MODEM_SPEAKER_VOL_PREF							_S("SpeakerVolPref")

/** General modem initialisation string

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_MODEM_INIT_STRING							_S("ModemInitString")
/** Data initialisation string

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_DATA_INIT_STRING							_S("DataInitString")
/** Fax initialisation string.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_FAX_INIT_STRING							_S("FaxInitString")
/** ISP Initialisation string. This field is written from the ISP_INIT_STRING.  Should not be accessed by user.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_ISP_INIT_STRING							_S("IspInitString")				// To be written by NetDial from ISP_INIT_STRING.

/** Command to modify the pause created during dialling using the comma character
(',')

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_DIAL_PAUSE_LENGTH							_S("DialPauseLength")
/** Command to set the time out the modem uses when establishing a link before
giving up and returning to command mode.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_CARRIER_TIMEOUT							_S("CarrierTimeout")
/** Command to set the number of rings before the modem auto answers.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_AUTO_ANSWER_RING_COUNT					_S("AutoAnswerRingCount")
/** Command to set the modem speaker volume to low.

Field type: 8-Bit Text. This field may be left as NULL. */
// This field is left misspelled to preserve backward compatibility
#define MODEM_SPEAKER_VOL_CONTROL_LOW					_S("SpeakerVolContorlLow")
/** Command to set the modem speaker volume to medium.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_SPEAKER_VOL_CONTROL_MEDIUM				_S("SpeakerVolControlMedium")
/** Command to set the modem speaker volume to high.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_SPEAKER_VOL_CONTROL_HIGH					_S("SpeakerVolControlHigh")
/** Command to set the modem speaker off.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_SPEAKER_ALWAYS_OFF						_S("SpeakerAlwaysOff")
/** Command to set the modem speaker on until the carrier.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_SPEAKER_ON_UNTIL_CARRIER					_S("SpeakerOnUntilCarrier")
/** Command to set the modem speaker on

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_SPEAKER_ALWAYS_ON							_S("SpeakerAlwaysOn")
/** Command to set the modem speaker on except during dialling.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_SPEAKER_ON_AFTER_DIAL_UNTIL_CARRIER		_S("SpeakerOnAfterUntilCarrier")
/** The dial command modifier to wait for dial tone.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_DIAL_TONE_WAIT_MODIFIER					_S("DialToneWaitModifier")
/** Disable busy and dial tone detection.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_CALL_PROGRESS_1							_S("CallProgress1")					// Dialtone & busy detection disabled, result codes 0-5,10
/** Dial tone detection enabled, busy detection disabled.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_CALL_PROGRESS_2							_S("CallProgress2")					// Dialtone detection enabled & busy detection disabled, result codes 0-6,10
/** Dial tone detection disabled, busy detection enabled.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_CALL_PROGRESS_3							_S("CallProgress3")					// Dialtone detection disabled , but busy detection enabled, result codes 0-5,7,10
/** Dial tone and busy detection enabled.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_CALL_PROGRESS_4							_S("CallProgress4")					// Dialtone detection & busy detection enabled, result codes 0-7,10
/** Switch echo mode off.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_ECHO_OFF									_S("EchoOff")
/** Switch verbose mode on.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_VERBOSE_TEXT								_S("VerboseText")
/** Switch quiet mode on.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_QUIET_ON									_S("QuietOn")
/** Switch quiet mode off.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_QUIET_OFF									_S("QuietOff")
/** Dial command modifier to return to command mode after dialling.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_DIAL_COMMAND_STATE_MODIFIER				_S("DialCommandStateModifier")
/** Enter on-line mode from on-line command mode.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_ON_LINE									_S("OnLine")
/** Reset the modem configuration.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_RESET_CONFIGURATION						_S("ResetConfiguration")
/** Return the modem configuration to its factory defaults.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_RETURN_TO_FACTORY_DEFS					_S("ReturnToFactoryDefs")
/** Command the modem to only assert DCD when a carrier is actually detected, i.e.
while the link is up.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_DCD_ON_DURING_LINK						_S("DCDOnDuringLink")
/** Command the modem to hang up the current call when the DTE drops the DTR line.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_DTR_HANG_UP								_S("DTRHangUp")
/** Command the modem to always assert DSR.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_DSR_ALWAYS_ON								_S("DSRAlwaysOn")
/** Command the modem to use RTS/CTS flow control.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_RTS_CTS_HANDSHAKE							_S("RTSCTSHandshake")
/** Command the modem to use software flow control.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_XON_XOFF_HANDSHAKE						_S("XonXoffHandshake")
/** The character used by the DTE to return to command mode from on-line mode.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_ESCAPE_CHARACTER							_S("EscapeCharacter")
/** Command the modem to use a particular escape sequence guard period.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_ESCAPE_GUARD_PERIOD						_S("EscapeGuardPeriod")
/** Ask the modem which fax modes are supported.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_FAX_CLASS_INTERROGATE						_S("FaxClassInterrogate")
/** Set the fax mode.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_FAX_CLASS									_S("FaxClass")

/** Modem response when no dial tone is detected.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_NO_DIAL_TONE								_S("NoDialTone")
/** Modem response when a busy tone is detected.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_BUSY										_S("Busy")
/** Modem response when no answer is detected.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_NO_ANSWER									_S("NoAnswer")
/** Carrier report message.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_CARRIER									_S("Carrier")
/** Connection report message.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_CONNECT									_S("Connect")
/** Compression Class 5 report message.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_COMPRESSION_CLASS_5						_S("CompressionClass5")
/** Compression V.42 bis report message.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_COMPRESSION_V42BIS						_S("CompressionV42bis")
/** No compression report message.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_COMPRESSION_NONE							_S("CompressionNone")
/** LAPD protocol report message.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_PROTOCOL_LAPD								_S("ProtocolLAPD")
/** ALT protocol report message.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_PROTOCOL_ALT								_S("ProtocolALT")
/** ALT-CELLULAR report message.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_PROTOCOL_ALTCELLULAR						_S("ProtocolALTCELLULAR")
/** No protocol report message.

Field type: 8-Bit Text. This field may be left as NULL. */
#define MODEM_PROTOCOL_NONE								_S("ProtocolNone")

/** Phone number of message centre.

Field type: TUint16. This field may be left as NULL. */
#define MODEM_MESSAGE_CENTRE_NUMBER						_S("MessageCentreNumber")
/** Validity period for SMS in minutes.

Field type: TUint32. This field may be left as NULL. */
#define MODEM_MESSAGE_VALIDITY_PERIOD					_S("MessageValidityPeriod")
/** Whether to produce an SMS delivery report.

Field type: TBool. This field may be left as NULL. */
#define MODEM_MESSAGE_DELIVERY_REPORT					_S("MessageDeliveryReport")
/** Minimum signal level.

Field type: TUint32. This field may not be left as NULL. */
#define MODEM_MIN_SIGNAL_LEVEL							_S("MinimumSignalLevel")
/** SIR settings.
Field type: TUint32. This field may be left as NULL. */
#define MODEM_SIR_SETTINGS								_S("SIRSettings")

/** Whether comm port should be opened in the role of DTE or DCE
Contents is a bit mask (see KModemCommRoleDCE)

Field type: TUint32. This field may be left as NULL. */
#define MODEM_COMM_ROLE									_S("CommRole")

/*Field type: TUint32. This field may be left as NULL. This is a TCommsDbBearerType value. */
#define BEARER_TECHNOLOGY								_S("BearerTechnology")

// Columns of the 'LAN Bearer' table
#define LAN_BEARER_AGENT								AGENT_NAME
#define LAN_BEARER_NIF_NAME								IF_NAME
#define LAN_BEARER_LDD_FILENAME							_S("LDDFilename")
#define LAN_BEARER_LDD_NAME								_S("LDDName")
#define LAN_BEARER_PDD_FILENAME							_S("PDDFilename")
#define LAN_BEARER_PDD_NAME								_S("PDDName")
#define LAN_BEARER_PACKET_DRIVER_NAME					_S("PacketDriverName")

// Columns of the `LOCATION` table.
// The following macros define values for the Location table fields.
// Note that in v6.1 there can be only a single record in this table.
/** International prefix code.

Field type: Text. This field may be left as NULL. */
#define LOCATION_INTL_PREFIX_CODE						_S("IntlPrefixCode")
/** National prefix code.

Field type: Text. This field may be left as NULL. */
#define LOCATION_NAT_PREFIX_CODE						_S("NatPrefixCode")
/** National code.

Field type: Text. This field may be left as NULL. */
#define LOCATION_NAT_CODE								_S("NatCode")
/** Area code.

Field type: Text. This field may be left as NULL. */
#define LOCATION_AREA_CODE								_S("AreaCode")
/** Number to dial for an outside line.

Field type: Text. This field may be left as NULL. */
#define LOCATION_DIAL_OUT_CODE							_S("DialOutCode")
/** Code to dial to disable call waiting facility.

Field type: Text. This field may be left as NULL. */
#define LOCATION_DISABLE_CALL_WAITING_CODE				_S("DisableCallWaitingCode")
/** Mobile phone?

Field type: TBool. This field may not be left as NULL. */
#define LOCATION_MOBILE									_S("Mobile")
/** Use pulse dialling?

Field type: TBool. This field may not be left as NULL. */
#define LOCATION_USE_PULSE_DIAL							_S("UsePulseDial")
/** Wait for the dial tone?

Field type: TBool. This field may not be left as NULL. */
#define LOCATION_WAIT_FOR_DIAL_TONE						_S("WaitForDialTone")
/** Pause time after dial out.

Field type: TUint32. This field may not be left as NULL. */
#define LOCATION_PAUSE_AFTER_DIAL_OUT					_S("PauseAfterDialout")

// Columns of the `CHARGECARD` table
// The following macros define values for the Chargecard table fields.
/** Account number.

Field type: Text. This field may be left as NULL. */
#define CHARGECARD_ACCOUNT_NUMBER						_S("AccountNumber")
/** PIN number.

Field type: Text. This field may be left as NULL. */
#define CHARGECARD_PIN									_S("Pin")
/** Order of dialling account number, PIN and phone number for local calls.

Field type: Text. This field may be left as NULL. */
#define CHARGECARD_LOCAL_RULE							_S("LocalRule")
/** Order of dialling account number, PIN and phone number for national calls.

Field type: Text. This field may be left as NULL. */
#define CHARGECARD_NAT_RULE								_S("NatRule")
/** Order of dialling account number, PIN and phone number for international calls.

Field type: Text. This field may be left as NULL. */
#define CHARGECARD_INTL_RULE							_S("IntlRule")

// Columns of the `PROXIES` table.
// The following macros define values for the Proxies table fields.
/** Identifier of the associated ISP record.

Field type: TUint32. This field may not be left as NULL.*/
#define PROXY_ISP										_S("ISP")							// ISP with which these proxies are associated
/** The table from which the PROXY_ISP has been taken.

This is necessary as GPRS records as well as ISP records can have proxy settings
associated with them.

If this field is read and its length is zero, the value DIAL_OUT_ISP is returned,
as it is assumed that any client that has not written this field is assuming
that proxies are only available to the dial out ISP and not other service
types.

Field type: Text. This field may be left as NULL. */
#define PROXY_SERVICE_TYPE								_S("ProxyServiceType")
/** Use proxy server?

Field type: TBool. This field may not be left as NULL. */
#define PROXY_USE_PROXY_SERVER							_S("UseProxyServer")
/** Name of the host.

Field type: Long Text. This field may be left as NULL. */
#define PROXY_SERVER_NAME								_S("ProxyServerName")				// Name of the proxy server
/** Name of the protocol for which this proxy can be used.

Field type: Text. This field may not be left as NULL. */
#define PROXY_PROTOCOL_NAME								_S("ProtocolName")
/** Port number.

Field type: TUint32. This field may be left as NULL. */
#define PROXY_PORT_NUMBER								_S("PortNumber")
/** Semi-colon separated list of the addresses for which the proxy server should
not be used.

Field type: Long Text. This field may be left as NULL. */
#define PROXY_EXCEPTIONS								_S("Exceptions")

// Columns of the `WAP_ACCESS_POINT` table. The following macros define values
// for the WAP access point table fields.
/** Text string indicating the name of the table from which to read the bearer
information.

This can be WAP_IP_BEARER or WAP_SMS_BEARER.

Field type: Text. This field may not be left as NULL. */
#define WAP_CURRENT_BEARER								_S("CurrentBearer")
/** WAP start page URL.

Field type: Text. This field may be left as NULL. */
#define WAP_START_PAGE									_S("StartPage")

// Columns common to the `WAP_IP_BEARER` and `WAP_SMS_BEARER` tables.
// The following macros define values for the WAP IP bearer and WAP SMS bearer table fields.
// These tables have no COMMDB_NAME field.
/** ID of the  WAP Access Point record to which this bearer information refers.

Field type: TUint32. This field may not be left as NULL.*/
#define WAP_ACCESS_POINT_ID								_S("AccessPointId")
/** WAP gateway address: an IP address or phone number.

Field type: Text. This field may be left as NULL. */
#define WAP_GATEWAY_ADDRESS								_S("GatewayAddress")
/** Whether connection-oriented or connectionless API should be used.

This is a TCommsDbWapWspOption value.

Field type: TUint8. This field may be left as NULL. */
#define WAP_WSP_OPTION									_S("WSPOption")							// Connectionless or Connection-oriented
/** Attempt secure WTLS connection to the gateway.

Field type: TBool. This field may be left as NULL. */
#define WAP_SECURITY									_S("Security")

// Additional Columns of the `WAP_IP_BEARER` table.
/** Identifier of a record in the IAP table to be used.

Field type: TUint32. This field may be left as NULL. */
#define WAP_IAP											_S("IAP")
/** Proxy port number. Required for WAP2.0 only.

Field type: TUint32. This field may not be left as NULL. */
#define WAP_PROXY_PORT									_S("ProxyPortNumber")
/** Proxy login name. Required for WAP2.0 only.

Field type: Text. This field may be left as NULL. */
#define WAP_PROXY_LOGIN_NAME							_S("ProxyLogin")
/** Proxy login password. Required for WAP2.0 only.

Field type: Text. This field may be left as NULL. */
#define WAP_PROXY_LOGIN_PASS							_S("ProxyPassword")

// Additional Column of the `WAP_SMS_BEARER` table.
/** Service Centre Address.

Field type: Text. This field may be left as NULL. */
#define WAP_SERVICE_CENTRE_ADDRESS						_S("ServiceCentreAddress")

// The following macros define values for the Bluetooth device table fields.
// These tables have no COMMDB_NAME field.

// Column common to the `BT_DEVICE_TABLE` and `BT_SECURITY_TABLE` tables.
/** Bluetooth device address.

Field type: 8-bit Text. This field may not be left as NULL. */
#define BT_DEV_ADD										_S("DeviceAddress")

// Additional Columns of the `BT_DEVICE_TABLE` table.
/** Bluetooth device default name.

Field type: Long text. This field may be left as NULL. */
#define BT_DEV_DFLT_NAME								_S("DeviceDefaultName")
/** Bluetooth device friendly name.

Field type: Long text. This field may be left as NULL. */
#define BT_DEV_FRIENDLY_NAME							_S("DeviceFriendlyName")
/** Bluetooth device class.

Field type: TUint32. This field may be left as NULL. */
#define BT_DEVICE_CLASS									_S("DeviceClass")
/** Bluetooth Link key.

Field type: 8-bit Text. This field may be left as NULL. */
#define BT_LINKKEY										_S("DeviceLinkKey")
/** Bluetooth device state.

Field type: TUint32. This field may not be left as NULL. */
#define BT_DEV_STATE									_S("DeviceStatus")
/** Bluetooth device global security setting.

Field type: TUint32. This field may be left as NULL. */
#define BT_DEVICE_GLOBAL_SECURITY						_S("DeviceGlobalSecurity")
/** Bluetooth page scan repetition mode.

Field type: TUint32. This field may be left as NULL. */
#define BT_PAGE_SCAN_REP_MODE							_S("DevicePageScanRepMode")
/** Field type: TUint32. This field may be left as NULL. */
#define BT_PAGE_SCAN_PERIOD_MODE						_S("DevicePageScanPeriodMode")
/** Field type: TUint32. This field may be left as NULL. */
#define BT_PAGE_SCAN_MODE								_S("DevicePageScanMode")
/** Internal*/
#define BT_SPARE										_S("DeviceSpareInfo")
/** Bluetooth clock offset.

Field type: TUint32. This field may be left as NULL. */
#define BT_CLK_OFFSET									_S("DeviceClkOffset")
/** Bluetooth class of device.

Field type: TUint32. This field may be left as NULL. */
#define BT_COD											_S("DeviceClassOfDevice")

// Additional Columns of the `WAP_SMS_BEARER` table.
/** Bluetooth security UID.

Field type: TUint32. This field may be left as NULL. */
#define BT_SEC_UID										_S("BTSecurityUID")
/** Bluetooth security device.

Field type: TUint32. This field may be left as NULL. */
#define BT_SEC_DEVICE_SECURITY							_S("BTSecurityDevice")

// Columns of the `BT_DEFAULT_TABLE` table.
// The following macros define values for the Bluetooth Default table fields.
// This table has no COMMDB_NAME field.
/** Bluetooth default service name.

Field type: Text. This field may be left as NULL. */
#define BT_DFLT_SER_NAME								_S("DefaultServicename")
/** Bluetooth default service ID.

Field type: TUint32. This field may not be left as NULL. */
#define BT_DFLT_SER_UUID								_S("DefaultServiceUID")
/** Bluetooth default device address.

Field type: 8-bit Text. This field may not be left as NULL. */
#define BT_DFLT_DEV_ADD									_S("DefaultDeviceAddress")
/** Bluetooth default virtual port number.

Field type: TUint32. This field may not be left as NULL. */
#define BT_DFLT_VIRTUAL_PORT    						_S("DefaultVirtualPort")
/** Bluetooth default security level.

Field type: TUint8. This field may be left as NULL. */
#define BT_DFLT_SECURITY_LEVEL  						_S("DefaultSecurityLevel")


// Columns of the BT Persistance Table
// The following macros define values for the Bluetooth Persist table fields.
// This table has no COMMDB_NAME field.
// These items are all internal and not intended for use.
#define BT_PERSIST_COD									_S("ClassOfDevice")
#define BT_PERSIST_PAGE_TIMEOUT							_S("PageTimeout")
#define BT_PERSIST_PAGE_SCAN_INTERVAL					_S("PageScanInterval")
#define BT_PERSIST_PAGE_SCAN_WINDOW						_S("PageScanWindow")
#define BT_PERSIST_INQUIRY_SCAN_INTERVAL				_S("InquiryScanInterval")
#define BT_PERSIST_INQUIRY_SCAN_WINDOW					_S("InquiryScanWindow")
#define BT_PERSIST_HOLD_MODE_ACTIVITY					_S("HoldModeActivity")
#define BT_PERSIST_SCO_FLOW_CONTROL						_S("SCOFlowControl")
#define BT_PERSIST_PAGE_SCAN_PERIOD						_S("PageScanPeriod")
#define BT_PERSIST_PAGE_SCAN_MODE						_S("PageScanMode")
#define BT_PERSIST_POWER_SETTINGS						_S("PowerSettings")
#define BT_PERSIST_SCAN									_S("Scan")
#define BT_PERSIST_LOCAL_NAME							_S("LocalName")
#define BT_PERSIST_IAC_LAP								_S("IacLap")
#define BT_PERSIST_STATE								_S("PersistState")


// Columns for `SS_PROTO_TABLE` table
// The following macros define values for the SS Proto table fields.
// This table has no COMMDB_NAME field.
/** Secure Sockets protocol name.

Field type: Text. This field may not be left as NULL. */
#define SS_PROTO_NAME									_S("ProtocolName")
/** Secure Sockets protocol library.

Field type: Text. This field may not be left as NULL. */
#define SS_PROTO_LIBRARY								_S("ProtoLibrary")

// Columns of the `DEFAULT_GPRS` table.
// The following macros define values for the Default GPRS fields.
// GPRS Phase 1 requires that a default PDP (Packet Data Protocol, e.g. IP4)
// context configuration for GPRS is stored for use when no other configuration
// information is specified for a context by the terminal. This table is
// provided for this purpose. Defaults can be used when the device
// is used as a GPRS modem with an external device that does not specify GPRS
// parameters for the  connection.In release v6.1 the enums that are now in
// RPacketContext and RPacketQoS were in RGprsContext.The following macros
// define the fields for  the  table. The description is followed by the field
// type, and whether the specified field may be left null or not.
/** Specifies usage of this default.

Field type: TUint32. This field may be left as NULL. */
#define DEFAULT_GPRS_USAGE								_S("Usage")
/** Default access point name.

Field type: Text. This field may not be left as NULL. */
#define DEFAULT_GPRS_APN								_S("APN")

/** Default PDP type (e.g. IP4).

The enum RPacketContext::TProtocolType defines valid values for this field.

Field type: TUint32. This field may not be left as NULL. */
#define DEFAULT_GPRS_PDP_TYPE							_S("PDPType")
/** Default PDP address.

Field type: Text. This field may not be left as NULL. */
#define DEFAULT_GPRS_PDP_ADDRESS						_S("PDPAddress")

/** Default quality of service precedence class.

Values for this are defined by the enum RPacketQoS::TQoSPrecedence.

Field type: TUint32. This field may be left as NULL. */
#define DEFAULT_GPRS_PRECEDENCE							_S("Precedence")

/** Default quality of service delay class.

Values for this are defined by the enum RPacketQoS::TQoSDelay.

Field type: TUint32. This field may be left as NULL. */
#define DEFAULT_GPRS_DELAY								_S("Delay")

/** Default quality of service reliability class.

Values for this are defined by the enum RPacketQoS::TQoSReliability.

Field type: TUint32. This field may be left as NULL. */
#define DEFAULT_GPRS_RELIABILITY						_S("Reliability")

/** Default quality of service peak throughput class.

Values for this are defined by the enum RPacketQoS::TQoSPeakThroughput.

Field type: TUint32. This field may be left as NULL. */
#define DEFAULT_GPRS_PEAK_THROUGHPUT					_S("PeakThroughput")

/** Default quality of service mean throughput class.

Values for this are defined by the enum RPacketQoS::TQoSMeanThroughput.

Field type: TUint32. This field may be left as NULL. */
#define DEFAULT_GPRS_MEAN_THROUGHPUT					_S("MeanThroughput")

#define DEFAULT_GPRS_MIN_PRECEDENCE						_S("MinPrecedence")

/** Minimum quality of service delay class.

Values for this are defined by the enum RPacketQoS::TQoSDelay.

Field type: TUint32. This field may be left as NULL. */
#define DEFAULT_GPRS_MIN_DELAY							_S("MinDelay")

/** Default minimum quality of service reliability class.

Values for this are defined by the enum RPacketQoS::TQoSReliability.

Field type: TUint32. This field may be left as NULL. */
#define DEFAULT_GPRS_MIN_RELIABILITY					_S("MinReliability")

/** Default minimum quality of service peak throughput class.

Values for this are defined by the enum RPacketQoS::TQoSMeanThroughput.

Field type: TUint32. This field may be left as NULL. */
#define DEFAULT_GPRS_MIN_PEAK_THROUGHPUT				_S("MinPeakThroughput")

/** Default minimum quality of service mean throughput class.

Values for this are defined by the enum RPacketQoS::TQoSMeanThroughput.

Field type: TUint32. This field may be left as NULL. */
#define DEFAULT_GPRS_MIN_MEAN_THROUGHPUT				_S("MinMeanThroughput")

/** Default value for whether data compression is on.

Field type: TBool. This field may be left as NULL. */
#define DEFAULT_GPRS_DATA_COMPRESSION					_S("DataCompression")

/** Default value for whether IP header compression is on.

Field type: TBool. This field may be left as NULL. */
#define DEFAULT_GPRS_HEADER_COMPRESSION					_S("HeaderCompression")

/** Default value for whether use of anonymous access is on.

Field type: TBool. This field may be left as NULL. */
#define DEFAULT_GPRS_ANONYMOUS_ACCESS					_S("AnonymousAccess")
/** Default value for EDGE parameter

Field type: TBool. This field may be left as NULL. */
#define DEFAULT_GPRS_USE_EDGE							_S("GprsUseEdge")

// Columns of the `AGENT_LOOKUP` table
// The following macros define values for the Agent Lookup fields.
// This table has no COMMDB_NAME field.
/** The Agent's name.

Field type: Text. This field may not be left as NULL. */
#define	AGENT_FRIENDLY_NAME								_S("AgentFriendlyName")
/** The internal file location of the agent.

Field type: Text. This field may not be left as NULL. */
#define AGENT_FILENAME									_S("AgentFilename")


/* Field type: Text. This field must not be left as NULL. */
#define CDMA_IWF_NAME									_S("IwfName")
/** Mask indicating valid service options.

Field type: TUint32 from RPacketContext::TServiceOption. This field may not be left as NULL. */
#define CDMA_SERVICE_OPTION								_S("ServiceOption")

/** PDP type. (IPv4 or IPv6).

Field type: TUint32 from RPacketContext::TProtocolType. This field may not be left as NULL. */
#define CDMA_PDP_TYPE									_S("PdpType")
/** PDP address of phone.

Field type: Text. This field may be left as NULL. */
#define CDMA_PDP_ADDRESS								_S("PdpAddress")

/** Requested forward priority.

Field type: TUint32 from RPacketQoS::TQoSLinkPriority. This field may be left as NULL. */
#define CDMA_REQ_FWD_PRIORITY							_S("ReqFwdPriority")
/** Requested reverse priority.

Field type: TUint32 from RPacketQoS::TQoSLinkPriority. This field may be left as NULL. */
#define CDMA_REQ_REV_PRIORITY							_S("ReqRevPriority")
/** Requested forward bitrate.

Field type: TUint32 from RPacketQoS::TQoSDataRate. This field may be left as NULL. */
#define CDMA_REQ_FWD_BITRATE							_S("ReqFwdBitrate")
/** Requested reverse bitrate.

Field type: TUint32 from RPacketQoS::TQoSDataRate. This field may be left as NULL. */
#define CDMA_REQ_REV_BITRATE							_S("ReqRevBitrate")
/** Requested forward frame loss rate.

Field type: TUint32 from RPacketQoS::TQoSDataLoss. This field may be left as NULL. */
#define CDMA_REQ_FWD_LOSS								_S("ReqFwdLoss")
/** Requested reverse frame loss rate.

Field type: TUint32 from RPacketQoS::TQoSDataLoss. This field may be left as NULL. */
#define CDMA_REQ_REV_LOSS								_S("ReqRevLoss")
/** Requested forward delay.

Field type: TUint32 from RPacketQoS::TQoSDelay. This field may be left as NULL. */
#define CDMA_REQ_FWD_MAXDELAY							_S("ReqFwdMaxdelay")
/** Requested reverse delay.

Field type: TUint32 from RPacketQoS::TQoSDelay. This field may be left as NULL. */
#define CDMA_REQ_REV_MAXDELAY							_S("ReqRevMaxdelay")
/** Minimum acceptable forward bitrate.

Field type: TUint32 from RPacketQoS::TQoSDataRate. This field may be left as NULL. */
#define CDMA_MIN_FWD_BITRATE							_S("MinFwdBitrate")
/** Minimum acceptable reverse bitrate.

Field type: TUint32 from RPacketQoS::TQoSDataRate. This field may be left as NULL. */
#define CDMA_MIN_REV_BITRATE							_S("MinRevBitrate")
/** Maximum acceptable forward frame loss rate.

Field type: TUint32 from RPacketQoS::TQoSDataLoss. This field may be left as NULL. */
#define CDMA_ACCPT_FWD_LOSS								_S("AccptFwdLoss")
/** Maximum acceptable forward frame loss rate.

Field type: TUint32 from RPacketQoS::TQoSDataLoss. This field may be left as NULL. */
#define CDMA_ACCPT_REV_LOSS								_S("AccptRevLoss")
/** Maximum acceptable forward delay.

Field type: TUint32 from RPacketQoS::TQoSDelay. This field may be left as NULL. */
#define CDMA_ACCPT_FWD_MAXDELAY							_S("AccptFwdMaxdelay")
/** Maximum acceptable reverse delay.

Field type: TUint32 from RPacketQoS::TQoSDelay. This field may be left as NULL. */
#define CDMA_ACCPT_REV_MAXDELAY							_S("AccptRevMaxdelay")
/** Data compression on?

Field type: TBool. This field may be left as NULL. */
#define CDMA_DATA_COMPRESSION							_S("DataCompression")
/** Van Jacobson IP header compression on?

Field type: TBool. This field may be left as NULL. */
#define CDMA_ENABLE_IP_HEADER_COMP						SERVICE_ENABLE_IP_HEADER_COMP
/** Use anonymous access on?

Field type: TBool. This field may be left as NULL. */
#define CDMA_ANONYMOUS_ACCESS							_S("AnonymousAccess")

//#define CDMA_IF_NAME									SERVICE_IF_NAME
/** Interface parameter string.

Field type: Text. This field may be left as NULL. */
#define CDMA_IF_PARAMS									SERVICE_IF_PARAMS
/** Comma separated list of network protocols, e.g. "PPP".

Field type: Text. This field may be left as NULL. */
#define CDMA_IF_NETWORKS								SERVICE_IF_NETWORKS
/** Always reject PPP peer authentication requests when using external IP configuration.

Field type: TBool. This field may be left as NULL. */
#define CDMA_IF_EXTERN_IP_CONFIG_ALWAYS_REJECT_AUTH	    SERVICE_IF_EXTERN_IP_CONFIG_ALWAYS_REJECT_AUTH
/** Prompt user for authentication username and password?

Field type: TBool. This field may not be left as NULL. */
#define CDMA_IF_PROMPT_FOR_AUTH							SERVICE_IF_PROMPT_FOR_AUTH
/** Authentication username used by PPP in the Simple IP case or
by the Mobile IP implementation when Mobile IP is enabled.

Field type: Text. This field may be left as NULL. */
#define CDMA_IF_AUTH_NAME								SERVICE_IF_AUTH_NAME
/** Authentication password used by PPP.

Field type: Text. This field may be left as NULL. */
#define CDMA_IF_AUTH_PASS								SERVICE_IF_AUTH_PASS
/** Number of times to retry authentication if it fails.

Field type: TUint32. This field may be left as NULL. */
#define CDMA_IF_AUTH_RETRIES							SERVICE_IF_AUTH_RETRIES
/** IP Net mask of Symbian OS mobile network interface.

Field type: Text. This field may be left as NULL. */
#define CDMA_IP_NETMASK									SERVICE_IP_NETMASK
/** IP address of gateway.

Field type: Text. This field may be left as NULL. */
#define CDMA_IP_GATEWAY									SERVICE_IP_GATEWAY
/** Get IP addresses (for Symbian OS mobile) from server?

Field type: TBool. This field may not be left as NULL. */
#define CDMA_IP_ADDR_FROM_SERVER						SERVICE_IP_ADDR_FROM_SERVER
/**
IP address (of Symbian OS mobile), to be used by PPP and TCP/IP.

Field type: Text. This field may be left as NULL. */
#define CDMA_IP_ADDR									SERVICE_IP_ADDR
/** Get DNS addresses from server?

Field type: TBool. This field may not be left as NULL. */
#define CDMA_IP_DNS_ADDR_FROM_SERVER					SERVICE_IP_DNS_ADDR_FROM_SERVER
/** IP Address of primary name server.

Field type: Text. This field may be left as NULL. */
#define CDMA_IP_NAME_SERVER1							SERVICE_IP_NAME_SERVER1
/** IP Address of secondary name server.

Field type: Text. This field may be left as NULL. */
#define CDMA_IP_NAME_SERVER2							SERVICE_IP_NAME_SERVER2
/** Get IP6 DNS address from Server?

Field type: TBool. This field may be left as NULL. */
#define CDMA_IP6_DNS_ADDR_FROM_SERVER					SERVICE_IP6_DNS_ADDR_FROM_SERVER
/** IP6 address of the primary name server

Field type: Text. This field may be left as NULL. */
#define CDMA_IP6_NAME_SERVER1							SERVICE_IP6_NAME_SERVER1
/** IP6 address of the secondary name server

Field type: Text. This field may be left as NULL. */
#define CDMA_IP6_NAME_SERVER2							SERVICE_IP6_NAME_SERVER2
/** IP address valid from this time,
used to store dynamically assigned address lease info.

Field type: Text. This field may be left as NULL. */
#define CDMA_IP_ADDR_LEASE_VALID_FROM					SERVICE_IP_ADDR_LEASE_VALID_FROM
/** IP address valid to this time,
used to store dynamically assigned address lease info.

Field type: Text. This field may be left as NULL. */
#define CDMA_IP_ADDR_LEASE_VALID_TO						SERVICE_IP_ADDR_LEASE_VALID_TO
/** Name of the ECOM configuration daemon manager component.
This component interfaces with the server identified in CDMA_CONFIG_DAEMON_NAME.
If specified, CDMA_CONFIG_DAEMON_NAME should also be specified.

Field type: Text. This field may be left as NULL.  */
#define CDMA_CONFIG_DAEMON_MANAGER_NAME					SERVICE_CONFIG_DAEMON_MANAGER_NAME
/** Name of the configuration daemon server. This server is used to
provide further configuration for a connection, e.g. dynamic IP
address assignment. If specified, CDMA_CONFIG_DAEMON_MANAGER_NAME
should also be specified.

Field type: Text. This field may be left as NULL.  */
#define CDMA_CONFIG_DAEMON_NAME							SERVICE_CONFIG_DAEMON_NAME
/** Enable LCP extensions?

Field type: TBool. This field may be left as NULL. */
#define CDMA_ENABLE_LCP_EXTENSIONS						SERVICE_ENABLE_LCP_EXTENSIONS
/** Disable plain text authentication?

Field type: TBool. This field may be left as NULL. */
#define CDMA_DISABLE_PLAIN_TEXT_AUTH					SERVICE_DISABLE_PLAIN_TEXT_AUTH
/** Service supports Internet only, WAP only or both.

Field type: TUint32 from TCommsDbIspType. This field may be left as NULL. */
#define CDMA_AP_TYPE									_S("ApType")
/** Notify user if the requested QOS can not be satisfied for this time in microseconds.
Set to 0xffffffff to disable.

Field type: TUint32. This field may be left as NULL. */
#define CDMA_QOS_WARNING_TIMEOUT						_S("QosWarningTimeout")

/** Transparent or non-transparent radio link protocol.

Field type: TUint32 from RPacketQoS::TRLPMode. This field may not be left as NULL. */
#define CDMA_RLP_MODE									_S("RlpMode")

// Deprecated Mobile IP fields

/** Attempt to use the mobile IP protocol.

Field type: TBool. This field may be left as NULL. */
#define CDMA_MIP										_S("CDMAMobileIP")
/** Home agent address.

Field type: Text. This field may be left as NULL. */
#define CDMA_MIP_HA_ADDRESS								_S("HomeAgentAddress")

/** A normal reconnect (informing the user) takes place if not both the PPP link
drops and a change in SID or NID is received with in this time (in milliseconds).
If they do, an automatic reconnect takes place.

Field type: TUint32 This field may be left as NULL. */
#define CDMA_MIP_TIMEOUT								_S("CDMAMobileIPTimeout")

// The following fields define the specific CDMA parameters provisioned through OTA
// and  defined in TIA-683B section 3.5.8
// These fields replace the old MIP fields (CDMA_MIP, CDMA_MIP_HA_ADDRESS,
// CDMA_MIP_TIMEOUT)

/** Defines the type of NAI this record refers to (SimpleIP or MobileIP).
Possible values: ECommDbCdmaNaiSimpleIp, ECommDbCdmaNaiMobileIp. Replaces the
CDMA_MIP field.

Field type: TUint32. This field must not be left as NULL. */
#define CDMA_NAI_TYPE									_S("CDMANaiType")
/** SimpleIP authentication algorithm. Possible values (TCommDbCdmaSimpIpAuthAlgorithm):

0000 - No authentication

0001 - PPP CHAP authentication

0002 - PPP PAP authentication

0003 - PPP CHAP to PAP fallback

Field type: TUint32, for the enum TCommDbCdmaSimpIpAuthAlgorithm. This field may be left as NULL. */
#define CDMA_SIMIP_AUTH_ALGORITHM						_S("CDMASimpleIpAuthAlgorithm")
/** SimpleIP PAP shared secret handle

Field type: TUint32. This field may be left as NULL.  */
#define CDMA_SIMIP_PAP_SS_HANDLE						_S("CDMASimpleIpPapSsHandle")
/** SimpleIP CHAP shared secret handle

Field type: TUint32. This field may be left as NULL. */
#define CDMA_SIMIP_CHAP_SS_HANDLE						_S("CDMASimpleIpChapSsHandle")
/** Reverse-tunnelling required flag.

Field type: TUint32.  This field may be left as NULL. */
#define CDMA_MIP_T_BIT									_S("CDMAMobileIpTBit")
/** Home address associated with the Mobile Station.

Field type: Text. This field may be left as NULL. */
#define CDMA_MIP_HOME_ADDRESS							_S("CDMAMobileIpHomeAddress")
/** IP address of the mobile node's primary home agent.
Replaces the CDMA_MIP_HA_ADDRESS field.

Field type: Text. This field may be left as NULL. */
#define CDMA_MIP_PRIMARY_HOME_AGENT						_S("CDMAMobileIpPrimaryHomeAgent")
/** IP address of the mobile node's secondary home agent.

Field type: Text. This field may be left as NULL.  */
#define CDMA_MIP_SECONDARY_HOME_AGENT					_S("CDMAMobileIpSecondaryHomeAgent")
/** Authentication algorithm (MN-AAA). Possible values:

0000 - No authentication

0001 - MD5 authentication (IETF RFC 2002)

Field type: TUint32. This field may be left as NULL. */
#define CDMA_MIP_MN_AAA_AUTH_ALGORITHM					_S("CDMAMobileIpMnAaaAuthAlgorithm")


/** Security Parameter Index Indicator (MN-AAA)

Field type: TBool. This field may be left as NULL. */
#define CDMA_MIP_MN_AAA_SPI_INDICATOR				    _S("CDMAMobileIpMnAaaSpiIndicator")


/** Security Parameter Index (MN-AAA).

Field type: TUint32. This field may be left as NULL. */
#define CDMA_MIP_MN_AAA_SPI								_S("CDMAMobileIpMnAaaSpi")
/** MobileIP MN-AAA shared secret data.

Field type: Text. This field may be left as NULL.  */
#define CDMA_MIP_MN_AAA_SS_DATA						_S("CDMAMobileIpMnAaaSsData")
/** Authentication algorithm (MN-HA). Possible values:

0000 - No authentication

0001 - MD5 authentication (IETF RFC 2002)

Field type: TUint32. This field may be left as NULL.  */
#define CDMA_MIP_MN_HA_AUTH_ALGORITHM					_S("CDMAMobileIpMnHaAuthAlgorithm")



/** Security Parameter Index Indicator (MN-HA)

Field type: TBool. This field may be left as NULL.  */
#define CDMA_MIP_MN_HA_SPI_INDICATOR				    _S("CDMAMobileIpMnHaSpiIndicator")

/** Security Parameter Index (MN-HA)

Field type: TUint32. This field may be left as NULL. */
#define CDMA_MIP_MN_HA_SPI								_S("CDMAMobileIpMnHaSpi")


/** MobileIP MN-HA shared secret data.

Field type: Text. This field may be left as NULL. */
#define CDMA_MIP_MN_HA_SS_DATA						_S("CDMAMobileIpMnHaSsData")

/** Fundamental channel multiplex option
Field type: TUint32. */
#define CDMA_FCH_MUX									_S("CDMAFchMux")

/** Supplemental channel multiplex option
Field type: TUint32. */
#define CDMA_SCH_MUX									_S("CDMASchMux")

/** IWF name.

Field type: Text. This field may not be left as NULL. */
#define DEFAULT_CDMA_IWF_NAME							CDMA_IWF_NAME
/** Mask indicating valid service options.

Field type: TUint32 from RPacketContext::TServiceOption. This field may not be left as NULL. */
#define DEFAULT_CDMA_SERVICE_OPTION						CDMA_SERVICE_OPTION
/** PDP type. (IPv4 or IPv6).

Field type: TUint32 from RPacketContext::TProtocolType. This field may not be left as NULL. */
#define DEFAULT_CDMA_PDP_TYPE							CDMA_PDP_TYPE
/** PDP address of phone.

Field type: Text. This field may be left as NULL. */
#define DEFAULT_CDMA_PDP_ADDRESS						CDMA_PDP_ADDRESS
/** Data compression?

Field type: TBool. This field may be left as NULL. */
#define DEFAULT_CDMA_DATA_COMPRESSION					CDMA_DATA_COMPRESSION
/** Van Jacobson IP header compression on?

Field type: TBool. This field may be left as NULL. */
#define DEFAULT_CDMA_ENABLE_IP_HEADER_COMP				CDMA_ENABLE_IP_HEADER_COMP
/** Use anonymous access?

Field type: TBool. This field may be left as NULL. */
#define DEFAULT_CDMA_ANONYMOUS_ACCESS					CDMA_ANONYMOUS_ACCESS

// Default values for the deprecated Mobile IP fields.
/** Default CDMA mobile IP?

Field type: TBool. This field may be left as NULL.

@deprecated */
#define DEFAULT_CDMA_MIP								CDMA_MIP
/** CDMA mobile IP home address.

Field type: Text. This field may be left as NULL.

@deprecated */
#define DEFAULT_CDMA_MIP_HA_ADDRESS						CDMA_MIP_HA_ADDRESS
/** CDMA Mobile IP timeout.

Field type: TUint32. This field may be left as NULL.

@deprecated */
#define DEFAULT_CDMA_MIP_TIMEOUT						CDMA_MIP_TIMEOUT

/** Default fundamental channel multiplex option
Field type: TUint32. */
#define DEFAULT_CDMA_FCH_MUX							CDMA_FCH_MUX

/** Default supplemental channel multiplex option
Field type: TUint32. */
#define DEFAULT_CDMA_SCH_MUX							CDMA_SCH_MUX


// The following fields define the specific CDMA parameters provisioned through OTA
// and defined in TIA-683B section 3.5.8

/** Defines the active operation mode in the mobile station. Possible values:

00 - SimpleIP only

01 - MobileIP with SimpleIP fallback

10 - MobileIP only

Field type: TUint32. */
#define CDMA_OP_MODE									_S("CDMAOperationMode")
/** Maximum number of retries for MobileIP registration.

Field type: TUint32. */
#define CDMA_MIP_MAX_NUM_RETRY							_S("CDMAMobileIpMaxNumRetry")
/** Amount of time between the first and second MobileIP registration requests,
while the Mobile Station did not receive a Registration Reply (units of 250ms).

Field type: TUint32. */
#define CDMA_MIP_FIRST_RETRY_TIMEOUT					_S("CDMAMobileIpFirstRetryTimeout")
/** Re-registration threshold (time in minutes before the expiration of the registration
lifetime).

Field type: TUint32. */
#define CDMA_MIP_REREG_THRESHOLD						_S("CDMAMobileIpReregThreshold")

/** Columns of the `VIRTUAL_BEARER table` */
#define VIRTUAL_BEARER_AGENT							AGENT_NAME
#define VIRTUAL_BEARER_NIF								IF_NAME

/** Columns of the `VIRTUAL_SERVICE table` */
#define VPN_SERVICE_POLICY							_S("Policy")
#define VPN_SERVICE_IAP								_S("HomeIAP")
#define VPN_SERVICE_NETWORKID						_S("HomeNetwork")

/** Columns of the `WLAN_SERVICE table` */
#define WLAN_PROFILE								_S("Wlan_Profile")
#define WLAN_SSID									_S("Wlan_SSID")
#define WLAN_ENCRYPTION_STATUS						_S("Wlan_Encryption_Status")
#define WLAN_ENCRYPTION_TYPE						_S("Wlan_Encryption_Type")
#define WLAN_DEFAULT_WEP_KEY						_S("Wlan_Wep_Default_Key")
#define WLAN_ENCRYPT_KEY1_VALID						_S("Wlan_Key1_Valid")
#define WLAN_WEP_Key1								_S("Wlan_Wep_Key1")
#define WLAN_ENCRYPT_KEY2_VALID						_S("Wlan_Key2_Valid")
#define WLAN_WEP_KEY2								_S("Wlan_Wep_Key2")
#define WLAN_ENCRYPT_KEY3_VALID						_S("Wlan_Key3_Valid")
#define WLAN_WEP_KEY3								_S("Wlan_Wep_Key3")
#define WLAN_ENCRYPT_KEY4_VALID						_S("Wlan_Key4_Valid")
#define WLAN_WEP_KEY4								_S("Wlan_Wep_Key4")
#define WLANRANKING 								_S("Wlan_Ranking")
#define WLAN_DIALOG_PREF							_S("Wlan_Dialog_Pref")
#define WLAN_NETWORK_TYPE							_S("Wlan_Network_Type")
#define WLAN_FRAG_THRES								_S("Wlan_Frag_Threshold")
#define WLAN_RTS_THRES								_S("Wlan_RTS_Threshold")
#define WLAN_DES_TRANS_RATE							_S("Wlan_Desired_Transmit_Rate")
#define WLAN_BEACON_INT								_S("Wlan_Beacon_Int")
#define WLAN_CHANNEL_ID								_S("Wlan_Channel_ID")
#define WLAN_AUTHENTICATION_MODE					_S("Wlan_Authentication_Mode")
#define WLAN_POWERSAVE_MODE							_S("Wlan_Power_Save_Mode")
#define WLAN_ALLOW_UNENCRYPT_MODE					_S("Wlan_Allow_UnEncrypt_Mode")
#define WLAN_ALLOW_SHORT_PRE						_S("Wlan_Allow_Short_Preamble")
#define WLAN_PREAMBLE_TYPE							_S("Wlan_Preamble_Type")
#define WLAN_TX_POWER_LEVEL							_S("Wlan_TX_Power_Level")
#define WLAN_EAP_PRESHARED_KEY						_S("Wlan_EAP_Preshared_Key")
#define WLAN_REG_DOMAIN								_S("Wlan_Reglutory_Domain")
#define WLAN_SEC_DATA								_S("Wlan_Security_Data")

/** Columns of the `NETWORK table` */
#define HOST_NAME									_S("HostName")

/** Columns of the AccessPoint table */
#define ACCESS_POINT_GID                            _S("AccessPointGID")
#define ACCESS_POINT_POLICY	    					_S("AccessPointSelectionPolicy")

#define ACCESS_POINT_TIER		        			_S("Tier")
#define ACCESS_POINT_MCPR	    					_S("MCpr")
#define ACCESS_POINT_CPR	    					_S("Cpr")
#define ACCESS_POINT_CPRCONFIGID   					_S("CprConfig")
#define ACCESS_POINT_SCPR	    					_S("SCpr")
#define ACCESS_POINT_PROTOCOL	    				_S("Protocol")
#define ACCESS_POINT_APPSID	    					_S("AppSID")
#define ACCESS_POINT_CONFIG_AP_ID_LIST              _S("ConfigAPIdList")
#define ACCESS_POINT_CUSTOM_SELECTION_POLICY        _S("CustomSelectionPolicy")
#define ACCESS_POINT_PRIORITY	    				_S("Priority")

#define TIER_IMPL_UID								_S("TierImplUid")
#define TIER_THREAD_NAME							_S("TierThreadName")
#define TIER_MANAGER_NAME							_S("TierManagerName")
#define DEFAULT_ACCESS_POINT     					_S("DefaultAccessPoint")

#define MCPR_UID									_S("MCprUid")
#define CPR_UID										_S("CprUid")
#define SCPR_UID									_S("SCprUid")
#define PROTOCOL_UID								_S("ProtocolUid")
#define PROTOCOL_CONFIG_LOADER_UID                  _S("ProtocolConfigLoaderUid")

#define PROTOCOL_CONFIG                             _S("ProtocolConfig")
#define LAYER_BELOW                                 _S("LayerBelow")


/**
@internalTechnology
*/
#define EAPSIM_PROTOCOL_ID							_S("ProtocolId")
#define EAPSIM_PROTOCOL_PSEUDONYM 					_S("Pseudonym")
#define EAPSIM_PROTOCOL_USE_NAI_REALM 				_S("UseNaiRealm")
#define EAPSIM_PROTOCOL_NAI_REALM 					_S("NaiRealm")
#define EAPSIM_PROTOCOL_MIN_RANDS 					_S("MinRands")
#define EAPAKA_PROTOCOL_ID							_S("ProtocolId")
#define EAPAKA_PROTOCOL_PSEUDONYM 					_S("Pseudonym")
#define EAPAKA_PROTOCOL_USE_NAI_REALM 				_S("UseNaiRealm")
#define EAPAKA_PROTOCOL_NAI_REALM 					_S("NaiRealm")

#define SELECTION_POLICY_IAP1			    		_S("IAP1")
#define SELECTION_POLICY_IAP2			    		_S("IAP2")
#define SELECTION_POLICY_IAP3			    		_S("IAP3")
#define SELECTION_POLICY_IAP4			    		_S("IAP4")
#define SELECTION_POLICY_IAP5			    		_S("IAP5")
#define SELECTION_POLICY_IAP6			    		_S("IAP6")
#define SELECTION_POLICY_IAP7			    		_S("IAP7")
#define SELECTION_POLICY_IAP8			    		_S("IAP8")
#define SELECTION_POLICY_IAP9			    		_S("IAP9")
#define SELECTION_POLICY_IAP10			    		_S("IAP10")
#define SELECTION_POLICY_IAP11			    		_S("IAP11")
#define SELECTION_POLICY_IAP12			    		_S("IAP12")
#define SELECTION_POLICY_IAP13			    		_S("IAP13")
#define SELECTION_POLICY_IAP14			    		_S("IAP14")
#define SELECTION_POLICY_IAP15			    		_S("IAP15")
#define SELECTION_POLICY_IAPCOUNT                   _S("IAPCOUNT")

#define SELECTION_POLICY_AP1			    		_S("AP1")
#define SELECTION_POLICY_AP2			    		_S("AP2")
#define SELECTION_POLICY_AP3			    		_S("AP3")
#define SELECTION_POLICY_AP4			    		_S("AP4")
#define SELECTION_POLICY_AP5			    		_S("AP5")
#define SELECTION_POLICY_AP6			    		_S("AP6")
#define SELECTION_POLICY_AP7			    		_S("AP7")
#define SELECTION_POLICY_AP8			    		_S("AP8")
#define SELECTION_POLICY_AP9			    		_S("AP9")
#define SELECTION_POLICY_AP10			    		_S("AP10")
#define SELECTION_POLICY_AP11			    		_S("AP11")
#define SELECTION_POLICY_AP12			    		_S("AP12")
#define SELECTION_POLICY_AP13			    		_S("AP13")
#define SELECTION_POLICY_AP14			    		_S("AP14")
#define SELECTION_POLICY_AP15			    		_S("AP15")
#define SELECTION_POLICY_APCOUNT                   _S("APCOUNT")

#define EAPSIM_PROTOCOL_ID							_S("ProtocolId")
#define EAPSIM_PROTOCOL_PSEUDONYM 					_S("Pseudonym")
#define EAPSIM_PROTOCOL_USE_NAI_REALM 				_S("UseNaiRealm")
#define EAPSIM_PROTOCOL_NAI_REALM 					_S("NaiRealm")
#define EAPSIM_PROTOCOL_MIN_RANDS 					_S("MinRands")

#define EAPAKA_PROTOCOL_ID							_S("ProtocolId")
#define EAPAKA_PROTOCOL_PSEUDONYM 					_S("Pseudonym")
#define EAPAKA_PROTOCOL_USE_NAI_REALM 				_S("UseNaiRealm")
#define EAPAKA_PROTOCOL_NAI_REALM 					_S("NaiRealm")

/** Columns of the `EAP Security Settings table` */
#define	EAP_SEC_OUTER_EAP_TYPE	 					_S("OuterEapMethodType")
#define	EAP_SEC_ID  								_S("EapId")
#define	EAP_SEC_PASSWORD  							_S("EapPassword")
#define	EAP_SEC_CONFIGID							_S("ConfigId")
#define	EAP_SEC_DATA 								_S("EAP_Method_Data")
#define EAP_SEC_VENDORID                            _S("EapVendorId")
#define EAP_SEC_VENDORTYPE                          _S("EapVendorType")

/** Columns of the `Tunnelled EAP Settings Table` */
#define	TUN_EAP_INNERTYPE	 						_S("InnerEapMethodType")
#define TUN_EAP_DATA 								_S("Tun_EAP_Method_Data")

/** Columns of the `EAP-TLS Settings Table` */
#define	EAP_TLS_CLIENT_CERT_NAME					_S("ClientCertificateName")

/** Columns of the `LEAP Settings Table` */
#define	EAP_LEAP_TIMEOUT					 			_S("LeapTimeout")

/** Columns of the base `Params Table` */
#define	STYPEID										_S("STypeId")

/** Columns of the `PolicySelector Table` */
#define POLICYID									_S("PolicyId")
#define APPUID										_S("AppUid")
#define SRCADDRESS									_S("SrcAddress")
#define SRCMASK										_S("SrcMask")
#define DSTADDRESS									_S("DstAddress")
#define DSTMASK										_S("DstMask")
#define SRCPORT										_S("SrcPort")
#define DSTPORT										_S("DstPort")
#define SRCPORTMAX									_S("SrcPortMax")
#define DSTPORTMAX									_S("DstPortMax")
#define PROTOCOLID									_S("ProtocolId")
#define IAPID										_S("IapId")
#define PRIORITY									_S("Priority")

/** Columns of the `PolicySelector2Params Table` */
#define POLICYSELECTORID							_S("PolicySelectorId")
#define PARAMSID									_S("ParamsId")

/** Columns of the `GenericQos Table` */
#define DOWNLINKBANDWIDTH							_S("DownlinkBandwidth")
#define UPLINKBANDWIDTH								_S("UplinkBandwidth")
#define DOWNLINKMAXIMUMBURSTSIZE					_S("DownLinkMaximumBurstSize")
#define UPLINKMAXIMUMBURSTSIZE						_S("UpLinkMaximumBurstSize")
#define DOWNLINKAVERAGEPACKETSIZE					_S("DownLinkAveragePacketSize")
#define UPLINKAVERAGEPACKETSIZE						_S("UpLinkAveragePacketSize")
#define DOWNLINKMAXIMUMPACKETSIZE					_S("DownLinkMaximumPacketSize")
#define UPLINKMAXIMUMPACKETSIZE						_S("UpLinkMaximumPacketSize")
#define DOWNLINKDELAY								_S("DownLinkDelay")
#define UPLINKDELAY									_S("UpLinkDelay")
#define DOWNLINKDELAYVARIATION						_S("DownLinkDelayVariation")
#define UPLINKDELAYVARIATION						_S("UpLinkDelayVariation")
#define DOWNLINKPRIORITY							_S("DownLinkPriority")
#define UPLINKPRIORITY								_S("UpLinkPriority")
#define HEADERMODE									_S("HeaderMode")
//#define QOSNAME										_S("QosName")

/** Columns of the `WifiScanEngine Table`
@internalTechnology
*/
#define SCANPERIODMS								_S("ScanPeriodMs")
#define RSSIMIN										_S("RSSIMin")
#define RSSIMAX										_S("RSSIMax")


/** Enumerated sets and bitmasks for the modem speaker `MODEM_BEARER:MODEM_SPEAKER_PREF`. */
enum TCommsDbModemSpeakerSetting
	{
	/** Speaker always off. */
	EModemSpeakerSettingNever,
	/** Speaker on until the call has been answered. */
	EModemSpeakerSettingUntilCallAnswered,
	/** Speaker always on. */
	EModemSpeakerSettingAlways,
	/** Speaker on after dialing until answer. */
	EModemSpeakerSettingAfterDialUntilAnswer
	};

/** Enumeration used by `MODEM_BEARER:MODEM_SPEAKER_VOL_PREF` */
enum TCommsDbModemSpeakerVolume
	{
	/** Speaker off. */
	EModemSpeakerVolumeQuiet,
	/** Speaker volume set to medium. */
	EModemSpeakerVolumeMedium,
	/** Speaker volume set to loud. */
	EModemSpeakerVolumeLoud
	};

/** Bit mask used by `MODEM_BEARER:MODEM_COMM_ROLE` */
const TUint KModemCommRoleDCE = 0x1;
const TUint32 KLinkableFlagTag = 0x80000000;

/** Enumeration used by DIAL_*_ISP:ISP_TYPE, OUTGOING_WCDMA:GPRS_AP_TYPE. */
enum TCommsDbIspType
	{
	/** ISP internet only. */
	EIspTypeInternetOnly,
	/** ISP WAP only. */
	EIspTypeWAPOnly,
	/** ISP internet and WAP. */
	EIspTypeInternetAndWAP
	};

/** Enum used by DIAL_*_ISP:ISP_BEARER_TYPE. */
enum TCommsDbBearerType
	{
	/** CSD bearer. */
	EBearerTypeCSD,
	/** HSCSD bearer. */
	EBearerTypeHSCSD
	};

/** Enum used by WAP_*_BEARER:WAP_WSP_OPTION. */
enum TCommsDbWapWspOption
	{
	/** WAP WSP option set to connectionless. */
	EWapWspOptionConnectionless,
	/** WAP WSP option set to connection oriented. */
	EWapWspOptionConnectionOriented
	};

/**
Enum for use in calls to `CCommsDatabase::OpenConnectionPrefTable*LC()`,
CCommsDatabase::OpenIAPTableViewMatchingBearerSetLC() and
CCommsDbConnectionPrefTableView::SwapConnectionPreferencesL()
*/
enum TCommDbConnectionDirection
	{
	/** Connection direction is unknown. */
	ECommDbConnectionDirectionUnknown,
	/** Connection direction is outgoing. */
	ECommDbConnectionDirectionOutgoing,
	/** Connection direction is incoming. */
	ECommDbConnectionDirectionIncoming
	};

/** Bitmask for use in calls to: CCommsDatabase::OpenIAPTableViewMatchingBearerSetLC() */
enum TCommDbBearer
	{
	/** Bearer support unknown. */
	KCommDbBearerUnknown = 0x0,
	/** CSD Bearer support. */
	KCommDbBearerCSD = 0x1,
	/** WCDMA Bearer support. */
	KCommDbBearerWcdma = 0x2,
	/** LAN Bearer support. */
	KCommDbBearerLAN = 0x4,
	KCommDbBearerVirtual = 0x10,
	KCommDbBearerPAN = 0x20,
	KCommDbBearerWLAN = 0x40
	};

#define DeprecatedCDMA2000 0x8
#define KCommDbBearerPSD (KCommDbBearerWcdma|DeprecatedCDMA2000)

/** The dialog preference determines whether or not a user should be prompted with
a dialog at connect time. Used in CCommsDbConnectionPrefTableView::UpdateDialogPrefL() */
enum TCommDbDialogPref
	{
	/** CommDB dialog preference is unknown. */
	ECommDbDialogPrefUnknown,
	/** Preference set to prompt user. */
	ECommDbDialogPrefPrompt,
	/** Preference set to warn user. */
	ECommDbDialogPrefWarn,
	/** Preference set not to prompt user. */
	ECommDbDialogPrefDoNotPrompt,
	/** Preference set to prompt user when in wrong mode. */
	ECommDbDialogPrefPromptIfWrongMode
	};

/** Enum for the global setting: GPRS_CLASS_C_BEARER. */
enum TCommDbGprsClassCBearer
	{
	/** GPRS Class C bearer. */
	ECommDbGprsClassCBearerGprs,
	/** GSM Class C bearer. */
	ECommDbGprsClassCBearerGsm
	};

/** Enum for use in calls to CCommsDatabase::NewL() */
enum TCommDbOpeningMethod
	{
	/** CommDB has been created. */
	ECommDbCreated = 0,
	/** CommDB Copied default. */
	ECommDbCopiedDefault,
	/** CommDB has been opened. */
	ECommDbOpened
	};

/** UIDs for system agent event notification */

/** The global setting `SMS_BEARER` has changed */
const TUid KUidCommDbSMSBearerChange = {0x10008F04};
/** The global setting `SMS_RECEIVE_MODE' has changed */
const TUid KUidCommDbSMSReceiveModeChange = {0x101F4AB0};
/** The global setting `GPRS_ATTACH_MODE' has changed */
const TUid KUidCommDbGPRSAttachModeChange = {0x100092BC};

/** The `MODEM_BEARER:MODEM_TSY_NAME` field has been written or a `MODEM` record has
	been deleted */
const TUid KUidCommDbModemTsyNameChange = {0x1000947f};
/** As `KUidCommDbModemTsyNameChange` above except only for a change in the
	record specified by the `MODEM_DATA_FAX` global setting */
const TUid KUidCommDbModemDataAndFaxChange =  {0x1000A43F} ;
/** As `KUidCommDbModemTsyNameChange` above except only for a change in the
	record specified by the `MODEM_PHONE_SERVICES_SMS` global setting */
const TUid KUidCommDbModemPhoneServicesAndSMSChange = {0x1000A440};

/** A record in the `DEFAULT_GPRS` table has been modified or deleted. */
const TUid KUidCommDbGPRSDefaultParamsChange = {0x10008F05};
/** A record in the `MODEM_BEARER` table has been modified or deleted. */
const TUid KUidCommDbModemRecordChange =  {0x1000A43D} ;
/** A record in the `PROXIES` table has been modified or deleted */
const TUid KUidCommDbProxiesRecordChange = {0x1000A43E};

/** Enum for DATABASE_TYPE field.
Was deprecated in v7.0 but replaced for BC with v6.1 in v7.0s and v8.0. */
enum TCommDbDatabaseType
	{
	/** Unspecified database type. Any type of database can be opened with this parameter.*/
	EDatabaseTypeUnspecified =0,
	/** The database has an IAP table, which defines sets of ISPs and chargecards that
	may be used together. */
	EDatabaseTypeIAP,
	/** The database is arranged using separate ISP, location, modem and chargecard
	tables whose records are not associated by using IAPs.

	ISP type databases are deprecated from version 6.1. */
	EDatabaseTypeISP
	};


/** Enum for the global setting `CDMA_OP_CAPABILITY`
@publishedAll
@released */
enum TCommDbCdmaOpCapability
	{
	/** Simple IP only supported. */
	ECommDbCdmaOpCapabilitySimpleIp = 0x1,
	/** Mobile IP only supported. */
	ECommDbCdmaOpCapabilityMobileIp = 0x2,
	/** Mobile IP with Simple IP fallback supported. */
	ECommDbCdmaOpCapabilityFallback = 0x4
	};

/** Enum for the global setting `CDMA_SIMIP_AUTH_SUPPORTED`
@publishedAll
@released */
enum TCommDbCdmaSimpIpAuthCapability
	{
	/** CHAP supported.  */
	ECommDbCdmaSimpIpCapabilityChap = 0x1,
	/** PAP supported. */
	ECommDbCdmaSimpIpCapabilityPap = 0x2
	};

/** Enum for the global settings `CDMA_MIP_MN_AAA_AUTH_ALGORITHM` and 'CDMA_MIP_MN_HA_AUTH_ALGORITHM'
@publishedAll
@released */
enum TCommDbCdmaMIpAuthCapability
	{
	/**  MD5 authentication is supported (RFC 3012). */
	ECommDbCdmaMIpMd5 = 0x1
	};

/** Enum for the fields `PAN_SERVICE_EXTENSION:PAN_LOCAL_ROLE` and
	`PAN_SERVICE_EXTENSION:PAN_REMOTE_ROLE`
@publishedAll
@released */
enum TCommDbBluetoothPanRole
	{
	/** Role not specified. */
	ECommDbPanRoleUnknown = 0x0000,
	/** PAN-U Role. */
	ECommDbPanRoleU = 0x1115,
	/** PAN-NAP Role. */
	ECommDbPanRoleNap = 0x1116,
	/** PAN-GN Role. */
	ECommDbPanRoleGn = 0x1117
	};

/** ENum for the fields `WLAN_SERVICE_EXTENSION:EAP_SEC_OUTER_EAP_TYPE` */
enum TCommsDatEAPSECOuterEAPType
	{
	/** No Security */
	ECommsDatEAPSecNone = 0x0,
	/** EAP-MD5 */
	ECommsDatEAPSecEAPMD5 = 0x1,
	/** EAP-TLS */
	ECommsDatEAPSecEAPTLS = 0x2,
	/** EAP-TTLS */
	ECommsDatEAPSecEAPTTLS = 0x3,
	/** PEAP */
	ECommsDatEAPSecPEAP = 0x4,
	/** LEAP */
	ECommsDatEAPSecLEAP = 0x5,
	/** EAP-SIM */
	ECommsDatEAPSecEAPSIM = 0x6,
	/** EAP-AKA */
	ECommsDatEAPSecEAPAKA = 0x7,
	/** EAP-MSCHAPv2 */
	ECommsDatEAPSecEAPMSCHAPv2 = 0x8,
	/** EAP-WPS */
	ECommsDatEAPSecEAPWPS = 0x9
	};

/** ENum for the fields `WLAN_SERVICE_EXTENSION:TUN_EAP_INNERTYPE` */
enum TCommsDatTunEapInnerType
	{
	/** No Security */
	ECommsDatTunEapNone = 0x0,
	/** EAP-MD5 */
	ECommsDatTunEapEAPMD5 = 0x1,
	/** EAP-MSCHAPv2 */
	ECommsDatTunEapMSCHAPv2 = 0x8
	};

/** ENum for the fields `WLAN_SERVICE_EXTENSION:WLAN_ENCRYPTION_TYPE` */
enum TCommsDatWlanEncrytionType
	{
	/** None */
	ECommsDatWlanEncryptionTypeNone = 0x0,
	/** Static WEP */
	ECommsDatWlanEncryptionTypeStaticWEP = 0x1,
	/** TKIP */
	ECommsDatWlanEncryptionTypeTKIP = 0x2,
	/** AES */
	ECommsDatWlanEncryptionTypeAES = 0x3
	/* Dynamic WEP */
	/*ECommsDatWlanEncryptionTypeDynamicWEP = 0x4*/
	};

/** ENum for the fields `WLAN_SERVICE_EXTENSION:WLAN_DIALOG_PREF` */
enum TCommsDatWlanDialogPref
	{
	/** Unknown */
	ECommsDatWlanDialogPrefUnknown = 0x0,
	/** Prompt */
	ECommsDatWlanDialogPrefPrompt = 0x1,
	/** Do Not Prompt */
	ECommsDatWlanDialogPrefNoPrompt = 0x2,
	/** Warnings */
	ECommsDatWlanDialogPrefWarn = 0x3
	};

/** ENum for the fields `WLAN_SERVICE_EXTENSION:WLAN_NETWORK_TYPE` */
enum TCommsDatWlanNetworkType
	{
	/** Infrastructure Network */
	ECommsDatWlanNetworkTypeInfrastructure = 0x0,
	/** Ad-Hoc / Independent Network */
	ECommsDatWlanNetworkTypeAdHoc = 0x1
	};

/** ENum for the fields `WLAN_SERVICE_EXTENSION:WLAN_DES_TRANS_RATE` */
enum TCommsDatWlanDesTransRate
	{
	/** 1 Mbps */
	ECommsDatWlanDesTransRate1Mbps    = 0x02,
	/** 2 Mbps */
	ECommsDatWlanDesTransRate2Mbps    = 0x04,
	/** 5.5 Mbps */
	ECommsDatWlanDesTransRate5Mbps    = 0x0b,
	/** 6 Mbps */
	ECommsDatWlanDesTransRate6Mbps	= 0x0c,
	/** 9 Mbps */
	ECommsDatWlanDesTransRate9Mbps	= 0x12,
	/** 11 Mbps */
	ECommsDatWlanDesTransRate11Mbps   = 0x16,
	/** 12 Mbps */
	ECommsDatWlanDesTransRate12Mbps	= 0x18,
	/** 18 Mbps */
	ECommsDatWlanDesTransRate18Mbps	= 0x24,
	/** 22 Mbps */
	ECommsDatWlanDesTransRate22Mbps   = 0x2c,
	/** 24 Mbps */
	ECommsDatWlanDesTransRate24Mbps	= 0x30,
	/** 33 Mbps */
	ECommsDatWlanDesTransRate33Mbps	= 0x42,
	/** 36 Mbps */
	ECommsDatWlanDesTransRate36Mbps	= 0x48,
	/** 48 Mbps */
	ECommsDatWlanDesTransRate48Mbps	= 0x60,
	/** 54 Mbps */
	ECommsDatWlanDesTransRate54Mbps	= 0x6c,
	/** Automatic */
	ECommsDatWlanDesTransRateAuto = 0xff
	};

/** ENum for the fields `WLAN_SERVICE_EXTENSION:WLAN_POWERSAVE_MODE` */
enum TCommsDatWlanPowerSaveMode
	{
	/** Disabled */
	ECommsDatWlanPowerSaveModeDisabled = 0x0,
	/** Fast Powersave mode */
	ECommsDatWlanPowerSaveModeFast = 0x1,
	/** Max Powersave mode */
	ECommsDatWlanPowerSaveModeMax = 0x2
	};

/** ENum for the fields `WLAN_SERVICE_EXTENSION:WLAN_PREAMBLE_TYPE` */
enum TCommsDatWlanPreambleType
	{
	/** Short */
	ECommsDatWlanPreambleTypeShort = 0x0,
	/** Long */
	ECommsDatWlanPreambleTypeLong = 0x1
	};

/** ENum for the fields `WLAN_SERVICE_EXTENSION:WLAN_REG_DOMAIN` */
enum TCommsDatWlanRegDomain
	{
	/** FCC - United States */
	ECommsDatWlanRegDomainFCC = 0x10,
	/** IC - Canada */
	ECommsDatWlanRegDomainIC = 0x20,
	/** ETSI - Most of Europe */
	ECommsDatWlanRegDomainETSI = 0x30,
	/** Spain */
	ECommsDatWlanRegDomainSpain = 0x31,
	/** France */
	ECommsDatWlanRegDomainFrance = 0x32,
	/** MKK - Japan */
	ECommsDatWlanRegDomainMKK = 0x40,

	// Country codes

	/** AT - Austria */
	ECommsDatWlanRegDomainAT = 0x4154,
	/** AU - Australia */
	ECommsDatWlanRegDomainAU = 0x4155,
	/** BE - Belgium */
	ECommsDatWlanRegDomainBE = 0x4245,
	/** BR - Brazil */
	ECommsDatWlanRegDomainBR = 0x4252,
	/** CA - Canada */
	ECommsDatWlanRegDomainCA = 0x4341,
	/** CH - Switzerland */
	ECommsDatWlanRegDomainCH = 0x4348,
	/** CN - China */
	ECommsDatWlanRegDomainCN = 0x434E,
	/** CY - Cyprus */
	ECommsDatWlanRegDomainCY = 0x4359,
	/** CZ - Czech Republic */
	ECommsDatWlanRegDomainCZ = 0x435A,
	/** DE - Germany */
	ECommsDatWlanRegDomainDE = 0x4445,
	/** DK - Denmark */
	ECommsDatWlanRegDomainDK = 0x444B,
	/** EE - Estonia */
	ECommsDatWlanRegDomainEE = 0x4545,
	/** ES - Spain */
	ECommsDatWlanRegDomainES = 0x4553,
	/** FI - Finland */
	ECommsDatWlanRegDomainFI = 0x4649,
	/** FR - France */
	ECommsDatWlanRegDomainFR = 0x4652,
	/** GB - United Kingdom */
	ECommsDatWlanRegDomainGB = 0x4742,
	/** GR - Greece */
	ECommsDatWlanRegDomainGR = 0x4752,
	/** HK - Hong Kong */
	ECommsDatWlanRegDomainHK = 0x484B,
	/** HU - Hungary */
	ECommsDatWlanRegDomainHU = 0x4855,
	/** ID - Indonesia */
	ECommsDatWlanRegDomainID = 0x4944,
	/** IE - Ireland */
	ECommsDatWlanRegDomainIE = 0x4945,
	/** IL - Israel */
	ECommsDatWlanRegDomainIL = 0x494C,
	/** IS - Iceland */
	ECommsDatWlanRegDomainIS = 0x4953,
	/** IT - Italy */
	ECommsDatWlanRegDomainIT = 0x4954,
	/** JP - Japan */
	ECommsDatWlanRegDomainJP = 0x4A50,
	/** KR - Republic of Korea */
	ECommsDatWlanRegDomainKR = 0x4B52,
	/** LT - Lithuania */
	ECommsDatWlanRegDomainLT = 0x4C54,
	/** LU - Luxembourg */
	ECommsDatWlanRegDomainLU = 0x4C55,
	/** LV - Latvia */
	ECommsDatWlanRegDomainLV = 0x4C56,
	/** MY - Malaysia */
	ECommsDatWlanRegDomainMY = 0x4D59,
	/** NL - Netherlands */
	ECommsDatWlanRegDomainNL = 0x4E4C,
	/** NO - Norway */
	ECommsDatWlanRegDomainNO = 0x4E4F,
	/** NZ - New Zealand */
	ECommsDatWlanRegDomainNZ = 0x4E5A,
	/** PH - Philippines */
	ECommsDatWlanRegDomainPH = 0x5048,
	/** PL - Poland */
	ECommsDatWlanRegDomainPL = 0x504C,
	/** PT - Portugal */
	ECommsDatWlanRegDomainPT = 0x5054,
	/** SE - Sweden */
	ECommsDatWlanRegDomainSE = 0x5345,
	/** SG - Singapore */
	ECommsDatWlanRegDomainSG = 0x5347,
	/** SI - Slovenia */
	ECommsDatWlanRegDomainSI = 0x5349,
	/** SK - Slovakia */
	ECommsDatWlanRegDomainSK = 0x534B,
	/** TH - Thailand */
	ECommsDatWlanRegDomainTH = 0x5448,
	/** TW - Taiwan */
	ECommsDatWlanRegDomainTW = 0x5457,
	/** US - United States */
	ECommsDatWlanRegDomainUS = 0x5553,
	/** ZA - South Africa */
	ECommsDatWlanRegDomainZA = 0x5A41,
	/** World */
	ECommsDatWlanRegDomainWorld = 0xFF
	};

//Regulatory Domain / Country Code information

struct TCountryInfo
	{
	TUint16 iDomain;
	TUint8  iFirstChannel;
	TUint8	iNumberOfChannels;
	TUint8	iMaxTxPower; // in units of dBm
	};

const struct TCountryInfo KCountryChannels[]= {//put supported channels at beginning of array and fill rest with zeros (0)
    { ECommsDatWlanRegDomainAT, 1, 11, 20},
    { ECommsDatWlanRegDomainAU, 1, 11, 23},
    { ECommsDatWlanRegDomainBE, 1, 13, 20},
    { ECommsDatWlanRegDomainBR, 1, 11, 60},
    { ECommsDatWlanRegDomainCA, 1, 11, 60},
    { ECommsDatWlanRegDomainCH, 1, 11, 20},
    { ECommsDatWlanRegDomainCN, 1, 13, 22},
    { ECommsDatWlanRegDomainCY, 1, 11, 60},
	{ ECommsDatWlanRegDomainCZ, 1, 11, 23},
    { ECommsDatWlanRegDomainDE, 1, 11, 20},
    { ECommsDatWlanRegDomainDK, 1, 11, 20},
    { ECommsDatWlanRegDomainEE, 1, 11, 60},
    { ECommsDatWlanRegDomainES, 1, 11, 20},
    { ECommsDatWlanRegDomainFI, 1, 11, 20},
    { ECommsDatWlanRegDomainFR, 1, 11, 20},
    { ECommsDatWlanRegDomainGB, 1, 11, 20},
	{ ECommsDatWlanRegDomainGR, 1, 11, 20},
    { ECommsDatWlanRegDomainHK, 1, 11, 20},
    { ECommsDatWlanRegDomainHU, 1, 11, 60},
    { ECommsDatWlanRegDomainID, 1, 13, 20},
    { ECommsDatWlanRegDomainIE, 1, 11, 20},
    { ECommsDatWlanRegDomainIL, 1, 13, 20},
    { ECommsDatWlanRegDomainIS, 1, 11, 20},
    { ECommsDatWlanRegDomainIT, 1, 11, 20},
    { ECommsDatWlanRegDomainJP, 1, 13, 10},
    { ECommsDatWlanRegDomainKR, 1, 13, 22},
    { ECommsDatWlanRegDomainLT, 1, 11, 60},
    { ECommsDatWlanRegDomainLU, 1, 11, 20},
    { ECommsDatWlanRegDomainLV, 1, 11, 60},
    { ECommsDatWlanRegDomainMY, 1, 13, 20},
    { ECommsDatWlanRegDomainNL, 1, 11, 20},
    { ECommsDatWlanRegDomainNO, 1, 11, 20},
	{ ECommsDatWlanRegDomainNZ, 1, 11, 60},
    { ECommsDatWlanRegDomainPH, 1, 11, 60},
    { ECommsDatWlanRegDomainPL, 1, 11, 20},
    { ECommsDatWlanRegDomainPT, 1, 11, 20},
    { ECommsDatWlanRegDomainSE, 1, 11, 20},
    { ECommsDatWlanRegDomainSG, 1, 13, 23},
    { ECommsDatWlanRegDomainSI, 1, 11, 60},
    { ECommsDatWlanRegDomainSK, 1, 11, 60},
	{ ECommsDatWlanRegDomainTH, 1, 13, 20},
    { ECommsDatWlanRegDomainTW, 1, 13, 20},
    { ECommsDatWlanRegDomainUS, 1, 11, 60},
    { ECommsDatWlanRegDomainZA, 1, 13, 60},
    { 0,                        0,  0, 0}
	};

/** ENum for the fields `WLAN_SERVICE_EXTENSION:WLAN_AUTHENTICATION_MODE` */
enum TCommsDatWlanAuthenticationMode
	{
	/** Open Authentication */
	ECommsDatWlanAuthenticationModeOpen,
	/** Shared Key */
	ECommsDatWlanAuthenticationModeShared,
	/** WPA */
	ECommsDatWlanAuthenticationModeWPA,
	/** WPA-PSK */
	ECommsDatWlanAuthenticationModeWPAPSK,
	/** Open Authentication using EAP */
	ECommsDatWlanAuthenticationModeOPENEAP,
	/** WPA2 */
	ECommsDatWlanAuthenticationModeWPA2,
	/* WPA2-PSK */
	ECommsDatWlanAuthenticationModeWPA2PSK,
	/* EAP-WPS */
	ECommsDatWlanAuthenticationModeEapWps
	};


#endif //CDBCOLS_H
