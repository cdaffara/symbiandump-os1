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

#ifndef __EGLTEST_STRESS_PROCESS_SGIMAGE_H__
#define __EGLTEST_STRESS_PROCESS_SGIMAGE_H__

#include "egltest_stress_common_sgimage.h"

/**
Standard prototypes
*/
GLDEF_C TInt E32Main(void);
LOCAL_C void LaunchClientProcessL();

//Panic string
_LIT(KStressTestChildAppPanic, "Stress Test Child App");

class CTReadWriteChild : public CTReadWrite
    {
public:
    static CTReadWriteChild* NewL(VGImage aImage, TInt aWidth, TInt aHeight, TInt aByteSize, VGImageFormat aFormat, const TTestType& aTestType, TBool& aTestPass);

private:
    CTReadWriteChild(VGImage aImage, TInt aWidth, TInt aHeight, TInt aByteSize, VGImageFormat aFormat, const TTestType& aTestType, TBool& aTestPass);
    void ReadImageFuncL();
    void ReadFuncL();
    void WriteImageFuncL();
    void VgImageFuncL();
    void MakeCurrentL() const;
    TBool IsFinished();
    void PixmapSurfaceFuncL();

private:
    VGImage iImage;
    };

#endif //__EGLTEST_STRESS_PROCESS_SGIMAGE_H__
