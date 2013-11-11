// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// csesuresessionbase.cpp
// 
//

#include "csecureserverbase.h"
#include "cshutdowntimer.h"
#include <e32debug.h>
// Lbs
#include "lbsdevloggermacros.h"

/** 
Create a new session, called from CPolicyServer

@param aVersion TVersion reference
@param aMessage RMessage2 object reference
@return CSession2 based object pointer
@internalTechnology
@released
 */	
CSession2* CSecureServerBase::NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const
	{
	TVersion v = GetServerVersion();
	if(!User::QueryVersionSupported(v, aVersion))
		{
		User::Leave(KErrNotSupported);	
		}
	CSession2* session = DoNewSessionL(aVersion, aMessage);
	return session;
	}
	
/** 
destrcutor, cancel if any outstanding request of shutdown timer and release it.

@internalTechnology
@released
 */	
CSecureServerBase::~CSecureServerBase()
	{
	LBSLOG(ELogP1,"->CSecureServerBase::~CSecureServerBase");
	if(iShutdownTimer)
		{
		if(iShutdownTimer->IsActive())
			iShutdownTimer->Cancel();		
		}
	delete iShutdownTimer;
	iShutdownTimer = NULL;
	LBSLOG(ELogP1,"->CSecureServerBase::~CSecureServerBase");
	}
	
/** 
Create shut down timer

@internalTechnology
@released
 */	
void CSecureServerBase::BaseConstructL(TBool aShutdownTimerRequired)
	{
	LBSLOG(ELogP1,"->CSecureServerBase::BaseConstructL");
	iBaseConstructCalled = ETrue;
	if(aShutdownTimerRequired)
		{
		iShutdownTimer = new (ELeave) CShutdownTimer();
		iShutdownTimer->ConstructL();
		}
	LBSLOG(ELogP1,"<-CSecureServerBase::BaseConstructL");
	}

/** 
constructor, create server base with specified priority and policy, default shut down time delay

@param aPriority Defined server priority
@param aSecurityPolicy A TPolicy object reference, to define server security policy
@internalTechnology
@released
 */	
CSecureServerBase::CSecureServerBase(TInt aPriority, const TPolicy& aSecurityPolicy) : CPolicyServer(aPriority, aSecurityPolicy),
																						iShutdownDelay(KDefaultShutdownDelay)	
	{
	// nothing to do?
	}

/** 
Increase the session count by 1
The function is called whenever a new session is created, cancel the current outstanding request of shutdown timer

@internalTechnology
@released
 */	
void CSecureServerBase::IncSession()
	{
	LBSLOG(ELogP1,"->CSecureServerBase::IncSession");
	__ASSERT_ALWAYS(iBaseConstructCalled, PanicServer(EBaseConstructNotCalled));
	++iSessionCount;
	if(iShutdownTimer)
		{
		iShutdownTimer->Cancel();
		}
	LBSLOG(ELogP1,"<-CSecureServerBase::IncSession");	
	}
	
/** 
Decrease the session count by 1
The function is called whenever a new session is released, start shutdown timer with current shut down timer delay

@internalTechnology
@released
 */	
void CSecureServerBase::DecSession()
	{
	LBSLOG(ELogP1,"->CSecureServerBase::DecSession");

	__ASSERT_ALWAYS(iBaseConstructCalled, PanicServer(EBaseConstructNotCalled));
	--iSessionCount;
	if(iSessionCount > 0)
		return; // bail out early
	// ~CSecureSessionBase calls this, but it can be called after
	// ~CSecureServerBase(!) e.g. a forced shutdown of the server with clients still connected
	// so we need to guard this.
	if(iShutdownTimer)
		{
		iShutdownTimer->Start(iShutdownDelay); // otherwise start the shutdown timer.
		}
	LBSLOG(ELogP1,"<-CSecureServerBase::DecSession");	
	}

/** 
Return the session count

@return Session count
@internalTechnology
@released
 */	
TInt CSecureServerBase::GetSessionCount() const
		{
		__ASSERT_ALWAYS(iBaseConstructCalled, PanicServer(EBaseConstructNotCalled));
		return iSessionCount;
		}

/** 
Get the current shut down delay

@return Shut down time delay
@internalTechnology
@released
 */			
TTimeIntervalMicroSeconds32 CSecureServerBase::GetShutdownDelay() const
	{
	__ASSERT_ALWAYS(iBaseConstructCalled, PanicServer(EBaseConstructNotCalled));
	return iShutdownDelay;
	}

/** 
Set shut down delay

@param aDelay Shut down time delay
@internalTechnology
@released
 */	
void CSecureServerBase::SetShutdownDelay(const TTimeIntervalMicroSeconds32 aDelay)
	{
	__ASSERT_ALWAYS(iBaseConstructCalled, PanicServer(EBaseConstructNotCalled));
	iShutdownDelay = aDelay;
	}

/** 
Panic the server with specified panic reason code

@param aPanicCode Panic reason code
@internalTechnology
@released
 */	
void CSecureServerBase::PanicServer(TInt aPanicCode) const
	{
	_LIT(KSecureServerBase, "SecureServerBase");
	User::Panic(KSecureServerBase, aPanicCode);
	}
