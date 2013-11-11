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

#include "tgsastatetran_base.h"
#include "gsatestapps.h"

#include <ssm/ssmstates.hrh>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmstate.h>
#include <ssm/ssmstatetransition.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmdomaindefs.h>

#include <e32debug.h>

void CTGsaStateTransition::ConstructL(const TDesC& aResultFile)
	{
	// Connects to RSsmStateAwareSession in order to enquire about the current system state
	TInt err = iSsmStateAwareSess.Connect(KSM2UiServicesDomain3);
	if (err == KErrNone)
		RDebug::Print(_L("\nStateTran Test: RSsmStateAwareSession sess Connect %d\n"), err);
	else
		{
		RDebug::Print(_L("\nStateTran Test: RSsmStateAwareSession sess Connect Failed\n"));	
		User::Leave(err);
		}

	//Connects to file server
	err = iFs.Connect();
	if (err == KErrNone)
		RDebug::Print(_L("\nStateTran Test: RFs sess Connect %d\n"), err);
	else
		{
		RDebug::Print(_L("\nStateTran Test: RFs sess Connect Failed\n"));
		User::Leave(err);
		}

	err = iFs.MkDirAll(aResultFile);
	if (err == KErrNone || err == KErrAlreadyExists)
		RDebug::Print(_L("\nStateTran Test: RFs sess Connect %d\n"), err);
	else
		{
		RDebug::Print(_L("\nStateTran Test: RFs sess Connect Failed\n"));
		User::Leave(err);
		}
	}

CTGsaStateTransition::~CTGsaStateTransition()
	{
	iSsmStateAwareSess.Close();
	iFileWriteStream.Close();
	iFs.Close();
	if (iAsyncCallbackForStartTest)
		delete iAsyncCallbackForStartTest;
	if(iAsyncCallbackForStopScheduler)
		delete iAsyncCallbackForStopScheduler;
	}

CTGsaStateTransition::CTGsaStateTransition()
	{
	}

// Gets the current system state and checks whether it is in Normal state.
void CTGsaStateTransition::CurrentSystemStateL()
	{
	TSsmState currenState;
	currenState = iSsmStateAwareSess.State();

	// Writes the current system state to the file
	iFileWriteStream.WriteUint16L(currenState.MainState());
	if (currenState.MainState() == ESsmStartup || currenState.MainState() == ESsmNormal || currenState.MainState() == ESsmShutdown)
		RDebug::Print(_L("\nStateTran Test: Current System State is %d\n"), currenState.MainState());
	else
		{
		RDebug::Print(_L("\nStateTran Test: System is not in Startup / Normal / Shutdown State\n"));
		User::Leave(KTestAppFailure);
		}
	}

TBool CTGsaStateTransition::DoStartStateTranTest()
	{
	TSsmState currenState;
	currenState = iSsmStateAwareSess.State();

	// Once the system enters into normal state start the state transition test
	if (currenState.MainState() == ESsmNormal)
		{
		RDebug::Print(_L("\nStateTran Test: System is in Normal state, so start the test\n"));
		return ETrue;
		}
	else
		{
		iAsyncCallbackForStartTest->CallBack();
		return EFalse;
		}
	}

void CTGsaStateTransition::CallStopSchedulerL()
	{
	TSsmState currenState;
	currenState = iSsmStateAwareSess.State();

	TUint16 currentMainState = currenState.MainState();
	// Active scheduler is stopped once the system goes into shudown state, else it issues a request for callback again
	// Once the system gets into normal state start the state transition test
	if (currentMainState == ESsmFail || currentMainState == ESsmShutdown)
		{
 		RDebug::Print(_L("\nStateTran Test: Stop Active Scheduler\n"));
		iFileWriteStream.WriteUint16L(currentMainState);
		CActiveScheduler::Stop();
		}
	else
		{
		iAsyncCallbackForStopScheduler->CallBack();
		}
	}

void CTGsaStateTransition::CommitTestResultsL(TInt aTestCompletionReason)
	{
	// Writes the test completion reason to the file
	iFileWriteStream.WriteInt32L(aTestCompletionReason);
	iFileWriteStream.CommitL();
	}

void CTGsaStateTransition::StateChanged(TSsmState aSsmState)
	{
	if (aSsmState.MainState() == ESsmFail || aSsmState.MainState() == ESsmShutdown)
		RDebug::Print(_L("\nStateTran Test: Current System State after completing the test %d\n"), aSsmState.MainState());
	else
		{
		RDebug::Print(_L("\nStateTran Test: System is not in Fail or Shutdown State\n"));
		}
	}




