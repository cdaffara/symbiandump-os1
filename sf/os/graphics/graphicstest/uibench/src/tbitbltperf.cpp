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
 
#include "tbitbltperf.h"
#include <bitdrawinterfaceid.h>
#include <bmalphablend.h>
#include <bitdraw.h>

_LIT(KBitmap12bit, "z:\\system\\data\\uibench_12bit.mbm");
_LIT(KBitmap16bit, "z:\\system\\data\\uibench_16bit.mbm");
_LIT(KBitmap24bit, "z:\\system\\data\\uibench_24bit.mbm");
_LIT(KBitmap32bit, "z:\\system\\data\\uibench_32bit.mbm");
_LIT(KBitmapTile, "z:\\system\\data\\uibench_tile.mbm");

const TInt KIterationsToTest = 51; 		// Number of iterations to run tests

CTBitBltPerf::~CTBitBltPerf()
	{
	delete iBitmap12bit;
	delete iBitmap16bit;
	delete iBitmap24bit;
	delete iBitmap32bit;
	delete iTile;
	delete iTile16bit;
	delete iTile32bit;				
	delete iAlpha8bit;
	}

CTBitBltPerf::CTBitBltPerf()
	{
	SetTestStepName(KTBitBltPerfName);
	}

/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CTBitBltPerf::doTestStepPreambleL()
	{	
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();

	iBitmap12bit = LoadBitmapL(KBitmap12bit, 0);
	iBitmap16bit = LoadBitmapL(KBitmap16bit, 0);
	iBitmap24bit = LoadBitmapL(KBitmap24bit, 0);
	iBitmap32bit = LoadBitmapL(KBitmap32bit, 0);
	iTile = LoadBitmapL(KBitmapTile, 0);
	
	iTile16bit = CopyIntoNewBitmapL(iTile, EColor64K);
	iTile32bit = CopyIntoNewBitmapL(iTile, EColor16M);
	iAlpha8bit = CopyIntoNewBitmapL(iBitmap12bit, EGray256);
		
	return TestStepResult();
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTBitBltPerf::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0005"));
	MaskedBitBltAlphaL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0004"));
	MaskedBitBltFlickerL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0003"));
	MaskedBitBltSameL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0001"));
	SimpleBitBltL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0002"));
	MaskedBitBltL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0056"));
	WriteAlphaLineExTestL();
	RecordTestResultL();

	return TestStepResult();
	}

/**
Helper function to test bitblt bitmaps for different types of blitting. The bitmap is tiled
across the target, avoiding any overlapping or clipping. 

@param aBitBltTest the bitblt test to carry out
@param aBitmapToBlt the bitmap to bitblt
@param aBitmapMask the bitmap mask
@param aCrop rectangle to crop bitblt to
@param aOrientation the orientation of the display
@param aTestName the name of the test
@param aGc the graphics context to bitblt to
*/
void CTBitBltPerf::BitBltBitmapTestL(TBitBltTest aBitBltTest, CFbsBitmap* aBitmapToBlt, CFbsBitmap* aBitmapMask, TRect aCropTo, TInt aOrientation, const TDesC& aTestName, CFbsBitGc* aGc)
	{
	aGc->Clear();
	
	const TSize targetSize = iScreenDevice->BitmapDevice().SizeInPixels();	

	// Calculate the number of tiles that will fit fully on the pixmap
	const TInt tileWidth = aCropTo.Width();
	const TInt tileHeight = aCropTo.Height();
	const TInt tileColumns = targetSize.iWidth / tileWidth;
	const TInt tileRows = targetSize.iHeight / tileHeight;
	const TInt numTiles = tileColumns * tileRows;
	const TInt numPixelsPerIteration = numTiles * tileWidth * tileHeight;

	iProfiler->InitResults();
			
	switch (aBitBltTest)
	{
	case EBitBlt:
		{
		for(TInt count=KIterationsToTest; count>0; --count)
			{
			TPoint bitmapPosition = TPoint(0,0);
			for (TInt tile = numTiles - 1; tile >= 0; --tile)
				{
				aGc->BitBlt(bitmapPosition, aBitmapToBlt, aCropTo);
				bitmapPosition.iX += tileWidth;
				if (bitmapPosition.iX + tileWidth > targetSize.iWidth)
					{
					bitmapPosition.iX = 0;
					bitmapPosition.iY += tileHeight;
					}
				}
			}
		}
		break;
	case EBitBltMasked:
		{
		for(TInt count=KIterationsToTest; count>0; --count)
			{
			TPoint bitmapPosition = TPoint(0,0);
			for (TInt tile = numTiles - 1; tile >= 0; --tile)
				{
				aGc->BitBltMasked(bitmapPosition, aBitmapToBlt, aCropTo, aBitmapMask, EFalse);
				bitmapPosition.iX += tileWidth;
				if (bitmapPosition.iX + tileWidth > targetSize.iWidth)
					{
					bitmapPosition.iX = 0;
					bitmapPosition.iY += tileHeight;
					}
				}
			}
		}
		break;
	}

	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisPixelRate(aTestName, aOrientation, aBitmapToBlt->DisplayMode(), iScreenDevice->BitmapDevice().DisplayMode(), KIterationsToTest, numPixelsPerIteration);	
	}


/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0001

@SYMTestCaseDesc
Tests how long it takes to bitblt a bitmap with different screen modes

@SYMTestActions
Compare the results over time, and before and after changes to bitblt code.

@SYMTestExpectedResults
Test should pass and display total test time and time per bitmap
*/
void CTBitBltPerf::SimpleBitBltL()
	{
	INFO_PRINTF1(_L("CTBitBltPerf::SimpleBitBlt"));
	
	_LIT(KTestName, "BitBlt");
	TBool orientation[4];
	TRect cropTo(0,0,200,200);
	
	// Bitblt for each display mode
	for(TInt dispModeIndex = 0; dispModeIndex < KNumValidDisplayModes; dispModeIndex++)
		{
		SetScreenModeL(KValidDisplayModes[dispModeIndex]);			
		iGc->OrientationsAvailable(orientation);
		for (TInt orient = CFbsBitGc::EGraphicsOrientationNormal; orient <= CFbsBitGc::EGraphicsOrientationRotated270; orient++)
			{
			if (orientation[orient])
				{
				iGc->SetOrientation((CFbsBitGc::TGraphicsOrientation)orient);
				
				BitBltBitmapTestL(EBitBlt, iBitmap12bit, NULL, cropTo, orient, KTestName, iGc);
				BitBltBitmapTestL(EBitBlt, iBitmap16bit, NULL, cropTo, orient, KTestName, iGc);
				BitBltBitmapTestL(EBitBlt, iBitmap24bit, NULL, cropTo, orient, KTestName, iGc);
				BitBltBitmapTestL(EBitBlt, iBitmap32bit, NULL, cropTo, orient, KTestName, iGc);
				}
			}		
		}
	iGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0002

@SYMTestCaseDesc
Tests how long it takes to bitblt a masked bitmap with different screen modes

@SYMTestActions
Compare the results over time, and before and after changes to bitblt code.

@SYMTestExpectedResults
Test should pass and display total test time and time per bitmap
*/
void CTBitBltPerf::MaskedBitBltL()
	{
	INFO_PRINTF1(_L("CTBitBltPerf::MaskedBitBlt"));
	
	_LIT(KTestName, "MBitBlt");
	TBool orientation[4];
	TRect cropTo(0,0,200,200);
	
	// test for each display mode
	for(TInt dispModeIndex = 0; dispModeIndex < KNumValidDisplayModes; dispModeIndex++)
		{
		SetScreenModeL(KValidDisplayModes[dispModeIndex]);			
		iGc->OrientationsAvailable(orientation);
		for (TInt orient = CFbsBitGc::EGraphicsOrientationNormal; orient <= CFbsBitGc::EGraphicsOrientationRotated270; orient++)
			{
			if (orientation[orient])
				{
				iGc->SetOrientation((CFbsBitGc::TGraphicsOrientation)orient);
				
				BitBltBitmapTestL(EBitBltMasked, iBitmap12bit, iTile, cropTo, orient, KTestName, iGc);
				BitBltBitmapTestL(EBitBltMasked, iBitmap16bit, iTile, cropTo, orient, KTestName, iGc);
				BitBltBitmapTestL(EBitBltMasked, iBitmap24bit, iTile, cropTo, orient, KTestName, iGc);
				BitBltBitmapTestL(EBitBltMasked, iBitmap32bit, iTile, cropTo, orient, KTestName, iGc);
				}
			}		
		}
	iGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);		
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0003

@SYMTestCaseDesc
Tests how long it takes to bitblt the same bitmap as a mask with different screen modes

@SYMTestActions
Compare the results over time, and before and after changes to bitblt code.

@SYMTestExpectedResults
Test should pass and display total test time and time per bitmap
*/	
void CTBitBltPerf::MaskedBitBltSameL()
	{
	INFO_PRINTF1(_L("CTBitBltPerf::MaskedBitBltSame"));
	
	_LIT(KTestName, "MBitBltS");
	TBool orientation[4];
	TRect cropTo(0,0,200,200);
	
	// test for each display mode
	for(TInt dispModeIndex = 0; dispModeIndex < KNumValidDisplayModes; dispModeIndex++)
		{		
		SetScreenModeL(KValidDisplayModes[dispModeIndex]);			
		iGc->OrientationsAvailable(orientation);
		for (TInt orient = CFbsBitGc::EGraphicsOrientationNormal; orient <= CFbsBitGc::EGraphicsOrientationRotated270; orient++)
			{
			if (orientation[orient])
				{
				iGc->SetOrientation((CFbsBitGc::TGraphicsOrientation)orient);
				
				BitBltBitmapTestL(EBitBltMasked, iTile, iTile, cropTo, orient, KTestName, iGc);
				BitBltBitmapTestL(EBitBltMasked, iTile16bit, iTile16bit, cropTo, orient, KTestName, iGc);
				BitBltBitmapTestL(EBitBltMasked, iTile32bit, iTile32bit, cropTo, orient, KTestName, iGc);					
				}
			}		
		}
	iGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);		
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0004

@SYMTestCaseDesc
Tests how long it takes to bitblt a masked bitmap to another bitmap

@SYMTestActions
Compare the results over time, and before and after changes to bitblt code.

@SYMTestExpectedResults
Test should pass and display total test time and time per bitmap
*/
void CTBitBltPerf::MaskedBitBltFlickerL()
	{
	INFO_PRINTF1(_L("CTBitBltPerf::MaskedBitBltFlickerL"));
	
	_LIT(KTestName, "MBitBltF");
	TBool orientation[4];
	TRect cropTo(0,0,200,200);
	
	// Test for each display mode
	for(TInt dispModeIndex = 0; dispModeIndex < KNumValidDisplayModes; dispModeIndex++)
		{
		SetScreenModeL(KValidDisplayModes[dispModeIndex]);
		CFbsBitmap* flickerBitmap=new(ELeave) CFbsBitmap;
		CleanupStack::PushL(flickerBitmap);
		TInt ret=flickerBitmap->Create(iTile->SizeInPixels(), KValidDisplayModes[dispModeIndex]);
		User::LeaveIfError(ret);
		
		iBitmapDevice = CFbsBitmapDevice::NewL(flickerBitmap);
		CFbsBitGc* gc;
		ret = iBitmapDevice->CreateContext(gc);
		User::LeaveIfError(ret);
		CleanupStack::PushL(gc);
		
		// Why is this gc not used? Should be used instead of iGc?
		gc->OrientationsAvailable(orientation);
		for (TInt orient = CFbsBitGc::EGraphicsOrientationNormal; orient <= CFbsBitGc::EGraphicsOrientationRotated270; orient++)
			{
			if (orientation[orient])
				{
				gc->SetOrientation((CFbsBitGc::TGraphicsOrientation)orient);

				BitBltBitmapTestL(EBitBltMasked, iBitmap12bit, iTile, cropTo, orient, KTestName, gc);
				BitBltBitmapTestL(EBitBltMasked, iBitmap16bit, iTile, cropTo, orient, KTestName, gc);
				BitBltBitmapTestL(EBitBltMasked, iBitmap24bit, iTile, cropTo, orient, KTestName, gc);
				BitBltBitmapTestL(EBitBltMasked, iBitmap32bit, iTile, cropTo, orient, KTestName, gc);		
				}			
			}
		gc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
		
		delete iBitmapDevice;
		iBitmapDevice = NULL;		
		CleanupStack::PopAndDestroy(2, flickerBitmap);
		}
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0005

@SYMTestCaseDesc
Tests how long it takes to bitblt a masked alpha bitmap with different screen modes

@SYMTestActions
Compare the results over time, and before and after changes to bitblt code.

@SYMTestExpectedResults
Test should pass and display total test time and time per bitmap
*/
void CTBitBltPerf::MaskedBitBltAlphaL()
	{
	INFO_PRINTF1(_L("CTBitBltPerf::MaskedBitBltAlpha"));	
	
	_LIT(KTestName, "MBitBltA");
	TBool orientation[4];
	TRect cropTo(0,0,200,200);
	
	// test for each display mode
	for(TInt dispModeIndex = 0; dispModeIndex < KNumValidDisplayModes; dispModeIndex++)
		{
		SetScreenModeL(KValidDisplayModes[dispModeIndex]);		
		iGc->OrientationsAvailable(orientation);
		for (TInt orient = CFbsBitGc::EGraphicsOrientationNormal; orient <= CFbsBitGc::EGraphicsOrientationRotated270; orient++)
			{
			if (orientation[orient])
				{
				iGc->SetOrientation((CFbsBitGc::TGraphicsOrientation)orient);
		
				BitBltBitmapTestL(EBitBltMasked, iBitmap12bit, iAlpha8bit, cropTo, orient, KTestName, iGc);
				BitBltBitmapTestL(EBitBltMasked, iBitmap16bit, iAlpha8bit, cropTo, orient, KTestName, iGc);
				BitBltBitmapTestL(EBitBltMasked, iBitmap24bit, iAlpha8bit, cropTo, orient, KTestName, iGc);
				BitBltBitmapTestL(EBitBltMasked, iBitmap32bit, iAlpha8bit, cropTo, orient, KTestName, iGc);		
				}
			}		
		}
	iGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0056

@SYMDEF PDEF096994

@SYMTestCaseDesc
Tests the performace of CopyPixel and CopyTwoPixels called from WriteAlphaLineEx 

@SYMTestActions
Compare the results over time, and before and after changes to CopyPixel and CopyTwoPixels code.

@SYMTestExpectedResults
Test should pass and display total test time and average time
*/

void CTBitBltPerf::WriteAlphaLineExTestL()
	{
	_LIT(KTestName, "WriteAlphaLineEx");

	TDisplayMode displayMode = EColor16MU;
	TDisplayMode sourceFormat = EColor64K;
	TInt err;
	FOREVER
		{
		TRAP(err,SetDrawDeviceModeL(displayMode));
		if (err==KErrNone)
			break;
		_LIT(KLog,"Failed to create draw device of mode %S, error %d");
		INFO_PRINTF3(KLog,&ColorModeName(displayMode),err);
		if (displayMode==EColor16MA)
			User::Leave(err);
		displayMode=EColor16MA;
		}
	_LIT(KLog,"Created draw device of mode %S");
	INFO_PRINTF2(KLog,&ColorModeName(displayMode));

	TAny* interface = NULL;
	err = GetDrawDeviceInterfaceL(KFastBlitInterfaceID, interface);
	if (err!=KErrNone)
		{
		_LIT(KLog,"No WriteAlphaLineExTest performed as getting draw device interface returned error %d");
		INFO_PRINTF2(KLog,err);
		}
	else
		{
		ClearDrawDeviceL(KRgbWhite);

		TSize size = TSize(50,50);
		TRect rect = TRect(size);
		TUint16* writeBuffer = new (ELeave) TUint16[size.iWidth];
		CleanupStack::PushL(writeBuffer);
		TUint8* maskBuffer =  new (ELeave) TUint8[size.iWidth];
		CleanupStack::PushL(maskBuffer);
		TInt nOffset = sizeof(TUint16) * size.iWidth;

		Mem::Fill(writeBuffer,nOffset,0xff);
		Mem::Fill(maskBuffer,size.iWidth/2,0x8e);
		Mem::Fill((maskBuffer+size.iWidth/2),size.iWidth/2,0xff);

		MFastBlit* fastBlit = reinterpret_cast<MFastBlit*>(interface);

		iProfiler->InitResults();
		for(TInt count=0;count<KIterationsToTest;count++)
			{
			for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
				{
				fastBlit->WriteAlphaLineEx(rect.iTl.iX,yy,rect.Width(),0,(TUint32*)writeBuffer,sourceFormat,0,(TUint32*)maskBuffer,MAlphaBlend::EShdwBefore);
				}
			iProfiler->MarkResultSetL();
			}
		
		CleanupStack::PopAndDestroy(2, writeBuffer);//maskBuffer, writeBuffer
		iProfiler->ResultsAnalysis(KTestName, CFbsBitGc::EGraphicsOrientationNormal, sourceFormat, displayMode, KIterationsToTest);	
		}
	}


