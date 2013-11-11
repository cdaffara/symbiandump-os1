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

/**
 @file
 @test
*/


#include <e32debug.h>

#include "egltestcommonprocess.h"
#include "egltestcommonstep.h"

// static
void EglTestCommonProcess::MainL()
	{
	TPckgBuf<TProcessInfo> pckgInfo;
	TInt ret = User::GetDesParameter(EProcSlotParams, pckgInfo);
	User::LeaveIfError(ret);
	TProcessInfo& info = pckgInfo();
	
	TBuf<200> bufTestDllName;
	ret = User::GetDesParameter(EProcSlotTestDllName, bufTestDllName);
	User::LeaveIfError(ret);

    TBuf<200> bufTestStepName;
    ret = User::GetDesParameter(EProcSlotTestStepName, bufTestStepName);
    User::LeaveIfError(ret);

#ifdef __WINS__
	// Construct and destroy a process-wide state object in emulator builds.
	// This will cause initialisation of PLS for EGL and SgDriver
	// and allow checking for leaks in tests
	eglReleaseThread();
#endif //__WINS__ 

    // Create test step and perform CTestStep style initialisation (e.g. logging)
	CEglTestStep* testStep = NULL;
    RLibrary library;
    if(library.Load(bufTestDllName) == KErrNone)
        {
        typedef CEglTestStep* (* TFnPtrGetEglTestStep) (const TDesC&);
        TFnPtrGetEglTestStep fnGetEglTestStep = reinterpret_cast<TFnPtrGetEglTestStep>(library.Lookup(1));
        testStep = fnGetEglTestStep(bufTestStepName);
        }
	
	if (testStep == NULL)
		{
		User::Leave(KErrNotFound);
		}
	CleanupStack::PushL(testStep);

	// perform CTestStep style initialisation (e.g. logging)
	testStep->PartialInitialiseL(bufTestStepName);
	
	User::LeaveIfError(testStep->iSemaphore[0].Open(EProcSlotSemaphore0, EOwnerThread));
	User::LeaveIfError(testStep->iSemaphore[1].Open(EProcSlotSemaphore1, EOwnerThread));
		
	// perform CTestStep pre-amble
	User::LeaveIfError(testStep->doTestStepPreambleL());
	
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	TBool sgIdHasBeenPassed = info.iSgId.iId != TUint64(0);
#else
	TBool sgIdHasBeenPassed = EFalse;
	for(TUint i = 0; i < 5; i++)
		{
		if((info.iSgId).iId[i] != 0)
			{
			sgIdHasBeenPassed = ETrue;
			break;
			}		
		}
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	
	if(!sgIdHasBeenPassed)
		{
		testStep->doProcessFunctionL(info.iIdx);
		}
	else
		{
		testStep->doProcessFunctionL(info.iIdx, info.iSgId);
		}

	// perform CTestStep post-amble
	User::LeaveIfError(testStep->doTestStepPostambleL());
	
	//clean-up
	CleanupStack::PopAndDestroy(testStep);
	}


GLDEF_C TInt E32Main()
	{
	//When EGL Logging is enabled this causes a file server session to be allocated
	//Which needs to be done before any allocation checks otherwise the test will fail
	eglReleaseThread();

	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	
	TRAPD(err, EglTestCommonProcess::MainL());
	
	delete cleanup;
	__UHEAP_MARKEND;

	return err;	
	}
