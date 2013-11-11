/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
//
// PropPipe.cpp
//
// (c) Symbian Software, Ltd 2007
//

#include "PropPipe.h"
#include "../../lbsnetprotocolproxy/inc/lbsnetprotocolproxydefs.h"
#include <lbs/test/lbstestlogger.h>
#include <lbs/test/lbsparamlogger.h>
//Size of the buffer to store incoming messages.  This currently is sufficient for up to
// 6 simultaneous sequences and may need to be increased in the future.
const TInt KDefPropBufSize = 6000;

_LIT_SECURITY_POLICY_PASS(KSecurityPolicy); // Test code, so msecurity is a non-issue

CPropPipeBase::CPropPipeBaseTimeOut* CPropPipeBase::CPropPipeBaseTimeOut::NewL(CPropPipeBase& aParent)
	{
	CPropPipeBaseTimeOut* self = new(ELeave) CPropPipeBaseTimeOut(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPropPipeBase::CPropPipeBaseTimeOut::CPropPipeBaseTimeOut(CPropPipeBase& aParent)
	: CTimer(EPriorityHigh), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}

void CPropPipeBase::CPropPipeBaseTimeOut::RunL()
	{
	iParent.TimedOutL();
	}


CPropPipeBase::CPropPipeBaseWatcher::CPropPipeBaseWatcher(CPropPipeBase& aParent, RProperty& aPropFlow)
	: CActive(EPriorityHigh), iParent(aParent), iPropFlow(aPropFlow)
	{
	CActiveScheduler::Add(this);
	}

void CPropPipeBase::CPropPipeBaseWatcher::Watch()
	{
	iStatus = KRequestPending;
	SetActive();	
	iPropFlow.Subscribe(iStatus);
	}

void CPropPipeBase::CPropPipeBaseWatcher::CompleteRequest()
	{
	iStatus = KRequestPending;
	SetActive();	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CPropPipeBase::CPropPipeBaseWatcher::RunL()
	{
	iParent.StateChangedL();
	}

void CPropPipeBase::CPropPipeBaseWatcher::DoCancel()
	{
	iPropFlow.Cancel();
	}

//-------------------------------------------------------------------------------------------------

EXPORT_C CPropPipeBase::~CPropPipeBase()
	{
	if(iTimeOut)
		{
		iTimeOut->Cancel();
		delete iTimeOut;
		}
	if(iWatcher)
		{
		iPropFlow.Cancel();
		iWatcher->Cancel();
		delete iWatcher;
		}
	iPropPipe.Close();
	iPropFlow.Close();
	
	if(iPropPipeOwned)
		RProperty::Delete(TUid::Uid(KUidSystemCategoryValue), iPipeUid);
	
	if(iPropFlowOwned)
		RProperty::Delete(TUid::Uid(KUidSystemCategoryValue), iFlowUid);
	}

void CPropPipeBase::BaseConstructL(TUint32 aKey, TUint32 aKeyFlowCtrl)
	{
	iPipeUid = aKey;
	iFlowUid = aKeyFlowCtrl; 
	
	User::LeaveIfError(CreateAndAttachKey(iPipeUid, RProperty::ELargeByteArray, iPropPipe, iPropPipeOwned));
	User::LeaveIfError(CreateAndAttachKey(iFlowUid, RProperty::EInt, iPropFlow, iPropFlowOwned));
	
	if(iPropPipeOwned) // If we own that, we /probably/ own the flow as well...
		{
		__ASSERT_DEBUG(iPropFlowOwned, User::Invariant());
		
		iPropPipe.Set(KNullDesC8);
		iPropFlow.Set(EReadyToWrite);
		   LBSTESTLOG_METHOD(_L("--"), "jcmi PropFlow.Set(EReadyToWrite)"); 

		}
	}


TBool CPropPipeBase::WaitForStateL(TFlowControl aDesiredState, TInt32 aTimeout)
	{
    LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::WaitForStateL a"); 

	TInt flowState = (aDesiredState == EReadyToRead)?EReadyToWrite:EReadyToRead;
	
	User::LeaveIfError(iPropFlow.Get(flowState));
	
	LBSTESTLOG_METHOD2(_L("--"), "flowstate is and desired state is ",flowState,aDesiredState); 

	if(aDesiredState != flowState)
		{
		RTimer timeout;
		timeout.CreateLocal();
		CleanupClosePushL(timeout);
		
		TRequestStatus statTime = KRequestPending, 
					   statProp = KRequestPending;
					   	
		timeout.After(statTime, aTimeout);
		iPropFlow.Subscribe(statProp);
		   LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::WaitForStateL b"); 

		User::LeaveIfError(iPropFlow.Get(flowState));
	     LBSTESTLOG_METHOD1(_L("--"), "iPropFlow.Get(flowState) c",flowState); 
		if(aDesiredState == flowState)
			{ // if the flow property was changed while subscribing then cancel the subscribtion and return
		     LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::WaitForStateL x"); 
		    
			iPropFlow.Cancel();
			User::WaitForRequest(statProp);
			timeout.Cancel();
			User::WaitForRequest(statTime);
			}
		else
			{
		     LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::WaitForStateL y"); 
			User::WaitForRequest(statProp, statTime);
			
			if(KRequestPending == statProp.Int())
				{
			      LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::WaitForStateL y1"); 
			    
				iPropFlow.Cancel();
				}
			else
				{
	               LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::WaitForStateL y2"); 

				timeout.Cancel();
			       LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::WaitForStateL d"); 
				User::LeaveIfError(iPropFlow.Get(flowState));
			       LBSTESTLOG_METHOD1(_L("--"), "xx iPropFlow.Get(flowState) e",flowState); 
				
				__ASSERT_DEBUG(aDesiredState == flowState, User::Invariant());
				}
				
			User::WaitForRequest(statProp, statTime); // Coz the one that didn't signal earlier has
													  // just been completed with KErrCancel, and
													  // we want to avoid the otherwise inevitable
													  // stray signal in any (nested) schedulers.
			}
		
		CleanupStack::PopAndDestroy(&timeout);
		}
    LBSTESTLOG_METHOD2(_L("--"), "return",aDesiredState,flowState); 
	
	return (aDesiredState == flowState);
	}

void CPropPipeBase::WriteL(const TDesC8& aBuf, TInt32 aTimeout)
	{
    LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::WriteL a"); 

	if(!WaitForStateL(EReadyToWrite, aTimeout))
	    
	    {
        LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::WriteL KErrNotRdy"); 

		User::Leave(KErrNotReady);
	    }
    LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::WriteL b"); 
	User::LeaveIfError(iPropPipe.Set(aBuf));
    LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::WriteL c"); 
	User::LeaveIfError(iPropFlow.Set(EReadyToRead));
    LBSTESTLOG_METHOD(_L("--"), "de WriteL iPropFlow.Set(EReadyToRead)"); 

    LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::WriteL d"); 
	}
	
void CPropPipeBase::ReadL(TDes8& aBuf, TInt32 aTimeout)
	{
	__ASSERT_DEBUG(aBuf.MaxLength() >= KDefPropBufSize, User::Invariant());
	__ASSERT_DEBUG(!aBuf.Length(), User::Invariant());
    LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::ReadL a");
	if(!WaitForStateL(EReadyToRead, aTimeout))
	    {
        LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::ReadLKErrNotRdy");
		User::Leave(KErrNotReady);
	    }
    LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::ReadL b");
	User::LeaveIfError(iPropPipe.Get(aBuf));
    LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::ReadL c");
	User::LeaveIfError(iPropFlow.Set(EReadyToWrite));
	   LBSTESTLOG_METHOD(_L("--"), "pp ReadL iPropFlow.Set(EReadyToWrite)"); 

    LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::ReadL d");
	}

void CPropPipeBase::ReadL(HBufC8* aBuf, TInt32 aTimeout, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(aBuf->Des().MaxLength() >= KDefPropBufSize, User::Invariant());
	__ASSERT_DEBUG(!aBuf->Length(), User::Invariant());
	
	// not more than one request at a time
	__ASSERT_DEBUG(!iTimeOut, User::Invariant());
	__ASSERT_DEBUG(!iWatcher, User::Invariant());

	  LBSTESTLOG_METHOD(_L("--"), "CCPropPipeBase::ReadL 2"); 

	aStatus = KRequestPending;
	iReadBuf = aBuf;

	TInt flowState = EReadyToWrite;
	
	User::LeaveIfError(iPropFlow.Get(flowState));
    LBSTESTLOG_METHOD1(_L("--"), "qw ReadL iPropFlow.Get(flowState)",flowState); 

	
	iTimeOut = CPropPipeBaseTimeOut::NewL(*this);
	iWatcher = new(ELeave) CPropPipeBaseWatcher(*this, iPropFlow);
	
	if(EReadyToRead != flowState)
		{
	    LBSTESTLOG_METHOD2(_L("--"), "(EReadyToRead != flowState)",EReadyToRead,flowState); 

		iTimeOut->After(aTimeout);
		iWatcher->Watch();
		}
	else
		{
	    LBSTESTLOG_METHOD2(_L("--"), "(EReadyToRead == flowState)",EReadyToRead,flowState); 

		iTimeOut->After(aTimeout);
		iWatcher->CompleteRequest();
		}
	}

void CPropPipeBase::CancelWait()
	{
	if(iTimeOut)
		{
		iTimeOut->Cancel();
		delete iTimeOut;
		iTimeOut = NULL;
		}
	if(iWatcher)
		{
		iWatcher->Cancel();
		delete iWatcher;
		iWatcher = NULL;
		}
	}

void CPropPipeBase::SendResponse()
	{
	// does nothing, this is for the read prop pipe to inherit from
	}

void CPropPipeBase::SendTimeOut()
	{
	// does nothing, this is for the read prop pipe to inherit from
	}

void CPropPipeBase::StateChangedL()
	{
    LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::StateChangedL a"); 
	iTimeOut->Cancel();
	TPtr8 p = iReadBuf->Des();
	User::LeaveIfError(iPropPipe.Get(p));
    LBSTESTLOG_METHOD(_L("--"), "CPropPipeBase::StateChangedL b"); 
	iReadBuf = NULL;
	User::LeaveIfError(iPropFlow.Set(EReadyToWrite));
    LBSTESTLOG_METHOD(_L("--"), "wer CPropPipeBase::StateChangedL c"); 
	SendResponse();
	delete iWatcher;
	iWatcher = NULL;
	delete iTimeOut;
	iTimeOut = NULL;
	}

void CPropPipeBase::TimedOutL()
	{
	iWatcher->Cancel();
	iReadBuf = NULL;
	SendTimeOut();
	delete iWatcher;
	iWatcher = NULL;
	delete iTimeOut;
	iTimeOut = NULL;
	}

TInt CPropPipeBase::CreateAndAttachKey(TUint32 aKey, TInt aAttr, RProperty& aProp, TBool& aOwnedFlag)
	{
  	TInt e = KErrNone;
  
   	aOwnedFlag = ETrue;
    e = RProperty::Define(TUid::Uid(KUidSystemCategoryValue), aKey, aAttr, KSecurityPolicy, KSecurityPolicy);//, EFalseTrue);
	switch(e)
		{
		case KErrAlreadyExists: aOwnedFlag = EFalse; 							  	  		// Fall through...
		case KErrNone:			e = aProp.Attach(TUid::Uid(KUidSystemCategoryValue), aKey);	// And again...
		default:				break;
		}
    return e;
	}
	
CPropPipeBase:: CPropPipeBase()
 	{
 	;
 	}

//-------------------------------------------------------------------------------------------------


EXPORT_C CWritePropPipe* CWritePropPipe::NewL(TUint32 aKey, TUint32 aKeyFlowCtrl)
	{
    CWritePropPipe* p = new(ELeave) CWritePropPipe();
    CleanupStack::PushL(p);
    p->ConstructL(aKey, aKeyFlowCtrl, KDefPropBufSize);
    CleanupStack::Pop(p);
    return p;
	}


void CWritePropPipe::ConstructL(TUint32 aKey, TUint32 aKeyFlowCtrl, TInt aBufSize)
	{
	BaseConstructL(aKey, aKeyFlowCtrl);
	
	iReadyToWrite = EFalse;
	iNumberOfOutstandingMsgs = 0;
	iPropWatch = CPropWatcher::NewL(KUidSystemCategoryValue, aKeyFlowCtrl, *this);
	
	iBufSize = aBufSize;
	iBuf = HBufC8::NewMaxL(aBufSize);
	iBuf->Des().Zero();
	}

	
EXPORT_C CWritePropPipe::~CWritePropPipe()
	{
	delete iPropWatch;
	delete iBuf;
	}
	
EXPORT_C CWritePropPipe& CWritePropPipe::operator<<(const TDesC8& aSrc)
//
// Write the string to the buffer. If there is insufficient space, then leave with 
// KErrOverflow
//
	{
	__ASSERT_DEBUG(iBuf, User::Invariant());
	
	TPtr8 p = iBuf->Des();
	if(p.MaxSize() < (p.Size() + aSrc.Size() + sizeof(TInt32)))
		{
		User::Leave(KErrOverflow);
		}
	
	TPckgBuf<TInt32> size(aSrc.Size());
	p.Append(size);	
	p.Append(aSrc);
	return *this;	
	}

EXPORT_C CWritePropPipe& CWritePropPipe::operator<<(const TDesC16& aSrc)
//
// Write the string to the buffer. If there is insufficient space, then leave with 
// KErrOverflow
//
	{
	__ASSERT_DEBUG(iBuf, User::Invariant());
	
	TPtr8 p = iBuf->Des();
	if(p.MaxSize() < (p.Size() + aSrc.Size() + sizeof(TInt32)))
		{
		User::Leave(KErrOverflow);
		}
	
	TPckgBuf<TInt32> size(aSrc.Size());
	p.Append(size);	
	
	TUint8* pTgt = new(ELeave)TUint8[aSrc.Size()+8];
	const TAny* pSrc = &aSrc[0];
		
	Mem::Copy(pTgt, pSrc, aSrc.Size());
	
	TPtr8 p2(pTgt, aSrc.Size(), aSrc.Size());
	p.Append(p2);
	delete pTgt;
	
	return *this;	
	}

EXPORT_C CWritePropPipe& CWritePropPipe::operator<<(const TInt32& aSrc)
	{
	__ASSERT_DEBUG(iBuf, User::Invariant());
	
	TPtr8 p = iBuf->Des();
	if(p.MaxSize() < (p.Size() + sizeof(TInt32)))
		{
		User::Leave(KErrOverflow);
		}
	
	TPckgBuf<TInt32> i(aSrc);
	p.Append(i);	
	
	return *this;	
	}
		
EXPORT_C void CWritePropPipe::CommitL(TInt32 aTimeout)
//
// Informs the Pipe that there is a new message in the pipe that needs
//  to be published to the property.  If the Property is ready to be
//  written to, the message is committed straightaway. 
//
	{
	//Increment the counter, there is data that needs to be committed
	iNumberOfOutstandingMsgs++;
	
	if(iReadyToWrite)
		{
		//Write the data straightway, the property is ready
		CommitDataL(aTimeout);
		}
	}

EXPORT_C void CWritePropPipe::CommitAndWaitL(TInt32 aTimeout)
	{
	__ASSERT_DEBUG(iBufSize, User::Invariant());
	__ASSERT_DEBUG(iBuf->Size(), User::Invariant());
	
	WriteL(*iBuf, aTimeout);
	}

EXPORT_C void CWritePropPipe::ResetL()
//
// Resets the buffer without committing it to the property
//
	{
	__ASSERT_DEBUG(iBufSize, User::Invariant());
	__ASSERT_DEBUG(iBuf, User::Invariant());
	
	iBuf->Des().Zero();
	
	}

void CWritePropPipe::CommitDataL(TInt32 aTimeout)
//
// Commits the contents of the buffer to the property -- waits for the
// flow control property to be "clear" for the object timeout value
// first. If the timeout fires, then this leaves with KErrNotReady.
// 
// Note: This is a blocking call
// 
	{
	__ASSERT_DEBUG(iBufSize, User::Invariant());
	__ASSERT_DEBUG(iBuf->Size(), User::Invariant());
	
	//Need to extract the first block of data in the pipe
	TInt position = iBuf->Find(KEoSMarker);
	if(position != KErrNotFound)
		{
		TPtrC8 block = iBuf->Left(position);
		
		iReadyToWrite = EFalse;
		WriteL(block, aTimeout);
		
		//Remove the committed data (and the EoS marker)
		iBuf->Des().Delete(0, (position + 2));
		iNumberOfOutstandingMsgs--;
		}
	}

void CWritePropPipe::OnChange(RProperty& aProperty)
	{
	TInt val = 0;
	TInt err = aProperty.Get(val);
	iReadyToWrite = EFalse;

	__ASSERT_DEBUG(KErrNone == err, User::Invariant());

	//Check to see whether the Property is ready to write to
	if(EReadyToWrite == val)
		{
		iReadyToWrite = ETrue;
		
		if(iNumberOfOutstandingMsgs > 0)
			{
			//Call CommitL to write the data to the Property
			TRAP(err, CommitDataL(KResponseChannelWriteTimeout));
			}
		}
	}

//-------------------------------------------------------------------------------------------------
EXPORT_C CReadPropPipe* CReadPropPipe::NewL(TUint32 aKey, TUint32 aKeyFlowCtrl)
	{
    CReadPropPipe* p = new(ELeave) CReadPropPipe();
    CleanupStack::PushL(p);
    p->ConstructL(aKey, aKeyFlowCtrl, KDefPropBufSize);
    CleanupStack::Pop(p);
    return p;
 	}
	
void CReadPropPipe::ConstructL(TUint32 aKey, TUint32 aKeyFlowCtrl, TInt32 aBufSize)
	{
	BaseConstructL(aKey, aKeyFlowCtrl);
	iBuf = HBufC8::NewMaxL(aBufSize);
	iMemoryManager = CMemoryManager::NewL();
	}
	
EXPORT_C CReadPropPipe::~CReadPropPipe()
	{
	delete iBuf;
	delete iMemoryManager;
	}

EXPORT_C CReadPropPipe& CReadPropPipe::operator>>(TDes8& aTgt)
	{
	__ASSERT_DEBUG(iBuf, User::Invariant());
	__ASSERT_DEBUG((iMark + sizeof(TInt32)) <= iBuf->Size(), User::Invariant());
		
	TPckgBuf<TInt32> size(0);
	size.Copy(&(*iBuf)[iMark], sizeof(TInt32));
	iMark += sizeof(TInt32);
	__ASSERT_DEBUG((iMark + size()) <= iBuf->Size(), User::Invariant());
	__ASSERT_DEBUG(aTgt.MaxLength() >= size(), User::Invariant());
	
	aTgt.Copy(&(*iBuf)[iMark], size());
	iMark += size();
	
	return *this;	
	}
	
EXPORT_C CReadPropPipe& CReadPropPipe::operator>>(TDes16& aTgt)
	{
	__ASSERT_DEBUG(iBuf, User::Invariant());
	__ASSERT_DEBUG((iMark + sizeof(TInt32)) <= iBuf->Size(), User::Invariant());
		
	TPckgBuf<TInt32> size(0);
	size.Copy(&(*iBuf)[iMark], sizeof(TInt32));
	
	iMark += sizeof(TInt32);
	
	__ASSERT_DEBUG((iMark + size()) <= iBuf->Size(), User::Invariant());
	__ASSERT_DEBUG(aTgt.MaxSize() >= size(), User::Invariant());
	
	TUint16* pTgt = new(ELeave)TUint16[size()];
	Mem::FillZ(pTgt, (size())* sizeof(TUint16));

	Mem::Copy(pTgt, &(*iBuf)[iMark], size());
	aTgt.Copy(pTgt, size()/2);
	delete pTgt;
	
	iMark += size();
	
	return *this;	
	}
	
EXPORT_C CReadPropPipe& CReadPropPipe::operator>>(TInt32& aTgt)
	{	
	__ASSERT_DEBUG(iBuf, User::Invariant());
	__ASSERT_DEBUG((iMark + sizeof(TInt32)) <= iBuf->Size(), User::Invariant());
	
	TPckgBuf<TInt32> i(0);
	i.Copy(&(*iBuf)[iMark], sizeof(TInt32));
	
	iMark += sizeof(TInt32);
	aTgt = i();
	
	return *this;	
	}
	
EXPORT_C void CReadPropPipe::RefreshL(TInt32 aTimeout)
	{
	__ASSERT_DEBUG(iBuf, User::Invariant());
	
	iBuf->Des().Zero();
	iMark = 0;
	
	TPtr8 p = iBuf->Des();
	ReadL(p, aTimeout);
	}
	
EXPORT_C void CReadPropPipe::RefreshL(TInt32 aTimeout, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iBuf, User::Invariant());
	
	iBuf->Des().Zero();
	iMark = 0;
	
	iClientStatus = &aStatus;
	ReadL(iBuf, aTimeout, aStatus);
	}

EXPORT_C void CReadPropPipe::CancelRefresh()
	{
	CancelWait();
	User::RequestComplete(iClientStatus, KErrCancel);	
	}

void CReadPropPipe::SendResponse()
	{
	TInt32 response;
	*this >> response;
	User::RequestComplete(iClientStatus, response);
	}

void CReadPropPipe::SendTimeOut()
	{
	User::RequestComplete(iClientStatus, ENetMsgTimeoutExpired);
	}

//-------------------------------------------------------------------------------------------------

EXPORT_C CNotifyReadPropPipe* CNotifyReadPropPipe::NewL(TUint32 aKey, TUint32 aKeyFlowCtrl, MReadPropPipeObserver& aObs)
	{
    CNotifyReadPropPipe* p = new(ELeave) CNotifyReadPropPipe(aObs);
    CleanupStack::PushL(p);
    p->ConstructL(aKey, aKeyFlowCtrl, KDefPropBufSize);
    CleanupStack::Pop(p);
    return p;
 	}

CNotifyReadPropPipe::CNotifyReadPropPipe(MReadPropPipeObserver& aObserver)
:iObserver(aObserver)
	{
	;	
	}

void CNotifyReadPropPipe::ConstructL(TUint32 aKey, TUint32 aKeyFlowCtrl, TInt32 aBufSize)
	{
	CReadPropPipe::ConstructL(aKey, aKeyFlowCtrl, aBufSize);
	iPropWatch = CPropWatcher::NewL(KUidSystemCategoryValue, aKeyFlowCtrl, *this);
	}
	
EXPORT_C CNotifyReadPropPipe::~CNotifyReadPropPipe()
	{
	delete iPropWatch;
	}

void CNotifyReadPropPipe::OnChange(RProperty& aProperty)
	{
	TInt val = 0;
	     
	TInt err = aProperty.Get(val);
	
	__ASSERT_DEBUG(KErrNone == err, User::Invariant());
	
	
	if(EReadyToRead == val)
		{
		iObserver.OnReadyToReadL(*this);	
		}
	}
	
	
	
	
