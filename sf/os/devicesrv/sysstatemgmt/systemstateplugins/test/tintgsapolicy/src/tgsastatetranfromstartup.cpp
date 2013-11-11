// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "gsatestapps.h"

#include <ssm/ssmstates.hrh>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmstate.h>
#include <ssm/ssmstatetransition.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmstateawaresession.h>
#include <ssm/ssmdomaindefs.h>

#include "tgsastatetran_base.h"

class CStateTranFromStartup : public CTGsaStateTransition
	{
public:
	static CStateTranFromStartup* NewL();
	~CStateTranFromStartup();

private:
	void ConstructL();
	CStateTranFromStartup();

public:
	void DoTestStateTranFromStartupL();
	};


CStateTranFromStartup* CStateTranFromStartup::NewL()
	{
	CStateTranFromStartup* self = new(ELeave) CStateTranFromStartup();
	self->ConstructL();
	return self;
	}

void CStateTranFromStartup::ConstructL()
	{
	CTGsaStateTransition::ConstructL(KGsaTestStateTranFromStartupResult);
	}

CStateTranFromStartup::~CStateTranFromStartup()
	{
	}

CStateTranFromStartup::CStateTranFromStartup()
	{
	}

// Tests Invalid state transition from startup
void CStateTranFromStartup::DoTestStateTranFromStartupL()
	{
	RSsmStateManager stateMan;
	TInt err = stateMan.Connect();
	CleanupClosePushL(stateMan);
	if (err == KErrNone)
		RDebug::Print(_L("\nStateTran Test: RSsmStateManager sess Connect %d\n"), err);
	else
		{
		RDebug::Print(_L("\nStateTran Test: Unable to connect to RSsmStateManager sess\n"));	
		User::Leave(KTestAppFailure);
		}

	// Creates and open the result file for streaming
	err = iFileWriteStream.Replace(iFs, KGsaTestStateTranFromStartupResult, EFileWrite);
	RDebug::Print(_L("\nStateTran Test: iFileWriteStream open with %d\n"), err);

	CurrentSystemStateL();
	//Scenario 1 - Request for state change from Startup to invalid state
	RDebug::Print(_L("\nStateTran Test: Scenario 1 \n"));
	TSsmState state(10, KSsmAnySubState);
	TSsmStateTransition stateTrans_InvalidState(state, 1);

	TRequestStatus status;
	stateMan.RequestStateTransition(stateTrans_InvalidState, status);
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from Startup to Invalid state with %d\n "), status.Int());
	
	User::WaitForRequest(status);
	iFileWriteStream.WriteInt32L(status.Int());
	if (status.Int() == KErrNotSupported)
		{
		CurrentSystemStateL();
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Startup to Invalid state completes with %d Expected -5\n "), status.Int());
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Startup to Invalid state Failed"));
		User::Leave(KTestAppFailure);
		}

	CleanupStack::PopAndDestroy(&stateMan);
	}


TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	TInt testCompletionReason = KErrNone;
	TInt argc = User::CommandLineLength();
	if (cleanup)
		{
		__UHEAP_MARK;
		CStateTranFromStartup* stateTran = NULL;
		TRAP(r, stateTran = CStateTranFromStartup::NewL());
		if (r == KErrNone)
			{
			TRAP(testCompletionReason, stateTran->DoTestStateTranFromStartupL());
			// testCompletionReason is the reason with which the test case completes.
			TRAP(r, stateTran->CommitTestResultsL(testCompletionReason));
			
			// Rendezvous'ng the process as it started as WaitForSignal execution behaviour
			RProcess::Rendezvous(KErrNone);
			delete stateTran;
			}
		delete cleanup;
		__UHEAP_MARKEND;

		if (r != KErrNone)
			{
			User::Panic(_L("tgsastatetranfromstartuppanic"), r);
			}
		}

	__UHEAP_MARKEND;
	return r;
	}



