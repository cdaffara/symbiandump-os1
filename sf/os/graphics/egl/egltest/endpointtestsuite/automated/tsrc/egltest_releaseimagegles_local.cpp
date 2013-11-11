/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "egltest_releaseimagegles.h"


//This test case acquires an image from an endpoint then attempts to 
//release the image, passing in EGL_OPENGL_ES_API as the API flag.
//There are two configurations for the test, that is controlled by
//a flag in aParams. The options are to have a valid GLES context 
//set current, or to have nothing current.


CEglTest_LocalTestStep_EndpointReleaseImageGles::CEglTest_LocalTestStep_EndpointReleaseImageGles() :
    CLocalTestStepBase(ETestUidEndpointReleaseImageGles)
    {
    }


CEglTest_LocalTestStep_EndpointReleaseImageGles::~CEglTest_LocalTestStep_EndpointReleaseImageGles()
    {
    }


void CEglTest_LocalTestStep_EndpointReleaseImageGles::DoPreambleL()
    {
    //Register the test id.
    _LIT(KTestId, "COV");
    RegisterTestIdsL(KTestId);
    SetCurrentTestIds(KTestId);
    }


void CEglTest_LocalTestStep_EndpointReleaseImageGles::DoPostambleL()
    {
    }


TVerdict CEglTest_LocalTestStep_EndpointReleaseImageGles::doTestStepL()
    {
    TRemoteTestParams params;
    StartRemoteTestStep(TRemoteTestParams());
    
    //Valid GLES context test.
    params.iEndpointReleaseImageGles.iUseValidGlesContext = ETrue;
    RunRemoteTestCase(0, params);
    
    //Invalid GLES context test.
    params.iEndpointReleaseImageGles.iUseValidGlesContext = EFalse;
    RunRemoteTestCase(0, params);
        
    EndRemoteTestStep(TRemoteTestParams());
    return TestStepResult();
    }

