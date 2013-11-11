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
#include "roleswitchhelper.h"
#include "physicallinksmanager.h"
#include "AclDataQController.h"
#include "ProxySAP.h"
#include "linkmgr.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

CRoleSwitcher::CRoleSwitcher(CPhysicalLinksManager& aLinkMgr, CPhysicalLink& aLink, TBTBasebandRole aRole)
	: CPhysicalLinkHelper(aLinkMgr, aLink)
	, iRole(aRole)
	, iIsEncryptionDisabledForRoleSwitch(EFalse)
	{
	LOG_FUNC
	iState = &iLinkMgr.RoleSwitcherStateFactory().GetState(CRoleSwitcherStateFactory::EIdle);
	}

CRoleSwitcher* CRoleSwitcher::NewL(CPhysicalLinksManager& aLinkMgr, CPhysicalLink& aLink, TBTBasebandRole aRole)
	{
	LOG_STATIC_FUNC
	CRoleSwitcher* self = new(ELeave) CRoleSwitcher(aLinkMgr, aLink, aRole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CRoleSwitcher::ConstructL()
	{
	LOG_FUNC
	
	BaseConstructL();
	
	// add ourselves to the list in LinkMgr, LinkMgr will kick off the role change state machine 	
	iLinkMgr.AddRoleSwitcher(*this);
	iAddedToLinkMgr = ETrue;
	}
	
CRoleSwitcher::~CRoleSwitcher()
	{
	LOG_FUNC
	if (iAddedToLinkMgr)
		{
		iLinkMgr.RemoveRoleSwitcher(*this);
		}
	}

void CRoleSwitcher::TimerExpired()
	{
	LOG_FUNC
	
	iState->TimerExpired(*this);
	}

void CRoleSwitcher::HandleError( TInt aError)
	{
	LOG_FUNC
	
	iState->Error(*this, aError);
	}

void CRoleSwitcher::DisableEncryption()
  	{
	LOG_FUNC
 	// data traffic suspended
	iLinkMgr.LinkManagerProtocol().ACLController().SetParked(iLink.Handle(), ETrue);
	NotifyBasebandEvent(ENotifyEncryptionChangeOff);
	iLinkMgr.Encrypt(EFalse, iLink);

	// set flag here, it's too late when we receive the event as AccessReqester
	// might receive the baseband notification earlier then the flag is set!	
	iIsEncryptionDisabledForRoleSwitch = ETrue;
  	}

void CRoleSwitcher::EnableEncryption()
  	{
	LOG_FUNC  	
	NotifyBasebandEvent(ENotifyEncryptionChangeOn);
	iLinkMgr.Encrypt(ETrue, iLink);	
	// data traffic is enabled in IoctlComplete
  	}
   	
void CRoleSwitcher::ChangeRole()
	{
	LOG_FUNC
	NotifyBasebandEvent(ENotifyAnyRole);
	iLinkMgr.ChangeRole(iRole, iLink);
	}

void CRoleSwitcher::StartHelper()
	{
	LOG_FUNC
	iState->Start(*this);
	}
		
void CRoleSwitcher::SaveEncryption()
	{
	LOG_FUNC
	iIsEncrypted = iLink.Encrypted();
	}

void CRoleSwitcher::LogRoleSwitchSuccessful(TBTBasebandEventNotification& aEvent) const
	{
	LOG_FUNC
	TInt eventType;
	eventType = (iRole == EMaster ? ENotifyMaster :ENotifySlave);
	
	if (aEvent.EventType()==eventType && aEvent.ErrorCode()==KErrNone)
		{
		LOG(_L("CRoleSwitcher RoleSwitch OK"));	
		}
	else 
		{
		LOG(_L("CRoleSwitcher RoleSwitch failed"));
		}
	}
	
void CRoleSwitcher::EventReceived(TBTBasebandEventNotification& aEvent)
	{
	LOG_FUNC
	iState->EventReceived(*this, aEvent);
	}
		

//----------------------------------------------------------------------------------
// STATE FACTORY
//----------------------------------------------------------------------------------

CRoleSwitcherStateFactory* CRoleSwitcherStateFactory::NewL()
	{
	LOG_STATIC_FUNC
	CRoleSwitcherStateFactory* ret=new (ELeave) CRoleSwitcherStateFactory();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

void CRoleSwitcherStateFactory::ConstructL()
	{
	LOG_FUNC	
	iStates[EIdle]					=new (ELeave) TRSStateIdle(*this);
	iStates[EDisablingLPM]			=new (ELeave) TRSStateDisablingLPM(*this);
	iStates[EDisablingEncryption]	=new (ELeave) TRSStateDisablingEncryption(*this);
	iStates[EChangingRole]			=new (ELeave) TRSStateChangingRole(*this);
	iStates[EChangingRoleWithEPR]	=new (ELeave) TRSStateChangingRoleWithEPR(*this);
	iStates[EEnablingEncryption]	=new (ELeave) TRSStateEnablingEncryption(*this);
	}

CRoleSwitcherStateFactory::CRoleSwitcherStateFactory()
	{
	LOG_FUNC
	iStates.DeleteAll();
	}

TRoleSwitcherState& CRoleSwitcherStateFactory::GetState(CRoleSwitcherStateFactory::TRoleSwitcherStates aState)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStates[aState],  Panic(ERoleSwitcherInvalidState));
	return *iStates[aState];
	}

TInt CRoleSwitcherStateFactory::StateIndex(const TRoleSwitcherState* aState) const
	{
	LOG_FUNC
	TInt state;
	for (state = 0; state < ERoleSwitcherMaxState; state++)
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

TRoleSwitcherState::TRoleSwitcherState(CRoleSwitcherStateFactory& aFactory)
: iFactory(aFactory)
	{
	LOG_FUNC
	}

void TRoleSwitcherState::PanicInState(TLinkPanic aPanic) const
	{
	LOG_FUNC
	Panic(aPanic, iFactory.StateIndex(this));
	}

void TRoleSwitcherState::ChangeState(CRoleSwitcher& aContext, CRoleSwitcherStateFactory::TRoleSwitcherStates aState) const
	{
	LOG_FUNC
	
	aContext.iState->Exit(aContext);

#ifdef __FLOG_ACTIVE
	TRoleSwitcherState* state=&iFactory.GetState(aState);
	LOG2(_L("RoleSwitcher: State %S -> %S"), &aContext.iState->iName, &state->iName);
#endif //__FLOG_ACTIVE
	aContext.iState=&iFactory.GetState(aState);

	aContext.iState->Enter(aContext);
	}

void TRoleSwitcherState::Enter(CRoleSwitcher& /*aContext*/) const
	{
	LOG_FUNC
	// do nothing
	}

void TRoleSwitcherState::Exit(CRoleSwitcher& /*aContext*/) const
	{
	LOG_FUNC
	// do nothing
	}

void TRoleSwitcherState::Start(CRoleSwitcher& /*aContext*/) const
	{
	LOG_FUNC
	PanicInState(ERoleSwitcherStateMachineInvalidEvent);
	}

void TRoleSwitcherState::Error(CRoleSwitcher& aContext, TInt /*aErr*/) const
	{
	LOG_FUNC
	aContext.CancelNotify();
	aContext.RemoveTimer();
	ChangeState(aContext, CRoleSwitcherStateFactory::EIdle);
	}

void TRoleSwitcherState::EventReceived(CRoleSwitcher& /*aContext*/, TBTBasebandEventNotification& /*aEvent*/) const
	{
	LOG_FUNC
	// do nothing
	}

void TRoleSwitcherState::TimerExpired(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	ChangeState(aContext, CRoleSwitcherStateFactory::EIdle);
	}

//----------------------------------------------------------------------------------

TRSStateIdle::TRSStateIdle(CRoleSwitcherStateFactory& aFactory)
: TRoleSwitcherState(aFactory)
	{
	LOG_FUNC
	STATENAME("TRSStateIdle");
	}

void TRSStateIdle::Start(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	aContext.QueueTimer(KTimeoutRoleSwitch);	// watchdog timer
	ChangeState(aContext, CRoleSwitcherStateFactory::EDisablingLPM);
	}	

void TRSStateIdle::Enter(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	// async call to delete the helper
	aContext.iLink.AsyncDeleteRoleSwitcher();
	}	

//----------------------------------------------------------------------------------

TRSStateDisablingLPM::TRSStateDisablingLPM(CRoleSwitcherStateFactory& aFactory)
: TRoleSwitcherState(aFactory)
	{
	LOG_FUNC
	STATENAME("TRSStateDisablingLPM");
	}

void TRSStateDisablingLPM::Enter(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	// DisableLPM even if link is active to prevent possible LPM requests during encryption disabling

	if (aContext.iLink.LinkMode() == EActiveMode)
		{
		aContext.DisableLPM();
		if (aContext.IsEPRSupported())
			{
			ChangeState(aContext, CRoleSwitcherStateFactory::EChangingRoleWithEPR);
			}
		else
			{
			ChangeState(aContext, CRoleSwitcherStateFactory::EDisablingEncryption);
			}
		// don't wait for notification
		}
	else
		{
		aContext.NotifyBasebandEvent(ENotifyActiveMode);
		aContext.DisableLPM();
		}
	}

void TRSStateDisablingLPM::EventReceived(CRoleSwitcher& aContext, TBTBasebandEventNotification& aEvent) const
	{
	LOG_FUNC
	if (aEvent.EventType()==ENotifyActiveMode && aEvent.ErrorCode()==KErrNone)
		{
		if (aContext.IsEPRSupported())
			{
			ChangeState(aContext, CRoleSwitcherStateFactory::EChangingRoleWithEPR);
			}
		else
			{
			ChangeState(aContext, CRoleSwitcherStateFactory::EDisablingEncryption);
			}
		}
	else 
		{
		LOG(_L("CRoleSwitcher RoleSwitch failed in DisableLPM"));
		// we can quit SM, don't need to rewind
		ChangeState(aContext, CRoleSwitcherStateFactory::EIdle);
		}
	}

//----------------------------------------------------------------------------------
TRSStateDisablingEncryption::TRSStateDisablingEncryption(CRoleSwitcherStateFactory& aFactory)
: TRoleSwitcherState(aFactory)
	{
	LOG_FUNC
	STATENAME("TRSStateDisablingEncryption");
	}

void TRSStateDisablingEncryption::Enter(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	aContext.SaveEncryption();
	if (aContext.iIsEncrypted)
		{
		aContext.DisableEncryption();
		}
	else
		{
		ChangeState(aContext, CRoleSwitcherStateFactory::EChangingRole);
		}
	}

void TRSStateDisablingEncryption::EventReceived(CRoleSwitcher& aContext, TBTBasebandEventNotification& aEvent) const
	{
	LOG_FUNC
	if (aEvent.EventType()==ENotifyEncryptionChangeOff && aEvent.ErrorCode()==KErrNone)
		{
		ChangeState(aContext, CRoleSwitcherStateFactory::EChangingRole);
		}
	else 
		{
		LOG(_L("CRoleSwitcher RoleSwitch failed in DisableEncryption"));
		// before quiting SM , try to enable LPM
		ChangeState(aContext, CRoleSwitcherStateFactory::EIdle);
		}
	}

void TRSStateDisablingEncryption::TimerExpired(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	aContext.CancelNotify();
	ChangeState(aContext, CRoleSwitcherStateFactory::EEnablingEncryption);
	}

//----------------------------------------------------------------------------------
TRSStateChangingRole::TRSStateChangingRole(CRoleSwitcherStateFactory& aFactory)
: TRoleSwitcherState(aFactory)
	{
	LOG_FUNC
	STATENAME("TRSStateChangingRole");
	}

void TRSStateChangingRole::Enter(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	aContext.ChangeRole();
	}

void TRSStateChangingRole::EventReceived(CRoleSwitcher& aContext, TBTBasebandEventNotification& __DEBUG_ONLY(aEvent)) const
	{
	LOG_FUNC
	aContext.RemoveTimer();	// cancel watchdog timer

	FTRACE(aContext.LogRoleSwitchSuccessful(aEvent));
	

	ChangeState(aContext, CRoleSwitcherStateFactory::EEnablingEncryption);
	}

void TRSStateChangingRole::TimerExpired(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	aContext.CancelNotify();
	ChangeState(aContext, CRoleSwitcherStateFactory::EEnablingEncryption);
	}

//----------------------------------------------------------------------------------
TRSStateChangingRoleWithEPR::TRSStateChangingRoleWithEPR(CRoleSwitcherStateFactory& aFactory)
: TRoleSwitcherState(aFactory)
	{
	LOG_FUNC
	STATENAME("TRSStateChangingRoleWithEPR");
	}

void TRSStateChangingRoleWithEPR::Enter(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	aContext.ChangeRole();
	}

void TRSStateChangingRoleWithEPR::EventReceived(CRoleSwitcher& aContext, TBTBasebandEventNotification& __DEBUG_ONLY(aEvent)) const
	{
	LOG_FUNC
	aContext.RemoveTimer();	// cancel watchdog timer

	FTRACE(aContext.LogRoleSwitchSuccessful(aEvent));
		
	ChangeState(aContext, CRoleSwitcherStateFactory::EIdle);
	}

void TRSStateChangingRoleWithEPR::TimerExpired(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	aContext.CancelNotify();
	ChangeState(aContext, CRoleSwitcherStateFactory::EIdle);
	}

//----------------------------------------------------------------------------------
TRSStateEnablingEncryption::TRSStateEnablingEncryption(CRoleSwitcherStateFactory& aFactory)
: TRoleSwitcherState(aFactory)
	{
	LOG_FUNC
	STATENAME("TRSStateEnablingEncryption");
	}

void TRSStateEnablingEncryption::Enter(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	if (aContext.iIsEncrypted)
		{
		aContext.QueueTimer(KTimeoutOneCommand);
		aContext.EnableEncryption();
		}
	else
		{
		ChangeState(aContext, CRoleSwitcherStateFactory::EIdle);
		}
	}
	
void TRSStateEnablingEncryption::Exit(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	if (aContext.iIsEncrypted)
		{
		// enable data traffic
		aContext.iLinkMgr.LinkManagerProtocol().ACLController().SetParked(aContext.iLink.Handle(), EFalse);
		}
	}

void TRSStateEnablingEncryption::EventReceived(CRoleSwitcher& aContext, TBTBasebandEventNotification& aEvent) const
	{
	LOG_FUNC
	aContext.RemoveTimer(); // watchdog timer
	if (aEvent.EventType()==ENotifyEncryptionChangeOn && aEvent.ErrorCode()==KErrNone)
		{	
		ChangeState(aContext, CRoleSwitcherStateFactory::EIdle);
		aContext.iIsEncryptionDisabledForRoleSwitch = EFalse;
		}
	else 
		{
		LOG(_L("CRoleSwitcher SetEncryption failed, disconnect link"));
		if (aContext.iLink.Terminate(ERemoteUserEndedConnection) != KErrNone) 
			{
			LOG(_L("CRoleSwitcher OOM"));
			}
		ChangeState(aContext, CRoleSwitcherStateFactory::EIdle);
		}
	}

void TRSStateEnablingEncryption::TimerExpired(CRoleSwitcher& aContext) const
	{
	LOG_FUNC
	LOG(_L("CRoleSwitcher Timeout in EncryptionEnable, disconnect"));
	aContext.CancelNotify();			
	if (aContext.iLink.Terminate(ERemoteUserEndedConnection) != KErrNone)
			{
			LOG(_L("CRoleSwitcher OOM"));
			}
	ChangeState(aContext, CRoleSwitcherStateFactory::EIdle);
	}



