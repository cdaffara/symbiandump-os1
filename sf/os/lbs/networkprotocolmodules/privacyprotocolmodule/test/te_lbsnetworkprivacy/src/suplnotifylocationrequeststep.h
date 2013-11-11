/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file suplnotifylocationrequeststep.h
*/
#if (!defined __SUPLNOTIFYLOCATIONREQUEST_STEP_H__)
#define __SUPLNOTIFYLOCATIONREQUEST_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsnetworkprivacysuitestepbase.h"


_LIT(KSuplNotifyLocationRequestStep,"SuplNotifyLocationRequestStep");

class CSuplNotifyLocationRequestStep : public CTe_LbsNetworkPrivacySuiteStepBase
	{
public:
	CSuplNotifyLocationRequestStep();
	~CSuplNotifyLocationRequestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
   	// from MLbsNetChannelObserver
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	};
#endif //suplnotifylocationrequeststep.h
