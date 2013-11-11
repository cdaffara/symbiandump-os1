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
 @file LbsRootApiTest19Step.h
*/
#if (!defined __LBSROOTAPITEST19_STEP_H__)
#define __LBSROOTAPITEST19_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsrootapitestsuiteStepBase.h"

class CLbsRootApiTest19Step : public CTe_LbsRootApiTestSuiteStepBase
	{
public:
	CLbsRootApiTest19Step();
	~CLbsRootApiTest19Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

protected: // From MLbsTestStepObserver
    void StartLbsTestStepL(TRequestStatus& aStatus,TBool& aFinished);
    void FinishLbsTestStep(TRequestStatus& aStatus,TBool& aFinished);    

// Please add/modify your class members here:
private:
    void DoMainL();
	};

_LIT(KLbsRootApiTest19Step,"LbsRootApiTest19Step");
#endif // __LBSROOTAPITEST19_STEP_H__
