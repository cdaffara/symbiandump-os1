/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_testprotocolmodulesuitestepbase.h
*/

#if (!defined __TE_TESTPROTOCOLMODULE_STEP_BASE__)
#define __TE_TESTPROTOCOLMODULE_STEP_BASE__

#include <test/testexecutestepbase.h>
#include <ecom/ecom.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include "cgatewayobserver.h"
#include "cnetworkobserver.h"


/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_testprotocolmoduleSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_testprotocolmoduleSuiteStepBase();
	CTe_testprotocolmoduleSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	
	void ResetObserversL();
	void ResetGatewayObserverL();
	void ResetNetworkObserverL();
	TVerdict CheckGatewayCallbackL(TInt aExpectedResult);
	TVerdict CheckNetworkCallbackL(TInt aExpectedResult);
	
	void SetDummyAssistanceData(RLbsAssistanceDataBuilderSet& aAssistData, 
								TLbsAsistanceDataGroup aDataRequestMask);

protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	
	// Active scheduler
	CActiveScheduler* iSched;
	
	// Module Observer on LBS Gateway side
	CGatewayObserver* iGatewayObserver;
	
	// Module Observer on Network side
	CNetworkObserver* iNetworkObserver;
	};

#endif
