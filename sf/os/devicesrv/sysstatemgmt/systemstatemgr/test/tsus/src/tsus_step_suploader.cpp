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
 @internalComponent - Internal Symbian test code 
*/

#include <ssm/ssmsuscli.h>
#include "suspluginloader.h"
#include "suspluginframe.h"
#include "tsus_step_suploader.h"

_LIT(KFileNameMissing,	"missing.bin");
_LIT(KFileNameBadSup,	"tsus_badsup.dll");
_LIT(KFileNameGoodSup,	"tsus_goodsup.dll");
const TUid KIdentityMissing = {123};

//
// TestStep
//

CSsmSupLoaderTest::~CSsmSupLoaderTest()
	{
	}

CSsmSupLoaderTest::CSsmSupLoaderTest()
	{
	SetTestStepName(KTSsmSupLoaderStep);
	}

/** */
TVerdict CSsmSupLoaderTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/**
Old Test CaseID 		APPFWK-SUS-0004
New Test CaseID 		DEVSRVS-SSMA-SUS-0004
 */

TVerdict CSsmSupLoaderTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmSupLoaderTest tests started...."));

	__UHEAP_MARK;		
	TRAPD(err,doTestLoadDllFileLCL());
	TEST(KErrNone == err);
	
	TRAP(err,doTestCreatePluginLCL());
	TEST(KErrNone == err);
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("....CSsmSupLoaderTest tests completed!"));
	
	return TestStepResult();
	}

/** */
TVerdict CSsmSupLoaderTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

void CSsmSupLoaderTest::doTestLoadDllFileLCL()
	{
	RLibrary lib;
	
	INFO_PRINTF1(_L("Testing error code for missing file / wrong filename"));
	TSsmSupInfo info(KFileNameMissing);	
	TRAPD(err, SusPluginLoader::LoadDllFileLC(lib, info));
	INFO_PRINTF3(_L("Loading a missing file / wrong filename: Expected: %d, Returned: %d"), KErrNotFound, err);
	TEST(KErrNotFound == err);
	
	INFO_PRINTF1(_L("Testing error code for wrong DLL type"));
	info.SetL(KFileNameBadSup, 1, KNullUid);	
	TRAP(err, SusPluginLoader::LoadDllFileLC(lib, info));
	INFO_PRINTF3(_L("Loading a wrong DLL type: Expected: %d, Returned: %d"), KErrNotSupported, err);
	TEST(KErrNotSupported == err);
	
	INFO_PRINTF1(_L("Testing error code for wrong identity"));
	info.SetL(KFileNameGoodSup, 1, KIdentityMissing);	
	TRAP(err, SusPluginLoader::LoadDllFileLC(lib, info));
	INFO_PRINTF3(_L("Loading a wrong identity: Expected: %d, Returned: %d"), KErrBadName, err);
	TEST(KErrBadName == err);

	INFO_PRINTF1(_L("Testing that loading a valid Sup works"));
	info.SetL(KFileNameGoodSup, 1, KNullUid);	
	INFO_PRINTF1(_L("Loading a valid Sup, SetL returned"));
	TRAP(err, SusPluginLoader::LoadDllFileLC(lib, info); CleanupStack::Pop(&lib);); 
	INFO_PRINTF3(_L("Loading a valid Sup: Expected: %d, Returned: %d"), KErrNone, err);
	TEST(KErrNone == err);
	TEST(lib.Handle() != KNullHandle);
	 //Close lib handle
	 lib.Close();
	}

void CSsmSupLoaderTest::doTestCreatePluginLCL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler();
    CleanupStack::PushL( sched );
    CActiveScheduler::Install( sched );
    
	RLibrary lib;
	
	INFO_PRINTF1(_L("Test valid Sup Creation"));
	TSsmSupInfo info(KFileNameGoodSup);	
	INFO_PRINTF1(_L("Load Sup ..."));
	TRAPD(err, SusPluginLoader::LoadDllFileLC(lib, info); CleanupStack::Pop(&lib););
	TEST(KErrNone==err);
	
	CleanupClosePushL(lib);
	CSusPluginFrame* frame = NULL;
	INFO_PRINTF1(_L("Create Sup plugin ..."));
	TRAP(err, frame = SusPluginLoader::CreatePluginLC(lib, 1); CleanupStack::Pop(frame););
	TEST(KErrNone == err);
	TEST(frame != NULL);
	INFO_PRINTF1(_L("Destroy Sup plugin and unload it..."));
	delete frame;
	frame = NULL;
	CleanupStack::PopAndDestroy(&lib);
	
	INFO_PRINTF1(_L("Test create Sup using the wrong ordinal function, one that returns NULL"));
	INFO_PRINTF1(_L("Load the library ..."));
	TEST(KErrNone == lib.Load(KFileNameBadSup));
	INFO_PRINTF1(_L("Create Sup plugin ..."));
	TRAPD(ret, frame = SusPluginLoader::CreatePluginLC(lib, 1));
	TEST(frame == NULL);
	INFO_PRINTF3(_L("Create Sup plugin: Expected: %d, Returned: %d"), KErrNoMemory, ret);
	TEST(KErrNoMemory == ret);
	lib.Close();
	
	CleanupStack::PopAndDestroy( sched );
	}
