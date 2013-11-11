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



#ifndef CMMFAXCALLTSY_H
#define CMMFAXCALLTSY_H

//  INCLUDES
#include <et_phone.h>
#include "cmmcalltsy.h"

// FORWARD DECLARATIONS
class CMmFaxLineTsy;

// CLASS DECLARATION

/**
 * CMmFaxCallTsy contains extended fax call functionality that is mode-dependent
 */
NONSHARABLE_CLASS( CMmFaxCallTsy ) : public CMmCallTsy
    {
        friend class CMmFaxExt;
        friend class CMmFaxExtStub;
            
    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         * @param aMmPhone Pointer to the Phone object
         * @param aMmLine Pointer to the Line from which this call object has 
         * been created.
         * @param aMode Call mode
         * @param aName Name of this call
         * @param aMessageManager Pointer to the Message Manager object
         * @return Created call object 
         */
        static CMmFaxCallTsy* NewL( CMmPhoneTsy* aMmPhone, 
            CMmFaxLineTsy* aMmLine, RMobilePhone::TMobileService aMode, 
            TDes& aName, CMmMessageManagerBase* aMessageManager);
        
        /**
         * Destructor.
         */
        ~CMmFaxCallTsy();

        // New functions

        /**
         * Complete status change notification
         *          
         *
         * @param aResult Result of the request
         * @param aDataPackage: new call status value
         */
        void CompleteNotifyStatusChange( TInt aResult, 
            CMmDataPackage* aDataPackage );

        // Functions from base classes

        /**
         * Opens a fax object using given name
         *          
         *
         * @param aName Object name that should be opened
         * @return Opened object 
         */
        CTelObject* OpenNewObjectByNameL( const TDesC& aName );
 
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
        TInt AcquireOwnershipCancel(
            const TTsyReqHandle aTsyReqHandle );
        
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
         * Cancels dialling.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone, KErrNotSupported, KErrInUse or KErrGeneral
         */
        TInt DialCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Answers to an incoming call
         *          
         *
         * @param aTsyReqHandle Request handle from the ETel server
         * @param aCallParams Call parameters
         * @return KErrNone/KErrNotSupported
         */
        TInt AnswerIncomingCall( const TTsyReqHandle aTsyReqHandle,
            const TDesC8* aCallParams );

        /**
         * Cancels answering to an incoming call
         *          
         *
         * @param aTsyReqHandle Request handle from the ETel server
         * @return KErrNone/KErrNotSupported
         */
        TInt AnswerIncomingCallCancel( 
            const TTsyReqHandle aTsyReqHandle );

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
            RCall::TBearerService* );

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
        TInt LoanDataPortCancel( const TTsyReqHandle /*aTsyReqHandle*/ );
        
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

        /**
         * Returns fax settings to the client
         *          
         *
         * @param aTsyReqHandle TSY req handle
         * @param aSettings Fax settings
         * @return KErrNone
         */
        TInt GetFaxSettings( const TTsyReqHandle aTsyReqHandle,
            RCall::TFaxSessionSettings* aSettings );
         
        /**
         * Sets new fax settings
         *          
         *
         * @param aTsyReqHandle TSY req handle
         * @param aSettings Fax settings
         * @return KErrNone
         */
        TInt SetFaxSettings( const TTsyReqHandle aTsyReqHandle,
            const RCall::TFaxSessionSettings* aSettings );

    protected:  // New functions

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
        CMmFaxCallTsy();

    private:  // Functions from base classes

        /**
         * Initialises extension modules
         *          
         *
         * @param RMobilePhone::TMobileService Call mode
         */
        void InitExtensionModulesL( RMobilePhone::TMobileService aMode );

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

    private:  // New functions

        /**
         * Get Data Call RLP Range.
         *          
         *
         * @param aTsyReqHandle
         * @param aRLPVersion
         * @param aRLPRange
         * @return KErrNotSupported
         */
        TInt GetMobileDataCallRLPRange(
            const TTsyReqHandle aTsyReqHandle,
            TInt* /*aRLPVersion*/,
            TDes8* /*aRLPRange*/ );

    private:    // Data

        /**Boolean that indicates if this call object is finished data call.
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

    private:    // Friend classes

        /**
         * Pointer to Internal fax call extension object.
         * Own.         
         */
        class CMmFaxExtInterface* iMmFaxExt;

    };

#endif      // CMMFAXCALLTSY_H
            
// End of File
