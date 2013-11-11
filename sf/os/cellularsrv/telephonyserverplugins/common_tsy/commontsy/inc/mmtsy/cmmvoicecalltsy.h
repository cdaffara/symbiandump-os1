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



#ifndef CMMVOICECALLTSY_H
#define CMMVOICECALLTSY_H

//  INCLUDES
#include "cmmcalltsy.h"
#include <ctsy/rmmcustomapi.h>

#include "ctsysystemstatepluginhandler.h"

// FORWARD DECLARATIONS
class CMmVoiceLineTsy;
class MTelephonyAudioControl;

// CLASS DECLARATION

/**
 *  CMmVoiceCallTsy contains extended voice call functionality that 
 *  is mode-dependent.
 */
NONSHARABLE_CLASS( CMmVoiceCallTsy ) : public CMmCallTsy, 
                                       public MCtsySsmPluginCallback                                       
    {
    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         * @param aMmPhone Pointer to the Phone object
         * @param aMmLine Pointer to the Line from which this call object has 
         * been created.
         * @param aMode Call mode
         * @param aName Name of this call
         * @param aMessageManager Pointer to the Message Manager object
         * @param aTelephonyAudioControl Pointer Telephony Audio Control        
         * @return Created call object 
         */
        static CMmVoiceCallTsy* NewL( CMmPhoneTsy* aMmPhone, 
            CMmVoiceLineTsy* aMmLine, 
		    RMobilePhone::TMobileService aMode, 
            TDes& aName, 
            CMmMessageManagerBase* aMessageManager,
            MTelephonyAudioControl* aTelephonyAudioControl );
        
        /**
         * Destructor.
         */
        ~CMmVoiceCallTsy();

        // New functions

        /**
         * Complete emergency call
         *          
         *
         * @param aResult Result of the request
         */
        void CompleteDialEmergencyCall( TInt aResult );

        /**
         * Complete hang up
         *          
         *
         * @param aResult Result of the request
         */
        void CompleteHangUp( TInt aResult );

        /**
         * Complete hold request
         *          
         *
         * @param aResult Result of the request
         */
        void CompleteHold( TInt aResult );

        /**
         * Complete resume request
         *          
         *
         * @param aResult Result of the request         
         */
        void CompleteResume( TInt aResult );

        /**
         * Complete swap request
         *          
         *
         * @param aResult Result of the request
         */
        void CompleteSwap( TInt aResult );

        /**
         * Completes GoOneToOne request
         *          
         *
         * @param aErrorCode error code
         */
        void CompleteGoOneToOne( TInt aErrorCode );

        /**
         * Complete status change notification
         *          
         *
         * @param aResult Result of the request
         * @param aDataPackage: new call status value
         */
        void CompleteNotifyStatusChange( TInt aResult, 
            CMmDataPackage* aDataPackage );

        /**
         * Fills current parameters of mobile call info 
         *          
         *
         * @param aInfo Pointer to mobile call information
         * @return Return value to ETel server
         */
        TInt FillMobileCallInfo( TDes8* aInfo );

#ifdef REQHANDLE_TIMER
        /**
         * Calls the appropriate complete method due timer expiration
         *          
         *
         * @param aReqHandleType TSY request handle type.
         * @param aError Error value for completion
         */
        void Complete( TInt aReqHandleType, TInt aError );
#endif
		/**
         * Completes emergency nbr check for 3rd party clients
         *          
         * @param aIsEmergencyNbr boolean indicating check result
         */
		void Complete3rdPartyCallNbrCheck( TBool aIsEmergencyNbr );

    public: // Functions from base classes

        /**
         * Returns request mode for given IPC number
         *          
         *
         * @param aIpc IPC number of the request
         * @return Request mode
         */
        CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Returns number of slots to be used for a given IPC
         *          
         *
         * @param aIpc IPC number of the request
         * @return Number of slots for this request
         */
        TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Cancels request that's IPC number and request handle are given as
         * parameters
         *          
         *
         * @param aIpc IPC number of the request
         * @param aTsyReqHandle Request handle from ETel server
         * @return KErrNone/KErrNotSupported
         */
        TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Register given notification
         *          
         *
         * @param aIpc IPC number of the request
         * @return TInt Result of the request
         */
        TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification
         *          
         *
         * @param aIpc IPC number of the request
         * @return Result of the request
         */
        TInt DeregisterNotification( const TInt aIpc );

        /**
         * Dials a call.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aCallParams Call parameters
         * @param aTelNumber Phone number to be called
         * @return KErrNone or KErrNotSupported
         */
        TInt Dial( const TTsyReqHandle aTsyReqHandle, 
            const TDesC8* aCallParams, TDesC* aTelNumber );

        /**
         * Answers to an incoming call
         *          
         *
         * @param aTsyReqHandle Request handle from the ETel server
         * @param aCallParams Call parameters
         * @return KErrNone/KErrNotSupported
         */
        TInt AnswerIncomingCall( const TTsyReqHandle aTsyReqHandle,
            const TDesC8* /*aCallParams*/ );

        /**
         * Hangs up the call
         *          
         *
         * @param aTsyReqHandle Request handle from ETel server
         * @return KErrNone/KErrNotSupported
         */
        TInt HangUp( const TTsyReqHandle aTsyReqHandle );

        /**
         * Returns bearer service information to the client.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aBearerService Contains Bearer service info on return
         * @return KErrNotSupported
         */
        TInt GetBearerServiceInfo( const TTsyReqHandle /*aTsyReqHandle*/,
            RCall::TBearerService* /*aBearerService*/ );

        /**
         * Connects to an existing call (data calls).
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aCallParams Call parameters
         * @return KErrNotSupported
         */
        TInt Connect( const TTsyReqHandle /*aTsyReqHandle*/,
            const TDesC8* /*aCallParams*/ );
        
        /**
         * Cancels connecting of a (data) call
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNotSupported
         */
        TInt ConnectCancel( const TTsyReqHandle /*aTsyReqHandle*/ );

        /**
         * Loans the dataport to the client.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCommPort Comm port
         * @return KErrNotSupported
         */
        TInt LoanDataPort( const TTsyReqHandle /*aTsyReqHandle*/,
            RCall::TCommPort* /*aCommPort*/ );
                
        /**
         * Cancels dataport loaning.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNotSupported
         */
        TInt LoanDataPortCancel( const TTsyReqHandle /*aTsyReqHandle*/ );

        /**
         * Recovers the dataport from the client.
         *          
         *
         * @param aTsyReqHandle
         * @return KErrNotSupported
         */
        TInt RecoverDataPort( const TTsyReqHandle /*aTsyReqHandle*/ );

        /**
         * Recovers the dataport from the client and relinquishes ownership.
         *          
         *
         * @return KErrNotSupported
         */
        TInt RecoverDataPortAndRelinquishOwnership();
        
        /**
         * Dial a call with NoFdn check
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aCallParams Call parameters
         * @param aTelNumber Phone number to be called
         * @return Symbian error value 
         */
        TInt DialNoFdnCheck( const TTsyReqHandle aTsyReqHandle, 
            const TDesC8* aCallParams, TDesC* aTelNumber );
            
    public: // Functions from MCtsySsmPluginCallback
     	/**
         * This method is called from CMmCallGsmWcdmaExt, after 
         * sendinig request to LTSY         
         *
         * @param aResult -- result, returned either from SSM or from LTSY 
         */
         void SsmPluginCallback(TInt aResult, TCtsySsmCallbackData& aCallbackData);    	        

    protected:  // New functions

        /**
         * This method requests a private communication to the remote party of 
         * one call within a conference call.
         *          
         *
         * @param aTsyReqHandle Request handle from ETel server
         * @return KErrNone/KErrNotSupported
         */
        TInt GoOneToOneL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Is this call an emergency call
         *          
         *
         * @return TRUE if emergency call, otherwise FALSE
         */
        TBool IsEmergencyCall() const;

        /**
         * Change emergency call mode
         *
         * @param aIsEmergency is call emergency call or normal call
         */
        void SetEmergencyMode( TBool aIsEmergency );

    private:  // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CMmVoiceCallTsy();
        
        // new functions
        
        /**
         * Establishes call routing control and updates its state.
         * 
         * @param aCallName Call name
         * @param aMobileCallStatus Call status
         */
        void UpdateCallRoutingControl(  const TName& aCallName, 
		                                RMobileCall::TMobileCallStatus aMobileCallStatus );

    private:  // Functions from base classes

        /**
         * Initialises miscellaneous internal attributes
         *          
         *
         */
        void InitInternalAttributes();

        /**
         * DoExtFuncL is called by the server when it has a "extended", 
         * i.e. non-core ETel request for the TSY. To process a request handle,
         * request type and request data are passed to the TSY. 
         *          
         *
         * @param aTsyReqHandle Request handle from ETel server
         * @param aIpc IPC number of the request
         * @param aPackage Reference to the input parameters.
         * @return KErrNone/KErrNotSupported
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, const TDataPackage& aPackage );

        /**
         * Sets the call on hold
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt HoldL( const TTsyReqHandle aTsyReqHandle );
 
        /**
         * Resumes a held call
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt ResumeL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Swaps between the active and held call
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt SwapL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Creates an emergency call
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aNumber Emergency number
         * @return Return value to the ETel Server
         */
        TInt DialEmergencyCall( const TTsyReqHandle aTsyReqHandle, 
            const TDataPackage& aNumber);
        
        /**
         * Cancels creation of an emergency call
         *
         *          
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt DialEmergencyCallCancel( const TTsyReqHandle aTsyReqHandle );

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *          
         *
         * @param aReqHandleType TSY request handle type.
         * @param aTsyReqHandle TSY request handle to be stored.
         */
        void SetTypeOfResponse( const TInt aReqHandleType, 
            const TTsyReqHandle aTsyReqHandle );
#endif

    public: //Data
          		
        /**
         * Boolean indicating number check state for 3rd party clients
         */        
        TBool i3rdPartyEmergencyNumberCheckDone;

        /**
         * Emergency number check mode 
         */  		 
        RMmCustomAPI::TEmerNumberCheckMode iNumberCheckMode;

    private: //Data

        /**
        * Temporary store for call params
        */        
        const TDesC8* iISVCallParams;

        /**
         * Dial made by 3rd Party client
         */
        TBool iIsDialISV;

    };

#endif      // CMMVOICECALLTSY_H
            
// End of File
