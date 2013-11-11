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
 @internalComponent - Internal Symbian test code
*/


#ifndef __EGLTEST_ENDPOINT_ENGINE_TYPES_H__
#define __EGLTEST_ENDPOINT_ENGINE_TYPES_H__

#include "egltest_surface.h"


enum TEngineCases
    {
    // ECreateEndpointCase
    // iEndPointIndex is used to store the endpoint at that index.
    ECreateEndpointCase = 0,

    // EBeginStreamingCase
    // Call EglEndpointBeginStreaming.
    // iEndpointIndex indicates the endpoint to call BeginStreaming on.
    EBeginStreamingCase = 1,

    // EAcquireImageCase
    // iEndpointIndex indicates the endpoint used to acquire the image.
    // iImageIndex is ignored.
    EAcquireImageCase = 2,

    // EReleaseImageCase
    // iEndpointIndex indicates the endpoint used to acquire the image.
    // iImageIndex indicates the slot of the eglImage to be used.
    EReleaseImageCase = 3,

    // EEndStreamingCase
    // Call EglEndPointEndStreaming.
    // iEndpointIndex indicates which endpoint to use.
    EEndStreamingCase = 4,

    // EContentUpdateCase
    // iEndpointIndex indicates which surface to use.
    // iImageIndex index to the rectangle set to pass in as "dirty rectangles".
    //             if index = 0, then a NULL is passed - indicating full surface 
    //             (no other value currently supported).
    // iFlags: ENoWait: complete immediately, rather than wait for displayed 
    //                  notification (only valid for non-EGL surfaces).
    //         ENoSync: Do not wait for contentupdate to complete, instead, 
    //                  use a short delay (300ms) to ensure the submitupdate
    //                  has reached SUS.
    //                  This is necessary to avoid a deadlock where the local
    //                  side is doing EglSwapBuffer [or similar], and the 
    //                  remote side has not yet done EglReleaseImageNOK.
    //                  If the local side was to wait for the EglSwapBuffer 
    //                  to complete, the EglReleaseImageNOK would not happen.
    //                  There is no need to have ENoSync if the EglReleaseBuffer
    //                  for that endpoint is called before the EContentUpdateCase.
    EContentUpdateCase = 5,

    // EDestroyEndpointCase
    // iEndpointIndex indciates the endpoint to be destroyed.
    EDestroyEndpointCase = 6,

    // ETerminateCase
    // Call eglTerminate and does various other cleanup.
    // No egl calls except eglInitialize is expected to "work" after this point.
    // This is also sent to the local exec thread to terminate it when test step
    // is finished. 
    ETerminateCase = 7,

    // EInitializeCase
    // Initialize. Call eglInitialize and other setup.
    EInitializeCase = 8,

    // EGetAttributeCase
    // iEndpointIndex indicates the endpoint to use.
    // iArg1 is the attribute key to request.
    // iArg2 is the value expected.
    EGetAttribCase = 9,

    // ESetAttributeCase
    // iEndpointIndex indicates the endpoint to use.
    // iImageIndex is ignored.
    // iArg1 is the attribute key to request.
    // iArg2 is the value to set.
    ESetAttribCase = 10,

    // ECreateSurfaceCase
    // iEndpointIndex indicates which the surface object is stored at
    // iImageIndex indicates the surfaceParams to use.
    ECreateSurfaceCase = 11,

    // EDrawContentCase
    // Draws an image.
    // iEndpointIndex indicates the surface to draw to.
    // iImageIndex is the index of the image to be drawn.
    EDrawContentCase = 12,

    // ECompareImageCase
    // iEndpointIndex = the index of the eglimage stored, the iImageIndex when it was acquired
    // iImageIndex is the index of the image to compare
    ECompareImageCase = 13,

    // EBufferCountCase
    // Query the surface to find out number of buffers, and only run the remainder
    // of case if the number of buffers match. If test fails, log a warning that
    // the test is not executed, then terminate the case.
    // iEndpointIndex = number of the surface to check.
    // iImageIndex = n/a - ignored.
    // arg1 = Min buffer count (inclusive). 0 -> any number.
    // arg2 = Max buffer count (inclusive). 0 -> any number
    EBufferCountCase = 14,

    // ENotifyWhenCase
    // iEndpointIndex = surface for which the notification applies.
    // iImageIndex = which notification (See TNotification in egltest_surface.h)
    // iArg1 = XTimes when ENotifyWhenDispXTimes
    ENotifyWhenCase = 15,

    // EWaitForCase
    // iExpectedError = KErrTimedOut if the notification is expected to "not be signaled", KErrNone for other cases.
    // iEndpointIndex = surface for which to wait on notification (also used for timestamp).
    // iImageIndex = which notification (See TNotification in egltest_surface.h)
    // iArg1 = Timeout length in microseconds.
    // iArg2 = expected timestamp. (NOtifywhendisplayed), relative to timestamp of ETimeStampCase.
    EWaitForCase = 16,

    // ECreateVgImageCase
    // iEndpointIndex = the index of the eglimage to create the vgimage from.
    // iImageIndex is the index in the vgimages array that the created vgimage will be saved to.
    ECreateVgImageCase = 17,

    // EDestroyVgImageCase
    // iImageIndex is the index in the vgimages array that will be destroyed.
    EDestroyVgImageCase = 18,

    // EDestroyEglImageCase
    // Note: This is not intended for normal endpoint tests - use EReleaseImageCase for that.
    // iEndpointIndex = ignored.
    // iImageIndex = image index to destroy.
    EDestroyEglImageCase = 19,

    // ETimeStampCase
    // Store current User::FastCounter().
    // iEndpointIndex = index to store timestamp at (should match the surface index).
    ETimeStampCase = 20,

    // ETestVgImageValidCase
    // Test that the VgImage is valid and "works" by reading some of the pixels in it.
    // No checking that the content is as expected, since where we use this
    // is to test that the VgImage is valid after destroying the endpoint - at which point
    // the content is undefined, but the VgImage must be usable by the effects engine code.
    // See also ECreateVgImageCase.
    // iEndpointIndex ignored.
    // iImageIndex is the index of the vgimage to be drawn.
    ETestVgImageValidCase = 21,

    // ERequestNotificationCase
    // Request a notification of update.
    // iEndpointIndexis the index of the endpoint to use.
    ERequestNotificationCase = 22,

    // ECancelNotificationCase
    // Cancel a notification of update.
    // iEndpointIndexis the index of the endpoint to use.
    ECancelNotificationCase = 23,

    // EWaitForNotificationCase
    // Wait for the notification of the
    // iEndpointIndex is the index of the endpoint to use.
    // iImageIndex - not used
    // iArg1 - timeout value in microseconds.
    EWaitForNotificationCase = 24,

    // EGetEndpointDirtyAreaCase
    // Get dirty area
    // iEndpointIndex is the index of the endpoint to use.
    // iImageIndex index to the rectangle set to pass in as "dirty rectangles".
    //             if index = 0, then full surface is expected.
    // iarg1 start_rect
    // iArg2 max_rects
    // iFlags - EUseNullRects - pass NULL as the rects value.
    //          EUseCollapseArea - pass TRUE for collapse
    EGetEndpointDirtyAreaCase = 25,
    
    // EGetSurfaceParamsCase
    // Get parameters of the surface. 
    // iEndpointIndex is the index of the surface being used.
    // This is used for internal communication between the controller thread
    // and the EGL execution thread, so the parameters can be passed over to
    // the remote side. This is due to the fact that we can only use the 
    // RWindow component that is part of an EGL surface in the thread that
    // created the RWindow.
    EGetSurfaceParamsCase = 26, 
    
    // EDestroySurfaceCase
    // Opposite of ECreatSurfaceCase.
    // iEndpointIndex indicates which surface object
    EDestroySurfaceCase = 27,
    
    // EStartLoadThreadCase
    // For tests that have some sort of second thread that does "loading" 
    // (e.g. heavy drawing or using up memory), this will start that thread.
    // iEndpointIndex is the thread number. 
    // Note, relies on the thread implementing a StartLoadThread() function -
    // default implementation will panic.
    EStartLoadThreadCase = 28,
    
    // EEndLoadThreadCase
    // For tests that have some sort of second thread that does "loading" 
    // (e.g. heavy drawing or using up memory), this will end the thread.
    // iEndpointIndex is the thread number. 
    // Note, relies on the thread implementing a EndLoadThread() function -
    // default implementation will panic.
    EEndLoadThreadCase = 29,
    
    // ECheckForMemoryLeaks
    // Used by tests that perform for example out-of-memory testing. 
    // Executed on the in the remote engine code. 
    // The amount of memory available is stored in an array, and when 
    // the ECheckForMemoryLeaksFinish is called, the statistics is
    // calculated abd deternubed if there is a leak or not. 
    ECheckForMemoryLeaks = 30,
    
    // ECheckForMemoryLeaksFinish
    // Check the collected data from ECheckForMemoryLeaks, and determine the
    // trend of memory usage. 
    // See ECheckForMemoryLeaks
    ECheckForMemoryLeaksFinish = 31,
    
    // ESpecialEglHeapCase
    // Configure a heap for the EGL calls. 
    // This is used to allow egl calls to be done with a heap that is full.
    // iEndpointIndex = min size
    // iImageIndex = max size
    // IMPORTANT NOTE: This HAS TO BE the first case sent to the remote engine. 
    ESpecialEglHeapCase = 32,

    
    ///
    /// Debug cases
    ///
    // EBreakPointCase
    // Used for debugging.
    // iFlags = EDebugExecThread, EDebugController, EDebugRemote - break 
    // exec_thread, controller and/or remote part of engine.
    EBreakPointCase = 64,

    // ELogEnableCase
    // Logging enable - all items executed after this point are written out, including data parameters.
    // iFlags = EDebugLocal, EDebugRemote - log on local/remote side.
    ELogEnableCase = 65,

    ///////////////////////////////////////////////////////
    // Control messages - "done", "sync", etc.
    ///////////////////////////////////////////////////////
    
    // ESetVerdictCase
    // Used to set the relevant threads "teststepresult".
    // Flags indicate which thread is updated.
    // iEndpointIndex = verdict to set. 
    // Note: Remote value is not using the ERtvXXX values, the standard
    // values in Test Framework are translated in the remote step.
    ESetVerdictCase = 996,
    
    // ESyncLocalCase
    // Used for synchronizing the local execution thread. 
    // Should normally not be needed for the common cases of coding, but used internally to 
    // perform the synchronization. 
    ESyncLocalCase = 997,

    // EPanicCase
    // Cause panic on local or remote side. Used for testing purposes only.
    // iFlags = EDebugExecThread, EDebugController, EDebugRemote - break 
    // exec_thread, controller and/or remote part of engine.
    EPanicCase = 998,

    // EIllegalCase
    // A case that is by definition not legal/valid. There should be no case-
    // statement for this in the engine, it should be treated as a "unknown" case,
    // and cause any test to fail in the same way as if an unused value was put
    // in the table. Used for testing purposes, and can also be used to make a
    // particular test fail on purpose (e.g. due to production code bugs).
    // All arguments ignored.
    EIllegalCase = 999,

    // Indicate end of sequence.
    EFinishedCase = 1000
    };

enum TEngineFlags
    {
    EUseBadDisplay      = 1 << 0,
    EUseBadEndpoint     = 1 << 1,
    EUseBadEglImage     = 1 << 2,
    EUseBadEndpointType = 1 << 3,
    EUseBadSourceType   = 1 << 4,
    EExpectError        = 1 << 5,  // Used when other flags are not indicating a failure expected.
    EUseBadSurfaceId    = 1 << 6,
    EUseBadApi          = 1 << 7,
    // Flag to modify EContentUpdateCase to not wait for displayed.
    EUseBadSync         = 1 << 8,
    EUseBadRects        = 1 << 9,
    // Extra flags for EGetEndpointDirtyAreaCase
    EUseNullRects       = 1 << 10,
    EUseCollapseArea    = 1 << 11,
    
    // flags used by EContentUpdateCase
    ENoWait             = 1 << 14,
    ENoSync             = 1 << 15,

    // Debug flags - perform operation on "Local"
    EDebugExecThread      = 1 << 16,
    EDebugRemote        = 1 << 17,
    EDebugController    = 1 << 18,
    
    // ThreadingFlags - just rename the debug flags, because it's actually the
    // same thing. We _RELY_ on this being the same to simplify the code in 
    // the controller thread.
    // Note: the lack of "execthread" is intentional. Since controller and exec
    // are in the same process, there is no reason to use the ExecThread to 
    // start a new thread. ??This may need to be changed int he future.??
    EThreadRemote       = EDebugRemote,
    EThreadController   = EDebugController,
    };

enum TEngineFlagsMask
    {
    // Mask to match all "bad".
    EExpectFailureMask  = EUseBadDisplay       |
                          EUseBadEndpoint      |
                          EUseBadEglImage      |
                          EUseBadEndpointType  |
                          EUseBadSourceType    |
                          EExpectError         |
                          EUseBadSurfaceId     |
                          EUseBadApi           |
                          EUseBadSync          |
                          EUseBadRects         |
                          0   // So that it's easier to merge.
    };


enum TRemoteThreadEngineThread
    {
    EUnknownThread = 0,
    EThreadLoadHeapMemory = 1,
    EThreadLoadGpuMemory = 2,
    EThreadLoadGpuProcessor = 3,
    };

struct TEngineTestCase
    {
    TEngineCases iCase;             // Command to execute. See TEngineCases for details.
    TInt         iFlags;            // Flags to indicate for example bad parameters (TEngineFlags).
    TInt         iErrorExpected;    // The value that eglGetError() is expected to return.
    TInt         iEndpointIndex;    // Index to the endpoint to use.
    TInt         iImageIndex;       // Index to image to draw/compare(egltest_endpoint_images)
    TInt         iArg1;             // E.g. GetAttrib or SetAttrib attrib.
    TInt         iArg2;             // E.g  GetAttrib expected value or SetAttrib value to set.
    };


struct TTestEndpointEngine
    {
    TSurfaceParamsRemote iSurfaceParams;
    TEngineTestCase      iEngineTestCase;
    };

// Passed to StartRemoteTestStep() during test startup.
struct TTestEndpointEngineConfig
    {
    TBool iLogErrors; 
    };

// Number of small steps that each test is allowed to contain. Since each test
// that uses the engine has at least one table like this, increasing this 
// hugely will increase the overall  memory usage of the application (each 
// entry is about 28 bytes). So, don't go multiplying by 10 or some such
// unless there is a REAL need for that.
// THe current tables take up approximately 28 * 80 = 2240 bytes.
// As of right now, there are about 100 tests -> 220KB of tables.
const TInt KMaxCases    = 80;

// Should be PLENTY of endpoints [also used for surfaces, images and other
// objects that have roughly 1:1 mapping with endpoints. This is dynamically
// allocated in the constructor of the engine class.
const TInt KMaxEndpoints = 1000;

// Max number of surfacetypes we expect to be listed. 
// This can be increased as and when needed. 
const TInt KMaxSurfaceList = 4;   

struct TTestCaseBase
    {
    const TText*       iRelatedTestIds;
    const TText*       iName;
    TInt               iSurfaceTypeCount;
    TSurfaceType       iSurfaceTypeList[KMaxSurfaceList];
    };

// Structure for lists of cases to execute
struct TTestCase
    {
    // Use anonymous struct to avoid changing code all over the place.
    TTestCaseBase      iBase;
    TEngineTestCase    iEngineTestCase[KMaxCases];
    };

struct TTestCases
    {
    const TTestCase *iCase;
    TInt             iCount;
    };

#endif
