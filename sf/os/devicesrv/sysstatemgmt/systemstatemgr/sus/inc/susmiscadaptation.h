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

#ifndef __SUSMISCADAPTATION_H__
#define __SUSMISCADAPTATION_H__

#include <e32base.h>
#include "ssmadaptation.h"
#include "ssmdebug.h"
#include "ssmadaptationcmn.h"


/**
CMiscAdaptation

This class implements the Miscellaneous Adaptation related functionality as part of 
Adaptation server.This class implements an active object to route the requests
to Adaptation plugins loaded by CSsmAdaptationServer.When requests are received
from multiple clients this class queues the requests and processes them in order.
Adaptation server  handles only one request from each session and uses Cancel()
methods to cancel any of the session's requests made before.
The queueing mechanism is to handle multiple clients at a time not to handle multiple
requests from one particular clients session.

@internalComponent
*/

class CMiscAdaptation : public CActive
{
public:
	static CMiscAdaptation* NewL(MMiscAdaptation& aAdaptation);
	~CMiscAdaptation();

	void Release();

	void DoSecurityStateChangeL(const RMessage2& aMessage);
	void DoGetGlobalStartupModeL(const RMessage2& aMessage);
	void DoPrepareSimLanguagesL(const RMessage2& aMessage);
	void DoMiscAdaptationCancelL(const RMessage2& aMessage);
	void DoMiscAdaptationHiddenResetL(const RMessage2& aMessage);
	void DoGetSimLanguagesArrayL(const RMessage2& aMessage);

protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();

private:
	CMiscAdaptation(MMiscAdaptation& aAdaptation);
	void SubmitOrQueueL(const RMessage2 &aMessage);
	void WriteResponseDataToClientMessageL();
	void Submit(CAdaptationMessage*& aMessage);

private:
	CAdaptationMessage *iCurrentMessage;
	RSsmAdaptationRequestQueue iPendingRequestsQueue;	
	
	MMiscAdaptation& iMiscAdaptation;
	TPckgBuf<TInt> iResponsePckg;
	TPckgBuf<TInt> iModePckg;
	TPckgBuf<TInt> iSizePckg;
	TPckgBuf<TInt> iListPckg;
	TPckgBuf<TBool> iHiddenResetPckg;
};

#endif // __SUSMISCADAPTATION_H__
