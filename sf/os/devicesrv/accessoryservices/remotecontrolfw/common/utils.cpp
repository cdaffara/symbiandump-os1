// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "utils.h"
#include <e32msgqueue.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_UTILS);
#endif

#ifdef _DEBUG
PANICCATEGORY("RCFUtils");
#endif

/**
Function passed in as a parameter to TCleanupItem constructor to reset and 
destroy the received aPtr.
@param aPtr A pointer to the object that is to be cleaned up
*/
void ResetAndDestroy(TAny* aPtr)
	{
	ASSERT_DEBUG(aPtr);
	reinterpret_cast<RImplInfoPtrArray*>(aPtr)->ResetAndDestroy();
	}

/**
Function that creates a cleanup item for aArray and specifies the cleanup 
function as ResetAndDestroy. When the item is removed from the cleanup stack 
the function ResetAndDestroy will be called on aArray.
@param aArray The array to be destroyed.
*/
void CleanupResetAndDestroyPushL(RImplInfoPtrArray& aArray)
	{		
	TCleanupItem item(ResetAndDestroy, &aArray);
	CleanupStack::PushL(item);
	}

void SwitchHeap(TAny* aPtr)
	{
	ASSERT_DEBUG(aPtr);
	User::SwitchHeap(reinterpret_cast<RHeap*>(aPtr));
	}

void CleanupSwitchHeapPushL(RHeap& aHeap)
	{
	TCleanupItem item(SwitchHeap, &aHeap);
	CleanupStack::PushL(item);
	}


RNestableLock::RNestableLock()
	: iThreadId(KInvalidThreadId)
	, iRefCount(0)
	{
	LOG_FUNC
	}

TInt RNestableLock::CreateLocal()
	{
	TInt err = iLock.CreateLocal();
	if(err == KErrNone)
		{
		err = iMetaLock.CreateLocal();
		}
	if(err != KErrNone)
		{
		Close();
		}
	return err;
	}

void RNestableLock::Close()
	{
	LOG_FUNC
	iLock.Close();
	iMetaLock.Close();
	iRefCount = 0;
	iThreadId = TThreadId(KInvalidThreadId);
	}

void RNestableLock::Wait()
	{
	iMetaLock.Wait();
	TThreadId currentThreadId = RThread().Id();
	if(iThreadId == TThreadId(KInvalidThreadId) || currentThreadId != iThreadId)
		{
		iMetaLock.Signal();
		iLock.Wait();
		iMetaLock.Wait();
		iThreadId = currentThreadId;
		}
	++iRefCount;
	iMetaLock.Signal();
	}

void RNestableLock::Signal()
	{
	iMetaLock.Wait();
	// Assert if current thread is stored current thread?
	if(--iRefCount == 0)
		{
		iLock.Signal();
		iThreadId = TThreadId(KInvalidThreadId);
		}
	iMetaLock.Signal();
	}




NONSHARABLE_CLASS(CSpecificThreadCallBackBody)
	: public CActive
	{
public:
	static CSpecificThreadCallBackBody* NewL(const TCallBack& aCallBack, TInt aPriority);
	~CSpecificThreadCallBackBody();
	
	TInt Start();
	TInt CallBack();
	void HandleCancel();
	
private:
	CSpecificThreadCallBackBody(const TCallBack& aCallBack, TInt aPriority);
	void ConstructL();
	
	TInt AsyncMessage(TInt aParam);
	
private: // from CActive
	void RunL();
	void DoCancel();
	
private:
	TCallBack		iCallBack;
	
	RThread			iLocalThread;
	
	RMsgQueue<TInt>	iInbound;
	RMsgQueue<TInt>	iOutbound;
	};


RSpecificThreadCallBack::RSpecificThreadCallBack()
	: iBody(NULL)
	{
	LOG_FUNC
	}

TInt RSpecificThreadCallBack::Create(const TCallBack& aCallBack, TInt aPriority)
	{
	TRAPD(err, iBody = CSpecificThreadCallBackBody::NewL(aCallBack, aPriority));
	return err;
	}

void RSpecificThreadCallBack::Close()
	{
	LOG_FUNC
	delete iBody;
	iBody = NULL;
	}

TInt RSpecificThreadCallBack::Start()
	{
	ASSERT_DEBUG(iBody);
	return iBody->Start();
	}

TInt RSpecificThreadCallBack::CallBack()
	{
	ASSERT_DEBUG(iBody);
	return iBody->CallBack();
	}

void RSpecificThreadCallBack::Cancel()
	{
	ASSERT_DEBUG(iBody);
	return iBody->HandleCancel();
	}








CSpecificThreadCallBackBody* CSpecificThreadCallBackBody::NewL(const TCallBack& aCallBack, TInt aPriority)
	{
	LOG_STATIC_FUNC
	CSpecificThreadCallBackBody* self = new(ELeave) CSpecificThreadCallBackBody(aCallBack, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSpecificThreadCallBackBody::CSpecificThreadCallBackBody(const TCallBack& aCallBack, TInt aPriority)
	: CActive(aPriority)
	, iCallBack(aCallBack)
	{
	LOG_FUNC
	}

void CSpecificThreadCallBackBody::ConstructL()
	{
	User::LeaveIfError(iInbound.CreateLocal(1));
	User::LeaveIfError(iOutbound.CreateLocal(1));
	}

CSpecificThreadCallBackBody::~CSpecificThreadCallBackBody()
	{
	LOG_FUNC
	HandleCancel();
	iInbound.Close();
	iOutbound.Close();
	iLocalThread.Close();
	}

TInt CSpecificThreadCallBackBody::Start()
	{
	TInt err = KErrNone;
	if(!IsAdded())
		{
		err = iLocalThread.Duplicate(RThread());
		if(err == KErrNone)
			{
			CActiveScheduler::Add(this);
			iInbound.NotifyDataAvailable(iStatus);
			SetActive();
			}
		}
	return err;
	}

TInt CSpecificThreadCallBackBody::CallBack()
	{
	TInt err = KErrUnknown;
	if(iLocalThread.Id() == RThread().Id())
		{
		// Simple synchronous case.
		err = iCallBack.CallBack();
		}
	else
		{
		RThread thisThread;
		err = thisThread.Duplicate(RThread());
		if(err == KErrNone)
			{
			err = AsyncMessage(thisThread.Handle());
			}
		}
	return err;
	}
	
TInt CSpecificThreadCallBackBody::AsyncMessage(TInt aParam)
	{
	TInt err = KErrNone;
	TRequestStatus logonStatus;
	iLocalThread.Logon(logonStatus);
	if(logonStatus == KErrNoMemory)
		{
		// This seems kludgy, but I think it is the most reliable way.
		User::WaitForRequest(logonStatus); // Ensure the all requests are correct...
		err = KErrNoMemory;
		}
	else
		{
		iInbound.SendBlocking(aParam);
		TRequestStatus status;
		iOutbound.NotifyDataAvailable(status);
		User::WaitForRequest(status, logonStatus);
		if(status == KRequestPending)
			{
			// Remote thread is dead
			iOutbound.CancelDataAvailable();
			User::WaitForRequest(status);
			err = KErrDied;
			}
		else
			{
			// Success (the thread may have subsequently died, but we are only concerned with this call).
			iLocalThread.LogonCancel(logonStatus);
			User::WaitForRequest(logonStatus);
			err = status.Int();
			if(err == KErrNone)
				{
				iOutbound.ReceiveBlocking(err);
				}
			}
		}
	return err;
	}


void CSpecificThreadCallBackBody::RunL()
	{
	TInt threadHandle;
	iInbound.ReceiveBlocking(threadHandle);
	if(threadHandle == 0)
		{
		// 0 is a cancel message
		// therefore don't do anything
		iOutbound.SendBlocking(KErrNone);
		}
	else
		{
		RThread remoteThread;
		remoteThread.SetHandleNC(threadHandle);
		
		TInt result = iCallBack.CallBack();
		
		// There doesn't seem to be a safe way of handling when the other thread
		// dies......
		iOutbound.SendBlocking(result);
		
		remoteThread.Close();
		
		iInbound.NotifyDataAvailable(iStatus);
		SetActive();
		}
	}

void CSpecificThreadCallBackBody::DoCancel()
	{
	if(RThread().Id() == iLocalThread.Id())
		{
		iInbound.CancelDataAvailable();
		}
	else
		{
		// other thread cancelling - so just complete the
		// request
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrCancel);
		}
	}

void CSpecificThreadCallBackBody::HandleCancel()
	{
	if(IsAdded())
		{
		if(RThread().Id() == iLocalThread.Id())
			{
			Cancel(); // synchronous cancel is fine in same thread...
			}
		else
			{
			// In a different thread - this is more interesting...
			TInt err = AsyncMessage(0); // 0 is special as it means cancel.
			if(err == KErrDied && IsActive())
				{
				// Remote thread has already died so we need to tidy up the
				// active object ourselves.
				Cancel();
				}
			}
		}
	// else shouldn't be active...
	}


#ifdef _DEBUG
_LIT(KCountSizeWriteStreamPanic, "CountStream");
#endif // _DEBUG
#define ASSERT_NOT_CALLED __ASSERT_DEBUG(EFalse, User::Panic(KCountSizeWriteStreamPanic, 0));

RCountSizeWriteStream::RCountSizeWriteStream()
	: RWriteStream()
	, iSize(0)
	{
	Attach(this);
	}

TInt RCountSizeWriteStream::Size() const
	{
	return iSize;
	}

void RCountSizeWriteStream::Reset()
	{
	iSize = 0;
	}

void RCountSizeWriteStream::DoRelease()
	{
	// No resources to free - Nothing to do
	}

void RCountSizeWriteStream::DoSynchL()
	{
	// No buffering - Nothing to do
	}

TInt RCountSizeWriteStream::DoReadL(TAny* /*aPtr*/,TInt /*aMaxLength*/)
	{
	LOG_FUNC
	ASSERT_NOT_CALLED;
	User::Leave(KErrNotSupported);
	return 0;
	}

TInt RCountSizeWriteStream::DoReadL(TDes8& /*aDes*/,TInt /*aMaxLength*/,TRequestStatus& /*aStatus*/)
	{
	LOG_FUNC
	ASSERT_NOT_CALLED;
	User::Leave(KErrNotSupported);
	return 0;
	}

TStreamTransfer RCountSizeWriteStream::DoReadL(MStreamInput& /*anInput*/,TStreamTransfer /*aTransfer*/)
	{
	LOG_FUNC
	ASSERT_NOT_CALLED;
	User::Leave(KErrNotSupported);
	return 0;
	}

void RCountSizeWriteStream::DoWriteL(const TAny* /*aPtr*/,TInt aLength)
	{
	iSize += aLength;
	}

TInt RCountSizeWriteStream::DoWriteL(const TDesC8& /*aDes*/,TInt aMaxLength,TRequestStatus& aStatus)
	{
	iSize += aMaxLength;
	TRequestStatus* stat = &aStatus;
	User::RequestComplete(stat, KErrNone);
	return aMaxLength;
	}

TStreamTransfer RCountSizeWriteStream::DoWriteL(MStreamOutput& /*anOutput*/,TStreamTransfer /*aTransfer*/)
	{
	ASSERT_NOT_CALLED;
	User::Leave(KErrNotSupported);
	return 0;
	}

TStreamPos RCountSizeWriteStream::DoSeekL(TMark /*aMark*/,TStreamLocation /*aLocation*/,TInt /*anOffset*/)
	{
	ASSERT_NOT_CALLED;
	User::Leave(KErrNotSupported);
	return 0;
	}

#undef ASSERT_NOT_CALLED


