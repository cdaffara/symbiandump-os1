/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file asyncreqwaiter.h
*/
#ifndef ASYNCREQWAITER_H
#define ASYNCREQWAITER_H

#include <test/testexecutestepbase.h>
#include "te_suplproxypmsuitestepbase.h"
#include <lbs/epos_comasuplposhandlerbase.h>    // For the POS Msg plugin
#include <lbs/epos_comasuplpossessionbase.h>    // For the POS Msg plugin
#include <e32base.h>


//Forward declarations
class COMASuplSETCapabilities;


class CAsyncReqWaiter : public CActive
    {
public:
    enum TestCase
        {
        EGetSuplInfo=0,
        ECancelGetSuplInfo,
        EGetPosition,
        ECancelGetPosition,
        EGetSuplInfoSessionEnd,
        ESuplInfoPosProtocolVersion,
        ERefLocPluginPositionError,
        };
public:
    static CAsyncReqWaiter* NewL(COMASuplPosSessionBase* aSessionPtr);
    ~CAsyncReqWaiter();
    void StartTest(TestCase aTestCaseId);
    inline COMASuplSETCapabilities* ReadReceivedSETCaps() {return iSETCapabilities;}
    inline COMASuplInfoRequestList* ReadReceivedSuplInfoReqList() {return iSuplInfoRequest;}
private:
    CAsyncReqWaiter(COMASuplPosSessionBase* aSessionPtr);
    void ConstructL();
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
private:
    COMASuplPosSessionBase* iPosSessionPtr;
    TestCase iTestCaseId;
    TBool iResponseExp;
    COMASuplInfoRequestList* iSuplInfoRequest;
    COMASuplPosition* iSuplPosition;
    COMASuplSETCapabilities* iSETCapabilities;
    };

#endif //ASYNCREQWAITER_H

