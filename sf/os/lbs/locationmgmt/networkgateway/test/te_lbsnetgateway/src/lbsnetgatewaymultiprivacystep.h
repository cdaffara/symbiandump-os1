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
 @file lbsnetgatewaymultiprivacystep.h
*/
#if (!defined __LBSNETGATEWAYMULTIPRIVACY_STEP_H__)
#define __LBSNETGATEWAYMULTIPRIVACY_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbsnetgatewaySuiteStepBase.h"

class Clbsnetgatewaymultiprivacystep : public CTe_lbsnetgatewaySuiteStepBase
	{
public:
	Clbsnetgatewaymultiprivacystep();
	~Clbsnetgatewaymultiprivacystep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	void ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage);
	
	TNetGatewayMsg* CreatePrivacyRequestMsgLC(const TLbsNetSessionId& aSessionId);
	TNetGatewayMsg* CreateSessionCompleteMsgLC(const TLbsNetSessionId& aSessionId);

// Please add/modify your class members here:
private:
	enum TMultiPrivacyState
		{
		EMultiPrivacyCheckBuffer = 0,
		EMultiPrivacyInitialRequest,
		};

	CActiveSchedulerWait iActiveWait;
	const TLbsNetInternalMsgBase* iNetInternalMessage;
	TNetGatewayMsg iNetProtocolMessage;
	TMultiPrivacyState iState;
	RPointerArray<TNetGatewayMsg> iMsgArray;
	};

_LIT(Klbsnetgatewaymultiprivacystep,"lbsnetgatewaymultiprivacystep");

#endif
