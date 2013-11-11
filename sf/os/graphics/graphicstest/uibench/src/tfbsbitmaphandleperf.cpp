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

#include "tfbsbitmaphandleperf.h"

const TInt KIterationsToTest = 1000;
const TInt KMaxNumSmallTestBitmaps = 40;
const TInt KMaxNumLargeTestBitmaps = 4;
const TInt KNumTestBitmapSizes = 8;
const TDisplayMode KTestDisplayMode = EColor256;
const TDisplayMode KTestDisplayMode2 = EColor64K;


CTFbsBitmapHandlePerf::CTFbsBitmapHandlePerf()
	{
	SetTestStepName(KTFbsBitmapHandlePerfName);
	} 

CTFbsBitmapHandlePerf::~CTFbsBitmapHandlePerf()
	{
	iTestBitmaps.ResetAndDestroy();
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTFbsBitmapHandlePerf::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0074"));
	SmallBitmapCreationL(KTestDisplayMode, _L("Small Bitmap-Create")); //8 bit
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0006"));
	LargeBitmapCreationL(KTestDisplayMode, _L("Large Bitmap-Create"));
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0087"));
	SmallBitmapCreationSimpleL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0088"));
	LargeBitmapCreationSimpleL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0007"));
	BitmapDuplicateL();		
	RecordTestResultL();
	return TestStepResult();
	}

/**
Creates and destroys bitmaps KIterationsToTest times with various sizes
using a pseudo-random sequence to try to simulate real usage patterns.

@param aSizes Array of KNumTestBitmapSizes sizes to create bitmaps with.
@param aMaxNumBitmaps Maximum number of created but not yet deleted bitmaps at any given time.
@param aDisplayMode The display mode to create bitmaps with.
*/
void CTFbsBitmapHandlePerf::BitmapCreationL(const TSize aSizes[], TInt aMaxNumBitmaps, const TDisplayMode aDisplayMode)
	{
	TInt64 seed = 0;
	iProfiler->InitResults();
	for (TInt count = KIterationsToTest; count > 0; --count)
		{
		CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bitmap);
		TInt err = bitmap->Create(aSizes[count % KNumTestBitmapSizes], aDisplayMode);
		User::LeaveIfError(err);
		iTestBitmaps.AppendL(bitmap);
		CleanupStack::Pop(bitmap);
		if (iTestBitmaps.Count() >= aMaxNumBitmaps)
			{
			TInt i = Math::Rand(seed) % aMaxNumBitmaps;
			delete iTestBitmaps[i];
			iTestBitmaps.Remove(i);
			}
		iProfiler->MarkResultSetL();
		}
	iTestBitmaps.ResetAndDestroy();
	}

/**
Creates and destroys bitmaps of the same size KIterationsToTest times.

@param aWidth The width of the created bitmaps.
@param aHeight  The height of the created bitmaps.
@param aDisplayMode The display mode to create bitmaps with.
@param aTestDescription The description of the test.
*/
void CTFbsBitmapHandlePerf::BitmapCreationSimpleL(const TInt aWidth, const TInt aHeight, const TDisplayMode aDisplayMode, const TDesC& aTestDescription)
	{
	iProfiler->InitResults();
	for (TInt count = KIterationsToTest; count > 0; --count)
		{
		CFbsBitmap bitmap;
		TInt err = bitmap.Create(TSize(aWidth, aHeight), aDisplayMode);
		User::LeaveIfError(err);
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(aTestDescription, aDisplayMode, 0, 0, KIterationsToTest);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0074

@SYMTestCaseDesc
The test determines how long it takes to create and destroy small bitmap objects.

@SYMTestActions
Compare the results over time, and before and after changes to bitmap construction and destruction code.

@SYMTestExpectedResults
Test should pass and display total test time and time per bitmap

@param aDisplayMode The display mode to create bitmaps with.
@param aTestDescription The description of the test.
*/
void CTFbsBitmapHandlePerf::SmallBitmapCreationL(const TDisplayMode aDisplayMode, const TDesC& aTestDescription)
	{
	// width X height X bytes per pixel < KMaxLargeBitmapAlloc (16KB as of writing)
	const TSize KSmallSizes[KNumTestBitmapSizes] =
		{
		TSize(20, 20), // 1st
		TSize(22, 22), // 2nd
		TSize(24, 24), // 3rd
		TSize(28, 28), // 4th
		TSize(32, 32), // 5th
		TSize(40, 40), // 6th
		TSize(48, 48), // 7th
		TSize(60, 60)  // 8th
		};
	BitmapCreationL(KSmallSizes, KMaxNumSmallTestBitmaps, aDisplayMode);
	iProfiler->ResultsAnalysis(aTestDescription, aDisplayMode, 0, 0, KIterationsToTest);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0006

@SYMTestCaseDesc
The test determines how long it takes to create and destroy large bitmap objects.

@SYMTestActions
Compare the results over time, and before and after changes to bitmap construction and destruction code.

@SYMTestExpectedResults
Test should pass and display total test time and time per bitmap

@param aDisplayMode The display mode to create bitmaps with.
@param aTestDescription The description of the test.
*/
void CTFbsBitmapHandlePerf::LargeBitmapCreationL(const TDisplayMode aDisplayMode, const TDesC& aTestDescription)
	{
	// width X height X bytes per pixel > KMaxLargeBitmapAlloc (16KB as of writing)
	const TSize KLargeSizes[KNumTestBitmapSizes] =
		{
		TSize(200, 200), // 1st
		TSize(220, 220), // 2nd
		TSize(240, 240), // 3rd
		TSize(280, 280), // 4th
		TSize(320, 320), // 5th
		TSize(400, 400), // 6th
		TSize(480, 480), // 7th
		TSize(600, 600)  // 8th
		};
	BitmapCreationL(KLargeSizes, KMaxNumLargeTestBitmaps, aDisplayMode);
	iProfiler->ResultsAnalysis(aTestDescription, aDisplayMode, 0, 0, KIterationsToTest);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0007

@SYMTestCaseDesc
Tests how long it takes to duplicate a bitmap

@SYMTestActions
Compare the results over time, and before and after changes to bitmap duplication code.

@SYMTestExpectedResults
Test should pass and display total test time and time per bitmap
*/
void CTFbsBitmapHandlePerf::BitmapDuplicateL()
	{
	
	TSize size(200,200);
	TInt err;
	
	RArray<TInt> handles;
	RPointerArray<CFbsBitmap> bitmapArray;
	CleanupClosePushL(bitmapArray);
	
	// Create bitmaps for each display mode and store in array
	for(TInt count=KNumValidBitmapModes-1; count>=0; --count)
		{
		CFbsBitmap* bitmap=new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bitmap);
		TDisplayMode mode = KValidBitmapModes[count];
		err = bitmap->Create(size, mode);
		User::LeaveIfError(err);
		User::LeaveIfError(bitmapArray.Append(bitmap));
		CleanupStack::Pop(bitmap);
		User::LeaveIfError(handles.Append(bitmap->Handle()));		
		}
	
	// Duplicate each bitmap in the array and measure performance
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
		CFbsBitmap* duplicateBitmap=new (ELeave) CFbsBitmap;
		CleanupStack::PushL(duplicateBitmap);
		TInt hn=count%KNumValidBitmapModes;
		TInt handle=handles[hn];
		err=duplicateBitmap->Duplicate(handle);
		User::LeaveIfError(err);
		CleanupStack::PopAndDestroy(duplicateBitmap);
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(_L("Bitmap-Duplicate"), 0, 0, 0, KIterationsToTest);	
	
	handles.Reset();
	
	bitmapArray.ResetAndDestroy();	
	CleanupStack::PopAndDestroy(&bitmapArray);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0087

@SYMTestCaseDesc
The test determines how long it takes to create and destroy small simple bitmap objects.

@SYMTestActions
Compare the results over time, and before and after changes to bitmap construction and destruction code.

@SYMTestExpectedResults
Test should pass and display total test time and time per bitmap

@param aDisplayMode The display mode to create bitmaps with.
@param aTestDescription The description of the test.
*/
void CTFbsBitmapHandlePerf::SmallBitmapCreationSimpleL()
	{
	BitmapCreationSimpleL(32, 32, KTestDisplayMode2, _L("Small Bitmap-Create-64K-Simple"));
	}
	
/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0088

@SYMTestCaseDesc
The test determines how long it takes to create and destroy large simple bitmap objects.

@SYMTestActions
Compare the results over time, and before and after changes to bitmap construction and destruction code.

@SYMTestExpectedResults
Test should pass and display total test time and time per bitmap

@param aDisplayMode The display mode to create bitmaps with.
@param aTestDescription The description of the test.
*/
void CTFbsBitmapHandlePerf::LargeBitmapCreationSimpleL()
	{
	BitmapCreationSimpleL(500, 500, KTestDisplayMode2, _L("Large Bitmap-Create-64K-Simple"));
	}
