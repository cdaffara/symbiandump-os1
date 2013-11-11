// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __EGLTEST_OOM_SGIMAGE_H__
#define __EGLTEST_OOM_SGIMAGE_H__

#include "eglteststep.h"


NONSHARABLE_CLASS(CEglTest_OOM_Base) : public CEglTestStep
    {
public:
    ~CEglTest_OOM_Base();

protected:
    void CleanGraphicsResources();
    void CheckDeviation();
    void RetrieveExtensionDataL();
    void SendIndexToMainProcessL(TInt aIndex);
    // from CTestStep
    TVerdict doTestStepPreambleL();
    TVerdict doTestStepPostambleL();
    //from CEglTestStep
    virtual void ReceiveMessageFromClient(RMsgQueue<TEglStepMessageBuffer>& aMessageQueueClientProcParam);
    
private:
    TInt Deviation(const RArray<TInt>& aArray) const;
    void GetMinMax(const RArray<TInt>& aArray, TInt& aMin, TInt& aMax) const;
    void PrintEglResourceProfilingInfoL(); // if NOK_resource_profiling2 extension is 
                                          //available it outputs to the log file GPU memory usage

protected:
    TInt iNumIterations;
    TSize iImageSize;
    TUidPixelFormat iPixelFormat;
    RArray<EGLSurface> iSurfaces;
    RArray<EGLImageKHR> iEglImages;
    RArray<VGImage> iVgImages;
    RArray<RSgImage> iSgImages;
    
    TInt iLastIterationNumber;
private:
    RArray<TInt> iGPUUsedMemory;
    RArray<TInt> iLastIterations;
    
    //Deviation in percentage between max and min of GPU memory 
    //retrieved at the end of each attempt through NOK_resource_profiling2 egl extension inteface
    TInt iThresholdGPUUsedMemory;

    //Deviation in percentage between max and min of successful iteration number 
    //retrieved at the end of each attempt 
    TInt iThresholdLastIteration;
    
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#ifdef EGL_PROF_MEMORY_USAGE_THRESHOLD_NOK    
    PFNEGLQUERYPROFILINGDATANOKPROC iPFnEglQueryProfilingDataNOK;
#endif    
#endif    
    };

_LIT(KOOM_CloseVGImageWithTermination, "OOM_CloseVGImageWithTermination");
NONSHARABLE_CLASS(CEglTest_OOM_CloseVGImageWithTermination) : public CEglTest_OOM_Base
    {
public:
    // from CTestStep
    TVerdict doTestStepL();
    
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
   };

_LIT(KOOM_CloseVGImage, "OOM_CloseVGImage");
NONSHARABLE_CLASS(CEglTest_OOM_CloseVGImage) : public CEglTest_OOM_Base
    {
public:
    // from CTestStep
    TVerdict doTestStepL();
    
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
   };

_LIT(KOOM_ClosePixmapSurfaceWithTermination, "OOM_ClosePixmapSurfaceWithTermination");
NONSHARABLE_CLASS(CEglTest_OOM_ClosePixmapSurfaceWithTermination) : public CEglTest_OOM_Base
    {
public:
    // from CTestStep
    TVerdict doTestStepL();
    
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
   };

_LIT(KOOM_ClosePixmapSurface, "OOM_ClosePixmapSurface");
NONSHARABLE_CLASS(CEglTest_OOM_ClosePixmapSurface) : public CEglTest_OOM_Base
    {
public:
    // from CTestStep
    TVerdict doTestStepL();
    
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
   };

_LIT(KOOM_CloseSgImageDifferentProcess, "OOM_CloseSgImageDifferentProcess");
NONSHARABLE_CLASS(CEglTest_OOM_CloseSgImageDifferentProcess) : public CEglTest_OOM_Base
    {
public:
    // from CTestStep
    TVerdict doTestStepL();
    
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
   };

_LIT(KOOM_CloseSgImageSameThread, "OOM_CloseSgImageSameThread");
NONSHARABLE_CLASS(CEglTest_OOM_CloseSgImageSameThread) : public CEglTest_OOM_Base
    {
public:
    // from CTestStep
    TVerdict doTestStepL();
    };

#endif // __EGLTEST_OOM_SGIMAGE_H__
