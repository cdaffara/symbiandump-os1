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

#ifndef __EGLTEST_ENDPOINT_ENGINE_EXECTHREAD_H__
#define __EGLTEST_ENDPOINT_ENGINE_EXECTHREAD_H__

#include <e32base.h>
#include <e32msgqueue.h>
#include "egltest_surface.h"
#include "egltest_endpoint_engine.h"
#include "egltest_endpoint_images.h"
#include "egltest_commscommon.h"
#include "egltest_endpoint_util.h"

_LIT(KExecResultQueueName, "EGLEndpointExecTestEnvResultQueue");
_LIT(KExecParamsQueueName, "EGLEndpointExecTestEnvParamsQueue");

// For description of the work done as part of this class, see the 
// egltest_endpoint_engine_execthread.cpp
NONSHARABLE_CLASS(CEgltest_Local_Engine_Exec): public CBase
    {
public:
    CEgltest_Local_Engine_Exec();
    virtual ~CEgltest_Local_Engine_Exec();
    static TInt ThreadEntry(TAny *aDummy);
    static CEgltest_Local_Engine_Exec* NewL();
    CTestExecuteLogger &Logger() { return iLogger; }
    void SetTestStepResult(TVerdict aVerdict);
    TVerdict TestStepResult() const;
    void TidyUp();
    void SetUpL();
    
private:
    // This is the actual thread execution loop.
    static void ThreadEntryL();
    TInt ThreadLoopL();
    void ConstructL();
    void SendResult(const TExecResult &aResult);
    
private:
    RMsgQueue<TExecResult>              iResultOutQueue;
    RMsgQueue<TRemoteTestParamsPacket>  iParamsInQueue;
    CSurface*                           iSurfaces[KMaxEndpoints];
    TRequestStatus                      iStatus[KMaxEndpoints];
    TUint32                             iTimeStamp[KMaxEndpoints];
    TInt                                iFastFreq;
    // iLogging: Log commands as they are processed in this thread.
    TBool                               iLogging;
    // iLogger: Standard TEF logging object.
    CTestExecuteLogger                  iLogger;
    TVerdict                            iVerdict;
    TBool                               iSurfaceTypeDisplayed;
    EGLDisplay                          iDisplay;
    };

#endif
