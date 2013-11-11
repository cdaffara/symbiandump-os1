/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

 
#if (!defined __TE_LBSREFFNPINTTESTSTEP1_H__)
#define __TE_LBSREFFNPINTTESTSTEP1_H__
#include <test/TestExecuteStepBase.h>
#include "te_lbsreffnpinttestsuitestepbase.h"

class CLbsReffnpintTestStep1 : public CTe_LbsReffnpintTestSuiteStepBase
	{
public:
	CLbsReffnpintTestStep1();
	~CLbsReffnpintTestStep1();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void TestNetworkLocationRequestInterface();
	void TestNetworkLocationResponseInterface();
	void TestReferencePositionUpdateInterface();
	void TestFinalPositionUpdateInterface();
	

// Please add/modify your class members here:
private:
	};

_LIT(KLbsReffnpintTestStep1,"lbsreffnpintteststep1");
                                   

#endif

