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
 @file LbsRootApiTest29Step.h
*/
#if (!defined __LBSROOTAPITest29_STEP_H__)
#define __LBSROOTAPITest29_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsrootapitestsuiteStepBase.h"

class CLbsRootApiTest29Step : public CTe_LbsRootApiTestSuiteStepBase
	{
public:
	CLbsRootApiTest29Step();
	~CLbsRootApiTest29Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:
    void DoMainL();
	};

_LIT(KLbsRootApiTest29Step,"LbsRootApiTest29Step");

#endif // __LBSROOTAPITest29_STEP_H__
