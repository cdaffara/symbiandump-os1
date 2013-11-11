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


 
#if (!defined __TE_LBSREFFNPINTTESTSUITESTEPBASE_H__)
#define __TE_LBSREFFNPINTTESTSUITESTEPBASE_H__
#include <test/TestExecuteStepBase.h>
#include "lbscommoninternaldatatypes.h"
#include "lbsngnlmint.h" 
#include "lbsreffnpint.h" 

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_LbsReffnpintTestSuiteStepBase : public CTestStep

	{
public:
	virtual ~CTe_LbsReffnpintTestSuiteStepBase();
	CTe_LbsReffnpintTestSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

//Please add your class members which will be common to all individual test steps:
protected:
	CActiveScheduler* iScheduler;
	};

#endif
