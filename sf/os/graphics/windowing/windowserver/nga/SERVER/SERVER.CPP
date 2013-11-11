// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Window server 'server' class
// 
//

#include "server.h"
#include "panics.h"
#include "wstop.h"
#include "EVENT.H"
#include <bitdraw.h>
#include <hal.h>
#include "inifile.h"
#include "wspluginmanager.h"
#include "graphics/windowserverconstants.h"

GLREF_D CDebugLogBase *wsDebugLog;

const TUint KRangeCount = 1; 
// We use a lot of 64 bit time calculations, but a periodic can only cope with signed 32 bit times
// which gives them a limit of about 35 minutes.
// Fortunately, our animtions are safe if redrawn early.  Every half an hour isn't going to hurt.
const TTimeIntervalMicroSeconds KHalfHour = 30 * 60 * 1000 * 1000;

const TInt KWsServRanges[KRangeCount] = 
	{	
	0
	};

const TUint8 KElementsIndex[KRangeCount] =
	{
	CPolicyServer::EAlwaysPass,		
	};

const CPolicyServer::TPolicyElement KPolicyElements[] = 
	{ 
	{_INIT_SECURITY_POLICY_C1(ECapabilityPowerMgmt), CPolicyServer::EFailClient}, 
	{_INIT_SECURITY_POLICY_C1(ECapabilitySwEvent), CPolicyServer::EFailClient}, 
	{_INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData), CPolicyServer::EFailClient} 
	};

const CPolicyServer::TPolicy KWsServPolicy =
	{
	CPolicyServer::EAlwaysPass, 
	KRangeCount,
	KWsServRanges,
	KElementsIndex,
	KPolicyElements 	
	};
 	
// CWindowServer::CDefaultAnimationScheduler \\\\\\\\\\\\\\\\\\\\\\\\\\\

class CWindowServer::CDefaultAnimationScheduler: public CBase, public MWsAnimationScheduler
	{
	// Associates a screen number with a CActiveSchedulerWait intance. This is used to
	// achieve synchronous update completion on a specific screen.
	class CScreenUpdateWait : public CActiveSchedulerWait
		{
		public:
			CScreenUpdateWait(TInt aScreenNumber) : iScreenNumber (aScreenNumber) {}
			TInt iScreenNumber;	
		};
	
	struct TScreenUpdateDetails
		{
		CWindowServer::CDefaultAnimationScheduler* iScheduler;
		TInt  iScreenNumber;
		};
	
	struct TSchedule
		{
		MWsScreen* iScreen; // used as a unique index, searching with FindInUnsignedKeyOrder
		TInt  iScreenNumber;
		TBool iScheduled;
		TTime iWhen;
		TBool iRedraw;
		};
public:
	enum TInactivityBehaviour
		{
		EStopAnimation,
		EStopAllDrawing,
		EIgnore,
		};
	class CScreenState;

	CDefaultAnimationScheduler(MWsGraphicDrawerEnvironment& aEnv);
	~CDefaultAnimationScheduler();
	void ConstructL();		//LeaveScan:  member of nested class declaration
	// implementing MWsAnimationScheduler
	void ScheduleAnimation(MWsScreen& aScreen,const TTime& aWhen);
	void UnscheduleAnimation(MWsScreen& aScreen);
	void Invalidate(const TGraphicDrawerId& aId);
	void OnInactive();
	void OnActive();
	void ScheduleRedraw(MWsScreen& aScreen,const TTime& aWhen);
	void DoRedrawNow(MWsScreen& aScreen);
	void DoRedrawNow(MWsScreen& aScreen, MWsAnimationScheduler::MScreenUpdateObserver& aObserver);
	void ClearScreenUpdateObserver(const MWsAnimationScheduler::MScreenUpdateObserver& aObserver);
private:
	static TBool OnIdleCallBack(TAny* aAny);
	void ScheduleUpdate (TInt aScreenNumber, TBool aForce);
	void OnIdleCallBack(TBool aForce);
	static TBool InvokeDueAnimation(TAny* aAny);
	void RedrawAllInvalidatedRegions (TInt aScreen);
	TSchedule* GetScheduledScreenUpdate(TInt aScreen);
	void InvokeDueAnimation(TInt aScreen);
	void ProcessUpdateCompletion (TInt aScreenNumber);
	TInt ScreenNumber(MWsScreen& aScreen) const;
	TTimeIntervalMicroSeconds GetDueDelta (TBool aForceRedraw, TSchedule* aScheduledUpdate);
private:
	RPointerArray<CScreenUpdateWait> iRedrawWaitLoop; 
	RPointerArray<CScreenState> iScreenState;
	MWsGraphicDrawerEnvironment& iEnv;
	static const TInt64 KRedrawGrace;
	static const TInt64 KAnimationGrace;
	CAsyncCallBack* iIdleInitiator;
	TBool iInRedrawNow;
	RArray<TSchedule> iSchedule;
	TBool iInactive;
	TBool iInactiveDraws;
	TBool iRedrawScheduled;
	TInt64 iRedrawGracePeriod;
	TInt64 iAnimationGracePeriod;
	TInactivityBehaviour iInactivityBehaviour;
	};

class CWindowServer::CDefaultAnimationScheduler::CScreenState : public CActive
	{
public:
	static CScreenState* NewL (CDefaultAnimationScheduler* aScheduler, TInt aScreenOrdinal);
	~CScreenState();
	void SetActive ();
	
	void WaitForRedraws(MWsAnimationScheduler::MScreenUpdateObserver& aObserver, TInt aNumRedraws);
	void ClearScreenUpdateObserver(const MWsAnimationScheduler::MScreenUpdateObserver& aObserver);
	CPeriodic* iUpdateOn;
	TTime      iExpectedTickTime;
	TScreenUpdateDetails iScreenUpdateDetails;
	RArray<TGraphicDrawerId> iInvalidated;
	TBool 		iInvalidateAll;
	
private:
	CScreenState (CDefaultAnimationScheduler* aScheduler, TInt aScreenOrdinal);
	void ConstructL ();
	void ReleaseRemainingClients();
	void ReleaseClientsWaitingFor(TUint aCurrentFrame);
	
	void RunL();
	void DoCancel() 
		{
		TRequestStatus* tmpTRS = &iStatus;
		User::RequestComplete(tmpTRS, KErrNone);
		};
	class TWaitingClient
		{
	public:
		TWaitingClient(MWsAnimationScheduler::MScreenUpdateObserver& aObserver, TInt aTargetFrame)
			: iObserver(aObserver), iTargetFrame(aTargetFrame)
			{
			}
		MWsAnimationScheduler::MScreenUpdateObserver& iObserver;
		TUint iTargetFrame;
		};
	TUint iFrameCount;
	RArray<TWaitingClient> iWaitingClients;
	};

// If using the default animation scheduler on a device, these two numbers may be worth tweaking in the inifile
// However, both are maximum periods - wserv will go faster than either if nothing else is using the system.
const TInt64 CWindowServer::CDefaultAnimationScheduler::KRedrawGrace = 0; // do redraws immediately
const TInt64 CWindowServer::CDefaultAnimationScheduler::KAnimationGrace = 0; // do animation redraws immediately
	
CWindowServer::CDefaultAnimationScheduler::CDefaultAnimationScheduler(MWsGraphicDrawerEnvironment& aEnv):
	iEnv(aEnv), iSchedule(1,_FOFF(TSchedule,iScreen))
	{
	}

CWindowServer::CDefaultAnimationScheduler::~CDefaultAnimationScheduler()
	{
	iSchedule.Close();
	delete iIdleInitiator;
	
	TInt screenCount = iScreenState.Count();
	for (TInt ii = 0; ii < screenCount; ii++)
		delete iScreenState[ii];

	iScreenState.Close();

	// Destroy the redraw wait loops associated each screen, and close
	// the associated Array objects.
	TInt waitLoopCount = iRedrawWaitLoop.Count();
	for (TInt waitLoop = 0; waitLoop < waitLoopCount; waitLoop++)
		delete iRedrawWaitLoop[waitLoop];
	iRedrawWaitLoop.Close();
	}
	
void CWindowServer::CDefaultAnimationScheduler::ConstructL()
	{
	_LIT(KOnInactive,"ONINACTIVE");
	_LIT(KStopAnimation,"STOPANIMATION");
	_LIT(KStopAllDrawing,"STOPALLDRAWING");
	_LIT(KIgnore,"IGNORE");
	
	TPtrC inactivityBehaviourString;
	WsIniFile->FindVar(KOnInactive,inactivityBehaviourString);
	if(inactivityBehaviourString.CompareF(KStopAnimation)==0)
		iInactivityBehaviour = EStopAnimation;
	else if(inactivityBehaviourString.CompareF(KStopAllDrawing)==0)
		iInactivityBehaviour = EStopAllDrawing;
	else if(inactivityBehaviourString.CompareF(KIgnore)==0)
		iInactivityBehaviour = EIgnore;
		
	_LIT(KRedrawGracePeriod, "REDRAWGRACEPERIOD");
	TInt tmp = KRedrawGrace;
	WsIniFile->FindVar(KRedrawGracePeriod, tmp);
	iRedrawGracePeriod = tmp;
	
	_LIT(KAnimationGracePeriod, "ANIMATIONGRACEPERIOD");
	tmp = KAnimationGrace;
	WsIniFile->FindVar(KAnimationGracePeriod, tmp);
	iAnimationGracePeriod = tmp;
	
	iIdleInitiator = new(ELeave) CAsyncCallBack(TCallBack(OnIdleCallBack,this),EWsGraphicAnimateAwaitIdlePriority);
	
	TInt screenCount;
	User::LeaveIfError(HAL::Get( HAL::EDisplayNumberOfScreens, screenCount)); 
	for (TInt i = 0; i < screenCount; i++)
		{
		CScreenState* screenState = CScreenState::NewL (this, i);
		CleanupStack::PushL(screenState);
		iScreenState.AppendL (screenState);
		CleanupStack::Pop(screenState);
		}
		
	
	// Ensure that the wait loop array has some allocated slots, making it highly 
	// unlikely that Append() will fail due to OOM.
	iRedrawWaitLoop.ReserveL (8);
	}

void CWindowServer::CDefaultAnimationScheduler::Invalidate(const TGraphicDrawerId& aId)
	{
	const TInt screenCount = iEnv.ScreenCount();
	for(TInt ii = 0; ii < screenCount; ii++)
		{
		CScreenState* screenState = iScreenState[ii];
		if(!screenState->iInvalidateAll)
			{
			switch(screenState->iInvalidated.InsertInOrder(aId,TLinearOrder<TGraphicDrawerId>(TGraphicDrawerId::Compare)))
				{
				case KErrNone:
				case KErrAlreadyExists:
					break;
				default:
					screenState->iInvalidateAll = ETrue;
					screenState->iInvalidated.Reset();
				}
			}
		}
	iIdleInitiator->CallBack();
	}

CWindowServer::CDefaultAnimationScheduler::CScreenState::CScreenState (CDefaultAnimationScheduler* aScheduler, TInt aScreenNumber):
	CActive(EComposeCompletePriority)
	{
	iScreenUpdateDetails.iScheduler = aScheduler;
	iScreenUpdateDetails.iScreenNumber = aScreenNumber;
	}

CWindowServer::CDefaultAnimationScheduler::CScreenState* 
CWindowServer::CDefaultAnimationScheduler::CScreenState::NewL (CDefaultAnimationScheduler* aScheduler, TInt aScreenOrdinal)
	{
	CScreenState* self = new(ELeave)CScreenState(aScheduler, aScreenOrdinal);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

void CWindowServer::CDefaultAnimationScheduler::CScreenState::ConstructL ()
	{
	iUpdateOn = CPeriodic::NewL(EComposeCompletePriority);
	iWaitingClients.ReserveL(8);
	CActiveScheduler::Add(this);
	}

CWindowServer::CDefaultAnimationScheduler::CScreenState::~CScreenState()
	{
	CActive::Cancel();
	iInvalidated.Close();
	delete iUpdateOn;
	TInt i = iWaitingClients.Count();
	while(i--)
		{
		iWaitingClients[i].iObserver.ScreenUpdateComplete(KErrAbort);
		}
	iWaitingClients.Close();
	}

void CWindowServer::CDefaultAnimationScheduler::CScreenState::SetActive()
	{
	CActive::SetActive ();
	}

/**
This function is called from CWsClient d'tor to make sure we will not hang on to any deleted objects. 
*/
void CWindowServer::CDefaultAnimationScheduler::CScreenState::ClearScreenUpdateObserver(const MWsAnimationScheduler::MScreenUpdateObserver& aObserver)
	{
	const TInt count = iWaitingClients.Count();
	for(TInt i = count-1 ; i >= 0; i--)
		{
		if( &aObserver == &(iWaitingClients[i].iObserver) )
			{
			TWaitingClient& client = iWaitingClients[i];
			client.iObserver.ScreenUpdateComplete(KErrCancel);
			iWaitingClients.Remove(i);
			}
		}
	}

void CWindowServer::CDefaultAnimationScheduler::CScreenState::WaitForRedraws(MWsAnimationScheduler::MScreenUpdateObserver& aObserver, TInt aNumRedraws)
	{
	const TUint targetFrame = iFrameCount + aNumRedraws;
	TWaitingClient request(aObserver, targetFrame);
	TInt err = iWaitingClients.Append(request);
	if(err != KErrNone)
		{
		//If OOM and already have 8 waiting clients we will not accept a 9th client
		aObserver.ScreenUpdateComplete(KErrNoMemory);
		}
	}

void CWindowServer::CDefaultAnimationScheduler::CScreenState::ReleaseRemainingClients()
	{
	const TInt count = iWaitingClients.Count();
	for(TInt i = count-1; i >= 0; i--)
		{
		TWaitingClient& client = iWaitingClients[i];
		client.iObserver.ScreenUpdateComplete(KErrNone);
		iWaitingClients.Remove(i);
		}
	}

void CWindowServer::CDefaultAnimationScheduler::CScreenState::ReleaseClientsWaitingFor(TUint aCurrentFrame)
	{
	const TInt count = iWaitingClients.Count();
	for(TInt i = count-1; i >= 0; i--)
		{
		TWaitingClient& client = iWaitingClients[i];
		if(aCurrentFrame == client.iTargetFrame)
			{
			client.iObserver.ScreenUpdateComplete(KErrNone);
			iWaitingClients.Remove(i);
			}
		}
	}

/** 
Invoked when the rendering pipline signals that it is ready to receive updates.
*/
void CWindowServer::CDefaultAnimationScheduler::CScreenState::RunL() 
	{
	iFrameCount++;
	
	//Complete any clients waiting for this frame
	ReleaseClientsWaitingFor(iFrameCount);
	
	iScreenUpdateDetails.iScheduler->ProcessUpdateCompletion (iScreenUpdateDetails.iScreenNumber);
	
	if(!IsActive())
		{
		//No further pending frames, release all remaining clients
		ReleaseRemainingClients();
		}
	
	}

void CWindowServer::CDefaultAnimationScheduler::ProcessUpdateCompletion (TInt aScreenNumber)
	{
	// Stop all waitloops related to the specified screen.
	TInt waitLoopCount = iRedrawWaitLoop.Count();
	for (TInt waitLoop = 0; waitLoop < waitLoopCount; waitLoop++)
		if (iRedrawWaitLoop[waitLoop]->IsStarted() && (iRedrawWaitLoop[waitLoop]->iScreenNumber == aScreenNumber))
			iRedrawWaitLoop[waitLoop]->AsyncStop();
	
	// Perform any outstanding redraws on the specified screen.
	ScheduleUpdate (aScreenNumber, ETrue);
	}

void CWindowServer::CDefaultAnimationScheduler::DoRedrawNow(MWsScreen& aScreen, MWsAnimationScheduler::MScreenUpdateObserver& aObserver)
	{
	TInt screenNumber = ScreenNumber (aScreen);
	TInt redrawCount = 0;
	
	// redrawCount is the number of times we should wait for redraws to complete.
	// If a redraw is not currently active then we need to wait (at most) once: for
	// any outstanding scheduled update to complete.
	// If a redraw is currently active then we need to wait (at most) twice: once for
	// the current update to complete, and once for any outstanding scheduled update to complete.   
	if (!iScreenState[screenNumber]->IsActive())
		{
		// No animation in progress, so force a redraw of due updates.
		ScheduleUpdate(screenNumber, ETrue);

		// If there is still nothing drawing, set redrawCount to zero to make sure we do not wait.
		if (!iScreenState[screenNumber]->IsActive())
			{
			redrawCount = 0;
			aObserver.ScreenUpdateComplete(KErrNone);
			}
		else
			{
			redrawCount = 1;
			iScreenState[screenNumber]->WaitForRedraws(aObserver, redrawCount);
			}
		}
	else
		{
		redrawCount = 2;
		iScreenState[screenNumber]->WaitForRedraws(aObserver, redrawCount);
		}
	}

void CWindowServer::CDefaultAnimationScheduler::ClearScreenUpdateObserver(const MWsAnimationScheduler::MScreenUpdateObserver& aObserver)
	{
	const TInt count = iScreenState.Count();
	for(TInt screenNumber=0; screenNumber<count; screenNumber++) 
		{
		iScreenState[screenNumber]->ClearScreenUpdateObserver(aObserver);
		}
	}

/**
Switch to deactivate animation or drawing (based on setting of iInactivityBehaviour).
See InvokeDueAnimation().
*/
void CWindowServer::CDefaultAnimationScheduler::OnInactive()
	{
	iInactive = ETrue;
	}

/**
Switch to deactivate/activate animation or drawing (based on setting of iInactivityBehaviour).
See InvokeDueAnimation().
*/	
void CWindowServer::CDefaultAnimationScheduler::OnActive()
	{
	iInactive = EFalse;
	if(iInactiveDraws)
		{
		iInactiveDraws = EFalse;
		iIdleInitiator->CallBack();
		}
	}

void CWindowServer::CDefaultAnimationScheduler::ScheduleRedraw(MWsScreen& aScreen,const TTime& aWhen)
	{
	iRedrawScheduled = ETrue;
	ScheduleAnimation(aScreen, aWhen);
	}

/** 
Given a MWsScreen pointer, return an integer value representing the ordinal position
of the screen in the Window Server.
*/
TInt CWindowServer::CDefaultAnimationScheduler::ScreenNumber(MWsScreen& aScreen) const
	{
	TInt numberOfScreens = CWsTop::NumberOfScreens();
	TInt theScreen;
	
	for (theScreen = 0; theScreen < numberOfScreens; theScreen++)
		if (CWsTop::Screen(theScreen) == &aScreen) 
			break;
	
	WS_ASSERT_DEBUG(theScreen < numberOfScreens, EWsPanicWsGraphic);
	return theScreen;
	}

/**
Perform redraw and return only when completed.
NOTE: This method uses CActiveSchedulerWait to run a "modal wait loop" while the
		redraw complete signal is pending. When the signal is received, AsyncStop() is
		invoked on all active wait loops for the signalling screen. 
*/
void CWindowServer::CDefaultAnimationScheduler::DoRedrawNow(MWsScreen& aScreen)
	{	
	TInt screenNumber = ScreenNumber (aScreen);
	TInt redrawCount = 0;
	
	// redrawCount is the number of times we should wait for redraws to complete.
	// If a redraw is not currently active then we need to wait (at most) once: for
	// any outstanding scheduled update to complete.
	// If a redraw is currently active then we need to wait (at most) twice: once for
	// the current update to complete, and once for any outstanding scheduled update to complete.	
	if (!iScreenState[screenNumber]->IsActive())
		{		
		// No animation in progress, so force a redraw of due updates.
		ScheduleUpdate(screenNumber, ETrue);
		
		// If there is still nothing drawing, set redrawCount to zero to make sure we do not wait.
		if (!iScreenState[screenNumber]->IsActive())
			redrawCount = 0;
		else
			redrawCount = 1;
		}
	else
		redrawCount = 2;
	
	// Wait for the sepecified number of redraws.
	if (redrawCount)
		{
		// Allocate the wait loop on the stack so we are not subject to heap OOM.
		TBuf8<sizeof (CScreenUpdateWait)> buf;
		Mem::FillZ(&buf, sizeof (CScreenUpdateWait));
		CScreenUpdateWait* waitLoop = new (&buf) CScreenUpdateWait(screenNumber);
		if (iRedrawWaitLoop.Append(waitLoop) == KErrNone)
			{
			// Run the active scheduler while updates are active
			while (redrawCount-- && iScreenState[screenNumber]->IsActive())
				waitLoop->Start();
			
			iRedrawWaitLoop.Remove(iRedrawWaitLoop.Count() - 1);			
			}
		waitLoop->~CScreenUpdateWait();
		}
	}

/**
Schedule an update for a specific screen at a given point in time.
*/ 
void CWindowServer::CDefaultAnimationScheduler::ScheduleAnimation(MWsScreen& aScreen, const TTime& aWhen)
	{	
	TSchedule schedule;
	schedule.iScreen = &aScreen;
	schedule.iScheduled = ETrue;
	schedule.iWhen = aWhen;
	schedule.iScreenNumber = ScreenNumber (aScreen);
	schedule.iRedraw = iRedrawScheduled;
	iRedrawScheduled = EFalse;
	TBool ok = EFalse;
	const TInt idx = iSchedule.FindInUnsignedKeyOrder(schedule);
	if(0 <= idx)
		{
		TSchedule& currSchedule=iSchedule[idx];
		if(currSchedule.iScheduled)
			{
			if(currSchedule.iWhen > aWhen)
				{
				currSchedule.iWhen = aWhen;
				}
			}
		else
			{
			currSchedule = schedule;
			}
		ok = ETrue;
		}
	else
		ok = (KErrNone == iSchedule.InsertInUnsignedKeyOrder(schedule));

	if(ok)
		iIdleInitiator->CallBack();
	}

void CWindowServer::CDefaultAnimationScheduler::UnscheduleAnimation(MWsScreen& aScreen)
	{
	TSchedule schedule;
	schedule.iScreen = &aScreen;
	const TInt idx = iSchedule.FindInUnsignedKeyOrder(schedule);
	if(0 <= idx)
		iSchedule[idx].iScheduled = EFalse;
	}

TBool CWindowServer::CDefaultAnimationScheduler::OnIdleCallBack(TAny* aAny)
	{
	WS_ASSERT_DEBUG(aAny, EWsPanicWsGraphic);
	
	if(aAny)
		static_cast<CDefaultAnimationScheduler*>(aAny)->OnIdleCallBack(EFalse);

	return EFalse; //ignored by caller
	}

void CWindowServer::CDefaultAnimationScheduler::OnIdleCallBack(TBool aForce)
	{
	const TInt screenCount = iEnv.ScreenCount();
	for(TInt ii = 0; ii < screenCount; ii++)
		ScheduleUpdate (ii, aForce);
	}



/** 
@return The number of microseconds (from now) that the specified scheduled update should be run at. This
takes into account any set grace period and protects the scheduler from entering an infinite loop servicing
animations with back-to-back frame updates.
*/
TTimeIntervalMicroSeconds CWindowServer::CDefaultAnimationScheduler::GetDueDelta (TBool aForceRedraw, TSchedule* aScheduledUpdate)
	{
	WS_ASSERT_DEBUG(aScheduledUpdate, EWsPanicWsGraphic);
	WS_ASSERT_DEBUG(aScheduledUpdate->iScheduled, EWsPanicWsGraphic);
	
	TTime now;
	TInt64 grace = I64LIT(0); 
	TTimeIntervalMicroSeconds thisUpdateDueIn = I64LIT(0); //Microseconds from now
	
	// Only use grace periods if not forcing due updates.
	if (!aForceRedraw)
		{
		if (aScheduledUpdate->iRedraw)
			grace = iRedrawGracePeriod;
		else
			grace = iAnimationGracePeriod;
		}
	
	now.UniversalTime();
	thisUpdateDueIn = aScheduledUpdate->iWhen.MicroSecondsFrom(now);
	
	// Add the grace period if the update is due in less time than the grace period.
	if (thisUpdateDueIn < grace)
		thisUpdateDueIn = grace;
	else if (thisUpdateDueIn > KHalfHour)
		thisUpdateDueIn = KHalfHour;
	
	return thisUpdateDueIn;
	}

/**
Schedule an actual screen update at the point in time at which it is due. The due time may be modified by
this method based on any "grace period" values.
 
@param aScreen		Screen number to update.
@param aForceRedraw Force redraws that are due. This causes grace periods not to be used.
*/
void CWindowServer::CDefaultAnimationScheduler::ScheduleUpdate (TInt aScreenNumber, TBool aForceRedraw)
	{	
	// Schedule updates for any invalidated regions.
	RedrawAllInvalidatedRegions (aScreenNumber);
	
	TSchedule* scheduledUpdate = GetScheduledScreenUpdate(aScreenNumber);
	if (scheduledUpdate)
		{
		WS_ASSERT_DEBUG(scheduledUpdate->iScheduled, EWsPanicWsGraphic);
		WS_ASSERT_DEBUG(aScreenNumber < iScreenState.Count(), EWsPanicWsGraphic);
		
		CScreenState& screenState = *iScreenState[aScreenNumber];
				
		// Initiate redraw if scheduled and not currently updating the display.
		if(!screenState.IsActive())
			{
			TTimeIntervalMicroSeconds thisUpdateDueIn =	
				GetDueDelta (aForceRedraw, scheduledUpdate);
			
			// Reschedule any preexisting update if this one is due earlier.
			// If this update is not due earlier than a preexisting update then 
			// there is nothing to do - just let the pending update occur.
			TTime now;
			now.UniversalTime();
			TBool performUpdate = ETrue;
			if(screenState.iUpdateOn->IsActive())
				{
				if (thisUpdateDueIn < screenState.iExpectedTickTime.MicroSecondsFrom(now))
					screenState.iUpdateOn->Cancel();
				else
					performUpdate = EFalse;
				}
			
			if (performUpdate)
				{
				if (thisUpdateDueIn.Int64() == 0) // Perform an immediate update if we are due.
					{
					screenState.iExpectedTickTime = now;
					InvokeDueAnimation(aScreenNumber);
					}
				else // Schedule the tick at the appropriate time.
					{
					WS_ASSERT_DEBUG(thisUpdateDueIn.Int64() > 0, EWsPanicWsGraphic);
					screenState.iExpectedTickTime = now + thisUpdateDueIn;
					screenState.iUpdateOn->Start(thisUpdateDueIn.Int64(),0,TCallBack(InvokeDueAnimation, &screenState.iScreenUpdateDetails));
					}
				}
			}
		}
	}

/**
@return 	A pointer to the scheduled update details currently associated with the specified screen.
			If there is no scheduled update then NULL is returned.
@note 		There is only ever one scheduled update per screen.
*/
CWindowServer::CDefaultAnimationScheduler::TSchedule* CWindowServer::CDefaultAnimationScheduler::GetScheduledScreenUpdate(TInt aScreenNumber)
	{
	TSchedule* result = NULL;
	const TInt count = iSchedule.Count();
	for(TInt ii = 0; ii < count; ii++)
		{
		if (iSchedule[ii].iScreenNumber == aScreenNumber)
			{
			if (iSchedule[ii].iScheduled)
				result = &iSchedule[ii];
			break;
			}
		}
	
	return result;
	}

/**
Redraw invalidated graphic IDs. If invalid regions exist, this will cause ScheduleRedraw() to be invoked.
*/ 
void CWindowServer::CDefaultAnimationScheduler::RedrawAllInvalidatedRegions (TInt aScreen)
	{	
	WS_ASSERT_DEBUG(iScreenState.Count() > aScreen, EWsPanicWsGraphic);
	
	CScreenState& screenState = *iScreenState[aScreen];
	if(screenState.iInvalidateAll || screenState.iInvalidated.Count())
		{		
		const TArray<TGraphicDrawerId> invalidArray = screenState.iInvalidated.Array();
		MWsScreen* screen = iEnv.Screen(aScreen);
		WS_ASSERT_DEBUG(screen, EWsPanicWsGraphic);
		if(screen)
			{
			if(screenState.iInvalidateAll)
				Redraw(*screen);
			else
				RedrawInvalid(*screen, screenState.iInvalidated.Array());
			}
		screenState.iInvalidateAll = EFalse;
		}
	screenState.iInvalidated.Reset();
	}
	
TBool CWindowServer::CDefaultAnimationScheduler::InvokeDueAnimation(TAny* aAny)
	{
	WS_ASSERT_DEBUG(aAny, EWsPanicWsGraphic);
	TScreenUpdateDetails* args = reinterpret_cast<TScreenUpdateDetails*>(aAny);
	if(args)
		args->iScheduler->InvokeDueAnimation (args->iScreenNumber);

	return EFalse;
	}

void CWindowServer::CDefaultAnimationScheduler::InvokeDueAnimation(TInt aScreen)
	{
	WS_ASSERT_DEBUG(aScreen < iScreenState.Count(), EWsPanicWsGraphic);
	CScreenState& screenState = *iScreenState[aScreen];
	WS_ASSERT_DEBUG(!screenState.IsActive(), EWsPanicWsGraphic);

	// All updates are driven through ScheduleRedraw() and ScheduleAnimation().
	screenState.iUpdateOn->Cancel();
	
	TSchedule* scheduledUpdate = GetScheduledScreenUpdate(aScreen);	
	if (scheduledUpdate)
		{
		WS_ASSERT_DEBUG(scheduledUpdate->iScheduled, EWsPanicWsGraphic);
		
		// Honour any flags that indicate we should not redraw. 
		switch(iInactivityBehaviour)
			{
			case EStopAnimation :
				// Stop server side drawing. Only the client may redraw if iInactive is set. 
				if(iInactive && !scheduledUpdate->iRedraw)
					{
					iInactiveDraws = ETrue;
					return;
					}
				break;
			case EStopAllDrawing :
				// Stop both client and server side drawing.
				if(iInactive) 
					{
					iInactiveDraws = ETrue;
					return;
					}
				break;
			case EIgnore :
				break;
			default :
				WS_ASSERT_DEBUG(EFalse, EWsPanicWsGraphic);
				break;
			}
	
		scheduledUpdate->iScheduled = EFalse;
		screenState.SetActive();
		Animate(*scheduledUpdate->iScreen, &(screenState.iStatus));
		}
	}

// CWindowServer::CServer \\\\\\\\\\\\\\\\\\\\\\

class CWindowServer::CServer : public CPolicyServer
	{
public:
	static CServer* NewL()
		{
		return new(ELeave) CServer;
		}
	void StartL()
		{
		CPolicyServer::StartL(KWSERVServerName);
		}
	TInt SessionCount()
		{
		iSessionIter.SetToFirst();
		TInt count=0;
		while(iSessionIter++)
			++count;
		return(count);
		}

public: //from CPolicyServer
	/** Creates a new client for this server. */
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const
		{
		TVersion v(KWservMajorVersionNumber, KWservMinorVersionNumber, KWservBuildVersionNumber);
		if (User::QueryVersionSupported(v, aVersion)==EFalse)
			User::Leave(KErrNotSupported);
		RThread thread;
		User::LeaveIfError(aMessage.Client(thread));
		return(new(ELeave) CWsClient(thread));
		}
private:
	CServer() : CPolicyServer(EMainServerPriority, KWsServPolicy)
	{}
	};


// CWindowServer \\\\\\\\\\\\\\\\\\\\\\\\\\\

CWindowServer *CWindowServer::NewL()
//
// Create a new CWindowServer.
//
	{
	CWindowServer* self = new(ELeave) CWindowServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CWindowServer::CWindowServer()
//
// Constructor.
//
	{
	}

CWindowServer::~CWindowServer()
	{
	delete iServer;

	iMemoryReleases.Reset();
	WS_ASSERT_DEBUG(iDrawerMasterIndex.IsEmpty(), EWsPanicWsGraphic);
	iDrawerMasterIndex.Close();
	
	delete iDefaultAnimationScheduler;
	iDefaultAnimationScheduler = NULL; //might be called from clients during server destruction	
	}
	
void CWindowServer::ConstructL()
	{
	iServer = CServer::NewL();
	CWsTop::PluginManager()->InitializePluginsL(*this); // plugins are loaded and own by CWsTop
	iDefaultAnimationScheduler = new(ELeave) CDefaultAnimationScheduler(*this);
	iDefaultAnimationScheduler->ConstructL();
	RegisterMemoryRelease(this);
	}

void CWindowServer::StartL()
	{
	iServer->StartL();
	}

void CWindowServer::SetPinClientDescriptors(TBool aPin)
	{
	iServer->SetPinClientDescriptors(aPin);
	}

TInt CWindowServer::SessionCount()
	{
	return iServer->SessionCount();
	}

const CWsGraphicDrawer* CWindowServer::ResolveGraphic(const TGraphicDrawerId& aId) const
	{
	return iDrawerMasterIndex.ResolveGraphic(aId);
	}
	
void CWindowServer::Invalidate(const TGraphicDrawerId& aId)
	{
	AnimationScheduler()->Invalidate(aId);
	}
	
TInt CWindowServer::ScreenCount() const
	{
	return CWsTop::NumberOfScreens();
	}

MWsScreen* CWindowServer::Screen(TInt aIndex)
	{
	if((aIndex >= 0) && (aIndex < ScreenCount()))
		{
		return CWsTop::Screen(aIndex);
		}
	return NULL;
	}
	
const MWsScreen* CWindowServer::Screen(TInt aIndex) const
	{
	if((aIndex >= 0) && (aIndex < ScreenCount()))
		{
		return CWsTop::Screen(aIndex);
		}
	return NULL;
	}
	
/**
Custom Animation Scheduler
*/
TBool CWindowServer::SetCustomAnimationScheduler(MWsAnimationScheduler* /*aScheduler*/)
	{
	return EFalse;
	}

TBool CWindowServer::HasCustomAnimationScheduler() const
	{
	return EFalse;
	}

TBool CWindowServer::ClearCustomAnimationScheduler(MWsAnimationScheduler* /*aCurrentScheduler*/)
	{
	return EFalse;
	}
	
MWsAnimationScheduler* CWindowServer::AnimationScheduler()
	{
	return iDefaultAnimationScheduler;
	}

void CWindowServer::PrepareShutdown()
	{
	//Stop the renderloop, i.e. prevent any further calls to MWsAnimationScheduler::Animate() 
	delete iDefaultAnimationScheduler;
	iDefaultAnimationScheduler = NULL;
	}

TInt CWindowServer::RegisterEventHandler(CWsGraphicDrawer* aDrawer, MWsEventHandler* aHandler, TUint32 aEventMask)
	{
	if (!aDrawer || !aHandler || aEventMask==0)
		return KErrArgument;
	TInt err = TWindowServerEvent::RegisterDrawerHandler(aDrawer, aEventMask);
	if (err != KErrNone)
		return err;
	aDrawer->SetEventHandler(aHandler);
	return KErrNone;
	}
	
TInt CWindowServer::UnregisterEventHandler(CWsGraphicDrawer* aDrawer)
	{
	if (!aDrawer || (aDrawer && !aDrawer->HasEventHandler()))
		return KErrArgument;
	TInt err = TWindowServerEvent::UnregisterDrawerHandler(aDrawer);
	if (err != KErrNone)
		return err;
	aDrawer->SetEventHandler(NULL);
	return KErrNone;
	}
	
TInt CWindowServer::RegisterWsEventHandler(MWsEventHandler* aHandler, TUint32 aEventMask)
	{
	if (!aHandler || aEventMask==0)
		return KErrArgument;
	return TWindowServerEvent::RegisterWsEventHandler(aHandler, aEventMask);
	}
	
TInt CWindowServer::UnregisterWsEventHandler(MWsEventHandler* aHandler)
	{
	return TWindowServerEvent::UnregisterWsEventHandler(aHandler);
	}
	
TAny* CWindowServer::ResolveObjectInterface(TUint aTypeId)
	{
	switch(aTypeId)
		{
		case MWsActiveSchedulerDebug::EWsObjectInterfaceId:
			return static_cast<MWsActiveSchedulerDebug*>(CWsActiveScheduler::Static());
		case MWsIniFile::EWsObjectInterfaceId:
			return static_cast<MWsIniFile*>(WsIniFile);
		}
	
	if (CWsPluginManager *plugMgr=CWsTop::PluginManager())
		return plugMgr->ResolveObjectInterface(aTypeId);
	
	return NULL;
	}

void CWindowServer::Log(TInt aPriority,const TDesC &aFmt,TInt aParam)
	{
	if (wsDebugLog)
		{
		wsDebugLog->MiscMessage(aPriority, aFmt, aParam);
		}
	}

// CWsGraphicDrawer master index

TInt CWindowServer::AddGraphicDrawer(CWsGraphicDrawer* aDrawer)
	{
	return iDrawerMasterIndex.Add(aDrawer);
	}

TInt CWindowServer::SwapGraphicDrawer(CWsGraphicDrawer* aDrawer)
	{
	return iDrawerMasterIndex.Swap(aDrawer);
	}
	
TInt CWindowServer::RemoveGraphicDrawer(const TGraphicDrawerId& aId)
	{
	return iDrawerMasterIndex.Remove(aId);
	}

TInt CWindowServer::RemoveAllGraphicDrawers(const MWsClient& aOwner)
	{
	return iDrawerMasterIndex.RemoveAll(aOwner);
	}

TInt CWindowServer::RegisterMemoryRelease(MWsMemoryRelease * aMemoryRelease)
	{
	return iMemoryReleases.Append(aMemoryRelease);
	}

void CWindowServer::UnregisterMemoryRelease(MWsMemoryRelease * aMemoryRelease)
	{
	for (TInt ii = iMemoryReleases.Count() - 1; ii >= 0; --ii)
		{
		if (iMemoryReleases[ii] == aMemoryRelease)
			{
			iMemoryReleases.Remove(ii);
			break;
			}
		}
	}

TBool CWindowServer::ReleaseMemory(TMemoryReleaseLevel aLevel)
	{
	return CWsWindow::ReleaseMemory(aLevel);
	}

TBool CWindowServer::ReleaseMemory()
	{
	TBool released = EFalse;
	for (TInt level = MWsMemoryRelease::ELow; !released && level <= MWsMemoryRelease::EHigh; ++level)
		{
		for (TInt ii = iMemoryReleases.Count() - 1; !released && ii >= 0; --ii)
			{
			released = iMemoryReleases[ii]->ReleaseMemory(static_cast<TMemoryReleaseLevel>(level));
			}
		}
	return released;
	}

void CWindowServer::DestroySessionsForShutdown()
	{
	delete iServer;
	iServer = NULL;
	}

