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

#include "tsgdriver.h"

CTSgDriver::CTSgDriver()
	{
	INFO_PRINTF1(_L("Graphics resource component test - SgDriver Tests.\r\n"));
	}

CTSgDriver::~CTSgDriver()
	{
	}

/** 
Overrides of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/
TVerdict CTSgDriver::doTestStepL()
	{	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0001"));
	INFO_PRINTF1(_L("Graphics resource component initialization and shutdown.\r\n"));
	TestInitializationAndShutdown();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0002"));
	INFO_PRINTF1(_L("Graphics resource component shutdown multiple time.\r\n"));
	TestInitializeShutdownManyTimes();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0022"));
	INFO_PRINTF1(_L("Shutting down an uninitialised driver.\r\n"));
	TestShutdownUninitialized();
	RecordTestResultL();

#ifdef _DEBUG
	SetTestStepID(_L("GRAPHICS-RESOURCE-0078"));
	INFO_PRINTF1(_L("SgDriver panic test - private heap memory leak.\r\n"));
	TestDriverMemoryLeakL();
	RecordTestResultL();
#ifndef __WINS__
	SetTestStepID(_L("GRAPHICS-RESOURCE-0023"));
	INFO_PRINTF1(_L("Shutting down without closing all resources.\r\n"));
	TestShutdownMemoryLeakL();
	RecordTestResultL();
#endif
	SetTestStepID(_L("GRAPHICS-RESOURCE-0079"));
	INFO_PRINTF1(_L("SgDriver panic test - ResourceCount() without initialising the driver.\r\n"));
	TestPanicResourceCountNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0090"));
	INFO_PRINTF1(_L("SgDriver panic test - AllocMarkStart() without initialising the driver.\r\n"));
	TestPanicAllocMarkStartNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0091"));
	INFO_PRINTF1(_L("SgDriver panic test - AllocMarkEnd() without initialising the driver.\r\n"));
	TestPanicAllocMarkEndNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0092"));
	INFO_PRINTF1(_L("SgDriver panic test - SetAllocFail() without initialising the driver.\r\n"));
	TestPanicSetAllocFailNoDriverL();
	RecordTestResultL();
#else
	INFO_PRINTF1(_L("Warning: Skipping the panic tests. \r\n"));
#endif	
	return TestStepResult();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0001
@SYMTestCaseDesc		Initialises and shuts down the graphics resource driver.
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				SgDriver::Open()\n 
						RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)\n
						SgDriver::Close()\n
						SgDriver::ResourceCount()\n
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the graphics resource driver can be initialised successfully
@SYMTestActions			Call SgDriver::Open() to start the graphics resource driver, then try 
						to call RSgImage::Create(). Call SgDriver::Open() the second time 
						and then call RSgImage::Create() again. Close the driver and call 
						RSgImage::Create(). Close the driver.
@SYMTestExpectedResults	RSgImage::Create() should return:\n
						\t 1. KErrNone \n
						\t 2. KErrNone \n
						\t 3. KErrNone \n
						SgDriver::Open() should return KErrNone both times.						
@SYMTestStatus			Implemented 
 */	
void CTSgDriver::TestInitializationAndShutdown()
	{
	__UHEAP_MARK;
	
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iCpuAccess = ESgCpuAccessReadWrite;
	info.iShareable = ETrue;
	
	RSgImage image;	
	TEST(KErrNone == SgDriver::Open());	
	TEST(KErrNone == image.Create(info, KImageData, 16));	
	TEST(0 < SgDriver::ResourceCount());
	image.Close();
	
	TEST(KErrNone == SgDriver::Open());	
	TEST(KErrNone == image.Create(info, KImageData, 16));	
	image.Close();
	
	SgDriver::Close();
	TEST(KErrNone == image.Create(info, KImageData, 16));	
	image.Close();
	
	TEST(0 == SgDriver::ResourceCount());
	SgDriver::Close();
	
	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0002
@SYMTestCaseDesc		Shuts down graphics resource driver multiple times.
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				SgDriver::Open()\n 
						SgDriver::Close()\n 
						SgDriver::ResourceCount()\n
						RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the graphics resource driver could be shut down multiple times.
@SYMTestActions			Initialise the graphics resource driver. Create an image then close it.
						Shutdown the driver twice. Open and driver and create an image. Close the
						image and shutdown the driver.
@SYMTestExpectedResults	The graphics resource driver is successfully shut down. The calls to
						RSgImage::Create() should return:\n
						\t 1. KErrNone\n
						\t 2. KErrNone\n
@SYMTestStatus			Implemented 
 */
void CTSgDriver::TestInitializeShutdownManyTimes()
	{
	__UHEAP_MARK;
	
	TEST(KErrNone == SgDriver::Open());	
	
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iCpuAccess = ESgCpuAccessReadWrite;
	info.iShareable = ETrue;
	
	RSgImage image;
	
	TEST(KErrNone == image.Create(info, KImageData, 16));	
	image.Close();
	
	SgDriver::Close();
	SgDriver::Close();
	
	TEST(KErrNone == SgDriver::Open());	
	TEST(KErrNone == image.Create(info, KImageData, 16));	
	image.Close();
	
	TEST(0 == SgDriver::ResourceCount());
	SgDriver::Close();
	
	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0022
@SYMTestCaseDesc		Shuts down an uninitialised driver.
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMFssID				SgDriver::Close()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure invalid Close() calls on the driver will cause no errors.
@SYMTestActions			Call SgDriver::Close() several times without calling SgDriver::Open().
@SYMTestExpectedResults	No errors should be returned.
@SYMTestStatus			Implemented 
 */	
void CTSgDriver::TestShutdownUninitialized()
	{
	__UHEAP_MARK;	
	
	SgDriver::Close();
	SgDriver::Close();
	SgDriver::Close();
	
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0023
@SYMTestCaseDesc		Shuts down the driver without closing all resources.
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				SgDriver::Open()\n 
						RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)\n 
						SgDriver::Close()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling Close() without closing all resources will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Try to shutdown the component.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 1(ESgPanicUnclosedResources).
@SYMTestStatus			Implemented 
 */
void CTSgDriver::TestShutdownMemoryLeakL()
	{
	TestOpenDriverL();
	//run the test in another process
	TSgImageInfo info;
	TSgresTestInfo processInfo = {KSgNullDrawableId, info, 0, ESgresSecondProcessPanicDriverUnclosedResources, ETrue};
	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestShutdownMemoryLeakL");
	CreateSecondProcessAndCheckPanicL(processInfo, 1, exitCategoryName, KTestName);
 	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0078
@SYMTestCaseDesc		Calls SgDriver::AllocMarkEnd() without shutting down all the resources.
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				SgDriver::AllocMarkStart()\n
						SgDriver::AllocMarkEnd()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling SgDriver::AllocMarkEnd() without closing all resources will cause a panic.
@SYMTestActions			Initialise the graphics resource component in a second process and call SgDriver::AllocMarkStart().
						Create an image and call SgDriver::AllocMarkEnd().
@SYMTestExpectedResults The function should panic in the second process with panic code SGRES-ADAPTER 0.
@SYMTestStatus			Implemented 
 */
void CTSgDriver::TestDriverMemoryLeakL()
	{
	TestOpenDriverL();
	//run the test in another process
	TSgImageInfo info;
	TSgresTestInfo processInfo = {KSgNullDrawableId, info, 0, ESgresSecondProcessPanicMemoryLeak, ETrue};
	TExitCategoryName exitCategoryName(_L("SGALLOC"));
	_LIT(KTestName, "TestDriverMemoryLeakL");
	CreateSecondProcessAndCheckPanicL(processInfo, 0, exitCategoryName, KTestName);
 	TestCloseDriver();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0079
@SYMTestCaseDesc		Calls SgDriver::ResourceCount() without initialising the driver.
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMFssID				SgDriver::ResourceCount())\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling SgDriver::ResourceCount() without initialising
						the driver first will cause a panic in the debug mode.
@SYMTestActions			Do not Initialise the graphics resource component and call 
						SgDriver::ResourceCount() in a second thread. 
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgDriver::TestPanicResourceCountNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicResourceCountNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicResourceCountNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0090
@SYMTestCaseDesc		Calls SgDriver::AllocMarkStart() without initialising the driver.
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMFssID				SgDriver::AllocMarkStart())\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling SgDriver::AllocMarkStart() without initialising
						the driver first will cause a panic in the debug mode.
@SYMTestActions			Do not Initialise the graphics resource component and call 
						SgDriver::AllocMarkStart() in a second thread. 
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgDriver::TestPanicAllocMarkStartNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicAllocMarkStartNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicAllocMarkStartNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0091
@SYMTestCaseDesc		Calls SgDriver::AllocMarkEnd() without initialising the driver.
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMFssID				SgDriver::AllocMarkEnd())\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling SgDriver::AllocMarkEnd() without initialising
						the driver first will cause a panic in the debug mode.
@SYMTestActions			Do not Initialise the graphics resource component and call 
						SgDriver::AllocMarkEnd() in a second thread. 
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgDriver::TestPanicAllocMarkEndNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicAllocMarkEndNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicAllocMarkEndNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0092
@SYMTestCaseDesc		Calls SgDriver::SetAllocFail() without initialising the driver.
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMFssID				SgDriver::SetAllocFail())\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling SgDriver::SetAllocFail() without initialising
						the driver first will cause a panic in the debug mode.
@SYMTestActions			Do not Initialise the graphics resource component and call 
						SgDriver::SetAllocFail() in a second thread. 
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgDriver::TestPanicSetAllocFailNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicSetAllocFailNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicSetAllocFailNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}
