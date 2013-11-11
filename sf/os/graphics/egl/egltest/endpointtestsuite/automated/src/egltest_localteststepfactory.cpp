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


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "localtestbase.h"
#include "egltest_endpointapiexposure.h"
#include "egltest_endpointcreate.h"
#include "egltest_endpointacquire.h"
#include "egltest_endpointattrib.h"
#include "egltest_endpointstreaming.h"
#include "egltest_endpointimage.h"
#include "egltest_endpointlocking.h"
#include "egltest_endpointtearing.h"
#include "egltest_endpointstreamingtests.h"
#include "egltest_endpointnotification.h"
#include "egltest_endpointdirtyarea.h"
#include "egltest_endpoint_stresstests.h"
#include "egltest_endpoint_stressload.h"
#include "egltest_threadedstress.h"
#include "egltest_releaseimagegles.h"

//LocalTestStep factory.
CTestStep* CEglEndpointTestServer::CreateTestStep(const TDesC& aStepName)
    {
    if (aStepName == KEGL_EndpointApiExposure)
        {
        return new CEglTest_LocalTestStep_EndpointApiExposure;
        }
    if (aStepName == KEGL_EndpointCreate)
        {
        return new CEglTest_LocalTestStep_EndpointCreate;
        }
    if (aStepName == KEGL_EndpointAttrib)
        {
        return new CEglTest_LocalTestStep_EndpointAttrib;
        }
    if (aStepName == KEGL_EndpointAcquire)
        {
        return new CEglTest_LocalTestStep_EndpointAcquire;
        }
    if (aStepName == KEGL_EndpointStreaming)
        {
        return new CEglTest_LocalTestStep_EndpointStreaming;
        }
    if (aStepName == KEGL_EndpointImage)
        {
        return new CEglTest_LocalTestStep_EndpointImage;
        }
    if (aStepName == KEGL_EndpointLocking)
        {
        return new CEglTest_LocalTestStep_EndpointLocking;
        }
    if (aStepName == KEGL_EndpointImageLifetime)
        {
        return new CEglTest_LocalTestStep_EndpointImageLifetime;
        }
    if (aStepName == KEGL_EndpointTearing)
        {
        return new CEglTest_LocalTestStep_EndpointTearing;
        }
    if (aStepName == KEGL_EndpointStreamingTests)
        {
        return new CEglTest_LocalTestStep_EndpointStreamingTests;
        }
    if (aStepName == KEGL_EndpointStreamingDispXTimesInStream)
        {
        return new CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream;
        }
    if (aStepName == KEGL_EndpointNotification)
        {
        return new CEglTest_LocalTestStep_EndpointNotification;
        }
    if (aStepName == KEGL_EndpointDirtyArea)
        {
        return new CEglTest_LocalTestStep_EndpointDirtyArea;
        }
    if (aStepName == KEGL_EndpointStressTests)
        {
        return CEglTest_TestStep_StressTests::New();
        }
    if (aStepName == KEGL_EndpointStressLoad)
        {
        return CEglTest_TestStep_StressLoad::New();
        }
    if (aStepName == KEGL_EndpointCrazyThreading)
        {
        return new CEglTest_LocalTestStep_EndpointCrazyThreading();
        }
    if (aStepName == KEGL_EndpointOutOfHeapMemory)
        {
        return new CEglTest_LocalTestStep_EndpointOutOfHeapMemory();
        }
    if (aStepName == KEGL_EndpointReleaseImageGles)
        {
        return new CEglTest_LocalTestStep_EndpointReleaseImageGles();
        }
    
    return NULL;
    }



