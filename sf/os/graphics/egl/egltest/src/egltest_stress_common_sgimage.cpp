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

#include "egltest_stress_common_sgimage.h"

void ChooseConfigAndCreateContextL(EGLDisplay& aDisplay, EGLContext& aContext, EGLConfig& aConfig, const RSgImage& aSgImage, const TDesC& aPanicString, TBool aPreMultAlpha)
    {
    EGLConfig configs[KMaxEglConfigs];
    EGLint numConfigs = 0;

    //If the aPixmap argument is ETrue, then the attributes are for a pixmap surface
    //Otherwise, the surface will be of type pbuffer. Note assumption on the position
    //of the EGL_MATCH_NATIVE_PIXMAP and EGL_SURFACE_TYPE attribute/value pair
    EGLint KAttrib_list_RSgImage[] = { EGL_MATCH_NATIVE_PIXMAP,    (TInt)&aSgImage,
                                       EGL_RENDERABLE_TYPE,       EGL_OPENVG_BIT,
                                       EGL_SURFACE_TYPE,          EGL_PIXMAP_BIT,
                                       EGL_NONE };
    
    if(aPreMultAlpha != EFalse)
        {
        KAttrib_list_RSgImage[5] |= EGL_VG_ALPHA_FORMAT_PRE_BIT;
        }

    EGL_LEAVE_ERROR(eglChooseConfig(aDisplay, KAttrib_list_RSgImage, configs, KMaxEglConfigs, &numConfigs));
    
    if(numConfigs > 0)
        {
        //Choose an arbitrary config
        aConfig = configs[0];        
        }
    else
        {
        User::Panic(aPanicString , KErrNotSupported);      
        }
    
    //One context is sufficient for all surfaces
    EGL_LEAVE_NULL(aContext, eglCreateContext(aDisplay, aConfig, EGL_NO_CONTEXT, NULL));
    }

EGLSurface CreatePixmapSurfaceL(EGLDisplay aDisplay, EGLConfig aConfig, const RSgImage& aSgImage, TBool aAlphaPre)
    {
    EGLSurface surface = EGL_NO_SURFACE;

    if(aAlphaPre == EFalse)
        {
        EGL_LEAVE_NULL(surface, eglCreatePixmapSurface(aDisplay, aConfig, (void*)&aSgImage, KPixmapAttribsVgAlphaFormatNonPre));
        }
    else
        {
        EGL_LEAVE_NULL(surface, eglCreatePixmapSurface(aDisplay, aConfig, (void*)&aSgImage, KPixmapAttribsVgAlphaFormatPre));
        }
    
    return surface;
    }

void GenerateVgImageL(const EGLDisplay aDisplay, RSgImage* aImage, VGImage& aVgImage)
    {
    EGLImageKHR eglImage = 0;

    TFPtrEglCreateImageKhr ipfnEglCreateImageKHR = reinterpret_cast<TFPtrEglCreateImageKhr>(eglGetProcAddress("eglCreateImageKHR"));
    EGL_LEAVE_ERROR(ipfnEglCreateImageKHR);

    TFPtrVgCreateEglImageTargetKhr ipfnvgCreateImageTargetKHR = reinterpret_cast<TFPtrVgCreateEglImageTargetKhr>(eglGetProcAddress("vgCreateEGLImageTargetKHR"));
    EGL_LEAVE_ERROR(ipfnvgCreateImageTargetKHR);

    TFPtrEglDestroyImageKhr ipfnEglDestroyImageKHR = reinterpret_cast<TFPtrEglDestroyImageKhr>(eglGetProcAddress("eglDestroyImageKHR"));
    EGL_LEAVE_ERROR(ipfnEglDestroyImageKHR);

    EGL_LEAVE_NULL(eglImage, ipfnEglCreateImageKHR(aDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, reinterpret_cast<EGLClientBuffer>(aImage), (int*)KEglImageAttribsPreservedTrue));
    EGL_LEAVE_NULL(aVgImage, ipfnvgCreateImageTargetKHR((VGeglImageKHR)eglImage));

    //Close the EGLImage
    EGL_LEAVE_ERROR(ipfnEglDestroyImageKHR(aDisplay, eglImage));
    }

void VgLeaveIfErrorL()
    {
    VGErrorCode ret = vgGetError();
    if(ret != VG_NO_ERROR)
        {
        User::Leave(ret);
        }
    }

/**
 *    class CTReadWrite
 *    A base class for use by both the main process and the child processes. Contains common
 *    data and functions that both children use. Caters for all stress test cases. The children 
 *    provide implementations of pure virtual functions, one for each test case. The function
 *    particular to each test case are called in the base class RunL() and these
 *    call the implemented virtual functions in either the main or the child process.
 *    The virtual functions read from and write to objects derived from a shared SgImage
 */
CTReadWrite::CTReadWrite(TInt aWidth, TInt aHeight, TInt aByteSize, VGImageFormat aFormat, const TTestType& aTestType, TBool& aTestPass)
: CTimer(CActive::EPriorityHigh),
    iWidth(aWidth),
    iHeight(aHeight),
    iByteSize(aByteSize),
    iFormat(aFormat),
    iTestType(aTestType),
    iTestPass(aTestPass)
    {
    }

CTReadWrite::~CTReadWrite()
    {
    delete[] iData;
    }

void CTReadWrite::ConstructL()
    {
    if(iByteSize == 2)
        {
        //Pack two bytes into a four byte value and halve the buffer size
        iInitialColour = (KColourInitial16 << 16) + KColourInitial16;
        iFinalColour = (KColourFinal16 << 16) + KColourFinal16;
        iBufferSize = iWidth*iHeight/2;
        }
    else
        {
        iInitialColour = KColourInitial32;
        iFinalColour = KColourFinal32;
        iBufferSize = iWidth*iHeight;
        }

    //Buffer must cater for an odd (as in not even) buffer size
    iData = new(ELeave) TUint32[iBufferSize + 1];

    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }

void CTReadWrite::RunL()
    {
    MakeCurrentL();

    switch(iTestType)
        {
        case EStressRead:
            {
            ReadL();
            break;
            }
        case EStressReadWriteSingleImage:
        case EStressReadWriteMultiImage:
            {
            ReadWriteImageL();
            break;
            }
        case EStressVGImage:
            {
            VgImageL();
            break;
            }
        case EStressPixmapSurface:
            {
            PixmapSurfaceL();
            break;
            }
        default:
            //Error
            User::Leave(KErrArgument);
            break;
        }
    }

void CTReadWrite::ReadL()
    {
    ReadFuncL();

    if(iFrameNumber != KNumberOfFrames)
         {
         iFrameNumber++;

         for(TInt i=0; i<iBufferSize; i++)
             {
             if(iData[i] != iInitialColour)
                 {
                 RDebug::Print(_L("Unexpected pixel colour %x"), iData[i]);
                 CActiveScheduler::Stop();
                 iTestPass = EFalse;
                 return;
                 }
             }
         //Re-issue the request
         After(TTimeIntervalMicroSeconds32(0));
         }
     else
         {
         //Stop the active scheduler and process with test termination
         CActiveScheduler::Stop();
         }
    }

void CTReadWrite::ReadWriteImageL()
    {
    ReadImageFuncL();

    TBool ret = EFalse; 
    for(TInt i=0; i<iBufferSize; i++)
        {
        if(iData[i] == iInitialColour)
            {
            iData[i] = iFinalColour;
            WriteImageFuncL();

            //Re-issue the request
            After(TTimeIntervalMicroSeconds32(0));
            
            ret = ETrue;
            break;
            }
        else if(iData[i] != iFinalColour)
            {
            CActiveScheduler::Stop();
            iTestPass = EFalse;
            
            ret = ETrue;
            break;
            }
        }

    //If no pixels have been modified, check to see if the test should finish
    if( (IsFinished() != EFalse) && (ret == EFalse) )
        {
        //Stop the active scheduler and process with test termination
        CActiveScheduler::Stop();
        }
    }

void CTReadWrite::VgImageL()
    {
    if(iFrameNumber != KNumberOfFrames)
          {
          iFrameNumber++;
          VgImageFuncL();

          //Re-issue the request
          After(TTimeIntervalMicroSeconds32(0));
          }
      else
          {
          //Stop the active scheduler and process with test termination
          CActiveScheduler::Stop();
          }
    }

void CTReadWrite::PixmapSurfaceL()
    {
    if(iFrameNumber != KNumberOfFrames)
          {
          iFrameNumber++;

          PixmapSurfaceFuncL();

          //Re-issue the request
          After(TTimeIntervalMicroSeconds32(0));
          return;
          }

    if(IsFinished() != EFalse)
        {
        //Stop the active scheduler and process with test termination
        CActiveScheduler::Stop();
        }
    }
