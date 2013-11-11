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
* Description: Test step for testing the methods of COMASuplPosSession class [POS message plugin]
*
*/

/**
 @file possessioninitstep.h
*/
#ifndef POSSESSIONINITSTEP_H
#define POSSESSIONINITSTEP_H

#include <test/testexecutestepbase.h>
#include "te_suplproxypmsuitestepbase.h"
#include <e32property.h>

_LIT(KPosSessionIntStep,"PosSessionInitStep");

//forward declaration
class CAsyncReqWaiter;
class COMASuplPosSessionBase;

class CPosSessionInitStep : public CTe_SuplProxyPMSuiteStepBase
    {
public:
    CPosSessionInitStep();
    ~CPosSessionInitStep();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepL();
    virtual TVerdict doTestStepPostambleL();

private:
    void TestByIndexL(TInt aIndex);
    void TestPosSessionInitL(TInt aIndex);
    void TestMultiPosSessionInitL(TInt aIndex);
    void TestPosSessionInitCancelL(TInt aIndex);
    void TestPosSessionReInitL(TInt aIndex);
    void TestGetSuplInfoWithoutInitL(TInt aIndex);
    void TestGetPositionWithoutInitL(TInt aIndex);
    void TestGetSuplInfoInvalidRequestL();
    void TestCancelGetSuplInfoL(void);
    void TestCancelGetPositionL(void);
    void TestCancelNonExistRequest(void);
    void TestCancelNonExistGetPos(void);
    void PosSessionEndwithRequestsL(void);    
    void PosSessionReinitL(void);
    void GetPositionWithoutReInitL(void);
    void GetSuplInfoWithoutReInitL(void);
    void TestSuplInfoPosProtocolVersionL();
    void TestSuplStateMethods();
    void TestSuplObserverHandle();
    void TestFollowsInitSuccessL(TInt aIndex);
    void TestSuplInfoAssDataArgClassesL();
    void TestSuplInfoGenericArgClassesL();
    void TestInvalidPosProtocolVersionL();
    void TestSetPosMethodAndAllowedCapabilities();
    void TestSuplInfoPositionArgClassesL();
    void TestSuplInfoPosPayloadArgClassesL();
    void TestRefLocPluginPositionError(TInt aIndex);
    
private:
    COMASuplPosSessionBase* iSuplPosSession;
    CAsyncReqWaiter* iReqWaiter;
    };

#endif //possessioninitstep.h

