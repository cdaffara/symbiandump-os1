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
// serverside.h
// server side exmaple of how to use the abstract server framework
// 
//

#ifndef __TE_DUMMY_SERVER_H__
#define __TE_DUMMY_SERVER_H__

#include <e32property.h>
#include "tserverstartparams.h"
#include "csecureasbase.h"
#include "csecureserverbase.h"
#include "csecuresessionbase.h"
#include "csecuresessionsubsessionbase.h"
#include "csubsessionbase.h"
#include "messageutils.h"


class CSuicideWatch;

class CTestDummyServerImpl : public CBase, public MCreateServerImpl
	{
public:
	virtual void CreateServerLC(TServerStartParams& aParams);	
	};
	
class MSuicideWatch
	{
public:
	virtual void OnSuicideFlag() = 0;
	};	
	
class CTestDummyServer : public CSecureServerBase, public MSuicideWatch
	{
public:
	CTestDummyServer(TInt aPriority, const CPolicyServer::TPolicy& aSecurityPolicy);
	~CTestDummyServer();
	void ConstructL(const TDesC& aServerName);
	CSession2* DoNewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	TVersion GetServerVersion() const;
	
protected: // From MSuicideWatch
	void OnSuicideFlag();
		
protected:
	// property
	TVersion iVersion;
	
private:
	CSuicideWatch	* iSuicideWatch;
	};
	
// this is a session WITH subsession support	
class CTestDummyServerSession : public CSecureSessionSubSessionBase
	{
public:
	void CreateL();
	void ServiceMessageL(const RMessage2& aMessage);
	void ServiceMessageError(const RMessage2& aMessage, const TInt aError);
	};
	
class  CSuicideWatch : public CActive 
	{
public:
	static CSuicideWatch* NewL(MSuicideWatch* aObserver);

    ~CSuicideWatch();

protected:
	CSuicideWatch(MSuicideWatch* aObserver);
    void ConstructL();

protected: // From CActive
    void RunL();
    void DoCancel();
    
private:
    MSuicideWatch* iObserver;
    RProperty iSuicideFlag;
    };
#endif //__TE_DUMMY_SERVER_H__
