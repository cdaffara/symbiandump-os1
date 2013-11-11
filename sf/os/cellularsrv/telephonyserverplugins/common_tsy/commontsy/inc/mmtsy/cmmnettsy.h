/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMMNETSY_H
#define CMMNETSY_H

//  INCLUDES
#include <e32base.h>
#include <et_tsy.h>
#include <etelmm.h>
#include "tcachedattribute.h"

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CListReadAllAttempt;
class CMmDataPackage;

// CLASS DECLARATION

/**
 *  CMmPhoneTsy contains mode-independent phone functionality.
 *  Extension request handles and parameters are stored as attributes.
 */
NONSHARABLE_CLASS( CMmNetTsy ) : public CBase
    {
    private:

        /** used to store Network information requests */
        struct TNwInfoRequest
            {
            /** ETel request handle */
            TTsyReqHandle iReqHandle; 
            /** Pointer to notified client-data; Not Own */         
            TPckg<RMobilePhone::TMultimodeType>* iRetNwInfoPckg; 
            };
            
        /** used to store Network information requests, with location */
        struct TNwInfoRequestWithLocation : public TNwInfoRequest
            {
            /** Pointer to notified client-data; Not Own */
            RMobilePhone::TMobilePhoneLocationAreaV1* iRetLocationArea; 
            };

        /** Used for queing TMobilePhoneRegistrationStatus requests */	   
        struct TGetNetworkRegisterationRequest
            {
          	/** ETel request handle */
			TTsyReqHandle iReqHandle;
			/** Pointer to client side data */
			RMobilePhone::TMobilePhoneRegistrationStatus* iRetGetNWRegistrationStatus;
          	};
        
        struct TCellInformationRequest
        	{
          	/** ETel request handle */
			TTsyReqHandle iReqHandle;
            /** Pointer to notified client-data; Not Own */
			TPckg<RMobilePhone::TMultimodeType>* iCellInfoPckg;
        	};
       
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * return created phone object 
         */
        static CMmNetTsy* NewL( CMmPhoneTsy* aPhoneTsy );

        /**
         * Destructor.
         */
        virtual ~CMmNetTsy(void);

        // New functions

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
        TInt DoExtFuncL(
                const TTsyReqHandle aTsyReqHandle, 
                const TInt aIpc, 
                const TDataPackage& aPackage );

        /**
         * Cancels request that's IPC number and request handle are given in
         * parameters
         *          
         *
         * @param aIpc request IPC number
         * @param aTsyReqHandle TSY request handle
         * @return result of the request
         */
        virtual TInt CancelService( 
                    const TInt aIpc, 
                    const TTsyReqHandle aTsyReqHandle );
    
        /**
         * Time info has been change, the client request is completed.
         *          
         *
         * @param aDataPackage
         */
        virtual void CompleteNotifyNITZInfoChange(
            CMmDataPackage* aDataPackage );

        /**
         * signal strength has changed, the client request is completed
         *          
         *
         * @param aSignalBars percentage of bars (0-100%)
         * @param aRssiInDbm signal strength
         */
        virtual void CompleteNotifySignalStrengthChange(
            CMmDataPackage* aDataPackage );

        /**
         * Current network information has changed, the client req is 
         * completed
         *          
         *
         * @param aDataPackage Includes new NW and location info
         * @param aResult Result code
         */
        virtual void CompleteNotifyCurrentNetworkChange(
            CMmDataPackage* aDataPackage, TInt aResult );

        /**
         * The LTSY has completed a cell information get request
         * 
         * @param aDataPackage the data package with the cell information
         * @param aResult the result of the get request
         * */
        virtual void CompleteCellInfoReq(
        		CMmDataPackage* aDataPackage, TInt aResult );
   
        /**
         * The LTSY has completed a request for notifications of changes in the cell
         * information.
         * 
         * @param aDataPackage the data package returned by the LTSY. In this case it
         * will not contain any relevant information.
         * @param aResult the result of the request. KErrNone if the LTSY supports 
         * automatic notification of cell information changes. KErrNotSupported if 
         * the LTSY does not support notification of cell information changes. Any other 
         * standard error codes if there has been an error processing the request.
         * 
         * */
        virtual void CompleteCellInfoIndReq(
        		CMmDataPackage* aDataPackage, TInt aResult );
             
        /**
         * Notifies client when network selection mode changes.
         *          
         *
         * @param aSuccessCode Success code
         */
        virtual void CompleteSetNetworkSelectionSetting( TInt aSuccessCode );

        /**
         * Completes the NotifyNetworkRegistrationStatusChange request
         *          
         *
         * @param aDataPackage the data package with the network registration status
         * @param aResult Result code
         */
        virtual void CompleteNotifyNetworkRegistrationStatusChange( 
            CMmDataPackage* aDataPackage, TInt aResult );

        /**
         * completes the NotifyNetworkSelectionSettingChange request
         *                   
         *
         * @param aSelection a selection
         */
        virtual void CompleteNotifyNetworkSelectionSettingChange( 
            CMmDataPackage* aDataPackage ); 

        /**        
         * Completes GetDetectedNetworksCancel request.
         *          
         *
         * @param aErrorCode Error code
         */
        virtual void CompleteGetDetectedNetworksCancel(
            TInt aErrorCode ) const;

        /**
         * Notifies the client when the network selection setting changes.
         *          
         *
         * @param aErrorCode Error code
         */
        virtual void CompleteSelectNetwork( TInt aErrorCode );

        /**
         * Notifies client when network selection setting is cancelled.
         *          
         *
         * @param aErrorCode Error code
         */
        virtual void CompleteSelectNetworkCancel( TInt aErrorCode );

        /**
         * Completes the NotifyModeChange request
         *          
         *
         * @param aMode Network Mode
         */
        virtual void CompleteNotifyModeChange( CMmDataPackage* aDataPackage,
            TInt aResult );

        /**
         * First phase of fetching detected networks
         *          
         *
         * @param aErrorCode Error code
         * @return KErrNone
         */
        TInt ProcessGetDetectedNetworksPhase1L( CMmDataPackage* aDataPackage,
            TInt aErrorCode );
                                                
        /**
         * Complete Get Network Registration Status
         *          
         *
         * @param aErrorValue EPOC error code
         */
        virtual void CompleteGetNetworkRegistrationStatus( TInt aErrorValue );
        
        /**
         * Get network mode
         *          
         *
         * @return NetworkMode
         */
        RMobilePhone::TMobilePhoneNetworkMode GetNetworkMode() const;

        /**
         * Get the call privacy information and informs all active calls
         *          
         *
         * @param aDataPackage
         */
        void ReceivedCallPrivacyInformation( CMmDataPackage* aDataPackage );

        /**
         * Get the call privacy information and informs all active calls
         *          
         *
         */
        RMobilePhone::TMobilePhonePrivacy& GetCallPrivacyInformation();

        /**
         * Completes an outstanding NotifyNetworkSecurityLevelChange
         *          
         *
         * @param aDataPackage
         */
        void CompleteNotifyNetworkSecurityLevelChange( 
            CMmDataPackage* aDataPackage );

        /**
         * Completes an outstanding GetCipheringIndicatorStatus
         *          
         *
         * @param aDataPackage
         * @param aError
         */
        void CompleteGetCipheringIndicatorStatus( CMmDataPackage* aDataPackage,
            TInt aError );

        /**
         * Returns ETrue if the ciphering indication is forced off.
         *          
         *
         * @return ETrue if the ciphering indication is forced off.
         */
        TBool CipheringIndicatorForcedOff();

        /**
         * Get the known network status
         *          
         *
         * @return status information
         */
        RMobilePhone::TMobilePhoneRegistrationStatus GetNetworkRegistrationStatus() const;
        
        /**
         * Copies buffer size to authorization info client
         * 
         *                  
         * @param CMmDataPackage* aDataPackage: Authorization info
         * @param TInt aError: Error code
         */
        void ProcessGetAuthorizationInfoPhase1L( CMmDataPackage* aDataPackage,
            TInt aError );
            
        /**
         * Completes an outstanding GetCurrentActiveUSimApplicationL,
         * which retrieves AID param from LTSY layer
         *          
         * @param CMmDataPackage* aDataPackage: contains AID parameter from LTSY layer
         * @param TInt aErrorValue: Error value
         */
        void CompleteGetCurrentActiveUSimApplication( 
        	CMmDataPackage* aDataPackage,
        	TInt aErrorValue );                        

    private:
    
        /**
         * C++ default constructor.
         */
        CMmNetTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Handles cancel methods that cause message sending to DOS.
         *          
         *
         * @param aIpc request IPC number
         * @return result of the request
         */
        TInt CancelToDosL( const TInt aIpc );

        /**
         * This function returns signal capabilities
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aCaps Capabilities
         * @return KErrNone
         */
        TInt GetSignalCaps( const TTsyReqHandle aTsyReqHandle, 
            TUint32* aCaps ) const;

        /**
         * This function returns network capabilities
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aCaps Capabilities
         * @return KErrNone
         */
        TInt GetNetworkCaps( const TTsyReqHandle aTsyReqHandle, 
            TUint32* aCaps ) const;

        /**
         * This function returns current mode
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aMode Mode
         * @return KErrNone
         */
        TInt GetCurrentMode( 
            const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneNetworkMode* aMode ) const;

        /**
         * This function cancels GetCurrentMode.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt GetCurrentModeCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This function is used to request mode change notifications.
         *          
         *
         * @param aMode Mode
         * @return KErrNone
         */
        TInt NotifyModeChange( 
            RMobilePhone::TMobilePhoneNetworkMode* aMode );

        /**
         * This function cancels an outstanding mode change notification req
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt NotifyModeChangeCancel( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * This function returns signal strength.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aSignalStrength Signal strength
         * @param aBar Signal bar
         * @return KErrNone
         */
        TInt GetSignalStrength( const TTsyReqHandle aTsyReqHandle, 
            TInt32* aSignalStrength, 
            TInt8* aBar ) const;

        /**
         * This function is used to request signal strength change 
         * notifications
         *          
         *
         * @param aSignalStrength signal strength
         * @param aBar absolute number of bars
         * @return KErrNone
         */
        TInt NotifySignalStrengthChange( TInt32* aSignalStrength, 
            TInt8* aBar );

        /**
         * This function cancels an outstanding signal strength change 
         * notification request
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone
         */
        TInt NotifySignalStrengthChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**        
         * This function returns home network.
         *          
         *         
         * @param aTsyReqHandle Tsy request handle
         * @param aNetworkInfoPckg Pointer to network info
         * @return KErrNone, KErrNotFound or KErrNotSupported
         */
        TInt GetHomeNetwork( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aNetworkInfoPckg ) const;

        /**
         * This function returns current network.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aNetworkInfoPckg network info
         * @param aArea location area
         * @return Return value to the ETel Server
         */
        TInt GetCurrentNetwork( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aNetworkInfoPckg, 
            RMobilePhone::TMobilePhoneLocationAreaV1* aArea ) const;
         
        /**
         * This function returns the cell information.
         *          
         *
         * @param aTsyReqHandle the TSY request handle
         * @param aCellInfo the cell information structure
         * @return Return value to the ETel Server
         */
        TInt GetCellInfoL( const TTsyReqHandle aTsyReqHandle, TDes8* aCellInfoPckg);
        
        /**
         * This function cancels an outstanding cell information getter request.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone if not error happened
         */
        TInt GetCellInfoCancel( 
            const TTsyReqHandle aTsyReqHandle );
        
        /**
         * This functions sends the cell information request to the LTSY.
         * 
         * @param aTsyReqHandle the TSY request handle
         * @param aCellInfoPckg the packaged cell information data structure
         * @return Error code in case of an error
         * */
        TInt DoGetCellInfoReqL(const TTsyReqHandle aTsyReqHandle, 
        		TPckg<RMobilePhone::TMultimodeType>* aCellInfoPckg);
    
        /**
         * This functions is called when new cell information is received. It updates 
         * the cache and completes the client requests.
         * 
         * @param aDataPackage the information received from the LTSY
         * @param aResult the result of the LTSY operation
         * @param aRequests the client requests that need to be completed
         * */
        void HandleCellInfoUpdate(CMmDataPackage* aDataPackage, TInt aResult,
        		RArray< TCellInformationRequest >& aRequests);
        
        /**
         * This function is used to request current cell information change 
         * notifications
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aCellInfoPckg cell info
         * @return KErrNone if not error happened
         */
        TInt NotifyCellInfoChangeL( 
            const TTsyReqHandle aTsyReqHandle,
            TDes8* aCellInfoPckg );

        /**
         * This function cancels an outstanding cell information change
         * notification request.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone if not error happened
         */
        TInt NotifyCellInfoChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );
        
        /**
         * This function do the actual registration for an outstanding cell information change
         * notification request.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone if not error happened
         */
        TInt DoNotifyCellInfoChangeL(const TTsyReqHandle aTsyReqHandle);
        
        /**
         * This function returns current network name.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aNetworkNamePckg network name
         * @param aPlmnPckg location package
         * @return Return value to the ETel Server
         */
        TInt GetNetworkName( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aNetworkNamePckg, 
            TDes8* aPlmnPckg ) const;
                   
        /**
         * This function returns current network, but not the location area.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aNetworkInfoPckg network info
         * @return Return value to the ETel Server
         */
        TInt GetCurrentNetworkNoLocation( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aNetworkInfoPckg ) const;

        /**
         * This function is used to request current network change 
         * notifications
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aNetworkInfoPckg network info
         * @return KErrNone
         */
        TInt NotifyCurrentNetworkNoLocationChangeL( 
            const TTsyReqHandle aTsyReqHandle,
            TDes8* aNetworkInfoPckg );

        /**
         * This function cancels an outstanding current network change
         * notification request.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone
         */
        TInt NotifyCurrentNetworkNoLocationChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * This function is used to request current network change 
         * notifications
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aNetworkInfoPckg network info
         * @param aArea location area
         * @return KErrNone
         */
        TInt NotifyCurrentNetworkChangeL( const TTsyReqHandle aTsyReqHandle,
            TDes8* aNetworkInfoPckg, 
            RMobilePhone::TMobilePhoneLocationAreaV1* aArea );

        /**
         * This function cancels an outstanding current network change
         * notification request.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone
         */
        TInt NotifyCurrentNetworkChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * This function returns current network registration status.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aStatus Registration status
         * @return KErrNone
         */
        TInt GetNetworkRegistrationStatusL( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneRegistrationStatus* aStatus );

        /**
         * This function is used to request network registration status 
         * change notifications.
         *          
         *
         * @param aStatus Registration status
         * @return KErrNone
         */
        TInt NotifyNetworkRegistrationStatusChange( 
            RMobilePhone::TMobilePhoneRegistrationStatus* aStatus );

        /**
         * This function cancels an outstanding network registration 
         * status change notification request.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone
         */
        TInt NotifyNetworkRegistrationStatusChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Set new network selection mode.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aSettingPckg New setting mode
         * @return KErrNone
         */
        TInt SetNetworkSelectionSettingL(
            const TTsyReqHandle aTsyReqHandle, 
            const TDataPackage& aSettingPackage );

        /**
         * Get network selection setting mode.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aSettingPckg New setting mode
         * @return KErrNone
         */
        TInt GetNetworkSelectionSetting( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aSettingPckg );

        /**
         * Notify a change in the network selection setting.
         *          
         *
         * @param aSettingPckg New setting mode
         * @return KErrNone
         */
        TInt NotifyNetworkSelectionSettingChange( TDes8* aSettingPckg );

        /**
         * Notify network selection setting change.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone
         */
        TInt NotifyNetworkSelectionSettingChangeCancel(
            const TTsyReqHandle aTsyReqHandle);
        
        /**
         * Get network time info.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aNWTimeInfo NW time
         * @return KErrNone
         */
        TInt GetNITZInfo( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneNITZ* aNWTimeInfo ) const;
    
        /**
         * Notify network time info change.
         *          
         *
         * @param aNWTimeInfo NW time info
         * @return KErrNone
         */
        TInt NotifyNITZInfoChange(
            RMobilePhone::TMobilePhoneNITZ* aNWTimeInfo );

        /**
         * Cancel notifications of network time info changes.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone
         */
        TInt NotifyNITZInfoChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Client manually selects a new network to register to.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aIsManual Network selection manual true or false
         * @param aManualSelection Manual selection parameter                                           
         * @return KErrNone
         */
        TInt SelectNetworkL( const TTsyReqHandle aTsyReqHandle, 
            const TDataPackage& aDataPackage);

        /**
         * Cancels an outstanding asynchronous SetNetwork request.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt SelectNetworkCancelL( const TTsyReqHandle aTsyReqHandle );

        /**
         * These functions are used to get detected networks.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aClient Client id
         * @param aBufSize Pointer to the buffer size
         * @return KErrNone
         */
        TInt GetDetectedNetworksPhase1L( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TClientId* aClient, 
            TInt* aBufSize );

        /**
         * Second phase of fetching detected networks.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aClient Client id
         * @param aBufSize Pointer to the buffer size
         * @return KErrNone
         */
        TInt GetDetectedNetworksPhase2( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TClientId* aClient, TDes8* aBuf );

        /**
         * Cancels an outstanding GetDetectedNetworks request.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt GetDetectedNetworksCancelL(
            const TTsyReqHandle aTsyReqHandle ) const;

        /**
         * This function cancels network registration status request.
         *          
         *
         * @return KErrNone
         */
        TInt GetNetworkRegistrationStatusCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Gets the security level of the current network
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aNetworkSecurityLevel
         * @return KErrNone
         */
        TInt GetNetworkSecurityLevel( const TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TMobilePhoneNetworkSecurity*
            aNetworkSecurityLevel ) const;

        /**
         * Notifies of the network security level change
         *          
         *
         * @param aNetworkSecurityLevel
         * @return KErrNone
         */
        TInt NotifyNetworkSecurityLevelChange(
            RMobilePhone::TMobilePhoneNetworkSecurity* aNetworkSecurityLevel );
        
        /**
         * Cancels an oustanding NotifyNetworkSecurityLevelChange request
         *          
         *
         * @return KErrNone
         */
        TInt NotifyNetworkSecurityLevelChangeCancel( void );

        /**
         * Gets the status of the ciphering indicator
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aDataPackage
         * @return KErrNone
         */
        TInt GetCipheringIndicatorStatusL( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage&  aDataPackage );

        /**
         * Cancels an outstanding GetCipheringIndicatorStatus request
         *          
         *
         * @return KErrNone
         */
        TInt GetCipheringIndicatorStatusCancel( void );

        /**
         * Checks if two TMobilePhoneNetworkInfoV8 differs
         *          
         *
         * @param aPreviousNetInfo: Previous network info
         * @param aNewNetInfo: New network info
         * @return ETrue if values are not same
         */
        TBool IsNetworkInfoChanged( 
            const RMobilePhone::TMobilePhoneNetworkInfoV8& aPreviousNetInfo,
            const RMobilePhone::TMobilePhoneNetworkInfoV8& aNewNetInfo );

        /**
         * Checks if two TMobilePhoneLocationAreaV1 differs
         *          
         *
         * @param aPreviousNetInfo: Previous network info
         * @param aNewNetInfo: New network info
         * @return ETrue if values are not same
         */
        TBool IsAreaInfoChanged(
            const RMobilePhone::TMobilePhoneLocationAreaV1& aPreviousNetInfo,
            const RMobilePhone::TMobilePhoneLocationAreaV1& aNewNetInfo );

        /**
         * Reads from ini file the values of the variant flags
         *          
         *
         * @return none
         */
        void ReadVariantOptions();

        /**
         * Returns the length of authorization data
         *          
         *         
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @param RMobilePhone::TClientId* aClient: Clients id
         * @param TInt* aBufSize: Pointer to the buffer size
         * @return KErrNone
         */
        TInt GetAuthorizationInfoPhase1L( const TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TClientId* aClient, TInt* aBufSize );

        /**
         * Returns the authorization data
         * 
         *                  
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @param RMobilePhone::TClientId* aClient: Clients id
         * @param TDes8* aBuffer: Pointer to authorization data
         * @return KErrNone
         */
        TInt GetAuthorizationInfoPhase2L( TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TClientId* aClient, TDes8* aBuffer );
            
         /**
         * This function cancels an outstanding request for authorization info
         * 
         *                  
         * @param const TTsyReqHandle aTsyReqHandle: Request handle
         * @return KErrNone
         */
        TInt GetAuthorizationInfoPhase1Cancel();
        
        /**
         * This function gets current active usim application
         * 
         * @param const TTsyReqHandle aTsyReqHandle: Request handle
         * @param aAID pointer to client side data
         * @return TInt: Error value
         */
        TInt GetCurrentActiveUSimApplicationL( 
            const TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TAID* aAID );
        
        /**
         * This function cancels an outstanding request for get current
         * active usim application
         *         
         * @return KErrNone
         */
        TInt GetCurrentActiveUSimApplicationCancel();        

    protected:  // Data
        //None

    private:    // Data

        /**
         * Ptr to phonetsy
         */                  
        CMmPhoneTsy* iMmPhoneTsy;

        /**
         * network registration status
         */         
        RMobilePhone::TMobilePhoneRegistrationStatus iNWRegistrationStatus;

        /**
         * network selection setting status
         */         
        RMobilePhone::TMobilePhoneSelectionMethod iNWSelectionSettingMethod;

        /**
         * signal strength
         */         
        TInt32 iSignalRssi;

        /**
         * pointer to the network registration status.       
         */         
        RMobilePhone::TMobilePhoneRegistrationStatus* 
            iRetNWRegistrationStatus;

        /**
         * pointer to the current network mode      
         */         
        RMobilePhone::TMobilePhoneNetworkMode* iRetNWMode;

        /**
         * pointer to the network selection setting     
         */         
        RMobilePhone::TMobilePhoneNetworkSelectionV1* iRetNWSelectionSetting;

        /**
         * pointer to the date and time
         */                  
        RMobilePhone::TMobilePhoneNITZ* iRetNWTimeInfo;

        /**
         * pointer to the signal strength
         */                  
        TInt32* iRetSignalRssi;

        /**
         * pointer to the location area
         */                   
        RMobilePhone::TMobilePhoneLocationAreaV1* iRetLocationArea;       
          
        /**
         * Stores all handle numbers and client-side pointers 
         * to notify current network information change.
         */         
        RArray< TNwInfoRequestWithLocation > iNotifyCurrentNwRequests;
	    
        /**
         * Stores all handle numbers and client-side pointers 
         * to notify current network information no location change.
         */         
        RArray< TNwInfoRequest > iNotifyCurrentNwNoLocationRequests;

        /**
         * size of the available networks buffer
         */          
        TInt* iRetGetDetectedNwSize;

        /**
         * Network client ID
         */         
        RMobilePhone::TClientId* iRetDetectedNwClientId;

        /**
         * Mode (GSM, DAMPS, CDMA, WCDMA,...)
         */         
        RMobilePhone::TMobilePhoneNetworkMode iMode;

        /**
         * for ProcessGetDetectedNetworksPhase1L
         * Own.         
         */         
        CArrayPtrFlat<CListReadAllAttempt>* iGetDetectedNetworks;

        /**
         * pointer to the absolute number of signal bars
         */                  
        TInt8* iRetSignalBars;

        /**
         * percentage of signal "bars" (0-100%) to be displayed.
         */         
        TInt8 iSignalBars;

        /**
         * pointer to the ciphering indicator
         */         
        RMobilePhone::TMobileCallCipheringIndicator* iRetCipheringIndicator;

        /**
         * pointer to the network security level
         */         
        RMobilePhone::TMobilePhoneNetworkSecurity* iRetNetworkSecurityLevel;

        /**
         * flag for forcing ciphering indicator off 
         */         
        TBool iCipheringIndicatorForcedOff;
        
        /**
         * Size of authorization data buffer
         */         
        TInt* iAuthorizationInfoBufferSize;

        /**
         * Pointer to authorization data client id
         */         
        RMobilePhone::TClientId iAuthorizationInfoClientId;
           
        /**
         * IMS authorization info buffer
         * Own.         
         */         
        HBufC8* iAuthInfoBuffer;
        
        /**
         * Stores all handle numbers and client-side pointers 
         * to network registeration requests
         */
        RPointerArray< TGetNetworkRegisterationRequest > 
            iRegisterationStatusRequests;
       
        /**
         * Stores all handle numbers and client-side pointers 
         * to complete cell information get requests.
         */         
        RArray< TCellInformationRequest > iCellInfoRequests;
        
        /**
         * Stores all handle numbers and client-side pointers 
         * to notify cell information change.
         */         
        RArray< TCellInformationRequest > iNotifyCellInfoRequests;
        
        /**
         *  cell info version 9
         */
        TCachedAttribute<RMobilePhone::TMobilePhoneCellInfoV9> iCachedCellInfoV9;
                    
        /**
         * Pointer to the current active usim application.       
         */         
        RMobilePhone::TAID* iRetCurrentActiveUSimApplication;            
    };

#endif // CMMNETSY_H
            
// End of File
