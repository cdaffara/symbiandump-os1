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
 @file te_lbsprivfwtestchannelteststepbase.h
*/

#ifndef _TE_LBS_PRIVFW_TESTCHANNEL_TEST_STEPBASE_H_
#define _TE_LBS_PRIVFW_TESTCHANNEL_TEST_STEPBASE_H_


#include <test/TestExecuteStepBase.h>
// Please add your include here if you have
#include "tlbsprivfwtestchannel.h" 

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_LbsPrivfwTestChannelTestStepBase : public CTestStep //, public MLbsTestChannelObserver
	{
	public:
		virtual ~CTe_LbsPrivfwTestChannelTestStepBase();
		CTe_LbsPrivfwTestChannelTestStepBase();
		virtual TVerdict doTestStepPreambleL(); 
		virtual TVerdict doTestStepPostambleL();
		
	protected:
	//	void LogMessageInfo(RLbsPrivFwChannel::TLbsPrivFwChannelId aChannelId, const TLbsPrivFwMsgBase& aMessage);
	//	void ProcessTestChannelMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage);
		
		void TestPrivFwTestChannelMessagesEquivalentL(const TLbsTestChannelMsgBase& aMsg,
												const TLbsTestChannelMsgBase& aMsg2);

	//Please add your class members which will be common to all individual test steps:
	protected:
		HBufC8*		iReadData;
		HBufC8*		iWriteData;
		CActiveScheduler* iScheduler;
	};

#endif //_TE_LBS_PRIVFW_TESTCHANNEL_TEST_STEPBASE_H_
