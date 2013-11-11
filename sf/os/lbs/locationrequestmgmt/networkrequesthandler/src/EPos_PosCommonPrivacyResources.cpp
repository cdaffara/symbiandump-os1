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
// INCLUDE FILES
// #ifdef _DEBUG
// #include <lbs/epos_cpostrace.h>
// #endif
//




#include    "EPos_PosCommonPrivacyResources.h"
#include    "EPos_PrivacyServerDebugPanic.h"

// GLOBAL VARIABLE
CPosCommonPrivacyResources* gResources = NULL;

// CONSTANTS
const TInt KPosRequestIdShift = 14;

// ================= LOCAL FUNCTIONS =======================

// ---------------------------------------------------------
// Static Returns the privacy common resources stored in TLS
// Returns: NULL: Resources not installed
//          other values: A pointer to the resources
// ---------------------------------------------------------
//
CPosCommonPrivacyResources* Static()
    {
    return gResources;
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// PosCommonPrivacyResources::Install
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt PosCommonPrivacyResources::Install()
    {
    if (gResources)
        {
        return KErrNone;
        }

    gResources = new CPosCommonPrivacyResources();

    if (!gResources)
        {
        return KErrNoMemory;
        }

    //Make a request id
    TTime now;
    now.UniversalTime();
    TInt64 requestId64 = now.Int64() >> KPosRequestIdShift;
    TUint low = I64LOW(requestId64);
    Static()->iLastAssignedRequestId = ~(~low | 0x80000000);

    return KErrNone;
    }

// ---------------------------------------------------------
// PosCommonPrivacyResources::Installed
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool PosCommonPrivacyResources::Installed()
    {
    return (Static()) ? ETrue : EFalse;
    }

// ---------------------------------------------------------
// PosCommonPrivacyResources::Uninstall
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void PosCommonPrivacyResources::Uninstall()
    {
    delete (gResources);
    gResources = NULL;
    }

// ---------------------------------------------------------
// PosCommonPrivacyResources::DialogCtrl
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPosDialogCtrl* PosCommonPrivacyResources::DialogCtrl()
    {
    return Static()->iDialogCtrl;
    }

// ---------------------------------------------------------
// PosCommonPrivacyResources::SetDialogCtrl
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void PosCommonPrivacyResources::SetDialogCtrl(
    CPosDialogCtrl* aDialogCtrl)
    {
    Static()->iDialogCtrl = aDialogCtrl;
    }

// ---------------------------------------------------------
// PosCommonPrivacyResources::GetNewRequestId
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt PosCommonPrivacyResources::GetNewRequestId()
    {
    // A request id may never be less than 1. Starting over if reaching limit.
    //
    if (Static()->iLastAssignedRequestId == KMaxTInt)
        {
        Static()->iLastAssignedRequestId = 0;
        }
    return ++Static()->iLastAssignedRequestId;
    }

/*
#ifdef _DEBUG

void PosCommonPrivacyResources::SetupTraceL()
    {
    Static()->iTrace = CPosTrace::NewL();
    }

void PosCommonPrivacyResources::TearDownTrace()
    {
    if (Static())
        {
        delete Static()->iTrace;
        Static()->iTrace = NULL;
        }
    }

void PosCommonPrivacyResources::Trace(const TDesC& aDescription,
                   const TDesC& aFilename,
                   const TInt aLineNumber)
    {
    if (Static() && Static()->iTrace)
        {
        TRAPD(err, Static()->iTrace->TraceL(
            aDescription, aFilename, aLineNumber));
        err = err; // avoids compiler warning
        }
    }
#endif
*/

//  End of File
