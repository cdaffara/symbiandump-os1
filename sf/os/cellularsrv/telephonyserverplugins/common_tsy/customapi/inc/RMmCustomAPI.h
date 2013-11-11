/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @deprecated
*/
#ifndef RMMCUSTOMAPI_H
#define RMMCUSTOMAPI_H

//  INCLUDES
#include <etelmm.h>
#include <et_clsvr.h>
#include <etelpckt.h>

// CONSTANTS
const TInt KIpcCustomExt = 1000000;
const TInt KIpcCustomMaxLimi = 1000299;
const TInt KIpcCustomAPIExtensionExt = 1000450;

_LIT(KCustomAPIName,"CUSTOMAPI");
_LIT(KTerminateAllCalls,"ALL_CALLS");
const TInt KSimLockMaxPasswordSize = 30;
const TInt KSimCbTopicNameLength = 20;
const TInt KSubAddressSize = 20;
const TInt KBearerCapabilitySize = 14;

// alpha tag length is 124
const TInt KMaxOperatorNameLength = 124;
// max length of operator length
const TInt KMaxOperatorLogoLength = 6405;

const TUint16 KFlexibleNameSeparator = 0x007C;  // '|'
const TUint8 KFlexibleLongNameMaxLength = 30;
const TUint8 KFlexibleShortNameMaxLength = 20;

// Sim file definitions
const TUint8 KSimFilePathLength = 8;
const TUint16 KSimFileMaxLength = 0xFFFF;

// Viag HomeZone setting flags
const TUint8 KUHziUeActive = 0x00001;
const TUint8 HCbHziUeActive = 0x00002;

// ADPU buffer maximum length 258 according to
// ISI ADPU server description
typedef TBuf8<258> TApduBuffer;

// PDP context activation failure in case of APN not in EFACL
const TInt KErrPacketDataTsyInvalidAPN = -6001;

// Cell info constant
const TInt KMaxTimeSlotIscpAmount = 1; //14; Changed from 14 to 1. Memory problem!!
const TInt KMaxCellMeasuredResults = 32;
const TInt KMaxNetworkMeasureReports = 8;
const TInt KMaxNmrAmount = 15;

// DATA TYPES
/** Enumerates IPC values used by Custom API. */
enum TCustomIPC
    {
    /** Notify ss additional info */
    ECustomSsAdditionalInfoNotificationIPC = KIpcCustomExt,		//1000000
    /** Notify ss request complete */
    ECustomNotifySsRequestCompleteIPC,		//1000001
    /** Cancel USSD session */
    ECustomCancelUssdSessionIPC,			//1000002
    /** Start SIM CB topic browsing */
    ECustomStartSimCbTopicBrowsingIPC,		//1000003
    /** Get next SIM CB topic */
	ECustomGetNextSimCbTopicIPC,			//1000004
    /** Delete SIM DB topic */
    ECustomDeleteSimCbTopicIPC,				//1000005
    /** Read SIM File */
    ECustomReadSimFileIPC,					//1000006
    /** Get wlan sim authentication data */
    ECustomGetSimAuthenticationDataIPC,		//1000007
    /** Set SIM SMS to "read" */
    ECustomSetSimMessageStatusReadIPC,		//1000008
    /** Notify Dtmf event */
    ECustomNotifyDtmfEventIPC,				//1000009
    /** Get Diagnostic Octets */
    ECustomGetDiagnosticOctetsIPC,			//1000010
    /** Set Als Blocked */
    ECustomSetAlsBlockedIPC,				//1000011
    /** Get Als Blocked */
    ECustomGetAlsBlockedIPC,				//1000012
    /** Check Als Pp Support */
    ECustomCheckAlsPpSupportIPC,			//1000013
    /** Remote Alerting Tone Status */
    ECustomGetRemoteAlertingToneStatusIPC,	//1000014
    /** Call Origin */
    ECustomCallOriginIPC,					//1000015
    /** Notify Als Blocked Change */
    ECustomNotifyAlsBlockedChangedIPC,		//1000016
    /** Terminate a Call */
    ECustomTerminateCallIPC,				//1000017
    /** Clear call blacklist */
    ECustomClearCallBlackListIPC,			//1000018
    /** Read HSxPA Status */
    ECustomReadHSxPAStatusIPC,				//1000019
    /** Write HSxPA Status */    
    ECustomWriteHSxPAStatusIPC,				//1000020
    /** Notify HSxPA Status change */    
    ECustomNotifyHSxPAStatusIPC,			//1000021
    /** Get cell(s) info */
    ECustomGetCellInfoIPC,					//1000022
    /** Notify cell(s) info change */
    ECustomNotifyCellInfoChangeIPC,			//1000023
    /** Activate SIM Lock */
    ECustomSimLockActivateIPC,				//1000024
    /** Deactivate SIM Lock */
    ECustomSimLockDeActivateIPC,			//1000025
    /** Read VIAG Home Zone Parameters */
    EReadViagHomeZoneParamsIPC,				//1000026
    /** Read VIAG Home Zone Cache */
    EReadViagHomeZoneCacheIPC,				//1000027
    /** Write VIAG Home Zone Cache */
    EWriteViagHomeZoneCacheIPC,				//1000028
    /** Get ciphering information */
    ECustomGetCipheringInfoIPC,				//1000029
    /** Notify ciphering information */
    ECustomNotifyCipheringInfoChangeIPC,	//1000030
    /** Notify NSPS status */
    ECustomNotifyNSPSStatusIPC,				//1000031
    /** Wake net from NSPS */
    ECustomNetWakeupIPC,					//1000032
    /** Get network provider name */
    ECustomGetNetworkProviderNameIPC,		//1000033
    /** Get operator name */
    ECustomGetOperatorNameIPC,				//1000034
    /** Get programmable operator logo */
    ECustomGetProgrammableOperatorLogoIPC,	//1000035
    /** Reset the net server */
    ECustomResetNetServerIPC,				//1000036
    /** Notify programmable operator logo change */
    ECustomNotifyProgrammableOperatorLogoChangeIPC,		//1000037
    /** Notify network connection failure */
    ECustomNotifyNetworkConnectionFailureIPC,			//1000038
    /** Get Air time duration */
    ECustomGetAirTimeDurationIPC,			//1000039
    /** Notify phonebook cache ready */
    ECustomNotifyPndCacheReadyIPC,			//1000040
    /** Get phonebook cache status */
    ECustomGetPndCacheStatusIPC,			//1000041
    /** Notify SS network event */
    ECustomNotifySsNetworkEventIPC,			//1000042
    /** Check two digit dial support */
    ECustomCheckTwoDigitDialSupportIPC,		//1000043
    /** Send APDU request */
    ECustomSendAPDUReqIPC,					//1000044
    /** Get the life timer's info */
    ECustomGetLifeTimeIPC,					//1000045
    /** Power SIM on */
    ECustomPowerSimOnIPC,					//1000046
    /** Power SIM off */
    ECustomPowerSimOffIPC,					//1000047
    /** Perform SIM warm reset */
    ECustomSimWarmResetIPC,					//1000048
    /** Get Answer To Reset */
    ECustomGetATRIPC,						//1000049
    /** Send APDU */
    ECustomSendAPDUReqV2IPC,				//1000050
    /** Get SIM card reader status */
    ECustomGetSimCardReaderStatusIPC,		//1000051
    /** Notify SIM card status */
    ECustomNotifySimCardStatusIPC,			//1000052
    /** Set Drive Mode status */
    ECustomSetDriveModeIPC,					//1000053
    /** Check if security code is blocked or not */
    ECustomIsBlockedIPC,					//1000054
    /** Check security code */
    ECustomCheckSecurityCodeIPC,			//1000055
    /** Check emergency number */
    ECustomCheckEmergencyNumberIPC,			//1000056
    /** SAT refresh complete notification */
    ECustomSatRefreshCompleteNotificationIPC,	//1000057
    /** Disable phone lock */
    ECustomDisablePhoneLockIPC,					//1000058
    /** Get currently active PIN */
    ECustomGetActivePinIPC,						//1000059
    /** Release CommDB file */
    ECustomReleaseFileIPC,						//1000060
    /** Restart reading CommDB */
    ECustomRestartFileIPC,						//1000061
    /** Notify GPRS connection change */
    ECustomNotifyEGprsInfoChange,				//1000062
    /** Get GPRS connection information */
    ECustomGetEGprsInfo,						//1000063
    /** Get phonebook 3G info */
    EGet3GPBInfoIPC,							//1000064
    /** Get system network modes */
    ECustomGetSystemNetworkModesIPC,			//1000065
    /** Set system network mode */
    ECustomSetSystemNetworkModeIPC,				//1000066
    /** Get current system network modes */
    ECustomGetCurrentSystemNetworkModesIPC,		//1000067
    /** Write WCDMA HomeZone */
    EWriteViagHomeZoneUHZIUESettingsIPC,		//1000068
    /** SetAlwaysOn Mode */
    ECustomSetAlwaysOnMode,						//1000069
    /** Notify Rau event */
    ECustomNotifyRauEventIPC,					//1000070
    /** Get Call forwarding info */
    ECustomGetIccCallForwardingStatusIPC,		//1000071
    /** Notify call forwarding info change */
    ECustomNotifyIccCallForwardingStatusChangeIPC,	//1000072
    /** Get 3G radio frequency band*/
    ECustomGetBandSelectionIPC,						//1000073
    /** Set 3G radio frequency band*/
    ECustomSetBandSelectionIPC,						//1000074
    /** Set 3G radio frequency band*/    
    ECustomGetServiceTableSupportbyApplicationIPC,	//1000075
    /* Notify remote alerting tone status change IPC */
    ECustomNotifyRemoteAlertingToneStatusChangeIPC, //1000076
    /** Maximum IPC enumeration value */
    ECustomMaxIpcLimit = KIpcCustomMaxLimi
    };

// FORWARD DECLARATIONS
class CMmCustomPtrHolder;

// CLASS DECLARATION

/**
 * RMmCustomAPI contains multimode API's vendor specific functionality.
 * RMmCustomAPI is a subsession extension i.e. it derives
 * from RtelSubSessionBase class.
 *
 * Notice that Custom TSY is able to handle only one instance of each
 * Custom API Set-method call at a time. Overlapping Set-method calls occurs
 * if same Set-method is called from an other instance of  RMmCustomAPI
 * object. Through one RMmCustomAPI object overlapping calls are not allowed
 * - trying this will panic Etel Server. For each overlapping request to the
 * client is returned error code KErrServerBusy either as a return value in
 * case of synchronous call or in a variable of type TRequestStatus in case of
 * an asynchronous call. This rule applies also in a situation where previous
 * identical request has been cancelled by client but the triggered operation
 * on DOS side has not been completed yet.
 *
 * On the other hand Custom TSY is able to handle several instances of each
 * Get-method call at a time. Again overlapping Get-method call can be
 * activated only from an other instance of  RMmCustomAPI object.
 *
 * Consider carefully consequences and implications of request cancellations.
 * If a request has triggered an operation on DOS side, the subsequent
 * calcellation of the request will not undo the operation on DOS side. It is
 * the responsibility of the Custom API's client to undo the operation if
 * necessary. Cancel request completes only when awaited DOS side response
 * arrives. Then if it is an OK-response, the request is completed normally
 * with error code KErrNone. . Or if the response is a FAILURE, the request is
 * completed with error code KErrCancel.
 *
 * APDU functionality:
 * Etel Server and Custom TSY are able to handle multiple SendAPDUReq calls
 * simultaneously.
 * However each RMmCustoAPI instance can have only one request active at
 * a time.
 *
 * Packet data related functionality:
 * Custom TSY implements functionality that is not supported by the packetAPI.
 *
 * Call realted functionality:
 * Custom TSY implements functionality that is not supported by the
 * MM ETEL API.
 *
 * @publishedPartner
 * @deprecated
 */
class RMmCustomAPI : public RTelSubSessionBase
    {
    public:

    /** Enumerates the Custom API version number.*/
    enum TMultimodeETelCustomV1Api
        {
        /** Version of Custom API extension */
        KETelCustomExtMultimodeV1=10000,
        /** Version of TTCustomCallParams */
        KETelCustomExtCustomCallParamsV3
        };

    /** Defines SS statuses to be used */
    enum TSsStatus
        {
        /** Active status */
        KSsStatusGsmActive = 0x01,
        /** Registered status */
        KSsStatusGsmRegistered = 0x02,
        /** Provisioned status */
        KSsStatusGsmProvisioned = 0x04,
        /** Quiescent status */
        KSsStatusGsmQuiescent = 0x08,
        /** Error status */
        KSsStatusReturnError = 0xFFFF,
        /** Network failure */
        KSsStatusNetworkFailure = 0xFFFE
        };


    /**
     * Maximum length of parameter data is 16 for Random parameter, SRES, AUTN,
     * CK, IK and AUTS
     */
    enum { KMaxParametersLength = 16 };
    /** Max length of ChipheringKey is 3 */
    enum { KMaxKCLength = 3 };
    /** Max length of RES is 4 */
    enum { KMaxRESLength = 4 };
    
    //Max length of iBTid, iKeyLifeTime, iNafId, iImpi, iMikey, iMtkSalt
    enum { KMaxAuthenticationParameterLength = 255 };
   
    // KMaxMikeyLength enum for maximum MIKEY protocol data length
	enum { KMaxMikeyLength = 255 };

	// KMaxKeyDomainIdLength enum for maximum key domain id length
	enum{ KMaxKeyDomainIdLength = 3 };

	// KMaxKeyGroupIdLength enum for maximum key group id length
	enum { KMaxKeyGroupIdLength = 2 };
	
	
    /** Parameter buffer */
    typedef TBuf8< KMaxParametersLength > TParameter;
	
	// GBA
    typedef TBuf8< KMaxAuthenticationParameterLength > TAuthenticationParameter;
    
	// TMikey datatype to be used for MIKEY data
    typedef TBuf8< KMaxMikeyLength > TMikey;
	
	// TKeyDomainId datatype to be used for key domain id information	
    typedef TBuf8< KMaxKeyDomainIdLength > TKeyDomainId;
	
	// TKeyGroupId datatype to be used for key group id information
    typedef TBuf8< KMaxKeyGroupIdLength > TKeyGroupId;

    // Datatype for parental rating data information
    typedef TBuf8< KMaxKeyGroupIdLength > TParentalRating;

    // Datatype for parental control operation information
    typedef TBuf8< KMaxKeyDomainIdLength > TParentalControl; 
    /**
     * Base class for TSimAuthenticationEapSim and
     * TSimAuthenticationEapAka 
     */
    class TSimAuthenticationBase
    {
    public:

    /** Possible values for the authentication method */
    enum
        {
        /** EAP-SIM authentication method */
        EEapSim = 0x01,
        /** EAP-AKA authentication method */
        EEapAka = 0x02,
        EGbaBootstrap = 0x03,
        EGbaBootstrapUpdate = 0x04, 
        EGbaBootstrapNafDerivation = 0x05, 
        EMgvMtkGeneration = 0x06, 
        EMgvMskDeletion = 0x07, 
        EMgvMskUpdate = 0x08 
        };

    /** Gets the value of iExtensionId 
     * @return iExtensionId */
    inline TInt ExtensionId() { return iExtensionId; };
    protected:
        TInt iExtensionId;
    };

// Class for holding GBA Bootstrap parameters
    class TSimAuthenticationGbaBootstrap : public TSimAuthenticationBase
        {
        public:

            IMPORT_C TSimAuthenticationGbaBootstrap ();

        public:

            // Random parameters from client
            TParameter iRandomParameters;
            // AUTN parameters from client
            TParameter iAUTN;
            // RES to client
            TParameter iRES;
            // AUTS to client
            TParameter iAUTS;
        };
    typedef TPckg< TSimAuthenticationGbaBootstrap > TGbaBootstrapDataPckg;


// Class for holding GBA Bootstrap updating parameters
    class TSimAuthenticationGbaBootstrapUpdate : public TSimAuthenticationBase
        {
        public:

            IMPORT_C TSimAuthenticationGbaBootstrapUpdate ();

        public:

            // B-TID from client 
            TAuthenticationParameter iBTid;
            // Key lifetime from client
            TAuthenticationParameter iKeyLifeTime;
            // Random parameters to client
            TParameter iRandomParameters;
        };
    typedef TPckg< TSimAuthenticationGbaBootstrapUpdate > TGbaBootstrapUpdateDataPckg;


// Class for holding GBA NAF Derivation parameters
    class TSimAuthenticationGbaNafDerivation : public TSimAuthenticationBase
        {
        public:

            IMPORT_C TSimAuthenticationGbaNafDerivation ();

        public:

            // Naf_id from client 
            TAuthenticationParameter iNafId;
            // Impi from client 
            TAuthenticationParameter iImpi;
            // Ks_ext_NAF to client 
            TBuf8 < 32 > iKsExtNaf;
        
        };
    typedef TPckg< TSimAuthenticationGbaNafDerivation > TGbaNafDerivationDataPckg;

      
// Class for holding MGV MSK update parameters
    class TSimAuthenticationMgvMskUpdate : public TSimAuthenticationBase
        {
        public:

            IMPORT_C TSimAuthenticationMgvMskUpdate ();

        public:

            // MIKEY to client 
            TMikey iMikey;
            // OMA BCAST management data object to client
            TParameter iBCASTManagement;
            // OMA BCAST SPE parental rating data operation to client
            TParentalRating iParentalRating;
            // Security Policy Extension to client
            TParameter iSecurityPolicyExt;
        
        };
     typedef TPckg< TSimAuthenticationMgvMskUpdate > TSimAuthenticationMgvMskUpdatePckg;
           
      
// Class for holding MGV MTK generation parameters
    class TSimAuthenticationMgvMtkGeneration : public TSimAuthenticationBase
        {
        public:

            IMPORT_C TSimAuthenticationMgvMtkGeneration ();

        public:

            // MIKEY from client 
            TMikey iMikey;
            // MTK salt to client 
            TMikey iMtkSalt;
        
            // Smartcard support for MobileTV
            // OMA BCAST management data object to client
            TParameter iBCASTManagement;
            // OMA BCAST SPE parental control operation to client
            TParentalControl iParentalControl;
            // TEK (Traffic Encryption Key) to client
            TParameter iTrafficEncryptionKey;   
        };
     typedef TPckg< TSimAuthenticationMgvMtkGeneration > TMgvMtkGenerationDataPckg;


// Class for holding MGV MSK deletion parameters
    class TSimAuthenticationMgvMskDeletion : public TSimAuthenticationBase
        {
        public:

            IMPORT_C TSimAuthenticationMgvMskDeletion ();

        public:

            // Key domain Id from client  
            TKeyDomainId iKeyDomainId;
            // Key Group Id from client 
            TKeyGroupId iKeyGroupIdPart;
        
        };
      typedef TPckg< TSimAuthenticationMgvMskDeletion > TMgvMskDeletionDataPckg;

      
    /**
    * Possible values for Multiple Subscriber Profile IDs
    */
    enum TMultipleSubscriberProfileID
        {
        /** Multiple Subscriber Profile ID 1 */
        KProfileIdentityOne = 0x01,
        /** Multiple Subscriber Profile ID 2 */
        KProfileIdentityTwo = 0x02,
        /** Multiple Subscriber Profile ID 3 */        
        KProfileIdentityThree = 0x03,
        /** Multiple Subscriber Profile ID 4 */        
        KProfileIdentityFour = 0x04,
        };

    /** Class for holding Call Forwarding parameters */
    class TCFIndicators: public RMobilePhone::TMobilePhoneCFUIndicatorV3
        {
        public:
        	/** Default constructor */
            IMPORT_C TCFIndicators();

        public:

	        /** CFU number . */ 
		    RMobilePhone::TMobileAddress iCFNumber; 
		 
		    /** MSP Profile ID */
       	    TMultipleSubscriberProfileID iMultipleSubscriberProfileID;
        };

    /** Package buffer to hold TCFIndicators objects */
    typedef TPckg<TCFIndicators> TCFIndicatorsPckg;

    /** Class for holding EAP SIM authentication parameters  */
    class TSimAuthenticationEapSim : public TSimAuthenticationBase
        {
    public:
    	/** Default constructor */
        IMPORT_C TSimAuthenticationEapSim();

    public:

        /** Random parameters from client */
        TParameter iRandomParameters;
        /** SRES to client */
        TParameter iSRES;
        /** Ciphering Key to Client */
        TParameter iKC;
        };

    /** Package buffer to hold TSimAuthenticationEapSim objects */
    typedef TPckg<TSimAuthenticationEapSim> TSimDataPckg;

    /** Class for holding EAP AKA authentication parameters */
    class TSimAuthenticationEapAka : public TSimAuthenticationBase
        {
    public:
    	/** Default constructor */
        IMPORT_C TSimAuthenticationEapAka();

    public:

        /** Random parameters from client */
        TParameter iRandomParameters;
        /** AUTN parameters from client */
        TParameter iAUTN;
        /** RES to client */
        TParameter iRES;
        /** UMTS Ciphering Key to client */
        TParameter iCK;
        /** UMTS Integrity Key to client */
        TParameter iIK;
        /** AUTS to client */
        TParameter iAUTS;
        };

    /** Package buffer to hold TSimAuthenticationEapAka objects */
    typedef TPckg<TSimAuthenticationEapAka> TAkaDataPckg;

    /**
     * TTApdu defines the pointers to the Send APDU request details
     */
    class TApdu
        {
    public:

        TApdu()
            {
            iInfo = 0;
            iData = 0;
            }

        TApdu
            (
            TDes8& aInfo,
            TDes8& aData
            )
            {
            iInfo = &aInfo;
            iData = &aData;
            }

    public:

        TDes8* iInfo; // Servicetype, Card Reader number, Application
                      // Type, Padding Byte
        TDes8* iData; // Transparent data
        };

    /**
     * TApduParameters 
     */
    class TApduParameters
        {
    public:

      TApduParameters()
            {
            iCardReaderId = 0;
            }

    public:  // Data
    	/** Card Reader ID */
        TUint8 iCardReaderId;
        /** Cmd Data */
        TApduBuffer iCmdData;
        /** RSP data */
        TApduBuffer iRspData;
        };

    /**
     * TSimCardReaderStatus
     */
    class TSimCardReaderStatus
        {
    public:

        /**
         * Default constructor
         */
        TSimCardReaderStatus() { iSimCardReaderStatus = 0; }

        /**
         * Constructor
         *
         * @param status Reader status
         * @return None
         */
        TSimCardReaderStatus( TUint8 aStatus )
            {
            iSimCardReaderStatus = aStatus;
            }

        /**
         * Set Card Status
         *
         * @param newStatus new reader status
         */
        IMPORT_C void SetStatus( TUint8 aNewStatus );

        /**
         * Get Card Status
         *
         * @return Reader status
         */
        IMPORT_C TUint8 GetStatus();

        /**
         * Get Card Reader ID.
         *
         * @return Reader ID
         */
        IMPORT_C TUint8 GetCardReaderID();

        /**
         * Is Card Reader Removable
         *
         * @return Is Card Reader Removable
         */
        IMPORT_C TBool IsCardReaderRemovable();

        /**
         * Is Card Reader Present
         *
         * @return Is Card Reader Present
         */
        IMPORT_C TBool IsCardReaderPresent();

        /**
         * Is Card Present
         *
         * @return Is Card Present
         */
        IMPORT_C TBool IsCardPresent();

        /**
         * Is Card Powered
         *
         * @return Is Card Powered
         */
        IMPORT_C TBool IsCardPowered();
        
    private:
    	TUint8 iSimCardReaderStatus; // Reader status
        };

    /** TLockNumber defines SIMLock types to be used */
    enum TLockNumber
        {
        /** Operator */
        EOperator = 1,
        /** GID1 */
        EGid1,
        /** GID2 */
        EGid2,
        /** IMSI */
        EImsi,
        /** Operator GID1 */
        EOperator_Gid1,
        /** Operator GID2 */
        EOperator_Gid2,
        /** Global unlock */
        EGlobalUnlock
        };

    /** Package buffer to hold TSimLockPassword objects */
    typedef TBuf<KSimLockMaxPasswordSize> TSimLockPassword;

    /** Constants for VIAG Home Zone data */
    enum TViagDataType
        {
        /** NBR size */
        KViagParamNbrSize = 15,
        /** Name size */
        KViagElemNameSize = 13,
        /** Element count */
        KViagElementCount = 4
        };

    /** VIAG parameter data structure */
    struct TViagParams
        {
        TBuf<KViagParamNbrSize> iSmsC;
        TBuf<KViagParamNbrSize> iScp;
        TUint8 iSubscribedZoneAndVersion;
        };

    /** Coordinates */
    struct TCoordinates
        {
        /** X */
        TUint32 iX;
        /** Y */
        TUint32 iY;
        /** R2 */
        TUint32 iR2;
        /** Zone ID */
        TUint iZoneId;
        };

    /** Active Flag values */
    enum TActiveFlag
        {
        /** Not active */
        ENotActive,
        /** Home Zone */
        EHomeZone,
        /** City Zone */
        ECityZone
        };

    /** VIAG Element data structure */
    struct TViagElement
        {
        /** Coordinates */
        TCoordinates iCoordinates;
        /** Name */
        TBuf<KViagElemNameSize> iName;
        /** Active flag */
        TActiveFlag iActiveFlag;
        };

    /** Array of TViagElement objects */
    typedef CArrayFixFlat <TViagElement> TViagElements;

    /** VIAG Cache Record ID data structure */
    struct TViagCacheRecordId
        {
        /** Cache ID */
        TInt iCacheId;
        /** Record ID */
        TInt iRecordId;
        };

    /** VIAG Cache Record Content data structure */ 
    struct TViagCacheRecordContent
        {
        /** LAC */
        TUint16 iLac;
        /** Cell ID */
        TUint16 iCellId;
        };

    /** VIAG UHZIUE settings data structure */
    struct TViagUHZIUESettings
        {
        /** Settings */
        TUint8 iSettings;
        /** Version */
        TUint8 iVersion;
        };

    /** Security code types */
    enum TSecurityCodeType
        {
        /** PIN1 code */
        ESecurityCodePin1,
        /** PIN2 code */
        ESecurityCodePin2,
        /** Universal PIN code */
        ESecurityUniversalPin,
        /** PUK1 code */
        ESecurityCodePuk1,
        /** PUK2 code */
        ESecurityCodePuk2,
        /** Phone SIM lock and phone device lock codes */
        ESecurityCodePassPhrase
        };

    /** Emergency number check modes */
    enum TCheckMode
        {
        /** Normal */
        EEmerNumberCheckNormal,
        /** Advanced */
        EEmerNumberCheckAdvanced
        };

    /** Caching status */
    enum TPndCacheStatus
        {
        /** Not ready */
        ECacheNotReady,
        /** Ready */
        ECacheReady,
        /** Not used */
        ECacheNotUsed,
        /** Failed */
        ECacheFailed
        };

    /** CB SIM Topics data type */
    struct TSimCbTopic
        {
        /** Number */
        TUint iNumber;
        /** Name */
        TBuf<KSimCbTopicNameLength> iName;
        };

    /** Buffer for telephone number */
    typedef TBuf<RMobilePhone::KMaxMobileTelNumberSize> TMobileTelNumber;

    /** Emergency number check mode data structure */
    struct TEmerNumberCheckMode
        {
        /** Check mode */
        TCheckMode iCheckMode;
        /** Telephone number */
        TMobileTelNumber iNumber;
        };

	// Service availability on service table on USIM 		
	struct TAppSupport
		{
		TInt iAppNum;	
		TBool iSupported;
		};

    public:
    	/** Default constructor */
        IMPORT_C RMmCustomAPI();

    public:

        /** DTMF Event types */
        enum TDtmfEventType
            {
            /** Manual */
            EDtmfManual,
            /** Sequence */
            EDtmfSequence
            };

        /** DTMF events */
        enum TDtmfEvent
            {
            /** Start */
            EDtmfStart,
            /** Stop */
            EDtmfStop,
            /** Abort */
            EDtmfAbort
            };

        /** DTMF information data structure */
        struct TDtmfInfo
            {
            /** Event type */
            TDtmfEventType iType;
            /** Event */
            TDtmfEvent iEvent;
            /** Tone */
            TChar iTone;
            };

        /** ALS block status values */
        enum TSetAlsBlock
            {
            /** Deactivate */
            EDeactivateBlock,
            /** Activate */
            EActivateBlock
            };

        /** Always On modes */
        enum TSetAlwaysOnMode
            {
            /** HPLMN */
            EAlwaysModeHPLMN,
            /** VPLMN */
            EAlwaysModeVPLMN,
            /** Both modes (HPLMN/VPLMN) */
            EAlwaysModeBoth,
            /** Neither mode */
            EAlwaysModeNeither
            };

        /** RAU event status values */
        enum TRauEventStatus
            {
            /** Active */
            ERauEventActive,
            /** Resumed */
            ERauEventResumed,
            };

        /** ALS block status values */
        enum TGetAlsBlockStatus
            {
            /** Not supported */
            EBlockStatusNotSupported,
            /** Active */
            EBlockStatusActive,
            /** Inactive */
            EBlockStatusInactive,
            /** Unknown */
            EBlockStatusUnknown
            };

        /** ALS Support values */
        enum TAlsSupport
            {
            /** ALS Support is On */
            EAlsSupportOn,
            /** ALS Support if Off */
            EAlsSupportOff
            };

        /** Remote alerting tone status */
        enum TRemoteAlertingToneStatus
            {
            /** No Tone */
            EUiNoTone,
            /** RBT Tone */
            EUiRbtTone,
            /** Stop Tone */
            EUiStopTone
            };

        /** Origin of the call */
        enum TCallOrigin
            {
            /** Unknown */
            EUnknown,
            /** ETel call */
            EETelCall,
            /** Outside */
            EOutsider
            };

        /** Status of the No Service Power Save state */
        enum TNspsStatus
            {
            /** No Service Power Save is ON */
            ENspsOn,
            /** No Service Power Save is OFF */
            ENspsOff
            };

        /** Ciphering Information data structure */
        struct TCipheringInfo
            {
            /** ciphering Indicator status */
            TBool   iIndStatus;
            /** ciphering status */
            TBool   iCiphStatus;
            };

        /** Additional info size */
        enum { KAdditionalInfoSize = 255 };

        /** Additional information data structure */
        struct TSsAdditionalInfo
            {
            /** Operation code */
            TUint8 iOperationCode;
            /** Additional information */
            TBuf8<KAdditionalInfoSize> iAdditionalInfo;
            };

        /** Operator name types */
        enum TOperatorNameType
            {
            /** NITZ Full */
            EOperatorNameNitzFull,
            /** NITZ Short */
            EOperatorNameNitzShort,
            /** Programmable UCS2 */
            EOperatorNameProgrammableUcs2,
            /** Programmable Latin */
            EOperatorNameProgrammableLatin,
            /** Hardcoded UCS2 */
            EOperatorNameHardcodedUcs2,
            /** Hardcoded Latin */
            EOperatorNameHardcodedLatin,
            /** Country MNC */
            EOperatorNameCountryMcn,
            /** MCC MNC */
            EOperatorNameMccMnc,
            /** Flexible PLMN */
            EOperatorNameFlexiblePlmn,
            /** Highest Priority */
            EOperatorNameHighestPriority
            };

        /** Operator name info structure */
        struct TOperatorNameInfo
            {
            /** Name type */
            TOperatorNameType iType;
            /** Name */
            TBuf<KMaxOperatorNameLength> iName;
            };

        /** Programmable operator Id */
        struct TOperatorId
            {
            /** MCC */
            TInt iMcc;
            /** MNC */
            TInt iMnc;
            };

        /** Programmable operator logo depth */
        enum TProgrammableOperatorLogoDepth
            {
            /** 1 bit per pixel */
            EBitPerPixel1,
            /** 2 bits per pixel */
            EBitPerPixel2,
            /** 4 bits per pixel */
            EBitPerPixel4,
            /** 8 bits per pixel */
            EBitPerPixel8
            };

        /** Programmable operator logo info */
        struct TProgrammableOperatorLogoInfo
            {
            /** Operator logo width */
            TInt iLogoWidth;
            /** Operator logo height */
            TInt iLogoHeight;
            /** Compression (yes/no) */
            TBool iCompression;
            /** Operator logo depth (bits per pixel) */
            TProgrammableOperatorLogoDepth iDepth;
            };

        /** Programmable operator logo structure */
        struct TOperatorLogo
            {
            /** Operator logo information */
            TProgrammableOperatorLogoInfo iLogoInfo;
            /** Operator logo buffer */
            TDes8* iOperatorLogo;
            };

        /** Two Digit Dial Support values */
        enum TTwoDigitDialSupport
            {
            /** On */
            ETwoDigitDialSupportOn,
            /** Off */
            ETwoDigitDialSupportOff
            };

        /** Custom call parameter class for calling party subaddress delivering */
        class TCustomCallParams : public RMobileCall::TMobileCallParamsV2
            {
        public:
        	/** Default constructor */
            IMPORT_C TCustomCallParams();

        public:

        	/** SubAddress */
            TBuf<KSubAddressSize> iSubAddress;
            /** Bearer */
            TBuf8<KBearerCapabilitySize> iBearer;
            };

        /** Package buffer for TCustomCallParams objects */
        typedef TPckg<TCustomCallParams> TCustomCallParamsPckg;

        /** Type of SS */
        enum TSsType
            {
            ESsAllSs = 0,
            ESsAllForwardings = 2,
            ESsForwUncond = 21,
            ESsForwBusy = 67,
            ESsForwNoReply = 61,
            ESsForwNoReach = 62,
            ESsAllBarrings = 330,
            ESsBarrAllOut = 33,
            ESsBarrOutInter = 331,
            ESsBarrOutInterExcHome = 332,
            ESsBarrAllIn = 35,
            ESsBarrAllInRoam = 351,
            ESsOutgoingBarrServ = 333,
            ESsIncomingBarrServ = 353,
            ESsCallWaiting = 43,
            ESsClip = 30,
            ESsClir = 31,
            ESsColp = 76,
            ESsColr = 77,
            ESsCnap = 300,
            ESsAllCondForwardings = 4,
            ESsRegPassword = 3
            };

        /** State of the SS feature */
        enum TSsMode
            {
            /** Active */
            ESsModeActive,
            /** Not active */
            ESsModeNotActive,
            /** Not registered */
            ESsModeNotRegistered,
            /** Not provisioned */
            ESsModeNotProvisioned,
            /** Not available */
            ESsModeNotAvailable,
            /** Unknown state */
            ESsModeUnknown
            };

        /** Forwarding mode */
        enum TSsForwMode
            {
            /** Incoming call is forwarded */
            ESsIncCallIsForw,
            ESsIncCallForwToC,
            ESsOutCallForwToC,
            /** Not Active */
            EForwNotActive
            };

        /** Hold mode */
        enum TSsHoldMode
            {
            /** Active */
            ESsHoldActive,
            /** Resume */
            ESsHoldResume,
            /** Not active */
            ESsHoldNotActive
            };

        /** Explicit call transfer (ECT) state */
        enum TSsEctState
            {
            /** ECT active */
            ESsEctActive,
            /** ECT alerting */
            ESsEctAlerting,
            /** ECT not active */
            ESsEctNotActive
            };

        /** Call Name Presentation choice */
        enum TSsChoice
            {
            /** Presentation allowed */
            ESsPresAllowed,
            /** Presentation restricted */
            ESsPresRestricted,
            /** Number not available */
            ESsNumberNotAvailable,
            /** Restricted address */
            ESsPresRestrictedAddress,
            /** Choice unknown */
            ESsChoiceUnknown
            };

        /** Remote Address Size */
        enum
        	{ 
            /** Remote Address Size */
        	KRemoteAddressSize = 64 
        	};

        /** SS type and mode */
        struct TSsTypeAndMode
            {
            /** SS Type */
            TSsType iSsType;
            /** SS Mode */
            TSsMode iSsMode;
            };

        /** SS info */
        struct TSsInfo
            {
            /** Forwarding mode */
            TSsForwMode iForwMode;
            TBool iCallWait;
            /** Hold mode */
            TSsHoldMode iCallHold;
            TBool iConfInd;
            /** CUG index */
            TUint16 iCugIndex;
            TBool iClirSuppReject;
            /** Explicit call transfer state */
            TSsEctState iEctCallState;
            /** Call name presentation choice */
            TSsChoice iChoice;
            /** Remote address */
            TBuf<KRemoteAddressSize> iRemoteAddress;
            };

        /** Gprs type info */
        enum TGprsInformationType
            {
            /** GPRS */
            EGprs = 0,
            /** EDGE GPRS */
            EEdgeGprs = 1
            };

        /** System network mode */
        enum TNetworkModeCaps
            {
            /** Unknown mode */
            KCapsNetworkModeUnknown = 0x00,
            /** GSM mode */
            KCapsNetworkModeGsm = 0x01,
            /** UMTS mode */
            KCapsNetworkModeUmts = 0x02,
            /** Dual mode (GPRS/UMTS) */
            KCapsNetworkModeDual = 0x04
            };

        /** Band Selection */
        enum TBandSelection
            {
            /** UMTS 850 */
            ENetworkBandUmts850 = 0x01,
            /** UMTS 2100 */
	        ENetworkBandUmts2100 = 0x02,
	        /** Any band */
	        ENetworkBandAny = 0x03
	        };
        
	    /** SIM Card status */
        enum TSIMCardStatus
            {
            /** Error */
            SimCardUnknowError = 0x00,
            /** Reset */
            SimCardResetPerformed,
            /** Not Accessible */
            SimCardNotAccessible,
            /** Removed */
            SimCardRemoved,
            /** Inserted */
            SimCardInserted,
            /** Recovered */
            SimCardRecovered
            };

        /** Drive mode status */
        enum TSetDriveMode
            {
            /** Drive Mode active */
            EDeactivateDriveMode,
            /** Drive Mode inactive */
            EActivateDriveMode
            };

        /** High speed channel status */
        enum THSxPAStatus
            {
            /** Disabled HSxPA */
            EHSxPADisabled,
            /** Enabled HSxPA */
            EHSxPAEnabled
            };

        /** Class to hold information about the EGPRS support in cell */
        class TGprsInformation
            {
        public:
        	/** Default constructor **/
            IMPORT_C TGprsInformation();

        public:
        	/** Type of GPRS network (GPRS or EDGE/GPRS)*/
            TGprsInformationType iGprsInfo;
            };

        /** Package buffer for TGprsInformation objects */
        typedef TPckg<TGprsInformation> TGprsInformationPckg;

        /** Package buffer for SIM File paths */
        typedef TBuf8<KSimFilePathLength> TSimFilePath;

        /** Holds SIM File Information */
        class TSimFileInfo
            {
        public:
        	/** Default constructor */
            IMPORT_C TSimFileInfo();

        public:
        	/** File path in SIM where file is read */
            TSimFilePath iPath;
            /** Offset from beginning of the file */
            TUint16 iOffSet;
            /** Amount of data to read */
            TUint16 iSize;
            };

        /** Package buffer for TSimFileInfo objects */
        typedef TPckg<TSimFileInfo> TSimFileInfoPckg;

        /** Holds lifetime information */
        class TLifeTimeData
            {
        public: // definitions

        	/** Lifetime data capability values */
            enum TLifeTimeDataCaps
                {
                /** Unknown */
                ELifeTimeDataCapsUnknown = 0x00000000,
                /** Lifetime */
                ELifeTimeDataCapsLifeTime = 0x00000001,
                /** Manufacturer Date */
                ELifeTimeDataCapsManufacturerDate = 0x00000002
                };

        public: // methods
        		
        	/** Default constructor */
            IMPORT_C TLifeTimeData();

        public: // internal attributes
        	/** Capabilities */
            TUint32 iCaps;
            /** Hours (range 0-999999) */
            TUint32 iHours;
            /** Minutes (range 0-59) */
            TUint8 iMinutes;
            /** Manufacturing date */
            TDateTime iManufacturingDate;
            };

        /** Package buffer for TLifeTimeData objects */
        typedef  TPckg<TLifeTimeData> TLifeTimeDataPckg;

        /** 3G Phonebook Information */
        struct T3GPBInfo
            {
            /** Max length of email */
            TInt16 iMaxLenEmail;
            /** Max length of Second Name */
            TInt16 iMaxLenSne;
            /** Max length of Additional Number */
            TInt16 iMaxLenAnr;
            /* Max length of Group Name */
            TInt16 iMaxLenGroupName;
            /* Max number of email fileds per entry */
            TInt8 iMaxNumEmail;
            /* Max number of Second Name fields per entry */
            TInt8 iMaxNumSne;
            /* Max number of Group Name fields per entry */
            TInt8 iMaxNumGroupName;
            // Max number of Additional Number fields per entry
            TInt8 iMaxNumAnr;
         };

        /** Cell Information */
        class TMmCellInfo
            {
            public: // definitions

            	/** Network modes */
                enum TNwMode
                    {
                    /** Unknown */
                    EUnknown,
                    /** GSM */
                    EGSM,
                    /** WCDMA */
                    EWCDMA
                    };

        /** WCDMA cell info */
        struct TFrequencyInfo
            {
            /** FDD: 0..16383 O */
            TInt iFddUL;
            /** FDD: 0..16383 M */
            TInt iFddDL;
            /** TDD: 0..16383 */
            TInt iTddNt;
            };

        /** FDD Information */
        struct TFddInfo
            {
            /** Primary CPICH (0..511) */
            TInt16 iPrimaryCPICH;
            /** CPICH ECN0 (0..63), max=49 */
            TInt8 iCpichEcN0;
            /** CPICH RSCP ((0..127), max=91 */
            TInt8 iCpichRscp;
            /** Path Loss (46..173), max=158 */
            TInt iPathloss;
            };

        /** TDD Information */
        struct TTddInfo
            {
            /** Cell Param ID (0..127) */
            TInt8 iCellParamID;
            /** Proposed TGSN (0..14) */
            TInt8 iProposedTGSN;
            /** Primary CCPCH RSCP (0..127) */
            TInt8 iPrimaryCcpchRscp;
            /** Pathloss (46..173), max=158 */
            TInt iPathloss;
            /** Timeslot ISCP (0..127), max.91 */
            TInt8 iTimeslotISCP[ KMaxTimeSlotIscpAmount ];
            };

        /** Cell Measured Result */
        struct TCellMeasuredResult
            {
            /** Cell Identity (0..268435455) */
            TInt32 iCID;
            /** FDD Information */
            TFddInfo iFddInfo;
            /** TDD Information */
            TTddInfo iTddInfo;
            };

        /** Network Measure Report */
        struct TNetworkMeasureReport
            {
            /** Frequency Information */
            TFrequencyInfo iFrequencyInfo;
            /** UTRA-CarrierRSSI (0..127), max=76 */
            TInt8 iCarrierRSSI;
            /** Cell measured result (1..32) */
            TCellMeasuredResult iCellMeasuredResult[
                KMaxCellMeasuredResults ];
            };

        /** WCDMA Cell Information */
        struct TWCDMACellInfo
            {
            /** Frequency information for current cell */
            TFrequencyInfo iFrequencyInfo;
            /** Scrambling Code for current cell (0..511) */
            TInt iPrimaryScrambilingCode;
            /** Network measure report (1..8) */
            TNetworkMeasureReport iNwkMeasureReport[
                KMaxNetworkMeasureReports ];
            };

        /** NMR Information */
        struct TNmr
            {
            /** ARFCN (initialised -1) */
            TInt iARFCN;
            /** BSIC (initialised -1) */
            TInt iBSIC;
            /** RxLEV (initialised -1) */
            TInt iRxLEV;
            };

        /** GSM cell information */
        struct TGSMCellInfo
            {
            /** NMR information on the cell */
            TNmr iNmr[ KMaxNmrAmount ];
            /** Timing advance (0.255) */
            TInt16 iTA;
            };

            public: //Methods

                /** Default constructor */
                IMPORT_C TMmCellInfo();

            public:  // Data

               /**
                * Network mode
                */
                TNwMode iMode; // used for identifying mode.

               /**
                * GSM cell info
                */
                TGSMCellInfo iGsmCellInfo;

               /**
                * WCDMA cell info
                */
                TWCDMACellInfo iWcdmaCellInfo;
            };

        /** Package buffer to hold TMmCellInfo objects */
		typedef TPckg<TMmCellInfo> TMmCellInfoPckg;

        public: // New functions

       /**
        * Opens a subsession.
        * @param aPhone reference to a Mobile Phone object
        * @return KErrNone if successful
        */
        IMPORT_C TInt Open(RMobilePhone& aPhone);

        /**
         * Closes the subsession.
         *
         */
        IMPORT_C void Close();

        /**
         * Sets Drive Mode status using the aModeStatus variable
         *
         * @param aStatus Request status
         * @param aModeStatus Drive mode status
         */
        IMPORT_C void SetDriveMode( TRequestStatus& aStatus,
            const TSetDriveMode aModeStatus );

        /**
         * Activate SIMLock synchronously
         *
         * @param aPassWord Lock specific password used to activate the SIMLock
         * @param aNumber Number of lock to be activated
         * @return Error value
         */
        IMPORT_C TInt ActivateSimLock( const TSimLockPassword& aPassWord,
            const TLockNumber aNumber ) const;

        /**
         * Activate SIMLock asynchronously
         *
         * @param aStatus Request status
         * @param aPassWord Lock specific password used to activate the SIMLock
         * @param aNumber Number of lock to be activated
         */
        IMPORT_C void ActivateSimLock( TRequestStatus& aStatus,
            const TSimLockPassword& aPassWord,
            const TLockNumber aNumber ) const;

        /**
         * Deactivate SIMLock synchronously
         *
         * @param aPassWord Lock specific password used to
         *        Deactivate the SIMLock
         * @param aNumber Number of lock to be activated
         * @return Error value
         */
        IMPORT_C TInt DeActivateSimLock( const TSimLockPassword& aPassWord,
            const TLockNumber aNumber ) const;

        /**
         * Deactivate SIMLock asynchronously
         *
         * @param aStatus Request status
         * @param aPassWord Lock specific password used
         *        to Deactivate the SIMLock
         * @param aNumber Number of lock to be activated
         */
        IMPORT_C void DeActivateSimLock( TRequestStatus& aStatus,
            const TSimLockPassword& aPassWord,
            const TLockNumber aNumber ) const;

        /**
         * Read VIAG Parameters synchronously
         *
         * @param aParam VIAG Home Zone Pararmeters
         * @param aElems VIAG Home Zone Elements
         * @return Error value
         */
        IMPORT_C TInt ReadViagHomeZoneParams( TViagParams& aParam,
            TViagElements& aElems ) const;

        /**
         * Read VIAG Parameters asynchronously
         *
         * @param aStatus Request status KErrNone if successful
         * @param aParam: VIAG Home Zone Pararmeters
         * @param aElems: VIAG Home Zone Elements
         */
        IMPORT_C void ReadViagHomeZoneParams( TRequestStatus& aStatus,
            TViagParams& aParam,
            TViagElements& aElems );

        /**
         * Read VIAG Cache asynchronously
         *
         * @param aId VIAG Home Zone Record Id to read from
         * @param aElems VIAG Home Zone Reacord Content
         * @return Error value
         */
        IMPORT_C TInt ReadViagHomeZoneCache( TViagCacheRecordId& aId,
            TViagCacheRecordContent& aRecord ) const;

        /**
         * Read VIAG Cache asynchronously
         *
         * @param aStatus Request status KErrNone if successful
         * @param aId VIAG Home Zone Record Id to read from
         * @param aElems VIAG Home Zone Reacord Content
         */
        IMPORT_C void ReadViagHomeZoneCache( TRequestStatus& aStatus,
            TViagCacheRecordId& aId,
            TViagCacheRecordContent& aRecord );

        /**
         * Write VIAG Cache asynchronously
         *
         * @param aId VIAG Home Zone Record Id to write to
         * @param aElems VIAG Home Zone Reacord Content
         * @return Error value
         */
        IMPORT_C TInt WriteViagHomeZoneCache( const TViagCacheRecordId& aId,
            const TViagCacheRecordContent& aRecord ) const;


        /**
        * Set the "always on" mode
        *
        * @param aMode Desired "always on" mode
        */
        IMPORT_C void SetAlwaysOn( TRequestStatus& aStatus,
                TSetAlwaysOnMode& aMode ) const;

        /**
        * Request notification of RAU event
        * @param aStatus reuquest status
        * @param aEventStatus RAU event status
        */
        IMPORT_C void NotifyRauEvent( TRequestStatus& aStatus,
                TRauEventStatus& aEventStatus );

        /**
         * Write VIAG Cache asynchronously
         *
         * @param aStatus: Request status KErrNone if successful
         * @param aId VIAG Home Zone Record Id to write to
         * @param aElems VIAG Home Zone Reacord Content
         */
        IMPORT_C void WriteViagHomeZoneCache( TRequestStatus& aStatus,
            const TViagCacheRecordId& aId,
            const TViagCacheRecordContent& aRecord );

        /**
         * Notify a DTMF event
         *
         * @param aStatus Request status
         * @param aInfo DTMF Info
         */
        IMPORT_C void NotifyDtmfEvent( TRequestStatus& aStatus,
            TDtmfInfo& aInfo );

        /**
         * Return diagnostics info
         *
         * @param aCallName call name
         * @return Error value
         */
        IMPORT_C TInt GetDiagnosticInfo( TName& aCallName ) const;

        /**
         * Sets the alternating line service block status using the
         * aBlockStatus variable
         *
         * @param aBlockStatus als block status
         * @return Error value
         */
        IMPORT_C TInt SetAlsBlocked( const TSetAlsBlock aBlockStatus ) const;

        /**
         * Sets the alternating line service block status using the aBlockStatus
         * variable
         *
         * @param aStatus Request status
         * @param aBlockStatus als block status
         */
        IMPORT_C void SetAlsBlocked( TRequestStatus& aStatus,
            const TSetAlsBlock aBlockStatus );

        /**
         * Gets the alternating line service block status
         *
         * @param aBlockStatus als block status
         * @return Error value
         */
        IMPORT_C TInt GetAlsBlocked( TGetAlsBlockStatus& aBlockStatus ) const;

        /**
         * Gets the alternating line service block status
         *
         * @param aStatus Request status
         * @param aBlockStatus als block status
         */
        IMPORT_C void GetAlsBlocked( TRequestStatus& aStatus,
            TGetAlsBlockStatus& aBlockStatus );

        /**
         * Reads the remote alerting tone status
         *
         * @param aToneStatus alerting tone status
         * @return Error value
         */
        IMPORT_C TInt GetRemoteAlertingToneStatus(
            TRemoteAlertingToneStatus& aToneStatus ) const;

        /**
         * Returns the origin of the call, defined by aCallName parameter
         *
         * @param aCallName call name
         * @param aOrigin call origin
         * @return Error value
         */
        IMPORT_C TInt CallOrigin( TName& aCallName,
            TCallOrigin& aOrigin ) const;

        /**
         * Terminates a call synchronously
         *
         * @param aCallName call name
         * @return Error value
         */
        IMPORT_C TInt TerminateCall( TName& aCallName ) const;

        /**
         * Terminates a call asynchronously
         *
         * @param aStatus Request status
         * @param aCallName call name
         */
        IMPORT_C void TerminateCall( TRequestStatus& aStatus,
            TName& aCallName );

        /**
         * Notifies a client of changed Als blocked status
         *
         * @param aStatus Request status
         * @param aBlockStatus ALS block status
         */
        IMPORT_C void NotifyAlsBlockedChanged( TRequestStatus& aStatus,
            TGetAlsBlockStatus& aBlockStatus );

        /**
         * Gets the ciphering information synchronously
         *
         * @param aInfo: Ciphering information
         * @return Error value
         */
        IMPORT_C TInt GetCipheringInfo( TCipheringInfo& aInfo );

        /**
         * Gets the ciphering info asynchronously
         *
         * @param aStatus Request status
         * @param aInfo Ciphering information
         */
        IMPORT_C void GetCipheringInfo( TRequestStatus& aStatus,
            TCipheringInfo& aInfo );

        /**
         * This function notifies a client of ciphering info change
         *
         * @param aStatus Request status
         * @param aInfo Ciphering information
         */
        IMPORT_C void NotifyCipheringInfoChange( TRequestStatus& aStatus,
            TCipheringInfo& aInfo );

        /**
         * Notifies a client of a change in NSPS (No Service Power Save)
         *
         * @param aStatus Request status
         * @param aNspsStatus status of the NSPS (on/off)
         */
        IMPORT_C void NotifyNSPSStatus( TRequestStatus& aStatus,
            TNspsStatus& aNspsStatus );

        /**
         * This function wake ups the net server from the NSPS state
         *
         * @param aStatus Request status
         */
        IMPORT_C void NetWakeup( TRequestStatus& aStatus );

        /**
         * Checks the status of the ALS in the product profile synchronously
         *
         * @param aSupport Als support Status
         * @return Error value
         */
        IMPORT_C TInt CheckAlsPpSupport( TAlsSupport& aSupport ) const;

        /**
         * Checks the status of the ALS in the product profile asynchronously
         *
         * @param aStatus Request status
         * @param aSupport Als support Status
         */
        IMPORT_C void CheckAlsPpSupport( TRequestStatus& aStatus,
            TAlsSupport& aSupport );

        /**
         * Gets network provider name synchronously
         *
         * @param aNetworkProviderName Network provider name
         * @return Error value
         */
        IMPORT_C TInt GetNetworkProviderName(
            TDes& aNetworkProviderName ) const;

        /**
         * Gets network provider name asynchronously
         *
         * @param aStatus Request status
         * @param aNetworkProviderName Network provider name
         */
        IMPORT_C void GetNetworkProviderName( TRequestStatus& aStatus,
            TDes& aNetworkProviderName );

        /**
         * This function retrieves syncronously the information
         * if the security code is blocked or not.
         *
         * @param aType Security code type
         * @param aIsBlocked Code blocking status
         * @return Error value
         */
        IMPORT_C TInt IsBlocked( TSecurityCodeType& aType,
            TBool& aIsBlocked );

        /**
         * This method activates syncronously the security code request and
         * triggers the security server to prompt PIN-code from client.
         *
         * @param aCode Security code type
         * @return Error value
         */
        IMPORT_C TInt CheckSecurityCode ( const TSecurityCodeType aCode );

        /**
         * This method activates asyncronously the security code request and
         * triggers the security server to prompt PIN-code from client.
         *
         * @param aStatus Request status
         * @param aCode Security code type
         */
        IMPORT_C void CheckSecurityCode ( TRequestStatus& aStatus,
            const TSecurityCodeType aCode );

        /**
         * This function retrieves syncronously the currently active PIN code
         *
         * @param aActivePin Currently active PIN
         */
        IMPORT_C void GetActivePin(
            RMobilePhone::TMobilePhoneSecurityCode& aActivePin );

        /**
         * Get the air time use duration synchronously
         *
         * @param aTime air time duration in seconds.
         * @return KErrNone
         */
        IMPORT_C TInt GetAirTimeDuration( TTimeIntervalSeconds& aTime ) const;

        /**
         * Checks the given number against emergency number list sychronously
         *
         * @param aNumMode Number and check mode
         * @param aResult Result of query
         * @return Error value
         */
        IMPORT_C TInt CheckEmergencyNumber( TEmerNumberCheckMode& aNumMode,
            TBool& aResult ) const;

        /**
         * Checks the given number against emergency number list asychronously
         *
         * @param aStatus Request status
         * @param aNumMode Number and check mode
         * @param aResult Result of query
         */
        IMPORT_C void CheckEmergencyNumber( TRequestStatus& aStatus,
            TEmerNumberCheckMode& aNumMode,
            TBool& aResult );

        /**
         * Clears Call Blacklist asynchronously
         *
         * @param aStatus Request status
         */
        IMPORT_C void ClearCallBlackList( TRequestStatus& aStatus );

        /**
         * Clears Call Blacklist synchronously
         *
         * @return Error value
         */
        IMPORT_C TInt ClearCallBlackList() const;

        /**
         * Notification request for additionalinfo message
         *
         * @param aStatus Request status
         * @param aSsAdditionalInfo SS additional information
         */
        IMPORT_C void SsAdditionalInfoNotification( TRequestStatus& aStatus,
            TSsAdditionalInfo& aSsAdditionalInfo );

        /**
         * Notify a client of the success of the sent SS command
         *
         * @param aStatus Request status
         * @param aSsStatus SS status
         */
        IMPORT_C void SsRequestCompleteNotification( TRequestStatus& aStatus,
            TInt& aSsStatus );

        /**
         * Notify a client when phonebook caching is ready
         *
         * @param aStatus Request status
         * @param aPndName Returns the name of phonebook having ready cache
         */
        IMPORT_C void NotifyPndCacheReady( TRequestStatus& aStatus,
            TName& aPndName );

        /**
         * Gets current cache status
         *
         * @param aStatus Request status
         * @param aPndStatus cache status
         * @param aPndName phonebook name
         */
        IMPORT_C void GetPndCacheStatus( TRequestStatus& aStatus,
            RMmCustomAPI::TPndCacheStatus& aPndStatus,
            const TName& aPndName ) const;

        /**
         * Gets the operator name of the current network
         *
         * @param aStatus Request status
         * @param aOperatorNameInfo Operator name info
         */
        IMPORT_C void GetOperatorName( TRequestStatus& aStatus,
            TOperatorNameInfo& aOperatorNameInfo );

        /**
         * Gets the programmable operator logo of the current network
         *
		 * This API is no longer supported!
		 *
         * @param aStatus Request status
         * @param aOperatorId operator Id
         * @param aLogo operator logo
         */
        IMPORT_C void GetProgrammableOperatorLogo( TRequestStatus& aStatus,
            TOperatorId& aOperatorId, TOperatorLogo& aLogo );

        /**
         * Notifies of the change in the operator logo of the current network
         *
         * @param aStatus Request status
         * @param aOperatorId Id of the operator
         */
        IMPORT_C void NotifyProgrammableOperatorLogoChange(
            TRequestStatus& aStatus,
            TOperatorId& aOperatorId );

        /**
         * Notifies a client of SAT refresh
         *
         * @param aStatus: Request status
         */
        IMPORT_C void SatRefreshCompleteNotification(
            TRequestStatus& aStatus );

        /**
         * Notifies the client about a network generated SS event
         *
         * @param aStatus Request status
         * @param aSsTypeAndMode Type and mode
         * @param aSsInfo SS info
         */
        IMPORT_C void NotifySsNetworkEvent( TRequestStatus& aStatus,
            TSsTypeAndMode& aSsTypeAndMode, TSsInfo& aSsInfo );

        /**
         * Cancels active ussd session synchronously
         *
         * @return Error value
         */
        IMPORT_C TInt CancelUssdSession() const;

        /**
         * Cancels active ussd session asynchronously
         *
         * @param aStatus Request status
         */
        IMPORT_C void CancelUssdSession( TRequestStatus& aStatus );

        /**
         * Checks two digit dial support synchronously
         *
         * @param aSupport two digit dial support status
         * @return Error value
         */
        IMPORT_C TInt CheckTwoDigitDialSupport(
            TTwoDigitDialSupport& aSupport ) const;

        /**
         * Checks two digit dial support asynchronously
         *
         * @param aStatus Request status
         * @param aSupport two digit dial support status
         */
        IMPORT_C void CheckTwoDigitDialSupport( TRequestStatus& aStatus,
            TTwoDigitDialSupport& aSupport );

        /**
         * This function resets the net server
         *
         */
        IMPORT_C void ResetNetServer() const;

        /**
         * This function resets the net server
         *
         * @param aStatus Request status
         */
        IMPORT_C void ResetNetServer( TRequestStatus& aStatus );

        /**
         * Releases CommDB file
         *
         * @param aStatus Request status
         * @param aFileName File to be released
         */
        IMPORT_C void ReleaseFile( TRequestStatus& aStatus,
            const TDesC& aFileName );

        /**
         * Restarts reading CommDB file
         *
         * @param aStatus Request status
         * @param aFileName File to be restarted
         */
        IMPORT_C void RestartFile( TRequestStatus& aStatus,
            const TDesC& aFileName );

        /**
         * Start Sim CB Topic Browsing
         *
         * @return TInt KErrNone if successful
         */
        IMPORT_C TInt StartSimCbTopicBrowsing();

        /**
         * Get next SIM CB Topic synchronously
         *
         * @param aSimCbTopic requested topic
         * @return KErrNone if successful
         */
        IMPORT_C TInt GetNextSimCbTopic( TSimCbTopic& aSimCbTopic );

        /**
         * Get next SIM CB Topic asynchronously
         *
         * @param aStatus request status
         * @param aSimCbTopic requested topic
         */
        IMPORT_C void GetNextSimCbTopic( TRequestStatus& aStatus,
            TSimCbTopic& aSimCbTopic );

        /**
         * Delete SIM CB Topic synchronously
         *
         * @param aSimCbTopicNumber topic to be deleted
         * @return Error value
         */
        IMPORT_C TInt DeleteSimCbTopic( const TUint &aSimCbTopicNumber );

        /**
         * Delete SIM CB Topic asynchronously. This asynchronous request can not be cancelled.
         *
         * @param aStatus request status
         * @param aSimCbTopic topic to be deleted
         */
        IMPORT_C void DeleteSimCbTopic( TRequestStatus& aStatus,
            const TUint &aSimCbTopicNumber );

        /**
         * Notifies of a network connection failure. This failure is so sever
         * that the only way to recover is to restart the phone.
         *
         * @param aStatus Request status
         */
        IMPORT_C void NotifyNetworkConnectionFailure(
            TRequestStatus& aStatus );

        /**
         * Send APDU request and wait for response synchronously
         *
         * @param anAPDUReq reference to structure containing APDU request's
         *        details
         * @return Error value
         */
        IMPORT_C TInt SendAPDUReq( TApdu& anAPDUReq );

        /**
         * Send APDU request asynchronously
         *
         * @param aStatus Request status
         * @param anAPDUReq reference to structure containing APDU request's
         *        details
         */
        IMPORT_C void SendAPDUReq( TRequestStatus& aStatus,
            TApdu& anAPDUReq );

        /**
         * Disables phone lock
         *
         * @param aStatus Request status
         * @param aVerifyCode Password for verification
         */
        IMPORT_C void DisablePhoneLock( TRequestStatus& aStatus,
                RMobilePhone::TMobilePassword& aVerifyCode ) const;

        /**
         * Notifies an EGPRS information change
         *
         * @param aStatus Request status
         * @param aGprsInfo is a reference to a TGprsInformation class
         *        packaged inside a TPckg<> class.
         */
        IMPORT_C void NotifyEGprsInfoChange( TRequestStatus& aStatus,
            TDes8& aGprsInfo ) const;

        /**
         * Gets EGPRS information
         *
         * @param aStatus Request status
         * @param aGprsInfo is a reference to a TGprsInformation class
         *        packaged inside a TPckg<> class.
         */
        IMPORT_C void GetEGprsInfo ( TRequestStatus& aStatus,
            TDes8& aGprsInfo ) const;

        /**
         * Reads information from a SIM file asynchronously
         *
         * @param aStatus Request status
         * @param aSimFileInfo reference to class containing SIM File's info
         * @param aResponseBytes reference to buffer in which store
         *        SIM File's data
         */
        IMPORT_C void ReadSimFile ( TRequestStatus& aStatus,
            TDes8& aSimFileInfo, TDes8& aResponseBytes ) const;

        /**
         * Gets the life time synchronously
         *
         * @param aLifeTimeInfo reference to class containing Life
         *          time information
         * @return TInt
         */
        IMPORT_C TInt GetLifeTime( TDes8& aLifeTimeInfo ) const;

        /**
         * Gets the life time asynchronously
         *
         * @param aStatus Request status
         * @param aLifeTimeInfo reference to class containing Life
         *          time information
         */
        IMPORT_C void GetLifeTime( TRequestStatus& aStatus,
            TDes8& aLifeTimeInfo ) const;

        /**
         * Gets phonebook 3G information
         *
         * @param aStatus Request status
         * @param aInfo reference to class containing phonebook 3G information
         */
        IMPORT_C void Get3GPBInfo( TRequestStatus& aStatus,
            T3GPBInfo& aInfo ) const;

        /**
         * This method reads all network system modes synchronously.
         *
         * @param aSupportedNetworkModes supported networkmodes
         * @return Error value
         */
        IMPORT_C TInt GetSystemNetworkModes(
            TUint32& aSupportedNetworkModes ) const;

        /**
         * This method reads all network system modes asynchronously.
         *
         * @param aStatus Request status
         * @param aSupportedNetworkModes supported networkmodes
         */
        IMPORT_C void GetSystemNetworkModes( TRequestStatus& aStatus,
            TUint32& aSupportedNetworkModes ) const;

        /**
         * This method sets network system mode synchronously.
         *
         * @param aNetworkMode system networkmode
         * @return KErrNone if successful
         */
        IMPORT_C TInt SetSystemNetworkMode(
            const TNetworkModeCaps aNetworkMode );

        /**
         * This method sets network system mode asynchronously.
         *
         * @param aStatus Request status
         * @param aSupportedNetworkModes supported networkmodes
         */
        IMPORT_C void SetSystemNetworkMode( TRequestStatus& aStatus,
            const TNetworkModeCaps aNetworkMode );

        /**
         * This method gets current network system mode synchronously.
         *
         * @param aCurrentNetworkModes Current networkmodes
         * @return Error value
         */
        IMPORT_C TInt GetCurrentSystemNetworkModes(
            TUint32& aCurrentNetworkModes ) const;

        /**
         * This method gets current system mode asynchronously.
         *
         * @param aStatus Request status
         * @param aCurrentNetworkModes Current networkmodes
         */
        IMPORT_C void GetCurrentSystemNetworkModes( TRequestStatus& aStatus,
            TUint32& aCurrentNetworkModes ) const;

        /**
	     * This method reads network system band synchronously.
	     *
	     * @param aSupportedNetworkBand supported networkband
	     * @return Error value
	     */
	    IMPORT_C TInt GetSystemNetworkBand(
	        TBandSelection& aSupportedNetworkBand,
	        TNetworkModeCaps& aNetworkMode ) const;
	        
	    /**
	     * This method reads network system band asynchronously.
	     *
	     * @param aStatus Request status
	     * @param aSupportedNetworkBands supported networkband
	     */
	    IMPORT_C void GetSystemNetworkBand( TRequestStatus& aStatus,
	        TBandSelection& aSupportedNetworkBand,
	        TNetworkModeCaps& aNetworkMode ) const;

	    /**
	     * This method sets network system band synchronously.
	     *
	     * @param aNetworkBand system networkband
	     * @return KErrNone if successful
	     */
	    IMPORT_C TInt SetSystemNetworkBand(
	        const TBandSelection aNetworkBand,
	        const TNetworkModeCaps aNetworkMode );
	    
	    /**
	     * This method sets network system band asynchronously.
	     *
	     * @param aStatus Request status
	     * @param aSupportedNetworkBand supported networkbands
         * @param aNetworkMode used network mode
	     */
	    IMPORT_C void SetSystemNetworkBand( TRequestStatus& aStatus,
	        const TBandSelection aNetworkBand,
	        const TNetworkModeCaps aNetworkMode );
        
        /**
         * Power SIM ON.
         *
         * @param aStatus Request status
         */
        IMPORT_C void PowerSimOn( TRequestStatus& aStatus );

        /**
         * Power SIM OFF.
         *
         * @param aStatus Request status
         */
        IMPORT_C void PowerSimOff( TRequestStatus& aStatus );

        /**
         * Perform Warm Reset to SIM.
         *
         * @param aStatus Request status
         */
        IMPORT_C void SimWarmReset( TRequestStatus& aStatus );

        /**
         * Send APDU Request to APDU server
         *
         * @param aStatus Request status
         * @param aParam Parameters for Apdu ( cardreaderid, commandApdu,
                 responseApdu )
         */
        IMPORT_C void SendAPDUReq( TRequestStatus& aStatus,
            TApduParameters& aParams );

        /**
         * Get Answer To Reset
         *
         * @param aStatus Request status
         * @param aATR reference containing Answer To Reset bytes from ICC
         */
        IMPORT_C void GetATR( TRequestStatus& aStatus, TDes8& aATR );

        /**
         * Get Card Reader Status
         *
         * @param aStatus Request status
         * @param aCardReaderStatus
         */
        IMPORT_C void GetSimCardReaderStatus( TRequestStatus& aStatus,
            TSimCardReaderStatus &aCardReaderStatus );

        /**
         * Notify SIM Card Status
         *
         * @param aStatus Request status
         * @param aCardStatus Card status
         */
        IMPORT_C void NotifySimCardStatus( TRequestStatus& aStatus,
            TSIMCardStatus& aCardStatus );

        /**
         * Get WLAN SIM Authentication Data
         *
         * @param aStatus Request status
         * @param aAuthenticationData Authentication Data
         */
        IMPORT_C void GetWlanSimAuthenticationData(
            TRequestStatus& aReqStatus,
            TDes8& aAuthenticationData ) const;

        /**
         * Sets "storage status" byte of a SIM-stored SMS message to "read".
         * Parameters are SM service center timestamp (as of 3GPP TS 23.040).
         *
         * @param aTime time
         * @param aTimezoneDiff timezone (difference to GMT, in quarters of
         *        an hour)
         */
        IMPORT_C void SetSimMessageStatusRead( TTime& aTime,
            TInt& aTimezoneDiff ) const;

        /**
         * This method writes specified VIAG Home Zone UHZIUE settings synchronously.
         *
         *
         * U-HZI-UE settings field is coded as follow:
         *
         *    -------------------------
         *    |b8|b7|b6|b5|b4|b3|b2|b1|
         *    -------------------------
         *                           <-0:The U-HZI-UE is deactivated
         *                            1:The U-HZI-UE is activated
         *
         *                        <----0:The CB-HZI-UE is deactivated
         *                             1:The CB-HZI-UE is activated
         *
         *    Constants RMmCustomAPI::KUHziUeActive and RMmCustomAPI::HCbHziUeActive
         *    can be used to determine and modificate states of different activation bits.
         *
         *    For example:
         *
         *    if ( iEfUHziUeSettings.settings & KUHziUeActive )
         *     then U-HZI-UE is activated
         *
         *    Activate:
         *    iEfUHziUeSettings.settings |= KUHziUeActive;
         *
         *    Deactivate:
         *    iEfUHziUeSettings.settings &= ~KUHziUeActive;
         *
         *
         * @param aSettings Settings
         * @return Error value
         */
        IMPORT_C TInt WriteViagHomeZoneUHZIUESettings(
            TViagUHZIUESettings& aSettings) const;

        /**
         * This method writes specified VIAG Home Zone UHZIUE settings
         * asynchronously.
         *
         * @param aStatus Request status
         * @param aSettings Settings
         */
        IMPORT_C void WriteViagHomeZoneUHZIUESettings (
            TRequestStatus& aStatus, TViagUHZIUESettings& aSettings) const;

        /**
         * Read HSxPA Status
         *
         * @param aStatus Request status
         * @param aHSxPAStatus
         */
        IMPORT_C void ReadHSxPAStatus( TRequestStatus& aStatus,
            THSxPAStatus& aHSxPAStatus );

        /**
         * Write HSxPA Status
         *
         * @param aStatus Request status
         * @param aHSxPAStatus
         */
        IMPORT_C void WriteHSxPAStatus( TRequestStatus& aStatus,
            THSxPAStatus& aHSxPAStatus );

        /**
         * Notify HSxPA Status change
         *
         * @param aStatus Request status
         * @param aHSxPAStatus
         */
        IMPORT_C void NotifyHSxPAStatus( TRequestStatus& aStatus,
            THSxPAStatus& aHSxPAStatus );

        /**
         * Get ICC call forwarding status
         *
         * @param aReqStatus Request status
         * @param aCFIndicators refrence to class CFIndicators
         */
        IMPORT_C void GetIccCallForwardingIndicatorStatus(
        	TRequestStatus& aReqStatus, //TUint8 aMspId,
        	TDes8& aCFIndicators ) const;

        /**
         * Notify ICC call forwarding status change
         *
         * @param aReqStatus Request status
         * @param aCFIndicators refrence to class CFIndicators
         */
        IMPORT_C void NotifyIccCallForwardingStatusChange(
            TRequestStatus& aReqStatus,
            TDes8& aCFIndicators );

        /**
         * Get GSM/WCDMA cell(s) info
         *
         * @param aStatus Request status
         * @param aCellInfo
         */
        IMPORT_C void GetCellInfo( TRequestStatus& aStatus,
            TDes8& aCellInfo ) const;

        /**
         * Notify Cell info change
         *
         * @param aStatus Request status
         * @param aCellInfo
         */
        IMPORT_C void NotifyCellInfoChange( TRequestStatus& aStatus,
            TDes8& aCellInfo ) const;


		/**
         * Get service table availability on USIM
         *
         * @since S60 v3.2
         * @param aStatus Request status
         * @param aParams aServiceNum. A service support in service table
         * specified by the client
         */
		IMPORT_C void GetUSIMServiceSupport( 
		    TRequestStatus& aStatus, TAppSupport& aParams ) const;
		
		/**
         * This function notifies a client of RemoteAlertingToneStatus change
         *
         * @param aStatus Request status
         * @param aToneStatus RemoteAlertingTone status
         */
        IMPORT_C void NotifyRemoteAlertingToneStatusChange( 
            TRequestStatus& aStatus, TRemoteAlertingToneStatus& aToneStatus );
	
    private:

        /**
         * Copy constructor
         *
         * @param aCustomAPI Custom API handle
         */
        RMmCustomAPI( const RMmCustomAPI& aCustomAPI );

    protected:

        /**
         * 2nd phase constructor.
         */
        IMPORT_C void ConstructL();

        /**
         * Destructor
         */
        IMPORT_C void Destruct();

    private:    // Data

        /*
         * Pointer to he Custom pointer holder
         * Own.
         */
        CMmCustomPtrHolder*  iCustomPtrHolder;
    };

/**
 * CMmCustomPtrHolder contains Custom Pointer Holder related functionality 
 *  @publishedPartner
 */
 class CMmCustomPtrHolder : public CBase
    {
    public: // Constructors and destructor

        static CMmCustomPtrHolder* NewL( const TInt aSizeOfPtrArray,
            const TInt aSizeOfPtrCArray=0 );

        ~CMmCustomPtrHolder();

        template <typename T> inline TPtr8& Set( TInt aSlot, T& aObject )
            {
            TPtr8& ptr=Ptr( aSlot );
            ptr.Set( REINTERPRET_CAST( TText8*, ( &aObject ) ), sizeof( T ),
                sizeof( T ) );

            return ptr;
            };

        template <typename T> inline TPtrC8& SetC(
            TInt aSlot, const T& aObject)
            {
            TPtrC8& ptr=PtrC( aSlot );
            ptr.Set( REINTERPRET_CAST( const TText8*, ( &aObject ) ),
                sizeof( T ) );

            return ptr;
            };

    protected:

        void ConstructL( const TInt aSizeOfPtrArray,
            const TInt aSizeOfPtrCArray );
        CMmCustomPtrHolder();

    public: // Data

        /*
         * Sim Topic
         */
        RMmCustomAPI::TSimCbTopic iSimCbTopic;

        /*
         * SimTopic number
         */
        TUint iSimCbTopicNumber;

        /*
         * Als block status for set
         */
        RMmCustomAPI::TSetAlsBlock iSetAlsBlock;

        /*
         * Drive Mode
         */
        RMmCustomAPI::TSetDriveMode iSetDriveMode;

        /*
         * Sim lock number
         */
        RMmCustomAPI::TLockNumber iSimLockNumber;

        /*
         * Sim lock password
         */
        RMmCustomAPI::TSimLockPassword iSimLockPassword;

        /*
         * Viag cache id
         */
        RMmCustomAPI::TViagCacheRecordId iViagCacheId;

        /*
         * Viag cache record
         */
        RMmCustomAPI::TViagCacheRecordContent iViagCacheRecord;

        /*
         * Security code typeiSecurityCodeType;
         */
        RMmCustomAPI::TSecurityCodeType iSecurityCodeType;

        /*
         * Operator name type
         */
        RMmCustomAPI::TOperatorNameType iOperatorNameType;

        /*
         * Phonebook 3G info
         */
        RMmCustomAPI::T3GPBInfo i3GPBInfo;

        /*
         * Supported network modes
         */
        TUint32 iSupportedNetworkModes;

        /*
         * System network mode
         */
        RMmCustomAPI::TNetworkModeCaps iNetworkModeCaps;

        /*
         * Current network modes
         */
        TUint32 iCurrentNetworkModes;
        
        /**
         * Network UMTS rf band
         */
        RMmCustomAPI::TBandSelection iNetworkBand;
    
	    /**
	     * Supported network band
	     */
	    RMmCustomAPI::TBandSelection iSupportedNetworkBand;

        /**
         * HSxPA Status
         */
        RMmCustomAPI::THSxPAStatus iHSxPAStatus;

    protected: // Data

        RArray<TPtr8> iPtrArray;
        RArray<TPtrC8> iPtrCArray;

    private: // Data

        TPtr8& Ptr( TInt aIndex );
        TPtrC8& PtrC( TInt aIndex );

    };

#endif      // RMMCUSTOMAPI_H

//  End of File
