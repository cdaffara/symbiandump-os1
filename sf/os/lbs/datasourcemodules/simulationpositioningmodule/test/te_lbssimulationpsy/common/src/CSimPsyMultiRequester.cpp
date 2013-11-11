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
#include "CSimPsyMultiRequester.h"
#include "CSimPsyRequester.h"
#include "s32file.h" //Rdebug

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSimPsyMultiRequester::CSimPsyMultiRequester() 
: CActive(EPriorityHigh), iRequestsActive(EFalse)
    {
    }

void CSimPsyMultiRequester::ConstructL(TInt aRequestCount) 
    {
    User::LeaveIfError(iTimer.CreateLocal());
    CActiveScheduler::Add(this);

    for (TInt i = 0; i < aRequestCount; i++)
        {
        CSimPsyRequester* requester = CSimPsyRequester::NewL(this);            
        CleanupStack::PushL(requester);
        User::LeaveIfError(iRequesters.Append(requester));
        CleanupStack::Pop();
        }
    }

CSimPsyMultiRequester* CSimPsyMultiRequester::NewLC(TInt aRequestCount) 
    {
    CSimPsyMultiRequester* self = new (ELeave) CSimPsyMultiRequester;
    CleanupStack::PushL(self);
    self->ConstructL(aRequestCount);
    return self;
    }
		
// Destructor
CSimPsyMultiRequester::~CSimPsyMultiRequester()
    {
    Cancel();
    iTimer.Close();
    TInt nrs = iRequesters.Count();
    for (TInt i = nrs-1; i >= 0; i--)
        {
        CSimPsyRequester* requester = iRequesters[i];            
        delete requester;
        requester = NULL;
        }
    iRequesters.Close();
    iRequestersWhoWillCancel.Close();
    }

void CSimPsyMultiRequester::ClosePositioner()
    {
    TInt nrs = iRequesters.Count();
    for (TInt i = nrs-1; i >= 0; i--)
        {
        CSimPsyRequester* requester = iRequesters[i];            
        requester->ClosePositioner();
        }    
    }

void CSimPsyMultiRequester::MakeRequests()
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
        // RDebug::Print(_L("CSimPsyMultiRequester::MakeRequests - start cancelling"));
        iTimer.After(iStatus, TTimeIntervalMicroSeconds32(1));
        SetActive();
        }

    for (TInt i = 0; i < requesterCount; i++)
        {
        iRequesters[i]->MakeRequest();
        }

    CActiveScheduler::Start();
    }

void CSimPsyMultiRequester::HandleRequestDone()
    {
    // RDebug::Print(_L("CSimPsyMultiRequester::HandleRequestDone"));
    iNofRequestsDone++;
    if (iNofRequestsDone == iRequesters.Count())
        {
        // RDebug::Print(_L("CSimPsyMultiRequester::HandleRequestDone - stopping Active Scheduler"));
        Cancel();
        iRequestsActive = EFalse;
        CActiveScheduler::Stop();
        }
    }

void CSimPsyMultiRequester::RunL()
    {
    // Cancel certain requests.
    // RDebug::Print(_L("CSimPsyMultiRequester::RunL"));
    TInt nofRequestersWhoWillCancel = iRequestersWhoWillCancel.Count();
    for (TInt i = 0; i < nofRequestersWhoWillCancel; i++)
        {
        // RDebug::Print(_L("CSimPsyMultiRequester::RunL - start cancelling"));
        iRequesters[iRequestersWhoWillCancel[i]]->Cancel();
        }
    }

void CSimPsyMultiRequester::DoCancel()
    {
    iTimer.Cancel();
    }

void CSimPsyMultiRequester::StopRequests()
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

void CSimPsyMultiRequester::GetResult(
            TInt& aStatus, 
            TPositionInfo& aPosition, 
            TTimeIntervalMicroSeconds& aRequestTime,
            TInt aRequesterIndex)
    {
    iRequesters[aRequesterIndex]->GetResult(aStatus, aPosition, aRequestTime);
    }
    
TBool CSimPsyMultiRequester::GetModuleStatus(TInt aRequesterIndex)
    {    
	return iRequesters[aRequesterIndex]->GetModuleStatus();
    }

TPositionModuleStatus CSimPsyMultiRequester::ModuleStatus(TInt aRequesterIndex)
    {    
	return iRequesters[aRequesterIndex]->ModuleStatus();
    }

TInt CSimPsyMultiRequester::ReportedStatuses(TInt aRequesterIndex)
    {    
	return iRequesters[aRequesterIndex]->ReportedStatuses();
    }

void CSimPsyMultiRequester::ResetReportedStatuses(TInt aRequesterIndex)
    {    
	iRequesters[aRequesterIndex]->ResetReportedStatuses();
    }

void CSimPsyMultiRequester::SetRequesterWillCancelL(TInt aIndex, TBool aBool)
    {
    TInt length = iRequestersWhoWillCancel.Count();
    for (TInt i = 0; i < length; i++)
        {
		if (aBool) 
			{
			if (iRequestersWhoWillCancel[i] == aIndex) return;
			}
		else 
			{
			iRequestersWhoWillCancel.Remove(aIndex);
			return;
			}
        }
    User::LeaveIfError(iRequestersWhoWillCancel.Append(aIndex));
    }


// End of File
