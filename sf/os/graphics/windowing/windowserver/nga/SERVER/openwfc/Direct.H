// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of Direct Screen Access class
// 
//

#ifndef __DIRECT_H__
#define __DIRECT_H__

#include <e32std.h>
#include <e32base.h>
#include "W32STD.H"
#include "w32cmd.h"
#include "OBJECT.H"
#include "CLIENT.H"
#include <e32msgqueue.h>

class CWsClientWindow;
class CWsDirectScreenAccess;
class CDsaMsgQueue;

NONSHARABLE_CLASS(CWsAbortDirect) : public CActive
	{
public:
	CWsAbortDirect(CWsDirectScreenAccess* aDirect,CDsaMsgQueue* aParent);
	~CWsAbortDirect();
	void Started();
	void Complete(TInt aReason);
private:
	//Pure virtual function from CActive
	void RunL();
	void DoCancel();
private:
	CWsDirectScreenAccess* iDirect;
	CDsaMsgQueue* iParent;
	};

NONSHARABLE_CLASS(CDsaMsgQueue) : public CBase
{
	public:
		static CDsaMsgQueue* NewL(CWsDirectScreenAccess* aDirect);
		~CDsaMsgQueue();
		TInt CreateSendQueue();
		TInt CreateRecQueue();
		RMsgQueueBase* SendQueue();
		RMsgQueueBase* RecQueue();
		void Cancel();
		void Complete();
		void CompleteAbort();
		void Started();
		TInt Send(TInt aData);
		TInt ReceiveData();
		TRequestStatus& Status()
		{
			return iAborted->iStatus;
		}
		
	private:
		CDsaMsgQueue();
		void ConstructL(CWsDirectScreenAccess* aDirect);
	private:
		RMsgQueueBase iSendQueue;
		RMsgQueueBase iRecQueue;
		CWsAbortDirect* iAborted;
	};

class CWsDirectScreenAccess : public CWsScreenObject
	{
public:
	enum TStatus
		{
		EDirectStatusTimeNotCreated,
		EDirectStatusNone,
		EDirectStatusInitialising,
		EDirectStatusRunning,
		EDirectStatusCompleted,
		EDirectStatusCanceling,
		EDirectStatusAbortedWindow,
		EDirectStatusAbortedGlobal,
		EDirectStatusAborted=EDirectStatusAbortedWindow,
		};
public:
	static CWsDirectScreenAccess* NewL(CWsClient* aOwner,TBool aRegionTrackingOnly);
	~CWsDirectScreenAccess();
	void Request(TInt handle);
	void GetRegion(TInt aNumRects);
	void Cancel();
	void Aborted();
	void AbortNow();
	TInt GetSendQueue();
	TInt GetRecQueue();
	void SignalAbort(RDirectScreenAccess::TTerminationReasons aReason);
	inline TRequestStatus& AbortStatus() {return iMsgQueue->Status();}
	void CancelAbortObject();
	void Abort();
	//Pure virtual function from CWsObject
	void CommandL(TInt aOpcode, const TAny* aCmdData);
	inline TBool IsVisible() const;
	inline TBool IsRegionTrackingOnly();
	TBool IsAbortRequired(const TRegion& aTopVisibleRegion) const;
	inline CWsClientWindow* ClientWindow() const;
public:
	TSglQueLink iLink;
	TSglQueLink iMultipleDSALink;
	TSglQueLink iAbortLink; // Used to build a list of DSA objects that need to be aborted on a specific window
private:
	inline CWsDirectScreenAccess(CWsClient* aOwner): CWsScreenObject(aOwner,WS_HANDLE_DIRECT, aOwner->Screen()) {}
	void ConstructL(TBool aRegionTrackingOnly);
	TInt Initiate();
	void Terminate1();
	void Terminate2();
	void CorrectScreen();
#if defined(_DEBUG)
	TBool OnQueue();
#endif
private:
	CWsClientWindow* iWin;
	RWsRegion iVisible;
	CDsaMsgQueue* iMsgQueue;
	TStatus iStatus;
	RDirectScreenAccess::TTerminationReasons iAbortReason;
	TBool iRegionTrackingOnly;
	};

inline TBool CWsDirectScreenAccess::IsVisible() const
	{
	return !iVisible.IsEmpty();
	}
inline CWsClientWindow* CWsDirectScreenAccess::ClientWindow() const
	{
	return iWin;
	}

inline TBool CWsDirectScreenAccess::IsRegionTrackingOnly()
	{
	return iRegionTrackingOnly;
	}
#endif
