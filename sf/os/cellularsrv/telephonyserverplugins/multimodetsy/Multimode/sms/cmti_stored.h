// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalAll 
*/

#ifndef __CMTI_STORED_H__
#define __CMTI_STORED_H__

#include "smsbase.H"

// Forward class declarations
class CReceiveSmsQueue;


class CATSmsWaitForAndHandleCMTIStored : public CATBase
	{
public:		// public methods
	static CATSmsWaitForAndHandleCMTIStored* NewL(CATIO* aIo, CTelObject* aTelObject, CPhoneGlobals* aGlobals, 
										          CReceiveSmsQueue& aReceiveQueue);
	~CATSmsWaitForAndHandleCMTIStored();

	void Enable();
	void Disable();


private:		// private methods
	CATSmsWaitForAndHandleCMTIStored(CATIO* aIo, CTelObject* aTelObject, CPhoneGlobals* aGlobals, 
							         CReceiveSmsQueue& aReceiveQueue);
	// Utility methods
	void CMTIResponseL(RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& aAttr);

	// Pure virtuals from CATBase
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void EventSignal(TEventSource aSource);

private:		// private data
	CCommChatString* iMsgArrivedExpectString;	// The CMTI expect string
	CReceiveSmsQueue&  iReceiveQueue;			// Reference to our client's receive queue
	};

#endif
