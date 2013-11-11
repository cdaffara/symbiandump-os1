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

#include "NetConTestBases.h"
#include "NetConLog.h"

//
// NetCon Test Base Class
//

CNetConTestBase::CNetConTestBase()
: CActive(CActive::EPriorityStandard), iResult(EUnknown), iAction(ENone), iState(EInitialised)
	{
	
	CActiveScheduler::Add(this);
	}

CNetConTestBase::~CNetConTestBase()
	{

	Cancel();
	}

void CNetConTestBase::RunL()
	{

	switch(iAction)
		{
		case EStart:
			LOG( NetConLog::Printf(_L("---------- Starting %S ----------"), &Name()); )
			
			StartTestL();
			break;

		case EStop:
			StopTest();
			break;

		default:
			User::Invariant();
		}
	}

TInt CNetConTestBase::RunError(TInt aError)
	{

	LOG( NetConLog::Printf(_L("Error %d encountered while starting test '%S'"), aError, &Name()); )

	(void)aError; // remove warning

	iResult = EFailed;

	StopTest();
	return KErrNone;
	}

void CNetConTestBase::DoCancel()
	{
	
	iResult = EUnknown;
	StopTest();
	}

void CNetConTestBase::Start()
	{

	iAction = EStart;
	iState = EStarting;

	TRequestStatus* status = (&iStatus);
	SetActive();
	User::RequestComplete(status, KErrNone);

	CActiveScheduler::Start();
	}

void CNetConTestBase::CompleteTest(TResult aResult)
	{
	
	// guard against multiple completion
	if(iState==EComplete)
		{
		return;
		}

	iResult = aResult;
	iAction = EStop;
	iState = EComplete;

	TRequestStatus* status = (&iStatus);
	SetActive();
	User::RequestComplete(status, KErrNone);
	}

void CNetConTestBase::StopTest()
	{

	LOG (
		NetConLog::Printf(_L("---------- %S Complete ----------"), &Name());
		
		switch(Result())
			{
			case EPassed:
				NetConLog::Printf(_L("Test Result: Passed"));
				break;

			case EFailed:
				NetConLog::Printf(_L("Test Result: Failed"));
				break;

			case EUnknown:
			default:
				NetConLog::Printf(_L("Test Result: Unknown"));
			}

		NetConLog::Printf(_L(""));
		)

	CActiveScheduler::Stop();
	}

MNetConTest::TResult CNetConTestBase::Result() const
	{

	return iResult;
	}

