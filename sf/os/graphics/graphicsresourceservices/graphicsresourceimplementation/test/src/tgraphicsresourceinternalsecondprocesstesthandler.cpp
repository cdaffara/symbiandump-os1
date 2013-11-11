// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
@internalComponent
 */
#include <e32debug.h>
#include "tgraphicsresourceinternalsecondprocesstesthandler.h"
#include "tgraphicsresourcemultiprocessthread.h"
#include "tgraphicsresourceinternalsecondprocessenums.h"
#include "tsgdrawablegeneric.h"
#include "tsgimagegeneric.h"

CTSgResInternalSecondProcessTestHandler* CTSgResInternalSecondProcessTestHandler::NewLC()
	{
	CTSgResInternalSecondProcessTestHandler* self = new(ELeave) CTSgResInternalSecondProcessTestHandler();
	CleanupStack::PushL(self);
	return self;
	}

/**
Runs the specified test

@param TInt The test case to be run
 */
TInt CTSgResInternalSecondProcessTestHandler::RunTestCaseL(const TSgResIntTestInfo& aInfo)
	{
	RDebug::Printf("CTSgResSecondProcessTestHandler::RunTestCaseL(%i)", aInfo.iTestCase);
	TInt result = 0;
	switch (aInfo.iTestCase)
		{
		case ESgResIntDriverMemoryLeak:
			TestDriverMemoryLeakL();
			break;
		case ESgResIntDrawableOOM:
			result = TestDrawableOOM();
			break;
		case ESgResIntImageOOM:
			result = TestImageOOM(aInfo);
			break;
		case ESgResIntInitializeAndShutdown:
			result = TestDriverInitializeAndShutdownL();
			break;
		case ESgResIntInitializeAndShutdownManyTimes:
			result = TestDriverInitializeAndShutdownManyTimes();
			break;
		case ESgResIntResourceProfiling:
			result = TestResourceProfiling(aInfo);
			break;
		default:
			result = KErrNotFound;
			break;
		}
	RDebug::Printf("CTSgResSecondProcessTestHandler::RunTestCaseL result=%i", result);
	return result;
	}

CTSgResInternalSecondProcessTestHandler::CTSgResInternalSecondProcessTestHandler()
	{
	}

CTSgResInternalSecondProcessTestHandler::~CTSgResInternalSecondProcessTestHandler()
	{
	iSgDriver.Close();
	}

/**
Opens the SgDriver and gets the test extension interfaces required for the
internal tests.
 */
void CTSgResInternalSecondProcessTestHandler::OpenDriverL()
	{
	User::LeaveIfError(iSgDriver.Open());
	User::LeaveIfError(iSgDriver.GetInterface(iTestExt));
	User::LeaveIfError(iSgDriver.GetInterface(iProfExt));
	}

/**
Second process implementaion of the test ESgResIntDriverMemoryLeak.

Memory leak detection is beun and an image opened.
The image is not closed before the memory leak checking is ended.
This should cause an SGALLOC panic.
 */
void CTSgResInternalSecondProcessTestHandler::TestDriverMemoryLeakL()
	{
	iTestExt->AllocMarkStart();
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageBitOpenVgImage;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	
	RSgImage image;
	image.Create(info, NULL, 0);
	iTestExt->AllocMarkEnd(0); //Expecting this to panic

	image.Close();
	}

/**
Second process impementation of the test ESgResIntDrawableOOM.

Tests RSgDrawable in a number of different situations with simulated
low memory conditions.
 */
TInt CTSgResInternalSecondProcessTestHandler::TestDrawableOOM()
	{
	TInt result = 0;
	TInt err = KErrNone;
	TInt tryCount = 0;
	do
		{
		iTestExt->SetAllocFail(RAllocator::EFailNext, ++tryCount);
		TRAP(err, DoDrawableMemoryTestsL());
		} 
	while(err == KErrNoMemory);

	iTestExt->SetAllocFail(RAllocator::ENone, 0);

	result |= EFirstTestPassed;
	return result;
	}

/**
Second process implementation of the test ESgResTestImageOOM.

Tests RSgImage in a number of different situations with simulated
low memory conditions.
 */
TInt CTSgResInternalSecondProcessTestHandler::TestImageOOM(const TSgResIntTestInfo& aInfo)
	{
	TInt result = 0;
	TInt err = KErrNone;
	TInt tryCount = 0;
	do
		{
		iTestExt->SetAllocFail(RAllocator::EFailNext, ++tryCount);
		TRAP(err, DoImageMemoryTestsL(aInfo));
		} 
	while(err == KErrNoMemory);

	iTestExt->SetAllocFail(RAllocator::ENone, 0);

	if(err == KErrNone)
	    {
	    result |= EFirstTestPassed;
	    }
	
	return result;
	}

/**
Performs the RSgDrawable low memory tests; checks for Heap and RSgDriver memory
leaks after each individual test.
 */
void CTSgResInternalSecondProcessTestHandler::DoDrawableMemoryTestsL()
	{
	//Construct the SgDrawable tests using EFalse to enable KErrNoMemory testing
	CTSgDrawableGeneric* drawableTests = new(ELeave)CTSgDrawableGeneric(EFalse);
	CleanupStack::PushL(drawableTests);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	drawableTests->TestOpenImageAsDrawableL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	drawableTests->TestGetDrawableDrawableIdL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	drawableTests->TestOpenDrawableInvalidL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	drawableTests->TestCloseDrawableWithoutOpenL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	CleanupStack::PopAndDestroy(drawableTests);
	}

/**
Performs the RSgImage low memory tests; checks for Heap and RSgDriver memory
leaks after each individual test.
 */
void CTSgResInternalSecondProcessTestHandler::DoImageMemoryTestsL(const TSgResIntTestInfo& aInfo)
	{
	// open image that is created in another process
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	TestOpenImageL(aInfo.iDrawableId);
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	
	//Construct the SgImage generic tests using EFalse to enable KErrNoMemory testing
	CTSgImageGeneric* imageTests = new(ELeave)CTSgImageGeneric(EFalse);
	CleanupStack::PushL(imageTests);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestGetPixelFormatsL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestCreateImageUninitializedL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestCreateImageL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestCreateImageFromExistingImageL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestGetImageInfoL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestGetImageDrawableIdL(); 
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);

	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestOpenImageL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestGetInterfaceL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestGetPixelFormatsInvalidL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestOpenImageInvalidL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestCloseImageManyTimesL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestCloseImageWithoutOpenL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestCreateImageInvalidL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);

	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestGetInfoImageInvalidL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);

	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestGetAttributesImageInvalidL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);

	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestCreateImageDataStrideL();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);

	__UHEAP_MARK; iTestExt->AllocMarkStart();
	imageTests->TestStress1L();
	__UHEAP_MARKEND; iTestExt->AllocMarkEnd(0);
	
	
	CleanupStack::PopAndDestroy(imageTests);
	}

/*
 Used for OOM testing for opening an image in another process. For this purpose,
 the image that is opened here must be created in another process.
 */
void CTSgResInternalSecondProcessTestHandler::TestOpenImageL(TSgDrawableId aId)
    {
    TSgDrawableId id = aId;
    
    RSgImage image;

    User::LeaveIfError(image.Open(id));
    
    image.Close();
    }

/**
Test the Local Reference count of RSgDriver when creating and destroying images
 */
TInt CTSgResInternalSecondProcessTestHandler::TestDriverInitializeAndShutdownL()
	{
	TInt result = 0;
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageBitOpenVgImage;
	info.iPixelFormat = EUidPixelFormatRGB_565;

	RSgImage image;	
	User::LeaveIfError(image.Create(info, KCrossImageData, KCrossImageDataStride));	

	if (1 == iProfExt->LocalResourceCount())
		result |= EFirstTestPassed;

	image.Close();

	User::LeaveIfError(image.Create(info, KCrossImageData, KCrossImageDataStride));	
	image.Close();

	if (KErrNone == image.Create(info, KCrossImageData, KCrossImageDataStride))
		result |= ESecondTestPassed;
	image.Close();

	if (0 == iProfExt->LocalResourceCount())
		result |= EThirdTestPassed;

	return result;
	}

/**
Test the Local Reference count of RSgDriver when creating and destroying images
with multiple driver sessions.
 */
TInt CTSgResInternalSecondProcessTestHandler::TestDriverInitializeAndShutdownManyTimes()
	{
	TInt result = 0;
	__UHEAP_MARK;
	
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageBitOpenVgImage;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	
	RSgImage image;
	
	if(KErrNone == image.Create(info, KCrossImageData, KCrossImageDataStride))
		result |= EFirstTestPassed;
	
	image.Close();
	
	iSgDriver.Close();
	iSgDriver.Close();
	
	if(KErrNone == iSgDriver.Open())
		result |= ESecondTestPassed;
	
	if(KErrNone == image.Create(info, KCrossImageData, KCrossImageDataStride))
		result |= EThirdTestPassed;
	
	image.Close();
	
	if(0 == iProfExt->LocalResourceCount())
		result |= EFourthTestPassed;
	
	iSgDriver.Close();
	
	__UHEAP_MARKEND;
	
	return result;
	}

/**
Test the SgDriver extension MSgDriver_Profiling is reporting the correct local and 
global memory usage and resource counts, when another process has created images 
and then called into this process.
 */
TInt CTSgResInternalSecondProcessTestHandler::TestResourceProfiling(const TSgResIntTestInfo& aInfo)
	{
	__UHEAP_MARK;
	TInt result = 0;
	const TSize KImageSize(8, 8);
	
	// Check that this process is reporting the same memory usage as the calling
	// process, and is using zero local memory.
	if (iProfExt->GlobalGraphicsMemoryUsed() == aInfo.iGlobalGraphicsMemory)
		result |= EFirstTestPassed;
	if (iProfExt->LocalGraphicsMemoryUsed() == 0)
		result |= ESecondTestPassed;
	if (iProfExt->GlobalResourceCount() == aInfo.iGlobalResourceCount)
		result |= EThirdTestPassed;
	
	RSgImage image;
	if (KErrNone == image.Create(TSgImageInfo(KImageSize, ESgPixelFormatARGB_8888, ESgUsageBitOpenVgImage)))
		{
		// Check that the local resource count is one, and the global resource count has
		// incremented by one.
		if (iProfExt->LocalResourceCount() == 1)
			result |=  EFourthTestPassed;
		if (iProfExt->GlobalResourceCount() == aInfo.iGlobalResourceCount+1)
			result |= EFifthTestPassed;
		
		// Check that creating an image in this process increases the global and
		// local memory usage reported by the extension, and destroying it will
		// set it back to how it was.
		TInt localGraphicsMemory = iProfExt->LocalGraphicsMemoryUsed();
		TInt globalGraphicsMemory = iProfExt->GlobalGraphicsMemoryUsed();
		if (localGraphicsMemory >= (KImageSize.iWidth * KImageSize.iHeight * 4))
			result |= ESixthTestPassed;
		if (globalGraphicsMemory == (localGraphicsMemory + aInfo.iGlobalGraphicsMemory))
			result |= ESeventhTestPassed;
		
		image.Close();
		
		// Check the local memory usage is the same as before the test started
		if (iProfExt->LocalGraphicsMemoryUsed() == 0)
			result |= EEighthTestPassed;
		if (iProfExt->GlobalGraphicsMemoryUsed() == aInfo.iGlobalGraphicsMemory)
			result |= ENinthTestPassed;		
		// Check the local resource count is zero and the global count is the same
		// as before the test started.
		if (iProfExt->LocalResourceCount() == 0)
			result |= ETenthTestPassed;
		if (iProfExt->GlobalResourceCount() == aInfo.iGlobalResourceCount)
			result |= EEleventhTestPassed;
		}
	
	__UHEAP_MARKEND;
	return result;
	}
