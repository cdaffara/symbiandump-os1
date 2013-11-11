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

#include "tsgdrawablegeneric.h"
#include <graphics/sgresourceinternal.h>

CTSgDrawableGeneric::CTSgDrawableGeneric()
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
/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0081
*/
	SetTestStepID(_L("GRAPHICS-RESOURCE-0081"));		
	INFO_PRINTF1(_L("RSgDrawable generic out of memory tests.\r\n"));
	TestOOM();
	RecordTestResultL();
	
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
@SYMPREQ				PREQ39
@SYMREQ 				REQ9225
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Id()\n 
						RSgDrawable::Open(const TSgDrawableId&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure an RSgDrawable object can reference an image.
@SYMTestActions			Initialise the graphics resource component. Create an image. Declare an
						RSgDrawable object. Call Open() on the object with the resource ID of the
						image.
@SYMTestExpectedResults	The function should return KErrNone. The RSgDrawable now references an
						image.
@SYMTestStatus			Implemented 
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
	RSgImage drawable;
	TEST(drawable.IsNull());
	CheckErrorL(KErrNone, drawable.Open(id), (TText8*)__FILE__, __LINE__);
	TEST(!drawable.IsNull());
	drawable.Close();
	
	//opening an image as a drawable in another thread in the same process
	TSgresTestInfo threadInfo = {id, imageinfo, 0, ESgresSecondThreadOpenDrawable};
	TInt testResult = CreateSecondThreadAndDoTestL(threadInfo);
	User::LeaveIfError(testResult);	
	TEST(testResult & EFirstTestPassed);
	TEST(testResult & ESecondTestPassed);
	TEST(testResult & EThirdTestPassed);
	
	//opening an image as drawbale in a different process
	TSgresTestInfo processInfo = {id, imageinfo, 0, ESgresSecondProcessOpenDrawable};
	testResult = CreateSecondProcessAndDoTestL(KSecondProcess, processInfo);	
	User::LeaveIfError(testResult);
	TEST(testResult & EFirstTestPassed);	
	TEST(testResult & ESecondTestPassed);	
	TEST(testResult & EThirdTestPassed);	

	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0014
@SYMTestCaseDesc		Retrieves drawable id of an RSgDrawable.
@SYMPREQ				PREQ39
@SYMREQ 				REQ9225
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Id()\n 
						RSgDrawable::Open(const TSgDrawableId&)\n 
						RSgDrawable::Id()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the globally unique drawable id of the RSgDrawable object can be retrieved.
@SYMTestActions			Initialise the graphics resource component. Create an image. Call Id() on the image.
						Construct an RSgDrawable object and open an image in it. Call Id() on the
						RSgDrawable object. Compare the two ids.
@SYMTestExpectedResults	The two drawable ids should be identical.
@SYMTestStatus			Implemented 
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
@SYMPREQ				PREQ39
@SYMREQ 				REQ9225
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Id()\n 
						RSgDrawable::Open(const TSgDrawableId&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check the correct error codes are returned if Open() uses invalid parameters.
@SYMTestActions			Initialise the graphics resource component. Create an image. Declare an RSgDrawable 
						object. Call Open() with:\n
						\t 1. null drawable id\n
						\t 2. a non-existing \n
						\t 3. drawable id of a non-sharable image\n 
						\t 4. on a RSgDrawable handle that is not null\n
						Do the same tests in a second thread in the same process and in a second process.
@SYMTestExpectedResults	The function should return:\n
						\t 1. KErrArgument\n
						\t 2. KErrNotFound\n
						\t 3. KErrNone in the same process and KErrPermissionDenied or KErrNotFound in the second process\n
						\t 4. KErrInUse
@SYMTestStatus			Implemented 
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
	TSgDrawableId fakeid = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
	CheckErrorL(KErrNotFound, drawable.Open(fakeid), (TText8*)__FILE__, __LINE__);
	drawable.Close();
	
	//create a non-sharable image
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiTarget;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iCpuAccess = ESgCpuAccessNone;
	info.iShareable = EFalse;
	
	RSgImage image;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	const TSgDrawableId id = image.Id();	
	
	//  open a non-sharable image - should succeed
	TEST(KErrNone == drawable.Open(id));
	
	//  open on a non-null RSgDrawable handle
	TEST(KErrInUse == drawable.Open(id));
	drawable.Close();	
	
	//different thread in the same process
	TSgresTestInfo threadInfo = {id, info, 0, ESgresSecondThreadOpenDrawableInvalid};
	TInt testResult = CreateSecondThreadAndDoTestL(threadInfo);
	User::LeaveIfError(testResult);
	TEST(testResult & EFirstTestPassed);
	TEST(testResult & ESecondTestPassed);
	TEST(testResult & EThirdTestPassed);
	TEST(testResult & EFourthTestPassed);	
	
	//different process
	TSgresTestInfo processInfo = {id, info, 0, ESgresSecondProcessOpenDrawableInvalid};
	testResult = CreateSecondProcessAndDoTestL(KSecondProcess, processInfo);
	User::LeaveIfError(testResult);
	TEST(testResult & EFirstTestPassed);
	TEST(testResult & ESecondTestPassed);
	TEST(testResult & EThirdTestPassed);
	TEST(testResult & EFourthTestPassed);
	TEST(testResult & EFifthTestPassed);

	drawable.Close();
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0033
@SYMTestCaseDesc		Closes RSgDrawable without first opening it.
@SYMPREQ				PREQ39
@SYMREQ 				REQ9225
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgDrawable::Close()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check the function does nothing if Open() was not called.
@SYMTestActions			Initialise the graphics resource component. Declare an RSgDrawable object. Call Close().
@SYMTestExpectedResults	The function should not panic or return any error.
@SYMTestStatus			Implemented 
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
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMFssID				RSgImage::Open()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling RSgDrawable::Open() with an uninitialised driver will cause a panic.
@SYMTestActions			Do not Initialise the graphics resource component and call RSgDrawable::Open() in a second thread.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgDrawableGeneric::TestPanicDrawableOpenNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicDrawableOpenNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicDrawableOpenNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

void CTSgDrawableGeneric::DoMemoryTestsL()
	{
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestOpenImageAsDrawableL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestGetDrawableDrawableIdL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestOpenDrawableInvalidL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestCloseDrawableWithoutOpenL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	}
