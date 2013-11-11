// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TESTSTEPVIDPLAYEREVENTHANDLER_H
#define TESTSTEPVIDPLAYEREVENTHANDLER_H

#include <e32hashtab.h>
#include <surfaceeventhandler.h>
#include <testframework.h>
#include "TestStepUnitMMFVidClient.h"

NONSHARABLE_CLASS(CTestStepVidPlayerAddDisplay) :
	public CTestStepUnitMMFVidClient,
	public MVideoPlayerUtilityObserver,
	public MMMFSurfaceEventHandler
	{
public:
	static CTestStepVidPlayerAddDisplay* NewL(const TDesC& aTestStepName);
	~CTestStepVidPlayerAddDisplay();	
	
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPostambleL();

	// MMMFSurfaceEventHandler
	virtual void MmsehSurfaceCreated(TInt aDisplayId, const TSurfaceId& aId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	virtual void MmsehSurfaceParametersChanged(const TSurfaceId& aId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	virtual void MmsehRemoveSurface(const TSurfaceId& aId);

	// MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
	
	void AddWindowsL(RArray<TInt>& aDisplays);
	void AddDisplaysL(RArray<TInt>& aDisplays);
	void ParseDisplaysL(TDesC& aList, RArray<TInt>& aDisplays);
	void ParseEventsL(TDesC& aList, RArray<TPtrC>& aEvents);
	
	RWindow* WindowForDisplayL(TInt aDisplay);
	RWindowGroup* RootWindowForDisplayL(TInt aDisplay);
	CWsScreenDevice* ScreenForDisplayL(TInt aDisplay);
	
protected:
	CTestStepVidPlayerAddDisplay(const TDesC& aTestStepName);
	
	TInt SendCustomCommandAsync(TMMFMessageDestinationPckg aDestPckg, TInt aFunction);
	
protected:
	RHashMap<TInt, RWindow> iWindows;
	RHashMap<TInt, RWindowGroup> iRootWindows;
	RHashMap<TInt, CWsScreenDevice*> iScreens;
	RArray<TInt> iDisplayIds;
	};

NONSHARABLE_CLASS(CTestStepVidPlayerEvent) : 
	public CTestStepVidPlayerAddDisplay
	{
public:
	static CTestStepVidPlayerEvent* NewL(const TDesC& aTestStepName);
	~CTestStepVidPlayerEvent();

	virtual void MvpuoOpenComplete(TInt aError);

	// MMMFSurfaceEventHandler
	virtual void MmsehSurfaceCreated(TInt aDisplayId, const TSurfaceId& aId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	virtual void MmsehSurfaceParametersChanged(const TSurfaceId& aId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	virtual void MmsehRemoveSurface(const TSurfaceId& aId);

	virtual TVerdict DoTestStepL();

protected:
	void CheckAndIssueEvent(const TDesC& aEvent);
	TInt SimulateNextEvent();
	
private:
	CTestStepVidPlayerEvent(const TDesC& aTestStepName);

private:
	RArray<TPtrC> iSimulatedEvents;
	RArray<TPtrC> iExpectedEvents;
	TInt iCurrentSimulatedEvent;
	TInt iCurrentExpectedEvent;
	};
	
NONSHARABLE_CLASS(CTestStepVidPlayer1106) :
	public CTestStepVidPlayerAddDisplay
	{
public:
	static CTestStepVidPlayer1106* NewL();
	~CTestStepVidPlayer1106();
	
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MmsehSurfaceCreated(TInt aDisplayId, const TSurfaceId& aId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	
	virtual TVerdict DoTestStepL();
	
private:
	CTestStepVidPlayer1106();
	void OnTimer();
private:
	class CTestTimer : public CTimer
		{
		public:
			static CTestTimer* NewL(CTestStepVidPlayer1106& aTestStepVidPlayer);
			CTestTimer(CTestStepVidPlayer1106& aTestStepVidPlayer):CTimer(EPriorityStandard),iContainer(aTestStepVidPlayer)
				{
				CActiveScheduler::Add(this);	
				}
		protected:
			void RunL()
				{
				iContainer.OnTimer();
				}
		private:
			CTestStepVidPlayer1106& iContainer;
		};
	CTestTimer* iTestTimer;

private:
	TBool iDisplayAdded;
	TBool iSurfaceCreated;
	};

NONSHARABLE_CLASS(CTestStepVidPlayer1107) :
	public CTestStepVidPlayerAddDisplay
	{
public:
	static CTestStepVidPlayer1107* NewL();
	~CTestStepVidPlayer1107();
	
	virtual void MvpuoOpenComplete(TInt aError);
	
private:
	CTestStepVidPlayer1107();
	};

NONSHARABLE_CLASS(CTestStepVidPlayer1108) :
	public CTestStepVidPlayerAddDisplay
	{
public:
	static CTestStepVidPlayer1108* NewL();
	~CTestStepVidPlayer1108();
	
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MmsehSurfaceCreated(TInt aDisplayId, const TSurfaceId& aId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	
private:
	CTestStepVidPlayer1108();
	};
	
NONSHARABLE_CLASS(CTestStepVidPlayer1110) :
	public CTestStepVidPlayerAddDisplay
	{
public:
	static CTestStepVidPlayer1110* NewL();
	~CTestStepVidPlayer1110();
	
	virtual void MvpuoOpenComplete(TInt aError);
	
private:
	CTestStepVidPlayer1110();
	};		

NONSHARABLE_CLASS(CTestStepVidPlayer1111) :
	public CTestStepVidPlayerAddDisplay
	{
public:
	static CTestStepVidPlayer1111* NewL();
	~CTestStepVidPlayer1111();
	
	virtual void MvpuoOpenComplete(TInt aError);
	
private:
	CTestStepVidPlayer1111();
	};

NONSHARABLE_CLASS(CTestStepVidPlayer1200) :
	public CTestStepVidPlayerAddDisplay
	{
public:
	static CTestStepVidPlayer1200* NewL();
	~CTestStepVidPlayer1200();
	
	virtual void MvpuoOpenComplete(TInt aError);
	
private:
	CTestStepVidPlayer1200();
	};

NONSHARABLE_CLASS(CTestStepVidPlayer1201) :
	public CTestStepVidPlayerAddDisplay
	{
public:
	static CTestStepVidPlayer1201* NewL();
	~CTestStepVidPlayer1201();
	
	virtual void MvpuoOpenComplete(TInt aError);
	
private:
	CTestStepVidPlayer1201();
	};

NONSHARABLE_CLASS(CTestStepVidPlayer1202) :
	public CTestStepVidPlayerAddDisplay
	{
public:
	static CTestStepVidPlayer1202* NewL();
	~CTestStepVidPlayer1202();
	
	virtual TVerdict DoTestStepL();
	
private:
	CTestStepVidPlayer1202();
	};
	
	
#endif // TESTSTEPVIDPLAYEREVENTHANDLER_H

