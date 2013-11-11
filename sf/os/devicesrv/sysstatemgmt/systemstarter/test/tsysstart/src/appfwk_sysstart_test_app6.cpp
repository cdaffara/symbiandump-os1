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

_LIT(KSysStartTestApp6Panic, "SysStartTestApp6");

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("appfwk_sysstart_test_app6::E32Main - entry"));
	__UHEAP_MARK; // mark heap state
	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();
	
	TRAPD(err, StartTestL());
	if (err)
		{
		User::Panic(KSysStartTestApp6Panic,err);
		}
		
    delete TheTrapCleanup;

	__UHEAP_MARKEND; // check no memory leak

	RDebug::Print(_L("appfwk_sysstart_test_app6::E32Main - exit"));
	return KErrNone;
	}

LOCAL_C void StartTestL(void)
	{
	RDebug::Print(_L("appfwk_sysstart_test_app6::StartTestL - entry"));
	
	// load in app3 results
	TAppfwkSysstartTestAppResults app3Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp3ResultFileName, app3Results);
	
	TAppfwkSysstartTestAppResults myResults;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp6ResultFileName, myResults);
	
	// check that t_app3 has been started 4 times, i.e. proving that
	// sysstart restarts t_app3 3 times before starting t_app6
	if (app3Results.iStartCount == 4)
		{
		myResults.iStartCount++;
		}
	
	CAppfwkSysstartTestAppUtils::ReplaceTestAppResultsFileL(KTApp6ResultFileName, myResults);
	
	RDebug::Print(_L("appfwk_sysstart_test_app6::StartTestL - exit"));
	}

