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


#include <e32property.h>
#include <s32mem.h>
#include "tmisc_adaptationplugin_step.h"

//
// Run the tests
//

const TUid KPropertyCategory={0x2000D75B};              // tcustomcmd_server SID = KSsmServerName SID (changed in tcustomcmd_server.mmp file)
const TUint32 KMiscPluginPropertyKey = 0x2000E658;

CTestMiscAdaptationPlugin::CTestMiscAdaptationPlugin()
	:CAdaptationTestBase(KTCTestMiscAdaptationPlugin)
	{
	
	}

CTestMiscAdaptationPlugin::~CTestMiscAdaptationPlugin()
	{

	}



void CTestMiscAdaptationPlugin::TestSecurityStateChange()
	{
	INFO_PRINTF1(_L(" >CTestMiscAdaptationPlugin::TestSecurityStateChange"));

	TRequestStatus status;
	TPckgBuf <TInt> responsePckg;
	TInt state = 0;
	iSsmMiscAdaptation.SecurityStateChange(state, responsePckg, status);
	User::WaitForRequest(status);

	if(iSsmMiscAdaptation.Handle())
		{
		TEST(KErrNone == status.Int());	
		INFO_PRINTF3(_L("CTestMiscAdaptationPlugin::TestSecurityStateChange completed with %d error : expected %d>"),status.Int(),KErrNone);	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		INFO_PRINTF3(_L("CTestMiscAdaptationPlugin::TestSecurityStateChange completed with %d error : expected %d>"),status.Int(),KErrDisconnected);	
		}
	}

void CTestMiscAdaptationPlugin::TestGetGlobalStartupMode()
	{
	INFO_PRINTF1(_L(" >CTestMiscAdaptationPlugin::TestGetGlobalStartupMode"));

	TRequestStatus status;
	TPckgBuf <TInt> modePckg;
	iSsmMiscAdaptation.GetGlobalStartupMode(modePckg, status);
	User::WaitForRequest(status);

	if(iSsmMiscAdaptation.Handle())
		{
		TEST(KErrNone == status.Int());	
		INFO_PRINTF3(_L("CTestMiscAdaptationPlugin::TestGetGlobalStartupMode completed with %d error : expected %d>"),status.Int(),KErrNone);	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		INFO_PRINTF3(_L("CTestMiscAdaptationPlugin::TestGetGlobalStartupMode completed with %d error : expected %d>"),status.Int(),KErrDisconnected);	
		}
	}

void CTestMiscAdaptationPlugin::DoTestPrepareSimLanguages(TSsmLanguageListPriority aPriority)
    {
    INFO_PRINTF1(_L(" >CTestMiscAdaptationPlugin::DoTestPrepareSimLanguages"));

    TRequestStatus status;
    TPckgBuf <TInt> sizePckg;

    iSsmMiscAdaptation.PrepareSimLanguages(aPriority, sizePckg, status);
    User::WaitForRequest(status);
    
    if(iSsmMiscAdaptation.Handle())
        {
        TEST(KErrNotSupported == status.Int()); 
        INFO_PRINTF3(_L("CTestMiscAdaptationPlugin::DoTestPrepareSimLanguages completed with %d error : expected %d>"),status.Int(),KErrNotSupported);    
        }
    else
        {
        TEST(KErrDisconnected == status.Int()); 
        INFO_PRINTF3(_L("CTestMiscAdaptationPlugin::DoTestPrepareSimLanguages completed with %d error : expected %d>"),status.Int(),KErrDisconnected);    
        }

    }

void CTestMiscAdaptationPlugin::TestPrepareSimLanguages()
	{
	INFO_PRINTF1(_L(" >CTestMiscAdaptationPlugin::TestPrepareSimLanguages"));

	DoTestPrepareSimLanguages(EPrimaryLanguages);

	DoTestPrepareSimLanguages(ESecondaryLanguages);
	
	DoTestPrepareSimLanguages(ETertiaryLanguages);
	}

void CTestMiscAdaptationPlugin::TestGetSimLanguages()
	{
	INFO_PRINTF1(_L(" >CTestMiscAdaptationPlugin::TestGetSimLanguages"));

	//Testing PrepareSimLaguages 
	DoTestPrepareSimLanguages(EPrimaryLanguages);

	TInt simLanguageCount = 2; //Setting simLanguageCount to dummy value '2' for testing as the server would not have written any response in sizePckg and hence it will contain garbage value
	TRAPD(res,iSsmMiscAdaptation.SimLanguagesArrayL(simLanguageCount));
	
	if(iSsmMiscAdaptation.Handle())
		{
		TEST(KErrNotSupported == res);	
		INFO_PRINTF3(_L("CTestMiscAdaptationPlugin::TestGetSimLanguages completed with %d error : expected %d>"),res,KErrNotSupported);	
		}
	else
		{
		TEST(KErrDisconnected == res);	
		INFO_PRINTF3(_L("CTestMiscAdaptationPlugin::TestGetSimLanguages completed with %d error : expected %d>"),res,KErrDisconnected);	
		}
	}

void CTestMiscAdaptationPlugin::TestGetHiddenReset()
	{
	INFO_PRINTF1(_L(" >CTestMiscAdaptationPlugin::TestGetHiddenReset"));

	TRequestStatus status;
	TPckgBuf<TBool> hiddenResetPckg;
	iSsmMiscAdaptation.GetHiddenReset(hiddenResetPckg, status);
	User::WaitForRequest(status);

	if(iSsmMiscAdaptation.Handle())
		{
		TEST(KErrNotSupported == status.Int());	
		INFO_PRINTF3(_L("CTestMiscAdaptationPlugin::TestGetHiddenReset completed with %d error : expected %d>"),status.Int(),KErrNotSupported);	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		INFO_PRINTF3(_L("CTestMiscAdaptationPlugin::TestGetHiddenReset completed with %d error : expected %d>"),status.Int(),KErrDisconnected);	
		}
	}

void CTestMiscAdaptationPlugin::TestCancel()
	{
	INFO_PRINTF1(_L(" >CTestMiscAdaptationPlugin::TestCancel"));
	iSsmMiscAdaptation.Cancel();
	INFO_PRINTF1(_L(" CTestMiscAdaptationPlugin::TestCancel>"));	
	}


//from CAdaptationTestBase
TVerdict CTestMiscAdaptationPlugin::doTestStepL()
	{
	__UHEAP_MARK;
	
	TInt err = RProperty::Define(KPropertyCategory, KMiscPluginPropertyKey, RProperty::EInt);
	INFO_PRINTF2(_L("Defining KMiscPluginPropertyKey returns %d"), err);
	TEST((KErrNone == err) || (KErrAlreadyExists == err));
	err = RProperty::Set(KPropertyCategory, KMiscPluginPropertyKey, 1);
	TEST(KErrNone == err);
		
	TestSecurityStateChange();

	TestGetGlobalStartupMode();
	
	TestPrepareSimLanguages();

	TestGetSimLanguages();
	
	TestGetHiddenReset();

	TestCancel();
	
	User::LeaveIfError(iSsmMiscAdaptation.Connect());

	TestSecurityStateChange();

	TestGetGlobalStartupMode();
	
	TestPrepareSimLanguages();

	TestGetSimLanguages();
	
	TestGetHiddenReset();

	TestCancel();
	//TestRelease();			// have to test this part too ...
	
	err = RProperty::Delete(KPropertyCategory, KMiscPluginPropertyKey);
	TEST(KErrNone == err);
	
	__UHEAP_MARKEND;

	return TestStepResult();
	}
