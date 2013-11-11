// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SERVER.H"
#include "BackGroundCompression.h"

_LIT(KFbsBgCompThrdName, "FbsBgCompressThread");
const TInt KFbsBgCompThrdStackSize = 4096;


CFbsBackgroundCompressionQueueElement::CFbsBackgroundCompressionQueueElement(CBitmapObject* aSrcObj, TBitmapfileCompressionScheme aScheme):
	iSourceObject(aSrcObj),
	iCompressionScheme(aScheme)
	{
	__ASSERT_DEBUG(aSrcObj->iCompressionQueueElement == NULL, User::Invariant());
	aSrcObj->iCompressionQueueElement = this;
	}

CFbsBackgroundCompressionQueueElement::~CFbsBackgroundCompressionQueueElement()
	{
	iLink.Deque();
	iSourceObject->iCompressionQueueElement = NULL;
	if (!iMessage.IsNull())
		iMessage.Complete(KErrNone);
	}

void CFbsBackgroundCompressionQueueElement::CompleteOutstandingRequestAndDestroy(TInt aReason)
	{
	if (!iMessage.IsNull())
		iMessage.Complete(aReason);
	delete this;
	}

void CFbsBackgroundCompressionQueueElement::CompleteOutstandingRequest(const CSession2* aSession)
	{
	if (!iMessage.IsNull() && iMessage.Session() == aSession)
		iMessage.Complete(KErrDisconnected);
	}


CFbsBackgroundCompression* CFbsBackgroundCompression::NewL(CFbTop& aFbTop)
	{
	CFbsBackgroundCompression* self = new(ELeave) CFbsBackgroundCompression(aFbTop);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFbsBackgroundCompression::CFbsBackgroundCompression(CFbTop& aFbTop):
	CActive(CActive::EPriorityIdle),
	iFbTop(aFbTop),
	iCompressionQueue(_FOFF(CFbsBackgroundCompressionQueueElement, iLink))
	{
	CActiveScheduler::Add(this);
	}

void CFbsBackgroundCompression::ConstructL()
	{
	TThreadCreateInfo threadCreateInfo(KFbsBgCompThrdName, (TThreadFunction)ThreadFunction, KFbsBgCompThrdStackSize, this);
	threadCreateInfo.SetUseHeap(NULL);
	threadCreateInfo.SetOwner(EOwnerProcess);
	threadCreateInfo.SetPaging(TThreadCreateInfo::EUnspecified); //Use the creating process's paging attributes.
	User::LeaveIfError(iThread.Create(threadCreateInfo));
	iThreadCreated = ETrue;
	iThread.SetPriority(EPriorityAbsoluteVeryLow);
	User::LeaveIfError(iThreadGo.CreateLocal(0));
	User::LeaveIfError(iThreadMutex.CreateLocal());
	iThread.Resume();
	}

CFbsBackgroundCompression::~CFbsBackgroundCompression()
	{
	// destructor should be called only after all sessions with the server have been closed
	// so there should not be any bitmap objects left
	__ASSERT_DEBUG(iCompressionQueue.IsEmpty(), User::Invariant());
	__ASSERT_DEBUG(iBitmapObject == NULL, User::Invariant());
	if (iThreadCreated)
		{
		iThread.Kill(KErrNone);
		iThread.Close();
		iThreadGo.Close();
		iThreadMutex.Close();
		}
	}

TInt CFbsBackgroundCompression::AddToCompressionQueue(CBitmapObject* aSrcObj, TBitmapfileCompressionScheme aScheme, const RMessage2* aMessage)
	{
	__ASSERT_DEBUG(aSrcObj != NULL, User::Invariant());
	CFbsBackgroundCompressionQueueElement* element = new CFbsBackgroundCompressionQueueElement(aSrcObj, aScheme);
	if (element == NULL)
		return KErrNoMemory;
	iCompressionQueue.AddLast(*element);
	if (!IsActive())
		{
		TInt ret = PrepareCompression();
		if (ret != KErrNone)
			{
			delete element;
			return ret;
			}
		SetActive();
		iThread.Rendezvous(iStatus);
		iThreadGo.Signal();
		}

	if (aMessage != NULL)
		element->iMessage = *aMessage;

	return KErrNone;
	}

void CFbsBackgroundCompression::RemoveFromCompressionQueue(CBitmapObject* aSrcObj)
	{
	__ASSERT_DEBUG(aSrcObj != NULL, User::Invariant());
	CFbsBackgroundCompressionQueueElement* element = aSrcObj->iCompressionQueueElement;
	if (element != NULL)
		{
		if (iBitmapObject != NULL && iCompressionQueue.IsFirst(element))
			{
			iThreadMutex.Wait();
			iBitmapObject->Close();
			iBitmapObject = NULL;
			iThreadMutex.Signal();
			}
		delete element;
		}
	}

void CFbsBackgroundCompression::CompleteOutstandingRequests(const CSession2* aSession)
	{
	TDblQueIter<CFbsBackgroundCompressionQueueElement> iterator(iCompressionQueue);
	CFbsBackgroundCompressionQueueElement* element;
	while ((element = iterator++) != NULL)
		element->CompleteOutstandingRequest(aSession);
	}

void CFbsBackgroundCompression::CompressAll()
	{
	if (iBitmapObject != NULL) // check if a compression is in progress
		{
		User::WaitForRequest(iStatus);
		FinalizeCompression(iStatus);
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrCompletion);
		}
	while (!iCompressionQueue.IsEmpty())
		{
		TInt ret = PrepareCompression();
		if (ret != KErrNone)
			{
			iCompressionQueue.First()->CompleteOutstandingRequestAndDestroy(ret);
			continue;
			}
		TRequestStatus status;
		iThread.Rendezvous(status);
		iThreadGo.Signal();
		User::WaitForRequest(status);
		FinalizeCompression(status);
		}
	}

TInt CFbsBackgroundCompression::PrepareCompression()
	{
	__ASSERT_DEBUG(!iCompressionQueue.IsEmpty(), User::Invariant());
	__ASSERT_DEBUG(iBitmapObject == NULL, User::Invariant());
	CFbsBackgroundCompressionQueueElement* element = iCompressionQueue.First();
	CBitwiseBitmap* srcBmp = element->iSourceObject->Address();
	CBitmapObject* bmpObj = NULL;
	TRAPD(ret, bmpObj = iFbTop.CreateBitmapL(srcBmp->SizeInPixels(), srcBmp->DisplayMode(), KUidCFbsBitmapCreation, ETrue));
	if (ret != KErrNone)
		return ret;
	ret = bmpObj->Address()->CopyData(*srcBmp);
	if (ret != KErrNone)
		{
		bmpObj->Close();
		return ret;
		}
	iThreadMutex.Wait();
	iBitmapObject = bmpObj;
	iCompressionScheme = element->iCompressionScheme;
	iThreadMutex.Signal();
	return KErrNone;
	}

void CFbsBackgroundCompression::FinalizeCompression(const TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(!iCompressionQueue.IsEmpty(), User::Invariant());
	__ASSERT_DEBUG(iBitmapObject != NULL, User::Invariant());
	iThreadMutex.Wait();
	CFbsBackgroundCompressionQueueElement* element = iCompressionQueue.First();
	if (aStatus == KErrNone)
		{
		element->iSourceObject->SetCleanBitmap(iBitmapObject);
		iFbTop.NotifyDirtyBitmap(*element->iSourceObject, NULL);
		}
	else
		{
		iBitmapObject->Close();
		}
	iBitmapObject = NULL;
	element->CompleteOutstandingRequestAndDestroy(aStatus.Int());
	iThreadMutex.Signal();
	}

void CFbsBackgroundCompression::RunL()
	{
	// check if CompressAll() or RemoveFromCompressionQueue() have been called before
	if (iBitmapObject != NULL)
		{
		FinalizeCompression(iStatus);
		}
	while (!iCompressionQueue.IsEmpty())
		{
		TInt ret = PrepareCompression();
		if (ret == KErrNone)
			{
			SetActive();
			iThread.Rendezvous(iStatus);
			iThreadGo.Signal();
			return;
			}
		iCompressionQueue.First()->CompleteOutstandingRequestAndDestroy(ret);
		}
	}

void CFbsBackgroundCompression::DoCancel()
	{
	iThread.RendezvousCancel(iStatus);
	}

TInt CFbsBackgroundCompression::ThreadFunction(CFbsBackgroundCompression* aSelf)
	{
	TBool notDone = ETrue; // to suppress compiler warning
	// coverity[dead_error_condition]
	// coverity[loop_condition]
	while (notDone)
		{
		aSelf->iThreadGo.Wait();
		aSelf->iThreadMutex.Wait();
		if (aSelf->iBitmapObject != NULL)
			{
			TInt ret = aSelf->iBitmapObject->Address()->CompressData(aSelf->iCompressionScheme);
			RThread::Rendezvous(ret);
			}
		else
			{
			// bitmap removed from queue before background thread had a chance to compress it
			RThread::Rendezvous(KErrAbort);
			}
		aSelf->iThreadMutex.Signal();
		}
	// coverity[dead_error_line]
	return KErrNone;
	}
