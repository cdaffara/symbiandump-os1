// SensibleServer.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef SENSIBLESERVER_H
#define SENSIBLESERVER_H

#include <e32base.h>
#include "cliserv.h"
#include <f32file.h>
#include <gdi.h>
#include <basched.h>

#include "SensibleCompat.h"

class CCallbackContext;
class CSensibleSession;
class CFilteringScheduler;

void PanicClient(const RMessage& aMessage, TInt TMyPanic);
extern void CleanupPanicPushL();

class CShutdown : public CTimer
	{
public:
	inline CShutdown();
	inline void ConstructL();
	inline void Start(TInt aDelay);
private:
	void RunL();
	};

class CSensibleServer : public CServerBase
	{
public:
	void AddSession();
	void DropSession();
	inline CFilteringScheduler* Scheduler();
	CSensibleServer();
	~CSensibleServer();
	virtual void ConstructL();

	// Some functions that call into the filtering scheduler
	// NOTE: These functions cannot be used at the same time as queuing a callback that requires a result, nor can they be nested (you'll get a panic)
	//void BlockAllAOsExceptServerRequests();
	void BlockRequestsFrom(CActive* aActive1, CActive* aActive2=NULL);
	void StopBlocking();

protected:
	#ifdef __HIDE_IPC_V1__
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	#else
	CSessionBase* NewSessionL(const TVersion& aVersion) const;
	#endif
	TInt RunError(TInt aError);
	virtual TInt TransientServerShutdownTime() const; // Return 0 to never shutdown

private:
	TInt iSessionCount;
	CShutdown iShutdown;
	CFilteringScheduler* iScheduler;
	CActiveScheduler* iOldScheduler;
	};

class CSensibleSession : public CSessionBase
	{
public:
	CSensibleSession();
	#ifndef __HIDE_IPC_V1__
	void CreateL(const CServer& aServer);
	#endif
	void CreateL();
	void QueueCallbackL(CCallbackContext* aContext); // This completes a callback immediately if possible, otherwise queues it. Used for callbacks that MUST be delivered.
	TBool DispatchCallback(TServerCallback& aCallback); // This completes a callback immediately if possible, otherwise returns EFalse. Used for notify-style callbacks. The callback cannot have a context.
	void CompleteNextCallback();

protected:
	virtual TBool DoServiceL(const RMessage& aMessage); // return EFalse if aMessage wasn't handled
	~CSensibleSession();

private:
	inline CSensibleServer& Server();
	void ServiceL(const RMessage& aMessage);
//	inline TBool ReceivePending() const;
private:
	TSglQue<CCallbackContext> iCallbackQ;
	RMessage iCallbackNotifier;
	TBool iCallbackPending;
	TBool iWaitingForCallbackResult;
	};

enum TCallbackContextFlags
	{
	EResultHBufC8 = 2,
	EResultHBufC16 = 4,
	EResultIsLeaveCode = 8,
	EActive = 16,
	EBlockServer = 32,
	};

class TCallbackWriter
	{
public:
	TCallbackWriter(TServerCallback& aCallback, HBufC8** aContext);

	// If any of the AddLs leave, they will delete aContext if it has been created
	void AddL(TInt aInt);
	void AddL(TUint aInt);
	void AddL(TPoint aPoint);
	void AddL(TSize aSize);
	void AddL(TRgb aRgb);
	void AddL(TRect aRect);
	void AddL(const TDesC16& aDesc);
	void AddL(const TDesC8& aDesc);

private:
	void AddL(const TDesC8& aData, char* aType);

private:
	TServerCallback& iCallback;
	HBufC8** iContext;
	TPtr8 iBuf;
	TBool iInContext;
	};

class CCallbackContext : public CBase
	{
public:
	union TResult
		{
		HBufC8** s;
		HBufC16** l;
		TDes8* pkg;
		TInt integer;
		};
		
public:
	CCallbackContext(TCallbackCode aCode);
	~CCallbackContext();
	void SetFlags(TInt aFlags);
	TBool Flag(TInt aFlags) const; // All bits of aFlags must be set to return true
	void ClearFlags(TInt aFlags);
	TBool CallbackRequiresResult() const;
	TBool CallbackHasContext() const;
	//void SetContext(HBufC8* aContext); // Takes ownership and clears flag EContextHBufC16
	//void SetContext(HBufC16* aContext); // Takes ownership and sets flag EContextHBufC16
	TCallbackWriter Writer();

	void SetResult(TDes8& aPkg); // Sets appropriate flags
	void SetResult(HBufC8*& aResult); // Sets appropriate flags
	void SetResult(HBufC16*& aResult); // Sets appropriate flags
	//void SetResult(TInt& aResult); // Sets appropriate flags
	HBufC8* Context();
	TResult& Result();
	TServerCallback& Callback();
	
private:
	void AddContext(const TUint8* aPtr, TInt aSize, char* aType);
private:
	TInt iFlags; // bitwise OR of TCallbackContextFlags
	TServerCallback iCallback;
	HBufC8* iContext;
	TResult iResult;

public:
	TSglQueLink iLink;
	};

#endif
