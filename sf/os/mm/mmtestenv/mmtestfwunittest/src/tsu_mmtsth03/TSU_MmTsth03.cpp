// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the test steps for Unit Test Suite 03 : Filename.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TSU_MmTsthStep03.h"
#include "TSU_MmTsthSuite03.h"

// Specific includes for these test steps
#include "TSU_MmTsth03.h"
#include "TestFramework/Filename.h"

// --------------------------------------------

// Unit Test Suite 03 : Filename.cpp
// Depends on : none

// Tests :-
// 1. Allocate and deallocate an object
// 2. Test assignment overloads
// 3. Test copy overloads
// 4. Test locate
// 5. Test slicing (Left)
// 11. Test data scope

// ---------------------
// RTestMmTsthU0301

RTestMmTsthU0301* RTestMmTsthU0301::NewL()
	{
	RTestMmTsthU0301* self = new(ELeave) RTestMmTsthU0301;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0301::RTestMmTsthU0301()
	{
	iTestStepName = _L("MM-TSTH-U-0301");
	}

// preamble
TVerdict RTestMmTsthU0301::OpenL()
	{
	// stub - to avoid calling RTestStep_MM_TSTH_U_03::OpenL() which
	// initialises iFileName
	return iTestStepResult = EPass;
	}

// postamble
void RTestMmTsthU0301::Close()
	{
	}

// do the test step.
TVerdict RTestMmTsthU0301::DoTestStepL()
	{
	// NB this test is the test for NewL() therefore it uses
	// a stubbed preamble / postamble
	
	INFO_PRINTF1(_L("Create a CFileName"));

	TVerdict currentVerdict = EPass;

	// create a CFileName with NewL
	CFileName* theFileName = NULL;
	TRAPD(err, theFileName = CFileName::NewL());
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CFileName::NewL left with code %d"), err);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("CFileName::NewL succeeded"));
	delete theFileName;

	// create a CFileName with NewLC
	CFileName* theLCFileName = NULL;
	TRAPD(err2, theLCFileName = CFileName::NewLC();
				CleanupStack::Pop(theLCFileName) );
	if(err2 != KErrNone)
		{
		ERR_PRINTF2(_L("CFileName::NewLC left with code %d"), err2);
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("CFileName::NewLC succeeded"));
	delete theLCFileName;
	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ------------------------
// RTestMmTsthU0302
RTestMmTsthU0302* RTestMmTsthU0302::NewL()
	{
	RTestMmTsthU0302* self = new(ELeave) RTestMmTsthU0302;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0302::RTestMmTsthU0302()
	{
	iTestStepName = _L("MM-TSTH-U-0302");
	}

// do the test step.
TVerdict RTestMmTsthU0302::DoTestStepL()
	{
	const TInt KTestBufSize = 48;	// allocate enough space for test strings

	INFO_PRINTF1(_L("Test CFileName assign overloads"));

	TVerdict currentVerdict = EPass;

	// test against TPtrC
	TPtrC testTPtrCStr = _L("Test TPtrC String");
	*iFileName = testTPtrCStr;
	if(testTPtrCStr != iFileName->FileName())
		{
		ERR_PRINTF1(_L("CFileName::operator=(const TPtrC&) failed"));
		return iTestStepResult = EFail;
		}

	// test against TDesC
	TBufC<KTestBufSize> testTDesCStr = _L("Test TDesC String");
	*iFileName = testTDesCStr;
	if(testTDesCStr != iFileName->FileName())
		{
		ERR_PRINTF1(_L("CFileName::operator=(const TDesC&) failed"));
		return iTestStepResult = EFail;
		}

	// test against TFileName
	TFileName testTFileNameStr = _L("Test TFileName String");
	*iFileName = testTFileNameStr;
	if(testTFileNameStr != iFileName->FileName())
		{
		ERR_PRINTF1(_L("CFileName::operator=(const TFileName&) failed"));
		return iTestStepResult = EFail;
		}

	// test against TText*
	const TText* testTTextStr = _S("Test TText String\0");
	*iFileName = testTTextStr;
	// convert testTTextStr for comparison purposes
	TBuf<KTestBufSize> testCompTTextStr(testTTextStr);
	if(testCompTTextStr != iFileName->FileName())
		{
		ERR_PRINTF1(_L("CFileName::operator=(const TText*) failed"));
		return iTestStepResult = EFail;
		}

	// test against self
	CFileName* testCFileName = CFileName::NewL();
	*testCFileName = _L("Test CFileName String");
	*iFileName = *testCFileName;
	if(testCFileName->FileName() != iFileName->FileName())
		{
		ERR_PRINTF1(_L("CFileName::operator=(const CFileName&) failed"));
		delete testCFileName;
		return iTestStepResult = EFail;
		}
	delete testCFileName;

	return iTestStepResult = currentVerdict; // should be EPass if we've got here

	}

// ------------------------
// RTestMmTsthU0303

RTestMmTsthU0303* RTestMmTsthU0303::NewL()
	{
	RTestMmTsthU0303* self = new(ELeave) RTestMmTsthU0303;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0303::RTestMmTsthU0303()
	{
	iTestStepName = _L("MM-TSTH-U-0303");
	}

// do the test step.
TVerdict RTestMmTsthU0303::DoTestStepL()
	{
	const TInt KTestBufSize = 48;	// allocate enough space for test strings

	INFO_PRINTF1(_L("Test CFileName::Copy"));

	TVerdict currentVerdict = EPass;

	// test against TDesC8
	TBufC8<KTestBufSize> testTDesC8Str = _L8("Test TDesC8 String");
	iFileName->Copy(testTDesC8Str);
	// convert testTDesC8Str for comparison purposes
	TBuf16<KTestBufSize> testCompTDesC8Str = _L16("Test TDesC8 String");
	if(testCompTDesC8Str != iFileName->FileName())
		{
		ERR_PRINTF1(_L("CFileName::Copy(const TDesC8&) failed"));
		return iTestStepResult = EFail;
		}

	// test against TDesC16
	TBufC16<KTestBufSize> testTDesC16Str = _L16("Test TDesC16 String");
	iFileName->Copy(testTDesC16Str);
	if(testTDesC16Str != iFileName->FileName())
		{
		ERR_PRINTF1(_L("CFileName::Copy(const TDesC16&) failed"));
		return iTestStepResult = EFail;
		}

	// test against TUint16* with length
	const TUint16* testTUint16Str = _S16("Test TUint16 String");
	// convert testTUint16Str for comparison purposes
	TBuf16<KTestBufSize> testCompTUint16Str(testTUint16Str);
	iFileName->Copy(testTUint16Str, testCompTUint16Str.Length());
	const TDesC& iFileNameLeft = iFileName->FileName().Left(testCompTUint16Str.Length());
	if(testCompTUint16Str != iFileNameLeft)
		{
		ERR_PRINTF1(_L("CFileName::Copy(const TUint16*, int) failed"));
		return iTestStepResult = EFail;
		}

	// test against TUint16* zero terminated
	const TUint16* testTUint16ZStr = _S16("Test TUint16Z String\0");
	iFileName->Copy(testTUint16ZStr);
	// convert testTUint16Str for comparison purposes
	TBuf16<KTestBufSize> testCompTUint16ZStr(testTUint16ZStr);
	if(testCompTUint16ZStr != iFileName->FileName())
		{
		ERR_PRINTF1(_L("CFileName::Copy(const TUint16*) failed"));
		return iTestStepResult = EFail;
		}

	return iTestStepResult = currentVerdict; // should be EPass if we've got here

	}

// ------------------------
// RTestMmTsthU0304
RTestMmTsthU0304* RTestMmTsthU0304::NewL()
	{
	RTestMmTsthU0304* self = new(ELeave) RTestMmTsthU0304;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0304::RTestMmTsthU0304()
	{
	iTestStepName = _L("MM-TSTH-U-0304");
	}

// Do the test step.
TVerdict RTestMmTsthU0304::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test CFileName::Locate"));

	TVerdict currentVerdict = EPass;

	// test is inconclusive if the assignment fails
	TPtrC testLocateStr = _L("ABCDEFG");
	*iFileName = testLocateStr;
	if(testLocateStr != iFileName->FileName())
		{
		ERR_PRINTF1(_L("CFileName::operator=(const TPtrC&) failed in Locate Test"));
		return iTestStepResult = EInconclusive;
		}
	TInt rc = iFileName->Locate('C');
	if(rc != 2)
		{
		ERR_PRINTF1(_L("CFileName::Locate() failed"));
		return iTestStepResult = EFail;
		}
	rc = iFileName->Locate('Z');
	if(rc != KErrNotFound)
		{
		ERR_PRINTF1(_L("CFileName::Locate() failed"));
		return iTestStepResult = EFail;
		}

	return iTestStepResult = currentVerdict; // should be EPass if we've got here
	}

// ------------------------
// RTestMmTsthU0305

RTestMmTsthU0305* RTestMmTsthU0305::NewL()
	{
	RTestMmTsthU0305* self = new(ELeave) RTestMmTsthU0305;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0305::RTestMmTsthU0305()
	{
	iTestStepName = _L("MM-TSTH-U-0305");
	}

// Do the test step.
TVerdict RTestMmTsthU0305::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test CFileName::Left"));

	TVerdict currentVerdict = EPass;

	// test is inconclusive if the assignment fails
	TPtrC testLocateStr = _L("ABCDEFG");
	*iFileName = testLocateStr;
	if(testLocateStr != iFileName->FileName())
		{
		ERR_PRINTF1(_L("CFileName::operator=(const TPtrC&) failed in Locate Test"));
		return iTestStepResult = EInconclusive;
		}
	const TFileName& iFileNameLeft = iFileName->Left(3);
	TPtrC testLeftStr = _L("ABC");

	if(testLeftStr != iFileNameLeft)
		{
		ERR_PRINTF1(_L("CFileName::Left() failed"));
		return iTestStepResult = EFail;
		}

	return iTestStepResult = currentVerdict; // should be EPass if we've got here

	}

// ------------------------
// RTestMmTsthU0311
RTestMmTsthU0311* RTestMmTsthU0311::NewL()
	{
	RTestMmTsthU0311* self = new(ELeave) RTestMmTsthU0311;
	return self;
	}

// Each test step initialises its own name.
RTestMmTsthU0311::RTestMmTsthU0311()
	{
	iTestStepName = _L("MM-TSTH-U-0311");
	}

// Do the test step.
TVerdict RTestMmTsthU0311::DoTestStepL()
	{
	const TInt KTestBufSize = 48;	// allocate enough space for test strings

	INFO_PRINTF1(_L("Test CFileName memory scope"));

	TVerdict currentVerdict = EPass;

	// send original copied string out of scope, to prove that CFileName
	// is using its own data space
	TPtr16 testDynTDesCStr(REINTERPRET_CAST(TUint16*,User::AllocLC(KTestBufSize*2)), 0, KTestBufSize);
	testDynTDesCStr = _L("Test TDesC String");
	*iFileName = testDynTDesCStr;

	CleanupStack::PopAndDestroy(); // testDynTDesCStr

	// if iFileName->FileName() is out of scope, this will panic with USER 7. If it doesn't, we've passed
	TPtrC resStr = iFileName->FileName();
	INFO_PRINTF2(_L("CFileName::FileName() is '%S'"), &resStr);

	return iTestStepResult = currentVerdict; // should be EPass if we've got here

	}
