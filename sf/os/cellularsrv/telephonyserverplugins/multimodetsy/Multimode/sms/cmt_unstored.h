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

#ifndef __CMT_UNSTORED_H__
#define __CMT_UNSTORED_H__

#include "smsbase.H"

// Forward class declarations
class CReceiveSmsQueue;


class CATSmsWaitForAndHandleCMTUnstored : public CATBase
	{
public:		// public methods
	static CATSmsWaitForAndHandleCMTUnstored* NewL(CATIO* aIo, CTelObject* aTelObject, CPhoneGlobals* aGlobals, 
										          CReceiveSmsQueue& aReceiveQueue);
	~CATSmsWaitForAndHandleCMTUnstored();

	void Enable();
	void Disable();


private:		// private methods
	CATSmsWaitForAndHandleCMTUnstored(CATIO* aIo, CTelObject* aTelObject, CPhoneGlobals* aGlobals, 
							         CReceiveSmsQueue& aReceiveQueue);
	// Utility methods
	void ParseFirstCMTLineL(TInt& aPduLen);
	void ParseSecondCMTLineL(RMobileSmsMessaging::TMobileSmsGsmTpdu& aPdu,RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& aAttr);

	// Pure virtuals from CATBase
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void EventSignal(TEventSource aSource);

private:		// private data
	// State machine states and current state
	enum TState {ENoState=0, EWaitForCMT, EWaitForCmtSecondLine, EWaitForCmtSecondLineThenDisable};
	TState iState;
	
	TInt iPduLen;		// Stores the length that the phone states the PDU is

	CCommChatString*	iCMTExpectString;	// The CMT expect string, this object not owned by this class
	CCommChatString*	iCDSExpectString;	// The CDS expect string, this object not owned by this class
	CCommChatString*	iWaitForAnything;	// The '*' anything expect string, this object not owned by this class

	CReceiveSmsQueue&	iReceiveQueue;			// Reference to our client's receive queue
	};

#endif
