// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UTILS.h"
#include "textendedbitmapcommon.h"
#include "textendedbitmappanic.h"
#include <e32math.h>
#include <s32file.h>
#include <bautils.h>
#include <hal.h>
#include <test/tefunit.h>


const TUint8 KTestData[] = "Extended bitmap test data 123456";
const TInt KTestDataSize = sizeof(KTestData);
const TSize KSizeInPixels = TSize(50,50);
const TDisplayMode KDisplayMode	= EColor64K;

GLREF_C void Panic(TFbsPanic aPanic);

CTExtendedBitmapPanic::CTExtendedBitmapPanic(CTestStep* aStep):
CTFbsBase(aStep, EFalse)
	{
	__ASSERT_DEBUG(CActiveScheduler::Current(), User::Invariant());	
	}

CTExtendedBitmapPanic::~CTExtendedBitmapPanic()
	{	
	delete iExtendedBitmap;
	}

void CTExtendedBitmapPanic::ConstructL()
	{
	iExtendedBitmap = new(ELeave)CFbsBitmap;
	TInt err = iExtendedBitmap->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	User::LeaveIfError(err);
	}

void CTExtendedBitmapPanic::RunFbsTestL(TInt /*aCurTestCase*/)
    {       
    _LIT(KCaseNumber, "CaseNumber");

    TInt stepNumber;
	TBool res = iStep->GetIntFromConfig(iStep->ConfigSection(), KCaseNumber, stepNumber);	
	if(!res)
		{
		return;
		}
	((CTExtendedBitmapPanicStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(stepNumber)
		{
	case 1:
		((CTExtendedBitmapPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0020"));
		TestSetScanLine();
		((CTExtendedBitmapPanicStep*)iStep)->RecordTestResultL();
		TEST(EFalse);
		TestComplete();
		break;
	case 2:
		((CTExtendedBitmapPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0041"));
		TestIsMonochrome();
		((CTExtendedBitmapPanicStep*)iStep)->RecordTestResultL();
		TEST(EFalse);
		TestComplete();
		break;
	default:
		((CTExtendedBitmapPanicStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
    }
   
/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0020

@SYMTestCaseDesc
	Panic test to check that CFbsBitmap::SetScanLine() panics when used with an extended bitmap

@SYMPREQ 
	PREQ2096
	
@SYMREQ
	REQ10847
	REQ10857	

@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions
	Create an extended bitmap using test data and test Uid.
	Call SetScanLine() on the extended bitmap.
		
@SYMTestExpectedResults
	CFbsBitmap::SetScanLine() should panic with FBSERV 28 (EFbsPanicBitmapReadOnly) in debug only
*/
void CTExtendedBitmapPanic::TestSetScanLine()
	{
	INFO_PRINTF1(_L("TestSetScanLine - panic test"));
		
	TBuf8<16> temp;		
	iExtendedBitmap->SetScanLine(temp, 0); // This should panic with FBSERV 28 (EFbsPanicBitmapReadOnly) - debug only panic
	
#ifndef _DEBUG
	// Force the panic when not in debug mode so that the test still passes
	INFO_PRINTF1(_L("Forcing a panic in release mode as SetScanLine() only panics in debug"));
	User::Panic(KFBSERVPanicCategory, EFbsPanicBitmapReadOnly);
#endif	
	}


/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0041

@SYMTestCaseDesc
	Panic test to check that CFbsBitmap::IsMonochrome() panics when used with an extended bitmap

@SYMPREQ
	CR1804

@SYMTestPriority 
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions
	Create an extended bitmap using test data and test Uid.
	Call IsMonochrome() on the extended bitmap.
		
@SYMTestExpectedResults
	CFbsBitmap::IsMonochrome() should panic with FBSERV 11 (EFbsBitmapInvalidCompression) in debug only
*/
void CTExtendedBitmapPanic::TestIsMonochrome()
	{
	INFO_PRINTF1(_L("TestIsMonochrome - panic test"));
		
	iExtendedBitmap->IsMonochrome(); // This should panic with FBSERV 11 (EFbsBitmapInvalidCompression) - debug only panic
	
#ifndef _DEBUG
	// Force the panic when not in debug mode so that the test still passes
	INFO_PRINTF1(_L("Forcing a panic in release mode as IsMonochrome() only panics in debug"));
	User::Panic(KFBSERVPanicCategory, EFbsBitmapInvalidCompression);
#endif	
	}

__CONSTRUCT_STEP__(ExtendedBitmapPanic)

