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
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#ifndef NOTIFIERSRV_H
#define	NOTIFIERSRV_H

#include <e32cons.h>
#include <e32property.h>

// server name
#define COMMSDATNOTIFIER_SERVER_NAME		_L("ShimNotifierSvr")


// A version must be specified when creating a session with the server
const TUint KCommsdatNotifierMajorVersionNumber=1;
const TUint KCommsdatNotifierMinorVersionNumber=0;
const TUint KCommsdatNotifierBuildVersionNumber=0;
const TUint KCommsdatNotifierSpecialBuildVersionNumber=1;

// opcodes used in message passing between client and server
enum TCommsdatNotifierServRqst
	{
	ERequestNotify,
	ECancelNotify,	
	ECancelAllNotify
	};

const TUid KUidCommsDatNotificationEvent = {0x1020762E};
class CShutdownWatchDog;

NONSHARABLE_CLASS(CCommsdatNotifierServer) : public CServer2
/**
@internalComponent
*/
	{
public:
	static CCommsdatNotifierServer* NewL();
	~CCommsdatNotifierServer();
	
// CServer
	virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
//
	void IncrementSessionCount();
	void DecrementSessionCount();
	void MaybeShutDown();
protected:
	CCommsdatNotifierServer();
	void ConstructL();

private:
	TInt iSessionCount;
	CShutdownWatchDog* iShutdownWatchDog;
	};	
	
class CCommsdatNotifierSessionHandler;
class CCommsdatNotifierSessionHandler2;
class CCommsdatNotifierSessionHandlerBase;
	
NONSHARABLE_CLASS(CCommsdatNotifierSession) : public CSession2
/**
@internalComponent
*/
	{
public:
	static CCommsdatNotifierSession* NewL(CCommsdatNotifierServer* aServer, TBool aClientSpecialVersion);
	~CCommsdatNotifierSession();	
	// CSession
	virtual void ServiceL(const RMessage2& aMessage);
	
private:
	CCommsdatNotifierSession(CCommsdatNotifierServer* aServer);
	void ConstructL(TBool aClientSpecialVersion);
	void PanicClient(TInt aPanic,const RMessage2& aMessage) const;
		
private:
	CCommsdatNotifierServer* iNotifierSvr;
	CCommsdatNotifierSessionHandlerBase* iNotifierHandler;
	};

class CCommsdatNotifierSessionHandlerBase : public CActive
/**
@internalComponent
*/
	{
public:
	
	virtual void RegisterNotifyL(const RMessage2& aMessage)=0;
	virtual void CancelNotify(const RMessage2& aMessage)=0;
	virtual void CancelAllNotify(const RMessage2& aMessage)=0;
	void PanicClient(TInt aPanic) const;	
	
	~CCommsdatNotifierSessionHandlerBase();
	
protected:

	// CActive
	void DoCancel();
		
	CCommsdatNotifierSessionHandlerBase();
	void ConstructL();	
    void CompleteMessage(const RMessage2& aMessage,TInt aVal);	

protected:
	RProperty iProperty;	
	};

NONSHARABLE_CLASS(CCommsdatNotifierSessionHandler) : public CCommsdatNotifierSessionHandlerBase
/**
@internalComponent
*/
	{
public:
	static CCommsdatNotifierSessionHandler* NewL();
	~CCommsdatNotifierSessionHandler();
	
	void RegisterNotifyL(const RMessage2& aMessage);
	void CancelNotify(const RMessage2& aMessage);
	void CancelAllNotify(const RMessage2& aMessage);
	void PanicClient(TInt aPanic) const;	
	
protected:

	// CActive
	void RunL();
	TInt RunError(TInt aError);
	
	CCommsdatNotifierSessionHandler();
	void ConstructL();

private:
	RMessage2 iMessage; 
	TInt iPendingEvent;
	};

NONSHARABLE_CLASS(CCommsdatNotifierSessionHandler2) : public CCommsdatNotifierSessionHandlerBase
/**
@internalComponent
*/
	{
public:
	static CCommsdatNotifierSessionHandler2* NewL();
	~CCommsdatNotifierSessionHandler2();
	
	void RegisterNotifyL(const RMessage2& aMessage);
	void CancelNotify(const RMessage2& aMessage);
	void CancelAllNotify(const RMessage2& aMessage);
	void PanicClient(TInt aPanic) const;	
	
protected:

	void CompleteRequests(TInt aErrorCode);
	void ResetRequestVector();

	// CActive
	void RunL();
	TInt RunError(TInt aError);
	
	CCommsdatNotifierSessionHandler2();
	void ConstructL();

private:
	CArrayFixFlat<RMessage2>* iMessagePool;
	};
		
NONSHARABLE_CLASS(CCommsdatNotifierScheduler) : public CActiveScheduler
/**
Scheduler
@internalComponent
*/
	{
public:
	~CCommsdatNotifierScheduler() 
		{
		delete iServer;	
		}
public:
	CCommsdatNotifierServer* iServer;
	};
	
	
	
NONSHARABLE_CLASS(CShutdownWatchDog) : public CTimer
/**
@internalComponent
*/
	{
public:
	void StartShutdown();
	static CShutdownWatchDog* NewL(CCommsdatNotifierServer& aNotifierServer);
protected:
	CShutdownWatchDog(CCommsdatNotifierServer& aNotifierServer);
	void RunL();
private:
	CCommsdatNotifierServer& iNotifierServer;
	};	
	
		
		
#endif
