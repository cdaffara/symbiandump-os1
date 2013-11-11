// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CPrivacyRequester.h"
#include "EPos_RPrivacyServer.h"
#include "EPos_CPositionInfo.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsPrivacyRequester::CPrivacyRequester()
: CActive(EPriorityStandard), iPositionStatus(NULL), iPosition(NULL)
    {
    }

// EPOC default constructor can leave.
void CT_LbsPrivacyRequester::ConstructL()
    {
    iPrivacyServer = new (ELeave) RPrivacyServer;
    User::LeaveIfError(iPrivacyServer->Connect());
    CActiveScheduler::Add(this);
    }

// Two-phased constructor.
CT_LbsPrivacyRequester* CT_LbsPrivacyRequester::NewL()
    {
    CT_LbsPrivacyRequester* self = new (ELeave) CT_LbsPrivacyRequester;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CT_LbsPrivacyRequester::~CPrivacyRequester()
    {
    iPrivacyServer->Close();
    delete iPrivacyServer;
    }


// ---------------------------------------------------------
// CT_LbsPrivacyRequester::VerifyAndGetPosition
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPrivacyRequester::VerifyAndGetPosition(
    TRequestStatus& aStatus,
    TInt            aQueryId,
    CPositionInfo*& aPosition)
    {
    iPositionStatus = &aStatus;
    *iPositionStatus = KRequestPending;
    iPosition = &aPosition;
    iPrivacyServer->VerifyL(iStatus, aQueryId);
    SetActive();
    }

// ---------------------------------------------------------
// CT_LbsPrivacyRequester::RunL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPrivacyRequester::RunL()
    {
    if (iStatus.Int() != KErrNone)
        {
        User::RequestComplete(iPositionStatus, iStatus.Int());
        }

    *iPosition = CPositionInfo::NewL();
    (*iPosition)->SetPosition(TPosition(65.345, 11.456));

    User::RequestComplete(iPositionStatus, KErrNone);
    }

// ---------------------------------------------------------
// CT_LbsPrivacyRequester::DoCancel
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPrivacyRequester::DoCancel()
    {
    }


//  End of File
