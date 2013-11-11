// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32debug.h>
#include "ssmtestapps.h"

#include <ssm/ssmstates.hrh>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmstate.h>
#include <ssm/ssmstatetransition.h>
#include <ssm/ssmstatemanager.h>

static void RunL()
	{
	User::LeaveIfError(RThread::RenameMe(KTestProcTranFromNormalWoCap));

	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);

	RSsmStateManager stateMan;
	TInt err = stateMan.Connect();
	User::LeaveIfError(err);
	RDebug::Print(_L("\n RSsmStateManager sess Connect %d\n"), err);
	
	// Request from application without powermanagement Capability for state change from Normal to fail state
	TRequestStatus status1;
	TSsmState state(ESsmFail, KSsmAnySubState);
	TSsmStateTransition stateTrans2(state, 1);

	stateMan.RequestStateTransition(stateTrans2,status1);
	RDebug::Print(_L("\n Start RequestStateTransition from Normal to fail state %d\n "), status1.Int());
	
	User::WaitForRequest(status1);
	if (status1.Int() == KErrNotSupported)
		{
		RDebug::Print(_L("\n RequestStateTransition from Normal to fail state completes with %d expected -5\n "), status1.Int());
		}
	else
		{
		User::Leave(KTestAppFailure);
		}
	
	// Request from application without powermanagement Capability for state change from Normal to shutdown state
	state.Set(ESsmShutdown, KSsmAnySubState);
	TSsmStateTransition stateTrans3(state, 1);

	stateMan.RequestStateTransition(stateTrans3,status1);
	RDebug::Print(_L("\n Start RequestStateTransition from Normal to shutdown state %d\n "), status1.Int());
	
	User::WaitForRequest(status1);
	if (status1.Int() == KErrNotSupported)
		{
		RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state completes with %d expected -5\n "), status1.Int());
		}
	else
		{
		User::Leave(KTestAppFailure);
		}

	CleanupStack::PopAndDestroy();
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunL());
		delete cleanup;
		if (r != KErrNone)
			{
			User::Panic(_L("ssmstatetranfromsnormalwocappanic"), r);
			}
		}

	__UHEAP_MARKEND;
	return r;
	}



