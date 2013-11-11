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

#include "cmocksyengine.h"
#include "rotatingstrbuf.h"

#define KMaxLogEntries 30
#define KCallbackBaseTime (1*100*1000)	// 0.1S

/**
Constructor
*/
EXPORT_C CMockSYEngine::CMockSYEngine()
	:iWaitingEventQue(_FOFF(TMockSYEvent,iWaitingEventQueLink))
	,iPendingEventQue(_FOFF(TMockSYEvent,iPendingEventQueLink))
	{
	}

/**
2nd phase construction 
*/
EXPORT_C void CMockSYEngine::ConstructL()
	{
	iTimer = CPeriodic::NewL(EPriorityHigh);
	iRotatingLog = CRotatingStrBuf::NewL(KMaxLogEntries);
	iPaused = EFalse;
	}

/**
destructor
*/
EXPORT_C CMockSYEngine::~CMockSYEngine()
	{
	Reset();
	delete iTimer;
	delete iRotatingLog;
	iListeners.Close();
	}

/**
Add a new listener to the listener list
*/
void CMockSYEngine::AddListenerL(MMockSYEngineListener& aListener)
	{
	iListeners.AppendL(&aListener);
	}

void CMockSYEngine::RemoveListener(MMockSYEngineListener& aListener)
	{
	TInt idx=0;
	while (idx < iListeners.Count())
		{
		if (iListeners[idx]==&aListener)
			{
			iListeners.Remove(idx);
			break;
			}
		idx++;
		}
	}

void CMockSYEngine::NotifyListeners(MMockSYEngineListener::TNotificationType aNotification)
	{
	for (TInt idx=0; idx<iListeners.Count(); idx++)
		{
		iListeners[idx]->Notify(aNotification);
		}
	}

/**
Called by the SY, handle received command by verifing that is the expected command
and queuing the conresponding completions
Return KErrCorrupt it it's not the expected command
*/
EXPORT_C TInt CMockSYEngine::ExecuteCommandL(TInt aCmdId, MMockSyEngineData& aData)
	{
	return ExecuteCommandL(aCmdId, aData, ETrue);		
	}
	
EXPORT_C TInt CMockSYEngine::ExecuteCommandL(TInt aCmdId, MMockSyEngineData& aData, TBool aCheckData)
	{
	if (iWaitingEventQue.IsEmpty())
		{
		if(CanIgnoreUnexpectedIpc(aCmdId))
			{
			LogExpectError(aCmdId, aData, 0, KNullDesC8, ETrue); // ETrue means this error has been ignored
			return KErrNone;
			}
		LogExpectError(aCmdId,aData,0,KNullDesC8);
		Failure();
		return KErrCorrupt;
		}
	TMockSYEvent* event = iWaitingEventQue.First();
	// Top of the queue is supposed to be an EMessage event !
	ASSERT(event->iEventType == TMockSYEvent::EMessage);
	
	// validate expected call
	if ( (aCmdId!=  event->iCmdId) ||  (aCheckData ? !(aData == event->iData) : EFalse) )
		{
		if(CanIgnoreUnexpectedIpc(aCmdId))
			{
			LogExpectError(aCmdId, aData, event->iCmdId, KNullDesC8, ETrue); // ETrue means this error has been ignored
			return KErrNone;
			}
		// failure ! it's the wrong event
		LogExpectError(aCmdId,aData,event->iCmdId,event->iData);
		Failure();
		return KErrCorrupt;
		}
	else
		{		
		TInt resultCode = event->iResultCode;
		TBool leave = event->iLeave;
		LogRequest(aCmdId,aData,resultCode);
		// remove the event from the waiting queue
		event->iWaitingEventQueLink.Deque();
		event->iData.Close();
		delete event;
		
		// and push all following ECompletion events to the pending queue
		if (!iWaitingEventQue.IsEmpty())
			event = iWaitingEventQue.First();
		while (!iWaitingEventQue.IsEmpty() && (event->iEventType == TMockSYEvent::ECompletion))
			{
			event->iWaitingEventQueLink.Deque();
			iPendingEventQue.Add(*event,event->iDelay);
			if (!iWaitingEventQue.IsEmpty())
				event = iWaitingEventQue.First();
			}
		// start the timer
		if (!iPendingEventQue.IsEmpty() && !iTimer->IsActive())
			{
			iTimer->Start(KCallbackBaseTime,KCallbackBaseTime,TCallBack(CMockSYEngine::TimerCallbackL, this));
			}
		// notify when both queue are empty
		if (iPendingEventQue.IsEmpty() && iWaitingEventQue.IsEmpty())
			{
			NotifyListeners(MMockSYEngineListener::EHandlingTerminated);
			}
		if (leave)
			{
			User::Leave(resultCode);
			}
		return resultCode;
		}
	}


/**
Queue an event in the waiting event queue
*/
EXPORT_C void CMockSYEngine::DoQueueEventL(TMockSYEvent::TEventType aType, TInt aCmdId, HBufC8* aData, 
	TInt aResultCode, TBool aLeave, TInt aDelay)
	{
	TMockSYEvent* event = new (ELeave)TMockSYEvent;
	event->iEventType 	= aType;
	event->iCmdId 		= aCmdId;
	event->iResultCode 	= aResultCode;
	event->iDelay 		= aDelay;
	event->iLeave		= aLeave;
	event->iData.Assign(aData);
	if ((aType == TMockSYEvent::ECompletion) && iWaitingEventQue.IsEmpty())
		{
		// it's a ECompletion msg and the waiting queue is empty, put the event directly in pending queue
		iPendingEventQue.Add(*event,event->iDelay);
		if (!iTimer->IsActive())
			{
			iTimer->Start(KCallbackBaseTime,KCallbackBaseTime,TCallBack(CMockSYEngine::TimerCallbackL, this));
			}
		}
	else
		{
		iWaitingEventQue.AddLast(*event);
		}
	}

/**
	Returns EFalse indicating no IPC error can be ignored.
 */
EXPORT_C TBool CMockSYEngine::CanIgnoreUnexpectedIpc(TInt /*aCmdId*/)
	{
	return EFalse;
	}

/**
Return the next log line from the rotating log buffer
*/
HBufC* CMockSYEngine::GetNextLogLine()
	{
	return iRotatingLog->Get();
	}

/**
Periodical timer callback: generate completions
*/
void CMockSYEngine::DoTimerCallbackL()
	{
    if(iPaused)
        {
        return;
        }
	iPendingEventQue.CountDown();
	TMockSYEvent* event = iPendingEventQue.RemoveFirst();
	while(event != NULL )
		{
		LogCompletion(event->iCmdId,event->iData, event->iResultCode);
		this->DoCompleteEventL(*event);
		event->iData.Close();
		delete event;
		event = iPendingEventQue.RemoveFirst();
		}
	// stop the timer if there is no more pending messages
	if (iPendingEventQue.IsEmpty())
		{
		iTimer->Cancel();
		// notify when both queue are empty
		if (iWaitingEventQue.IsEmpty())
			{
			NotifyListeners(MMockSYEngineListener::EHandlingTerminated);
			}
		}
	}


/**
This function is called when an receive message doesn't correspond to the expected one
*/
EXPORT_C void CMockSYEngine::Failure()
	{
	iFailure = ETrue;
	NotifyListeners(MMockSYEngineListener::EFailure);
	Reset();
	}

/**
Reset waiting and pending event queue
*/
EXPORT_C void CMockSYEngine::Reset()
	{
    iPaused = EFalse;
	TMockSYEvent* event;
	while (!iWaitingEventQue.IsEmpty())
		{
		event = iWaitingEventQue.First();
		event->iWaitingEventQueLink.Deque();
		event->iData.Close();
		delete event;
		}
	while (!iPendingEventQue.IsEmpty())
		{
		iPendingEventQue.CountDown();
		event = iPendingEventQue.RemoveFirst();
		while(event != NULL )
			{
			event->iData.Close();
			delete event;
			event = iPendingEventQue.RemoveFirst();
			}
		}
	}

EXPORT_C void CMockSYEngine::LogRequest(TInt aCmdId, const MMockSyEngineData& /*aData*/,TInt aResultCode)
	{
	TBuf<KMaxLogLineSize> buffer;
	buffer.Format(_L(">>> Cmd=%d Err=%d"),aCmdId, aResultCode);
	Log(buffer);	
	}

EXPORT_C void CMockSYEngine::LogCompletion(TInt aCmdId, const TDesC8& /*aData*/,TInt aResultCode)
	{
	TBuf<KMaxLogLineSize> buffer;
	buffer.Format(_L("<<< Cmd=%d Err=%d"),aCmdId, aResultCode);
	Log(buffer);	
	}

EXPORT_C void CMockSYEngine::LogExpectError(TInt aCmdId, const MMockSyEngineData& /*aData*/,
	TInt aExpectedCmd,const TDesC8& /*aExpectedData*/, TBool /*aIsErrorIgnored*/)
	{
	TBuf<KMaxLogLineSize> buffer;
	buffer.Format(_L("ERROR: Expected Cmd=%d Received Cmd=%d"),aExpectedCmd, aCmdId);
	Log(buffer);	
	}

EXPORT_C void CMockSYEngine::Log(const TDesC& aDesc)
	{
	iRotatingLog->Put(aDesc);	
	}

TInt CMockSYEngine::TimerCallbackL(TAny* aPtr)
	{
	static_cast<CMockSYEngine*>(aPtr)->DoTimerCallbackL();
	return 0;
	}
EXPORT_C void CMockSYEngine::CheckAndUpdateTransId(TUint8 aTransId, TInt aCommandId)
    {
    TMockSYEvent* event = NULL;
    TDblQueIter<TMockSYEvent> iter(iWaitingEventQue);
    			
    while(iter)
        {
        event = (TMockSYEvent*)iter++;
        if((event->iCmdId == aCommandId) && (event->iData[0] == 0xFF) && (event->iEventType == TMockSYEvent::ECompletion))
            {
            event->iData[0] = aTransId;
            event->iData[1] = 1;
            break;
            }
        }
    }   

TInt CMockSYEngine::PauseCompletion()
    {
    TInt ret = (iPaused) ? KErrInUse : KErrNone;
    iPaused = ETrue;
    return ret;
    }
	
TInt CMockSYEngine::ResumeCompletion()
    {
    TInt ret = (!iPaused) ? KErrInUse : KErrNone;
    iPaused = EFalse;
    return ret;
    }
