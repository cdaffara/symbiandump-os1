// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



// INCLUDE FILES
#include <lbs/epos_privacy.h>
#include    "EPos_PrivacyInternal.h"

#include    "EPos_CPosPrivManager.h"
#include    "EPos_CPosDialogCtrl.h"
#include    "EPos_PosCommonPrivacyResources.h"
#include    "EPos_PrivacyServerDebugPanic.h"

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CPosPrivManager* CPosPrivManager::NewL()
    {
    CPosPrivManager* self = new (ELeave) CPosPrivManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPrivManager::~CPosPrivManager()
    {
    Cancel();
    if (iRequestInfo)
    	{
    	delete iRequestInfo;
    	}
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPrivManager::CPosPrivManager()
    : CActive(CActive::EPriorityStandard),
      iDlgCtrl(PosCommonPrivacyResources::DialogCtrl())
    {
    CActiveScheduler::Add(this);
    iActivity = EPosInactive;
    }

// EPOC default constructor can leave.
void CPosPrivManager::ConstructL()
    {
    iRequestInfo = new (ELeave) TLbsExternalRequestInfo2;
    }

// ---------------------------------------------------------
// CPosPrivManager::VerifyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPrivManager::VerifyL(
    TRequestStatus& aRequestStatus,
    const TLbsExternalRequestInfo& aRequestInfo,
    const TPosRequestData& aRequestData,
    TInt aRequestId)
    {
    if (iActivity != EPosInactive)
        {
        TRequestStatus* status = &aRequestStatus;
        User::RequestComplete(status, KErrOverflow);
        return;
        }

    Mem::Copy(iRequestInfo,
    		  &aRequestInfo,
    		  aRequestInfo.ClassSize());
    iRequestStatus = &aRequestStatus;
    *iRequestStatus = KRequestPending;
    iRequestData = aRequestData;
    iRequestId = aRequestId;
    iActivity = EPosResolvingForVerification;

    ResolveL();
    }

// ---------------------------------------------------------
// CPosPrivManager::CancelVerify
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPrivManager::CancelVerify()
    {
    if (IsVerifying())
        {
        User::RequestComplete(iRequestStatus, KErrCancel);
        Cancel();
        iActivity = EPosInactive;
        }
    }

// ---------------------------------------------------------
// CPosPrivManager::CancelVerifyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPrivManager::CancelVerifyL(
    TPosVerifyCancelReason aCancelReason)
    {
    if (!IsVerifying() && iActivity != EPosInactive)
        { // Notification is running. Cancel is then not allowed.
        User::Leave(KErrOverflow);
        }

    switch (iActivity)
        {
        case EPosVerifying:
            {
            //Cancel could occur on four places during the state EPosVerifying.
            //1. The request is queued in CPosDlgCtrl
            //2. The request is cancelled when the dialog is shown.
            //3. The user has dismissed the dialog but CPosDlgCtrl::RunL has not
            //   yet been run by the AS.
            //4. The user has dismissed the dialog, CPosDlgCtrl::RunL has been
            //   run, however, CPosPrivManager::RunL has not. In this case, it
            //   is not necessary to cancel CPosDialogCtrl since it has already
            //   completed the request.
            //
            //Case 3 and 4 are treated the same way, i.e. the
            //user will be notified.
            //
            if (iRequestData.iTimeoutStrategy != EPosDecisionNotAvailable)
                {
                if (iStatus == KRequestPending)
                    {
                    TInt err;
                    TBool requestQueued;
                    iDlgCtrl->CancelLaunch(iStatus, err, iUserResponse,
                        requestQueued, aCancelReason);

                    if (requestQueued)
                        {
                        iActivity = EPosCancelBeforeDialog; //Case 1
                        }
                    else if (err == KErrArgument ||
                        err == KErrNotReady &&
                            iUserResponse != KRequestPending || //S60
                        err == KErrNone &&
                            iUserResponse != KRequestPending)   //S90
                        {
                        iActivity = EPosCancelAfterDialog2; //Case 3
                        }
                    else
                        {
                        iActivity = EPosCancelDuringDialog; //Case 2
                        }
                    }
                else
                    {
                    iActivity = EPosCancelAfterDialog1; //Case 4.
                    }
                }
            else
                {
                CancelVerify();
                }
            break;
            }
        case EPosInactive:
            {
            //A cancel request is received after user response,
            //but client has not yet received the request.
            if (iRequestData.iTimeoutStrategy != EPosDecisionNotAvailable)
                {
                TInt result = iLastVerificationResult == KErrNone ?
                    KErrNone : KErrAccessDenied;
                TInt reqDecision = iRequestData.iTimeoutStrategy ==
                    EPosDecisionRejected ? KErrAccessDenied : KErrNone;
                if (result != reqDecision)
                    {
                    NotifyPluginL();
                    }
                }
            break;
            }
        default: break;
        }
    }

// ---------------------------------------------------------
// CPosPrivManager::NotifyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPrivManager::NotifyL(
    TRequestStatus& aRequestStatus,
    const TLbsExternalRequestInfo& aRequestInfo,
    const TPosRequestData& aRequestData,
    TInt aRequestId)
    {
    if (iActivity != EPosInactive)
        {
        TRequestStatus* status = &aRequestStatus;
        User::RequestComplete(status, KErrOverflow);
        return;
        }

    iRequestStatus = &aRequestStatus;
    *iRequestStatus = KRequestPending;

    iRequestData = aRequestData;
    iRequestId = aRequestId;
    
    TLbsExternalRequestInfo2* reqInfo = new (ELeave) TLbsExternalRequestInfo2;
    Mem::Copy(reqInfo, 
    		  &aRequestInfo,
    		  aRequestInfo.ClassSize());
    delete iRequestInfo;
    iRequestInfo = reqInfo;
    
    iActivity = EPosResolvingForNotification;
    ResolveL();
    }

// ---------------------------------------------------------
// CPosPrivManager::RunL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPrivManager::RunL()
    {
    if (iStatus.Int() == KErrNoMemory)
        {
        User::Leave(KErrNoMemory);
        }

    if (iActivity == EPosResolvingForVerification)
        {
        // Resolving is done. Start verification
        iActivity = EPosVerifying;

        // Launch the verification dialog
        iDlgCtrl->VerifyL(iStatus, *iRequestInfo, iRequestData);
        SetActive();
        }
    else if (iActivity == EPosResolvingForNotification)
        {
        if (iRequestData.iNotificationReason == EPosVerificationTimeout)
        // Notify verification timeout
            {
            NotifyPluginL();
            }
        else // Notify (without previous verification)
            {
            iDlgCtrl->NotifyL(*iRequestInfo, iRequestData);
            iRequestInfo = NULL;
            iRequestInfo = new (ELeave) TLbsExternalRequestInfo2;
            }

        // Notify message not completed until now, in order to prevent
        // parallell notifications
        User::RequestComplete(iRequestStatus, KErrNone);
        iActivity = EPosInactive;
        }
    else if (iActivity == EPosVerifying)
        {
        CompleteRequestL(iStatus.Int());
        }
    else
        {
        if (iActivity == EPosCancelBeforeDialog  &&
            iRequestData.iTimeoutStrategy == EPosDecisionAccepted)
            {
            iRequestData.iNotificationReason = EPosDecisionByRequestSource;
            iDlgCtrl->NotifyL(*iRequestInfo, iRequestData);
            iRequestInfo = NULL;
            iRequestInfo = new (ELeave) TLbsExternalRequestInfo2;
            }
        else if (iActivity == EPosCancelAfterDialog1
            ||   iActivity == EPosCancelAfterDialog2)
            {
            TInt userResponse;
            if (iActivity == EPosCancelAfterDialog1)
                {
                userResponse = iStatus.Int() == KErrNone ?
                        KErrNone : KErrAccessDenied;
                }
            else
                {
                userResponse = iUserResponse == KErrNone ?
                        KErrNone : KErrAccessDenied;
                }

            TInt timeoutStrategy = iRequestData.iTimeoutStrategy ==
                EPosDecisionRejected ? KErrAccessDenied : KErrNone;

            if (userResponse != timeoutStrategy)
                {
                NotifyPluginL();
                }
            }
        User::RequestComplete(iRequestStatus, KErrCancel);
        iActivity = EPosInactive;
        }
    }

// ---------------------------------------------------------
// CPosPrivManager::DoCancel
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPrivManager::DoCancel()
    {
    if (PosCommonPrivacyResources::Installed())
        {
        if (iActivity == EPosVerifying)
            {
            iDlgCtrl->CancelLaunch(iStatus);
            }
        }
    }

// ---------------------------------------------------------
// CPosPrivManager::RunError
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPrivManager::RunError(TInt aError)
    {
    User::RequestComplete(iRequestStatus, aError);
    iActivity = EPosInactive;
    return KErrNone;
    }

// ---------------------------------------------------------
// CPosPrivManager::ResolveL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPrivManager::ResolveL()
    {
    // No resolving of requestors is implemented
    iStatus = KErrNone;
    RunL();
    }

// ---------------------------------------------------------
// CPosPrivManager::CompleteRequestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPrivManager::CompleteRequestL(TInt aCompleteCode)
    {
    iLastVerificationResult = aCompleteCode;

    TInt result = aCompleteCode ==
        KErrNone ? KErrNone : KErrAccessDenied;
    User::RequestComplete(iRequestStatus, result);
    iActivity = EPosInactive;
    }

// ---------------------------------------------------------
// CPosPrivManager::NotifyPluginL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPrivManager::NotifyPluginL()
    {
    if (iRequestData.iRequestDecision == EPosDecisionNotAvailable)
        {
        iRequestData.iRequestDecision = iRequestData.iTimeoutStrategy;
        }

    iRequestData.iTimeoutStrategy = EPosDecisionNotAvailable;
    iRequestData.iNotificationReason = EPosVerificationTimeout;
    iRequestData.iCancelReason = EPosCancelReasonNotAvailable;

    iDlgCtrl->NotifyL(*iRequestInfo, iRequestData);
    iRequestInfo = NULL;
    iRequestInfo = new (ELeave) TLbsExternalRequestInfo2;
    }

// ---------------------------------------------------------
// CPosPrivManager::IsVerifying
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPrivManager::IsVerifying() const
    {
    switch (iActivity)
        {
        case EPosInactive:
        case EPosResolvingForNotification:
            return EFalse;
        case EPosResolvingForVerification:
        case EPosVerifying:
        case EPosCancelBeforeDialog:
        case EPosCancelDuringDialog:
        case EPosCancelAfterDialog1:
        case EPosCancelAfterDialog2:
            return ETrue;
        default:
            //DebugPanic(EPosPrivSrvPanicUnknownActivity);
            break;
        }
    return EFalse;
    }

//  End of File
