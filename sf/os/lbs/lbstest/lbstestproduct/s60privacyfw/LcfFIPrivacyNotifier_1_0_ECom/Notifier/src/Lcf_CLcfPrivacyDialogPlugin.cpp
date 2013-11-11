// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <techview/eikinfo.h>
#include <EPos_CPosRequestor.h>
#include <EPos_CPosServiceRequestor.h>
//#include <EPos_CPosContactRequestor.h>
#include <EPos_CPosRequestor.h>
#include <eiknotapi.h>

#include <eikdef.h>

#include "Lcf_CLcfPrivacyDialogPlugin.h"

const TUid KScreenOutputChannel = { 0x100065ac };

// ================= MEMBER FUNCTIONS =======================


// C++ default constructor can NOT contain any code, that
// might leave.
//
CLcfPrivacyDialogPlugin::CLcfPrivacyDialogPlugin()
	: iDialogLaunched(EFalse)
    {
    iInfoDialog = NULL;
    }

// Destructor
CLcfPrivacyDialogPlugin::~CLcfPrivacyDialogPlugin()
    {
    delete iControlFilesManager;
    if (iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }
    }


// Two-phased constructor
CLcfPrivacyDialogPlugin* CLcfPrivacyDialogPlugin::NewL()
    {
    CLcfPrivacyDialogPlugin* self = new (ELeave) CLcfPrivacyDialogPlugin;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// EPOC default constructor can leave.
void CLcfPrivacyDialogPlugin::ConstructL()
    {
    this->BaseConstructL(KScreenOutputChannel, MEikSrvNotifierBase2::ENotifierPriorityVHigh);

    iControlFilesManager = CLcfNotifierManager::NewL(EFalse);
    }

// ---------------------------------------------------------
// CLcfPrivacyDialogPlugin::HandleNewRequestL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CLcfPrivacyDialogPlugin::HandleNewRequestL(/*IN*/ TPosQNRequestId aRequestId)
    {
    // If control data exists, then try to handle request automatically
    // otherwise handle it manually by displaying request on screen.

    // Note that current implementation can only hande one request at
    // a time.

    if (iControlFilesManager->ControlDataExists())
        {
        if (iControlFilesManager->ResetRequested())
            {
            iNotificationCount = 0;
            iVerificationCount = 0;
            }

        TRAPD(err, HandleNextRequestAutomatisedL(aRequestId));

        // If automatic handling failed, then handle request
        // manually
        if (err != KErrNone)
            {
            HandleNextRequestL(aRequestId);
            }
        }
    else
        {
        HandleNextRequestL(aRequestId);
        }

    
  }

// ---------------------------------------------------------
// CLcfPrivacyDialogPlugin::HandleNextRequestL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CLcfPrivacyDialogPlugin::HandleNextRequestAutomatisedL(TPosQNRequestId aRequestId)
  {
    SetCurrentRequestL(aRequestId);

    TInt numEntries = RequestorCountL();

    TReal delay;

    TRequestType requestType = RequestTypeL(aRequestId);

    // Construct diagnostics. ASSERT is put here because current implementation
    // can only handle one request at a time.
    __ASSERT_ALWAYS(!iDiagnostics, User::Panic(_L("Diagnostics exist"),1));
    iDiagnostics = CLcfPluginDiagnostics::NewL();

    // Read expected behaviour
    if (requestType == EQuery)
        {
        iControlFilesManager->GetVerificationAnswerL(iVerificationCount, iAnswer, delay);
        iVerificationCount++;
        iDiagnostics->SetIsVerificationRequest(ETrue);
        }
    else
        {
        iControlFilesManager->GetNotificationAnswerL(iNotificationCount, iAnswer, delay);
        iNotificationCount++;
        iDiagnostics->SetIsVerificationRequest(EFalse);
        }

    // New functionality allows Completion of any request with any error code, for
    // this reason we have to read some more information
    if (iAnswer == ELcfComplete)
        {
        iControlFilesManager->GetCompletionAnswer(iStyleToComplete, iRequestIdToComplete, iCompletionCode);
        }

    // Store requestor info
    iDiagnostics->SetRequestId(aRequestId);
    
    // Store each requestor
    for (TInt i=0; i < numEntries;i++)
        {
        CPosRequestor* req = RequestorLC(i);

        CLcfRequestor* requestorInfo = CLcfRequestor::NewLC();

        requestorInfo->iType = req->RequestorType();
        requestorInfo->iFormat = req->RequestorIdFormat();
        requestorInfo->iIdString.Copy(req->RequestorIdString());
		requestorInfo->iSource = RequestSource();
		
		
        // Initialize to EFalse in order to simplify if-clauses
        requestorInfo->iHasDataBaseRef = EFalse;

        if (requestorInfo->iType == CPosRequestor::ERequestorService)
            {
            /*CPosServiceRequestor* myServiceReq = (CPosServiceRequestor*) req;

            if (myServiceReq->HasDatabaseReference())
                {
                requestorInfo->iHasDataBaseRef = ETrue;
                requestorInfo->iDataBaseRef = myServiceReq->ServiceItemId();
                requestorInfo->iDataBase.Copy(_L(""));   
                }*/

            }
        else if (requestorInfo->iType == CPosRequestor::ERequestorContact)
            {
            
            //CPosContactRequestor* myContactReq = (CPosContactRequestor*) req;
            // Todo: Harley
            /*
            CPosRequestor* myContactReq = (CPosRequestor*) req

            if (myContactReq->HasDatabaseReference())
                {
                requestorInfo->iHasDataBaseRef = ETrue;
                requestorInfo->iDataBaseRef = myContactReq->ContactItemId();
                requestorInfo->iDataBase.Copy(myContactReq->ContactDatabase());   
                }*/
            }

        // Currently there is a bug in the interface, and it is
        // impossible to know if there are multiple
        // matches or not
        // --> Set to EFalse always
        requestorInfo->iHasMultipleMatches = EFalse; 
        
        iDiagnostics->AddRequestorLD(requestorInfo);

        CleanupStack::Pop(); // requestorInfo

        CleanupStack::PopAndDestroy(); // req
        }


    // If delay is needed, then start timer, otherwise request can be completed immediately
    if (delay == 0.0)
        {
        DismissDialog();
        return;
        }


    // Start timer
    if (iTimer)
        {
        delete iTimer;
        iTimer = NULL;
        }

    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);

    TTimeIntervalMicroSeconds32 microSecDelay(TInt(delay * 1000000));

    iTimer->Start(microSecDelay, 0,TCallBack(TimerCallBack, this));
}
// ---------------------------------------------------------
// CLcfPrivacyDialogPlugin::HandleNextRequestL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CLcfPrivacyDialogPlugin::HandleNextRequestL(/*IN*/ TPosQNRequestId aRequestId)
  {
  
  SetCurrentRequestL(aRequestId);

  TInt nr = RequestorCountL();
  TBuf<100> buf;
  _LIT(KTEXT, "LCF: Accept req. from");
  TBuf<250> buf1(KTEXT);

  _LIT(KContact, "Contact ");
  _LIT(KService, "Service ");
  _LIT(KUnknown, "Unknown ");

  _LIT(KTEXT1, "LCF: Notificat. from");
  TBuf<250> buf2(KTEXT1);
  
  TInt type = 0;
  
  for (TInt i=0;i<nr; i++)
    {
    // Can currently display only one requestor
    if (i == 1) 
        break;
    CPosRequestor* req = RequestorLC(i);
    buf = req->RequestorIdString();
    type = req->RequestorType();
    CleanupStack::PopAndDestroy(req);
    }

  TBuf<8> bufType;

  switch (type)
    {
  case CPosRequestor::ERequestorService:
      bufType.Append(KService);
    break;
    case CPosRequestor::ERequestorContact:
      bufType.Append(KContact);
    break;
    default:
      bufType.Append(KUnknown);
    break;
    }

  TRequestType result = RequestTypeL(aRequestId);
  TInt completionCode = -1;

  if (result == ENotification)
    {
    _LIT(KTITLE, "-Notification-");
    buf2.Append(bufType);
    buf2.Append(buf);

    CEikInfoDialog* infoDialog = new (ELeave) CEikInfoDialog(KTITLE, buf2);
    CleanupStack::PushL(infoDialog);
    iInfoDialog = infoDialog;
    
    TBool result = infoDialog->RunDlgLD(KTITLE, buf2, infoDialog->EAllowEnter);
    
    if (result)
      {
      // OK Pressed
      completionCode = KErrNone;
      CompleteRequest(aRequestId, completionCode);
      }
    else 
      {
      // REJECT Pressed
      completionCode = KErrNone;
      CompleteRequest(aRequestId, completionCode);
      }

    CleanupStack::PopAndDestroy(infoDialog);

    }
  else if (result == EQuery)
    {
    _LIT(KTITLE, "-Privacy Query-");
    buf1.Append(bufType);
    buf1.Append(buf);
    
    CEikInfoDialog* infoDialog = new (ELeave) CEikInfoDialog(KTITLE, buf1);
    CleanupStack::PushL(infoDialog);
    iInfoDialog = infoDialog;


    TBool result = infoDialog->RunDlgLD(KTITLE, buf1, infoDialog->EAllowEnter);
    // Yes Pressed
    if (result)
      {
      completionCode = KErrNone;
      CompleteRequest(aRequestId, completionCode);
      }
    else 
      {
      completionCode = KErrAccessDenied;
      CompleteRequest(aRequestId, completionCode);
      }

    CleanupStack::PopAndDestroy(infoDialog);
    }
  }


// ---------------------------------------------------------
// CLcfPrivacyDialogPlugin::
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CLcfPrivacyDialogPlugin::HandleRequestCancelled(/*IN*/ TPosQNRequestId /*aRequestId*/)
  {
    if (iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }

    if (iDiagnostics)
        {
        iDiagnostics->SetExitReason(ELcfExitCancelledByEPos);

        // This function must not leave
        TRAP_IGNORE(iControlFilesManager->AppendDiagnosticsL(iDiagnostics));

        delete iDiagnostics;
        iDiagnostics = NULL;
        }
    else if (iInfoDialog) // dialog is on screen, must dismiss it
        {
        TKeyEvent keyEvent;
        
        keyEvent.iCode      = EKeyEnter;
        keyEvent.iScanCode  = EStdKeyEnter;
        keyEvent.iModifiers = 0;
        keyEvent.iRepeats   = 0;

        CCoeEnv::Static()->SimulateKeyEventL(keyEvent, EEventKey);
        //iInfoDialog->OfferKeyEventL(keyEvent, EEventKeyUp);
        iInfoDialog = NULL;
        }


    /*RArray<TPosQNRequestId> requestArray;

    GetRequestsL(requestArray);

    TInt nb = requestArray.Count();*/
  }

// ---------------------------------------------------------
// CLcfPrivacyDialogPlugin::
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CLcfPrivacyDialogPlugin::HandleAllRequestCancelled()
  {
    if (iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }


    if (iDiagnostics)
    {
        delete iTimer;
        iTimer = NULL;

        iDiagnostics->SetExitReason(ELcfExitAllCancelledByEpos);

        iControlFilesManager->AppendDiagnosticsL(iDiagnostics);

        delete iDiagnostics;
        iDiagnostics = NULL;
    }
  }

// ---------------------------------------------------------
// CLcfPrivacyDialogPlugin::Dismiss
// cancels the dialog
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CLcfPrivacyDialogPlugin::Dismiss()
    {
	if (iDialogLaunched)
		{
		// Just stop the dialog.
		// remove sleeping dialog from the stack
		//ExitSleepingDialog();
		iDialogLaunched = EFalse;
		}
    }


TInt CLcfPrivacyDialogPlugin::TimerCallBack(TAny* aPtr)
    {

    ((CLcfPrivacyDialogPlugin*)aPtr)->DismissDialog();
    return 0; // callback is not called again
    }

void CLcfPrivacyDialogPlugin::DismissDialog()
    {
    // Can now put exit reason
    if (!iDiagnostics)
        return;

    // Special case, if we are completing something else than
    // current request, then do nothing else
    if (iAnswer == ELcfComplete && iStyleToComplete == ELcfSpecificRequest && iRequestIdToComplete != CurrentRequest())
        {
        CompleteRequest(iRequestIdToComplete, iCompletionCode);
        return;
        }

    // Enumerations TLcfPluginAnswer and TLcfPluginExitReason match
    // Change 10.6.2003: New exit reason ELcfTimeOutAll has to
    // be handled separately.
    // New exit reason completion has also to be handled separately
    if (iAnswer == ELcfTimeOutAll)
        {
        iDiagnostics->SetExitReason(ELcfExitAllTimedOut);
        }
    else if (iAnswer == ELcfComplete)
        {
        iDiagnostics->SetExitReason(ELcfExitRequestCompleted);
        }
    else
        {
        iDiagnostics->SetExitReason(TLcfPluginExitReason(iAnswer));
        }

    iControlFilesManager->AppendDiagnosticsL(iDiagnostics);

    // now must delete diagnostics
    delete iDiagnostics;
    iDiagnostics = NULL;

    if (iAnswer == ELcfAccept)
        {
        CompleteRequest(CurrentRequest(), KErrNone);
        }
    else if (iAnswer == ELcfReject)
        {
        CompleteRequest(CurrentRequest(), KErrAccessDenied);
        }
    else if (iAnswer == ELcfTimeOut)
        {
        CompleteRequest(CurrentRequest(), KErrTimedOut);
        }
    else if (iAnswer == ELcfAcceptAll)
        {
        CompleteAllRequests(KErrNone);
        }
    else if (iAnswer == ELcfRejectAll)
        {
        CompleteAllRequests(KErrAccessDenied);
        }
    else if (iAnswer == ELcfTimeOutAll)
        {
        CompleteAllRequests(KErrTimedOut);
        }
    else if (iAnswer == ELcfComplete)
        {
        switch(iStyleToComplete)
            {
        case ELcfCurrentRequest:
            CompleteRequest(CurrentRequest(), iCompletionCode);
            break;
        case ELcfSpecificRequest:
            CompleteRequest(iRequestIdToComplete, iCompletionCode);
            break;
        case ELcfAllRequests:
            CompleteAllRequests(iCompletionCode);
            break;
        default:
            CompleteRequest(CurrentRequest(), KErrNotFound);
            break;
            }
        }
    else
        {
        // TODO: What to do here?
        CompleteRequest(CurrentRequest(), KErrNotFound);
        }



    }




//  End of File  
