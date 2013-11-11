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
// This class does performance tests for eglSwapBuffers() and eglSwapBuffersRegionNOK().
// The function eglSwapBuffersRegionNOK() is a vendor specific EGL extension and allows users to 
// perform region based surface updates. The test should show how the performance of the
// extension function compares to the default one. 
// 

/**
 @file
 @test 
*/

#include "egltest_benchmark_swapbuffers.h"

#include <VG/openvg.h>
#include <test/tprofiler.h>
#include <test/egltestcommonutils.h>
#include <test/egltestcommoninisettings.h>

_LIT(KSwapBuffersSection, "SwapBuffers");

// The test draws alternating backgrounds to show the affect
// of different swapBuffer functions 
static const TInt KMaxClearColors = 2;
static const VGfloat KClearColors[KMaxClearColors][4] = 
    {
        {0.5f, 0.5f, 0.5f, 1.0f}, // gray
        {0.1f, 0.2f, 0.4f, 1.0f}  // blue
    };

// Number of iterations, it defines how often the swapBuffer function is called
static const TInt KIterationsToTest = 10;

// Maximum number of rectangles for eglSwapBuffersRegionNOK() stress test
static const TInt KStressTestMaxNoRects = 100;
// Defines the increase of number of rectangles for each iteration
static const TInt KStressTestNoRectsStepSize = 5;
// Gap between the dirty Rectangles
static const TInt KStressTestRectGap = 3;

// This test step meassures the performance of eglSwapBuffers()
_LIT(KTestStep0528,"GRAPHICS-EGL-0528");
// This test step meassures the performance of eglSwapBuffersRegionNOK()
_LIT(KTestStep0529,"GRAPHICS-EGL-0529");
// This test step meassures the performance of eglSwapBuffersRegionNOK() with a lot of dirty rectangles
_LIT(KTestStep0530,"GRAPHICS-EGL-0530");

_LIT(KErrEglConfigNotSupported, "EGL config is not supported.");
_LIT(KInfoRectangles, "Number of dirty rectangles: %d");
_LIT(KWarnStressTestRectCount, "Dirty rectangles for stress test don't fit onto window surface (%d of %d).");

_LIT(KEglSwapBuffersRegionNokMsg, "eglSwapBuffersRegionNOK extension is not supported.");

CEglTest_Benchmark_SwapBuffers::CEglTest_Benchmark_SwapBuffers()
	{
	SetTestStepName(KBenchmark_SwapBuffers);
	}
	
CEglTest_Benchmark_SwapBuffers::~CEglTest_Benchmark_SwapBuffers()	
	{
    // empty
	}

/**
 * It's called by the test framework before the actual test. It's used
 * to do the preparation for the test. It's important to call the
 * baseclass implementation also.
 * 
 * @return test framework code
 * @leave Standard system errors
 */
TVerdict CEglTest_Benchmark_SwapBuffers::doTestStepPreambleL()
    {
    CEglTestStep::doTestStepPreambleL();
    iProfiler = CTProfiler::NewL(*this);
    //read parameters from config (WindowSize)
    CEglTestCommonIniSettings* iniParser = CEglTestCommonIniSettings::NewL();
    CleanupStack::PushL(iniParser);
    iWindowSize = iniParser->GetWindowSize(KSwapBuffersSection);
    if(iWindowSize == TSize(0,0))
        {
        ERR_PRINTF1(_L("The window size whether is not specified in INI file or is TSize(0,0), the test will not be executed!"));
        User::Leave(KErrArgument);      
        }
    CleanupStack::PopAndDestroy(iniParser);
    
    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    TESTL(iWs.Connect() == KErrNone);    
    iWindowGroup = RWindowGroup(iWs);
    TESTL(iWindowGroup.Construct(0) == KErrNone);  
    iWindow = RWindow(iWs);
    // The window is automatically fullscreen if it's a child of a window group
    TESTL(iWindow.Construct(iWindowGroup, reinterpret_cast<TUint32>(this)) == KErrNone);
    iWindow.SetSize(iWindowSize);
    // Window dimensions
    const TPoint KWindowPosition(30, 30);
    iWindow.SetPosition(KWindowPosition);
    iWindow.Activate();
        
    // Create display object
    CEglTestStep::GetDisplayL();
    ASSERT_EGL_TRUE(eglInitialize(iDisplay, 0, 0));
        
    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    eglChooseConfig(iDisplay, KConfigAttribs[2], matchingConfigs, 1, &numConfigs);
    if (numConfigs <= 0) // Abort the test if the EGL config is not supported
        {
        ERR_PRINTF1(KErrEglConfigNotSupported);
        SetTestStepError(KErrNotSupported);
        return TestStepResult();
        }
    
    // Use OpenVG to draw
    ASSERT_EGL_TRUE(eglBindAPI(EGL_OPENVG_API));
    
    // Create the window surface and the egl context and make them current
    iEglSurface = eglCreateWindowSurface(iDisplay, matchingConfigs[0], &iWindow, KPixmapAttribsVgAlphaFormatPre);
    ASSERT_EGL_TRUE(iEglSurface != EGL_NO_SURFACE);
    iEglContext = eglCreateContext(iDisplay, matchingConfigs[0], EGL_NO_CONTEXT, NULL);
    ASSERT_EGL_TRUE(iEglContext != EGL_NO_CONTEXT);
    ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, iEglSurface, iEglSurface, iEglContext));
    
    // Get the function pointer for eglSwapBuffersRegionNOK()
    iPfnEglSwapBuffersRegionNok = reinterpret_cast<PFNEGLSWAPBUFFERSREGIONNOKPROC>(eglGetProcAddress("eglSwapBuffersRegionNOK"));
    
    return TestStepResult();
    }

TVerdict CEglTest_Benchmark_SwapBuffers::doTestStepPostambleL()
    {
    // Make sure that this EGL status is active
    eglMakeCurrent(iDisplay, iEglSurface, iEglSurface, iEglContext);
    // Call eglMakeCurrent() to ensure the surfaces and contexts are truly destroyed
    eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (iEglContext != EGL_NO_CONTEXT)
        {
        eglDestroyContext(iDisplay, iEglContext);
        }
    if (iEglSurface != EGL_NO_SURFACE)
        {
        eglDestroySurface(iDisplay, iEglSurface);
        }   
    eglTerminate(iDisplay);
    eglReleaseThread();
    
    iWindow.Close();
    iWindowGroup.Close();
    iWs.Close();
    
    delete iProfiler;
    iProfiler = NULL;

    return CEglTestStep::doTestStepPostambleL();
    }

/**
 * Override of base class pure virtual function.
 * This implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value should be EPass.
 *
 * @return test framework code
 */
TVerdict CEglTest_Benchmark_SwapBuffers::doTestStepL()
    {
    // Tests  the performance of eglSwapBuffers()
    SetTestStepID(KTestStep0528);    
    TRAPD(err, EglSwapBufferL());
    if (err != KErrNone)
        {
        SetTestStepResult(EAbort);
        }    
    RecordTestResultL();
    
    // We only perform swap region benchmark test if eglSwapBuffersRegionNOK extension is supported
    if (iPfnEglSwapBuffersRegionNok != NULL)
        {
        // Tests the maximum performance of eglSwapBuffersRegionNOK()
        SetTestStepID(KTestStep0529);    
        TRAP(err, EglSwapBufferRegionL());
        if (err != KErrNone)
            {
            SetTestStepResult(EAbort);
            }    
        RecordTestResultL();
        
        // Stress tests the performance of eglSwapBuffersRegionNOK()
        SetTestStepID(KTestStep0530);    
        for (TInt noRects = KStressTestNoRectsStepSize; noRects <= KStressTestMaxNoRects; noRects += KStressTestNoRectsStepSize)
            {
            // TRAP here is on purpose, normally you shouldn't use it in loops
            TRAP(err, EglSwapBufferRegionStressL(noRects));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("EglSwapBufferRegionStressL (leave code: %d)."), err);
                SetTestStepResult(EAbort);
                }
            }
        RecordTestResultL();
        }
    else
        {
        INFO_PRINTF1(KEglSwapBuffersRegionNokMsg);
        }
    
    // Close the test and return result to the testframework
    CloseTMSGraphicsStep();    
    return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0528

@SYMTestPriority 1

@SYMPREQ 2677

@SYMTestCaseDesc
Tests how long it takes to swap window surface buffers if the whole surface is updated.

@SYMTestActions
Clear the window surface with alternating background colors, swap the surface buffers 
(using eglSwapBuffers extension)and measure how long it takes.

@SYMTestExpectedResults
Test should pass and print the average framerate to a log file.
*/
void CEglTest_Benchmark_SwapBuffers::EglSwapBufferL()
    {    
    //-------   start profiling
    iProfiler->InitResults();
    // Perform the test
    for(TInt i = KIterationsToTest; i > 0; --i)
        {
        // Clean the surface with the background color
        vgSetfv(VG_CLEAR_COLOR, 4, KClearColors[i % KMaxClearColors]);
        vgClear(0, 0, iWindowSize.iWidth, iWindowSize.iHeight);
        // Swap the surface buffers
        ASSERT_EGL_TRUE(eglSwapBuffers(iDisplay, iEglSurface));
        }
    // Mark the time and print the results to the log file
    iProfiler->MarkResultSetL();
    iProfiler->ResultsAnalysisFrameRate(KTestStep0528, 0, 0, 0,
            KIterationsToTest, iWindowSize.iWidth * iWindowSize.iHeight);   
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0529

@SYMTestPriority 1

@SYMPREQ 2677

@SYMTestCaseDesc
Tests how long it takes to swap window surface buffers if only a small region is updated. This
test should show the maximum possible performance increase.

@SYMTestActions
Clear the window surface with alternating background colors, swap the surface buffers
and measure how long it takes.

@SYMTestExpectedResults
Test should pass and print the average framerate to a log file.
The average time shall be made available in an easy-to-use format for further 
analysis and comparison.
*/
void CEglTest_Benchmark_SwapBuffers::EglSwapBufferRegionL()
    {
    // Region dimensions (top left hand corner and bottom right hand corner)
    const TRect KRegionRect(50, 50, 60, 60);
    // Rectangle for partial swap buffer function
    const EGLint rects[] = {KRegionRect.iTl.iX, KRegionRect.iTl.iY, KRegionRect.Width(), KRegionRect.Height()};
    // Number of rectangles (one rectangle consist of 4 values)
    const EGLint count = (sizeof(rects) / (sizeof(rects[0] * 4)));
            
    // We obtain the func ptr in doTestStepPreambleL and only execute this test if it is not null
    TESTL(iPfnEglSwapBuffersRegionNok != NULL);
    
    // Clear the surface
    vgSetfv(VG_CLEAR_COLOR, 4, KClearColors[0]);
    vgClear(0, 0, iWindowSize.iWidth, iWindowSize.iHeight);
    ASSERT_EGL_TRUE(eglSwapBuffers(iDisplay, iEglSurface));
    
    // Initialise uibench and reset the timer
    iProfiler->InitResults();
    // Perform the test
    for(TInt i = KIterationsToTest; i > 0; --i)
        {
        // Clean the surface with the background color
        vgSetfv(VG_CLEAR_COLOR, 4, KClearColors[i % KMaxClearColors]);
        vgClear(0, 0, iWindowSize.iWidth, iWindowSize.iHeight);
        // Swap the surface buffers
        ASSERT_EGL_TRUE(iPfnEglSwapBuffersRegionNok(iDisplay, iEglSurface, count, rects));
        }
    // Mark the time and print the results to the log file
    iProfiler->MarkResultSetL();
    iProfiler->ResultsAnalysisFrameRate(KTestStep0529, 0, 0, 0,
            KIterationsToTest, iWindowSize.iWidth * iWindowSize.iHeight);
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0530

@SYMTestPriority 1

@SYMPREQ 2677

@SYMTestCaseDesc
Stress test to show maximum possible performance increase when adding Rectangles to the region i.e. adding 100 rectangles with step size 5 

@SYMTestActions
Clear the window surface with alternating background colors, swap the surface buffers
and measure how long it takes.

@SYMTestExpectedResults
Test should pass and print the average framerate to a log file.

@Param aCount
Number of rectangles to add to the region
*/
void CEglTest_Benchmark_SwapBuffers::EglSwapBufferRegionStressL(EGLint aCount)
    {
    TInt* rects = static_cast<TInt*>(User::AllocLC(sizeof(TInt) * 4 * aCount));
    TInt actualRectCount = 0;
    TInt idx = 0;
    // Size of the dirty rectangles for the stress test
    const TSize KStressTestRectSize(10, 10);
    for (TInt y = 0; (y < iWindowSize.iHeight - KStressTestRectSize.iHeight - 1) && (actualRectCount < aCount); y += KStressTestRectSize.iHeight + KStressTestRectGap)
        {
        for (TInt x = 0; (x < iWindowSize.iWidth - KStressTestRectSize.iWidth - 1) && (actualRectCount < aCount); x += KStressTestRectSize.iWidth + KStressTestRectGap)
            {
            rects[idx++] = x;
            rects[idx++] = y;
            rects[idx++] = KStressTestRectSize.iWidth;
            rects[idx++] = KStressTestRectSize.iHeight;
            actualRectCount++;
            }
        }
    TESTL(actualRectCount > 0);
    if (actualRectCount != aCount)
        {
        WARN_PRINTF3(KWarnStressTestRectCount, actualRectCount, aCount);
        }
    
    // We obtain the func ptr in doTestStepPreambleL and only execute this test if it is not null
    TESTL(iPfnEglSwapBuffersRegionNok != NULL);
    
    // Clear the surface
    vgSetfv(VG_CLEAR_COLOR, 4, KClearColors[0]);
    vgClear(0, 0, iWindowSize.iWidth, iWindowSize.iHeight);
    ASSERT_EGL_TRUE(eglSwapBuffers(iDisplay, iEglSurface));
    
    // Initialise uibench and reset the timer
    iProfiler->InitResults();
    // Perform the test
    for(TInt i = KIterationsToTest; i > 0; --i)
        {
        // Clean the surface with the background color
        vgSetfv(VG_CLEAR_COLOR, 4, KClearColors[i % KMaxClearColors]);
        vgClear(0, 0, iWindowSize.iWidth, iWindowSize.iHeight);
        // Swap the surface buffers
        ASSERT_EGL_TRUE(iPfnEglSwapBuffersRegionNok(iDisplay, iEglSurface, actualRectCount, rects));
        }
    // Mark the time and print the results to the log file
    iProfiler->MarkResultSetL();
    INFO_PRINTF2(KInfoRectangles, aCount);
    iProfiler->ResultsAnalysisFrameRate(KTestStep0530, 0, 0, 0,
            KIterationsToTest, iWindowSize.iWidth * iWindowSize.iHeight);
    CleanupStack::PopAndDestroy(rects);
    }
