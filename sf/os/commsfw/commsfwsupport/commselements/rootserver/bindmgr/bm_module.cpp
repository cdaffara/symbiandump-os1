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

/**
 @file 
 @internalComponent
*/

#include <cflog.h>
#include <commschan.h>
#include <cfshared.h>
using namespace CommsFW;
#include "bm_defs.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemRootServerbm, "ElemRootServerbm");
#endif

CCommsChannelSendQueue* CCommsChannelSendQueue::NewL(RCFChannel& aChannel)
/**
Generates a new CCommsChannelSendQueue object
@param aChannel the send channel on which to send messages
@return A ptr to the new CCommsChannelSendQueue
@leave KErrNoMemory
*/
	{
	CCommsChannelSendQueue* This = 
		new(ELeave) CCommsChannelSendQueue(aChannel);
	CActiveScheduler::Add(This);
	return This;
	}


TInt CCommsChannelSendQueue::Send(const TCFMessage& aMsg)
/**
 Send method either sends the requested message immediately or stores it 
internally to send when space is available
param aMsg the message to be sent
@return A standard error code indicating success or failure
*/
	{ 
	TInt ret = KErrNone;

	if (IsActive())
		{
		// currently busy waiting for space
		ret = iMsgQueue.Append(aMsg);
		}
	else
		{
		__ASSERT_DEBUG(iMsgQueue.Count() == 0, User::Panic(KBindMgrPanic, EMsgQueueFault));
		ret = iChannel.Send(aMsg);
		if (KErrOverflow == ret)
			{
			ret = iMsgQueue.Append(aMsg);
			if (KErrNone == ret)
				{
				iChannel.NotifySpaceAvailable(iStatus);
				SetActive();
				}
			}
		}

	return ret;
	}


TInt CCommsChannelSendQueue::ReserveSpace()
/**
Called to make sure a message can be put on the queue at this time in 
this context. For the purpose it is used it would be a nice optimization
to also check whether there is actually room in the output channel first
as well, as the space then wouldn't be needed.
*/
	{

	// Add dummy message to the queue, to make sure there is space
	TCFMessage msg;
	TInt ret = iMsgQueue.Append(msg);
	if(KErrNone!=ret)
		{
		return ret;
		}
	iMsgQueue.Remove(iMsgQueue.Count()-1);
	return KErrNone;
	}


void CCommsChannelSendQueue::RunL()
/**
Active object RunL method is called when space becomes available on the send
queue. Attempts to send the first message in the queue.
@leave any code from RCFChannel.Send() except KErrOverflow
*/
	{
    __CFLOG_1(KLogSubSysRS, KLogCode, 
		_L8("CCommsChannelSendQueue::RunL &iStatus %08X"), &iStatus);
	switch(iStatus.Int())
		{
	case KErrNone:
			{
			while (iMsgQueue.Count() > 0)
				{
				TInt ret = iChannel.Send(iMsgQueue[0]);
				if (KErrNone == ret)
					{
					iMsgQueue.Remove(0);
					}
				else if (KErrOverflow == ret)
					{
					iChannel.NotifySpaceAvailable(iStatus);
					SetActive();
					break;
					}
				else
					{
					User::Leave(ret);
					}
				}
			}
		break;
	default:
		break;
		}
	}

void CCommsChannelSendQueue::DoCancel()
/** Cancel CCommsChannelSendQueue NotifySpaceAvailable().
*/
	{
	iChannel.CancelSpaceAvailable();
	}

CCommsChannelSendQueue::CCommsChannelSendQueue(RCFChannel &aChannel)
	: CActive(EPriorityStandard),
	  iChannel(aChannel)
/** Constructor for CCommsChannelSendQueue
*/
	{
	}

CCommsChannelSendQueue::~CCommsChannelSendQueue()
/** Destructor for CCommsChannelSendQueue
*/
	{
	Cancel();
	iMsgQueue.Close();
	}

CModule* CModule::NewL(MBindManagerNotify *aNotifier,
					   const TCFModuleNameF &aModule,
					   TransactionId &aTransactionId,
					   const RCFChannel::TMsgQueues& aInputQueues, 
					   const RCFChannel::TMsgQueues& aOutputQueues)
/** Generates a new CModule object
@param aNotifier bindmanager object to report events to
@param aModule name of module
@param aTransactionId xx
@param aInputQueues queues on which messages should sent to the module
@param aOutputQueues queues on which messages will be received from the module
@return A ptr to the new CModule
@leave KErrNoMemory
*/
	{
    CModule* pM = new (ELeave) CModule(aNotifier, aTransactionId, aModule);
    CleanupStack::PushL(pM);
	pM->ConstructL(aInputQueues, aOutputQueues);
    CleanupStack::Pop(pM);
	__CFLOG_STMT(TBuf8<KCFMaxModuleName> buf1;);
	__CFLOG_STMT(buf1.Copy(aModule););
    __CFLOG_2(KLogSubSysRS, KLogCode, 
		_L8("CModule::NewL module %S, &iStatus %08X"), &buf1, &(pM->iStatus));
    return pM;
	}

void CModule::ConstructL(const RCFChannel::TMsgQueues& aInputQueues, 
						 const RCFChannel::TMsgQueues& aOutputQueues)
/** Second phase constructor for CModule
@param aInputQueues queues on which messages should sent to the module
@param aOutputQueues queues on which messages will be received from the module
@leave KErrNoMemory
*/
	{
	User::LeaveIfError(iSendChannel.Create(aInputQueues, EOwnerProcess));
	User::LeaveIfError(iRecvChannel.Create(aOutputQueues, EOwnerProcess));

	iDiscoveryArray = new(ELeave) TCFSubModuleNameF[KDefaultSubmoduleArraySize];
	iSendQueue = CCommsChannelSendQueue::NewL(iSendChannel);

	// make us active
	CActiveScheduler::Add(this);
	iRecvChannel.NotifyDataAvailable(iStatus);
	SetActive();
	}

void CModule::DoCancel()
/** Cancel CModule NotifyDataAvailable
*/
	{
	iRecvChannel.CancelDataAvailable();
	}

TInt CModule::ReserveSpaceInSendQueue()
	{
	return iSendQueue->ReserveSpace();
	}

void CModule::RunL()
/** Active object RunL method, called when data becomes available on the receive
queue
@leave 
*/
	{
	__CFLOG_2(KLogSubSysRS, KLogCode, 
		_L8("CModule::RunL &iStatus %08X, status %d"), &iStatus, iStatus.Int());

	switch(iStatus.Int())
			{
	case KErrNone:
			{
		TCFMessage newMsg;
		TInt ret = iRecvChannel.Receive(newMsg);
		__CFLOG_1(KLogSubSysRS, KLogEvent, 
			_L8("CModule::RunL recv returned %d"), ret);
		if (ret == KErrUnderflow)
			{
			break;
			}

		TDblQueIter<CTask> taskIter(iTaskList);
		CTask *pTask;
		while ((pTask = taskIter++) != NULL)
			{
			if (pTask->ReplyMatches(newMsg))
				{
				break;
				}
			}
			
		if (pTask)
			{
			__CFLOG_2(KLogSubSysRS, KLogCode, _L8("CModule::RunL task found (code %d, id %d)"), newMsg.Code(), *((TId*)newMsg.ParamBlock()) );
			TBool finished = ETrue;
			TRAPD(result, finished = CheckDiscoveryMsgFinishedL( newMsg ) );
			if (result<KErrNone)
				{
			// it left, probably KErrNoMemory
			// so complete the message with this
				iNotifier->TaskCompleted(*this, *pTask, result);
				delete pTask;
				}
			else if (finished)
				{
				TInt status;
				CTask::GetMsgReturnCode(newMsg, status);
				// discovery messages overload the return code, to return
				// the number of submodulenames they wrote into the message
				// therefore a status retrieved that is greater than 0, implies
				// we should complete the request with KErrNone.
				if(status >= 0)
					{
					iNotifier->TaskCompleted(*this, *pTask, KErrNone);
					}
				else
					iNotifier->TaskCompleted(*this, *pTask, status);
				delete pTask;
				}
			}
		else
			{
			__CFLOG_2(KLogSubSysRS, KLogCode, 
				_L8("CModule::RunL matching task NOT found (code %d, id %d)"), newMsg.Code(), *((TId*)newMsg.ParamBlock()) );
			iNotifier->ReceivedMessage(Name(), newMsg);
			}
		}
		break;

	default:
		break;
		}

	iRecvChannel.NotifyDataAvailable(iStatus);
	SetActive();
	}

void CModule::Dequeue()
/** Remove this from the linked list
*/
	{
	iLink.Deque();
	}

const TCFModuleNameF& CModule::Name() const
/** Accessor method for the module name
@return The name
*/
	{
	return iName;
	}

const TBool& CModule::Initialising() const
/** Accessor method for the module initialisation state
@return True if module is still initialising
*/
	{
	return iInitialising;
	}

void CModule::SetInitialising(const TBool aInitialising)
/** Set method for the module initialisation state
@param aInitialising set true if module is initialising
*/
	{
	iInitialising = aInitialising;
	}

CModule::CModule(MBindManagerNotify *aNotifier, 
				 TransactionId &aTransactionId,
				 const TCFModuleNameF &aModule) :
	CActive(EPriorityStandard),
	iNotifier(aNotifier),
	iName(aModule),
	iInitialising(ETrue),
	iTaskList(_FOFF(CTask, iLink)),
	iTransactionId(aTransactionId)
/** Constructor for CModule
@param aNotifier bindmanager object to report events to
@param aTransactionId Generator of unique transaction ids
@param aModule the name of the module
*/
	{
	RS_DETLOG((KLogSubSysRS, KLogEvent, _L8("CModule::CModule(%X %S)"), this, &iName));
	}

void CModule::SendL(const TCFMessage &aMessage)
/** Public method to send a message to a comms server or CPM
@param aMessage the message to be sent
@leave KErrNoMemory
*/
	{
	// put msg on send queue
	User::LeaveIfError(iSendQueue->Send(aMessage));
	}

void CModule::DoDiscoverL(TUint &aTaskId)
/** Creates a task to send a discovery request to the module and sends the
message itself
@param aTaskId filled in with the id of the new task
@leave KErrNoMemory
*/
	{

	TCFDiscoverMsg discoverMsg(GetNextTransactionId(), ETrue, KDefaultSubmoduleArraySize, iDiscoveryArray);
	CTask* pTask = CTask::NewL(discoverMsg, discoverMsg.Identifier());
    CleanupStack::PushL(pTask);
	aTaskId = pTask->TaskId();

	// put msg on send queue
	SendL(discoverMsg);

    CleanupStack::Pop(pTask);
	iTaskList.AddLast(*pTask);
	}


TBool CModule::GetBindTaskL(const RCFChannel::TMsgQueues& aInputQueues, 
						    const RCFChannel::TMsgQueues& aOutputQueues,
						    const TCFSubModuleAddress& aSubModule1,
						    const TCFSubModuleAddress& aSubModule2,
						    const TCFBindType aType,
						    CTask*& aTask)
/** Either creates a task to send a bind request or returns an existing task
if a matching one already exists
@param aInputQueues queues for comms between the modules being bound
@param aOutputQueues queues for comms between the modules being bound
@param aSubModule1 first submodule being bound
@param aSubModule2 second submodule being bound
@param aType the type of binding
@param aTask set to point to the task
@return Etrue if a new task was created
@leave KErrNoMemory
*/
	{
	TBool newTask = EFalse;

	// see if this task already exists
	TCFBindMsg bindtest(0, aInputQueues, aOutputQueues, &aSubModule1, &aSubModule2, aType);
    TDblQueIter<CTask> taskIter(iTaskList);
	CTask *pTask;
	while ((pTask = taskIter++) != NULL)
		{
		if (pTask->CheckIfSame(bindtest))
			{
			break;
			}
		}

	// need to create a new one
	if (!pTask)
		{
		TCFBindMsg bindmsg(GetNextTransactionId(), aInputQueues, aOutputQueues, &aSubModule1, &aSubModule2, aType);
		pTask = CTask::NewL(bindmsg, bindmsg.Identifier());
		newTask = ETrue;
		}

	aTask = pTask;
	return newTask;
	}


TBool CModule::GetUnbindTaskL(const TCFSubModuleAddress& aSubModule1,
							  const TCFSubModuleAddress& aSubModule2,
							  const TBool aPeerIsDead,
							  CTask*& aTask)
/** Either creates a task to send an unbind request or returns an existing task
if a matching one already exists
@param aSubModule1 first submodule being unbound
@param aSubModule2 second submodule being unbound
@param aPeerIsDead indicates whether the second module is dead or not
@param aTask the task
@return Etrue if a new task was created
@leave KErrNoMemory
*/
	{
	TBool newTask = EFalse;

	// see if this task already exists
	TCFUnbindMsg unbindtest(0, &aSubModule1, &aSubModule2, aPeerIsDead);

    TDblQueIter<CTask> taskIter(iTaskList);
	CTask *pTask;
	while ((pTask = taskIter++) != NULL)
		{
		if (pTask->CheckIfSame(unbindtest))
			{
			break;
			}
		}
		
	if (!pTask)
		{
		TCFUnbindMsg unbindmsg(GetNextTransactionId(), &aSubModule1, &aSubModule2, aPeerIsDead);
		pTask = CTask::NewL(unbindmsg, unbindmsg.Identifier());
		newTask = ETrue;
		}

	aTask = pTask;
	return newTask;
	}

void CModule::DoStartTaskL(CTask* aTask)
/** Sends the task's message and adds it the the task in progress list
@param aTask the task to process
@leave KErrNoMemory
*/
	{
	__ASSERT_DEBUG(aTask, User::Panic(KSpecAssert_ElemRootServerbm, 1));	// Reassures LINT's ptr tracking
	SendL(aTask->Message());
	iTaskList.AddLast(*aTask);
	}

	
TInt CModule::EnumerateSubModules(const TInt& aPosition, TCFSubModuleNameF& aModuleName)
/** Enumerate all known submodules.
@param aPosition The position in the array of submodulenames.
@param aModuleName The name to be returned
@return Error code.
*/
    {
    if(aPosition>=iSubModuleNames.Count())
    	return KErrEof;
   	aModuleName = iSubModuleNames[aPosition];
    return KErrNone;
    }
	
TInt CModule::NumSubModules() const
/** Returns the number of submodules within this module
@return Number of submodules within this module
*/	
	{
	return iSubModuleNames.Count();
	}


TBool CModule::HasSubModule(const TCFSubModuleNameF &aSubModule) const
/** Searches for a specified submodule name within this module
@param aSubModule the name of the submodule to search for
@return Etrue if the submodule is found
*/
	{
	TInt i;
	TBool found = EFalse;
	for (i=0; i<iSubModuleNames.Count(); i++)
		{
		if (iSubModuleNames[i] == aSubModule)
			{
			found =  ETrue;
			break;
			}
		}
	return found;
	}


const TCFSubModuleAddress& CModule::SubModuleNameRefL(const TCFSubModuleAddress& aSubModule)
/** Provides a reference to a local copy of the submodule name passed as a 
parameter which is guaranteed to last the lifetime of the cmodule
@param aSubModule the name of the submodule
@return Reference to the internal submodule address
@leave KErrNoMemory
*/
	{
	TInt i;
	TBool found = EFalse;
	for (i=0; i<iSubModuleAddrs.Count(); i++)
		{
		if (iSubModuleAddrs[i] == aSubModule)
			{
			found = ETrue;
			break;
			}
		}

	if (!found)
		{
		User::LeaveIfError(iSubModuleAddrs.Append(aSubModule));
		}

	return iSubModuleAddrs[i];
	}


void CModule::EndTask(const TUint &aTaskId, const TInt aStatus)
/** Called by bindmanager to indicate that the task with the specified id has
completed
@param aTaskId id of the finished task
@param aStatus the status code with which the task has ended
*/
	{
	RS_DETLOG((KLogSubSysRS, KLogEvent, _L8("CModule(%X - %S)::EndTask(%d, %d)"), this, &iName, aTaskId, aStatus));
    TDblQueIter<CTask> taskIter(iTaskList);
	CTask *pTask;
	while ((pTask = taskIter++) != NULL)
		{
		if (pTask->TaskId() == aTaskId)
			{
			break;
			}
		}
		
	if (pTask)
		{
		iNotifier->TaskCompleted(*this, *pTask, aStatus);
		delete pTask; 
		// note if a reply arrives now, it's too late! msg should be discarded
		}
	}
	
void CModule::EndAllTasks(const TInt aStatus)
/** Called by bindmanager to end all tasks for this module.  Called on receipt of a
moduleEnded request
@param aStatus the status code with which the tasks will be ended
*/
	{
	__CFLOG_1(KLogSubSysRS, KLogEvent, _L8("CModule::EndAllTasks - cancelling any outstanding tasks for %S"), &iName);
    TDblQueIter<CTask> taskIter(iTaskList);
	CTask *pTask;
	while ((pTask = taskIter++) != NULL)
		{
		iNotifier->TaskCompleted(*this, *pTask, aStatus);
		delete pTask;
		}
	}

CModule::~CModule()
/** Destructor for CModule
*/
	{
	RS_DETLOG((KLogSubSysRS, KLogEvent, _L8("CModule::~CModule(%X %S)"), this, &iName));
	Cancel();
	Dequeue();
	delete iSendQueue;
	if(iDiscoveryArray)
		{
		delete[] iDiscoveryArray;
		}

	iSendChannel.Close();
	iRecvChannel.Close();

	iSubModuleNames.Close();
	iSubModuleAddrs.Close();

    TDblQueIter<CTask> taskIter(iTaskList);
	CTask *pTask;
	while ((pTask = taskIter++) != NULL)
		{
		delete pTask;
		}
	}

TUint CModule::GetNextTransactionId()
/** Generate a transaction id 'unique' to this module. Uses a straightforward 32 bit
count so will wrap eventually.
@return New transaction identifier
*/
	{
	TId iNextTransactionId = iTransactionId.GetId();
	if(iNextTransactionId==static_cast<TId>(CTask::ENoReplyExpected))
		{
		iNextTransactionId = iTransactionId.GetId();
		}
	return iNextTransactionId;
	}

TBool CModule::CheckDiscoveryMsgFinishedL( const TCFMessage& aMsg )
	{
	TBool retCode = ETrue;
	if(aMsg.Code()==TCFCommsMessage::ECodeDiscoverResp)
		{
		const TCFDiscoverRespMsg& discoverRespMsg =(reinterpret_cast<const TCFDiscoverRespMsg&>(aMsg));

		if(discoverRespMsg.ReturnCode()>0)
			{
			__ASSERT_DEBUG(iDiscoveryArray, User::Panic(KSpecAssert_ElemRootServerbm, 2));	// reassure LINT
			for(TInt i=0;i<discoverRespMsg.ReturnCode();i++)
				{
				iSubModuleNames.Append(iDiscoveryArray[i]);
				}
			if(discoverRespMsg.MoreToWrite())
				{
				TCFDiscoverMsg discoverMsg(discoverRespMsg.Identifier(), EFalse, KDefaultSubmoduleArraySize, iDiscoveryArray);		
				// put msg on send queue
				SendL(discoverMsg);
				retCode = EFalse;
				}
			else
				{
				delete[] iDiscoveryArray;
				iDiscoveryArray = NULL;
				}
			}
		}
	return retCode;
	}

