// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsstepclearlog.cpp
// This is the class implementation to clear the log file
// 
//

/**
 @file
 @internalTechnology
 @test
*/
		  
#include "ctlbsstepclearlog.h"


CT_LbsStep_ClearLog::CT_LbsStep_ClearLog(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_ClearLog);
	}

/**
Static Constructor
*/
CT_LbsStep_ClearLog* CT_LbsStep_ClearLog::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_ClearLog(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
@pre 	
@return Test verdict
@post	Log file cleared
*/
 
TVerdict CT_LbsStep_ClearLog::doTestStepL()
	{
	SetTestStepResult(EPass);

	// Create necessary variables for Log reading
	RFs theFs;
	CLogClient* client;
	CLogViewEvent* view;
	CT_LbsAsyncWaiter* activeWaiter;
		
	//	Initialise variables
	TInt ErrorCode = theFs.Connect();
	if(ErrorCode)
		{
		INFO_PRINTF1(_L("\t Error: Open FileServer failed."));
		SetTestStepResult(EFail);
		}
		

    client = CLogClient::NewL(theFs, CActive::EPriorityStandard);
	view = CLogViewEvent::NewL(*client, CActive::EPriorityStandard);
	activeWaiter = CT_LbsAsyncWaiter::NewL();			// NEW activeWaiter
	
	
	// Setup a time in the future, before which all events will be deleted
 	_LIT(KDateCorrect1,"20900101:"); 
 	TTime time;
	TBuf <10> theDate(KDateCorrect1);
	TInt err=time.Set(theDate); 
	
	// Ensure time has been set correctly
	if(err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to set time"));
		}
		
	if(err == KErrGeneral)
		{
		INFO_PRINTF1(_L("Time syntax is incorrect"));
		}
	
	// Set the filter to view ALL logs
	CLogFilter* filter = CLogFilter::NewL();			// NEW FILTER
	CleanupStack::PushL(filter);
	TBool res = view->SetFilterL(*filter, activeWaiter->iStatus);
	if (res == EFalse)
		{
		INFO_PRINTF1(_L("\t No such events to filter in the view."));
		client->Cancel();
		}
	else
		{
		// else If there are logs, flush them
			
		// Cancel outstanding requests
		client->Cancel();	
		activeWaiter->StartAndWait();
		if (activeWaiter->iStatus != KErrNone)
			{
			INFO_PRINTF2(_L("\t Error: Cancel returned error %d."),activeWaiter->iStatus.Int());
			SetTestStepResult(EFail);
			}

		// Clear ALL logs
		client->ClearLog(time, activeWaiter->iStatus);
		activeWaiter->StartAndWait();
		if (activeWaiter->iStatus != KErrNone)
			{
			INFO_PRINTF2(_L("\t Error: Clear log returned error %d."),activeWaiter->iStatus.Int());
			SetTestStepResult(EFail);
			}	
			
		//Confirm log is cleared
		if(view->CountL() != 0)
			{
			INFO_PRINTF2(_L("<FONT><B>ERROR: Log still has %d entries</B></FONT>"),view->CountL());
			SetTestStepResult(EFail);
			}
				
		}
	
	CleanupStack::PopAndDestroy(filter);	
		
	// Cleanup everything
	view->Cancel();
	client->Cancel();

	delete view;
	delete client;
	theFs.Close();
	delete activeWaiter;

	return TestStepResult();
	}

