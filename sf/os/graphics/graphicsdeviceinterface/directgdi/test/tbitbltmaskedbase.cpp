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

#include "tbitbltmaskedbase.h"


CTBitBltMaskedBase::CTBitBltMaskedBase()
	{
	
	}

CTBitBltMaskedBase::~CTBitBltMaskedBase()
	{

	}

/**
Write target output.
Call method from base class with test case name and combined source pixel format and mask type string.
@param aTestCaseName Name of test case.
@return KErrNone if successful, otherwise one of the system-wide error codes.
*/
TInt CTBitBltMaskedBase::WriteTargetOutput(TPtrC aTestCaseName)
	{
	TBuf<KFileNameLength> postfix;	
	postfix.Append(KSourceString);
	postfix.Append(KSeparator);
	postfix.Append(TDisplayModeMapping::ConvertPixelFormatToShortPixelFormatString(
				iTestParams.iSourcePixelFormat));	

	postfix.Append(KSeparator);
	if(iInvertMask)
		{
		postfix.Append(_L16("Invert"));
		}
	postfix.Append(_L16("M"));

	if(iMaskPixelFormat == EUidPixelFormatL_8)
		{
		postfix.Append(_L16("L8"));
		}
	else if(iMaskPixelFormat == EUidPixelFormatL_1)
		{
		postfix.Append(_L16("L1"));
		}
	
	return CTDirectGdiStepBase::WriteTargetOutput(iTestParams, aTestCaseName, &postfix);
	}

/**
Create set of bitmaps needed for tests.
@param aPixelFormat Source pixel format of bitmap.
*/
void CTBitBltMaskedBase::CreateBitmapsL(TUidPixelFormat aPixelFormat)
	{
	CTBitBltBase::CreateBitmapsL(aPixelFormat);
	iNotInitialisedMask = new (ELeave)CFbsBitmap();
	iZeroSizeMask = new (ELeave)CFbsBitmap();
	TESTL(KErrNone == iZeroSizeMask->Create(TSize(0,0), TDisplayModeMapping::MapPixelFormatToDisplayMode(EUidPixelFormatL_1)));
	iMask1L1 = CreateCheckedBoardBitmapL(EUidPixelFormatL_1, KBitmap1Size, TSize(4, 4));
	iMask2L1 = CreateCheckedBoardBitmapL(EUidPixelFormatL_1, KBitmap2Size, TSize(8, 8));
	iMask1L8 = CreateMaskingPixmapL(EUidPixelFormatL_8, KBitmap1Size);
	iMask2L8 = CreateMaskingPixmapL(EUidPixelFormatL_8, KBitmap2Size);
	iCompressedMaskL8 = CreateMaskingPixmapL(EUidPixelFormatL_8, KBitmap2Size);
	iCompressedMaskL8->Compress();
	iBlackWhiteBitmap = CreateBlackWhiteBitmapL(aPixelFormat, KBitmap2Size, TSize(8, 8));
	}

/**
Delete set of test bitmaps.
*/
void CTBitBltMaskedBase::DeleteBitmaps()
	{
	delete iNotInitialisedMask;
	iNotInitialisedMask = NULL;
	delete iZeroSizeMask;
	iZeroSizeMask = NULL;
	delete iMask1L1;
	iMask1L1 = NULL;
	delete iMask2L1;
	iMask2L1 = NULL;
	delete iMask1L8;
	iMask1L8 = NULL;
	delete iMask2L8;
	iMask2L8 = NULL;
	delete iCompressedMaskL8;
	iCompressedMaskL8 = NULL;
	delete iBlackWhiteBitmap;
	iBlackWhiteBitmap = NULL;
	CTBitBltBase::DeleteBitmaps();
	}

/**
Begin iteration through mask types.
All needed variables are initialized to start iteration.
*/
void CTBitBltMaskedBase::BeginMaskIteration()
	{
	iMaskType = EMaskL1;
	iMaskPixelFormat = EUidPixelFormatL_1;
	iInvertMask = EFalse;
	iCurrentMask1 = iMask1L1;
	iCurrentMask2 = iMask2L1;
	}

/**
Next iteration of mask types.
Generates next mask type.
@return EFalse is returned if end of iterations else ETrue.
*/
TBool CTBitBltMaskedBase::NextMaskIteration()
	{
	iMaskType++;
	switch(iMaskType)
		{
		case EMaskL1:
			{
			iMaskPixelFormat = EUidPixelFormatL_1;
			iInvertMask = EFalse;
			iCurrentMask1 = iMask1L1;
			iCurrentMask2 = iMask2L1;
			break;
			}

		case EMaskL1Inv:
			{
			iMaskPixelFormat = EUidPixelFormatL_1;
			iInvertMask = ETrue;
			iCurrentMask1 = iMask1L1;
			iCurrentMask2 = iMask2L1;
			break;
			}

		case EMaskL8:
			{
			iMaskPixelFormat = EUidPixelFormatL_8;
			iInvertMask = EFalse;
			iCurrentMask1 = iMask1L8;
			iCurrentMask2 = iMask2L8;
			break;
			}
			
		case EMaskIterationEnd:
			{
			return EFalse;
			}
		}

	return ETrue;
	}

/**
Common positioning test function for BitBltMasked() and DrawBitmapMasked() tests.
The test iterates positions over whole target, outside target and on the target boundaries
and call tested function for those positions.
@param aTestName Name of test case.
@param aFunc Tested function. EBitBltMasked and EDrawBitmapMasked are supported.
*/
void CTBitBltMaskedBase::TestPositioningBaseL(const TDesC& aTestName, TBitBltFuncType aFunc)
	{
	if (iRunningOomTests)
		{
		// OOM tests take too long if this test is run.  Doesn't test anything new for OOM over other tests.
		return;
		}
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

	ResetGc();

	TPositionIterator posIterator(-30, width+30, bmpWidth, -30, height+30, bmpHeight);
	posIterator.Begin();

	do
		{
		TPoint pos(posIterator.iPosX+posIterator.iIndexY, posIterator.iPosY+posIterator.iIndexX);

		if(aFunc == EBitBltMasked)
			{
			if(iMaskPixelFormat == EUidPixelFormatL_1)
				{
				iGc->BitBltMasked(pos, *bitmap,
					TRect(TPoint(0, 0), bmpSize), *iCurrentMask1, iInvertMask);
				}
			else
				{
				// additionaly mask position is iterated
				iGc->BitBltMasked(pos, *bitmap,
					TRect(TPoint(0, 0), bmpSize), *iCurrentMask1, TPoint(posIterator.iIndexX, posIterator.iIndexY));
				}
			}
		else // (aFunc == EDrawBitmapMasked)
			{
			iGc->DrawBitmapMasked(TRect(pos, bmpSize), *bitmap,
					TRect(TPoint(0, 0), bmpSize), *iCurrentMask1, iInvertMask);
			}
		}
	while(posIterator.Next());

	// WORKAROUND:
	// It looks like AlphaBlendBitmaps() (which is mapped on DrawBitmapMasked() with alpha for BitGDI)
	// do not support clipping and returns KErrArgument for destination rect (partialy) outside the target.
	if(aFunc == EBitBltMasked)
		{
		if(iUseDirectGdi || (iMaskPixelFormat == EUidPixelFormatL_1) || (iGc->GetError() != KErrArgument))
			{
			TESTNOERROR(iGc->GetError());
			}
		}
	else
		{
		TESTNOERROR(iGc->GetError());
		}
	if(!iTestParams.iDoCompressed)
		TESTNOERRORL(WriteTargetOutput(aTestName));
	}

/**
Common invalid parameters test function for BitBltMasked() and DrawBitmapMasked() tests.
The function tests against invalid source rectangle, zero size source bitmap (both should
return KErrArgument) and not initialised source bitmap (should return KErrBadHandle). 
@param aTestName Name of test case.
@param aFunc Tested function. EBitBltMasked and EDrawBitmapMasked are supported.
*/
void CTBitBltMaskedBase::TestInvalidParametersBaseL(const TDesC& aTestName, TBitBltFuncType aFunc)
	{	
	ResetGc();

	CFbsBitmap* bitmap = iConcentricRectsBitmap2;

	TInt bmpWidth = bitmap->SizeInPixels().iWidth;
	TInt bmpHeight = bitmap->SizeInPixels().iHeight;
	TSize bmpSize(bmpWidth, bmpHeight);

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
		if(aFunc == EBitBltMasked)
			{
			iGc->BitBltMasked(TPoint(i*20, 0), *bitmap, rectArray[i], *iMask1L1, EFalse);
			}
		else // (aFunc == EDrawBitmapMasked)
			{
			iGc->DrawBitmapMasked(TRect(TPoint(i*20, 0), bmpSize), *bitmap, rectArray[i], *iMask1L1, EFalse);
			}
		}

	TESTNOERRORL(iGc->GetError());

	CleanupStack::PopAndDestroy(&rectArray);

	TRect zeroRect(TPoint(0, 0), TSize(0, 0));

	if(aFunc == EBitBltMasked)
		{
		// invalid source bitmap, zero size bitmap
		iGc->BitBltMasked(TPoint(0, 20), *iZeroSizeBitmap, zeroRect, *iMask1L1, EFalse);
		CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);
		
		iGc->BitBltMasked(TPoint(20, 20), *iZeroSizeBitmap, zeroRect, *iMask1L8, TPoint(0, 0));
		TESTL(iGc->GetError() == KErrArgument);
		
		iGc->BitBltMasked(TPoint(20, 20), *iZeroSizeBitmap, zeroRect, *iMask1L8, TPoint(0, 0));
		TESTL(iGc->GetError() == KErrArgument);

		// invalid source bitmap, not initialised bitmap
		iGc->BitBltMasked(TPoint(0, 40), *iNotInitialisedBitmap, zeroRect, *iMask1L1, EFalse);
		CheckErrorsL(KErrBadHandle, KErrNone, (TText8*)__FILE__, __LINE__);
		
		iGc->BitBltMasked(TPoint(20, 40), *iNotInitialisedBitmap, zeroRect, *iMask1L8, TPoint(0, 0));
		TESTL(iGc->GetError() == iUseDirectGdi ? KErrBadHandle : KErrArgument);

		TRect bmpRect(TPoint(0, 0), bmpSize);
		
		// BitGDI performs drawing when mask is zero sized or not initialised
		// so we test only in DirectGDI
		if(iUseDirectGdi)
			{
			// invalid mask, zero size mask
			iGc->BitBltMasked(TPoint(0, 60), *bitmap, bmpRect, *iZeroSizeMask, EFalse);
			TESTL(iGc->GetError() == KErrArgument);
		
			iGc->BitBltMasked(TPoint(20, 60), *bitmap, bmpRect, *iZeroSizeMask, TPoint(0, 0));
			TESTL(iGc->GetError() == KErrArgument);
		
			// invalid mask, not initialised mask
			iGc->BitBltMasked(TPoint(0, 80), *bitmap, bmpRect, *iNotInitialisedMask, EFalse);
			TESTL(iGc->GetError() == KErrBadHandle);
		
			iGc->BitBltMasked(TPoint(20, 80), *bitmap, bmpRect, *iNotInitialisedMask, TPoint(0, 0));
			TESTL(iGc->GetError() == KErrBadHandle);
			}
		}
	else // (aFunc == EDrawBitmapMasked)
		{
		// invalid source bitmap, zero size bitmap
		iGc->DrawBitmapMasked(TRect(TPoint(0, 20), TSize(0, 0)), *iZeroSizeBitmap, zeroRect, *iMask1L1, EFalse);
		CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);

		// invalid source bitmap, not initialised bitmap
		iGc->DrawBitmapMasked(TRect(TPoint(0, 40), TSize(0, 0)), *iNotInitialisedBitmap, zeroRect, *iMask1L1, EFalse);
		CheckErrorsL(KErrBadHandle, KErrNone, (TText8*)__FILE__, __LINE__);

		// invalid mask, zero size mask
		TRect bmpRect(TPoint(0, 0), bmpSize);
		iGc->DrawBitmapMasked(TRect(TPoint(0, 60), bmpSize), *bitmap, bmpRect, *iZeroSizeMask, EFalse);
		CheckErrorsL(KErrArgument, KErrNone, (TText8*)__FILE__, __LINE__);

		// invalid mask, not initialised mask
		iGc->DrawBitmapMasked(TRect(TPoint(0, 80), bmpSize), *bitmap, bmpRect, *iNotInitialisedMask, EFalse);
		CheckErrorsL(KErrBadHandle, KErrNone, (TText8*)__FILE__, __LINE__);
		}

	// Test if target is clear. By definition, images rendered using any invalid parameter,
	// should result in no change in the target.
	//This test is not valid for BitGDI as it generates an output if mask size is zero.
	if(iUseDirectGdi)
		{
		TBool res = TestTargetL(KRgbWhite);
		TEST(res);
		// output the bitmap if there was an error to assist with debugging
		if (res == EFalse)
			{				
			TESTNOERRORL(WriteTargetOutput(aTestName));
			}
		}
	
	}

/**
Common source bitmap cloning test function for BitBltMasked() and DrawBitmapMasked() tests.
@param aTestName Name of test case.
@param aFunc Tested function. EBitBltMasked and EDrawBitmapMasked are supported.
*/
void CTBitBltMaskedBase::TestSourceBitmapCloningBaseL(const TDesC& aTestName, TBitBltFuncType aFunc)
	{	
	ResetGc();

	TInt width = iGdiTarget->SizeInPixels().iWidth;
	TInt height = iGdiTarget->SizeInPixels().iHeight;

	TSize bmpSize(KBitmap2Size);

	for(TInt i = 0; i < 5; i++)
		{
		CFbsBitmap* bitmap = CreateConcentricRectsBitmapL(iTestParams.iSourcePixelFormat, bmpSize);
		if(iTestParams.iDoCompressed)
			bitmap->Compress();
			
		if(aFunc == EBitBltMasked)
			{
			iGc->BitBltMasked(TPoint(55-35+i*10+1, -60+80+i*7+1), *bitmap,
				TRect(TPoint(0, 0), bmpSize), *iCurrentMask2, iInvertMask);
			}
		else // (aFunc == EDrawBitmapMasked)
			{
			iGc->DrawBitmapMasked(TRect(TPoint(55-35+i*10+1, -60+80+i*7+1), bmpSize), *bitmap,
					TRect(TPoint(0, 0), bmpSize), *iCurrentMask2, iInvertMask);
			}

		delete bitmap;
		bitmap = NULL;

		bmpSize -= TSize(10, 15);
		}
	
	TESTNOERRORL(iGc->GetError());
	if(!iTestParams.iDoCompressed)
		TESTNOERRORL(WriteTargetOutput(aTestName));
	}

/**
Common mask cloning test function for BitBltMasked() and DrawBitmapMasked() tests.
@param aTestName Name of test case.
@param aFunc Tested function. EBitBltMasked and EDrawBitmapMasked are supported.
*/
void CTBitBltMaskedBase::TestMaskCloningBaseL(const TDesC& aTestName, TBitBltFuncType aFunc)
	{	
	ResetGc();

	TSize maskSize(KBitmap2Size);

	for(TInt i = 0; i < 5; i++)
		{
		CFbsBitmap* mask = CreateCheckedBoardBitmapL(EUidPixelFormatL_1, maskSize, TSize(8, 8));
		if(iTestParams.iDoCompressed)
			mask->Compress();
			
		if(aFunc == EBitBltMasked)
			{
			iGc->BitBltMasked(TPoint(55-35+i*10+1, -60+80+i*7+1), *iConcentricRectsBitmap2,
					TRect(TPoint(0, 0), KBitmap2Size), *mask, iInvertMask);
			}
		else // (aFunc == EDrawBitmapMasked)
			{
			iGc->DrawBitmapMasked(TRect(TPoint(55-35+i*10+1, -60+80+i*7+1), iConcentricRectsBitmap2->SizeInPixels()),
					*iConcentricRectsBitmap2, TRect(TPoint(0, 0), KBitmap2Size), *mask, iInvertMask);
			}

		delete mask;
		mask = NULL;

		maskSize -= TSize(10, 15);
		}
	
	TESTNOERRORL(iGc->GetError());
	
	if(!iTestParams.iDoCompressed)
		TESTNOERRORL(WriteTargetOutput(aTestName));
	}
