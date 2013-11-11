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

#include "tbitbltperf_directgdi.h"
#include <graphics/directgdidriver.h>
#include <graphics/directgdidrawablesource.h>
#include <bitdrawinterfaceid.h>
#include <bmalphablend.h>
#include <bitdraw.h>

#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
_LIT(KBitmap12bit, "z:\\system\\data\\uibench_12bit.mbm");
_LIT(KBitmap16bit, "z:\\system\\data\\uibench_16bit.mbm");
_LIT(KBitmap24bit, "z:\\system\\data\\uibench_24bit.mbm");
_LIT(KBitmap32bit, "z:\\system\\data\\uibench_32bit.mbm");
_LIT(KBitmapTile, "z:\\system\\data\\uibench_tile.mbm");
#endif

#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
const TInt KIterationsToTest = 5; 		// Number of iterations to run tests
#endif
const TInt KMaxPixelFormats = 30;		// Number of bitmaps/tiles to create

// For many of these tests, the size of which the destination blit is cropped to. 
const TRect KCropTo = TRect(0,0,200,200);

CTBitBltPerfDirectGdi::~CTBitBltPerfDirectGdi()
	{
	}

CTBitBltPerfDirectGdi::CTBitBltPerfDirectGdi()
	:iBitmapImage(KMaxPixelFormats),
	iBitmapTile(KMaxPixelFormats)
	{
	SetTestStepName(KTBitBltPerfDirectGdi);
	}

#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CTBitBltPerfDirectGdi::doTestStepPreambleL()
	{	
	CTDirectGdiTestBase::doTestStepPreambleL();
	
	// Create a CFbsBitmap image for each source mode we are testing.
	for (TInt srcMode = 0; srcMode < iSourcePixelFormatArray.Count(); ++srcMode)
		{
		TDisplayMode bitmapDisplayMode = TDisplayModeMapping::MapPixelFormatToDisplayMode(iSourcePixelFormatArray[srcMode]);
		
		CFbsBitmap* bitmapImage = NULL;
		switch(bitmapDisplayMode)
		{
			case EColor4K:
			case EGray256:
				bitmapImage = LoadBitmapL(KBitmap12bit, 0);
				break;
			case EColor64K:
				bitmapImage = LoadBitmapL(KBitmap16bit, 0);
				break;
			case EColor16M:
				bitmapImage = LoadBitmapL(KBitmap24bit, 0);
				break;
			default:
				bitmapImage = LoadBitmapL(KBitmap32bit, 0);
				break;
		}
		if (bitmapImage && bitmapImage->DisplayMode() != bitmapDisplayMode)
			{
			CleanupStack::PushL(bitmapImage);
			CFbsBitmap* tempBitmap = CopyIntoNewBitmapL(bitmapImage, bitmapDisplayMode);
			CleanupStack::PopAndDestroy(1, bitmapImage); //bitmapImage
			bitmapImage = tempBitmap;
			}
		iBitmapImage.AppendL(bitmapImage);
		}

	// Create 8bit alpha bitmap
	CFbsBitmap* tempBitmap = LoadBitmapL(KBitmap12bit, 0);
	CleanupStack::PushL(tempBitmap);
	iAlpha8bit = CopyIntoNewBitmapL(tempBitmap, EGray256);
	CleanupStack::PopAndDestroy(tempBitmap);	// tempBitmap

	// Create CFbsBitmaps for the tile images of various pixel formats.
	// The first entry in the array is always the default pixelformat loaded from the mbm.
	// Then in the rest of the array are bitmaps converted to the source pixel formats.
	CFbsBitmap* tile = LoadBitmapL(KBitmapTile, 0);	
	iBitmapTile.AppendL(tile);
	for (TInt srcMode = 0; srcMode < iSourcePixelFormatArray.Count(); ++srcMode)
		{
		TDisplayMode bitmapDisplayMode = TDisplayModeMapping::MapPixelFormatToDisplayMode(iSourcePixelFormatArray[srcMode]);
		CFbsBitmap* tileCopy = CopyIntoNewBitmapL(tile, bitmapDisplayMode);
		iBitmapTile.AppendL(tileCopy);
		}
	
	return TestStepResult();
	}

/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CTBitBltPerfDirectGdi::doTestStepPostambleL()
	{	
	delete iAlpha8bit;
	iAlpha8bit = NULL;

	for (TInt ii = iBitmapImage.Count() - 1; ii >= 0; --ii)
		{
		delete iBitmapImage[ii];
		}
	iBitmapImage.Close();

	for (TInt ii = iBitmapTile.Count() - 1; ii >= 0; --ii)
		{
		delete iBitmapTile[ii];
		}
	iBitmapTile.Close();

	return CTDirectGdiTestBase::doTestStepPostambleL();
	}
#endif
/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTBitBltPerfDirectGdi::doTestStepL()
	{
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    INFO_PRINTF1(_L("CTBitBltPerfDirectGdi can only be run with RSgImage legacy"));
    return TestStepResult();
#else
	// for each display mode
	for(TInt dispModeIndex = 0; dispModeIndex < iTargetPixelFormatArray.Count(); ++dispModeIndex)
		{
		TDisplayMode targetDisplayMode = TDisplayModeMapping::MapPixelFormatToDisplayMode(iTargetPixelFormatArray[dispModeIndex]);
		if(SetTargetPixelFormatL(targetDisplayMode))
			{
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0150"));
			SimpleBitBltL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0151"));
			SimpleDrawBitmapL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0153"));
			MaskedBitBltAlphaL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0154"));
			MaskedBitBltSameL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0155"));
			MaskedBitBltL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0156"));
			MaskedDrawBitmapL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0136"));
			MaskedDrawBitmapScaledL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0157"));
			BitBltCpuAccessL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-UI-BENCH-0158"));
			SimpleDrawResourceL();
			RecordTestResultL();
			}
		}

    CloseTMSGraphicsStep();
	return TestStepResult();
#endif	
	}

#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0150

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of BitBlt() for the current screen mode and for various bitmap pixel formats.

@SYMTestActions
Draw an entire bitmap using BitBlt(), with no overlapping or clipping. 
Measure the time taken.

@SYMTestExpectedResults
The performance to be logged as a pixel rate, per bitmap.
*/
void CTBitBltPerfDirectGdi::SimpleBitBltL()
	{
	INFO_PRINTF1(_L("CTBitBltPerfDirectGdi::SimpleBitBlt"));
	
	_LIT(KTestName, "DirectGdiBitBlt");
	for (TInt source = 0; source < iBitmapImage.Count(); ++source)
		{
		BitBltBitmapTestL(EBitBlt, iBitmapImage[source], NULL, KCropTo, KTestName, iContext);
		}
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0151

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measures the performance of DrawBitmap() for the current screen mode, for various bitmap pixel formats.
The bitmap is drawn without scaling.

@SYMTestActions
Create a copy of the bitmap, and scale to the required size, prior to running the test.
For each required source pixel format, use DrawBitmap() to tile a bitmap across the target, avoiding any
clipping or overlap. 

@SYMTestExpectedResults
The performance to be logged as a pixel rate, per bitmap.
*/
void CTBitBltPerfDirectGdi::SimpleDrawBitmapL()
	{
	INFO_PRINTF1(_L("CTBitBltPerfDirectGdi::SimpleDrawBitmap"));
	
	_LIT(KTestName, "DirectGdiDrawBitmap");

	for (TInt source = 0; source < iBitmapImage.Count(); ++source)
		{
		// Use a bitmap that needs no scaling.
		CFbsBitmap* bitmapBitBltImage = CopyIntoNewBitmapL(iBitmapImage[source], iBitmapImage[source]->DisplayMode());
		CleanupStack::PushL(bitmapBitBltImage);
		bitmapBitBltImage->Resize(KCropTo.Size());
		BitBltBitmapTestL(EDrawBitmap, bitmapBitBltImage, NULL, bitmapBitBltImage->SizeInPixels(), KTestName, iContext);
		CleanupStack::PopAndDestroy(1, bitmapBitBltImage);
		}
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0155

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of BitBltMasked() for the current screen mode, using bitmaps of various 
pixel formats.

@SYMTestActions
Call BitBltMasked() with a bitmap for various pixel formats of bitmap, using the same bitmap 
as the mask bitmap each time. The bitmap is tiled across the target to avoid overlapping or clipping. 

@SYMTestExpectedResults
The performance to be logged as a pixel rate, per bitmap.
*/
void CTBitBltPerfDirectGdi::MaskedBitBltL()
	{
	INFO_PRINTF1(_L("CTBitBltPerfDirectGdi::MaskedBitBlt"));
	
	_LIT(KTestName, "DirectGdiMBitBlt");
	
	// for each display mode
	for (TInt source = 0; source < iBitmapImage.Count(); ++source)
		{
		BitBltBitmapTestL(EBitBltMasked, iBitmapImage[source], iBitmapTile[0], KCropTo, KTestName, iContext);
		}
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0154

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of BitBlt(), using the same bitmap as a mask with different screen modes.

@SYMTestActions
Call BitBltMasked() with a bitmap for various pixel formats of bitmap, using the bitmap 
as the mask bitmap each time. The bitmap is tiled across the target to avoid overlapping or clipping.

@SYMTestExpectedResults
The performance to be logged as a pixel rate, per bitmap.
*/	
void CTBitBltPerfDirectGdi::MaskedBitBltSameL()
	{
	INFO_PRINTF1(_L("CTBitBltPerfDirectGdi::MaskedBitBltSame"));
	
	_LIT(KTestName, "DirectGdiMBitBltS");
	
	for (TInt tile = 0; tile < iBitmapTile.Count(); ++tile)
		{
		BitBltBitmapTestL(EBitBltMasked, iBitmapTile[tile], iBitmapTile[tile], KCropTo, KTestName, iContext);
		}
	}


/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0153

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of BitBltMasked(), with an alpha bitmap with different screen modes.

@SYMTestActions
Call BitBltMasked() with a bitmap for various pixel formats of bitmap, using the same alpha bitmap 
as the mask bitmap each time. The bitmap is tiled across the target to avoid overlapping or clipping.

@SYMTestExpectedResults
The performance to be logged as a pixel rate, per bitmap.
*/
void CTBitBltPerfDirectGdi::MaskedBitBltAlphaL()
	{
	INFO_PRINTF1(_L("CTBitBltPerfDirectGdi::MaskedBitBltAlpha"));	
	
	_LIT(KTestName, "DirectGdiMBitBltA");
	
	for (TInt source = 0; source < iBitmapImage.Count(); ++source)
		{
		BitBltBitmapTestL(EBitBltMasked, iBitmapImage[source], iAlpha8bit, KCropTo, KTestName, iContext);
		}
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0157

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of BitBlt() for the current screen mode and for various bitmap pixel formats,
with a bitmap that is accessed by the CPU via DataAddresss(), and is therefore never cached by DirectGDI.

@SYMTestActions
Create a copy of a test bitmap.
Call DataAddress() on the bitmap.
Draw an entire bitmap using BitBlt(), with no overlapping or clipping. 
Measure the time taken.

@SYMTestExpectedResults
The performance to be logged as a pixel rate, per bitmap.
*/
void CTBitBltPerfDirectGdi::BitBltCpuAccessL()
	{
	INFO_PRINTF1(_L("CTBitBltPerfDirectGdi::BitBltCpuAccess"));
	
	_LIT(KTestName, "DirectGdiBitBltCpuAccess");

	for (TInt source = 0; source < iBitmapImage.Count(); ++source)
		{
		CFbsBitmap* bitmapBitBltImage = CopyIntoNewBitmapL(iBitmapImage[source], iBitmapImage[source]->DisplayMode());
		TESTL(bitmapBitBltImage != NULL);
		CleanupStack::PushL(bitmapBitBltImage);
		bitmapBitBltImage->DataAddress();	// CPU Access so that it is not cached by DirectGDI.
		BitBltBitmapTestL(EBitBlt, bitmapBitBltImage, NULL, KCropTo, KTestName, iContext);
		CleanupStack::PopAndDestroy(1, bitmapBitBltImage);		
		}
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0158

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of DrawResource() for the current screen mode and for various bitmap pixel formats,
with no rotation.

@SYMTestActions
Draw an entire bitmap using DrawResource(), by tiling the image across the target to avoid overlapping and
clipping. Measure the time taken.

@SYMTestExpectedResults
The performance to be logged as a pixel rate, per bitmap.
*/
void CTBitBltPerfDirectGdi::SimpleDrawResourceL()
	{
	INFO_PRINTF1(_L("CTBitBltPerfDirectGdi::SimpleDrawResource"));
	_LIT(KTestName, "DirectGdiDrawResource");
		
	for (TInt source = 0; source < iBitmapImage.Count(); ++source)
		{
		TSgImageInfo imageInfo;
		imageInfo.iSizeInPixels = iBitmapImage[source]->SizeInPixels();
		imageInfo.iPixelFormat = TDisplayModeMapping::MapDisplayModeToPixelFormat(iBitmapImage[source]->DisplayMode());
		imageInfo.iUsage = ESgUsageDirectGdiSource;

		// Create an RSgImage from the CFbsBitmap.
		RSgImage sgImage;	
		TInt res = sgImage.Create(imageInfo, iBitmapImage[source]->DataAddress(), iBitmapImage[source]->DataStride());
		TESTNOERRORL(res);
		CleanupClosePushL(sgImage);		
			
		CDirectGdiDriver* drv = CDirectGdiDriver::Static();
		TEST(drv != NULL);
		
		RDirectGdiDrawableSource* imageSrc = new RDirectGdiDrawableSource(*drv);
		TEST(imageSrc != NULL);
		res = imageSrc->Create(sgImage);		
		TESTNOERRORL(res);	

		// The test loop.
		// Tile across the target, and avoid clipping and overlapping of tiles.
		const TSize targetSize = GetPixmapSizeInPixels();
		const TSize tileSize = imageInfo.iSizeInPixels;
		const TInt tileColumns = targetSize.iWidth / tileSize.iWidth;
		const TInt tileRows = targetSize.iHeight / tileSize.iHeight;
		const TInt numTiles = tileColumns * tileRows;
		const TInt numPixelsPerIteration = numTiles * tileSize.iWidth * tileSize.iHeight;
		iProfiler->InitResults();
		for(TInt count=KIterationsToTest; count>0; --count)
			{
			TPoint bitmapPosition = TPoint(0,0);
			for (TInt tile = numTiles - 1; tile >= 0; --tile)
				{
				iContext->DrawResource(bitmapPosition, *imageSrc, DirectGdi::EGraphicsRotationNone);
				bitmapPosition.iX += tileSize.iWidth;
				if (bitmapPosition.iX + tileSize.iWidth > targetSize.iWidth)
					{
					bitmapPosition.iX = 0;
					bitmapPosition.iY += tileSize.iHeight;
					}
				}
			iDGdiDriver->Finish();
			}
		iProfiler->MarkResultSetL();
		TESTNOERRORL(iDGdiDriver->GetError());
		
		WriteTargetOutput(KTestName());		
		const TDisplayMode targetDisplayMode = TDisplayModeMapping::MapPixelFormatToDisplayMode(iImageInfo.iPixelFormat);
		iProfiler->ResultsAnalysisPixelRate(KTestName, CFbsBitGc::EGraphicsOrientationNormal, iBitmapImage[source]->DisplayMode(), targetDisplayMode, KIterationsToTest, numPixelsPerIteration);
		
		imageSrc->Close();
		delete imageSrc;
		CleanupStack::PopAndDestroy(1, &sgImage);
		}
	}


/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0156

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of DrawBitmapMasked() for the current screen mode, using bitmaps of various 
pixel formats.

@SYMTestActions
Call DrawBitmapMasked() with a bitmap for various pixel formats of bitmap, using the same bitmap 
as the mask bitmap each time. The bitmap is tiled across the target to avoid overlapping or clipping. 

@SYMTestExpectedResults
The performance to be logged as a pixel rate, per bitmap.
*/
void CTBitBltPerfDirectGdi::MaskedDrawBitmapL()
	{
	INFO_PRINTF1(_L("CTBitBltPerfDirectGdi::MaskedDrawBitmap"));
	
	_LIT(KTestName, "DirectGdiDrawBitmapM");
	
	// for each display mode
	for (TInt source = 0; source < iBitmapImage.Count(); ++source)
		{
		BitBltBitmapTestL(EDrawBitmapMasked, iBitmapImage[source], iBitmapTile[0], KCropTo, KTestName, iContext, EFalse);
		}
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0136

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure the performance of DrawBitmapMasked() for the current screen mode, using bitmaps of various 
pixel formats.

@SYMTestActions
Call DrawBitmapMasked() with a bitmap for various pixel formats of bitmap, using the same bitmap 
as the mask bitmap each time. The bitmap is tiled across the target to avoid overlapping or clipping. 

@SYMTestExpectedResults
The performance to be logged as a pixel rate, per bitmap.
*/
void CTBitBltPerfDirectGdi::MaskedDrawBitmapScaledL()
	{
	INFO_PRINTF1(_L("CTBitBltPerfDirectGdi::MaskedDrawBitmapScaled"));
	
	_LIT(KTestName, "DirectGdiDrawBitmapMS");
	
	// for each display mode
	for (TInt source = 0; source < iBitmapImage.Count(); ++source)
		{
		BitBltBitmapTestL(EDrawBitmapMasked, iBitmapImage[source], iBitmapTile[0], KCropTo, KTestName, iContext, ETrue);
		}
	}


/**
Helper function to test bitblt bitmaps for different types of blitting. The bitmap is 
tiled across the target to avoid any overdraw. Once the target is filled, the process
is performed for @c KIterationsToTest times.

@param aBitBltTest the bitblt test to carry out
@param aBitmapToBlt the bitmap to bitblt
@param aBitmapMask the bitmap mask
@param aCrop rectangle to crop bitblt to
@param aOrientation the orientation of the display
@param aTestName the name of the test
@param aGc the graphics context to bitblt to
*/
void CTBitBltPerfDirectGdi::BitBltBitmapTestL(TBitBltTest aBitBltTest, CFbsBitmap* aBitmapToBlt, CFbsBitmap* aBitmapMask, TRect aCropTo, const TDesC& aTestName, CDirectGdiContext* aGc, TBool aScaled)
	{
	aGc->Clear();
	iDGdiDriver->Finish();
	const TSize targetSize = GetPixmapSizeInPixels();

	const TInt scalingFactor = aScaled ? 2 : 1;
	// Calculate the number of tiles that will fit fully on the pixmap
	const TSize tileSize = TSize(aCropTo.Width()/scalingFactor, aCropTo.Height()/scalingFactor);
	const TInt tileColumns = targetSize.iWidth / tileSize.iWidth;
	const TInt tileRows = targetSize.iHeight / tileSize.iHeight;
	const TInt numTiles = tileColumns * tileRows;
	const TInt numPixelsPerIteration = numTiles * tileSize.iWidth * tileSize.iHeight;
	
	switch (aBitBltTest)
	{
	case EDrawBitmap:
		{
		// Do a draw outside the loop so that we aren't measuring the time to load and cache the bitmap.
		aGc->DrawBitmap(TRect(0, 0, tileSize.iWidth, tileSize.iHeight), *aBitmapToBlt);
		iProfiler->InitResults();
		for(TInt count=KIterationsToTest; count>0; --count)
			{
			TPoint bitmapPosition = TPoint(0,0);
			for (TInt tile = numTiles - 1; tile >= 0; --tile)
				{
				aGc->DrawBitmap(TRect(bitmapPosition.iX, bitmapPosition.iY, bitmapPosition.iX+tileSize.iWidth, bitmapPosition.iY+tileSize.iHeight), *aBitmapToBlt);
				bitmapPosition.iX += tileSize.iWidth;
				if (bitmapPosition.iX + tileSize.iWidth > targetSize.iWidth)
					{
					bitmapPosition.iX = 0;
					bitmapPosition.iY += tileSize.iHeight;
					}
				}
			iDGdiDriver->Finish();
			}
		}
		break;
	case EDrawBitmapMasked:
		{
		// Do a draw outside the loop so that we aren't measuring the time to load and cache the bitmap.
		aGc->DrawBitmapMasked(TRect(0, 0, tileSize.iWidth, tileSize.iHeight), *aBitmapToBlt, aCropTo, *aBitmapMask, EFalse);
		iProfiler->InitResults();
		for(TInt count=KIterationsToTest; count>0; --count)
			{
			TPoint bitmapPosition = TPoint(0,0);
			for (TInt tile = numTiles - 1; tile >= 0; --tile)
				{
				aGc->DrawBitmapMasked(TRect(bitmapPosition.iX, bitmapPosition.iY, bitmapPosition.iX+tileSize.iWidth, bitmapPosition.iY+tileSize.iHeight), *aBitmapToBlt, aCropTo, *aBitmapMask, EFalse);
				bitmapPosition.iX += tileSize.iWidth;
				if (bitmapPosition.iX + tileSize.iWidth > targetSize.iWidth)
					{
					bitmapPosition.iX = 0;
					bitmapPosition.iY += tileSize.iHeight;
					}
				}
			iDGdiDriver->Finish();
			}
		}
		break;
	case EBitBlt:
		{
		// Do a draw outside the loop so that we aren't measuring the time to load and cache the bitmap.
		aGc->BitBlt(TPoint(0,0), *aBitmapToBlt, aCropTo);
		iProfiler->InitResults();
		for(TInt count=KIterationsToTest; count>0; --count)
			{
			TPoint bitmapPosition = TPoint(0,0);
			for (TInt tile = numTiles - 1; tile >= 0; --tile)
				{
				aGc->BitBlt(bitmapPosition, *aBitmapToBlt, aCropTo);
				bitmapPosition.iX += tileSize.iWidth;
				if (bitmapPosition.iX + tileSize.iWidth > targetSize.iWidth)
					{
					bitmapPosition.iX = 0;
					bitmapPosition.iY += tileSize.iHeight;
					}
				}
			iDGdiDriver->Finish();
			}
		}
		break;
	case EBitBltMasked:
		{
		// Do a draw outside the loop so that we aren't measuring the time to load and cache the bitmap.
		aGc->BitBltMasked(TPoint(0,0), *aBitmapToBlt, aCropTo, *aBitmapMask, EFalse);
		iProfiler->InitResults();
		for(TInt count=KIterationsToTest; count>0; --count)
			{
			TPoint bitmapPosition = TPoint(0,0);
			for (TInt tile = numTiles - 1; tile >= 0; --tile)
				{
				aGc->BitBltMasked(bitmapPosition, *aBitmapToBlt, aCropTo, *aBitmapMask, EFalse);
				bitmapPosition.iX += tileSize.iWidth;
				if (bitmapPosition.iX + tileSize.iWidth > targetSize.iWidth)
					{
					bitmapPosition.iX = 0;
					bitmapPosition.iY += tileSize.iHeight;
					}
				}
			iDGdiDriver->Finish();
			}
		}
		break;
	}
	
	TESTNOERRORL(iDGdiDriver->GetError());
	iProfiler->MarkResultSetL();
	
	WriteTargetOutput(aTestName);

	const TDisplayMode targetDisplayMode = TDisplayModeMapping::MapPixelFormatToDisplayMode(iImageInfo.iPixelFormat);
	iProfiler->ResultsAnalysisPixelRate(aTestName, CFbsBitGc::EGraphicsOrientationNormal, aBitmapToBlt->DisplayMode(), targetDisplayMode, KIterationsToTest, numPixelsPerIteration);
	}
#endif
