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

_LIT(KSysStartTestApp20Panic, "SysStartTestApp20");

GLDEF_C TInt E32Main(void)
	{
	RDebug::Print(_L("appfwk_sysstart_test_app20::E32Main - entry"));
	__UHEAP_MARK; // mark heap state
	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();
	
	TRAPD(err, StartTestL());
	if (err)
		{
		User::Panic(KSysStartTestApp20Panic,err);
		}
		
    delete TheTrapCleanup;

	__UHEAP_MARKEND; // check no memory leak

	RDebug::Print(_L("appfwk_sysstart_test_app20::E32Main - exit"));
	return KErrNone;
	}

LOCAL_C void StartTestL(void)
	{
	RDebug::Print(_L("appfwk_sysstart_test_app20::StartTestL - entry"));
	
	TAppfwkSysstartTestAppResults myResults;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp20ResultFileName, myResults);
	
	User::After(100);
	
	TAppfwkSysstartTestAppResults app19Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp19ResultFileName, app19Results);
	
	if ((app19Results.iRendCount==0) && (app19Results.iStartCount==1))
		{
		myResults.iStartCount++;
		}
		
	myResults.iRendCount++;
	CAppfwkSysstartTestAppUtils::ReplaceTestAppResultsFileL(KTApp20ResultFileName, myResults);
	RProcess::Rendezvous(KErrNone);
	
	RDebug::Print(_L("appfwk_sysstart_test_app20::StartTestL - exit"));
	}
