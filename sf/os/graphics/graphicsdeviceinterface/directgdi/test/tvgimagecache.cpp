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

#include "tvgimagecache.h"
#include <graphics/directgdiengine.h>

_LIT(KRomBitmap, "z:\\system\\data\\dgdi32bit.mbm");

CTVgImageCache::CTVgImageCache()
	{
	SetTestStepName(KTDirectGdiVgImageCacheStep);
	}

CTVgImageCache::~CTVgImageCache()
	{	
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-VGIMAGECACHE-0001

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test entries are added to the cache.

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions
	1.	Create bitmaps and bitmap masks.  Blit them so that they are stored in the cache.
		Check the VGImages created from the bitmaps are stored in the cache.
	2.	Blit the bitmaps again in a different order.
		Check the order that the bitmaps are stored in the cache.
		
@SYMTestExpectedResults
	1.	There are entries for each bitmap and bitmap mask.
	2.	The bitmaps are stored in the cache in order of most-recently used to least-recently used.
 */
void CTVgImageCache::TestAddEntriesL()
	{	
	_LIT(KTestName, "Test Bitmaps Are Added To Cache"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	CFbsBitmap* romBitmap = new(ELeave) CFbsBitmap;
	TInt err = romBitmap->Load(KRomBitmap);
	TESTNOERRORL(err);
	CleanupStack::PushL(romBitmap);
	
	TSize bitmapSize(48,48);
	TRect bitmapRect(TPoint(0,0), bitmapSize);
	CFbsBitmap* bitmap1 = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat,  bitmapSize, TSize (20, 20));
	TESTL(bitmap1 != NULL);
	CleanupStack::PushL(bitmap1);
	CFbsBitmap* bitmap2 = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat, bitmapSize, TSize (10, 10));
	TESTL(bitmap2 != NULL);
	CleanupStack::PushL(bitmap2);
	CFbsBitmap* bitmap3 = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat, bitmapSize, TSize (8, 8));
	TESTL(bitmap3 != NULL);
	CleanupStack::PushL(bitmap3);
	CFbsBitmap* mask1 = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat, bitmapSize, TSize (4, 4));
	TESTL(mask1 != NULL);
	CleanupStack::PushL(mask1);
	CFbsBitmap* mask2 = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat, bitmapSize, TSize (2, 2));
	TESTL(mask2 != NULL);
	CleanupStack::PushL(mask2);

	ResetGc();
	CleanupStack::PushL(TCleanupItem(ResetCache, iVgImageCache));

	iGc->BitBlt(TPoint(0,0), *romBitmap);		
	iGc->DrawBitmap(bitmapRect, *bitmap1);
	iGc->BitBltMasked(TPoint(0,0), *bitmap2, bitmapRect, *mask1, EFalse);
	iGc->DrawBitmapMasked(bitmapRect, *bitmap3, bitmapRect, *mask2, EFalse);
	
	// Check bitmaps added
	TInt numEntries = iVgImageCache->NumEntries();
	TEST(numEntries == 6);
	// The following tests must leave if they fail,
	// otherwise a panic will occur in the ordered cache entry test
	TESTL(iVgImageCache->IsInCache(romBitmap->SerialNumber()));
	TESTL(iVgImageCache->IsInCache(bitmap1->SerialNumber()));
	TESTL(iVgImageCache->IsInCache(bitmap2->SerialNumber()));
	TESTL(iVgImageCache->IsInCache(bitmap3->SerialNumber()));
	TESTL(iVgImageCache->IsInCache(mask1->SerialNumber()));
	TESTL(iVgImageCache->IsInCache(mask2->SerialNumber()));
	
	//Blit a few more times and check order of items in cache
	iGc->BitBlt(TPoint(0,0), *bitmap3);
	iGc->BitBlt(TPoint(0,0), *bitmap1);
	iGc->BitBlt(TPoint(0,0), *romBitmap);
	iGc->BitBlt(TPoint(0,0), *mask2);
	iGc->BitBlt(TPoint(0,0), *bitmap2);
	
	// Expect the most recently used to be at the head of the list
	// i.e. bitmap2, mask2, romBitmap, bitmap1, bitmap3, mask1
	TInt64* serialNumList = new TInt64[numEntries];
	TESTL(serialNumList != NULL);
	iVgImageCache->GetOrderedCacheEntries(*serialNumList,numEntries);
	TEST(serialNumList[0] == bitmap2->SerialNumber());
	TEST(serialNumList[1] == mask2->SerialNumber());
	TEST(serialNumList[2] == romBitmap->SerialNumber());
	TEST(serialNumList[3] == bitmap1->SerialNumber());
	TEST(serialNumList[4] == bitmap3->SerialNumber());
	TEST(serialNumList[5] == mask1->SerialNumber());
	delete[] serialNumList;

	CleanupStack::PopAndDestroy(7, romBitmap);
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-VGIMAGECACHE-0002

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test entries are added to the cache.

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions
	Create a bitmap.  Blit it so that it is stored in the cache.
	Check there is an entry for that bitmap in the cache.
	Resize the bitmap (so that the bitmap's touch count is increased).
	Blit the bitmap again.
	Check the value of the touch count stored in the cache.
	
@SYMTestExpectedResults
	Entry exists for bitmap and touch count value stored in cache
	is same as the bitmap after it was resized.

 */
void CTVgImageCache::TestBitmapResizedL()
	{	
	_LIT(KTestName, "Test That Images In Cache Are Updated When Associated Bitmap Is Resized"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	TSize bitmapSize(48,48);
	CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat,  bitmapSize, TSize (8, 8));
	TESTL(bitmap != NULL);

	ResetGc();

	iGc->BitBlt(TPoint(0,0), *bitmap);
	TInt64 serialNumber = bitmap->SerialNumber();
	// Check bitmaps added
	TEST(iVgImageCache->IsInCache(serialNumber));
	TInt oldTouchCount = iVgImageCache->TouchCount(serialNumber);
	TEST(oldTouchCount == bitmap->TouchCount());
	
	//Resize bitmap
	TESTNOERROR(bitmap->Resize(TSize(80,24)));
	iGc->BitBlt(TPoint(0,0), *bitmap);
	TEST(iVgImageCache->TouchCount(serialNumber) != oldTouchCount);
	TEST(iVgImageCache->TouchCount(serialNumber) == bitmap->TouchCount());

	delete bitmap;
	iVgImageCache->ResetCache();
	}


/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-VGIMAGECACHE-0003

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test entries are added to the cache.

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions
	Create a bitmap.  Blit it so that it is stored in the cache.
	Check there is an entry for that bitmap in the cache.
	Swap the width and height of the bitmap (so that the bitmap's touch count is increased).
	Blit the bitmap again.
	Check the value of the touch count stored in the cache.
	
@SYMTestExpectedResults
	Entry exists for bitmap and touch count value stored in cache
	is same as the bitmap after its width and height were swapped.

 */
void CTVgImageCache::TestBitmapSwapWidthAndHeightL()
	{	
	_LIT(KTestName, "Test That Images In Cache Are Updated When Associated Bitmap Swaps Height and Width"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	TSize bitmapSize(40,60);
	CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat,  bitmapSize, TSize (8, 8));
	TESTL(bitmap != NULL);

	ResetGc();
	iVgImageCache->ResetCache();

	iGc->BitBlt(TPoint(0,0), *bitmap);
	// Check bitmap is added to cache
	TEST(iVgImageCache->IsInCache(bitmap->SerialNumber()));
	TInt oldTouchCount = bitmap->TouchCount();
	TEST(oldTouchCount == iVgImageCache->TouchCount(bitmap->SerialNumber()));
	
	// Swap width & height of bitmap within a Begin/End bounds
	// so that touch count increases
	TESTNOERROR(bitmap->SwapWidthAndHeight());
	TInt newTouchCount = bitmap->TouchCount();
	iGc->BitBlt(TPoint(0,0), *bitmap);
	// Check touch count is now different
	TEST(newTouchCount != oldTouchCount);
	// Expect cache to update entry for bitmap
	TEST(iVgImageCache->TouchCount(bitmap->SerialNumber()) == newTouchCount);

	delete bitmap;
	iVgImageCache->ResetCache();
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-VGIMAGECACHE-0004

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test entries are not added to the cache for volatile bitmaps.

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions
	Create Font Image Storage. In the cycle request Glyph Image entry for particular Glyph 
	code and font ID. 
	Delete Glyph Storage.
	
@SYMTestExpectedResults
	No entry exists for the bitmap in the cache.

 */
void CTVgImageCache::TestVolatileBitmapL()
	{	
	_LIT(KTestName, "Test That Volatile Bitmaps Are not Stored In Cache"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	TSize bitmapSize(40,60);
	CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat,  bitmapSize, TSize (8, 8));
	TESTL(bitmap != NULL);

	ResetGc();
	iVgImageCache->ResetCache();

	// Make bitmap volatile
	bitmap->DataAddress();
	iGc->BitBlt(TPoint(0,0), *bitmap);
	// Check bitmap is added to cache
	TEST(!iVgImageCache->IsInCache(bitmap->SerialNumber()));

	delete bitmap;
	iVgImageCache->ResetCache();
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-VGIMAGECACHE-0005

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test least recently used entries are deleted  when the cache is full and a new entry is added to the cache.

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions
	Fill the cache up (by blitting many different bitmaps).
	Blit one more bitmap so that the cache has to delete an entry.
	Get the driver's MDirectGdiDriverCacheSize extension interface.
	Check that a new maximum cache size cannot be set that is smaller
	than the existing cache size.
	
@SYMTestExpectedResults
	An entry was deleted and that entry was the least recently used.
	A cache size cannot be set that is smaller than the existing cache size.
 */
void CTVgImageCache::TestFullCacheL()
	{	
	_LIT(KTestName, "Test Adding Image to Cache when Cache is Full"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	iVgImageCache->ResetCache();
	CleanupStack::PushL(TCleanupItem(ResetCache, iVgImageCache));
	TInt maxCacheSize = iVgImageCache->MaxCacheSize();

	TSize bitmapSize(200,200);
	TInt dataStride = CFbsBitmap::ScanLineLength(bitmapSize.iWidth, TDisplayModeMapping::MapPixelFormatToDisplayMode(iTestParams.iSourcePixelFormat));
	TInt imageSizeInBytes = bitmapSize.iHeight * dataStride;
	TSize checksPerAxis(1,1);
	// Create the first bitmap to be added to the cache.
	// This is also be the bitmap to be removed from the cache when full.
	CFbsBitmap* firstBitmap = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat,  bitmapSize, checksPerAxis);
	TESTL(firstBitmap != NULL);
	CleanupStack::PushL(firstBitmap);
	iGc->BitBlt(TPoint(0,0), *firstBitmap);
	TEST(iVgImageCache->IsInCache(firstBitmap->SerialNumber()));

	// Fill the cache up.
	while(imageSizeInBytes + iVgImageCache->CacheSizeInBytes() < maxCacheSize)
		{
		CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat,  bitmapSize, checksPerAxis);
		TESTL(bitmap != NULL);
		iGc->BitBlt(TPoint(0,0), *bitmap);
		delete bitmap;
		}
	
	// Add one more entry to the cache
	CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat,  bitmapSize, checksPerAxis);
	TESTL(bitmap != NULL);
	TEST(!iVgImageCache->IsInCache(bitmap->SerialNumber()));
	TEST(iVgImageCache->IsInCache(firstBitmap->SerialNumber()));
	iGc->BitBlt(TPoint(0,0), *bitmap);
	
	// firstBitmap should now have been removed from the cache as it was the least recently used.
	TEST(!iVgImageCache->IsInCache(firstBitmap->SerialNumber()));
	// last bitmap should be in cache
	TEST(iVgImageCache->IsInCache(bitmap->SerialNumber()));
	
	// get the driver's MDirectGdiDriverCacheSize extension interface and attempt
	// to set the maximum cache size to be smaller than the existing cache size
	CDirectGdiDriver* driver = CDirectGdiDriver::Static();
	TESTL(driver != NULL);
	MDirectGdiDriverCacheSize* driverCacheSize = NULL;
	TInt res = driver->GetInterface(TUid::Uid(KDirectGdiDriverCacheSizeUid), (TAny*&)driverCacheSize);
	TESTNOERRORL(res);
	// save the original cache size
	TInt originalCacheSize = driverCacheSize->MaxImageCacheSize();
	// setting the cache size to a size smaller than the existing cache should fail
	res = driverCacheSize->SetMaxImageCacheSize(iVgImageCache->CacheSizeInBytes()-1);
	TEST(res == KErrArgument);	
	// setting the cache size to the same size as the existing cache should pass
	res = driverCacheSize->SetMaxImageCacheSize(iVgImageCache->CacheSizeInBytes());
	TESTNOERROR(res);
	TEST(iVgImageCache->CacheSizeInBytes() == driverCacheSize->MaxImageCacheSize());
	// reset the original driver cache size
	res = driverCacheSize->SetMaxImageCacheSize(originalCacheSize);
	TESTNOERROR(res);

	delete bitmap;
	CleanupStack::PopAndDestroy(firstBitmap);
	CleanupStack::PopAndDestroy(iVgImageCache);
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-VGIMAGECACHE-0006

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test adding a bitmap larger than maximum size of cache.

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions.
	Blit a small bitmap which is added to the cache.
	Blit a bitmap whose size in pixels is more than the maximum size of the cache.
	
@SYMTestExpectedResults
	An entry for the large bitmap should not appear in the cache.
	An entry for the small bitmap should appear in the cache.

 */
void CTVgImageCache::TestImageLargerThanCacheL()
	{	
	_LIT(KTestName, "Test Adding Image Larger Than Cache"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();
	iVgImageCache->ResetCache();
	TInt maxCacheSize = iVgImageCache->MaxCacheSize();
	
	// Calculate height of bitmap whose width is 512 pixels so that the total size in bytes of the bitmap
	// is the same as the maximum cache size.
	TInt dataStride = CFbsBitmap::ScanLineLength(512, TDisplayModeMapping::MapPixelFormatToDisplayMode(iTestParams.iSourcePixelFormat));
	TInt largeBitmapHeight = maxCacheSize / dataStride;
	// Want bitmap with a larger size in bytes than cache,
	// so create a bitmap with width of 550 pixels and largeBitmapHeight
	TSize largeBitmapSize(550,largeBitmapHeight);

	TSize smallBitmapSize(200,200);

	TSize checksPerAxis(1,1);

	// Create a small bitmap to be added to the cache.
	CFbsBitmap* smallBitmap = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat,  smallBitmapSize, checksPerAxis);
	TESTL(smallBitmap != NULL);
	CleanupStack::PushL(smallBitmap);
	// Create large bitmap which is too large to fit in cache
	CFbsBitmap* largeBitmap = CreateCheckedBoardBitmapL(iTestParams.iSourcePixelFormat,  largeBitmapSize, checksPerAxis);
	TESTL(largeBitmap != NULL);

	// Add small bitmap to cache
	iGc->BitBlt(TPoint(0,0), *smallBitmap);
	TEST(iVgImageCache->IsInCache(smallBitmap->SerialNumber()));

	// Blit large bitmap
	iGc->BitBlt(TPoint(0,0), *largeBitmap);
	TEST(!iVgImageCache->IsInCache(largeBitmap->SerialNumber()));
	TEST(iVgImageCache->IsInCache(smallBitmap->SerialNumber()));

	delete largeBitmap;
	CleanupStack::PopAndDestroy(smallBitmap);
	iVgImageCache->ResetCache();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTVgImageCache::doTestStepPreambleL()
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
TVerdict CTVgImageCache::doTestStepL()
	{
	if (iUseDirectGdi)
		{
		// Test independent of target pixel formats, so just use first.
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[0];
		// Test needs a VG compatible source pixel format.
		iTestParams.iSourcePixelFormat = EUidPixelFormatRGB_565;
		SetTargetL(iTestParams.iTargetPixelFormat);
		if (!iUseSwDirectGdi)
			{
			RunTestsL();
			RunOomTestsL();
			}
		else
			{
			INFO_PRINTF1(_L("SW DirectGDI has no image cache to test!"));
			}

		}
	else
		{
		INFO_PRINTF1(_L("BitGDI has no image cache to test!"));
		}
	return TestStepResult();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTVgImageCache::RunTestsL()
	{
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-VGIMAGECACHE-0001"));
	TestAddEntriesL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-VGIMAGECACHE-0005"));
	TestFullCacheL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-VGIMAGECACHE-0006"));
	TestImageLargerThanCacheL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-VGIMAGECACHE-0002"));
	TestBitmapResizedL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-VGIMAGECACHE-0003"));
	TestBitmapSwapWidthAndHeightL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-VGIMAGECACHE-0004"));
	TestVolatileBitmapL();
	RecordTestResultL();
	}
