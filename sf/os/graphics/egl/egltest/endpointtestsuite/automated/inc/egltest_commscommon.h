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


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


/*
 * To add a new message that can be passed over the message queue:
 * 1. Add the test name to the TTestUid enum.
 * 2. Create a T class to hold the params.
 * 3. Add the T class to the TRemoteTestParams union.
 *
 * These steps are described in more detail below, where the
 * individual bits should be added.
 */


#ifndef __EGLTEST_COMMSCOMMON_H__
#define __EGLTEST_COMMSCOMMON_H__


#include <e32base.h>
#include <graphics/surface.h>
#include <EGL/egl.h>
#include "log.h"
#include "egltest_endpoint_engine_types.h"

#define ENDPOINT_ASSERT_DEBUG(x, y) do { if (!x) { RDebug::Printf("Assertion (%s) failed: %s:%d ", #x, __FILE__, __LINE__); y; } } while(0)

//The maximum size of a message in an async message queue is 256 bytes.
//If these valuses are increased, the maximum size will be exceeded.
const TInt KRSLogMessageLength = 172;
const TInt KRSLogFileLength = 32;


//Names for the Async message queues that are used for
//communication between the TEF driver app and the test
//thread within window server.
_LIT(KResultQueueName, "RemoteTestEnvResultQueue");
_LIT(KParamsQueueName, "RemoteTestEnvParamsQueue");

const TInt KStartTestStepCaseNumber = -1;
const TInt KEndTestStepCaseNumber = -2;

//Uids for all of the tests that the test thread knows about.
enum TTestUid
    {
    //Endpoint Api Exposure Test.
    ETestUidEndpointApiExposure,

    // Common UID for engine code.
    ETestUidEndpointEngine,

    // Endpoint Lifetime test.
    ETestUidEndpointLifetime,

    //Image Tearing Test.
    ETestUidEndpointTearing,
    
    //Multithreaded stress tests.
    ETestUidEndpointThreadStress,
    
    //Release Image with Gles context tests.
    ETestUidEndpointReleaseImageGles,
    
    // vgThreading test.
    ETestUidVgThreading,
    };


//Each test should have a struct with data members to hold
//the test's parameters. The name name should begin
//"TTest..." and should contain no member functions.
//(c-style struct).

//If it is found that many tests use the same params, we should
//create a TTestGeneric class to hold the params and derive
//classes from it.

struct TTestEndpointApiExposure
    {
    };

struct TTestEndpointTearing
    {
    TSurfaceId iSurfaceId;
    };

struct TTestEndpointThreadStress
    {
    TInt iNumThreads;
    };

struct TTestEndpointReleaseImageGles
    {
    TBool iUseValidGlesContext;
    };


//Union for all of the structs that tests use to pass
//params between the local side and the remote side.
union TRemoteTestParams
    {
    //Endpoint Api Exposure Test.
    TTestEndpointApiExposure iTestEndpointApiExposure;

    //Endpoint engine data.
    TTestEndpointEngine iEndpointEngine;
    
    // Endpoint Engine configuration data.
    TTestEndpointEngineConfig iEndpointEngineConfig;

    //Image Tearing Test.
    TTestEndpointTearing iEndpointTearing;
    
    //Multithreaded stress tests.
    TTestEndpointThreadStress iEndpointThreadStress;
    
    //Release Image with Gles context tests.
    TTestEndpointReleaseImageGles iEndpointReleaseImageGles;
    };


//Structure that is passed from local side to remote side
//in the async message queue. It contains the test Uid,
//test case and the params.
class TRemoteTestParamsPacket
    {
public:
    TRemoteTestParamsPacket();
    TRemoteTestParamsPacket(TTestUid aTestUid, TInt aTestCase, const TRemoteTestParams& aParams);
    const TTestUid iUid;
    const TInt iTestCase;
    const TRemoteTestParams iParams;
    };


//The object passed back to the TEF driver app from the test
//thread. It can be used for sending the resuult of a test and
//for logging.
class TRemoteTestResult
    {
public:
    TRemoteTestResult();

    //Constructor for sending logging info.
    TRemoteTestResult(TTestUid aUid, TInt aTestCase, const TDesC8& aFile, TInt aLine, TInt aSeverity, const TDesC& aMessage);

    //Constructor for sending result info.
    TRemoteTestResult(TTestUid aUid, TInt aTestCase, TRemoteTestVerdict aVerdict);

public:
    //If EFalse this is a logging message, else a result message.
    TBool iFinished;
    TTestUid iUid;
    TInt iTestCase;

    //Result message.
    TRemoteTestVerdict iVerdict;

    //Logging message.
    TBuf8<KRSLogFileLength> iFile;
    TInt iLine;
    TInt iSeverity;
    TBuf8<KRSLogMessageLength> iMessage;
    };


//Inline functions --------------------------------------------------------

inline TRemoteTestParamsPacket::TRemoteTestParamsPacket() :
    iUid((TTestUid)0),
    iTestCase(0),
    iParams(TRemoteTestParams())
    {
    }


inline TRemoteTestParamsPacket::TRemoteTestParamsPacket(TTestUid aTestUid, TInt aTestCase, const TRemoteTestParams& aParams) :
    iUid(aTestUid),
    iTestCase(aTestCase),
    iParams(aParams)
    {
    }


inline TRemoteTestResult::TRemoteTestResult()
    {
    }


inline TRemoteTestResult::TRemoteTestResult(TTestUid aUid, TInt aTestCase, const TDesC8& aFile, TInt aLine, TInt aSeverity, const TDesC& aMessage) :
    iFinished(EFalse),
    iVerdict(ERtvInconclusive),
    iUid(aUid),
    iTestCase(aTestCase),
    iLine(aLine),
    iSeverity(aSeverity)
    {
    //Copy the filename to the log object. If the string is too long,
    //truncate the string and append an elipsis to the log.
    //The "-1"s ensure that a free char is left for appending a NULL (in the TEF app).
    TBool truncate = iFile.MaxLength()-1 < aFile.Length();
    TInt numChars = truncate ? iFile.MaxLength()-3-1 : aFile.Length();
    iFile = aFile.Left(numChars);
    if(truncate)
        {
        iFile.Append(_L("..."));
        }

    //Copy the message to the log object. If the string is too long,
    //truncate the string and append an elipsis to the log.
    //Note we convert message from unicode to ascii to conserve space in the message queue.
    truncate = iMessage.MaxLength() < aMessage.Length();
    numChars = truncate ? iMessage.MaxLength()-3 : aMessage.Length();
    iMessage.Copy(aMessage.Left(numChars));
    if(truncate)
        {
        iMessage.Append(_L8("..."));
        }
    }


inline TRemoteTestResult::TRemoteTestResult(TTestUid aUid, TInt aTestCase, TRemoteTestVerdict aVerdict) :
    iFinished(ETrue),
    iUid(aUid),
    iTestCase(aTestCase),
    iVerdict(aVerdict)
    {
    }

//------------------------------------------------------------------------------------


#endif
