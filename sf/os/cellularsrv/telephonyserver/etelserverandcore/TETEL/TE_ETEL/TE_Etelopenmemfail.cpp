// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TE_EtelBase.h"
#include "TE_Etelopenmemfail.h"

CTestOpenMemFail::CTestOpenMemFail()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenMemFail"));
	}

enum TVerdict CTestOpenMemFail::doTestStepL()
	{
	INFO_PRINTF1(_L("Test server Open Memory Failure"));

// OOM testing only works with debug builds
#ifdef _DEBUG
	//
	// Execute increasing failure rates until success is acheived.
	//
	TInt  failure = 0;
	const TInt KMaxTests = 1000;
	TBool oomAvailable = EFalse;

	// Floggerv2 based logging in Etel is designed to not produce an error 
	// when it encounters a problem and hence the simulated heap failure 
	// is considered to have been "eaten" as the function call will pass 
	// unexpectedly. To overcome this we loop through the test until the
	// function call passes and we still have the simulated heap allocation 
	// failure available to us because it has not been consumed by either a
	// genuine heap allocation in Etel or a call to the logger.
	while (!oomAvailable && failure < KMaxTests)
		{
		INFO_PRINTF2(_L("Failing alloc %d. Loading phone module... "),failure);

		//
		// Connect to the server and end the test if it fails....
		//
		RTelServer server;
		TInt ret;

		//
		// If the server doesn't connect, then we need to return a fail as trying to
		// execute any other call on 'server' will panic this process.
		//
		ret = server.Connect();
		INFO_PRINTF2(_L("Connect result was %d"),ret);
		TESTCHECK(ret, KErrNone);
		if (ret != KErrNone)
			{
			return(EFail);
			}

		//
		// Set the failure rate so that only failure - 1 allocs will be successful.
		// This will cause either KErrNone or KErrNoMemory.
		//
		server.__DbgFailNext(failure);

   		ret = server.LoadPhoneModule(DSTD_MODULE_NAME);
   		
   		TESTCHECKCONDITION(ret == KErrNone  ||  ret == KErrNoMemory);
		
		if (ret == KErrNone)
			{
			INFO_PRINTF2(_L("Passing after %d Allocs"),failure);

			ret = server.UnloadPhoneModule(DSTD_MODULE_NAME);
			TESTCHECK(ret, KErrNone);
			}
		else
			{
			INFO_PRINTF2(_L("Failing after %d Allocs"),failure);
			server.__DbgFailNext(0);
			
			if (ret == KErrNoMemory)
				{
				//
				// This execution did not succeed, but that's to be expected.  Check the
				// unloading of the phone module is okay.
				//
  				ret = server.UnloadPhoneModule(DSTD_MODULE_NAME);
				TESTCHECK(ret, KErrNotFound);
				}
			}
		
			
		server.__DbgFailNextAvailable(oomAvailable);	

		ServerClose(server);	
			
		failure++;
		}
	
	// KMaxTests is used to stop the above while statement looping
	// indefinitely. If KMaxTests is reached then we can guess that something
	// has gone wrong and the test should be failed.
	TESTCHECKCONDITION(failure < KMaxTests);

#else
	INFO_PRINTF1(_L("Test disabled on release builds."));	
#endif // _DEBUG
	
	return TestStepResult();
	}


