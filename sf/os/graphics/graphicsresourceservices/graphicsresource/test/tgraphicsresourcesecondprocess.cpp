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

#include "tgraphicsresourcemultiprocessthread.h"
#include <e32debug.h>
#include <graphics/sgimage_sw.h>

/**
Helper function to test the equivalence of two TSgImageInfo structures.

@param   aInfo1 A TSgImageInfo structure to compare.
@param   aInfo2 A TSgImageInfo structure to compare.

@return  ETrue if the two are identical, EFalse otherwise.
*/
TBool CompareInfos(TSgImageInfo& aInfo1, TSgImageInfo& aInfo2)
	{
	TBool result = EFalse;
	if(aInfo1.iCpuAccess == aInfo2.iCpuAccess 
		&& aInfo1.iPixelFormat == aInfo2.iPixelFormat
		&& aInfo1.iScreenId == aInfo2.iScreenId
		&& aInfo1.iShareable == aInfo2.iShareable
		&& aInfo1.iSizeInPixels == aInfo2.iSizeInPixels
		&& aInfo1.iUsage | aInfo2.iUsage
		&& aInfo1.iUserAttributeCount == aInfo2.iUserAttributeCount)
		{
		for(TInt i=0; i<aInfo1.iUserAttributeCount; ++i)
			{
			if(aInfo1.iUserAttributes[i].iUid != aInfo2.iUserAttributes[i].iUid)
				{
				break;
				}
			}
		result = ETrue;
		}
	return result;
	}

/**
Opens an image in a different process.
@param aInfo The test information passed from outside the current thread
@return The test result indicating which tests passed
*/
TInt TestOpenImageL(TSgresTestInfo& aInfo)
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
	if(KErrNone == image.GetInfo(imageInfo2))
		{
		result |= ESecondTestPassed;
		}
	if(CompareInfos(imageInfo1, imageInfo2))
		{
		result |= EThirdTestPassed;
		}
	if(image.Id() == id)
		{
		result |= EFourthTestPassed;
		}
	
	image.Close();
	
	return result;
	}

/**
Opens an image in a different process into a RSgDrawable object.
@param aInfo The test information passed from outside the current thread
@return The test result indicating which tests passed
*/
TInt TestOpenDrawableL(TSgresTestInfo& aInfo)
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
TInt TestOpenImageInvalidL(TSgresTestInfo& aInfo)
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
	TSgDrawableId fakeid = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
	if(KErrNotFound == image.Open(fakeid))
		{
		result |= EFourthTestPassed;
		}
	image.Close();
	
	//  open a non-sharable image
	TInt openResult = image.Open(id);
	if(KErrPermissionDenied == openResult || KErrNotFound == openResult)
		{
		result |= EFifthTestPassed;
		}
	image.Close();
	
	return result;
	}

/**
Calls SgDriver::AllocMarkEnd() without closing all the resources.
@param aInfo The test information passed from outside the current thread
@panic SGRES-ADAPTER 0
@return The test result indicating which tests passed
*/
TInt TestDriverMemoryLeakL(TSgresTestInfo& /*aInfo*/)
	{
	SgDriver::AllocMarkStart();
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iCpuAccess = ESgCpuAccessReadWrite;
	info.iShareable = EFalse;
	info.iScreenId = 0;
	
	RSgImage image;
	image.Create(info, NULL, 0);
	SgDriver::AllocMarkEnd(0); //should panic here
	image.Close();
	return KErrNone;
	}

/**
Shuts down the driver when there are still resources open.
@param aInfo The test information passed from outside the current thread
@panic SGRES 1
@return The test result indicating which tests passed
*/
TInt TestShutdownMemoryLeakL(TSgresTestInfo& /*aInfo*/)
	{
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iCpuAccess = ESgCpuAccessReadWrite;
	info.iShareable = EFalse;
	info.iScreenId = 0;
	
	RSgImage image;
	TInt ret = image.Create(info, NULL, 0);
	if(KErrNone != ret)
		{
		return ret;
		}		
	SgDriver::Close(); //should panic here
	return KErrNone;
	}

/**
Opens a drawable in a different process with different invalid operations.
@param aInfo The test information passed from outside the current thread
@return The test result indicating which tests passed
*/
TInt TestOpenDrawableInvalidL(TSgresTestInfo& aInfo)
	{	
	TSgDrawableId id = aInfo.iDrawableId;
	
	RSgDrawable  drawable;
	TInt result = 0;
	
	//null drawable id	
	if(KErrArgument == drawable.Open(KSgNullDrawableId))
		{
		result |= EFirstTestPassed;
		}
	drawable.Close();
	
	//non-existing drawable id
	TSgDrawableId fakeid = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
	if(KErrNotFound == drawable.Open(fakeid))
		{
		result |= ESecondTestPassed;
		}
	drawable.Close();
	
	//open a non-sharable image - should fail
	TInt openResult = drawable.Open(id);
	if(KErrPermissionDenied == openResult || KErrNotFound == openResult)
		{
		result |= EThirdTestPassed;
		}
	drawable.Close();
	
	//non-empty handle
	//create an image
	TSgImageInfo info1;
	info1.iSizeInPixels = TSize(8, 8);
	info1.iUsage = ESgUsageDirectGdiTarget;
	info1.iPixelFormat = EUidPixelFormatRGB_565;
	info1.iCpuAccess = ESgCpuAccessNone;
	info1.iShareable = ETrue;
	
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
			result |= EFourthTestPassed;
			}		
		
		//handle is non-null now
		if(KErrInUse == drawable.Open(id))
			{
			result |= EFifthTestPassed;
			}
		drawable.Close();
		}
	image.Close();
	return result;
	}

/**
Maps an image in a different process.
The operation should fail.
@param aInfo The test information passed from outside the current thread
@return The test result indicating which tests passed
*/
TInt TestMapImageNonOwnerL(TSgresTestInfo& aInfo)
	{
	TSgDrawableId id = aInfo.iDrawableId;
	
	RSgImage image;
	const TAny* dataAddressRead;
	TAny* dataAddressWrite;
	TInt dataStride;
	TInt result = 0;
	
	if(KErrNone == image.Open(id))
		{
		result |= EFirstTestPassed;
		}
	if(KErrPermissionDenied == image.MapReadOnly(dataAddressRead, dataStride))
		{
		result |= ESecondTestPassed;
		}
	if(KErrPermissionDenied == image.MapWriteOnly(dataAddressWrite, dataStride))
		{
		result |= EThirdTestPassed;
		}
	if(KErrPermissionDenied == image.MapReadWrite(dataAddressWrite, dataStride))
		{
		result |= EFourthTestPassed;
		}

	TInt unmapResult = image.Unmap();
	if(KErrGeneral == unmapResult || KErrPermissionDenied == unmapResult)
		{
		result |= EFifthTestPassed;
		}
	
	image.Close();
	return result;
	}

/**
Unmaps an image in another process.
@param aInfo The test information passed from outside the current thread
@return The test result indicating which tests passed
*/
TInt TestUnmapImageL(TSgresTestInfo& aInfo)
	{
	TSgDrawableId id = aInfo.iDrawableId;
	
	RSgImage image;
	TInt result = 0;
	
	if(KErrNone == image.Open(id))
		{
		result |= EFirstTestPassed;
		}
	if(KErrGeneral == image.Unmap())
		{
		result |= ESecondTestPassed;
		}
	image.Close();
	return result;
	}

// Real main function
TInt MainL()
	{				
	TInt procHandles1  =0;
	TInt threadHandles1=0;
	RThread().HandleCount(procHandles1, threadHandles1);
	
	TPckgBuf<TSgresTestInfo> infoPkg;
	User::LeaveIfError(User::GetDesParameter(KSecondProcessParametersSlot, infoPkg));
	TSgresTestInfo& info = infoPkg();
	TSgresTestCase testCase = info.iTestCase;
	TInt result = 0;

	if(KErrNone == SgDriver::Open())
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
				result = TestOpenDrawableInvalidL(info);
				break;
			case ESgresSecondProcessMapImage:
				result = TestMapImageNonOwnerL(info);
				break;
			case ESgresSecondProcessUnmapImage:
				result = TestUnmapImageL(info);
				break;
			case ESgresSecondProcessPanicDriverUnclosedResources:
				result = TestShutdownMemoryLeakL(info);
				break;
			case ESgresSecondProcessPanicMemoryLeak:
				result = TestDriverMemoryLeakL(info);
				break;
			}
		}	
	
	SgDriver::Close();
	
	// Handle check
	TInt procHandles2  =0;
	TInt threadHandles2=0;  
	RThread().HandleCount(procHandles2,threadHandles2);
	if (threadHandles1 != threadHandles2)
		{
		result = KErrBadHandle;  // Thread-owned handles not closed
		}
	
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
