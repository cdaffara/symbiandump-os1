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

#include <barsread.h> //TResourceReader
#include <barsc.h> //RResourceFile
#include <e32modes.h> //EStartupModeUndefined
#include <startupproperties.h>
#include <tstartupproperties.rsg> 
#include "tstartupproperties_stepdefval.h"

_LIT(KStartupPropertiesTestRsc,"z:\\resource\\test\\tstartupproperties.rsc");

CStartupPropertiesTestStepDefVal::CStartupPropertiesTestStepDefVal()
	{
	SetTestStepName (KCTestCaseDefVal);
	}

/**
Old Test CaseID 		APPFWK-STARTUPPROPERTIES-0001
New Test CaseID 		DEVSRVS-SYSSTART-STARTUPPROPERTIES-0001
 */
TVerdict CStartupPropertiesTestStepDefVal::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-STARTUPPROPERTIES-0001"));
	
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL (fsSession);
	
	RResourceFile rFile;
	TRAPD(err, rFile.OpenL(fsSession, KStartupPropertiesTestRsc));
	if(err)
		{
		ERR_PRINTF3(_L("Problem %d with test resource file %S"), err, &KStartupPropertiesTestRsc );
		User::Leave(err);
		}
	CleanupClosePushL (rFile);
	
	TestDefaultValues_CtorL();
	TestDefaultValues_START_APP_INFOL(rFile);
	TestDefaultValues_START_APP_INFO2L(rFile);
	TestDefaultValues_START_PROCESS_INFOL(rFile);
	TestDefaultValues_START_PROCESS_INFO2L(rFile);
	
	CleanupStack::PopAndDestroy(&rFile);
	CleanupStack::PopAndDestroy(&fsSession);

	return TestStepResult();
	}

CStartupProperties* CStartupPropertiesTestStepDefVal::CreateFromResourceLC(RResourceFile& aRFile, TInt aResourceId)
	{
	HBufC8* resource = aRFile.AllocReadLC (aResourceId);
	TResourceReader reader;
	reader.SetBuffer(resource);
	CStartupProperties* prop = CStartupProperties::NewL(reader);
	CleanupStack::PopAndDestroy(); //resource
	CleanupStack::PushL (prop);
	return prop;
	}

/** Verify default values set by C++ c'tor */
void CStartupPropertiesTestStepDefVal::TestDefaultValues_CtorL()
	{
	INFO_PRINTF1(_L("Verifying default values set by C++ c'tor"));	
	CStartupProperties* cprop = CStartupProperties::NewLC();
	TEST(cprop->Version() 				== 1);
	TEST(cprop->StartupType() 			== EStartProcess);
	TEST(cprop->StartMethod() 			== EFireAndForget);
	TEST(cprop->RecoveryMethod()		== EIgnoreOnFailure);
	TEST(cprop->RestartMode() 			== EStartupModeUndefined);
	// values here below not explicit initialised in c'tor
	TEST(cprop->ActionOnCommandFailure()== EIgnoreCommandFailure);
	TEST(cprop->FileName() 				== KNullDesC);
	TEST(cprop->Args() 					== KNullDesC);
	TEST(cprop->Viewless() 				== EFalse);
	TEST(cprop->StartInBackground()		== EFalse);
	TEST(cprop->Timeout() 				== 0);
	TEST(cprop->NoOfRetries() 			== 0);
	TEST(cprop->Version() 				== 1);
	TEST(cprop->Monitored() 			== EFalse);
	CleanupStack::PopAndDestroy (cprop);
	}

/** Verify default values set in struct START_APP_INFO */
void CStartupPropertiesTestStepDefVal::TestDefaultValues_START_APP_INFOL(RResourceFile& aRFile)
	{
	INFO_PRINTF1(_L("Verifying default values set in struct START_APP_INFO"));	
	CStartupProperties* rapp = CreateFromResourceLC(aRFile, R_APP_INFO_DEFAULT);
	TEST(rapp->StartupType() 			== EStartApp);
	TEST(rapp->FileName() 				== KNullDesC);
	TEST(rapp->Args() 					== KNullDesC);
	TEST(rapp->StartMethod() 			== EFireAndForget);
	TEST(rapp->Viewless() 				== EFalse);
	TEST(rapp->StartInBackground()		== EFalse);
	TEST(rapp->Timeout() 				== 0);
	TEST(rapp->ActionOnCommandFailure() == EPanicOnCommandFailure);
	TEST(rapp->NoOfRetries() 			== 0);
	// verify the ones not used by this struct as well
	TEST(rapp->RecoveryMethod()			== ERestartOS);
	TEST(rapp->Version() 				== 1);
	TEST(rapp->RestartMode() 			== EStartupModeUndefined);
	TEST(rapp->Monitored() 				== EFalse);
	CleanupStack::PopAndDestroy (rapp);
	}
	
/** Verify default values set in struct START_APP_INFO2 */
void CStartupPropertiesTestStepDefVal::TestDefaultValues_START_APP_INFO2L(RResourceFile& aRFile)
	{
	INFO_PRINTF1(_L("Verifying default values set in struct START_APP_INFO2"));	
	CStartupProperties* rapp2 = CreateFromResourceLC(aRFile, R_APP_INFO2_DEFAULT);
	TEST(rapp2->StartupType() 			== EStartApp);
	TEST(rapp2->FileName() 				== KNullDesC);
	TEST(rapp2->Args() 					== KNullDesC);
	TEST(rapp2->StartMethod() 			== EFireAndForget);
	TEST(rapp2->Viewless() 				== EFalse);
	TEST(rapp2->StartInBackground()		== EFalse);
	TEST(rapp2->Timeout() 				== 0);
	TEST(rapp2->RecoveryMethod()		== ERestartOS);
	TEST(rapp2->NoOfRetries() 			== 0);
	TEST(rapp2->Monitored() 			== EFalse);
	TEST(rapp2->RestartMode() 			== EStartupModeUndefined);
	// verify the ones not used by this struct as well
	TEST(rapp2->ActionOnCommandFailure()== EPanicOnCommandFailure);
	TEST(rapp2->Version() 				== 1);
	CleanupStack::PopAndDestroy (rapp2);
	}

/** Verify default values set in struct START_PROCESS_INFO */
void CStartupPropertiesTestStepDefVal::TestDefaultValues_START_PROCESS_INFOL(RResourceFile& aRFile)
	{
	INFO_PRINTF1(_L("Verifying default values set in struct START_PROCESS_INFO"));		
	CStartupProperties* rproc = CreateFromResourceLC(aRFile, R_PROCESS_INFO_DEFAULT);
	TEST(rproc->StartupType() 			== EStartProcess);
	TEST(rproc->FileName() 				== KNullDesC);
	TEST(rproc->Args() 					== KNullDesC);
	TEST(rproc->StartMethod() 			== EFireAndForget);
	TEST(rproc->Timeout() 				== 0);
	TEST(rproc->ActionOnCommandFailure()== EPanicOnCommandFailure);
	TEST(rproc->NoOfRetries() 			== 0);
	// verify the ones not used by this struct as well
	TEST(rproc->RecoveryMethod()		== ERestartOS);
	TEST(rproc->Version() 				== 1);	
	TEST(rproc->Viewless() 				== EFalse);
	TEST(rproc->StartInBackground()		== EFalse);
	TEST(rproc->Monitored() 			== EFalse);
	TEST(rproc->RestartMode() 			== EStartupModeUndefined);
	CleanupStack::PopAndDestroy (rproc);
	}
	
/** Verify default values set in struct START_PROCESS_INFO2 */
void CStartupPropertiesTestStepDefVal::TestDefaultValues_START_PROCESS_INFO2L(RResourceFile& aRFile)
	{
	INFO_PRINTF1(_L("Verifying default values set in struct START_PROCESS_INFO2"));			
	CStartupProperties* rproc2 = CreateFromResourceLC(aRFile, R_PROCESS_INFO2_DEFAULT);
	TEST(rproc2->StartupType() 			== EStartProcess);
	TEST(rproc2->FileName() 			== KNullDesC);
	TEST(rproc2->Args() 				== KNullDesC);
	TEST(rproc2->StartMethod() 			== EFireAndForget);
	TEST(rproc2->Timeout() 				== 0);
	TEST(rproc2->RecoveryMethod()		== ERestartOS);
	TEST(rproc2->NoOfRetries() 			== 0);
	TEST(rproc2->Monitored() 			== EFalse);
	TEST(rproc2->RestartMode() 			== EStartupModeUndefined);
	// verify the ones not used by this struct as well
	TEST(rproc2->ActionOnCommandFailure()== EPanicOnCommandFailure);
	TEST(rproc2->Version() 				== 1);	
	TEST(rproc2->Viewless() 			== EFalse);
	TEST(rproc2->StartInBackground()	== EFalse);
	CleanupStack::PopAndDestroy(rproc2);
	}
