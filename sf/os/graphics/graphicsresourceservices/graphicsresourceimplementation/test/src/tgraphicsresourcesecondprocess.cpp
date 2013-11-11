// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Graphics Resource API Conformance Test Suite
*/

#include <e32debug.h>
#include "tgraphicsresourcemultiprocessthread.h"
#include "tgraphicsresourceteststepbase.h"

/**
Helper function to test the equivalence of two TSgImageInfo structures.

@param   aInfo1 A TSgImageInfo structure to compare.
@param   aInfo2 A TSgImageInfo structure to compare.

@return  ETrue if the two are identical, EFalse otherwise.
*/
TBool CompareInfos(TSgImageInfo& aInfo1, TSgImageInfo& aInfo2)
	{
	TBool result = EFalse;
	if(aInfo1.iPixelFormat == aInfo2.iPixelFormat
		&& aInfo1.iSizeInPixels == aInfo2.iSizeInPixels
		&& aInfo1.iUsage == aInfo2.iUsage)
		{
		result = ETrue;
		}
	return result;
	}

/**
Opens an image in a different process.
@param aInfo The test information passed from outside the current thread
@return The test result indicating which tests passed
*/
TInt TestOpenImageL(TSgProcessTestInfo& aInfo)
	{
	TSgDrawableId id = aInfo.iDrawableId;
	TSgImageInfo imageInfo1 = aInfo.iImageInfo;
	
	RSgImage image;
	TInt result = 0;

	if(KErrNone == image.Open(id))
		{
		result |=  EFirstTestPassed;
		}
	
	TSgImageInfo imageInfo2;
	TInt attribVal = KMaxTInt;
	if(KErrNone == image.GetInfo(imageInfo2))
		{
		result |= ESecondTestPassed;
		}
	if(CompareInfos(imageInfo1, imageInfo2))
		{
		result |= EThirdTestPassed;
		}
	if (image.Id() != KSgNullDrawableId)
		{
		result |= EFourthTestPassed;
		}
	if(image.Id() == id)
		{
		result |= EFifthTestPassed;
		}
	TUid uid = { 0x12345678 };
	if (KErrNotSupported == image.GetAttribute(uid, attribVal))
	    {
	    result |= ESixthTestPassed;
	    }
	if (KErrArgument == image.GetAttribute(KNullUid, attribVal))
	    {
	    result |= ESeventhTestPassed;
	    }
	image.Close();

	return result;
	}

/**
Opens an image in a different process into a RSgDrawable object.
@param aInfo The test information passed from outside the current thread
@return The test result indicating which tests passed
*/
TInt TestOpenDrawableL(TSgProcessTestInfo& aInfo)
	{
	TSgDrawableId id = aInfo.iDrawableId;
	
	RSgDrawable drawable;
	TInt result = 0;
	
	if(KErrNone == drawable.Open(id))
		{
		result |=  EFirstTestPassed;
		}
	TSgDrawableId id2 = drawable.Id();
	if(id2 != KSgNullDrawableId)
		{
		result |= ESecondTestPassed;
		}
	if(id2 == id)
		{
		result |= EThirdTestPassed;
		}

	drawable.Close();
	return result;
	}

/**
Opens an image in a different process with different invalid operations.
@param aInfo The test information passed from outside the current thread
@return The test result indicating which tests passed
*/
TInt TestOpenImageInvalidL(TSgProcessTestInfo& aInfo)
	{	
	TSgDrawableId id = aInfo.iDrawableId;
	TSgImageInfo imageInfo = aInfo.iImageInfo;
	
	RSgImage image;
	TInt result = 0;
	
	//create image
	if(KErrNone == image.Create(imageInfo, NULL, 0))
		{
		result |= EFirstTestPassed;
		}
	//  non-empty handle
	if(KErrInUse == image.Open(id))
		{
		result |= ESecondTestPassed;
		}
	image.Close();
	
	//  null drawable id	
	if(KErrArgument == image.Open(KSgNullDrawableId))
		{
		result |= EThirdTestPassed;
		}
	image.Close();
	
	//  non-existing drawable id
	TSgDrawableId fakeid = {0xFFFFFFFFFFFFFFFFU};
	if(KErrNotFound == image.Open(fakeid))
		{
		result |= EFourthTestPassed;
		}
	image.Close();
	
	//Valid Drawable Id
	if (KErrNone == image.Open(id))
		{
		result |= EFifthTestPassed;
		}
	image.Close();
	return result;
	}

/**
Opens a drawable in a different process with different invalid operations.
@param aInfo The test information passed from outside the current thread
@return The test result indicating which tests passed
*/
TInt TestOpenDrawableInvalidL()
	{	
	RSgDrawable  drawable;
	TInt result = 0;
	
	//null drawable id	
	if(KErrArgument == drawable.Open(KSgNullDrawableId))
		{
		result |= EFirstTestPassed;
		}
	drawable.Close();
	
	//non-existing drawable id
	TSgDrawableId fakeid = {0xFFFFFFFFFFFFFFFFU};
	if(KErrNotFound == drawable.Open(fakeid))
		{
		result |= ESecondTestPassed;
		}
	drawable.Close();
		
	//non-empty handle
	//create an image
	TSgImageInfo info1;
	info1.iSizeInPixels = TSize(8, 8);
	info1.iUsage = ESgUsageBitOpenVgImage;
	info1.iPixelFormat = EUidPixelFormatRGB_565;

	
	RSgImage image;
	TInt err = image.Create(info1, NULL, 0);
	if(KErrNoMemory == err)
		{
		result = KErrNoMemory;
		}
	else if(KErrNone != err)
		{
		result = err;
		}
	else
		{		
		if(KErrNone == drawable.Open(image.Id()))
			{
			result |= EThirdTestPassed;
			
			if (KErrInUse == drawable.Open(image.Id()))
				{
				result |= EFourthTestPassed;
				}
			}		
		drawable.Close();
		}
	image.Close();
	return result;
	}

TInt TestCloseDriverOpenResources(RSgDriver& aDriver)
	{
	RSgImage image;
	TSgImageInfo info1;
	info1.iSizeInPixels = TSize(8, 8);
	info1.iUsage = ESgUsageBitOpenVgImage;
	info1.iPixelFormat = EUidPixelFormatRGB_565;
	
	TInt result = image.Create(info1, KCrossImageData, KCrossImageDataStride);
	
	if (result == KErrNone)
		{
		//Close the driver without closing the image.
		aDriver.Close(); //Should panic with SGRES2
		}
	
	return result;
	}

/**
Method executed by secondary thread for test TestOpenImageMulththreadedL
*/
_LIT(KTestOpenImageMultithreadedSem1, "TestOpenImageMultithreadedSem1");
_LIT(KTestOpenImageMultithreadedSem2, "TestOpenImageMultithreadedSem2");

TInt OpenImageMultiSecondThread(TAny* aAny)
	{
	TInt err = KErrNone;
	TSgProcessTestInfo* info = static_cast<TSgProcessTestInfo*>(aAny);	

	RSemaphore sem[2];
	err = sem[0].OpenGlobal(KTestOpenImageMultithreadedSem1, EOwnerThread);
	if (err != KErrNone)
		{
		return err;
		}
	err = sem[1].OpenGlobal(KTestOpenImageMultithreadedSem2, EOwnerThread);
	if (err != KErrNone)
		{
		sem[0].Close();
		return err;
		}

	RSgImage sgImage; 
	err = sgImage.Open(info->iDrawableId);

	sem[0].Signal();
	sem[1].Wait();

	sgImage.Close();	
	sem[0].Signal();
	return err;	
	}

/**
Creates a second thread which will initially open a handle to the passed TSgDrawableId.
The main thread then opens a new handle to the image.
The second thread will then close its handle to the image.
The main thread will then attempt to access the data of the image.
 */
TInt TestOpenImageMultithreadedL(TSgProcessTestInfo& aInfo)
	{
	TInt result = 0;
	
	//create two semaphores
	RSemaphore sem[2];
	User::LeaveIfError(sem[0].CreateGlobal(KTestOpenImageMultithreadedSem1, 0, EOwnerThread));
	CleanupClosePushL(sem[0]);
	User::LeaveIfError(sem[1].CreateGlobal(KTestOpenImageMultithreadedSem2, 0, EOwnerThread));
	CleanupClosePushL(sem[1]);
		
	//create secondary thread
	_LIT(KSecondaryThreadName, "TestOpenImageMultithreadedL");
	RThread thread;
	User::LeaveIfError(thread.Create(KSecondaryThreadName, OpenImageMultiSecondThread, KDefaultStackSize, KSecondThreadMinHeapSize, KSecondThreadMaxHeapSize, &aInfo));
	thread.Resume();
	
	// Make the second thread open the image before this thread.
	sem[0].Wait();

	// Second thread has opened image, now primary thread opens image
	RSgImage sgImage;
	TInt err = sgImage.Open(aInfo.iDrawableId);
	CleanupClosePushL(sgImage);
	sem[1].Signal();
	sem[0].Wait();
		
	// Second thread has closed image and terminated, now wait for thread to clean-up
	User::After(100000);

	if (err == KErrNone)
		{
		// Do something that requires data access of sgImage, in this case, creating a copy.
		result |= EFirstTestPassed;
		RSgImage sgImageCopy;
		err = sgImageCopy.Create(aInfo.iImageInfo, sgImage);
		sgImageCopy.Close();
		if (err == KErrNone)
			{
			result |= ESecondTestPassed;
			}	
		}
	
	CleanupStack::PopAndDestroy(3); // sgImage, sem[0], sem[1]		
	return result;	
	}

TInt MainL()
	{				
	TPckgBuf<TSgProcessTestInfo> infoPkg;
	User::LeaveIfError(User::GetDesParameter(KSecondProcessParametersSlot, infoPkg));
	TSgProcessTestInfo& info = infoPkg();
	TSgresTestCase testCase = info.iTestCase;
	TInt result = 0;

	RSgDriver sgDriver;
	CleanupClosePushL(sgDriver);
	
	if(KErrNone == sgDriver.Open())
		{
		switch(testCase)
			{
			case ESgresSecondProcessOpenImage:
				result = TestOpenImageL(info);
				break;
			case ESgresSecondProcessOpenDrawable:
				result = TestOpenDrawableL(info);
				break;
			case ESgresSecondProcessOpenImageInvalid:
				result = TestOpenImageInvalidL(info);
				break;
			case ESgresSecondProcessOpenDrawableInvalid:
				result = TestOpenDrawableInvalidL();
				break;
			case ESgresSecondProcessPanicDriverCloseOpenResources:
				result = TestCloseDriverOpenResources(sgDriver);
				break;
			case ESgresSecondProcessOpenImageMultithreaded:
				result = TestOpenImageMultithreadedL(info);
				break;
			}
		}	
	
	CleanupStack::PopAndDestroy(&sgDriver);
	
	return result;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if(cleanupStack == NULL)
		{
		return KErrNoMemory;
		}
	TInt ret = 0;
	TRAP_IGNORE(ret=MainL());
	delete cleanupStack;
	__UHEAP_MARKEND;
	return ret;
	}
