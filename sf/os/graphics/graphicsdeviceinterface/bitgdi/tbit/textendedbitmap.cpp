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

const TDisplayMode KDisplayMode[] = {EColor16MAP, EColor16MA, EColor16MU, EColor16M, EColor64K, 
						   EColor4K, EColor256, EColor16, EGray256, EGray16, EGray4, EGray2};
// The only display modes supported by the example rasterizer are EGray256, EColor64K,
// EColor16MU and EColor16MAP, these tests will fail if KBitmapMode is changed to a
// display mode that is not one of these modes.
const TDisplayMode KBitmapMode = EColor16MAP;
const TSize KBitmapSize(640,240);

const TInt KNumColors = 3;
const TRgb KColors[KNumColors] = {TRgb(0,255,255), TRgb(255,0,255), TRgb(255,255,0)};
const TUid KUidExampleExtendedBitmap = {0x10285A78};

//----------------------------------------------------------------------------
// Positive Extended Bitmap Tests
//----------------------------------------------------------------------------
CTExtendedBitmap::CTExtendedBitmap(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	}

void CTExtendedBitmap::ConstructL()
	{
#ifdef USE_SCREEN_DEVICE
	// Create a screen device
	TInt ii = 0;
	TInt err = KErrNotSupported;
	for(;(ii<TInt(sizeof(KDisplayMode)/sizeof(KDisplayMode[0]))) && (err == KErrNotSupported);++ii)
		{
		TRAP(err, iDevice = CFbsScreenDevice::NewL(_L("scdv"),KDisplayMode[ii]));
		}
	if (err != KErrNone)
		{
		_LIT(KLog,"Failed to create screen device %S return value %d");
		INFO_PRINTF3(KLog,&ColorModeName(KDisplayMode[ii]),err);
		User::Leave(err);
		}
	else
		{
		_LIT(KLog,"Created Screen Device with mode %S");
		INFO_PRINTF2(KLog,&ColorModeName(KDisplayMode[ii]));
		}
	static_cast<CFbsScreenDevice*>(iDevice)->SetAutoUpdate(ETrue);
#else
	// Create a standard bitmap, bitmap device
	iBitmap = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Create(KBitmapSize, KBitmapMode));
	iDevice = CFbsBitmapDevice::NewL(iBitmap);
#endif

	INFO_PRINTF2(_L("Testing CFbsBitGc with an extended bitmap, display mode %d"), KBitmapMode);

	// Create a graphics context from chosen device
	User::LeaveIfError(iDevice->CreateContext(iBitGc));
	
	// Check to see if the example rasterizer is available
	CTExtendedBitmapGc::TDrawMode drawMode = CTExtendedBitmapGc::EDrawFlag;
	if (CFbsBitmap::Rasterizer() == NULL)
		drawMode = CTExtendedBitmapGc::EDrawWhite;
	
	switch (drawMode)
		{
		case CTExtendedBitmapGc::EDrawFlag:
			INFO_PRINTF1(_L("Testing WITH the example rasterizer - Rasterizer Available"));
			break;
		case CTExtendedBitmapGc::EDrawWhite:
			INFO_PRINTF1(_L("Testing WITHOUT the example rasterizer - Rasterizer NOT Available"));
			break;
		default:
			ERR_PRINTF1(_L("Unknown draw mode"));
			break;
		}				
	
	
	iExtendedBitmapGcTests = CTExtendedBitmapGc::NewL(this, *iBitGc, drawMode, KBitmapMode);
	}

CTExtendedBitmap::~CTExtendedBitmap()
	{
	((CTExtendedBitmapStep*)iStep)->CloseTMSGraphicsStep();
	delete iExtendedBitmapGcTests;
	delete iBitGc;
	delete iDevice;
#ifndef USE_SCREEN_DEVICE
	delete iBitmap;
#endif
	}

/**
@SYMTestCaseID
	GRAPHICS-BITGDI-0103

@SYMTestCaseDesc
	Test extended bitmaps are drawn correctly to an offscreen bitmap using CFbsBitGc.
	Test that shapes are filled correctly when an extended bitmap is set as the brush pattern 
	using CFbsBitGc::UseBrushPattern().

@SYMPREQ 
	PREQ2096

@SYMREQ
	REQ10847
	REQ10857
	REQ10859

@SYMTestPriority  
	High

@SYMTestStatus 
	Implemented

@SYMTestActions
	Create an extended bitmap with extended data to generate a flag pattern when rendered,
	if the example rasterizer is available.	
	
	Create a matching standard bitmap.

	Draw the extended bitmap on the left of the screen and the standard bitmap on the right, 
	using each of the following CFbsBitGc APIs.
	This is done with both vertical and horizonatal flag orientations,
	and with and without an active region of interest:	 	
		- BitBlt() (one version tested as both versions call through to DoBitBlt())
		- BitBltMasked()
		- DrawBitmap() (one version tested as both versions call through to DoDrawBitmap())
		- DrawBitmapMasked()
		- UseBrushPattern()
		- AlphaBlendBitmaps(const TPoint& aDestPt, const CFbsBitmap* aSrcBmp1, const CFbsBitmap* aSrcBmp2, const TRect& aSrcRect1, const TPoint& aSrcPt2, 
				const CFbsBitmap* aAlphaBmp, const TPoint& aAlphaPt);
		- AlphaBlendBitmaps(const TPoint& aDestPt, const CFbsBitmap* aSrcBmp, const TRect& aSrcRect, const CFbsBitmap* aAlphaBmp, const TPoint& aAlphaPt);
		
	To test that shapes are filled correctly using an extended bitmap, an extended bitmap that would generate 
	a flag pattern if the example rasterizer is available, and a matching standard bitmap are in turn set as 
	the brush pattern using CFbsBitGc::UseBrushPattern(). Each of the following APIs are used to draw a shape,
	using the extended bitmap as a brush on the left, and the standard bitmap on the right. Calling each
	of these APIs is necessary as each one contains code specific to extended bitmaps.
		- DrawPie()
		- DrawRoundRect()
		- DrawPolygon() (2 versions)
		- DrawEllipse()
		- DrawRect()
		- DrawText()
		- DrawTextVertical()
		
	Note that the test cases are actually run from a separate test DLL that is built as part of the FbServ
	tests (textendedbitmapgc.dll). This DLL allows the same extended bitmap tests to be run using different graphics
	contexts. 

@SYMTestExpectedResults
	When the rasterizer is available both sides of the test screen should show a horizontally/vertically striped rectangle for every 
	test case, when the rasterizer is not present both sides of the test screen should show a white rectangle for each test case.
	The left and right side of the test should always match exactly for each test case, i.e. what is drawn using an
	extended bitmap on the left of the screen should match what is drawn using a corresponding standard bitmap on the right of
	the screen.
*/
void CTExtendedBitmap::RunTestCaseL(TInt aCurTestCase)
	{
	((CTExtendedBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0103"));
	iExtendedBitmapGcTests->RunTestCaseL(aCurTestCase);
	
	// Compare the two sides of the screen, they should match exactly
	TSize scrSize = iDevice->SizeInPixels();
	TEST(iDevice->RectCompare(TRect(0,0,scrSize.iWidth>>1,scrSize.iHeight), *iDevice, TRect(scrSize.iWidth>>1,0,scrSize.iWidth,scrSize.iHeight)));
	((CTExtendedBitmapStep*)iStep)->RecordTestResultL();
	}

CTestExecuteLogger& CTExtendedBitmap::Logger()
	{
	return CTGraphicsBase::Logger();
	}

void CTExtendedBitmap::TestTrue(TBool aCondition)
	{
	TEST(aCondition);
	}

void CTExtendedBitmap::TestComplete()
	{
	CTGraphicsBase::TestComplete();
	}

__CONSTRUCT_STEP__(ExtendedBitmap)


//----------------------------------------------------------------------------
// Negative Extended Bitmap Tests
//----------------------------------------------------------------------------
CTExtendedBitmapNegative::CTExtendedBitmapNegative(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	}

void CTExtendedBitmapNegative::RunTestCaseL(TInt aCurTestCase)
	{	
	((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{	
	case 1:
		((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0104"));
		TestCFbsBitmapDeviceNewLLeavesL();
		break;
	default:
		((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTExtendedBitmapNegativeStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;	
		}	
	((CTExtendedBitmapNegativeStep*)iStep)->RecordTestResultL();
	}

/**
@SYMTestCaseID
	GRAPHICS-BITGDI-0104

@SYMTestCaseDesc
	Test that passing an extended bitmap to CFbsBitmapDevice::NewL() leaves with error KErrAccessDenied.

@SYMPREQ 
	PREQ2096
	
@SYMREQ
	REQ10847
	REQ10856	

@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions
	Create an extended bitmap using test data and test Uid.
	Call CFbsBitmapDevice::NewL() with the extended bitmap as the parameter.
		
@SYMTestExpectedResults
	CFbsBitmapDevice::NewL() leaves with error KErrAccessDenied.
*/
void CTExtendedBitmapNegative::TestCFbsBitmapDeviceNewLLeavesL()
	{
	INFO_PRINTF1(_L("Test that CFbsBitmapDevice leaves with KErrAccessDenied when created with an extended bitmap"));			
	TInt dataSize = sizeof(KColors)+sizeof(TUint8); // estimate the data size
	TUint8* data = new(ELeave) TUint8[dataSize];
	CleanupStack::PushL(data);	
	
	// Write the colours to be used in the extended bitmap to the data	
	CTExtendedBitmapGc::WriteExtendedBitmapInfoL(data, dataSize, KColors, CTExtendedBitmapGc::EHorizontalStripe);
	
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt err = bmp->CreateExtendedBitmap(TSize(10,10), KBitmapMode, KUidExampleExtendedBitmap, data, dataSize);
	TEST(err == KErrNone);
	
	// CFbsBitmapDevice::NewL() should leave with KErrAccessDenied
	CFbsBitmapDevice* bmpDevice1 = NULL;
	TRAPD(error, bmpDevice1 = CFbsBitmapDevice::NewL(bmp));	
	TEST(error == KErrAccessDenied);	
	
	delete bmpDevice1;
	CleanupStack::PopAndDestroy(2, data);
	}

__CONSTRUCT_STEP__(ExtendedBitmapNegative)
