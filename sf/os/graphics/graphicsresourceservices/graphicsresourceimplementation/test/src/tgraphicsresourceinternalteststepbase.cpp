// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
@internalComponent
 */

#include "tgraphicsresourceinternalteststepbase.h"

CTGraphicsResourceInternalBase::CTGraphicsResourceInternalBase()
	{
	}

CTGraphicsResourceInternalBase::~CTGraphicsResourceInternalBase()
	{
	}

/**
Creates a second process and do some tests in it.
@param aExecutableName The name of the new process
@param aTestInfo The information for the tests
@leave Gets system wide error code
*/
TInt CTGraphicsResourceInternalBase::CreateSecondProcessAndDoTestL(const TDesC &aExecutableName, TSgResIntTestInfo& aTestInfo)
	{
	// Create a second process
    RProcess process;
    User::LeaveIfError(process.Create(aExecutableName, KNullDesC));
	CleanupClosePushL(process);

	// Specify the id passed to the second process
	TPckg<TSgResIntTestInfo> ptr(aTestInfo);
	TInt err = process.SetParameter(KSecondProcessParametersSlot, ptr);
	TESTEL(KErrNone == err, err);

	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);

	//ExitReason() returns the test result
	TInt exitreason = process.ExitReason();
	
	TEST(process.ExitType() == EExitKill);
	
	CleanupStack::PopAndDestroy();
	
	return exitreason;
	}

/**
Creates a second process, runs the requested test and ensures that
the specified panic occurs.

@param aExecutableName The name of the new process
@param aTestInfo The specification for this test
@param aPanicCategory Descriptor containing the start of the expected panic string e.g. for "ALLOC:xxxxxxxx" this would be "ALLOC"
@leave One of the system wide error codes
*/
void CTGraphicsResourceInternalBase::CreateSecondProcessAndCheckAllocPanicL(const TDesC &aExecutableName, TSgResIntTestInfo& aTestInfo, const TDesC &aPanicCategory)
	{
	// Create a second process
	RProcess process;
	TInt err = process.Create(aExecutableName, KNullDesC);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(process);

	// Specify the id passed to the second process
	TPckg<TSgResIntTestInfo> ptr(aTestInfo);
	err = process.SetParameter(KSecondProcessParametersSlot, ptr);
	TESTEL(KErrNone == err, err);

	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);

	if(EExitPanic != process.ExitType())
		{
		ERR_PRINTF3(_L("Expected exit type: %d, Actual exit type: %d"), EExitPanic, process.ExitType());
		TEST(EFalse);
		}

	TExitCategoryName secondProcessExitCategory = process.ExitCategory();
	if(0 != secondProcessExitCategory.Match(aPanicCategory))
		{
		ERR_PRINTF3(_L("Expected panic category: %S, Actual panic category: %S"), &aPanicCategory, &secondProcessExitCategory);
		TEST(EFalse);
		}
	
	CleanupStack::PopAndDestroy();
	}

