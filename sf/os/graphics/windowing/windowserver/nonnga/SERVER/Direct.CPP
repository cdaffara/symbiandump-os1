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
// Direct Screen Access class, to allow the client to draw directly to the screen
// 
//

#include <e32base.h>
#include "Direct.H"
#include "server.h"
#include "rootwin.h"
#include "wstop.h"
#include "panics.h"
#include <e32msgqueue.h>

const TInt KMsgQueueLength = 1;

static TBool RegionsMatch(const TRegion & aOne, const TRegion & aTwo);		


CDsaMsgQueue* CDsaMsgQueue::NewL(CWsDirectScreenAccess* aDirect)
	{
	CDsaMsgQueue* self = new(ELeave)CDsaMsgQueue;
	CleanupStack::PushL(self);
	self->ConstructL(aDirect);
	CleanupStack::Pop(self);
	return self;
	}

CDsaMsgQueue::CDsaMsgQueue()
{}

CDsaMsgQueue::~CDsaMsgQueue()
	{
	delete iAborted;
	iSendQueue.Close();
	iRecQueue.Close();
	}

void CDsaMsgQueue::ConstructL(CWsDirectScreenAccess* aDirect)
	{
	iAborted=new(ELeave) CWsAbortDirect(aDirect,this);	
	}

void CDsaMsgQueue::Cancel()
	{
	//purge the queue of data
	TInt ret = KErrNone;
	do
		{
		TInt data = 0;
		ret = iRecQueue.Receive(&data,sizeof(TInt));
		}while(ret == KErrNone);
	iAborted->Cancel();
	}

TInt CDsaMsgQueue::Send(TInt aData)
	{
	return iSendQueue.Send(&aData,sizeof(TInt));
	}

TInt CDsaMsgQueue::CreateSendQueue()
	{
	if(iSendQueue.Handle() == 0)
		{
		return iSendQueue.CreateGlobal(KNullDesC,KMsgQueueLength,sizeof(TInt), EOwnerProcess);
		}
	else
		{
		return 0;
		}
	}

TInt CDsaMsgQueue::CreateRecQueue()
	{
	if(iRecQueue.Handle() == 0)
		{
		return iRecQueue.CreateGlobal(KNullDesC,KMsgQueueLength,sizeof(TInt), EOwnerProcess);
		}
	else
		{
		return 0;
		}
	}

RMsgQueueBase* CDsaMsgQueue::SendQueue()
	{
	return &iSendQueue;
	}

RMsgQueueBase* CDsaMsgQueue::RecQueue()
	{
	return &iRecQueue;
	}

void CDsaMsgQueue::Started()
	{
	iAborted->Started();
	}
	
void CDsaMsgQueue::Complete()
	{
	iAborted->Complete(RDirectScreenAccess::ETerminateCancel);	
	}

void CDsaMsgQueue::CompleteAbort()
	{
	iAborted->Complete(KErrNone);	
	}

TInt CDsaMsgQueue::ReceiveData()
	{
	TInt data = 0;
	iRecQueue.Receive(&data,sizeof(TInt));
	return data;
	}

/*CWsAbortDirect*/

CWsAbortDirect::CWsAbortDirect(CWsDirectScreenAccess* aDirect,CDsaMsgQueue* aParent) : CActive(EDirectAbort), iDirect(aDirect),iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}

CWsAbortDirect::~CWsAbortDirect()
	{
	Cancel();
	}

void CWsAbortDirect::Started()
	{
	iStatus=KRequestPending;
	TRequestStatus& status = iStatus;	
	iParent->RecQueue()->NotifyDataAvailable(status);
	SetActive();
	}

void CWsAbortDirect::RunL()
	{
	iParent->ReceiveData();
	iParent->RecQueue()->CancelDataAvailable();
	iDirect->Aborted();
	}

void CWsAbortDirect::DoCancel()
	{
	iParent->RecQueue()->CancelDataAvailable();
	if (iStatus==KRequestPending)
		{
		Complete(KErrNone);
		}
	}

void CWsAbortDirect::Complete(TInt aReason)
	{
	if(IsActive())
		{
		TRequestStatus* status=&iStatus;
		RThread().RequestComplete(status,aReason);
		}
	}


/*CWsDirectScreenAccess*/

CWsDirectScreenAccess* CWsDirectScreenAccess::NewL(CWsClient* aOwner)
	{
	CWsDirectScreenAccess* self = new(ELeave) CWsDirectScreenAccess(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CWsDirectScreenAccess::~CWsDirectScreenAccess()
	{
	DeleteRegionSyncWatchcat();
	iFrozenRegion.Close();
	iVisible.Close();
	if (iStatus!=EDirectStatusTimeNotCreated)
		{
		if (iStatus==EDirectStatusRunning)
			AbortNow();
		if (iStatus>=EDirectStatusAborted)
			CorrectScreen();
		}
	WS_ASSERT_DEBUG(!OnQueue(), EWsPanicDirectScreenAccess);
	delete iMsgQueue;
	}

void CWsDirectScreenAccess::ConstructL()
	{
	NewObjL();
	iMsgQueue = CDsaMsgQueue::NewL(this);
	iStatus=EDirectStatusNone;
	}

void CWsDirectScreenAccess::Request(TInt handle)
	{
	if (iStatus!=EDirectStatusNone)
		{
		if (iStatus==EDirectStatusCompleted)
			{
			iMsgQueue->Cancel();
			}
		else
			{
			iWsOwner->PPanic(EWservPanicDirectMisuse);
			}
		}
	iWsOwner->HandleToClientWindow(handle,&iWin);
	iScreen=iWin->Screen();
	STACK_REGION region;
	iWin->GenerateTopRegion(region);
	const TInt regionCount=region.Count();
	region.Close();
	SetReply(regionCount);
	iStatus=EDirectStatusInitialising;
	}

void CWsDirectScreenAccess::GetRegion(TInt aNumRects)
	{
#if defined(_DEBUG)
	if (iStatus!=EDirectStatusInitialising)
		iWsOwner->PPanic(EWservPanicDirectMisuse);
#endif
	STACK_REGION region;
	iWin->GenerateTopRegion(region);
	const TInt regionCount=region.Count();
	if (region.Count()==aNumRects)
		{
		iVisible.Copy(region);
		if (iVisible.CheckError())
			{
			iStatus=EDirectStatusNone;
			SetReply(KErrNotReady);
			}
		else
			{
			TPtrC8 rectList(REINTERPRET_CAST(const TUint8*,region.RectangleList()),region.Count()*sizeof(TRect));
			CWsClient::ReplyBuf(rectList);
			iStatus=EDirectStatusRunning;
			Initiate();
			iMsgQueue->Started();
			SetReply(KMaxTInt);
			}
		}
	else
		{
		SetReply(region.Count());
		}
	region.Close();
	}

void CWsDirectScreenAccess::Cancel()
	{
	TInt ret = 0;
	switch (iStatus)
		{
#if defined(_DEBUG)
	case EDirectStatusInitialising:
		iWsOwner->PPanic(EWservPanicDirectMisuse);
#endif
	case EDirectStatusNone:
		break;
	case EDirectStatusRunning:
		Terminate1();
		Terminate2(); 
		/*Fall through*/
	case EDirectStatusCanceling:
		ret = 1;
		iMsgQueue->Send(RDirectScreenAccess::ETerminateCancel);	
		break;
	case EDirectStatusAbortedWindow:
	case EDirectStatusAbortedGlobal:
		CorrectScreen();
		break;
	case EDirectStatusCompleted:
		break;
	default:
			{
			}
		}
	SetReply(ret);
	iStatus=EDirectStatusNone;
	}

void CWsDirectScreenAccess::Aborted()
	{
	switch (iStatus)
		{
	case EDirectStatusRunning:
		Terminate1();
		Terminate2();
		iStatus=EDirectStatusCanceling;
		break;
	case EDirectStatusAbortedWindow:
	case EDirectStatusAbortedGlobal:
		CorrectScreen(); 
		/*Fall through*/
	case EDirectStatusCompleted:
		iStatus=EDirectStatusNone;
		break;
	default:
		iWsOwner->PPanic(EWservPanicDirectMisuse);
		}
	}

void CWsDirectScreenAccess::AbortNow()
	{
	if (iStatus!=EDirectStatusRunning)
		{
		iWsOwner->PPanic(EWservPanicDirectMisuse);
		}
	SignalAbort(RDirectScreenAccess::ETerminateRegion);
	TRequestStatus timerStatus;
	RTimer& timer=CWsTop::Timer();
	timer.After(timerStatus,400000);		//0.4secs
	User::WaitForRequest(iMsgQueue->Status(),timerStatus);
	if (timerStatus!=KRequestPending)
		{
		Abort();
		}
	else
		{
		CancelAbortObject();
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	}

void CWsDirectScreenAccess::SignalAbort(RDirectScreenAccess::TTerminationReasons aReason)
	{
	DeleteRegionSyncWatchcat();
	
	iAbortReason=aReason;
	if (iStatus==EDirectStatusAbortedWindow)
		{
		WS_ASSERT_DEBUG(iAbortReason>RDirectScreenAccess::ETerminateRegion, EWsPanicDirectScreenAccess);
		Terminate2();
		return;
		}
	if (iStatus!=EDirectStatusRunning)
		{
		iWsOwner->PPanic(EWservPanicDirectMisuse);
		}
	if (RDirectScreenAccess::ETerminateCancel != aReason)
		{
		TInt err;
		TRAP(err, iRegionSync = CWsDirectScreenAccess::CDSARegionSyncTimer::NewL( *this ) );
		if ( KErrNone == err )
			{
			iFrozenRegion.Copy( iVisible );			
			Screen()->DSARegionSyncStart( *this );
			}
		}
	iMsgQueue->Send(aReason);
	}

void CWsDirectScreenAccess::CancelAbortObject()
	{
	iMsgQueue->Complete();
	iMsgQueue->Cancel();
	iStatus=EDirectStatusNone;
	Terminate1();
	Terminate2();
	}

void CWsDirectScreenAccess::Abort()
	{
	if ( iStatus == EDirectStatusRunning ) 	
		{
		Terminate1();
		if (iMsgQueue->Status()==KRequestPending)
			iStatus=(iAbortReason<=RDirectScreenAccess::ETerminateRegion ? EDirectStatusAbortedWindow:EDirectStatusAbortedGlobal);
		else
			iStatus=EDirectStatusCompleted;
		if (iStatus!=EDirectStatusAbortedWindow)
			Terminate2();
		}
	}


TInt CWsDirectScreenAccess::GetSendQueue()
	{
	TInt ret = iMsgQueue->CreateSendQueue();
	if(ret == KErrNone)
		{
		iWsOwner->SetResponseHandle(iMsgQueue->SendQueue());	
		}
	return ret;
	}

TInt CWsDirectScreenAccess::GetRecQueue()
	{
	TInt ret = iMsgQueue->CreateRecQueue();
	if(ret == KErrNone)
		{
		iWsOwner->SetResponseHandle(iMsgQueue->RecQueue());	
		}
	return ret;
	}


void CWsDirectScreenAccess::CommandL(TInt aOpcode,const TAny* aCmdData)
	{
	TWsDirectCmdUnion pData;
	pData.any=aCmdData;
	switch(aOpcode)
		{
		case EWsDirectOpFree:
			delete this;
			break;
		case EWsDirectOpRequest:
			Request(*pData.Int);
			break;
		case EWsDirectOpInitFailed:
	#if defined(_DEBUG)
			if (iStatus!=EDirectStatusInitialising)
				{
				iWsOwner->PPanic(EWservPanicDirectMisuse);
				}
	#endif
			iStatus=EDirectStatusNone;
			break;
		case EWsDirectOpGetRegion:
			GetRegion(*pData.Int);
			break;
		case EWsDirectOpCancel:
			Cancel();
			break;
		case EWsDirectOpGetSendQueue:
			GetSendQueue();
			break;
		case EWsDirectOpGetRecQueue:
			GetRecQueue();
			break;
		default:
			OwnerPanic(EWservPanicOpcode);
			break;
		}
	}

void CWsDirectScreenAccess::Initiate()
	{
	iWin->AddDSA(*this);
	iScreen->AddDirect(*this);
	if ( IsSyncTimeoutPending() )
		{
		Screen()->DSARegionSyncOver( *this );
		
		RRegion deltaRgn;
		deltaRgn.Copy( iFrozenRegion );
		
		deltaRgn.SubRegion( iVisible );
		deltaRgn.Tidy();
		
		if ( !deltaRgn.IsEmpty() )
			{
			Screen()->ScheduleRegionUpdate( &deltaRgn );
			}
		
		deltaRgn.Close();
		iFrozenRegion.Clear();

		DeleteRegionSyncWatchcat();
		}
	}
TBool CWsDirectScreenAccess::IsAbortRequired(const TRegion& aTopVisibleRegion) const
	{
	return ( iStatus == EDirectStatusRunning ) && !RegionsMatch(aTopVisibleRegion, iVisible);
	}

void CWsDirectScreenAccess::Terminate1()
	{
	WS_ASSERT_DEBUG(!iWin->DSAs().IsEmpty(), EWsPanicDirectScreenAccess);
	iWin->RemoveDSA(*this);
	}

void CWsDirectScreenAccess::Terminate2()
	{
	iScreen->RemoveDirect(*this);
	WS_ASSERT_DEBUG(!OnQueue(), EWsPanicDirectScreenAccess);
	}

void CWsDirectScreenAccess::CorrectScreen()
	{
	if (iAbortReason<=RDirectScreenAccess::ETerminateRegion)
		{
		Terminate2();
		RootWindow()->Invalidate(&iVisible);
		}
	else
		RootWindow()->InvalidateWholeScreen();

	WS_ASSERT_DEBUG(!OnQueue(), EWsPanicDirectScreenAccess);
	}



#if defined(_DEBUG)
TBool CWsDirectScreenAccess::OnQueue()
	{
	return iScreen->IsDirectOnQueue(this);
	}
#endif


TBool CWsDirectScreenAccess::IsSyncTimeoutPending() const
	{
	TBool res;
	res = ( NULL != iRegionSync ) && ( iRegionSync->IsActive() );
	return res; 
	}

void CWsDirectScreenAccess::CancelFrozenRegion()
	{
	if ( !iFrozenRegion.IsEmpty() )
		{
		Screen()->DSARegionSyncOver( *this );
		Screen()->ScheduleRegionUpdate( &iFrozenRegion );
		iFrozenRegion.Clear();
		}	
	}

void CWsDirectScreenAccess::DeleteRegionSyncWatchcat()
	{
	if ( NULL != iRegionSync )
		{
		if ( iRegionSync->IsActive() )
			{
			iRegionSync->Cancel();			
			}
		delete iRegionSync;
		iRegionSync = NULL;
		}
	CancelFrozenRegion();
	}	

void CWsDirectScreenAccess::RegionSyncTimeout()
	{
	CancelFrozenRegion();
	}

CWsDirectScreenAccess::CDSARegionSyncTimer* CWsDirectScreenAccess::CDSARegionSyncTimer::NewL(CWsDirectScreenAccess& aDSA)
	{
	CWsDirectScreenAccess::CDSARegionSyncTimer* self = new ( ELeave ) CWsDirectScreenAccess::CDSARegionSyncTimer( aDSA );
	
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	
	CActiveScheduler::Add( self );
	self->After( TTimeIntervalMicroSeconds32( KRegionSyncTimeoutMicrosec ) );
	return self;
	}
	
void CWsDirectScreenAccess::CDSARegionSyncTimer::RunL()
	{
	iDSA.RegionSyncTimeout();
	}

CWsDirectScreenAccess::CDSARegionSyncTimer::CDSARegionSyncTimer(CWsDirectScreenAccess& aDSA):
	CTimer( CActive::EPriorityHigh ),
	iDSA( aDSA )
	{ } // empty	


static TBool RegionsMatch(const TRegion & aOne, const TRegion & aTwo)
	{
	// Check if the regions have equal areas.
	const TRect* rect1 = aOne.RectangleList();
	TUint area1 = 0;
	for(TInt i = 0; i < aOne.Count(); ++i)
		{
		area1 += (rect1->Width() * rect1->Height());
		rect1++;
		}

	const TRect* rect2 = aTwo.RectangleList();
	TUint area2 = 0;
	for(TInt i = 0; i < aTwo.Count(); ++i)
		{
		area2 += (rect2->Width() * rect2->Height());
		rect2++;
		}
	
	if(area1 != area2)
		{
		return EFalse;
		}

	// Check if one region is completely contained within the other.
	STACK_REGION tempRegion;
	tempRegion.Copy(aOne);	
	tempRegion.SubRegion(aTwo);

	const TBool ret(tempRegion.IsEmpty());
	tempRegion.Close();

	return ret;
	}

