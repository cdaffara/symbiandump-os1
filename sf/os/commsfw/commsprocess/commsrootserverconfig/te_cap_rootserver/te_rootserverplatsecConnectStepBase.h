// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Te_ipsec_platsecSuiteStepBase.h
// Root Server platform security test suite test step base class header file.
// 
//

/**
 @file 
*/

#if (!defined __TE_ROOTSERV_PLATSEC_STEP_BASE__)
#define __TE_ROOTSERV_PLATSEC_STEP_BASE__
#include <test/testexecutestepbase.h>


class CSessionConnectionHelper : public RSessionBase
	{
public:
	CSessionConnectionHelper();
	virtual ~CSessionConnectionHelper();
	
	TInt OpenSession(const TDesC& aServer, const TVersion& aVersion);
	};


/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class Cte_rootserverplatsecConnectStepBase : public CTestStep
	{ 
public:
	virtual ~Cte_rootserverplatsecConnectStepBase();
	Cte_rootserverplatsecConnectStepBase ();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

//Please add your class members which will be common to all individual test steps:
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	CSessionConnectionHelper* iSessionHelper;	
	};

#endif

