// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include "tsysstart2_procmondeferred.h"
#include "testprocgood.h"
#include "testapps.h"


CAppFwkSysStart2TestStepProcMonDeferred::CAppFwkSysStart2TestStepProcMonDeferred()
	{
	SetTestStepName(KCTestCaseProcMonDeferred);
	}

/**
Old Test CaseID 		APPFWK-SYSSTART-0082
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP2-0082
 */

TVerdict CAppFwkSysStart2TestStepProcMonDeferred::doTestStepL()
	{	
	
	// The test process should have been started, as indicated in tsysstart2-procstart_emulator.rss
	TFullName name;
	RProcess process;
	CleanupClosePushL( process );

	
	if( ProcessExists(KTestProcGood, name) && (KErrNone == process.Open(name)) )
		{
		INFO_PRINTF1( _L("The test-process was started successfully at startup with \n\t\t\tstart_method of EDeferredWaitForStart & EIgnoreOnFailure") );		
		INFO_PRINTF1( _L("Now Kill() the test-process and wait for the throttle time") );
		
		
		process.Kill( KSysStart2TestKillCode );
		process.Close();
		User::After( KThrottleTime );
		
		
		if( !ProcessExists(KTestProcGood, name) )
			{
			INFO_PRINTF1( _L("The test-process has correctly _not been restarted") );
			
			SetTestStepResult( EPass );						
			}
		else
			{
			ERR_PRINTF1( _L("The test-process has INcorrectly been restarted") );	
			
			SetTestStepResult( EFail );
			}
		
		}
	else
		{
		INFO_PRINTF1(_L("The test-process was not started or a handle could not be opened on it"));	
		
		SetTestStepResult( EFail );
		}

	CleanupStack::PopAndDestroy( &process );
	
	return TestStepResult();	
	}
