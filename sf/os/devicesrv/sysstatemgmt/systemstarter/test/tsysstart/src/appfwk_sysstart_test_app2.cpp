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

_LIT(KSysStartTestApp2Panic, "SysStartTestApp2");

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("appfwk_sysstart_test_app2::E32Main - entry"));
	__UHEAP_MARK; // mark heap state
	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();
	
	TRAPD(err, StartTestL());
	if (err)
		{
		User::Panic(KSysStartTestApp2Panic,err);
		}
		
    delete TheTrapCleanup;

	__UHEAP_MARKEND; // check no memory leak

	RDebug::Print(_L("appfwk_sysstart_test_app2::E32Main - exit"));
	return KErrNone;
	}

LOCAL_C void StartTestL(void)
	{
	RDebug::Print(_L("appfwk_sysstart_test_app2::StartTestL - entry"));
	
	TAppfwkSysstartTestAppResults myResults;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp2ResultFileName, myResults);
	
	// start count will only be increased if the t_app1 results file is found,
	// and the rendezvouz count for t_app1 is 0, i.e. SysStart has done a fire and
	// forget of t_app1 before then starting t_app2. This is important when 
	// implementing the startup resource files.
	TAppfwkSysstartTestAppResults app1Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp1ResultFileName, app1Results);
	if (app1Results.iRendCount==0)
		{
		myResults.iStartCount++;
		}
	
	myResults.iRendCount++;
	CAppfwkSysstartTestAppUtils::ReplaceTestAppResultsFileL(KTApp2ResultFileName, myResults);
	User::After(5000000);
	RProcess::Rendezvous(KErrNone);
	
	RDebug::Print(_L("appfwk_sysstart_test_app2::StartTestL - exit"));
	}
