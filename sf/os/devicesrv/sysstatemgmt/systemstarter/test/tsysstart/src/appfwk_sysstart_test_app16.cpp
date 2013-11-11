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

_LIT(KSysStartTestApp16Panic, "SysStartTestApp16");

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("appfwk_sysstart_test_app16::E32Main - entry"));
	__UHEAP_MARK; // mark heap state
	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();
	
	TRAPD(err, StartTestL());
	if (err)
		{
		User::Panic(KSysStartTestApp16Panic,err);
		}
		
    delete TheTrapCleanup;

	__UHEAP_MARKEND; // check no memory leak

	RDebug::Print(_L("appfwk_sysstart_test_app16::E32Main - exit"));
	return KErrNone;
	}

LOCAL_C void StartTestL(void)
	{
	RDebug::Print(_L("appfwk_sysstart_test_app16::StartTestL - entry"));
	
	TAppfwkSysstartTestAppResults myResults;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp16ResultFileName, myResults);
	
	TAppfwkSysstartTestAppResults app12Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp12ResultFileName, app12Results);
	TAppfwkSysstartTestAppResults app14Results;
	CAppfwkSysstartTestAppUtils::ReadTestAppResultsFileL(KTApp14ResultFileName, app14Results);
	
	if ((app12Results.iRendCount==1) && (app14Results.iRendCount==1))
		{
		myResults.iStartCount++;
		}
	
	CAppfwkSysstartTestAppUtils::ReplaceTestAppResultsFileL(KTApp16ResultFileName, myResults);
	
	RDebug::Print(_L("appfwk_sysstart_test_app16::StartTestL - exit"));
	}

