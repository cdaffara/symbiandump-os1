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

#include "tbitbltbase.h"


CTBitBltBase::CTBitBltBase()
	{
	
	}

CTBitBltBase::~CTBitBltBase()
	{
	}

/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTBitBltBase::doTestStepL()
	{
	// Test for each target pixel format
	for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
		TBuf<KPixelFormatNameLength> targetPixelFormatName(TDisplayModeMapping::ConvertPixelFormatToPixelFormatString(iTestParams.iTargetPixelFormat));

		// Test for each source pixel format
		for(TInt sourcePixelFormatIndex = iSourcePixelFormatArray.Count() - 1; sourcePixelFormatIndex >= 0; sourcePixelFormatIndex--)
			{
			TBool isEColor256 = EFalse;
			iTestParams.iSourcePixelFormat = iSourcePixelFormatArray[sourcePixelFormatIndex];
			if(EUidPixelFormatP_8 == iTestParams.iSourcePixelFormat)
				isEColor256 = ETrue;
			TBuf<KPixelFormatNameLength> sourcePixelFormatName(TDisplayModeMapping::ConvertPixelFormatToPixelFormatString(iTestParams.iSourcePixelFormat));

			INFO_PRINTF3(_L("Target Pixel Format: %S;  Source Pixel Format: %S"), &targetPixelFormatName, &sourcePixelFormatName);

			// small target
			SetTargetL(iTestParams.iTargetPixelFormat, EOneContextOneTarget, KTarget1Size);
			iLargeTarget = EFalse;
			
			// create source bitmaps for tests
			CreateBitmapsL(iTestParams.iSourcePixelFormat);
			RunTestsL();
			// only run OOM tests for one target pixel format to prevent duplication of tests
			if ((targetPixelFormatIndex == 0) && isEColor256)
				{
				RunOomTestsL();  //from base class
				}
			// delete source bitmaps
			DeleteBitmaps();
						
			// large target
			SetTargetL(iTestParams.iTargetPixelFormat, EOneContextOneTarget, KTarget2Size);
			iLargeTarget = ETrue;
			
			// create source bitmaps for tests
			CreateBitmapsL(iTestParams.iSourcePixelFormat);
			RunTestsL();
			if ((targetPixelFormatIndex == 0) && isEColor256)
				{
				RunOomTestsL();  //from base class
				}
			// delete source bitmaps
			DeleteBitmaps();
			}
		}
	CloseTMSGraphicsStep();

	return TestStepResult();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTBitBltBase::doTestStepPreambleL()
	{
	CTDirectGdiStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTBitBltBase::doTestStepPostambleL()
	{
	DeleteBitmaps();
	CTDirectGdiStepBase::doTestStepPostambleL();
	return TestStepResult();
	}

/**
Write target output.
Call method from base class with test case name and source pixel format string.
@param aTestCaseName Name of test case.
*/
void CTBitBltBase::WriteTargetOutput(TPtrC aTestCaseName)
	{	
	TBuf<KFileNameLength> postfix;
	postfix.Append(KSourceString);
	postfix.Append(KSeparator);
	postfix.Append(TDisplayModeMapping::ConvertPixelFormatToShortPixelFormatString(iTestParams.iSourcePixelFormat));
	
	TESTNOERROR(CTDirectGdiStepBase::WriteTargetOutput(iTestParams, aTestCaseName, &postfix));
	}

/**
Create set of bitmaps needed for tests.
@param aPixelFormat Source pixel format of bitmap.
*/
void CTBitBltBase::CreateBitmapsL(TUidPixelFormat aPixelFormat)
	{
	iNotInitialisedBitmap = new (ELeave)CFbsBitmap();
	iZeroSizeBitmap	 = new (ELeave)CFbsBitmap();
	TESTNOERRORL(iZeroSizeBitmap->Create(TSize(0,0),TDisplayModeMapping::MapPixelFormatToDisplayMode(aPixelFormat)));
	iCompressedBitmap = CreateCheckedBoardBitmapL(aPixelFormat, KBitmap2Size, TSize(8, 8));
	iCompressedBitmap->Compress();
	iCompressedBitmapSmall = CreateCheckedBoardBitmapL(aPixelFormat, KBitmap1Size, TSize(2,2));
	iCompressedBitmapSmall->Compress();
	iCheckedBoardBitmap1 = CreateCheckedBoardBitmapL(aPixelFormat, KBitmap1Size, KBitmap1Size);
	iCheckedBoardBitmap2 = CreateCheckedBoardBitmapL(aPixelFormat, KBitmap2Size, TSize(8, 8));
	iConcentricRectsBitmap1 = CreateConcentricRectsBitmapL(aPixelFormat, KBitmap1Size);
	iConcentricRectsBitmap2 = CreateConcentricRectsBitmapL(aPixelFormat, KBitmap2Size);
	iCheckedBoardWithAlphaBitmap = CreateCheckedBoardBitmapL(aPixelFormat, KBitmap2Size, TSize(16, 16), ETrue);
	}

/**
Delete set of test bitmaps.
*/
void CTBitBltBase::DeleteBitmaps()
	{	
	delete iNotInitialisedBitmap;
	iNotInitialisedBitmap = NULL;
	delete iZeroSizeBitmap;
	iZeroSizeBitmap = NULL;
	delete iCompressedBitmap;
	iCompressedBitmap = NULL;
	delete iCompressedBitmapSmall;
	iCompressedBitmapSmall = NULL;
	delete iCheckedBoardBitmap1;
	iCheckedBoardBitmap1 = NULL;
	delete iCheckedBoardBitmap2;
	iCheckedBoardBitmap2 = NULL;
	delete iConcentricRectsBitmap1;
	iConcentricRectsBitmap1 = NULL;
	delete iConcentricRectsBitmap2;
	iConcentricRectsBitmap2 = NULL;
	delete iCheckedBoardWithAlphaBitmap;
	iCheckedBoardWithAlphaBitmap = NULL;
	}

/**
Common positioning test function for BitBlt() and DrawBitmap() tests.
The test iterates positions over whole target, outside target and on the target boundaries
and call tested function for those positions.
@param aTestName Name of test case.
@param aFunc Tested function. EBitBlt and EDrawBitmap are supported.
*/
void CTBitBltBase::TestPositioningBaseL(const TDesC& aTestName, TBitBltFuncType aFunc)
	{	
	CFbsBitmap* bitmap;
	if(iTestParams.iDoCompressed)
		{
		bitmap = iCompressedBitmap;
		}
	else
		{
		bitmap = iConcentricRectsBitmap1;
		}
	
	TInt width = iGdiTarget->SizeInPixels().iWidth;
	TInt height = iGdiTarget->SizeInPixels().iHeight;
	TInt bmpWidth = bitmap->SizeInPixels().iWidth;
	TInt bmpHeight = bitmap->SizeInPixels().iHeight;
	TSize bmpSize(bmpWidth, bmpHeight);

	// test two versions of function
	for(TInt i = 0; i < 2; i++)
		{
		ResetGc();

		TPositionIterator posIterator(-30, width+30, bmpWidth, -30, height+30, bmpHeight);
		posIterator.Begin();

		do
			{
			//It is done to shift the rectangle drawn. It gives a slope effect in the image.
			TPoint pos(posIterator.iPosX+posIterator.iIndexY, posIterator.iPosY+posIterator.iIndexX);
			if(aFunc == EBitBlt)
				{
				if(i == 0)
					{
					iGc->BitBlt(pos, *bitmap);
					}
				else
					{
					// additionally source rect size is iterated
					iGc->BitBlt(pos, *bitmap, TRect(TPoint(0, 0), TSize(posIterator.iIndexX, posIterator.iIndexY)));
					}
				}
			else // (aFunc == EDrawBitmap)
				{
				if(i == 0)
					{
					iGc->DrawBitmap(TRect(pos, bmpSize), *bitmap);
					}
				else
					{
					// additionally source rect size is iterated
					iGc->DrawBitmap(TRect(pos, bmpSize),*bitmap, TRect(TPoint(0, 0), TSize(posIterator.iIndexX, posIterator.iIndexY)));
					}
				}
			}
		while(posIterator.Next());

		TESTNOERRORL(iGc->GetError());

		if(!iTestParams.iDoCompressed)
			{
			// add 1 or 2 to test case name to identity function version
			TBuf<30> testName;
			testName.Append(aTestName);
			testName.AppendNum(i+1);
			WriteTargetOutput(testName);
			}
		}
	}

/**
Common invalid parameters test function for BitBlt() and DrawBitmap() tests.
The function tests against invalid source rectangle, zero size source bitmap
(both cases should return KErrArgument) and not initialised source bitmap (should
return KErrBadHandle).
@param aTestName Name of test case.
@param aFunc Tested function. EBitBlt and EDrawBitmap are supported.
*/
void CTBitBltBase::TestInvalidParametersBaseL(const TDesC& aTestName, TBitBltFuncType aFunc)
	{		
	ResetGc();

	CFbsBitmap* bitmap = iConcentricRectsBitmap2;

	TInt bmpWidth = bitmap->SizeInPixels().iWidth;
	TInt bmpHeight = bitmap->SizeInPixels().iHeight;

	// invalid source rectangle
	RArray<TRect> rectArray;
	CleanupClosePushL(rectArray);
	TInt err = KErrNone;
	err |= rectArray.Append(TRect(-30, -30, -10, -10));
	err |= rectArray.Append(TRect(bmpWidth+10, bmpHeight+10, bmpWidth+20, bmpHeight+20));
	err |= rectArray.Append(TRect(bmpWidth, bmpHeight, 0, 0));
	err |= rectArray.Append(TRect(-10, -10, -30, -30));
	err |= rectArray.Append(TRect(0, 0, 0, 0));
	TESTL(KErrNone == err);
	
	for(TInt i = 0; i < rectArray.Count(); i++)
		{
		if(aFunc == EBitBlt)
			{
			iGc->BitBlt(TPoint(i*20, 0), *bitmap, rectArray[i]);
			}
		else // (aFunc == EDrawBitmap)
			{
			iGc->DrawBitmap(TRect(TPoint(i*20, 0), TSize(50, 50)), *bitmap, rectArray[i]);
			}
		}

	TESTNOERRORL(iGc->GetError());

	CleanupStack::PopAndDestroy(&rectArray);

	if(aFunc == EBitBlt)
		{
		// invalid source bitmap, zero size bitmap
		iGc->BitBlt(TPoint(0, 20), *iZeroSizeBitmap);
		CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
		
		iGc->BitBlt(TPoint(20, 20), *iZeroSizeBitmap, TRect(TPoint(0, 0), TSize(0, 0)));
		CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);

		// invalid source bitmap, not initialised bitmap
		iGc->BitBlt(TPoint(0, 40), *iNotInitialisedBitmap);
		CheckErrorsL(KErrBadHandle, KErrNone, (TText8*)__FILE__, __LINE__);
		
		iGc->BitBlt(TPoint(20, 40), *iNotInitialisedBitmap, TRect(TPoint(0, 0), TSize(0, 0)));
		CheckErrorsL(KErrBadHandle, KErrNone, (TText8*)__FILE__, __LINE__);
		}
	else // (aFunc == EDrawBitmap)
		{
		// invalid source bitmap, zero size bitmap
		iGc->DrawBitmap(TRect(TPoint(0, 20), TSize(50, 50)), *iZeroSizeBitmap);
		CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
		
		iGc->DrawBitmap(TRect(TPoint(20, 20), TSize(50, 50)), *iZeroSizeBitmap,	TRect(TPoint(0, 0), TSize(10, 10)));
		CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);

		// invalid source bitmap, not initialised bitmap
		iGc->DrawBitmap(TRect(TPoint(0, 40), TSize(50, 50)), *iNotInitialisedBitmap);
		CheckErrorsL(KErrBadHandle, KErrNone, (TText8*)__FILE__, __LINE__);
		
		iGc->DrawBitmap(TRect(TPoint(20, 40), TSize(50, 50)), *iNotInitialisedBitmap,
				TRect(TPoint(0, 0), TSize(10, 10)));
		CheckErrorsL(KErrBadHandle, KErrNone, (TText8*)__FILE__, __LINE__);
		}

	// test if target is still clear
	TBool res = TestTargetL(KRgbWhite); 
	TEST(res);
	// output the bitmap if there was an error to assist with debugging
	if (res == EFalse)
		{			
		WriteTargetOutput(aTestName);
		}
	}

/**
Common source bitmap cloning test function for BitBlt() and DrawBitmap() tests.
@param aTestName Name of test case.
@param aFunc Tested function. EBitBlt and EDrawBitmap are supported.
*/
void CTBitBltBase::TestSourceBitmapCloningBaseL(const TDesC& aTestName, TBitBltFuncType aFunc)
	{
	ResetGc();

	TSize bmpSize(KBitmap2Size);

	for(TInt i = 0; i < 5; i++)
		{
		CFbsBitmap* bitmap = CreateConcentricRectsBitmapL(iTestParams.iSourcePixelFormat, bmpSize);
		if(iTestParams.iDoCompressed)
			bitmap->Compress();
			
		if(aFunc == EBitBlt)
			{
			iGc->BitBlt(TPoint(55-35+i*10+1, -60+80+i*7+i), *bitmap);
			}
		else // (aFunc == EDrawBitmap)
			{
			iGc->DrawBitmap(TRect(TPoint(55-35+i*10+1, -60+80+i*7+i), bmpSize), *bitmap);
			}

		delete bitmap;
		bitmap = NULL;

		bmpSize -= TSize(10, 15);
		}
	
	TESTNOERRORL(iGc->GetError());
	if(!iTestParams.iDoCompressed)
		WriteTargetOutput(aTestName);
	}
