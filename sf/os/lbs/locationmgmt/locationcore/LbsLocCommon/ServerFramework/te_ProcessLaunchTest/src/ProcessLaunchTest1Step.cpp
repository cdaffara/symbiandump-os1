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
// Example CTestStep derived implementation
// 
//

/**
 @file ProcessLaunchTest1Step.cpp
 @internalTechnology
*/
#include "ProcessLaunchTest1Step.h"
#include "Te_ProcessLaunchTestSuiteDefs.h"
#include "lbsprocessuiddefs.h"
#include "cserverlaunch.h"
#include "tserverstartparams.h"

CProcessLaunchTest1Step::~CProcessLaunchTest1Step()
/**
 * Destructor
 */
	{
	}

CProcessLaunchTest1Step::CProcessLaunchTest1Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KProcessLaunchTest1Step);
	}

TVerdict CProcessLaunchTest1Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class CProcessLaunchTest1Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_ProcessLaunchTestSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CProcessLaunchTest1Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	TServerStartParams serverParams;
	_LIT(KServerFileName, "\\sys\\bin\\te_serverframework_server.exe");
	_LIT(KNonExistentServerFileName, "\\sys\\bin\\te_serverframework_not_found.exe");
	_LIT(KInvalidServerFileName, "\\sys\\bin\\te_serverframework _server.exe");
	_LIT(KInvalidServerFilePathName, "\\sys\bin\\te_serverframework _server.exe");
	_LIT(KTestStep1ServerName, "te_serverframework_server");
	_LIT(KInvalidServerName, "te_server?framework_server");

	TInt serverStartError = KErrNone;
	if (TestStepResult()==EPass)
		{
		serverParams.SetServerFileName(KNonExistentServerFileName);
		serverParams.SetServerName(KTestStep1ServerName);
		serverParams.SetNumberOfServerSlots(KServerFrameworkDefaultMessageSlots);
		serverParams.SetServerUids(KNullUid, KNullUid, KLbsNetRequestHandlerUid);
		
		serverStartError = CServerLaunch::ServerLaunch(serverParams);
		if(serverStartError != KErrNotFound)
			{
			INFO_PRINTF3(_L("***Error starting server with non-existent file. Returned %d, should be %d"), serverStartError, KErrNotFound);
			SetTestStepResult(EFail);	
			}

		serverParams.SetServerFileName(KInvalidServerFileName);
		serverParams.SetServerName(KTestStep1ServerName);
		serverStartError = CServerLaunch::ServerLaunch(serverParams);
		if(serverStartError != KErrNotFound)
			{
			INFO_PRINTF3(_L("***Error starting server with badly-formatted filename. Returned %d, should be %d"), serverStartError, KErrNotFound);
			SetTestStepResult(EFail);	
			}

		serverParams.SetServerFileName(KInvalidServerFilePathName);
		serverParams.SetServerName(KTestStep1ServerName);
		serverStartError = CServerLaunch::ServerLaunch(serverParams);
		if(serverStartError != KErrBadName)
			{
			INFO_PRINTF3(_L("***Error starting server with badly-formatted filename path. Returned %d, should be %d"), serverStartError, KErrNotFound);
			SetTestStepResult(EFail);	
			}
#if 0			
// TODO: Need to move this one to a separate test step, run at a sensible interval after this one,
// as if it's run here the 
// next one fails, because the server startup code finds a process hanging around
// (albeit with ExitType() == EExitKill, and doesn't do the whole setup business.
// There would be the same problem if things were the other way round as well.
// The alternative of course is to have a completely different dummy test server just for this 
// bit of the test step, which can then die with dignity in its own time....
		serverParams.SetServerFileName(KServerFileName);
		serverParams.SetServerName(KInvalidServerName);
		serverStartError = CServerLaunch::ServerLaunch(serverParams);
		if(serverStartError != KErrBadName)
			{
			INFO_PRINTF3(_L("***Error starting server with badly-formatted servername. Returned %d, should be %d"), serverStartError, KErrBadName);
			SetTestStepResult(EFail);	
			}
#endif
						
		serverParams.SetServerFileName(KServerFileName);
		serverParams.SetServerName(KTestStep1ServerName);
		serverStartError = CServerLaunch::ServerLaunch(serverParams);
		if(serverStartError != KErrNone)
			{
			INFO_PRINTF2(_L("***Error starting server - should have succeeded, returned %d"), serverStartError);
			SetTestStepResult(EFail);	
			}
		else
			{
			iSuicide.Attach(KProcessLaunchTestUid, KServerFrameworkTestSuicideKey);
			iSuicide.Set(1);
			}						
		}
		
	return TestStepResult();
	}



TVerdict CProcessLaunchTest1Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CProcessLaunchTest1Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_ProcessLaunchTestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
