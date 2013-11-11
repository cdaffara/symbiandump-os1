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
#include "tsgdrawablegeneric.h"

/**
Construct the CTsgDrawableGeneric tests; set whether the tests are being used as conformance
tests or OOM tests.

@param TBool ETrue to run the conformance tests, EFalse to run the OOM tests.
 */
CTSgDrawableGeneric::CTSgDrawableGeneric(TBool aConformanceTests) :
	CTSgTestStepBase(aConformanceTests)
	{
	INFO_PRINTF1(_L("Graphics resource component test - RSgDrawable Generic Tests.\r\n"));
	}

CTSgDrawableGeneric::~CTSgDrawableGeneric()
	{
	}

/** 
Overrides of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/
TVerdict CTSgDrawableGeneric::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-RESOURCE-0013"));
	INFO_PRINTF1(_L("Opening an image in RSgDrawable.\r\n"));
	TestOpenImageAsDrawableL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0014"));
	INFO_PRINTF1(_L("Retrieving drawable id of an RSgDrawable object.\r\n"));
	TestGetDrawableDrawableIdL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0032"));
	INFO_PRINTF1(_L("RSgDrawable::Open() with invalid parameters.\r\n"));
	TestOpenDrawableInvalidL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0033"));
	INFO_PRINTF1(_L("Releasing reference to the image in RSgDrawable without first opening it.\r\n"));
	TestCloseDrawableWithoutOpenL();
	RecordTestResultL();
	
#ifdef _DEBUG
	SetTestStepID(_L("GRAPHICS-RESOURCE-0080"));	
	INFO_PRINTF1(_L("RSgDrawable generic panic test - Open() when the driver is not initialised.\r\n"));
	TestPanicDrawableOpenNoDriverL();
	RecordTestResultL();
#else
	INFO_PRINTF1(_L("Warning: Skipping the panic tests in release build. \r\n"));
#endif

	return TestStepResult();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0013
@SYMTestCaseDesc		Opens an RSgImage as an RSgDrawable.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Id()
						RSgDrawable::Open(const TSgDrawableId&)
@SYMTestPriority		High
@SYMTestType			UT
@SYMTestPurpose			To ensure an RSgDrawable object can reference an image.
@SYMTestActions			Initialise the graphics resource component. Create an image. Declare an
						RSgDrawable object. Call Open() on the object with the resource ID of the
						image. These tests are repeated in a seperate thread and seperate process.
@SYMTestExpectedResults	The function should return KErrNone. The RSgDrawable now references an
						image. The same should occur in the seperate thread and seperate process.
 */	
void CTSgDrawableGeneric::TestOpenImageAsDrawableL()
	{
	TestOpenDriverL();
	RSgImage image;
	CreateImageL(image);
	CleanupClosePushL(image);
	const TSgDrawableId id = image.Id();
	TSgImageInfo imageinfo;
	image.GetInfo(imageinfo);
	
	//opening an image as a drawable in the current thread
	RSgImage image2;
	TEST(image2.IsNull());
	CheckErrorL(KErrNone, image2.Open(id), (TText8*)__FILE__, __LINE__);
	TEST(!image2.IsNull());
	image2.Close();
	if (iEnableConformanceTests)
		{
		//opening an image as a drawable in another thread in the same process
		TSgThreadTestInfo threadInfo(id, imageinfo, ESgresSecondThreadOpenDrawable);
		TInt testResult = 0;
		TRAPD(err, testResult=CreateSecondThreadAndDoTestL(threadInfo));
		TEST(err == KErrNone);
		TEST(testResult >= 0);	
		TEST(testResult & EFirstTestPassed);
		TEST(testResult & ESecondTestPassed);
		TEST(testResult & EThirdTestPassed);
		
		//opening an image as drawbale in a different process
		TSgProcessTestInfo processInfo(id, imageinfo, ESgresSecondProcessOpenDrawable);
		TRAP(err, testResult = CreateSecondProcessAndDoTestL(KSecondProcess, processInfo));
		TEST(err == KErrNone);
		TEST(testResult >= 0);
		TEST(testResult & EFirstTestPassed);	
		TEST(testResult & ESecondTestPassed);	
		TEST(testResult & EThirdTestPassed);	
		}
	
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0014
@SYMTestCaseDesc		Retrieves drawable id of an RSgDrawable.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Id() 
						RSgDrawable::Open(const TSgDrawableId&) 
						RSgDrawable::Id()
@SYMTestPriority		Critical
@SYMTestType			UT
@SYMTestPurpose			To ensure the globally unique drawable id of the RSgDrawable object can be retrieved.
@SYMTestActions			Initialise the graphics resource component. Create an image. Call Id() on the image.
						Construct an RSgDrawable object and open an image in it. Call Id() on the
						RSgDrawable object. Compare the two ids.
@SYMTestExpectedResults	The two drawable ids should be identical.
 */	
void CTSgDrawableGeneric::TestGetDrawableDrawableIdL()
	{
	TestOpenDriverL();
	RSgImage image;
	CreateImageL(image);
	CleanupClosePushL(image);
	TSgDrawableId id1 = image.Id();
	TEST(id1 != KSgNullDrawableId);
	
	RSgDrawable drawable;
	TEST(KErrNone == drawable.Open(id1));
	TSgDrawableId id2 = drawable.Id();
	TEST(id2 != KSgNullDrawableId);
	TEST(id1 == id2);	
	
	drawable.Close();
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0032
@SYMTestCaseDesc		Calls RSgDrawable::Open() with invalid parameters.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Id()
						RSgDrawable::Open(const TSgDrawableId&)
@SYMTestPriority		Medium
@SYMTestType			UT
@SYMTestPurpose			To check the correct error codes are returned if Open() uses invalid parameters.
@SYMTestActions			Initialise the graphics resource component. Create an image. Declare an RSgDrawable 
						object. Call Open() with:
						1. null drawable id
						2. a non-existing
						3. valid drawable id
						4. on a RSgDrawable handle that is not null
						Do the same tests in a second thread in the same process and in a second process.
@SYMTestExpectedResults	The function should return:
						1. KErrArgument
						2. KErrNotFound
						3. KErrNone
						4. KErrInUse
 */	
void CTSgDrawableGeneric::TestOpenDrawableInvalidL()
	{		
	TestOpenDriverL();	
	//same thread
	//  null drawable id
	RSgDrawable drawable;
	CheckErrorL(KErrArgument, drawable.Open(KSgNullDrawableId), (TText8*)__FILE__, __LINE__);
	drawable.Close();
	
	//  non-existing drawable id
	TSgDrawableId fakeid = {0xFFFFFFFFFFFFFFFFU};
	CheckErrorL(KErrNotFound, drawable.Open(fakeid), (TText8*)__FILE__, __LINE__);
	drawable.Close();
	
	//create a valid image
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageBitOpenVgImage;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	
	RSgImage image;
	CheckErrorL(KErrNone, image.Create(info, KCrossImageData, KCrossImageDataStride), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	const TSgDrawableId id = image.Id();	
	
	//  open an image - should succeed
	TEST(KErrNone == drawable.Open(id));
	
	//  open on a non-null RSgDrawable handle
	TEST(KErrInUse == drawable.Open(id));
	drawable.Close();	
	
	if (iEnableConformanceTests)
		{
		//different thread in the same process
		TSgThreadTestInfo threadInfo(id, info, ESgresSecondThreadOpenDrawableInvalid);
		TInt testResult = 0;
		TRAPD(err, testResult = CreateSecondThreadAndDoTestL(threadInfo));
		TEST(err == KErrNone);
		TEST(testResult >= 0);
		TEST(testResult & EFirstTestPassed);
		TEST(testResult & ESecondTestPassed);
		TEST(testResult & EThirdTestPassed);
		TEST(testResult & EFourthTestPassed);
		
		//different process
		TSgProcessTestInfo processInfo(id, info, ESgresSecondProcessOpenDrawableInvalid);
		TRAP(err, testResult = CreateSecondProcessAndDoTestL(KSecondProcess, processInfo));
		TEST(err == KErrNone);
		TEST(testResult >= 0);
		TEST(testResult & EFirstTestPassed);
		TEST(testResult & ESecondTestPassed);
		TEST(testResult & EThirdTestPassed);
		TEST(testResult & EFourthTestPassed);
		}
	CleanupStack::PopAndDestroy(&image);
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0033
@SYMTestCaseDesc		Closes RSgDrawable without first opening it.
@SYMPREQ				PREQ2637
@SYMFssID				RSgDrawable::Close()
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			To check the function does nothing if Open() was not called.
@SYMTestActions			Initialise the graphics resource component. Declare an RSgDrawable object. Call Close().
@SYMTestExpectedResults	The function should not panic or return any error.
 */	
void CTSgDrawableGeneric::TestCloseDrawableWithoutOpenL()
	{
	TestOpenDriverL();	
	RSgDrawable drawable;
	drawable.Close();
	drawable.Close();
	drawable.Close();	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0080
@SYMTestCaseDesc		Calls RSgDrawable::Open() when the driver was not initialised.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Open() 
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			To ensure calling RSgDrawable::Open() with an uninitialised driver will cause a panic.
@SYMTestActions			Do not Initialise the graphics resource component and call RSgDrawable::Open() in a second thread.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 1 (ESgPanicNoDriver).
 */
void CTSgDrawableGeneric::TestPanicDrawableOpenNoDriverL()
	{
	TSgImageInfo info;
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicDrawableOpenNoDriver);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicDrawableOpenNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 1, exitCategoryName, KTestName);
	}


