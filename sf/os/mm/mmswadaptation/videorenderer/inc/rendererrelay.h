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

#ifndef RENDERERRELAY_H
#define RENDERERRELAY_H

#include <e32base.h>
#include <e32msgqueue.h>
#include <graphics/surfaceupdateclient.h>
#include <mmf/devvideo/devvideobase.h>
#include "videorenderer.h"

class CBufAvailListener;
class CBufDisplayListener;
class CRendererTimer;

/** Interface for renderer relay object */
class MVideoRendererRelay
	{
public:
	virtual ~MVideoRendererRelay() {};
	virtual void UpdateBuffer(TVideoFrameBuffer* aBuffer, const TTime& aPresentationTime, TRequestStatus* aRequestStatus) = 0;
	virtual void DestroySurface(TRequestStatus* aRequestStatus) = 0;
	virtual void SetBufferManager(CRendererBufferManager* aBufManager) = 0;
	virtual void PrepareL(const TSurfaceId& aSurfaceId, TInt aNumBuffers, TRequestStatus* aRequestStatus) = 0;
	virtual void SetRendererThread(RThread* aRendererThread) = 0;
	virtual void Terminate(TRequestStatus& aRequestStatus) = 0;
	virtual void SetTimerInfo(TInt64 aDefaultDelay, TInt64 aMaxDelay) = 0;
	};

/** 
Implemenation of MVideoRendererRelay. This class is responsible for submitting 
update for CVideoRenderer in non-timed mode. In timed mode, this class submits 
update for CRendererThreadRelay.
*/
NONSHARABLE_CLASS(CRendererRelay) : public CBase, public MVideoRendererRelay
	{
public:
	static CRendererRelay* NewL(MVideoRendererObserver& aObserver);
	~CRendererRelay();

	// from MRendererRelay
	void UpdateBuffer(TVideoFrameBuffer* aBuffer, const TTime& aPresentationTime, TRequestStatus* aRequestStatus);
	void DestroySurface(TRequestStatus* aRequestStatus);
	void SetBufferManager(CRendererBufferManager* aBufManager);
	void PrepareL(const TSurfaceId& aSurfaceId, TInt aNumBuffers, TRequestStatus* aRequestStatus);
	void SetRendererThread(RThread* aRendererThread);
	void Terminate(TRequestStatus& aRequestStatus);
	void SetTimerInfo(TInt64 aDefaultDelay, TInt64 aMaxDelay);

	void BufferDisplayed(TBool aDisplayed, TInt64 aDelay);
	TBool UpdateSubmitted();

	void SetRendererTimer(CRendererTimer* aRendererTimer);
	void RendererTimerExpired();
	void SubmitBufferTimed();
	TInt64 Delay();

private:
	CRendererRelay(MVideoRendererObserver& aObserver);
	void ConstructL();

	CBufAvailListener* BufAvailListener();
	void SubmitBuffer();
	void DoUpdateBuffer(TInt aBufferId, const TTime& aTime, TUint32 aFastCounter);

private:
	MVideoRendererObserver& iObserver;
	RSurfaceUpdateSession iSurfaceUpdateSession;
	CRendererBufferManager* iBufManager; // not owned
	CBufDisplayListener* iDisplayListener;
	TDblQue<CBufAvailListener> iBufAvailListeners;
	TDblQueIter<CBufAvailListener> iAvailListenersIter;
	TSurfaceId iSurfaceId;
	TBool iUpdateSubmitted;	// true if an update has been submitted
	TInt iFastCounterFrequency;
	TBool iFastCounterCountsUp;

	CRendererTimer* iRendererTimer; // not owned, null in non-timed mode
	TInt64 iDelay; // delay in microseconds
	TInt64 iMaxDelay;
	};
	
/** 
Listener in main thread for observer callback so that callback is done in main thread.

This class is needed so that callback to observer is done in the main thread 
instead of the renderer thread in timed mode.
*/
NONSHARABLE_CLASS(CRendererCallbackListener) : public CActive
	{
public:
	enum TFunctionCode
		{
		EBufferAvailable,
		EBufferDisplayed,
		EBufferSkipped
		};

	static CRendererCallbackListener* NewL(MVideoRendererObserver& aObserver, TInt aNumBuffer);
	~CRendererCallbackListener();
	void Start();
	void SendCallback(TFunctionCode aFunctionCode, TInt aBufferId, const TTime& aTime);
	void ExtendMsgQueueL(TInt aNumBuffer);

private:
	CRendererCallbackListener(MVideoRendererObserver& aObserver);
	void ConstructL(TInt aNumBuffer);
	
	// From CActive
	void RunL();
	void DoCancel();

private:

	class TCallbackData
		{
	public:
		TFunctionCode iFunctionCode;
		TInt iBufferId;
		TTime iTime;
		};

	MVideoRendererObserver& iObserver;
	RMsgQueue<TCallbackData> iMsgQueue;
	};
	
/** 
Implemenation of MVideoRendererRelay. This class is responsible taking submit
update request to the renderer thread in timed mode.
*/
NONSHARABLE_CLASS(CRendererThreadRelay) : public CActive, public MVideoRendererRelay, public MVideoRendererObserver
	{
public:
	~CRendererThreadRelay();
	static CRendererThreadRelay* NewL(MVideoRendererObserver& aObserver, TThreadId aMainThreadId);
	
	void Start();
	void SignalSetupComplete(TRequestStatus* aSetupComplete);
	TRequestStatus* Status();

	// from MRendererRelay
	void UpdateBuffer(TVideoFrameBuffer* aBuffer, const TTime& aPresentationTime, TRequestStatus* aRequestStatus);
	void DestroySurface(TRequestStatus* aRequestStatus);
	void SetBufferManager(CRendererBufferManager* aBufManager);
	void PrepareL(const TSurfaceId& aSurfaceId, TInt aNumBuffers, TRequestStatus* aRequestStatus);
	void SetRendererThread(RThread* aRendererThread);
	void Terminate(TRequestStatus& aRequestStatus);
	void SetTimerInfo(TInt64 aDefaultDelay, TInt64 aMaxDelay);
	
	// from MVideoRendererObserver
	void MvroVideoBufferAvailable();
	void MvroBufferDisplayed(TInt aBufferId, const TTime& aTime);
	void MvroBufferSkipped(TInt aBufferId);

private:
	CRendererThreadRelay(MVideoRendererObserver& aObserver);
	void ConstructL(TThreadId aMainThreadId);
	void RunL();
	void DoCancel();

	void RunUpdateBuffer(TVideoFrameBuffer* aBuffer, const TTime& aPresentationTime);
	void RunDestroySurface();
	void RunPrepareL();
	void RunSetBufferManager();
	
private:
	enum TFunctionCode
		{
		ETermination,
		ESubmitUpdate,
		EDestroySurface,
		EPrepare,
		ESetBufferManager
		};
	
	MVideoRendererObserver& iObserver;
	RThread iMainThread;
	CRendererRelay* iRendererRelay;
	CRendererTimer* iRendererTimer;
	CRendererCallbackListener* iRendererCallbackListener;
	RThread* iRendererThread; //Not owned
	CRendererBufferManager* iBufManager; // not owned
	
	TRequestStatus* iCallingStatus; // not owned
	TFunctionCode iFunctionCode;
	TVideoFrameBuffer* iBuffer; // parameter for UpdateBuffer
	TTime iPresentationTime; // parameter for UpdateBuffer
	TSurfaceId iSurfaceId; // parameter for PrepareL
	TInt iNumBuffers; // parameter for PrepareL
	TInt64 iMaxDelay;
	};

/** Internal class for passing parameter to renderer thread */
class TThreadRelayParam
	{
public:
	MVideoRendererObserver* iObserver; //Not owned
	MVideoRendererRelay** iThreadRelay; //Not owned
	TThreadId iMainThreadId;
	TRequestStatus* iSetupComplete; //Not owned
	};	

#endif // RENDERERRELAY_H
