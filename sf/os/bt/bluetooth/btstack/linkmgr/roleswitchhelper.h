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

#ifndef ROLESWITCHHELPER_H
#define ROLESWITCHHELPER_H

#include <bt_sock.h>
#include <bluetooth/hci/hciutil.h> 

#include "debug.h"
#include "linkutil.h"
#include "physicallinkhelper.h"
#include "physicallinks.h"

// watchdog for first half of the SM including:
// DisablingLPM, DisablingEncryption, RoleSwitch
const TUint KTimeoutRoleSwitch =   3000000;  // 3 s; 

// watchdog for EnablingEncryption
const TUint KTimeoutOneCommand =   2000000;  // 2 s;
 
class TRoleSwitcherState;

NONSHARABLE_CLASS(CRoleSwitcher) : public CPhysicalLinkHelper
	{
	friend class TRoleSwitcherState;
	friend class TRSStateIdle;
	friend class TRSStateDisablingLPM;
	friend class TRSStateDisablingEncryption;
	friend class TRSStateChangingRole;
	friend class TRSStateChangingRoleWithEPR;
	friend class TRSStateEnablingEncryption;

public:
	static CRoleSwitcher* NewL(CPhysicalLinksManager& aLinkMgr, CPhysicalLink& aLink, TBTBasebandRole aRole);
	~CRoleSwitcher();

	virtual void StartHelper();
	
	inline TBool IsEncryptionDisabledForRoleSwitch() const;
	inline TBTBasebandRole RequestedRole() const;

	TSglQueLink   iQLink;

private:
	CRoleSwitcher(CPhysicalLinksManager& aLinkMgr, CPhysicalLink& aLink, TBTBasebandRole aRole);
	void ConstructL(); 
	
	virtual void TimerExpired();
	virtual void HandleError(TInt aError);
	virtual void EventReceived(TBTBasebandEventNotification& aEvent);

	void DisableEncryption();
	void ChangeRole();
	void EnableEncryption();
	void SaveEncryption();
	
	void LogRoleSwitchSuccessful(TBTBasebandEventNotification& aEvent) const;

	TBool					iIsEncrypted;
	TBool					iIsActive;  // LinkMode
	TBool					iAddedToLinkMgr;
	TBTBasebandRole			iRole;  // Requested role    
	TRoleSwitcherState*		iState;
	TBool					iIsEncryptionDisabledForRoleSwitch;             
	};

//--------------------------------------------------
// STATE FACTORY
//--------------------------------------------------

/**
   Factory for the RoleSwitcher states

   The states are flyweight classes
**/
NONSHARABLE_CLASS(CRoleSwitcherStateFactory) : public CBase
	{
public:
	static CRoleSwitcherStateFactory* NewL();

	enum TRoleSwitcherStates
		{
		EIdle,
		EDisablingLPM,
		EDisablingEncryption,
		EChangingRole,
		EChangingRoleWithEPR,
		EEnablingEncryption,
		// Note that we don't need a state to enable LPM again
		// as this will happen implicitly when we close our 
		// proxy SAP.
	// *** keep next one last ***
		ERoleSwitcherMaxState,
		};
	
	TRoleSwitcherState& GetState(TRoleSwitcherStates aState);
	TInt StateIndex(const TRoleSwitcherState* aState) const;
	
private:
	CRoleSwitcherStateFactory();
	void ConstructL();
	TFixedArray<TRoleSwitcherState*, ERoleSwitcherMaxState> iStates;
	};



//--------------------------------------------------
// STATES, base 
//--------------------------------------------------

NONSHARABLE_CLASS(TRoleSwitcherState)
	{
public:
	TRoleSwitcherState(CRoleSwitcherStateFactory& aFactory);
	
	virtual void Enter(CRoleSwitcher& aContext) const;
	virtual void Exit(CRoleSwitcher& aContext) const;

	virtual void Start(CRoleSwitcher& aContext) const;
	virtual void EventReceived(CRoleSwitcher& aContext, TBTBasebandEventNotification& aEvent) const;
	virtual void Error(CRoleSwitcher& aContext, TInt aErr) const;
	virtual void TimerExpired(CRoleSwitcher& aContext) const;

protected:
	// Exits old state, sets the new state, and enters it.
	void ChangeState(CRoleSwitcher& aContext, CRoleSwitcherStateFactory::TRoleSwitcherStates aState) const;
	void PanicInState(TLinkPanic aPanic) const;
	
protected:
	CRoleSwitcherStateFactory& iFactory;
#ifdef __FLOG_ACTIVE
	TBuf<48> iName;
#endif
	};


//--------------------------------------------------
// STATES 
//--------------------------------------------------

NONSHARABLE_CLASS(TRSStateIdle) : public TRoleSwitcherState
	{
public:
	TRSStateIdle(CRoleSwitcherStateFactory& aFactory);

	virtual void Enter(CRoleSwitcher& aContext) const;
	virtual void Start(CRoleSwitcher& aContext) const;
	};


NONSHARABLE_CLASS(TRSStateDisablingLPM) : public TRoleSwitcherState
	{
public:
	TRSStateDisablingLPM(CRoleSwitcherStateFactory& aFactory);

	virtual void Enter(CRoleSwitcher& aContext) const;
	virtual void EventReceived(CRoleSwitcher& aContext, TBTBasebandEventNotification& aEvent) const;
	};

NONSHARABLE_CLASS(TRSStateDisablingEncryption) : public TRoleSwitcherState
	{
public:
	TRSStateDisablingEncryption(CRoleSwitcherStateFactory& aFactory);

	virtual void Enter(CRoleSwitcher& aContext) const;
	virtual void EventReceived(CRoleSwitcher& aContext, TBTBasebandEventNotification& aEvent) const;
	virtual void TimerExpired(CRoleSwitcher& aContext) const;
	};

NONSHARABLE_CLASS(TRSStateChangingRole) : public TRoleSwitcherState
	{
public:
	TRSStateChangingRole(CRoleSwitcherStateFactory& aFactory);

	virtual void Enter(CRoleSwitcher& aContext) const;
	virtual void EventReceived(CRoleSwitcher& aContext, TBTBasebandEventNotification& aEvent) const;
	virtual void TimerExpired(CRoleSwitcher& aContext) const;
	};

NONSHARABLE_CLASS(TRSStateChangingRoleWithEPR) : public TRoleSwitcherState
	{
public:
	TRSStateChangingRoleWithEPR(CRoleSwitcherStateFactory& aFactory);

	virtual void Enter(CRoleSwitcher& aContext) const;
	virtual void EventReceived(CRoleSwitcher& aContext, TBTBasebandEventNotification& aEvent) const;
	virtual void TimerExpired(CRoleSwitcher& aContext) const;
	};

NONSHARABLE_CLASS(TRSStateEnablingEncryption) : public TRoleSwitcherState
	{
public:
	TRSStateEnablingEncryption(CRoleSwitcherStateFactory& aFactory);

	virtual void Enter(CRoleSwitcher& aContext) const;
	virtual void Exit(CRoleSwitcher& aContext) const;   
	virtual void EventReceived(CRoleSwitcher& aContext, TBTBasebandEventNotification& aEvent) const;
	virtual void TimerExpired(CRoleSwitcher& aContext) const;
	};

#ifdef __FLOG_ACTIVE
#define STATENAME(x)  iName=_L(x)
#else
#define STATENAME(x)
#endif

inline TBTBasebandRole CRoleSwitcher::RequestedRole() const
	{
	return iRole;
	}

inline TBool CRoleSwitcher::IsEncryptionDisabledForRoleSwitch() const
	{
	return iIsEncryptionDisabledForRoleSwitch;
	}

#endif /* ROLESWITCHHELPER_H */
