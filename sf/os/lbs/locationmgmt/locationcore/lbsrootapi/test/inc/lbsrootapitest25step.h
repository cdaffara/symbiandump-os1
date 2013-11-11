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
 @file LbsRootApiTest25Step.h
*/
#if (!defined __LBSROOTAPITest25_STEP_H__)
#define __LBSROOTAPITest25_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsrootapitestsuiteStepBase.h"

class CLbsRootApiTest25Step : public CTe_LbsRootApiTestSuiteStepBase
	{
public:
	CLbsRootApiTest25Step();
	~CLbsRootApiTest25Step();
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

_LIT(KLbsRootApiTest25Step,"LbsRootApiTest25Step");
#endif
