// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// These are the unit tests for the CLbsSuplPushRecChannel class
// 
//

#if (!defined __TE_LBSSUPLPUSHAPIRECCHANNEL_STEP_BASE__)
#define __TE_LBSSUPLPUSHAPIRECCHANNEL_STEP_BASE__

#include <test/testexecutestepbase.h>
#include "Te_LbsSuplPushRecObserver.h"
#include "Te_LbsSuplPushRecChannel.h"

/** The name of the test step */
_LIT(KLbsSuplPushApiRecChannelStep, "LbsSuplPushApiRecChannelStep");


/**
The unit tests for the CLbsSuplPushRecChannel class. This test step runs various successfull
and error scenarios while receiving the message.
*/
class CTe_LbsSuplPushApiRecChannelStep : public CTestStep
	{
public:
	CTe_LbsSuplPushApiRecChannelStep();
	virtual ~CTe_LbsSuplPushApiRecChannelStep();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
private:
	void SendMessageL(TInt aCategory, const TDesC8& aMessage, TInt aReqId, TInt aAckId);
	void SendReceiveMessageL(const TDesC8& aMessage);
	void CheckNoMessageL();
	
private:
	/** An active scheduler created and installed for this step */
	CActiveScheduler* iScheduler;
	/** An utility implementation of the MLbsSuplPushRecObserver */
	TLbsSuplPushRecObserver iRecObserver;
	/** A channel that is tested */
	CLbsSuplPushRecChannel* iChannel;
	};

#endif //__TE_LBSSUPLPUSHAPIRECCHANNEL_STEP_BASE__
