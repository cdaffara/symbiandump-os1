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

#ifndef __EGLTEST_STRESS_SGIMAGE_H__
#define __EGLTEST_STRESS_SGIMAGE_H__

#include <VG/openvg.h>
#include <test/egltestcommonstep.h>
#include "egltest_stress_common_sgimage.h"

//Test step names
_LIT(KStressPixmapSurface, "StressPixmapSurface");
_LIT(KStressVGImage, "StressVGImage");
_LIT(KStressReadOnly, "StressReadOnly");
_LIT(KEStressReadWriteSingleImage, "StressReadWriteSingleImage");
_LIT(KStressReadWriteMultiImage, "StressReadWriteMultiImage");

//ini file parameters
_LIT(KPixelFormat, "PixelFormat");
_LIT(KNumberRSgImages, "NumberRSgImages");
_LIT(KNumberChildProcesses, "NumberChildProcesses");
_LIT(KNumberMainImages, "NumberMainImages");
_LIT(KRSgImageWidth, "RSgImageWidth");
_LIT(KRSgImageHeight, "RSgImageHeight");

//default section for sim load app ini file
_LIT(KDefaultSectionName,"default");

//supported pixel formats for these test cases
_LIT(KUidPixelFormatARGB_8888, "PixelFormatARGB_8888");
_LIT(KUidPixelFormatARGB_8888_PRE, "PixelFormatARGB_8888_PRE");
_LIT(KUidPixelFormatRGB_565, "PixelFormatRGB_565");

//Child application
_LIT(KStressTestClientApp, "z:\\sys\\bin\\eglteststressprocesssgimage.exe");

//Panic string
_LIT(KStressTestMainAppPanic, "Stress Test Main App");

_LIT(KStress, "Stress");
NONSHARABLE_CLASS(CEglTest_Stress) : public CEglTestStep
    {
public:
    ~CEglTest_Stress();
    TVerdict doTestStepL();

private:
    void PaintSurfaceL(EGLDisplay aDisplay, EGLSurface aSurface, EGLContext aContext);
    void CreateChildProcessL(TInt aProcessNumber,  TTestType aTestType, TSgDrawableId aDrawableId);
    void CreateChildProcessesL();
    void ClearDownArraysL(EGLDisplay aDisplay);
    void CreateSimLoadAppL();
    void CreateSimLoadProcessL(const TDesC& aApp);
    void DestroySimLoadProcess();
    void ReadIniValueL(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult);
    void ReadIniValuesL();
    void PrintConfigDataL();
    void DeleteConfigData();

private:
    TPtrC iPixelFormat;
    TInt iNumberRSgImages;
    TInt iNumberChildProcesses;
    TInt iNumberMainImages;
    TInt iRSgImageWidth;
    TInt iRSgImageHeight;
    TInt iSimLoadValue;
    TInt iByteSize;
    TUidPixelFormat iFormat;
    VGImageFormat iVgFormat;
    TBool iAlphaPre;
    TTestType iTestType;
    RArray<RSgImage> iSgImageArray;
    RArray<VGImage> iVGImageArray;
    RArray<EGLSurface> iSurfaceArray;
    RArray<RProcess> iProcessArray;
    RArray<RProcess> iProcessList;
    RFs iFs;
};

class CTReadWriteMain : public CTReadWrite
    {
public:
    static CTReadWriteMain* NewL(EGLDisplay aDisplay, EGLSurface aSurface, EGLContext aContext, TInt aWidth, TInt aHeight, TInt aByteSize, VGImageFormat aFormat, TBool& aTestPass, TInt& aFinishedCounter, const TTestType& aTestType, VGImage aVGImage = 0);

private:
    CTReadWriteMain(EGLDisplay aDisplay, EGLSurface aSurface, EGLContext aContext, TInt aWidth, TInt aHeight, TInt aByteSize, VGImageFormat aFormat, TBool& aTestPass, TInt& aFinishedCounter, const TTestType& aTestType);
    void ConstructL(VGImage aVGImage);
    void ReadImageFuncL();
    void ReadFuncL();
    void WriteImageFuncL();
    void VgImageFuncL();
    void MakeCurrentL() const;
    TBool IsFinished();
    void PixmapSurfaceFuncL();

private:
    EGLDisplay iDisplay;
    EGLSurface iSurface;
    EGLContext iContext;
    TInt& iFinishedCounter;
    VGImage iVGImage;
    };

#endif //__EGLTEST_STRESS_SGIMAGE_H__
