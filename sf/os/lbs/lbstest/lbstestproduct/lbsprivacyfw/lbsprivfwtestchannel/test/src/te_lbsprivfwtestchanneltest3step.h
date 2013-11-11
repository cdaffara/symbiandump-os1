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
 @file te_lbsprivfwtestchanneltest3step.h
*/
#ifndef _TE_LBS_PRIVFW_TESTCHANNEL_TEST3_STEP_H_
#define _TE_LBS_PRIVFW_TESTCHANNEL_TEST3_STEP_H_

#include <test/TestExecuteStepBase.h>
#include "te_lbsprivfwtestchannelteststepbase.h"
#include "tlbsprivfwtestchannel.h"

_LIT(KLbsPrivfwTestChannelTest3Step,"LbsPrivfwTestChannelTest3Step");

class  CTe_LbsPrivfwTestChannelTest3Step : public CTe_LbsPrivfwTestChannelTestStepBase, public MLbsTestChannelObserver
	{
public:
	CTe_LbsPrivfwTestChannelTest3Step();
	~CTe_LbsPrivfwTestChannelTest3Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void ProcessTestChannelMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage);

	TLbsTestChannelMsgBase* CreateTLbsPrivacyRequestMsgL();

	TLbsTestChannelMsgBase* CreateTLbsPrivacyResponseMsgL();

// Please add/modify your class members here:

	//From MLbsTestChannelRequestObserver
	//void ProcessTestChannelRequestMessage(const TLbsTestChannelMsgBase& aMessage);

	void ProcessNetChannelMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage);

private:
	TLbsTestChannelMsgBase* iSentMsg;

	};

#endif //_TE_LBS_PRIVFW_TESTCHANNEL_TEST3_STEP_H_
