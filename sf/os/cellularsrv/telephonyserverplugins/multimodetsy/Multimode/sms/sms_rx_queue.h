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
// Class declaration for CReceiveSmsQueue
// This class declaration is intended to be private and must not be exported.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __SMS_RX_QUEUE_H__
#define __SMS_RX_QUEUE_H__

#include <et_tsy.h>		// for TTsyReqHandle


// Forward class declarations
class CMobileSmsMessaging;
class CATSmsReadPDU;

class CReceiveSmsQueue : public CBase
	{
	friend class CATSmsReadPDU;		// CATSmsReadPDU needs to access CompleteClientReqIfPossible

public:
	static CReceiveSmsQueue* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aGlobals,CMobileSmsMessaging& aMobileSmsMessaging);
	~CReceiveSmsQueue();
	
	void Push(const RMobileSmsMessaging::TMobileSmsGsmTpdu& aPdu, 
			  const RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& aAttr);

	void PopAndCompleteClientWhenPossible(const TTsyReqHandle aReqHandle,
	                                      RMobileSmsMessaging::TMobileSmsGsmTpdu* aPdu, 
										  RMobileSmsMessaging::TMobileSmsReceiveAttributesV1* aAttr);

	void PopAndCompleteClientWhenPossibleCancel();
	void CompleteClientReqWithError(TInt aError);

	TBool inline ClientReqOutstanding() {return iClientReqOutstanding;};


	//
	// ReadPDUFromPhone should be called by CATCommands::Complete at which time the 
	// TSY base classes guarantee that there is not another AT command which is running.
	// ReadPDUFromPhone starts the reading of the PDU of the message at the front of
	// the queue if we do not already store it's PDU.
	void ReadPDUFromPhone();

private:  // private methods 
	CReceiveSmsQueue(CMobileSmsMessaging& aMobileSmsMessaging,CPhoneGlobals& aGlobals);

	void ConstructL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aGlobals);

	// Queue utility methods
	class TItem
		{
	public:
		RMobileSmsMessaging::TMobileSmsGsmTpdu				iPdu;
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1	iAttr;
		};

	TBool QueueEmpty();
	TBool QueueFull();

	// Client completion utility method
	void CompleteClientReqIfPossible();

private:  // private data
	enum { KReceiveSmsQueueSize=5 };

	CPhoneGlobals& iGlobals;					// Reference to the TSY global CPhoneGlobals object
	CMobileSmsMessaging& iMobileSmsMessaging;	// Reference to the client who owners the instance of this class

	CATSmsReadPDU* iATReadPDU;		// This object owned by this class

	//
	// Fixed size C array as we want to ensure that all the memory we
	// need is pre-allocated for us on construction. This is so the TSY
	// does not fail to receive a SMS because the user has decided to edit 
	// 'War and Peace' in Symbian Word.
	//
	TItem iQueue[KReceiveSmsQueueSize];
	TInt iQueueFront;	// The front of the queue (where items are popped off)
	TInt iQueueBack;	// The back of the queue (where items are pushed on)

	//
	// Outstanding client request storage
	TBool												iClientReqOutstanding;
	TTsyReqHandle										iClientReq;		
	RMobileSmsMessaging::TMobileSmsGsmTpdu*				iClientReqPdu;	// This class does not own this object
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1*	iClientReqAttr;	// This class does not own this object
	};

#endif
