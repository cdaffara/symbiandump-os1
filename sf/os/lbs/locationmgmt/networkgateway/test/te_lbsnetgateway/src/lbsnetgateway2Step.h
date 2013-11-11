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
 @file lbsnetgateway2Step.h
*/
#if (!defined __LBSNETGATEWAY2_STEP_H__)
#define __LBSNETGATEWAY2_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbsnetgatewaySuiteStepBase.h"

class Clbsnetgateway2Step : public CTe_lbsnetgatewaySuiteStepBase
	{
public:
	Clbsnetgateway2Step();
	~Clbsnetgateway2Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	void ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage);
	
	void WaitForResponse();

// Please add/modify your class members here:
private:
	TNetGatewayMsg iSentProtocolMsg[4];
	TLbsAsistanceDataGroup iSentAgpsDataGroup;	
	TInt iSentMsgCount;
	TInt iReceivedMsgCount;
	TLbsAsistanceDataGroup  iRequestedDataGroup;
	TLbsNetSessionIdInt 	iSessionId;
	TNetGatewayMsg 			iNetGatMsg;
	
	TLbsNetPosRequestOptionsAssistanceInt iOptions;
	CActiveSchedulerWait iActiveWait;
	};

_LIT(Klbsnetgateway2Step,"lbsnetgateway2Step");

#endif
