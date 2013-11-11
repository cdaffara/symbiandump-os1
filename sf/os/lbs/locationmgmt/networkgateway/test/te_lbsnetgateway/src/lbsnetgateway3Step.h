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
 @file lbsnetgateway3Step.h
*/
#if (!defined __LBSNETGATEWAY3_STEP_H__)
#define __LBSNETGATEWAY3_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbsnetgatewaySuiteStepBase.h"

class Clbsnetgateway3Step : public CTe_lbsnetgatewaySuiteStepBase
	{
public:
	Clbsnetgateway3Step();
	~Clbsnetgateway3Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	void ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage);	
	void ProcessNetworkLocationResponseMessage(const TLbsNetLocMsgBase& aMessage);
	
	// from MLbsNetChannelObserver
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);

// Please add/modify your class members here:
private:
	TNetGatewayMsg iTestPmMsg;
	TLbsNetLocMsgBase* iNetLocMsg;
	
	TBool iNetworkLocationResponse;
	TBool iNetChannelMessage;
	};

_LIT(Klbsnetgateway3Step,"lbsnetgateway3Step");

#endif
