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
// server side exmaple of how to use the abstract server framework
// 
//

#ifndef __SERVERSIDE_H__
#define __SERVERSIDE_H__

#include "tserverstartparams.h"
#include "csecureasbase.h"
#include "csecureserverbase.h"
#include "csecuresessionbase.h"
#include "csecuresessionsubsessionbase.h"
#include "csubsessionbase.h"
#include "messageutils.h"

/*
class CExTimeServerAs : public CSecureASBase
	{
public:
	virtual MCreateServerImpl* GetImplLC();
	};
*/

class CExTimeServerImpl : public CBase, public MCreateServerImpl
	{
public:
	virtual void CreateServerLC(TServerStartParams& aParams);	
	};
	
	
class CExTimeServer : public CSecureServerBase
	{
public:
	CExTimeServer(TInt aPriority, const CPolicyServer::TPolicy& aSecurityPolicy);
	~CExTimeServer();
	void ConstructL(const TDesC& aServerName);
	CSession2* DoNewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	TVersion GetServerVersion() const;
protected:
	// property
	TVersion iVersion;
	};
	
// this is a session WITH subsession support	
class CExTimeSession : public CSecureSessionSubSessionBase
	{
public:
	void CreateL();
	void ServiceMessageL(const RMessage2& aMessage);
	void ServiceMessageError(const RMessage2& aMessage, const TInt aError);
	};
	

class CExTimeSubSession : public CBase, public MSubSessionImpl
	{
public:
	void ConstructL();
	~CExTimeSubSession();
	// from MSubSessionImpl
	void DispatchL(const RMessage2& aMessage);
	void DispatchError(const RMessage2& aMessage, TInt aError);
	void CreateSubSessionL(const RMessage2& aMessage,const CSecureServerBase* aServer);
	void CloseSubSession();
	// via MSubSessionImpl::MRelease	
	void VirtualRelease();
	// action methods
	void GetTime(const RMessage2& aMessage);
	void GetTimeAsync(const RMessage2& aMessage);
	void CancelGetTime(const RMessage2& aMessage);
	static TInt TimerExpired(TAny* aObject);
	void DoTimerExpired();
protected:
	// property
	CPeriodic* iTimer;
	RMessage2 iStoredMsg;
	};
	
class CExServerNameGetterSession : public CBase, public MSubSessionImpl
	{
public:
	void ConstructL();
	~CExServerNameGetterSession();
	// from MSubSessionImpl
	void DispatchL(const RMessage2& aMessage);
	void DispatchError(const RMessage2& aMessage, TInt aError);
	void CreateSubSessionL(const RMessage2& aMessage,const CSecureServerBase* aServer);
	void CloseSubSession();
	// via MSubSessionImpl::MRelease	
	void VirtualRelease();
	// action methods
	void GetNames(const RMessage2& aMessage);
	};
	
#endif //__SERVERSIDE_H__
