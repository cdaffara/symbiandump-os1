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
 @file Te_lbsnetinternaltestSuiteStepBase.h
*/

#if (!defined __TE_LBSNETINTERNALTEST_STEP_BASE__)
#define __TE_LBSNETINTERNALTEST_STEP_BASE__
#include <test/TestExecuteStepBase.h>
#include "lbscommoninternaldatatypes.h"
#include "lbsnetinternalapi.h" 
#include "lbsagpsngmsgs.h" 
#include "lbsnrhngmsgs.h" 
#include "lbsngnlmint.h"
#include "lbspsyngmsgs.h"

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_lbsnetinternaltestSuiteStepBase : public CTestStep, public MLbsNetChannelObserver

	{
public:
	virtual ~CTe_lbsnetinternaltestSuiteStepBase();
	CTe_lbsnetinternaltestSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	
protected:
	void LogMessageInfo(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	
	void TestNetInternalMessagesEquivalentL(const TLbsNetInternalMsgBase& aMsg,
											const TLbsNetInternalMsgBase& aMsg2);

//Please add your class members which will be common to all individual test steps:
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	CActiveScheduler* iScheduler;
	};

#endif
