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
 @file lbsnetgateway5Step.h
*/
#if (!defined __LBSNETGATEWAY5_STEP_H__)
#define __LBSNETGATEWAY5_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbsnetgatewaySuiteStepBase.h"

/* Enum of test steps/states that this
   test goes through.
*/
enum TTestState
	{
	ETestStateUnknown = 0,
	ETestStateStart,
	ETestStateWaitForNetworkLocationRequest,
	ETestStateWaitForRefPosition,
	ETestStateWaitForLocationRequest,
	ETestStateWaitForLocationResponse,
	ETestStateWaitForSessionComplete,
	ETestStateFinish
	};

class Clbsnetgateway5Step : public CTe_lbsnetgatewaySuiteStepBase
	{
public:
	Clbsnetgateway5Step();
	~Clbsnetgateway5Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void StartTest();
	void GoToNextStateL();
	
	void TestPositionsAreEqualL(const TPosition& pos1, const TPosition& pos2);
	
	void ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage);
	void ProcessNetworkLocationResponseMessage(const TLbsNetLocMsgBase& aMessage);
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);

// Please add/modify your class members here:
private:
	TTestState iState;

	TNetGatewayMsg iTestPmMsg;
	TLbsNetLocMsgBase* iNetworkLocationMsg;
	TLbsNetLocationResponseMsg iNetworkRequestMsg;
	
	TPosition iReferencePosition;
	TPositionInfo iReceivedReferencePosition;
	
	TBool iReceivedNetRequestSessionComplete;
	TBool iReceivedNetLocationSessionComplete;
	};

_LIT(Klbsnetgateway5Step,"lbsnetgateway5Step");

#endif
