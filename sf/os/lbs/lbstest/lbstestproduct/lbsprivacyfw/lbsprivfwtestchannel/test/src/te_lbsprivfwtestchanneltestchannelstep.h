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
 @file te_lbsprivfwtestchanneltestchannelstep.h
*/
#ifndef _TE_LBS_PRIVFW_TESTCHANNEL_TESTCHANNEL_STEP_H_
#define _TE_LBS_PRIVFW_TESTCHANNEL_TESTCHANNEL_STEP_H_

#include <test/TestExecuteStepBase.h>
#include "te_lbsprivfwtestchannelteststepbase.h"
#include <lbs/test/tlbstestrequestchannel.h>

_LIT(KLbsPrivfwTestChannelTestChannelStep,"LbsPrivfwTestChannelTestChannelStep");

class  CTe_LbsPrivfwTestChannelTestChannelStep : public CTe_LbsPrivfwTestChannelTestStepBase, public MLbsTestChannelRequestObserver
	{
public:
	CTe_LbsPrivfwTestChannelTestChannelStep();
	~CTe_LbsPrivfwTestChannelTestChannelStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void ProcessTestChannelMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage);

	TLbsTestChannelMsgBase* CreateTLbsPrivacyRequestMsgL();

	TLbsTestChannelMsgBase* CreateTLbsPrivacyResponseMsgL();

	// Please add/modify your class members here:

	//From MLbsTestChannelRequestObserver
	void ProcessTestChannelRequestMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage);

private:
	TLbsTestChannelMsgBase *iSentMsg;
	//TUint iRequestId;

	};

#endif //_TE_LBS_PRIVFW_TESTCHANNEL_TESTCHANNEL_STEP_H_
