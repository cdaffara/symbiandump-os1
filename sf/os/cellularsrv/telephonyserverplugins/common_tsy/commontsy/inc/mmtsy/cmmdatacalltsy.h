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



#ifndef CMMDATACALLTSY_H
#define CMMDATACALLTSY_H

//  INCLUDES
#include "cmmcalltsy.h"

// FORWARD DECLARATIONS
class CMmDataLineTsy;

// CLASS DECLARATION

/**
 * CMmDataCallTsy contains extended data call functionality that is 
 * mode-dependent.
 */
NONSHARABLE_CLASS( CMmDataCallTsy ) : public CMmCallTsy
    {
    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         * @param aMmPhone Pointer to the Phone object
         * @param aMmLine Pointer to the Line from which this call object
         * has been created.
         * @param aMode Call mode
         * @param aName Name of this call
         * @param aMessageManager Pointer to the Message Manager object
         * @return CMmDataCallTsy* Created call object
         */
        static CMmDataCallTsy* NewL( CMmPhoneTsy* aMmPhone, 
            CMmDataLineTsy* aMmLine, RMobilePhone::TMobileService aMode,
            TDes& aName, CMmMessageManagerBase* aMessageManager );
        
        /**
         * Destructor.
         */
        ~CMmDataCallTsy();

        // New functions

        /**
         * Complete dial request
         *          
         *
         * @param aResult Result of the request
         */
        void CompleteDial( TInt aResult );

        /**
         * Complete dialnofdn request
         *          
         * @param aResult Result of the request
         */
        void CompleteDialNoFdn( TInt aResult );

        /**
         * Complete hang up
         *          
         *
         * @param aResult Result of the request
         */
        void CompleteHangUp( TInt aResult );

        /**
         * Complete answering to the incoming call
         *          
         *
         * @param aResult Result of the request
         */
        void CompleteAnswerIncomingCall( TInt aResult );

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
         * Complete a NotifyMobileDataCallCapsChange request.
         *          
         *
         */
        void CompleteNotifyMobileDataCallCapsChange();

        /**
         * Complete a SetDynamicHscsdParams request.
         *          
         *
         * @param aErrorCode Error value for completion
         */
        void CompleteSetDynamicHscsdParams( TInt aErrorCode );

        /**
         * Complete a NotifyHscsdInfoChange request.
         *          
         *
         */
        void CompleteNotifyHscsdInfoChange();

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
         * @param aIpc: IPC number of the request
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
         * @return Result of the request
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
         * Transfers call ownership.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return KErrNone
         */
        TInt TransferOwnership( const TTsyReqHandle aTsyReqHandle );

        /**
         * Acquires ownership.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return KErrNone
         */
        TInt AcquireOwnership( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Cancels method for AcquireOwnership.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return KErrNone
         */
        TInt AcquireOwnershipCancel( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Relinquishes ownership.
         *          
         *
         * @return KErrNone
         */
        TInt RelinquishOwnership();

        /**
         * Get call ownership status.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aOwnershipStatus Ownership status
         * @return KErrNone
         */
        TInt GetOwnershipStatus(
            const TTsyReqHandle aTsyReqHandle,
            RCall::TOwnershipStatus* aOwnershipStatus );

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
         * Dials a call using NoFdnCheck feature.
         *          
         * @param aTsyReqHandle Tsy request handle
         * @param aCallParams Call parameters
         * @param aTelNumber Phone number to be called
         * @return KErrNone or KErrNotSupported
         */
        TInt DialNoFdnCheck( const TTsyReqHandle aTsyReqHandle, 
            const TDesC8* aCallParams, TDesC* aTelNumber );            

#ifdef TF_LOGGING_ENABLED       
        /**
         * Overloads original ReqCompleted for logging purposes
         *          
         * @param aTsyReqHandle TSY request handle
         * @param aError error value
         */
        virtual void ReqCompleted( const TTsyReqHandle aTsyReqHandle,
            const TInt aError );
#endif

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
         * Cancels answering to an incoming call
         *          
         *
         * @param aTsyReqHandle Request handle from the ETel server
         * @return KErrNone/KErrNotSupported
         */
        TInt AnswerIncomingCallCancel( const TTsyReqHandle aTsyReqHandle );

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
         * @return KErrNone
         */
        TInt GetBearerServiceInfo( const TTsyReqHandle aTsyReqHandle,
            RCall::TBearerService* aBearerService );

        /**
         * Set correct data call attributes, depending on parameter extension.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aCallParams Call parameters
         * @return KErrNone/KErrArgument
         */
        TInt Connect( const TTsyReqHandle aTsyReqHandle,
            const TDesC8* aCallParams );
        
        /**
         * Cancels connecting of a (data) call
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone
         */
        TInt ConnectCancel( const TTsyReqHandle /*aTsyReqHandle*/ );

        /**
         * Loans the dataport to the client.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCommPort Comm port
         * @return KErrNone
         */
        TInt LoanDataPort( const TTsyReqHandle aTsyReqHandle,
            RCall::TCommPort* aCommPort );
                
        /**
         * Cancels dataport loaning.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone
         */
        TInt LoanDataPortCancel( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Recovers the dataport from the client.
         *          
         *
         * @param aTsyReqHandle
         * @return KErrNone
         */
        TInt RecoverDataPort( const TTsyReqHandle aTsyReqHandle );

        /**
         * Recovers the dataport from the client and relinquishes ownership.
         *          
         *
         * @return KErrNone
         */
        TInt RecoverDataPortAndRelinquishOwnership();

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

    protected: // New functions

        /**
         * Get data call caps.
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: Tsy request handle
         * @param TDes8* aCaps: Capabilities
         * @return KErrNone
         */
        TInt GetMobileDataCallCaps( const TTsyReqHandle aTsyReqHandle,
            TDes8* aCaps );

        /**
         * Notifies client when the data call capabilities change.
         *          
         *
         * @param aTsyReqHandle TSY Request handle
         * @param aCaps Contains Mobile call capabilities on return
         * @return KErrNone
         */
        TInt NotifyMobileDataCallCapsChange(
            const TTsyReqHandle /*aTsyReqHandle*/,
            TDes8* aCaps );

        /**
         * Cancels MobileDataCallCaps change notifications.
         *          
         *
         * @param aTsyReqHandle TSY request handle to be cancelled
         * @return KErrNone
         */
        TInt NotifyMobileDataCallCapsChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Set HSCSD dynamic data parameters.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aAiur,
         * @param aRxTimeslots
         * @return KErrNone 
         */
        TInt SetDynamicHscsdParams( const TTsyReqHandle aTsyReqHandle,
            RMobileCall::TMobileCallAiur* aAiur, TInt* aRxTimeslots );

        /**
         * Cancel set HSCSD dynamic data parameters.
         *          
         *
         * @param aTsyReqHandle
         * @return KErrNone 
         */
        TInt SetDynamicHscsdParamsCancel( 
            const TTsyReqHandle /*aTsyReqHandle*/ ) const;

        /**
         * Get current HSCSD info.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aHSCSDInfo Current HSCSD info
         * @return KErrNone
         */
        TInt GetCurrentHscsdInfo( const TTsyReqHandle aTsyReqHandle,
            TDes8* aHSCSDInfo );

        /**
         * Notify HSCSD info change.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aHSCSDInfo Contains current HSCSD info on return
         * @return TInt KErrNone
         */
        TInt NotifyHscsdInfoChange( const TTsyReqHandle /*aTsyReqHandle*/,
            TDes8* aHSCSDInfo );

        /**
         * Cancels HSCSD info change notifications.
         *          
         *
         * @param aTsyReqHandle TSY request handle to be cancelled
         * @return KErrNone
         */
        TInt NotifyHscsdInfoChangeCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Starts to answer an incoming call.
         *          
         *
         */
        void CallComingForWaitingCall();

        /**
         * Returns ETrue if the call object is "used" data call object
         *          
         *
         * @return Value of iIsFinishedDataCall boolean
         */
        TBool IsFinishedData() const;

    private:  // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CMmDataCallTsy();

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
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Resets the status of the call
         *          
         *
         */
        void ClearCallStatus();

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

    private: // New functions

        /**
         * Get Data Call RLP Range.
         *          
         *
         * @param aTsyReqHandle
         * @param aRLPVersion
         * @param aRLPRange
         * @return KErrNotSupported
         */
        TInt GetMobileDataCallRLPRange( const TTsyReqHandle aTsyReqHandle,
            TInt* /*aRLPVersion*/,
            TDes8* /*aRLPRange*/ );

    private: // Data

        /**
         * Boolean that indicates if this call object is finished data call.
         * Required as Symbian CSD agent in situation where first data call
         * fails due wrong number and it then opens second data call, which
         * dials correct number. Call status notifications go to wrong call
         * object (the first one) and causes problems for clients.
         */         
        TBool iIsFinishedDataCall;

        /**
         * Dataport loaned to client
         */         
        RCall::TCommPort iLoanedCommPort;

        /**
         * Pointer to data call caps
         */         
        TDes8* iRetDataCallCaps;

        /**
         * Pointer to HSCSD info
         */         
        TDes8* iRetHscsdInfo;

    };

#endif      // CMMDATACALLTSY_H
            
// End of File
