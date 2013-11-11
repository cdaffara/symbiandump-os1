// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#include <s32mem.h>
#include <e32property.h>
#include "tsim_adaptationplugin_step.h"

//
// Run the tests
//
const TUint32 KSimPluginPropertyKey = 0x2000D76B;
const TUid KPropertyCategory={0x2000D75B};              // tcustomcmd_server SID = KSsmServerName SID (changed in tcustomcmd_server.mmp file)

CTestSimAdaptationPlugin::CTestSimAdaptationPlugin()
	:CAdaptationTestBase(KTCTestSimAdaptationPlugin)
	{
	
	}

CTestSimAdaptationPlugin::~CTestSimAdaptationPlugin()
	{

	}


void CTestSimAdaptationPlugin::TestGetSimOwned()
	{
	INFO_PRINTF1(_L(" >CTestSimAdaptationPlugin::TestGetSimOwned"));

	TRequestStatus status;
	TPckgBuf <TInt> ownedPckg;

	iSsmSimAdaptation.GetSimOwned(ownedPckg, status);
	User::WaitForRequest(status);
	if(iSsmSimAdaptation.Handle())
		{
		TEST(KErrNone == status.Int());	
		INFO_PRINTF3(_L("CTestSimAdaptationPlugin::TestGetSimOwned completed with %d error : expected %d>"),status.Int(),KErrNone);	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		INFO_PRINTF3(_L("CTestSimAdaptationPlugin::TestGetSimOwned completed with %d error : expected %d>"),status.Int(),KErrDisconnected);	
		}
	}

void CTestSimAdaptationPlugin::TestGetCancel()
	{
	INFO_PRINTF1(_L(" >CTestSimAdaptationPlugin::TestGetCancel"));
	iSsmSimAdaptation.GetCancel();
	INFO_PRINTF1(_L(" CTestSimAdaptationPlugin::TestGetCancel>"));		
	}

void CTestSimAdaptationPlugin::TestNotifySimEvent()
	{
	INFO_PRINTF1(_L(" >CTestSimAdaptationPlugin::TestNotifySimEvent"));

	TRequestStatus status;
	TPckgBuf <TInt> typePckg;
	typePckg = ESsmSimUsable;
	iSsmSimAdaptation.NotifySimEvent(typePckg, status);
	User::WaitForRequest(status);

	if(iSsmSimAdaptation.Handle())
		{
		TEST(KErrNone == status.Int());	
		INFO_PRINTF3(_L("CTestSimAdaptationPlugin::TestNotifySimEvent completed with %d error : expected %d>"),status.Int(),KErrNone);	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		INFO_PRINTF3(_L("CTestSimAdaptationPlugin::TestNotifySimEvent completed with %d error : expected %d>"),status.Int(),KErrDisconnected);	
		}
	}

void CTestSimAdaptationPlugin::TestNotifyCancel()
	{
	INFO_PRINTF1(_L(" >CTestSimAdaptationPlugin::TestNotifyCancel"));
	iSsmSimAdaptation.NotifyCancel();
	INFO_PRINTF1(_L(" CTestSimAdaptationPlugin::TestNotifyCancel >"));	
	}


//from CAdaptationTestBase
TVerdict CTestSimAdaptationPlugin::doTestStepL()
	{
	__UHEAP_MARK;
	
    TInt err = RProperty::Define(KPropertyCategory, KSimPluginPropertyKey, RProperty::EInt);
    TEST((KErrNone == err) || (KErrAlreadyExists == err));
    err = RProperty::Set(KPropertyCategory, KSimPluginPropertyKey, 1);
    TEST(KErrNone == err);
	
	TRAP(err, TestGetSimOwned());
	TEST(err == KErrNone);

	TRAP(err, TestNotifySimEvent());
	TEST(err == KErrNone);
	
	TestNotifyCancel();
	TestGetCancel();
	
	User::LeaveIfError(iSsmSimAdaptation.Connect());
	

	TRAP(err, TestGetSimOwned());
	TEST(err == KErrNone);
	
	TRAP(err, TestNotifySimEvent());
	TEST(err == KErrNone);
	
	TestNotifyCancel();
	TestGetCancel();
	//TestRelease();			// have to test this part too ...

    err = RProperty::Delete(KPropertyCategory, KSimPluginPropertyKey);
    TEST(KErrNone == err);
	__UHEAP_MARKEND;

	return TestStepResult();
	}
