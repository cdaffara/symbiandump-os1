// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// BT socket interface types
// 
//

/**
 @file
 @publishedAll
 @released
*/

#ifndef BT_SOCK_H
#define BT_SOCK_H

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <bthci.h>
#include <bluetooth/lmoptions.h>
#endif

#include <bluetooth/btscoparams.h>
#include <bluetooth/btbaseband.h>
#include <btsocketoptionlevels.h>
#include <btdevice.h>

#include <es_sock.h>
#include <bttypes.h>
#include <d32public.h> // for RFCOMM API
#include <bluetooth/hci/hcierrors.h>
#include <btmanclient.h>
#include <bt_subscribe.h>
#include <btsockaddr.h>
#include <btsecurity.h>
#include <btbasebandpolicy.h>
#include <btsyncbandwidth.h>



_LIT(KRFCOMMDesC,"RFCOMM");	/*!< Descriptor name for RFCOMM */

_LIT(KL2CAPDesC,"L2CAP"); 	/*!< Descriptor name for L2CAP */

//
// BT Protocol Family
//

const TUint KBTAddrFamily = 0x101; 		/*!< BT Address Family */

const TUint KBTLinkManager = 0x0099; 	/*!< Protocol Number for Link Manager */
const TUint KL2CAP = 0x0100;			/*!< Protocol Number for L2CAP */
const TUint KRFCOMM = 0x0003;			/*!< Protocol Number for RFCOMM */
const TUint KSDP = 0x0001;				/*!< Protocol Number for SDP */
const TUint	KAVCTP = 0x0017;			/*!< Protocol Number for AVCTP */
const TInt KTCIL2CAP = 0xF100;			/*!< Protocol Number for TCI L2CAP */

const TInt KBTMajor = 0;				/*!< BT version number for major version */
const TInt KBTMinor = 1;				/*!< BT version number for minor version */
const TInt KBTBuild = 1;				/*!< BT version number for build version */

// The Socket Options are now in btsocketoptionlevels.h

const static TUint8 KSCOListenQueSize = 1; /*!< Length of SCO listening queue */

const static TUint16 KL2MinMTU = 48; 			/*!< BT Spec defined min. supported MTU size */


// All PSM values must be ODD, that is, the least significant bit of the least significant
// octet must be '1'. Also, all PSM values must be assigned such that the
// least significant bit of the most significant octet equals '0'.
const static TUint16 KMaxPSM = 0xFEFF;	/*!< Max L2CAP PSM value */

/**
 * This is the minimum user PSM. Smaller PSM values are possible but are reserved for use by the 
 * Bluetooth stack only.
 */
const static TUint16 KMinUserPSM = 0x1001;

/**
 * This constant has been deprecated since its name is misleading becasue it doesn't represent the 
 * absolute minimum PSM but the minimum user PSM instead. 
 * Use KMinUserPSM instead of this constant.
 * @deprecated
 */
const static TUint16 KMinPSM = 0x1001;

const static TInt KErrBtEskError = -6999;	/*!< BT ESK error code */


// Options available for all Bluetooth protocols

/** BT options. */
enum TBTOptions
	{
	KBTSetNoSecurityRequired = 0x997,	/*!< Set No security required for Security Mode 4*/
    KBTRegisterCodService = 0x998,		/*!< Set a CoD Service bit(s) */
	KBTSecurityDeviceOverride = 0x999,	/*!< Override device security */
	};

typedef TPckgBuf<TBTServiceSecurityPerDevice> TBTServiceSecurityPerDeviceBuf;	/*!< Package definition for securty settings */


// Link manager error codes.
const static TInt KLinkManagerErrBase = -6450;								/*!< Link manager base error value */
const static TInt KErrInsufficientBasebandResources = KLinkManagerErrBase;	/*!< Insufficient baseband resources error value */
const static TInt KErrProxyWriteNotAvailable = KLinkManagerErrBase-1;		/*!< Proxy write not available error value */
const static TInt KErrReflexiveBluetoothLink = KLinkManagerErrBase-2;		/*!< Reflexive BT link error value */
const static TInt KErrPendingPhysicalLink = KLinkManagerErrBase-3;			/*!< Physical link connection already pending when trying to connect the physical link */
const static TInt KErrRemoteDeviceIndicatedNoBonding = KLinkManagerErrBase-4; /*!< Dedicated bonding attempt failure when the remote device responds with No-Bonding */

//
// L2CAP
//

const static TInt KL2CAPErrBase = -6300;								/*!< Base error value for L2CAP error codes */
const static TInt KErrBadAddress = KL2CAPErrBase;						/*!< L2CAP Bad address error code */
const static TInt KErrSAPUnexpectedEvent = KL2CAPErrBase - 1;			/*!< L2CAP unexpected SAP event error code */
const static TInt KErrBadPacketReceived = KL2CAPErrBase - 2;			/*!< L2CAP bad packet received error code */
const static TInt KErrL2CAPBadResponse = KL2CAPErrBase - 3;				/*!< L2CAP bad response error code */
const static TInt KErrHCIConnectFailed = KL2CAPErrBase - 4;				/*!< L2CAP HCI connection failed error code */
const static TInt KErrHCILinkDisconnection = KL2CAPErrBase - 5;			/*!< L2CAP HCI link disconnection error code */
const static TInt KErrSAPNotConnected = KL2CAPErrBase - 6;				/*!< L2CAP SAP not connected error code */
const static TInt KErrConfigBadParams = KL2CAPErrBase - 7;				/*!< L2CAP bad configuration parameters error code */
const static TInt KErrConfigRejected = KL2CAPErrBase - 8;				/*!< L2CAP configuration rejected error code */
const static TInt KErrConfigUnknownOptions = KL2CAPErrBase - 9;			/*!< L2CAP unknown configuration options error code */
const static TInt KErrL2PeerDisconnected = KL2CAPErrBase - 10;			/*!< L2CAP peer disconnected error code */
const static TInt KErrL2CAPAccessRequestDenied = KL2CAPErrBase - 11;	/*!< L2CAP access request denied error code */
const static TInt KErrL2CAPRequestTimeout = KL2CAPErrBase - 12;			/*!< L2CAP request timeout error code */
const static TInt KErrL2PeerRejectedCommand = KL2CAPErrBase - 13;		/*!< L2CAP peer rejected command error code */
const static TInt KErrHostResNameTooLong = KL2CAPErrBase - 14;			/*!< L2CAP host resolver name too long error code */
const static TInt KErrL2CAPNoMorePSMs = KL2CAPErrBase - 15;				/*!< L2CAP no more PSMs error code */
const static TInt KErrL2CAPMaxTransmitExceeded = KL2CAPErrBase - 16;	/*!< L2CAP in reliable mode: the maximum L2Cap retransmissions have been made and channel will disconnect error code*/
const static TInt KErrL2CAPDataControllerDetached = KL2CAPErrBase - 17;	/*!< L2CAP problems (e.g. no memory) whilst sending data error code*/
const static TInt KErrL2CAPConfigPending = KL2CAPErrBase - 18;			/*!< L2CAP configuration is in progress error code 
																			 @internalComponent*/
const static TInt KErrL2CAPConfigAlreadyInProgress = KL2CAPErrBase - 19;/*!< L2CAP attempt to alter config whilst configuration is in progress error code*/
const static TInt KErrL2CAPNoFreeCID = KL2CAPErrBase - 21;				/*!< L2CAP no more channel IDs available error code*/
const static TInt KErrL2CAPPeerDoesNotSupportRequestedChannelMode = KL2CAPErrBase - 22; /*!< L2CAP peer doesn't support channel modes satisfying requested channel configuration. */
const static TInt KErrL2CAPInvalidPacketSequenceNumber = KL2CAPErrBase - 23;	/*!< L2CAP packet with an invalid sequence number was received */
const static TInt KErrL2CAPInvalidAcknowledgementNumber = KL2CAPErrBase - 24;	/*!< L2CAP invalid acknowledgment number was received */
const static TInt KErrL2CAPIllegalRemoteBehavior = KL2CAPErrBase - 25;			/*!< L2CAP remote broke the L2CAP specification */
const static TInt KErrL2CAPNegotiatedDifferentModesForEachDirection = KL2CAPErrBase - 26; /*!< L2CAP different channel modes were negotiated for incoming and outgoing direction */
const static TInt KErrL2CAPIncomingSduSegmentationError = KL2CAPErrBase - 27;	/*!< L2CAP received I-Frame does not match the sequence of start, continuation, end of SDU */
const static TInt KErrL2CAPIncomingSduTooBig = KL2CAPErrBase - 28;		/*!< L2CAP received SDU size is larger than the negotiated MTU */
const static TInt KErrL2CAPIncomingIFrameTooSmall = KL2CAPErrBase - 29;	/*!< L2CAP Incoming I-Frame size is too small */
const static TInt KErrL2CAPIncomingIFrameTooBig = KL2CAPErrBase - 30;	/*!< L2CAP Incoming I-Frame information payload is larger than the negotiated MPS */
const static TInt KErrL2CAPInvalidIncomingSFrameSize = KL2CAPErrBase - 31; /*!< L2CAP Incoming S-Frame size is incorrect */
const static TInt KErrL2CAPAttemptToSetMinMtuGreaterThanMtu = KL2CAPErrBase - 32; /*!< L2CAP An attempt to configure minimal acceptable MTU that's larger than the preferred MTU was made */
const static TInt KErrL2CAPAttemptToSetMinMruGreaterThanMru = KL2CAPErrBase - 33; /*!< L2CAP An attempt to configure minimal acceptable MRU that's larger than the preferred MRU was made */

// Host Resolver
const static TInt KErrHostResNoMoreResults = KErrEof;  					/*!< Host resolver has no more results error code */
const static TUint KHostResInquiry = 1;			/*!< Host resolver inquiry option */ 
const static TUint KHostResName = 2;			/*!< Host resolver name option. This is mutually exclusive with KHostResEir */ 
const static TUint KHostResIgnoreCache = 16; 	/*!< A way of ignoring the cache */ 
const static TUint KHostResCache = 32; 			/*!< A way to get CoD from cache */
const static TUint KHostResEir = 64; 			/*!< Host resolver Eir option. This is mutually exclusive with KHostResName  */

// L2CAP Ioctls
const static TInt KL2CAPEchoRequestIoctl			= 0;	/*!< Echo Request Ioctl name */
const static TInt KL2CAPIncomingMTUIoctl			= 1;	/*!< Change incoming MTU Ioctl name */
const static TInt KL2CAPOutgoingMTUIoctl			= 2;	/*!< Change outgoing MTU Ioctl name */
const static TInt KL2CAPUpdateChannelConfigIoctl	= 3;	/*!< Change configuration parameters Ioctl name */
const static TInt KL2CAPPretendIncomingSduQFull		= 4;	/*!< Pretend incoming SDU Queue is full - internal testing only @internalComponent */

struct TLMDisconnectACLIoctl
/**
Structure to specify the remote ACL connection to disconnect.
The reason passed in iReason will be sent to the remote device.

Use with KLMDisconnectACLIoctl.
@deprecated
*/
	{
	TBTDevAddr	iDevAddr;	/*!< Device address */
	TUint8		iReason;	/*!< Reason code */
	};

typedef TPckgBuf<TLMDisconnectACLIoctl> TLMDisconnectACLBuf;	/*!< Package for TLMDisconnectACLIoctl structure */

// private tokens for use by RBTBaseband facade and stack
_LIT8(KDisconnectOnePhysicalLink, "1");		/*!< Specifes one physical link should be disconnected */
_LIT8(KDisconnectAllPhysicalLinks, "A");	/*!< Specifes all physical links should be disconnected */
_LIT8(KDisconnectAllPhysicalLinksForPowerOff, "P");	/*!< Specifes all physical links should be disconnected for power off*/

// HCI Ioctls
/** Add SCO connnection Ioctl
@deprecated
*/
static const TUint KLMAddSCOConnIoctl			=0;
/** Remove SCO connection Ioctl
@deprecated
*/
static const TUint KHCIRemoveSCOConnIoctl		=1;
/** Change packet types allowed Ioctl
@deprecated
*/
static const TUint KHCIChangePacketTypeIoctl	=2;
/** Request authorisation Ioctl 
@deprecated
*/
static const TUint KHCIAuthRequestIoctl			=3;
/** Request encryption Ioctl
@deprecated
*/
static const TUint KHCIEncryptIoctl				=4;
/** Change link key Ioctl
@deprecated
*/
static const TUint KHCIChangeLinkKeyIoctl		=5;
/** Master link key Ioctl
@deprecated
*/
static const TUint KHCIMasterLinkKeyIoctl		=6;
/** Enable hold mode Ioctl
@deprecated
*/
static const TUint KHCIHoldModeIoctl			=7;
/** Enable sniff mode Ioctl
@deprecated
*/
static const TUint KHCISniffModeIoctl			=8;
/** Exit sniff mode Ioctl
@deprecated
*/
static const TUint KHCIExitSniffModeIoctl		=9;
/** Enable park mode Ioctl
@deprecated
*/
static const TUint KHCIParkModeIoctl			=10;
/** Exit park mode Ioctl
@deprecated
*/
static const TUint KHCIExitParkModeIoctl		=11;

/** Read page timeout Ioctl
@deprecated
*/
static const TUint KHCIReadPageTimeoutIoctl		=12;
/** Write page timeout Ioctl
@deprecated
*/
static const TUint KHCIWritePageTimeoutIoctl	=13;
/** Read scan enable Ioctl
@deprecated
@see bt_subscribe.h
*/
static const TUint KHCIReadScanEnableIoctl		=14;
/** Write scan enable Ioctl
@deprecated
@see bt_subscribe.h
*/
static const TUint KHCIWriteScanEnableIoctl		=15;
/** Read device class Ioctl
@deprecated
@see bt_subscribe.h
*/
static const TUint KHCIReadDeviceClassIoctl		=16;
/** Write device class Ioctl
@deprecated
@see bt_subscribe.h
*/
static const TUint KHCIWriteDeviceClassIoctl	=17;
/** Read voice settings Ioctl
@deprecated
*/
static const TUint KHCIReadVoiceSettingIoctl	=18;
/** Write voice settings Ioctl
@deprecated
*/
static const TUint KHCIWriteVoiceSettingIoctl	=19;
/** Read hold mode activity Ioctl
@deprecated
*/
static const TUint KHCIReadHoldModeActivityIoctl=20;
/** Write hold mode activity Ioctl
@deprecated
*/
static const TUint KHCIWriteHoldModeActivityIoctl=21;
/** Local version Ioctl
@deprecated
*/
static const TUint KHCILocalVersionIoctl		=22;
/** Local features Ioctl
@deprecated
*/
static const TUint KHCILocalFeaturesIoctl		=23;
/** Country code Ioctl
@deprecated
*/
static const TUint KHCICountryCodeIoctl			=24;
/** Local address Ioctl
@deprecated
@see bt_subscribe.h
*/
static const TUint KHCILocalAddressIoctl		=25;
/** Write discoverability Ioctl
@deprecated
@see bt_subscribe.h
*/
static const TUint KHCIWriteDiscoverabilityIoctl=26;
/** Read discoverability Ioctl
@deprecated
@see bt_subscribe.h
*/
static const TUint KHCIReadDiscoverabilityIoctl	=27;
/** Read authentification enabled Ioctl
@deprecated
*/
static const TUint KHCIReadAuthenticationEnableIoctl=33;
/** Write authentification enabled Ioctl
@deprecated
*/
static const TUint KHCIWriteAuthenticationEnableIoctl=34;


// Structs for ioctl parameters

/**
Enumerations for the four possible scan enable modes.
Use Inquiry scan for discoverability and Page scan for
connectability.

Use with KHCIReadScanEnableIoctl and KHCIWriteScanEnableIoctl.
@deprecated
@see bt_subscribe.h
*/
enum THCIScanEnableIoctl 
    {
     EHCINoScansEnabled=0x00,          /*!< No scans enabled. */
     EHCIInquiryScanOnly,              /*!< Inquiry scan only. */
     EHCIPageScanOnly,                 /*!< Page scan only. */
     EHCIInquiryAndPageScan            /*!< Both inquiry and page scan enabled. */
    };
/**
Package for THCIScanEnableIoctl structure 
@deprecated
*/
typedef TPckgBuf<THCIScanEnableIoctl> THCIScanEnableBuf;	

struct TLMAddSCOConnectionIoctl
/**
Structure for specifying SCO type to add to a connected socket.
The connection handle for the SCO link is returned in iConnH when
the Ioctl completes.

Use with KHCIAddSCOConnIoctl.
@deprecated
*/
	{
	TUint16        iPktType;	/*!< Packet type */
	};
/**
Package for TLMAddSCOConnectionIoctl structure
@deprecated
*/
typedef TPckgBuf<TLMAddSCOConnectionIoctl> TLMAddSCOConnectionBuf;	

struct THCISetEncryptionIoctl
/**
Request to change the encryption state of a connection.
iEncrypt specifies whether to turn encryption on or off.

Use with KHCIEncryptIoctl.
@deprecated
*/
	{
	TBool             iEncrypt;		/*!< Encryption enabled / disabled */
	};
/**
Package for THCISetEncryptionIoctl structure 
@deprecated
*/
typedef TPckgBuf<THCISetEncryptionIoctl> THCISetEncryptionBuf;

struct THCIDeviceClassIoctl
/**
Structure to specify the class of device when getting or setting
the local device class.

Use with KHCIReadDeviceClassIoctl and KHCIWriteDeviceClassIoctl.
@deprecated
@see bt_subscribe.h
*/
	{
	TUint16    iMajorServiceClass;		/*!< Major Service class */
	TUint8     iMajorDeviceClass;		/*!< Major Device class */
	TUint8     iMinorDeviceClass;		/*!< Minor Device class */
	};
/**
Package for THCIDeviceClassIoctl structure 
@deprecated
*/
typedef TPckgBuf<THCIDeviceClassIoctl> THCIDeviceClassBuf;
	
struct THCILocalVersionIoctl
/**
Structure describing the local Bluetooth hardware version.

Use with KHCILocalVersionIoctl.
@deprecated
*/
	{
	TUint8   iHCIVersion;			/*!< HCI version */
	TUint16  iHCIRevision;			/*!< HCI Revision */
	TUint8   iLMPVersion;			/*!< LMP version */
	TUint16  iLMPSubversion;		/*!< LMP subversion */
	TUint16  iManufacturerName;		/*!< Manufacturer name */
	};
/**
Package for THCILocalVersionIoctl structure 
@deprecated
*/
typedef TPckgBuf<THCILocalVersionIoctl> THCILocalVersionBuf;	

/** L2CAP Get / Set Options. */
enum TBTL2CAPOptions
	{
	/** 
	Get the outbound MTU size taking into account both the negotiated MTU size 
    and best use of underlying packet sizes.
	For example: If the default outgoing MTU of 672 has been negotiated for a L2CAP channel
	that is in Retransmission or Flow control mode then this GetOpt will return an
	outgoing MTU of 668 bytes. This allows a maximally sized SDU 
	(which due to this adjustment will be 668 bytes) to be sent in two DH5 packets.
	KL2CAPOutboundMTUForBestPerformance may also be used to set the current negotiated 
	(or to be negotiated) outbound MTU size.
	Note that the outbound MTU size returned when using KL2CAPOutboundMTUForBestPerformance 
	with a GetOpt may not be the same as the outbound MTU size previously set 
	when using KL2CAPOutboundMTUForBestPerformance with a SetOpt.
	*/ 
	KL2CAPOutboundMTUForBestPerformance,	

	/** 
	This is the legacy value for setting getting \ setting the outbound MTU size and behaves
	in the same way as KL2CAPOutboundMTUForBestPerformance.
	@see KL2CAPOutboundMTUForBestPerformance
	*/ 
	KL2CAPGetOutboundMTU = KL2CAPOutboundMTUForBestPerformance,

	KL2CAPGetDebug1,				/*!< Debug Command */
	KL2CAPInboundMTU,				/*!< Get / Set the current inbound MTU size */
	KL2CAPRTXTimer,					/*!< Change the extended L2CAP command retransmission timeout */
	KL2CAPERTXTimer,				/*!< Change the L2CAP command retransmission timeout */
	KL2CAPGetMaxOutboundMTU,		/*!< Get the max outbound MTU supported by the stack */
	KL2CAPGetMaxInboundMTU,			/*!< Get the max inbound MTU supported by the stack */
	KL2CAPUpdateChannelConfig,		/*!< Get and Set the current configuration parameters */

	/** Get / Set the current negotiated (or to be negotiated) outbound MTU size */
	KL2CAPNegotiatedOutboundMTU,    

	KL2CAPTestConfigure,	
	KL2CAPDebugFlush,				// not implemented anymore, just a placeholder for BC
	KL2CAPVersion1_2,
	KL2CAPHeapAlloc,
	KL2CAPDataPlaneConfig,
	
	/**
	This is similar to KL2CAPOutboundMTUForBestPerformance except that the caller can specify
	a restriction that the returned MTU shall not exceed. This is useful for layers above L2CAP
	that still want an optimal MTU but have their own restrictions on packet size.
	*/
	KL2CAPOutboundMTUForBestPerformanceWithRestriction,
	KL2CAPLocalPSM,					/*!< Return local PSM of the socket (only works before the socket is accepted, stack internal use only) @internalTechnology */
	KL2CAPNegotiatedChannelMode		/*!< Get the negotiated L2CAP channel mode */
	};

typedef TUint16 TL2CAPPort;			/*!< Definition of a L2CAP port number type */

const static TL2CAPPort KL2CAPPassiveAutoBind = KMaxTUint16;  /*!< PSM out of the valid range used for passive auto bind operation */

NONSHARABLE_CLASS(TL2CAPSockAddr) : public TBTSockAddr
/** L2CAP Socket Address.

Use this class to specify a local or remote L2CAP connection end-point,
that is Device Address and PSM/CID.
When unconnected, the Port() specifies the PSM, once connected it refers to
the CID.
@see TBTSockAddr
@publishedAll
@released
*/
	{
public:
	IMPORT_C TL2CAPSockAddr();
	IMPORT_C TL2CAPSockAddr(const TSockAddr& aSockAddr);
	IMPORT_C static TL2CAPSockAddr& Cast(const TSockAddr& aSockAddr);

	IMPORT_C TL2CAPPort Port() const;
	IMPORT_C void SetPort(const TL2CAPPort aHomePort);
	};

NONSHARABLE_CLASS(TInquirySockAddr) : public TSockAddr
/** Socket address class used for inquiries.

Used to specify the inquiry request, and then filled with information
about remote devices discovered through the inquiry process.

Use the BTAddr() method to extract the device address to connect to.


Note: Usage of RHostResolver class for Bluetooth protocol.

The RHostResolver class is a generic interface to host name 
resolution services, such as DNS, that may be provided 
by particular protocol modules.

The points to remember when using RHostResolver::GetByAddress(), 
RHostResolver::GetByName(), or RHostResolver::Next() 
with Bluetooth protocol are:

1)  If you operate on one instance of RHostResolver you can perform 
	only one request by calling either RHostResolver::GetByAddress() 
	or  RHostResolver::GetByName(). If these functions are called again 
	and if there is more than one possible response	for a given host name
	then that will be returned (The host resolving process will
	not start from the beginning). It is exactly as if the RHostResolve::Next() 
	method was called.

2)  In order to start resolving new hosts from the beginning using the same 
	instance of RHostResolver, the instance must be closed and reopened again.
	
3)  In order to perform several RHostResolver requests they	should be issued on 
	separate instances of RHostResolver (many RHostResolver instances might 
	exist and perform requests at the same time).
	
4)  The KHostResIgnoreCache flag is only valid when issuing RHostResolver::GetByAddress() 
	or RHostResolver::GetByName() request for the first time.

5)  As an RHostResolver is only intended to be used once, it is recommended that it 
	be closed as soon as it is finished with as the semantics of Cancel merely
	indicates that one client server request should be cancelled.
	
@publishedAll
@released
*/
	{
// Max size for this structure is 24 bytes (32 - 8, see TSockAddr). However, due to padding and to prevent BC breaks, 
// only up to 3 TUint8(s) may be added after iFlags
	struct SInquiryAddr
		{
		TBTDevAddr iAddress;
		TUint8 iFormatTypeField; // since 'Format Type' only occupies 2 bits (least significant), we use 4 bits (most significant) for version information (and leave 2 bits unused)
		TUint16 iMajorServiceClass;
		TUint8 iMajorDeviceClass;
		TUint8 iMinorDeviceClass;
		TUint iIAC;
		TUint8 iActionFlags;
		TUint8 iResultFlags;
		TInt8 iRssi;
		};
	
public:
	// This defines all the bit masks for the inquiry result
	enum TResolverResultFlags
		{
		ERssiValid = 0x1,
		};

public:
	IMPORT_C TInquirySockAddr();
	IMPORT_C TInquirySockAddr(const TSockAddr& aSockAddr);
	IMPORT_C TBTDevAddr BTAddr() const;
	IMPORT_C void SetBTAddr(const TBTDevAddr& aRemote);

	IMPORT_C static TInquirySockAddr& Cast(const TSockAddr& aSockAddr);
	IMPORT_C TUint16 MajorServiceClass() const;
	IMPORT_C void SetMajorServiceClass(TUint16 aClass);
	IMPORT_C TUint8 MajorClassOfDevice() const;
	IMPORT_C void SetMajorClassOfDevice(TUint8 aMajorClassOfDevice);
	IMPORT_C TUint8 MinorClassOfDevice() const;
	IMPORT_C void SetMinorClassOfDevice(TUint8 aMinorClassOfDevice);
	IMPORT_C TUint IAC() const;
	IMPORT_C void SetIAC(const TUint aIAC);
	IMPORT_C TUint8 Action() const;
	IMPORT_C void SetAction(TUint8 aFlags);
	IMPORT_C TVersion Version() const;	
	IMPORT_C TInt8 Rssi() const;	
	IMPORT_C void SetRssi(const TInt8 aRssi);
	IMPORT_C TUint8 ResultFlags() const;
	IMPORT_C void SetResultFlags(TUint8 aResultFlags);

protected:
	void SetVersion(TVersion aVersion);
	IMPORT_C TUint8 FormatTypeField() const;
	void SetFormatTypeField(TUint8 aType);
	
private:
	SInquiryAddr& InquiryAddrStruct() const;
	TPtr8 AddressPtr() const;
	};

//
// RFCOMM
//

const static TInt KRFErrorBase = -6350;										/*!< RFCOMM base error code. */
const static TInt KErrRfcommSAPUnexpectedEvent = KRFErrorBase;				/*!< RFCOMM unexpected event error code. */
const static TInt KErrRfcommAlreadyBound = KRFErrorBase-1;					/*!< RFCOMM SAP already bound error code. */
const static TInt KErrRfcommBadAddress = KRFErrorBase-2;					/*!< RFCOMM bad address error code. */
const static TInt KErrRfcommMTUSize = KRFErrorBase-3;						/*!< RFCOMM MTU size exceeded error code. */
const static TInt KErrRfcommFrameResponseTimeout = KRFErrorBase-4;			/*!< RFCOMM frame response timeout error code. */
const static TInt KErrRfcommMuxRemoteDisconnect = KRFErrorBase-5;			/*!< RFCOMM remote end disconnected error code. */
const static TInt KErrRfcommNotBound = KRFErrorBase-6;						/*!< RFCOMM SAP not bound error code. */
const static TInt KErrRfcommParameterNegotiationFailure = KRFErrorBase-7;	/*!< RFCOMM parameter negotiation failure error code. */
const static TInt KErrRfcommNotListening = KRFErrorBase-8;					/*!< RFCOMM not listening error code. */
const static TInt KErrRfcommNoMoreServerChannels = KRFErrorBase-9;			/*!< RFCOMM no more server channels available error code. */
const static TInt KErrRfcommMuxChannelErrored = KRFErrorBase-10;			/*!< RFCOMM Mux channel errored */

//RFCOMMIoctls

const static TInt KRFCOMMModemStatusCmdIoctl =0;		/*!< RFCOMM status command Ioctl */
const static TInt KRFCOMMRemoteLineStatusCmdIoctl = 1;	/*!< RFCOMM remote line status command Ioctl */
const static TInt KRFCOMMRemotePortNegCmdIoctl = 2;		/*!< RFCOMM remote port negotiation command Ioctl */
const static TInt KRFCOMMRemotePortNegRequestIoctl = 3;	/*!< RFCOMM remote port negotiation request Ioctl */
const static TInt KRFCOMMConfigChangeIndicationIoctl = 4;	/*!< RFCOMM MSC activity Ioctl */

// RFCOMM Options

const static TInt KRFCOMMLocalPortParameter = 0;	/*!< RFCOMM local port parameter option (Get + Set) */
/** RFCOMM Get Available server channel option (Get only)
@deprecated
*/
const static TInt KRFCOMMGetAvailableServerChannel = 1;

const static TInt KRFCOMMMaximumSupportedMTU = 2; 	/*!< RFCOMM maximum supported option (Get + Set) */
const static TInt KRFCOMMGetDebug1 = 3;   			/*!< RFCOMM debug option (Get only) */
const static TInt KRFCOMMGetRemoteModemStatus = 4; 	/*!< RFCOMM remote modem status option (Get + Set) */

const static TInt KRFCOMMGetTransmitCredit = 5;		/*!< RFCOMM get transmit credits option */
const static TInt KRFCOMMGetReceiveCredit = 6;		/*!< RFCOMM get receive credits option */
const static TInt KRFCOMMGetReUsedCount = 7;		/*!< RFCOMM get number of remote used credits option */
const static TInt KRFCOMMFlowTypeCBFC = 8; 			/*!< RFCOMM Credit based flow control option (Get + Set) */
const static TInt KRFCOMMErrOnMSC = 9;				/*!< RFCOMM set the value of MSC signals that will cause a disconnect error to be generated */
const static TUint KRFCOMMLocalModemStatus = 10;   	/*!< RFCOMM local modem status option (Get + Set) */
const static TUint KRFCOMMForgiveCBFCOverflow = 11;   	/*!< RFCOMM only when credit-based flow control is used. When unset (default), the remote overflowing us will cause us to disconnect. When set, we keep the connection up and process as much of the data as we can (i.e. RFCOMM becomes unreliable). (Set only) */

const static TInt KRFCOMMMaximumMTU = 12; 	/*!< RFCOMM maximum optimal MTU (Get only) */

// Masks for interpreting signalling commands
const static TUint8 KModemSignalFC  = 0x01;			/*!< RFCOMM FC signalling command mask */
const static TUint8 KModemSignalRTC = 0x02;			/*!< RFCOMM RTC signalling command mask */
const static TUint8 KModemSignalRTR = 0x04;			/*!< RFCOMM RTR signalling command mask */
const static TUint8 KModemSignalIC  = 0x20;			/*!< RFCOMM IC signalling command mask */
const static TUint8 KModemSignalDV  = 0x40;			/*!< RFCOMM DV signalling command mask */

enum TRPNParameterMask
/** Remote port negotiation parameter masks
*/
	{
	EPMBitRate		= 0x0001,		/*!< Remote port negotiation parameter masks for bit rate */
	EPMDataBits		= 0x0002,		/*!< Remote port negotiation parameter masks for data bits */
	EPMStopBit		= 0x0004,		/*!< Remote port negotiation parameter masks for stop bit */
	EPMParity		= 0x0008,		/*!< Remote port negotiation parameter masks for parity */
	EPMParityType	= 0x0010,		/*!< Remote port negotiation parameter masks for parity type */
	EPMXOnChar		= 0x0020,		/*!< Remote port negotiation parameter masks for on character */
	EPMXOffChar		= 0x0040,		/*!< Remote port negotiation parameter masks for off character */
	// RESERVED		= 0x0080		
	EPMXOnOffInput	= 0x0100,		/*!< Remote port negotiation parameter masks for XOn/Off input */
	EPMXOnOffOutput	= 0x0200,		/*!< Remote port negotiation parameter masks for XOn/Off output */
	EPMRTRInput		= 0x0400,		/*!< Remote port negotiation parameter masks for read to receive input */
	EPMRTROutput	= 0x0800,		/*!< Remote port negotiation parameter masks for read to receive output */
	EPMRTCInput		= 0x1000,		/*!< Remote port negotiation parameter masks for RTC input */
	EPMRTCOutput	= 0x2000		/*!< Remote port negotiation parameter masks for RTC output */
	// RESERVED		= 0x4000
	// RESERVED		= 0x8000
	};

enum TRPNFlowCtrlMask
/** Remote port negotiation flow control masks
*/
	{
	EFCXOnOffInput	=0x01,	/*!< Remote port negotiation flow control masks for XOn/Off input */
	EFCXOnOffOutput =0x02,	/*!< Remote port negotiation flow control masks for XOn/Off output */
	EFCRTRInput		=0x04,	/*!< Remote port negotiation flow control masks for ready to receive input */
	EFCRTROutput	=0x08,	/*!< Remote port negotiation flow control masks for ready to receive output */
	EFCRTCInput		=0x10,	/*!< Remote port negotiation flow control masks for RTC input */
	EFCRTCOutput	=0x20	/*!< Remote port negotiation flow control masks for RTC output */
	};

enum TRPNValidityMask
/** Remote port negotiation validity masks
*/
	{
	EVMBitRate	= 0x01,	/*!< Remote port negotiation validity masks for bit rate */
	EVMDataBits	= 0x02,	/*!< Remote port negotiation validity masks for data bits */
	EVMStopBit	= 0x04,	/*!< Remote port negotiation validity masks for stop bit */
	EVMParity	= 0x08,	/*!< Remote port negotiation validity masks for parity */
	EVMFlowCtrl	= 0x10,	/*!< Remote port negotiation validity masks for flow control */
	EVMXOnChar	= 0x20,	/*!< Remote port negotiation validity masks for XOn charater */
	EVMXOffChar	= 0x40	/*!< Remote port negotiation validity masks for XOff charater */
	};

// structs for RFCOMM Ioctls

class TRfcommRPNTransaction;

NONSHARABLE_CLASS(TRfcommRemotePortParams)
/** RF COMM remote port parameters. 

@publishedAll
@released
*/
	{
public:
	IMPORT_C TRfcommRemotePortParams();
	IMPORT_C TUint8	IsValid() const;
	IMPORT_C TBool  GetBitRate(TBps& aBitRate) const;
	IMPORT_C TInt   SetBitRate(TBps  aBitRate);
	IMPORT_C TBool  GetDataBits(TDataBits& aDataBits) const;
	IMPORT_C TInt   SetDataBits(TDataBits  aDataBits);
	IMPORT_C TBool  GetStopBit(TStopBits& aStopBit) const;
	IMPORT_C TInt   SetStopBit(TStopBits  aStopBit);
	IMPORT_C TBool  GetParity(TParity& aParity) const;
	IMPORT_C TInt   SetParity(TParity  aParity);
	IMPORT_C TBool  GetFlowCtrl(TUint8& aFlowCtrl) const;
	IMPORT_C TInt   SetFlowCtrl(TUint8  aFlowCtrl);
	IMPORT_C TBool  GetXOnChar(TUint8& aXOnChar) const;
	IMPORT_C TInt   SetXOnChar(TUint8  aXOnChar);
	IMPORT_C TBool  GetXOffChar(TUint8& aXOffChar) const;
	IMPORT_C TInt   SetXOffChar(TUint8  aXOffChar);
	IMPORT_C void   UpdateFlowCtrlBit(TUint8 aFlowCtrl, TRPNFlowCtrlMask aFCMask);
	IMPORT_C void   UpdateWholeFlowCtrl(TUint16 aParamMask, TUint8 aFlowCtrl);
	IMPORT_C void   UpdateFromRPNTransaction(const TRfcommRPNTransaction& 
											aRPNTransaction);	
private:	
	TBps iBitRate;
	TDataBits iDataBits;
	TStopBits iStopBit; //It's really only one bit - ignore what the type implies...
	TParity iParity;
	TUint8 iFlowCtrl;
	TUint8 iXOnChar;
	TUint8 iXOffChar;
	TUint8 iValidMask;
		
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 
	};

// structs for RFCOMM Ioctls

NONSHARABLE_CLASS(TRfcommRPNTransaction)
/** RF COMM IO control structs.

@publishedAll
@released
*/
	{
public: // Functions
	IMPORT_C TRfcommRPNTransaction();
public: // Variables
	TRfcommRemotePortParams iPortParams;	/*!< Remote port parameters */
	TUint16 iParamMask;						/*!< Parameter mask */
	};

// RFCOMM addresses

typedef TUint8 TRfcommChannel;	/*!< RFCOMM channel type definition */

const static TRfcommChannel KMinRfcommServerChannel = 1;	/*!< Minimum RFCOMM server channel value */
const static TRfcommChannel KMaxRfcommServerChannel = 30;	/*!< Maximum RFCOMM server channel value */

const static TRfcommChannel KRfcommPassiveAutoBind = KMaxTUint8;	/*!< Channel value out of the valid range used for passive auto bind. */

NONSHARABLE_CLASS(TRfcommSockAddr) : public TBTSockAddr
/** Defines the format of an Rfcomm address.

This class uses the TSockAddr data area to hold the address so that
it can be passed through the ESOCK boundary.
Assumes that the remote RFCOMM instance is always bound to PSM 3 on
L2CAP, so there is no way of specifying another remote PSM.

@see TBTSockAddr
@publishedAll   
@released
*/
	{
public:
	IMPORT_C TRfcommSockAddr();
	IMPORT_C TRfcommSockAddr(const TSockAddr& aSockAddr);
	IMPORT_C static TRfcommSockAddr& Cast(const TSockAddr& aSockAddr);
	};

/*****BASEBAND CLIENT*********/

class CBTBasebandSocketProxy;
class CBTBasebandPropertySubscriber;

class MBasebandObserver;
		
NONSHARABLE_CLASS(RBTPhysicalLinkAdapter)
/** Class to enable modification of a physical link:

Modifications may be requested or prevented (blocked).
Whilst a modification is being prevented, any request to
perform that modification by this or any other
RBTPhysicalLinkAdapter client will be ignored.
If a low power mode is being used on the physical link, a
call to prevent that low power mode will, if possible, cause
the physical link to exit that low power mode. An
arbitration between all RBTPhysicalLinkAdapter clients will then occur
to decide whether the physical link should remain active or 
enter another low power mode. (If all low power modes are prevented
then that arbitration will result in the physical link remaining
active.)

Methods to prevent modifications begin 'Prevent...'

Methods to cancel the prevention of modification begin 'Allow...'

Requests for low power mode modifications, and notification of modifications
take the form of continuously repeated requests which can be switched on or 
switched off. 

Only one low power mode requester may active on a single RBTPhysicalLinkAdapter
client at a time. If several RBTPhysicalLinkAdapter clients have differing low
power mode requests active at a given moment then the priority will be:
	Hold
	Sniff
	Park

Methods to perform these requests start 'Activate...'

Methods to cancel these requests start 'Cancel...'
@publishedAll
@released
*/
	{
public:
	IMPORT_C RBTPhysicalLinkAdapter();
	IMPORT_C TInt Open(RSocketServ& aSocketServ, RSocket& aSocket);
	IMPORT_C TInt Open(RSocketServ& aSocketServ, const TBTDevAddr& aDevAddr);
	IMPORT_C TBool IsOpen() const;		
	IMPORT_C void Close();
	IMPORT_C TInt PhysicalLinkState(TUint32& aState);
	IMPORT_C TInt PreventRoleSwitch();
	IMPORT_C TInt AllowRoleSwitch();
	IMPORT_C TInt RequestMasterRole();
	IMPORT_C TInt RequestSlaveRole();
	IMPORT_C TInt PreventLowPowerModes(TUint32 aLowPowerModes);
	IMPORT_C TInt AllowLowPowerModes(TUint32 aLowPowerModes);
	IMPORT_C TInt ActivateSniffRequester();
	IMPORT_C TInt ActivateParkRequester();
	IMPORT_C TInt ActivateActiveRequester();
	IMPORT_C TInt CancelLowPowerModeRequester();
	IMPORT_C TInt RequestChangeSupportedPacketTypes(TUint16 aPacketTypes);
	IMPORT_C void NotifyNextBasebandChangeEvent(TBTBasebandEvent& aEventNotification, 
		                                        TRequestStatus& aStatus, 
						     		            TUint32 aEventMask = ENotifyAnyPhysicalLinkState);

	IMPORT_C void CancelNextBasebandChangeEventNotifier();
	IMPORT_C TInt Authenticate();
	
private:
	RBTBaseband iBTBaseband;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};

/**
This allows for notification of events relating to a CBluetoothPhysicalLinkMetrics object.

These notifications will be called when the value of a physical link metric that has been
subscribed to in CBluetoothPhysicalLinkMetrics changes. If the notification is not called,
the value has not changed since the last notification

Mixin class to be used with CBluetoothPhysicalLinkMetrics
@publishedAll
@released
*/
class MBluetoothPhysicalLinkMetricsObserver
	{
public:
	/**
	Notification for the initial RSSI (received signal strengh indication) value
	and whenever the RSSI changes
	@param aRssi Current RSSI Value in dB
	*/
	virtual void MbplmoRssiChanged(TInt8 aRssi) = 0;
	
	/**
	Notification for the initial Link Quality value and whenever the Link Quality changes.
	@param aLinkQuality Link quality for this connection where the higher value represents a better quality link
	*/
	virtual void MbplmoLinkQualityChanged(TUint8 aLinkQuality) = 0;
	
	/**
	Notification for the initial failed contact counter value, and whenever the failed contact counter changes.
	@param aFailedContactCounter Number of consecutive failed contacts for this connection
	*/
	virtual void MbplmoFailedContactCounterChanged(TUint16 aFailedContactCounter) = 0;
	
	/**
	Notification for the initial transmit power level, and whenever the transmit power level changes.
	@param aTransmitPowerLevel Transmit power level in dBm
	*/
	virtual void MbplmoTransmitPowerLevelChanged(TInt8 aTransmitPowerLevel) = 0;
	
	/**
	Notification that an error has occured. When this is called, the current
	subscriptions will be cancelled.
	If a link disconnection occurs, the CBluetoothPhysicalLinkMetrics object will
	no longer be valid, so it is recommended to delete the object in this callback
	*/
	virtual void MbplmoError(TInt aError) = 0;
	
	/**
	Returns a null aObject if the extension is not implemented, or a pointer to another interface if it is.
	@param aInterface UID of the interface to return
	@return The container for another interface as specified by aInterface
	*/
	IMPORT_C virtual TAny* MbplmoExtensionInterfaceL(TUid aInterface);
	};

class CBluetoothPhysicalLinkMetricSubscriber;

/**
Class to subscribe to physical link metrics
@publishedAll
@released
*/
NONSHARABLE_CLASS(CBluetoothPhysicalLinkMetrics): public CBase
	{
public:
	IMPORT_C static CBluetoothPhysicalLinkMetrics* NewL(
						MBluetoothPhysicalLinkMetricsObserver& aObserver,
						RSocketServ& aSockServ,
						RSocket& aConnectedSocket);
	
	IMPORT_C static CBluetoothPhysicalLinkMetrics* NewL(
						MBluetoothPhysicalLinkMetricsObserver& aObserver,
						RSocketServ& aSockServ,
						const TBTDevAddr& aAddr);
	
	IMPORT_C ~CBluetoothPhysicalLinkMetrics();
	IMPORT_C void SubscribeRssi();
	IMPORT_C void SubscribeLinkQuality();
	IMPORT_C void SubscribeFailedContactCounter();
	IMPORT_C void SubscribeTransmitPowerLevel();
	IMPORT_C void Cancel();
	
	void RssiChanged(TInt8 aRssi);
	void LinkQualityChanged(TUint8 aLinkQuality);
	void FailedContactCounterChanged(TUint16 aFailedContactCounter);
	void TransmitPowerLevelChanged(TInt8 aTransmitPowerLevel);
	void SubscribeError(TInt aError);

	
private:
	CBluetoothPhysicalLinkMetrics(MBluetoothPhysicalLinkMetricsObserver& aObserver);
	template<class T> void ConstructL(RSocketServ& aSockServ, T& aLinkIdentifier);	
	
private:
	MBluetoothPhysicalLinkMetricsObserver& iObserver;
	CBluetoothPhysicalLinkMetricSubscriber* iRssiBaseband;
	CBluetoothPhysicalLinkMetricSubscriber* iLinkQualityBaseband;
	CBluetoothPhysicalLinkMetricSubscriber* iFailedContactCounterBaseband;
	CBluetoothPhysicalLinkMetricSubscriber* iTransmitPowerLevelBaseband;
	};


class MBluetoothSocketNotifier
/** This allows for notification of events relating to a CBluetoothSocket object.

Such notification consists of notification of logical link events (for example receipt 
of a user packet) and physical link state events (for example change of power mode).

Mixin class to be used with CBluetoothSocket
@publishedAll
@released
*/
	{
public:
	/** Notification of a connection complete event.
	
	 If no error is reported, then the connection is ready for use.
	 @note If the implementation of this function needs to delete associated 
	 CBluetoothSocket object, it should NOT use delete operator. The implementation 
	 should call CBluetoothSocket::AsyncDelete() method instead.
	 @param aErr the returned error
	*/
	virtual void HandleConnectCompleteL(TInt aErr) = 0;

	/** Notification of an accept complete event.
	
	 If no error is reported, then we have accepted a connection request 
	 and that connection is ready for use.
	 @note If the implementation of this function needs to delete associated 
	 CBluetoothSocket object, it should NOT use delete operator. The implementation 
	 should call CBluetoothSocket::AsyncDelete() method instead.
	 @param aErr the returned error
	*/
	virtual void HandleAcceptCompleteL(TInt aErr) = 0;

	/** Notification of a shutdown complete event.
	
	 If no error is reported, then the connection has been closed.
	 @note If the implementation of this function needs to delete associated 
	 CBluetoothSocket object, it should NOT use delete operator. The implementation 
	 should call CBluetoothSocket::AsyncDelete() method instead.
	 @param aErr the returned error
	*/
	virtual void HandleShutdownCompleteL(TInt aErr) = 0;

	/** Notification of a send complete event.
	
	 If no error is reported, then an attempt to send data over Bluetooth has succeeded.
	 @note If the implementation of this function needs to delete associated 
	 CBluetoothSocket object, it should NOT use delete operator. The implementation 
	 should call CBluetoothSocket::AsyncDelete() method instead.
	 @param aErr the returned error
	*/
	virtual void HandleSendCompleteL(TInt aErr) = 0;

	/** Notification of a receive complete event.
	
	 If no error is reported, then then we have successfully received
	 a specified quantity of data.
	 @note If the implementation of this function needs to delete associated 
	 CBluetoothSocket object, it should NOT use delete operator. The implementation 
	 should call CBluetoothSocket::AsyncDelete() method instead.
	 @param aErr the returned error
	*/
	virtual void HandleReceiveCompleteL(TInt aErr) = 0;

	/** Notification of a ioctl complete event.
	
	 An HCI request that has an associated command complete has completed.
	 @note If the implementation of this function needs to delete associated 
	 CBluetoothSocket object, it should NOT use delete operator. The implementation 
	 should call CBluetoothSocket::AsyncDelete() method instead.
	 @param aErr the returned error
	*/
	virtual void HandleIoctlCompleteL(TInt aErr) = 0;

	/** Notification of a baseband event.
	
	 If no error is reported, then a baseband event has been retrieved successfully.
	 @note If the implementation of this function needs to delete associated 
	 CBluetoothSocket object, it should NOT use delete operator. The implementation 
	 should call CBluetoothSocket::AsyncDelete() method instead.
	 @param aErr the returned error
	 @param TBTBasebandEventNotification Bit(s) set in TBTBasebandEventNotification bitmap indicate what event has taken place.
	 @see TBTPhysicalLinkStateNotifier
	*/
	virtual void HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification) = 0;
	
    /**
 	 Returns a null aObject if the extension is not implemented, or a pointer to another interface if it is.
	 @param aInterface UID of the interface to return
	 @param aObject the container for another interface as specified by aInterface
	 */
	IMPORT_C virtual void MBSN_ExtensionInterfaceL(TUid aInterface, void*& aObject);	
	};


class CBTConnecter;
class CBTAccepter;
class CBTShutdowner;
class CBTReceiver;
class CBTSender;
class CBTIoctler;
class CBTBasebandChangeEventNotifier;
class CAutoSniffDelayTimer;
class CBTBasebandManager;
class CBTBasebandChangeEventDelegate;

NONSHARABLE_CLASS(CBluetoothSocket): public CBase
    /** This allows Bluetooth ACL socket-based services to be run.

    It allows all user-plane data flow to occur, plus control-plane Bluetooth 
    baseband modification capabilities.
    
    For a more detailed description of RBTBaseband functionality see the class and function documentation for
    RBTPhysicalLinkAdapter.
    
	This class doesn't provide the functionality to directly activate Active mode
	(this is implementated in class RBTPhysicalLinkAdapter.)
	@see RBTPhysicalLinkAdapter::ActivateActiveRequester()
	Explicit Active mode requests are made automatically when using the Automatic Sniff Requester 
	utility provided by this class.
	@see CBluetoothSocket::SetAutomaticSniffMode	

    @see RBTPhysicalLinkAdapter
    @publishedAll
    @released
    */
	{
friend class CBTAccepter;		
public:
	IMPORT_C static CBluetoothSocket* NewL(MBluetoothSocketNotifier& aNotifier, 
										   RSocketServ& aServer,TUint aSockType,
										   TUint aProtocol);
	IMPORT_C static CBluetoothSocket* NewLC(MBluetoothSocketNotifier& aNotifier, 
										   RSocketServ& aServer,TUint aSockType,
										   TUint aProtocol);
	IMPORT_C static CBluetoothSocket* NewL(MBluetoothSocketNotifier& aNotifier, 
										   RSocketServ& aServer,TUint aSockType,
										   TUint aProtocol, 
										   RConnection& aConnection);
	IMPORT_C static CBluetoothSocket* NewLC(MBluetoothSocketNotifier& aNotifier, 
										   RSocketServ& aServer,
										   TUint aSockType,TUint aProtocol, 
										   RConnection& aConnection);
	IMPORT_C static CBluetoothSocket* NewL(MBluetoothSocketNotifier& aNotifier, 
										   RSocketServ& aServer,
										   const TDesC& aName);
	IMPORT_C static CBluetoothSocket* NewLC(MBluetoothSocketNotifier& aNotifier, 
										   RSocketServ& aServer,
										   const TDesC& aName);
	IMPORT_C static CBluetoothSocket* NewL(MBluetoothSocketNotifier& aNotifier, 
										   RSocketServ& aServer);
	IMPORT_C static CBluetoothSocket* NewLC(MBluetoothSocketNotifier& aNotifier, 
										   RSocketServ& aServer);
	IMPORT_C static CBluetoothSocket* NewL(MBluetoothSocketNotifier& aNotifier, 
										   RSocketServ& aServer,
										   RSocket& aSocket);
	IMPORT_C static CBluetoothSocket* NewLC(MBluetoothSocketNotifier& aNotifier, 
										   RSocketServ& aServer,
										   RSocket& aSocket);								
	IMPORT_C ~CBluetoothSocket();

	//Forwarding functions to RSocket
	IMPORT_C TInt Send(const TDesC8& aDesc,TUint someFlags);
	IMPORT_C TInt Send(const TDesC8& aDesc,TUint someFlags,TSockXfrLength& aLen);
	IMPORT_C void CancelSend();
	IMPORT_C TInt Recv(TDes8& aDesc,TUint flags);
	IMPORT_C TInt Recv(TDes8& aDesc,TUint flags,TSockXfrLength& aLen);
	IMPORT_C TInt RecvOneOrMore(TDes8& aDesc,TUint flags,TSockXfrLength& aLen);
	IMPORT_C void CancelRecv();
	IMPORT_C TInt Read(TDes8& aDesc);
	IMPORT_C void CancelRead();
	IMPORT_C TInt Write(const TDesC8& aDesc);
	IMPORT_C void CancelWrite();
	IMPORT_C TInt SendTo(const TDesC8& aDesc,TSockAddr& aSockAddr,TUint flags);
	IMPORT_C TInt SendTo(const TDesC8& aDesc,TSockAddr& aSockAddr,TUint flags,TSockXfrLength& aLen);
	IMPORT_C TInt RecvFrom(TDes8& aDesc,TSockAddr& aSockAddr,TUint flags);
	IMPORT_C TInt RecvFrom(TDes8& aDesc,TSockAddr& aSockAddr,TUint flags,TSockXfrLength& aLen);
	IMPORT_C TInt Connect(TBTSockAddr& aSockAddr);
	IMPORT_C TInt Connect(TBTSockAddr& aSockAddr,const TDesC8& aConnectDataOut,TDes8& aConnectDataIn);
	IMPORT_C TInt Connect(TBTSockAddr& aAddr, TUint16 aServiceBits);
	IMPORT_C void CancelConnect();
	IMPORT_C TInt Bind(TSockAddr& aSockAddr);
	IMPORT_C TInt SetLocalPort(TInt aPort);
	IMPORT_C TInt Accept(CBluetoothSocket& aBlankSocket);
	IMPORT_C TInt Accept(CBluetoothSocket& aBlankSocket,TDes8& aConnectData);
	IMPORT_C void CancelAccept();
	IMPORT_C TInt Listen(TUint qSize);
	IMPORT_C TInt Listen(TUint qSize,const TDesC8& aConnectData);
	IMPORT_C TInt Listen(TUint qSize, TUint16 aServiceBits);
	IMPORT_C TInt SetOpt(TUint aOptionName,TUint aOptionLevel,TInt aOption);
	IMPORT_C TInt SetOption(TUint aOptionName,TUint aOptionLevel,const TDesC8& aOption);
	IMPORT_C TInt GetOpt(TUint aOptionName,TUint aOptionLevel,TDes8& aOption);
	IMPORT_C TInt GetOpt(TUint aOptionName,TUint aOptionLevel,TInt &aOption);
	IMPORT_C TInt Ioctl(TUint aLevel, TUint aCommand, TDes8* aDesc);
	IMPORT_C void CancelIoctl();
	IMPORT_C TInt GetDisconnectData(TDes8& aDesc);
	IMPORT_C void LocalName(TSockAddr& aSockAddr);
	IMPORT_C TUint LocalPort();
	IMPORT_C void RemoteName(TSockAddr& aSockAddr);
	IMPORT_C TInt Shutdown(RSocket::TShutdown aHow);
	IMPORT_C TInt Shutdown(RSocket::TShutdown aHow,const TDesC8& aDisconnectDataOut,TDes8& aDisconnectDataIn);
	IMPORT_C void CancelAll();
	IMPORT_C TInt Info(TProtocolDesc& aProtocol);
	IMPORT_C TInt Name(TName& aName);
	IMPORT_C TInt Transfer(RSocketServ& aServer, const TDesC& aName);

	
	//Forwarding functions to RBTBaseband
	IMPORT_C TInt PhysicalLinkState(TUint32& aState);
	IMPORT_C TInt PreventRoleSwitch();
	IMPORT_C TInt AllowRoleSwitch();
	IMPORT_C TInt RequestMasterRole();
	IMPORT_C TInt RequestSlaveRole();
	IMPORT_C TInt PreventLowPowerModes(TUint32 aLowPowerModes);
	IMPORT_C TInt AllowLowPowerModes(TUint32 aLowPowerModes);
	IMPORT_C TInt ActivateSniffRequester();
	IMPORT_C TInt ActivateParkRequester();
	IMPORT_C TInt CancelLowPowerModeRequester();
	IMPORT_C TInt RequestChangeSupportedPacketTypes(TUint16 aPacketTypes);
	IMPORT_C TInt ActivateBasebandEventNotifier(TUint32 aEventTypes);
	IMPORT_C void CancelBasebandEventNotifier();
	
	IMPORT_C void SetNotifier(MBluetoothSocketNotifier& aNewNotifier);
	IMPORT_C TInt SetAutomaticSniffMode(TBool aAutoSniffMode);
	IMPORT_C TInt SetAutomaticSniffMode(TBool aAutoSniffMode, TInt aIdleSecondsBeforeSniffRequest);
	IMPORT_C TBool AutomaticSniffMode() const;
	
	IMPORT_C void AsyncDelete();
	
	//Callback functions called by active object RunLs.
	// NB These functions kill the active objects that call them (cf mating spiders) 
	MBluetoothSocketNotifier& Notifier();
	void HandleConnectCompleteL(TInt aErr);
	void HandleAcceptCompleteL(TInt aErr);
	void HandleShutdownCompleteL(TInt aErr);
	void HandleSendCompleteL(TInt aErr);
	void HandleReceiveCompleteL(TInt aErr);
	void HandleIoctlCompleteL(TInt aErr);
	void HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification aEventNotification);

	/**
	@deprecated
	*/
	IMPORT_C TInt Ioctl(TUint aCommand,TDes8* aDesc=NULL,TUint aLevel=KLevelUnspecified);

	/**
	@deprecated
	*/
	IMPORT_C TInt SetOpt(TUint aOptionName,TUint aOptionLevel,const TDesC8& aOption=TPtrC8(NULL,0));
public:
	RSocket& Socket();
	RSocketServ& SocketServer();
	RBTBaseband& BTBaseband();
	CBTBasebandManager& BTBasebandManager();

private:
	CBluetoothSocket(MBluetoothSocketNotifier& aNotifier, RSocketServ& aServer);
	void ConstructL(TUint aSockType,TUint aProtocol);
	void ConstructL(TUint aSockType,TUint aProtocol, RConnection& aConnection);
	void ConstructL(const TDesC& aName);
	void ConstructL();
	void ConstructL(RSocket& aSocket);
	void InitialiseL();
	static TInt AsyncDeleteCallBack(TAny *aThisPtr);
	void ConfigureSocket();

private:
	RSocket							iSocket;
	RSocketServ&					iSockServer;
	TBTSockAddr						iSockAddr;

	MBluetoothSocketNotifier*		iNotifier;

	CBTConnecter* 					iBTConnecter;
	CBTAccepter*					iBTAccepter;
	CBTShutdowner*					iBTShutdowner;
	CBTReceiver*					iBTReceiver;	//for read, recv etc
	CBTSender*						iBTSender;		//for send, write etc
	CBTIoctler*						iBTIoctler;
	CBTBasebandChangeEventNotifier* iBTBasebandChangeEventNotifier;
	TUint32							iNotifierEventMask;
	TBool							iSending;
	TBool							iReceiving;

	RBTBaseband 					iBTBaseband;
	CAutoSniffDelayTimer*			iAutoSniffDelayTimer;
	CBTBasebandManager*				iBTBasebandManager;
	CBTBasebandChangeEventDelegate*	iBTBasebandChangeEventDelegate;
	
	CAsyncCallBack*					iAsyncDestroyer;	//for async deletion
	};


class MBluetoothSynchronousLinkNotifier
/** SCO and eSCO link notification events.

This allows for notification of Connect, Disconnect, Send and
Receive events relating to SCO and eSCO links.

Mixin class to be used with CBluetoothSynchronousLink
Note that although the function signatures allow it, these functions should
not be allowed to leave as the error will be ignored.

@publishedAll
@released
*/
	{
public:
	/** Notification that a synchronous link (SCO) has been set up
	
	 If no error is reported, then the synchronous link is ready for use.
	 @note 1) Observe that although the function signature allows it, this function should
	 not be allowed to leave as the error will be ignored.
	 @note 2) The implementation of this function should NOT be used to delete the associated 
	 CBluetoothSynchronousLink object.
	 @param aErr the returned error
	*/
	virtual void HandleSetupConnectionCompleteL(TInt aErr) = 0;

	/** Notification that a synchronous link (SCO) has disconnected
	
	 If no error is reported, then the synchronous link has been closed.
	 @note 1) Observe that although the function signature allows it, this function should
	 not be allowed to leave as the error will be ignored.
	 @note 2) The implementation of this function should NOT be used to delete the associated 
	 CBluetoothSynchronousLink object.
	 @param aErr the returned error
	*/
	virtual void HandleDisconnectionCompleteL(TInt aErr) = 0;

	/** Notification that a synchronous link (SCO) has been accepted
	
	 If no error is reported, then we have accepted a request for a synchronous link.
	 That synchronous link is ready for use.
	 @note 1) Observe that although the function signature allows it, this function should
	 not be allowed to leave as the error will be ignored.
	 @note 2) The implementation of this function should NOT be used to delete the associated 
	 CBluetoothSynchronousLink object.
	 @param aErr the returned error
	*/
	virtual void HandleAcceptConnectionCompleteL(TInt aErr) = 0;

	/** Notification of a send complete event
	
	 If no error is reported, then an attempt to send synchronous (SCO) data 
	 (e.g. voice) over Bluetooth has succeeded.
	 @note 1) Observe that although the function signature allows it, this function should
	 not be allowed to leave as the error will be ignored.
	 @note 2) The implementation of this function should NOT be used to delete the associated 
	 CBluetoothSynchronousLink object.
	 @param aErr the returned error
	*/
	virtual void HandleSendCompleteL(TInt aErr) = 0;

	/** Notification of a receive complete event
	
	 If no error is reported, then then we have successfully received
	 a specified quantity of synchronous (SCO) data.
	 @note 1) Observe that although the function signature allows it, this function should
	 not be allowed to leave as the error will be ignored.
	 @note 2) The implementation of this function should NOT be used to delete the associated 
	 CBluetoothSynchronousLink object.
	 @param aErr the returned error
	*/
	virtual void HandleReceiveCompleteL(TInt aErr) = 0;
	
    /**
 	 Returns a null aObject if the extension is not implemented, or a pointer to another interface if it is.
	 @param aInterface UID of the interface to return
	 @param aObject the container for another interface as specified by aInterface
	 */
	IMPORT_C virtual void MBSLN_ExtensionInterfaceL(TUid aInterface, void*& aObject);	
	};

class CBTSynchronousLinkAttacher;
class CBTSynchronousLinkDetacher;
class CBTSynchronousLinkAccepter;
class CBTSynchronousLinkSender;
class CBTSynchronousLinkReceiver;
class CBTSynchronousLinkBaseband;

/**
eSCO 64Kbit link utility constant.
*/
static const TUint KBTSync64KBit = (64000 / 8);

NONSHARABLE_CLASS(CBluetoothSynchronousLink): public CBase
/** Provides Bluetooth SCO functionality.

This allows Bluetooth SCO (synchronous) link Connect, Disconnect, Send and Receive.
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CBluetoothSynchronousLink* NewL(MBluetoothSynchronousLinkNotifier& aNotifier, 
											  RSocketServ& aServer);
	IMPORT_C static CBluetoothSynchronousLink* NewLC(MBluetoothSynchronousLinkNotifier& aNotifier,
											   RSocketServ& aServer);
	IMPORT_C ~CBluetoothSynchronousLink();

	IMPORT_C TInt SetupConnection(const TBTDevAddr& aDevAddr);
	IMPORT_C TInt SetupConnection(const TBTDevAddr& aDevAddr, const TUint16 aPacketTypes);
	IMPORT_C TInt SetupConnection(const TBTDevAddr& aDevAddr, const TBTSyncPackets& aPacketTypes);
	IMPORT_C void CancelSetup();
	IMPORT_C TInt Disconnect();
	IMPORT_C TInt Send(const TDesC8& aData);
	IMPORT_C void CancelSend();
	IMPORT_C TInt Receive(TDes8& aData);
	IMPORT_C void CancelReceive();
	IMPORT_C TInt AcceptConnection();
	IMPORT_C TInt AcceptConnection(const TBTSyncPackets& aPacketTypes);
	IMPORT_C void CancelAccept();
	
	IMPORT_C void SetCoding(TUint16 aVoiceSetting);
	IMPORT_C void SetMaxBandwidth(TBTSyncBandwidth aMaximum);
	IMPORT_C void SetMaxLatency(TUint16 aLatency);
	IMPORT_C void SetRetransmissionEffort(TBTeSCORetransmissionTypes aRetransmissionEffort);
	
	IMPORT_C TUint16 Coding();
	IMPORT_C TBTSyncBandwidth Bandwidth();
	IMPORT_C TUint16 Latency();
	IMPORT_C TUint8 RetransmissionEffort();
	
	IMPORT_C void RemoteName(TSockAddr& aAddr);
	
	IMPORT_C void SetNotifier(MBluetoothSynchronousLinkNotifier& aNotifier);

	MBluetoothSynchronousLinkNotifier& Notifier();
    RSocket& SCOSocket();
	RSocket& ESCOSocket();
	RSocket& ListeningSCOSocket();
	RSocket& ListeningESCOSocket();
	RSocketServ& SocketServer();
	RBTBaseband& Baseband();

	
	//Callback methods called by active object RunLs.
	void HandleSetupConnectionCompleteL(TInt aErr, TSCOType aSCOType);
	void HandleAcceptConnectionCompleteL(TInt aErr, TSCOType aSCOType);
	void HandleDisconnectionCompleteL(TInt aErr);
	void HandleSendCompleteL(TInt aErr);
	void HandleReceiveCompleteL(TInt aErr);

private:
	CBluetoothSynchronousLink(MBluetoothSynchronousLinkNotifier& aNotifier, RSocketServ& aServer);
	void ConstructL();
	void UpdateLinkParams(TSCOType aSCOType);
	TInt LinkUp(TBTDevAddr aAddr);
	void LinkDown();
	void AcceptConnectionL(const TBTSyncPackets& aPacketTypes);
	static void StaticResetScoTypes(TAny* aThis);

private:
	CBTSynchronousLinkSender*			iBTSynchronousLinkSenderSCO;
	CBTSynchronousLinkSender*			iBTSynchronousLinkSenderESCO;
	CBTSynchronousLinkReceiver*			iBTSynchronousLinkReceiverSCO;
	CBTSynchronousLinkReceiver*			iBTSynchronousLinkReceiverESCO;
	CBTSynchronousLinkAccepter*			iBTSynchronousLinkAccepterSCO;
	CBTSynchronousLinkAccepter*			iBTSynchronousLinkAccepterESCO;
	CBTSynchronousLinkAttacher* 		iBTSynchronousLinkAttacherSCO;
	CBTSynchronousLinkAttacher* 		iBTSynchronousLinkAttacherESCO;
	CBTSynchronousLinkDetacher* 		iBTSynchronousLinkDetacherSCO;
	CBTSynchronousLinkDetacher* 		iBTSynchronousLinkDetacherESCO;
	CBTSynchronousLinkBaseband*			iBTSynchronousLinkBaseband;
	MBluetoothSynchronousLinkNotifier*	iNotifier;
	RSocket								iSCOSocket;
	RSocket								iESCOSocket;
	RSocket 							iListeningSCOSocket;
	RSocket 							iListeningESCOSocket;
	RBTBaseband							iBaseband;
	RSocketServ& 						iSockServer;
	TBTSockAddr 						iSockAddr;
	TBuf8<1> 							iDummySCOShutdownDescriptor;
	
	TBTeSCOLinkParams					iRequestedLink;
	TBTeSCOLinkParams					iNegotiatedLink;
	
	TUint8		 						iSCOTypes;
	
	TBool								iOpeningSCO;
	TBool								iOpeningESCO;
	TBool								iOpenedSCO;
	};


class MBluetoothPhysicalLinksNotifier
/** This allows for notifications of Physical link connections & disconnections

Mixin class to be used with CBluetoothPhysicalLinks


@publishedAll
@released
*/
	{
public:
	/** Notification of a requested connection coming up
	
	 If no error is reported, then that connection is ready for use.
	 @note 1) While this function may leave, any errors are ignored.  Implementers are
	 responsible for performing their own cleanup prior to exiting the function.	 
	 @note 2) The implementation of this function should NOT be used to delete the associated 
	 CBluetoothPhysicalLinks object.
	 
	 @param aErr the returned error
	*/
	virtual void HandleCreateConnectionCompleteL(TInt aErr) = 0;

	/** Notification of a requested disconnection having taken place
	
	 If no error is reported, then that connection has been closed.
	 @note 1) While this function may leave, any errors are ignored.  Implementers are
	 responsible for performing their own cleanup prior to exiting the function.
	 @note 2) The implementation of this function should NOT be used to delete the associated 
	 CBluetoothPhysicalLinks object.
	 @param aErr the returned error
	*/
	virtual void HandleDisconnectCompleteL(TInt aErr) = 0;

	/** Notification that all existing connections have been torn down
	
	 If no error is reported, then there are no Bluetooth connections existing.
	 @note 1) While this function may leave, any errors are ignored.  Implementers are
	 responsible for performing their own cleanup prior to exiting the function.
	 @note 2) The implementation of this function should NOT be used to delete the associated 
	 CBluetoothPhysicalLinks object.
	 @param aErr the returned error
	*/
	virtual void HandleDisconnectAllCompleteL(TInt aErr) = 0;

    /**
 	 Returns a null aObject if the extension is not implemented, or a pointer to another interface if it is.
	 @param aInterface UID of the interface to return
	 @param aObject the container for another interface as specified by aInterface
	 */
	IMPORT_C virtual void MBPLN_ExtensionInterfaceL(TUid aInterface, void*& aObject);	
	};


class CBTBasebandConnecter;
class CBTBasebandShutdowner;
class CBTDisconnector;

NONSHARABLE_CLASS(CBluetoothPhysicalLinks): public CBase
/** This is used to enumerate members of piconet, and attach and remove members thereof

It may also be used for non-service dependent reads and writes.
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CBluetoothPhysicalLinks* NewL(MBluetoothPhysicalLinksNotifier& aNotifier,
											      RSocketServ& aServer);
	IMPORT_C static CBluetoothPhysicalLinks* NewLC(MBluetoothPhysicalLinksNotifier& aNotifier,
											      RSocketServ& aServer);
	IMPORT_C ~CBluetoothPhysicalLinks();

	IMPORT_C TInt CreateConnection(const TBTDevAddr& aDevAddr);
	IMPORT_C void CancelCreateConnection();
	IMPORT_C TInt Disconnect(const TBTDevAddr& aDevAddr);
	IMPORT_C TInt DisconnectAll();
	IMPORT_C TInt DisconnectAllForPowerOff();
	
	IMPORT_C TInt Broadcast(const TDesC8& aData);
	IMPORT_C TInt ReadRaw(TDes8& aData);
	IMPORT_C TInt Enumerate(RBTDevAddrArray& aBTDevAddrArray, TUint aMaxNumber);

	
	//Callback methods called by active object RunLs.
	//NB These methods kill the active objects that call them
	/**
	This method is for internal sub-system use only and should be not be used otherwise.
	@internalTechnology
	*/
	void HandleCreateConnectionCompleteL(TInt aErr);
	/**
	This method is for internal sub-system use only and should be not be used otherwise.
	@internalTechnology
	*/
	void HandleDisconnectCompleteL(TInt aErr);
	/**
	This method is for internal sub-system use only and should be not be used otherwise.
	@internalTechnology
	*/
	void HandleDisconnectAllCompleteL(TInt aErr);

	/**
	This method is for internal sub-system use only and should be not be used otherwise.
	@internalTechnology
	*/
	RSocketServ& SockServer();
	/**
	This method is for internal sub-system use only and should be not be used otherwise.
	@internalTechnology
	*/
	RBTBaseband& BTBaseband();
	/**
	This method is for internal sub-system use only and should be not be used otherwise.
	@internalTechnology
	*/
	MBluetoothPhysicalLinksNotifier& Notifier();


private:
	CBluetoothPhysicalLinks(MBluetoothPhysicalLinksNotifier& aNotifier, 
							RSocketServ& aServer);
	void ConstructL();

private:
	CBTBasebandConnecter* iBTBasebandConnecter;
	CBTDisconnector* iBTDisconnector;
	MBluetoothPhysicalLinksNotifier& iNotifier;
	RSocketServ& iSockServer;
	RBTBaseband iBTBaseband;
	};

#endif
