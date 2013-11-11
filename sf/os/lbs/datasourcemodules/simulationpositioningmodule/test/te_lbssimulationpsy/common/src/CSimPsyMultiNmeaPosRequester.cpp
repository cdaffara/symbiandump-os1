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



//  INCLUDES
#include "CSimPsyMultiNmeaPosRequester.h"
#include "CSimPsyNmeaPosRequester.h"
#include "s32file.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSimPsyMultiNmeaPosRequester::CSimPsyMultiNmeaPosRequester() 
: CActive(EPriorityHigh), iRequestsActive(EFalse)
    {
    }

void CSimPsyMultiNmeaPosRequester::ConstructL(TInt aRequestCount) 
    {
    User::LeaveIfError(iTimer.CreateLocal());
    CActiveScheduler::Add(this);

    for (TInt i = 0; i < aRequestCount; i++)
        {
        CSimPsyNmeaPosRequester* requester = CSimPsyNmeaPosRequester::NewL(this);        
        CleanupStack::PushL(requester);            
        User::LeaveIfError(iRequesters.Append(requester)); //i==6
        CleanupStack::Pop();
        }
    }

CSimPsyMultiNmeaPosRequester* CSimPsyMultiNmeaPosRequester::NewLC(TInt aRequestCount) 
    {
    CSimPsyMultiNmeaPosRequester* self = new (ELeave) CSimPsyMultiNmeaPosRequester; //i==0
    CleanupStack::PushL(self);
    self->ConstructL(aRequestCount);
    return self;
    }
     
// Destructor
CSimPsyMultiNmeaPosRequester::~CSimPsyMultiNmeaPosRequester()
    {
    if (IsActive())
        {
        }
    else
        {
        }
    Cancel();
    iTimer.Close();
    TInt nrs = iRequesters.Count();
    for (TInt i = nrs-1; i >= 0; i--)
        {
        CSimPsyNmeaPosRequester* requester = iRequesters[i];            
        delete requester;
        requester = NULL;
        }
    iRequesters.Close();
    iRequestersWhoWillCancel.Close();
    }

void CSimPsyMultiNmeaPosRequester::MakeRequests(TPositionInfo* aPositionInfo)
    {
    if (iRequestsActive)
        {
        return;
        }
    iRequestsActive = ETrue;
    TInt requesterCount = iRequesters.Count();
    iNofRequestsDone = 0;
	
	// initiate cancelling if this is set.
    if (iRequestersWhoWillCancel.Count() > 0)
        {
		iTimer.After(iStatus, iMicroSecondsToCancel);
        SetActive();
        }

    for (TInt i = 0; i < requesterCount; i++)
        {
        iRequesters[i]->MakeRequest(aPositionInfo);
        }	
    CActiveScheduler::Start();    
    }

void CSimPsyMultiNmeaPosRequester::HandleRequestDone()
    {
    iNofRequestsDone++;
    if (iNofRequestsDone == iRequesters.Count())
        {
        iRequestsActive = EFalse;          
        Cancel();
        CActiveScheduler::Stop();
        
        }
    }

void CSimPsyMultiNmeaPosRequester::RunL()
    {
    // Cancel certain requests.
    TInt nofRequestersWhoWillCancel = iRequestersWhoWillCancel.Count();
    for (TInt i = 0; i < nofRequestersWhoWillCancel; i++)
        {
        iRequesters[iRequestersWhoWillCancel[i]]->Cancel();
        }   
    }

void CSimPsyMultiNmeaPosRequester::DoCancel()
    {
    iTimer.Cancel();
    }

void CSimPsyMultiNmeaPosRequester::StopRequests()
    {
    if (!iRequestsActive)
        {
		TInt requesterCount = iRequesters.Count();
		for (TInt i = 0; i < requesterCount; i++)
			{
			iRequesters[i]->CancelRequest();
			}	
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

void CSimPsyMultiNmeaPosRequester::GetResult(
            TInt& aStatus,             
            TTimeIntervalMicroSeconds& aRequestTime,
            TInt aRequesterIndex)
    {
    iRequesters[aRequesterIndex]->GetResult(aStatus, aRequestTime);
    }

void CSimPsyMultiNmeaPosRequester::SetPartialUpdateRestriction(
    TBool aPartialUpdateRestriction)
    {
    for (TInt i = 0; i < iRequesters.Count(); i++)
        {
        iRequesters[i]->SetPartialUpdateRestriction(aPartialUpdateRestriction);
        }
    }

void CSimPsyMultiNmeaPosRequester::SetPartialUpdateRestriction(
    TBool aPartialUpdateRestriction,
    TInt aRequesterIndex)
    {
    iRequesters[aRequesterIndex]->SetPartialUpdateRestriction(aPartialUpdateRestriction);
    }

void CSimPsyMultiNmeaPosRequester::SetRequesterWillCancelL(
    TInt aIndex,
    TTimeIntervalMicroSeconds32 aMicroSeconds)
    {
    iMicroSecondsToCancel = aMicroSeconds;
    TInt length = iRequestersWhoWillCancel.Count();
    for (TInt i = 0; i < length; i++)
        {
        if (iRequestersWhoWillCancel[i] == aIndex) return;
        }
    User::LeaveIfError(iRequestersWhoWillCancel.Append(aIndex));
    }


// End of File
