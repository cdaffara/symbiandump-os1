/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_lbsnetlocMgrtestSuiteStepBase.h
*/

#if (!defined __TE_LBSNETLOCMGRTEST_STEP_BASE__)
#define __TE_LBSNETLOCMGRTEST_STEP_BASE__
#include <test/testexecutestepbase.h>
// Please add your include here if you have
#include "lbsnetinternalapi.h"
#include "LbsInternalInterface.h"
#include <centralrepository.h>
#include "lbsrootcenrepdefs.h"
 

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_LbsNetLocMgrTestSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_LbsNetLocMgrTestSuiteStepBase();
	CTe_LbsNetLocMgrTestSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	
//Please add your class members which will be common to all individual test steps:
protected:
	TRequestStatus iStat;
	TRequestStatus iStatusUpd;
	TRequestStatus iStatusReq;
	RLbsPositionUpdates			iPositionUpd; // to read location from manager	
	RLbsPositionUpdateRequests	iPositionReq; // to send request to manager
	};

#endif
