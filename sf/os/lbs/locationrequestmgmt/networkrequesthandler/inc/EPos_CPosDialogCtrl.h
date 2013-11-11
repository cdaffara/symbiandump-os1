/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/

#ifndef CPOSDIALOGCTRL_H
#define CPOSDIALOGCTRL_H

//  INCLUDES
#include <e32base.h>
#include <lbs/epos_cposrequestor.h>
#include <lbs/epos_rposrequestorstack.h>
#include <lbs/epos_tposqninputdata.h>
#include "EPos_PrivacyInternal.h"
#include <lbs/lbsloccommon.h>

// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Base class for controlling plugin privacy dialogs.
*  Uses the notifier plugin framework.
*/
class CPosDialogCtrl : public CActive
    {
    public:  // Constructor

        /**
        * Two-phased constructor.
        */
        static CPosDialogCtrl* InstanceL();

        /**
        * Destructor.
        */
        virtual ~CPosDialogCtrl();

    public: // New functions

        /**
        * Queues a launch of the notification dialog.
        * If no leave occurs, this class takes ownership of the entry list.
        * Do not call Close() or ResetAndDestroy() on the entry list after
        * function has been called.
        * @param aEntryList The entries involved in the request.
        * @param aRequestData Contains data for notification requests.
        */
        void NotifyL(
        				TLbsExternalRequestInfo& aRequestInfo,
        /* IN  */   	const TPosRequestData& aRequestData
        );

        /**
        * Queues a launch of the verification dialog.
        * @param aStatus Asynchronous request status.
        * @param aEntryList The entries to verify.
        * @param aRequestData Contains data for verification requests.
        */
        void VerifyL(
        /* IN/OUT */    TRequestStatus&         aStatus,
        				TLbsExternalRequestInfo& aRequestInfo,
        /* IN     */    const TPosRequestData&      aRequestData
        );

        /**
        * Cancels a verification request.
        * @param aStatus Asynchronous request status for the request to cancel.
        */
        void CancelLaunch(
        /* IN/OUT */    TRequestStatus&         aStatus
        );

         /**
        * Cancels a verification request.
        * @param aStatus Asynchronous request status for the request to cancel.
        * @param aErrorCode An error code.
        * @param aResponse The user response
        * @param aRequestQueued Whether the request is queued or not.
        * @param aCancelReason The cancel reason.
        */
        void CancelLaunch(
        /* IN/OUT */    TRequestStatus&         aStatus,
        /* OUT */       TInt&                   aErrorCode,
        /* OUT */       TInt&                   aResponse,
        /* OUT */       TBool&                  aRequestQueued,
        /* IN  */       TPosVerifyCancelReason aCancelReason =
                        EPosCancelReasonNotAvailable
        );

        /**
        * Returns whether or not the dialog is showing.
        * @return ETrue if a request is active, EFalse otherwise.
        */
        TBool DialogActive() const;

    protected: // Functions from base classes

        /**
        * From CActive
        *
        * Handles request completion.
        */
        void RunL();

        /**
        * From CActive
        *
        * Implements cancel of an outstanding request.
        */
        void DoCancel();

    private:

        struct TRequest
            {
            TPosQNInputData::TPosRequestType iType;
            TPosQNRequestId iId;
            TLbsExternalRequestInfo* iRequestInfo;
            TRequestStatus* iStatus;
            TPosRequestSource iRequestSource;
            TPosRequestDecision iTimeoutStrategy;
            TPosRequestDecision iRequestDecision;
            TPosVerifyCancelReason iCancelReason;
            TPosNotificationReason iNotificationReason;
            };

        /**
        * C++ default constructor.
        */
        CPosDialogCtrl();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CPosDialogCtrl( const CPosDialogCtrl& );
        // Prohibit assigment operator
        CPosDialogCtrl& operator= ( const CPosDialogCtrl& );

        void NextRequest();

        void DoCompleteRequest(
        /* IN     */    TInt aIndex,
        /* IN     */    TInt aCompletionCode
        );

        void ScheduleRequestL(
        /* IN     */    TRequest& aRequest
        );

        TInt StartNotifierRequest();

        void CreateSerializedStackL(
        /* OUT    */    TDes8& aBuffer,
        /* IN */		const RPosRequestorStack& aRequestStack
        );

        TInt CalcSize(
        /* IN */		const RPosRequestorStack& aRequestStack
        );

        void ExtractRequestorsL(
     		const TLbsExternalRequestInfo& aRequestInfo,
         	RPosRequestorStack& aRequestors
        );
        
        void AddRequestorL(
     		CPosRequestor::TRequestorType aRequestorType,
     		TLbsExternalRequestInfo::TFormatIndicator aIdType,
     	    CPosRequestor::TRequestType aRequestType,
     	    CPosRequestor::TNetworkType aNetworkType,
     		const TDesC& aId,
     		RPosRequestorStack& aRequestors
         );

        void GivePriorityToVerificationL(
        /* IN     */    TRequest& aRequest
        );
        
        void EnqueueRequestL(
        /* IN     */    TRequest& aRequest
        );

        void DeferNotificationL();
        void CreateNotificationAndEnqueueL();
        void UpdateProperty();
		
    private:    // Data
        RNotifier                       iNotifier;
        RArray<TRequest>                iRequestQueue;
        TInt                            iRequestId;
        TBool                           iNotifierStarted;
        TBool                           iNotifierActive;
    };

#endif      // CPOSDIALOGCTRL_H

// End of File
