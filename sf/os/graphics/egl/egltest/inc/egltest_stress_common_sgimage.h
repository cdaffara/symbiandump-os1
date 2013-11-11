// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __EGLTEST_STRESS_COMMON_SGIMAGE_H__
#define __EGLTEST_STRESS_COMMON_SGIMAGE_H__

#include <test/egltestcommonsgimageinfo.h>
#include <test/egltestcommonprocess.h>

#define EGL_LEAVE_ERROR(PASS) { if (!(PASS)) { User::Leave (eglGetError()); } }
#define EGL_LEAVE_NULL(RET, FUNC) { EGLint res = FUNC; if(!(res)){ User::Leave (eglGetError()); } else{RET = res;} }

_LIT(KTestType, "TestType");
_LIT(KEglStressTest, "EglStressTest Semaphore");
const TInt KTestStressUnexpectedPixelError = -911;


//Use to pass RSGImage ID to the child processes
const TInt KMultiProcessSlot = 1;

//Supported formats for these stress tests
const TUint32 KColourInitial32 = 0xFF80A0C0;
const TUint32 KColourFinal32 = 0xFFC0407F;
const TUint16 KColourInitial16 = 0x2408;
const TUint16 KColourFinal16 = 0x6144;

const TInt KNumberOfFrames = 100;

void ChooseConfigAndCreateContextL(EGLDisplay& aDisplay, EGLContext& aContext, EGLConfig& aConfig, const RSgImage& aSgImage, const TDesC& aPanicString, TBool aPreMultAlpha);
EGLSurface CreatePixmapSurfaceL(EGLDisplay aDisplay, EGLConfig aConfig, const RSgImage& aSgImage, TBool aAlphaPre);
void GenerateVgImageL(const EGLDisplay aDisplay, RSgImage* aImage, VGImage& aVgImage);
void VgLeaveIfErrorL();

typedef enum TTestType
    {
    EStressRead,
    EStressReadWriteSingleImage,
    EStressReadWriteMultiImage,
    EStressVGImage,
    EStressPixmapSurface,
    }TTestType;

struct TStressProcessInfo
    {
    TTestType iTestType;
    TInt iByteSize;
    TSgDrawableId iSgId;
    TBool iAlphaPre;
    };

class CTReadWrite : public CTimer
    {
public:
    ~CTReadWrite();

protected:
    CTReadWrite(TInt aWidth, TInt aHeight, TInt aByteSize, VGImageFormat aFormat, const TTestType& aTestType, TBool& aTestPass);
    void RunL();
    void ConstructL();
    void ReadL();
    void ReadWriteImageL();
    void PixmapSurfaceL();
    virtual void ReadImageFuncL() = 0;
    virtual void ReadFuncL() = 0;
    virtual void WriteImageFuncL() = 0;
    virtual void VgImageFuncL() = 0;
    virtual void MakeCurrentL() const = 0;
    virtual TBool IsFinished() = 0;
    virtual void PixmapSurfaceFuncL() = 0;

private:
    void VgImageL();

protected:
    TInt iWidth;
    TInt iHeight;
    TInt iByteSize;
    VGImageFormat iFormat;
    TTestType iTestType;
    TBool& iTestPass;
    TUint32 iInitialColour;
    TUint32 iFinalColour;
    TInt iBufferSize;
    TInt iFrameNumber;
    TUint32* iData;
    };

#endif //__EGLTEST_STRESS_COMMON_SGIMAGE_H__
