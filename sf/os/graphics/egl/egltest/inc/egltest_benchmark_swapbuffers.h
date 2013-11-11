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
*/
 
#ifndef EGLTEST_BENCHMARK_SWAPBUFFERS_H
#define EGLTEST_BENCHMARK_SWAPBUFFERS_H

#include "eglteststep.h"
#include <EGL/egl.h>

class CTProfiler;

NONSHARABLE_CLASS(CEglTest_Benchmark_SwapBuffers) : public CEglTestStep
	{
public:
    CEglTest_Benchmark_SwapBuffers();
    virtual ~CEglTest_Benchmark_SwapBuffers();
	
private:
	// From CTestStep
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
	
	void EglSwapBufferL();
	void EglSwapBufferRegionL();
	void EglSwapBufferRegionStressL(EGLint aCount);
	
private:
    RWsSession iWs;
    RWindowGroup iWindowGroup;
    RWindow iWindow;
    
    EGLSurface iEglSurface;
    EGLContext iEglContext;
    CTProfiler* iProfiler;
    TSize iWindowSize;

    PFNEGLSWAPBUFFERSREGIONNOKPROC iPfnEglSwapBuffersRegionNok;
	};
	
// Testname, is used by the test server and the test scripts to identify the test
_LIT(KBenchmark_SwapBuffers, "Benchmark_SwapBuffers");

#endif
