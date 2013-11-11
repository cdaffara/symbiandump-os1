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

#include <e32const.h>	// KMaxFilename
#include <e32modes.h>	// EStartupModeUndefined
#include <startupproperties.h>
#include "tstartupproperties_stepinternalexternal.h"

CStartupPropertiesTestStepInternalExternal::CStartupPropertiesTestStepInternalExternal()
	{
	SetTestStepName(KCTestCaseInternalExternal);
	}

/**
Old Test CaseID 		APPFWK-STARTUPPROPERTIES-0002
New Test CaseID 		DEVSRVS-SYSSTART-STARTUPPROPERTIES-0002
 */

TVerdict CStartupPropertiesTestStepInternalExternal::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-STARTUPPROPERTIES-0002"));
	DoTestWithDataL();
	DoTestWithNullPointersL();
	return TestStepResult();
	}

void CStartupPropertiesTestStepInternalExternal::DoTestWithDataL()
	{
	INFO_PRINTF1(_L("Test 1A, using valid HBufC:s"));
	
	// Create an startupprop instance with non default values
	CStartupProperties* orig = CStartupProperties::NewLC();
	_LIT(KString1, "abcåäö_logfile.txt");
	_LIT(KString2, "abcåäö logfile txt");
	orig->SetFileParamsL(KString1, KString2);
	orig->SetStartupType(EStartApp);
	orig->SetStartMethod(EWaitForStart);
	const TInt KNumber1 = 999;
	orig->SetNoOfRetries(KNumber1);
	orig->SetTimeout(KNumber1);
	const TInt KNonDefaultRestartMode = EStartupModeUndefined+10;
	orig->SetRecoveryParams(ERestartOSWithMode, KNonDefaultRestartMode);    
	orig->SetMonitored(ETrue);
	orig->SetViewless(ETrue);
	orig->SetStartInBackground(ETrue);

	//Externalize it
	CBufFlat* const buf = CBufFlat::NewL(orig->Size());
	CleanupStack::PushL(buf);
	orig->ExternalizeL(*buf);
	
	// Create a new startupprop instance 
	CStartupProperties* copy = CStartupProperties::NewLC();
	
	// Make sure orig uses non-default values
    TEST(orig->FileName() 				!= copy->FileName());
    TEST(orig->Args() 					!= copy->Args());
    TEST(orig->StartupType() 			!= copy->StartupType());
    TEST(orig->StartMethod() 			!= copy->StartMethod());
    TEST(orig->NoOfRetries() 			!= copy->NoOfRetries());
    TEST(orig->Timeout() 				!= copy->Timeout());
    TEST(orig->Monitored() 				!= copy->Monitored());
    TEST(orig->ActionOnCommandFailure() != copy->ActionOnCommandFailure());
    TEST(orig->RecoveryMethod() 		!= copy->RecoveryMethod());
    TEST(orig->RestartMode() 			!= copy->RestartMode());
    TEST(orig->Viewless() 				!= copy->Viewless());
    TEST(orig->StartInBackground() 		!= copy->StartInBackground());
	
	// Internalize the data
	TPtr8 bufPtr(buf->Ptr(0));
	copy->InternalizeL(bufPtr);
	
	//And TEST that all members was read back successfully
    TEST(orig->Version() 				== copy->Version());
    TEST(orig->FileName() 				== copy->FileName());
    TEST(orig->Args() 					== copy->Args());
    TEST(orig->StartupType() 			== copy->StartupType());
    TEST(orig->StartMethod() 			== copy->StartMethod());
    TEST(orig->NoOfRetries() 			== copy->NoOfRetries());
    TEST(orig->Timeout() 				== copy->Timeout());
    TEST(orig->Monitored() 				== copy->Monitored());
    TEST(orig->ActionOnCommandFailure() == copy->ActionOnCommandFailure());
    TEST(orig->RecoveryMethod() 		== copy->RecoveryMethod());
    TEST(orig->RestartMode() 			== copy->RestartMode());
    TEST(orig->Viewless() 				== copy->Viewless());
    TEST(orig->StartInBackground() 		== copy->StartInBackground());
	
	//Externalize the second instance
	CBufFlat* const buf2 = CBufFlat::NewL(copy->Size());
	CleanupStack::PushL(buf2);
	copy->ExternalizeL(*buf2);

	//Assert that the first and second buffer have the same contents
	TEST(buf->Ptr(0) == buf2->Ptr(0));
    
	CleanupStack::PopAndDestroy(buf2);
	CleanupStack::PopAndDestroy(copy);
	CleanupStack::PopAndDestroy(buf);		
	CleanupStack::PopAndDestroy(orig);	
	}

/**
Protect against NULL-pointer errors in the externalize/internalize methods.
*/
void CStartupPropertiesTestStepInternalExternal::DoTestWithNullPointersL()
	{
	INFO_PRINTF1(_L("Test 1B, using NULL pointers"));
	
	// Create an startupprop instance with non NULL iFilename and iArgs
	CStartupProperties* orig = CStartupProperties::NewLC();
	
	//SetActionOnCommandFailure wasn't covered in any other testcase, so to increase testcoverage test it here
	orig->SetActionOnCommandFailure(EPanicOnCommandFailure);
	TEST(orig->ActionOnCommandFailure() == EPanicOnCommandFailure);
	
	//Externalize it
	CBufFlat* const buf = CBufFlat::NewL(orig->Size());
	CleanupStack::PushL(buf);
	orig->ExternalizeL(*buf); // this call could dereference NULL
	
	// Create a new startupprop instance 
	CStartupProperties* copy = CStartupProperties::NewLC();
	
	// Internalize the data
	TPtr8 bufPtr(buf->Ptr(0));
	copy->InternalizeL(bufPtr);
	
	//And TEST that all members was read back successfully
    TEST(orig->Version() 				== copy->Version());
    TEST(orig->FileName() 				== copy->FileName());
    TEST(orig->Args() 					== copy->Args());
    TEST(orig->StartupType() 			== copy->StartupType());
    TEST(orig->StartMethod() 			== copy->StartMethod());
    TEST(orig->NoOfRetries() 			== copy->NoOfRetries());
    TEST(orig->Timeout() 				== copy->Timeout());
    TEST(orig->Monitored() 				== copy->Monitored());
    TEST(orig->ActionOnCommandFailure() == copy->ActionOnCommandFailure());
    TEST(orig->RecoveryMethod() 		== copy->RecoveryMethod());
    TEST(orig->RestartMode() 			== copy->RestartMode());
    TEST(orig->Viewless() 				== copy->Viewless());
    TEST(orig->StartInBackground() 		== copy->StartInBackground());
	
	//Externalize the second instance
	CBufFlat* const buf2 = CBufFlat::NewL(copy->Size());
	CleanupStack::PushL(buf2);
	copy->ExternalizeL(*buf2);

	//Assert that the first and second buffer have the same contents
	TEST(buf->Ptr(0) == buf2->Ptr(0));
    
	CleanupStack::PopAndDestroy(buf2);
	CleanupStack::PopAndDestroy(copy);
	CleanupStack::PopAndDestroy(buf);		
	CleanupStack::PopAndDestroy(orig);
	}
