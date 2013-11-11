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
 @file LbsRootApiTest7Step.h
*/
#if (!defined __LBSROOTAPITEST7_STEP_H__)
#define __LBSROOTAPITEST7_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbsrootapiTestSuiteStepBase.h"

class CLbsRootApiTest7Step : public CTe_LbsRootApiTestSuiteStepBase
	{
public:
	CLbsRootApiTest7Step();
	~CLbsRootApiTest7Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
// Please add/modify your class members here:
private:
    void DoMainL();
	};

_LIT(KLbsRootApiTest7Step,"LbsRootApiTest7Step");
#endif // __LBSROOTAPITEST7_STEP_H__
