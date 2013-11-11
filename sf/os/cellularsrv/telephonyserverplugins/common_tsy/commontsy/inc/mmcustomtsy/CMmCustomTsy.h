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



#ifndef CMMCUSTOMTSY_H
#define CMMCUSTOMTSY_H

#include <featmgr/featurecontrol.h>
#include "cmmphonetsy.h"
#include <ctsy/rmmcustomapi.h>
#include "CMmCustomExtInterface.h"
#include <ctsy/serviceapi/mcustomvendorextcomplete.h>
#include "CMmSubTsyBase.h"
#include "CMmTsyBase.h"
#include "MmTsy_numberOfSlots.h"
#include "MmTsy_timeoutdefs.h"
#include <f32file.h>
#include <hash.h>
#include "cmmvoicecalltsy.h"

// CONSTANTS
// Valid range: 10-16
#define BLOB_SIZE          16
#define METADATA_SIZE      2

// Rand database file paths
_LIT( KRandDb, "rand_db.cur" );
_LIT( KRandDbPrev, "rand_db.prv" );

// FORWARD DECLARATIONS
class CMmCustomExtInterface;
class CMmCustomVendorExt;
class CMmPacketTsy;

// CLASS DECLARATION

/**
 *  CMmCustomTsy contains mode-independent custom functionality and
 *  is an aggregate class for its subsystems.
 *
 */
NONSHARABLE_CLASS( CMmCustomTsy ) : public CMmTsyBase, 
    public MCustomVendorExtComplete
    {
    friend class CMmWimTsy;
    friend class CMmSimLockTsy;
    friend class CMmSIMTsy;
    friend class CMmCustomSecurityTsy;
    friend class CMmPhonebookStoreTsy;

    // TCustomRequestType enumerates indexes to Custom TSY's request handle
    // table. Request handles are stored there while waiting for
    // completion of the request.
    enum TCustomRequestType
        {
        ECustomTsyReqHandleUnknown,
        ECustomTsyNotifyDtmfEvent,
        ECustomTsyGetDiagnosticInfo,
        ECustomTsyGetRemoteAlertingToneStatus,
        ECustomTsyCallOrigin,
        ECustomTsyTerminateCall,
        ECustomTsyGetAlsBlocked,
        ECustomTsySetAlsBlocked,
        ECustomTsyNotifyAlsBlockedChanged,
        ECustomTsyNotifyNSPSStatus,
        ECustomTsyGetCipheringInfo,
        ECustomTsyNotifyCipheringInfoChange,
        ECustomTsyNetWakeup,
        ECustomTsyNotifySsAdditionalInfo,
        ECustomTsyNotifySsRequestComplete,
        ECustomTsyNotifyCacheReady,
        ECustomTsyGetPndCacheStatus,
        ECustomTsyGetAlsPpSupport,
        ECustomTsyCallGsmBlackListClear,
        ECustomTsyCheckEmergencyNumber,
        ECustomTsyGetOperatorName,
        ECustomTsyGetProgrammableOperatorLogo,
        ECustomTsySsNotification,
        ECustomTsyGetNetworkOperatorName,
        ECustomTsyCheckTwoDigitDialSupport,
        ECustomTsySatRefreshCompleteNotification,
        ECustomTsyResetNetServer,
        ECustomTsyNotifyNetworkConnectionFailure,
        ECustomTsyGetSimFileInfo,
        ECustomTsyGetLifeTimerInfo,
        ECustomTsyGet3GPBInfo,
        ECustomTsyGetSystemNetworkModes,
        ECustomTsySetSystemNetworkMode,
        ECustomTsyGetCurrentSystemNetworkMode,
        ECustomTsyIMSAuthenticate,
        ECustomTsyGetSimAuthenticationData,
        ECustomTsySetDriveMode,
        ECustomTsyNotifyRauEvent,
        ECustomTsyReadHSxPAStatus,
        ECustomTsyWriteHSxPAStatus,
        ECustomTsyNotifyHSxPAStatus,
        ECustomTsyNotifyIccCallForwardingStatusChange,
        ECustomTsyGetIccCallForwardingStatus,
        ECustomTsyGetCellInfo,
        ECustomTsyNotifyCellInfoChange,
        ECustomTsyGetSystemNetworkBand,
        ECustomTsySetSystemNetworkBand,
		ECustomTsyGetUSIMServiceSupport,
		ECustomTsyNotifyRemoteAlertingToneStatusChange,
        // ATTENTION:: Declare constant for those requests that need
        // own request handle record in iTsyReqHandleStore above the
        // following!
        ECustomTsyMaxNumOfRequests
        };

    private:
    
        /** used for queuing CheckAlsPpSupport requests */
        struct TCheckAlsPpSupportRequest
            {
            /** ETel request handle */
            TTsyReqHandle iReqHandle; 
            /** pointer to client-side    data */
            RMmCustomAPI::TAlsSupport* iRetSupport; 
            };
        
        /** used for queuing GetSimFileInfo requests */
        struct TGetSimFileInfoRequest
            {
            /** ETel request handle */
            TTsyReqHandle iReqHandle; 
            /** pointer to client-side data */
            TDes8* iSimFileInfo; 
            };

    public:

        /**
         * Two-phased constructor.
         *
         * @param aMmPhoneTsy Pointer to a MM PhoneTSY
         */
        static CMmCustomTsy* NewL( CMmPhoneTsy* aMmPhoneTsy );

        /**
         * Destructor
         */
        virtual ~CMmCustomTsy();

        /**
         * Deliver code.
         *
         * @param aCodes
         */
        void DeliverCodeL( RMobilePhone::TCodeAndUnblockCode aCodes );

        /**
         * Returns pointer to the Phone object
         *
         * @return Pointer to the Phone object
         */
        CMmPhoneTsy* Phone();

        /**
         * Returns pointer to CMmWim object.
         *
         * @return Pointer to base class of CMmWim object
         */
        CMmSubTsyBase* GetWimTsyPtr() { return iMmSubTsy[ESubTsyIdxWIM]; };

        /**
         * Returns pointer to CMmSs object.
         *

         * @return Pointer to base class of CMmSs object
         */
        CMmCustomExtInterface* GetActiveExtensionPtr()
            { return iMmCustomExtInterface; };

        /**
         * Returns pointer to CMmSimLockTsy object.
         *
         * @return Pointer to base class of CMmSimLockTsy object
         */
        CMmSubTsyBase* GetSimLockTsyPtr()
            { return iMmSubTsy[ESubTsyIdxSimLock]; };

        /**
         * Returns pointer to CMmSIMTsy object.
         *
         * @return Pointer to base class of CMmSIMTsy object
         */
        CMmSubTsyBase* GetSIMTsyPtr() { return iMmSubTsy[ESubTsyIdxSIM]; };

        /**
         * Returns pointer to CMmPacketTsy object.
         *
         * @return Pointer to base class of CMmPacketTsy object
         */
        CMmPacketTsy* PacketTsy()
            { return ( CMmPacketTsy* )iMmSubTsy[ESubTsyIdxPacket]; };

        /**
         * Gives pointer to the array of TSY's subsystems.
         *
         * @return Array of pointers to CMmSubTsyBase classes
         */
        virtual CMmSubTsyBase** GetSubsystemArrayPtr()
            { return &iMmSubTsy[0]; };

        /**
         * Gives the maximun number of TSY's subsystems.
         *
         * @return Number of subsystems
         */
        virtual TInt GetMaxNumberOfSubsystems() { return ESubTsyIdxMaxNum; };

        /**
         * TRAP's all CMmCustomTsy related Custom API requests in case that
         * they fail. This method functions only as a centralized TRAP for the
         * DoExtFuncL method that does the actual mapping of IPC number to
         * TSY method call.
         *
         * @param aTsyReqHandle Request handle
         * @param aIpc IPC number of the request
         * @param aPackage Reference to a data package
         * @return KErrNone/KErrNotSupported
         */
        TInt ExtFunc ( const TTsyReqHandle aTsyReqHandle,
            const TInt aIpc,
            const TDataPackage& aPackage );

        /**
         * Handles extended client requests. Uses TRAP to ensure functioning
         * on memory allocation failure.
         *
         * @param aTsyReqHandle Request handle
         * @param aIpc IPC number of the request
         * @param aPackage Reference to a data package
         * @return Error value
         */
        virtual TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle,
            const TInt aIpc,
            const TDataPackage& aPackage );

        /**
         * Returns request mode for given IPC number.
         *

         * @param aIpc An interprocess communication command identifier
         * @return The ReqMode corresponding to given IPC
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Returns number of slots to be used for given IPC.
         *
         * @param aIpc An interprocess communication command identifier
         * @return The number of slots reserved for a given IPC
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Returns security policy for given IPC.
         *
         * @param aIpc An interprocess communication command identifier
         * @return Security policy for this IPC
         */
        virtual TSecurityPolicy GetRequiredPlatSecCaps( const TInt aIpc );

        /**
         * Cancels request of which IPC number and request handle are given in
         * parameters.
         *
         * @param aIpc An interprocess communication command identifier
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        virtual TInt CancelService( const TInt aIpc,
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Register given notification.
         *
         * @param aIpc An interprocess communication command identifier
         * @return TInt KErrNotSupported
         */
        virtual TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification.
         *
         * @param aIpc An interprocess communication command identifier
         * @return TInt KErrNotSupported
         */
        virtual TInt DeregisterNotification( const TInt aIpc );

        /**
         * Completes a CompleteTerminateCall request
         *
         * @param aError Error code
         */
        void CompleteTerminateCall( TInt aError );

        /**
         * Completes the GetCipheringInfoRequest
         *
         * @param aCipherIndStatus Status of ciphering indicator
         * @param aErrorCode Error code
         */
        void CompleteGetCipheringInfo( TBool aCipherIndStatus,
            TInt aErrorCode );

        /**
         * Completes a NetWakeup request
         *
         * @param aErrorCode Error code
         */
        void CompleteNetWakeup( TInt aErrorCode );

        /**
         * Completes a NotifyCipheringInfoChange request
         *
         * @param aCipherIndStatus Status of ciphering indicator
         * @param aCipherStatus Status of ciphering
         * @param aErrorCode Error code
         */
        void CompleteNotifyCipheringInfoChange( TBool aCipherIndStatus,
            TBool aCipherStatus,
            TInt aErrorCode );

        /**
         * Completes a NotifyNSPSStatus request
         *
         * @param aNspsStatus Status of NSPS (On/Off)
         */
        void CompleteNotifyNSPSStatus( TBool aNspsStatus );

        /**
         * Completes a GetAlsBlocked request
         *
         * @param aBlockStatus Status of ALS block
         * @param aErrorCode Error code
         */
        void CompleteGetAlsBlocked(
            RMmCustomAPI::TGetAlsBlockStatus aBlockStatus,
            TInt aErrorCode );

        /**
         * Completes a SetAlsBlocked request
         *
         * @param aErrorCode Error code
         */
        void CompleteSetAlsBlocked( TInt aErrorCode );

        /**
         * Completes a NotifyAlsBlockedChanged request
         *
         */
        void CompleteNotifyAlsBlockedChanged();

        /**
         * Completes a NotifyAlsBlockedChanged request
         *

         * @param aAlsSupport
         * @param aErrorCode Error code
         */
        void CompleteGetAlsPpSupport( RMmCustomAPI::TAlsSupport aAlsSupport,
            TInt aErrorCode );

        /**
         * Completes a NotifyDtmfEvent request
         *

         * @param aInfo Dtmf info
         * @param aErrorCode Error code
         */
        void CompleteNotifyDtmfEvent( RMmCustomAPI::TDtmfInfo aInfo,
            TInt aErrorCode );

        /**
         * Completes a ClearCallBlackListL request
         *

         * @param aErrorCode Error code
         */
        void CompleteClearCallBlackList( TInt aErrorCode );

        /**
         * Completes emergency number checking request
         *

         * @param aTelNumber Emergency number or NULL if check failed
         * @param aErrorValue Error value
         */
        void CompleteCheckEmergencyNumber(
            RMmCustomAPI::TMobileTelNumber* aTelNumber,
            TInt aErrorValue );

        /**
         * Completes get network provider name request
         *

         * @param aName Network provider name
         * @param aError Error value
         */
        void CompleteGetNetworkOperatorName( TDes* aName, TInt aError );

        /**
         * Completes the notification of SAT refresh to the client
         *

         */
        void CompleteSatRefreshCompleteNotification();

        /**
         * Notify RAU event
         *
         * @param aEventStatus RAU event status
         * @return Symbian error value
         */
        TInt NotifyRauEvent( RMmCustomAPI::TRauEventStatus* aEventStatus );

        /**
         * Completes RAU event notification
         *
         * @param aEventStatus RAU event status
         */
        void CompleteNotifyRauEvent(
                RMmCustomAPI::TRauEventStatus aEventStatus,
                TInt aErrorCode );

        /**
         * Cancels RAU event Notification
         *
         * @param aTsyReqHandle TSY reqhandle
         * @return Symbian error value
         */
        TInt NotifyRauEventCancel();

#ifdef REQHANDLE_TIMER
        /**
         * Calls the needed complete method due timer expiration
         *

         * @param aReqHandleType Request handle type.
         * @param aError Error value
         * @param aIPC IPC value
         */
        void Complete( TInt aReqHandleType, TInt aError, TInt aIPC );
#endif // REQHANDLE_TIMER

        /**
         * Completes checking of two digit dial support
         *

         * @param aTwoDigitDialSupport Two digit dial support status
         * @param aErrorCode Error code
         */
        void CompleteCheckTwoDigitDialSupport(
            RMmCustomAPI::TTwoDigitDialSupport aTwoDigitDialSupport,
            TInt aErrorCode );

        /**
         * Completes a requested NotifySsNetworkEvent
         *

         * @param aSsTypeAndMode ss type and mode
         * @param aSsInfo ss info
         * @param aErrorCode Error code
         */
        void CompleteNotifySsNetworkEvent(
            RMmCustomAPI::TSsTypeAndMode& aSsTypeAndMode,
            RMmCustomAPI::TSsInfo& aSsInfo,
            TInt aErrorCode );

        /**
         * Completes GetOperatorName
         *

         * @param aOperatorNameInfo Programmable operator name info
         * @param aErrorValue Error value
         */
        void CompleteGetOperatorName(
            RMmCustomAPI::TOperatorNameInfo& aOperatorNameInfo,
            TInt aErrorValue );

        /**
         * Completes GetProgrammableOperatorLogo
         *

         * @param aOperatorIdInfo Operator id
         * @param aLogoData Operator logo info
         * @param aErrorValue Error value
         */
        void CompleteGetProgrammableOperatorLogo(
            RMmCustomAPI::TOperatorId* aOperatorIdInfo,
            RMmCustomAPI::TOperatorLogo* aLogoData,
            TInt aErrorValue );

        /**
         * Sets SatRefresh Status
         *

         * @param aSatRefreshStatus SatRefresh status
         */
        IMPORT_C void SetSatRefreshStatus ( TBool aSatRefreshStatus );

        /**
         * Completes a ResetNetServer request
         *

         * @param aErrorValue Error value
         */
        void CompleteResetNetServer( TInt aErrorValue );

        /**
         * Completes a NotifyNetworkConnectionFailure request
         *

         */
        void CompleteNotifyNetworkConnectionFailure();

        /**
         * Completes SsRequestComplete notification
         *

         * @param aStatus SS status
         * @param aError Error variable
         */
        void CompleteSsRequestCompleteNotification( TInt aSsStatus,
            TInt aError );

        /**
         * Completes SsAdditionalInfo notification
         *

         * @param aSsAdditionalInfo
         * @param aError
         */
        void CompleteSsAdditionalInfoNotification(
            RMmCustomAPI::TSsAdditionalInfo* aSsAdditionalInfo,
            TInt aError );

        /**
         * Notifies client when Phonebook cache is ready
         *

         * @param aTsyReqHandle Request handle
         * @param aPndName Phonebookname
         * @return TInt KErrNone
         */
        TInt NotifyPhoneBookCacheReady( const TTsyReqHandle aTsyReqHandle,
            TName* aPndName );

        /**
         * Completes NotifyPhoneBookCacheReady notification
         *

         * @param aPndName Phonebookname
         * @param aResult
         */
        void CompleteNotifyPhoneBookCacheReady( TName& aPndName,
            TInt aResult );

        /**
         * Gets current status of caching
         *

         * @param aTsyReqHandle Request handle
         * @param aPndStatus Status of caching
         * @param aPndName Phonebookname
         * @return TInt KErrNone
         */
        TInt GetPndCacheStatus(const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TPndCacheStatus* aPndStatus,
            const TName* aPndName );

        /**
         * This method updates caching status
         *

         * @param aPndStatus Cache status
         * @param aPndName Phonebookname
         */
        void UpdateCacheStatus( RMmCustomAPI::TPndCacheStatus aPndStatus,
            TName& aPndName );

        /**
         * Cancels notify phonebook cache ready notification
         *

         * @param aTsyReqHandle TSY req handle
         * @return KErrNone
         */
        TInt NotifyPhoneBookCacheReadyCancel(
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Completes a GetSimFileInfo request
         *

         * @param aData File-data retrieved from SIM
         * @param  Error value
         */
        void CompleteGetSimFileInfo( TDesC8* aData,
            TInt aError );

        /**
         * Completes a GetSimFileInfo request

         * @param aLifeTimerData Life timer information
         * @param Error value
         */
        void CompleteGetLifeTime(
            RMmCustomAPI::TLifeTimeData& aLifeTimerData,
            TInt aErrorValue );

        /**
         * Sets diagnostic octets when received
         *

         * @param aCallId Id of the call that has diagnostics
        * @param aDiags The diagnostic octets
         */
        void CompleteGetDiagnosticOctects( TInt aCallId,
            TUint8 aDiags );

        /**
         * Get supported network modes
         *

         * @param aTsyReqHandle Request handle
        * @param aSystemNetworkModes Pointer to network modes
         * @return Error value
         */
        TInt GetSystemNetworkModesL( const TTsyReqHandle aTsyReqHandle,
            TUint32* aSystemNetworkModes );

        /**
         * Complete GetSystemNetworkModes or CurrentNetworkMode request
         *

         * @param aSystemNetworkModes Network modes
         * @param aResult Error value
         */
        void CompleteGetSystemOrCurrentNetworkModes(
            TUint32 aSystemNetworkModes,
            TInt aResult );

        /**
         * Cancel GetSystemNetworkModes request
         *

         * @param aTsyReqHandle TSY req handle
         * @return Error value
         */
        TInt GetSystemNetworkModesCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Set system network mode
         *

         * @param aTsyReqHandle Request handle
        * @param aSystemNetworkModeCaps Pointer to system network mode
         * @return Error value
         */
        TInt SetSystemNetworkModeL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TNetworkModeCaps* aSystemNetworkModeCaps );

        /**
         * Complete SetSystemNetworkMode request
         *

         * @param Error value
         */
        void CompleteSetSystemNetworkMode( TInt aResult );

        /**
         * Get currently selected network mode
         *

         * @param aTsyReqHandle Request handle
        * @param aNetworkMode Pointer to network mode
         * @return TInt KErrNone
         */
        TInt GetCurrentSystemNetworkModeL( const TTsyReqHandle aTsyReqHandle,
            TUint32* aNetworkMode );

        /**
         * Cancel GetCurrentSystemNetworkMode request
         *

         * @param aTsyReqHandle TSY req handle
         * @return TInt KErrNone
         */
        TInt GetCurrentSystemNetworkModeCancel(
            const TTsyReqHandle aTsyReqHandle );

        /**
	     * Get network band
	     *
	     * @param aTsyReqHandle Request handle
	     * @param aSystemNetworkBand Pointer to network bands
	     * @return Error value
	     */
	    TInt GetSystemNetworkBandL( const TTsyReqHandle aTsyReqHandle,
	        RMmCustomAPI::TBandSelection* aSystemNetworkBand,
	        RMmCustomAPI::TNetworkModeCaps* aSystemNetworkMode );

	    /**
	     * Complete GetSystemNetworkBand request
	     *
	     * @param aSystemNetworkBand Network band
	     * @param aResult Error value
	     */
	    void CompleteGetSystemNetworkBand(
	        RMmCustomAPI::TBandSelection aSystemNetworkBand,
	        RMmCustomAPI::TNetworkModeCaps aSystemNetworkMode,
	        TInt aResult );
	        
	    /**
	     * Cancel GetSystemNetworkBand request
	     *
	     * @param aTsyReqHandle TSY req handle
	     * @return Error value
	     */
	    TInt GetSystemNetworkBandCancel( const TTsyReqHandle aTsyReqHandle );

	    /**
	     * Set system network band
	     *
	     * @param aTsyReqHandle Request handle
	     * @param aSystemNetworkBand Pointer to system network band
         * @param aSystemNetworkModeCaps Pointer to system network mode
	     * @return Error value
	     */
	    TInt SetSystemNetworkBandL( const TTsyReqHandle aTsyReqHandle,
	        RMmCustomAPI::TBandSelection* aSystemNetworkBand,
	        RMmCustomAPI::TNetworkModeCaps* aSystemNetworkModeCaps );
	        
	    /**
	     * Complete SetSystemNetworkBand request
	     *
	     * @param Error value
	     */
	    void CompleteSetSystemNetworkBand( TInt aResult );
    
        /**
         * Completes sim authentication request
         *
         * @param aDataPackage Holds authentication data from sim
         * @param aResult Error value
         */
        void CompleteSimAuthentication( CMmDataPackage* aDataPackage,
            TInt aResult );

        /**
         * Completes ims authentication request
         *
         * @param aDataPackage Holds authentication data from sim
         * @param aResult Error value
         */
        void CompleteImsAuthentication( CMmDataPackage* aDataPackage,
            TInt aResult );

        /**
         * Completes a Set Drive Mode request
         *
         * @param aErrorCode
         */
        void CompleteSetDriveMode( TInt aErrorCode );

          /**
         * Completes Get Current System Network Mode
         *
         * @param aCurrentNetwork Network
         * @param aResult Result value
         */
        void CompleteGetCurrentSystemNetworkMode( TUint32 aCurrentNetwork,
            TInt aResult );

        /**
         * Saves call object for completion of emergency nbr check
         *
         * @param aCallObject call object
         */
        void SetObjectForISVDialNumberCheck( CMmVoiceCallTsy* aCallObject );

        /**
         * This checks whether given number is an emergency number
         *
         * @param aTsyReqHandle TSY req handle
         * @param aNumberMode Number to be checked and check mode
         * @param aResult Result of check
         * @return TInt KErrNone
         */
        TInt CheckEmergencyNumberL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TEmerNumberCheckMode* aNumberMode,
            TBool* aResult );

        /**
         * Complete ReadHSxPAStatus request
         *
         * @param aDataPackage Datapackage
         * @param aErrorCode Error code
         */
        void CompleteReadHSxPAStatus( CMmDataPackage* aDataPackage,
            TInt aErrorCode );

        /**
         * Complete WriteHSxPAStatus request
         *
         * @param aErrorCode Error code
         */
        void CompleteWriteHSxPAStatus( TInt aErrorCode );

        /**
         * Complete HSxPA Status notification
         *
         * @param aErrorCode Error code
         */
        void CompleteNotifyHSxPAStatus( CMmDataPackage* aDataPackage,
            TInt aErrorCode );
            
        /**
         * Get Icc Call forwarding parameters
         *
         * @param aTsyReqHandle reghandle
         * @param aCFIndicators pointer to client side data
         * @return error value
         */       
       	TInt GetIccCallForwardingStatusL( TTsyReqHandle aTsyReqHandle, 
       		TDes8* aCFIndicators );
       	
       	/**
         * Completes get icc CF status request
         *
         * @param aDataPackage data package
         * @param aErrorCode Error Code
         */	
       	void CompleteGetIccCallForwardingStatus( 
       		CMmDataPackage* aDataPackage,
       		TInt aErrorCode );

       	/**
         * Sets notification on for CF status change
         *
         * @param aCFIndicators pointer to client side data
         * @return error value
         */
		TInt NotifyIccCallForwardingStatusChange( TDes8* aCFIndicators );
			
		/**
         * Completes icc CF status change notification
         *
         * @param aDataPackage data package
         * @param aErrorCode Error Code
         */
		void CompleteNotifyIccCallForwardingStatusChange( 
			CMmDataPackage* aDataPackage,
    		TInt aErrorCode );
    		
    	/**
         * Compares indicator packages 
         *
         * @param aNotifyCFIndicator indicator to be compared
         * @return Boolean indicating change
         */
    	TBool IsIccCallForwardingStatusChanged( 
    		RMmCustomAPI::TCFIndicators& aNotifyCFIndicator );
    	
    	/**
         * Cancels icc callforwarding notification
         *
         * @param aTsyReqHandle reghandle
         * @return error value
         */	
    	TInt NotifyIccCallForwardingStatusChangeCancel(
    		const TTsyReqHandle aTsyReqHandle );
    		
        /**
         * Complete cell info change notification
         *
         * @param aCellInfo pointer to client side data
         * @return error code
         */
        void CompleteNotifyCellInfoChange( 
            RMmCustomAPI::TMmCellInfo* aCellInfo,
            TInt aErrorCode  );
            
        /**
         * Completes an outstanding GetCellInfo request.
         *
         * @param aCellInfo pointer to client side data
         * @return error value
         */ 
        void CompleteGetCellInfo( RMmCustomAPI::TMmCellInfo* aCellInfo,
            TInt aErrorValue );

        /**
         * Get phonebook cache status
         *
         * @param aPhoneBookType Phonebooktype
         * @return Cache status
         */

        RMmCustomAPI::TPndCacheStatus GetPhonebookCacheStatus( 
            TUint8 aPhoneBookType );

        /**
         * From MCustomVendorExtComplete
         * Completes a request back to the client.
         *
         * @param aTsyReqHandle Request handle
         * @param aError Error code
         */
        void ReqCompleted( const TTsyReqHandle aTsyReqHandle, 
            const TInt aError ); 
        
        /**
         * Change the bootup flag of ICC Callforward.
         * PhoneTSY will need to change the bootupflag after CentRep reset. 
         *
         * @param aBootUp Bootup flag: (ETrue if initial data)
         */
        void SetIccCfBootUpFlag( TBool aBootUp ); 

        /**
         * Complete GetUSIMServiceSupport request
         *
         * @since S60 v3.2
         * @param aDataPackage Datapackage
         * @param aErrorCode Error code
         */
        void CompleteGetUSIMServiceSupport( CMmDataPackage* aDataPackage,
            TInt aErrorCode );
            
        /**
         * Notify RemoteAlertingToneStatusChange
         *
         * @param aToneStatus RemoteToneAlerting status
         * @return Symbian error value
         */
        TInt NotifyRemoteAlertingToneStatusChange( 
            RMmCustomAPI::TRemoteAlertingToneStatus* aToneStatus );

        /**
         * Completes NotifyRemoteAlertingToneStatusChange notification
         *
         * @param aToneStatus RemoteToneAlerting status
         * @param aErrorCode Error code
         */
        void CompleteNotifyRemoteAlertingToneStatusChange(
            RMmCustomAPI::TRemoteAlertingToneStatus aToneStatus,
            TInt aErrorCode );

        /**
         * Cancels RemoteAlertingToneStatusChange Notification
         *
         * @return Symbian error value
         */
        TInt NotifyRemoteAlertingToneStatusChangeCancel();
            
    private:

        /**
         * By default Symbian 2nd phase constructor is private.
         *
         * @param aMmPhoneTsy Pointer to MM Phone Tsy object
         */
        void ConstructL( CMmPhoneTsy* aMmPhoneTsy );

        /**
         * C++ default constructor.
         */
        CMmCustomTsy();

        /**
         * Terminate call/all calls
         *
         * @param aTsyReqHandle TSY req handle
         * @param aCallName Name of the call(s) to be terminated
         * @return TInt KErrNone
         */
        TInt TerminateCallL( const TTsyReqHandle aTsyReqHandle,
            const TName* aCallName );

        /**
         * Notify Dtmf Event
         *
         * @param aInfo Dtmf info
         * @return TInt KErrNone
         */
        TInt NotifyDtmfEvent( RMmCustomAPI::TDtmfInfo* aInfo );

        /**
         * Cancel NotifyDtmfEvent request
         *
         * @param aTsyReqHandle TSY req handle
         * @return TInt KErrNone
         */
        TInt NotifyDtmfEventCancel( const TTsyReqHandle aTsyReqHandle );

         /**
         * Getting diagnostic info
         *
         * @param aTsyReqHandle TSY req handle
         * @param aCallName Call name
         * @return Error value
         */
        
        TInt GetDiagnosticInfo( const TTsyReqHandle aTsyReqHandle,
            TName* aCallName );

        /**
         * Reads the remote alerting tone status
         *
         * @param aTsyReqHandle TSY req handle
         * @param aToneStatus Tone status
         * @return KErrNone
         */
        TInt GetRemoteAlertingToneStatus( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TRemoteAlertingToneStatus* aToneStatus );

        /**
         * Returns the origin of the call
         *
         * @param aTsyReqHandle TSY req handle
         * @param aCallName Call name
         * @param aOrigin Call origin
         * @return KErrNone
         */
        TInt GetCallOrigin( const TTsyReqHandle aTsyReqHandle,
            TName* aCallName,
            RMmCustomAPI::TCallOrigin* aOrigin );

        /**
         * Gets alternating line service blocked status
         *
         * @param aTsyReqHandle TSY req handle
         * @param aBlockStatus Block status
         * @return KErrNone
         */
        TInt GetAlsBlockedL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TGetAlsBlockStatus* aBlockStatus );

        /**
         * Cancel GetAlsBlocked request
         *
         * @return KErrNone
         */
        TInt GetAlsBlockedCancel();

        /**
         * Gets phonebook 3G info
         *
         * @param aTsyReqHandle TSY req handle
         * @param aInfo 3G phonebook info
         * @return KErrNone
         */
        TInt Get3GPBInfo( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::T3GPBInfo* aInfo );

        /**
         * Cancel Get3GPBInfo request
         *
         * @return KErrNone
         */
        TInt Get3GPBInfoCancel();

        /**
         * Sets alternating line service blocked status
         *
         * @param aTsyReqHandle TSY req handle
         * @param aBlockStatus Block status
         * @return KErrNone
         */
        TInt SetAlsBlockedL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TSetAlsBlock* aBlockStatus );

        /**
         * Notifies a client of changed Als blocked status
         *
         * @param aTsyReqHandle TSY req handle
         * @param aBlockStatus Block status
         * @return KErrNone
         */
        TInt NotifyAlsBlockedChanged(
            RMmCustomAPI::TGetAlsBlockStatus* aBlockStatus );

        /**
         * Cancels an Outstanding NotifyAlsBlockedChanged
         *
         * @param aTsyReqHandle TSY req handle
         * @return KErrNone
         */
        TInt NotifyAlsBlockedChangedCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Get ALS pp Support
         *
         * @param aTsyReqHandle TSY req handle
         * @param aSupport
         * @return KErrNone
         */
        TInt GetAlsPpSupportL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TAlsSupport* aSupport );

        /**
         * Cancels an outstanding GetAlsPpSupport function
         *
         * @return KErrNone
         */
        TInt GetAlsPpSupportCancel();

        /**
         * Gets the ciphering info
         *
         * @param aTsyReqHandle TSY req handle
         * @param aInfo Ciphering information
         * @return KErrNone
         */
        TInt GetCipheringInfoL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TCipheringInfo* aInfo );

        /**
         * Cancels an outstanding GetCipheringInfo function
         *
         * @return KErrNone
         */
        TInt GetCipheringInfoCancel();

        /**
         * This function notifies a client of ciphering info change
         *
         * @param aInfo Ciphering information
         * @return KErrNone
         */
        TInt NotifyCipheringInfoChange( RMmCustomAPI::TCipheringInfo* aInfo );

        /**
         * Cancels an outstanding NotifyCipheringInfoChange function
         *
         * @param aTsyReqHandle TSY req handle
         * @return KErrNone
         */
        TInt NotifyCipheringInfoChangeCancel(
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Notifies a client of a change in NSPS (No Service Power Save)
         *
         * @param aNspsStatus Status of the NSPS (on/off)
         * @return KErrNone
         */
        TInt NotifyNSPSStatus( RMmCustomAPI::TNspsStatus* aNspsStatus );

        /**
         * This function cancels an outstanding NotifyNSPSStatus function
         *
         * @param aTsyReqHandle TSY req handle
         * @return KErrNone
         */
        TInt NotifyNSPSStatusCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This function wake ups the net server from the NSPS state
         *
         * @param aTsyReqHandle TSY req handle
         * @return KErrNone
         */
        TInt NetWakeupL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Clears Call Blacklist
         *
         * @param aTsyReqHandle TSY req handle
         * @return KErrNone
         */
        TInt ClearCallBlackListL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Cancels an outstanding ClearCallBlackListL function
         *
         * @return KErrNone
         */
        TInt ClearCallBlackListCancel();

        /**
         * Get the air time use duration synchronously
         *
         * @param aTsyReqHandle Request handle
         * @param aTime Air time duration in seconds.
         * @return KErrNone
         */
        TInt GetAirTimeDuration( const TTsyReqHandle aTsyReqHandle,
            TTimeIntervalSeconds* aTime );

        /**
         * Cancels an outstanding CheckEmergencyNumber function
         *
         * @return KErrNone
         */
        TInt CheckEmergencyNumberCancel();

        /**
         * Gets network operator name
         *
         * @param aNetworkOperatorName Network provider name
         * @return KErrNone
         */
        TInt GetNetworkOperatorNameL( TDes* aNetworkOperatorName );

        /**
         * Cancels an outstanding GetNetworkOperatorName function
         *
         * @return KErrNone
         */
        TInt GetNetworkOperatorNameCancel();

        /**
         * Checks two digit dial support status
         *
         * @param aTsyReqHandle TSY req handle
         * @param aSupport Two digit dial support status
         * @return KErrNone
         */
        TInt CheckTwoDigitDialSupportL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TTwoDigitDialSupport* aSupport );

        /**
         * Cancels an outstanding CheckTwoDigitDialSupport request
         *
         * @return KErrNone
         */
        TInt CheckTwoDigitDialSupportCancel();

        /**
         * Notifies the client of network generated SS events
         *
         * @param aSsTypeAndMode SS type and mode info
         * @param aSsInfo SS general info
         * @return KErrNone
         */
        TInt NotifySsNetworkEvent( 
            RMmCustomAPI::TSsTypeAndMode* aSsTypeAndMode,
            RMmCustomAPI::TSsInfo* aSsInfo );

        /**
         * Cancels an outstanding NotifySsNetworkEvent function
         *
         * @return KErrNone
         */
        TInt NotifySsNetworkEventCancel();

        /**
         * Cancels ss additional information notification
         *
         * @param aTsyReqHandle TSY req handle
         * @return KErrNone
         */
        TInt SsAdditionalInfoNotificationCancel(
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Notification request for additionalinfo message
         *
         * @param aSsAdditionalInfo Ss additional information
         * @return KErrNone
         */
        TInt SsAdditionalInfoNotification(
            RMmCustomAPI::TSsAdditionalInfo* aSsAdditionalInfo );

        /**
         * Gets the programmable operator name info of the current network
         *
         * @param aTsyReqHandle TSY req handle
         * @param aOperatorNameInfo Operator name info
         * @return KErrNone
         */
        TInt GetOperatorNameL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TOperatorNameInfo* aOperatorNameInfo );

        /**
         * Cancels an outstanding GetOperatorName
         *
         * @return KErrNone
         */
        TInt GetOperatorNameCancel();

        /**
         * Gets the programmable operator logo of the current network
         *
         * @param aTsyReqHandle TSY req handle
         * @param aOperatorId
         * @param aLogo
         * @return KErrNone
         */
        TInt GetProgrammableOperatorLogoL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TOperatorId* aOperatorId,
            RMmCustomAPI::TOperatorLogo* aLogo );

        /**
         * Cancels an outstanding GetProgrammableOperatorLogo
         *
         * @return KErrNone
         */
        TInt GetProgrammableOperatorLogoCancel();

        /**
         * Notifies the client of SAT refresh
         *
         * @return KErrNone
         */
        TInt SatRefreshCompleteNotification();

        /**
         * Cancels an outstanding SatRefreshCompleteNotification request
         *
         * @param aTsyReqHandle TSY req handle
         * @return KErrNone
         */
        TInt SatRefreshCompleteNotificationCancel(
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Resets the net server to previous network selection when user
         * doesn't select a network from the network list got in a search
         *
         * @param aTsyReqHandle TSY req handle
         * @return KErrNone
         */
        TInt ResetNetServerL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Notifies of a network connection failure. This failure is so
         * severe that the only way to recover is to restart the phone.
         *
         * @return KErrNone
         */
        TInt NotifyNetworkConnectionFailure();

        /**
         * Cancels an outstanding NotifyNetworkConnectionFailure
         * severe that the only way to recover is to restart the phone.
         *

         * @return KErrNone
         */
        TInt NotifyNetworkConnectionFailureCancel();

        /**
         * Notification request for SS request Complete
         *
         * @param aTsyReqHandle Request handle
         * @param aSsStatus Pointer to a supplementary service status variable
         * @return KErrNone
         */
        TInt SsRequestCompleteNotification( TInt* aSsStatus );

        /**
         * Cancel SsRequestCompleteNotification request
         *
         * @param aTsyReqHandle TSY req handle
         * @return KErrNone
         */
        TInt SsRequestCompleteNotificationCancel(
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Gets a given SIM file's information
         *
         * @param aTsyReqHandle Request handle
         * @param aSimFileInfoPckg Pointer to a packaged object
         * @param aSimResponseBuffer Pointer to client side buffer
         * for storing data
         * @return KErrNone
         */
        TInt GetSimFileInfoL( const TTsyReqHandle aTsyReqHandle,
            TDes8* aSimFileInfoPckg,
            TDes8* aSimResponseBuffer );

        /**
         * Cancel GetSimFileInfo request
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt GetSimFileInfoCancel( TTsyReqHandle aTsyReqHandle );

        /**
         * Gets the life timer's information
         *
         * @param aTsyReqHandle Request handle
         * @param aLifeTimerInfoPckg Pointer to packaged TLifeTimeInfo class
         * @return KErrNone
         */
        TInt GetLifeTimeL( const TTsyReqHandle aTsyReqHandle,
            TDes8* aLifeTimerInfoPckg );

        /**
         * Cancel GetLifeTime request
         *
         * @return KErrNone
         */
        TInt GetLifeTimeCancel();

        /**
         * Sets SIM SMS storage status to "read". Implements CustomAPI
         * method SetSimMessageStatusRead.
         *
         * @param aTsyReqHandle Request handle
         * @param aTime Client-side SMSC timestamp of the SIM-stored message.
         * @param aTimezoneDiff Client-side SMSC timezone difference of the
         * SIM-stored message.
         * @return KErrNone
         */
        TInt SetSimMessageStatusReadL( const TTsyReqHandle aTsyReqHandle,
            TTime& aTime,
            TInt& aTimezoneDiff );

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *
         * @param aReqHandleType
         * @param aTsyReqHandle
         */
        void SetTypeOfResponse( const TInt aReqHandleType,
            const TTsyReqHandle aTsyReqHandle );
#endif //REQHANDLE_TIMER

        /**
         * Checks wether or not a ETel request can be performed while offline
         * mode is enabled
         *
         * @param aIpc Ipc number of the request
         * @return Boolean
         */
        TBool IsRequestPossibleInOffline( TInt aIpc );

        /**
         * Gets authentication data from sim
         *

         * @param aTsyReqHandle Request handle
         * @param aAuthenticationData Pointer to IMS authentication data.
         * @return KErrNone
         */
        TInt ImsAuthenticationL( const TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TImsAuthenticateDataV5* aAuthenticationData );

        /**
         * Cancels sim authenticaiton request
         *
         * @return KErrNone
         */
        TInt ImsAuthenticationCancel();

        /**
         * Gets authentication data from sim
         *
         * @param aTsyReqHandle Request handle
         * @param aAuthData Reference to packed authentication class
         * @return KErrNone
         */
        TInt SimAuthenticationL( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aAuthData );

        /**
         * Cancels sim authenticaiton request
         *
         * @return KErrNone
         */
        TInt SimAuthenticationCancel();

        /**
         * Checks if rand received from network is valid
         *

         * @param aRand RAND
         * @return KErrNone
         */
        TInt CheckRandValidityL( TBuf8<16> aRand );

        /**
         * Creates database to hold rand information
         *

         * @return KErrNone
         */
        TInt CreateRandDb();

        /**
         * Checks rand_db.prv for set bits
         *
         * @param aMessageDigest Hash sum(sha1) of RAND
         * @return KErrNone
         */
        TInt FindSetBitsFromBackup( TPtrC8 aMessageDigest );

        /**
         * Inserts blobs to rand_db
         *

         * @param aMessageDigest Hash sum(sha1) of RAND
         * @return KErrNone
         */
        TInt InsertBlobsToRandDb( TPtrC8 aMessageDigest );

        /**
         * Update bit counter on rand_db
         *

         * @return KErrNone
         */
        TInt UpdateBitCounter();

        /**
         * Sets Drive Mode status
         *
         * @param aTsyReqHandle TSY req handle
         * @param aModeStatus Mode status
         * @return KErrNone
         */
        TInt SetDriveModeL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TSetDriveMode* aModeStatus );

        /**
         * Read HSxPAStatus
         *
         * @param aTsyReqHandle TSY req handle
         * @param aHSxPAStatus HSxPA status
         * @return Error value
         */
        TInt ReadHSxPAStatusL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::THSxPAStatus* aHSxPAStatus );

        /**
         * Cancel ReadHSxPAStatus request
         *
         * @return Error value
         */
        TInt ReadHSxPAStatusCancel();

        /**
         * Write HSxPA Status
         *
         * @param aTsyReqHandle TSY req handle
         * @param aHSxPAStatus HSxPA status
         * @return Error value
         */
        TInt WriteHSxPAStatusL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::THSxPAStatus* aHSxPAStatus );

        /**
         * Cancel WriteHSxPAStatus request
         *
         * @return Error value
         */
        TInt WriteHSxPAStatusCancel();

        /**
         * Set HSxPAStatus notification
         *
         * @param aHSxPAStatus
         * @return Error value
         */
        TInt NotifyHSxPAStatus( RMmCustomAPI::THSxPAStatus* aHSxPAStatus );

        /**
         * Cancel HSxPAStatus notification
         *
         * @return Error value
         */
        TInt NotifyHSxPAStatusCancel();

        /**
         * Gets GSM/WCDMA cell(s) info
         *
         * @param aCellInfoPckg Pointer to packaged TMmCellInfo class
         */
        TInt GetCellInfoL( TDes8* aCellInfoPckg );
         
        /**
         * Cancels an outstanding GetCellInfo request
         *
         * @return Error value
         */   
        TInt GetCellInfoCancel();
        
        /**
         * Notification cell(s) info changes
         *
         * @param aCellInfoPckg Pointer to packaged TMmCellInfo class
         * @return Error value
         */
        TInt NotifyCellInfoChange( TDes8* aCellInfoPckg );

        /**
         * Cancel cell info change notification
         *
         * @return Error value
         */
        TInt NotifyCellInfoChangeCancel();
        /**
         * Read GetUSIMServiceSupport
         *
         * @since S60 v3.2
         * @param aTsyReqHandle TSY req handle
         * @param aAppSupport Application provided data
         * @return Error value
         */
        TInt GetUSIMServiceSupportL( const TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TAppSupport* aAppSupport );

        /**
         * Cancel GetUSIMServiceSupport request
         *
         * @since S60 v3.2
         * @return Error value
         */
        TInt GetUSIMServiceSupportCancel();

        /**
         * Cancels get icc callforwarding request
         *
         * @since S60 v.3.2
         * @param aTsyReqHandle TSY req handle
         * @return KErrNone
         */
        TInt GetIccCallForwardingStatusCancel(
        	const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Cancels GetIccCallForwardingStatus request
         *
         * @return KErrNone
         */
        TInt GetIccCallForwardingStatusCancel();
        
		/**
         * Resets an entry in the req handle store 
         * 
         * @param aTsyReqHandle req handle 
         * @param aIpc IPC value
         * @return EFalse if req handle not found
         */
        virtual TBool ResetReqHandle( const TTsyReqHandle aTsyReqHandle, const TInt aIpc );
        
    private:

        enum TSubTsyIdx
            {
            ESubTsyIdxWIM,
            ESubTsyIdxPacket,
            ESubTsyIdxSs,
            ESubTsyIdxSimLock,
            ESubTsyIdxSIM,
            ESubTsyIdxSecurity,

            // ATTENTION: Let the following constant be the last one
            ESubTsyIdxMaxNum
            };

        /**
         * Pointer to the active extension
         * Own.
         */
        CMmCustomExtInterface* iMmCustomExtInterface;

        /**
         * Array of subsystem pointers
         * Own.
         */
        CMmSubTsyBase* iMmSubTsy[ESubTsyIdxMaxNum];

        /**
         * Ptr to CMmPhoneTsy object
         * Not Own.
         */
        CMmPhoneTsy* iMmPhoneTsy;

        /**
         * Pointer to the Req handle store
         * Own.
         */
        CMmTsyReqHandleStore* iTsyReqHandleStore;

        /**
         * Table for custom request handles
         */
        TTsyReqHandle iCustomReqHandles[ECustomTsyMaxNumOfRequests];

        /**
         * Stores last reserved transaction id number
         */
        TUint8 iTransId;

        /**
         * Saves temporarily last TSY request type
         */
        TCustomRequestType iReqHandleType;

        /**
         * Pointer to Dtmf Info
         * Not Own.
         */
        RMmCustomAPI::TDtmfInfo* iNotifyInfo;

        /**
         * Remote alerting status
         */
        RMmCustomAPI::TRemoteAlertingToneStatus iRemoteAlertingToneStatus;

        /**
         * Pointer to ciphering information
         * Not Own.
         */
        RMmCustomAPI::TCipheringInfo* iRetCipheringInfo;

        /**
         * Pointer to NSPS status information
         * Not Own.
         */
        RMmCustomAPI::TNspsStatus* iRetNspsStatus;

        /**
         * Pointer to notification ciphering information
         * Not Own.
         */
        RMmCustomAPI::TCipheringInfo* iRetNotifyCipheringInfoChange;

        /**
         * Ciphering information
         */
        RMmCustomAPI::TCipheringInfo iCipheringInfo;

        /**
         * Pointer to Als block status for notification
         * Not Own.
         */
        RMmCustomAPI::TGetAlsBlockStatus* iRetNotifyAlsBlockStatus;

        /**
         * Pointer to ALS block status for get
         * Not Own.
         */
        RMmCustomAPI::TGetAlsBlockStatus* iRetAlsBlockStatus;

        /**
         * Pointer to ALS block status for set
         * Not Own.
         */
#ifndef USING_CTSY_DISPATCHER
        RMmCustomAPI::TSetAlsBlock* iSetBlockStatus;
#else //USING_CTSY_DISPATCHER
        const RMmCustomAPI::TSetAlsBlock* iSetBlockStatus;
#endif //USING_CTSY_DISPATCHER

        /**
         * Pointer to ALS support
         * Not Own.
         */
        RMmCustomAPI::TAlsSupport* iAlsSupport;

        /**
         * Pointer to SS additional information
         * Not Own.
         */
        RMmCustomAPI::TSsAdditionalInfo* iRetSsAdditionalInfo;

        /**
         * Pointer to SS request complete information
         * Not Own.
         */
        TInt* iRetSsRequestComplete;

        /**
         * Pointer to check emergency number result
         * Not Own.
         */
        TBool* iEmergencyNumberCheckResult;

        /**
         * Pointer to check emergency number mode
         * Not Own.
         */
        RMmCustomAPI::TEmerNumberCheckMode* iEmergencyNumberCheckMode;

        /**
         * Pointer to get current network mode
         * Not Own.
         */
        TUint32* iCurrentNetworkMode;

        /**
         * Pointer to client's phonebookname buffer
         * Not Own.
         */
        TName* iPndName;

        /**
         * Pointer to network provider name
         * Not Own.
         */
        TDes* iNetworkOperatorName;

        /**
         * Two digit dial support check
         * Not Own.
         */
        RMmCustomAPI::TTwoDigitDialSupport *iTwoDigitDialSupport;

        /**
         * Programmable operator name info
         * Not Own.
         */
        RMmCustomAPI::TOperatorNameInfo* iOperatorNameInfo;

        /**
         * SS type and Mode
         * Not Own.
         */
        RMmCustomAPI::TSsTypeAndMode* iSsTypeAndMode;

        /**
         * SS information
         * Not Own.
         */
        RMmCustomAPI::TSsInfo* iSsInfo;

        /**
         * Operator Id for getting the logo
         * Not Own.
         */
        RMmCustomAPI::TOperatorId* iOperatorId;

        /**
         * Information of the logo
         * Not Own.
         */
        RMmCustomAPI::TOperatorLogo* iLogo;

        /**
         * Is refresh done?
         */
        TBool iIsRefresh;

        /**
         * Programmable operator name info
         * Not Own.
         */
        RMmCustomAPI::TOperatorNameInfo* iRetOperatorNameInfo;

        /**
         * Programmable operator logo info
         * Not Own.
         */
        RMmCustomAPI::TOperatorLogo* iRetOperatorLogoInfo;

        /**
         * Operator Id info
         * Not Own.
         */
        RMmCustomAPI::TOperatorId* iRetOperatorIdInfo;

        /**
         * Pointer to supplementary service status
         * Not Own.
         */
        TInt* iRetSsStatus;

        /**
         * Attribute tells status of the cache
         */
        RMmCustomAPI::TPndCacheStatus iAdnCacheStatus;

        /**
         * Attribute tells status of the cache
         */
        RMmCustomAPI::TPndCacheStatus iFdnCacheStatus;

        /**
         * Pointer to the buffer to hold Sim file information
         * Not Own.
         */
        TDes8* iLifeTimerInfoPckg;

        /**
         * Pointer to get supported network modes
         * Not Own.
         */
        TUint32* iSystemNetworkModes;

        /**
         * Pointer to selected system network mode
         * Not Own.
         */
        RMmCustomAPI::TNetworkModeCaps* iSystemNetworkModeCaps;

        /**
         * Network 3G radio frequency band
	     * not own
	     */
	    RMmCustomAPI::TBandSelection* iSystemNetworkBand;
	    
	    /**
	     * Available network 3G radio frequency band
	     * not own
	     */
	    RMmCustomAPI::TBandSelection* iAvailableSystemNetworkBand;
        
        /**
         * Pointer to Custom Security Tsy
         * Own.
         */
        CMmCustomSecurityTsy* iMmSecurityTsy;

        /**
         * Pointer to 2G authentication data
         * Not Own.
         */
        RMmCustomAPI::TSimAuthenticationEapSim* iEapSim;

        /**
         * Pointer to 3G authentication data
         * Not Own.
         */
        RMmCustomAPI::TSimAuthenticationEapAka* iEapAka;

        /**
         * Number of fresh bits found from RAND
         * Not Own.
         */
        TUint16 iFreshBitCounter;

        /**
         * Pointer to the IMS authentication data
         * Not Own.
         */
        RMobilePhone::TImsAuthenticateDataV5* iIMSAuthenticationData;

        /**
         * Pointer to call object
         * Not Own.
         */
        CMmVoiceCallTsy* iISVDialNumberCheckObject;

        /**
         * Boolean that indicates that number check is called because
         * of 3rd party dial request
         * Not Own.
         */
        TBool iISVDialNumberCheck;

        /**
         * Pointer to client side Rau event data
         * Not Own
         */
        RMmCustomAPI::TRauEventStatus* iRauEventStatus;

        /**
         * HSxPA status
         */
        RMmCustomAPI::THSxPAStatus iHSxPAStatus;

        /**
         * Pointer to client side HSxPA status
         * Not Own.
         */
        RMmCustomAPI::THSxPAStatus* iRetHSxPAStatus;

        /**
         * Pointer to client side HSxPA status
         * Not Own.
         */
        RMmCustomAPI::THSxPAStatus* iSetHSxPAStatus;

        /**
         * Pointer to client side HSxPA status
         * Not Own.
         */
        RMmCustomAPI::THSxPAStatus* iRetNotifyHSxPAStatus;
        
        /**
         * Pointer to client side data
         * Not Own.
         */        
        TDes8* iRetCFIndicators;

        /**
         * TCF indicator
         */
        RMmCustomAPI::TCFIndicators iCurrentTCFIndicator;
		
		/**
         * Pointer to client side data
         * Not Own.
         */ 
		TDes8* iNotifyCFIndicator;
		
		/**
         * Pointer to Public Central Repository
         */
        CRepository* iCFISCentRep;
        
       /**
        * Pointer to Cfis Private Central Repository
        */
        CRepository* iCFISPrivateCentRep;
        
        /**
         * Boolean indicating that request is made during boot-up
         */
        TBool iGetIccCfStatusBootUp;

        /**
         * Pointer to client side notify cell info
         * Not Own.
         */
        RMmCustomAPI::TMmCellInfoPckg* iRetNotifyCellInfoPckg;
                
        /**
         * Pointer to the buffer to hold cell info information
         * Not Own.
         */
        RMmCustomAPI::TMmCellInfoPckg* iCellInfoPckg;

        /**
         * Cell info
         * Not Own.
         */
		RMmCustomAPI::TMmCellInfo iCellInfo;
		
		/**
		 * Used to query for supported features
		 */
		RFeatureControl iFeatureControl;

		/**
		 * Tells if Feature Manager is available. Will be EFalse if 
		 * RFeatureControl::Open fails to connect to the Feature Manager
		 */
		TBool iUsingFeatureManager;        
        /**
         * Pointer to client support structure
         * Not Own.
         */
		RMmCustomAPI::TAppSupport* iAppSupport;

        /**
         * Pointer to client GBA bootstrap structure
         * Not Own.
         */
		RMmCustomAPI::TSimAuthenticationGbaBootstrap* iGbaBootstrap;

        /**
         * Pointer to client GBA update structure
         * Not Own.
         */
		RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate* iGbaBootstrapUpdate;

        /**
         * Pointer to client NAF derivation structure
         * Not Own.
         */
		RMmCustomAPI::TSimAuthenticationGbaNafDerivation* iGbaNafDerivation;

        /**
         * Pointer to client MSK update structure
         * Not Own.
         */
		RMmCustomAPI::TSimAuthenticationMgvMskUpdate* iMgvMskUpdate;

        /**
         * Pointer to client MTK generation structure
         * Not Own.
         */
		RMmCustomAPI::TSimAuthenticationMgvMtkGeneration* iMgvMtkGeneration;

        /**
         * Pointer to client MSK deletion structure
         * Not Own.
         */
		RMmCustomAPI::TSimAuthenticationMgvMskDeletion* iMgvMskDeletion;
		
		/**
         * Pointer to client side RemoteAlertingToneStatus data
         * Not Own
         */
        RMmCustomAPI::TRemoteAlertingToneStatus* iToneStatus;

        /**
         * Array for buffering CheckAlsPpSupport requests
         */             
        RPointerArray< TCheckAlsPpSupportRequest > 
            iCheckAlsPpSupportRequests;
        
        /**
         * Requested SIM file info
         */
        RMmCustomAPI::TSimFileInfo iGetSimFileInfoReq;
		
        /**
	     * Array for buffering GetSimFileInfo requests
	     */             
	    RPointerArray< TGetSimFileInfoRequest > iGetSimFileInfoRequests;
        };

#endif // CMMCUSTOMTSY_H

//  End of File
