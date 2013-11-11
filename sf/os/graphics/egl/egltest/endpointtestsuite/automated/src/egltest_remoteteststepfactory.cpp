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


#include "remotetestbase.h"
#include "egltest_endpointapiexposure.h"
#include "egltest_endpoint_engine.h"
#include "egltest_endpointtearing.h"
#include "egltest_threadedstress.h"
#include "egltest_releaseimagegles.h"


//RemoteTestStep Factory.
CRemoteTestStepBase* CRemoteTestEnv::CreateRemoteTestStepL(TTestUid aTestUid)
    {
    switch (aTestUid)
        {
        case ETestUidEndpointApiExposure:       return new (ELeave) CEglTest_RemoteTestStep_EndpointApiExposure();
        case ETestUidEndpointEngine:            return new (ELeave) CEgltest_Remote_Engine();
        case ETestUidEndpointTearing:           return new (ELeave) CEglTest_RemoteTestStep_EndpointTearing();
        case ETestUidEndpointThreadStress:      return new (ELeave) CEglTest_RemoteTestStep_EndpointThreadStress();
        case ETestUidEndpointReleaseImageGles:  return new (ELeave) CEglTest_RemoteTestStep_EndpointReleaseImageGles();
        default:                                User::Leave(KErrUnknown);
        }

    //Stop the warning.
    return NULL;
    }

