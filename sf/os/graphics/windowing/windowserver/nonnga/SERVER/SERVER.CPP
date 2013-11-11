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
public:
	enum TInactivityBehaviour
		{
		EStopAnimation,
		EStopAllDrawing,
		EIgnore,
		};
	class CKickBack;
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
	void ClearScreenUpdateObserver(const MWsAnimationScheduler::MScreenUpdateObserver& /*aObserver*/) {}
	TInt RemoveGraphicDrawer(const TGraphicDrawerId &aId);
	
private:
	static TBool OnIdleCallBack(TAny* aAny);
	void OnIdleCallBack(TBool aForceRedraw);
	static TBool OnTickCallBack(TAny* aAny);
	void OnTickCallBack();
	static TBool OnKickBack(TAny* aAny);
	void OnKickBack();
private:
	MWsGraphicDrawerEnvironment& iEnv;
	static const TInt64 KRedrawGrace;
	static const TInt64 KAnimationGrace;
	static const TInt64 KDsaAnimationGrace;
	CAsyncCallBack* iIdleInitiator;
	CPeriodic* iTick;
	CKickBack* iKickBack;
	struct TSchedule
		{
		MWsScreen* iScreen; // used as a unique index, searching with FindInUnsignedKeyOrder
		TBool iSchedule;
		TTime iWhen;
		TInt iGeneration;
		};
	RArray<TSchedule> iSchedule;
	TInt iGeneration;
	RArray<TGraphicDrawerId> iInvalidated;
	TBool iInvalidateAll; // if we could not add to iInvalidated, we have to instead redraw everything
	TTime iWhenDesired;
	TBool iInactive;
	TBool iInactiveDraws;
	TTime iExpectedTime;
	TBool iRedrawScheduled;
	TInt64 iRedrawGracePeriod;
	TInt64 iAnimationGracePeriod;
	TInt64 iDsaAnimationGracePeriod;
	TInactivityBehaviour iInactivityBehaviour;
	};
	
TInt CWindowServer::CDefaultAnimationScheduler::RemoveGraphicDrawer(const TGraphicDrawerId &aId)
	{
	TInt index=iInvalidated.Find(aId);
	if (index!=KErrNotFound)
		iInvalidated.Remove(index);
	return index;
	}	
	
class CWindowServer::CDefaultAnimationScheduler::CKickBack: public CActive
	{
public:
	CKickBack(const TCallBack& aCallBack);
	void ConstructL();
	void RequestKickBack();
	~CKickBack();
private:
	static TInt IdleThreadFunc(TAny* aAny);
	void Loop();
	// from CActive
	void RunL(); // fires when kicked back by the idle thread
	void DoCancel();
private:
	RThread iWservThread;
	RThread iIdleThread;
	TRequestStatus iIdleStatus;
	TCallBack iCallBack;
	};
	
CWindowServer::CDefaultAnimationScheduler::CKickBack::CKickBack(const TCallBack& aCallBack) :
CActive(EPriorityNormal),
iCallBack(aCallBack)
	{
	CActiveScheduler::Add(this);
	}
	
void CWindowServer::CDefaultAnimationScheduler::CKickBack::ConstructL()
	{
	_LIT(KIdleThreadName,"NearlyIdleKickBack");
	const TInt KStackSize = 1024;
	User::LeaveIfError(iWservThread.Open(iWservThread.Id()));
	User::LeaveIfError(iIdleThread.Create(KIdleThreadName(),IdleThreadFunc,KStackSize,NULL,this));
	iIdleThread.SetPriority(EPriorityAbsoluteVeryLow);
	iIdleThread.Resume();
	}
	
void CWindowServer::CDefaultAnimationScheduler::CKickBack::RequestKickBack()
	{
	if (!IsActive())
		{
		iStatus = KRequestPending;
		SetActive();
		TRequestStatus * status = &iIdleStatus;
		iIdleThread.RequestComplete(status, KErrNone);
		}
	}

void CWindowServer::CDefaultAnimationScheduler::CKickBack::Loop()
	{
	FOREVER
		{
		// This is used here for performance reasons. 
		User::WaitForRequest(iIdleStatus); 
		iIdleStatus = KRequestPending;
		if (IsActive()&& (iStatus == KRequestPending))
			{
			TRequestStatus * status = &iStatus;
			iWservThread.RequestComplete(status,KErrNone);
			}
		}
	}

void CWindowServer::CDefaultAnimationScheduler::CKickBack::RunL()
	{
	iCallBack.CallBack();
	}

void CWindowServer::CDefaultAnimationScheduler::CKickBack::DoCancel()
	{
	}

CWindowServer::CDefaultAnimationScheduler::CKickBack::~CKickBack()
	{
	Cancel();
	iWservThread.Close();
	iIdleThread.Kill(0);
	iIdleThread.Close();
	}
	
TInt CWindowServer::CDefaultAnimationScheduler::CKickBack::IdleThreadFunc(TAny* aAny)
	{
	CKickBack* owner = reinterpret_cast<CKickBack*>(aAny);
	ASSERT(owner);
	if(owner)
		{
		owner->Loop();
		}
	return KErrNone;
	}

// If using the default animation scheduler on a device, these two numbers may be worth tweaking in the inifile
// However, both are maximum periods - wserv will go faster than either if nothing else is using the system.
const TInt64 CWindowServer::CDefaultAnimationScheduler::KRedrawGrace = 0; // do redraws immediately
const TInt64 CWindowServer::CDefaultAnimationScheduler::KAnimationGrace = 35000; // insist upon 35ms grace for other threads to run when animating
const TInt64 CWindowServer::CDefaultAnimationScheduler::KDsaAnimationGrace = 35000; // insist upon 35ms grace for other threads to run when animating during DSA
	
CWindowServer::CDefaultAnimationScheduler::CDefaultAnimationScheduler(MWsGraphicDrawerEnvironment& aEnv):
	iEnv(aEnv), iSchedule(1,_FOFF(TSchedule,iScreen))
	{
	}
	
CWindowServer::CDefaultAnimationScheduler::~CDefaultAnimationScheduler()
	{
	iSchedule.Close();
	iInvalidated.Close();
	delete iKickBack;
	delete iIdleInitiator;
	delete iTick;
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
	
	_LIT(KDsaAnimationGracePeriod, "DSAANIMATIONGRACEPERIOD");
	tmp = KDsaAnimationGrace;
	WsIniFile->FindVar(KDsaAnimationGracePeriod, tmp);
	iDsaAnimationGracePeriod = tmp;
	
	iIdleInitiator = new(ELeave) CAsyncCallBack(TCallBack(OnIdleCallBack,this),EWsGraphicAnimateAwaitIdlePriority);
	iTick = CPeriodic::NewL(EWsGraphicAnimatePriority);
	
	_LIT(KDisableIdleAnimation, "DISABLEIDLEANIMATION");
	if (!WsIniFile->FindVar(KDisableIdleAnimation))
		{
		iKickBack = new CKickBack(TCallBack(OnKickBack,this));
		iKickBack->ConstructL();
		}
	}
	
void CWindowServer::CDefaultAnimationScheduler::Invalidate(const TGraphicDrawerId& aId)
	{
	if(!iInvalidateAll)
		{
		switch(iInvalidated.InsertInOrder(aId,TLinearOrder<TGraphicDrawerId>(TGraphicDrawerId::Compare)))
			{
			case KErrNone:
				break;
			case KErrAlreadyExists:
				break;
			default:
				iInvalidateAll = ETrue;
				iInvalidated.Reset();
			}
		}
	iIdleInitiator->CallBack();
	}

void CWindowServer::CDefaultAnimationScheduler::OnInactive()
	{
	iInactive = ETrue;
	}
	
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
	
void CWindowServer::CDefaultAnimationScheduler::DoRedrawNow(MWsScreen& /*aScreen*/)
	{
	OnIdleCallBack(ETrue);
	}
	
void CWindowServer::CDefaultAnimationScheduler::DoRedrawNow(MWsScreen& /*aScreen*/, MWsAnimationScheduler::MScreenUpdateObserver& aObserver)
	{
	OnIdleCallBack(ETrue);
	aObserver.ScreenUpdateComplete(KErrNone);
	}	
	
void CWindowServer::CDefaultAnimationScheduler::ScheduleAnimation(MWsScreen& aScreen,const TTime& aWhen)
	{
	TSchedule schedule;
	schedule.iScreen = &aScreen;
	schedule.iSchedule = ETrue;
	schedule.iWhen = aWhen;
	schedule.iGeneration = iGeneration;
	TBool ok = EFalse;
	const TInt idx = iSchedule.FindInUnsignedKeyOrder(schedule);
	if(0 <= idx)
		{
		if(iSchedule[idx].iSchedule)
			{
			if(iSchedule[idx].iWhen > aWhen)
				{
				iSchedule[idx].iWhen = aWhen;
				}
			}
		else
			{
			iSchedule[idx] = schedule;
			}
		iSchedule[idx].iGeneration = iGeneration;
		ok = ETrue;
		}
	else
		{
		ok = (KErrNone == iSchedule.InsertInUnsignedKeyOrder(schedule));
		}
	if(ok)
		{
		//If the animation runs at very high rate which exceeds the rate WSERV can 
		//perform the rendering, it is possible that WSERV animation loop will monopolize  
		//processor time. 
  		User::After(0);	// to yeild from the animation loop 

		iIdleInitiator->CallBack();
		}
	}

void CWindowServer::CDefaultAnimationScheduler::UnscheduleAnimation(MWsScreen& aScreen)
	{
	TSchedule schedule;
	schedule.iScreen = &aScreen;
	const TInt idx = iSchedule.FindInUnsignedKeyOrder(schedule);
	if(0 <= idx)
		{
		iSchedule[idx].iSchedule = EFalse;
		}
	}

TBool CWindowServer::CDefaultAnimationScheduler::OnIdleCallBack(TAny* aAny)
	{
	CDefaultAnimationScheduler* self = reinterpret_cast<CDefaultAnimationScheduler*>(aAny);
	ASSERT(self);
	if(self)
		{
		self->OnIdleCallBack(EFalse);
		}
	return EFalse; //ignored by caller
	}
	
void CWindowServer::CDefaultAnimationScheduler::OnIdleCallBack(TBool aForceRedraw)
	{
	TBool wasActive = EFalse;
	if (iTick->IsActive())
		{
		wasActive = ETrue;
		iTick->Cancel(); // stop ticker, as we'll reschedule if necessary
		}
	if (aForceRedraw)
		{
		// Don't need to update iExpectedTime as we will not schedule a tick.
		// Don't need to update iWhenDesired as we will not schedule a kick-back.
		OnTickCallBack();
		return;
		}
	TBool tick = (iInvalidateAll || iInvalidated.Count());
	TTimeIntervalMicroSeconds next = 0LL;
	const TInt count = iSchedule.Count();
	TTime now;
	now.UniversalTime();
	if(count)
		{
		// work out the next wanted tick
		TBool animTick = EFalse;
		for(TInt i=0; i<count; i++)
			{
			if(iSchedule[i].iSchedule)
				{
				const TTime when = iSchedule[i].iWhen;
				const TTimeIntervalMicroSeconds fromNow = when.MicroSecondsFrom(now);
				if(!animTick || (fromNow < next))
					{
					animTick = ETrue;
					tick = ETrue;
					next = fromNow;
					iWhenDesired = when;
					}
				}
			}
		}
	if(tick)
		{
		TInt64 grace;
		if (iRedrawScheduled)
			grace = iRedrawGracePeriod;
		else if (EFalse) // DSA active
			grace = iDsaAnimationGracePeriod;
		else
			grace = iAnimationGracePeriod;
		if (wasActive)
			{
			TInt64 minimum = iExpectedTime.MicroSecondsFrom(now).Int64();
			if (minimum >= 0 && grace > minimum)
				grace = minimum;
			}

		if (next.Int64() <= 0)
			next = 0LL ;  // No kickback/tick is needed. Make sure next == default grace period.

		if(next < grace)
			{
			next = grace;
			if (iKickBack)
				iKickBack->RequestKickBack();
			}
		else if (next > KHalfHour)
			next = KHalfHour;
		
		iExpectedTime=now + next;
		if (next.Int64() > 0)
			{
			iTick->Start(next.Int64(),0,TCallBack(OnTickCallBack,this));
			}
		else
			{
			OnTickCallBack(); // scheduling for 0 doesn't actually execute immediately
			}
		}
	}
	
TBool CWindowServer::CDefaultAnimationScheduler::OnKickBack(TAny* aAny)
	{
	CDefaultAnimationScheduler* self = reinterpret_cast<CDefaultAnimationScheduler*>(aAny);
	ASSERT(self);
	if(self)
		{
		self->OnKickBack();
		}
	return EFalse; //ignored by caller
	}
	
void CWindowServer::CDefaultAnimationScheduler::OnKickBack()
	{
	if (iTick->IsActive())
		{
		iTick->Cancel();
		TTime now;
		now.UniversalTime();
				
		TTimeIntervalMicroSeconds fromNow = iWhenDesired.MicroSecondsFrom(now);

		if (fromNow < 0)
			{
			OnTickCallBack();
			}
		else
			{
			if (fromNow > KHalfHour)
				fromNow = KHalfHour;
			iTick->Start(fromNow.Int64(),0,TCallBack(OnTickCallBack, this));
			}
		}
	}
	
TBool CWindowServer::CDefaultAnimationScheduler::OnTickCallBack(TAny* aAny)
	{
	CDefaultAnimationScheduler* self = reinterpret_cast<CDefaultAnimationScheduler*>(aAny);
	ASSERT(self);
	if(self)
		{
		self->OnTickCallBack();
		}
	return EFalse;
	}
	
void CWindowServer::CDefaultAnimationScheduler::OnTickCallBack()
	{
	iTick->Cancel();

	switch(iInactivityBehaviour)
		{
		case EStopAnimation :
			// only client redraws if inactive. server side drawing stopped.
			if(iInactive && !iRedrawScheduled) 
				{
				iInactiveDraws = ETrue;
				return;
				}
			break;
		case EStopAllDrawing :
			// if screen off, stop both client and server side drawing.
			if(iInactive) 
				{
				iInactiveDraws = ETrue;
				return;
				}
			break;
		case EIgnore :
		default :
			// ignore inactivity and draw as normal
			break;
		}
		
	iRedrawScheduled = EFalse;

	TTime now;
	now.UniversalTime();
	CWsActiveScheduler::Static()->AccumReclaimedIdleTime(Max(0LL,now.MicroSecondsFrom(iExpectedTime).Int64()));
		
	TInt drewCount = 0;
	TInt scheduledCount = 0;
	/* first redraw any screens that are affected by invalidated graphic IDs */
	if(iInvalidateAll || iInvalidated.Count())
		{
		// cancel idle callback if it's already scheduled as we're going to redraw all invalidated 
		// request at this point and clear invalidated array
		if (iIdleInitiator->IsActive())
			iIdleInitiator->Cancel();
		const TArray<TGraphicDrawerId> invalidArray = iInvalidated.Array();
		const TInt screenCount = iEnv.ScreenCount();
		for(TInt i=0; i<screenCount; i++)
			{
			MWsScreen* screen = iEnv.Screen(i);
			if(screen)
				{
				drewCount++;
				if(iInvalidateAll)
					{
					Redraw(*screen);
					}
				else
					{
					RedrawInvalid(*screen,invalidArray);
					}
				}
			}
		iInvalidateAll = EFalse;
		iInvalidated.Reset();
		}
	/* iSchedule might resize during this call because an Animate() on a screen might reschedule another animation etc */
	TInt generation = iGeneration++;
	TBool drew;
	do
		{
		drew = EFalse;
		const TInt count = iSchedule.Count();
		for(TInt i=0; i<count; i++)
			{
			if(iSchedule[i].iSchedule)
				{
				scheduledCount++;
				if(iSchedule[i].iGeneration == generation)
					{
					iSchedule[i].iSchedule = EFalse; // denote not to tick; it can be rescheduled in the subsequent call to Animate()
					Animate(*(iSchedule[i].iScreen));
					drew = ETrue;
					drewCount++;
					break; // go back to outer do while(drew)
					}
				}
			}
		} while(drew);
	__DEBUG_ONLY(
		TBool idleIsActive = iIdleInitiator->IsActive();
		if(scheduledCount || !drewCount)
			{
			TBool rescheduled = EFalse;
			for(TInt i=0; i<iSchedule.Count(); i++)
				{
				rescheduled |= iSchedule[i].iSchedule;
				}
			if(!rescheduled)
				{
				//__DEBUGGER();
				}
			})
	}

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
	: CPolicyServer(EMainServerPriority,KWsServPolicy)
	{
	}

CWindowServer::~CWindowServer()
	{
	iMemoryReleases.Reset();
	WS_ASSERT_DEBUG(iDrawerMasterIndex.IsEmpty(), EWsPanicWsGraphic);
	iDrawerMasterIndex.Close();
	
	delete iDefaultAnimationScheduler;
	iDefaultAnimationScheduler = NULL; // might be called from clients during server destruction
	delete iPluginManager;
	}
	
void CWindowServer::ConstructL()
	{
	iPluginManager = CWsPluginManager::NewL(*this);
	iDefaultAnimationScheduler = new(ELeave) CDefaultAnimationScheduler(*this);
	iDefaultAnimationScheduler->ConstructL();
	RegisterMemoryRelease(this);
	}

/** Creates a new client for this server. */
CSession2* CWindowServer::NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const
	{
	TVersion v(KWservMajorVersionNumber,KWservMinorVersionNumber,KWservBuildVersionNumber);
	if (User::QueryVersionSupported(v,aVersion)==EFalse)
		User::Leave(KErrNotSupported);
	RThread thread;
	User::LeaveIfError(aMessage.Client(thread));
	return(new(ELeave) CWsClient(thread));
	}

TInt CWindowServer::SessionCount()
	{
	iSessionIter.SetToFirst();
	TInt count=0;
	while(iSessionIter++)
		count++;
	return(count);
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
	
TInt CWindowServer::RegisterRawEventHandler(MEventHandler* aHandler)
	{
	if (!aHandler)
		return KErrArgument;
	TRAPD(err, TWindowServerEvent::PotentialEventHandlerL(1));
	if (err != KErrNone)
		return err;
	TWindowServerEvent::AddEventHandler(aHandler);
	return KErrNone;
	}
	
void CWindowServer::UnregisterRawEventHandler(MEventHandler* aHandler)
	{
	TWindowServerEvent::RemoveEventHandler(aHandler);
	TWindowServerEvent::PotentialEventHandlerL(-1); // can't leave for -1
	}

void CWindowServer::PostRawEvent(const TRawEvent & aEvent)
	{
	TWindowServerEvent::ProcessRawEvent(aEvent);
	}
	
void CWindowServer::PostKeyEvent(const TKeyEvent & aEvent)
	{
	TWindowServerEvent::ProcessKeyEvent(aEvent, 0);
	}
	
TAny* CWindowServer::ResolveObjectInterface(TUint aTypeId)
	{
	switch(aTypeId)
		{
		case MWsActiveSchedulerDebug::EWsObjectInterfaceId:
			return static_cast<MWsActiveSchedulerDebug*>(CWsActiveScheduler::Static());
		case MWsIniFile::EWsObjectInterfaceId:
			return static_cast<MWsIniFile*>(WsIniFile);
		case MWsRawEventServer::EWsObjectInterfaceId:
			return static_cast<MWsRawEventServer*>(this);
		}
	
	if (iPluginManager)
		return iPluginManager->ResolveObjectInterface(aTypeId);
	
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
	iDefaultAnimationScheduler->RemoveGraphicDrawer(aId);
	return iDrawerMasterIndex.Remove(aId);
	}

TInt CWindowServer::RemoveAllGraphicDrawers(const MWsClient& aOwner)
	{
	return iDrawerMasterIndex.RemoveAll(aOwner);
	}

CWsPluginManager* CWindowServer::PluginManager()
	{
	return iPluginManager;
	}

TInt CWindowServer::RegisterMemoryRelease(MWsMemoryRelease * aMemoryRelease)
	{
	return iMemoryReleases.Append(aMemoryRelease);
	}

void CWindowServer::UnregisterMemoryRelease(MWsMemoryRelease * aMemoryRelease)
	{
	for (TInt i = iMemoryReleases.Count() - 1; i >= 0; --i)
		{
		if (iMemoryReleases[i] == aMemoryRelease)
			{
			iMemoryReleases.Remove(i);
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
		for (TInt i = iMemoryReleases.Count() - 1; !released && i >= 0; --i)
			{
			released = iMemoryReleases[i]->ReleaseMemory(static_cast<TMemoryReleaseLevel>(level));
			}
		}
	return released;
	}

