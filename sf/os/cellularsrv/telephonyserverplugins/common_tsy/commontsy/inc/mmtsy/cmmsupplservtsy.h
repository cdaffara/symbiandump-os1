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



#ifndef CMMSUPPLSERVTSY_H
#define CMMSUPPLSERVTSY_H

// INCLUDES
#include <e32base.h>  
#include <etelmm.h>   
#include <et_tsy.h>   
#include <mmretrieve.h> 
#include <centralrepository.h>

// FORWARD DECLARATIONS
class TDataPackage;
class CMmPhoneTsy;
class CMmTsyReqHandleStore;
class CMmSupplServGsmWcdmaExt;
class CListReadAllAttempt;
class CMmDataPackage;

// CLASS DECLARATION
/**
 *  Supplementary Services-related functionality of Phone Tsy
 */
NONSHARABLE_CLASS( CMmSupplServTsy ) : public CBase
    {
    private: // Enumerations
    	struct TMwisInfoRequest
    		{
    		/** ETel request handle */
    		TTsyReqHandle iReqHandle;
    		/** pointer to client-side data */
    		RMobilePhone::TMobilePhoneMessageWaitingV1* iMwisInfo;
    		}; 

    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         * @param aPhoneTsy The Phone Tsy object
         */
        static CMmSupplServTsy* NewL( CMmPhoneTsy* aPhoneTsy );

        /**
         * Destructor.
         */
        virtual ~CMmSupplServTsy();

        // New functions
        
        /**
         * Handles Supplementary Service requests
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
         * Cancels Supplementary Service request
         *          
         *
         * @param aIpc request IPC number
         * @param aTsyReqHandle TSY request handle
         * @return result of the request
         */
        virtual TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Calls the needed complete method due timer expiration
         *          
         *
         * @param aReqHandleType req handle type.
         * @param aError error value
         */
        void Complete( TInt aReqHandleType, TInt aError );

        /**
         * Notifies client about call service capabilities change
         *
         * @param aCallServiceCaps The changed call service capabilities
         */
        void CompleteNotifyCallServiceCapsChange(
                RMobilePhone::TMobilePhoneCallServiceCaps aCallServiceCaps ); 

        /**
         * Completes request to retrieve call forwarding status
         *          
         *
         * @param aResults List with results
         * @param aErrorCode error/success code
         * @return None
         */
        void CompleteGetCallForwardingStatusPhase1L( 
			     CMobilePhoneCFList* aResults, TInt aErrorCode );

        /**
         * Completes call forwarding status setting request
         *          
         *
         * @param aErrorCode error/success code
         */
        void CompleteSetCallForwardingStatus( TInt aErrorCode );

        /**
         * Cancels setting of call forwarding status
         *          
         *
         * @param aTsyReqHandle Request handle
         * return KErrNone
         */
        TInt SetCallForwardingStatusCancel(
			const TTsyReqHandle aTsyReqHandle );

        /**
         * Notifies client about forwarding status change
         *
		     * @param aCondition The condition which has a changed status
         */
        void CompleteNotifyCallForwardingStatusChange(
			     RMobilePhone::TMobilePhoneCFCondition aCondition );

        /**
         * Completes asynchronous CallForwardingActive notification
         *          
         *
		     * @param aServiceGroup service group
		     * @param aActiveType type of 
		     * forwarding (conditional/unconditional)e
         */
        void CompleteNotifyCallForwardingActive(
			     RMobilePhone::TMobileService aServiceGroup,
			     RMobilePhone::TMobilePhoneCFActive aActiveType );

        // Call Barring

        /**
         * Completes fetching of call barring status
         *          
         *
         * @param aResults List with results
         * @param aErrorCode error/success code
         */
        void CompleteGetCallBarringStatusPhase1L(
			     CMobilePhoneCBList* aResults, TInt aErrorCode );

        /**
         * Completes barring status setting
         *          
         *
         * @param aErrorCode error/success code
         */
        void CompleteSetCallBarringStatus( TInt aErrorCode );

        /**
         * Notifies client about barring status change
         *          
         *
		     * @param aCondition The condition which has a changed status
         * @return None
         */
        void CompleteNotifyCallBarringStatusChange(
			RMobilePhone::TMobilePhoneCBCondition aCondition );


		    /**
         * Completes SetSSPassword request.
         *          
         *
         * @param aErrorCode Error value
         */
        void CompleteSetSSPassword( TInt aErrorCode );


        /**
         * Complete GetCallWaitingStatusPhase1
         *          
         *
         * @param aResults List with results
         * @param aErrorCode Error code
         */
        void CompleteGetCallWaitingStatusPhase1L( 
			     CMobilePhoneCWList* aResults, TInt aErrorCode );
        
        /**
         * Completes setting of call waiting status
         *          
         *
         * @param aErrorCode error/success code
         */
        void CompleteSetCallWaitingStatus( TInt aErrorCode );
        
        /**
         * Notifies client about change of call waiting status if ordered
         *          
         *
         * @param aChangedCwStatus The changed service group and status
         */
        void CompleteNotifyCallWaitingStatusChange(
		        RMobilePhone::TMobilePhoneCWInfoEntryV1 const* aChangedCwStatus );

        /**
         * completes the GetIdentityServiceStatus method
         *          
         *
         * @param aStatus the returned status
         * @param aErrorCode:error/success code
         */
        void CompleteGetIdentityServiceStatus( 
			     RMobilePhone::TMobilePhoneIdServiceStatus aStatus, 
			     TInt aErrorCode );

        /**
         * Completes network service request
         *          
         *
         * @param aErrorCode error/success code 
         */
        virtual void CompleteSendNetworkServiceRequest( TInt aErrorCode );

        /** 
         * Completes network service request with no FDN check
         *          
         *
         * @param aErrorCode error/success code 
         */
        virtual void CompleteSendNetworkServiceRequestNoFdnCheck( TInt aErrorCode );
        
        /**
         * Completes the GetIccMessageWaitingIndicator request.  
         *          
         *
         * @param aMessageWaiting ICC message waiting indicators
         * @param aErrorCode Error value
         */
        void CompleteGetIccMessageWaitingIndicators( 
            RMobilePhone::TMobilePhoneMessageWaitingV1* aMessageWaiting, 
            TInt aErrorCode );
        
        /**
         * Cancels an outstanding GetIccMessageWaitingIndicators request  
         *          
         *
         * @param aTsyReqHandle TSY request handle,complete with KErrCancel
         * @return result of the request
         */
        TInt GetIccMessageWaitingIndicatorsCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Completes the SetIccMessageWaitingIndicator request.  
         *          
         *
         * @param aErrorCode Error value
         */
        void CompleteSetIccMessageWaitingIndicators( TInt aErrorCode );
        
        /**
         * Cancels an outstanding SetIccMessageWaitingIndicators request 
         *          
         *
         * @param aTsyReqHandle TSY request handle, complete with KErrCancel
         * @return KErrNone
         */
        TInt SetIccMessageWaitingIndicatorsCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Completes the IccMessageWaitingIndicatorChange-notificatio if
         * the indicator values has changed. 
         *          
         *
         * @param aMessageWaiting ICC message waiting indicators
         * @param aErrorCode Error value
         */
        void CompleteNotifyIccMessageWaitingIndicatorChange(
            RMobilePhone::TMobilePhoneMessageWaitingV1* aMessageWaiting,
            TInt& aErrorCode );   
        
        /**
         * Cancels an outstanding NotifyIccMessageWaitingIndicatorChange 
         * request 
         *          
         *
         * @param aTsyReqHandle TSY request handle, complete with KErrCancel
         * @return KErrNone
         */
        TInt NotifyIccMessageWaitingIndicatorChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Set ICC support for message waiting indicators. 
         * request 
         *          
         *
         * @param aSupport ETrue if ICC supports
         */
        void SetIccSupportForMWIS( TBool aSupport ); 

        /**
         * Complete notification for send network requests
         *          
         *
         * @param aErrorCode Error code
         * @param aDataPackage data package
         */
        void CompleteSendNetworkServiceRequestInd( TInt aError,
        	CMmDataPackage* aDataPackage  );
        
        /**
         * Complete notification for all send network requests
         *          
         *
         * @param aErrorCode Error code
         * @param aDataPackage data package
         */
        void CompleteNotifyAllSendNetworkServiceRequest( TInt aError, 
            CMmDataPackage* aDataPackage  );
        
        /**
         * Change the bootup flag of ICC MWIS.
         * PhoneTSY will need to change the bootupflag after CentRep reset.
         *
         * @param aBootUp Bootup flag: (ETrue if initial data)
         */
        void SetIccMwisBootUpFlag( TBool aBootUp );
        
    private:

        /**
         * C++ default constructor.
         */
        CMmSupplServTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * This method returns the combination of the static and dynamic call 
         * service capabilities of the phone.
         *           
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aCaps current capabilities
         * @return KErrNone
         */
        TInt GetCallServiceCaps( const TTsyReqHandle aTsyReqHandle, 
            TUint32* aCaps );

        /**
         * This request allows a client to be notified when the phone's call 
         * service capabilities change
         *           
         *
         * @param aCaps new capabilities
         * @return KErrNone
         */
        TInt NotifyCallServiceCapsChange( TUint32* aCaps );

        /**
         * Cancels order to notify if phone's call service capabilities change
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone
         */
        TInt NotifyCallServiceCapsChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * First phase of fetching call forwarding status.
         *          
         *
         * @param aTsyReqHandle: request handle
         * @param aReqData request data
         * @param aBufSize buffer size
         * @return KErrNone
         */
        TInt GetCallForwardingStatusPhase1L( 
            const TTsyReqHandle aTsyReqHandle, 
            CRetrieveMobilePhoneCFList::TGetCallForwardingRequest* aReqData, 
            TInt* aBufSize );

        /**
         * Second phase of fetching call forwarding status
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aClient client id
         * @param aBuf buffer for status storing
         * @return KErrNone
         */
        TInt GetCallForwardingStatusPhase2( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TClientId const* aClient, 
            TDes8* aBuf );

        /**
         * Cancels order to retrieve call forwarding status
         *          
         *
         * @param aTsyReqHandle request handle
         * @return KErrNone
         */
        TInt GetCallForwardingStatusCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * This method sets a new status and/or registered information for 
         * the call forwarding condition specified by the aCondition parameter
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aCondition forwarding  condition
         * @param aInfo the status and information change to be applied  
         * @return KErrNone
         */
        TInt SetCallForwardingStatusL( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneCFCondition* aCondition, 
            const RMobilePhone::TMobilePhoneCFChangeV1* aInfo );

        /**
         * This request allows a client to be notified when the status or 
         * associated information changes for any of the call forwarding 
         * conditions as applied to any of the basic service groups
         *          
         *
         * @param aCondition forwarding condition
         * @return KErrNone
         */
        TInt NotifyCallForwardingStatusChange( 
            RMobilePhone::TMobilePhoneCFCondition* aCondition );

        /**
         * This notification triggers whenever a call is made on a particular 
         * line and a call forwarding service is active on this line at the 
         * time
         *          
         *
         * @param aServiceGroup Active basic service groups
         * @param aActiveType Active type of CF services
         * @return KErrNone
         */
        TInt NotifyCallForwardingActive( 
            RMobilePhone::TMobileService* aServiceGroup,
            RMobilePhone::TMobilePhoneCFActive* aActiveType );

        /**
         * Cancels order to notify if a call is made on particular line and 
         * a call forwarding service is active on this line at the time
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone
         */
        TInt NotifyCallForwardingActiveCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Cancels order to notify if call forwarding status changes
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone
         */
        TInt NotifyCallForwardingStatusChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * First phase of fetching call barring status
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aReqData request data
         * @param aBufSize size of buffer where status will be stored
         * @return KErrNone
         */
        TInt GetCallBarringStatusPhase1L( const TTsyReqHandle aTsyReqHandle, 
            CRetrieveMobilePhoneCBList::TGetCallBarringRequest* aReqData, 
            TInt* aBufSize );

        /**
         * Second phase of fetching call barring status
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aClient client identity
         * @param aBuf buffer where data will be stored
         * @return KErrNone
         */
        TInt GetCallBarringStatusPhase2( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TClientId const* aClient, 
            TDes8* aBuf );

        /**
         * Cancels fetching of call barring status
         *          
         *
         * @param aTsyReqHandle request handle
         * @return KErrNone
         */
        TInt GetCallBarringStatusCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * This method sets the call barring status for the call barring 
         * condition specified by the aCondition parameter
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aCondition barring condition
         * @param aInfo barring information
         * @return KErrNone
         */
        TInt SetCallBarringStatusL( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneCBCondition* aCondition,
            const RMobilePhone::TMobilePhoneCBChangeV1* aInfo );

        /**
         * Cancels setting of call barring status
         *          
         *
         * @param aTsyReqHandle Request handle
         * return KErrNone
         */
        TInt SetCallBarringStatusCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This request allows a client to be notified when the status or 
         * associated information changes for any of the call barring conditions
         * as applied to any of the basic service groups
         *          
         *
         * @param aCondition Parameter where possible new data will be stored
         * @return KErrNone
         */
        TInt NotifyCallBarringStatusChange(
            RMobilePhone::TMobilePhoneCBCondition* aCondition );

        /**
         * Cancels order to notify if barring status changes
         *          
         *
         * @param aTsyReqHandle request handle
         * @return KErrNone
         */
        TInt NotifyCallBarringStatusChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Set SS password. Leaves checking of password to network
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aPasswordPckg; Password package
         * @param aSsService SS service for password change
         * @return KErrNone
         */
        TInt SetSSPasswordL( const TTsyReqHandle aTsyReqHandle,
            TDes8* aPasswordPckg, TInt const* aSsService );

        /**
         * Cancels setting of SS password
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt SetSSPasswordCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * First phase of fetching call waiting status
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aReqData request data
         * @param aBufSize Size of buffer where data will be stored
         * @return KErrNone
         */
        TInt GetCallWaitingStatusPhase1L( const TTsyReqHandle aTsyReqHandle, 
            CRetrieveMobilePhoneCWList::TGetCallWaitingRequest const* aReqData, 
            TInt* aBufSize );

        /**
         * Second phase of fetching call waiting status
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aClient client identity
         * @param aBuf buffer where data will be stored
         * @return KErrNone
         */
        TInt GetCallWaitingStatusPhase2( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TClientId const* aClient, 
            TDes8* aBuf );

        /**
         * Cancels fetching of call waiting status
         *          
         *
         * @param aTsyReqHandle request handle
         * @return KErrNone
         */
        TInt GetCallWaitingStatusCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This method sets the call waiting status
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aServiceGroup service group where new status will be applied
         * @param aAction action to take
         * @return KErrNone
         */
        TInt SetCallWaitingStatusL( const TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TMobileService* aServiceGroup, 
            RMobilePhone::TMobilePhoneServiceAction* aAction );

        /**
         * Cancels setting of call waiting status
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt SetCallWaitingStatusCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This request allows a client to be notified when the status changes 
         * for the call waiting service as applied to any of the basic service 
         * groups
         *          
         *
         * @param aCWStatus parameter where possible new status will be stored
         * @return KErrNone or KErrArgument
         */
        TInt NotifyCallWaitingStatusChange( TDes8* aCWStatus );

        /**
         * Cancels order to notify if call waiting status changes
         *          
         *
         * @param aTsyReqHandle request handle
         * @return KErrNone
         */
        TInt NotifyCallWaitingStatusChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * This method retrieves the current status of the identity service 
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aServiceAndLocation location whence information will be 
         * retrieved and service to specify the group of requested status
         * @param aStatus variable where information will be gathered
         * @return KErrNone
         */
        TInt GetIdentityServiceStatusL( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TIdServiceAndLocation* aServiceAndLocation,
            RMobilePhone::TMobilePhoneIdServiceStatus* aStatus );

        /**
         * Cancels the request to retrieve identity service status
         *          
         *
         * @param aTsyReqHandle request handle
         * @return KErrNone
         */
        TInt GetIdentityServiceStatusCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * This method sends a network service request string 
         *          
         *
         * @param aTsyReqHandle request handle
         * @param aServiceString request string
         * @return KErrNone
         */
        TInt SendNetworkServiceRequestL( const TTsyReqHandle aTsyReqHandle, 
            const TDesC* aServiceString );
           
        /**
         * Retrieves the set of message waiting indicators 
         * from the current ICC.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aMessageWaiting ICC message waiting indicators
         * @return Error value
         */
        TInt GetIccMessageWaitingIndicatorsL( 
            const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneMessageWaitingV1* aMessageWaiting );
        
        /**
         * Sets the message waiting indicators on the current ICC.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aParams ICC message waiting indicators
         * @return Error value
         */
        TInt SetIccMessageWaitingIndicatorsL( 
            const TTsyReqHandle aTsyReqHandle, 
            TDes8* aParams );        

        /**
         * Notifies if the message waiting indicators on the 
         * current ICC has been changed. 
         *          
         *
         * @param aMessageWaiting ICC message waiting indicators
         * @return KErrNone
         */
        TInt NotifyIccMessageWaitingIndicatorsChange( 
            RMobilePhone::TMobilePhoneMessageWaitingV1*  aMessageWaiting );
            
        /**
         * Read Icc Message Waiting indicators from the Central Repository.
         *          
         *
         * @param aMessageWaiting ICC message waiting indicators
         * @return Error value
         */
        TInt ReadIccMessageWaitingIndicatorsFromCentRep( 
            RMobilePhone::TMobilePhoneMessageWaitingV1& aMessageWaiting );
            
        /**
         * Write Icc Message Waiting indicators to the Central Repository.
         *          
         *
         * @param aMessageWaiting ICC message waiting indicators
         * @return Error value
         */
        TInt WriteIccMessageWaitingIndicatorsToCentRep( const 
            RMobilePhone::TMobilePhoneMessageWaitingV1& aMessageWaiting );
        
        /**
         * Is the Message Waiting indicator -information changed. 
         *          
         *
         * @param aMessageWaiting ICC message waiting indicators
         * @return Etrue if the information is unequal
         */
        TBool IsMwisInfoChanged( const 
            RMobilePhone::TMobilePhoneMessageWaitingV1& aNewMessageWaiting );            

        /**
         * Notification for send network request
         * 
         * @param aOperation Supplies the SS operation to be notified about 
         * @param aRequestComplete operation code and any additional information parameters 
         * @return Error value
         */
        TInt NotifySendNetworkServiceRequest(
            const RMobilePhone::TMobilePhoneNotifySendSSOperation* aOperation,
            TDes8* aRequestComplete );

        /**
         * Cancel notification for send network request         
         *
         * @param aTsyReqHandle TSY request handle
         * @return KErrCancel
         */    
        TInt NotifySendNetworkServiceRequestCancel(
            const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Notification for all send network request
         *           
         *
         * @param aOperation On return, will contain the type of operation i.e. Send SS request invokation, return result/error or reject.
         * @param aRequestComplete operation code and any additional information parameters 
         * @return Error value
         */
        TInt NotifyAllSendNetworkServiceRequest( 
            RMobilePhone::TMobilePhoneNotifySendSSOperation* aOperation,
            TDes8* aRequestComplete );

        /**
         * Cancel notification for all send network request
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return KErrCancel
         */    
        TInt NotifyAllSendNetworkServiceRequestCancel(
            const TTsyReqHandle aTsyReqHandle );            

	private: // member data

       /** 
        * Pointer to phone Tsy
        */		     
        CMmPhoneTsy* iMmPhone;

       /** 
        * Call service capabilities
        */		     
        TUint32 iCallServiceCaps;

        /**
         * Pointer to client side for SS capabilities
         */         
        TUint32* iRetCallServiceCaps;

        /**
         * Flow control is removed and 
         * iSsTransactionOngoing flag is used instead.
         */         
        TBool iSsTransactionOngoing;

        /**
         * Get call forwarding status size buffer (phase 1)
         */         
        TInt* iRetCFStatusSize;

        /**
         * Call forwarding condition
         */         
        RMobilePhone::TMobilePhoneCFCondition* iRetCallForwardingCondition;

        /**
         * Pointer to client side for call forwarding service group changes
         */         
        RMobilePhone::TMobileService* iRetServiceGroup;
        
        /**
         * Pointer to client side for call forwarding active type changes
         */         
        RMobilePhone::TMobilePhoneCFActive* iRetActiveType;

        /**
         * Call forwarding list, in streamed format, for two phased retrieval
         */		     
        CBufBase* iCFList;

        /**
         * Call forwarding client id for two phased list retrieval
         */		     
		    RMobilePhone::TClientId* iCFClientId;
		    
        /**
         * Get call barring status size buffer (phase 1)
         */         
        TInt* iRetCBStatusSize;

        /**
         * Call barring condition
         */         
        RMobilePhone::TMobilePhoneCBCondition* iRetCallBarringCondition;

        /**
         * Call barring list, in streamed format, for two phased retrieval
         */		     
        CBufBase* iCBList;

        /**
         * Call barring client id for two phased list retrieval
         */		     
        RMobilePhone::TClientId* iCBClientId;
		    
        /** 
         * Get call waiting status size buffer (phase 1)
         */         
        TInt* iRetCWStatusSize;

        /**
         * Call waiting list, in streamed format, for two phased retrieval
         */		     
        CBufBase* iCWList;

        /**
         * Call waiting client id for two phased list retrieval
         */		     
        RMobilePhone::TClientId* iCWClientId;

        /**
         * Pointer to client side for call waiting status changes
         */         
        TDesC8* iRetNotifyCallWaitingStatus;

        /**
         * Pointer to client side for identity service status
         */         
        RMobilePhone::TMobilePhoneIdServiceStatus* iRetGetIdentityServiceStatus;
        
        /**
         * Pointer to client side for CFCondition
         */ 
        RMobilePhone::TMobilePhoneCFCondition* iCondition;
    	
        /**
         * Pointer to client side for CFChangeV1
         */ 
    	const RMobilePhone::TMobilePhoneCFChangeV1* iInfo;
    	
       /**
        * Pointer to Mwis Central Repository
        * Not Own.
        */
        CRepository* iMwisCentRep;
       
       /**
        * Pointer to Cfis Central Repository
        */
        CRepository* iCfisCentRep;
        
       /**
        * Pointer to Cfis Private Central Repository
        */
        CRepository* iCFISPrivateCentRep;
             
        /**
         * Up-to-date information of Message Waiting Indicators.
         */
        RMobilePhone::TMobilePhoneMessageWaitingV1 iMessageWaiting; 
        
        /**
         * Pointer to clients Message Waiting Indicators, Set-method.
         * Not Own.
         */
        RMobilePhone::TMobilePhoneMessageWaitingV1* 
            iRetSetMessageWaiting;
       
        /**
         * Pointer to clients Message Waiting Indicators, Notify-method. 
         * Not Own.
         */
        RMobilePhone::TMobilePhoneMessageWaitingV1* 
            iRetNotifyMessageWaiting;
        
        /**
         * Get Icc support during the boot up.
         */
        TBool iGetIccSupportAtBoot;
        
        /**
         * Mwis is supported by ICC.
         */
        TBool iMessageWaitingSupportedByIcc;
        
        /** 
         * Indicating status of NoFdnCheck
         */ 
        enum TSsNoFdnCheckStatus
            {
            ESsNoFdnCheckUnknown, 
            ESsNoFdnCheckUsed,
            ESsNoFdnCheckNotUsed                      
            }; 
 
        /**
         * Indicating NoFdnCheck request usage
         */
        TSsNoFdnCheckStatus iSsNoFdnCheckFlag;

        /**
         * Pointer to client's network service event data, to be populated on receiving notification of an event 
         */  
        RMobilePhone::TMobilePhoneSendSSRequestV3* iRetSSDataPtrIndRequest;
        
        /**
         * Holds the SS operation the client wants to be notified about 
         */
        RMobilePhone::TMobilePhoneNotifySendSSOperation iInputSSOperation;
        
        /**
         * Pointer to client side SS operation
         */        
        RMobilePhone::TMobilePhoneNotifySendSSOperation* iRetSSOperationPtr;
       
        /**
         * Pointer to client side data
         */  
        RMobilePhone::TMobilePhoneSendSSRequestV3* iRetSSDataPtrAllRequest;

        /**
         * Array for buffering MWIS information requests
         */
        RPointerArray< TMwisInfoRequest > iMwisInfoRequests;

    };

#endif  // CMMSUPPLSERVTSY_H            

// End of File
