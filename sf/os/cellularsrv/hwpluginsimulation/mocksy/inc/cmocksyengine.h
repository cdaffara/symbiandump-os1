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
//

#ifndef CMOCKSYENGINE_H
#define CMOCKSYENGINE_H

#include <e32def.h>
#include <e32base.h>
class CRotatingStrBuf;

#define KMaxLogLineSize 255

/**
Listener to CMockSYEngine events
*/
class MMockSYEngineListener
	{
public:
	enum TNotificationType
		{
		EHandlingTerminated,	//< all messages are handled
		EFailure,				//< receive message doesn't correspond to the expected one
		};

	virtual void Notify(TNotificationType aNotification) =0;	
	};


/**
Abstract class for command data
*/
class MMockSyEngineData
	{
public:
	/**
	Comparator: compare the data with it packaged version from an Event
	*/
	virtual TBool operator ==(const TDesC8& aData) const =0;
	};


/**
Abstract class for the logger
*/

/**
Mock SY engine base class
*/
class CMockSYEngine : public CBase
	{
public:
	IMPORT_C virtual ~CMockSYEngine();

public:
	/**
	Called by the SY, handle received command by verifing that is the expected command
	and queuing the conresponding completions
	Return KErrCorrupt it it's not the expected command
	*/
	IMPORT_C TInt ExecuteCommandL(TInt aCmdId, MMockSyEngineData& aData);
	/**
	Check data or not depending on the aCheckData
	*/
    IMPORT_C TInt ExecuteCommandL(TInt aCmdId, MMockSyEngineData& aData, TBool aCheckData);	

public:	
	inline void SessionClosed();
	inline void QueueEpectedCallL(TInt aCmdId, TInt aErrorCode, TBool aLeave, HBufC8* aData);
	inline void QueueReturnCallL(TInt aCmdId,  TInt aErrorCode, TInt aDelay, HBufC8* aData);
	HBufC* GetNextLogLine();
	
	inline TBool HasWaitingEvents() const;
	inline TBool HasPendingEvents() const;
	inline TBool HasFailure() const;
        IMPORT_C void CheckAndUpdateTransId(TUint8 aTransId, TInt aCommandId);
	
public: // listener API
	void AddListenerL(MMockSYEngineListener& aListener);
	void RemoveListener(MMockSYEngineListener& aListener);
    TInt PauseCompletion();
    TInt ResumeCompletion();

protected:
	IMPORT_C virtual void ConstructL();
	IMPORT_C CMockSYEngine();

protected:
	/**
	A LTSY event.
	It can be a message as received from the LTSY or a completion to send to CTSY
	*/
	class TMockSYEvent
		{
	public:
		enum TEventType
			{
			EMessage,
			ECompletion
			};
		TEventType	iEventType;					//< Event type
		TInt 		iCmdId;						//< expected cmdId or cmdId to complete
		TInt 		iResultCode;				//< Message or completion result code 
		TBool		iLeave;						//< If True, leave with iResultCode error
		TInt		iDelay;						//< for completion: delay before sending the completion
		RBuf8		iData;						//< Message or completion data
	public:
		TDblQueLink	iWaitingEventQueLink;		//< Link for the waiting event queue
		TDeltaQueLink iPendingEventQueLink;		//< Link for the pending event queue
		};

protected:
	/**
	Complete aEvent. Subclass must implement this function and complete the 
	SY request
	*/ 
	virtual void DoCompleteEventL(const TMockSYEvent& aEvent) =0;
	/**
	Queue an event. 
	*/
	IMPORT_C virtual void DoQueueEventL(TMockSYEvent::TEventType aType, TInt aCmdId, HBufC8* aData, 
		TInt aResultCode, TBool aLeave, TInt aDelay);
	/**
	 	Returns if MockSY can ignore a given unexpected IPC error.
	 */
	IMPORT_C virtual TBool CanIgnoreUnexpectedIpc(TInt aCmdId);
	
private:
	TDblQue<TMockSYEvent> 	iWaitingEventQue;
	TDeltaQue<TMockSYEvent>	iPendingEventQue;
	TBool iFailure;

	CPeriodic* iTimer;

	RPointerArray<MMockSYEngineListener> iListeners;
	
private:
	void DoTimerCallbackL();

protected:
	IMPORT_C virtual void Failure();
	IMPORT_C virtual void Reset();

private: // listener API
	void NotifyListeners(MMockSYEngineListener::TNotificationType aNotification);
	
protected: // logging: can be override to log specific informations
	IMPORT_C virtual void LogRequest(TInt aCmdId, const MMockSyEngineData& aData,TInt aResultCode);
	IMPORT_C virtual void LogCompletion(TInt aCmdId, const TDesC8& aData,TInt aResultCode);
	IMPORT_C virtual void LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,TInt aExpectedCmd,const TDesC8& aExpectedData, TBool aIsErrorIgnored=EFalse);	
	IMPORT_C virtual void Log(const TDesC& aDesc);
	
private: // static timer callback
	CRotatingStrBuf* iRotatingLog;
	static TInt TimerCallbackL(TAny* aPtr);
	TBool iPaused;
	};
	

// inlines
inline void CMockSYEngine::QueueEpectedCallL(TInt aCmdId, TInt aErrorCode, TBool aLeave, HBufC8* aData)
	{
	DoQueueEventL(TMockSYEvent::EMessage, aCmdId, aData, aErrorCode, aLeave, 0);
	}

inline void CMockSYEngine::QueueReturnCallL(TInt aCmdId, TInt aErrorCode, TInt aDelay, HBufC8* aData)
	{
	DoQueueEventL(TMockSYEvent::ECompletion, aCmdId, aData, aErrorCode, EFalse, aDelay);
	}

inline TBool CMockSYEngine::HasWaitingEvents() const
	{
	return !iWaitingEventQue.IsEmpty();
	}
	
inline TBool CMockSYEngine::HasPendingEvents() const
	{
	return !iPendingEventQue.IsEmpty();
	}

inline TBool CMockSYEngine::HasFailure() const
	{
	return iFailure;
	}

/**
Called when the session using this engine is closed
*/
inline void CMockSYEngine::SessionClosed()
	{
	iFailure = EFalse;
	Reset();
	}

#endif
