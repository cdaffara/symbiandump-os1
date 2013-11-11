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

#include "textendedbitmap.h"
#include <s32mem.h>

const TUid KUidExampleExtendedBitmap = {0x10285A78};

CTExtendedBitmap::CTExtendedBitmap()
	{
	SetTestStepName(KTDirectGdiExtendedBitmapStep);
	}

CTExtendedBitmap::~CTExtendedBitmap()
	{
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-EXTENDED-BITMAP-0001

@SYMPREQ 
	PREQ2096

@SYMREQ	

@SYMTestCaseDesc
	Test all CDirectGdiContext APIs that take CFbsBitmap as a parameter correctly return or set
	an error code when used with an extended bitmap.

@SYMTestPriority  
	High

@SYMTestStatus
	Implemented

@SYMTestActions
	Create a CFbsBitmap using CFbsBitmap::CreateExtendedBitmap().
	Call the following CDirectGdiContext APIs passing the extended bitmap as a parameter:	
		- SetBrushPattern()
		- BitBlt()
		- BitBltMasked() x 2
		- DrawBitmap()
		- DrawBitmapMasked()

@SYMTestExpectedResults
	Each API should return or set the error KErrNotSupported when called with an extended bitmap.
*/ 
void CTExtendedBitmap::TestErrorCodesL()
	{
	TBuf<KTestCaseLength> testCaseName;
	_LIT(KTestName, "ExtendedBitmap-ErrorCodes: source format "); 		
	if(!iRunningOomTests)
		{
		testCaseName.Append(KTestName);
		testCaseName.Append(TDisplayModeMapping::ConvertPixelFormatToPixelFormatString(iTestParams.iSourcePixelFormat));
		INFO_PRINTF1(testCaseName);
		}
	
	ResetGc();
	
	const TSize sizeInPixels(32,32);
	const TRect srcRect(TPoint(0,0),sizeInPixels);
	const TRgb colors[] = {TRgb(0,255,255), TRgb(255,0,255), TRgb(255,255,0)};
	TInt dataSize = sizeof(colors)+sizeof(TUint8); // estimate the size to be written
	TUint8* data = new(ELeave) TUint8[dataSize];
	CleanupStack::PushL(data);
	
	RMemWriteStream ws;
	ws.Open(data, dataSize);
	CleanupClosePushL(ws);
	ws << colors[0] << colors[1] << colors[2] << static_cast<TUint8>(1); // horizontal stripe
	dataSize = ws.Sink()->TellL(MStreamBuf::EWrite).Offset(); // get the actual size written
	CleanupStack::PopAndDestroy(1, &ws);
	
	// Create an extended bitmap
	CFbsBitmap* exBmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(exBmp);
	TInt err = exBmp->CreateExtendedBitmap(sizeInPixels, 
									TDisplayModeMapping::MapPixelFormatToDisplayMode(iTestParams.iSourcePixelFormat), 
									KUidExampleExtendedBitmap, 
									data, 
									dataSize);	
	TESTNOERRORL(err);
	
	// Create a normal bitmap to use when testing cases where the main bitmap being used is a normal
	// bitmap and the mask is an extended bitmap
	CFbsBitmap* nmlBmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(nmlBmp);
	err = nmlBmp->Create(sizeInPixels, TDisplayModeMapping::MapPixelFormatToDisplayMode(iTestParams.iSourcePixelFormat));
	TESTNOERRORL(err);
	
	// Test that each of the CDirectGdiContext APIs that use CFbsBitmap return or set the
	// error KErrNotSupported when used with the extended bitmap
	// SetBrushPattern
	iGc->SetBrushPattern(*exBmp);
	TEST(iGc->GetError() == KErrNotSupported);
		
	// BitBlt
	iGc->BitBlt(TPoint(0,0), *exBmp);
	TEST(iGc->GetError() == KErrNotSupported);
	iGc->BitBlt(TPoint(0,0), *exBmp, srcRect);
	TEST(iGc->GetError() == KErrNotSupported);
	
	// BitBltMasked
	iGc->BitBltMasked(TPoint(0,0), *exBmp, srcRect, *nmlBmp, EFalse);
	TEST(iGc->GetError() == KErrNotSupported);
	iGc->BitBltMasked(TPoint(0,0), *nmlBmp, srcRect, *exBmp, EFalse);
	TEST(iGc->GetError() == KErrNotSupported);
	iGc->BitBltMasked(TPoint(0,0), *exBmp, srcRect, *nmlBmp, TPoint(1,-1));
	TEST(iGc->GetError() == KErrNotSupported);
	iGc->BitBltMasked(TPoint(0,0), *nmlBmp, srcRect, *exBmp, TPoint(1,-1));
	TEST(iGc->GetError() == KErrNotSupported);
	
	// DrawBitmap
	iGc->DrawBitmap(srcRect, *exBmp);
	TEST(iGc->GetError() == KErrNotSupported);
	iGc->DrawBitmap(srcRect, *exBmp, srcRect);
	TEST(iGc->GetError() == KErrNotSupported);
	
	// DrawBitmapMasked	
	iGc->DrawBitmapMasked(srcRect, *exBmp, srcRect, *nmlBmp, EFalse);
	TEST(iGc->GetError() == KErrNotSupported);
	iGc->DrawBitmapMasked(srcRect, *nmlBmp, srcRect, *exBmp, EFalse);
	TEST(iGc->GetError() == KErrNotSupported);
	
	CleanupStack::PopAndDestroy(3, data);
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTExtendedBitmap::doTestStepPreambleL()
	{			
	CTDirectGdiStepBase::doTestStepPreambleL();	
	return TestStepResult();
	}
	
/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTExtendedBitmap::doTestStepL()
	{			
	for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
		SetTargetL(iTestParams.iTargetPixelFormat);
		
		// Test for each pixel format	
		for(TInt sourcePixelFormatIndex = iSourcePixelFormatArray.Count() - 1; sourcePixelFormatIndex >= 0 ; sourcePixelFormatIndex--)
			{		
			iTestParams.iSourcePixelFormat = iSourcePixelFormatArray[sourcePixelFormatIndex];
			
			RunTestsL();
			RunOomTestsL();
			}
		}
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTExtendedBitmap::RunTestsL()
	{	
	// These tests should only be run when using DirectGdi as the error codes
	// we are checking for are only returned by DirectGdi. Extended bitmaps are 
	// supported in BitGdi but not in DirectGdi.	
	if (iUseDirectGdi)
		{
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-EXTENDED-BITMAP-0001"));
		TestErrorCodesL();
		RecordTestResultL();
		}
	}
