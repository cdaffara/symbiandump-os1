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
 @file te_TransportStep.cpp
*/
#include "te_TransportStep.h"
#include <es_mbuf.h>
#include <comms-infras/mbufchain.h>
#include <comms-infras/mbuf.h>
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
#include <comms-infras/commsbufpondop.h>
#else
#include <es_mbman.h>
#endif
#include <elements/cfmacro.h>
#include <elements/nm_address_internal.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemCommsFwtrnsp, "ElemCommsFwtrnsp");
#endif

__CFLOG_DECLARATION_MEMBER;

//
//************************ Global data to be passed between Threads ************************//
//

// Time interval specified by config used for setting time outs
TInt KTimeInterval;

// Number of Slots in message Queue, specified by Config 
TInt KSlots;

// Array to keep track of thread exit values
TInt exitArray[KTotalThreads];

//Array used to ensure correct heap cleanup within worker threads
TInt memoryArray[KMemoryTestDimensions][KMemorySlots];

//Used to iterate through the memory array
TInt memoryItr;

// journal for keeping track of thread transport activity
TInt journal[KTotalThreads][KJournalSlots];

// Struct containing cookie information
TGlobalInfo globalInfo;

// array of Heap ptr's used to manipulate worker thread heaps from control thread
RHeap* threadHeapPtr[KTotalThreads];

// request status used to sync worker threads with control thread
TRequestStatus statusThread1;

// request status used to sync worker threads with control thread
TRequestStatus statusThread2;


//
//******************************* BASIC MESSAGES *************************************//
//


TCFBaseImpl::TCFBaseImpl()
	{
	}

void TCFBaseImpl::Error(const Messages::TRuntimeCtxId& /*aSender*/, const Messages::TRuntimeCtxId& /*aRecipient*/, TInt /*aError*/)
	{
	}
 	
void TCFBaseImpl::DispatchL(const Messages::TRuntimeCtxId& /*aSender*/, const Messages::TRuntimeCtxId& /*aRecipient*/)
	{
 	__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 1));
 	}
 
 
//
//****************************** SIMPLE DISPATCH *************************************//
//


START_ATTRIBUTE_TABLE( TSimpleDispatch, EMBasicItfId, MBasicItf::ESimpleDispatch )
	REGISTER_ATTRIBUTE( TSimpleDispatch, iId, TMetaNumber )
    REGISTER_ATTRIBUTE( TSimpleDispatch, iMsg, TMeta<TNormalMessage> )
END_ATTRIBUTE_TABLE()

DEFINE_MVIP_CTR(TSimpleDispatch);

inline TSimpleDispatch::TSimpleDispatch()
	{
	}

inline TSimpleDispatch::TSimpleDispatch(TInt aId, TNormalMessage& aMsg) 
	: iId(aId), iMsg(aMsg)
	{
	}
	
inline void TSimpleDispatch::DispatchL(const Messages::TRuntimeCtxId& /*aSender*/, const Messages::TRuntimeCtxId& aRecipient)
	{
	const Messages::TNodeId& nodeId = Messages::address_cast<Messages::TNodeId>(aRecipient);
	__ASSERT_DEBUG(nodeId.Ptr(), User::Panic(KSpecAssert_ElemCommsFwtrnsp, 2));
	TAny* extnItf = static_cast<TInterfaceControl*>(nodeId.Ptr())->FetchInterfaceL(EMBasicItfId);
	reinterpret_cast<MBasicItf*>(extnItf)->SimpleDispatch(iId);
	}


//
//******************************* NULL DISPATCH **************************************//
//



START_ATTRIBUTE_TABLE( TNullDispatch, EMBasicItfId, MBasicItf::ENullDispatch )
	REGISTER_ATTRIBUTE( TNullDispatch, iId, TMetaNumber )
    REGISTER_ATTRIBUTE( TNullDispatch, iMsg, TMeta<TNormalMessage> )
END_ATTRIBUTE_TABLE()

DEFINE_MVIP_CTR(TNullDispatch);

inline TNullDispatch::TNullDispatch()
	{
	}

inline TNullDispatch::TNullDispatch(TInt aId, TNormalMessage& aMsg) 
	: iId(aId), iMsg(aMsg)
	{
	}
	
inline void TNullDispatch::DispatchL(const Messages::TRuntimeCtxId& /*aSender*/, const Messages::TRuntimeCtxId& aRecipient)
	{
	const Messages::TNodeId& nodeId = Messages::address_cast<Messages::TNodeId>(aRecipient);
	__ASSERT_DEBUG(nodeId.Ptr(), User::Panic(KSpecAssert_ElemCommsFwtrnsp, 3));
	TAny* extnItf = reinterpret_cast<TInterfaceControl*>(nodeId.Ptr())->FetchInterfaceL(EMBasicItfId);
	reinterpret_cast<MBasicItf*>(extnItf)->NullDispatch(iId);
	}


START_ATTRIBUTE_TABLE( TLargeMessage, TLargeMessage::KItfId, TLargeMessage::KFn1Id )
	REGISTER_ATTRIBUTE( TLargeMessage, iBuf, TMeta<TLarge> )
END_ATTRIBUTE_TABLE()


START_ATTRIBUTE_TABLE( TNormalMessage, TNormalMessage::KItfId, TNormalMessage::KFn1Id )
	REGISTER_ATTRIBUTE( TNormalMessage, iBuf, TMeta<TNormal> )
END_ATTRIBUTE_TABLE()


const TImplementationProxy BasicImplementationTable[] = 
	{
	MVIP_CTR_ENTRY(MBasicItf::ESimpleDispatch, TSimpleDispatch),
	MVIP_CTR_ENTRY(MBasicItf::ENullDispatch, TNullDispatch),
	};

//
//*************************** StartResp MESSAGES *************************************//
//


START_ATTRIBUTE_TABLE( TStartRespMsg, EMLinkItfId, MLinkItf::EStartResp )
	REGISTER_ATTRIBUTE( TStartRespMsg, iError, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( Messages::TSignalBase, 0)

DEFINE_MVIP_CTR(TStartRespMsg)


inline TStartRespMsg::TStartRespMsg()
	{ 
	}

inline TStartRespMsg::TStartRespMsg(TInt aError) :
	iError(aError)
	{ 
	}

inline void TStartRespMsg::DispatchL(const Messages::TRuntimeCtxId& /*aSender*/, const Messages::TRuntimeCtxId& aRecipient)
	{
	const Messages::TNodeId& nodeId = Messages::address_cast<Messages::TNodeId>(aRecipient);
	__ASSERT_DEBUG(nodeId.Ptr(), User::Panic(KSpecAssert_ElemCommsFwtrnsp, 4));
	TAny* extnItf = reinterpret_cast<TInterfaceControl*>(nodeId.Ptr())->FetchInterfaceL(EMLinkItfId);
	reinterpret_cast<MLinkItf*>(extnItf)->StartRespL(iError);
	}
	
inline void TStartRespMsg::Error(const Messages::TRuntimeCtxId& /*aSender*/, const Messages::TRuntimeCtxId& /*aRecipient*/, TInt /*aError*/)
	{
	}


//
//*************************** DISPATCH MESSAGES **************************************//
//

START_ATTRIBUTE_TABLE( TLargeDispatchThread1, EMLinkItfId, MLinkItf::ELargeDispatch1 )
	REGISTER_ATTRIBUTE( TLargeDispatchThread1, iBuf, TMeta<TSmall> )
END_ATTRIBUTE_TABLE()

DEFINE_MVIP_CTR(TLargeDispatchThread1)

inline TLargeDispatchThread1::TLargeDispatchThread1()
	{
	}

inline TLargeDispatchThread1::TLargeDispatchThread1(const TDesC& aBuf) 
: iBuf(aBuf)
	{
	}

inline void TLargeDispatchThread1::DispatchL(const Messages::TRuntimeCtxId& /*aSender*/, const Messages::TRuntimeCtxId& aRecipient)
	{
	const Messages::TNodeId& nodeId = Messages::address_cast<Messages::TNodeId>(aRecipient);
	__ASSERT_DEBUG(nodeId.Ptr(), User::Panic(KSpecAssert_ElemCommsFwtrnsp, 5));
	
	TAny* extnItf = reinterpret_cast<TInterfaceControl*>(nodeId.Ptr())->FetchInterfaceL(EMLinkItfId);
	reinterpret_cast<MLinkItf*>(extnItf)->LargeDispatchThread1(iBuf);
	}



START_ATTRIBUTE_TABLE( TLargeDispatchThread2, EMLinkItfId, MLinkItf::ELargeDispatch2 )
	REGISTER_ATTRIBUTE( TLargeDispatchThread2, iBuf, TMeta<TSmall> )
END_ATTRIBUTE_TABLE()

DEFINE_MVIP_CTR(TLargeDispatchThread2)

inline TLargeDispatchThread2::TLargeDispatchThread2()
	{ 
	}

inline TLargeDispatchThread2::TLargeDispatchThread2(const TDesC& aBuf) 
: iBuf(aBuf)
	{ 
	}


inline void TLargeDispatchThread2::DispatchL(const Messages::TRuntimeCtxId& /*aSender*/, const Messages::TRuntimeCtxId& aRecipient)
	{
   	const Messages::TNodeId& nodeId = Messages::address_cast<Messages::TNodeId>(aRecipient);
	__ASSERT_DEBUG(nodeId.Ptr(), User::Panic(KSpecAssert_ElemCommsFwtrnsp, 6));

	TAny* extnItf = reinterpret_cast<TInterfaceControl*>(nodeId.Ptr())->FetchInterfaceL(EMLinkItfId);
	reinterpret_cast<MLinkItf*>(extnItf)->LargeDispatchThread2(iBuf);
	}
 
 
START_ATTRIBUTE_TABLE( TProvisionRequestMsg, EMLinkItfId, MLinkItf::EProvisionRequest )
    REGISTER_ATTRIBUTE( TProvisionRequestMsg, iParams, TMeta<TLarge8>)
END_ATTRIBUTE_TABLE()

DEFINE_MVIP_CTR(TProvisionRequestMsg)

TProvisionRequestMsg::TProvisionRequestMsg()
	{
	}

TProvisionRequestMsg::TProvisionRequestMsg(const Meta::SMetaData& aParams)
	{
	aParams.Store(iParams);
	}

inline void TProvisionRequestMsg::DispatchL(const Messages::TRuntimeCtxId& /*aSender*/, const Messages::TRuntimeCtxId& aRecipient)
	{
	const Messages::TNodeId& nodeId = Messages::address_cast<Messages::TNodeId>(aRecipient);
	__ASSERT_DEBUG(nodeId.Ptr(), User::Panic(KSpecAssert_ElemCommsFwtrnsp, 7));
	
	TAny* extnItf = reinterpret_cast<TInterfaceControl*>(nodeId.Ptr())->FetchInterfaceL(EMLinkItfId);
	TPtrC8 ptr(iParams);
	reinterpret_cast<MLinkItf*>(extnItf)->ProvisionRequest(ptr);
	}


const TImplementationProxy LinkImplementationTable[] = 
	{
	MVIP_CTR_ENTRY(MLinkItf::EStartResp, TStartRespMsg),
	MVIP_CTR_ENTRY(MLinkItf::ELargeDispatch1, TLargeDispatchThread1),
	MVIP_CTR_ENTRY(MLinkItf::ELargeDispatch2, TLargeDispatchThread2),
	MVIP_CTR_ENTRY(MLinkItf::EProvisionRequest, TProvisionRequestMsg),
	};


//
//****************************** INTERFACE SENDERSIDE ********************************//
//


RLinkItf::RLinkItf() 
	{
	}

/*RLinkItf::RLinkItf(MTransportReceiver& aReceiver) : 
	RBasicItf(aReceiver)
	{
	}*/

void RLinkItf::StartResp(TInt aError)
	{
	PostMessage(RecipientId(), TStartRespMsg(aError));
	}

void RLinkItf::DispatchThread1(const TDesC& aBuf)
	{
	PostMessage(RecipientId(), TLargeDispatchThread1(aBuf));
	}
	
void RLinkItf::DispatchThread2(const TDesC& aBuf)
	{
	PostMessage(RecipientId(), TLargeDispatchThread2(aBuf));
	}

void RLinkItf::ProvisionRequestL(const Meta::SMetaData& aParams)
	{
	PostMessage(RecipientId(), TProvisionRequestMsg(aParams));
	}

//
//**************************************** RBasicItf ******************************************//
//


RBasicItf::RBasicItf() 
	{ 
	}
	
void RBasicItf::SimpleDispatch(TInt aInt)
	{
	TNormalMessage msg;
	msg.iBuf.Copy(KMeta);
//	ASSERT(0);
	PostMessage(RecipientId(), TSimpleDispatch(aInt, msg));
	}


void RBasicItf::NullDispatch(TInt aInt)
	{
	TNormalMessage msg;
	msg.iBuf.Copy(KMeta);
//	ASSERT(0);
	PostMessage(RecipientId(), TNullDispatch(aInt, msg));
	}



/*RBasicHookItf::RBasicHookItf(const MTransportReceiver& aTransport) 
:	RCFHookInterfaceBase(aTransport)
	{ 
	}

void RBasicHookItf::NullDispatch(TInt aInt)
	{
	TNormalMessage msg;
	msg.iBuf.Copy(KMeta);
	PostMeta(TNullDispatch(aInt), msg);
	}*/


//
//************************** RECIEVER-SIDE IMPLEMENTATION ****************************//
//

CItfTestImpl::CItfTestImpl() 
:   TIfStaticFetcherNearestInHierarchy(this) //, iThread1State(0), iThread2State(0), iJournalPosition(0)
	{
	}

void CItfTestImpl::SetJournalPosition(TInt aJournalPosition)
	{
	iJournalPosition = aJournalPosition;
	}

void CItfTestImpl::ReturnInterfacePtrL(MBasicItf*& aInterface)
	{
	aInterface = this;
	}

void CItfTestImpl::ReturnInterfacePtrL(MLinkItf*& aInterface)
	{
	aInterface = this;
	}

void CItfTestImpl::SimpleDispatch(TInt aId)
//Simple dispatch of an EObjDispatchMessage
	{
	RMutex mutex;
	OpenMutexAndWait(mutex);
	journal[KThread1Hash][0] = aId;
	SignalMutexAndClose(mutex);
	CActiveScheduler::Stop();
	}

void CItfTestImpl::NullDispatch(TInt aId)
//Null dispatch of an EObjDispatchMessage
	{
	RMutex mutex;
	OpenMutexAndWait(mutex);
	journal[iJournalPosition][0] = aId;
	SignalMutexAndClose(mutex);
	CActiveScheduler::Stop();
	}

void CItfTestImpl::StartRespL(TInt /*aError*/)
	{
	}

void CItfTestImpl::OpenMutexAndWait(RMutex& aMutex)
	{
	TInt err = aMutex.OpenGlobal(KMutex);
	__ASSERT_DEBUG(err == KErrNone, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 8));
	aMutex.Wait();
	}
	
void CItfTestImpl::SignalMutexAndClose(RMutex& aMutex)
	{
	aMutex.Signal();
	aMutex.Close();
	}


void CItfTestImpl::LargeDispatchThread1(const TDesC& aBuf)
//Dispatch of EObjectDispatch messages - deadlocked OOM Case, Checks contents of Msg and then
//writes to journal
	{
	RMutex mutex;
	OpenMutexAndWait(mutex);
	iThread1State++;
	switch(iThread1State)
		{
		case 1:
			journal[KThread1Hash][0] = (aBuf.Compare(KSmall4) == 0) ? EMessageRecieved : EBadMessageRecieved;
			break;
		case 2:
			journal[KThread1Hash][1] = (aBuf.Compare(KSmall3) == 0) ? EMessageRecieved : EBadMessageRecieved;
			break;
		case 3:
			journal[KThread1Hash][2] = (aBuf.Compare(KSmall2) == 0) ? EMessageRecieved : EBadMessageRecieved;
			break;
		case 4:
			journal[KThread1Hash][3] = (aBuf.Compare(KSmall1) == 0) ? EMessageRecieved : EBadMessageRecieved;
			CActiveScheduler::Stop();
			break;
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 9));
			break;
		}
	SignalMutexAndClose(mutex);
	}
	
	
void CItfTestImpl::LargeDispatchThread2(const TDesC& aBuf)
//Dispatch of EObjectDispatch messages - deadlocked OOM Case, Checks contents of Msg and then
//writes to journal
	{
	RMutex mutex;
	OpenMutexAndWait(mutex);
	iThread2State++;
	switch(iThread2State)
		{
		case 1:
			journal[KThread2Hash][0] = (aBuf.Compare(KSmall1) == 0) ? EMessageRecieved : EBadMessageRecieved;
			break;
		case 2:
			journal[KThread2Hash][1] = (aBuf.Compare(KSmall2) == 0) ? EMessageRecieved : EBadMessageRecieved;
			break;
		case 3:
			journal[KThread2Hash][2] = (aBuf.Compare(KSmall3) == 0) ? EMessageRecieved : EBadMessageRecieved;
			break;
		case 4:
			journal[KThread2Hash][3] = (aBuf.Compare(KSmall4) == 0) ? EMessageRecieved : EBadMessageRecieved;
			CActiveScheduler::Stop();
			break;
		default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 10));
		 	break;
		}
	SignalMutexAndClose(mutex);
	}
	

void CItfTestImpl::ProvisionRequest(TPtrC8& aMetaParams)
	{
	STypeId typeId;
	Mem::Copy(&typeId.iUid, aMetaParams.Ptr(), sizeof(TUid));
	aMetaParams.Set(aMetaParams.Ptr()+sizeof(TUint32),aMetaParams.Length()-sizeof(TUint32)); //update pointer (uid)
	Mem::Copy(&typeId.iType, aMetaParams.Ptr(), sizeof(TInt32));
	aMetaParams.Set(aMetaParams.Ptr()+sizeof(TUint32),aMetaParams.Length()-sizeof(TUint32)); //update pointer (type)
	TLargeMessage msg;
	msg.Load(aMetaParams);
	if(msg.iBuf.Compare(KBlob) == 0)
		{
		RMutex mutex;
		OpenMutexAndWait(mutex);
		journal[KThread1Hash][0] = EBlobRecievedCorrectly;
		SignalMutexAndClose(mutex);
		}
	CActiveScheduler::Stop();
	}
	
//
//****************** Active object to stop the Scheduler after a set amount of time *******************//
//

CSchedulerStopper::~CSchedulerStopper()
	{
	Cancel();
	iTimer.Close();
	}
	
CSchedulerStopper* CSchedulerStopper::NewL()
	{
	CSchedulerStopper* self = CSchedulerStopper::NewLC();
	CleanupStack::Pop(self);
	return self;
	}
	
CSchedulerStopper* CSchedulerStopper::NewLC()
	{
	CSchedulerStopper* self = new (ELeave) CSchedulerStopper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
void CSchedulerStopper::RunL()
	{
	CActiveScheduler::Stop();
	}
void CSchedulerStopper::StartL()
	{
	iTimer.After(iStatus, KTimeInterval);
	SetActive();
	}
void CSchedulerStopper::Cancel()
	{
	DoCancel();
	}
CSchedulerStopper::CSchedulerStopper() : CActive(EPriorityHigh)
	{
	}
void CSchedulerStopper::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
	}
void CSchedulerStopper::DoCancel()
	{
	iTimer.Cancel();
	}

//
//************************** Class definations for CTransport ****************************//
//

void CTransport::OpenSemaphores(RSemaphore& aSem1, RSemaphore& aSem2)
	{
	TInt err = aSem1.OpenGlobal(KSem1);
	__ASSERT_DEBUG(err == KErrNone, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 11));
	err = aSem2.OpenGlobal(KSem2);
	__ASSERT_DEBUG(err == KErrNone, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 12));
	}

void CTransport::OpenMutexAndWait(RMutex& aMutex)
	{
	TInt err = aMutex.OpenGlobal(KMutex);
	__ASSERT_DEBUG(err == KErrNone, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 13));
	aMutex.Wait();
	}
	
void CTransport::SignalMutexAndClose(RMutex& aMutex)
	{
	aMutex.Signal();
	aMutex.Close();
	}

void CTransport::ConstructL(TWorkerParams& aParams, Meta::CMetaDataVirtualCtorInPlace* aVCtor)
	{
	iJournalCounter = 0;
	iActSched = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActSched); 
	iThreadRegister = TWorkerRegister::NewL(aParams.iWorkerId, &aParams.iThreadGlobals);
	iPeerId = aParams.iPeerId;
	//iHook = new (ELeave) CCFTransportHooks();
	iTransport = CCommsTransport::NewL(*iThreadRegister, aVCtor, NULL);
	TWorkerThreadDataBase* selfData = iThreadRegister->GetSelfWorkerGlobals();
	selfData->Init(aParams.iWorkerId, ETrue);
	User::LeaveIfError(iTransport->EstablishTransportToPeer(iPeerId, static_cast<const RCFChannelMsgQueues&>(aParams.iInQueues), 
				static_cast<const RCFChannelMsgQueues&>(aParams.iOutQueues)));
	}

RHeap& CTransport::__START_HEAPCHECK()
// Starts HeapChecking on threads
	{
	RHeap& heap = User::Heap();
	heap.__DbgMarkStart();
	return heap;
	}
	
void CTransport::__END_HEAPCHECK(RHeap& aHeap)
// Ends HeapChecking on threads and records results
	{
	TUint32 value;
	value = aHeap.__DbgMarkEnd(0);
	RMutex mutex;
	TInt err = mutex.OpenGlobal(KMutex);
	__ASSERT_DEBUG(err == KErrNone, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 14));
	mutex.Wait();
	if(value == 0)
		{
		memoryArray[KMemoryTestResultHash][memoryItr] = ENoMemoryLeak;
		memoryArray[KOrphanedMemoryHash][memoryItr++] = value;
		}
	else
		{
	 	memoryArray[KMemoryTestResultHash][memoryItr] = EMemoryLeak;
		memoryArray[KOrphanedMemoryHash][memoryItr++] = value;
		}
	mutex.Signal();
	mutex.Close();
	}

CTransport::~CTransport()
	{
	User::LeaveIfError(iTransport->DropTransportToPeer(iPeerId));
	delete iThreadRegister;
	delete iTransport;
	CActiveScheduler::Install(NULL);
	// delete iHook; the hook is deleted by the iTransport
	delete iActSched; 
	}	

//
//************************* LEGACY TEST CASE, CLASS DEFINATIONS *******************************//
//


/* ------------------ CRecieve ---------------------- */
void CRecieve::DispatchL(const TCFMessage& aMessage, TWorkerId /*aSenderId*/)
// CRecieve used as standard reciever in the following functions
// NET-COMNF-FWORK-770001, NET-COMNF-FWORK-770003
	{
 	RMutex mutex;
 	OpenMutexAndWait(mutex);
 	journal[KThread1Hash][iJournalCounter] = aMessage.Code();
 	SignalMutexAndClose(mutex);
 	iJournalCounter++;
 	if(iJournalCounter>=3)
 		{
 		CActiveScheduler::Stop();
 		}
	}
	
void CRecieve::EntryPoint(TWorkerParams* aParams)
	{
 	RHeap& heap = __START_HEAPCHECK();
 	CTrapCleanup* cleanup = CTrapCleanup::New();
 	CRecieve* self;
 	TInt ret;
 	TRAP(ret,self = CRecieve::NewLC(*aParams);
  			  self->DoItL();
  			  CleanupStack::PopAndDestroy(self);
 		 );
 	delete cleanup;
 	__END_HEAPCHECK(heap);
	}
	
CRecieve* CRecieve::NewLC(TWorkerParams& aParams)		
	{
 	CRecieve* self = new (ELeave) CRecieve;
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams);	
 	return self;
	}	


void CRecieve::DoItL()
//contains functionality for the reading and dispatching of messages
	{
 	iTransport->RegisterLegacyInterface(static_cast<MLegacyMessageReceiver*>(this));
	globalInfo.iThread2LegacyWorkerId = iTransport->WorkerId();
 	RThread::Rendezvous(KErrNone);
 	CActiveScheduler::Start();

	globalInfo.iThread2LegacyWorkerId = KBadWorkerId;	
	}


/* ------------------ CDropTransportRecieve/NET-COMNF-FWORK-770019 ---------------------- */
void CDropTransportRecieve::EntryPoint(TWorkerParams* aParams)
	{
 	RHeap& heap = __START_HEAPCHECK();
 	CTrapCleanup* cleanup = CTrapCleanup::New();
 	CDropTransportRecieve* self;
 	TInt ret;
 	TRAP(ret,self = CDropTransportRecieve::NewLC(*aParams);
  			  self->DoItL();
  			  CleanupStack::PopAndDestroy(self);
 		 );
 	delete cleanup;
 	__END_HEAPCHECK(heap);
	}

void CDropTransportRecieve::DoItL()
//contains functionality for the reading and dispatching of messages
	{
 	iTransport->RegisterLegacyInterface(static_cast<MLegacyMessageReceiver*>(this));
	globalInfo.iThread2LegacyWorkerId = iTransport->WorkerId();
 	RThread::Rendezvous(KErrNone);
	}
CDropTransportRecieve* CDropTransportRecieve::NewLC(TWorkerParams& aParams)		
	{
 	CDropTransportRecieve* self = new (ELeave) CDropTransportRecieve;
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams);	
 	return self;
	}

// From MLegacyMessageReciever
void CDropTransportRecieve::DispatchL(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/)
	{
 	//Shouldn't get here!
 	if(iJournalCounter == 2)
 		CActiveScheduler::Stop();
 	iJournalCounter++;
	}

void CDropTransportRecieve::OnDispatchLeave(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/, TInt /*aFirstDispatchLeaveReason*/)
	{
	}


/* ------------------ CEmptyRecieve ---------------------- */
void CEmptyRecieve::EntryPoint(TWorkerParams* aParams)
	{
 	RHeap& heap = __START_HEAPCHECK();
 	CTrapCleanup* cleanup = CTrapCleanup::New();
 	CEmptyRecieve* self;
 	TInt ret;
 	TRAP(ret,self = CEmptyRecieve::NewLC(*aParams);
  			  self->DoItL();
  			  CleanupStack::PopAndDestroy(self);
 		 );
 	delete cleanup;
 	__END_HEAPCHECK(heap);
	}
	
CEmptyRecieve::~CEmptyRecieve()
	{
 	delete iStopper;
	}
	
void CEmptyRecieve::DoItL()
//contains functionality for the reading and dispatching of messages
	{
 	iTransport->SetLastRequestIdConcerningPeer(iPeerId, KReqId);
 	iTransport->RegisterLegacyInterface(static_cast<MLegacyMessageReceiver*>(this));
	globalInfo.iBadWorkerId = iTransport->WorkerId();
 	RThread::Rendezvous(KErrNone);
 	iStopper->StartL();
 	__ASSERT_DEBUG(iTransport->LastRequestIdConcerningPeer(iPeerId) == KReqId, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 15));
 	CActiveScheduler::Start();

	globalInfo.iBadWorkerId = KBadWorkerId;
	}

CEmptyRecieve* CEmptyRecieve::NewLC(TWorkerParams& aParams)		
	{
 	CEmptyRecieve* self = new (ELeave) CEmptyRecieve;
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams);	
 	self->iStopper = CSchedulerStopper::NewL();
 	return self;
	}

// From MLegacyMessageReciever
void CEmptyRecieve::DispatchL(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/)
	{
 	CActiveScheduler::Stop();
	}
void CEmptyRecieve::OnDispatchLeave(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/, TInt /*aFirstDispatchLeaveReason*/)
	{
	}

/* ------------------ CLeavingRecieve ---------------------- */
void CLeavingRecieve::OnDispatchLeave(const TCFMessage& aMessage, TWorkerId /*aSenderId*/, TInt aFirstDispatchLeaveReason)
// CLeavingRecieve used as reciver for dispatch that leaves
// NET-COMNF-FWORK-770020
	{
 	RMutex mutex;
 	OpenMutexAndWait(mutex);
 	journal[KThread1Hash][iJournalCounter] = aMessage.Code();
 	journal[KThread1Hash][5 + iJournalCounter] = aFirstDispatchLeaveReason;
 	SignalMutexAndClose(mutex);
 	iJournalCounter++;
 	if(iJournalCounter >= 3)
 		{
 		CActiveScheduler::Stop();
 		}
	}

void CLeavingRecieve::EntryPoint(TWorkerParams* aParams)
	{
 	RHeap& heap = __START_HEAPCHECK();
 	CTrapCleanup* cleanup = CTrapCleanup::New();
 	CLeavingRecieve* self;
 	TInt ret;
 	TRAP(ret,self = CLeavingRecieve::NewLC(*aParams);
  			  self->DoItL();
  			  CleanupStack::PopAndDestroy(self);
 		 );
 	delete cleanup;
 	__END_HEAPCHECK(heap);
	}

void CLeavingRecieve::DoItL()
//contains functionality for the reading and dispatching of messages
	{
	iTransport->RegisterLegacyInterface(static_cast<MLegacyMessageReceiver*>(this));
	globalInfo.iThread2LegacyWorkerId = iTransport->WorkerId();
 	RThread::Rendezvous(KErrNone);
 	CActiveScheduler::Start();

	globalInfo.iThread2LegacyWorkerId = KBadWorkerId;
	}
	
CLeavingRecieve* CLeavingRecieve::NewLC(TWorkerParams& aParams)		
	{
 	CLeavingRecieve* self = new (ELeave) CLeavingRecieve;
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams);	
 	return self;
	}
	
// From MLegacyMessageReciever
void CLeavingRecieve::DispatchL(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/)
	{
	User::Leave(-666);
	}

/* ------------------ CUnregisteredRecieve ---------------------- */
void CUnregisteredRecieve::EntryPoint(TWorkerParams* aParams)
	{
 	RHeap& heap = __START_HEAPCHECK();
 	CTrapCleanup* cleanup = CTrapCleanup::New();
 	CUnregisteredRecieve* self;
 	TInt ret;
 	TRAP(ret, self = CUnregisteredRecieve::NewLC(*aParams); 
 				self->DoItL();
 				CleanupStack::PopAndDestroy(self);
 				);
 	delete cleanup;
 	__END_HEAPCHECK(heap);
	}

CUnregisteredRecieve::~CUnregisteredRecieve()
	{
	delete iStopper;
	}

void CUnregisteredRecieve::DoItL()
	{
 	iStopper->StartL();
 	RThread::Rendezvous(KErrNone);
 	CActiveScheduler::Start();
	}

CUnregisteredRecieve* CUnregisteredRecieve::NewLC(TWorkerParams& aParams)
	{
 	CUnregisteredRecieve* self = new (ELeave) CUnregisteredRecieve();
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams);
 	self->iStopper =  CSchedulerStopper::NewL();
 	return self;	
	} 

void CUnregisteredRecieve::DispatchL(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/)
	{
	//shouldn't get here
	__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 16));
	}


/* ------------------ CBusyReciever ---------------------- */
void CBusyReciever::DispatchL(const TCFMessage& aMessage, TWorkerId /*aSenderId*/)
// CBusyReciever dispatcher, adds one message to journal then stops the scheduler
// NET-COMNF-FWORK-770005, NET-COMNF-FWORK-770004
	{
 	if(aMessage.Code() == TCFCommsMessage::ECodeForward)
 		{	
 		RMutex mutex;
 		OpenMutexAndWait(mutex);
 		journal[KThread1Hash][0] = EMessageRecieved;
 		SignalMutexAndClose(mutex);
 		}
 	CActiveScheduler::Stop();
	}
	
void CBusyReciever::EntryPoint(TWorkerParams* aParams)
	{
 	RHeap& heap = __START_HEAPCHECK();
 	CTrapCleanup* cleanup = CTrapCleanup::New();
 	CBusyReciever* self;
 	TInt ret;
 	TRAP(ret,self = CBusyReciever::NewLC(*aParams);	
 				self->DoItL(KTimeInterval);
 				CleanupStack::PopAndDestroy(self);
 				);
 	delete cleanup;
 	__END_HEAPCHECK(heap);
	}
	
void CBusyReciever::DoItL(TInt aTimeInterval)
//contains functionality for the reading and dispatching of messages
//Takes a Time interval from Config File
	{
	iTransport->RegisterLegacyInterface(static_cast<MLegacyMessageReceiver*>(this));
	globalInfo.iThread2LegacyWorkerId = iTransport->WorkerId();
 	RThread::Rendezvous(KErrNone);
 	User::After(aTimeInterval*10000);
 	CActiveScheduler::Start();

	globalInfo.iThread2LegacyWorkerId = KBadWorkerId;	
	}
	
CBusyReciever* CBusyReciever::NewLC(TWorkerParams& aParams)		
	{
 	CBusyReciever* self = new (ELeave) CBusyReciever;
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams);
 	self->iStopper = CSchedulerStopper::NewL();	
 	return self;
	}
	
CBusyReciever::~CBusyReciever()
	{
	delete iStopper;
	}
		
/* ------------------  ---------------------- */
void CSend::DoItL()
// Simple send of Legacy Messages used in Test Cases
// NET-COMNF-FWORK-770015, NET-COMNF-FWORK-770001, NET-COMNF-FWORK-770003,NET-COMNF-FWORK-770016, 
// NET-COMNF-FWORK-770018, NET-COMNF-FWORK-770019, NET-COMNF-FWORK-770020
	{
	RThread::Rendezvous(KErrNone);
	
 	TCFCommsMessage msg1(TCFCommsMessage::ECodeForward);
 	TCFCommsMessage msg2(TCFCommsMessage::ECodeShutdown);
 	TCFCommsMessage msg3(TCFCommsMessage::ECodeBind);
	TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread2LegacyWorkerId, msg1);
	TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread2LegacyWorkerId, msg2);
	TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread2LegacyWorkerId, msg3);
 	iTransport->PostMessage(msg1);	
 	iTransport->PostMessage(msg2);	
 	iTransport->PostMessage(msg3);		
	}
	
void CSend::EntryPoint(TWorkerParams* aParams)
	{
 	RHeap& heap = __START_HEAPCHECK();
 	CTrapCleanup* cleanup=CTrapCleanup::New(); 
 	CSend* self;
 	TInt ret;
 	TRAP(ret, self = CSend::NewLC(*aParams);
 				self->DoItL();
 				CleanupStack::PopAndDestroy(self);
 				);
 	delete cleanup;
 	__END_HEAPCHECK(heap);
	}	

CSend* CSend::NewLC(TWorkerParams& aParams)
	{
 	CSend* self = new (ELeave) CSend();
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams);
 	return self;
	}

/* ------------------ CSendRecieve used in Test Cases NET-COMNF-FWORK-770007 ---------------------- */

void CSendRecieve::EntryPoint(TWorkerParams* aParams)
	{
 	RHeap& heap = __START_HEAPCHECK();
 	CTrapCleanup* cleanup = CTrapCleanup::New();
 	CSendRecieve* self;
 	TInt ret;
 	TRAP(ret, self = CSendRecieve::NewLC(*aParams);	
 				self->DoItL();
 				CleanupStack::PopAndDestroy(self);
 				);
 	delete cleanup;
 	__END_HEAPCHECK(heap);
	}

CSendRecieve* CSendRecieve::NewLC(TWorkerParams& aParams)
	{
 	CSendRecieve* self = new (ELeave) CSendRecieve;
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams);
 	return self;
	}

void CSendRecieve::DoItL()
	{
 	__ASSERT_DEBUG(iTransport->PeerReachable(iPeerId), User::Panic(KSpecAssert_ElemCommsFwtrnsp, 17));
 	if(iPeerId == 1)
 	// thread 1
 		{
 		// Make sure Both peers have registered the interface(s) and are ready to recieve
 		RSemaphore s1;
 		RSemaphore s2;
 		OpenSemaphores(s1, s2);
 		RegisterInterfaces();
 		RThread::Rendezvous(KErrNone);
 		s2.Signal();
 		s1.Wait();
 		s1.Close();
 		s2.Close();
 		}
 	else if(iPeerId == 0)
 	// thread 2
 		{
 		RSemaphore s1;
 		RSemaphore s2;
 		OpenSemaphores(s1, s2);
 		RegisterInterfaces();
		RThread::Rendezvous(KErrNone);
 		s1.Signal();
 		s2.Wait();
 		s1.Close();
 		s2.Close();
 		}
 	Send();
 	CActiveScheduler::Start();	
 	DeregisterInterfaces();
	}
	
void CSendRecieve::Send()
	{
 	// Will Send 4 cookies (1 more than available slots). 
 	switch(iPeerId)
 		{
 		case 0:	
 		// Thread 2	
 			{
 			TCFCommsMessage msg1(TCFCommsMessage::ECodeForward);
 			TCFCommsMessage msg2(TCFCommsMessage::ECodeShutdown);
 			TCFCommsMessage msg3(TCFCommsMessage::ECodeDiscoverResp);
 			TCFCommsMessage msg4(TCFCommsMessage::ECodeDiscover);
			TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread2LegacyWorkerId, msg1);
			TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread2LegacyWorkerId, msg2);
			TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread2LegacyWorkerId, msg3);
			TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread2LegacyWorkerId, msg4);
 			iTransport->PostMessage(msg1);	
 			iTransport->PostMessage(msg2);	
 			iTransport->PostMessage(msg3);	
 			iTransport->PostMessage(msg4);	
 			break;
 			}
 		case 1:
 		// Thread 1
 			{
 			TCFCommsMessage msg5(TCFCommsMessage::ECodeInvalid);
 			TCFCommsMessage msg6(TCFCommsMessage::ECodeBind);
 			TCFCommsMessage msg7(TCFCommsMessage::ECodeUnbind);
 			TCFCommsMessage msg8(TCFCommsMessage::ECodeBindComplete);
			TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread1LegacyWorkerId, msg5);
			TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread1LegacyWorkerId, msg6);
			TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread1LegacyWorkerId, msg7);
			TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread1LegacyWorkerId, msg8);
 			iTransport->PostMessage(msg5);	
 			iTransport->PostMessage(msg6);	
 			iTransport->PostMessage(msg7);	
 			iTransport->PostMessage(msg8);	
 			break;
 			}
 		default :
 			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 18));
 			break;
 		}
	}	

void CSendRecieve::DispatchL(const TCFMessage& aMessage, TWorkerId /*aSenderId*/)
/**
Dispatch messages of type ELegacyId, depending on which peer is 
dispatching the message the journal will be updated in a different
position
*/
	{
 	RMutex mutex;
 	OpenMutexAndWait(mutex);
 	switch(iPeerId)
 		{
 		case 0: 
 		//thread 2
 			journal[KThread2Hash][iJournalCounter] = aMessage.Code();
 			break;
 		case 1:
 		//thread 1
 			journal[KThread1Hash][iJournalCounter] = aMessage.Code();
 			break;
 		default:
 			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 19));
 			break;
 		}
 	SignalMutexAndClose(mutex);	
 	iJournalCounter++;	
 	if(iJournalCounter >= 4)
 		{
 		CActiveScheduler::Stop();
 		}
	}
	
void CSendRecieve::OnDispatchLeave(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/, TInt /*aFirstDispatchLeaveReason*/)
	{
	}
	
void CSendRecieve::RegisterInterfaces()
// Fucntion for registration of Legacy Interfaces
	{
 	switch(iPeerId)
 		{
 		case 0:
 		// thread 2
 			iTransport->RegisterLegacyInterface(static_cast<MLegacyMessageReceiver*>(this));
			globalInfo.iThread1LegacyWorkerId = iTransport->WorkerId();
 			break;
 		case 1:
 		// thread 1
 			iTransport->RegisterLegacyInterface(static_cast<MLegacyMessageReceiver*>(this));
			globalInfo.iThread2LegacyWorkerId = iTransport->WorkerId();
 			break;
 		default:
 			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 20));
 			break;
 		}
	}
	
void CSendRecieve::DeregisterInterfaces()
// Fucntion for deregistration of Legacy Interfaces
	{
 	switch(iPeerId)
 		{
 		case 0:
 		// thread 2
			globalInfo.iThread2LegacyWorkerId = KBadWorkerId;
 			break;
 		case 1:
 		// thread 1
			globalInfo.iThread1LegacyWorkerId = KBadWorkerId;
 			break;	
 		default:
 			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 21));
 			break;
 		}
	}


/* ------------------ COneThreadSendRecieve ---------------------- */
//Used in Test Cases NET-COMNF-FWORK-770002

void COneThreadSendRecieve::ConstructL(TWorkerParams& aParams)
	{
 	iJournalCounter = 0;
 	iThreadRegister = TWorkerRegister::NewL(aParams.iWorkerId, &aParams.iThreadGlobals);
 	iPeerId = aParams.iPeerId;
 	//CCFTransportHooks *hook = new (ELeave) CCFTransportHooks();
 	iTransport = CCommsTransport::NewL(*iThreadRegister, globalInfo.iVCtor, NULL);
 	iClientItfTest = new (ELeave) CItfTestImpl;
 	iDispatchTestItf = new (ELeave) CItfTestImpl;
 	TWorkerThreadDataBase* selfData = iThreadRegister->GetSelfWorkerGlobals();
 	selfData->Init(aParams.iWorkerId, ETrue);
	}

COneThreadSendRecieve::~COneThreadSendRecieve()
	{
 	delete iThreadRegister;
 	delete iTransport;
 	delete iClientItfTest; 
 	delete iDispatchTestItf;
	}	

void COneThreadSendRecieve::Send()
	{
 	TCFCommsMessage msg5(TCFCommsMessage::ECodeInvalid);
 	TCFCommsMessage msg6(TCFCommsMessage::ECodeBind);
 	TCFCommsMessage msg7(TCFCommsMessage::ECodeUnbind);
	TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread1LegacyWorkerId, msg5);
	TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread1LegacyWorkerId, msg6);
	TCFLegacyMessagePacker::PackForPostage(globalInfo.iThread1LegacyWorkerId, msg7);
 	iTransport->PostMessage(msg5);	
 	iTransport->PostMessage(msg6);	
 	iTransport->PostMessage(msg7);
	}	

void COneThreadSendRecieve::DispatchL(const TCFMessage& aMessage, TWorkerId /*aSenderId*/)
	{
 	journal[KThread1Hash][iJournalCounter]=aMessage.Code();
 	iJournalCounter++;
 	if(iJournalCounter >= 3)
 		{
 		CActiveScheduler::Stop();
 		}
	}

COneThreadSendRecieve* COneThreadSendRecieve::NewLC(TWorkerParams& aParams)
	{
 	COneThreadSendRecieve* self = new (ELeave) COneThreadSendRecieve;
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams);
 	return self;
	}

void COneThreadSendRecieve::OnDispatchLeave(TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/, TInt /*aFirstDispatchLeaveReason*/)
	{
	}

void COneThreadSendRecieve::RegisterInterfaces()
//Fucntion for registration of Legacy Interfaces
	{
 	iTransport->RegisterLegacyInterface(static_cast<MLegacyMessageReceiver*>(this));
	globalInfo.iThread1LegacyWorkerId = iTransport->WorkerId();

 	globalInfo.iVCtor->RegisterInterfaceL(TUid::Uid(EMBasicItfId), sizeof(BasicImplementationTable) / sizeof(BasicImplementationTable[0]), BasicImplementationTable);
	Messages::TNodeIdOp(globalInfo.iDispatchCookie).SetPtr(static_cast<NetInterfaces::TInterfaceControl*>(iClientItfTest));
	Messages::TNodeIdOp(globalInfo.iDispatchCookie2).SetPtr(static_cast<NetInterfaces::TInterfaceControl*>(iDispatchTestItf));
	iTransport->RegisterAddress(globalInfo.iDispatchCookie);
	iTransport->RegisterAddress(globalInfo.iDispatchCookie2);
	}


void COneThreadSendRecieve::DeregisterInterfaces()
//Fucntion for deregistration of Legacy Interfaces
	{
	globalInfo.iThread1LegacyWorkerId = KBadWorkerId;
	
	iTransport->DeregisterAddress(globalInfo.iDispatchCookie);
	iTransport->DeregisterAddress(globalInfo.iDispatchCookie2);
	globalInfo.iDispatchCookie = Messages::TNodeId::NullId();
	globalInfo.iDispatchCookie2 = Messages::TNodeId::NullId();
	
 	globalInfo.iVCtor->DeregisterInterface(TUid::Uid(EMBasicItfId));
	}	

void COneThreadSendRecieve::DropTransport()
	{	
 	if(!iTransport->IsDropTransportPending(iPeerId))
 		{
 		iTransport->SetDropTransportPending(iPeerId, ETrue);
 		}
	}
	
void COneThreadSendRecieve::SendDispatch()
	{
 	RBasicItf link;
 	link.Open(globalInfo.iDispatchCookie2, &(iTransport->GetSender()));
 	RBasicItf link2;
 	link2.Open(globalInfo.iDispatchCookie2, &(iTransport->GetSender()));
 	link2.SimpleDispatch(42);
 	link.Close();
 	link.Close();
	}

//
/***************************** Object Dispatch class Definations *******************************/
//


/* ------------------ CSimpleDispatchSend ---------------------- */
void CSimpleDispatchSend::DoItL()
// Will be used as main funct for threads that will send simple Dispatch messages
// NET-COMNF-FWORK-770017, NET-COMNF-FWORK-770006
	{
	RThread::Rendezvous(KErrNone);
	
 	RLinkItf link;
 	link.Open(globalInfo.iDispatchCookie, &(iTransport->GetSender()));
 	__ASSERT_DEBUG(link.IsOpen(), User::Panic(KSpecAssert_ElemCommsFwtrnsp, 22));
 	link.SimpleDispatch(42);
 	link.Close();
	}
void CSimpleDispatchSend::EntryPoint(TWorkerParams* aParams)
	{
 	CTrapCleanup* cleanup=CTrapCleanup::New(); 
 	CSimpleDispatchSend* self;
 	TInt ret;
 	TRAP(ret, self = CSimpleDispatchSend::NewLC(*aParams);
 				self->DoItL();
 				CleanupStack::PopAndDestroy(self);
 				);
 	delete cleanup;
	}	
		
CSimpleDispatchSend* CSimpleDispatchSend::NewLC(TWorkerParams& aParams)
	{
 	CSimpleDispatchSend* self = new (ELeave) CSimpleDispatchSend;
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams);
 	return self;
	}

/* ------------------ CSendBadCookie ---------------------- */
void CSendBadCookie::EntryPoint(TWorkerParams* aParams)
	{
 	RHeap& heap = __START_HEAPCHECK();
 	CTrapCleanup* cleanup=CTrapCleanup::New(); 
 	CSendBadCookie* self;
 	TInt ret;
 	TRAP(ret, self = CSendBadCookie::NewLC(*aParams);
 				self->DoItL();
 				CleanupStack::PopAndDestroy(self);
 				);
 	delete cleanup;
 	__END_HEAPCHECK(heap);
	}	

void CSendBadCookie::DoItL()
	{
	RThread::Rendezvous(KErrNone);
	
	//sending legacy message
 	TCFCommsMessage msg1(TCFCommsMessage::ECodeForward);
	TCFLegacyMessagePacker::PackForPostage(globalInfo.iBadWorkerId, msg1);
 	iTransport->PostMessage(msg1);		
	}
	
CSendBadCookie* CSendBadCookie::NewLC(TWorkerParams& aParams)
	{
 	CSendBadCookie* self = new (ELeave) CSendBadCookie();
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams);
 	return self;
	}


/* ------------------ CSimpleDispatchRecieve ---------------------- */
void CSimpleDispatchRecieve::DoItL(Meta::CMetaDataVirtualCtorInPlace* aVCtor)
// Used in Test Case NET-COMNF-FWORK-770006
	{
 	aVCtor->RegisterInterfaceL(TUid::Uid(EMBasicItfId), sizeof(BasicImplementationTable) / sizeof(BasicImplementationTable[0]), BasicImplementationTable);
 	aVCtor->RegisterInterfaceL(TUid::Uid(EMLinkItfId), sizeof(LinkImplementationTable) / sizeof(LinkImplementationTable[0]), LinkImplementationTable);

	Messages::TNodeIdOp(globalInfo.iDispatchCookie).SetPtr(static_cast<NetInterfaces::TInterfaceControl*>(iClientItfTest));
 	iTransport->RegisterAddress(globalInfo.iDispatchCookie);

 	iTransport->RegisterLegacyInterface(static_cast<MLegacyMessageReceiver*>(this));
	globalInfo.iThread1LegacyWorkerId = iTransport->WorkerId();

 	RThread::Rendezvous(KErrNone);
 	CActiveScheduler::Start();

	globalInfo.iThread1LegacyWorkerId = KBadWorkerId;
	
	iTransport->DeregisterAddress(globalInfo.iDispatchCookie);
	globalInfo.iDispatchCookie = Messages::TNodeId::NullId();
	
 	aVCtor->DeregisterInterface(TUid::Uid(EMBasicItfId));
 	aVCtor->DeregisterInterface(TUid::Uid(EMLinkItfId));
	}

void CSimpleDispatchRecieve::EntryPoint(TWorkerParams* aParams)
	{
 	RHeap& heap = __START_HEAPCHECK();
 	CTrapCleanup* cleanup = CTrapCleanup::New();
 	CSimpleDispatchRecieve* self;
 	Meta::CMetaDataVirtualCtorInPlace* VCtor = new Meta::CMetaDataVirtualCtorInPlace;
 	TInt ret;
 	TRAP(ret, self = CSimpleDispatchRecieve::NewLC(*aParams, VCtor);
  			  self->DoItL(VCtor);
  			  delete VCtor;
  			  CleanupStack::PopAndDestroy(self);
 		 );
 	delete cleanup;
 	__END_HEAPCHECK(heap);
	}
	
CSimpleDispatchRecieve* CSimpleDispatchRecieve::NewLC(TWorkerParams& aParams, Meta::CMetaDataVirtualCtorInPlace* aVCtor)		
	{
 	CSimpleDispatchRecieve* self = new (ELeave) CSimpleDispatchRecieve;
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams, aVCtor);
 	self->iClientItfTest = new(ELeave) CItfTestImpl;
 	self->iClientNarrowTest = new(ELeave) CItfTestImpl;
 	return self;
	}		

CSimpleDispatchRecieve::~CSimpleDispatchRecieve()
	{
 	delete iClientItfTest;
 	delete iClientNarrowTest;
	}
	
void CSimpleDispatchRecieve::DispatchL(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/)
	{
	CActiveScheduler::Stop();
	}
	
/* ------------------ CDeregDispatchRecieve ---------------------- */
void CDeregDispatchRecieve::DoItL(Meta::CMetaDataVirtualCtorInPlace* aVCtor)
// Used in Test Case NET-COMNF-FWORK-770017
	{
 	aVCtor->RegisterInterfaceL(TUid::Uid(EMBasicItfId), sizeof(BasicImplementationTable) / sizeof(BasicImplementationTable[0]), BasicImplementationTable);
 	aVCtor->RegisterInterfaceL(TUid::Uid(EMLinkItfId), sizeof(LinkImplementationTable) / sizeof(LinkImplementationTable[0]), LinkImplementationTable);

	Messages::TNodeIdOp(globalInfo.iDispatchCookie).SetPtr(static_cast<NetInterfaces::TInterfaceControl*>(iClientItfTest));
 	iTransport->RegisterAddress(globalInfo.iDispatchCookie);

	Messages::TNodeId deRegestierCookie; 
 	deRegestierCookie = globalInfo.iDispatchCookie;
 	iTransport->DeregisterAddress(deRegestierCookie);	

 	__ASSERT_DEBUG(deRegestierCookie.IsNull(), User::Panic(KSpecAssert_ElemCommsFwtrnsp, 23));	
	aVCtor->DeregisterInterface(TUid::Uid(EMBasicItfId));
 	aVCtor->DeregisterInterface(TUid::Uid(EMLinkItfId));
	
 	RThread::Rendezvous(KErrNone);
 	iStopper->StartL();
 	CActiveScheduler::Start();
 	
 	globalInfo.iDispatchCookie = Messages::TNodeId::NullId();
	}

void CDeregDispatchRecieve::EntryPoint(TWorkerParams* aParams)
	{
 	RHeap& heap = __START_HEAPCHECK();
	CTrapCleanup* cleanup = CTrapCleanup::New();
 	CDeregDispatchRecieve* self;
 	Meta::CMetaDataVirtualCtorInPlace* VCtor = new Meta::CMetaDataVirtualCtorInPlace;
 	TInt ret;
 	TRAP(ret, self = CDeregDispatchRecieve::NewLC(*aParams, VCtor);
  			  self->DoItL(VCtor);
  			  CleanupStack::PopAndDestroy(self);
 		 );
 	delete VCtor;
 	delete cleanup;
 	__END_HEAPCHECK(heap);
	}
	
CDeregDispatchRecieve* CDeregDispatchRecieve::NewLC(TWorkerParams& aParams,Meta::CMetaDataVirtualCtorInPlace* aVCtor)		
	{
 	CDeregDispatchRecieve* self = new (ELeave) CDeregDispatchRecieve;
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams, aVCtor);
 	self->iClientItfTest = new(ELeave) CItfTestImpl;
 	self->iStopper = CSchedulerStopper::NewL();
 	return self;
	}		

CDeregDispatchRecieve::~CDeregDispatchRecieve()
	{
 	delete iClientItfTest;
 	delete iStopper;
	}
	

/* ------------------ COOMDispatchSendRecieve ---------------------- */
// Used in Test Cases NET-COMNF-FWORK-770019 & NET-COMNF-FWORK-770020

void COOMDispatchSendRecieve::EntryPoint(TWorkerParams* aParams)
	{
 	RHeap& heap = __START_HEAPCHECK();

 	CTrapCleanup* cleanup = CTrapCleanup::New();
 	COOMDispatchSendRecieve* self;
 	Meta::CMetaDataVirtualCtorInPlace* VCtor = new Meta::CMetaDataVirtualCtorInPlace;
 	TInt ret;
 	TRAP(ret, self = COOMDispatchSendRecieve::NewLC(*aParams, VCtor);
 			  self->DoItL(VCtor);
 			  CleanupStack::PopAndDestroy(self);
 			  );
 	delete cleanup;
 	delete VCtor;

 	__END_HEAPCHECK(heap);
	}
	
COOMDispatchSendRecieve::~COOMDispatchSendRecieve()
	{
 	delete iClientItfTest;
 	delete iStopper;
	}
	
COOMDispatchSendRecieve* COOMDispatchSendRecieve::NewLC(TWorkerParams& aParams, Meta::CMetaDataVirtualCtorInPlace* aVCtor)
	{
 	COOMDispatchSendRecieve* self = new (ELeave) COOMDispatchSendRecieve;
 	CleanupStack::PushL(self);
 	self->ConstructL(aParams, aVCtor);
 	self->iClientItfTest = new (ELeave) CItfTestImpl;
 	self->iStopper = CSchedulerStopper::NewL();
 	return self;
	}

void COOMDispatchSendRecieve::Send()
	{
 	// Will Send 4 msgs (1 more than available slots, 3 defined by config).
 	switch(iPeerId)
 		{
 		case 0:	
 		// Thread 2
 			{
 			// Open Link interface
 			RLinkItf link2;
 			link2.Open(globalInfo.iThread2DispatchCookie, &(iTransport->GetSender()));
 			// send messages
 			link2.DispatchThread2(KSmall1);
 			link2.DispatchThread2(KSmall2);
 			link2.DispatchThread2(KSmall3);
 			link2.DispatchThread2(KSmall4);
 			link2.Close();
 			break;
 			}
 		case 1:
 		// Thread 1
 			{
 			// Open Link interface
 			RLinkItf link1;
 			User::After(10000);	
 			link1.Open(globalInfo.iThread1DispatchCookie, &(iTransport->GetSender()));
 			// send messages
 			link1.DispatchThread1(KSmall4);
 			link1.DispatchThread1(KSmall3);
 			link1.DispatchThread1(KSmall2);
 			link1.DispatchThread1(KSmall1);
 			link1.Close();
			break;
 			}
 		default :
 			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 24));
 			break;
 		}
	}	

void COOMDispatchSendRecieve::RegisterInterfacesL(Meta::CMetaDataVirtualCtorInPlace* aVCtor)
// Fucntion for registration of Interfaces
	{
 	switch(iPeerId)
 		{
 		case 0:
			// thread 2
 			{
 			if(!globalInfo.iThread1DispatchCookie.IsNull())
 				{
				globalInfo.iThread1DispatchCookie = Messages::TNodeId::NullId();
 				}
			aVCtor->RegisterInterfaceL(TUid::Uid(EMBasicItfId), sizeof(BasicImplementationTable) / sizeof(BasicImplementationTable[0]), BasicImplementationTable);
			aVCtor->RegisterInterfaceL(TUid::Uid(EMLinkItfId), sizeof(LinkImplementationTable) / sizeof(LinkImplementationTable[0]), LinkImplementationTable);

			Messages::TNodeIdOp(globalInfo.iThread1DispatchCookie).SetPtr(static_cast<NetInterfaces::TInterfaceControl*>(iClientItfTest));	
 			iTransport->RegisterAddress(globalInfo.iThread1DispatchCookie);

 			break;
 			}
 		case 1:
			// thread 1
 			{
			aVCtor->RegisterInterfaceL(TUid::Uid(EMBasicItfId), sizeof(BasicImplementationTable) / sizeof(BasicImplementationTable[0]), BasicImplementationTable);
			aVCtor->RegisterInterfaceL(TUid::Uid(EMLinkItfId), sizeof(LinkImplementationTable) / sizeof(LinkImplementationTable[0]), LinkImplementationTable);

			Messages::TNodeIdOp(globalInfo.iThread2DispatchCookie).SetPtr(static_cast<NetInterfaces::TInterfaceControl*>(iClientItfTest));	
 			iTransport->RegisterAddress(globalInfo.iThread2DispatchCookie);
			
 			break;
 			}
 		default:
 			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 25));
 			break;
 		}
	}

void COOMDispatchSendRecieve::DeregisterInterfaces(Meta::CMetaDataVirtualCtorInPlace* aVCtor)
// Fucntion for deregistration of Interfaces
	{
 	switch(iPeerId)
 		{
 		case 0:
			// thread 2
 			iTransport->DeregisterAddress(globalInfo.iThread1DispatchCookie);
			globalInfo.iThread1DispatchCookie = Messages::TNodeId::NullId();
 			aVCtor->DeregisterInterface(TUid::Uid(EMLinkItfId));
			aVCtor->DeregisterInterface(TUid::Uid(EMBasicItfId));
 			break;
 		case 1:
			// thread 1
 			iTransport->DeregisterAddress(globalInfo.iThread2DispatchCookie);
			globalInfo.iThread2DispatchCookie = Messages::TNodeId::NullId();
 			aVCtor->DeregisterInterface(TUid::Uid(EMLinkItfId));
			aVCtor->DeregisterInterface(TUid::Uid(EMBasicItfId));
   			break;	
 		default:
 			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 26));
 			break;
 		}
	}

void COOMDispatchSendRecieve::DoItL(Meta::CMetaDataVirtualCtorInPlace* aVCtor)
// register interfaces, send messages, dispatch and de-register interfaces
	{
 	if(iPeerId == 1)
		// thread 1
 		{
 		RHeap& heap1 = User::Heap();
 		threadHeapPtr[0] = &heap1;
 		RSemaphore s1;
 		RSemaphore s2;
 		OpenSemaphores(s1, s2);
 		RegisterInterfacesL(aVCtor);
 		RThread::Rendezvous(KErrNone);
 		s2.Signal();
 		s1.Wait();		
 		s1.Close();
 		s2.Close();
 		TRequestStatus& stat1 = statusThread1;
 		User::WaitForRequest(stat1);
 		}
 	else if(iPeerId == 0)
		// thread 2
 		{
 		RHeap& heap2 = User::Heap();
 		threadHeapPtr[1] = &heap2;
 		RSemaphore s1;
 		RSemaphore s2;
 		OpenSemaphores(s1, s2);
 		RegisterInterfacesL(aVCtor);
		RThread::Rendezvous(KErrNone);
 		TRequestStatus& stat2 = statusThread2;
 		User::WaitForRequest(stat2);
 		s1.Signal();
 		s2.Wait();
 		s1.Close();
 		s2.Close();
 		}
	
 	Send();
 	if(iPeerId == 0 && KTimeInterval < 10000)
 		{
 		iStopper->StartL();
 		}
 	CActiveScheduler::Start();	
 	DeregisterInterfaces(aVCtor);
	}

//
//********************** CTransportTestStep Member definitions ****************************//
//

CTransportTestStep::~CTransportTestStep()
// Destructor for Test Step base Class
	{
	TInt i;
	for(i = 0; i < sizeof( iConfigDataList ) / sizeof( iConfigDataList[0] ); i++)
		{
		delete iConfigDataList[i];	
		}
	}
	
TVerdict CTransportTestStep::doTestStepPreampleL()
// Test Step Preample
	{
	SetTestStepResult(EFail);
	return TestStepResult();
	} 
	
void CTransportTestStep::CloseChannelPairs()
// Function to close Channel Pairs used for Transport
	{
	TInt i;
	for(i = 0; i < sizeof( iChannelPairs ) / sizeof( iChannelPairs[0] ); i++)
		{
		iChannelPairs[i].CloseNonGracefully();	
		}
	}
	
void CTransportTestStep::VerifyTwoWaySend(TInt aCode)
// Function to verify Two-way Dispatch message code has been recieved
	{
 	if(journal[KThread1Hash][0] == aCode && journal[KThread1Hash][1] == aCode && journal[KThread1Hash][2] == aCode 
 	&& journal[KThread1Hash][3] == aCode &&journal[KThread1Hash][0] == aCode && journal[KThread1Hash][1] == aCode 
 	&& journal[KThread1Hash][2] == aCode && journal[KThread1Hash][3] == aCode)
 		{
 		SetTestStepResult(EPass);
 		}
 	TInt blobCounter = 1;
 	for(TInt i = 0; i < KJournalSlots; i++)
 		{
 		if(journal[KThread1Hash][i]== EBlobRecievedCorrectly)
 			{
 			INFO_PRINTF2(_L("blob %d was delivered sucessfully"),blobCounter++);
 			}
 		else if(journal[KThread1Hash][i]== EMessageRecieved)
 			{
 			INFO_PRINTF2(_L("Message %d was delivered sucessfully"),blobCounter++);
 			}
 		if(journal[KThread2Hash][i]== EBlobRecievedCorrectly)
 			{
 			INFO_PRINTF2(_L("blob %d was delivered sucessfully"),blobCounter++);
 			}
 		else if(journal[KThread2Hash][i]== EMessageRecieved)
 			{
 			INFO_PRINTF2(_L("Message %d was delivered sucessfully"),blobCounter++);
 			}
 		}
	}

void CTransportTestStep::VerifyHeapChecks()
//Function to Verify that all Transport Threads have exited without any memory leaks
	{
 	TInt j; 
 	for(j = 0; j < KMemorySlots; j++)
 		{
 		if(memoryArray[KMemoryTestResultHash][j] != ENoMemoryLeak)
 			{
 			INFO_PRINTF2(_L("TEST HAS FAILED AS A RESULT OF A MEMORY LEAK IN ADDR 0x%x"),memoryArray[KOrphanedMemoryHash][j]);
 			SetTestStepResult(EFail);
 			}
 		}
	}

void CTransportTestStep::VerifyCSend()
// Function to verify that the CSend Function has completed Correctly
	{
 	if(journal[KThread1Hash][0] == TCFCommsMessage::ECodeForward && journal[KThread1Hash][1] == TCFCommsMessage::ECodeShutdown
 	&& journal[KThread1Hash][2] ==TCFCommsMessage::ECodeBind)
 		{
 		SetTestStepResult(EPass);
		}
 	TInt MessageCounter = 1;
 	for(TInt i = 0; i < KJournalSlots; i++)
 		{
 		if(journal[KThread1Hash][i]!=ENoMessageRecieved)
 		INFO_PRINTF2(_L("TCFMessage %d was delivered sucessfully"),MessageCounter++);
 		}
	}


void CTransportTestStep::CreateChannelPairsL()
/**
 Create the RCFChannelPair for each peer in config. Creates first ChannelPair using KSlots and 
 uses the messages queues from the first one to create the remaining ChannelPairs
 */
	{		
	User::LeaveIfError(iChannelPairs[0].CreateRecv(KSlots));
	User::LeaveIfError(iChannelPairs[0].CreateSend(KSlots));
	RCFChannelPair::TMsgQueues msgQueues;
	for(TInt index=1;index<iTasks;index++)
		{
		iChannelPairs[0].GetSendMsgQueues(msgQueues);
		User::LeaveIfError(iChannelPairs[index].CreateRecv(msgQueues, EOwnerProcess));
		iChannelPairs[0].GetRecvMsgQueues(msgQueues);
		User::LeaveIfError(iChannelPairs[index].CreateSend(msgQueues, EOwnerProcess));
		}
	}

void CTransportTestStep::OpenMutexAndWait(RMutex& aMutex)
// Used to open global Mutex and call wait
	{
 	TInt err = aMutex.OpenGlobal(KMutex);
 	__ASSERT_DEBUG(err == KErrNone, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 27));
 	aMutex.Wait();
	}


void CTransportTestStep::SignalMutexAndClose(RMutex& aMutex)
// Used to Signal Global Mutex and then Close
	{
 	aMutex.Signal();
 	aMutex.Close();
	}

void CTransportTestStep::InitDataL()
// Takes data from ini file
	{
	TPtrC tasks;
	TPtrC threads;
	
	if(GetStringFromConfig(KTe_TaskSection,KTe_Tasks,tasks) == EFalse)
		{
		INFO_PRINTF1(_L("No tasks defined in Task section of ini file"));
		}
	else	
		{	
		TPeerId peerId;
		TInt workerId;
		TLex lex(tasks);
		TChar ch=0;
		TInt taskCounter = 0;	
		do
			{
			// Takes all the tasks from the config file and puts then in a TPtrC
			lex.Mark();
			do	
				{
				ch=lex.Get();
				}while (ch!=',' && ch!=0);	
			if (ch==',') 
				{
				lex.UnGet();
				}
			
			TPtrC taskTag=lex.MarkedToken();
			lex.Get();
			
			// Get task information from Config
			if(EFalse == GetIntFromConfig(taskTag, KTe_PeerId, peerId))
				{
				INFO_PRINTF1(_L("No PeerId defined in Peer section of ini file"));
				}

			if(EFalse == GetIntFromConfig(taskTag, KTe_WorkerId, workerId))
				{
				INFO_PRINTF1(_L("No WorkerId defined in Peer section of ini file"));
				}	
			iConfigDataList[taskCounter] = new (ELeave) TConfigData(peerId, workerId);
			taskCounter++;
			}while(ch!=0 && taskCounter < KMaxPeers);
		
			if(taskCounter >= KMaxPeers)
				{
				iTasks=KMaxPeers;
				}				
			else
				{
				iTasks=taskCounter+1;
				}
		
		// Assign KSlots and KTimeInterval
		if(GetIntFromConfig(KTe_TaskSection,KTe_Slots,KSlots) == EFalse)
			{
			KSlots=3;
			INFO_PRINTF1(_L("No Queue Slot size defined in Task section of ini file, default value of 3 will be used"));
			}
		if(KSlots <= 0)
			{
			KSlots = 3;
			INFO_PRINTF1(_L("Slot size defined in Task section of ini file must be greater than 0, default value of 3 will be used"));
			}	
		if(GetIntFromConfig(KTe_TaskSection,KTe_TimeInterval,KTimeInterval) == EFalse)
			{
			KTimeInterval=0;
			INFO_PRINTF1(_L("No TimeInterval was defined in Task section of ini file"));
			}
		}
		
	TInt index;
	for(TInt threadCount = 0; threadCount < KTotalThreads; threadCount ++)
	// Initialise all journal slots to ENoMessageRecieved
		{
		for(index = 0; index < KJournalSlots; index ++)
			{
			journal[threadCount][index] = ENoMessageRecieved;
			}
		}
		
	for(TInt MemCount = 0; MemCount < KMemorySlots; MemCount ++)
	// Initialise all memArray slots to ENoMessageRecieved
		{
		memoryArray[KMemoryTestResultHash][MemCount] = ENoMemoryLeak;
		}	
	
	// Initialise memoryItr to 0 for all tests.
	memoryItr = 0;
	}

void CTransportTestStep::RunOOMThreadsL(TThreadFunction aThreadFucntion1, TThreadFunction aThreadFucntion2, const TDesC &aThreadName1, const TDesC &aThreadName2,const TInt aThreadMaxHeap1,const TInt aThreadMaxHeap2)
	{
 	/**
      This is a routine to initialise two threads for ThreadTransport Communication, then run them 
	  and retrieve relevent exit information. 
	*/
	
 	// semaphores to be used to syncronize cookie registration
 	RSemaphore sem1;
 	RSemaphore sem2;
 	sem1.CreateGlobal(KSem1,0);
 	sem2.CreateGlobal(KSem2,0);
	
 	CActiveScheduler* actSched = new (ELeave) CActiveScheduler;
 	CleanupStack::PushL(actSched);
 	CActiveScheduler::Install(actSched);
#ifdef SYMBIAN_ZERO_COPY_NETWORKING	
	RCommsBufPondOp pond;
	CreateCommsBufPondLC(pond);
#else
	CMBufManager* mgr = CreateCMBufManagerLC();
#endif	

	
 	// Create global thread information, contains workerId, threadId and heap inforamtion for Transport
 	TWorkerGlobals* threadGlobals = TWorkerGlobals::NewL();
 	CleanupStack::PushL(threadGlobals);
	
 	// Initialise information from config file
 	InitDataL();
 	CreateChannelPairsL();

 	RCFThread thread1;
 	// create parameters to be passed into threads, threadGlobals, configdata, channelPair
 	TWorkerParams params1(iChannelPairs[1], *threadGlobals, iConfigDataList[0]->iWorkerId,iConfigDataList[0]->iPeerId);
 	User::LeaveIfError(thread1.Create(aThreadName1, (TThreadFunction) aThreadFucntion1, KDefaultStackSize, KMinHeapSize, aThreadMaxHeap1, &params1));
 	thread1.SetPriority(EPriorityNormal);
 	TRequestStatus logon1;
 	thread1.Logon(logon1);
 	TRequestStatus rendezvous1;
 	thread1.Rendezvous(rendezvous1);
 	// Set thread1 ready to run
 	thread1.Resume();
 	INFO_PRINTF1(_L("The first thread has been created and is running"));

 	RCFThread thread2;
 	// create parameters to be passed into threads, threadGlobals, configdata, channelPair
 	TWorkerParams params2(iChannelPairs[0], *threadGlobals, iConfigDataList[1]->iWorkerId,iConfigDataList[1]->iPeerId);
 	User::LeaveIfError(thread2.Create(aThreadName2, (TThreadFunction) aThreadFucntion2, KDefaultStackSize, KMinHeapSize, aThreadMaxHeap2, &params2));
 	thread1.SetPriority(EPriorityLess);
 	TRequestStatus logon2;
 	thread2.Logon(logon2);
	TRequestStatus rendezvous2;
	thread2.Rendezvous(rendezvous2);
 	// Wait for Thread 1 to signal it has registered Interfaces. 
 	User::WaitForRequest(rendezvous1);
 	// Set thread2 ready to run
 	thread2.Resume();
	// Wait for thread 2 to signal it has registered Interfaces
	User::WaitForRequest(rendezvous2);
 	INFO_PRINTF1(_L("The second thread has been created and is running"));

 	// Set worker thread heaps to fail memory alocation
 	threadHeapPtr[1]->__DbgSetAllocFail( RAllocator::EDeterministic,1);
 	threadHeapPtr[0]->__DbgSetAllocFail( RAllocator::EDeterministic,1);

 	TRequestStatus* reqPtr1 = &statusThread1;
 	TRequestStatus* reqPtr2 = &statusThread2;

 	thread1.RequestComplete(reqPtr1, 0);
 	thread2.RequestComplete(reqPtr2, 0);

 	User::After(KTimeInterval);
 	if(KTimeInterval > 100000)
 	// if KTimeInterval > 100000 then memory will become available to thread heaps again during 
 	// transport post sleep cycle
 		{
 		threadHeapPtr[1]->__DbgSetAllocFail( RAllocator::ENone,1);
 		threadHeapPtr[0]->__DbgSetAllocFail( RAllocator::ENone,1);
 		}
	
 	// Wait for threads to finish
 	User::WaitForRequest(logon1,logon2);
 	User::WaitForRequest(logon1 == KRequestPending ? logon1 : logon2);

 	// capture exit information 
 	exitArray[0] = thread1.ExitReason();
 	exitArray[1] = thread2.ExitReason();
	
 	// Close ChannelPairs
 	iChannelPairs[0].CloseNonGracefully();
 	iChannelPairs[1].CloseNonGracefully();

 	CleanupStack::PopAndDestroy(threadGlobals);
 	CleanupStack::PopAndDestroy();	// pond
 	CActiveScheduler::Install(NULL);
 	CleanupStack::PopAndDestroy(actSched);	
 	thread1.Close();
 	thread2.Close();
	}
	


void CTransportTestStep::RunThreadsL(TThreadFunction aThreadFucntion1, TThreadFunction aThreadFucntion2,TThreadPriority aPriority1,TThreadPriority aPriority2, const TDesC &aThreadName1, const TDesC &aThreadName2,const TInt aThreadMaxHeap1,const TInt aThreadMaxHeap2)
/*
 Function to Run two threads. 
 Generally the first will be the reciever and the second the sender
 */
	{
	__CFLOG_CREATEL;
	// Semaphores to be used to synchronise Threads using the transport system
	RSemaphore sem1;
	RSemaphore sem2;
	sem1.CreateGlobal(KSem1,0);
	sem2.CreateGlobal(KSem2,0);
	
	CActiveScheduler* actSched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(actSched);
	CActiveScheduler::Install(actSched);
#ifdef SYMBIAN_ZERO_COPY_NETWORKING	
	RCommsBufPondOp pond;
	CreateCommsBufPondLC(pond);
#else
	CMBufManager* mgr = CreateCMBufManagerLC();
#endif	
	// Create global thread information, contains workerId, threadId and heap inforamtion for Transport
	TWorkerGlobals* threadGlobals = TWorkerGlobals::NewL();
	CleanupStack::PushL(threadGlobals);
	
	// Initialise data from Config file
	InitDataL();
	CreateChannelPairsL();

	RCFThread thread1;
	// create parameters to be passed into threads, threadGlobals, configdata, channelPair
	TWorkerParams params1(iChannelPairs[1], *threadGlobals, iConfigDataList[0]->iWorkerId,iConfigDataList[0]->iPeerId);
	User::LeaveIfError(thread1.Create(aThreadName1, (TThreadFunction) aThreadFucntion1, KDefaultStackSize, KMinHeapSize, aThreadMaxHeap1, &params1));
	thread1.SetPriority(aPriority1);
	TRequestStatus logon1;
	thread1.Logon(logon1);
		
	TRequestStatus rendezvous1;

	thread1.Rendezvous(rendezvous1);
	// Set thread1 ready to run
	thread1.Resume();
	INFO_PRINTF1(_L("The first thread has been created and is running"));
	
	User::After(100);
	RCFThread thread2;
	// create parameters to be passed into threads, threadGlobals, configdata, channelPair
	TWorkerParams params2(iChannelPairs[0], *threadGlobals, iConfigDataList[1]->iWorkerId,iConfigDataList[1]->iPeerId);
	User::LeaveIfError(thread2.Create(aThreadName2, (TThreadFunction) aThreadFucntion2, KDefaultStackSize, KMinHeapSize, aThreadMaxHeap2, &params2));
	thread1.SetPriority(aPriority2);
	TRequestStatus logon2;
	thread2.Logon(logon2);
	// Wait for thread1 to signal that it has registered interfaces for thread2 to send
	User::WaitForRequest(rendezvous1);

	TRequestStatus rendezvous2;
	thread2.Rendezvous(rendezvous2);

	// Set Thread 2 ready to run
	thread2.Resume();
	// Wait for thread2 to signal that it has registered interfaces for thread1 to send
	User::WaitForRequest(rendezvous2);

	INFO_PRINTF1(_L("The second thread has been created and is running"));

	// Wait for threads to complete
	User::WaitForRequest(logon1,logon2);
	User::WaitForRequest(logon1 == KRequestPending? logon1: logon2);
	
	// capture exit information
	exitArray[0] = thread1.ExitReason();
	exitArray[1] = thread2.ExitReason();
	
	iChannelPairs[0].CloseNonGracefully();
	iChannelPairs[1].CloseNonGracefully();

	CleanupStack::PopAndDestroy(threadGlobals);
	CleanupStack::PopAndDestroy();	// pond
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(actSched);
	
	//Clear TWorkerDatabase and Close the Threads

	thread1.Close();
	thread2.Close();
	__CFLOG_DELETE;
	}


//
/*********************************** Derived Test Step Definations *********************************************/
//

TVerdict CTransportTestStep1_1::doTestStepL()
// Very simple Send, Recieve and dispatch test case, Queueslots must be set to 3. NET-COMNF-FWORK-770001

	{
	_LIT(KThread1, "Thread1_1_1");
	_LIT(KThread2, "Thread1_1_2");
	RMutex mutex;
	mutex.CreateGlobal(KMutex);
	RunThreadsL((TThreadFunction)CRecieve::EntryPoint,(TThreadFunction) CSend::EntryPoint, EPriorityAbsoluteHigh, EPriorityLess, KThread1, KThread2);
	OpenMutexAndWait(mutex);
	VerifyCSend();
	SignalMutexAndClose(mutex);
	VerifyHeapChecks();
	return TestStepResult();
	}
	
	
TVerdict CTransportTestStep1_2::doTestStepL()
// Simple one-way Same thread using self-sender. NET-COMNF-FWORK-770002
	{
	__CFLOG_CREATEL;
	SetTestStepResult(EFail);
	CActiveScheduler* actSched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(actSched);
	CActiveScheduler::Install(actSched);
#ifdef SYMBIAN_ZERO_COPY_NETWORKING	
	RCommsBufPondOp pond;
	CreateCommsBufPondLC(pond);
#else
	CMBufManager* mgr = CreateCMBufManagerLC();
#endif	

	// initialize data from config file
	InitDataL();
	CreateChannelPairsL();
	
	// create parameters to be passed into threads, threadGlobals, configdata, channelPair 
	TWorkerGlobals* threadGlobals = TWorkerGlobals::NewL();
	CleanupStack::PushL(threadGlobals);
	TWorkerParams params1(iChannelPairs[0], *threadGlobals, iConfigDataList[0]->iWorkerId,iConfigDataList[0]->iPeerId);
	globalInfo.iVCtor = new Meta::CMetaDataVirtualCtorInPlace;
	// Create class to register the interfaces, send to itself, dispatch and de-register
	COneThreadSendRecieve* s = COneThreadSendRecieve::NewLC(params1);
	
	//Register Interfaces and then post messages
	s->RegisterInterfaces();
	s->Send();
	
	//Dispatch all messages and Deregiester messages
	CActiveScheduler::Start();
	s->DeregisterInterfaces();
	
	CleanupStack::PopAndDestroy(s);
	CleanupStack::PopAndDestroy(threadGlobals);
	CleanupStack::PopAndDestroy();	// pond
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(actSched);	
	if(journal[KThread1Hash][0] == TCFCommsMessage::ECodeInvalid && journal[KThread1Hash][1] == TCFCommsMessage::ECodeBind
	&& journal[KThread1Hash][2] == TCFCommsMessage::ECodeUnbind)
		{
		SetTestStepResult(EPass);
		}
	
	TInt MessageCounter = 1;
	for(TInt i = 0; i < KJournalSlots; i++)
		{
		if(journal[KThread1Hash][i] != ENoMessageRecieved)
			INFO_PRINTF2(_L("TCFMessage %d was delivered sucessfully"),MessageCounter++);
		}
	delete globalInfo.iVCtor;
	CloseChannelPairs();
	__CFLOG_DELETE;
	return TestStepResult();
	}
	
TVerdict CTransportTestStep1_3::doTestStepL()
// Queue more messages than slots. Needs QueueSlots field in .ini file to be set below 3.NET-COMNF-FWORK-770003
	{
	_LIT(KThread1, "Thread1_3_1");
	_LIT(KThread2, "Thread1_3_2");
	RMutex mutex;
	mutex.CreateGlobal(KMutex);
	// Run the threads, One to recieve, One to send. QueSlots is one less than the amount of messages sent.
	RunThreadsL((TThreadFunction)CRecieve::EntryPoint, (TThreadFunction)CSend::EntryPoint, EPriorityAbsoluteHigh, EPriorityLess, KThread1, KThread2);
	OpenMutexAndWait(mutex);
	VerifyCSend();
	SignalMutexAndClose(mutex);
	VerifyHeapChecks();
	return TestStepResult();
	}
	
	
TVerdict CTransportTestStep1_4::doTestStepL()
// Simple one-way dispatch, peer1 (Receiver) blocked beyond time out
// Needs QueueSlots field in .ini file to be set to 2 && TimeoutInterval set to over 100000000.NET-COMNF-FWORK-770004
	{
	_LIT(KThread1, "Thread1_4_1");
	_LIT(KThread2, "Thread1_4_2");
	SetTestStepResult(EFail);
	RMutex mutex;
	mutex.CreateGlobal(KMutex);

	// Run the threads, 1 reciever and 1 sender - the reciever is unable to dispatch the messages as it is busy
	// After the sender sleep cycle ends the sender will panic the reciever
	RunThreadsL((TThreadFunction)CBusyReciever::EntryPoint,(TThreadFunction) CSend::EntryPoint, EPriorityLess, EPriorityAbsoluteHigh, KThread1, KThread2);
	if(exitArray[0] == ECFTransPeerDrainFailure)
		{
		SetTestStepResult(EPass);
		INFO_PRINTF2(_L("Recieving Peer was Correctly Panicked by Sending Thread with TCFTransportPanics %d"), exitArray[0]);
		}
	else
		{
		SetTestStepResult(EFail);
		INFO_PRINTF2(_L("Blocked Recieving Peer was incorrectly panicked or not panicked at all by Sending Thread, Exit Type = %d"), exitArray[0]);
		}
	return TestStepResult();
	}
	
TVerdict CTransportTestStep1_5::doTestStepL()
// Simple one-way dispatch, peer1 (Receiver) blocked BUT NOT beyond timeout checks for premature panicking
// Needs QueueSlots field in .ini file to be set to 2 && TimeoutInterval set below 1000. NET-COMNF-FWORK-770005
	{
	_LIT(KThread1, "Thread1_5_1");
	_LIT(KThread2, "Thread1_5_2");
	SetTestStepResult(EFail);
	RMutex mutex;
	mutex.CreateGlobal(KMutex);
	// Run the threads, 1 reciever and 1 sender - the reciever is unable to dispatch the messages as it is busy
	// Before the sender Sleepcycle ends the reciever should become available to dispatch again. 
	RunThreadsL((TThreadFunction)CBusyReciever::EntryPoint,(TThreadFunction) CSend::EntryPoint, EPriorityLess, EPriorityAbsoluteHigh, KThread1, KThread2);
	OpenMutexAndWait(mutex);
	if(journal[KThread1Hash][0] == EMessageRecieved)
		{
		SetTestStepResult(EPass);
		INFO_PRINTF2(_L("Message Delivered Correctly, No Premature Panicking occured %d"), exitArray[0]);
		}
	else
		{
		SetTestStepResult(EFail);
		INFO_PRINTF2(_L("Message Delivered Incorrectly and/or Blocked Recieving Peer was incorrectly panicked before timeout, Exit Type = %d"), exitArray[0]);
		}
	SignalMutexAndClose(mutex);
	VerifyHeapChecks();
	return TestStepResult();
	}
	
	
TVerdict CTransportTestStep1_6::doTestStepL() 
// send msg of Dispatch type. NET-COMNF-FWORK-770006
	{
	SetTestStepResult(EFail);
	_LIT(KThread1, "Thread1_6_1");
	_LIT(KThread2, "Thread1_6_2");
	RMutex mutex;
	mutex.CreateGlobal(KMutex);

	// Run the threads, 1 reciever and 1 sender - The sender posts a message of type ObjectDispatchItf, the reciever
	// should dispatch the message correctly
	RunThreadsL((TThreadFunction)CSimpleDispatchRecieve::EntryPoint,(TThreadFunction)CSimpleDispatchSend::EntryPoint, EPriorityAbsoluteHigh, EPriorityLess, KThread1, KThread2);

	
	OpenMutexAndWait(mutex);
	if(journal[KThread1Hash][0] == 42)
		{
		SetTestStepResult(EPass);
		}
	TInt MessageCounter = 1;
	for(TInt i = 0; i < KJournalSlots; i++)
		{
		if(journal[KThread1Hash][i] != ENoMessageRecieved)
			INFO_PRINTF2(_L("Object Dispatch Message %d was delivered sucessfully"),MessageCounter++);
		}
	SignalMutexAndClose(mutex);
	VerifyHeapChecks();
	return TestStepResult();
	}
	
TVerdict CTransportTestStep1_7::doTestStepL()
// Simple one-way Same thread using self-sender, Dispatch Messages. NET-COMNF-FWORK-770007
	{
	__CFLOG_CREATEL;
	SetTestStepResult(EFail);
	CActiveScheduler* actSched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(actSched);
	CActiveScheduler::Install(actSched);
#ifdef SYMBIAN_ZERO_COPY_NETWORKING	
	RCommsBufPondOp pond;
	CreateCommsBufPondLC(pond);
#else
	CMBufManager* mgr = CreateCMBufManagerLC();
#endif	



	// initialize data from config file
	InitDataL();
	CreateChannelPairsL();
	
	// create parameters to be passed into threads, threadGlobals, configdata, channelPair 
	TWorkerGlobals* threadGlobals = TWorkerGlobals::NewL();
	CleanupStack::PushL(threadGlobals);
	TWorkerParams params1(iChannelPairs[0], *threadGlobals, iConfigDataList[0]->iWorkerId,iConfigDataList[0]->iPeerId);
	globalInfo.iVCtor = new Meta::CMetaDataVirtualCtorInPlace;
	// Create class to register the interfaces, send to itself, dispatch and de-register
	COneThreadSendRecieve* s = COneThreadSendRecieve::NewLC(params1);
	
	s->RegisterInterfaces();
	s->SendDispatch();
	CActiveScheduler::Start();
	s->DeregisterInterfaces();
	
	CleanupStack::PopAndDestroy(s);
	CleanupStack::PopAndDestroy(threadGlobals);
	CleanupStack::PopAndDestroy();	// pond
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(actSched);	
	if(journal[KThread1Hash][0] == 42)
		{
		SetTestStepResult(EPass);
		}
	TInt MessageCounter = 1;
	for(TInt i = 0; i < KJournalSlots; i++)
		{
		if(journal[KThread1Hash][i] != ENoMessageRecieved)
			INFO_PRINTF2(_L("Object Dispatch Message %d was delivered sucessfully"),MessageCounter++);
		}

	delete globalInfo.iVCtor;
	CloseChannelPairs();
	__CFLOG_DELETE;
	return TestStepResult();
	}




TVerdict CTransportTestStep2_1::doTestStepL()
// Complicated 2 sender/recievers both end up blocked and have to prefetch from inbound queue to avoid deadlock.
// NET-COMNF-FWORK-770007
	{
	_LIT(KThread1, "Thread2_1_1");
	_LIT(KThread2, "Thread2_1_2");
	SetTestStepResult(EFail);
	RMutex mutex;
	mutex.CreateGlobal(KMutex);

	// Run the threads, both threads have the ability to send and recieve. Each thread sends 4 messages to the other
	// (Queslots is set to 3). Therefore threads must pre fetch from their inbound queues in order to avoid deadlocking 
	RunThreadsL((TThreadFunction)CSendRecieve::EntryPoint,(TThreadFunction) CSendRecieve::EntryPoint, EPriorityAbsoluteHigh, EPriorityLess, KThread1, KThread2);

	OpenMutexAndWait(mutex);
	if(journal[KThread1Hash][0] == 17 && journal[KThread1Hash][1] == 16 && journal[KThread1Hash][2] ==15 && journal[KThread1Hash][3] ==14 
	&& journal[KThread2Hash][0] ==0 && journal[KThread2Hash][1] ==10 && journal[KThread2Hash][2] ==12 && journal[KThread2Hash][3] ==11)
		{
		SetTestStepResult(EPass);
		}
	TInt MessageCounter = 1;
	for(TInt i = 0; i < KJournalSlots; i++)
		{
		if(journal[KThread1Hash][i] != ENoMessageRecieved)
			{
			INFO_PRINTF2(_L("TCFMessage %d was delivered sucessfully"),MessageCounter++);
			}
		if(journal[KThread2Hash][i] != ENoMessageRecieved)
			{
			INFO_PRINTF2(_L("TCFMessage %d was delivered sucessfully"),MessageCounter++);
			}
		}
	SignalMutexAndClose(mutex);
	VerifyHeapChecks();
	return TestStepResult();
	}
	
TVerdict CTransportTestStep3_5::doTestStepL() 
/**
OOM deadlocked case using dispatch messages, all memory allocations will fail when threads try to drain messages 
from inbound queue
NET-COMNF-FWORK-770012
*/
	{
	__CFLOG_CREATEL;
	SetTestStepResult(EFail);
	RMutex mutex;
	mutex.CreateGlobal(KMutex);
	User::LeaveIfError(mutex.OpenGlobal(KMutex));
	_LIT(KThread1, "Thread3_5_1");
	_LIT(KThread2, "Thread3_5_2");
	
	// Run the threads, both threads have the ability to send and recieve. Each thread sends 4 Large ObjectDispatchItf
	// (blobs)to the other(Queslots is set to 3). When threads pre-fetch from their inbound queues,
	// in order to avoid deadlocking, no memory is available and the threads stay in sleep cycles until
	// the first thread to reach the end of the cycle is panicked
	RunOOMThreadsL((TThreadFunction)COOMDispatchSendRecieve::EntryPoint,(TThreadFunction)COOMDispatchSendRecieve::EntryPoint, KThread1, KThread2,KStandardHeapSize ,KStandardHeapSize);

	if(exitArray[0] == 0)
		{
		SetTestStepResult(EPass);
		INFO_PRINTF1(_L("Thread1 has been correctly panicked with KCFTransportPanic::ECFTransPeerDrainFailure"));
		}
	mutex.Close();
	globalInfo.iThread1DispatchCookie = Messages::TNodeId::NullId();
	globalInfo.iThread2DispatchCookie = Messages::TNodeId::NullId();
	VerifyHeapChecks();
	__CFLOG_DELETE;
	return TestStepResult();
	}


TVerdict CTransportTestStep3_6::doTestStepL() 
/**
OOM deadlocked test case using dispatch messages, after threads enter sleep cycle memory becomes available again
Time interval in config needs to be set to 1000000 
NET-COMNF-FWORK-770013
*/
	{
	__CFLOG_CREATEL;

	SetTestStepResult(EFail);
	RMutex mutex;
	mutex.CreateGlobal(KMutex);
	_LIT(KThread1, "Thread3_6_1");
	_LIT(KThread2, "Thread3_6_2");
	
	// Run the threads, both threads have the ability to send and recieve. Each thread sends 4 Large ObjectDispatchItf
	// (blobs)to the other(Queslots is set to 3). When threads pre-fetch from their inbound queues,
	// in order to avoid deadlocking, no memory is available and the threads stay in sleep cycles. Before the 
	// Threads complete the sleep cycles memory becomes available again and the threads dispatch all messages
	RunOOMThreadsL((TThreadFunction)COOMDispatchSendRecieve::EntryPoint,(TThreadFunction)COOMDispatchSendRecieve::EntryPoint, KThread1, KThread2,KStandardHeapSize ,KStandardHeapSize);
	
	OpenMutexAndWait(mutex);
	VerifyTwoWaySend(EMessageRecieved);
	SignalMutexAndClose(mutex);
	VerifyHeapChecks();
	
	__CFLOG_DELETE;
	return TestStepResult();
	}

TVerdict CTransportTestStep4_1::doTestStepL()
// Send with bad Cookie Arguments interface. NET-COMNF-FWORK-770014
	{
	_LIT(KThread1, "Thread4_1_1");
	_LIT(KThread2, "Thread4_1_2");
	SetTestStepResult(EFail);
	// Run the threads, 1 reciever and 1 sender - The sender posts a Legacy message however the cookie  
	// sender uses contains bad cookie arguments. The sender should be panicked
	RunThreadsL((TThreadFunction)CEmptyRecieve::EntryPoint,(TThreadFunction)CSendBadCookie::EntryPoint, EPriorityAbsoluteHigh, EPriorityLess, KThread1, KThread2);
	if(exitArray[1] == ECFTransInvalidWorkerId)
		SetTestStepResult(EPass);
	else
		{
		SetTestStepResult(EFail);
		INFO_PRINTF2(_L("Recieving Peer was incorrectly panicked or not panicked at all by Sending Thread, Exit Type = %d"), exitArray[0]);
		}
	VerifyHeapChecks();
	return TestStepResult();
	}

		
TVerdict CTransportTestStep5_1::doTestStepL()
// Send on Unregistered interface needs TimeInterval in .ini to be set to over 100000
// NET-COMNF-FWORK-770015
	{
 	SetTestStepResult(EFail);
	globalInfo.iThread2LegacyWorkerId = 0;
 	_LIT(KThread1, "Thread5_1_1");
 	_LIT(KThread2, "Thread5_1_2");

 	// Run the threads, 1 reciever and 1 sender - The sender posts a Legacy message however the cookie  
 	// sender uses has not been registered by the receiver. The reviever should be panicked
 	RunThreadsL((TThreadFunction)CUnregisteredRecieve::EntryPoint, (TThreadFunction)CSend::EntryPoint, EPriorityAbsoluteHigh, EPriorityLess, KThread1, KThread2);

 	if(exitArray[0] == ECFTransLegacyHandlerAbsent)
 		{
 		SetTestStepResult(EPass);
 		INFO_PRINTF1(_L("Recieving Peer was correctly panicked with TCFTransportPanics::ECFTransLegacyHandlerAbsent"));	
 		}
 	else
 		{
 		INFO_PRINTF2(_L("Recieving Peer was incorrectly panicked or not panicked at all, Exit Type = %d"), exitArray[0]);
 		}
 	VerifyHeapChecks();
 	return TestStepResult();
	}
	

TVerdict CTransportTestStep5_3::doTestStepL()
/**
Send on Deregistered interface	
used with EDispatchItfPtr type cookies - needs TimeInterval to be set over 100000 in .ini file
NET-COMNF-FWORK-770017

TODO find out what happens in UREL builds
*/
	{
	_LIT(KThread1, "Thread5_3_1");
	_LIT(KThread2, "Thread5_3_2");
	SetTestStepResult(EFail);
	RMutex mutex;
	mutex.CreateGlobal(KMutex);
	
	// Run the threads, 1 reciever and 1 sender - The sender posts a ObjectDispatchItf message however the cookie  
	// the sender uses has been de-registered by the receiver. The reviever should be panicked
	RunThreadsL((TThreadFunction)CDeregDispatchRecieve::EntryPoint, (TThreadFunction)CSimpleDispatchSend::EntryPoint, EPriorityAbsoluteHigh, EPriorityAbsoluteLow, KThread1, KThread2);
	if(exitArray[0] == ECFInterfaceNotRegistered)
		{
		SetTestStepResult(EPass);
		}
		
	else INFO_PRINTF2(_L("Error:The Recieving peer should have been panicked with TCFTransportPanics::ECFInterfaceNotRegistered ExitType = %d"), exitArray[0]);
	VerifyHeapChecks();
	return TestStepResult();
	}
	
TVerdict CTransportTestStep7_1::doTestStepL()
// Reciever Drops Transport to Sender before Sender gets a chance to Send
// NET-COMNF-FWORK-770019
	{
	_LIT(KThread1, "Thread7_1_1");
	_LIT(KThread2, "Thread7_1_2");
	SetTestStepResult(EFail);
	RMutex mutex;
	mutex.CreateGlobal(KMutex);

	// Run the threads, 1 reciever and 1 sender - The sender posts 3 Legacy messages however the reciever 
	// drops the transport to the sender before the receiver manages to complete the posts
	RunThreadsL((TThreadFunction)CDropTransportRecieve::EntryPoint, (TThreadFunction)CSend::EntryPoint, EPriorityAbsoluteHigh, EPriorityLess, KThread1, KThread2);
	if(exitArray[KThread1Hash] == KErrNone )
		{
		SetTestStepResult(EPass);
		}
	mutex.Close();
	VerifyHeapChecks();
	return TestStepResult();
	}
	
	
TVerdict CTransportTestStep7_2::doTestStepL()
// The Dispatch of the message leaves
// NET-COMNF-FWORK-770020
	{
	RMutex mutex;
	mutex.CreateGlobal(KMutex);
	_LIT(KThread1, "Thread7_2_1");
	_LIT(KThread2, "Thread7_2_2");
	
	// Run the threads, 1 reciever and 1 sender - The sender posts 3 Legacy messages however in the
	// reciever the dispatch of the message leaves and the reciever must handle the leaving dispatch
	RunThreadsL((TThreadFunction)CLeavingRecieve::EntryPoint, (TThreadFunction)CSend::EntryPoint, EPriorityAbsoluteHigh, EPriorityLess, KThread1, KThread2);
	
	OpenMutexAndWait(mutex);
	if (journal[KThread1Hash][5] == -666 && journal[KThread1Hash][0] == TCFCommsMessage::ECodeForward)
		{
		INFO_PRINTF2(_L("This dispatch correctly left with the error: %d"),journal[KThread1Hash][5]);	
		}
	else
		{
		ERR_PRINTF1(_L("This dispatch left incorrectly"));
		}
	SignalMutexAndClose(mutex);
	VerifyHeapChecks();
	return TestStepResult();
	}

static const TInt KMBuf_MBufSize = 128;
static const TInt KMBuf_MinGrowth = 64;
static const TInt KMBuf_GrowthThreshold = 40;
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
void CTransportTestStep::CreateCommsBufPondLC(RCommsBufPondOp& aPond)
	{
	const TUint KMBufDefaultHeapSize	= 256 * 1024;
	
	RArray<TCommsBufPoolCreateInfo> createInfos;
	// Create a pool of mbufs big enough to fill the available heap space
	// Filling the heap at startup avoids having to grow the pools so the growth threshold is ignored.

	TCommsBufPoolCreateInfo createInfo;
	createInfo.iBufSize	=	KMBuf_MBufSize;
	createInfo.iInitialBufs	= (KMBufDefaultHeapSize /(KMBuf_MBufSize+sizeof(RMBuf)))-1;	
	createInfo.iGrowByBufs	= KMBuf_MinGrowth;
	createInfo.iMinFreeBufs = KMBuf_GrowthThreshold;
	createInfo.iCeiling = KMBufDefaultHeapSize;
	createInfos.AppendL(createInfo);

	User::LeaveIfError(aPond.Open(createInfos));
	CleanupClosePushL(aPond);
	}
#else
 
CMBufManager* CTransportTestStep::CreateCMBufManagerLC()
	{
    MMBufSizeAllocator* mbufSizeAllocator;
    CMBufManager* mbufMgr = CMBufManager::NewL(KMBufDefaultHeapSize, mbufSizeAllocator);
    CleanupStack::PushL(mbufMgr);
    if (mbufMgr && mbufSizeAllocator)
         {
         // Create a pool of mbufs big enough to fill the available heap space
         // Filling the heap at startup avoids having to grow the pools so the growth threshold is ignored.
         mbufSizeAllocator->AddL(KMBuf_MBufSize, (KMBufDefaultHeapSize /(KMBuf_MBufSize+sizeof(RMBuf)))-1, KMBuf_MinGrowth, KMBuf_GrowthThreshold);
         }
     return mbufMgr;
     }

#endif // SYMBIAN_ZERO_COPY_NETWORKING
