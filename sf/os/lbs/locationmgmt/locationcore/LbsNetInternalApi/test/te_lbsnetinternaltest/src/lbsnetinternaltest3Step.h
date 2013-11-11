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
 @file lbsnetinternaltest3Step.h
*/
#if (!defined __LBSNETINTERNALTEST3_STEP_H__)
#define __LBSNETINTERNALTEST3_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_lbsnetinternaltestSuiteStepBase.h"

class Clbsnetinternaltest3Step : public CTe_lbsnetinternaltestSuiteStepBase
	{
public:
	Clbsnetinternaltest3Step();
	~Clbsnetinternaltest3Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);

	TLbsNetInternalMsgBase* CreateTLbsNetLocationRequestMsgL();

	TLbsNetInternalMsgBase* CreateTLbsNetLocationResponseMsgL();

	TLbsNetInternalMsgBase* CreateTLbsNetTransmitLocationRequestMsgL();

	TLbsNetInternalMsgBase* CreateTLbsNetTransmitLocationCancelMsgL();

	TLbsNetInternalMsgBase* CreateTLbsNetMtLrRequestMsgL();

	TLbsNetInternalMsgBase* CreateTLbsNetMtLrReponseMsgL();

	TLbsNetInternalMsgBase* CreateTLbsNetSessionCompleteMsgL();

	TLbsNetInternalMsgBase* CreateTLbsNetECancelFromPrivacyControllerMsgL();


// Please add/modify your class members here:
private:
	TLbsNetInternalMsgBase* iSentMsg;
	};

_LIT(Klbsnetinternaltest3Step,"lbsnetinternaltest3Step");

#endif
