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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include <e32base.h>
#include <e32debug.h>
#include <s32file.h>
#include "appfwk_sysstart_test_appUtils.h"
#include "appfwk_sysstart_test_consts.h"

_LIT(KSysStartTestApp3Panic, "SysStartTestApp3");

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("appfwk_sysstart_test_app3::E32Main - entry"));
	__UHEAP_MARK; // mark heap state
	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();
	
	TRAPD(err, StartTestL());
	if (err)
		{
		User::Panic(KSysStartTestApp3Panic,err);
		}
		
    delete TheTrapCleanup;

	__UHEAP_MARKEND; // check no memory leak

	RDebug::Print(_L("appfwk_sysstart_test_app3::E32Main - exit"));
	return KErrNone;
	}

LOCAL_C void StartTestL(void)
	{
	RDebug::Print(_L("appfwk_sysstart_test_app3::StartTestL - entry"));
	
	TAppfwkSysstartTestAppResults app2Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp2ResultFileName, app2Results);
	
	TAppfwkSysstartTestAppResults myResults;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp3ResultFileName, myResults);
	
	// check to make sure that t_app2 has rendezvous'd which proves
	// that SysStart has adhered to the EWaitForStart request in the
	// resource file.
	if (app2Results.iRendCount >= 1)
		{
		myResults.iStartCount++;
		}
		
	CAppfwkSysstartTestAppUtils::ReplaceTestAppResultsFileL(KTApp3ResultFileName, myResults);
	
	// for timeout test cases, t_app3 has been set a timeout of 1 second
	// to perform a rendezvous. The 3 second wait placed here, is to show that
	// sysstart will kill the process if it hasn't rendezvous'd. 
	User::After(3000000);
	myResults.iRendCount++;
	CAppfwkSysstartTestAppUtils::ReplaceTestAppResultsFileL(KTApp3ResultFileName, myResults);
	RProcess::Rendezvous(KErrNone);
	
	RDebug::Print(_L("appfwk_sysstart_test_app3::StartTestL - exit"));
	}
