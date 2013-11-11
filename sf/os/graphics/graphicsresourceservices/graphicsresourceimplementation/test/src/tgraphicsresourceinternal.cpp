// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tgraphicsresourceinternal.h"

#include <sgresource/sgresource.h>
#include <sgresource/sgimage.h>

#include "tgraphicsresourceteststepbase.h"
#include "tgraphicsresourceinternalextensions.h"


CTGraphicsResourceInternal::CTGraphicsResourceInternal()
	{
	SetTestStepName(KTGraphicsResourceInternalStep);
	}

CTGraphicsResourceInternal::~CTGraphicsResourceInternal()
	{
	}

/**
Overrides of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/
TVerdict CTGraphicsResourceInternal::doTestStepL()
	{
#ifndef _DEBUG
	INFO_PRINTF1(_L("Some tests cannot run in release builds, skipping..."));
#else
	// These tests use KInternalTestsSecondProcess, which can only run in UDEB mode
	// otherwise a panic will occur when attempting to find the UDEB extension interface
	// MSgDriver_Test...
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0200"));
	TestDriverMemoryLeakL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0201"));
	TestOOML();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0202"));
	TestInitializeAndShutdownL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0203"));
	TestInitializeAndShutdownManyTimesL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0111"));
	TestResourceProfilingL();
	RecordTestResultL();	
#endif
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0204"));
	TestUsageBitsL();
	RecordTestResultL();

	return TestStepResult();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0200
@SYMTestCaseDesc		Test SGAlloc panic occurs when we deliberately leak resources.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Create()
						RSgDriver:AllocMarkEnd()
@SYMTestPriority		Medium
@SYMTestType			UT
@SYMTestPurpose			To ensure that leaking resources causes the internal Memory Leak checking routine to panic
@SYMTestActions			Initialise the graphics resource component. Call AllocMarkStart to begin memory leak checking
						Create a new RSgImage and then call AllocMarkEnd before closing the RSgImage to cause an 
						SGALLOC panic
@SYMTestExpectedResults	The panic SGALLOC:xxxxxxxx where xxxxxxxx is the address of the leak.
 */	
void CTGraphicsResourceInternal::TestDriverMemoryLeakL()
	{
	TSgResIntTestInfo testInfo = { ESgResIntDriverMemoryLeak };
	CreateSecondProcessAndCheckAllocPanicL(KInternalTestsSecondProcess, testInfo, KSgAllocPanic);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0201
@SYMTestCaseDesc		Test Graphics Resource with low memory conditions.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage
						RSgDrawable
						RSgDriver
@SYMTestPriority		High
@SYMTestType			UT
@SYMTestPurpose			To ensure the correct errors or KErrNoMemory are returned by graphics resource apis under low
						memory conditions.
@SYMTestActions			Force allocations to fail on the RSgDrivers heap, try running each of the conformance tests and
						stress tests to ensure no RSgDriver allocated memory or heap memory is leaked. It also creates
						an image in this process which is used in another process for OOM testing. 
@SYMTestExpectedResults	Return codes of the functions tested should be either the expected value or KErrNoMemory
						This is handled by CTSgTestStepBase::CheckErrorL. No ALLOC or SGALLOC panics should occur.
 */	
void CTGraphicsResourceInternal::TestOOML()
	{
	// drawable OOM test
	TSgResIntTestInfo testInfo = { ESgResIntDrawableOOM };
	TInt result = CreateSecondProcessAndDoTestL(KInternalTestsSecondProcess, testInfo);
	TEST(result & EFirstTestPassed);
	
	// image OOM test
    testInfo.iTestCase = ESgResIntImageOOM;
    
    RSgDriver sgDriver;
    TInt err = sgDriver.Open();
    TESTEL(err == KErrNone, err);
    CleanupClosePushL(sgDriver);
    
    RSgImage image1;
    TSgImageInfo info1(TSize(8, 8), EUidPixelFormatRGB_565, ESgUsageBitOpenVgImage);
    err = image1.Create(info1);
    TESTEL(err == KErrNone, err);
    CleanupClosePushL(image1);
    testInfo.iDrawableId = image1.Id();
            
    result = CreateSecondProcessAndDoTestL(KInternalTestsSecondProcess, testInfo);
    TEST(result & EFirstTestPassed);
        
    CleanupStack::PopAndDestroy(2, &sgDriver); 
	}
/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0202
@SYMTestCaseDesc		Test valid resource count
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage
						RSgDrawable
						RSgDriver
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			To ensure that RSgDriver reports the correct local resource count when images are opened and closed.
@SYMTestActions			Open an RSgDriver and create an image, then close it checking the resource count of the
						driver is valid at each stage (0,1,0).
@SYMTestExpectedResults	Resource counts should be as expected.
 */	
void CTGraphicsResourceInternal::TestInitializeAndShutdownL()
	{
	TSgResIntTestInfo testInfo = { ESgResIntInitializeAndShutdown };
	TInt result = CreateSecondProcessAndDoTestL(KInternalTestsSecondProcess, testInfo);
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	}

/*
@SYMTestCaseID			GRAPHICS-RESOURCE-0203
@SYMTestCaseDesc		Test valid resource count with multiple driver sessions
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage
						RSgDrawable
						RSgDriver
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			To ensure that RSgDriver reports the correct local resource count when images are opened and closed.
@SYMTestActions			Open an RSgDriver and create an image, then close it checking the resource count of the
						driver is valid at each stage (0,1,0). Attempt to close the driver multiple times and then perform the
						same tests again.
@SYMTestExpectedResults	Resource counts should be as expected.
 */	
void CTGraphicsResourceInternal::TestInitializeAndShutdownManyTimesL()
	{
	TSgResIntTestInfo testInfo = { ESgResIntInitializeAndShutdownManyTimes };
	TInt result = CreateSecondProcessAndDoTestL(KInternalTestsSecondProcess, testInfo);
	TEST(result & EFirstTestPassed);
	TEST(result & ESecondTestPassed);
	TEST(result & EThirdTestPassed);
	TEST(result & EFourthTestPassed);
	}

/*
@SYMTestCaseID			GRAPHICS-RESOURCE-0111
@SYMTestCaseDesc		Test MSgDriver_Profiling extension.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage
						RSgDriver
						MSgDriver_Profiling
@SYMTestPriority		Medium
@SYMTestType			UT
@SYMTestPurpose			To test that the extension MSgDriver_Profiling correctly reports
						the correct global and local resource count and memory usage when
						resources are created in separate processes.
@SYMTestActions			Get the MSgDriver_Profiling extension, query memory/resource usage.
						Create an image and check  the memory usage and count.
						Launch a separate process that checks the same memory usage/count.
						Create an image in this separate process and check the global resource 
						count and memory. Close the image nad check the memory usage and resource
						count. Terminate the second process. Check the local and global count 
						in the first process. Close the image in this process and check the global 
						and local resource count.
@SYMTestExpectedResults When creating the first image, the local resource count should equal one,
						the global count should increment. The local memory usage should increase
						by at least the size of the image in pixels * byte-depth. The global memory
						usage should increase by the same amount.
						Second process should report the same global resouce count and memory as
						the first process. 
						Second process image creation to cause same usage/count increase as did 
						first image.
						Closing the image in the second process should set count and memory usage 
						back to initial values, and local count/usage to zero. 
						Closing the image in the first process should set the count and memory usage
						back to their pre-test values, and local count/usage to zero.
 */	
void CTGraphicsResourceInternal::TestResourceProfilingL()
	{
	__UHEAP_MARK;
	
	RSgDriver sgDriver;
	TInt err = sgDriver.Open();
	TESTE(err == KErrNone, err);

	if (KErrNone == err)
		{
		MSgDriver_Profiling* profiling = NULL;
		err = sgDriver.GetInterface(profiling);
		if (!profiling || err != KErrNone)
			{
			ERR_PRINTF2(_L("Failed to get MSgDriver_Profiling extension [%d]"), err);
			SetTestStepResult(EFail);
			return;
			}
		
		const TInt originalGlobalResourceCount = profiling->GlobalResourceCount();
		const TInt originalGlobalGraphicsMemory = profiling->GlobalGraphicsMemoryUsed();

		TEST(profiling->LocalGraphicsMemoryUsed() == 0);
		
		RSgImage image;
		const TSize KImageSize(8, 8);
		err = image.Create(TSgImageInfo(KImageSize, EUidPixelFormatARGB_8888, ESgUsageBitOpenVgImage));
		TESTE(err == KErrNone, err);

		// Check that having created an image, the global resource count and memory usage has
		// increased.
		TInt localGraphicsMemory = profiling->LocalGraphicsMemoryUsed();
		TEST(localGraphicsMemory >= (KImageSize.iWidth * KImageSize.iHeight * 4));
		TEST(profiling->GlobalResourceCount() == (originalGlobalResourceCount + 1));
		TEST(profiling->GlobalGraphicsMemoryUsed() == (localGraphicsMemory + originalGlobalResourceCount));
		
		TSgResIntTestInfo testInfo = { ESgResIntResourceProfiling };
		testInfo.iGlobalGraphicsMemory = profiling->GlobalGraphicsMemoryUsed();
		testInfo.iGlobalResourceCount = profiling->GlobalResourceCount();
		TInt result = CreateSecondProcessAndDoTestL(KInternalTestsSecondProcess, testInfo);
		TEST(result & EFirstTestPassed);
		TEST(result & ESecondTestPassed);
		TEST(result & EThirdTestPassed);
		TEST(result & EFourthTestPassed);
		TEST(result & EFifthTestPassed);
		TEST(result & ESixthTestPassed);
		TEST(result & ESeventhTestPassed);
		TEST(result & EEighthTestPassed);
		TEST(result & ENinthTestPassed);
		TEST(result & ETenthTestPassed);
		TEST(result & EEleventhTestPassed);
		
		// Check that the global and local counts are unchanged.
		TEST(profiling->LocalGraphicsMemoryUsed() == localGraphicsMemory);
		TEST(profiling->GlobalResourceCount() == (originalGlobalResourceCount + 1));
		TEST(profiling->GlobalGraphicsMemoryUsed() == (localGraphicsMemory + originalGlobalResourceCount));
				
		// Check that closing the image shows the image memory is back to zero.
		image.Close();		
		TEST(profiling->LocalGraphicsMemoryUsed() == 0);
		TEST(profiling->GlobalGraphicsMemoryUsed() == originalGlobalGraphicsMemory);
		TEST(profiling->GlobalResourceCount() == originalGlobalResourceCount);
		
		// Cleanup
		sgDriver.Close();
		profiling = NULL;
		}
	__UHEAP_MARKEND;
	}

 /*
 Used for Usage Bit test (GRAPHICS-RESOURCE-0204)
 */
struct TUsageBitTest
    {
    TUint32     iUsageBit;              // usage bit to create an image
    TInt        iPixelFormat;           // pixel format to create an inmage
    
    TInt        iExpectedReturnCode;    // expected return code for RSgImage::Create with the usage bit iRequestedUsageBit
    TUint32     iExpectedUsageBit;      // expected usage bit of TSgImageInfo that is obtained by calling RSgImage::GetInfo
    };

/*
 set of usage bit test cases
 */
const static TUsageBitTest KUsageBitTestCases[] = {
        // with ESgPixelFormatARGB_8888_PRE
        { ESgUsageBitOpenVgImage,       ESgPixelFormatARGB_8888_PRE, KErrNone,           ESgUsageBitOpenVgImage}, // 0
        { ESgUsageBitOpenGlesTexture2D, ESgPixelFormatARGB_8888_PRE, KErrNone,           ESgUsageBitOpenGlesTexture2D}, // 1
        { ESgUsageBitOpenGles2Texture2D,ESgPixelFormatARGB_8888_PRE, KErrNotSupported,   ESgUsageBitOpenGles2Texture2D}, // 2
        { ESgUsageBitOpenVgSurface,     ESgPixelFormatARGB_8888_PRE, KErrNone,           ESgUsageBitOpenVgSurface | ESgUsageBitOpenGlesSurface}, // 3
        { ESgUsageBitOpenGlesSurface,   ESgPixelFormatARGB_8888_PRE, KErrNone,           ESgUsageBitOpenVgSurface | ESgUsageBitOpenGlesSurface }, // 4
        { ESgUsageBitOpenGles2Surface,  ESgPixelFormatARGB_8888_PRE, KErrNotSupported,   0 }, // 5
        { ESgUsageBitOpenGlSurface,     ESgPixelFormatARGB_8888_PRE, KErrNotSupported,   0 }, // 6
        
        { ESgUsageBitOpenVgImage | ESgUsageBitOpenGlesTexture2D,    ESgPixelFormatARGB_8888_PRE, KErrNone, ESgUsageBitOpenVgImage | ESgUsageBitOpenGlesTexture2D},// 7
        { ESgUsageBitOpenVgImage | ESgUsageBitOpenVgSurface,        ESgPixelFormatARGB_8888_PRE, KErrNone, ESgUsageBitOpenVgImage | ESgUsageBitOpenVgSurface | ESgUsageBitOpenGlesSurface}, // 8
        { ESgUsageBitOpenVgSurface | ESgUsageBitOpenGlesSurface,    ESgPixelFormatARGB_8888_PRE, KErrNone, ESgUsageBitOpenVgSurface | ESgUsageBitOpenGlesSurface}, // 9
        { ESgUsageBitOpenVgSurface | ESgUsageBitOpenGles2Surface,   ESgPixelFormatARGB_8888_PRE, KErrNotSupported, 0}, // 10
        { ESgUsageBitOpenVgSurface | ESgUsageBitOpenGlSurface,      ESgPixelFormatARGB_8888_PRE, KErrNotSupported, 0}, // 11
        { ESgUsageBitOpenVgSurface | ESgUsageBitOpenGles2Texture2D, ESgPixelFormatARGB_8888_PRE, KErrNotSupported, 0},//  12
        
        { ESgUsageBitOpenVgImage | ESgUsageBitOpenGlesTexture2D |ESgUsageBitOpenVgSurface |ESgUsageBitOpenGlesSurface, ESgPixelFormatARGB_8888_PRE, KErrNone, 
          ESgUsageBitOpenVgImage | ESgUsageBitOpenGlesTexture2D |ESgUsageBitOpenVgSurface |ESgUsageBitOpenGlesSurface}, //  13
        

        //with ESgPixelFormatA_8
        { ESgUsageBitOpenVgImage,       ESgPixelFormatA_8, KErrNone,           ESgUsageBitOpenVgImage}, // 14
        { ESgUsageBitOpenGlesTexture2D, ESgPixelFormatA_8, KErrNone,           ESgUsageBitOpenGlesTexture2D}, // 15
        { ESgUsageBitOpenGles2Texture2D,ESgPixelFormatA_8, KErrNotSupported,   ESgUsageBitOpenGles2Texture2D}, // 16
        { ESgUsageBitOpenVgSurface,     ESgPixelFormatA_8, KErrNotSupported,   0 }, // 17
        { ESgUsageBitOpenGlesSurface,   ESgPixelFormatA_8, KErrNotSupported,   0 }, // 18
        { ESgUsageBitOpenGles2Surface,  ESgPixelFormatA_8, KErrNotSupported,   0 }, // 19
        { ESgUsageBitOpenGlSurface,     ESgPixelFormatA_8, KErrNotSupported,   0 }, // 20
        
        // with EUidPixelFormatARGB_8888
        { ESgUsageBitOpenVgImage,       ESgPixelFormatARGB_8888, KErrNone,           ESgUsageBitOpenVgImage}, // 21
        { ESgUsageBitOpenGlesTexture2D, ESgPixelFormatARGB_8888, KErrNone,           ESgUsageBitOpenGlesTexture2D}, // 22
        { ESgUsageBitOpenGles2Texture2D,ESgPixelFormatARGB_8888, KErrNotSupported,   ESgUsageBitOpenGles2Texture2D}, // 23
        { ESgUsageBitOpenVgSurface,     ESgPixelFormatARGB_8888, KErrNone,           ESgUsageBitOpenVgSurface | ESgUsageBitOpenGlesSurface}, // 3
        { ESgUsageBitOpenGlesSurface,   ESgPixelFormatARGB_8888, KErrNone,           ESgUsageBitOpenVgSurface | ESgUsageBitOpenGlesSurface }, // 4
        { ESgUsageBitOpenGles2Surface,  ESgPixelFormatARGB_8888, KErrNotSupported,   0 }, // 24
        { ESgUsageBitOpenGlSurface,     ESgPixelFormatARGB_8888, KErrNotSupported,   0 }, // 25
        
        // with ESgPixelFormatXRGB_8888
        { ESgUsageBitOpenVgImage,       ESgPixelFormatXRGB_8888, KErrNone,           ESgUsageBitOpenVgImage}, // 26
        { ESgUsageBitOpenGlesTexture2D, ESgPixelFormatXRGB_8888, KErrNone,           ESgUsageBitOpenGlesTexture2D}, // 27
        { ESgUsageBitOpenGles2Texture2D,ESgPixelFormatXRGB_8888, KErrNotSupported,   ESgUsageBitOpenGles2Texture2D}, // 28
        { ESgUsageBitOpenVgSurface,     ESgPixelFormatXRGB_8888, KErrNone,           ESgUsageBitOpenVgSurface | ESgUsageBitOpenGlesSurface}, // 29
        { ESgUsageBitOpenGlesSurface,   ESgPixelFormatXRGB_8888, KErrNone,           ESgUsageBitOpenVgSurface | ESgUsageBitOpenGlesSurface }, // 30
        { ESgUsageBitOpenGles2Surface,  ESgPixelFormatXRGB_8888, KErrNotSupported,   0 }, // 31
        { ESgUsageBitOpenGlSurface,     ESgPixelFormatXRGB_8888, KErrNotSupported,   0 }, // 32
        
        // invalid usage bit
        { 0x0000, ESgPixelFormatRGB_565, KErrArgument, 0 }, // 33
        { 0x1000, ESgPixelFormatRGB_565, KErrNotSupported, 0 }, // 34
    };

const TInt KUsageBitTestCount = sizeof(KUsageBitTestCases)/sizeof(KUsageBitTestCases[0]);

/*
@SYMTestCaseID          GRAPHICS-RESOURCE-0204
@SYMTestCaseDesc        Test usage bit
@SYMPREQ                PREQ2637
@SYMFssID               RSgImage
                        RSgDrawable
                        RSgDriver
@SYMTestPriority        High
@SYMTestType            UT
@SYMTestPurpose         To ensure that an image can be created using various usage bit, and the usage bit of created image
                        is correct.
@SYMTestActions         Create an image with selection of usage bit. Check if the return code of SgImage::Create
                        is correct. If creation succeeds, open the image and check if the usage bit is the same 
                        to the one expected. 
                        NOTE: The usage bit may not be the same as the one that is used to create an image - it 
                        depends on the implementation.
@SYMTestExpectedResults The usage bit should match the expected usage bit.
 */ 
void CTGraphicsResourceInternal::TestUsageBitsL()
    {
 
    RSgDriver driver;
    User::LeaveIfError(driver.Open());
    CleanupClosePushL(driver);
    
    for(TInt i=0; i < KUsageBitTestCount; ++i)
        {
        TSgImageInfo info(TSize(8, 8), KUsageBitTestCases[i].iPixelFormat,  KUsageBitTestCases[i].iUsageBit);
            
        RSgImage image;
        TInt err = image.Create(info);
        CleanupClosePushL(image);
        
        if(err != KUsageBitTestCases[i].iExpectedReturnCode)
            {
            ERR_PRINTF3(_L("Test case index: %d, Test failed with error: %d"), i, err);
            SetTestStepResult(EFail);
            }
        
        if(err == KErrNone)
            {    
            TSgImageInfo info1;
            User::LeaveIfError(image.GetInfo(info1));   
            if(info1.iUsage != KUsageBitTestCases[i].iExpectedUsageBit)
                {
                ERR_PRINTF3(_L("Test case index: %d, info1 usage bit 0x%04X"), i, info1.iUsage);
                SetTestStepResult(EFail);
                }
            }
            
        CleanupStack::PopAndDestroy(&image);
        }
    
    CleanupStack::PopAndDestroy(&driver);
    }

