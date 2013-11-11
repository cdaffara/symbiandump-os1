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

#include "tgraphicsresourceteststepbase.h"
#include <e32math.h>

CTSgTestStepBase::CTSgTestStepBase()
	{
	}

CTSgTestStepBase::~CTSgTestStepBase()
	{
	iSecondThread.Close();
	}

/**
Overrides of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTSgTestStepBase::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTSgTestStepBase::doTestStepPostambleL()
	{
	return TestStepResult();
	}

/**
Override of function from CTestStep so that each test failure is reported in output.
*/
EXPORT_C void CTSgTestStepBase::testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine) 
	{
	TRAP_IGNORE(testBooleanTrueL(aCondition, aFile, aLine, ETrue));
	}

/**
Creates an image with some predefined parametres.
@param aImage output image handle
@leave Gets system wide error code
*/
void CTSgTestStepBase::CreateImageL(RSgImage& aImage)
	{
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iCpuAccess = ESgCpuAccessReadWrite;
	info.iShareable = ETrue;
	
	CheckErrorL(KErrNone, aImage.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	}

/**
Creates an image collection with some predefined parametres.
@param aCollection output image collection handle
@leave Gets system wide error code
*/
void CTSgTestStepBase::CreateImageCollectionL(RSgImageCollection& aCollection)
	{
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iCpuAccess = ESgCpuAccessReadWrite;
	CheckErrorL(KErrNone, aCollection.Create(info, KNumImagesInCollection), (TText8*)__FILE__, __LINE__);
	TEST(!aCollection.IsNull());
	}

/**
Creates a second process and do some tests in it.
@param aProcessName The name of the new process
@param aTestInfo The information for the tests
@leave Gets system wide error code
*/
TInt CTSgTestStepBase::CreateSecondProcessAndDoTestL(const TDesC &aProcessName, TSgresTestInfo& aTestInfo)
	{
	// Create a second process
    RProcess process;
    User::LeaveIfError(process.Create(aProcessName, KNullDesC));
	CleanupClosePushL(process);

	// Specify the test for the second process
	TESTL(KErrNone == process.SetParameter(KSecondProcessFunctionSlot, ((TSgresTestInfo)aTestInfo).iTestCase));
	// Specify the id passed to the second process
	TPckg<TSgresTestInfo> ptr(aTestInfo);
	
	TESTL(KErrNone == process.SetParameter(KSecondProcessParametersSlot, ptr));
	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	TRequestStatus status = KRequestPending;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	
	TInt exitreason = process.ExitReason();
	
	CleanupStack::PopAndDestroy();
	
	//return test result
	return exitreason;
	}

/**
Creates a second thread and do some tests in it.
@param aTestInfo The information for the tests
@leave Gets system wide error code
*/
TInt CTSgTestStepBase::CreateSecondThreadAndDoTestL(TSgresTestInfo aTestInfo)
	{
	//create a semaphore
	RSemaphore sem;
	User::LeaveIfError(sem.CreateGlobal(KSecondThreadSemaphore, 0, EOwnerThread));
	CleanupClosePushL(sem);
	
	User::LeaveIfError(iSecondThread.Create(KSecondThread, SecondThreadStart, KDefaultStackSize, &User::Heap(), &aTestInfo));
	// Launch second thread
	TRequestStatus statusSecondThread;
	iSecondThread.Logon(statusSecondThread);
	iSecondThread.SetPriority(EPriorityLess);
	iSecondThread.Resume();	
	
	User::WaitForRequest(statusSecondThread);
	
	TInt result = iSecondThread.ExitReason();
	
	//Close the handle
    CleanupStack::PopAndDestroy(1, &sem);
    iSecondThread.Close();
	
	return result;
	}

/**
Creates a second thread and do some panic tests in it.
@param aTestInfo The information for the tests
@param aPanicCode The expected panic code
@param aExitCategory The expected panic category
@param aThreadName The name of the new thread
@leave Gets system wide error code
*/
void CTSgTestStepBase::CreateSecondThreadAndCheckPanicL(TSgresTestInfo aTestInfo, TInt aPanicCode, TExitCategoryName aExitCategory, const TDesC &aThreadName)
	{
	User::LeaveIfError(iSecondThread.Create(aThreadName, SecondThreadStart, KDefaultStackSize, 0x100, 0x100, &aTestInfo));
	// Launch second thread
	TRequestStatus statusSecondThread;
	iSecondThread.Logon(statusSecondThread);
	iSecondThread.SetPriority(EPriorityLess);
	iSecondThread.Resume();	
	
	User::WaitForRequest(statusSecondThread);
	
	if(EExitPanic != iSecondThread.ExitType())
		{
		ERR_PRINTF3(_L("Expected exit type: %d, Actual exit type: %d"), EExitPanic, iSecondThread.ExitType());
		TEST(EFalse);
		}
	
	if(aPanicCode != iSecondThread.ExitReason())
		{
		ERR_PRINTF3(_L("Expected panic code: %d, Actual panic code: %d"), aPanicCode, iSecondThread.ExitReason());
		TEST(EFalse);
		}
	
	TExitCategoryName secondThreadExitCategory = iSecondThread.ExitCategory();
	if(aExitCategory != secondThreadExitCategory)
		{
		ERR_PRINTF3(_L("Expected panic category: %S, Actual panic category: %S"), &aExitCategory, &secondThreadExitCategory);
		TEST(EFalse);
		}
	
	 //close the driver if the second thread exited with type EExitKill
	 //assumming the second thread only calls SgDriver::Open() once
	 if(iSecondThread.ExitType() != EExitKill)
	   	{
	   	SgDriver::Close();
	   	}
	
	//Close the handle
    iSecondThread.Close();
	}

/**
Creates a second process and do some panic tests in it.
@param aTestInfo The information for the tests
@param aPanicCode The expected panic code
@param aExitCategory The expected panic category
@param aProcessName The name of the new process
@leave Gets system wide error code
*/
void CTSgTestStepBase::CreateSecondProcessAndCheckPanicL(TSgresTestInfo aTestInfo, TInt aPanicCode, TExitCategoryName aExitCategory, const TDesC &aProcessName)
	{
	// Create a second process
    RProcess process;
    User::LeaveIfError(process.Create(KSecondProcess, aProcessName));
	CleanupClosePushL(process);

	// Specify the test for the second process
	TESTL(KErrNone == process.SetParameter(KSecondProcessFunctionSlot, ((TSgresTestInfo)aTestInfo).iTestCase));
	// Specify the id passed to the second process
	TPckg<TSgresTestInfo> ptr(aTestInfo);
	
	TESTL(KErrNone == process.SetParameter(KSecondProcessParametersSlot, ptr));
	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	
	if(EExitPanic != process.ExitType())
		{
		ERR_PRINTF3(_L("Expected exit type: %d, Actual exit type: %d"), EExitPanic, process.ExitType());
		TEST(EFalse);
		}
	if(aPanicCode != process.ExitReason())
		{
		ERR_PRINTF3(_L("Expected panic code: %d, Actual panic code: %d"), aPanicCode, process.ExitReason());
		TEST(EFalse);
		}
	
	_LIT(KMemoryLeakCategory, "SGALLOC");
	TExitCategoryName processExitCategory = process.ExitCategory();
	TBool matchCategory;
	if (aExitCategory != KMemoryLeakCategory)
		{
		matchCategory = aExitCategory == processExitCategory;
		}
	else
		{
		matchCategory = processExitCategory.Left(KMemoryLeakCategory().Length()) == KMemoryLeakCategory;
		}
	if (!matchCategory)
		{
		ERR_PRINTF3(_L("Expected panic category: %S, Actual panic category: %S"), &aExitCategory, &processExitCategory);
		TEST(EFalse);
		}
	
	CleanupStack::PopAndDestroy();
	}

/**
Gets the supporting pixel formats according to the specified cpu access, usage, shareability and screen id flags.
@leave Gets system wide error code
*/
void CTSgTestStepBase::CallGetPixelFormatsL(TSgCpuAccess aCpuAccess, TUint32 aUsage, TBool aShareable, TInt aScreenId)
	{
	TSgImageInfo info;
	info.iCpuAccess = aCpuAccess;
	info.iUsage = aUsage;
	info.iShareable = aShareable;
	info.iScreenId = aScreenId;
	info.iSizeInPixels = TSize(10, 10);
	iPixelFormatCount = KMaxPixelFormats;
	Mem::FillZ(iPixelFormatArray, KMaxPixelFormats * sizeof(TUidPixelFormat));
	CheckErrorL(KErrNone, RSgImage::GetPixelFormats(info, iPixelFormatArray, iPixelFormatCount), (TText8*)__FILE__, __LINE__);
	}

/**
Checks the pixel formats returned against the compatibility guarantee table.
@leave Gets system wide error code
*/
void CTSgTestStepBase::TestGetPixelFormatCompatibilityGuaranteesL()
	{
	CallGetPixelFormatsL(ESgCpuAccessNone, ESgUsageDirectGdiSource, ETrue, KSgScreenIdAny);
	CheckPixelFormatPresent(EUidPixelFormatARGB_8888_PRE);
	CheckPixelFormatPresent(EUidPixelFormatXRGB_8888);
	CheckPixelFormatPresent(EUidPixelFormatRGB_565);

	CallGetPixelFormatsL(ESgCpuAccessReadWrite, ESgUsageDirectGdiSource, ETrue, KSgScreenIdMain);
	CheckPixelFormatPresent(EUidPixelFormatARGB_8888_PRE);
	CheckPixelFormatPresent(EUidPixelFormatXRGB_8888);
	CheckPixelFormatPresent(EUidPixelFormatRGB_565);
	
	CallGetPixelFormatsL(ESgCpuAccessNone, ESgUsageDirectGdiSource|ESgUsageDirectGdiTarget, ETrue, KSgScreenIdMain);
	CheckPixelFormatPresent(EUidPixelFormatARGB_8888_PRE);
	CheckPixelFormatPresent(EUidPixelFormatXRGB_8888);	
	
	CallGetPixelFormatsL(ESgCpuAccessNone, ESgUsageDirectGdiSource|ESgUsageOpenGlesTarget, ETrue, KSgScreenIdMain);
	CheckPixelFormatPresent(EUidPixelFormatARGB_8888_PRE);
	CheckPixelFormatPresent(EUidPixelFormatXRGB_8888);
	
	CallGetPixelFormatsL(ESgCpuAccessNone, ESgUsageDirectGdiSource|ESgUsageOpenVgTarget, ETrue, KSgScreenIdMain);
	CheckPixelFormatPresent(EUidPixelFormatARGB_8888_PRE);
	CheckPixelFormatPresent(EUidPixelFormatXRGB_8888);
	
	CallGetPixelFormatsL(ESgCpuAccessNone, ESgUsageDirectGdiSource|ESgUsageOpenGles2Target, ETrue, KSgScreenIdMain);
	CheckPixelFormatPresent(EUidPixelFormatARGB_8888_PRE);
	CheckPixelFormatPresent(EUidPixelFormatXRGB_8888);
	}

/**
Helper function to check if a certain pixel format is present
in the returned pixel formats array.
@param aPixelFormat The pixelformat to check
*/
void CTSgTestStepBase::CheckPixelFormatPresent(TUidPixelFormat aPixelFormat)
	{
	for(TInt i=0; i<iPixelFormatCount; ++i)
		{		
		if(aPixelFormat == iPixelFormatArray[i])
			{
			return;
			}
		}
	TEST(EFalse);
	}

/**
Helper function to test the equivalence of two TSgImageInfo structures.

@see     CTDirectGdiContextTarget::CompareInfos
@param   aInfo1 A TSgImageInfo structure to compare.
@param   aInfo2 A TSgImageInfo structure to compare.

@return  ETrue if the two are identical, EFalse otherwise.
*/
TBool CTSgTestStepBase::CompareInfos(TSgImageInfo& aInfo1, TSgImageInfo& aInfo2)
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
Wrapper function to open the graphics resource driver and puts an cleanup item
on the cleanup stack.
@leave Gets system wide error code
*/
void CTSgTestStepBase::TestOpenDriverL()
	{
	CheckErrorL(KErrNone, SgDriver::Open(), (TText8*)__FILE__, __LINE__);
	CleanupStack::PushL(TCleanupItem((TCleanupOperation)CloseDriverWhenLeave, NULL));
	}

/**
Wrapper function to close the graphics resource driver.
*/
void CTSgTestStepBase::TestCloseDriver()
	{
	CleanupStack::PopAndDestroy();
	}

/**
Creates an image with specified parametres.
@param aImage output image handle
@return KErrNone if the image was created successfully, otherwise one of the system-wide error codes
*/
TInt CreateImageWithParameters(TInt aWidth, TInt aHeight, TUidPixelFormat aPixelFormat, TUint32 aUsage, TBool aShareable, TSgCpuAccess aCpuAccess, RSgImage& aImage)
	{
	TSgImageInfo info;
	info.iSizeInPixels = TSize(aWidth, aHeight);
	info.iPixelFormat = aPixelFormat;
	info.iUsage = aUsage;
	info.iCpuAccess = aCpuAccess;
	info.iShareable = aShareable;
	return aImage.Create(info, NULL, 0);
	}

/**
Creates an image collection with some predefined parametres.
@param aCollection output image collection handle
@return KErrNone if the image was created successfully, otherwise one of the system-wide error codes
*/
TInt CreateImageCollectionWithParameters(TInt aWidth, TInt aHeight, TUidPixelFormat aPixelFormat, TUint32 aUsage, TBool aShareable, TSgCpuAccess aCpuAccess, RSgImageCollection& aCollection)
	{
	TSgImageInfo info;
	info.iSizeInPixels = TSize(aWidth, aHeight);
	info.iPixelFormat = aPixelFormat;
	info.iUsage = aUsage;
	info.iCpuAccess = aCpuAccess;
	info.iShareable = aShareable;
	return aCollection.Create(info, KNumImagesInCollection);
	}


/**
Second thread entry function.
*/
TInt CTSgTestStepBase::SecondThreadStart(TAny* aInfo)
	{
	TInt procHandles1  =0;
	TInt threadHandles1=0;
	RThread().HandleCount(procHandles1, threadHandles1);
	__UHEAP_MARK;
	
	RSemaphore sem;
	TInt result = 0;
	
	TInt openSem = sem.OpenGlobal(KSecondThreadSemaphore, EOwnerThread);
	
	TSgresTestCase testcase = ((TSgresTestInfo*)aInfo)->iTestCase;
	
	RSgImage image;
	RSgDrawable drawable;
	TSgImageInfo info1;
	TSgImageInfo info2;
	TSgDrawableId id;
	const TAny* dataAddressRead;
	TAny* dataAddressWrite;
	TInt dataStride;
	TSgDrawableId fakeid = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

	//test cases without the need of an initialised driver
	switch (testcase)
	{
	case ESgresSecondThreadPanicResourceCountNoDriver:
		{
		SgDriver::ResourceCount(); // should panic with SGRES 5
		}
		break;
	case ESgresSecondThreadPanicAllocMarkStartNoDriver:
		{
		SgDriver::AllocMarkStart(); // should panic with SGRES 5
		}
		break;
	case ESgresSecondThreadPanicAllocMarkEndNoDriver:
		{
		SgDriver::AllocMarkEnd(0); // should panic with SGRES 5
		}
		break;	
	case ESgresSecondThreadPanicSetAllocFailNoDriver:
		{
		SgDriver::SetAllocFail(RAllocator::EFailNext, 1); // should panic with SGRES 5
		}
		break;	
	case ESgresSecondThreadPanicDrawableOpenNoDriver:
		{
		drawable.Open(KSgNullDrawableId); // should panic with SGRES 5
		}
		break;
	case ESgresSecondThreadPanicImageOpenNoDriver1:
		{
		image.Open(KSgNullDrawableId); // should panic with SGRES 5
		}
		break;
	case ESgresSecondThreadPanicImageOpenNoDriver2:
		{
		image.Open(KSgNullDrawableId, ESgDoNotRestrictUsage); // should panic with SGRES 5
		}
		break;		
	case ESgresSecondThreadPanicImageCreateNoDriver1:
		{
		image.Create(info1, NULL, 0); // should panic with SGRES 5
		}
		break;
	case ESgresSecondThreadPanicImageCreateNoDriver2:
		{
		RSgImage tempImage;
		image.Create(info1, tempImage); // should panic with SGRES 5
		}
		break;
	case ESgresSecondThreadPanicImageGetPixelFormatsNoDriver:
		{
		TInt count = 0;
		RSgImage::GetPixelFormats(info1, NULL, count); // should panic with SGRES 5
		}
		break;
	case ESgresSecondThreadPanicImageCollectionCreateNoDriver1:
		{
		RSgImageCollection c;
		c.Create(info1, 0); // should panic with SGRES 5
		}
		break;
	case ESgresSecondThreadPanicImageCollectionCreateNoDriver2:
		{
		RSgImageCollection c;
		c.Create(NULL, 0, NULL, 0); // should panic with SGRES 5
		}
		break;
	}
	
	
	//open driver
	TInt ret = SgDriver::Open();
	if(KErrNoMemory == ret)
		{
		sem.Close();
		return KErrNoMemory;
		}
	if(KErrNone == ret)
		{			
		switch (testcase)
			{
			case ESgresSecondThreadOpenImage:
				{
				TInt err = image.Open(((TSgresTestInfo*)aInfo)->iDrawableId);
				if(KErrNoMemory == err)
					{
					result = KErrNoMemory;
					break;
					}
				if(KErrNone == err)
					{
					result |= EFirstTestPassed;
					}
				if(KErrNone == image.GetInfo(info2))
					{
					result |= ESecondTestPassed;
					}
				info1 = ((TSgresTestInfo*)aInfo)->iImageInfo;
				if(CompareInfos(info1, info2))
					{
					result |= EThirdTestPassed;
					}
				id = image.Id();
				if(id != KSgNullDrawableId)
					{
					result |= EFourthTestPassed;
					}
				if(id == ((TSgresTestInfo*)aInfo)->iDrawableId)
					{
					result |= EFifthTestPassed;
					}
				}					
				break;
			case ESgresSecondThreadOpenDrawable:
				{
				TInt err = drawable.Open(((TSgresTestInfo*)aInfo)->iDrawableId);
				if(KErrNoMemory == err)
					{
					result = KErrNoMemory;
					break;
					}
				if(KErrNone == err)
					{
					result |= EFirstTestPassed;
					}
				id = drawable.Id();
				if(id != KSgNullDrawableId)
					{
					result |= ESecondTestPassed;
					}
				if(id == ((TSgresTestInfo*)aInfo)->iDrawableId)
					{
					result |= EThirdTestPassed;
					}
				}
				break;
			case ESgresSecondThreadOpenImageInvalid:
				{
				TSgImageInfo info;
				info.iSizeInPixels = TSize(8, 8);
				info.iUsage = ESgUsageDirectGdiSource;
				info.iPixelFormat = EUidPixelFormatRGB_565;
				info.iCpuAccess = ESgCpuAccessReadWrite;
				
				TInt err = image.Create(info, KImageData, 16);
				if(KErrNoMemory == err)
					{
					result = KErrNoMemory;
					break;
					}
				if(KErrNone == err)
					{
					result |= EFirstTestPassed;
					}
				
				//  non-empty handle
				id = ((TSgresTestInfo*)aInfo)->iDrawableId;
				err = image.Open(id);
				if(KErrNoMemory == err)
					{
					result = KErrNoMemory;
					break;
					}
				if(KErrInUse == err)
					{
					result |= ESecondTestPassed;
					}
				image.Close();
				
				//  null drawable id	
				err = image.Open(KSgNullDrawableId);
				if(KErrNoMemory == err)
					{
					result = KErrNoMemory;
					break;
					}
				if(KErrArgument == err)
					{
					result |= EThirdTestPassed;
					}
				image.Close();
				
				//  non-existing drawable id
				err = image.Open(fakeid);
				if(KErrNoMemory == err)
					{
					result = KErrNoMemory;
					break;
					}
				if(KErrNotFound == err)
					{
					result |= EFourthTestPassed;
					}
				image.Close();
				
				//  open a non-sharable image
				err = image.Open(id);
				if(KErrNoMemory == err)
					{
					result = KErrNoMemory;
					break;
					}
				if(KErrNone == err)
					{
					result |= EFifthTestPassed;
					}
				image.Close();
				}
				break;
			case ESgresSecondThreadOpenDrawableInvalid:	
				//  null drawable id	
				{
				TInt err = drawable.Open(KSgNullDrawableId);
				if(KErrNoMemory == err)
					{
					result = KErrNoMemory;
					break;
					}
				if(KErrArgument == err)
					{
					result |= EFirstTestPassed;
					}
				drawable.Close();
				
				//  non-existing drawable id
				err = drawable.Open(fakeid);
				if(KErrNoMemory == err)
					{
					result = KErrNoMemory;
					break;
					}
				
				if(KErrNotFound == err)
					{
					result |= ESecondTestPassed;
					}
				drawable.Close();
				
				//  open a non-sharable image - should succeed
				id = ((TSgresTestInfo*)aInfo)->iDrawableId;
				err = drawable.Open(id);
				if(KErrNoMemory == err)
					{
					result = KErrNoMemory;
					break;
					}
				if(KErrNone == err)
					{
					result |= EThirdTestPassed;
					}
									
				//  non-empty handle
				if(KErrInUse == drawable.Open(id))
					{
					result |= EFourthTestPassed;
					}
				drawable.Close();
				}
				break;
			case ESgresSecondThreadMapImage:
				{
				id = ((TSgresTestInfo*)aInfo)->iDrawableId;
				TInt err = image.Open(id);		
				if(KErrNoMemory == err)
					{
					result = KErrNoMemory;
					break;
					}
				if(KErrNone == err)
					{
					result |= EFirstTestPassed;
					}
				if(KErrNone == image.MapReadOnly(dataAddressRead, dataStride))
					{
					result |= ESecondTestPassed;
					}
				if(KErrNone == image.Unmap())
					{
					result |= EThirdTestPassed;
					}
				if(KErrNone == image.MapWriteOnly(dataAddressWrite, dataStride))
					{
					result |= EFourthTestPassed;
					}
				if(KErrNone == image.Unmap())
					{
					result |= EFifthTestPassed;
					}
				if(KErrNone == image.MapReadWrite(dataAddressWrite, dataStride))
					{
					result |= ESixthTestPassed;
					}
				if(KErrNone == image.Unmap())
					{
					result |= ESeventhTestPassed;
					}
				}					
				break;
			case ESgresSecondThreadUnmapImage:
				{
				id = ((TSgresTestInfo*)aInfo)->iDrawableId;
				TInt err = image.Open(id);
				if(KErrNoMemory == err)
					{
					result = KErrNoMemory;
					break;
					}
				if(KErrNone == err)
					{
					result |= EFirstTestPassed;
					}
				if(KErrNone == image.Unmap())
					{
					result |= ESecondTestPassed;
					}
				}					
				break;
			case ESgresSecondThreadPanicImageGetInterfaceInvalidHandle:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				
				MSgImage_Sw* swInterface = NULL;
				ret = anotherImage.GetInterface(swInterface); //should panic with SGRES 2
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageGetInterfaceNoDriver:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				
				SgDriver::Close();
				MSgImage_Sw* swInterface = NULL;
				anotherImage.GetInterface(swInterface); // should panic with SGRES 5
				}
				break;
			case ESgresSecondThreadPanicImageCloseInvalidHandle:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				
				anotherImage.Close(); //should panic with SGRES 2
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageCloseNoDriver:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				SgDriver::Close();
				anotherImage.Close(); // should panic with SGRES 5					
				}
				break;
			case ESgresSecondThreadPanicImageIdInvalidHandle:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				
				anotherImage.Id(); //should panic with SGRES 2
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageIdNoDriver:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				SgDriver::Close();					
				anotherImage.Id(); // should panic with SGRES 5
				}
				break;
			case ESgresSecondThreadPanicImageDrawableTypeInvalidHandle:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				
				anotherImage.DrawableType(); //should panic with SGRES 2
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageDrawableTypeNoDriver:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				SgDriver::Close();					
				anotherImage.DrawableType(); // should panic with SGRES 5
				}
				break;	
			case ESgresSecondThreadPanicImageCreateInvalidHandle:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				
				RSgImage newImage;
				TSgImageInfo info;
				image.GetInfo(info);
				newImage.Create(info, anotherImage); //should panic with SGRES 3
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageGetInfoInvalidHandle:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				
				TSgImageInfo anotherInfo;
				anotherImage.GetInfo(anotherInfo); //should panic with SGRES 3
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageGetInfoNoDriver:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				SgDriver::Close();
				TSgImageInfo anotherInfo;
				anotherImage.GetInfo(anotherInfo); // should panic with SGRES 5
				}
				break;
			case ESgresSecondThreadPanicImageMapReadOnlyInvalidHandle:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				
				anotherImage.MapReadOnly(dataAddressRead, dataStride); //should panic with SGRES 3
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageMapReadOnlyNoDriver:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				SgDriver::Close();
				anotherImage.MapReadOnly(dataAddressRead, dataStride); // should panic with SGRES 5
				}
				break;
			case ESgresSecondThreadPanicImageMapWriteOnlyInvalidHandle:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				
				anotherImage.MapWriteOnly(dataAddressWrite, dataStride); //should panic with SGRES 3
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageMapWriteOnlyNoDriver:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				SgDriver::Close();					
				anotherImage.MapWriteOnly(dataAddressWrite, dataStride); // should panic with SGRES 5
				}
				break;
			case ESgresSecondThreadPanicImageMapReadWriteInvalidHandle:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				
				anotherImage.MapReadWrite(dataAddressWrite, dataStride); //should panic with SGRES 3
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageMapReadWriteNoDriver:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				SgDriver::Close();					
				anotherImage.MapReadWrite(dataAddressWrite, dataStride); // should panic with SGRES 5
				}
				break;
			case ESgresSecondThreadPanicImageUnmapInvalidHandle:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				
				anotherImage.Unmap(); //should panic with SGRES 3
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageUnmapNoDriver:
				{
				RSgImage image;	
				TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}					
				RSgImage anotherImage;
				anotherImage = image;
				image.Close();
				SgDriver::Close();
				anotherImage.Unmap(); // should panic with SGRES 5
				}
				break;
			case ESgresSecondThreadPanicImageCollectionCloseInvalidHandle:
				{
				RSgImageCollection collection;
				TInt ret = CreateImageCollectionWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, EFalse, ESgCpuAccessReadWrite, collection);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}
				
				RSgImageCollection anotherCollection;
				anotherCollection = collection;
				collection.Close();
				anotherCollection.Close(); //should panic with SGRES 4
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageCollectionCloseNoDriver:
				{
				RSgImageCollection collection;
				TInt ret = CreateImageCollectionWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, EFalse, ESgCpuAccessReadWrite, collection);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}
				
				RSgImageCollection anotherCollection;
				anotherCollection = collection;
				collection.Close();
				SgDriver::Close();
				anotherCollection.Close(); // should panic with SGRES 5
				}
				break;
			case ESgresSecondThreadPanicImageCollectionSurfaceIdInvalidHandle:
				{
				RSgImageCollection collection;
				TInt ret = CreateImageCollectionWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, EFalse, ESgCpuAccessReadWrite, collection);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}
				
				RSgImageCollection anotherCollection;
				anotherCollection = collection;
				collection.Close();
				anotherCollection.SurfaceId(); //should panic with SGRES 4
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageCollectionSurfaceIdNoDriver:
				{
				RSgImageCollection collection;
				TInt ret = CreateImageCollectionWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, EFalse, ESgCpuAccessReadWrite, collection);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}
				
				RSgImageCollection anotherCollection;
				anotherCollection = collection;
				collection.Close();
				SgDriver::Close();
				anotherCollection.SurfaceId(); // should panic with SGRES 5
				}
				break;
			case ESgresSecondThreadPanicImageCollectionGetInfoInvalidHandle:
				{
				RSgImageCollection collection;
				TInt ret = CreateImageCollectionWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, EFalse, ESgCpuAccessReadWrite, collection);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}
				
				RSgImageCollection anotherCollection;
				anotherCollection = collection;
				collection.Close();
				TSgImageInfo anotherInfo;
				anotherCollection.GetInfo(anotherInfo); //should panic with SGRES 4
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageCollectionGetInfoNoDriver:
				{
				RSgImageCollection collection;
				TInt ret = CreateImageCollectionWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, EFalse, ESgCpuAccessReadWrite, collection);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}
				
				RSgImageCollection anotherCollection;
				anotherCollection = collection;
				collection.Close();
				TSgImageInfo anotherInfo;
				SgDriver::Close();
				anotherCollection.GetInfo(anotherInfo); // should panic with SGRES 5
				}
				break;
			case ESgresSecondThreadPanicImageCollectionCountInvalidHandle:
				{
				RSgImageCollection collection;
				TInt ret = CreateImageCollectionWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, EFalse, ESgCpuAccessReadWrite, collection);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}
				
				RSgImageCollection anotherCollection;
				anotherCollection = collection;
				collection.Close();
				anotherCollection.Count(); //should panic with SGRES 4
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageCollectionCountNoDriver:
				{
				RSgImageCollection collection;
				TInt ret = CreateImageCollectionWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, EFalse, ESgCpuAccessReadWrite, collection);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}
				
				RSgImageCollection anotherCollection;
				anotherCollection = collection;
				collection.Close();
				SgDriver::Close();
				anotherCollection.Count(); // should panic with SGRES 5
				}
				break;
			case ESgresSecondThreadPanicImageCollectionOpenImageInvalidHandle:
				{
				RSgImageCollection collection;
				TInt ret = CreateImageCollectionWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, EFalse, ESgCpuAccessReadWrite, collection);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}
				
				RSgImageCollection anotherCollection;
				anotherCollection = collection;
				collection.Close();
				RSgImage image;
				anotherCollection.OpenImage(0, image); //should panic with SGRES 4
				SgDriver::Close();
				}
				break;
			case ESgresSecondThreadPanicImageCollectionOpenImageNoDriver:
				{
				RSgImageCollection collection;
				TInt ret = CreateImageCollectionWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, EFalse, ESgCpuAccessReadWrite, collection);
				if(KErrNone != ret)
					{
					result = ret;
					break;
					}
				
				RSgImageCollection anotherCollection;
				anotherCollection = collection;
				collection.Close();
				RSgImage image;
				SgDriver::Close();
				anotherCollection.OpenImage(0, image); // should panic with SGRES 5
				}
				break;
			case ESgresMultipleThreadStressTest:
				{
				for (TInt i = 0; i < 1000 && result == KErrNone; ++i)
					{
					TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageDirectGdiSource, ETrue, ESgCpuAccessReadWrite, image);
					if (KErrNone != ret)
						{
						result = ret;
						break;
						}
					const TInt KMaxOpenCount = 100;
					RSgImage images[KMaxOpenCount];
					TInt count = Math::Random() % KMaxOpenCount;
					for (TInt k = 0; k < count; ++k)
						{
						ret = images[k].Open(((TSgresTestInfo*)aInfo)->iDrawableId);
						if (KErrNone != ret)
							{
							result = ret;
							break;
							}
						}
					image.Close();
					for (TInt k = 0; k < count; ++k)
						{
						images[k].Close();
						}
					}
				}
				break;
			};
		}
	image.Close();
	drawable.Close();
	SgDriver::Close();
	if (KErrNone == openSem)
		{
		sem.Signal();
		}
	__UHEAP_MARKEND;
	sem.Close();	
	TInt procHandles2  =0;
	TInt threadHandles2=0;
	RThread().HandleCount(procHandles2,threadHandles2);
	if (threadHandles1 != threadHandles2)
		{
		result = KErrGeneral;  // Thread-owned handles not closed
		}

	return result;
	}

/**
Static function used by the cleanup item to close the driver.
*/
void CTSgTestStepBase::CloseDriverWhenLeave(TAny* /*aInfo*/)
	{
	SgDriver::Close();
	}

/**
Checks the function for the passed error codes and logs an error if the codes do not match. 
If the test is running out of memory tests, KErrNoMemory is also an expected error code and 
the function would just leave with KErrNoMemory in that case.

@param aExpectedErrorCode The expected error code to check against
@param aActualErrorCode The actual error code
@param aFile The filename to use when reporting the error
@param aLine The line number to use when reporting the error
*/
void CTSgTestStepBase::CheckErrorL(TInt aExpectedErrorCode, TInt aActualErrorCode, const TText8* aFile, TInt aLine)
	{
	if(iRunningOomTests && KErrNoMemory == aActualErrorCode)
		{
		User::Leave(KErrNoMemory);
		}
	TESTWITHFILENAMEANDLINENUMBERL(aExpectedErrorCode == aActualErrorCode, aFile, aLine);
	}

/**
Out of memory tests.
*/
void CTSgTestStepBase::TestOOM()
	{
	SgDriver::Open();
	TInt err = KErrNone;
	TInt tryCount = 0;
	iRunningOomTests = ETrue;
	do
		{
		SgDriver::SetAllocFail(RAllocator::EFailNext, ++tryCount);
		TRAP(err, DoMemoryTestsL());
		} 
	while(err == KErrNoMemory);
	
	SgDriver::SetAllocFail(RAllocator::ENone, 0);
	iRunningOomTests = EFalse;
	SgDriver::Close();
	INFO_PRINTF2(_L("- server succeeded at private heap failure rate of %i\n"), tryCount);
	}

/**
Specifies which functions to run in out of memory conditions.
To be overridden by the derived test classes.
*/
void CTSgTestStepBase::DoMemoryTestsL()
	{
	}
