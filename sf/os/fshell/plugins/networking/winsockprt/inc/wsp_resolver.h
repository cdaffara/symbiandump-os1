// wsp_resolver.h
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __WSP_RESOLVER_H__
#define __WSP_RESOLVER_H__

#include <es_sock.h>
#include <es_prot.h>
#include "wsp_subsession.h"


class RWin32Factory;
class TInetAddr;


class RWin32Resolver : public RWin32SubSession
	{
public:
	RWin32Resolver();
	TInt Open(RWin32Factory& aFactory);
	void Close();
	TInt GetByName(TNameRecord& aName, TRequestStatus& aStatus);
	TInt GetByAddress(TNameRecord& aName, TRequestStatus& aStatus);
	void Cancel();
private:
	TPtr8 iNameRecordPckg;
	};


class CWin32Resolver : public CWin32SubSession
	{
public:
	enum TOppCode
		{
		EGetByName,
		EGetByAddress,
		ECancel
		};
public:
	static CWin32Resolver* NewL(CWin32Scheduler& aScheduler);
	virtual ~CWin32Resolver();
private: // From CWin32ActiveObject.
	virtual void Run();
private: // From CWin32SubSession.
	virtual void ServiceL(TWin32Message& aMessage);
private:
	CWin32Resolver(CWin32Scheduler& aScheduler);
	void ConstructL();
	void Cancel();
	void CreateWorkerThread();
private: // Methods used by the worker thread.
	static TInt WorkerThreadStart(TAny* aPtr);
	void GetByName();
	void GetByAddress();
private:
	TWin32Message* iMessage;
	RThread iWorkerThread;	// Synchronous name lookup calls are run in their own thread.
							// This is because the asychronous versions use window messages rather event object, and so can't fit into this framework.
	TInt iError;
	TInt64 iRandSeed;
	};


class MWin32ResolverObserver
	{
public:
	virtual void HandleWin32ResolverCompletion(TInt aError) = 0;
	};


class CWin32ResolverWrapper : public CActive
	{
public:
	static CWin32ResolverWrapper* NewL(MWin32ResolverObserver& aObserver, RWin32Factory& aWin32Factory);
	~CWin32ResolverWrapper();
	TInt GetByName(TNameRecord& aName);
	TInt GetByAddress(TNameRecord& aName);
private: // From CActive.
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
public:
	CWin32ResolverWrapper(MWin32ResolverObserver& aObserver);
	void ConstructL(RWin32Factory& aWin32Factory);
private:
	MWin32ResolverObserver& iObserver;
	RWin32Resolver iWin32Resolver;
	};


class CWinSockResolver : public CHostResolvProvdBase, public MWin32ResolverObserver
	{
public:
	static CWinSockResolver* NewL(RWin32Factory& aFactory);
	virtual ~CWinSockResolver();
private: // From CResolverProvdBase.
	virtual void CancelCurrentOperation();
private: // From CHostResolvProvdBase.
	virtual void GetByName(TNameRecord& aName);
	virtual void GetByAddress(TNameRecord& aName);
	virtual void SetHostName(TDes& aNameBuf);
	virtual void GetHostName(TDes& aNameBuf);
private: // From MWin32ResolverObserver.
	virtual void HandleWin32ResolverCompletion(TInt aError);
private:
	CWinSockResolver();
	void ConstructL(RWin32Factory& aFactory);
private:
	CWin32ResolverWrapper* iResolverWrapper;
	};


#endif // __WSP_RESOLVER_H__
