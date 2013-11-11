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

#ifndef TGRAPHICSRESOURCEMULTIPROCESSTHREAD_H
#define TGRAPHICSRESOURCEMULTIPROCESSTHREAD_H

#include "sgimage.h"
#include "sgimagecollection.h"
#include "sgresource.h"
#include <e32base.h>
#include <graphics/sgresourceinternal.h>

//define default slot for multiprocess communication
const TInt KSecondProcessFunctionSlot = 1;
const TInt KSecondProcessParametersSlot = 2;

/**
* Enums for passing between the test process and the second process used in testing,
* used to define which test should be run on the second process.
*/
enum TSgresTestCase
	{
	ESgresSecondProcessOpenImage,
	ESgresSecondProcessOpenDrawable,
	ESgresSecondThreadOpenImage,
	ESgresSecondThreadOpenDrawable,
	ESgresSecondProcessOpenImageInvalid,
	ESgresSecondThreadOpenImageInvalid,
	ESgresSecondProcessMapImage,
	ESgresSecondThreadMapImage,
	ESgresSecondProcessUnmapImage,
	ESgresSecondThreadUnmapImage,
	ESgresSecondThreadOpenDrawableInvalid,
	ESgresSecondProcessOpenDrawableInvalid,
	ESgresSecondProcessPanicDriverUnclosedResources,
	ESgresSecondProcessPanicMemoryLeak,
	ESgresSecondThreadPanicDrawableOpenNoDriver,
	ESgresSecondThreadPanicImageOpenNoDriver1,
	ESgresSecondThreadPanicImageOpenNoDriver2,
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
	ESgresSecondThreadPanicImageGetPixelFormatsNoDriver,
	ESgresSecondThreadPanicImageMapReadOnlyInvalidHandle,
	ESgresSecondThreadPanicImageMapReadOnlyNoDriver,
	ESgresSecondThreadPanicImageMapWriteOnlyInvalidHandle,
	ESgresSecondThreadPanicImageMapWriteOnlyNoDriver,
	ESgresSecondThreadPanicImageMapReadWriteInvalidHandle,
	ESgresSecondThreadPanicImageMapReadWriteNoDriver,
	ESgresSecondThreadPanicImageUnmapInvalidHandle,
	ESgresSecondThreadPanicImageUnmapNoDriver,
	ESgresSecondThreadPanicImageCollectionCreateNoDriver1,
	ESgresSecondThreadPanicImageCollectionCreateNoDriver2,
	ESgresSecondThreadPanicImageCollectionCloseInvalidHandle,
	ESgresSecondThreadPanicImageCollectionCloseNoDriver,
	ESgresSecondThreadPanicImageCollectionSurfaceIdInvalidHandle,
	ESgresSecondThreadPanicImageCollectionSurfaceIdNoDriver,
	ESgresSecondThreadPanicImageCollectionGetInfoInvalidHandle,
	ESgresSecondThreadPanicImageCollectionGetInfoNoDriver,
	ESgresSecondThreadPanicImageCollectionCountInvalidHandle,
	ESgresSecondThreadPanicImageCollectionCountNoDriver,
	ESgresSecondThreadPanicImageCollectionOpenImageInvalidHandle,
	ESgresSecondThreadPanicImageCollectionOpenImageNoDriver,
	ESgresSecondThreadPanicResourceCountNoDriver,
	ESgresSecondThreadPanicAllocMarkStartNoDriver,
	ESgresSecondThreadPanicAllocMarkEndNoDriver,
	ESgresSecondThreadPanicSetAllocFailNoDriver,
	ESgresMultipleThreadStressTest,
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
 * Class for passing test info to other threads or processes.
 */	
struct TSgresTestInfo 
	{
	TSgDrawableId iDrawableId;
	TSgImageInfo iImageInfo;
	TInt iImageCount;
	TSgresTestCase iTestCase;
	TBool iPanic;
	};
	
#endif
