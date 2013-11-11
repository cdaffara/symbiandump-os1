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

#include <s32file.h>
#include <test/t_simload.h>
#include <test/tefunit.h>    //ASSERT_TRUE
#include "egltest_stress_sgimage.h"

TVerdict CEglTest_Stress::doTestStepL()
    {
    INFO_PRINTF1(_L("CEglTest_Stress:doTestStepL()"));
    
    TBuf<100> testCaseId;
    TestCaseName(testCaseId);
    SetTestStepID(testCaseId);

#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    __UHEAP_MARK;
    
    //File server is used to generate simload ini file
    User::LeaveIfError(iFs.Connect());
  
    TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KVG_KHR_EGL_image | KEGL_KHR_image_pixmap);
    if(!ret)
        {
        // The extension is not supported
        RecordTestResultL();
        CloseTMSGraphicsStep();
        return TestStepResult();
        }

    //Create a config file and launch the simulated load app
    ReadIniValuesL();
    PrintConfigDataL();
    CreateSimLoadAppL();

    INFO_PRINTF1(_L("Creating a Semaphore to signal all child processes at once"));
    RSemaphore sem;
    User::LeaveIfError(sem.CreateGlobal(KEglStressTest(), iNumberChildProcesses));
    CleanupClosePushL(sem);

    //Information to be passed to the child process
    TSgImageInfo info;
    TSize size(iRSgImageWidth, iRSgImageHeight);
    info.iUsage = ESgUsageBitOpenVgImage | ESgUsageBitOpenVgSurface;
    info.iPixelFormat = iFormat;
    info.iSizeInPixels = size;

    //Utilise egl helper functions
    CreateEglSessionL();
    
    //Create a display and initialise it
    GetDisplayL();
    iEglSess->InitializeL();
    
    //Open RSGImage driver    
    iEglSess->OpenSgDriverL();
    
    EGL_LEAVE_ERROR(eglBindAPI(EGL_OPENVG_API));
 
    EGLContext context;
    for(TInt i=0; i<iNumberRSgImages; i++)
        {
        RSgImage image;
        iSgImageArray.InsertL(image, i);
        User::LeaveIfError(iSgImageArray[i].Create(info, NULL, NULL));

        EGLConfig config;
        if(i == 0)
            {
            //Only need to determine a matching configuration once
            ChooseConfigAndCreateContextL(iDisplay, context, config, iSgImageArray[i], KStressTestMainAppPanic, iAlphaPre);
            }
 
        EGLSurface surface = EGL_NO_SURFACE; //remove arm warning
        EGL_LEAVE_NULL(surface, CreatePixmapSurfaceL(iDisplay, config, iSgImageArray[i], iAlphaPre));

        EGL_LEAVE_ERROR(eglMakeCurrent(iDisplay, surface, surface, context));
         
        PaintSurfaceL(iDisplay, surface, context);
        EGL_LEAVE_ERROR(eglSwapBuffers(iDisplay, surface));

        if(iTestType == EStressVGImage)
            {
            EGL_LEAVE_ERROR(eglMakeCurrent(iDisplay, surface, surface, context));
            
            EGLImageKHR eglImage = 0; //removes arm compiler warning
            VGImage vgImage;

            EGL_LEAVE_NULL(eglImage, iEglSess->eglCreateImageKhrL(iDisplay, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, &iSgImageArray[i], (int*)KEglImageAttribsPreservedTrue));
            EGL_LEAVE_NULL(vgImage, iEglSess->vgCreateImageTargetKHR(eglImage));

            //Close the EGLImage
            EGL_LEAVE_ERROR(iEglSess->DestroyEGLImage(iDisplay, eglImage));
                        
            User::LeaveIfError(iVGImageArray.Insert(vgImage, i));
            }

        User::LeaveIfError(iSurfaceArray.Insert(surface, i));
        }

    /* Create and install the active scheduler */
    CActiveScheduler* sched = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CleanupStack::PushL(sched);

    TInt exitCounter = iNumberMainImages;
    TBool testResult = ETrue;

    //Create an active object for each RSgImage accessed in the main process
    CTReadWriteMain* painter = 0;
    for(TInt i=0; i<iNumberMainImages; i++)
        {
        if(iTestType == EStressVGImage)
            {
            painter = CTReadWriteMain::NewL(iDisplay, iSurfaceArray[i], context, iRSgImageWidth, iRSgImageHeight, iByteSize, iVgFormat, testResult, exitCounter, iTestType, iVGImageArray[i]);
            }
        else
            {
            painter = CTReadWriteMain::NewL(iDisplay, iSurfaceArray[i], context, iRSgImageWidth, iRSgImageHeight, iByteSize, iVgFormat, testResult, exitCounter, iTestType);            
            }
            
        CleanupStack::PushL(painter);
        painter->After(TTimeIntervalMicroSeconds32(0));
        }

    CreateChildProcessesL();

    INFO_PRINTF1(_L("Signaling all child processes at once - starts data access in the child processes"));
    sem.Signal(iNumberChildProcesses);

    //Start the active scheduler - starts data access in the main process
    sched->Start();

    if(testResult == EFalse)
        {
		if (iTestType == EStressReadWriteSingleImage || iTestType == EStressReadWriteMultiImage)
			{
			// For GRAPHICS-EGL-0428 and GRAPHICS-EGL-0437 data integrity cannot be guaranteed on 
			// all implementations, so the pixel value checking aspects of these tests are regarded as optional   
			WARN_PRINTF1(_L("Unexpected pixel colour"));
			}
		else
			{
			ERR_PRINTF1(_L("Unexpected pixel colour"));
			SetTestStepResult(EFail);
			}
        }

    //Check that each child process has completed without error
    for(TInt i=0; i<iNumberChildProcesses; i++)
       {
       TRequestStatus status;
       iProcessArray[i].Logon(status);
       User::WaitForRequest(status);

       if(status != KErrNone)
           {
		   if (status == KTestStressUnexpectedPixelError && (iTestType == EStressReadWriteSingleImage || iTestType == EStressReadWriteMultiImage))
			   {
			   // For GRAPHICS-EGL-0428 and GRAPHICS-EGL-0437 data integrity cannot be guaranteed on 
			   // all implementations, so the pixel value checking aspects of these tests are regarded as optional
			   // So check that if it fails, it fails for the expected reason of no matching pixel.
			   WARN_PRINTF2(_L("Child Process completed with code %d, expected KErrNone"), status.Int());
			   }
		   else
			   {
			   ERR_PRINTF2(_L("Child Process completed with code %d, expected KErrNone"), status.Int());
			   SetTestStepResult(EFail);
			   }
           }
       }

    DestroySimLoadProcess();
    DeleteConfigData();
    ClearDownArraysL(iDisplay);
    EGL_LEAVE_ERROR(eglDestroyContext(iDisplay, context));

    CleanupStack::PopAndDestroy(iNumberMainImages + 2, &sem); //(iNumberMainImages + 1) active objects, sched, sem
    iEglSess->CloseSgDriver();
    
    CleanAll();
    iFs.Close();

    __UHEAP_MARKEND;

#else
    INFO_PRINTF2(_L("%S can only be run with SgImage-Lite"), &testCaseId);
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    
    //Comply with the terrific TMS
    RecordTestResultL();
    CloseTMSGraphicsStep();

    return TestStepResult();
    }

CEglTest_Stress::~CEglTest_Stress()
    {
    //Free memory and close handles in case of panic in the doTestStepL()
    delete iEglSess;
    
    iFs.Close();
    iSurfaceArray.Close();
    iProcessArray.Close();
    iSgImageArray.Close();
    iVGImageArray.Close();
    }

void CEglTest_Stress::PaintSurfaceL(EGLDisplay aDisplay, EGLSurface aSurface, EGLContext aContext)
    {
    //Clear surface background, format of the constant is ARGB
    //Match the colour to that of the defined constant
    VGfloat red = 0.0;
    VGfloat green = 0.0;
    VGfloat blue = 0.0;
    VGfloat alpha = 0.0;
    
    //Care taken to avoid fixed width -> floating point -> fixed width rounding errors
    if(iByteSize == 4)
        {
        red = 1.0 * ((KColourInitial32 & 0x00FF0000) >> 16)/255;
        green = 1.0 * ((KColourInitial32 & 0x0000FF00) >> 8)/255;
        blue = 1.0 * (KColourInitial32 & 0x000000FF)/255;
        alpha = 1.0 * ((KColourInitial32 & 0xFF000000) >> 24)/255;        
        }
    else //iByteSize == 2
        {
        red = 1.0 * ((KColourInitial16 & 0x7C00) >> 11)/31;
        green = 1.0 * ((KColourInitial16 & 0x0480) >> 5)/63;
        blue = 1.0 * (KColourInitial16 & 0x001F)/31;
        }

    //Format of the constant is RGBA (32 bit) 
    VGfloat bgColor[] = {red, green, blue, alpha};

    EGL_LEAVE_ERROR(eglMakeCurrent(aDisplay, aSurface, aSurface, aContext));
 
    vgSetfv(VG_CLEAR_COLOR, 4, bgColor);
    ASSERT_VG_TRUE(vgGetError() == VG_NO_ERROR);
    vgClear(0, 0, iRSgImageWidth, iRSgImageHeight);
    ASSERT_VG_TRUE(vgGetError() == VG_NO_ERROR);
    }

void CEglTest_Stress::CreateChildProcessL(TInt aProcessNumber, TTestType aTestType, TSgDrawableId aDrawableId)
    {
    TRequestStatus status;
    RProcess client;
    User::LeaveIfError(client.Create(KStressTestClientApp, KNullDesC));

    //Pass image and test information to the child process
    TStressProcessInfo info;
    info.iTestType = aTestType;
    info.iSgId = aDrawableId;
    info.iByteSize = iByteSize;
    info.iAlphaPre = iAlphaPre;

    TPckg<TStressProcessInfo> pckgInfo(info);
    User::LeaveIfError((client.SetParameter(KMultiProcessSlot, pckgInfo)));

    client.Rendezvous(status);
    client.Resume();
    User::WaitForRequest(status);
    TEST(status == KErrNone);

    //Store image handle for cleanup
    iProcessArray.InsertL(client, aProcessNumber);
    }

void CEglTest_Stress::CreateChildProcessesL()
    {
    for(TInt i=0; i<iNumberChildProcesses; i++)
        {
        switch(iTestType)
            {
            case EStressReadWriteMultiImage:
            case EStressPixmapSurface:
                {
                //Each child process accesses one SgImage
                CreateChildProcessL(i, iTestType, iSgImageArray[i].Id());
                break;
                }
            case EStressRead:
            case EStressReadWriteSingleImage:
            case EStressVGImage:
                {
                //All child processes access the same SgImage
                CreateChildProcessL(i, iTestType, iSgImageArray[0].Id());
                break;
                }
            default:
                User::Panic(KStressTestMainAppPanic, KErrNotFound);
                break;
            }
        }
    }

void CEglTest_Stress::ClearDownArraysL(EGLDisplay aDisplay)
    {
    for(TInt i=0; i<iSurfaceArray.Count(); i++)
        {
        ASSERT_EGL_TRUE(eglDestroySurface(aDisplay, iSurfaceArray[i]));
        }
    
    for(TInt i=0; i<iVGImageArray.Count(); i++)
        {
        vgDestroyImage(iVGImageArray[i]);
        ASSERT_VG_TRUE(vgGetError() == VG_NO_ERROR);
        }
    
    for(TInt i=0; i<iProcessArray.Count(); i++)
        {
        iProcessArray[i].Close();
        }

    for(TInt i=0; i<iSgImageArray.Count(); i++)
        {
        iSgImageArray[i].Close();
        }
    
    iSurfaceArray.Close();
    iProcessArray.Close();
    iSgImageArray.Close();
    iVGImageArray.Close();
    }

void CEglTest_Stress::CreateSimLoadAppL()
    {
    TInt index = 1;
    TBuf<100> tempStore;

    //Three simload processes need to be launched t_simloadapp1.exe - t_simloadapp3.exe
    while(index <= KNumSimLoadApps)
        {
        tempStore.Format(KSimLoadApp, index++);
        CreateSimLoadProcessL(tempStore);
        }
    }

void CEglTest_Stress::CreateSimLoadProcessL(const TDesC& aApp)
    {
    INFO_PRINTF2(_L("Starting App: %S"), &aApp);

    RProcess process;
    User::LeaveIfError(process.Create(aApp, KNullDesC));

    //Give the simulated load high priority to be sure it does its job
    process.SetPriority(EPriorityHigh);
    TEST(process.Priority() == EPriorityHigh);
    INFO_PRINTF3(_L("Process Priority: Actual: %d, Expected: %d"), process.Priority(), EPriorityHigh);
    process.Resume();
    
    iProcessList.AppendL(process);
    }

void CEglTest_Stress::DestroySimLoadProcess()
    {
    for (TInt index = 0; index < iProcessList.Count(); index++)
        {
        // check process
        INFO_PRINTF3(_L("Process Check: Actual: %d, Expected: %d"), iProcessList[index].ExitReason(), KErrNone);
        TEST(iProcessList[index].ExitReason( )== KErrNone);

        // kill process
        iProcessList[index].Kill(KErrGeneral);
        INFO_PRINTF3(_L("Process Exit Reason: Actual: %d, Expected: %d"), iProcessList[index].ExitReason(), KErrGeneral);
        TEST(iProcessList[index].ExitReason() == KErrGeneral);

        iProcessList[index].Close();
        }

    iProcessList.Close();
    }

void CEglTest_Stress::ReadIniValueL(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult)
    {
    if(!GetIntFromConfig(aSectName, aKeyName, aResult))
        {
        ERR_PRINTF2(_L("Error reading %S value from ini file"), &aKeyName);
        User::Leave(KErrNotFound);
        }

    INFO_PRINTF3(_L("Ini file value %S = %d"), &aKeyName, aResult);
    }

void CEglTest_Stress::ReadIniValuesL()
    {
    ReadIniValueL(ConfigSection(), KNumberRSgImages, iNumberRSgImages);
    ReadIniValueL(ConfigSection(), KNumberMainImages, iNumberMainImages);
    ReadIniValueL(ConfigSection(), KNumberChildProcesses, iNumberChildProcesses);
    ReadIniValueL(ConfigSection(), KRSgImageWidth, iRSgImageWidth);
    ReadIniValueL(ConfigSection(), KRSgImageHeight, iRSgImageHeight);
    ReadIniValueL(ConfigSection(), KConfigSimLoadValue, iSimLoadValue);
    
    //Check for erroneous ini values
    ASSERT_TRUE(iNumberChildProcesses <= iNumberRSgImages);
    ASSERT_TRUE(iNumberMainImages <= iNumberRSgImages);
    ASSERT_TRUE(iRSgImageWidth >= 0);
    ASSERT_TRUE(iRSgImageHeight >= 0);
    ASSERT_TRUE(iSimLoadValue >= 0);
    ASSERT_TRUE(iSimLoadValue <= 100);
    
    TBuf16<100> buffer;
    TPtrC16 ptrBuffer(buffer);
    if(!GetStringFromConfig(ConfigSection(), KPixelFormat, ptrBuffer))
        {
        ERR_PRINTF2(_L("Error reading %S value from ini file"), &KPixelFormat);
        User::Leave(KErrNotFound);
        }

    INFO_PRINTF3(_L("Ini file value %S = %S"), &KPixelFormat, &ptrBuffer);

    //Derive information from the pixel format
    if(ptrBuffer == KUidPixelFormatARGB_8888)
        {
        iFormat = EUidPixelFormatARGB_8888;
        iVgFormat = VG_sARGB_8888;
        iAlphaPre = EFalse;
        iByteSize = 4;
        }
    else if(ptrBuffer == KUidPixelFormatARGB_8888_PRE)
        {
        iFormat = EUidPixelFormatARGB_8888_PRE;
        iVgFormat = VG_sARGB_8888_PRE;
        iAlphaPre = ETrue;
        iByteSize = 4;
        }
    else if(ptrBuffer == KUidPixelFormatRGB_565)
        {
        iFormat = EUidPixelFormatRGB_565;
        iVgFormat = VG_sRGB_565;
        iAlphaPre = EFalse;
        iByteSize = 2;
        }
    else
        {
        ERR_PRINTF2(_L("Unsupported pixel format %S"), &ptrBuffer);
        User::Leave(KErrNotFound);
        }

     //Determine the test type from the ini file section name
     if(ConfigSection().Find(KStressReadOnly) != KErrNotFound)
         {
         iTestType = EStressRead;
         }
     else if(ConfigSection().Find(KEStressReadWriteSingleImage) != KErrNotFound)
         {
         iTestType = EStressReadWriteSingleImage;
         }
     else if(ConfigSection().Find(KStressReadWriteMultiImage) != KErrNotFound)
         {
         iTestType = EStressReadWriteMultiImage;
         }
     else if(ConfigSection().Find(KStressVGImage) != KErrNotFound)
         {
         iTestType = EStressVGImage;
         }
     else if(ConfigSection().Find(KStressPixmapSurface) != KErrNotFound)
         {
         iTestType = EStressPixmapSurface;
         }
     else
         {
         ERR_PRINTF2(_L("Unknown test case %S"), &ptrBuffer);
         User::Leave(KErrNotFound);
         }
    }

/** 
 *   This function generates an ini file for the simulated load application
 *   containing two lines to determine
 *   1. The type of load (static or spiked, in this case always static)
 *   2. The simulated load level
 */
void CEglTest_Stress::PrintConfigDataL()
    {
    RFileWriteStream writer;
    writer.PushL();

    TInt err = iFs.MkDirAll(KSimLoadConfigFile);
    TEST(err == KErrNone || err == KErrAlreadyExists);
    INFO_PRINTF2(_L("Create Config File: %S"), &KSimLoadConfigFile);
    User::LeaveIfError(writer.Replace(iFs, KSimLoadConfigFile, EFileStreamText|EFileWrite));
    writer.CommitL();

    CleanupStack::PopAndDestroy(&writer);

    CIniData* data=CIniData::NewL(KSimLoadConfigFile);
    CleanupStack::PushL(data);

    INFO_PRINTF3(_L("Config Name: %S, \t\tConfig Data: %S"), &KConfigSimLoadType, &KConfigSimLoadStatic);
    err = data->AddValue(KDefaultSectionName, KConfigSimLoadType, KConfigSimLoadStatic);
    INFO_PRINTF3(_L("AddValue - Expected: %d, Actual: %d"), KErrNone, err);
    TEST(err == KErrNone);

    TBuf16<100> buffer;
    TPtrC16 ptrBuffer(buffer);
    User::LeaveIfError(GetStringFromConfig(ConfigSection(), KConfigSimLoadValue, ptrBuffer));

    INFO_PRINTF3(_L("Config Name: %S, \t\tConfig Data: %S"), &KConfigSimLoadValue, &ptrBuffer);
    err = data->AddValue(KDefaultSectionName, KConfigSimLoadValue, ptrBuffer);

    INFO_PRINTF3(_L("AddValue - Expected: %d, Actual: %d"), KErrNone, err);
    TEST(err == KErrNone);

    data->WriteToFileL();
    CleanupStack::PopAndDestroy(data);
    }

void CEglTest_Stress::DeleteConfigData()
    {
    INFO_PRINTF2(_L("Deleting Config File Name: %S"), &KSimLoadConfigFile);
    TInt err = iFs.Delete(KSimLoadConfigFile);
    TEST(err==KErrNone);
    }

/**
 *    class CTReadWriteMain
 *    A Child of CTReadWrite which contains member data not included in the base class and 
 *    implementations of pure virtual functions.
 *    a) One for each particular test case
 *    b) Support functions MakeCurrentL() and IsFinished()
 *    
 *    The base class is an active object and the implemented virtual functions are invoked
 *    indirectly from the RunL() function
 */
CTReadWriteMain::CTReadWriteMain(EGLDisplay aDisplay, EGLSurface aSurface, EGLContext aContext, TInt aWidth, TInt aHeight, TInt aByteSize, VGImageFormat aFormat, TBool& aTestPass, TInt& aFinishedCounter, const TTestType& aTestType)
: CTReadWrite(aWidth, aHeight, aByteSize, aFormat, aTestType, aTestPass),
    iDisplay(aDisplay),
    iSurface(aSurface),
    iContext(aContext),
    iFinishedCounter(aFinishedCounter)
    {
    }

CTReadWriteMain* CTReadWriteMain::NewL(EGLDisplay aDisplay, EGLSurface aSurface, EGLContext aContext, TInt aWidth, TInt aHeight, TInt aByteSize, VGImageFormat aFormat, TBool& aTestPass, TInt& aFinishedCounter, const TTestType& aTestType, VGImage aVGImage)
    {
    CTReadWriteMain* self = new (ELeave) CTReadWriteMain(aDisplay, aSurface, aContext, aWidth, aHeight, aByteSize, aFormat, aTestPass, aFinishedCounter, aTestType);
    CleanupStack::PushL(self);
    self->ConstructL(aVGImage);
    CleanupStack::Pop(self);
    return self;
    }

void CTReadWriteMain::ConstructL(VGImage aVGImage)
    {
    //NULL values indicate a programming error
    if( (iDisplay == EGL_NO_DISPLAY) || (iSurface == EGL_NO_SURFACE) || (iContext == EGL_NO_CONTEXT) || (iFinishedCounter == 0) || ((iTestType == EStressVGImage) && (aVGImage == NULL)) )
        {
        User::Leave(KErrArgument);
        }
    
    iVGImage = aVGImage;
    
    //Call base class function to complete construction
     CTReadWrite::ConstructL();
    }

void CTReadWriteMain::MakeCurrentL() const
    {
    EGL_LEAVE_ERROR(eglMakeCurrent(iDisplay, iSurface, iSurface, iContext));
    }

void CTReadWriteMain::ReadImageFuncL()
    {
    vgReadPixels(iData, iWidth*iByteSize, iFormat, 0, 0, iWidth, iHeight);
    VgLeaveIfErrorL();
    }

void CTReadWriteMain::ReadFuncL()
    {
    ReadImageFuncL();
    }

void CTReadWriteMain::WriteImageFuncL()
    {
    vgWritePixels(iData, iWidth*iByteSize, iFormat, 0, 0, iWidth, iHeight);
    VgLeaveIfErrorL();
    }

TBool CTReadWriteMain::IsFinished()
    {
    iFinishedCounter--;

    if(iFinishedCounter <= 0)
        {
        return ETrue;
        }

    return EFalse;
    }

void CTReadWriteMain::VgImageFuncL()
    {
    //Alter the image data, actual pixel values are not important
    for(TInt i=0; i<iBufferSize; i++)
        {
        const TUint32 temp = iBufferSize % iFrameNumber;
        iData[i] = temp + (temp << 8) + (temp << 16) + (temp << 24);
        }
    
    //Currently panics as context->scanlinebuffer is NULL
    vgImageSubData(iVGImage, iData, iWidth*iByteSize, iFormat, 0, 0, iWidth, iHeight);
    VgLeaveIfErrorL();
    
    EGL_LEAVE_ERROR(eglSwapBuffers(iDisplay, iSurface));
    }

void CTReadWriteMain::PixmapSurfaceFuncL()
    {
    // clear surface background to an arbitrary colour
    VGfloat arbitraryColour = (1.0*iFrameNumber)/KNumberOfFrames;
    VGfloat backgroundColour[] = {1.0 - arbitraryColour/2, arbitraryColour/2, 1.0 - arbitraryColour, arbitraryColour};
    vgSetfv(VG_CLEAR_COLOR, 4, backgroundColour);
    VgLeaveIfErrorL();
    vgClear(0, 0, iWidth, iHeight);
    VgLeaveIfErrorL();

    EGL_LEAVE_ERROR(eglSwapBuffers(iDisplay, iSurface));
    }
