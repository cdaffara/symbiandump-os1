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
// Client side classes for handling direct screen access
// 
//

#include <e32std.h>
#include <e32base.h>
#include "../SERVER/w32cmd.h"
#include "CLIENT.H"
#include "w32comm.h"
#include <e32msgqueue.h>

NONSHARABLE_CLASS(CDsaMsgQueue) : public CActive
{
	public:
		CDsaMsgQueue();
		~CDsaMsgQueue();
		void Request(TRequestStatus* aClientRequest);
		TBool Started() { return iStarted;}
		TBool Completed();
		void OpenRecQueue(TInt aHandle);
		void OpenSendQueue(TInt aHandle);
		TInt Send(TInt aData);
		RMsgQueueBase& SendQueue() {return iSendQueue; }
		RMsgQueueBase& Queue() { return iRecQueue; }
		TRequestStatus* Status() { return &iStatus; }
		TBool RequestStarted() { return iStarted;}
	private:
		void DoCancel();
		void RunL();
		void Listen();
		
	private:
		RMsgQueueBase iRecQueue;
		RMsgQueueBase iSendQueue;
		TRequestStatus* iClientRequest;
		TBool iStarted;
		RThread* iServer;
};

//
CDsaMsgQueue::CDsaMsgQueue() : CActive(RDirectScreenAccess::EPriorityVeryHigh)
	{
	CActiveScheduler::Add(this);
	}

CDsaMsgQueue::~CDsaMsgQueue()
	{
	Cancel();
	iRecQueue.Close();
	iSendQueue.Close();
	}

TInt CDsaMsgQueue::Send(TInt aData)
	{
	return iSendQueue.Send(&aData,sizeof(TInt));
	}

void CDsaMsgQueue::OpenRecQueue(TInt aHandle)
	{
	iRecQueue.SetHandle(aHandle);
// With RmessagePtr2 compelete using an RHandle the returned handle is already duplicated
	}

void CDsaMsgQueue::OpenSendQueue(TInt aHandle)
	{
	iSendQueue.SetHandle(aHandle);
// With RmessagePtr2 compelete using an RHandle the returned handle is already duplicated
	}

void CDsaMsgQueue::DoCancel()
	{
	iRecQueue.CancelDataAvailable();
	TInt ret = KErrNone;
	do
		{
		TInt data = 0;
		ret = iRecQueue.Receive(&data,sizeof(TInt));
		}while(ret == KErrNone);
	if(iClientRequest)
		{
		RThread().RequestComplete(iClientRequest,KErrCancel);
		}
	}
	
void CDsaMsgQueue::RunL()
	{
	// get the data from the msg queue
	TInt reason = 0;
	iRecQueue.Receive(&reason,sizeof(TInt));
	
	if(iClientRequest)
		{
		// if there is an outstanding client request, complete and pass on the abort reason
		User::RequestComplete(iClientRequest,reason);
		iClientRequest = NULL;
		}
	}

void CDsaMsgQueue::Listen()
	{
	if(!IsActive())
		{
		SetActive();	
		iRecQueue.NotifyDataAvailable(iStatus);
		}
	}

void CDsaMsgQueue::Request(TRequestStatus* aClientRequest)
	{
	__ASSERT_ALWAYS(!IsActive(),User::Invariant());
	iClientRequest = aClientRequest;
	iStarted = ETrue;
	Listen();
	}

TBool CDsaMsgQueue::Completed()
	{
	if(iStarted)
		{
		Send(KErrNone);
		iStarted = EFalse;
		return ETrue;
		}
	return EFalse;
	}
	
//
// RDirectScreenAccess
//

EXPORT_C RDirectScreenAccess::RDirectScreenAccess()
/** Default constructor.

Developers should use the other constructor overload instead. */
	{
	}

EXPORT_C RDirectScreenAccess::RDirectScreenAccess(RWsSession& aWs) : MWsClientClass(aWs.iBuffer), iWs(&aWs), iMsgQueue(NULL)
/** C++ constructor with a connected window server session.

Construct() must be called to complete construction.

@param aWs Connected session with the window server. */
	{
	}

EXPORT_C TInt RDirectScreenAccess::Construct()
/** Second phase constructor.

Creates the server side resource and initialises the client's handle to it.

This function always causes a flush of the window server buffer.

@return KErrNone if successful, otherwise one of the system wide error codes. 
@panic TW32Panic 17 in debug builds if called on an already constructed object.*/
	{
	__ASSERT_DEBUG(iWsHandle == KNullHandle, Panic(EW32PanicGraphicDoubleConstruction));
	TInt ret = KErrNone;
	if ((ret = iBuffer->WriteReplyWs(EWsClOpCreateDirectScreenAccess)) >= 0)
		{
		iWsHandle = ret;
		TRAP(ret,iMsgQueue = new (ELeave)CDsaMsgQueue);
		if(ret == KErrNone)
			{
			// the servers send queue is the client receive queue
			TInt h = WriteReply(EWsDirectOpGetSendQueue);
			iMsgQueue->OpenRecQueue(h);	
		
			// servers receive queue is the clients send queue
			h = WriteReply(EWsDirectOpGetRecQueue);
			iMsgQueue->OpenSendQueue(h);	
			}
		else
			{
			Close();
			}
		}
	return(ret);
	}

EXPORT_C TInt RDirectScreenAccess::Construct(TBool /*aRegionTrackingOnly*/)
/** Second phase constructor.
This is not supported in WSERV non NGA. It's available just when NGA is present.*/
	{
	return KErrNotSupported;
	}

EXPORT_C TInt RDirectScreenAccess::Request(RRegion*& aRegion,TRequestStatus& aStatus,const RWindowBase& aWindow)
/** Issues a request to the window server for permission to perform direct screen 
access on a window.

Direct access to the screen may be refused due to lack of memory or if the 
target window is completely obscured.

If direct access is allowed, the function passes back a clipping region which 
is the part of the screen the caller can draw to. 

When direct screen access must stop, for instance because a dialog is to be 
displayed in front of the region where direct screen access is taking place, 
the window server completes the request. The recommended way to check for 
this is for aStatus to be the request status of an active object that will 
be run when the request completes, i.e. if Request() returns KErrNone, call 
SetActive(), and in the object's RunL(), you should immediately abort direct 
screen access.

While the DSA is in operation, it is strongly advised that the client should 
not make any call to WSERV that will affect the visible area of the window in 
which the DSA is taking place. 

When WSERV tells the client that it needs to abort its DSA, it waits to receive
the acknowledgment from the client that it has done so. However, it doesn't wait 
for ever, since the client may have entered some long running calculation or even
an infinite loop. So WSERV also waits on a timer: if the timer expires before the
client acknowledges, then WSERV continues; if, later on, WSERV gets notification
from the client that it has aborted the DSA, then WSERV will invalidate the region
in which the DSA was taking place, just in case there had been a conflict between
the DSA and another client.


This function always causes a flush of the window server buffer.

@param aRegion On return, the clipping region that the caller can draw to. 
NULL if the function was not successful.
If the target window is invisible or completely covered by other windows
then the region will be empty.
@param aStatus A request status that is set to a completion code by the window 
server when direct screen access must stop.
@param aWindow The window that you want to perform the direct screen access 
on. There must not already be direct access on this window or a panic occurs.
@return KErrNone if the request was successful, KErrNone with empty region if 
none of the window is currently visible, otherwise one of the system wide error codes,
e.g. KErrNoMemory if out of memory. */
	{
	__ASSERT_ALWAYS(iMsgQueue,Panic(EW32PanicDirectMisuse));

	aRegion = NULL;

	// Allocate the memory for the RRegion here so it is simple to back out
	// in case of failure
	TAny* regionMem = User::Alloc (sizeof (RRegion));
	if (!regionMem)
		{
		return KErrNoMemory;
		}

	TInt ret = WriteReplyInt(aWindow.WsHandle(),EWsDirectOpRequest);
	if (ret<KErrNone)
		{
		User::Free (regionMem);
		return ret;
		}
	TRect* rectList = NULL;
	TRect* newRectList;
	TInt numRect;

	do
		{
		numRect = ret;
		newRectList = STATIC_CAST(TRect*,User::ReAlloc(rectList,numRect*sizeof(TRect)));
		if (!newRectList)
			{
			Write(EWsDirectOpInitFailed);
			User::Free (regionMem);
			delete rectList;
			return KErrNoMemory;
			}
		rectList = newRectList;
		TPtr8 ptr(REINTERPRET_CAST(TUint8*,rectList),ret*sizeof(TRect));
		ret = WriteReplyIntP(ret,&ptr,EWsDirectOpGetRegion);
		} while(ret >=0 && ret != KMaxTInt);
	if (ret<0)
		{
		User::Free (regionMem);
		delete rectList;
		return ret;
		}

	aRegion = new (regionMem) RRegion (numRect, rectList);
	aStatus = KRequestPending;
	iMsgQueue->Request(&aStatus);
	iWs->DirectAcessActivation(ETrue);
	return KErrNone;
	}

EXPORT_C void RDirectScreenAccess::Completed()
/** Indicates to the window server that you have responded to the completion of 
the request status passed to Request(), by stopping direct screen access. */
	{
	__ASSERT_ALWAYS(iMsgQueue->Started(),Panic(EW32PanicDirectMisuse));
	if(iMsgQueue->Completed())
		{
		iWs->DirectAcessActivation(EFalse);
		}
	}

EXPORT_C void RDirectScreenAccess::Cancel()
/** Indicates to the window server that you have finished performing direct screen 
access. */
	{
	if(iMsgQueue->Started())
		{
		Completed();
		}
	TInt ret = WriteReply(EWsDirectOpCancel);
	if(ret != 0) // the server is sending us some data.
		{
		iMsgQueue->Queue().CancelDataAvailable();
		TInt data = 0;
		iMsgQueue->Queue().ReceiveBlocking(&data,sizeof(TInt));
		}
	iMsgQueue->Cancel();
	}

EXPORT_C void RDirectScreenAccess::Close()
/** Calls Completed() then deletes the server side resource and sets the client's 
handle to it to NULL. */
	{
	if (iBuffer && iWsHandle)
		{
		if(iMsgQueue && iMsgQueue->Started())
			{
			Completed();
			}
		Write(EWsDirectOpFree);
		delete iMsgQueue;
		iMsgQueue = NULL;
		}
	iWsHandle = NULL;
	}

//
// CDirectScreenAccess
//

EXPORT_C CDirectScreenAccess* CDirectScreenAccess::NewL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWin,MDirectScreenAccess& aAbort)
/** Allocates and constructs the object and adds it to the current active scheduler.

This function always causes a flush of the window server buffer.

@param aWs A session with the window server.
@param aScreenDevice Specifies the characteristics of the screen device to 
draw to.
@param aWin The window to draw to directly.
@param aAbort Defines an AbortNow() and a Restart() function which are both 
called on aborting, as part of the RunL(). Restart() is called from an idle 
time active object (CIdle).
@return The newly constructed object. */
	{
	CDirectScreenAccess* self = new(ELeave) CDirectScreenAccess(aWs,&aScreenDevice,aWin,aAbort);
	CleanupStack::PushL(self);
	self->ConstructL(aWs,EFalse); //this EFalse has no meaning here, it is used just to comply with the changes in NGA code
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDirectScreenAccess* CDirectScreenAccess::NewL(RWsSession& /*aWs*/,CWsScreenDevice&/* aScreenDevice*/,RWindowBase&/* aWin*/,MDirectScreenAccess&/*aAbort*/,TBool /*aRegionTrackingOnly*/)
/** This is not supported in WSERV non NGA. It's available just when NGA is present.*/
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CDirectScreenAccess::~CDirectScreenAccess()
	{
	__ASSERT_ALWAYS(!iAborting,Panic(EW32PanicDirectMisuse));
	Cancel();
	delete iGc;
	delete iScreenDevice;
	if (iDrawingRegion)
		iDrawingRegion->Destroy();
	iDirectAccess.Close();
	delete iRestart;
	}

void CDirectScreenAccess::ConstructL(RWsSession& aWs,TBool /*aRegionTrackingOnly*/)
	{
	iScreenNumber = iWsScreenDevice->GetScreenNumber();
	
	User::LeaveIfError(iDirectAccess.Construct());
	iRestart = CIdle::NewL(RDirectScreenAccess::EPriorityVeryHigh-5);
	CActiveScheduler::Add(this);
	if (aWs.GetColorModeList(NULL)>1)
		iFlags |= EDirectCheckModeChange;
	if (iWsScreenDevice->NumScreenModes() == 1)
		{
		if (iWsScreenDevice->GetRotationsList(0,NULL) == 1)
			return;
		}
	iFlags |= EDirectCheckSizeModeChange;
	}

void CDirectScreenAccess::CreateScreenObjectsL(TDisplayMode aCurrentMode)
	{
	delete iScreenDevice;
	iScreenDevice = NULL;
	
	iScreenDevice = CFbsScreenDevice::NewL(iScreenNumber,aCurrentMode);
	
	if (iGc)
		iGc->Activate(iScreenDevice);
	else
		{
		User::LeaveIfError(iScreenDevice->CreateContext(iGc));
		if (!(iFlags&EDirectCheckSizeModeChange))
			UpdateSizeAndRotation(iGc);
		}
	}

EXPORT_C void CDirectScreenAccess::StartL()
/** Informs the window server that you are going to start direct screen access 
and sets up a graphics context with which you can draw to the screen.

It should also be called to restart direct screen access after Cancel() has 
been called to stop it. 

While the DSA is in operation, it is strongly advised that the client should 
not make any call to WSERV that will affect the visible area of the window in 
which the DSA is taking place. 

When WSERV tells the client that it needs to abort its DSA, it waits to receive
the acknowledgment from the client that it has done so. However, it doesn't wait
for ever, since the client may have entered some long running calculation or even
an infinite loop. So WSERV also waits on a timer: if the timer expires before the
client acknowledges, then WSERV continues; if, later on, WSERV gets notification
from the client that it has aborted the DSA, then WSERV will invalidate the region
in which the DSA was taking place, just in case there had been a conflict between
the DSA and another client.


This function always causes a flush of the window server buffer. */
	{
	if (iDrawingRegion)
		iDrawingRegion->Destroy();
	User::LeaveIfError(iDirectAccess.Request(iDrawingRegion,iStatus,iWindow));
	SetActive();
	if ((iFlags&EDirectCheckModeChange) || iScreenDevice == NULL)
		{
		TDisplayMode currentDisplayMode = iWsScreenDevice->DisplayMode();
		if (iScreenDevice == NULL || currentDisplayMode != iScreenDevice->DisplayMode())
			{
			TRAPD(err,CreateScreenObjectsL(currentDisplayMode));
			if (err != KErrNone)
				{
				Cancel();
				User::Leave(err);
				}
			}
		}
	if (iFlags&EDirectCheckSizeModeChange)
		UpdateSizeAndRotation(iGc);
	iGc->SetOrigin(iWindow.AbsPosition());
	iDrawingRegion->ClipRect(iScreenSize);
	iGc->SetClippingRegion(iDrawingRegion);
	}

TInt CDirectScreenAccess::Restart(TAny* aDirect)		//static
	{
	STATIC_CAST(CDirectScreenAccess*,aDirect)->Restart();
	return(KErrNone);
	}

void CDirectScreenAccess::Restart()
	{
	iAbort.Restart(iReason);
	}

void CDirectScreenAccess::UpdateSizeAndRotation(CFbsBitGc* aGc)
	{
	TPixelsAndRotation sizeAndRotation;
	iWsScreenDevice->GetDefaultScreenSizeAndRotation(sizeAndRotation);
	iScreenSize = sizeAndRotation.iPixelSize;
	TSize scale = iWsScreenDevice->GetCurrentScreenModeScale();
	iScreenDevice->SetScalingFactor(iWsScreenDevice->GetDefaultScreenModeOrigin(),scale.iWidth,scale.iHeight,1,1);
	if (aGc)
		aGc->SetOrientation(sizeAndRotation.iRotation);
	}

void CDirectScreenAccess::RunL()
	{
	iAborting = ETrue;
	iReason = REINTERPRET_CAST(RDirectScreenAccess::TTerminationReasons&,iStatus);
	iAbort.AbortNow(iReason);
	iAborting = EFalse;
	iDirectAccess.Completed();
	iRestart->Start(TCallBack(CDirectScreenAccess::Restart,this));
	}

void CDirectScreenAccess::DoCancel()
	{
	iDirectAccess.Cancel();
	}
