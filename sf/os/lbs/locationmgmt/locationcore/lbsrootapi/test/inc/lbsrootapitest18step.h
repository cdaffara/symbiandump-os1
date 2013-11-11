/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file LbsRootApiTest18Step.h
*/
#if (!defined __LBSROOTAPITEST18_STEP_H__)
#define __LBSROOTAPITEST18_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_lbsrootapiTestSuiteStepBase.h"

class CLbsRootApiTest18Step : public CTe_LbsRootApiTestSuiteStepBase
	{
public:
	CLbsRootApiTest18Step();
	~CLbsRootApiTest18Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

protected: // From MLbsTestStepObserver
    void StartLbsTestStepL(TRequestStatus& aStatus);
    void FinishLbsTestStep(TRequestStatus& aStatus,TBool& aFinished);    

// Please add/modify your class members here:
private:
    void DoMainL();
	};

_LIT(KLbsRootApiTest18Step,"LbsRootApiTest18Step");
#endif
