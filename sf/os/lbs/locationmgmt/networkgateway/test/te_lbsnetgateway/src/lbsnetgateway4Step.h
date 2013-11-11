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
 @file lbsnetgateway4Step.h
*/
#if (!defined __LBSNETGATEWAY4_STEP_H__)
#define __LBSNETGATEWAY4_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbsnetgatewaySuiteStepBase.h"


/* List of tests/steps done within this test.
*/
enum TTestStep
	{
	ETestStepNotUsed = 0,
	
	// Location Request
	ETestStepLocReqBufferEmpty,		// 1.1) : Buffer is empty
	ETestStepLocReqPendingPrivReq,	// 1.2) : Buffer contains a previous privacy request
	ETestStepLocReqPendingLocReq,	// 1.3) : Buffer contains a previous location request
	ETestStepLocReqPendingCancel,	// 1.4) : Buffer contains a previous cancel request
	ETestStepLocReqPendingPrivReqAndLocReq,	// 1.5) : Buffer contains a matching privacy and location request
	
	// Privacy Request
	ETestStepPrivReqBufferEmpty,		// 2.1) : Buffer is empty
	ETestStepPrivReqPendingPrivReq,	// 2.2) : Buffer contains a previous privacy request
	ETestStepPrivReqPendingLocReq,	// 2.3) : Buffer contains a previous location request
	ETestStepPrivReqPendingCancel,	// 2.4) : Buffer contains a previous cancel request
	ETestStepPrivReqPendingPrivReqAndLocReq, // 2.5) : Buffer contains a matching privacy and location request

	// Session Complete
	ETestStepSessCompBufferEmpty,	// 3.1) : Buffer is empty
	ETestStepSessCompPendingPrivReq,	// 3.2) : Buffer contains a previous privacy request
	ETestStepSessCompPendingLocReq,	// 3.3) : Buffer contains a previous location request
	ETestStepSessCompPendingCancel,	// 3.4) : Buffer contains a previous cancel request
	ETestStepSessCompPendingPrivReqAndLocReq,	// 3.5) : Buffer contains a matching privacy and location request
	};
	

class Clbsnetgateway4Step : public CTe_lbsnetgatewaySuiteStepBase
	{
public:
	Clbsnetgateway4Step();
	~Clbsnetgateway4Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	// from MLbsNetChannelObserver
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	
	void FinishTestStep();

// Please add/modify your class members here:
private:
	TInt iNumExpectedMessages;
	TInt iNumReceivedMessages;
	TNetGatewayMsg iMsg;
	TNetGatewayMsg iExpectedMessages[8];
	TTestStep iTestStep;
	
	TLbsNetSessionId iSessionId;
	SNetMsgProcessLocationRequest iLocReq;
	SNetMsgProcessPrivacyRequest iPrivReq;
	SNetMsgProcessSessionComplete iSessComp;
	};

_LIT(Klbsnetgateway4Step,"lbsnetgateway4Step");

#endif
