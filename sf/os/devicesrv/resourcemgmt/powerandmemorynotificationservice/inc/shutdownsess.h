// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology 
*/

#if !defined(SHUTDOWNSESS_H)
#define SHUTDOWNSESS_H

#include "shutdownsrv.h"
class CShutdownTimer;

/**
This class describes an object, which handles requests, such as of MSaveObserver::TSaveType type.
When CServShutdownServer::HandleShutdownEventL() gets called, the CServShutdownServer implementation
will notify all registered clients, completing their asynchronous messages 
(CServShutdownSession::iPtr), then it will wait until all clients re-register itself and
if this is a beginning of a powerdown sequence, the method will store the locales and the HAL
properties subsequently switching off the power.
*/
class CServShutdownServer : public CServer2, public MShutdownEventObserver
	{
public:
	IMPORT_C static CServShutdownServer* NewL();
	IMPORT_C ~CServShutdownServer();
	IMPORT_C void HandlePowerNotifRequest(const RThread& aClient);
	IMPORT_C void NotifySave(MSaveObserver::TSaveType aSaveType);
	IMPORT_C TBool IsPowerOff() const;
	IMPORT_C void CancelPowerOff();
	void SwitchOff();
#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
	TInt ClientArrayCount();
	void ClientArrayL(const RMessage2& aMessage);
#endif //SYMBIAN_SSM_GRACEFUL_SHUTDOWN	
public:
	IMPORT_C virtual void ConstructL();
public: // from MShutdownEventObserver
	IMPORT_C void HandleShutdownEventL(MSaveObserver::TSaveType aAction,TBool aPowerOff, TPowerState aEvent = EPwStandby);
	IMPORT_C CArrayFix<TThreadId>* ClientArrayLC();
	IMPORT_C TBool IsClientHung(TThreadId aId) const;
	IMPORT_C void GetShutdownState(TBool& aPowerOff, TBool& aAllSessionsHavePendingRequest) const;

protected:
	IMPORT_C CServShutdownServer(TInt aPriority);
private:
	TBool AllSessionsHavePendingRequest() const;
	void DoSwitchOff();
	// from CServer
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
private:
	TBool iPowerOff;
	TPowerState iPowerEvent;	
	CShutdownTimer* iShutdownTimer;	
	};

/**
This class describes a server side session object, which handles reqistration requests
of clients, which are interested in power down events.
*/
class CServShutdownSession : public CSession2
	{
public:
	IMPORT_C ~CServShutdownSession();
protected:
	IMPORT_C CServShutdownSession();
	// from CSession
	IMPORT_C void ServiceL(const RMessage2& aMessage);
public:
	static CServShutdownSession* NewL();
	TBool HasPendingRequest() const;
	void NotifySave(MSaveObserver::TSaveType aSaveType);

private:
	void RequestNotifyPowerDown(const RMessage2& aMessage);
	void RequestNotifyPowerDownCancel();
	void DoServiceL(const RMessage2& aMessage, TBool& aCompleteRequest);
	void PowerOffL(const RMessage2& aMessage);
	void PowerStateL(const RMessage2& aMessage) const;

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
	void HandleShutdownEventL(const RMessage2& aMessage);
	void ClientArrayL(const RMessage2& aMessage);
	void IsClientHung(const RMessage2& aMessage) const;
	void GetShutdownState(const RMessage2& aMessage) const;
	void ClientArrayCount(const RMessage2& aMessage) const;
#endif //SYMBIAN_SSM_GRACEFUL_SHUTDOWN

public:
	TThreadId ClientThreadId() const;

private:
	RMessagePtr2 iPtr;
	TInt iCurrentEvent;
	TInt iOutstandingEvent;
	};


#endif// SHUTDOWNSESS_H
