// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file define the classes for SMS message queues
// 
//

/**
 @file
 @internalAll
*/

#ifndef SMSP_QUEUE_H__
#define SMSP_QUEUE_H__

#include <e32std.h>
#include "smspmain.h"
#include "Gsmumsg.h"
#include "smsuact.h"

class CSmsMessage;
class MSmsMessageObserver;
class MSmsComm;
class CSmsMessageWrite;
class TSmsSettings;
class CSmsSegmentationStore;
class CSmsMessageSend;
class CSmsPDUDelete;
class CSmspSetBearer;

/**
 *  Used by CSmsProtocol to hold CSmsMessage objects queued for sending
 *  or writing to the phone.
 *  @internalComponent
 */
class CQueuedSmsMessage : public CBase
	{
public:
	CQueuedSmsMessage(CSmsMessage* aSmsMessage,MSmsMessageObserver& aObserver,TUint aOptions);
	~CQueuedSmsMessage();
public:

	CSmsMessage* iSmsMessage;
	MSmsMessageObserver& iObserver;
	TUint iOptions;
	TSglQueLink iLink;
	};


/**
 *  @internalComponent
 */
class CSmspProtocolQueue : public CSmsuActiveBase
	{
	public:

		virtual void Start()=0;
		virtual void CancelObserver(MSmsMessageObserver& aObserver)=0;
		virtual void CompleteFirst(TInt aStatus)=0;

	protected:

		CSmspProtocolQueue(MSmsComm& aSmsComm, TInt aPriority);

	protected:

		MSmsComm& iSmsComm;
	};


/**
 *  @internalComponent
 */
class CSmspMessageQueue : public CSmspProtocolQueue
	{
	public:

		void Queue(CSmsMessage* aSmsMessage, MSmsMessageObserver& aObserver, TUint aOptions);
		void CancelObserver(MSmsMessageObserver& aObserver);

	protected:

		CSmspMessageQueue(MSmsComm& aSmsComm, TInt aPriority);
		~CSmspMessageQueue();

		void DoRunL();
		void DoCancel();
		void DoQueueL(CSmsMessage* aSmsMessage, MSmsMessageObserver& aObserver, TUint aOptions);
		void CompleteRequest(CQueuedSmsMessage* aQueuedMessage, TInt aStatus);
		void CompleteFirst(TInt aStatus);

		virtual void CompleteObserver(MSmsMessageObserver& aObserver, TInt aStatus, const CSmsMessage* aSmsMessage = NULL)=0;
		virtual void CancelRequestIfObserved(MSmsMessageObserver& aObserver)=0;

	protected:

		TSglQue<CQueuedSmsMessage> iMessageQueue;	//< Queue of messages
	};


/**
 *  @internalComponent
 */
class CSmspWriteQueue : public CSmspMessageQueue
	{
	public:

		static CSmspWriteQueue* NewL(MSmsComm& aSmsComm, const TSmsSettings& aSmsSettings, RMobilePhone& aGsmPhone, CSmsSegmentationStore& aSegmentationStore,TInt aPriority);
		void Start(); //< From CSmspProtocolQueue
		~CSmspWriteQueue();

	private:

		CSmspWriteQueue(MSmsComm& aSmsComm, TInt aPriority);
		void ConstructL(const TSmsSettings& aSmsSettings, RMobilePhone& aGsmPhone, CSmsSegmentationStore& aSegmentationStore);
		void CancelRequestIfObserved(MSmsMessageObserver& aObserver);
		void CompleteObserver(MSmsMessageObserver& aObserver, TInt aStatus, const CSmsMessage* aSmsMessage = NULL); //< From CSmspMessageQueue
		void Complete(TInt aStatus); //< From CSmsuActiveBase

	private:

		CSmsMessageWrite* iSmsMessageWrite;
	};


/**
 *  @internalComponent
 */
class CSmspSendQueue : public CSmspMessageQueue
	{
	public:

		static CSmspSendQueue* NewL(MSmsComm& aSmsComm, CSmsSegmentationStore& aSegmentationStore, const TSmsSettings& aSmsSettings, const RMobileSmsMessaging::TMobileSmsCapsV1& aMobileSmsCaps, RMobileSmsMessaging& aSmsMessaging, TInt aPriority, CSmspSetBearer& aSmspSetBearer);
		void Start(); //< From CSmspProtocolQueue
		~CSmspSendQueue();

	private:

		CSmspSendQueue(MSmsComm& aSmsComm, TInt aPriority);
		void ConstructL(CSmsSegmentationStore& aSegmentationStore, const TSmsSettings& aSmsSettings, const RMobileSmsMessaging::TMobileSmsCapsV1& aMobileSmsCaps, RMobileSmsMessaging& aSmsMessaging, CSmspSetBearer& aSmspSetBearer);
		void CancelRequestIfObserved(MSmsMessageObserver& aObserver);
		void CompleteObserver(MSmsMessageObserver& aObserver, TInt aStatus, const CSmsMessage* aSmsMessage = NULL); //< From CSmspMessageQueue
		void Complete(TInt aStatus); //< From CSmsuActiveBase

	private:

		CSmsMessageSend* iSmsMessageSend;
	};


/**
 *  Used by CSmsProtocol to hold CSmsMessage objects queued for deletion
 *  from the phone.
 *  @internalComponent
 */
class CQueuedPDUDelete : public CBase
	{
public:

	static CQueuedPDUDelete* NewL(const CArrayFix<TGsmSmsSlotEntry>& aSlotArray,MSmsMessageObserver* aObserver);
	~CQueuedPDUDelete();

private:

	CQueuedPDUDelete(MSmsMessageObserver* aObserver);
	void ConstructL(const CArrayFix<TGsmSmsSlotEntry>& aSlotArray);

public:

	CArrayFixFlat<TGsmSmsSlotEntry> iLocationArray;
	MSmsMessageObserver* iObserver;
	TSglQueLink iLink;
	};


/**
 *  @internalComponent
 */
class CSmspDeleteQueue : public CSmspProtocolQueue
	{
	public:

		void Queue(const CArrayFix<TGsmSmsSlotEntry>& aSlotArray, MSmsMessageObserver* aObserver);

		static CSmspDeleteQueue* NewL(MSmsComm& aSmsComm, const TSmsSettings& aSmsSettings, RMobileSmsMessaging& aSmsMessaging, TInt aPriority);
		void Start(); //< From CSmspProtocolQueue
		void CancelObserver(MSmsMessageObserver& aObserver); //< From CSmspProtocolQueue
		~CSmspDeleteQueue();

    protected:
        void RunL();

	private:

		CSmspDeleteQueue(MSmsComm& aSmsComm, TInt aPriority);
		void ConstructL(const TSmsSettings& aSmsSettings, RMobileSmsMessaging& aSmsMessaging);
		void CompleteObserver(MSmsMessageObserver& aObserver, TInt aStatus, const CSmsMessage* aSmsMessage = NULL); //< From CSmspMessageQueue
		void CompleteFirst(TInt aStatus); //< From CSmspProtocolQueue
		void DoQueueL(const CArrayFix<TGsmSmsSlotEntry>& aSlotArray, MSmsMessageObserver* aObserver);

		void DoRunL();
		void DoCancel(); //< From CActive

		void CompleteRequest(CQueuedPDUDelete* aQueuedMessage, TInt aStatus);

	private:

		CSmsPDUDelete* iSmsPDUDelete;
		TSglQue<CQueuedPDUDelete> iDeleteQueue;
	};

#endif //< SMSP_QUEUE_H__
