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
#include    "TAutomatisation.h"
#include <EPos_CPosRequestor.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//

TAutomatisation::TAutomatisation()
: iActive(EFalse), iEnv(CEikonEnv::Static()), iPrivCtrl(iEnv->FsSession()),
iDiagnostics(), iPrivCtrlData()
    {
    }


// ---------------------------------------------------------
// TAutomatisation::Enabled
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool TAutomatisation::Enabled()
    {
    return (iActive || iPrivCtrl.ControlDataExists());
    }

// ---------------------------------------------------------
// TAutomatisation::StartL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TAutomatisation::StartL(TInt aEntryCount, TInt aType, TInt aServiceIdFormat, TInt aContactIdFormat, 
                             CPosPrivacyNotifier::TRequestType aNotOrVer, const TDesC& aServiceStringId,
                             const TDesC& aContactStringId, 
                             TBool aServiceMultipleMarches, TBool aContactMultipleMarches,
                             TBool aServiceResolved, TBool aContactResolved,
                             TPosQNRequestId aRequestId, CPosPrivacyNotifier* aPrivPlugin)
    {
    if (iActive)
        {
        // Handle of multiple queries later
        //aMessage.Complete(KErrInUse);
        return;
        }

    iPrivPlugin = aPrivPlugin;
    iRequestId = aRequestId;
    iActive = ETrue;
    iNotOrVer = aNotOrVer;

    iDiagnostics = TPrivDlgDiagnostics();

    iDiagnostics.iServiceMultipleMarches = aServiceMultipleMarches;
    iDiagnostics.iContactMultipleMarches = aContactMultipleMarches;

    for (TInt i = 0; i < aEntryCount; i++)
        {
        if (i == 1) break; // Can only handle two entries, should be fixed

        if (aServiceStringId != KNullDesC)
          {
          iDiagnostics.iServicePresent = ETrue;
          //iDiagnostics.iServiceName = aStringId;
          iDiagnostics.iServiceId = aServiceStringId;
          iDiagnostics.iServiceIdFormat = aServiceIdFormat;
          iDiagnostics.iServiceResolved = aServiceResolved;

          }
        if (aContactStringId != KNullDesC)
          {
          iDiagnostics.iContactPresent = ETrue;
          //iDiagnostics.iContactName = aStringId;
          iDiagnostics.iContactId = aContactStringId;
          iDiagnostics.iContactIdFormat = aContactIdFormat;
          iDiagnostics.iContactResolved = aContactResolved;
          }

        if (aServiceStringId == KNullDesC && aContactStringId == KNullDesC)
          {
          iDiagnostics.iUnknownPresent = ETrue;
          }
        }

    iPrivCtrl.GetControlDataL(iPrivCtrlData);

    if (aNotOrVer == CPosPrivacyNotifier::EQuery)
        {
        if (iPrivCtrlData.iButton == EYesButton)
            {
            Complete(KErrNone);
            }
        else if (iPrivCtrlData.iButton == ENoButton)
            {
            Complete(KErrAccessDenied);
            }
        else if (iPrivCtrlData.iButton == ECancelButton)
            {
            //Complete(KErrTimedOut);
            return;
            }
        } 
    else if (aNotOrVer == CPosPrivacyNotifier::ENotification)
        {

        iDiagnostics.iNotification = ETrue;
        if (iPrivCtrlData.iButton == EYesButton || iPrivCtrlData.iButton == ENoButton)
            {
            Complete(KErrNone);
            }
        else if (iPrivCtrlData.iButton == ECancelButton)
            {
            // XXX What to do here
            //Complete(KErrTimedOut);
            return;
            }

        }

    iActive = EFalse;
    }

// ---------------------------------------------------------
// TAutomatisation::Cancel
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TAutomatisation::Cancel()
    {
    if (iActive)
        {
        iActive = EFalse;
        iDiagnostics.iWasCancelledByEPos = ETrue;
        Complete(KErrCancel);
        }
    }

// ---------------------------------------------------------
// TAutomatisation::Complete
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TAutomatisation::Complete(TInt aCompletionCode)
    {
    iActive = EFalse;
    iPrivCtrl.SetDiagnosticsL(iDiagnostics);
    iPrivPlugin->CompleteRequest(iRequestId, aCompletionCode);
    }

// ---------------------------------------------------------
// TAutomatisation::CompleteAllRequests
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TAutomatisation::CompleteAllRequests(TInt aCompletionCode)
    {
    iActive = EFalse;
    iPrivCtrl.SetDiagnosticsL(iDiagnostics);
    iPrivPlugin->CompleteAllRequests(aCompletionCode);
    }

//  End of File  
