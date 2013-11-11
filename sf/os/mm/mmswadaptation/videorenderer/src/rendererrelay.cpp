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

/**
 @file
 @internalComponent
*/

#include <hal.h>
#include <graphics/suerror.h>
#include "rendererrelay.h"
#include "buflistener.h"
#include "buffermanager.h"
#include "videoframebuffer.h"
#include "rendererutil.h"
#include "renderertimer.h"

/** static factory contruction */
CRendererRelay* CRendererRelay::NewL(MVideoRendererObserver& aObserver)
	{
	CRendererRelay* self = new (ELeave) CRendererRelay(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRendererRelay::CRendererRelay(MVideoRendererObserver& aObserver)
:iObserver(aObserver), 
 iBufAvailListeners(CBufAvailListener::iOffset), 
 iAvailListenersIter(iBufAvailListeners)
	{
	}

void CRendererRelay::ConstructL()
	{
	User::LeaveIfError(iSurfaceUpdateSession.Connect());
	User::LeaveIfError(HAL::Get(HAL::EFastCounterFrequency, iFastCounterFrequency));
	User::LeaveIfError(HAL::Get(HAL::EFastCounterCountsUp, iFastCounterCountsUp));
	}

/** Destructor */
CRendererRelay::~CRendererRelay()
	{
	delete iDisplayListener;
	
	iAvailListenersIter.SetToFirst();
	CBufAvailListener* listener = NULL;
	while ((listener = iAvailListenersIter++) != NULL)
		{
		listener->iDblQueLink.Deque();
		delete listener;
		}
	
	iSurfaceUpdateSession.Close();
	}

/** Update buffer manager pointer in this class and listeners */
void CRendererRelay::SetBufferManager(CRendererBufferManager* aBufManager)
	{
	iBufManager = aBufManager;
	
	// change buffer manager pointer for listeners
	iAvailListenersIter.SetToFirst();
	CBufAvailListener* listener = NULL;
	while ((listener = iAvailListenersIter++) != NULL)
		{
		listener->SetBufferManager(aBufManager);
		
		if (!aBufManager && listener->IsAdded())
			{
			listener->Deque();
			}
		else if (aBufManager && !listener->IsAdded())
			{
			CActiveScheduler::Add(listener);
			}
		}
	}

/** Return the next unused CBufAvailListener. */
CBufAvailListener* CRendererRelay::BufAvailListener()
	{
	CBufAvailListener* listener = NULL;
	iAvailListenersIter.SetToFirst();
	while ((listener = iAvailListenersIter++) != NULL)
		{
		if (listener->IsActive() == EFalse)
			{
			// Move to end so that the next search is a bit faster
			listener->iDblQueLink.Deque();
			iBufAvailListeners.AddLast(*listener);
			return listener;
			}
		}

	// Should not reach here as the number of listeners is same as number of buffer
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CRR::BufAvailListener"), KErrUnknown));
	return listener;
	}

/** Set update parameter and create listeners */
void CRendererRelay::PrepareL(const TSurfaceId& aSurfaceId, TInt aNumBuffers, TRequestStatus* /*aRequestStatus*/)
	{
	iUpdateSubmitted = EFalse;
	iSurfaceId = aSurfaceId;
	
	if (!iDisplayListener)
		{
		iDisplayListener = CBufDisplayListener::NewL(iObserver, iSurfaceUpdateSession, *this, iFastCounterFrequency, iFastCounterCountsUp);
		}

	TInt numListeners = 0;
	iAvailListenersIter.SetToFirst();
	while (iAvailListenersIter++ != NULL)
		{
		numListeners++;
		}
	
	CBufAvailListener* listener = NULL;
	// create new listeners if there are more numBuffers than listeners
	for (TInt i = numListeners; i < aNumBuffers; ++i)
		{
		listener = CBufAvailListener::NewL(iObserver, iSurfaceUpdateSession);
		iBufAvailListeners.AddFirst(*listener);
		}
	}

/** Handle update buffer request in non-timed mode */
void CRendererRelay::UpdateBuffer(TVideoFrameBuffer* aBuffer, const TTime& aPresentationTime, TRequestStatus* /* aRequestStatus */)
	{
	__ASSERT_DEBUG(iBufManager != NULL, User::Panic(_L("CRR::UpdateBuffer"), KErrNotReady));
	
	iBufManager->UpdateBuffer(aBuffer, aPresentationTime);
	
	if (iUpdateSubmitted == EFalse)
		{
		// an update haven't been submitted, submit one now
		SubmitBuffer();
		}
	}

/** Callback from display listener that a buffer has been displayed or skipped*/
void CRendererRelay::BufferDisplayed(TBool aDisplayed, TInt64 aDelay)
	{
	DEBUGPRINT3(_L("CRendererRelay::BufferDisplayed entered aDisplayed=%d, aDelay=%Ld"), aDisplayed, aDelay);

	iUpdateSubmitted = EFalse;
	
	if (iRendererTimer)
		{
		// rendering in timed mode
		if (aDisplayed)
			{
			// update delay value
			iDelay = aDelay;
			}
		SubmitBufferTimed();
		}
	else
		{
		// rendering in non timed mode
		SubmitBuffer();
		}
	}

/** Submit the next buffer that is waiting to be submitted in non-timed mode*/
void CRendererRelay::SubmitBuffer()
	{
	DEBUGPRINT1(_L("CRendererRelay::SubmitBuffer entered"));
	__ASSERT_DEBUG(iBufManager != NULL, User::Panic(_L("CRR::SubmitBuffer"), KErrCorrupt));
	__ASSERT_DEBUG(iUpdateSubmitted == EFalse, User::Panic(_L("CRR::SubmitBuffer"), KErrGeneral));

	TBool lastBuf;
	TVideoFrameBuffer* buf = iBufManager->WaitingBuffer(ETrue, lastBuf);
	TInt numBufferSkipped = 0;
	
	TTime now;
	now.UniversalTime();
	TUint32 fastCounter = User::FastCounter();
	while (buf != NULL)
		{
		TInt bufId = buf->BufferId();
		
		DEBUGPRINT4(_L("bufId=%d presTime=%Ld, now=%Ld"), 
					bufId, buf->PresentationTime().Int64(), now.Int64());

		// submit the buffer for update if presntation time is not in past 
		// or if the buffer is the last in queue or presentation time is zero
		if (buf->PresentationTime() >= now || lastBuf || buf->PresentationTime().Int64() == 0)
			{
			DoUpdateBuffer(bufId, now, fastCounter);
			break;
			}
		
		// The buffer presentation time occurs in past if codeflow reaches here.
		// Change the buffer status to available and notify observer about the skipped buffer
		iBufManager->BufferAvailable(bufId);
		iObserver.MvroBufferSkipped(bufId);
		numBufferSkipped++;

		// get next buffer
		buf = iBufManager->WaitingBuffer(ETrue, lastBuf);
		}
	
	//notifiy observer about the available buffers
	for (TInt i = 0; i < numBufferSkipped; ++i)
		{
		iObserver.MvroVideoBufferAvailable();
		}
	}

/** Set a pointer to renderer timer in timed mode */
void CRendererRelay::SetRendererTimer(CRendererTimer* aRendererTimer)
	{
	iRendererTimer = aRendererTimer;
	}

/** Callback function when a renderer timer has expired */
void CRendererRelay::RendererTimerExpired()
	{
	SubmitBufferTimed();
	}

/** Submit the next buffer in timed mode */
void CRendererRelay::SubmitBufferTimed()
	{
	DEBUGPRINT1(_L("CRendererRelay::SubmitBufferTimed entered"));

	__ASSERT_DEBUG(iBufManager != NULL, User::Panic(_L("CRR::SubmitBufferTimed"), KErrCorrupt));
	__ASSERT_DEBUG(iUpdateSubmitted == EFalse, 
			User::Panic(_L("CRR::SubmitBufferTimed"), KErrGeneral));
	__ASSERT_DEBUG(iRendererTimer && iRendererTimer->IsActive() == EFalse, 
					User::Panic(_L("CRR::SubmitBufferTimed"), KErrInUse));
	
	TBool lastBuf;
	TVideoFrameBuffer* buf = iBufManager->WaitingBuffer(EFalse, lastBuf);
	TInt numBufferSkipped = 0;
	
	TTime now;
	now.UniversalTime();
	TUint32 fastCounter = User::FastCounter();
	while (buf != NULL)
		{
		TInt bufId = buf->BufferId();
		TTimeIntervalMicroSeconds deltaFromNow = buf->PresentationTime().MicroSecondsFrom(now);
		
		TInt64 waitTime = 0;
		if (buf->PresentationTime().Int64() != 0)
			{
			// presentation time is not zero, calculate wait time. Otherwise, wait time is zero.
			waitTime = deltaFromNow.Int64() - iDelay;
			}
		
		DEBUGPRINT4(_L("bufId=%d presTime=%Ld, now=%Ld"), 
				bufId, buf->PresentationTime().Int64(), now.Int64());

		// submit the buffer for update if presntation time is not in past 
		// or if the buffer is the last in queue
		if (waitTime > 0)
			{
			iRendererTimer->Start(waitTime);
			break;
			}
		else if (buf->PresentationTime().Int64() == 0 ||
					deltaFromNow.Int64() + iMaxDelay >= 0 || 
					lastBuf)
			{
			// if presentation time is zero (waitTime is not used for checking because it may be zero from calculation)
			// or the frame is within maximun allowed delay, i.e. (presentation time + max delay >= now)
			// or submission time has passed but this is the last buf, submit the buffer now

			iBufManager->BufferSubmitted(buf);
			DoUpdateBuffer(bufId, now, fastCounter);
			break;
			}
		
		// The buffer presentation time has passed maxDelay if codeflow reaches here, skip the buffer.
		// Change the buffer status to available and notify observer
		iBufManager->BufferAvailable(bufId);
		iObserver.MvroBufferSkipped(bufId);
		numBufferSkipped++;

		// get next buffer
		buf = iBufManager->WaitingBuffer(EFalse, lastBuf);
		}
	
	//notifiy observer about the available buffers
	for (TInt i = 0; i < numBufferSkipped; ++i)
		{
		iObserver.MvroVideoBufferAvailable();
		}
	}

/**
Submits a buffer to be updated at the specified time.
*/

void CRendererRelay::DoUpdateBuffer(TInt aBufferId, const TTime& aTime, TUint32 aFastCounter)
	{
	CBufAvailListener* availListener = BufAvailListener();
			
	availListener->Start(aBufferId);
	iDisplayListener->Start(aBufferId, aTime, aFastCounter);
	
	TInt err = iSurfaceUpdateSession.SubmitUpdate(KAllScreens, iSurfaceId, aBufferId);

	DEBUGPRINT2(_L("SubmitUpdate return %d"), err);
			
	// error will also be returned from listener, so the next submit updated will be triggered by display listener
	iUpdateSubmitted = ETrue;
	}

/** 
Return ETrue if an update has been submitted and the display notification 
haven't been received yet. i.e. Need to wait till for listener callback before 
the next buffer can be submitted.
*/
TBool CRendererRelay::UpdateSubmitted()
	{
	return iUpdateSubmitted;
	}

/** return the delay for surface update */
TInt64 CRendererRelay::Delay()
	{
	return iDelay;
	}

/** Cancel all update notification when a surface is destroyed */
void CRendererRelay::DestroySurface(TRequestStatus* /* aRequestStatus */ )
	{
	iSurfaceUpdateSession.CancelAllUpdateNotifications();
	}

/* This function is not used */
void CRendererRelay::SetRendererThread(RThread* /* aRendererThread */)
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CRR::SetRendererThread"), KErrUnknown));
	}

/* This function is not used */
void CRendererRelay::Terminate(TRequestStatus& /* aRequestStatus */)
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CRR::Terminate"), KErrUnknown));
	}

/** Set timer info for timed mode, this function is not called in non-timed mode */
void CRendererRelay::SetTimerInfo(TInt64 aDefaultDelay, TInt64 aMaxDelay)
	{
	iDelay = aDefaultDelay;
	iMaxDelay = aMaxDelay;
	}

/** Two-phased constructor. */
CRendererThreadRelay* CRendererThreadRelay::NewL(MVideoRendererObserver& aObserver, TThreadId aMainThreadId)
	{
	CRendererThreadRelay* self = new (ELeave) CRendererThreadRelay(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aMainThreadId);
	CleanupStack::Pop(self);
	return self;
	}

CRendererThreadRelay::CRendererThreadRelay(MVideoRendererObserver& aObserver) 
: CActive(EPriorityStandard), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

/** Second-phase constructor */
void CRendererThreadRelay::ConstructL(TThreadId aMainThreadId)
	{
	User::LeaveIfError(iMainThread.Open(aMainThreadId));
	iRendererRelay = CRendererRelay::NewL(*this);
	iRendererTimer = CRendererTimer::NewL(*iRendererRelay);
	iRendererRelay->SetRendererTimer(iRendererTimer);
	}

/** 
Destructor. 
This active object is always cancelled by main thread so no need to cancel this active object here
*/
CRendererThreadRelay::~CRendererThreadRelay()
	{
	delete iRendererCallbackListener;
	delete iRendererTimer;
	delete iRendererRelay;
	iMainThread.Close();
	}

void CRendererThreadRelay::DoCancel()
	{
	// Don't need to do anything, will be stopped by main thread
	}

/** Function for making the initial request */
void CRendererThreadRelay::Start()
	{
	__ASSERT_DEBUG(IsActive() == EFalse, User::Panic(_L("CRTR::Start"), KErrInUse));
	iStatus = KRequestPending;
	SetActive();
	}

/** Handle requests from main thread */
void CRendererThreadRelay::RunL()
	{
	__ASSERT_DEBUG(iStatus == KErrNone, User::Panic(_L("CRTR::RunL"), KErrUnknown));
	TInt result = KErrNone;

	if (iFunctionCode == ESubmitUpdate)
		{
		Start();
		RunUpdateBuffer(iBuffer, iPresentationTime);
		}
	else if (iFunctionCode == EDestroySurface)
		{
		Start();
		RunDestroySurface();
		}
	else if (iFunctionCode == EPrepare)
		{
		Start();
		TRAP(result, RunPrepareL());
		}
	else if (iFunctionCode == ESetBufferManager)
		{
		Start();
		RunSetBufferManager();
		}
	else // iFunctionCode == ETermination
		{
		CActiveScheduler::Stop();
		}

	TRequestStatus *status = iCallingStatus;
	iMainThread.RequestComplete(status, result);
	}

/** Send a signal to the main thread to indicate that the thread startup was successful. */
void CRendererThreadRelay::SignalSetupComplete(TRequestStatus* aSetupComplete)
	{
	iMainThread.RequestComplete(aSetupComplete, KErrNone);
	}

/** Send update buffer request from main thread to renderer thread */
void CRendererThreadRelay::UpdateBuffer(TVideoFrameBuffer* aBuffer, const TTime& aPresentationTime, TRequestStatus* aRequestStatus)
	{
	__ASSERT_DEBUG(aRequestStatus != NULL, User::Panic(_L("CRTR::UpdateBuffer"), KErrArgument));

	// set function parameters
	iCallingStatus = aRequestStatus;
	iFunctionCode = ESubmitUpdate;
	iBuffer = aBuffer;
	iPresentationTime = aPresentationTime;
	
	// send request to renderer thread
	TRequestStatus* rendererRequest = Status();
	iRendererThread->RequestComplete(rendererRequest, KErrNone);
	}

/** Send terminate renderer thread request from main thread to renderer thread */
void CRendererThreadRelay::Terminate(TRequestStatus& aRequestStatus)
	{
	iCallingStatus = &aRequestStatus;
	iFunctionCode = ETermination;
	
	if (iRendererCallbackListener)
		{
		iRendererCallbackListener->Cancel();
		}
	
	// send request to renderer thread
	TRequestStatus* rendererRequest = Status();
	iRendererThread->RequestComplete(rendererRequest, KErrNone);
	}

/** Send destroy surface request from main thread to renderer thread */
void CRendererThreadRelay::DestroySurface(TRequestStatus* aRequestStatus)
	{
	__ASSERT_DEBUG(aRequestStatus != NULL, User::Panic(_L("CRTR::DestroySurface"), KErrArgument));
	
	iCallingStatus = aRequestStatus;
	iFunctionCode = EDestroySurface;
	
	// send request to renderer thread
	TRequestStatus* rendererRequest = Status();
	iRendererThread->RequestComplete(rendererRequest, KErrNone);
	}

/* Prepare the object after a surface is created */
void CRendererThreadRelay::PrepareL(const TSurfaceId& aSurfaceId, TInt aNumBuffers, TRequestStatus* aRequestStatus)
	{
	__ASSERT_DEBUG(aRequestStatus != NULL, User::Panic(_L("CRTR::PrepareL"), KErrArgument));
	
	if(!iRendererCallbackListener)
		{
		// first, create callback listener in the main thread
		iRendererCallbackListener = CRendererCallbackListener::NewL(iObserver, aNumBuffers);
		iRendererCallbackListener->Start();		
		}
	else if (iNumBuffers < aNumBuffers)
		{
		iRendererCallbackListener->Cancel();
		iRendererCallbackListener->ExtendMsgQueueL(aNumBuffers);
		iRendererCallbackListener->Start();		
		}

	// set function parameters
	iCallingStatus = aRequestStatus;
	iFunctionCode = EPrepare;
	iSurfaceId = aSurfaceId;
	iNumBuffers = aNumBuffers;
	
	// send request to renderer thread
	TRequestStatus* rendererRequest = Status();
	iRendererThread->RequestComplete(rendererRequest, KErrNone);
	}

/* Prepare the object after a surface is created */
void CRendererThreadRelay::RunPrepareL()
	{
	iRendererRelay->PrepareL(iSurfaceId, iNumBuffers, NULL);
	}

/** Run update buffer in renderer thread */
void CRendererThreadRelay::RunUpdateBuffer(TVideoFrameBuffer* aBuffer, const TTime& aPresentationTime)
	{
	DEBUGPRINT1(_L("CRendererThreadRelay::RunUpdateBuffer entered"));
	__ASSERT_DEBUG(iBufManager != NULL, User::Panic(_L("CRTR::RunUpdateBuffer"), KErrCorrupt));

	/*
	Buffer update is determined as follow:
	 
	If wait list is empty (imply no active timer), always add buffer to list, 
		If a preceding buffer hasn't been displayed, the new buffer will be handled after display callback.
			otherwise, decide whether timer should be started for submit update
	If wait list is not empty (imply either timer is active or waiting for display callback)
		If waiting for display callback, add new buffer to list and it will be handled after display callback.
			(note: presentation time is not check because the new buffer may be newer than the waiting buffers even though both have passed due time)
		If timer is active, first check if presentation time is zero. If so, display right away
			otherwise, then check if this frame can be timed (presentation time - delay <= now), if not, check max display to skip the frame or display right away
			otherwise, then check if presentation time < head list presentation time
				if so, add the buffer to wait list, stop the timer and start timer with new time
				else, just need to add buffer to wait list, the next timer expiry will hander the head buffer
	*/
	
	if (iBufManager->WaitingListIsEmpty())
		{
		iBufManager->UpdateBuffer(aBuffer, aPresentationTime);

		if (iRendererRelay->UpdateSubmitted() == EFalse)
			{
			// an update haven't been submitted, prepare to submit one now
			iRendererRelay->SubmitBufferTimed();
			}
		}
	else
		{
		// waiting list is not empty
		if (iRendererRelay->UpdateSubmitted())
			{
			// waiting for listener callback, just update waiting list
			iBufManager->UpdateBuffer(aBuffer, aPresentationTime);
			}
		else
			{
			// the timer is waiting

			if (aPresentationTime.Int64() == 0)
				{
				// presentation time is zero, display right away.
				iRendererTimer->Cancel();
				iBufManager->UpdateBuffer(aBuffer, aPresentationTime);
				iRendererRelay->SubmitBufferTimed();
				return;
				}

			TTime now;
			now.UniversalTime();
			TTimeIntervalMicroSeconds deltaFromNow = aPresentationTime.MicroSecondsFrom(now);
			TInt64 waitTime = deltaFromNow.Int64() - iRendererRelay->Delay();
				
			if (waitTime <= 0)
				{
				// the wait time has passed
				if (deltaFromNow.Int64() + iMaxDelay >= 0)
					{
					// the frame is within maximun allowed delay, i.e. (presentation time + max delay >= now)
					// display right away
					iRendererTimer->Cancel();
					iBufManager->UpdateBuffer(aBuffer, aPresentationTime);
					iRendererRelay->SubmitBufferTimed();
					}
				else
					{
					// skip the buffer
					iBufManager->ReleaseBuffer(aBuffer);
					MvroBufferSkipped(aBuffer->BufferId());
	
					//notifiy observer about the available buffers
					MvroVideoBufferAvailable();
					}
				}
			else
				{
				// wait time has not passed, add to waiting list
				if (iBufManager->UpdateBuffer(aBuffer, aPresentationTime))
					{
					// head of waiting list has changed, start timer with new wait time
					iRendererTimer->Cancel();
					iRendererTimer->Start(waitTime);
					}
				}
			}
		}
	}

/** Run destroy surface in renderer thread */
void CRendererThreadRelay::RunDestroySurface()
	{
	iRendererTimer->Cancel();
	iRendererRelay->DestroySurface(NULL);
	}

/* Update buffer manager pointer */
void CRendererThreadRelay::SetBufferManager(CRendererBufferManager* aBufManager)
	{
		TRequestStatus request = KRequestPending;
		TRequestStatus logonRequest = KRequestPending;

		// While a function call is in progress this thread is suspended
		// and the undertaker will not catch panics, listen for these here
		iRendererThread->Logon(logonRequest);

		// Set the call parameters
		iCallingStatus = &request;
		iFunctionCode = ESetBufferManager;
		iBufManager = aBufManager;
	
		// send request to renderer thread
		TRequestStatus* rendererRequest = Status();
		iRendererThread->RequestComplete(rendererRequest, KErrNone);
		
		User::WaitForRequest(logonRequest, request);

		if(logonRequest != KRequestPending)
			{
			// renderer thread got panic from surface update session, so panic client
			TInt reason = iRendererThread->ExitReason();
			TExitCategoryName category = iRendererThread->ExitCategory();
			User::Panic(category,reason);
			}

		// Thread is still alive and well
		iRendererThread->LogonCancel(logonRequest);
		User::WaitForRequest(logonRequest); // Consume the signal

		__ASSERT_DEBUG(request != KRequestPending, User::Panic(_L("CRTR::SetBufferManager"), KErrCorrupt));
	}

void CRendererThreadRelay::RunSetBufferManager()
	{
	iRendererRelay->SetBufferManager(iBufManager);
	}

/** Store a pointer to the renderer thread object. */
void CRendererThreadRelay::SetRendererThread(RThread* aRendererThread)
	{
	iRendererThread = aRendererThread;
	}

/** Return a pointer to the function call listener's request status. */
TRequestStatus* CRendererThreadRelay::Status()
	{
	return &iStatus;
	}

/** Set timer info for timed mode */
void CRendererThreadRelay::SetTimerInfo(TInt64 aDefaultDelay, TInt64 aMaxDelay)
	{
	iMaxDelay = aMaxDelay;
	iRendererRelay->SetTimerInfo(aDefaultDelay, aMaxDelay);
	}

void CRendererThreadRelay::MvroVideoBufferAvailable()
	{
	iRendererCallbackListener->SendCallback(CRendererCallbackListener::EBufferAvailable, -1, 0);
	}

void CRendererThreadRelay::MvroBufferDisplayed(TInt aBufferId, const TTime& aTime)
	{
	iRendererCallbackListener->SendCallback(CRendererCallbackListener::EBufferDisplayed, aBufferId, aTime);
	}

void CRendererThreadRelay::MvroBufferSkipped(TInt aBufferId)
	{
	iRendererCallbackListener->SendCallback(CRendererCallbackListener::EBufferSkipped, aBufferId, 0);
	}


/** Private constructor */
CRendererCallbackListener::CRendererCallbackListener(MVideoRendererObserver& aObserver)
: CActive(EPriorityStandard),
  iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

/** Two-phased constructor */
CRendererCallbackListener* CRendererCallbackListener::NewL(MVideoRendererObserver& aObserver, TInt aNumBuffer)
	{
	CRendererCallbackListener* self = new (ELeave) CRendererCallbackListener(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aNumBuffer);
	CleanupStack::Pop(self);
	return self;
	}

/** Second-phase constructor */
void CRendererCallbackListener::ConstructL(TInt aNumBuffer)
	{
	// There could potentially be three messages outstanding per buffer
	// size message queue accordingly
	TInt slot = aNumBuffer * 3;
	if (slot > RMsgQueueBase::KMaxLength)
		{
		slot = RMsgQueueBase::KMaxLength;
		}
	User::LeaveIfError(iMsgQueue.CreateLocal(slot));
	}

CRendererCallbackListener::~CRendererCallbackListener()
	{
	Cancel(); // Cancel any request, if outstanding
	iMsgQueue.Close();
	}

void CRendererCallbackListener::ExtendMsgQueueL(TInt aNumBuffer)
	{
	// close the message queue and construct another one
	iMsgQueue.Close();
	ConstructL(aNumBuffer);
	}

void CRendererCallbackListener::DoCancel()
	{
	iMsgQueue.CancelDataAvailable();
	}

/** Start listener */
void CRendererCallbackListener::Start()
	{
	if (!IsActive())
		{
		iMsgQueue.NotifyDataAvailable(iStatus);
		SetActive();
		}
	}

/** Set the callback function */
void CRendererCallbackListener::SendCallback(TFunctionCode aFunctionCode, TInt aBufferId, const TTime& aTime)
	{
	DEBUGPRINT2(_L("CRendererCallbackListener::SendCallback entered aFunctionCode=%d"), aFunctionCode);
	
	TCallbackData data;
	data.iFunctionCode = aFunctionCode;
	data.iBufferId = aBufferId;
	data.iTime = aTime;
	
	iMsgQueue.Send(data);
	}

/** Call the callback function within the main thread */
void CRendererCallbackListener::RunL()
	{
	TCallbackData data;
	TInt err = iMsgQueue.Receive(data);
	if (err == KErrNone)
		{
		if (data.iFunctionCode == EBufferAvailable)
			{
			DEBUGPRINT1(_L("CRendererCallbackListener::RunL - EBufferAvailable"));
			iObserver.MvroVideoBufferAvailable();
			}
		else if (data.iFunctionCode == EBufferDisplayed)
			{
			DEBUGPRINT1(_L("CRendererCallbackListener::RunL - EBufferDisplayed"));
			iObserver.MvroBufferDisplayed(data.iBufferId, data.iTime);
			}
		else if (data.iFunctionCode == EBufferSkipped)
			{
			DEBUGPRINT1(_L("CRendererCallbackListener::RunL - EBufferSkipped"));
			iObserver.MvroBufferSkipped(data.iBufferId);
			}
		}
	else
		{
		DEBUGPRINT2(_L("CRendererCallbackListener::RunL err=%d"), err);
		}

	Start();
	}
