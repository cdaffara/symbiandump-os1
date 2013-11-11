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

#ifndef TESTRENDERER_H
#define TESTRENDERER_H

#include <testframework.h>
#include <videorenderer.h>

/* 
Basic test for video renderer in timed and non-timed mode 
MM-MMF-VIDREND-U-0002 and MM-MMF-VIDREND-U-0003
*/
class RTestRendererStep : public RTestStep, public MVideoRendererObserver
	{
public:

	enum TTestRendererEvents
		{
		EStartTest,
		EBufferAvailable,
		EBufferDisplayed,
		EBufferSkipped,
		ETimerExpired
		};

	enum TTestRendererState
		{
		EStateIdle,
		EStateCreate,
		EStateUpdate,
		EStateReleaseBuffer,
		EStateReplaceSurface,
		EStateUpdateAfterReplace
		};

	static RTestRendererStep* NewL(const TDesC& aStepName, TBool aTimed);

	// from MVideoRendererObserver
	void MvroVideoBufferAvailable();
	void MvroBufferDisplayed(TInt aBufferId, const TTime& aTime);
	void MvroBufferSkipped(TInt aBufferId);

protected:
	RTestRendererStep(const TDesC& aStepName, TBool aTimed);

	virtual void FsmL(TTestRendererEvents aEventCode);
    void InitWservL();
    void DeInitWserv();
    TInt SetBackgroundSurface(TSurfaceId& id);
    void RemoveBackgroundSurface();
    void EndTest(TVerdict aVerdict);
    void CreateRendererAndSurfaceL(TInt aNumBuffers);
    void CreateSurfaceL(TInt aNumBuffers);
    void GetNextBufferAndSubmitUpdateL(TInt aExpectedBufId, TRgb aColor, const TTime& aPresentationTime);
    TVideoFrameBuffer* GetNextBufferL(TInt aExpectedBufId, TRgb aColor);
    
	// from RTestStep
    TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    TVerdict DoTestStepL();

	
protected:
	CActiveScheduler* iActiveScheduler;
	TBool iActiveSchedulerStarted;
	CVideoRenderer* iVideoRenderer;
	TBool iTimed;
	TSurfaceId iSurfaceId;

    // Surface creation parameter
	TSize iSize;
	TUncompressedVideoFormat iVideoFormat;

    // Window server related objects
    RWsSession iWs;
    CWsScreenDevice* iScreen;
    RWindow* iWindow;
    RWindowGroup iRootWindow;

	TTestRendererState iFsmState;
	TInt iBuffAvailCallback;
	TInt iBuffDisplayCallback;
	TInt iBuffSkipCallback;
	
    // values stored for Mvro callbacks
	TInt iBufferId;
	TTime iDisplayedTime;
	};

/* 
Test replacing surface in timed and non-timed mode
MM-MMF-VIDREND-U-0010 and MM-MMF-VIDREND-U-0011
*/
class RTestRendererReplaceStep : public RTestRendererStep
	{
public:
	static RTestRendererReplaceStep* NewL(const TDesC& aStepName, TBool aTimed);

protected:
	RTestRendererReplaceStep(const TDesC& aStepName, TBool aTimed);
	void FsmL(TTestRendererEvents aEventCode);
	void ReplaceSurfaceL();
	};

/*
Timer callback class
*/
class CCallBackTimer : public CTimer
	{
public:
	static CCallBackTimer* NewL(TCallBack aCallBack, TPriority aPriority = EPriorityStandard);

private:
	void RunL();
	CCallBackTimer(TCallBack aCallBack, TPriority aPriority);

private:
	TCallBack iCallBack;
	};

#endif // TESTRENDERER_H
