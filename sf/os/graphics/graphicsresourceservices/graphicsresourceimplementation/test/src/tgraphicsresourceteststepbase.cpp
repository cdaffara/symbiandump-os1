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
 @internalComponent - Graphics Resource API Conformance Test Suite
*/

#include "tgraphicsresourceteststepbase.h"
#include <e32math.h>

CTSgTestStepBase::CTSgTestStepBase(TBool aConformanceTests) :
	iEnableConformanceTests(aConformanceTests)
	{
	}

CTSgTestStepBase::~CTSgTestStepBase()
	{
	iSecondThread.Close();
	iPixelFormatArray.Close();
	User::Free(iDiagonalImageData);
	iSgDriver.Close();
	}

/**
Overrides of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTSgTestStepBase::doTestStepPreambleL()
	{
	// Set the logger to shared so that secondary threads can write to the log
	User::LeaveIfError(Logger().ShareAuto());	
	SetTestStepResult(EPass);
	iDiagonalImageData = static_cast<TUint32*>(User::AllocL(KDiagonalImageSize * KDiagonalImageDataStride));
	// Populate iDiagonalImageData with green diagonal from top-left to bottom-right on white background
	Mem::Fill(iDiagonalImageData, KDiagonalImageSize * KDiagonalImageDataStride, 0xFF);
	for (TInt i = 0; i < KDiagonalImageSize; ++i)
		{
		iDiagonalImageData[i * (KDiagonalImageSize + 1)] = 0xFF00FF00;
		}
	return TestStepResult();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTSgTestStepBase::doTestStepPostambleL()
	{
	User::Free(iDiagonalImageData);
	iDiagonalImageData = NULL;
	return TestStepResult();
	}

/**
Creates an image with some predefined parameters.
@param aImage output image handle
@leave Gets system wide error code
*/
void CTSgTestStepBase::CreateImageL(RSgImage& aImage)
	{
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageBitOpenVgImage;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	
	CheckErrorL(KErrNone, aImage.Create(info, KCrossImageData, KCrossImageDataStride), (TText8*)__FILE__, __LINE__);
	}

/**
Creates a second process and do some tests in it.
@param aProcessName The name of the new process
@param aTestInfo The information for the tests
@return A bitwise mask of test passes (never an error code)
@leave Gets system wide error code
*/
TInt CTSgTestStepBase::CreateSecondProcessAndDoTestL(const TDesC &aProcessName, TSgProcessTestInfo& aTestInfo)
	{
	// Create a second process
	RProcess process;
	TInt err = process.Create(aProcessName, KNullDesC);
	User::LeaveIfError(err);
	CleanupClosePushL(process);

	// Specify the id passed to the second process
	TPckg<TSgProcessTestInfo> ptr(aTestInfo);
	TESTL(KErrNone == process.SetParameter(KSecondProcessParametersSlot, ptr));
	
	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	TRequestStatus status = KRequestPending;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);

	// exitreason could either be a negative error code or a bitwise
	// mask of test passes.
	TInt exitreason = process.ExitReason();
	
	CleanupStack::PopAndDestroy();
	
	if (exitreason < KErrNone)
		{
		INFO_PRINTF2(_L("The second process returned error code %d"), exitreason);
		TEST(EFalse);
		exitreason = 0;
		}
	//return test result
	return exitreason;
	}

/**
Creates a second process, runs the requested test and ensures that
the specified panic occurs.

@param aTestInfo The specification for this test
@param aPanicCode The expected panic code
@param aExitCategory The expected panic category
@param aProcessName The name of the new process

@leave One of the system wide error codes
*/
void CTSgTestStepBase::CreateSecondProcessAndCheckPanicL(TSgProcessTestInfo& aTestInfo, TInt aPanicCode, TExitCategoryName aExitCategory, const TDesC &aProcessName)
	{
	// Create a second process
	RProcess process;
	User::LeaveIfError(process.Create(aProcessName, KNullDesC));
	CleanupClosePushL(process);

	// Specify the id passed to the second process
	TPckg<TSgProcessTestInfo> ptr(aTestInfo);
	TESTL(KErrNone == process.SetParameter(KSecondProcessParametersSlot, ptr));
	
	// Kick off the second process and wait for it to complete
	// The actual testing is done in the second process
	TRequestStatus status = KRequestPending;
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
	
	TExitCategoryName secondProcessExitCategory = process.ExitCategory();
	if(aExitCategory != secondProcessExitCategory)
		{
		ERR_PRINTF3(_L("Expected panic category: %S, Actual panic category: %S"), &aExitCategory, &secondProcessExitCategory);
		TEST(EFalse);
		}
	
	CleanupStack::PopAndDestroy();
	}

/**
Creates a second thread and do some tests in it.
@param aTestInfo The information for the tests
@leave Gets system wide error code
*/
TInt CTSgTestStepBase::CreateSecondThreadAndDoTestL(TSgThreadTestInfo aTestInfo)
	{
	//create a semaphore
	RSemaphore sem;
	User::LeaveIfError(sem.CreateGlobal(KSecondThreadSemaphore, 0, EOwnerThread));
	CleanupClosePushL(sem);
	
	aTestInfo.iTestStep = this;
	
	User::LeaveIfError(iSecondThread.Create(KSecondThread, SgTestSecondThread::ThreadStart, KDefaultStackSize, KSecondThreadMinHeapSize, KSecondThreadMaxHeapSize, &aTestInfo));
	// Launch second thread
	TRequestStatus statusSecondThread;
	iSecondThread.Logon(statusSecondThread);
	iSecondThread.SetPriority(EPriorityLess);
	iSecondThread.Resume();	
	
	User::WaitForRequest(statusSecondThread);
	
	TExitCategoryName exitCategory = iSecondThread.ExitCategory();
	if (exitCategory.Compare(_L("Kill")) != 0)
		{
		ERR_PRINTF2(_L("Second thread terminated with reason category '%S'"), &exitCategory);
		SetTestStepResult(EFail);
		}
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
void CTSgTestStepBase::CreateSecondThreadAndCheckPanicL(TSgThreadTestInfo aTestInfo, TInt aPanicCode, TExitCategoryName aExitCategory, const TDesC &aThreadName)
	{
	aTestInfo.iTestStep = this;
	User::LeaveIfError(iSecondThread.Create(aThreadName, SgTestSecondThread::ThreadStart, KDefaultStackSize, KSecondThreadMinHeapSize, KSecondThreadMaxHeapSize, &aTestInfo));
	
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
	
	//Close the handle
	iSecondThread.Close();
	}

/**
Gets the supporting pixel formats according to the specified usage bit.
@leave Gets system wide error code
*/
void CTSgTestStepBase::CallGetPixelFormatsL(TUint32 aUsage)
	{
	iPixelFormatArray.Reset();
	CheckErrorL(KErrNone, RSgImage::GetPixelFormats(aUsage, iPixelFormatArray), (TText8*)__FILE__, __LINE__);
	}

/**
Checks the pixel formats returned against the compatibility guarantee table.
@leave Gets system wide error code
*/
void CTSgTestStepBase::TestGetPixelFormatCompatibilityGuaranteesL()
	{
	_LIT(KLibOpenGLES2, "libGLESv2.dll");
	
	RLibrary lib;
	TBool supportGLES2 = (lib.Load(KLibOpenGLES2) == KErrNone);
	lib.Close();

	// OpenVG - Mandatory support
	CallGetPixelFormatsL(ESgUsageBitOpenVgImage);
	TEST(CheckPixelFormatPresent(ESgPixelFormatARGB_8888_PRE));
	TEST(CheckPixelFormatPresent(ESgPixelFormatARGB_8888));
	TEST(CheckPixelFormatPresent(ESgPixelFormatXRGB_8888));
	TEST(CheckPixelFormatPresent(ESgPixelFormatRGB_565));
	TEST(CheckPixelFormatPresent(ESgPixelFormatA_8));
	CallGetPixelFormatsL(ESgUsageBitOpenVgSurface);
	TEST(CheckPixelFormatPresent(ESgPixelFormatARGB_8888_PRE));
	TEST(CheckPixelFormatPresent(ESgPixelFormatXRGB_8888));
	TEST(CheckPixelFormatPresent(ESgPixelFormatRGB_565));
	
	// OpenGL ES - Not mandatory, but should cause no errors. 
	CallGetPixelFormatsL(ESgUsageBitOpenGlesTexture2D);
	CallGetPixelFormatsL(ESgUsageBitOpenGlesSurface);

	// OpenGL ES 2 - Mandatory if present.
	CallGetPixelFormatsL(ESgUsageBitOpenGles2Texture2D);
	if (supportGLES2)
	    {
        TEST(CheckPixelFormatPresent(ESgPixelFormatARGB_8888_PRE));
        TEST(CheckPixelFormatPresent(ESgPixelFormatARGB_8888));
        TEST(CheckPixelFormatPresent(ESgPixelFormatXRGB_8888));
        TEST(CheckPixelFormatPresent(ESgPixelFormatRGB_565));
        TEST(CheckPixelFormatPresent(ESgPixelFormatA_8));
	    }
	CallGetPixelFormatsL(ESgUsageBitOpenGles2Surface);
	if (supportGLES2)
	    {
	    TEST(CheckPixelFormatPresent(ESgPixelFormatARGB_8888_PRE));
	    TEST(CheckPixelFormatPresent(ESgPixelFormatXRGB_8888));
	    TEST(CheckPixelFormatPresent(ESgPixelFormatRGB_565));
	    }

	iPixelFormatArray.Reset();
	}

/**
Helper function to check if a certain pixel format is present
in the pixel formats array retrieved by CallGetPixelFormatsL().

@param aPixelFormat The pixelformat to check
@return ETrue if the pixel format is present, otherwise EFalse
@see CTsgTestStepBase::CallGetPixelFormats()
*/
TBool CTSgTestStepBase::CheckPixelFormatPresent(TSgPixelFormat aPixelFormat)
	{
	for(TInt i=0; i<iPixelFormatArray.Count(); ++i)
		{		
		if(aPixelFormat == iPixelFormatArray[i])
			{
			return ETrue;
			}
		}
	return EFalse;
	}

/**
Helper function to test the equivalence of two TSgImageInfo structures.

@param   aInfo1 A TSgImageInfo structure to compare.
@param   aInfo2 A TSgImageInfo structure to compare (may have extra usage bits).

@return  ETrue if the two are equivalent, EFalse otherwise.
*/
TBool CTSgTestStepBase::CompareInfos(TSgImageInfo& aInfo1, TSgImageInfo& aInfo2)
	{
	return (aInfo1.iPixelFormat == aInfo2.iPixelFormat
		&& aInfo1.iSizeInPixels == aInfo2.iSizeInPixels
		// check that all requested usage bits are set in the returned usage bits
		&& !(aInfo1.iUsage & ~aInfo2.iUsage));
	}

/**
Wrapper function to open the graphics resource driver.
Only opens the driver if the test step has been constructed for use
as the conformance tests.

@leave One of the system wide error codes
*/
void CTSgTestStepBase::TestOpenDriverL()
	{
	if (iEnableConformanceTests)
		CheckErrorL(KErrNone, iSgDriver.Open(), (TText8*)__FILE__, __LINE__);
	}

/**
Wrapper function to close the graphics resource driver.

Only closes the driver if the test step has been constructed for use
as the conformance tests.
*/
void CTSgTestStepBase::TestCloseDriver()
	{
	if (iEnableConformanceTests)
		iSgDriver.Close();
	}

/**
Creates an image with specified parameters.
@param aImage output image handle
@return KErrNone if the image was created successfully, otherwise one of the system-wide error codes
*/
TInt CreateImageWithParameters(TInt aWidth, TInt aHeight, TUidPixelFormat aPixelFormat, TUint32 aUsage, RSgImage& aImage)
	{
	TSgImageInfo info;
	info.iSizeInPixels = TSize(aWidth, aHeight);
	info.iPixelFormat = aPixelFormat;
	info.iUsage = aUsage;
	return aImage.Create(info, NULL, 0);
	}

/**
Second thread entry function.
*/
TInt SgTestSecondThread::ThreadStart(TAny* aInfo)
	{
	__UHEAP_MARK;
	
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (!cleanupStack)
		{
		return KErrNoMemory;
		}
	
	TSgThreadTestInfo* testInfo = static_cast<TSgThreadTestInfo*>(aInfo);

	RSemaphore sem;
	TInt openSem = sem.OpenGlobal(KSecondThreadSemaphore, EOwnerThread);

	TInt result = KErrNone;
	TRAPD(leaveCode, result = SgTestSecondThread::ThreadMainL(testInfo));
	if (KErrNone == openSem)
		{
		sem.Signal();
		}
	sem.Close();
	
	if (leaveCode != KErrNone)
		{
		testInfo->iTestStep->ERR_PRINTF2(_L("Second thread caused Leave (%d)"), leaveCode);
		testInfo->iTestStep->SetTestStepResult(EFail);
		result = leaveCode;
		}
	
	delete cleanupStack;

	__UHEAP_MARKEND;
	
	return result;
	}

/**
Run the test contained within the TSgresTestInfo object. A new thread is
created for each test and only one of the cases in the switch statements
below will be used.

The first switch statement contains tests that must have no driver open,
the second set require an open driver to initiate the test but may
close it part way through.

@param aInfo The parameters for the test
@return One of the system wide error codes or an enumeration of passed tests.
 */
TInt SgTestSecondThread::ThreadMainL(TSgThreadTestInfo* aInfo)
	{
	TInt result = 0;

	TSgresTestCase testcase = ((TSgThreadTestInfo*)aInfo)->iTestCase;

	RSgDriver driver;
	CleanupClosePushL(driver);
	TInt ret = driver.Open();
	if(KErrNoMemory == ret)
		{
		return KErrNoMemory;
		}
	//test cases without the need of an initialised driver
	switch (testcase)
	{
	case ESgresSecondThreadPanicDrawableOpenNoDriver:
		{
		driver.Close();
		RSgDrawable drawable;
		drawable.Open(KFakeSgDrawableId); // should panic with SGRES 1
		}
		break;
	case ESgresSecondThreadPanicImageOpenNoDriver:
		{
		driver.Close();
		RSgImage image;
		image.Open(KFakeSgDrawableId); // should panic with SGRES 1
		image.Close();
		}
	case ESgresSecondThreadPanicImageCreateNoDriver1:
		{
		driver.Close();
		RSgImage image;
		image.Create(KSgImageInfo1, NULL, 0); // should panic with SGRES 1
		image.Close();
		}
		break;
	case ESgresSecondThreadPanicImageCreateNoDriver2:
		{
		driver.Close();
		RSgImage image;
		RSgImage tempImage;
		image.Create(KSgImageInfo1, tempImage); // should panic with SGRES 1
		image.Close();
		}
		break;
	case ESgresSecondThreadOpenImage:
		result = OpenImage(((TSgThreadTestInfo*)aInfo), driver);
		break;
	case ESgresSecondThreadOpenDrawable:
		result = OpenDrawable(((TSgThreadTestInfo*)aInfo));
		break;
	case ESgresSecondThreadOpenImageInvalid:
		result = OpenImageInvalid(((TSgThreadTestInfo*)aInfo));
		break;
	case ESgresSecondThreadOpenDrawableInvalid: 
		result = OpenDrawableInvalid(((TSgThreadTestInfo*)aInfo));
		break;
	case ESgresSecondThreadPanicImageGetInterfaceInvalidHandle:
		result = PanicImageGetInterfaceInvalidHandle(driver);
		break;
	case ESgresSecondThreadPanicImageGetInterfaceNoDriver:
		result = PanicImageGetInterfaceNoDriver(driver);
		break;
	case ESgresSecondThreadPanicImageCloseInvalidHandle:
		result = PanicImageCloseInvalidHandle(driver);
		break;
	case ESgresSecondThreadPanicImageCloseNoDriver:
		result = PanicImageCloseNoDriver(driver);
		break;
	case ESgresSecondThreadPanicImageIdInvalidHandle:
		result = PanicImageIdInvalidHandle(driver);
		break;
	case ESgresSecondThreadPanicImageIdNoDriver:
		result = PanicImageIdNoDriver(driver);
		break;
	case ESgresSecondThreadPanicImageDrawableTypeInvalidHandle:
		result = PanicImageDrawableTypeInvalidHandle(driver);
		break;
	case ESgresSecondThreadPanicImageDrawableTypeNoDriver:
		result = PanicImageDrawableTypeNoDriver(driver);
		break;  
	case ESgresSecondThreadPanicImageCreateInvalidHandle:
		result = PanicImageCreateInvalidHandle(driver);
		break;
	case ESgresSecondThreadPanicImageGetInfoInvalidHandle:
		result = PanicImageGetInfoInvalidHandle(driver);
		break;
	case ESgresSecondThreadPanicImageGetInfoNoDriver:
		result = PanicImageGetInfoNoDriver(driver);
		break;
	case ESgresSecondThreadPanicImageGetAttributeInvalidHandle:
		result = PanicImageGetAttributeInvalidHandle(driver);
		break;
	case ESgresSecondThreadPanicImageGetAttributeNoDriver:
		result = PanicImageGetAttributeNoDriver(driver);
		break;
	case ESgresMultipleThreadStressTest:
		result = MultipleThreadStressTest(((TSgThreadTestInfo*)aInfo));
		break;
	case ESgresSecondThreadPanicAttributeArrayInvalidIndex:
		PanicAttributeArrayInvalidIndexL();
		break;
	case ESgresSecondThreadPanicAttributeArrayInvalidIndex2:
		PanicAttributeArrayInvalidIndex2L();
		break;
	case ESgresSecondThreadOpenPassedDriver:
		result = ((TSgThreadTestInfo*)aInfo)->iSgDriver->Open();
		break;
	case ESgresSecondThreadClosePassedDriver:
		((TSgThreadTestInfo*)aInfo)->iSgDriver->Close();
		break;
	case ESgresSecondThreadCreatePassedImage:
		result = CreatePassedImageL(((TSgThreadTestInfo*)aInfo)->iSgImage);
		break;
	case ESgresSecondThreadOpenPassedImage:
		((TSgThreadTestInfo*)aInfo)->iSgImage->Open(((TSgThreadTestInfo*)aInfo)->iDrawableId);
		break;
	case ESgresSecondThreadClosePassedImage:
		((TSgThreadTestInfo*)aInfo)->iSgImage->Close();
		break;
	};

	CleanupStack::PopAndDestroy(&driver);

	return result;
	}

/**
Checks the function for the passed error codes and logs an error if the codes do not match. 
If run under OOM testing, this function will leave if KErrNoMemory or KErrNoGraphicsMemory was 
the actual error; this is essential for OOM testing.

@leave One of the System Wide Error Codes

@param aExpectedErrorCode The expected error code to check against
@param aActualErrorCode The actual error code
@param aFile The filename to use when reporting the error
@param aLine The line number to use when reporting the error
*/
void CTSgTestStepBase::CheckErrorL(TInt aExpectedErrorCode, TInt aActualErrorCode, const TText8* aFile, TInt aLine)
	{
	if (iEnableConformanceTests)
		{
		TESTWITHFILENAMEANDLINENUMBERL(aExpectedErrorCode == aActualErrorCode, aFile, aLine);
		}
	else
		{
		//OOM Tests Enabled - Also test for KErrNoMemory/KErrNoGraphicsMemory
		TESTWITHFILENAMEANDLINENUMBERL((aExpectedErrorCode == aActualErrorCode) || (KErrNoMemory == aActualErrorCode) || (KErrNoGraphicsMemory == aActualErrorCode), aFile, aLine);
		if (aActualErrorCode == KErrNoMemory || aActualErrorCode == KErrNoGraphicsMemory)
			{
			User::Leave(aActualErrorCode);
			}
		}
	}
/**
Implementation of SecondThread test ESgresSecondThreadOpenImage

@return One of the system wide error codes or an enumeration of passed tests.

@param TSgresTestInfo* The test parameters
@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::OpenImage(TSgThreadTestInfo* aInfo, RSgDriver& aSgDriver)
	{
	RSgImage image;
	TInt result = 0;
	TInt err = image.Open(aInfo->iDrawableId);
	if(KErrNoMemory == err)
		{
		return KErrNoMemory;
		}
	if(KErrNone == err)
		{
		result |= EFirstTestPassed;
		}
	TSgImageInfo info;
	if(KErrNone == image.GetInfo(info))
		{
		result |= ESecondTestPassed;
		}
	if(CTSgTestStepBase::CompareInfos(aInfo->iImageInfo, info))
		{
		result |= EThirdTestPassed;
		}
	TSgDrawableId id = image.Id();
	if(id != KSgNullDrawableId)
		{
		result |= EFourthTestPassed;
		}
	if(id == aInfo->iDrawableId)
		{
		result |= EFifthTestPassed;
		}
	TInt attribVal = KMaxTInt;
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
	aSgDriver.Close();
	if (KErrBadHandle == image.GetAttribute(uid, attribVal))
		{
		result |= EEighthTestPassed;
		}
	
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadOpenDrawable

@return One of the system wide error codes or an enumeration of passed tests.

@param TSgresTestInfo* The test parameters
 */
TInt SgTestSecondThread::OpenDrawable(TSgThreadTestInfo* aInfo)
	{
	TInt result = 0;
	RSgDrawable drawable;
	TInt err = drawable.Open(aInfo->iDrawableId);
	if(KErrNoMemory == err)
		{
		result = KErrNoMemory;
		return result;
		}
	if(KErrNone == err)
		{
		result |= EFirstTestPassed;
		}
	TSgDrawableId id = drawable.Id();
	if(id != KSgNullDrawableId)
		{
		result |= ESecondTestPassed;
		}
	if(id == aInfo->iDrawableId)
		{
		result |= EThirdTestPassed;
		}
	drawable.Close();
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadOpenImageInvalid

@return One of the system wide error codes or an enumeration of passed tests.

@param TSgresTestInfo* The test parameters
 */
TInt SgTestSecondThread::OpenImageInvalid(TSgThreadTestInfo* aInfo)
	{
	TInt result = 0;
	RSgImage image;
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iUsage = ESgUsageBitOpenVgImage;
	
	TInt err = image.Create(info, KCrossImageData, KCrossImageDataStride);
	if(KErrNoMemory == err)
		{
		return KErrNoMemory;
		}
	if(KErrNone == err)
		{
		result |= EFirstTestPassed;
		}
	
	//  non-empty handle
	TSgDrawableId id = aInfo->iDrawableId;
	err = image.Open(id);
	if(KErrNoMemory == err)
		{
		return KErrNoMemory;
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
		return KErrNoMemory;
		}
	if(KErrArgument == err)
		{
		result |= EThirdTestPassed;
		}
	image.Close();
	
	//  non-existing drawable id
	err = image.Open(KFakeSgDrawableId);
	if(KErrNoMemory == err)
		{
		return KErrNoMemory;
		}
	if(KErrNotFound == err)
		{
		result |= EFourthTestPassed;
		}
	image.Close();
	
	err = image.Open(id);
	if(KErrNoMemory == err)
		{
		return KErrNoMemory;
		}
	if(KErrNone == err)
		{
		result |= EFifthTestPassed;
		}
	image.Close();
	
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadOpenDrawableInvalid

@return One of the system wide error codes or an enumeration of passed tests.

@param TSgresTestInfo* The test parameters
*/
TInt SgTestSecondThread::OpenDrawableInvalid(TSgThreadTestInfo* aInfo)
	{
	TInt result = 0;
	RSgDrawable drawable;
	TInt err = drawable.Open(KSgNullDrawableId);
	if(KErrNoMemory == err)
		{
	    return KErrNoMemory;
		}
	if(KErrArgument == err)
		{
	    result |= EFirstTestPassed;
	    }
	drawable.Close();
	
	//  non-existing drawable id
	err = drawable.Open(KFakeSgDrawableId);
	if (KErrNoMemory == err)
		{
		return KErrNoMemory;
		}
	
	if(KErrNotFound == err)
		{
		result |= ESecondTestPassed;
		}
	drawable.Close();
	
	//  open a non-sharable image - should succeed
	err = drawable.Open(aInfo->iDrawableId);
	
	if(KErrNoMemory == err)
		{
		return KErrNoMemory;
		}
	if(KErrNone == err)
		{
		result |= EThirdTestPassed;
		
		if (KErrInUse == drawable.Open(aInfo->iDrawableId))
			{
			result |= EFourthTestPassed;
			}
		}

	drawable.Close();
	
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageGetInterfaceInvalidHandle

@panic SGRES2 If the test is successful

@return One of the system wide error codes.

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageGetInterfaceInvalidHandle(RSgDriver& aSgDriver)
	{
	RSgImage image;
	TInt result = CTSgTestStepBase::CreateImageAndReturnCopy(image); 

	MTSgImage_Interface* swInterface = NULL;
	result = image.GetInterface(swInterface); //should panic with SGRES 3
	aSgDriver.Close();
	
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageGetInterfaceNoDriver

@panic SGRES1 If the test is successful

@return One of the system wide error codes

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageGetInterfaceNoDriver(RSgDriver& aSgDriver)
	{
	RSgImage image;
	TInt result = CTSgTestStepBase::CreateImageAndReturnCopy(image);
	
	aSgDriver.Close();
	MTSgImage_Interface* swInterface = NULL;
	image.GetInterface(swInterface); // should panic with SGRES 1
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageCloseInvalidHandle

@panic SGRES2 If the test is successful

@return One of the system wide error codes

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageCloseInvalidHandle(RSgDriver& aSgDriver)
	{
	RSgImage image;
	TInt result = CTSgTestStepBase::CreateImageAndReturnCopy(image);
	
	image.Close(); //should panic with SGRES 3
	aSgDriver.Close();
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageCloseNoDriver

@panic SGRES1 If the test is successful

@return One of the system wide error codes

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageCloseNoDriver(RSgDriver& aSgDriver)
	{
	RSgImage image;
	TInt result = CTSgTestStepBase::CreateImageAndReturnCopy(image);
	aSgDriver.Close();
	image.Close(); // should panic with SGRES 1 
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageIdIvalidHandle

@panic SGRES2 If the test is successful

@return One of the system wide error codes

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageIdInvalidHandle(RSgDriver& aSgDriver)
	{
	RSgImage image;
	TInt result = CTSgTestStepBase::CreateImageAndReturnCopy(image);
	
	image.Id(); //should panic with SGRES 3
	aSgDriver.Close();
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageIdNoDriver

@panic SGRES1 If the test is successful

@return One of the system wide error codes

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageIdNoDriver(RSgDriver& aSgDriver)
	{
	RSgImage image;
	TInt result = CTSgTestStepBase::CreateImageAndReturnCopy(image);

	aSgDriver.Close();
	image.Id(); // should panic with SGRES 1
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageDrawableTypeInvalidHandle

@panic SGRES2 If the test is successful

@return One of the system wide error codes

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageDrawableTypeInvalidHandle(RSgDriver& aSgDriver)
	{
	RSgImage image; 
	TInt result = CTSgTestStepBase::CreateImageAndReturnCopy(image);
	
	image.DrawableType(); //should panic with SGRES 3
	aSgDriver.Close();
	
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageDrawableTypeNoDriver

@panic SGRES1 If the test is successful

@return One of the system wide error codes

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageDrawableTypeNoDriver(RSgDriver& aSgDriver)
	{
	RSgImage image; 
	TInt result = CTSgTestStepBase::CreateImageAndReturnCopy(image);
	aSgDriver.Close();
	image.DrawableType(); // should panic with SGRES 1
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageCreateInvalidHandle

@panic SGRES3 If the test is successful

@return One of the system wide error codes

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageCreateInvalidHandle(RSgDriver& aSgDriver)
	{
	RSgImage image; 
	TInt result = CTSgTestStepBase::CreateImageAndReturnCopy(image);
	
	RSgImage newImage;
	TSgImageInfo info;
	image.GetInfo(info);
	newImage.Create(info, image); //should panic with SGRES 3
	aSgDriver.Close();
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageGetInfoInvalidHandle

@panic SGRES3 If the test is successful

@return One of the system wide error codes

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageGetInfoInvalidHandle(RSgDriver& aSgDriver)
	{
	RSgImage image; 
	TInt result = CTSgTestStepBase::CreateImageAndReturnCopy(image);
	
	TSgImageInfo anotherInfo;
	image.GetInfo(anotherInfo); //should panic with SGRES 3
	aSgDriver.Close();
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageNoDriver

@panic SGRES1 If the test is successful

@return One of the system wide error codes or an enumeration of passed tests.

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageGetInfoNoDriver(RSgDriver& aSgDriver)
	{
	RSgImage image; 
	TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageBitOpenVgImage, image);
	if(KErrNone != ret)
		{
		return ret;
		}
	RSgImage anotherImage;
	Mem::Copy(&anotherImage, &image, sizeof(image));
	image.Close();
	aSgDriver.Close();
	TSgImageInfo anotherInfo;
	anotherImage.GetInfo(anotherInfo); // should panic with SGRES 1
	return ret;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageGetAttributeInvalidHandle

@panic SGRES3 If the test is successful

@return One of the system wide error codes

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageGetAttributeInvalidHandle(RSgDriver& aSgDriver)
	{
	RSgImage image; 
	TInt result = CTSgTestStepBase::CreateImageAndReturnCopy(image);
	
	TInt attrib = KMaxTInt;
	image.GetAttribute(TUid::Uid(1), attrib); //Should panic with SGRES 3
	
	aSgDriver.Close();
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicImageGetAttributeNoDriver

@panic SGRES1 If the test is successful

@return One of the system wide error codes

@param RSgDriver An open RSgDriver for the test code to use
 */
TInt SgTestSecondThread::PanicImageGetAttributeNoDriver(RSgDriver& aSgDriver)
	{
	RSgImage image; 
	TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageBitOpenVgImage, image);
	if(KErrNone != ret)
		{
		return ret;
		}
	RSgImage anotherImage;
	Mem::Copy(&anotherImage, &image, sizeof(image));
	image.Close();
	aSgDriver.Close();
	TInt attrib = KMaxTInt;
	TUid uid = { 0x12345678 };
	anotherImage.GetAttribute(uid, attrib); //Should panic with SGRES 1
	return ret;
	}

/**
Implementation of SecondThread test ESgresSecondThreadMultipleThreadStressTest

@return One of the system wide error codes

@param TSgresTestInfo* The test parameters
 */
TInt SgTestSecondThread::MultipleThreadStressTest(TSgThreadTestInfo* aInfo)
	{
	TInt result = 0;
	RSgImage image;
	for (TInt i = 0; i < 1000 && result == KErrNone; ++i)
		{
		TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageBitOpenVgImage, image); 
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
			ret = images[k].Open(aInfo->iDrawableId);
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
	return result;
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicAttributeArrayInvalidIndex

@panic SGRES4 If the test is successful

@param TSgresTestInfo* The test parameters
 */
void SgTestSecondThread::PanicAttributeArrayInvalidIndexL()
	{
	TSgAttributeArray<5> attribArray;
	TSgAttribute attrib = attribArray[6]; //Should panic with SGRES 4
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicAttributeArrayInvalidIndex2

@panic SGRES4 If the test is successful

@param TSgresTestInfo* The test parameters
 */
void SgTestSecondThread::PanicAttributeArrayInvalidIndex2L()
	{
	TSgAttributeArray<5> attribArray;
	TSgAttribute attrib = attribArray[-1]; //Should panic with SGRES 4
	}

/**
Implementation of SecondThread test ESgresSecondThreadPanicDriverCloseOpenResources

@return One of the system wide error codes

@param RSgImage* A closed or uninitialised RSgImage to use for image creation 
 */
TInt SgTestSecondThread::CreatePassedImageL(RSgImage* aSgImage)
	{
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageBitOpenVgImage;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	
	return aSgImage->Create(info, KCrossImageData, KCrossImageDataStride);
	}

/**
Helper function which Creates an RSgImage and copies to into another RSgImage 
using Mem::Copy; the target of the copy is returned in the open state.

@param aSgImage An uninitialised image which will have an SgImage Mem::Copied into it.
@return One of the system wide error codes.
 */
TInt CTSgTestStepBase::CreateImageAndReturnCopy(RSgImage& aSgImage)
	{
	RSgImage image; 
	TInt ret = CreateImageWithParameters(8, 8, EUidPixelFormatRGB_565, ESgUsageBitOpenVgImage, image);
	if(KErrNone != ret)
		{
		return ret;
		}
	Mem::Copy(&aSgImage, &image, sizeof(image));
	image.Close();
	
	return ret;
	}
