// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDES
#include "ctlbsposclientholder.h"
#include "ctlbsposclient.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsPosClientHolder::CT_LbsPosClientHolder() 
: CActive(EPriorityHigh), iRequestsActive(EFalse), iCancelTime(0)
    {
    }

// ---------------------------------------------------------
// CT_LbsPosClientHolder::ConstructL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClientHolder::ConstructL(TInt aRequestCount, TUid aPsy, TBool aSameServer) 
    {
    User::LeaveIfError(iTimer.CreateLocal());
    CActiveScheduler::Add(this);
    User::LeaveIfError(iPosServer.Connect());
    CT_LbsPosClient* requester = NULL;
    for (TInt i = 0; i < aRequestCount; i++)
        {
        if (aSameServer)
            {
            requester = CT_LbsPosClient::NewL(this, aPsy,iPosServer);
            }
        else
            {
            requester = CT_LbsPosClient::NewL(this, aPsy);
            }
        CleanupStack::PushL(requester);
        User::LeaveIfError(iRequesters.Append(requester));
        CleanupStack::Pop(requester);
        }
    }

// ---------------------------------------------------------
// CT_LbsPosClientHolder::NewLC
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
CT_LbsPosClientHolder* CT_LbsPosClientHolder::NewLC(TInt aRequestCount, TUid aPsy, TBool aSameServer) 
    {
    CT_LbsPosClientHolder* self = new (ELeave) CT_LbsPosClientHolder;
    CleanupStack::PushL(self);
    self->ConstructL(aRequestCount, aPsy, aSameServer);
    return self;
    }
     


// Destructor
CT_LbsPosClientHolder::~CT_LbsPosClientHolder()
    {
    Cancel();
    iTimer.Close();
    iRequesters.ResetAndDestroy();
    iRequesters.Close();
    iRequestersWhoWillCancel.Close();
    iPosServer.Close();
    }

// ---------------------------------------------------------
// CT_LbsPosClientHolder::MakeRequests
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClientHolder::MakeRequests()
    {
    if (iRequestsActive)
        {
        return;
        }
    iRequestsActive = ETrue;
    TInt requesterCount = iRequesters.Count();
    iNofRequestsDone = 0;
    for (TInt i = 0; i < requesterCount; i++)
        {
        iRequesters[i]->MakeRequest();
        }

    // initiate cancelling if this is set.
    if (iRequestersWhoWillCancel.Count() > 0)
        {
        iTimer.After(iStatus, iCancelTime);
        SetActive();
        }

    CActiveScheduler::Start();
    }

// ---------------------------------------------------------
// CT_LbsPosClientHolder::HandleRequestDone
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClientHolder::HandleRequestDone()
    {
    iNofRequestsDone++;
    if (iNofRequestsDone == iRequesters.Count())
        {
        iRequestsActive = EFalse;
        CActiveScheduler::Stop();
        }
    }

// ---------------------------------------------------------
// CT_LbsPosClientHolder::RunL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClientHolder::RunL()
    {
    // Cancel certain requests.
    TInt nofRequestersWhoWillCancel = iRequestersWhoWillCancel.Count();
    for (TInt i = 0; i < nofRequestersWhoWillCancel; i++)
        {
        iRequesters[iRequestersWhoWillCancel[i]]->Cancel();
        HandleRequestDone();
        }
    }

// ---------------------------------------------------------
// CT_LbsPosClientHolder::DoCancel
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClientHolder::DoCancel()
    {
    iTimer.Cancel();
    }

void CT_LbsPosClientHolder::StopRequests()
    {
    if (!iRequestsActive)
        {
        return;
        }
    Cancel();
    TInt requesterCount = iRequesters.Count();
    for (TInt i = 0; i < requesterCount; i++)
        {
        iRequesters[i]->Cancel();
        }
    CActiveScheduler::Stop();
    iRequestsActive = EFalse;
    }

// ---------------------------------------------------------
// CT_LbsPosClientHolder::GetResult
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClientHolder::GetResult(
            TInt& aStatus, 
            TPositionInfo& aModuleInfo,
            TTimeIntervalMicroSeconds& aRequestTime,
            TInt aRequesterIndex)
    {
    iRequesters[aRequesterIndex]->GetResult(aStatus, aModuleInfo, aRequestTime);
    }



// ---------------------------------------------------------
// CT_LbsPosClientHolder::SetRequesterWillCancelL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClientHolder::SetCancelTime(TTimeIntervalMicroSeconds32 aTimeInterval)
	{
	iCancelTime = aTimeInterval;
	}

// ---------------------------------------------------------
// CT_LbsPosClientHolder::SetRequesterWillCancelL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPosClientHolder::SetRequesterWillCancelL(TInt aIndex)
    {
    TInt length = iRequestersWhoWillCancel.Count();
    for (TInt i = 0; i < length; i++)
        {
        if (iRequestersWhoWillCancel[i] == aIndex) return;
        }
    User::LeaveIfError(iRequestersWhoWillCancel.Append(aIndex));
    }


// End of File
