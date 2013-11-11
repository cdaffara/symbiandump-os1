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
* Description: Test step for testing the methods of COMASuplPosHandler class [POS message plugin]
*
*/

/**
 @file poshandlerinitstep.h
*/

#ifndef POSHANDLERINITSTEP_H
#define POSHANDLERINITSTEP_H

#include <test/testexecutestepbase.h>
#include "te_suplproxypmsuitestepbase.h"

_LIT(KPosHandlerIntStep,"PosHandlerInitStep");

class CPosHandlerInitStep : public CTe_SuplProxyPMSuiteStepBase
	{
public:
    CPosHandlerInitStep();
	~CPosHandlerInitStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
    void TestByIndexL(TInt aIndex);
    void TestPosHandlerInitL(TInt aIndex);
    void TestPosHandlerCancelInitL(TInt aIndex);
    void TestCreatePosSessionL(TInt aIndex);
    void TestCreatePosSessionHeapFailL(TInt aIndex);
    void TestCheckPluginVersionL(TInt aIndex);
    void TestCheckPluginUidL(TInt aIndex);
private:
    COMASuplPosSessionBase* iSuplPosSession;
	};
#endif //POSHANDLERINIT_STEP_H
