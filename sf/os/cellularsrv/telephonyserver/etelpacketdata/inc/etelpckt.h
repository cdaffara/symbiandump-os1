// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ETel Multimode Packet Data API 
// MBMS Broadcast,GPRS Release 97/98, CDMAOne, CDMA2000, Release 99 and Release 4.
// Copyright (c) Symbian Software Ltd. 2001-2009.  All rights reserved.
// 
//

#if !defined(__ETELPCKT_H__)
#define __ETELPCKT_H__

#include <e32base.h>
#include <s32mem.h>
#include <etel.h>
#include <cdblen.h>
#include <exterror.h>
#include <etelutils.h>
#include <mbmstypes.h>

//
//  Global constants and types
// 

// General
// Passed into RTelServer::IsSupportedByModule()
/**
Unique Packet Data API Identifier for Packet API version 1.0.

@publishedPartner
@released
*/
const TInt KETelExtPcktV1 = 4000;	
/**
Unique Packet Data API Identifier for Packet API version 2.0.

@publishedPartner
@released
*/

const TInt KETelExtPcktV2 = 8000;	 


/**
Unique Packet Data API Identifier for Packet API version 3.0.
aka 3GPP Release 5 support

@publishedPartner
@released
*/
const TInt KETelExtPcktV3 = 12000;	 
 

// GPRS-specific causes for Session Management (Annex I, 3GPP TS 24.008)
/** This cause is sent to the MS if it requests an IMSI attachment for GPRS services, 
but is not allowed to operate GPRS services.  

@publishedPartner
@released
*/
const TInt KErrGprsServicesNotAllowed=KErrGsmMobilityManagementBase-7;
/** This cause is sent to the MS if it requests a combined IMSI attach for GPRS 
and non-GPRS services, but is not allowed to operate either of them. 

@publishedPartner
@released
*/
const TInt KErrGprsAndNonGprsServicesNotAllowed=KErrGsmMobilityManagementBase-8;
/** This cause is sent to the MS when the network cannot derive the MS's identity 
from the P-TMSI in case of an inter-SGSN routing area update.  

@publishedPartner
@released
*/
const TInt KErrGprsMSIdentityCannotBeDerivedByTheNetwork=KErrGsmMobilityManagementBase-9;
/** This cause is sent to the MS either if the network has implicitly detached 
the MS, e.g. some while after the Mobile reachable timer has expired, or if 
the GMM context data relating to the subscription does not exist in the SGSN, 
e.g. because of a SGSN restart.  

@publishedPartner
@released
*/
const TInt KErrGprsMSImplicitlyDetached=KErrGsmMobilityManagementBase-10;
/** This cause is sent to the MS if it requests a combined GPRS attachment or routing 
is updating in a PLMN where the MSC is temporarily not reachable via the GPRS 
part of the GSM network.  

@publishedPartner
@released
*/
const TInt KErrGprsMSCTemporarilyNotReachable=KErrGsmMobilityManagementBase-16;
/** The PDP context is deactivated because of a LLC or SNDCP failure.  

@publishedPartner
@released
*/
const TInt KErrGprsLlcOrSndcpFailure=KErrGsmMobilityManagementBase-25;
/** Insufficient GPRS resources are available for the request to complete successfully.  

@publishedPartner
@released
*/
const TInt KErrGprsInsufficientResources=KErrGsmMobilityManagementBase-26;
/** Access Point Name is either missing from the Activation request or is unknown.  

@publishedPartner
@released
*/
const TInt KErrGprsMissingorUnknownAPN=KErrGsmMobilityManagementBase-27;
/** The PDP address supplied with the Activation request is unknown.  

@publishedPartner
@released
*/
const TInt KErrGprsUnknownPDPAddress=KErrGsmMobilityManagementBase-28;
/** User authentication has failed.  

@publishedPartner
@released
*/
const TInt KErrGprsUserAuthenticationFailure=KErrGsmMobilityManagementBase-29;
/** The GGSN has rejected the activation request for some reason.  

@publishedPartner
@released
*/
const TInt KErrGprsActivationRejectedByGGSN=KErrGsmMobilityManagementBase-30;
/** The activation request has been rejected for an unspecified reason.  

@publishedPartner
@released
*/
const TInt KErrGprsActivationRejected=KErrGsmMobilityManagementBase-31;
/** The requested service is not supported by the PLMN.

@publishedPartner
@released
*/
const TInt KErrGprsServiceOptionNotSupported=KErrGsmMobilityManagementBase-32;
/** There is no active subscription to the requested service.

@publishedPartner
@released
*/
const TInt KErrGprsRequestedServiceOptionNotSubscribed=KErrGsmMobilityManagementBase-33;
/** The service is temporarily out of order.

@publishedPartner
@released
*/
const TInt KErrGprsServiceOptionTemporarilyOutOfOrder=KErrGsmMobilityManagementBase-34;
/** The NSAPI supplied with the activation request is not unique.  

@publishedPartner
@released
*/
const TInt KErrGprsNSAPIAlreadyUsed=KErrGsmMobilityManagementBase-35;
/** The context has been deactivated successfully.  

@publishedPartner
@released
*/
const TInt KErrGprsRegularContextDeactivation=KErrGsmMobilityManagementBase-36;
/** The Quality of Service requested has not been accepted.  

@publishedPartner
@released
*/
const TInt KErrGprsQoSNotAccepted=KErrGsmMobilityManagementBase-37;	
/** The GPRS network is requesting reactivation of a context.  

@publishedPartner
@released
*/
const TInt KErrGprsReactivationRequested=KErrGsmMobilityManagementBase-39;
/** The feature is not supported.  

@publishedPartner
@released
*/
const TInt KErrGprsFeatureNotSupported=KErrGsmMobilityManagementBase-40;
/** There is a semantic error in the TFT operation.

@publishedPartner
@released
*/
const TInt KErrGprsSemanticErrorInTFTOperation=KErrGsmMobilityManagementBase-41;
/** There is a syntactical error in the TFT operation.

@publishedPartner
@released
*/
const TInt KErrGprsSyntacticalErrorInTFTOperation=KErrGsmMobilityManagementBase-42;
/** The PDP context activation request is not active. 

@publishedPartner
@released
*/
const TInt KErrGprsUnknownPDPContext=KErrGsmMobilityManagementBase-43;
/** There is one or more semantic errors in packet filter(s) of the TFT.
 
@publishedPartner
@released
*/
const TInt KErrGprsSemanticErrorsInPacketFilters=KErrGsmMobilityManagementBase-44;
/** There is one or more syntactical errors in packet filter(s) of the TFT.

@publishedPartner
@released
*/
const TInt KErrGprsSyntacticalErrorInPacketFilters=KErrGsmMobilityManagementBase-45;
/** The GPRS network has already activated a PDP context without TFT.

@publishedPartner
@released
*/
const TInt KErrGprsPDPContextWithoutTFTAlreadyActivated=KErrGsmMobilityManagementBase-46;

// UMTS-specific cause values for Call control (Annex H, 3GPP TS 24.008)
/** QoS not available.

@publishedPartner
@released
*/
const TInt KErrUmtsQoSNotAvailable=KErrGsmMobilityManagementBase-49;

/** Maximum number of PDP contexts supported by phone exceeded.  

@publishedPartner
@released
*/
const TInt KErrUmtsMaxNumOfContextExceededByPhone=KErrGsmMobilityManagementBase-50;
/** Maximum number of PDP contexts supported by the network exceeded.  

@publishedPartner
@released
*/
const TInt KErrUmtsMaxNumOfContextExceededByNetwork=KErrGsmMobilityManagementBase-51;
/** Phone is in offline mode.  

@publishedPartner
@released
*/
const TInt KErrGprsOfflineMode=KErrGsmMobilityManagementBase-52;
/** 
 * MBMS network support does not exist.
 * @publishedPartner
 * @released
 */
const TInt KErrMbmsNotSupported=KErrGsmMobilityManagementBase-53;
/**
 * Specific MBMS service is not available.  
 * @publishedPartner
 * @released
 */
const TInt KErrMbmsServiceUnavailable=KErrGsmMobilityManagementBase-54;
/**
 * Preempts low priority MBMS/Non-MBMS service.  
 * @publishedPartner
 * @released
 */
const TInt KErrMbmsServicePreempted=KErrGsmMobilityManagementBase-55;
/**
 * One or more invalid MBMS service entry supplied.  
 * @publishedPartner
 * @released
 */
const TInt KErrMbmsImpreciseServiceEntries=KErrGsmMobilityManagementBase-56;

class TPacketDataConfigBase 
/** Base class for the packet configuration classes.

The Context configuration, QoS profile (Requested and Negotiated) and QoS 
Capabilities classes inherit from this class. It is outside the scope of all 
the Packet data classes. 

To allow future configuration structures (e.g. TContextConfigWCDMA, 
TContextConfigPDC, etc.) to be added in such a way that a completely different 
structure may be defined and passed into the same functions as this one, all 
existing configuration structures are derived from this thin base class 
which provides just one function, ExtensionId(). This function returns the 
one protected member variable, iExtensionId. A new structure would define an 
associated new constant to be set in iExtensionId on construction, which would 
then be queried by the TSY prior to unpackaging the structure. 

@see RPacketContext::TContextConfigCDMA 
@see RPacketContext::TContextConfigR99_R4  
@see RPacketContext::TContextConfig_R5
@see RPacketContext::TContextConfigMbmsV1

@publishedPartner
@released

*/
	{
public:
	/**
	Flags identifying the different packet networks.
	*/
	enum 
		{
		/**
		GPRS Release 97/98 packet network.
		*/
		KConfigGPRS= 0x01,				
		/**
		CDMA and CDMA2000 packet networks.
		@deprecated 9.5
		*/
		KConfigCDMA = 0x02,		
		/**
		GPRS/UMTS Release 99 and UMTS Release 4 networks.
		*/ 
		KConfigRel99Rel4 = 0x04,
		/**
		UMTS/IMS 3GPP Release 5 networks.
		*/ 
		KConfigRel5 = 0x05,	
		/**
		MBMS networks
		*/
		KConfigMBMS = 0x06		
		};
		
	/** This member returns the type of class.
	
	@return The type of class. */
	inline TInt ExtensionId(){return iExtensionId;};
protected:
	/**
	Defines the type of class: either KConfigGPRS, KConfigCDMA or KConfigRel99Rel4,
	which identifies the type of packet network (GPRS R97/98, CDMA, Release 99 or 4 or 5)
	*/
	TInt iExtensionId;	
	};

class TConnectionInfoBase 
/** Base class for the configuration info classes. It is outside the scope of 
all the Packet data classes. 

To allow future connection information structures (e.g. TConnectionInfoV2, 
TConnectionInfoV3, etc.) to be added and as a result information carried by the 
structure can be increased. All information structures are derived from this 
thin base class which provides just one function, ExtensionId(). This function 
returns the protected member variable, iExtensionId. A new structure would 
define an associated new constant to be set in iExtensionId on construction, 
which would then be queried by the TSY prior to unpackaging the structure. 

@see RPacketContext::TConnectionInfoV1

@publishedPartner
@released
*/
	{
public:
	/**
	Flags identifying the different connection information structures
	*/
	enum 
		{
		/**
		Connection information version 1.
		*/
		KConnectionInfoV1 = 0x01
		};
		
	/** Return the type of class.
	*
	@return The version of class. 
	*/
	inline TInt ExtensionId(){return iExtensionId;};
	
public:
	/** A bit-wise sum of TConnectionInfoFlags indicating which of the optional 
	fields contain valid data. */
    TUint32 iValid;        	
	
protected:
	/** Defines the type of class. */
	TInt iExtensionId;	
	};

class TPacketBase
/** 
Base class for the Packet data ETel API parameters which are version dependent
(version 2 onwards). Note that even though the 7.0 version of the API does not define
version dependent classes, this implementation HAS been employed in the v7.0S version
(V2.0) of the API.

The TPacketBase::ExtensionId() method is used to determine which version of a packaged
parameter has been created. 
Note that the TPacketDataConfigBase::ExtensionId() method returns a packet network 
type.

@publishedPartner
@released
*/
	{
public:
	IMPORT_C TInt ExtensionId() const;
protected:
	TPacketBase();
	TInt iExtensionId;					//< Extension identifier
	};
											   
class TPacketType
/**
Base class for all the V1 parameter types.

@publishedPartner
@released
*/
{
public:
	IMPORT_C TInt ExtensionId() const;
protected:
	TPacketType();
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
protected:
	TInt iExtensionId;
};
	
struct TClientId
/**Holds the session and subsession handles which would be used  
within the 2-phase list retrieval mechanism. During both the phases, 
the client-side API code will pass down the TClientId so that TSY can use
this information to match the first phase of the request to the second phase. */
	{
	/** The handle to the underlying RTelServer session. */
	TInt iSessionHandle;
	/** The handle to the sub-session to which this API request relates. */
	TInt iSubSessionHandle;
	};	
		
class RPacketService;
class CPacketContextPtrHolder;
class RPacketContext : public RTelSubSessionBase
/** Defines the context and the configuration of a connection to the network's 
packet-switched service. 

The functionality associated with the context of a packet-switched connection 
(configuring, modifying, activating, etc) are encapsulated within this class.

Each instance of this class has an RPacketService parent, to which it belongs.

@publishedPartner
@released
*/
	{
public:
	IMPORT_C RPacketContext();
	IMPORT_C TInt OpenNewContext(RPacketService& aPacketNetwork, TDes& aContextName);	
	IMPORT_C TInt OpenExistingContext(RPacketService& aPacketNetwork,const TDesC& aContextName);	
	IMPORT_C void Close();
// 
//	Nested classes dealing with context configurations for each network type
//
	enum {KGSNNameLength = 252};	// 3GPP TS 24.008, section 10.5.6.1 gives a range of 3 - 102 octets for the APN. 252 has been chosen as an adequate value for practical purposes.
	enum {KMaxPDPAddressLength = KCommsDbSvrMaxFieldLength}; 
	enum 
		{
		KPdpDataCompression		= 0x01,
		KPdpHeaderCompression	= 0x02
		};

/** Buffer to hold the GSN address.

@see KGSNNameLength */
	typedef TBuf8<KGSNNameLength> TGSNAddress;				//< GPRS Support Node (GSN) name
/** Buffer to hold the protocol address.

@see KMaxPDPAddressLength */
	typedef TBuf8<KMaxPDPAddressLength> TProtocolAddress;	//< GPRS Rel97/98, CDMA, Rel99 and Rel4.

	//
	// TProtocolType - defines the protocol used to connect to the packet data gateway
	// 
/** Defines the protocol used to connect to the packet data gateway.

Modes: GRPS */
	enum TProtocolType
		{
	/** IPv4 type gateway.
	
	Modes: GRPS */
		EPdpTypeIPv4,	
	/** IPv6 type gateway.
	
	Modes: GPRS */
		EPdpTypeIPv6,	
	/** X.25 type gateway.
	
	Modes: GPRS */
		EPdpTypeX25,	
	/** PPP type gateway.
	
	Modes: GRPS, CDMA */
		EPdpTypePPP,	
	/** CDPD type gateway.
	
	Modes: CDMA 
	@deprecated 9.5
	*/
		EPdpTypeCDPD
		};

	//
	// TServiceOption - service option numbers are CDMA specific and used in the API to 
	// identify the various packet data services available on the CDMA packet network.
	// 
/** Service option numbers identify the various packet data services available 
on the CDMA packet network.

Modes: CDMA
@deprecated 9.5
 */
	enum TServiceOption
		{
	/** Service Options 7 and 15, 8-16. */
		KLowSpeedData		   = 0x01,	
	/** Service Options 22-25 IP and 26-29 CDPD. */
		KHighSpeedData	 	   = 0x02,	
	/** Service Option 33 on CDMA2000 network only. */
		KHighSpeedCDMA2000Data = 0x04	
		};

	/** Anonymous access required.

	Modes: GRPS, CDMA */
	enum TAnonymousAccess
		{
	/** Anonymous access not applicable.
	
	Modes: GRPS, CDMA */
		ENotApplicable,
	/** Anonymous access required.
	
	Modes: GRPS */
		ERequired,		
	/** Anonymous access not required.
	
	Modes: GPRS */
		ENotRequired	
		};

	enum { KMaxAuthDataLength = 50 };
	/** Buffer for various sorts of authorisation data since version 2 of the API.
	
	Supported from v7.0S.
	*/
	typedef TBuf8<KMaxAuthDataLength> TAuthData;

	//
	// TAuthProtocol - this enables the client to set the protocol type used on the context
	// 
	enum TAuthProtocol
        {
        EProtocolNone,
        EProtocolPAP,
        EProtocolCHAP
        };

	//
	// TAuthInfo - this enables the client to set the authentication data used on the context
	//
	struct TAuthInfo
		{
		TAuthProtocol iProtocol;
        TAuthData iUsername;
        TAuthData iPassword;
		};

	//
	// TDnsInfoV2 - this class enables the client to set the primary and secondary DNS server
	// names used on the context 
	//
	class TDnsInfoV2 : public TPacketBase
	/**
	Enables the client to set the primary and secondary DNS server
	names used on the context since version 2 of the API.
	
	Supported from v7.0S.

	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TDnsInfoV2();
	public:
		/** Primary DNS server name.*/
		TProtocolAddress iPrimaryDns;
		/** Secondary DNS server name.*/
		TProtocolAddress iSecondaryDns;
		};
	/**
	DNS information package since version 2 of the API.
	
	Supported from v7.0S.
	*/
	typedef TPckg<TDnsInfoV2> TTDnsInfoV2Pckg;

	//
	//  - this buffer can be used to store protocol related data
	// (such as "PPP config options") which do not fit into the other fields of the
	// TProtocolConfigOptionV2 class (see below).  The length of the buffer is based on
	// the previous buffer used (before TProtocolConfigOptionV2 was implemented), which 
	// was based on 3GPP TS 24.008 (253 octets).
	//
	enum {KMiscProtocolBufferLength = 253};
	typedef TBuf8<KMiscProtocolBufferLength> TMiscProtocolBuffer;
		
	class TProtocolConfigOptionV2 : public TPacketBase
	/**
	Holds authentication data encapsulated in TAuthInfo, as 
	well as further data that may be required for CHAP protocol 
	authentication, such a challenge and response, since version
	2 of the API.
	
	Can contain the DNS server names, the username, password, 
	and CHAP associated data.

	Normal authentication uses only TAuthInfo::iUsername and 
	TAuthInfo::iPassword. The other parameters are for secure 
	authentication. 

	The maximum combined length of the TAuthInfo::iUsername 
	and the secure information (iPassword or iResponse) is 50 bytes.
	
	Rather than keep the Protocol Config Option as a buffer it 
	is encapsulated as a class. This enables data extraction to 
	occur easily.
	
	Supported from v7.0S. 

	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TProtocolConfigOptionV2();
	public:
		/** Authentication data.*/
		TAuthInfo iAuthInfo;
		/** The pseudorandom data that is chosen by the client.*/
		TAuthData iChallenge;
		/** Calculated by the client with the MD5 algorithm using 
		iChallenge, iId and iPassword.*/
		TAuthData iResponse;
		/** A one byte identifier that is chosen by the client.*/
		TUint8 iId;
		/** Primary and secondary DNS server names since version 2 
		of the API.*/
		TDnsInfoV2 iDnsAddresses;
		TMiscProtocolBuffer iMiscBuffer;
		};
		
	
	class TContextConfigGPRS : public TPacketDataConfigBase
	/**
	Context configurations for GPRS packet data support. 
	
	Contains all the data necessary to configure a context on a GPRS network or 
	UMTS packet network.

	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TContextConfigGPRS();
	public:
		/** The GPRS protocol type. The default value is EpdpTypeIPv4.
	
		@see TProtocolType */
		TProtocolType iPdpType;
		/** The GPRS Access point name. The default value is an empty string.
	
		@see TGSNAddress */
		TGSNAddress iAccessPointName;
		/** The GPRS protocol address. The default value is an empty string.
	
		@see TProtocolAddress */
		TProtocolAddress iPdpAddress;		
		/** The GRPS compression. This is a bit-wise sum of the wanted compression. The 
		default value is 0.
	
		@see KPdpDataCompression
		@see KPdpHeaderCompression */
		TUint iPdpCompression;
		/** Anonymous access requested. The default value is ENotRequired.
	
		@see TAnonymousAccess */
		TAnonymousAccess iAnonymousAccessReqd;
		/** Use Edge. The default value is EFalse. True for EGPRS.*/
		TBool iUseEdge; // 
		/** Protocol configuration options for version 2 of the API.
		
		Supported from v7.0S.*/
		TProtocolConfigOptionV2 iProtocolConfigOption;
		/** Network initiated context indication. */
		TBool iNWIContext;
		};
	
  	
   	/**
	Identifiers for Packet Flow Context.  See Table 10.5.161/3GPP TS 24.008.
   	*/
   	enum TPacketFlowIdentifier
   		{
   		/**
   		Best Effort
   		*/
   		EBestEffort,
		/**
		Signalling
		*/
		ESignalling,
		/**
		Sms
		*/
   		ESms
   		};
   		
  	class TContextConfigR99_R4 : public TPacketDataConfigBase
	/**
	Enables Release 99 and Release 4 context configuration for version 2 of 
  	the API.
  	
  	Contains all the data necessary to configure a context on a R99/R4 packet 
  	network. 
  	
  	Supported from v7.0S.

	@publishedPartner
	@released
  	*/
  		{
  	public:
  		IMPORT_C TContextConfigR99_R4();
  	public:
  		/** The Rel99/Rel4 protocol type. The default value is EpdpTypeIPv4.*/
  		TProtocolType iPdpType;
  		/** The Rel99/Rel4 access point name. The default is an empty string.*/
  		TGSNAddress iAccessPointName;
  		/** The Rel99/Rel4 protocol address. The default value is an empty string.*/
  		TProtocolAddress iPdpAddress;		
  		/** Use Edge. The default is EFalse. True for EGPRS.*/
   		TBool iUseEdge; 
   		/** Protocol configuration options.*/
		TProtocolConfigOptionV2 iProtocolConfigOption;
		TBool iNWIContext; // Network initiated context indication.
		TPacketFlowIdentifier iPFI; // Packet Flow indicator.
		};

	class TContextConfigCDMA : public TPacketDataConfigBase
	/**
	Enables context configurations for CDMA and CDMA2000 packet data 
	and high speed packet data support. 
	
	Contains all the data necessary to configure a context on a CDMA packet 
	network.

	@publishedPartner
	@released
	@deprecated 9.5
	*/
		{
	public:
		IMPORT_C TContextConfigCDMA();
	public:
	/** Identifies the CDMA packet service option range (Low, High, High CDMA2000). 
	The default value is KLowSpeedData.
	
	@see TServiceOption */
		TUint			iServiceOption; 
	/** The CDMA protocol type, either PPP or CDPD. The default value is EPdpTypePPP.
	
	@see TProtocolType */
		TProtocolType	iProtocolType;	
		};

	// Configuration-related calls
	IMPORT_C void SetConfig(TRequestStatus& aStatus, const TDesC8& aConfig) const;
	IMPORT_C void GetConfig(TRequestStatus& aStatus, TDes8& aConfig) const;
	IMPORT_C void NotifyConfigChanged(TRequestStatus& aStatus, TDes8& aConfig) const;
	IMPORT_C void Activate(TRequestStatus& aStatus) const;
	IMPORT_C void Deactivate(TRequestStatus& aStatus) const;
	IMPORT_C void Delete(TRequestStatus& aStatus) const;
	IMPORT_C void LoanCommPort(TRequestStatus& aStatus, RCall::TCommPort& aDataPort) const; // Deprecated method.
	IMPORT_C void RecoverCommPort(TRequestStatus& aStatus) const; // Deprecated method.
	IMPORT_C void GetDnsInfo(TRequestStatus& aStatus, TDes8& aDnsInfo) const; 



	enum TContextStatus
		{
	/** Current status is unknown. */
		EStatusUnknown,
	/** Context is not activated. */
		EStatusInactive,
	/** Context is currently being activated. */
		EStatusActivating,
	/** Context is active. */
		EStatusActive,
	/** Context is currently being deactivated. */
		EStatusDeactivating,		
	/** Context is suspended. */
		EStatusSuspended,
	/** Context has been deleted. */
		EStatusDeleted,
	/** Context is currently in data receiving mode.The service transitions 
		to this state when the data transmission starts.This is expected 
		to happen after some time of arrival of SESSION START message from the network.
	    Note : Applicable only to MBMS context */	
		EStatusReceiving
		};

	IMPORT_C TInt GetStatus(TContextStatus& aContextStatus) const;
	IMPORT_C void NotifyStatusChange(TRequestStatus& aStatus,TContextStatus& aContextStatus);
	IMPORT_C TInt GetProfileName(TName& aQoSProfile) const;

	struct TDataVolume 
/** Holds the amount of sent and received data.

The total number of bytes sent is: iOverflowCounterSent*(2^32)+iOverflowCounterSent */
		{
		TUint32 iBytesSent;
	/** Overflow counter for the bytes sent.
	
	The total number of bytes sent is: iOverflowCounterSent*(2^32)+iOverflowCounterSent */
		TUint32 iOverflowCounterSent;
	/** The number of bytes received.
	
	The total number of bytes received is: iOverflowCounterReceived*(2^32)+iOverflowCounterReceived */
		TUint32 iBytesReceived;
	/** Overflow counter for the bytes received.
	
	The total number of bytes received is: iOverflowCounterReceived*(2^32)+iOverflowCounterReceived */
		TUint32 iOverflowCounterReceived;
		};

	struct TNotifyDataTransferredRequest
		{
		TUint iRcvdGranularity;
		TUint iSentGranularity;
		};

    /** 
    Determines which fields in the TConnectionInfoXXX parameter class are valid.
    
    @see TConnectionInfoV1 
    */
	enum TConnectionInfoFlags
		{
    	/** The HSDPA category field is valid in the connection info class.
    	
    	Modes: WCDMA */
		KHSDPACategory		= 0x00000001,
		
    	/** The HSUPA category field is valid in the connection info class.
    	
    	Modes: WCDMA */		
		KHSUPACategory		= 0x00000002
        };

  	class TConnectionInfoV1 : public TConnectionInfoBase
	/**
	Connection info class is used to carry miscellaneous information about data connection.
  	
  	Contains all kind of packet data connection related information that is useful for
  	ETel API clients.
  	
  	Supported from v9.5.

	@publishedPartner
	@released
  	*/
  		{
  	public:
  		IMPORT_C TConnectionInfoV1();
  	public:
        /**HSDPA UE category as defined in 3GPP TS 25.306 Table 5.1a: "FDD 
        HS-DSCH physical layer categories". Value "0" means HSDPA is not used,
        value "1" means category 1, value "2" means category 2 etc.*/
        TUint iHSDPACategory;
 
        /*HSUPA (E-DCH) UE category as defined in 3GPP TS 25.306 Table 5.1g: 
        "FDD E-DCH physical layer categories". Value "0" means HSUPA is not used,
        value "1" means category 1, value "2" means category 2 etc.*/       
        TUint iHSUPACategory;
		};


	IMPORT_C TInt GetDataVolumeTransferred(TDataVolume& aVolume) const; // Deprecated method.
	IMPORT_C void NotifyDataTransferred(TRequestStatus& aStatus, TDataVolume& aVolume, TUint aRcvdGranularity=0x1000,TUint aSentGranularity=0x1000) const; 
	IMPORT_C void GetConnectionSpeed(TRequestStatus& aStatus, TUint& aRate) const; // get the current speed (bits per second) of the connection
	IMPORT_C void NotifyConnectionSpeedChange(TRequestStatus& aStatus, TUint& aRate) const;  // notify client if there's a change in the connection speed

	// N.B. the following 2 methoda are only supported if telephony was built with SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW macro
    IMPORT_C void GetConnectionInfo(TRequestStatus& aStatus, TDes8& aInfo) const; // get connection information
	IMPORT_C void NotifyConnectionInfoChange(TRequestStatus &aStatus, TDes8& aInfo) const; // notify client if connection information changes	

	IMPORT_C TInt GetLastErrorCause(TInt& aError) const;

	IMPORT_C void GetDataVolumeTransferred(TRequestStatus& aStatus, TDataVolume& aVolume) const; // Recommended method variant.
	
	class TDataChannelV2 : public TPacketBase
	/**
	Contains data channel parameters.
	
	Contains the information previously returned by	LoanCommPort() 
	(iCsy and iPort), as well as the new information which is generated 
	by context creation, e.g. the context (data channel) identity, 
	for version 2 of the API.  

	It is passed as a packaged parameter to RPacketContext::InitialiseContext().
	
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TDataChannelV2();
	public:
		/**CSY name.
		@deprecated Data previously used by LoanDataPort() in version 1 of the API.*/
		TFileName iCsy;		
		/**Port name 
		@deprecated Data previously used by LoanDataPort(). */
		TName iPort;			
		/**Context Identity.
		The Channel name information returned by the lower layers (Nif). */
		TName iChannelId;	
		};
	/** 
	Data channel package since version 2 of the API.

	Supported from v7.0S.
	*/
	typedef TPckg<TDataChannelV2> TDataChannelV2Pckg;	

	IMPORT_C void InitialiseContext(TRequestStatus& aStatus, TDes8& aDataChannelV2Pckg) const; // This method supersedes the LoanCommPort() and RecoverCommPort() methods.

	enum {KIPAddressSize = 16};				   //< TIPAddress size
	/**
	Array to hold either IPv4 or IPv6 address or Subnet mask since version 2 of the API.
	
	Supported from v7.0S.
	*/	
	typedef TUint8 TIPAddress[KIPAddressSize]; 
	
	
	class TPacketFilterV2 : public TPacketBase
	/**
	Packet filter package since version 2 of the API.

	Contains member data compatible with parameters required for configuring 
	a packet filter on a Rel99/R4 packet network. 

	Inherits from the TPacketBase class to make it future 
	proof (extensible).

	Supported from v7.0S.

	@publishedPartner
	@released
	*/

		{
	public:
		IMPORT_C TPacketFilterV2();
	public:
		/** Packet filter identifier. Value range: 1 - 8.*/
		TInt iId;						
		/** Evaluation Precedence Index. Value range: 255 (lowest) - 0 (highest).*/
		TInt iEvaluationPrecedenceIndex;	
		/** Domain Source address.*/
		TIPAddress iSrcAddr;				
		/** Domain Subnet mask.*/
		TIPAddress iSrcAddrSubnetMask;		
		/** Protocol number (IPv4) or Next Header (IPv6) field of incoming packets. 
		Value range 0 - 255.*/
		TInt iProtocolNumberOrNextHeader;	 
		/** Minimum source port number of incoming packets. Value range 0 - 65535.*/
		TInt iSrcPortMin;					 
		/** Maximum source port number of incoming packets. Value range 0 - 65535.*/
		TInt iSrcPortMax;					 
		/** Lowest destination port number of incoming packets. Value range 0 - 65535.*/
		TInt iDestPortMin;					
		/** Maximum destination port number of incoming packets. Value range 0 - 65535.*/
		TInt iDestPortMax;					 
		/** Security Parameter Index, 32-bit field.*/
		TUint32 iIPSecSPI;					
		/** Type of Service (IPv4) or Traffic Class (IPv6). 
		1 octet TOS/Traffic class + 1 octet TOS/Traffic class Mask.*/
		TUint16 iTOSorTrafficClass;			 
		/** Flow label (IPv6 only) of incoming packets. 3 octets with 20-bit field 
		containing the flow label.*/
		TUint32 iFlowLabel;					
		};
	/** Packaged TPacketFilterV2.*/
	typedef TPckg<TPacketFilterV2> TPacketFilterV2Pckg;	
		
	
	/**
	Maximum length of DNS Fully Qualified Domain Name (RFC 1123)
	
	@publishedPartner
	*/
	enum TFQDNLength
	{
	KMaxFQDNLength=255,
	};
	
	typedef TBuf8<KMaxFQDNLength> TAuthorizationToken;
	
	/**
	Packet Filter type identifier.
	
	@publishedPartner
	@released
	*/
	enum TPacketFilterType
		{
		/** Unknown filter type.*/
		EPacketFilterTypeUnknown,
		/** Filter Type One.*/
		EPacketFilterTypeOne,
		/** Filter Type Two.*/
		EPacketFilterTypeTwo,
		/** Filter Type Three.*/
		EPacketFilterTypeThree,
		};
		
	class TPacketFilterV3 : public TPacketFilterV2
	/**
	Packet filter that supports packet filter types.
	Contains member data compatible with parameters required
    for configuring a packet filter on a Rel99/R4/R5 packet network. . 

	Inherits from the TPacketFilterV2 class.

	Supported from 9.1

	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TPacketFilterV3(TPacketFilterType aType);
		IMPORT_C TPacketFilterV3();
		enum TPacketFilterDataBitmapPos
			{
			/** Bitmap for Domain Source address.*/
			EPacketFilterSrcAddr						= 0x001,
			/** Bitmap for Domain Subnet mask.*/
			EPacketFilterSrcAddrSubnetMask			= 0x002,
			/** Bitmap for Protocol number (IPv4) or Next Header (IPv6) field of incoming packets.*/
			EPacketFilterProtocolNumberOrNextHeader	= 0x004,
			/** Bitmap for Minimum source port number of incoming packets.*/
			EPacketFilterSrcPortMin					= 0x008,
			/** Bitmap for Maximum source port number of incoming packets.*/
			EPacketFilterSrcPortMax					= 0x010,
			/** Bitmap for Lowest destination port number of incoming packets.*/
			EPacketFilterDestPortMin					= 0x020,
			/** Bitmap for Maximum destination port number of incoming packets.*/
			EPacketFilterDestPortMax					= 0x040,
			/** Bitmap for Security Parameter Index.*/
			EPacketFilterIPSecSPI					= 0x080,
			/** Bitmap for Type of Service (IPv4) or Traffic Class (IPv6).*/
			EPacketFilterTOSorTrafficClass			= 0x100,
			/** Bitmap for Flow label (IPv6 only) of incoming packets.*/
			EPacketFilterFlowLabel					= 0x200,
			};
			
		inline TBool IsPresent(TPacketFilterDataBitmapPos aDataMemberBitmapPos);
		inline void Reset(TPacketFilterDataBitmapPos aDataMemberBitmapPos);
		inline void Reset();
	public:
		/** Packet filter identifier. Value range: 1 - 8.*/
		using TPacketFilterV2::iId;
		/** Evaluation Precedence Index. Value range: 255 (lowest) - 0 (highest).*/
		using TPacketFilterV2::iEvaluationPrecedenceIndex;
	
	public:
		
		IMPORT_C TBool SetSrcAddr(TIPAddress &aSrcAddr);
		IMPORT_C TBool SetSrcAddrSubnetMask(TIPAddress &aSrcAddrSubnetMask);
		
		IMPORT_C TBool SetProtocolNumberOrNextHeader(TInt aProtocolNumberOrNextHeader);
		
		IMPORT_C TBool SetSrcPortMin(TInt aSrcPortMin);
		IMPORT_C TBool SetSrcPortMax(TInt aSrcPortMax);
		
		IMPORT_C TBool SetDestPortMin(TInt aDestPortMin);
		IMPORT_C TBool SetDestPortMax(TInt aDestPortMax);
		
		IMPORT_C TBool SetSPI(TUint32 aIPSecSPI);
		
		IMPORT_C TBool SetTOSorTrafficClass(TUint16 aTOSorTrafficClass);
		IMPORT_C TBool SetFlowLabel(TUint32 aFlowLabel);
		
	
	public:
	
		/** Return the Domain Source address. 
		
		@return Domain Source address.
		*/
		inline const TIPAddress& SrcAddr()
		{
			return iSrcAddr;
		};
		
		/** Return the Domain Subnet mask.
		
		@return Domain Subnet mask.
		*/
		inline const TIPAddress& SrcAddrSubnetMask()
		{
			return iSrcAddrSubnetMask;
		};
				
		inline TInt ProtocolNumberOrNextHeader();
		
		inline TInt SrcPortMin();
		inline TInt SrcPortMax();
		
		inline TInt DestPortMin();
		inline TInt DestPortMax();
		
		inline TUint32 SPI(); 
		
		inline TUint16 TOSorTrafficClass();
		inline TUint32 FlowLabel();
	protected:
		/** Initialised in the constructor - type can be I, II or III
		*/
		TPacketFilterType iType;
		/** Bitmap of allowed attributes set in the constructor in accordance to type
		of the filter as defined in TS 23.060.
		*/
		TUint16 iAllowedDataBitmap;
		/** Bitmap of data members that are set(initialised) - data member can be set
		in allowed for the specified type of the filter only.
		*/
		TUint16 iPresentDataBitmap;
		};
	/** Packaged TPacketFilterV3.*/
	typedef TPckg<TPacketFilterV3> TPacketFilterV3Pckg;	
	
	/**
	Contains Authorization Token and associated Flow Identifiers.
	Defines member data needed to set media authorization TFT parameters - single Authorization Token parameter 
	followed by the list of Flow Identifier parameter(s), as defined in 3GPP TS 24.008. 
	
	Contains data member that's instance of TPacketBase class to make it future proof (extensible); In order to support serialization,
	present implementation of TPacketBase has to be extended so that it supports methods needed (ExternalizeL()/InternalizeL()).
	Supported from 9.1.

	@publishedPartner
	@released
	*/
	class CTFTMediaAuthorizationV3 : public CBase	
		{
	public:
		IMPORT_C ~CTFTMediaAuthorizationV3();
		IMPORT_C static CTFTMediaAuthorizationV3* NewL();			
	public:	
		IMPORT_C virtual void ExternalizeL(HBufC8*& aBuffer) const;
		IMPORT_C virtual void InternalizeL(TDes8& aBuffer);
		IMPORT_C TUint ExtensionId()const;


		/** Flow identifier */	
		struct TFlowIdentifier
			{
			/** Media component number. */
			TUint16 iMediaComponentNumber;
			/** IP flow number. */
			TUint16 iIPFlowNumber;
			};				
		/** Authorization token */		
		TAuthorizationToken iAuthorizationToken;			 
		/** List of flow identifiers authorization token is granted for*/
		RArray<TFlowIdentifier>	iFlowIds;
	protected:
		CTFTMediaAuthorizationV3();
		TUint iExtensionId;
		};
	
	IMPORT_C void AddMediaAuthorizationL(TRequestStatus& aStatus, CTFTMediaAuthorizationV3& aMediaAuthorization) const;
	IMPORT_C void RemoveMediaAuthorization(TRequestStatus& aStatus, TAuthorizationToken& aAuthorizationToken) const;


	IMPORT_C TInt OpenNewSecondaryContext(RPacketService& aPacketService, const TDesC& aExistingContextName, TDes& aNewContextName);
	IMPORT_C void EnumeratePacketFilters(TRequestStatus& aStatus, TInt& aCount) const;
	IMPORT_C void GetPacketFilterInfo(TRequestStatus& aStatus, TInt aIndex, TDes8& aPacketFilterInfo) const;
	IMPORT_C void AddPacketFilter(TRequestStatus& aStatus, const TDesC8& aPacketFilterInfo) const;
	IMPORT_C void RemovePacketFilter(TRequestStatus& aStatus, TInt aId) const;
	IMPORT_C void ModifyActiveContext(TRequestStatus& aStatus) const;
	IMPORT_C void CreateNewTFT(TRequestStatus& aStatus, const TInt aSize) const;
	IMPORT_C void DeleteTFT(TRequestStatus& aStatus) const;
	
	
	
	/**
	Protocol identifiers, as defined in 24.008, section 10.5.6.3
	*/
	enum TPcoProtocolId
	{
	/** PCO identifier for LCP.*/
	EETelPcktProtocolLcpId = 0xc021,	
	/** PCO identifier for PAP.*/
	EETelPcktProtocolPapId = 0xc023,
	/** PCO identifier for CHAP.*/
	EETelPcktProtocolChapId = 0xc223,
	/** PCO identifier for IPCP.*/
	EETelPcktProtocolIpcpId = 0x8021
	};

	/**
	PCO additional parameter identifiers in MS to network direction, as defined in 3GPP TS 24.008, section 10.5.6.3
	*/
	enum TPcoIDMSToNetwork
	{
	/** PCO MS additional parameter identifier for a PCSCF address request.*/
	EEtelPcktPCSCFAddressRequest = 0x01,
	/** PCO MS additional parameter identifier for an IM CN subsystem signalling flag.*/
	EEtelPcktIMCNMSSubsystemSignallingFlag = 0x02,
	/** PCO MS additional parameter identifier for a DNS server address request.*/
	EEtelPcktDNSServerAddressRequest = 0x03,
	/** Not currently supported.*/
	EEtelPcktNotSupported = 0x04,
	};

	/**
	PCO additional parameter identifiers in  network to MS direction, as defined in 3GPP TS 24.008, section 10.5.6.3
	*/
	enum TPcoIDNetworkToMS
	{
	/** PCO network additional parameter identifier for a PCSCF address.*/
	EEtelPcktPCSCFAddress = 0x01,
	/** PCO network additional parameter identifier for an IM CN subsystem signalling flag.*/
	EEtelPcktIMCNNetworkSubsystemSignallingFlag = 0x02,
	/** PCO network additional parameter identifier for a DNS server address.*/
	EEtePcktDNSServerAddress= 0x03,
	/** PCO network additional parameter identifier for a policy control rejection code.*/
	EEtelPcktPolicyControlRejectionCode = 0x04,
	};


	/** 
	Helper class that implements PCO IE identifier.
	
	@publishedPartner
	@released
	*/
	class TPcoId : public MTlvItemIdType
	{
	public:
		inline TPcoId();
		inline TPcoId(TUint16 aId);
		inline void ExternalizeL(TDes8&)const;
		inline void InternalizeL(TDesC8&);
		inline TUint SerializedLength() const;	
		inline TUint16 Id()const;
		inline void SetId(TUint16);
		inline TBool IsEqual(const MTlvItemIdType& aOtherIdType)const;	
	protected:
		/** PCO Id*/
  		TUint16 iId;
	};
	
	/** 
	Helper class that implements PCO IE identifier length.
	
	@publishedPartner
	@released
	*/
	class TPcoItemDataLength: public MTlvItemDataLengthType
	{
	public:
		inline TPcoItemDataLength();
		inline TPcoItemDataLength(TUint8 aLen);
		inline void SetDataLength(TUint aLength);
		inline TUint DataLength() const;
		inline void ExternalizeL(TDes8&) const;
		inline void InternalizeL(TDesC8&);
		inline TUint SerializedLength()const;
	protected:
		/** length of associated item data*/
  		TUint8 iDataLength;
	};

	/** 
	Identifiers for the type of data data compression applicable for SNDCP only, 
	as defined in 3GPP TS  44.065, section 6.6.1.1.4. 
	Values listed in the enumeration do not hold same numerical values as those defined in the specifications, 
	as ENoDataComression and EManufacturerDefaultDataComression are introduced in order to be utilised when no 
	compression and device default compression algorithm are needed, respectively. 
	*/ 
	enum TDataCompression 
		{ 
		/** 
		No compression 
		*/ 
		ENoDataCompression,         
		/** 
		Compression algorithm preferred by the device manufacturer 
		*/ 
		EManufacturerDefaultDataCompression,         
		/** 
		V.42 bis 
		*/ 
		EV42, 
        /** 
        V.44   
        */ 
        EV44, 
        };
        
	/** 
	Identifiers for type of SNDCP & PDCP header compression as defined 
	in 3GPP TS  44.065, section 6.5.1.1.4, 
	Values listed in the enumeration do not hold same numerical values as those defined in the specifications, 
	as ENoHeaderCompression and EManufacturerDefaultHeaderCompression are introduced in order to be utilised when no 
	compression and device default compression algorithm are needed, respectively. 
	*/ 
	enum THeaderCompression 
		{ 
		/** 
		No compression 
		*/ 
		ENoHeaderCompression,         
		/** 
		Compression algorithm preferred by the device manufacturer 
		*/ 
		EManufacturerDefaultHeaderCompression,         
		/** 
		Compression algorithm defined in RFC 1144 
		*/ 
		ERfc1144,         
		/** 
		Compression algorithm defined in RFC 2507 
		*/ 
		ERfc2507,         
		/** 
		Robust header compression algorithm defined in RFC 3095 
		*/ 
		ERohc,         
		};

	class TContextConfig_R5 : public TContextConfigR99_R4 
	/** 
	Reintroduces support for setting of header/data compression in SNDCP & PDCP         
	Supported from v9.1. 
	@publishedPartner 
	@released
	*/ 
		{ 
	public: 
		IMPORT_C TContextConfig_R5(); 
	         
	public: 
		/** defines whether SNDCP & PDCP header compression shall be utilised or not. 
		If former is the case, it holds value of that defines compression algorithm to be exercised. 
		The default value is ENone - no compression is needed. 
		*/ 
		THeaderCompression iPdpHeaderCompression; 
		/** defines whether SNDCP data compression shall be utilised or not. 
		If former is the case, it holds value of that defines compression algorithm to be exercised.                   
		The default value is ENone - no compression is needed. 
		*/ 
		TDataCompression iPdpDataCompression; 
		}; 
	
protected:
	IMPORT_C virtual void ConstructL();
	IMPORT_C virtual void Destruct();
protected:
	CPacketContextPtrHolder* iEtelPacketContextPtrHolder;
	};

//
//                       MBMS Context 
//
class RPacketMbmsContext : public RPacketContext
/** 
Defines the MBMS context and the configuration of a connection to the network's 
packet-switched service. 
The functionality associated with the context of a MBMS connection is inherited 
from RPacketContext class.
The behaviour of OpenNewContext API is redefined.
Every instance of this class has RPacketService as a base class.

@publishedPartner
@released
*/	
	{
public:
	IMPORT_C RPacketMbmsContext();
	IMPORT_C TInt OpenNewContext(RPacketService& aPacketNetwork, TDes& aContextName);	
	IMPORT_C void Close();
	friend class CRetrievePcktMbmsSessionList;
	
	class TContextConfigMbmsV1: public TPacketDataConfigBase
	/**
	Context configurations for MBMS support. 
	
	Contains all the data necessary to configure a context on a MBMS network.
	
	@see RPacketContext::SetConfig()
	
   	@publishedPartner
	@released
	*/	
		{
	public:
		IMPORT_C TContextConfigMbmsV1();
	public:
		/** Input parameter : Temporary mobile group identity 
		 * A unique name to an MBMS service
		 */
		TTmgi   iTmgi;
		
		/** Input Parameter : MBMS access bearer information 
		 * @see TMbmsScope
		 */
		TMbmsScope	iMbmsAccessBearer;
		
		/** Input parameter : MBMS service priority options 
		 * Specifies the priority of the MBMS service to be received.
		 * @see  TMbmsServicePriority
		 */
		TMbmsServicePriority	iMbmsServicePriority;
		
		/** Input parameter : MBMS service mode 
		 * @see TMbmsServiceMode
		 */
		TMbmsServiceMode	iMbmsServiceMode;
		
		/** Input parameter : MBMS session flag 
		 * ETrue indicates that an MBMS service has one or more sessions. 
		 */
		TBool	iMbmsSessionFlag;
     	};
		
    
	//Packaged version of TContextConfigMbmsV1, used when this structure has to be passed via IPC.
	typedef TPckg<TContextConfigMbmsV1> TContextConfigMbmsV1Pckg;
	
	IMPORT_C void UpdateMbmsSessionList(TRequestStatus& aStatus,const TMbmsAction aAction, const TMbmsSessionId aSessionId = 0) const;	
	
	class CMbmsSession : public CBase
	/**
	 * Captures the list of session(s) of an MBMS service.
	 * @publishedPartner
	 * @released
	 */
     	{
     public:
	     IMPORT_C static CMbmsSession* NewL();
	     IMPORT_C void InternalizeL(const TDesC8& aBuffer);
	     IMPORT_C void ExternalizeL(HBufC8*& aBuffer);
	     IMPORT_C TUint ExtensionId() const;
	     IMPORT_C ~CMbmsSession();
     public:
     	/**
     	 * Output Parameter.
     	 * Contains the list of sessions pertaining to the MBMS service.
     	 */
     	TMbmsSessionIdList iSessionIdList;
     	/**
     	 * Defines the type of the class
     	 */
     	TUint iExtensionId;
     protected:
     	CMbmsSession();
     	};
     
	};

class CPacketPtrHolder;
class CPcktMbmsMonitoredServiceList;
class RPacketService : public RTelSubSessionBase
/**
Defines a packet-switched connection to a packet network.

This is the main packet network class. 
It encapsulates functionality associated with controlling a packet-switched connection: 
attaching, detaching, querying packet network capabilities, etc.
@publishedPartner
@released
*/
	{
public:
	IMPORT_C RPacketService();
	IMPORT_C TInt Open(RPhone& aPhone);
	IMPORT_C void Close();

	IMPORT_C void NotifyContextAdded(TRequestStatus& aStatus, TDes& aContextId) const;
	IMPORT_C void Attach(TRequestStatus& aStatus) const;	
	IMPORT_C void Detach(TRequestStatus& aStatus) const;	

	friend TInt RPacketContext::OpenNewSecondaryContext(RPacketService& aPacketService, const TDesC& aExistingContextName, TDes& aNewContextName);	//< This method calls the internal method RPacketService::PrepareOpenSecondary(const TDesC& aOriginalContextName)
	friend TInt RPacketContext::OpenNewContext(RPacketService& aPacketNetwork, TDes& aContextName);	
	friend TInt RPacketMbmsContext::OpenNewContext(RPacketService& aPacketNetwork, TDes& aContextName);	

	friend class CRetrievePcktMbmsMonitoredServices;
	
/** Defines the packet data connection status. */
	enum TStatus
		{
	/** Unattached to network and inactive packet data connection. */
		EStatusUnattached,	
	/** Attached to network and inactive packet data connection. */
		EStatusAttached,	
	/** Attached to network and active packet data connection. */
		EStatusActive,		
	/** Attached to network and suspended packet data connection. */
		EStatusSuspended	
		};

	IMPORT_C TInt GetStatus(TStatus& aPacketStatus) const;
	IMPORT_C void NotifyStatusChange(TRequestStatus& aStatus,TStatus& aPacketStatus) const;
	IMPORT_C void NotifyContextActivationRequested(TRequestStatus& aStatus, TDes8& aContextParameters) const;
	IMPORT_C void RejectActivationRequest(TRequestStatus& aStatus) const;

	struct TContextInfo
	/** Struct to contain the context information.

	@see GetContextInfo() */
		{
		/** The name of the context.
		
		@see TName */
		TName iName;
		/** The current status of the context.
		
		@see RPacketContext::TContextStatus */
		RPacketContext::TContextStatus iStatus;
		};

	IMPORT_C void EnumerateContexts(TRequestStatus& aStatus,TInt& aCount, TInt& aMaxAllowed) const; //< Enumerate all Primary and Secondary contexts (whether active or inactive)
	IMPORT_C void GetContextInfo(TRequestStatus& aStatus, TInt aIndex, TContextInfo& aInfo) const;

	//
	//	Packet Network Registration and Availability
	//
	/** Packet Network Registration/Availability. */
	enum TRegistrationStatus
		{
		/** Not registered and not searching for a network. A network may or may not be 
		available. */
		ENotRegisteredNotSearching,
		/** Registered on a home network. */
		ERegisteredOnHomeNetwork,
		/** Not registered but searching for a network. */
		ENotRegisteredSearching,
		/** Registration denied. */
		ERegistrationDenied,
		/** Registration status unknown. */
		EUnknown,
		/** Registered on foreign network. */
		ERegisteredRoaming,
		/** Not registered or searching, but network available. */
		ENotRegisteredButAvailable,	
		/** Not registered or searching, and network not available. */
		ENotRegisteredAndNotAvailable
		};

	IMPORT_C void GetNtwkRegStatus(TRequestStatus& aStatus, TRegistrationStatus& aRegistrationStatus) const;
	IMPORT_C void NotifyChangeOfNtwkRegStatus(TRequestStatus& aStatus,TRegistrationStatus& aRegistrationStatus) const;

	/** MS class configuration. */
	enum TMSClass
		{
		/** Active simultaneous PS and CS calls supported (Class A). */
		EMSClassDualMode,			
		/** Active CS and Suspended PS simultaneous calls supported (Class B). */
		EMSClassSuspensionRequired, 
		/** Active CS or Active PS only call supported (Class C). */
		EMSClassAlternateMode,		
		/** Active CS only call supported (Class C). */
		EMSClassCircuitSwitchedOnly,
		/** Active PS only call supported (Class C) */
		EMSClassPacketSwitchedOnly, 
		/** Unknown what configuration is supported */
		EMSClassUnknown				
		};

	IMPORT_C void GetMSClass(TRequestStatus& aStatus, TMSClass& aCurrentClass, TMSClass& aMaxClass) const;
	IMPORT_C void SetMSClass(TRequestStatus& aStatus, TMSClass aClass) const;
	IMPORT_C void NotifyMSClassChange(TRequestStatus& aStatus, TMSClass& aNewClass) const;

	/** Static capabilities.

	Modes: GPRS */
	enum TStaticMiscCaps
			{
		//
		// GPRS-specific
		//
			/** Suspend supported.
			
			Modes: GPRS */
			KCapsSuspendSupported=0x0001,
			/** Anonymous Access supported.
			
			Modes: GPRS */
			KCapsAASupported=0x0002,
			/** Network availability supported.
			
			Modes: GPRS */
			KCapsNetworkAvailabilitySupported=0x0004,
			/** Set default context supported.
			
			Modes: GPRS */
			KCapsSetDefaultContextSupported=0x0008,
			/** Change attach mode supported.
			
			Modes: GPRS */
			KCapsChangeAttachModeSupported=0x0010,
			/** Get data transferred supported.
			
			Modes: GPRS */
			KCapsGetDataTransferredSupported=0x0020,
			/** Notify data transferred supported
			
			Modes: GPRS */
			KCapsNotifyDataTransferredSupported=0x0040,
			/** Preferred bearer supported.
			
			Modes: GPRS */
			KCapsPreferredBearerSupported=0x0080,
			/** PDP data compression supported.
			
			Modes: GPRS */
			KCapsPdpDataCompSupported=0x0100,
			/** PDP header compression supported
			
			Modes: GPRS */
			KCapsPdpHeaderCompSupported=0x0200,
			/** MS class supported.
			
			Modes: GPRS */
			KCapsMSClassSupported=0x0400,
			/** Notify MS class changes supported.
			
			Modes: GPRS */
			KCapsNotifyMSClassSupported=0x0800,
		//
		// CDMA Specific
		//
			/** Service Option (7,15) or (8,16).
			
			Modes: CDMA2000 
			@deprecated 9.5
			*/
			KCapsCDMAOneLowSpeedDataSupported=0x00001000,	
			/** Service Option (22-25) or (26-29).
			
			Modes: CDMA2000 
			@deprecated 9.5
			*/
			KCapsCDMAOneHighSpeedDataSupported=0x00002000,	
			/** Service Option 33 or 34.
			
			Modes: CDMA2000
			@deprecated 9.5
			 */
			KCapsCDMA2000HighSpeedDataSupported=0x00004000,	
			/** PPP protocol supported.
			
			Modes: CDMA2000 
			@deprecated 9.5
			*/
			KCapsProtocolPPPSupported=0x00008000,
			/** CDPD protocol supported
			
			Modes: CDMA2000 
			@deprecated 9.5
			*/
			KCapsProtocolCDPDSupported=0x00010000,
			/*@deprecated 9.5*/
			KCapsPacketReleaseModeSupported=0x00020000,
			/*@deprecated 9.5*/
			KCapsNotifyReleaseModeChangeSupported=0x00040000,
			
			/** The phone supports HSDPA
			
			Modes: WCDMA */
			KCapsHSDPASupported=0x00080000,
			
			/** The phone supports EGPRS
			
			Modes: GPRS/WCDMA */
			KCapsEGPRSSupported=0x00100000,
			
			/** The phone supports HSUPA
			
			Modes: WCDMA */
			KCapsHSUPASupported=0x00200000,
			/** The phone supports MBMS
			
			Modes: GPRS/WCDMA */
			KCapsMBMSSupported=0x00400000
			};

	IMPORT_C TInt GetStaticCaps(TUint& aCaps, RPacketContext::TProtocolType aPdpType) const;

	/** Dynamic capabilities.

	Modes: GPRS/CDMA */
	enum TDynamicCaps
		{
		// Common GPRS and CDMA caps
		/** Activate supported.
		
		Modes: GPRS/CDMA */
		KCapsActivate=0x00000001,
		/** Rx Circuit Switched call supported.
		
		Modes: GPRS/CDMA */
		KCapsRxCSCall=0x00000002,
		/** RX context activation request supported.
		
		Modes: GPRS/CDMA */
		KCapsRxContextActivationReq=0x00000004,
		// GPRS-specific caps
		/** Manual attach supported.
		
		Modes: GPRS */
		KCapsManualAttach=0x00000008,
		/** Manual detach supported.
		
		Modes: GPRS */
		KCapsManualDetach=0x00000010,
		/** SMS transfer supported.
		
		Modes: GPRS */
		KCapsSMSTransfer=0x00000020,
		// CDMA-specific caps
		/** Service Option (7,15) or (8,16).
		
		Modes: CDMA
		 @deprecated 9.5
		 */
		KCapsCDMAOneLowSpeedDataAllowed=0x00000040,
		/** Service Option (22-25) or (26-29).
		
		Modes: CDMA
		@deprecated 9.5
		 */
		KCapsCDMAOneHighSpeedDataAllowed=0x00000080,
		/** Service Option 33 or 34.
		
		Modes: CDMA 
		@deprecated 9.5
		*/
		KCapsCDMA2000HighSpeedDataAllowed=0x00000100,
		
		/** HSDPA is currently being used
		
		Modes: WCDMA */
		KCapsHSDPA=0x00000200,
		
		/** EGPRS is currently being used
		
		Modes: GPRS/WCDMA  */
		KCapsEGPRS=0x00000400,
		
		/** HSUPA is currently being used
		
		Modes: WCDMA */
		KCapsHSUPA=0x00000800,
		
		/** MBMS is currently being used
		
		Modes: GPRS/WCDMA */
		KCapsMBMS=0x00001000
		
		};
	
	/** Typedef to hold the RPacketService::TDynamicCaps. */
	typedef TUint TDynamicCapsFlags;

	IMPORT_C TInt GetDynamicCaps(TDynamicCapsFlags& aCaps) const;
	IMPORT_C void NotifyDynamicCapsChange(TRequestStatus& aStatus, TDynamicCapsFlags& aCaps) const;

	/** The preferred bearer. */
	enum TPreferredBearer
		{
		/** Packet Switched (PS) bearer preferred. */
		EBearerPacketSwitched,
		/** Circuit Switched (CS) bearer preferred. */
		EBearerCircuitSwitched
		};

	IMPORT_C void SetPreferredBearer(TRequestStatus& aStatus, TPreferredBearer aBearer) const;
	IMPORT_C TInt GetPreferredBearer(TPreferredBearer& aBearer) const;// Deprecated method.
	
	IMPORT_C void GetPreferredBearer(TRequestStatus& aStatus, TPreferredBearer& aBearer) const; // Recommended method variant

	/** Defines when the attachment to the GPRS network should occur.

	Modes: GPRS */
	enum TAttachMode
		{
		/** The phone will attach to the packet network whenever it can. */
		EAttachWhenPossible,// instruct phone stack to attach to packet network whenever it can
		/** The phone will attach to the packet network only when required (e.g. when activating 
		a connection). */
		EAttachWhenNeeded	// instruct phone stack to attach to network only when required
		};

	IMPORT_C TInt SetAttachMode(TAttachMode aMode) const;  // Deprecated method.
	IMPORT_C TInt GetAttachMode(TAttachMode& aMode) const; // Deprecated method.
	IMPORT_C TInt SetDefaultContextParams(const TDesC8& aPckg) const; // Deprecated method.
	IMPORT_C TInt GetDefaultContextParams(TDes8& aPckg) const; // Deprecated method.

	IMPORT_C void SetAttachMode(TRequestStatus& aStatus, TAttachMode aMode) const; // Recommended method variant
	IMPORT_C void GetAttachMode(TRequestStatus& aStatus, TAttachMode& aMode) const; // Recommended method variant
	IMPORT_C void NotifyAttachModeChange(TRequestStatus& aStatus, TAttachMode& aMode) const;
	IMPORT_C void SetDefaultContextParams(TRequestStatus& aStatus, const TDesC8& aPckg) const; // Recommended method variant
	IMPORT_C void GetDefaultContextParams(TRequestStatus& aStatus, TDes8& aPckg) const; // Recommended method variant

	enum TPacketReleaseMode
		{
		EReleaseModeUnknown,
		EReleaseModeUnregistered,
		EReleaseMode97_98,
		EReleaseMode99,
		EReleaseMode4,
		/*@deprecated 9.5*/
		EReleaseModeCDMA2000
		};

	IMPORT_C void GetCurrentReleaseMode(TRequestStatus& aStatus, TPacketReleaseMode& aReleaseMode) const;
	IMPORT_C void NotifyReleaseModeChange(TRequestStatus& aStatus, TPacketReleaseMode& aReleaseMode) const;

	enum TContextType
		{
		EUnspecified,		//< Unspecified context type
		EInternalContext,	//< Internally created context
		EExternalContext	//< Externally created context
		};

	
	class TNifInfoV2 : public TPacketBase
	/**
	Rel99/R4 Network information class. This V2 class inherits from the 
	TPacketBase class to make it future proof (extensible).
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TNifInfoV2();							//< Constructor
	public:
		TName iContextName;								//< The name of any defined context for this Network Interface
		TInt iNumberOfContexts;							//< Number of contexts belonging to the Network Interface
		RPacketContext::TContextStatus iNifStatus;		//< Status of the Network Interface
		RPacketContext::TProtocolAddress iPdpAddress;	//< PDP address of the interface
		TContextType iContextType;						//< Context type (internally or externally created)
		};
	typedef TPckg<TNifInfoV2> TNifInfoV2Pckg;			//< TNifInfoV2 package

	// Internal API/TSY struct, used by GetContextNameInNif() method
	struct TContextNameInNif
		{
		TInt iIndex;				//< Integer specifying a context within a Nif.
		TName iExistingContextName;	//< Existing Context name.
		};

	IMPORT_C void EnumerateNifs(TRequestStatus& aStatus, TInt& aCount) const;
	IMPORT_C void GetNifInfo(TRequestStatus& aStatus, TInt aIndex, TDes8& aNifInfoV2) const;
	IMPORT_C void EnumerateContextsInNif(TRequestStatus& aStatus, const TDesC& aExistingContextName, TInt& aCount) const;
	IMPORT_C void GetContextNameInNif(TRequestStatus& aStatus,  const TDesC& aExistingContextName, TInt aIndex, TDes& aContextName) const;
	IMPORT_C void DeactivateNIF(TRequestStatus& aStatus, const TDesC& aContextName) const;

//
//
// MBMS Service Availability
//
//
	
	class TMbmsServiceAvailabilityV1 : public TPacketType
	/**
	Holds the service availability parameters needed to register with TSY
	for the services to be monitored.
	
	@publishedPartner
	@released
	*/
		{
	public:	
		IMPORT_C TMbmsServiceAvailabilityV1();
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		/** Input Parameter : Temporary Mobile Group Identity */
		TTmgi	iTmgi;
		
		/** Input Parameter : MBMS service mode */
		TMbmsServiceMode iMbmsServiceMode;
		
		/** Output Parameter : MBMS availability status */
		TMbmsAvailabilityStatus iMbmsAvailabilityStatus;
		
		/** Input Parameter : MBMS access bearer information */
		TMbmsScope iMbmsAccessBearer;
		};
	
	//Packaged version of TMbmsServiceAvailabilityV1, used when this structure has to be passed via IPC.
	typedef TPckg<TMbmsServiceAvailabilityV1> TMbmsServiceAvailabilityV1Pckg;
		
	IMPORT_C void NotifyMbmsServiceAvailabilityChange(TRequestStatus& aReqStatus) const;
	IMPORT_C void UpdateMbmsMonitorServiceListL (TRequestStatus& aReqStatus, const TMbmsAction aAction, CPcktMbmsMonitoredServiceList* aList = NULL) const;		
	IMPORT_C void GetMbmsNetworkServiceStatus(TRequestStatus& aStatus, TBool aAttemptAttach, TMbmsNetworkServiceStatus& aServiceStatus) const;
	IMPORT_C void NotifyMbmsNetworkServiceStatusChange(TRequestStatus& aStatus, TMbmsNetworkServiceStatus& aServiceStatus) const;
	IMPORT_C void EnumerateMbmsMonitorServiceList(TRequestStatus& aStatus, TInt& aCount, TInt& aMaxAllowed) const;
	IMPORT_C void EnumerateMbmsActiveServiceList(TRequestStatus& aStatus, TInt& aCount, TInt& aMaxAllowed) const;
protected:
	IMPORT_C virtual void ConstructL();
	IMPORT_C virtual void Destruct();
private:
	TInt PrepareOpenSecondary(const TDesC& aOriginalContextName); //< Internal method, not for Client use
	CPacketPtrHolder* iEtelPacketPtrHolder;
	RMutex iMutex;   //< Internal mutex, used to prevent concurent use of PrepareOpenSecondary() and OpenNewContext()
	};
	
#include <etelpckt.inl>
#endif
