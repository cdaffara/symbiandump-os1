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
#ifndef __EGLTEST_BENCHMARK_SGIMAGE_H__
#define __EGLTEST_BENCHMARK_SGIMAGE_H__

#include "eglteststep.h"

class CTProfiler;

NONSHARABLE_CLASS(CEglTest_Benchmark_Base) : public CEglTestStep
    {
public: 
    ~CEglTest_Benchmark_Base();
    // from CTestStep
    TVerdict doTestStepPreambleL();
    TVerdict doTestStepPostambleL();
protected:
    CTProfiler* iProfiler;
    TInt iNumIterations;
    TSize iImageSize;
    TUidPixelFormat iPixelFormat;
    };

_LIT(KBenchmark_CreateCloseImage, "Benchmark_CreateCloseImage");
NONSHARABLE_CLASS(CEglTest_Benchmark_CreateCloseImage) : public CEglTest_Benchmark_Base
    {
public: 
    // from CTestStep
    TVerdict doTestStepL();
    };

_LIT(KBenchmark_Multi_Process_CreateCloseImage, "Benchmark_Multi_Process_CreateCloseImage");
NONSHARABLE_CLASS(CEglTest_Benchmark_Multi_Process_CreateCloseImage) : public CEglTest_Benchmark_Base
    {
public:
    // from CTestStep
    TVerdict doTestStepL();
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
   };

_LIT(KBenchmark_DrawImage, "Benchmark_DrawImage");
NONSHARABLE_CLASS(CEglTest_Benchmark_DrawImage) : public CEglTest_Benchmark_Base
    {
public: 
    // from CTestStep
    TVerdict doTestStepL();
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
    };

#endif // __EGLTEST_BENCHMARK_SGIMAGE_H__
