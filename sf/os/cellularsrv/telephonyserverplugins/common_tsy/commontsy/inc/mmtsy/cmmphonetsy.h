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



#ifndef CMMPHONETSY_H
#define CMMPHONETSY_H

//  INCLUDES
#include <etelmm.h>
#include <et_phone.h>
#include <mmretrieve.h>
#include <pcktcs.h>
#include <e32property.h>
#include <centralrepository.h>
#include <ctsy/mltsyfactorybase.h>
#include "cmmmessagemanagerbase.h"
#include "MmTsy_timeoutdefs.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include "CMmPrivateUtility.h"
#include <ctsy/serviceapi/ctsydomainpskeys.h>
#include <ctsy/serviceapi/mctsysatservice.h>
#include <ctsy/pluginapi/mtelephonyaudiocontrolcallback.h>
#include <hwrmpower.h>
#include "BatteryInfoObserver.h"
#include "ChargingStatusObserver.h"

// FORWARD DECLARATIONS
#ifdef REQHANDLE_TIMER
class CResponseTimer;
class CResponseTimerStore;
#endif
class CMmConferenceCallTsy;
class CMmSmsTsy;
class CMmBroadcastTsy;
class CMmONStoreTsy;
class CMmENStoreTsy;
class CMmPhoneBookStoreTsy;
class CStorageInfoData;
class CTsySatMessagingBase;
class CMmUssdTsy;
class CMmCustomTsy;
class CMmPacketServiceTsy;

class CMmPhoneExtInterface;
class CMmTsyReqHandleStore;
class CMmCallList;
class CMmLineList;
class CMmPBList;

class CMmCallTsy;
class CHeartbeatRunner;
class CMmNetTsy;
class CMmDtmfTsy;
class CMmSupplServTsy;
class CMmSecurityTsy;
class CMmCustomSecurityTsy;

class CMmPhoneFactoryTsy;
class CMmDataPackage;

class MTelephonyAudioControl;

class CCtsySystemStatePluginHandler;

// CLASS DECLARATION

/**
 *  CMmPhoneTsy contains mode-independent phone functionality.
 *  Extension request handles and parameters are stored as attributes.
 */
 
NONSHARABLE_CLASS( CMmPhoneTsy ) : public CPhoneBase,
                                   public MTelephonyAudioControlCallback,
                                   public MBatteryInfoNotifier,
                                   public MChargingStatusNotifier
    {
    
    public: // Friend classes

        /** Classes which also implement RMobilePhone functionality
            are declared as friends */
            
        friend class CMmNetTsy;
        friend class CMmDtmfTsy;
        friend class CMmSupplServTsy;
        friend class CMmSecurityTsy;
        
    public: // Enumerations

        /** Phone request types*/
        
        enum TPhoneRequestTypes
            {
            EMultimodePhoneReqHandleUnknown, //0
            EMultimodePhoneNotifyModemDetected,
            EMultimodePhoneCapsChangeNotification,
            EMultimodePhoneNotifyDTMFCaps,
            EMultimodePhoneSendDTMFTones,
            EMultimodePhoneStartDTMFTone, //5
            EMultimodePhoneStopDTMFTone,
            EMultimodePhoneSendNetworkServiceRequest,
            EMultimodePhoneGetDetectedNetworksPhase1, 
            EMultimodePhoneNotifyNetworkRegistrationStatusChange, 
            EMultimodePhoneNotifyModeChange, //10
            EMultimodePhoneNotifyCurrentNetworkChange, 
            EMultimodePhoneNotifySignalStrengthChange, 
            EMultimodePhoneNotifyNetworkTimeInfoChange, 
            EMultimodePhoneNotifyNWSelectionSettingChange, 
            EMultimodePhoneNotifyNWTimeInfoChange, //15
            EMultimodePhoneSelectNetwork, 
            EMultimodePhoneSetNWSelectionSetting, 
            EMultimodePhoneSetCallForwardingStatus, 
            EMultimodePhoneGetIdentityServiceStatus,
            EMultimodePhoneSetCallBarringStatus, //20
            EMultimodePhoneSetSSPassword,
            EMultimodePhoneSetCallWaitingStatus,
            EMultimodePhoneSetFdnSetting,
            EMultimodePhoneGetFdnStatus,
            EMultimodePhoneNotifyCallForwardingStatusChange,
            EMultimodePhoneNotifyCallBarringStatusChange,
            EMultimodePhoneNotifyCallWaitingStatusChange,
            EMultimodePhoneNotifyAlsLineChange, 
            EMultimodePhoneNotifyFdnStatusChange, 
            EMultimodePhoneGetCallForwardingStatusPhase1,
            EMultimodePhoneGetCallBarringStatusPhase1,
            EMultimodePhoneGetCallWaitingStatusPhase1, 
            EMultimodePhoneNotifySecurityCapsChange,
            EMultimodePhoneGetLockInfo, 
            EMultimodePhoneNotifyLockInfoChange,
            EMultimodePhoneSetLockSetting,
            EMultimodePhoneChangeSecurityCode,
            EMultimodePhoneNotifySecurityEvent,
            EMultimodePhoneVerifySecurityCode,
            EMultimodePhoneAbortSecurityCode,
            EMultimodePhoneNotifyBatteryInfoChange,
            EMultimodePhoneNotifyStopInDTMFString,
            EMultimodePhoneNotifyCallServiceCapsChange,
            EMultimodePhoneNotifyCallForwardingActive,
            EMultimodePhoneNotifyIccAccessCapsChange,
            EMultimodePhoneGetCustomerServiceProfile,
            EMultimodePhoneGetServiceTable, 
            EMultimodePhoneGetPhoneStoreInfo, 
            EMultimodePhoneOutOfMemory, 
            EMultimodePhoneGetAlsLine,
            EMultimodePhoneSetAlsLine,
            EMultimodePhoneGetNITZInfo,
            EMultimodePhoneGetCellInfo,
            EMultimodePhoneNotifyCellInfoChange,
            EMultimodePhoneGetServiceProviderName,
            EMultimodePhoneGetNetworkRegistrationStatus,
            EMultimodePhoneGetSubscriberId,
            EMultimodePhoneNotifyNetworkSecurityLevelChange,
            EMultimodePhoneGetCipheringIndicatorStatus,
            EMultimodePhoneGetBatteryInfo,
            EMultimodePhoneGetPhoneId,
            EMultimodePhoneNotifyCurrentNetworkNoLocationChange, 
            EMultimodePhoneNotifyIndicatorChange,
            EMultimodePhoneGetAuthorizationInfoPhase1,
            EMultimodePhoneIMSAuthenticate,
            EMultimodePhoneGetMailboxNumbers,
            EMultimodePhoneEnumerateAPNEntries,
            EMultimodePhoneGetAPNname,    
            EMultimodePhoneGetIccMessageWaitingIndicators,
            EMultimodePhoneSetIccMessageWaitingIndicators,
            EMultimodePhoneNotifyIccMessageWaitingIndicatorsChange,
            EMultimodePhoneGetSecurityCodeInfoPin1,
            EMultimodePhoneGetSecurityCodeInfoPin2,
            EMultimodePhoneGetSecurityCodeInfoPuk1,
            EMultimodePhoneGetSecurityCodeInfoPuk2,
            EMultimodePhoneGetSecurityCodeInfoPhonePassword,
            EMultimodePhoneGetSecurityCodeInfoSPC,
            EMultimodePhoneGetSecurityCodeInfoPhonebookHiddenKey,
            EMultimodePhoneGetSecurityCodeInfoUSIMAppPin,
            EMultimodePhoneGetSecurityCodeInfoSecondUSIMAppPin,
            EMultimodePhoneGetSecurityCodeInfoUniversalPin,
            EMultimodePhoneGetSecurityCodeInfoUniversalPuk,
            EMultimodePhoneNotifySecurityCodeInfoChange,
            EMultimodePhoneAppendAPNName,
            EMultimodePhoneDeleteAPNName,
            EMultimodePhoneNotifyAPNListChange,
            EMultimodePhoneSetAPNControlListServiceStatus,
            EMultimodePhoneGetAPNControlListServiceStatus,
            EMultimodePhoneNotifyAPNControlListServiceStatusChange,
            EMultimodePhoneSendNetworkServiceRequestNoFdnCheck,
            EMultimodePhoneNotifySendNetworkServiceRequest,
            EMultimodePhoneNotifyAllSendNetworkServiceRequest,
            EMultimodePhoneGetCurrentActiveUSimApplication,             
            EMultimodePhoneTerminateAllCalls,
            EMultimodePhoneTerminateActiveCalls,
            EMultimodePhoneNotifyDtmfEvent, 
            // ATTENTION:: Declare constant for those requests that need
            // own request handle record in iTsyReqHandleStore above the
            // following!
            EMultimodePhoneMaxNumOfRequests
            };

        struct TNosBootState
            {
                TBool iSIMReady;
                TBool iADNReady;
                TBool iSecReady;
                TBool iCachingActive;
            };
            
        /**
        * Every KLifeTimeUpdateInterval sec of the call, life time duration param is updated
        */
        enum { KLifeTimeUpdateInterval = 10	};                    
  
     public: // From MTelephonyAudioControlCallback
    
        /**
         * Informs errors from the Telephony Audio Control
         *
         * @param aCallName Call where the error happened
         * @param aError The error code
         */
        void NotifyTelephonyAudioControlError( const TName& aCallName, 
                                               const TInt& aError );
        
        /**
         * This method returns the information when modem status is ready
         *
         * @return ETrue if the modem is ready
         */
         TBool IsModemStatusReady() const;
         
         CCtsySystemStatePluginHandler* SystemStatePluginHandler()
            {
             return iSystemStatePluginHandler;
            }
    private:
		  /** used for queuing GetServiceTable requests */
		  struct TServiceTableRequests
		      {
		      /** ETel request handle */
		      TTsyReqHandle iReqHandle;
		      /** Table to be cached */ 
		      RMobilePhone::TMobilePhoneServiceTable* iTable;
		      /** Table to be cached */
		      RMobilePhone::TMobilePhoneServiceTableV1* iTableData;	
		      };
		
          /** used for queuing GetSubscriberId requests */
          struct TSubscriberIdRequest
               {
               /** ETel request handle */
               TTsyReqHandle iReqHandle; 
               /** pointer to client-side    data*/
               RMobilePhone::TMobilePhoneSubscriberId* iRetSubscrId; 
               };
          
          /** used for queuing GetPhoneId requests */
          struct TGetPhoneIdRequest
               {
               /** ETel request handle */
               TTsyReqHandle iReqHandle; 
               /** pointer to client-side    data*/
               RMobilePhone::TMobilePhoneIdentityV1* iRetPhoneId; 
               };
               
          /**Used for queing GetServiceProviderName requests */       
          struct TServiceProviderNameRequest
               {
               /** ETel request handle */
               TTsyReqHandle iReqHandle;
               /** pointer to client side data */
               RMobilePhone::TMobilePhoneServiceProviderNameV2*
                   iServiceProviderName;     
               };

          /** Service table request types */
          enum TServiceTableRequest
               {
               ENoRequest,
               ESIMRequest,
               EUSIMRequest
               };

          struct TCustomerServiceProfileIdRequest
               {
               /** ETel request handle */
               TTsyReqHandle iReqHandle; 
               /** pointer to client-side    data */
               RMobilePhone::TMobilePhoneCspFileV1* iRetCspFile; 
               };
            
          struct TGetServiceTableRequest
               {
               /** ETel request handle */
               TTsyReqHandle iReqHandle;
               /** Table to be cached */ 
               RMobilePhone::TMobilePhoneServiceTable* iTable;
               /** Table to be cached */
               RMobilePhone::TMobilePhoneServiceTableV1* iTableData; 
               //** Cache done for this req*/
               TBool iCaching;
               };
               
          struct TGetAlsLineRequest
               {
               /** ETel request handle */
               TTsyReqHandle iReqHandle; 
               /** pointer to client-side    data */
               RMobilePhone::TMobilePhoneALSLine* iALSLine; 
               };

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         *          
         *
         * @param aMessageManager: Pointer to the message manager. The returned 
         * 		CMmPhoneTsy takes ownership of this parameter.
         * @param aMmPhoneFactoryTsy Pointer to the phone factory (which is needed
         * 		for Sim Atk TSY creation). The returned CMmPhoneTsy does not take
         * 		ownership of this parameter. 
         * @param aFactory Pointer Ltsy factory base. The returned CMmPhoneTsy
         * 		takes ownership of this parameter.
         * return created phone object 
         */
        IMPORT_C static CMmPhoneTsy* NewL(         
            CMmMessageManagerBase* aMessageManager,
            CMmPhoneFactoryTsy* aMmPhoneFactoryTsy,
            MLtsyFactoryBase* aFactory );

        /**
         * Destructor.
         */
        virtual ~CMmPhoneTsy();

        // New functions

        /**
         * Initialisation method that is called from ETel Server.
         *          
         *
         */
        virtual void Init();

        /**
         * Creates new object by name and returns a pointer to it.
         *          
         *
         * @param aNewName 
         * return pointer to the created object
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& aName );
    
        /**
         * Creates new Line object and returns a pointer to it. All 
         * objects opened from the phone are opened by name, hence 
         * this method is NOT SUPPORTED.
         *          
         *
         * @param aNewName 
         * return pointer to the created object
         */
        virtual CTelObject* OpenNewObjectL( TDes& aNewName );

        /**
         * Dispatches multimode API functions calls
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aIpc request IPC number
         * @param aPackage packaged request parameters
         * @return result of the request
         */
        virtual TInt ExtFunc( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Returns request mode for given IPC.
         *          
         *
         * @param aIpc IPC number
         * @return request mode
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );
    
        /**
         * Returns number of slots to be used for given IPC
         *          
         *
         * @param aIpc request IPC number
         * @return result of the request
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Cancels request that's IPC number and request handle are given in
         * parameters
         *          
         *
         * @param aIpc request IPC number
         * @param aTsyReqHandle TSY request handle
         * @return result of the request
         */
        virtual TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );
    
        /**
         * Fills the given pointer with phone information (core)
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aPhoneInfo phone information
         * @return Return value to the ETel Server
         */
        virtual TInt GetInfo( const TTsyReqHandle aTsyReqHandle, 
            RPhone::TPhoneInfo* aPhoneInfo );
    
        /**
         * Request for caps change notifications
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCaps phone caps
         * @return Return value to the ETel Server
         */
        virtual TInt NotifyCapsChange( const TTsyReqHandle aTsyReqHandle, 
            RPhone::TCaps* aCaps );
    
        /**
         * Cancels caps change notifications
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        virtual TInt NotifyCapsChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );
    
        /**
         * Controlled initialisation
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        virtual TInt ControlledInitialisation( 
            const TTsyReqHandle aTsyReqHandle );
    
        /**
         * Cancels controlled initialisation
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        virtual TInt ControlledInitialisationCancel( 
            const TTsyReqHandle aTsyReqHandle );
    
        /**
         * Request for modem detected notifications.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aDetection Ptr to TModemDetection 
         * object
         * @return KErrNone
         */
        virtual TInt NotifyModemDetected( const TTsyReqHandle aTsyReqHandle, 
            RPhone::TModemDetection* aDetection );
        
        /**
         * Cancels modem detected notifications.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        virtual TInt NotifyModemDetectedCancel( 
            const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Fills the given pointer with phone capabilities (core)
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCaps phone capabities
         * @return Return value to the ETel Server
         */
        virtual TInt GetCaps( const TTsyReqHandle aTsyReqHandle, 
            RPhone::TCaps *aCaps );

        /**
         * Fills the given pointer with phone status (core)
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aStatus phone status
         * @return Return value to the ETel Server
         */
        virtual TInt GetStatus( const TTsyReqHandle aTsyReqHandle, 
            RPhone::TStatus *aStatus );
    
        /**
         * Fills the given pointer with maximum number of supported lines
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCount line count
         * @return Return value to the ETel Server
         */
        virtual TInt EnumerateLines( const TTsyReqHandle aTsyReqHandle, 
            TInt *aCount );
    
        /**
         * Fills the given pointer with line information
         *          
         *
         * @param aTsyReqHandle: TSY request handle
         * @param aLineInfo: line information
         * @return TInt: Return value to the ETel Server
         */
        virtual TInt GetLineInfo(  const TTsyReqHandle aTsyReqHandle, 
          TLineInfoIndex *aLineInfo );

        /**
         * Register given notification
         *          
         *
         * @param aIpc IPC number
         * @return Return value to the ETel Server
         */
        virtual TInt RegisterNotification( const TInt aIpc );
    
        /**
         * Deregister given notification
         *          
         *
         * @param aIpc IPC number
         * @return Return value to the ETel Server
         */
        virtual TInt DeregisterNotification( const TInt aIpc );
        
        /**
         * Completes caps change notification
         *          
         *
         * @param None
         */
        void CompleteNotifyCapsChange();

        /**
         * Complete the client request when modem has been detected.
         *          
         *
         * @param aModemStatus Modem status
         */
        virtual void CompleteNotifyModemDetected( 
            RPhone::TStatus aModemStatus );

        /**
         * Returns the active phone extension
         *          
         *
         * @return pointer to the active phone extension
         */
        virtual CMmPhoneExtInterface* GetActivePhoneExtension();

        /**
         * Returns a pointer to the call list
         *          
         *
         * @return pointer to call list
         */
        virtual CMmCallList* CallList();

        /**
         * Returns a pointer to the line list
         *          
         *
         * @return pointer to line list
         */
        virtual CMmLineList* LineList();

        /**
         * Returns a pointer to the line list
         * 
         *                  
         * @return pointer to phonebook list
         */
        virtual CMmPBList* PBList();

        /**
         * Sets pointer to the conference call
         *          
         *
         * @param aMmConferenceCallTsy pointer to CC object
         */
        virtual void SetConferenceCall(
            CMmConferenceCallTsy* aMmConferenceCallTsy );

        /**
         * Returns pointer to the conference call
         *          
         *
         * @return pointer to conference call
         */
        virtual CMmConferenceCallTsy* ConferenceCall();
       
        /**
         * Notifies client about ALS line change
         *          
         *
         */
        virtual void CompleteNotifyALSLineChange( );

        /**
         * Completes the fdn setting request
         *          
         *
         * @param aErrorCode error/success code
         */
        virtual void CompleteSetFdnSettingL( TInt aErrorCode );
        
        /**
         * Notifies client about fdn status change
         *          
         *
         */
        virtual void CompleteNotifyFdnStatusChange( );

        /**
         * Completes the fdn status request
         *          
         *
         * @param aErrorCode error/success code
         */
        virtual void CompleteGetFdnStatus( CMmDataPackage* aDataPackage,
            TInt aErrorCode );

        /**
         * Completes client GetSubcriberId request
         *          
         *
         * @param aImsiData IMSI code
         * @param aError error/success code
         */
        virtual void CompleteGetSubscriberIdL( 
            TBuf8<RMobilePhone::KIMSISize> aImsiData,
            TInt aError );

        /**
         * Completes initialization of the phone
         *          
         *
         */
        virtual void CompleteNotifyModemStatusReady();

        /**
         * Initializes SIM dependent parts
         *          
         *
         */
        virtual void CompleteNotifySimStatusReadyL();

        /**
         * Sets the boot security flag iSecReady to ETrue
         *          
         *
         */
        void CompleteNotifySecurityReady();

        /**
         * Initializes Phonebook caching, this happens after SIM is ready
         *          
         *
         */
        void StartPBCachingL();

        /**
         * Returns current DOS bootstate
         *          
         *
         * @return DOS boot state
         */
        virtual CMmPhoneTsy::TNosBootState* NosBootState();

        /**
         * Returns current Phonebook state
         *          
         *
         * @return pointer to pb state -class
         */
        virtual CStorageInfoData* PhoneBookState();


        /**
         * Returns a pointer to active phone extension.
         *          
         *
         * @return pointer to requested object
         */
        CMmPhoneExtInterface* PhoneExtInterface();

        /**
         * Sets iPhoneIdentitys iSerialNumber
         *          
         *
         * @param aSerialData serial number
         * @param aError error code
         */
        virtual void CompleteGetPhoneId( 
            TBuf8<KSerialNumberLength> aSerialData, 
            TInt aError );
            
        /**
         * Cancels get phone id request 
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Result of the request
         */
        TInt MobilePhoneGetPhoneIdCancel( const TTsyReqHandle aTsyReqHandle );            

        /**
         * Returns pointer to the message manager
         *          
         *
         * @return pointer to the requested object
         */
        virtual CMmMessageManagerBase* MessageManager();

        /**
         * Returns pointer to the SMS session
         *          
         *
         * @param None
         * @return pointer to the SmsSession
         */
        virtual CMmSmsTsy* SmsSession();

        /**
         * Returns pointer to the SMS service as a MCtsySatService.
         *          
         *
         * @return pointer to the SmsService
         */
        virtual MCtsySatService& SmsService();
        
        /**
         * Set SMS session
         *          
         *
         * @param aMmSmsTsy
         */
        virtual void SetSmsSession( CMmSmsTsy* aMmSmsTsy );

        /**
         * Returns pointer to the Packet Data session
         *          
         *
         * @param None
         * @return pointer to the Packet Data session
         */
        virtual CMmPacketServiceTsy* PacketDataSession();

        /**
         * Sets Packet Data pointer to NULL
         *          
         *
         * @param None
         */
        virtual void ResetPacketDataSession();

        /**
         * Returns pointer to the SmsTsy
         *          
         *
         * @return pointer to the SmsTsy
         */
        virtual CMmSmsTsy* GetSmsTsy();
        
        /**
         * Returns pointer to the CustomTsy
         *          
         *
         * @return pointer to the CustomTsy
         */
        virtual CMmCustomTsy* GetCustomTsy();

        /**
         * Set pointer to the CustomTsy
         *          
         *
         * @param aMmCustomTsy
         */
        virtual void SetCustomTsy( CMmCustomTsy* aMmCustomTsy );

        /**
         * Complete notify ICC access capabilities change
         *          
         *
         * @param TInt aErrorCode Error code
         */
        virtual void CompleteNotifyIccAccessCapsChange();

        /**
         * Complete Get Customer Service Profile
         *          
         *
         * @param aCspFile CSP result from SIM
         * @param aErrorCode error code
         */
        virtual void CompleteGetCustomerServiceProfile( 
            RMobilePhone::TMobilePhoneCspFileV1 aCspFile,
            TInt aErrorCode );

        /**
         * This function cancels an outstanding ICC caps change 
         * notification request
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt NotifyIccAccessCapsChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Complete Get Service Table
         *          
         *
         * @param aTableData result from SIM
         * @param aErrorCode error code
         */
        virtual void CompleteGetServiceTableL( 
            RMobilePhone::TMobilePhoneServiceTableV1 aTableData,
            TInt aErrorCode );

        /**
         * Complete Get Phone Store Info
         *          
         *
         * @param aPhoneStoreInfo Result from SIM
         * @param aErrorCode error code
         */
        virtual void CompleteGetPhoneStoreInfo( 
            RMobilePhoneStore::TMobilePhoneStoreInfoV1 aPhoneStoreInfo, 
            TInt aErrorCode );
        
        /**
         * This function completes battery informations.
         *          
         *
         * @param aDataPackage data package
         * @param aErrorCode error code      
         */
        void CompleteNotifyBatteryInfoChange(
            CMmDataPackage* aDataPackage,
            TInt aErrorCode );

        /**
         * Complete Get ServiceProvider Name
         *          
         *
         * @param aDataPackage
         * @param aErrorCode error code
         */
        virtual void CompleteGetServiceProviderName(
            CMmDataPackage* aDataPackage, TInt aErrorCode );

        /**
         * This method can be used to find out what protocol mode is
         * currently active.
         *          
         *         
         * @param aMode Mode
         */
        virtual void GetStaticExtensionMode( 
            RMobilePhone::TMobilePhoneNetworkMode* aMode );

        /**
         * Removes a line form LineList and recalculates phone capabilities.
         * Checks the mode from all 
         * lines and then defines the capabilities.
         *          
         *
         * @param aLineName
         */
        void RemoveLine( const TName &aLineName );

        /**
         * Sets pointer to call object wishing to answer incoming data call.
         *          
         *
         * @param aMmCall
         * @return Call object
         */
        void SetWaitingCallForData( CMmCallTsy* aMmCall );

        /**
         * Returns pointer to call object wishing to answer incoming data call
         *          
         *
         * @param None
         * @return Call object
         */
        CMmCallTsy* WaitingCallForData();

#ifdef REQHANDLE_TIMER
        /**
         * Called when the timer expires and a request must be completed
         * automatically
         *          
         *
         * @param aReqHandleType req handle type.
         * @param aObject object that should handle this
         * @param aIPC
         */
        virtual void TimerExpired( CTelObject *aObject, TInt aReqHandleType, 
            TInt aIPC );
       
        /**
         * Calls the needed complete method due timer expiration
         *          
         *
         * @param aReqHandleType req handle type.
         * @param aError error value
         */
        void Complete( TInt aReqHandleType, TInt aError );

        /**
         * Gets the pointer to the private member attribute iTimeStampStore
         *          
         *
         * @return pointer to the time stamp store
         */
        virtual CResponseTimerStore* GetTimeStampStore();

        /**
         * gets the pointer to the member private attribute iReqHandleTimer
         *          
         *
         * @return pointer to the the iReqHandleTimer
         */
        virtual CResponseTimer* GetTimer();
#endif

        /**
         * Completes GetAlsLine
         *          
         *
         * @param aAlsStatus ALS line status
         * @param aErrorCode Symbian OS error code
         */
        virtual void CompleteGetAlsLine( 
            RMobilePhone::TMobilePhoneALSLine aAlsStatus, TInt aErrorCode );
        /**
         * Completes SetAlsLine
         *          
         *
         * @param aErrorCode Symbian OS error code
         */
        virtual void CompleteSetAlsLine( TInt aErrorCode );

        /**
         * Cancels an outstanding GetAlsLine request
         *          
         *
         */
        virtual TInt GetAlsLineCancel ( const TTsyReqHandle aTsyReqHandle );

        /**
         * Starts the air time duration timer
         *          
         *
         */
        virtual void AirTimeTimerCheckStart();

        /**
         * Starts the air time duration timer
         *          
         *
         */
        virtual void AirTimeTimerCheckStop();

        /**
         * Gets the value of the air time duration timer
         *          
         *
         * @param aTime value of the air time duration
         */
        virtual void GetAirTimeDuration( TTimeIntervalSeconds& aTime );

        /**
         * Returns pointer to the SAT messaging
         *          
         *
         * @return pointer to SAT messaging
         */
        virtual MStkTsySatService* GetSatMessaging();

        /**
         * Get State Info
         *          
         *
         * @return TRfStateInfo state info
         */
        virtual TRfStateInfo GetRfStateInfo();

        /**
         * Set State Info
         *          
         *
         * @param aStateInfo state info
         */
        virtual void SetRfStateInfo( TRfStateInfo aStateInfo );

        /**
         * Set Rf Status
         *          
         *
         * @param aRfStatus rf status
         */
        virtual void SetRfStatus( TRfStatus aRfStatus );

        /**
         * Get Rf Status
         *          
         *
         * @return iRfStatusFlag rf status
         */
        virtual TRfStatus GetRfStatus();

        /**
         * Registers SIM files for refresh
         *          
         *
         * @return Error value
         */
        TInt SimRefreshRegisterL();
        
        /**
         * Completes the SIM refresh register
         *          
         *
         * @param aErrorCode Symbian OS error code
         */
        void CompleteSimRefreshRegister( TInt aErrorCode );
        
        /**
         * Completes update life timer request
         *          
         *
         * @param aErrorCode Symbian OS error code
         */
        void CompleteUpdateLifeTimer( TInt aErrorCode );                

        /*
         * Returns true if refresh called (caching initiated from SIM)
         *          
         *
         * @return ETrue if refresh initiated from SIM, otherwise EFalse
         */
        TBool IsRefreshOngoing();

        /**
         * Completes the refresh now -indication, sets refresh flag to ETrue 
         * and initiates the SIM file caching
         *          
         *
         * @param aFilesToRefresh SIM file types that need to be refreshed 
         */
        void CompleteSimRefreshNowL( TUint16 aFilesToRefresh );

        /**
         * Starts caching the next SIM file in queue
         * const TCacheFileTypes aFileType SIM file type to be refreshed
         *          
         *
         * @return EFalse if no files in queue, otherwise ETrue
         */
        TBool CacheNextL();

        /**
         * Removes SIM file(s) from caching queue and calls complete for 
         * RefreshNow if no more files in queue
         *          
         *
         * @param aFileType SIM filetypes to be removed from the queue
         * @param aReadError Error value for reading the cache
         */
        void CompleteCacheSimL( const TUint8 aFileType, 
            TInt aReadError );

        /**
         * Completes the refresh by notifying server for completion
         * TBool aReadError: If error occured when reading files: ETrue, 
         * otherwise EFalse
         *          
         *
         * @param aReadError
         */
        void NotifySimRefreshCompletionL( TBool aReadError );

        /**
         * Completes ims authentication request
         *          
         *
         * @param aDataPackage Holds authentication data from sim
         * @param aResult Error value
         */
        void CompleteImsAuthentication( 
            CMmDataPackage* aDataPackage,
            TInt aResult );
            
        /**
         * Completes the NotifyIndicatorChange if indicator state
         * has changed.
         *          
         *
         */
        void CompleteNotifyIndicatorChange();

        /**
         * Completes GetMailboxNumbers request
         *          
         *
         */
        void CompleteGetMailboxNumbers(  
            CMmDataPackage* aDataPackage, 
            TInt aResult );
    
#ifdef TF_LOGGING_ENABLED        
        /**
         * Overloads original ReqCompleted for logging pourpouses
         *          
         *
         * @param aTsyReqHandle
         * @param  aError
         */
        virtual void ReqCompleted(
            const TTsyReqHandle aTsyReqHandle,
            const TInt aError );
#endif

        /**
         * Get pointer to Net Tsy
         *          
         *
         */
        CMmNetTsy* GetNetTsy();

        /**
         * Get pointer to Security Tsy
         *          
         *
         */
        virtual CMmSecurityTsy* GetSecurityTsy();

        /**
         * Get pointer to ONStoreTsy
         *          
         *
         * @return pointer to ONStoreTsy
         */
        CMmONStoreTsy* GetONStoreTsy();

		/**
         * Set pointer to ONStoreTsy
         *          
         *
         */
        void SetONStoreTsy(CMmONStoreTsy* aMmONStoreTsy);

        /** 
         * Get pointer to DTMF Tsy
         *          
         *
         */
        virtual CMmDtmfTsy* GetDtmfTsy();

        /** 
         * Get pointer to Supplementary Services Tsy
         *          
         *
         */
        virtual CMmSupplServTsy* GetSupplServTsy();

        /**
         * Sets the message manager pointer
         *          
         *
         * @param aMessageManager Message manager
         */
        void SetMessageManager( 
            CMmMessageManagerBase* aMessageManager );
        /**
         * Completes get call forwarding number request
         *          
         *
         * @param aError error value from 
         * @param aPackage includes data needed
         */
        void CompleteGetCallForwardingNumber( TInt aError, 
            const CMmDataPackage* aPackage );

        /**
         * Complete enumerate APN entries request
         *          
         *
         * @param aDataPackage Datapackage
         * @param aErrorCode Error value
         */
        void CompleteEnumerateAPNEntries( CMmDataPackage* aDataPackage, 
            TInt aErrorCode );
        
        /**
         * Complete Get APN name request
         *          
         *
         * @param aDataPackage Datapackage
         * @param aErrorCode Error value
         */
        void CompleteGetAPNname( CMmDataPackage* aDataPackage, 
            TInt aErrorCode );
            
        /**
         * Sets ALS Line service status by sim
         *          
         *
         * @param aState boolean for sim based ALS
         */
        void SetAlsState( TBool aState );
        
        /**
         * Maps ALS line info value
         *          
         *
         * @param aAlsLineInfo
         */        
        RMobilePhone::TMobilePhoneALSLine MapMEALSLine( TInt aAlsLineInfo );
        
        /**
         * Resets ME ALS info from CentRep.
         *          
         *
         */
        void ResetMEAlsInfo();
        
        /**
         * Returns sim refresh registration status 
         *          
         *
         */    
        TBool GetSimRefreshRegister(); 
    
        /**
         * Starts new cache when PB read is called and sim refresh registration 
         * is failed 
         *          
         *
         * @param aPhoneBookType phonebook to be cached
         * @return error value
         */  
        TInt StartNewCacheL( TUint8 aPhoneBookType );
        
        /**
         * Returns pointer to public CentRep
         *          
         *
         */
        CRepository* GetCfisCenRep();
        
        /**
         * Returns pointer to public CentRep:
         *          
         *
         */
        CRepository* GetMwisCenRep();

        /**
         * Returns pointer to private CentRep:
         *          
         *
         */
        CRepository* GetCfisPRCenRep();

        /**
         * Complete Append APN name request
         * 
         *
         * @param aErrorCode Error value
         */
        void CompleteAppendAPNName( TInt aErrorCode );
        
        /**
         * Complete delete APN entry
         * 
         *
         * @param aErrorCode Error value
         */
        void CompleteDeleteAPNName( TInt aErrorCode );
        
        /**
         * Complete notification request
         *          
         *
         * @param aErrorCode Error value
         */
        void CompleteNotifyAPNListChanged( TInt aErrorCode );

        /**
         * Complete Set APN service status
         * 
         *
         * @param aErrorCode Error value
         */
        void CompleteSetAPNControlListServiceStatus( TInt aErrorCode );
        
        /**
         * Complete Get APN service status
         * 
         *
         * @param aDataPackage Datapackage
         * @param aErrorCode Error value
         */
        void CompleteGetAPNControlListServiceStatus( CMmDataPackage* 
            aDataPackage, TInt aErrorCode );
        
        /**
         * Complete service status notification
         *          
         *
         * @param aDataPackage Datapackage
         * @param aErrorCode Error value
         */
        void CompleteNotifyAPNControlListServiceStatusChange( 
            CMmDataPackage* aDataPackage, TInt aErrorCode );
            
        /**
         * Returns current nr of active calls
         *
         */
        TInt NumberOfActiveCalls() const { return iAmountOfActiveCalls; }
        
        /**
         * Complete call duration change notification
         *          
         *
         */
        void CompleteNotifyCallDurationChange();                    
            
        /**
         * SetAlsQueryInBoot request
         *
         * @param aInBoot bootupflag if als is asked during boot
         */
        void SetAlsQueryInBoot( TBool aInBoot );
            
        /**
         * GetAlsQueryInBoot request
         *
         * @return TBool
         */
        TBool GetAlsQueryInBoot();
        
        /**
         * If HomeZoneParams has been asked
         *
         * @return TBool
         */
		TBool GetHomeZoneParamsChecked();
		
		/**
         * Set HomeZoneParams status
         *
         * @param aHomeZoneChecked
         */
		void SetHomeZoneParamsChecked( TBool aHomeZoneChecked );      
        
		/**
	     * This method set the initialization status of the phonebook
	     *
		 * @param aIsInitializationActive is initialization active or not
		 */
		void SetPBInitActiveStatus( TBool aIsInitializationActive );
		
		/**
		 * This method returns the initialization status of the phonebook
		 *
		 * @return Phonebook init status
		 */
		 TBool IsPBInitActive();
        
		
        /**
		 * Complete TerminateAllCalls request
		 * 
		 * @param aErrorCode The result of the request
		 */
        void CompleteTerminateAllCallsReq(TInt aErrorCode);
        
        /**
         * Complete TerminateActiveCalls request
         * 
         * @param aErrorCode The result of the request
         */
        void CompleteTerminateActiveCallsReq(TInt aErrorCode);
        
        /**
        * Notify change of battery info
        *          
        * @param aErrorCode error code
        * @param aBatteryData data structure used to retreive data from a call
        *  to GetBatteryInfo
        */
        void NotifyBatteryInfo( TInt aErrorCode, 
            CHWRMPower::TBatteryConsumptionData aBatteryData );
        
        /**
        * Notify change of charging status
        *          
        * @param aErrorCode error code
        * @param aBatteryInfo package containing power and battery status
        */
        void NotifyChargingStatus( TInt aErrorCode, 
            RMobilePhone::TMobilePhoneBatteryInfoV1 aBatteryInfo );
#ifdef USING_CTSY_DISPATCHER
        /**
         * Sets iPhoneIdentity
         * This is for when the LTSY can return the entire TMobilePhoneIdentityV1           
         *
         * @param aPhoneId The phone identity
         * @param aError error code
         */
        virtual void CompleteGetPhoneId(
        	const RMobilePhone::TMobilePhoneIdentityV1& aPhoneId,
            TInt aError );
#endif //USING_CTSY_DISPATCHER
        
        /**
         * This method returns whether SIM is ready or not.
         *
         * @return SIM status
         */
        TBool IsSIMReady();
	
        /*
		 * This method saves FDN details in case of FDN Phonebook not created
         *
         * @since S60 v3.2
         * @param  aErrorCode Error value
         * @param  aDataPackage DataPackage
         */
        void SaveFdnInfoDetails(TInt aErrorCode, CMmDataPackage* aDataPackage );

        /*
		 * This method used to notify the phone that the phone book store initilization was complete
         *
         * @param  aError Error value
         */
        void PhoneBookStoreInitCompleteL(TInt aError);
        
    private:
    
        /**
         * C++ default constructor.
         */
        CMmPhoneTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Initialises transaction handler, TSY req handle store, 
         * and object lists
         *          
         *
         */
        void InitStoresAndListsL();

        /**
         * Initialises extension modules
         *          
         *
         */
        void InitExtensionModulesL();

        /**
         * Initialise miscellaneous internal attributes
         *          
         *
         */
        void InitInternalAttributes();

        /**
         * Handles extended client requests. Uses TRAP to ensure functioning on
         * memory allocation failure
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aIpc request IPC number
         * @param aPackage packaged request parameters
         * @return result of the request
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Handles cancel methods that cause message sending to DOS.
         *          
         *
         * @param aIpc request IPC number
         * @param aTsyReqHandle TSY request handle
         * @return result of the request
         */
        TInt CancelToPhonetL( const TInt aIpc, 
                const TTsyReqHandle aTsyReqHandle );

        /**
         * Traped Initialisation method
         *          
         *
         */
        void InitL();

        /**
         * Get ICC Access Capabilities
         *          
         *
         * @param aTsyReqHandle
         * @param aCaps
         * @return error value
         */
        TInt GetIccAccessCaps( const TTsyReqHandle aTsyReqHandle, 
            TUint32* aCaps );

        /**
         * Notify Change of ICC Access Caps
         *          
         *
         * @param aCaps
         * @return error value
         */
        TInt NotifyIccAccessCapsChange( TUint32* aCaps );
        
        /**
         * Get Service Table and check validity of client parameters
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aTable Service Table specifier
         * @param aTableData Service Table file contents
         * @return KErrNone or system wide error
         */
        TInt GetServiceTableCheckParametersL( 
            const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneServiceTable* aTable,
            TDes8* aTableData );        

        /**
         * Get Service Table
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aTable Service Table specifier
         * @param aTableData Service Table file contents
         * @return status
         */
        TInt GetServiceTableL( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneServiceTable* aTable,
            RMobilePhone::TMobilePhoneServiceTableV1* aTableData );

        /**
         * This function cancels GetServiceTable request
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return status
         */
        TInt GetServiceTableCancel ( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Get Phone Store Info
         *          
         *
         * @param aInfo pointer to ETel server phone store info data
         * @param aStoreName name of the store
         * @return error value
         */
        TInt GetPhoneStoreInfoL( TDes8* aInfo,
            const RMobilePhone::TMobileName* aStoreName );

        /**
         * This function cancels GetPhoneStoreInfo request
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return status
         */
        TInt GetPhoneStoreInfoCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This function returns phone ID.
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aId aId pointer
         * @return KErrNone
         */
        TInt GetPhoneIdL( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneIdentityV1* aId );

        /**
         * This is a helper function for GetPhoneIdL.
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aId aId pointer
         */        
        void DoGetPhoneIdL(const TTsyReqHandle aTsyReqHandle,
        		RMobilePhone::TMobilePhoneIdentityV1* aId);

        /**
         * This function returns subscriber ID.
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aId aId pointer
         * @return KErrNone
         */
        TInt GetSubscriberIdL( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneSubscriberId* aId );

        /**
         * This function cancels GetSubscriberId request
         *          
         * 
         * @param aTsyReqHandle request handle
         * @return KErrNone
         */
        TInt GetSubscriberIdCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This request function returns Phone Indicator state to the client.
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aIndicators indicator pointer
         * @return KErrNone, or KErrGeneral, if the input pointer is NULL
         */
        TInt GetIndicators( const TTsyReqHandle aTsyReqHandle,    
             TUint32* aIndicators);
        
        /**
         * Returns multimode capabilities
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCaps multimode capabilities
         * @return Return value to the ETel Server
         */
        TInt GetMultimodeCaps( const TTsyReqHandle aTsyReqHandle, 
            TUint32* aCaps );

        /**
         * This function returns signal capabilities
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aCaps Capabilities
         * @return KErrNone
         */
        TInt GetSignalCaps( const TTsyReqHandle aTsyReqHandle, 
            TUint32* aCaps );

        /**
         * This function returns identity capabilities
         *          
         *
         * @param aTsyReqHandle request handle
         * @param TaCaps pointer to aCaps
         * @return KErrNone
         */
        TInt GetIdentityCaps( const TTsyReqHandle aTsyReqHandle, 
            TUint32* aCaps );

        /**
         * This function returns network capabilities
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aCaps Capabilities
         * @return KErrNone
         */
        TInt GetNetworkCaps( const TTsyReqHandle aTsyReqHandle, 
            TUint32* aCaps );

        /**
         * This function returns phone indicator capabilities
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aActCaps: Action Capabilities (get, notify)
         * using a RMobilePhone::TMobilePhoneIndicatorCaps -type bitmasks
         * @param aIndCaps: the kind of indicators can be given
         * using a RMobilePhone::TMobilePhoneIndicators -type bitmasks
         * @return KErrNone, or KErrGeneral if any of the input 
         *              pointers are NULL
         */
        TInt GetIndicatorCaps( const TTsyReqHandle aTsyReqHandle,    
            TUint32* aActCaps,                        
            TUint32* aIndCaps );
        
        /**
         * Get Customer Service Profile
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aCspFile Customer Service Profile file
         * @return status
         */
        TInt GetCustomerServiceProfileL( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aCspFile );
 
        /**
         * This function cancels an outstanding get customer service profile
         * request
         *         
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt GetCustomerServiceProfileCancel(
                const TTsyReqHandle aTsyReqHandle );
        
        /**
         * This function returns battery capabilities.
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aCaps pointer to aCaps
         * @return KErrNone
         */
        TInt GetBatteryCaps( 
            const TTsyReqHandle aTsyReqHandle, 
            TUint32* aCaps );

        /**
         * This function returns battery informations.
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aInfo aInfo pointer
         * @return KErrNone
         */
        TInt GetBatteryInfoL( 
            const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneBatteryInfoV1* aInfo );

        /**
         * This function is used to request battery info change notifications.
         *          
         *
         * @param aInfo battery info
         * @return KErrNone
         */
        TInt NotifyBatteryInfoChange( 
            RMobilePhone::TMobilePhoneBatteryInfoV1* aInfo );

        /**
         * This function cancels an outstanding battery info change notification
         * request
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt NotifyBatteryInfoChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );


        /**
         * This function is used to request indicator change notifications.
         *          
         *
         * @param aIndicatorState indicator state
         * @return KErrNone
         */
        TInt NotifyIndicatorChange( TUint32* aIndicatorState );

        /**
         * Cancels NotifyIndicatorChange
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return result of the request
         *
         */
        TInt NotifyIndicatorChangeCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Get service provider name from Sim
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aName Service provider name info
         * @return KErrNone or system wide error code
         */
        TInt GetServiceProviderNameL( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aName );

        /** This function cancels the service provider name retrieving.
         *        
         *
         * @param aTsyReqHandle request handle
         * @return KErrNone
         */
        TInt GetServiceProviderNameCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This method allows a client to find out which voice line 
         * is currently selected as the active ALS line for outgoing 
         * voice calls.
         *          
         * 
         * @param aALSLine Selected line
         * @return KErrNone
         */
        TInt GetALSLineL( const TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TMobilePhoneALSLine* aALSLine );
        
        /**
         * This method allows a client to set which voice line is currently 
         * selected as the active ALS line for outgoing voice calls
         *          
         *
         * @param aALSLine line selection
         * @return KErrNone
         */
        TInt SetALSLineL( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneALSLine* aALSLine );
        
        /**
         * This method allows a client to be notified if the currently selected 
         * ALS line changes
         *          
         *
         * @param aALSLine new active line 
         * @return KErrNone
         */
        TInt NotifyALSLineChange( 
            RMobilePhone::TMobilePhoneALSLine* aALSLine );

        /**
         * This method cancels order to notify if ALS line changes
         *          
         *
         * @param aTsyReqHandle request handle
         * @return KErrNone
         */
        TInt NotifyALSLineChangeCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This method retrieves the current status of the Fixed Dialling 
         * Numbers service
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aStatus current status
         * @return KErrNone
         */
        TInt GetFdnStatusL( const TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TMobilePhoneFdnStatus* aFdnStatus );
        
        /**
         * This method cancels Fixed Dialling Numbers service request
         *
         *
         * @param aTsyReqHandle request handle
         * @return KErrNone
         */
        TInt GetFdnStatusCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This method activates (ON) or deactivates (OFF) the Fixed 
         * Dialling Numbers (FDN) service. 
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aFdnsetting new setting
         * @return KErrNone
         */
        TInt SetFdnSettingL( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
         * This request allows a client to be notified when the status of the 
         * FDN service has changed
         *          
         *
         * @param aStatus status
         * @return KErrNone
         */
        TInt NotifyFdnStatusChange( 
            RMobilePhone::TMobilePhoneFdnStatus* aStatus );

        /**
         * This request cancels order to notify if status changes
         *          
         *
         * @param aTsyReqHandle request handle
         * @return KErrNone
         */
        TInt NotifyFdnStatusChangeCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Returns current indicator state according to batteryInfo,
         * networkStatus and call state
         *          
         *
         * @return bit mask of enabled features using
         * RMobilePhone::TMobilePhoneIndicators
         */
        TUint32 GetIndicatorState();

        /**
         * Get current subscriber ID (IMSI) for TSY internal use
         *          
         *
         */
        void GetSubscriberIdForTSYL();

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *          
         *
         * @param aReqHandleType req handle type.
         * @param aTsyReqHandle: req handle to be stored.
         */
        void SetTypeOfResponse( const TInt aReqHandleType, 
            const TTsyReqHandle aTsyReqHandle ); 
        
#endif
 
        /**
         * Checks wether or not a ETel request can be performed
         * while offline mode is enabled
         *          
         *
         * @param aIpc Ipc number of the request
         * @return Boolean indicating if request is possible in offline
         */
        TBool IsRequestPossibleInOffline( TInt aIpc );
        
        /**
         * Authenticates with data provided by client
         * @param aAuthenticationData Pointer to IMS authentication data.
         * @return TInt KErrNone
         */
        TInt ImsAuthenticationL( TDes8* aAuthenticationData );

        /**
         * Cancels sim authenticaiton request
         *          
         *
         * @return KErrNone
         */
        TInt ImsAuthenticationCancel();
        
        /**
         * Caches Service Tables
         *          
         *
         * @return KErrNone
         */
        void CacheServiceTablesL();
        
        /**
         * Completes caching of Service Tables
         *          
         *
         * @return TInt KErrNone
         */
        void CompleteCacheServiceTablesL(const TInt aError);
        
        /**
         * Requests mailbox numbers identifier information
         * @param aMailBox mailbox data.
         *          
         *
         * @return KErrNone
         */
        TInt GetMailboxNumbersL( TDes8* aMailBox );
        
        /**
         * Cancels get mailbox numbers  request
         *          
         *
         * @return KErrNone
         */
        TInt GetMailboxNumbersCancel();
            
        /**
         * Enumerates number of APN entries in ACL
         *          
         *
         * @param aTsyReqHandle Request Handle
         * @param aIndex Pointer to index
         * @return Error value
         */
        TInt EnumerateAPNEntriesL( const TTsyReqHandle aTsyReqHandle, 
            TUint32* aIndex );
        
        /**
         * Cancel enumerate APN entries request
         *          
         *
         * @return Error value
         */
        TInt EnumerateAPNEntriesCancel();
        
        /**
         * Get APN name
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aIndex Index of requested APN
         * @param aAPN APN name
         * @return Error value
         */
        TInt GetAPNnameL( const TTsyReqHandle aTsyReqHandle, TUint32* aIndex, 
            TDes8* aAPN );
        
        /**
         * Cancel get APN name request
         *          
         *
         * @return Error value
         */
        TInt GetAPNnameCancel();

        /**
         * Append an entry to the access point name (APN) control list
         * 
         *
         * @param aAPNEntry APN Entry
         * @return Error value
         */
        TInt AppendAPNNameL( RMobilePhone::TAPNEntryV3* aAPNEntry );
        
        /**
         * Cancel AppendAPNName
         * 
         *
         * @return KErrNone
         */
        TInt AppendAPNNameCancel();

        /**
         * Delete an entry from the access point name (APN) control list
         * 
         *
         * @param aTsyReqHandle TSY req handle
         * @param aIndex Deleted index
         * @return Error value
         */
        TInt DeleteAPNNameL( const TTsyReqHandle aTsyReqHandle, 
            TUint32* aIndex );
        
        /**
         * Cancel DeleteAPNName
         * 
         *
         * @return KErrNone
         */
        TInt DeleteAPNNameCancel();
        
        /**
         * This function is used to request to receive notification of when 
         * the APN control list changes.
         *          
         *
         * @return KErrNone
         */
        TInt NotifyAPNListChanged();

        /**
         * This function cancels an outstanding access point name (APN) 
         * control list change notification request.
         *          
         *
         * @return KErrNone
         */
        TInt NotifyAPNListChangedCancel();

        /**
         * Enabling and disabling the access point name (APN) 
         * control list service
         * 
         * @param aTsyReqHandle TSY req handle
         * @param aAPNControlListServiceStatus ACL status
         * @return Error value
         */
        TInt SetAPNControlListServiceStatusL( 
            const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TAPNControlListServiceStatus* 
            aAPNControlListServiceStatus );
        
        /**
         * Cancel the request to set ACL status
         * 
         *
         * @return KErrNone
         */
        TInt SetAPNControlListServiceStatusCancel();
        
        /**
         * Get the status of the access point name (APN) control list service
         * 
         *
         * @param aTsyReqHandle TSY req handle
         * @param aAPNControlListServiceStatus ACL status
         * @return Error value
         */
        TInt GetAPNControlListServiceStatusL( 
            const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TAPNControlListServiceStatus* 
            aAPNControlListServiceStatus );
        
        /**
         * Cancel the request to get ACL status
         * 
         *
         * @return KErrNone
         */
        TInt GetAPNControlListServiceStatusCancel();

        /**
         * This function is used to request to receive notification of when 
         * the status of the APN control list service changes.
         *          
         *
         * @param aAPNControlListServiceStatus
         * @return KErrNone
         */
        TInt NotifyAPNControlListServiceStatusChange( 
            RMobilePhone::TAPNControlListServiceStatus* 
            aAPNControlListServiceStatus );

        /**
         * This function cancels an outstanding access point name (APN) 
         * control list service status change notification request.
         *          
         *
         * @return KErrNone
         */
        TInt NotifyAPNControlListServiceStatusChangeCancel();
        
        /**
          * Terminate all calls simultaneously
          * 
          * @param aTsyReqHandle TSY req handle
          * @return Error value
          */
        TInt TerminateAllCallsL(const TTsyReqHandle aTsyReqHandle);

        /**
          * Terminate all active calls simultaneously
          * 
          * @param aTsyReqHandle TSY req handle
          * @return Error value
          */
        TInt TerminateActiveCallsL(const TTsyReqHandle aTsyReqHandle);
        
        /*
         * This function return availability of Hareware Resource Manager in the current ROM
         * 
         * @return TTrue indicate HWRM is available. TFalse indicate it is unavailable
         */
        TBool IsHWRMSupported();

#ifdef REQHANDLE_TIMER
        /**
         * Sets the security code when the request completes due to timer expiration
         * and then it calls the completion function to handle the timeout event.         
         *
         * @param aReqHandleType req handle type.
         * @param aError: error code.
         */
        void TimeoutGetSecurityCodeInfo(const TInt aReqHandleType, const TInt aError);
#endif

    private:    // Data
        
        /**
         * Property for call forwarding number
         */          
        RProperty iEmergencyCallInfoProperty;
    
        /**
         * Buffer for call forwarding number
         */             
        TBuf<40> iCallForwardingNumber;

        /**
         * A pointer to the Phone factory
         * Own.
         */                  
        CMmPhoneFactoryTsy* iMmPhoneFactory;

        /**
         * A pointer to the active phone extension
         * Own.
         */                  
        CMmPhoneExtInterface* iMmPhoneExtInterface;

        /**
         * A pointer to the call list
         * Own.
         */                  
        CMmCallList* iCallList;

        /** 
         * A pointer to the line list
         * Own         
         */         
        CMmLineList* iLineList;

        /**
         * A pointer to the phonebook list
         * own.
         */                   
        CMmPBList* iPBList;

        /**
         * A pointer to the conference call object'
         */                  
        CMmConferenceCallTsy* iMmConferenceCall; 
        
        /**
         * A pointer to the SAT object       
         */                  
        CTsySatMessagingBase* iTsySatMessaging;

        /**
         * A pointer to the USSD object         
         */                  
        CMmUssdTsy* iMmUssdTsy;

        /** 
         * A pointer to the onstore object
         * Own.         
         */                  
        CMmONStoreTsy* iMmONStoreTsy;

        /**
         * A pointer to the enstore object
         * Own.         
         */                  
        CMmENStoreTsy* iMmENStoreTsy;

        /**
         * A pointer to the phonebookstore object
         * Own.         
         */                  
        CMmPhoneBookStoreTsy* iMmPhoneBookStoreTsy;

        /**
         * A pointer to the Custom TSY                 
         */         
        CMmCustomTsy* iMmCustomTsy;

        /** 
         * A pointer to the multimode messaging object
         */         
        CMmSmsTsy* iMmSmsTsy;

        /**
         * a pointer to the Packet Data object
         */         
        CMmPacketServiceTsy* iMmPacketServiceTsy;

        /**
         * Transaction id
         */         
        TUint8 iTransId;

        /**
         * Phone capabilities
         */         
        RPhone::TCaps iPhoneCaps;

        /**
         * Phone's modem status
         */         
        RPhone::TStatus iPhoneStatus;

        /** 
         * Phone version information
         */         
        RMobilePhone::TMobilePhoneIdentityV1 iPhoneIdentity;
        
        /** 
         * Phone version information
         */
        RMobilePhone::TMobilePhoneIdentityV1* iRetPhoneIdentity;

        /**
         * Subscriber ID (IMSI)
         */         
        RMobilePhone::TMobilePhoneSubscriberId iSubscriberId;

        /**
         * Pointers to client address space
         */         
        RPhone::TCaps* iRetCaps;

        /*
         * Pointer to client side for modem detection changes
         */         
        RPhone::TModemDetection* iRetModemDetection;

        /**
         * Flag for refresh; set to true when "refresh now" -indication 
         * received and to false when all registered files cached
         */         
        TBool iRefreshOngoing;

        /**
         * For keeping list of SIM files left to be cached
         */         
        TUint16 iSimCacheQueue;        

        /** 
         * notify fixed dialing number status
         */         
        RMobilePhone::TMobilePhoneFdnStatus* iRetNotifyFdnStatus;

        /**
         * notify als line
         */         
        RMobilePhone::TMobilePhoneALSLine* iRetNotifyALSLine;

        /**
         * ICC capabolities
         */         
        TUint32* iICCCaps;

        /**
         * Battery info
         */         
        RMobilePhone::TMobilePhoneBatteryInfoV1 iBatteryInfo;
        
        /**
         * Battery info
         */
        RMobilePhone::TMobilePhoneBatteryInfoV1* iRetBatteryInfo;

        /**
         * Phone store info (return pointer to client)
         */         
        RMobilePhoneStore::TMobilePhoneStoreInfoV1* iRetPhoneStoreInfo;

        /**
         * Service Table (return pointer to client)
         */         
        RMobilePhone::TMobilePhoneServiceTableV1* iRetTableData;
        
        /**
         * Service Table (SIM )(return pointer to client)
         */         
        RMobilePhone::TMobilePhoneServiceTableV1* iServiceTableDataCache;
        
        /**
         * Service Table (USIM) (return pointer to client)
         */         
        RMobilePhone::TMobilePhoneServiceTableV1* iServiceTableUSIMDataCache;
        
        /**
         * Pending Service Table cache request
         */         
        TServiceTableRequest iPendingServiceTableCacheReq;
        
        /** 
         * Pending Service Table request
         */         
        TServiceTableRequest iPendingServiceTableReq;
        
        /**
         * A table for phone request handles
         */         
        TTsyReqHandle iPhoneReqHandles[EMultimodePhoneMaxNumOfRequests];

        /**
         * Pointer to CMmMessageManagerBase
         */                  
        CMmMessageManagerBase* iMessageManager;

        /**
         * a Pointer to the broadcast object
         */         
        CMmBroadcastTsy* iMmBroadcastTsy;

        /** 
         * new fdn setting
         */         
        RMobilePhone::TMobilePhoneFdnSetting* iRetFdnSetting;

        /**
         * TSY req handle store
         * Own.
         */                  
        CMmTsyReqHandleStore* iTsyReqHandleStore;
        
        /**
         * TSY request type
         */         
        TPhoneRequestTypes iReqHandleType;

        /**
         * A pointer to call object waiting for incoming call
         */         
        CMmCallTsy* iMmWaitingDataCall; 

        /**
         * Als Line status
         */         
        RMobilePhone::TMobilePhoneALSLine iAlsLine;

        /**
         * Temporary Als Line status, for set operation
         */         
        RMobilePhone::TMobilePhoneALSLine iTempAlsLine;

        /**
         * DOS BootState
         */         
        TNosBootState iBootState;

        /**
         * Phonebook state
         */         
        CStorageInfoData* iPhoneBookState;


#ifdef REQHANDLE_TIMER

        /**
         * Timer
         * Own.         
         */         
        CResponseTimer *iReqHandleTimer;
        
        /**
         * Time store
         * Own.         
         */         
        CResponseTimerStore *iTimeStampStore;

#endif
        /**
         * timer for air time duration monitoring
         * Own.
         */                  
        CHeartbeatRunner* iAirTimeDurationTimer;

        /**
         * Amount of active calls for air time duration calculation
         */         
        TInt iAmountOfActiveCalls;

        /**
         * Offline
         */         
        TRfStateInfo iStateInfo;
        TRfStatus iRfStatusFlag;

        /**
         * Pointer to the NetTsy class
         * Own.
         */                  
        CMmNetTsy* iMmNetTsy;

        /**
         * Pointer to the DtmfTsy class
         * Own.
         */                  
        CMmDtmfTsy* iMmDtmfTsy;

        /**
         * Pointer to the SupplServTsy class
         * Own.         
         */         
        CMmSupplServTsy* iMmSupplServTsy;
        
        /**
         * Pointer to the SecurityTsy class
         * Own.         
         */ 
        CMmSecurityTsy* iMmSecurityTsy;
        
        /**
         * Fdn Status
         */                 
        RMobilePhone::TMobilePhoneFdnStatus* iRetGetFdnStatus;
		
		/**
		 * Pointer to struct for storing get servicetable parameters
		 */
		RPointerArray< TServiceTableRequests > iServiceTableRequests;
		        
        /**
         * state of phone indicator
         */         
        RPointerArray< TSubscriberIdRequest > iSubscriberIdRequests;

        /**
         * Array for buffering Phone Id requests
         */         
        RPointerArray< TGetPhoneIdRequest > iGetPhoneIdRequests;
        
        /**
         * Array for buffering Customer service profile requests
         */             
        RPointerArray< TCustomerServiceProfileIdRequest > 
            iCustomerServiceProfileRequests;
        
        /**
         * Array for buffering Get Als Line requests
         */             
        RPointerArray< TGetAlsLineRequest > iGetAlsLineRequests;
        
        /**
         * Indicator state
         */                         
        TUint32 iIndicatorState;

        /**
         * Indicator state
         */ 
        TUint32* iRetIndicatorState;
        
        /**
         * Pointer to the IMS authentication data
         */         
        TDes8* iIMSAuthenticationData; 
        
        /** 
         * Pointer to the Mailbox dialling numbers data
         */             
        TDes8* iMailBoxData;
        
        /** 
         * Number of APNs in ACL (return pointer to client)
         * Not Own.
         */
        TUint32* iRetAPNEntries;
        
        /**
         * APN name (return pointer to client)
         * Not Own.
         */
        TDes8* iRetAPNname;
        
        /**
         * Sim support for ALS
         */
        TBool iAlsSupportedBySim;
        
        /**
         * Pointer to Central Repository
         * Own.
         */
        CRepository* iCentRep;
        
        /**
         * Indicates about sim refresh registration status
         */ 
        TBool iSimRefreshRegisterOk;
        
        /**
         * Indicates that GetServiceTableL method is called
         */
        TBool iCalledOnGet;
        
        /** 
         * Pointer to struct for storing get servicetable parameters 
         * Own.
         */
        TGetServiceTableRequest* iServiceTableReq; 
        
        /**
         * Service table cache error
         */
        TInt iCacheServiceTableError;

        /**
         * Pointer to Public Central Repository
         * Own.
         */
        CRepository* iCFISCentRep;
        
        /**
         * Pointer to MWIS Central Repository
         * Own.
         */
        CRepository* iMWISCentRep;
        
        /**
         * Pointer to CFIS Private Central Repository
         * Own.
         */
        CRepository* iCFISPrivateCentRep;
        
        /**
         * Get Subscriber Id requested from CommonTSY
         */
        TBool iTSYSubscriberIdReq;

        /**
         * Pointer to ACL status
         * Not Own.         
         */
        RMobilePhone::TAPNControlListServiceStatus* iRetAclStatus;
        
        /**
         * Pointer to ACL status
         * Not Own.         
         */
        RMobilePhone::TAPNControlListServiceStatus* iSetAclStatus;
        
        /**
         * Pointer to ACL status
         * Not Own.         
         */
        RMobilePhone::TAPNControlListServiceStatus* iRetNotifyAclStatus;
        
        /**
         * Array for storing get service provider name requests
         */         
        RPointerArray< TServiceProviderNameRequest > 
            iServiceProviderNameRequests;
            
        /**
         * Pointer to LicenseeTsy instance
         */     
        MLtsyFactoryBase* iLtsyFactory;
	    	
	    /**
	     * A boolean to tell CSP file request that 
	     * it's called internally during the boot
	     */
	    TBool iCspFileQueryInBoot;
       
	    /**
	     * A boolean to tell ALS support request that 
	     * it's called internally during the boot
	     */
	    TBool iAlsQueryInBoot;
	    	
	    /**
	     * A boolean to hold CSP file's ALS data
	     */
	    TBool iCspFileALS;
	    	
	    /**
	     * A boolean to hold ALS Pp Support
	     * it's called internally during the boot
	     */
	    TBool iAlsPpSupport;
	    	
	    /**
	     * A boolean to tell if CSP file's ALS data
	     * is being set into ALS state
	     */
	    TBool iCspFileAlsSet;
	    
	    /**
	     * A boolean to tell if HomeZoneParams
	     * has been checked
	     */
	    TBool iViagHomeZoneParamsChecked;
	    
        /**
		*Pointer to the Telephony Audio Control instance
		*/ 
        MTelephonyAudioControl* iTelephonyAudioControl;
	    
        /** 
		*Attribute to tell the status of PB initialization
		*/
        TBool iIsInitializationActive;		

        /*
         * Attribute to tell the status is modem status ready
         */
        TBool iIsModemReady;
        
        // ==== For getting Battery info from HWRM ====	    
	    /**
	    * Pointer to CHWRMPower instance
	    */
	    CHWRMPower* iPowerManager;
	    
	    /**
	    * Pointer to the BatteryInfoObserver
	    */	    
	    CBatteryInfoObserver* iBattery;
	    
	    /**
	    * Pointer to the ChargingStatusObserver
	    */
        CChargingStatusObserver* iChargingStatus;
        
        /**
        * Pointer to SystemStatePlugin
        */
        CCtsySystemStatePluginHandler* iSystemStatePluginHandler;
        
        /**
        * Holds the mailbox data
        */
        RMobilePhone::TMobilePhoneVoicemailIdsV3 iMailboxData;
    };

#endif // CMMPHONETSY_H

// End of File
