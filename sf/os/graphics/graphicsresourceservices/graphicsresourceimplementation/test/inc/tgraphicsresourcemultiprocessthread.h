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

#ifndef TGRAPHICSRESOURCEMULTIPROCESSTHREAD_H
#define TGRAPHICSRESOURCEMULTIPROCESSTHREAD_H

#include <e32base.h>
#include <sgresource/sgimage.h>

class CTTMSGraphicsStep;

//define default slot for multiprocess communication
const TInt KSecondProcessFunctionSlot = 1;
const TInt KSecondProcessParametersSlot = 2;

/**
* Enums for passing between the test process and the second process used in testing,
* used to define which test should be run on the second process.
*/
enum TSgresTestCase
	{
	ESgresUndefinedTest,
	ESgresSecondProcessOpenImage,
	ESgresSecondProcessOpenDrawable,
	ESgresSecondThreadOpenImage,
	ESgresSecondThreadOpenDrawable,
	ESgresSecondProcessOpenImageInvalid,
	ESgresSecondThreadOpenImageInvalid,
	ESgresSecondThreadOpenDrawableInvalid,
	ESgresSecondProcessOpenDrawableInvalid,
	ESgresSecondThreadPanicDrawableOpenNoDriver,
	ESgresSecondThreadPanicImageOpenNoDriver,
	ESgresSecondThreadPanicImageGetInterfaceInvalidHandle,
	ESgresSecondThreadPanicImageGetInterfaceNoDriver,
	ESgresSecondThreadPanicImageCloseInvalidHandle,
	ESgresSecondThreadPanicImageCloseNoDriver,
	ESgresSecondThreadPanicImageIdInvalidHandle,
	ESgresSecondThreadPanicImageIdNoDriver,
	ESgresSecondThreadPanicImageDrawableTypeNoDriver,
	ESgresSecondThreadPanicImageDrawableTypeInvalidHandle,
	ESgresSecondThreadPanicImageCreateNoDriver1,
	ESgresSecondThreadPanicImageCreateNoDriver2,
	ESgresSecondThreadPanicImageCreateInvalidHandle,
	ESgresSecondThreadPanicImageGetInfoInvalidHandle,
	ESgresSecondThreadPanicImageGetInfoNoDriver,
	ESgresMultipleThreadStressTest,
	ESgresSecondThreadPanicImageGetAttributeInvalidHandle,
	ESgresSecondThreadPanicImageGetAttributeNoDriver,
	ESgresSecondThreadPanicAttributeArrayInvalidIndex,
	ESgresSecondThreadPanicAttributeArrayInvalidIndex2,
	ESgresSecondProcessPanicDriverCloseOpenResources,
	ESgresSecondThreadOpenPassedDriver,
	ESgresSecondThreadClosePassedDriver,
	ESgresSecondThreadCreatePassedImage,
	ESgresSecondThreadOpenPassedImage,
	ESgresSecondThreadClosePassedImage,
	ESgresSecondProcessOpenImageMultithreaded,
	};

/**
* Enums for the test results. Used in multi-thread or multi-process tests.
* Each test function call may contain several sub-tests. Each EXXXTestPassed
* will be bitwised ored to the test result as each sub-test passes.
*/
enum TSgresTestResult
	{
	EFirstTestPassed = 0x00000001,
	ESecondTestPassed = 0x00000002,
	EThirdTestPassed = 0x00000004,
	EFourthTestPassed = 0x00000008,
	EFifthTestPassed = 0x00000010,
	ESixthTestPassed = 0x00000020,
	ESeventhTestPassed = 0x00000040,
	EEighthTestPassed = 0x00000080,
	ENinthTestPassed = 0x00000100,
	ETenthTestPassed = 0x00000200,
	EEleventhTestPassed = 0x00000400,
	ETwelvethTestPassed = 0x00000800,
	EThirteenthTestPassed = 0x00001000,
	EFourteenthTestPassed = 0x00002000,
	EFifteenthTestPassed = 0x00004000,
	};
	
	
/**
Utility class for passing test info between processes.
 */	
class TSgProcessTestInfo 
	{
public:
	TSgProcessTestInfo() : 
		iDrawableId(KSgNullDrawableId), iImageInfo(), iTestCase(ESgresUndefinedTest)
		{} 
	TSgProcessTestInfo(TSgDrawableId aId, const TSgImageInfo& aImageInfo, TSgresTestCase aTestCase) : 
			iDrawableId(aId), iImageInfo(aImageInfo), iTestCase(aTestCase)
			{}	
	TSgDrawableId iDrawableId;
	TSgImageInfo iImageInfo;
	TSgresTestCase iTestCase;
	};

/**
Utility class for passing test info between threads.
 */
class TSgThreadTestInfo 
	{
public:
	TSgThreadTestInfo() : 
		iDrawableId(KSgNullDrawableId), iImageInfo(), iTestCase(ESgresUndefinedTest), 
		iSgDriver(NULL), iSgImage(NULL), iTestStep(NULL) 
		{}
	TSgThreadTestInfo(TSgDrawableId aId, const TSgImageInfo& aImageInfo, TSgresTestCase aTestCase) : 
		iDrawableId(aId), iImageInfo(aImageInfo), iTestCase(aTestCase), 
		iSgDriver(NULL), iSgImage(NULL), iTestStep(NULL) 
		{}
	TSgDrawableId iDrawableId;
	TSgImageInfo iImageInfo;
	TSgresTestCase iTestCase;
	RSgDriver* iSgDriver;
	RSgImage* iSgImage;
	CTTMSGraphicsStep* iTestStep;
	};

// Message queue name for inter-process communication
_LIT(KSgTestMultiprocessMsgQ, "TestGraphicsResourceMultiprocessMsgQ");

#endif
