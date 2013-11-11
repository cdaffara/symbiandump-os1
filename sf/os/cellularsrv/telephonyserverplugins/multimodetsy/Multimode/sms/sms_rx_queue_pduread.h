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

#ifndef __SMS_RX_QUEUE_PDUREAD_H__
#define __SMS_RX_QUEUE_PDUREAD_H__

#include "smsbase.H"
#include "sms_rx_queue.h"  // for CReceiveSmsQueue which CATSmsReadPDU is a friend of 


class CATSmsReadPDU : public CATSmsCommands
	{
public:
	static CATSmsReadPDU* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aGlobals,CReceiveSmsQueue& aClient);

private:		// Private methods
	CATSmsReadPDU(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aGlobals,CReceiveSmsQueue& aClient);

	// Pure virtual from base classes
	virtual void Complete(TInt aError, TEventSource aSource);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource, TInt aStatus);
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	
	// Utility method
	void ParseAndStoreCMGRResponseL();

private:		// Private data

	CReceiveSmsQueue& iClient; // Reference to our client, of which we are a friend class

	// State machine possible and current state
	// ENoState must be 0 so that ENoState is the state iState is initialised
	// to when an object of this class is constructed.
	enum TState {ENoState=0,ESetPhonesPrefMemory,EReadSMSFromPhone};
	TState iState;

	TStorageType iStoreName;

	// Pointer to the client data that this class will have to fill.
	// This object is not owned by this class
	CReceiveSmsQueue::TItem* iClientItem;
	
	// +CMGR: expect string. A pointer is needed to it so we can remove the 
	// expect string after adding it.
	// This class does not own this object.
	CCommChatString* iCMGRExpectString;
	};

#endif





