// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __SUSRTCADAPTATION_H__
#define __SUSRTCADAPTATION_H__

#include <e32base.h>
#include "ssmadaptation.h"
#include "ssmdebug.h"
#include "ssmadaptationcmn.h"

/**
CRtcAdaptation

This class implements the RTC Adaptation related functionality as part of 
Adaptation server.This class implements an active object to route the requests
to Adaptation plugins loaded by CSsmAdaptationServer.When requests are received
from multiple clients this class queues the requests and processes them in order.
Adaptation server  handles only one request from each session and uses Cancel()
methods to cancel any of the session's requests made before.
The queueing mechanism is to handle multiple clients at a time not to handle multiple
requests from one particular clients session.

@internalComponent
*/


class CRtcAdaptation : public CActive
{
public:
	static CRtcAdaptation* NewL(MRtcAdaptation& aAdaptation);
	~CRtcAdaptation();

	void Release();

	void DoValidateRtcL(const RMessage2& aMessage);
	void DoSetWakeupAlarmL(const RMessage2& aMessage);
	void DoUnsetWakeupAlarmL(const RMessage2& aMessage);
	void DoRtcCancelL(const RMessage2& aMessage);

protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();

private:
	CRtcAdaptation(MRtcAdaptation& aAdaptation);
	void SubmitOrQueueL(const RMessage2 &aMessage);	 
	void WriteResponseDataToClientMessageL();
	void SubmitL(CAdaptationMessage*& aMessage);

private:

	CAdaptationMessage *iCurrentMessage;
	RSsmAdaptationRequestQueue iPendingRequestsQueue;	
	
	HBufC8 *iParam;
	MRtcAdaptation& iRtcAdaptation;
	TPckgBuf<TBool> iValidityPckg;
};

#endif // __SUSRTCADAPTATION_H__
