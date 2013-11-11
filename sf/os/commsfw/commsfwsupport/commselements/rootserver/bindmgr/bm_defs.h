// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__BM_DEFS_H__)
#define __BM_DEFS_H__

#include <cfmsgs.h>
#include "bindmgr.h"
#include <cflog.h>

/**
@file
@internalComponent
*/

__CFLOG_STMT(_LIT8(KLogSubSysRS,"RootServer");) // subsystem name

_LIT(KBindMgrPanic, "BindMgr");
enum TBindMgrPanics
	{
	EBadState,
	EDiscoveryRespOutOfSequence,
	EModuleRefMissing,
	EMsgQueueFault,
	EUnsupportedCFMessage
	};


class CCommsChannelSendQueue : public CActive
/** This class adds an input buffer to a comms framework CFChannel using
an array to store unsent messages.
*/
	{
public:
	static CCommsChannelSendQueue* NewL(RCFChannel& aChannel);
	~CCommsChannelSendQueue();
	TInt Send(const TCFMessage& aMsg);
	TInt ReserveSpace();

protected:
	void RunL();
	void DoCancel();
private:
	CCommsChannelSendQueue(RCFChannel& aChannel);
private:
	RArray<TCFMessage> iMsgQueue;
	RCFChannel& iChannel;
	};

class CTask;

class CModule : public CActive
/** Manages the state of a CPM or comms-server and handles the asynchronous
message queueuing.
*/
	{
public:
	static CModule* NewL(MBindManagerNotify *aNotifier,
						 const TCFModuleNameF &aModule,
						 TransactionId &aTransactionId,
						 const RCFChannel::TMsgQueues& aInputQueues,
						 const RCFChannel::TMsgQueues& aOutputQueues);

	~CModule();
	void SendL(const TCFMessage &aMessage);

	void DoDiscoverL(TUint &aTaskId);
	TBool CheckDiscoveryMsgFinishedL( const TCFMessage &aMessage );

	TBool GetBindTaskL(const RCFChannel::TMsgQueues& aInputQueues,
					   const RCFChannel::TMsgQueues& aOutputQueues,
					   const TCFSubModuleAddress &aSubModule1,
					   const TCFSubModuleAddress &aSubModule2,
					   const TCFBindType aType,
					   CTask*& aTask);

	TBool GetUnbindTaskL(const TCFSubModuleAddress& aSubModule1,
						 const TCFSubModuleAddress& aSubModule2,
						 const TBool aPeerIsDead,
						 CTask*& aTask);
	void DoStartTaskL(CTask* aTask);

	void DoShutdownL();

	void EndTask(const TUint &aTaskId, const TInt aStatus);
	void EndAllTasks(const TInt aStatus);

	TInt EnumerateSubModules(const TInt& aPosition, TCFSubModuleNameF& aSubModuleName);
	TInt NumSubModules() const;

	TBool HasSubModule(const TCFSubModuleNameF &aSubModule) const;
	TInt ReserveSpaceInSendQueue();
	const TCFSubModuleAddress &SubModuleNameRefL(const TCFSubModuleAddress &aSubModule);

	const TCFModuleNameF& Name() const;
	const TBool& Initialising() const;
	void SetInitialising(const TBool aInitialising);
	void Dequeue();

public:
	TDblQueLink iLink;

protected:
	virtual void DoCancel();
	virtual void RunL();
private:
	CModule(MBindManagerNotify *aNotifier,
			TransactionId& aTransactionId,
			const TCFModuleNameF &aModule);
	void ConstructL(const RCFChannel::TMsgQueues& aInputQueues,
					const RCFChannel::TMsgQueues& aOutputQueues);
	TUint GetNextTransactionId();

private:
	MBindManagerNotify *iNotifier;
	TCFModuleNameF iName;
	TBool iInitialising;
	RCFChannel iRecvChannel;
	RCFChannel iSendChannel;
    TDblQue<CTask> iTaskList;
	CCommsChannelSendQueue *iSendQueue;
	TCFSubModuleNameF* iDiscoveryArray;
	RArray<TCFSubModuleNameF> iSubModuleNames;
	RArray<TCFSubModuleAddress> iSubModuleAddrs;
	TransactionId &iTransactionId;
	};


class CBindInfo : public CBase
/** Stores details about an individual binding between two modules, including
the binding state and any errors associated with it.
*/
	{
public:
	static CBindInfo* NewL(const TRSBindType aType,
						   const TCFSubModuleAddress& aSubModule1,
						   const TCFSubModuleAddress& aSubModule2,
						   TInt aChannel1Length, TInt aChannel2Length);
	void ConstructL(TInt aChannel1Length, TInt aChannel2Length);
	void Dequeue();
	const RCFChannel& Channel1() const;
	const RCFChannel& Channel2() const;
	CBindManager::TBindingInfo& Info();
	~CBindInfo();
	void TaskCompleted(const CTask &aTask, const TInt aStatus);

	const TCFSubModuleAddress& SubModuleAddr1() const;
	const TCFSubModuleAddress& SubModuleAddr2() const;
	const TRSBindType& BindType() const;

	CBindManager::TBMBindState State1() const;
	CBindManager::TBMBindState State2() const;
	void SetState1(const CBindManager::TBMBindState aState);
	void SetState2(const CBindManager::TBMBindState aState);

	TInt Result1() const;
	TInt Result2() const;
	void SetResult1(const TInt aResult);
	void SetResult2(const TInt aResult);

public:
	TDblQueLink iLink;

private:
	CBindInfo(const TRSBindType aType,
			  const TCFSubModuleAddress& aSubModule1,
			  const TCFSubModuleAddress& aSubModule2);

private:
	class TSubModuleDetails
		{
		public:
			TSubModuleDetails(const TCFSubModuleAddress& aSubModule);

			const TCFSubModuleAddress iSubModule;
			CBindManager::TBMBindState iState;
			TInt iResult;
		};

	TRSBindType iType;
	TSubModuleDetails iSubModule1;
	TSubModuleDetails iSubModule2;
	RCFChannel iChannel1;
	RCFChannel iChannel2;
	};

class CTask : public CBase
/** Holds details about an individual message exchange with a server or CPM.
*/
	{
public:
	static CTask* NewL(const TCFMessage &aMessage, const TUint aTaskId = 0);
	TUint TaskId() const;
	const TCFMessage& Message() const;
	TBool CheckIfSame(TCFMessage &aMessage) const;
	TBool NeedReply() const;
	TBool ReplyMatches(const TCFMessage &aMessage) const;
	void Dequeue();
	~CTask();

	static TBool GetMsgReturnCode(const TCFMessage &aMessage, TInt &aStatus);

public:
	TDblQueLink iLink;

	enum { ENoReplyExpected = 0xFFFFFFFF };

private:
	CTask(const TCFMessage &aMessage, TUint aTaskId);

private:
	TInt iTaskId;
	TCFMessage iMessage;
	};


class MBMNotifyTimerShot
/** Abstract mixin interface class defining an up-call interface for a timer
expiry
*/
    {
public:
	virtual void ModuleTimerShot()=0;
    };


class CJobTimeout : public CTimer
/** Simple active object class providing timeout functionality to CJob
*/
    {
public:
    static CJobTimeout* NewL(MBMNotifyTimerShot* aNotifier);
protected:
    void RunL();
private:
    CJobTimeout(MBMNotifyTimerShot* aNotifier);
    MBMNotifyTimerShot* iNotifier;
    };

class CJob : public CBase, public MBMNotifyTimerShot
/** Manages an asynchronous call from the rootserver into bindmanager, including
monitoring a list of individual tasks associated with the job, and dealing
with a timeout if a task fails to complete
*/
	{
public:
    static CJob* NewL(TRequestStatus& iRsStatus, MBindManagerNotify *aBindManager);
	virtual void ModuleTimerShot();
	~CJob();
	void Dequeue();
	void AddTaskL(TUint aTaskId, const TCFModuleNameF &aModule);
	void StartTimeout(TUint aMicroseconds);
	void TaskCompleted(const TCFModuleNameF &aModuleName, const TUint aTaskId,
					   const TInt aStatus);
	TBool HasStatus(const TRequestStatus& aRsStatus) const;
	void Cancel();

public:
	TDblQueLink iLink;

private:
	CJob(TRequestStatus& aRsStatus, MBindManagerNotify *aBindManager);
	void ConstructL();

private:
	TRequestStatus& iRsStatus;	// as received from rootserver
	MBindManagerNotify *iBindManager;
	TInt iCompletionCode;
	TBool iCancelled;
	CJobTimeout *iTimeout;

	class TTaskData
		{
	public:
		TTaskData(TUint aTaskId, const TCFModuleNameF &aModule) :
			iTaskId(aTaskId),
			iModule(aModule)
				{
				}
	public:
		TUint iTaskId;
		const TCFModuleNameF &iModule;
		};

    RArray<TTaskData> iTaskDataList;
	};

#endif // __BM_DEFS_H__

