// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include <cs_port.h>
#include "btcomm.h"
#include "btstate.h"
#include "btcommactive.h"
#include <btmanclient.h>
#include "BTSimTimer.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_COMM);
#endif

CBTPortProxy* CBTPortProxy::NewL(TUint32 aPortNo,CBTPort *aParent,CBTPortStateFactory *aFactory)
/**
	CBTPortProxy NewL(..) creates an instance of the BT Port Proxy.
	This is the object that represents the CSY state pattern context.

	In order to avoid deadlock between the esock and c32 threads it is
	necessary to employ a locking mechanism to serialise synchronous
	accesses to esock thread.  This mechanism works as follows:

		1)	Defer all synchronous access attempts on esock via the CSY until 
			a locker active object has been run.
		2)	When we hit the RunL of this locker active object we know that
			the present session on the esock server is exclusive - ie all
			other esock sessions are temporarily disabled.
		3)	At this point, it is possible to run the original synchronous 
			request on esock safe in the knowledge that no other esock session
			can deadlock the c32 thread.  This is the DoLockedActions phase.
		4)	Once the request has completed the unlocker active object is invoked
			to release the exlusive session lock on esock.
*/
	{
	LOG_STATIC_FUNC
	CBTPortProxy *pp;
	pp=new (ELeave) CBTPortProxy(aPortNo);
	CleanupStack::PushL(pp);
	pp->InitL(aParent,aFactory);
	CleanupStack::Pop();
	return pp;
	}

void CBTPortProxy::InitL(CBTPort *aParent,CBTPortStateFactory *aFactory)
/**
	Initialises the CBTPortProxy context and enters the 'starting' state.
	This is where we create the PortLocker, Port Reader, Port Writer, 
	circular read buffer and fixed write buffer. Note that we also fire up 
	the state machine here too (with an EStarting::Entry).  
	This is thus the main CSY Bootstrap function.
**/
	{
	LOG_FUNC
	iPort=aParent;
	iPortStateFactory=aFactory;
	iState=&iPortStateFactory->GetState(CBTPortStateFactory::EIdle);
	iPortLocker=CBTPortLocker::NewL(this);
	iCircularReadBuf=CBTPortBuffer::NewL(this,KBTCOMMCircularBufferLength);
	iReadBuf=HBufC8::NewMaxL(KBTCOMMRecvBufferLength);

	iSendBuf=HBufC8::NewMaxL(KBTCOMMSendBufferLength);
	iSendBufPtr.Set(iSendBuf->Des());

	iReadOutBuf=HBufC8::NewMaxL(KBTCOMMCircularBufferLength);
	iReadBufPtr.Set(iReadOutBuf->Des());

	iPortReader=CBTPortReader::NewL(this);
	iPortWriter=CBTPortWriter::NewL(this);

	iSockServConnector = CSockServConnector::NewL(iSockServ);
	
	User::LeaveIfError(iRegServ.Connect());
	User::LeaveIfError(iPortSettings.Open(iRegServ));

	CActiveScheduler::Add(this);

	iShutdownTimer = CBTTimerSimple::NewL(EPriorityLow,this);

	iState->Open(this);
	}


CBTPortProxy::CBTPortProxy(TUint32 aPortNo) 
	: CActive (EPriorityStandard), iPortNo(aPortNo),
	  iSendBufPtr(NULL,0,0),
	  iReadBufPtr(NULL,0,0)
/**
	CBTPortProxy constructor.
**/
	{
	LOG_FUNC
	}

/**
	Initiates the destruction of the port its proxy and the corresponding states.
	Calls to this method should be made by the state responsible for destructing 
	the CSY.
	No further methods should be invoked after this call.
*/
void CBTPortProxy::DestructContext()
	{
	LOG_FUNC
	if(iPort->AccessCount())
		{
		// someone else managed to reopen us lets not die :-)
		// and check to see if we have a cached read or write request
		if(iClientWritePtr||iClientReadPtr)
			{
			LOG(_L("**CBTPortProxy::DestructContext - Cached Read/Write check **"));
			//then kickstart the starting state (we can only be in the starting state)
			((TBTPortStateIdle*) iState)->SockServConnect(this);
			}
		return;
		}
	//else go down
	iPort->DestructNow(); //taking CBTPortProxy with it
	}

CBTPortProxy::~CBTPortProxy()
/**
	CBTPortProxy destructor.
**/
	{
	LOG_FUNC
	Cancel();
	delete iPortReader;
	delete iPortWriter;
	delete iPortLocker;
	delete iSockServConnector;
	delete iSendBuf;
	delete iReadBuf;
	delete iCircularReadBuf;
	delete iReadOutBuf;
	delete iShutdownTimer;
	iPortSettings.Close();
	iRegServ.Close();
	}

void CBTPortProxy::RunL()
	{
	LOG_FUNC
	LOG1(_L("CBTPortProxy::RunL with result %d"), iStatus.Int());
	iState->DoRunL(this);
	}

void CBTPortProxy::DoCancel()
/**
	CBTPortProxy::DoCancel
	With a normal active object this function would cancel 
	any outstanding requests on aysnchronous services.  In
	this case we cannot do that as to make a synchronous
	request on esock we need to be in the locker.  Instead
	we ensure that when we do a Cancel() we have passed
	through the closing state, which cancels things in the
	locker.  
	
	If there are any requests outstanding when Cancel() is 
	called then this thread will block doing
	User::WaitForRequest() to eat the signal.
**/
	{
	LOG_FUNC
	}

void CBTPortProxy::StartWriter()
/**
	CBTPortProxy StartWrite.
	This function is invoked by the state machine
	on the context when it wishes to start a write.
	Note that this function has to fragment the writes 
	into sensible chunks to write across RFComm.  The 
	size of these chunks is dictated by 
	KBTCOMMSendBufferLength.
**/
	{
	LOG_FUNC
	iPortWriter->QueueWrite(iSendBufPtr);
	}

void CBTPortProxy::DoWriteCompleted(TInt aError)
	{
	LOG_FUNC
	iState->DoWriteCompleted(this,aError);
	}

void CBTPortProxy::StopWriter()
/**
	CBTPortProxy StopWrite.
	This function is invoked by the state machine
	on the context when it wishes to stop a write
**/
	{
	LOG_FUNC
	}

void CBTPortProxy::StartReader()
/**
	CBTPortProxy StartReader.
	This function is invoked by the state machine
	on the context when it wishes to queue a read.
**/
	{
	LOG_FUNC
	iPortReader->StartReading();
	}

void CBTPortProxy::DoReadCompleted(TInt aError)
/**
	Completes the reads to the state indicating that new data arrived from socket.

	According to the delegated state it will check to see if it has reached 
	its circular buffer watermark and hence refrain from re-queueing another 
	read to the socket.
*/
	{
	LOG_FUNC		
	iState->DoReadCompleted(this,aError); // new data arrived from the socket
	}

/**
	Checks whether the low watermark for the circular read-in (from the socket) buffer has been passed.
	This method wil be used by the friendly open state to restart the reader if
	necessary.
*/
TBool CBTPortProxy::ReadInBufferLowWatermarkReached()
	{
	LOG_FUNC
	if (iCircularReadBuf->Count()<=KBTCOMMCircularBufferLowWatermark)
		{
		return ETrue;
		}
	return EFalse;
	}

/**
	Checks whether the high watermark for the circular read-in (from the socket) buffer has been reached or passed.
	This method wil be used by the friendly open state to start or stop the 
	reader if necessary.
*/
TBool CBTPortProxy::ReadInBufferHighWatermarkReached()
	{
	LOG_FUNC
	if (iCircularReadBuf->Count()>=KBTCOMMCircularBufferHighWatermark)
		{
		return ETrue;
		}
	return EFalse;
	}

void CBTPortProxy::StopReader()
	{
	LOG_FUNC
	iPortReader->StopReading();
	}

void CBTPortProxy::StartLocker()
	{
	LOG_FUNC
	iPortLocker->LockSession();
	}

void CBTPortProxy::DoLockedAction()
	{
	LOG_FUNC
	iState->DoLockedAction(this);
	}

void CBTPortProxy::StopLocker()
	{
	LOG_FUNC
	iPortLocker->UnlockSession();
	}

TBool CBTPortProxy::IsLockerOn()
	{
	LOG_FUNC
	return (iPortLocker->IsSessionLocked());
	}

void CBTPortProxy::StartShutdownTimerL()
	{
	LOG_FUNC
	iShutdownTimer->CancelAlarm();
	iShutdownTimer->After(KBtcommShutdownTimer);
	}

void CBTPortProxy::CancelShutDownTimer()
	{
	LOG_FUNC
	iShutdownTimer->CancelAlarm();
	}

void CBTPortProxy::ShutdownAlarm()
/**
	ShutdownAlarm() is the callback of the shutdown timer.
	To be called for the final stage of closing down connection to ESock.
*/
	{
	LOG_FUNC
	// this startlocker will basically invoke the locked action of the 
	// closing state
	StartLocker();
	}

// Methods to be called by the Open state only
void CBTPortProxy::SetWriteCancelPending()
	{
	LOG_FUNC
	iWriteCancelationPending=ETrue;
	}

TBool CBTPortProxy::IsWriteCancelPending()
	{
	LOG_FUNC
	return iWriteCancelationPending;
	}

TBool CBTPortProxy::IsNetDbInUse()
	{
	LOG_FUNC
	return iNetDbInUse;
	}

void CBTPortProxy::SetNetDbInUse()
	{
	LOG_FUNC
	iNetDbInUse=ETrue;
	}

void CBTPortProxy::SetNetDbNotInUse()
	{
	LOG_FUNC
	iNetDbInUse=EFalse;
	}

// *********************  PUBLIC INTERFACE  ***************************

void CBTPortProxy::Write(const TAny* aClientBuffer,TInt aLength)
/**
	CBTPortProxy Write.
	The CBTPort has attempted a Write.
**/
	{
	LOG_FUNC
	iState->Write(this,const_cast<TAny*>(aClientBuffer),aLength);
	}

void CBTPortProxy::WriteCancel()
/**
	CBTPortProxy WriteCancel.
	The CBTPort has attempted to cancel a write.
	Delegate behaviour to the active state.
**/
	{
	LOG_FUNC
	iState->WriteCancel(this);  
	}

void CBTPortProxy::DoWriteCancel()
/** 
	Only to be called by the active state, NOT directly.
	This will cancel the ESock write.
*/
	{
	LOG_FUNC
	iWriteCancelationPending=EFalse;
	iSocket.CancelWrite();
	}

void CBTPortProxy::Read(const TAny* aClientBuffer,TInt aLength)
/**
	Delegates the client Read request to the active state.
	If the state is the Open state then the request is serviced 
	otherwise it gets cached.

	@param		aLength is either the bytecount to be read or -1
				indicating a 'Read One Or More bytes' request
**/
	{
	LOG_FUNC
	iClientReadOneOrMore=EFalse;
	if (aLength<0)
        {
        aLength=-aLength;
        iClientReadOneOrMore=ETrue;
        }
	iState->Read(this,const_cast<TAny*>(aClientBuffer),aLength);	
	}

void CBTPortProxy::ReadCancel()
/**
	Delegates 'cancel the read' behaviour to the active state.
**/
	{
	LOG_FUNC
	iState->ReadCancel(this);  
	}

TInt CBTPortProxy::QueryReceiveBuffer(TInt& aLength)
/**
	Retrieves number of bytes waiting in the receive buffer.
*/	{
	LOG_FUNC
	aLength=iCircularReadBuf->Length();
	return KErrNone;
	}

TInt CBTPortProxy::GetReceiveBufferLength(TInt& aLength)
/**
	Sets aLength to KBTCOMMRecvBufferLength.
	Always returns KErrNone;
*/	{
	LOG_FUNC
	aLength=KBTCOMMRecvBufferLength;
	return KErrNone;
	}

void CBTPortProxy::Close()
/** 
	Delegates Close request to the active state.
**/
	{
	LOG_FUNC
	iState->Close(this);
	}	

void CBTPortProxy::MoveToErrorState()
	{
	LOG_FUNC
	iState=&iPortStateFactory->GetState(CBTPortStateFactory::EError);	
	// Notify the error state as we move in to it about the current error.
	iState->Error(this,iLastError);
	}

void CBTPortProxy::ResetRxBuffer()
	{
	LOG_FUNC
	iCircularReadBuf->Reset();
	}
