// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ctlbsclienteventtimer.h"
#include "mctlbsrequestobserver.h"
#include <coemain.h>
#include "ctlbsclientrequestor.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsClientEventTimer::CT_LbsClientEventTimer()
	: CActive(EActivePriorityDefault),	
	iTimer(),
	iObserver(NULL),
	iCompletionCode(KNoEvent)
    {
    }

// EPOC default constructor can leave.
void CT_LbsClientEventTimer::ConstructL(CTestExecuteLogger& aLogger)
	{
	iLogger = aLogger;
    User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
    }

// Two-phased constructor.
CT_LbsClientEventTimer* CT_LbsClientEventTimer::NewL(CTestExecuteLogger& aLogger)
    {
	CT_LbsClientEventTimer* self = NewLC(aLogger);
	CleanupStack::Pop();
	return self;
    }

// Two-phased constructor.
CT_LbsClientEventTimer* CT_LbsClientEventTimer::NewLC(CTestExecuteLogger& aLogger)
    {
	CT_LbsClientEventTimer* self =
		new (ELeave) CT_LbsClientEventTimer(); 
	CleanupStack::PushL(self); // push to clean-up stack
	self->ConstructL(aLogger); // construct
	return self; // return new object
    }

// Destructor
CT_LbsClientEventTimer::~CT_LbsClientEventTimer()
    {
    iObserver = NULL;
	Cancel();
	iTimer.Close();
    }

// ---------------------------------------------------------
// CT_LbsClientEventTimer::SetObserver
//
// (other items were commented in a header).
// ---------------------------------------------------------
// 
void CT_LbsClientEventTimer::SetObserver(MCT_LbsRequestObserver* aObserver)
    {
    iObserver = aObserver;
    }

// ---------------------------------------------------------
// CT_LbsClientEventTimer::CompletionCode
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsClientEventTimer::CompletionCode() const
    {
    return iCompletionCode;
    }

// ---------------------------------------------------------
// CT_LbsClientEventTimer::ResetCompletionCode
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientEventTimer::ResetCompletionCode()
	{
	iCompletionCode = KNoEvent;
	}

// ---------------------------------------------------------
// CT_LbsClientEventTimer::Start
// starts the RTimer
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientEventTimer::Start(TInt aTimeout)
    {
	iTimer.After(iStatus, aTimeout);
	SetActive();
    }

// ---------------------------------------------------------
// CT_LbsClientEventTimer::DoCancel
// cancels the RTimer
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientEventTimer::DoCancel()
    {
	iTimer.Cancel();
    }

// ---------------------------------------------------------
// CPosNotifierTimer::RunL
// report a timeout to the observer
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientEventTimer::RunL()
    {
	iCompletionCode = iStatus.Int();
	RequestComplete();
    }

// ---------------------------------------------------------
// CT_LbsClientEventTimer::RunError
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsClientEventTimer::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// ---------------------------------------------------------
// CT_LbsClientEventTimer::RequestComplete
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientEventTimer::RequestComplete()
    {
	if (iObserver)
        {
        TRAPD(err, iObserver->RequestCompleteL());
        if(KErrNone != err)
        	{
        	_LIT(KRequestCompleteError, "Error in LbsRequester: %d");
        	ERR_PRINTF2(KRequestCompleteError, err);
        	}
        }
    }

//  End of File  
