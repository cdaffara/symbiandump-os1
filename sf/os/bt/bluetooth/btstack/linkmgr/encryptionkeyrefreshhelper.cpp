// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>

#include "encryptionkeyrefreshhelper.h"
#include "hcifacade.h"
#include "linkmgr.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

CEncryptionKeyRefresher::CEncryptionKeyRefresher(CPhysicalLinksManager& aLinkMgr, CPhysicalLink& aLink)
	: CPhysicalLinkHelper(aLinkMgr, aLink)
	{
	LOG_FUNC
	}

CEncryptionKeyRefresher* CEncryptionKeyRefresher::NewL(CPhysicalLinksManager& aLinkMgr, CPhysicalLink& aLink)
	{
	LOG_STATIC_FUNC
	CEncryptionKeyRefresher* self = new(ELeave) CEncryptionKeyRefresher(aLinkMgr, aLink);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CEncryptionKeyRefresher::ConstructL()
	{
	LOG_FUNC
	
	BaseConstructL();
	
	iStateFactory = CEncryptionKeyRefresherStateFactory::NewL();
	iState = &(iStateFactory->GetState(CEncryptionKeyRefresherStateFactory::EIdle));
	}
	
CEncryptionKeyRefresher::~CEncryptionKeyRefresher()
	{
	LOG_FUNC
	}

void CEncryptionKeyRefresher::EncryptionKeyRefreshComplete(TInt aError)
	{
	LOG_FUNC
	iState->EncryptionKeyRefreshComplete(*this, aError);
	}

void CEncryptionKeyRefresher::TimerExpired()
	{
	LOG_FUNC
	iState->TimerExpired(*this);
	}

void CEncryptionKeyRefresher::HandleError( TInt aError)
	{
	LOG_FUNC
	iState->Error(*this, aError);
	}

void CEncryptionKeyRefresher::StartHelper()
	{
	LOG_FUNC
	iState->Start(*this);
	}
	
void CEncryptionKeyRefresher::EventReceived(TBTBasebandEventNotification& aEvent)
	{
	LOG_FUNC
	iState->EventReceived(*this, aEvent);
	}
		

//----------------------------------------------------------------------------------
// STATE FACTORY
//----------------------------------------------------------------------------------

CEncryptionKeyRefresherStateFactory* CEncryptionKeyRefresherStateFactory::NewL()
	{
	LOG_STATIC_FUNC
	CEncryptionKeyRefresherStateFactory* ret=new (ELeave) CEncryptionKeyRefresherStateFactory();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

void CEncryptionKeyRefresherStateFactory::ConstructL()
	{
	LOG_FUNC	
	iStates[EIdle]					=new (ELeave) TEkrStateIdle(*this);
	iStates[EDisablingLPM]			=new (ELeave) TEkrStateDisablingLPM(*this);
	iStates[ERefreshingKey]			=new (ELeave) TEkrStateRefreshingKey(*this);
	}

CEncryptionKeyRefresherStateFactory::CEncryptionKeyRefresherStateFactory()
	{
	LOG_FUNC
	iStates.DeleteAll();
	}

const TEncryptionKeyRefresherState& CEncryptionKeyRefresherStateFactory::GetState(CEncryptionKeyRefresherStateFactory::TEncryptionKeyRefresherStates aState) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStates[aState],  Panic(EEncryptionKeyRefresherInvalidState));
	return *iStates[aState];
	}

TInt CEncryptionKeyRefresherStateFactory::StateIndex(const TEncryptionKeyRefresherState* aState) const
	{
	LOG_FUNC
	TInt state;
	for (state = 0; state < EEncryptionKeyRefresherMaxState; state++)
		{
		if (iStates[state] == aState)
			{
			return state;
			}
		}
	
	return KUnknownState;
	}


//----------------------------------------------------------------------------------
// STATES
//----------------------------------------------------------------------------------

TEncryptionKeyRefresherState::TEncryptionKeyRefresherState(CEncryptionKeyRefresherStateFactory& aFactory)
: iFactory(aFactory)
	{
	LOG_FUNC
	}

void TEncryptionKeyRefresherState::PanicInState(TLinkPanic aPanic) const
	{
	LOG_FUNC
	Panic(aPanic, iFactory.StateIndex(this));
	}

void TEncryptionKeyRefresherState::ChangeState(CEncryptionKeyRefresher& aContext, CEncryptionKeyRefresherStateFactory::TEncryptionKeyRefresherStates aState) const
	{
	LOG_FUNC
	
	aContext.iState->Exit(aContext);

#ifdef __FLOG_ACTIVE
	const TEncryptionKeyRefresherState* state=&iFactory.GetState(aState);
	LOG2(_L("EncryptionKeyRefresher: State %S -> %S"), &aContext.iState->iName, &state->iName);
#endif //__FLOG_ACTIVE
	aContext.iState=&iFactory.GetState(aState);

	aContext.iState->Enter(aContext);
	}

void TEncryptionKeyRefresherState::Enter(CEncryptionKeyRefresher& /*aContext*/) const
	{
	LOG_FUNC
	// do nothing
	}

void TEncryptionKeyRefresherState::Exit(CEncryptionKeyRefresher& /*aContext*/) const
	{
	LOG_FUNC
	// do nothing
	}

void TEncryptionKeyRefresherState::Start(CEncryptionKeyRefresher& /*aContext*/) const
	{
	LOG_FUNC
	PanicInState(EEncryptionKeyRefresherStateMachineInvalidEvent);
	}

void TEncryptionKeyRefresherState::Error(CEncryptionKeyRefresher& aContext, TInt /*aErr*/) const
	{
	LOG_FUNC
	aContext.CancelNotify();
	aContext.RemoveTimer();
	ChangeState(aContext, CEncryptionKeyRefresherStateFactory::EIdle);
	}

void TEncryptionKeyRefresherState::EventReceived(CEncryptionKeyRefresher& /*aContext*/, TBTBasebandEventNotification& /*aEvent*/) const
	{
	LOG_FUNC
	// do nothing
	}

void TEncryptionKeyRefresherState::TimerExpired(CEncryptionKeyRefresher& aContext) const
	{
	LOG_FUNC
	aContext.CancelNotify();
	ChangeState(aContext, CEncryptionKeyRefresherStateFactory::EIdle);
	}

void TEncryptionKeyRefresherState::EncryptionKeyRefreshComplete(CEncryptionKeyRefresher& /*aContext*/, TInt /*aError*/) const
	{
	LOG_FUNC
	// do nothing
	}

//----------------------------------------------------------------------------------

TEkrStateIdle::TEkrStateIdle(CEncryptionKeyRefresherStateFactory& aFactory)
: TEncryptionKeyRefresherState(aFactory)
	{
	LOG_FUNC
	STATENAME("TEkrStateIdle");
	}

void TEkrStateIdle::Start(CEncryptionKeyRefresher& aContext) const
	{
	LOG_FUNC
	aContext.QueueTimer(KTimeoutEncryptionKeyRefresh);	// watchdog timer
	ChangeState(aContext, CEncryptionKeyRefresherStateFactory::EDisablingLPM);
	}	

void TEkrStateIdle::Enter(CEncryptionKeyRefresher& aContext) const
	{
	LOG_FUNC
	// async call to delete the helper
	aContext.iLink.AsyncDeleteKeyRefresher();
	}	

//----------------------------------------------------------------------------------

TEkrStateDisablingLPM::TEkrStateDisablingLPM(CEncryptionKeyRefresherStateFactory& aFactory)
: TEncryptionKeyRefresherState(aFactory)
	{
	LOG_FUNC
	STATENAME("TEkrStateDisablingLPM");
	}

void TEkrStateDisablingLPM::Enter(CEncryptionKeyRefresher& aContext) const
	{
	LOG_FUNC
	
	if (aContext.iLink.LinkMode() == EActiveMode)
		{
		// Skip straight on to refresh the key
		ChangeState(aContext, CEncryptionKeyRefresherStateFactory::ERefreshingKey);
		}
	else
		{
		aContext.NotifyBasebandEvent(ENotifyActiveMode);
		}
	
	// DisableLPM even if link is active to prevent possible LPM requests during key refresh
	aContext.DisableLPM();
	}

void TEkrStateDisablingLPM::EventReceived(CEncryptionKeyRefresher& aContext, TBTBasebandEventNotification& aEvent) const
	{
	LOG_FUNC
	if (aEvent.EventType()==ENotifyActiveMode && aEvent.ErrorCode()==KErrNone)
		{
		ChangeState(aContext, CEncryptionKeyRefresherStateFactory::ERefreshingKey);
		}
	else 
		{
		LOG(_L("CEncryptionKeyRefresher failed in DisableLPM"));
		// If we can't put ourselves into active mode then don't bother with 
		// refresh, it'll probably cause more problems than it would solve
		// we can quit SM, don't need to rewind
		ChangeState(aContext, CEncryptionKeyRefresherStateFactory::EIdle);
		}
	}

//----------------------------------------------------------------------------------
TEkrStateRefreshingKey::TEkrStateRefreshingKey(CEncryptionKeyRefresherStateFactory& aFactory)
: TEncryptionKeyRefresherState(aFactory)
	{
	LOG_FUNC
	STATENAME("TEkrStateRefreshingKey");
	}

void TEkrStateRefreshingKey::Enter(CEncryptionKeyRefresher& aContext) const
	{
	LOG_FUNC
	
	TRAPD(err, aContext.iLinkMgr.HCIFacade().RefreshEncryptionKeyL(aContext.iLink.Handle()));
	
	if(err)
		{
		ChangeState(aContext, CEncryptionKeyRefresherStateFactory::EIdle);
		}
	}

void TEkrStateRefreshingKey::EncryptionKeyRefreshComplete(CEncryptionKeyRefresher& aContext, TInt __DEBUG_ONLY(aError)) const
	{
	LOG_FUNC
	LOG1(_L("EncryptionKeyRefresh completed with error %d"), aError);

	// Don't really care what the error is, this is just a best effort service
	// anyway
	ChangeState(aContext, CEncryptionKeyRefresherStateFactory::EIdle);
	}

