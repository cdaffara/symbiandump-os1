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
 @file lbsrootapitest5step.h
*/
#if (!defined __LBSROOTAPITEST5_STEP_H__)
#define __LBSROOTAPITEST5_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsrootapitestsuiteStepBase.h"


class CLbsRootApiTest5Step : public CTe_LbsRootApiTestSuiteStepBase
	{
public:
	CLbsRootApiTest5Step();
	~CLbsRootApiTest5Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void CheckProcessesAreUp();

private:
    TInt iCount;    
	};

_LIT(KLbsRootApiTest5Step,"LbsRootApiTest5Step");
#endif // __LBSROOTAPITEST5_STEP_H__
