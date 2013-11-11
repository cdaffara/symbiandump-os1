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
// These are the integration unit tests for the CLbsSuplPush, CLbsSuplPushImpl and related classes
// 
//

#if (!defined __TE_LBSSUPLPUSHINTEG_STEP__)
#define __TE_LBSSUPLPUSHINTEG_STEP__

#include <test/testexecutestepbase.h>
#include <lbs/lbssuplpush.h>
#include <lbs/lbssuplpushreceiver.h>

#include "Te_LbsSuplPushBaseStep.h"

/** The name of the test step */
_LIT(KLbsSuplPushIntegStep, "LbsSuplPushIntegStep");

/**
The integration unit tests for the CSuplPushImpl class.
*/
class CTe_LbsSuplPushIntegStep : public CTe_LbsSuplPushBaseStep, public MLbsSuplPushObserver,
	public MLbsSuplPushRecObserver
	{

public:
	CTe_LbsSuplPushIntegStep();
	
	virtual void OnSuplInitComplete(TLbsSuplPushChannel aChannel, 
			TLbsSuplPushRequestId aReqId, TInt aError, TInt aReserved);
	virtual void OnSuplInit(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId, TDesC8& aMsg);
	
protected:
	void ReallyDoTestStepL();

private:	
	void SendMessageL(TUint aNum);
	
private:	
	/** The number to fill the messages sent with */
	TUint iMsgPattern; 
	/** The message buffer */
	TBuf8<32> iMessage;
	/** The request id of a message sent */
	TLbsSuplPushRequestId iReqId;
	/** The expected error for the scenario */
	TInt iError;
	/** The number of messages to send/receive in the scenario */
	TUint iMsgCount;
	
	/** The message sender*/
	CLbsSuplPush* iSuplPush;
	/** The message receiver*/
	CLbsSuplPushRec* iSuplPushRec;	
	};

#endif //__TE_LBSSUPLPUSHINTEG_STEP__
