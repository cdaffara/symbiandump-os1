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
#include "eglteststep.h"

#include "egltest_general.h"
#include "egltest_geterroranddisplay.h"
#include "egltest_syncobject.h"
#include "egltest_nativepixmaptype.h"
#include "egltest_createpixmapsurface.h"
#include "egltest_image.h"
#include "egltest_sibling.h"
#include "egltest_image_negative.h"
#include "egltest_vgimagetosurfaces.h"
#include "egltest_image_multithread.h"
#include "egltest_image_multiprocess.h"
#include "egltest_benchmark_sgimage.h"
#include "egltest_oom_sgimage.h"
#include "egltest_stress_sgimage.h"
#include "egltest_benchmark_swapbuffers.h"
#include "egltest_surfacescaling.h"

/*
Entry point from egltest to create test step
*/
EXPORT_C CEglTestStep* EglTestStepFactory::GetEglTestStep(const TDesC& aStepName)
// static
	{
	CEglTestStep* testStep = NULL;

	// General tests
	if (aStepName == KDumpStrings)																	testStep = new CEglTest_DumpStrings;
	else if (aStepName == KQueryString_Extensions)													testStep = new CEglTest_QueryString_Extensions;
    else if (aStepName == KEglTerminate_Negative)                                                   testStep = new CEglTest_EglTerminate_Negative;
    else if (aStepName == KGeneral_Negative_ProcAddress)                                            testStep = new CEglTest_General_Negative_ProcAddress;

    // GetError and Display tests
    else if (aStepName == KGetError)                                                                testStep = new CEglTest_GetError;
    else if (aStepName == KGetErrorMultiThread)                                                     testStep = new CEglTest_GetErrorMultiThread;
    else if (aStepName == KDisplay_Positive_Basic)                                                  testStep = new CEglTest_Display_Positive_Basic;
    else if (aStepName == KDisplay_Negative_InvalidDisplay)                                         testStep = new CEglTest_Display_Negative_InvalidDisplay;
    else if (aStepName == KDisplay_Negative_NotInitializedDisplay)                                  testStep = new CEglTest_Display_Negative_NotInitializedDisplay;   
    else if (aStepName == KDisplay_Positive_ReinitializeDisplay)                                    testStep = new CEglTest_Display_Positive_ReinitializeDisplay;
    else if (aStepName == KDisplay_Positive_MultipleInitialization)                                 testStep = new CEglTest_Display_Positive_MultipleInitialization;
    else if (aStepName == KDisplay_Positive_MultipleTermination)                                    testStep = new CEglTest_Display_Positive_MultipleTermination;
    else if (aStepName == KDisplay_Positive_Multithread_Basic)                                      testStep = new CEglTest_Display_Positive_Multithread_Basic;
    else if (aStepName == KDisplay_Positive_Multithread_InitTerminateInOneTread)                    testStep = new CEglTest_Display_Positive_Multithread_InitTerminateInOneTread;
    else if (aStepName == KDisplay_Positive_Multithread_InitTerminateFromDifferentThread)           testStep = new CEglTest_Display_Positive_Multithread_InitTerminateFromDifferentThread;
    else if (aStepName == KDisplay_Positive_Multithread_Stress)                                     testStep = new CEglTest_Display_Positive_Multithread_Stress;
    else if (aStepName == KDisplay_Positive_Multiprocess_Basic)                                     testStep = new CEglTest_Display_Positive_Multiprocess_Basic;
    else if (aStepName == KDisplay_OOM_ClientHeap)                                                  testStep = new CEglTest_Display_OOM_ClientHeap;
    else if (aStepName == KDisplay_OOM_ClientHeap_Initialize)                                       testStep = new CEglTest_Display_OOM_ClientHeap_Initialize;

	// SyncObject tests excluding WaitFlush which requires OpenVG and OpenGL ES
	else if (aStepName == KSyncObject_Positive_GetProcAddress)										testStep = new CEglTest_SyncObject_Positive_GetProcAddress;
	else if (aStepName == KSyncObject_Positive_CreateDestroy)										testStep = new CEglTest_SyncObject_Positive_CreateDestroy;
	else if (aStepName == KSyncObject_Positive_WaitSignal)											testStep = new CEglTest_SyncObject_Positive_WaitSignal;
	else if (aStepName == KSyncObject_Positive_WaitSignal2)											testStep = new CEglTest_SyncObject_Positive_WaitSignal2;
	else if (aStepName == KSyncObject_Positive_WaitDelete)											testStep = new CEglTest_SyncObject_Positive_WaitDelete;
	else if (aStepName == KSyncObject_Positive_WaitWithTimeoutExpired)								testStep = new CEglTest_SyncObject_Positive_WaitWithTimeoutExpired;
	else if (aStepName == KSyncObject_Positive_SignalBeforeTimeoutExpired)							testStep = new CEglTest_SyncObject_Positive_SignalBeforeTimeoutExpired;
	else if (aStepName == KSyncObject_Positive_DeleteBeforeTimeoutExpired)							testStep = new CEglTest_SyncObject_Positive_DeleteBeforeTimeoutExpired;
	else if (aStepName == KSyncObject_Positive_Multithread_SignalBeforeTimeout)						testStep = new CEglTest_SyncObject_Positive_Multithread_SignalBeforeTimeout;
	else if (aStepName == KSyncObject_Positive_Multithread_SignalAfterTimeout)						testStep = new CEglTest_SyncObject_Positive_Multithread_SignalAfterTimeout;
	else if (aStepName == KSyncObject_Positive_Multithread_WaitOnTwoSyncObject)						testStep = new CEglTest_SyncObject_Positive_Multithread_WaitOnTwoSyncObject;
	else if (aStepName == KSyncObject_Positive_Wait_TestMode)										testStep = new CEglTest_SyncObject_Positive_Wait_TestMode;
	else if (aStepName == KSyncObject_Positive_Multithread_Deletion)								testStep = new CEglTest_SyncObject_Positive_Multithread_Deletion;
	else if (aStepName == KSyncObject_Positive_Terminate)                                           testStep = new CEglTest_SyncObject_Positive_Terminate;
	else if (aStepName == KSyncObject_Positive_TerminateBeforeTimeoutExpired)                       testStep = new CEglTest_SyncObject_Positive_TerminateBeforeTimeoutExpired; 
	else if (aStepName == KSyncObject_Positive_Stress)												testStep = new CEglTest_SyncObject_Positive_Stress;
	else if (aStepName == KSyncObject_Negative_WrongParameters)										testStep = new CEglTest_SyncObject_Negative_WrongParameters;
	else if (aStepName == KSyncObject_Negative_SignalImpl)											testStep = new CEglTest_SyncObject_Negative_SignalImpl;
	else if (aStepName == KSyncObject_Negative_OOM)													testStep = new CEglTest_SyncObject_Negative_OOM;
	// SyncObjecte WaitFlush test is implementation specific as it uses OpenVG and OpenGL ES
	else if (aStepName == KSyncObject_Positive_WaitFlush)                                           testStep = new CEglTest_SyncObject_Positive_WaitFlush;

	// NativePixmapType tests
	else if (aStepName == KNativePixmapType_SingleThread_Positive_RSgImage)							testStep = new CEglTest_NativePixmapType_SingleThread_Positive_RSgImage;	
	else if (aStepName == KNativePixmapType_MultiThread_Positive_RSgImage_InitTwice)				testStep = new CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice;
	else if (aStepName == KNativePixmapType_MultiThread_Positive_RSgImage_InitTwice_NoRendezvous)	testStep = new CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice_NoRendezvous;
	else if (aStepName == KNativePixmapType_MultiThread_Positive_RSgImage_InitOneThreadOnly)		testStep = new CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitOneThreadOnly;	
	else if (aStepName == KNativePixmapType_MultiProcess_Both_RSgImage)								testStep = new CEglTest_NativePixmapType_MultiProcess_Both_RSgImage;
	else if (aStepName == KChooseConfig_EGL_MATCH_NATIVE_PIXMAP_KHR)								testStep = new CEglTest_ChooseConfig_EGL_MATCH_NATIVE_PIXMAP_KHR;
	else if (aStepName == KCFbsBitmap_EGL_MATCH_NATIVE_PIXMAP_KHR)									testStep = new CEglTest_CFbsBitmap_EGL_MATCH_NATIVE_PIXMAP_KHR;

	// CreatePixmapSurface tests
	else if (aStepName == KCreatePixmapSurface_RSgImage_Negative_RSgImage_NotInitialised)			testStep = new CEglTest_CreatePixmapSurface_RSgImage_Negative_RSgImage_NotInitialised;
	else if (aStepName == KCreatePixmapSurface_Negative_Invalid_Pixmap_Type)						testStep = new CEglTest_CreatePixmapSurface_Negative_Invalid_Pixmap_Type;
	else if (aStepName == KCreatePixmapSurface_Alpha_bit_RSgImage)									testStep = new CEglTest_CreatePixmapSurface_Alpha_bit_RSgImage;
    else if (aStepName == KCreatePixmapSurface_RSgImage_Positive_MantadoryFormatUsageSupport)       testStep = new CEglTest_CreatePixmapSurface_RSgImage_Positive_MantadoryFormatUsageSupport;

	// Image tests
	else if (aStepName == KEGL_Image_SgDriverHandle)												testStep = new CEglTest_EGL_Image_SgDriverHandle;
	else if (aStepName == KEGL_Image_RSgImage_UseOpenVG_PersistImageData)							testStep = new CEglTest_EGL_Image_RSgImage_UseOpenVG_PersistImageData;
	else if (aStepName == KEGL_Image_Consistent_Linked_List)										testStep = new CEglTest_EGL_Image_Consistent_Linked_List;	

	// Image negative tests
	else if (aStepName == KEGL_Image_eglCreateImage_Bad_Parameter)									testStep = new CEglTest_EGL_Image_eglCreateImage_Bad_Parameter;
	else if (aStepName == KEGL_Image_UsageBits_Enforcement)											testStep = new CEglTest_EGL_Image_UsageBits_Enforcement;
	else if (aStepName == KEGL_Image_DestroyImageKHR)												testStep = new CEglTest_EGL_Image_DestroyImageKHR;	
	else if (aStepName == KEGL_Image_VGImage_From_Invalid_EGLHandle)								testStep = new CEglTest_EGL_Image_VGImage_From_Invalid_EGLHandle;
	else if (aStepName == KEGL_Image_Self_Drawing)													testStep = new CEglTest_EGL_Image_Self_Drawing;

	// Siblings tests
	else if (aStepName == KEGL_Image_Sibling_Basic)													testStep = new CEglTest_EGL_Image_Sibling_Basic;
	else if (aStepName == KEGL_Image_Sibling_VGImage)												testStep = new CEglTest_EGL_Image_Sibling_VGImage;
	else if (aStepName == KEGL_Image_Sibling_Two_VGImages)											testStep = new CEglTest_EGL_Image_Sibling_Two_VGImages;
	else if (aStepName == KEGL_Image_Sibling_VGImage_Child)											testStep = new CEglTest_EGL_Image_Sibling_VGImage_Child;
	else if (aStepName == KEGL_Image_Sibling_VGImage_Child_CheckContents)							testStep = new CEglTest_EGL_Image_Sibling_VGImage_Child_CheckContents;
	else if (aStepName == KEGL_Image_Sibling_VGImage_Child_CheckContents_CloseSgEarlier)			testStep = new CEglTest_EGL_Image_Sibling_VGImage_Child_CheckContents_CloseSgEarlier;
	else if (aStepName == KEGL_Image_CreatePBufferFromClient_With_Sibling_VGImage)					testStep = new CEglTest_EGL_Image_CreatePBufferFromClient_With_Sibling_VGImage;

	// VgImage to surfaces tests
	else if (aStepName == KEGL_Image_VgImage_To_Pixmap)												testStep = new CEglTest_EGL_Image_VgImage_To_Pixmap;
	else if (aStepName == KEGL_Image_VGImage_To_Pixmap_CFbs)										testStep = new CEglTest_EGL_Image_VGImage_To_Pixmap_CFbs;
	else if (aStepName == KEGL_Image_VgImage_To_pBuffer)											testStep = new CEglTest_EGL_Image_VgImage_To_pBuffer;
	else if (aStepName == KEGL_Image_VgImage_To_Window)												testStep = new CEglTest_EGL_Image_VgImage_To_Window;

	//MultiThreaded
	else if (aStepName == KEGL_Image_Multi_Thread_Parallel)											testStep = new CEglTest_EGL_Image_Multi_Thread_Parallel;
	else if (aStepName == KEGL_Image_Multi_Thread_Sibling_Basic)									testStep = new CEglTest_EGL_Image_Multi_Thread_Sibling_Basic;
	else if (aStepName == KEGL_Image_Multi_Thread_Sibling_VGImage)									testStep = new CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage;
	else if (aStepName == KEGL_Image_Multi_Thread_Sibling_VGImage_PassingEGLImage)					testStep = new CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage_PassingEGLImage;
	else if (aStepName == KEGL_Image_Multi_Thread_Exit_Thread)										testStep = new CEglTest_EGL_Image_Multi_Thread_Exit_Thread;
	else if (aStepName == KEGL_Image_Multi_Thread_DrawAfterTerminate)								testStep = new CEglTest_EGL_Image_Multi_Thread_DrawAfterTerminate;
	
    //MultiProcess
	else if (aStepName == KEGL_Image_Multi_Process_Parallel)										testStep = new CEglTest_EGL_Image_Multi_Process_Parallel;
	else if (aStepName == KEGL_Image_Multi_Process_Sibling_Basic)									testStep = new CEglTest_EGL_Image_Multi_Process_Sibling_Basic;
	else if (aStepName == KEGL_Image_Multi_Process_Sibling_CheckContents)							testStep = new CEglTest_EGL_Image_Multi_Process_Sibling_CheckContents;
	else if (aStepName == KEGL_Image_Multi_Process_VgImage_Source)									testStep = new CEglTest_EGL_Image_Multi_Process_VgImage_Source;
	else if (aStepName == KEGL_Image_Multi_Process_VgImage_DrawAfterTerminate)						testStep = new CEglTest_EGL_Image_Multi_Process_VgImage_DrawAfterTerminate;
	else if (aStepName == KEGL_Image_Multi_Process_FontServer_Upfront)								testStep = new CEglTest_EGL_Image_Multi_Process_FontServer_Upfront;
	else if (aStepName == KEGL_Image_Multi_Process_FontServer_Deferred)								testStep = new CEglTest_EGL_Image_Multi_Process_FontServer_Deferred;
	else if (aStepName == KEGL_Image_Multi_Process_ThemeServer)										testStep = new CEglTest_EGL_Image_Multi_Process_ThemeServer;
	else if (aStepName == KEGL_Image_Multi_Process_VgImage_ProcessTerminate)						testStep = new CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminate;
	else if (aStepName == KEGL_Image_Multi_Process_VgImage_ProcessTerminateNegative)				testStep = new CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminateNegative;
	else if (aStepName == KEGL_Image_Multi_Process_VgImage_ReadWrite)								testStep = new CEglTest_EGL_Image_Multi_Process_VgImage_ReadWrite;

	// Benchmark - SgImage
    else if (aStepName == KBenchmark_CreateCloseImage)                                              testStep = new CEglTest_Benchmark_CreateCloseImage;
    else if (aStepName == KBenchmark_Multi_Process_CreateCloseImage)                                testStep = new CEglTest_Benchmark_Multi_Process_CreateCloseImage;
    else if (aStepName == KBenchmark_DrawImage)                                                     testStep = new CEglTest_Benchmark_DrawImage;

	// Benchmark - SwapBuffers
    else if (aStepName == KBenchmark_SwapBuffers)                                                   testStep = new CEglTest_Benchmark_SwapBuffers;

	//OOM - SgImage
    else if (aStepName == KOOM_CloseVGImageWithTermination)                                         testStep = new CEglTest_OOM_CloseVGImageWithTermination;
    else if (aStepName == KOOM_CloseVGImage)                                                        testStep = new CEglTest_OOM_CloseVGImage;
    else if (aStepName == KOOM_ClosePixmapSurfaceWithTermination)                                   testStep = new CEglTest_OOM_ClosePixmapSurfaceWithTermination;
    else if (aStepName == KOOM_ClosePixmapSurface)                                                  testStep = new CEglTest_OOM_ClosePixmapSurface; 
    else if (aStepName == KOOM_CloseSgImageDifferentProcess)                                        testStep = new CEglTest_OOM_CloseSgImageDifferentProcess; 
    else if (aStepName == KOOM_CloseSgImageSameThread)                                              testStep = new CEglTest_OOM_CloseSgImageSameThread; 

    //Stress - SgImage
    else if (aStepName == KStress)                                                                  testStep = new CEglTest_Stress;

    // Surface Scaling
    else if (aStepName == KSurfaceScaling_Positive)                                                 testStep = new CEglTest_SurfaceScaling_Positive;
    else if (aStepName == KSurfaceScaling_WindowResize)                                             testStep = new CEglTest_SurfaceScaling_WindowResize;
    else if (aStepName == KSurfaceScaling_ExtentPositionChange)                                     testStep = new CEglTest_SurfaceScaling_ExtentPositionChange;
    else if (aStepName == KSurfaceScaling_ExtentSizeChange)                                     	testStep = new CEglTest_SurfaceScaling_ExtentSizeChange;
    else if (aStepName == KSurfaceScaling_SwapBuffers)                                     			testStep = new CEglTest_SurfaceScaling_SwapBuffers;
    else if (aStepName == KSurfaceScaling_WindowSurface_Check)                                      testStep = new CEglTest_SurfaceScaling_WindowSurface_Check;
	else if (aStepName == KSurfaceScaling_Negative_CreateWindowSurface)                             testStep = new CEglTest_SurfaceScaling_Negative_CreateWindowSurface;
    else if (aStepName == KSurfaceScaling_Negative_FixedSize_NonWindowSurface)                      testStep = new CEglTest_SurfaceScaling_Negative_FixedSize_NonWindowSurface;
    else if (aStepName == KSurfaceScalingDefaultBorderColor)                                        testStep = new CEglTest_SurfaceScalingDefaultBorderColor;
    else if (aStepName == KSurfaceScalingModifyingBorderColor)                                      testStep = new CEglTest_SurfaceScalingModifyingBorderColor;
    else if (aStepName == KSurfaceScalingModifyingBorderColorNonFixed)                          	testStep = new CEglTest_SurfaceScalingModifyingBorderColorNonFixed;
    else if (aStepName == KSurfaceScalingModifyingInvalidBorderColor)                               testStep = new CEglTest_SurfaceScalingModifyingInvalidBorderColor;
    else if (aStepName == KSurfaceScalingModifyingExtent)                                           testStep = new CEglTest_SurfaceScalingModifyingExtent;
    else if (aStepName == KSurfaceScalingModifyingExtentNonFixed)                                   testStep = new CEglTest_SurfaceScalingModifyingExtentNonFixed;
    else if (aStepName == KSurfaceScalingQuerySurface)                                              testStep = new CEglTest_SurfaceScalingQuerySurface;
    else if (aStepName == KSurfaceScalingQuerySurfaceNonFixed)                                      testStep = new CEglTest_SurfaceScalingQuerySurfaceNonFixed;
    else if (aStepName == KSurfaceScalingCapability)                                                testStep = new CEglTest_SurfaceScalingCapability;
    else if (aStepName == KSurfaceScalingSet)                                                		testStep = new CEglTest_SurfaceScalingSet;
    else if (aStepName == KSurfaceScalingSetNonFixed)                              					testStep = new CEglTest_SurfaceScalingSetNonFixed;
    else if (aStepName == KSurfaceScalingSetInvalidAttributes)                                      testStep = new CEglTest_SurfaceScalingSetInvalidAttributes;
    else if (aStepName == KSurfaceScalingNotInitialized)                                      		testStep = new CEglTest_SurfaceScalingNotInitialized;
		
	return testStep;
	}
