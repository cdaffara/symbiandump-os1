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

#include "../DSTD/DSTD.H"
#include "../DSTD/ACQUIRE.H"
#include "../DSTD/DACQDEF.H"
#include <e32def.h>
#include <f32file.h>

void TsyPanic(TTsyPanic aPanic)
//
// Panic in tsy
//
	{
	_LIT(KTsyPanic,"Tsy Panic");
	User::Panic(KTsyPanic,aPanic);
	}

TTsyTimer::TTsyTimer()
	{
	iPending=EFalse;
	}
//
// CAcquireEntry class
//
CAcquireEntry* CAcquireEntry::NewL(const TTsyReqHandle aTsyReqHandle)
//
//	Create new request entry
//
	{
	return new(ELeave) CAcquireEntry(aTsyReqHandle);
	}

CAcquireEntry::CAcquireEntry(const TTsyReqHandle aTsyReqHandle)
//
//	 constructor
//
	{
	iTsyReqHandle=aTsyReqHandle;
	}

CAcquireEntry::~CAcquireEntry()
//
//	destructor
//
	{}

void CAcquireEntry::Deque()
//
//	Deque  list
//
	{
	iLink.Deque();
	iLink.iPrev=iLink.iNext=NULL;
	}

CAcquireOwnerList::CAcquireOwnerList()
	{}

CAcquireOwnerList::~CAcquireOwnerList()
	{}

CAcquireOwnerList* CAcquireOwnerList::NewL()
//
//	Static function to create new call
//
	{
	CAcquireOwnerList* ac=new(ELeave) CAcquireOwnerList();
	CleanupStack::PushL(ac);
	ac->ConstructL();
	CleanupStack::Pop();
	return ac;
	}

void CAcquireOwnerList::ConstructL()
	{
	iAcquireList.SetOffset(_FOFF(CAcquireEntry,iLink));
	}

CAcquireEntry* CAcquireOwnerList::FindByTsyReqHandle(const TTsyReqHandle aTsyReqHandle)
	{
	CAcquireEntry* entry;
	TDblQueIter<CAcquireEntry> iter(iAcquireList);
	while(entry = iter++, entry!=NULL)
		{
		if(entry->iTsyReqHandle==aTsyReqHandle)
			return entry;
		}
	return NULL;
	}

void CAcquireOwnerList::Remove(CAcquireEntry* aEntry)
	{
	aEntry->Deque();
	delete aEntry;
	}

//
// CFaxDummyBase
//
CFaxDummyBase::CFaxDummyBase(CPhoneFactoryDummyBase* aFac)
	{
	iFac=aFac;
	}

CFaxDummyBase::~CFaxDummyBase()
	{
	iFac->RemoveTimer(iRead);
	iFac->RemoveTimer(iWrite);
	iFac->RemoveTimer(iWaitForEndOfPage);
	iFac->RemoveTimer(iProgressNotification);
	}

TInt CFaxDummyBase::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

TInt CFaxDummyBase::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

CTelObject::TReqMode CFaxDummyBase::ReqModeL(const TInt aIpc)
//
//	mode request for fax
//
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
	case EEtelFaxTerminateFaxSession:
	case EEtelFaxWaitForEndOfPage:
		break;
	case EEtelFaxRead:
	case EEtelFaxWrite:
		ret=KReqModeFlowControlObeyed;
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return ret;
	}

CFaxDummyBase* CFaxDummyBase::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CFaxDummyBase*,aPtr);
	}

TInt CFaxDummyBase::WaitForEndOfPageHandler(TAny* aPtr)
//
//	Completed	req
//
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iWaitForEndOfPage);
	This(aPtr)->ReqCompleted(This(aPtr)->iWaitForEndOfPage.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::WaitForEndOfPage(const TTsyReqHandle aTsyReqHandle)
	{
	iFac->QueueTimer(iWaitForEndOfPage,aTsyReqHandle,DACQ_ASYN_TIMEOUT,CFaxDummyBase::WaitForEndOfPageHandler,this);
	return KErrNone;
	}

TInt CFaxDummyBase::Read(const TTsyReqHandle aTsyReqHandle,TDes8* aDes)
//
//	Fax	read
//
	{
	iReadParams=aDes;
	iFac->QueueTimer(iRead,aTsyReqHandle,DACQ_ASYN_TIMEOUT,CFaxDummyBase::ReadHandler,this);
	return KErrNone;
	}

TInt CFaxDummyBase::ReadHandler(TAny* aPtr)
//
//	Completed Request
//
	{
	This(aPtr)->iReadParams->Copy(DACQ_FAX_BUF_DATA);
	This(aPtr)->iFac->ResetPending( This(aPtr)->iRead);
	This(aPtr)->ReqCompleted(This(aPtr)->iRead.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::TerminateFaxSession(const TTsyReqHandle aTsyReqHandle)
	{
	if (iFac->RemoveTimer(iRead))
		ReqCompleted(iRead.iTsyReqHandle,KErrCancel);

	if (iFac->RemoveTimer(iWrite))
		ReqCompleted(iWrite.iTsyReqHandle,KErrCancel);

	if (iFac->RemoveTimer(iWaitForEndOfPage))
		ReqCompleted(iWaitForEndOfPage.iTsyReqHandle,KErrCancel);

//	if (iFac->RemoveTimer(iProgressNotification))
//		ReqCompleted(iProgressNotification.iTsyReqHandle,KErrCancel);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::Write(const TTsyReqHandle aTsyReqHandle,TDesC8* aDes)
	{
	if (aDes->Compare(DACQ_FAX_BUF_DATA)!=KErrNone)
		TsyPanic(KTsyPanicDataCorrupted);
	iFac->QueueTimer(iWrite,aTsyReqHandle,DACQ_ASYN_TIMEOUT,CFaxDummyBase::WriteHandler,this);
	return KErrNone;
	}

TInt CFaxDummyBase::WriteHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iWrite);
	This(aPtr)->ReqCompleted(This(aPtr)->iWrite.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::GetProgress(const TTsyReqHandle aTsyReqHandle,RFax::TProgress* aProgress)
	{
	aProgress->iSpeed=DACQ_GET_PROGRESS_SPEED;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::ProgressNotificationHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iProgressNotification);
	This(aPtr)->ReqCompleted(This(aPtr)->iProgressNotification.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::ProgressNotification(const TTsyReqHandle aTsyReqHandle, RFax::TProgress* aProgress)
	{
	iProgressNotificationParams=aProgress;
	iFac->QueueTimer(iProgressNotification,aTsyReqHandle,DACQ_ASYN_TIMEOUT,CFaxDummyBase::ProgressNotificationHandler,this);
	return KErrNone;
	}

TInt CFaxDummyBase::ProgressNotificationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if(aTsyReqHandle==iProgressNotification.iTsyReqHandle)
		{
		iFac->RemoveTimer(iProgressNotification);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

//
// CCallDummyBase
//
CCallDummyBase::CCallDummyBase(CPhoneFactoryDummyBase* aFac)
	{
	iFac=aFac;
	}

void CCallDummyBase::ConstructL()
	{
	iList=CAcquireOwnerList::NewL();
	}

CCallDummyBase::~CCallDummyBase()
	{
	delete iList;
	iFac->RemoveTimer(iNotifyHookChange);
	iFac->RemoveTimer(iNotifyStatusChange);
	iFac->RemoveTimer(iDial);
	iFac->RemoveTimer(iConnect);
	iFac->RemoveTimer(iAnswer);
	iFac->RemoveTimer(iHangUp);
	}

TInt CCallDummyBase::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}
TInt CCallDummyBase::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}
	
CCallDummyBase* CCallDummyBase::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CCallDummyBase*,aPtr);
	}

CPhoneFactoryDummyBase* CCallDummyBase::FacPtr() const
	{
	return iFac;
	}

TInt CCallDummyBase::CancelSubSession()
	{
	return KErrNone;
	}

TInt CCallDummyBase::ProcessingOwnership()
	{
	if (iList)
		{	
		if(iList->iAcquireList.IsEmpty()) // no one interested in this call !
			{
			SetUnowned();
			return KErrNone;
			}

		CAcquireEntry* entry=iList->iAcquireList.First();
		if (entry) // someone interested in this call
			{
			SetOwnership(entry->iTsyReqHandle);
			ReqCompleted(entry->iTsyReqHandle,KErrNone);
			iList->Remove(entry);
			}
		return KErrNone;
		}
	else
		return KErrNotFound;
	}

TInt CCallDummyBase::RelinquishOwnership()
	{
	ProcessingOwnership();
	RelinquishOwnershipCompleted(KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::TransferOwnership(const TTsyReqHandle aTsyReqHandle)
//
//	Transfer Call Ownership
//
	{
	if (CheckOwnership(aTsyReqHandle)!=CCallBase::EOwnedTrue)
		{
		ReqCompleted(aTsyReqHandle,KErrEtelNotCallOwner);
		return KErrNone;
		}

	if(iList->iAcquireList.IsEmpty()) // no one interested in this call !
		{
		ReqCompleted(aTsyReqHandle,KErrEtelNoClientInterestedInThisCall);
		return KErrNone;
		}

	CAcquireEntry* entry=iList->iAcquireList.First();
	if (entry) // someone interested in this call
		{
		SetOwnership(entry->iTsyReqHandle);
		ReqCompleted(entry->iTsyReqHandle,KErrNone);
		iList->Remove(entry);
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CCallDummyBase::AcquireOwnership(const TTsyReqHandle aTsyReqHandle)
//
//	Acquire call Ownership
//
	{
	TInt ret(KErrNone);
	if (CheckOwnership(aTsyReqHandle)==CCallBase::EOwnedUnowned)
		{
		SetOwnership(aTsyReqHandle);
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else
		{
		if(iList->iAcquireList.IsEmpty())
			// List is empty. Client is the first one to request ownership of the call.
			{
			CAcquireEntry* entry=NULL;
			TRAP(ret, entry=CAcquireEntry::NewL(aTsyReqHandle));
			if (ret==KErrNone)
				iList->iAcquireList.AddLast(*entry);
			}
		else
			// List is not empty. Another client has already requested to acquire ownership of the call.
			// Only one client can be waiting to acquire ownership at any one time.
			return KErrInUse;
		}
	return ret;
	}

TInt CCallDummyBase::AcquireOwnershipCancel(const TTsyReqHandle aTsyReqHandle)
//
//	Cancel Acquire Call Ownership
//
	{
	CAcquireEntry* entry=iList->FindByTsyReqHandle(aTsyReqHandle);
	if (entry)
		{
		ReqCompleted(entry->iTsyReqHandle,KErrCancel);
		iList->Remove(entry);
		}
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyCapsChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyCapsChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCapsChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle,RCall::TCaps*)
	{
	iFac->QueueTimer(iNotifyCapsChange,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CCallDummyBase::NotifyCapsChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyCapsChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyCapsChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyHookChange(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus* aHookStatus)
	{
	iNotifyHookChangeParams=aHookStatus;
	iFac->QueueTimer(iNotifyHookChange,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CCallDummyBase::NotifyHookChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyHookChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyHookChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	iNotifyStatusChangeParams=aStatus;
	iFac->QueueTimer(iNotifyStatusChange,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CCallDummyBase::NotifyStatusChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyStatusChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyStatusChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}


TInt CCallDummyBase::NotifyDurationChange(const TTsyReqHandle aTsyReqHandle,TTimeIntervalSeconds*)
	{
	iFac->QueueTimer(iNotifyDurationChange,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CCallDummyBase::NotifyDurationChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyDurationChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyDurationChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyDurationChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyDurationChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyDurationChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyDurationChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::DialHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iDial);
	This(aPtr)->ReqCompleted(This(aPtr)->iDial.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::ConnectHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iConnect);
	This(aPtr)->ReqCompleted(This(aPtr)->iConnect.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::AnswerHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iAnswer);
	This(aPtr)->ReqCompleted(This(aPtr)->iAnswer.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::HangUpHandler(TAny* aPtr)
	{
	This(aPtr)->SetUnowned();
	This(aPtr)->iFac->ResetPending( This(aPtr)->iHangUp);
	This(aPtr)->ReqCompleted(This(aPtr)->iHangUp.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyHookChangeHandler(TAny* aPtr)
	{
	//test server Up call
	__ASSERT_ALWAYS(This(aPtr)->UpCallOption(20,NULL)==KErrNotSupported,TsyPanic(KTsyPanicUnexpectedReturnValue));
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyHookChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyHookChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyStatusChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyStatusChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyStatusChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::LoanDataPortHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iLoanDataPortTimer);
	This(aPtr)->ReqCompleted(This(aPtr)->iLoanDataPortTimer.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetInfo(const TTsyReqHandle aTsyReqHandle, RCall::TCallInfo* /*aCallInfo*/)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	*aStatus=DACQ_CALL_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetCaps(const TTsyReqHandle aTsyReqHandle,RCall::TCaps*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::LoanDataPort(const TTsyReqHandle aTsyReqHandle,RCall::TCommPort*)
	{
	iFac->QueueTimer(iLoanDataPortTimer,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CCallDummyBase::LoanDataPortHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::LoanDataPortCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iDial.iTsyReqHandle)
		{
		iFac->RemoveTimer(iDial);
		SetUnowned();
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CCallDummyBase::RecoverDataPortAndRelinquishOwnership()
	{
	ProcessingOwnership();
	RecoverDataPortAndRelinquishOwnershipCompleted(KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::RecoverDataPort(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8* /*aParams*/,TDesC* aNumber)
	{
	
	if (CheckOwnership(aTsyReqHandle)==CCallBase::EOwnedUnowned)
		{
		SetOwnership(aTsyReqHandle);
		if(aNumber->Compare(DACQ_PHONE_NUMBER_TO_DIAL)!=KErrNone)
			TsyPanic(KTsyPanicDataCorrupted);
		iFac->QueueTimer(iDial,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CCallDummyBase::DialHandler,this);
		}
	else if (CheckOwnership(aTsyReqHandle)==CCallBase::EOwnedFalse)
		ReqCompleted(aTsyReqHandle,KErrEtelNotCallOwner);
	else
		ReqCompleted(aTsyReqHandle,KErrEtelCallAlreadyActive);

	return KErrNone;
	}

TInt CCallDummyBase::Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8*)
	{
	iFac->QueueTimer(iConnect,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CCallDummyBase::ConnectHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8*)
	{
	iFac->QueueTimer(iAnswer,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CCallDummyBase::AnswerHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::HangUp(const TTsyReqHandle aTsyReqHandle)
	{
	if (CheckOwnership(aTsyReqHandle)==CCallBase::EOwnedTrue)
		{
		iFac->QueueTimer(iHangUp,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CCallDummyBase::HangUpHandler,this);
		return KErrNone;
		}
	else
		return KErrEtelNotCallOwner;
	}

TInt CCallDummyBase::DialCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iDial.iTsyReqHandle)
		{
		iFac->RemoveTimer(iDial);
		SetUnowned();
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CCallDummyBase::ConnectCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iConnect.iTsyReqHandle)
		{
		iFac->RemoveTimer(iConnect);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CCallDummyBase::AnswerIncomingCallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iAnswer.iTsyReqHandle)
		{
		iFac->RemoveTimer(iAnswer);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CCallDummyBase::HangUpCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iHangUp.iTsyReqHandle)
		{
		iFac->RemoveTimer(iHangUp);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CCallDummyBase::GetBearerServiceInfo(const TTsyReqHandle aTsyReqHandle,RCall::TBearerService* aService)
	{
	aService->iBearerSpeed=DACQ_CALL_BEARER_SPEED;
	aService->iBearerCaps=DACQ_CALL_BEARER_CAPS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetCallParams(const TTsyReqHandle aTsyReqHandle, TDes8*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetCallDuration(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime)
	{
	*aTime = 5;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetFaxSettings(const TTsyReqHandle aTsyReqHandle,
									RCall::TFaxSessionSettings* aSettings)
	{
	aSettings->iFaxId=DACQ_MYFAX;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::SetFaxSettings(const TTsyReqHandle aTsyReqHandle,
									const RCall::TFaxSessionSettings* aSettings)
	{
	if (aSettings->iFaxId.Compare(DACQ_MYFAX)!=KErrNone)
		TsyPanic(KTsyPanicDataCorrupted);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}
	
TInt CCallDummyBase::SetFaxSharedHeaderFile(const TTsyReqHandle aTsyReqHandle, CFaxSharedFileHandles* aFaxSharedFileHandles)
	{
	TBuf8<25> line;
	TInt ret = aFaxSharedFileHandles->File().Read(0, line);

	if(ret!=KErrNone || line.Compare(DACQ_MFAXFILE) != KErrNone)
		TsyPanic(KTsyPanicDataCorrupted);
	
	delete aFaxSharedFileHandles;
	aFaxSharedFileHandles = NULL;
	
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


//
// CLineDummyBase
//
CLineDummyBase::CLineDummyBase(CPhoneFactoryDummyBase* aFac)
	{
	iFac=aFac;
	}

void CLineDummyBase::ConstructL()
	{}

CLineDummyBase::~CLineDummyBase()
	{
	iFac->RemoveTimer(iNotifyIncomingCall);
	iFac->RemoveTimer(iNotifyHookChange);
	iFac->RemoveTimer(iNotifyStatusChange);
	}

TInt CLineDummyBase::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}
TInt CLineDummyBase::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

CLineDummyBase* CLineDummyBase::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CLineDummyBase*,aPtr);
	}

TInt CLineDummyBase::CancelSubSession()
	{
	return KErrNone;
	}

CTelObject::TReqMode CLineDummyBase::ReqModeL(const TInt aIpc) 
//
//	Mode Request for Line
//
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
	case EEtelLineNotifyIncomingCall:
	case EEtelLineNotifyHookChange:
	case EEtelLineNotifyStatusChange:
		ret=KReqModeMultipleCompletionEnabled;
		break;


	case EEtelLineGetHookStatus:
	case EEtelLineGetCaps:
	case EEtelLineGetStatus:
	case EEtelLineGetInfo:
	case EEtelLineEnumerateCall:
	case EEtelLineGetCallInfo:
		ret=KReqModeFlowControlObeyed;
		break;

	default:
		User::Leave(KErrNotSupported); // Server error = Should not call this
		break;
		}
	return ret;
	}

TInt CLineDummyBase::NotifyCapsChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyCapsChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCapsChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle,RLine::TCaps*)
	{
	iFac->QueueTimer(iNotifyCapsChange,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CLineDummyBase::NotifyCapsChangeHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyCapsChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyCapsChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyStatusChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyStatusChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyStatusChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	iStatusParams=aStatus;
	iFac->QueueTimer(iNotifyStatusChange,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CLineDummyBase::NotifyStatusChangeHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyStatusChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyStatusChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyIncomingCall(const TTsyReqHandle aTsyReqHandle, TName*)
	{
	iFac->QueueTimer(iNotifyIncomingCall,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CLineDummyBase::NotifyIncomingCallHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyIncomingCallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyIncomingCall.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyIncomingCall);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyIncomingCallHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyIncomingCall);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyIncomingCall.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* aHookStatus)
	{
	iHookStatusParams=aHookStatus;
	iFac->QueueTimer(iNotifyHookChange,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CLineDummyBase::NotifyHookChangeHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyHookChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyHookChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyHookChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyHookChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyHookChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyCallAdded(const TTsyReqHandle aTsyReqHandle, TName* /*aName*/)
	{
	iFac->QueueTimer(iNotifyCallAdded,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CLineDummyBase::NotifyCallAddedHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyCallAddedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyCallAdded.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyCallAdded);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyCallAddedHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyCallAdded);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCallAdded.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::GetCaps(const TTsyReqHandle aTsyReqHandle,RLine::TCaps*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::GetInfo(const TTsyReqHandle aTsyReqHandle, RLine::TLineInfo* /*aLineInfo*/)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	*aStatus=DACQ_LINE_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

CPhoneFactoryDummyBase* CLineDummyBase::FacPtr() const
	{
	return iFac;
	}

TInt CLineDummyBase::GetHookStatus(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

//
// CPhoneDummyBase
//
CPhoneDummyBase::CPhoneDummyBase(CPhoneFactoryDummyBase* aFac)
	{
	iFac=aFac;
	}

void CPhoneDummyBase::ConstructL()
	{
	}
CPhoneDummyBase::~CPhoneDummyBase()
	{
	iFac->RemoveTimer(iNotifyPhoneDetected);
	}

CPhoneDummyBase* CPhoneDummyBase::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CPhoneDummyBase*,aPtr);
	}

TInt CPhoneDummyBase::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}
TInt CPhoneDummyBase::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

TInt CPhoneDummyBase::CancelSubSession()
	{
	return KErrNone;
	}

TInt CPhoneDummyBase::ControlledInitialisation(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::ControlledInitialisationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

void CPhoneDummyBase::Dummy()
	{
	RPhone::TPhoneInfo dummy0;
	TPckg<RPhone::TPhoneInfo> dummy1(dummy0);
	dummy1.Zero();
	}

TInt CPhoneDummyBase::NotifyCapsChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyCapsChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCapsChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle,RPhone::TCaps*)
	{
	iFac->QueueTimer(iNotifyCapsChange,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CPhoneDummyBase::NotifyCapsChangeHandler,this);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyCapsChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyCapsChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyPhoneDetectedHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending(This(aPtr)->iNotifyPhoneDetected);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyPhoneDetected.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyModemDetected(const TTsyReqHandle aTsyReqHandle,RPhone::TModemDetection* aDetection)
	{
	iDetectionParams=aDetection;
	iFac->QueueTimer(iNotifyPhoneDetected,aTsyReqHandle,DACQ_ASYN_TIMEOUT,
						CPhoneDummyBase::NotifyPhoneDetectedHandler,this);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyModemDetectedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyPhoneDetected.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyPhoneDetected);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	TsyPanic(KTsyPanicNotRecognisedCancelHandle);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetInfo(const TTsyReqHandle aTsyReqHandle, RPhone::TPhoneInfo* /*aPhoneInfo*/)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetStatus(const TTsyReqHandle aTsyReqHandle,RPhone::TStatus* aPhoneStat)
	{
	aPhoneStat->iModemDetected=DACQ_PHONE_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetCaps(const TTsyReqHandle aTsyReqHandle,RPhone::TCaps*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

CPhoneFactoryDummyBase* CPhoneDummyBase::FacPtr() const
	{
	return iFac;
	}

//
//	CPhoneFactoryDummyBase
//
CPhoneFactoryDummyBase::CPhoneFactoryDummyBase()
	{
	}

void CPhoneFactoryDummyBase::ConstructL()
	{
	iTimer=CDeltaTimer::NewL(CActive::EPriorityHigh,KEtelTimerGranularity);
	}

CPhoneFactoryDummyBase::~CPhoneFactoryDummyBase()
	{
	delete iTimer;
	}

void CPhoneFactoryDummyBase::QueueTimer(TTsyTimer& aTsyTimer,
						const TTsyReqHandle aTsyReqHandle,
						TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,
						TInt (*aFunction)(TAny *aPtr),TAny* aPtr)
	{
	TCallBack callBackFn(aFunction,aPtr);
 	aTsyTimer.iEntry.Set(callBackFn);

	aTsyTimer.iPending=ETrue;
	aTsyTimer.iTsyReqHandle=aTsyReqHandle;
	aTimeInMicroSeconds=aTimeInMicroSeconds.Int()+(KEtelTimerGranularity>>2);
	if(aTimeInMicroSeconds.Int()<100000)
		aTimeInMicroSeconds=aTimeInMicroSeconds.Int()+KEtelTimerGranularity;
	iTimer->Queue(aTimeInMicroSeconds,aTsyTimer.iEntry);
	}

TBool CPhoneFactoryDummyBase::RemoveTimer(TTsyTimer& aTsyTimer)
	{
	if (aTsyTimer.iPending)
		{
		aTsyTimer.iPending=EFalse;
		iTimer->Remove(aTsyTimer.iEntry);
		return ETrue;
		}
	return EFalse;
	}

void CPhoneFactoryDummyBase::ResetPending(TTsyTimer& aTsyTimer)
	{
	aTsyTimer.iPending=EFalse;
	}

TBool CPhoneFactoryDummyBase::IsSupported(const TInt /*aMixin*/)
	{
	return EFalse;
	}

CSubSessionExtDummyBase::CSubSessionExtDummyBase(CPhoneFactoryDummyBase* aFac)
	{
	iFac=aFac;
	}

CSubSessionExtDummyBase::~CSubSessionExtDummyBase()
	{}

CPhoneFactoryDummyBase* CSubSessionExtDummyBase::FacPtr() const
	{
	return iFac;
	}

TInt CSubSessionExtDummyBase::CancelSubSession()
	{
	return KErrNone;
	}

TInt CSubSessionExtDummyBase::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}
TInt CSubSessionExtDummyBase::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}
