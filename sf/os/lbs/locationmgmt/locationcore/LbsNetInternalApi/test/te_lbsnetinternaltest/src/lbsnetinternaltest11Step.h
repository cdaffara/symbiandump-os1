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
 @file lbsnetinternaltest11Step.h
*/
#if (!defined __LBSNETINTERNALTEST11_STEP_H__)
#define __LBSNETINTERNALTEST11_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbsnetinternaltestSuiteStepBase.h"

class Clbsnetinternaltest11Step : public CTe_lbsnetinternaltestSuiteStepBase
	{
public:
	Clbsnetinternaltest11Step();
	~Clbsnetinternaltest11Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);

	TLbsNetInternalMsgBase* CreateTLbsCellLocationRequestMsgL();

	TLbsNetInternalMsgBase* CreateTLbsCellLocationResponseMsgL();
	
	TLbsNetInternalMsgBase* CreateTLbsCellLocationCancelMsgL();


// Please add/modify your class members here:
private:
	TLbsNetInternalMsgBase* iSentMsg;
	};

_LIT(Klbsnetinternaltest11Step,"lbsnetinternaltest11Step");

#endif
