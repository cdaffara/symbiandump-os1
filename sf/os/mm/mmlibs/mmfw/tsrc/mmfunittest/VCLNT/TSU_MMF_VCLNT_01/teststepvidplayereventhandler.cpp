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

#include "teststepvidplayereventhandler.h"
#include "TS_Codes.h"

_LIT(KAddWindows, "AddWindows");
_LIT(KAddDisplays, "AddDisplays");
_LIT(KExpectedEvents, "ExpectedEvents");
_LIT(KSimulatedEvents, "SimulatedEvents");
_LIT(KClockFileName,"C:\\mm\\mmf\\testfiles\\vclnt\\clock.dummy");
_LIT(KIterations, "Iterations");

CTestStepVidPlayerAddDisplay* CTestStepVidPlayerAddDisplay::NewL(const TDesC& aTestStepName)
	{
	CTestStepVidPlayerAddDisplay* self = new (ELeave) CTestStepVidPlayerAddDisplay(aTestStepName);
	return self;
	}
	
CTestStepVidPlayerAddDisplay::CTestStepVidPlayerAddDisplay(const TDesC& aTestStepName)
	{
	iTestStepName = aTestStepName;
	}
	
CTestStepVidPlayerAddDisplay::~CTestStepVidPlayerAddDisplay()
	{
	}
	
TVerdict CTestStepVidPlayerAddDisplay::DoTestStepPreambleL()
	{
	InitWservL();
	return CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	}
	
TVerdict CTestStepVidPlayerAddDisplay::DoTestStepL()
	{
	iTestStepResult = EFail;
	
	iFileName.Set(KClockFileName);
	iVpu2 = ETrue;
		
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	OpenAndStartSchedulerL(EFalse);
	
	return iTestStepResult;
	}

TVerdict CTestStepVidPlayerAddDisplay::DoTestStepPostambleL()
	{
	delete iPlayer2;
	
	RHashMap<TInt, CWsScreenDevice*>::TIter screenIter(iScreens);
	screenIter.Reset();
	
	while (screenIter.NextValue())
		{
		delete *screenIter.CurrentValue();
		}
	
	iScreens.Close();
	
	RHashMap<TInt, RWindow>::TIter windowIter(iWindows);
	windowIter.Reset();
	
	while (windowIter.NextValue())
		{
		windowIter.CurrentValue()->Close();
		}

	iWindows.Close();

	RHashMap<TInt, RWindowGroup>::TIter rootIter(iRootWindows);
	rootIter.Reset();
	
	while (rootIter.NextValue())
		{
		rootIter.CurrentValue()->Close();
		}
		
	iRootWindows.Close();
	iDisplayIds.Close();
		
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

void CTestStepVidPlayerAddDisplay::MmsehSurfaceCreated(TInt /*aDisplayId*/, const TSurfaceId& /*aId*/, const TRect& /*aCropRect*/, TVideoAspectRatio /*aAspectRatio*/)
	{
	
	}
	
void CTestStepVidPlayerAddDisplay::MmsehSurfaceParametersChanged(const TSurfaceId& /*aId*/, const TRect& /*aCropRect*/, TVideoAspectRatio /*aAspectRatio*/)
	{
	
	}
	
void CTestStepVidPlayerAddDisplay::MmsehRemoveSurface(const TSurfaceId& /*aId*/)
	{
	
	}

void CTestStepVidPlayerAddDisplay::MvpuoOpenComplete(TInt /*aError*/)
	{
	TRAPD
		(err, 
		TPtrC addWindows;
		if (GetStringFromConfig(iTestStepName, KAddWindows, addWindows))
			{
			RArray<TInt> displayIds;
			CleanupClosePushL(displayIds);
		
			ParseDisplaysL(addWindows, displayIds);
			AddWindowsL(displayIds);
			CleanupStack::PopAndDestroy(&displayIds);
			}
	
		TPtrC addDisplays;
		if (GetStringFromConfig(iTestStepName, KAddDisplays, addDisplays))
			{
			ParseDisplaysL(addDisplays, iDisplayIds);		
			AddDisplaysL(iDisplayIds);
			}
		);

	if (err == KErrNone)
		{
		iTestStepResult = EPass;
		}
		
	CActiveScheduler::Stop();	
	}
	
void CTestStepVidPlayerAddDisplay::MvpuoPrepareComplete(TInt /*aError*/)
	{
	
	}
	
void CTestStepVidPlayerAddDisplay::MvpuoFrameReady(CFbsBitmap& /*aFrame*/,TInt /*aError*/)
	{
	
	}
	
void CTestStepVidPlayerAddDisplay::MvpuoPlayComplete(TInt /*aError*/)
	{
	
	}
	
void CTestStepVidPlayerAddDisplay::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	
	}
	
void CTestStepVidPlayerAddDisplay::ParseDisplaysL(TDesC& aList, RArray<TInt>& aDisplays)
	{
	TPtrC list(aList);
	
	for (TInt pivot = list.Locate(TChar(','));
		 pivot >= KErrNone;
		 pivot = list.Locate(TChar(',')))
		{
		TLex lex(list.Left(pivot));
		
		TInt display;
		User::LeaveIfError(lex.Val(display));
		aDisplays.Append(display);
		
		list.Set(list.Mid(pivot + 1));
		}
	}

void CTestStepVidPlayerAddDisplay::ParseEventsL(TDesC& aList, RArray<TPtrC>& aEvents)
	{
	TPtrC list(aList);
		
	for (TInt pivot = list.Locate(TChar(','));
		pivot >= KErrNone;
		pivot = list.Locate(TChar(',')))
		{
		aEvents.AppendL(list.Left(pivot));
		list.Set(list.Mid(pivot + 1));
		}
	}

void CTestStepVidPlayerAddDisplay::AddWindowsL(RArray<TInt>& aDisplays)
	{
	TInt count = aDisplays.Count();
	
	for (TInt i = 0; i < count; ++i)
		{
		CWsScreenDevice* screen = ScreenForDisplayL(aDisplays[i]);
		RWindow* window = WindowForDisplayL(aDisplays[i]);
		
		iPlayer2->AddDisplayWindowL(iWs, *screen, *window);
		}
	}

void CTestStepVidPlayerAddDisplay::AddDisplaysL(RArray<TInt>& aDisplays)
	{
	TInt count = aDisplays.Count();
	
	for (TInt i = 0; i < count; ++i)
		{
		iPlayer2->AddDisplayL(iWs, aDisplays[i], *this);
		}
	}

CWsScreenDevice* CTestStepVidPlayerAddDisplay::ScreenForDisplayL(TInt aDisplay)
	{
	CWsScreenDevice** screenPtr = iScreens.Find(aDisplay);
	
	if (screenPtr != NULL)
		{
		return *screenPtr;
		}
		
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(iWs);
	CleanupStack::PushL(screen);
	User::LeaveIfError(screen->Construct(aDisplay));
		
	iScreens.InsertL(aDisplay, screen);
	CleanupStack::Pop(screen);
	return screen;
	}

RWindowGroup* CTestStepVidPlayerAddDisplay::RootWindowForDisplayL(TInt aDisplay)
	{
	RWindowGroup* rootWindowPtr = iRootWindows.Find(aDisplay);
	
	if (rootWindowPtr != NULL)
		{
		return rootWindowPtr;
		}
		
	RWindowGroup rootWindow(iWs);
	User::LeaveIfError(rootWindow.Construct(reinterpret_cast<TUint32>(this + 16 + aDisplay), ETrue));
	iRootWindows.InsertL(aDisplay, rootWindow);
	
	return iRootWindows.Find(aDisplay);
	}

RWindow* CTestStepVidPlayerAddDisplay::WindowForDisplayL(TInt aDisplay)
	{
	RWindow* windowPtr = iWindows.Find(aDisplay);
	
	if (windowPtr != NULL)
		{
		return windowPtr;
		}
	
	RWindowGroup* rootWindow = RootWindowForDisplayL(aDisplay);
	
	RWindow window(iWs);
	User::LeaveIfError(window.Construct(*rootWindow, reinterpret_cast<TUint32>(this + 24 + aDisplay)));
	iWindows.InsertL(aDisplay, window);
	
	window.SetRequiredDisplayMode(EColor16MA);
	window.SetExtent(TPoint(0,0), TSize(400,200));
	window.SetVisible(ETrue);
	window.Activate();
	iWs.Flush();
	
	return iWindows.Find(aDisplay);
	}
	
TInt CTestStepVidPlayerAddDisplay::SendCustomCommandAsync(TMMFMessageDestinationPckg aDestPckg, TInt aFunction)
	{
	TInt err = KErrNone;
	err = iPlayer2->CustomCommandSync(aDestPckg,
					aFunction,
					KNullDesC8,
					KNullDesC8);
								
	if (err!=KErrNone)
		{
		ERR_PRINTF2(_L("Failed when calling CustomCommandSync(): %d"), err);
		return err;
		}

	return err;	
	}

	
CTestStepVidPlayerEvent* CTestStepVidPlayerEvent::NewL(const TDesC& aTestStepName)
	{
	CTestStepVidPlayerEvent* self = new (ELeave) CTestStepVidPlayerEvent(aTestStepName);
	return self;
	}

CTestStepVidPlayerEvent::CTestStepVidPlayerEvent(const TDesC& aTestStepName) :
	CTestStepVidPlayerAddDisplay(aTestStepName)
	{
	}
	
CTestStepVidPlayerEvent::~CTestStepVidPlayerEvent()
	{
	}
	
void CTestStepVidPlayerEvent::MvpuoOpenComplete(TInt aError)
	{
	CTestStepVidPlayerAddDisplay::MvpuoOpenComplete(aError);
	
	iTestStepResult = EFail;
	
	TInt err = SimulateNextEvent();
	
	if (err == KErrNone)
		{
		CActiveScheduler::Start();
		}
	}

void CTestStepVidPlayerEvent::CheckAndIssueEvent(const TDesC& aEvent)
	{
	if (iCurrentExpectedEvent >= iExpectedEvents.Count())
		{
		return;
		}
	
	if (aEvent == iExpectedEvents[iCurrentExpectedEvent])
		{
		iTestStepResult = EPass;
		}
	else
		{
		iTestStepResult = EFail;
		}
	
	++iCurrentExpectedEvent;
	
	if (iCurrentExpectedEvent >= iExpectedEvents.Count())
		{
		CActiveScheduler::Stop();
		return;
		}
	
	SimulateNextEvent();
	}

TInt CTestStepVidPlayerEvent::SimulateNextEvent()
	{
	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	
	TInt err = KErrNone;
	
	if(iCurrentSimulatedEvent >= iSimulatedEvents.Count())
		{
		return err;
		}
	
	if (iSimulatedEvents[iCurrentSimulatedEvent] == _L("SurfaceCreated"))
		{
		err = SendCustomCommandAsync(destPckg, EMMFTestCustomCommandSimulateSurfaceCreated);
		}
	else if (iSimulatedEvents[iCurrentSimulatedEvent] == _L("SurfaceParametersChanged"))
		{
		err = SendCustomCommandAsync(destPckg, EMMFTestCustomCommandSimulateParaChanged);
		}
	else if (iSimulatedEvents[iCurrentSimulatedEvent] == _L("RemoveSurface"))
		{
		err = SendCustomCommandAsync(destPckg, EMMFTestCustomCommandSimulateRemoveSurface);
		}
	iCurrentSimulatedEvent++;
	return err;
	}

	// MMMFSurfaceEventHandler
void CTestStepVidPlayerEvent::MmsehSurfaceCreated(TInt /*aDisplayId*/, const TSurfaceId& /*aId*/, const TRect& /*aCropRect*/, TVideoAspectRatio /*aAspectRatio*/)
	{
	CheckAndIssueEvent(_L("SurfaceCreated"));	
	}
	
void CTestStepVidPlayerEvent::MmsehSurfaceParametersChanged(const TSurfaceId& /*aId*/, const TRect& /*aCropRect*/, TVideoAspectRatio /*aAspectRatio*/)
	{
	CheckAndIssueEvent(_L("SurfaceParametersChanged"));
	}
	
void CTestStepVidPlayerEvent::MmsehRemoveSurface(const TSurfaceId& /*aId*/)
	{
	CheckAndIssueEvent(_L("RemoveSurface"));
	}

TVerdict CTestStepVidPlayerEvent::DoTestStepL()
	{
	TPtrC simulatedEvents;
	if (GetStringFromConfig(iTestStepName, KSimulatedEvents, simulatedEvents))
		{
		ParseEventsL(simulatedEvents, iSimulatedEvents);
		}
	
	TPtrC expectedEvents;
	if (GetStringFromConfig(iTestStepName, KExpectedEvents, expectedEvents))
		{
		ParseEventsL(expectedEvents, iExpectedEvents);
		}
	
	return CTestStepVidPlayerAddDisplay::DoTestStepL();
	}

CTestStepVidPlayer1106::CTestTimer* 
CTestStepVidPlayer1106::CTestTimer::NewL(CTestStepVidPlayer1106& aTestStepVidPlayer) 
	{
	CTestTimer* self = new(ELeave)CTestTimer(aTestStepVidPlayer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CTestStepVidPlayer1106* CTestStepVidPlayer1106::NewL()
	{
	CTestStepVidPlayer1106* self = new (ELeave) CTestStepVidPlayer1106();
	return self;
	}
	
CTestStepVidPlayer1106::CTestStepVidPlayer1106() :
	CTestStepVidPlayerAddDisplay(_L("MM-MMF-VCLNT-U-1106-CP")),
	iDisplayAdded(EFalse),
	iSurfaceCreated(EFalse)
	{
	}

CTestStepVidPlayer1106::~CTestStepVidPlayer1106()
	{
	}
	
void CTestStepVidPlayer1106::MvpuoOpenComplete(TInt /*aError*/)
	{
	iTestStepResult = EFail;
	
	CActiveScheduler::Stop();
	
	TRAP_IGNORE
		(
		TPtrC addWindows;
		if (GetStringFromConfig(iTestStepName, KAddWindows, addWindows))
			{
				RArray<TInt> displayIds;
				CleanupClosePushL(displayIds);
	
			ParseDisplaysL(addWindows, displayIds);
			AddWindowsL(displayIds);
			CleanupStack::PopAndDestroy(&displayIds);
			}
		);
	
	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	
	SendCustomCommandAsync(destPckg, EMMFTestCustomCommandSimulateSurfaceCreated);

	iTestTimer->After(2000000);	
	CActiveScheduler::Start();
	
	if(iSurfaceCreated)
		{
		return;
		}
	
	TRAP_IGNORE
		(
		iDisplayAdded = ETrue;
		TPtrC addDisplays;
		if (GetStringFromConfig(iTestStepName, KAddDisplays, addDisplays))
			{
			RArray<TInt> displayIds;
			
			ParseDisplaysL(addDisplays, displayIds);		
			AddDisplaysL(displayIds);
			}
		);
	}

void CTestStepVidPlayer1106::MmsehSurfaceCreated(TInt /*aDisplayId*/, const TSurfaceId& /*aId*/, const TRect& /*aCropRect*/, TVideoAspectRatio /*aAspectRatio*/)
	{
	iSurfaceCreated = ETrue;
	
	if(iDisplayAdded)
		{
		iTestStepResult = EPass;
		}
	}

TVerdict CTestStepVidPlayer1106::DoTestStepL()
	{
	iTestTimer = CTestTimer::NewL(*this);
	
	return CTestStepVidPlayerAddDisplay::DoTestStepL();
	}

void CTestStepVidPlayer1106::OnTimer()
	{
	CActiveScheduler::Stop();
	}

CTestStepVidPlayer1107* CTestStepVidPlayer1107::NewL()
	{
	CTestStepVidPlayer1107* self = new (ELeave) CTestStepVidPlayer1107();
	return self;
	}

CTestStepVidPlayer1107::CTestStepVidPlayer1107() :
	CTestStepVidPlayerAddDisplay(_L("MM-MMF-VCLNT-U-1107-CP"))
	{
	}
	
CTestStepVidPlayer1107::~CTestStepVidPlayer1107()
	{
	}
	
void CTestStepVidPlayer1107::MvpuoOpenComplete(TInt aError)
	{
	CTestStepVidPlayerAddDisplay::MvpuoOpenComplete(aError);	
	iTestStepResult = EFail;

	RHashMap<TInt, RWindow>::TIter windowIter(iWindows);
	windowIter.Reset();
	
	while (windowIter.NextValue())
		{
		iPlayer2->RemoveDisplayWindow(*windowIter.CurrentValue());
		}
		

	TInt count = iDisplayIds.Count();
			
	for (TInt i = 0; i < count; ++i)
		{
		iPlayer2->RemoveDisplay(iDisplayIds[i]);
		}
			
	iTestStepResult = EPass;	
	}
	
CTestStepVidPlayer1108* CTestStepVidPlayer1108::NewL()
	{
	CTestStepVidPlayer1108* self = new (ELeave) CTestStepVidPlayer1108;
	return self;
	}

CTestStepVidPlayer1108::CTestStepVidPlayer1108() :
	CTestStepVidPlayerAddDisplay(_L("MM-MMF-VCLNT-U-1108-CP"))
	{
	}

CTestStepVidPlayer1108::~CTestStepVidPlayer1108()
	{
	}

void CTestStepVidPlayer1108::MvpuoOpenComplete(TInt aError)
	{
	CTestStepVidPlayerAddDisplay::MvpuoOpenComplete(aError);
	
	iTestStepResult = EFail;
	
	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	
	SendCustomCommandAsync(destPckg, EMMFTestCustomCommandSimulateSurfaceCreated);
	CActiveScheduler::Start();
	}

void CTestStepVidPlayer1108::MmsehSurfaceCreated(TInt /*aDisplayId*/, const TSurfaceId& /*aId*/, const TRect& /*aCropRect*/, TVideoAspectRatio /*aAspectRatio*/)
	{
	iTestStepResult = EFail;
	
	TInt count = iDisplayIds.Count();

	for (TInt i = 0; i < count; ++i)
		{
		iPlayer2->RemoveDisplay(iDisplayIds[i]);
		}
	
	iTestStepResult = EPass;
	CActiveScheduler::Stop();
	
	TInt err = KErrNone;
	// Run the active scheduler to the end
	while(CActiveScheduler::RunIfReady(err, CActive::EPriorityIdle))
		{
		}
	}

//void CTestStepVidPlayer1108::MmsehSurfaceUpdated(const TSurfaceId& /*aId*/)
//	{
//	iTestStepResult = EFail;
//	}

CTestStepVidPlayer1110* CTestStepVidPlayer1110::NewL()
	{
	CTestStepVidPlayer1110* self = new (ELeave) CTestStepVidPlayer1110;
	return self;
	}

CTestStepVidPlayer1110::CTestStepVidPlayer1110() :
	CTestStepVidPlayerAddDisplay(_L("MM-MMF-VCLNT-U-1110-CP"))
	{
	}

CTestStepVidPlayer1110::~CTestStepVidPlayer1110()
	{
	}
	
void CTestStepVidPlayer1110::MvpuoOpenComplete(TInt aError)
	{
	CTestStepVidPlayerAddDisplay::MvpuoOpenComplete(aError);
	
	RHashMap<TInt, RWindow>::TIter windowIter(iWindows);
	windowIter.Reset();
	
	while (windowIter.NextValue())
		{
		iPlayer2->RemoveDisplayWindow(*windowIter.CurrentValue());
		}
	
	TInt count = iDisplayIds.Count();
	
	for (TInt i = 0; i < count; ++i)
		{
		iPlayer2->RemoveDisplay(iDisplayIds[i]);
		}
	}

CTestStepVidPlayer1111* CTestStepVidPlayer1111::NewL()
	{
	CTestStepVidPlayer1111* self = new (ELeave) CTestStepVidPlayer1111;
	return self;
	}

CTestStepVidPlayer1111::CTestStepVidPlayer1111() :
	CTestStepVidPlayerAddDisplay(_L("MM-MMF-VCLNT-U-1111-CP"))
	{
	}

CTestStepVidPlayer1111::~CTestStepVidPlayer1111()
	{
	}
	
void CTestStepVidPlayer1111::MvpuoOpenComplete(TInt /*aError*/)
	{
	iTestStepResult = EFail;
	TInt iterations = 0;

	if(GetIntFromConfig(iTestStepName, KIterations, iterations))
		{
		TInt i = 0;

		RArray<TInt> displayIds;
		
		TRAPD(err, 
			TPtrC addWindows;
			if (GetStringFromConfig(iTestStepName, KAddWindows, addWindows))
				{		
				ParseDisplaysL(addWindows, displayIds);
				}
					
			TPtrC addDisplays;
			if (GetStringFromConfig(iTestStepName, KAddDisplays, addDisplays))
				{
				ParseDisplaysL(addDisplays, iDisplayIds);
				}
			);

		if (err == KErrNone)
			{
			for (i = 0; i < iterations; ++i)
				{
				TRAP(err,
					AddWindowsL(displayIds);
					AddDisplaysL(iDisplayIds);
					);
			
				if (err != KErrNone)
					{
					break;
					}

				RHashMap<TInt, RWindow>::TIter windowIter(iWindows);
				windowIter.Reset();
	
				while (windowIter.NextValue())
					{
					iPlayer2->RemoveDisplayWindow(*windowIter.CurrentValue());
					}
	
				TInt count = iDisplayIds.Count();
	
				for (TInt i = 0; i < count; ++i)
					{
					iPlayer2->RemoveDisplay(iDisplayIds[i]);
					}
				}
			}

		displayIds.Close();
	
		if (i == iterations)
			{
			iTestStepResult = EPass;
			}
		}

	CActiveScheduler::Stop();
	}

CTestStepVidPlayer1200* CTestStepVidPlayer1200::NewL()
	{
	CTestStepVidPlayer1200* self = new (ELeave) CTestStepVidPlayer1200;
	return self;
	}

CTestStepVidPlayer1200::CTestStepVidPlayer1200() :
	CTestStepVidPlayerAddDisplay(_L("MM-MMF-VCLNT-U-1200-CP"))
	{
	}

CTestStepVidPlayer1200::~CTestStepVidPlayer1200()
	{
	}

void CTestStepVidPlayer1200::MvpuoOpenComplete(TInt /*aError*/)
	{
	iTestStepResult = EFail;

	CWsScreenDevice* screen = NULL;
	RWindow* window = NULL;

	TRAPD(err, screen = ScreenForDisplayL(0));
	TRAP(err, window = WindowForDisplayL(0));
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *screen, *window));
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *screen, *window));
	
	if (err == KErrInUse)
		{
		iTestStepResult = EPass;
		}
	
	CActiveScheduler::Stop();
	}
	
CTestStepVidPlayer1201* CTestStepVidPlayer1201::NewL()
	{
	CTestStepVidPlayer1201* self = new (ELeave) CTestStepVidPlayer1201;
	return self;
	}

CTestStepVidPlayer1201::CTestStepVidPlayer1201() :
	CTestStepVidPlayerAddDisplay(_L("MM-MMF-VCLNT-U-1201-CP"))
	{
	}

CTestStepVidPlayer1201::~CTestStepVidPlayer1201()
	{
	}

void CTestStepVidPlayer1201::MvpuoOpenComplete(TInt /*aError*/)
	{
	iTestStepResult = EFail;
	
	TRAPD(err, 
		TPtrC addDisplays;
		if (GetStringFromConfig(iTestStepName, KAddDisplays, addDisplays))
			{
			ParseDisplaysL(addDisplays, iDisplayIds);		
			AddDisplaysL(iDisplayIds);
			}
		);

	if (err == KErrInUse)
		{
		iTestStepResult = EPass;
		}
		
	CActiveScheduler::Stop();
	}

CTestStepVidPlayer1202* CTestStepVidPlayer1202::NewL()
	{
	CTestStepVidPlayer1202* self = new (ELeave) CTestStepVidPlayer1202;
	return self;
	}

CTestStepVidPlayer1202::CTestStepVidPlayer1202() :
	CTestStepVidPlayerAddDisplay(_L("MM-MMF-VCLNT-U-1202-CP"))
	{
	}

CTestStepVidPlayer1202::~CTestStepVidPlayer1202()
	{
	}

TVerdict CTestStepVidPlayer1202::DoTestStepL()
	{
	iTestStepResult = EFail;
	
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	TRAPD(err, iPlayer2->AddDisplayL(iWs, 0, *this));
	
	if (err == KErrNotReady)
		{
		iTestStepResult = EPass;
		}
	
	return iTestStepResult;
	}

