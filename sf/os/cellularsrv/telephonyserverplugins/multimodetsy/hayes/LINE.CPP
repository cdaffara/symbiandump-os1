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

#include "PHONE.H"
#include "LINE.H"
#include "CALL.H"
#include "mSLOGGER.H"
#include "ATINIT.H"
#include "NOTIFY.H"
#include "et_struct.h"

//
// Dummy function is necessary so that compiler knows how to expand a TPckg<RLine::TLineInfo>
//

void CLineHayes::Dummy1()
	{
	RLine::TLineInfo dummy0;
	TPckg<RLine::TLineInfo> dummy1(dummy0);
	dummy1.Zero();
	}

//
// CLineHayes
// General Line Functionality
//
void CLineHayes::CloseLine(TAny* aObj)
//
// Utility func for cleanup stack
//
	{
	((CObject*)aObj)->Close();
	}

CLineHayes::CLineHayes(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals) : iIo(aATIO)
													,iInit(aInit),iPhoneGlobals(aPhoneGlobals)
	{}

void CLineHayes::ConstructL(const TName& aName)
	{
	LOGTEXT(_L8("CLineHayes::ConstructL()"));
	iLineName = aName;
	iCalls.SetOffset(_FOFF(CCallEntry,iLink));
	iSizeOfMemberData = new(ELeave) CArrayFixFlat<TInt>(4);
	}

void CLineHayes::AppendNewCallL(CCallHayes* aNewCall)
	{
	CCallEntry* entry = CCallEntry::NewL(aNewCall);
	iCalls.AddLast(*entry);
	}

CLineHayes::~CLineHayes()
	{
	LOGTEXT(_L8("CLineHayes Destructor"));
	if (Owner())
		((CPhoneHayes*)Owner())->RemoveLine(this);
	iPhoneGlobals->iNotificationStore->RemoveClientFromLastEvents(this);
	if (iSizeOfMemberData!=NULL)
		iSizeOfMemberData->Reset();
	delete iSizeOfMemberData;

	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	if (!(iCalls.IsEmpty()))
		{
		while (callEntry=iter++, callEntry)
			{
			if (callEntry->iCallHayes->IsOwnedByTSY())
				{
				callEntry->Deque();
				callEntry->iCallHayes->Close();
				delete callEntry;
				__ASSERT_ALWAYS(iCalls.IsEmpty(),Panic(ECalls_Remaining));
				return;	// there should not be an iPreAlloc if one of the calls in the list was still
						// owned by the TSY (ie no client had taken a handle on it)
				}
			}
		}
	if (iPreAlloc)
		{
		__ASSERT_ALWAYS(iPreAlloc,Panic(EPreAllocatedCallDoesNotExist));
		iPreAlloc->iCallHayes->Close();
		delete iPreAlloc;
		}
	}

CTelObject::TReqMode CLineHayes::ReqModeL(const TInt aIpc)
	{
	TReqMode reqMode = CLineBase::ReqModeL(aIpc);
	if ((reqMode & KReqModeFlowControlObeyed) && iPhoneGlobals->iPhoneStatus.iDataPortLoaned)
		{
		LOGTEXT2(_L8("ReqModeL Leaving with KErrInUse as data port is loaned (aIpc=%d)"),aIpc);
		User::Leave(KErrInUse);
		}
	return reqMode;
	}

TInt CLineHayes::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

TInt CLineHayes::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

void CLineHayes::Init()
	{}

TInt CLineHayes::NotifyIncomingCall(const TTsyReqHandle aTsyReqHandle, TName* aName)
	{
	iPhoneGlobals->iNotificationStore->RegisterNotification(EIncomingCall,aTsyReqHandle,this,aName);
	LOGTEXT(_L8("Line:\tIncoming Call Notification lodged"));
	iNotifyIncomingCallOutstanding=ETrue;

	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while (callEntry=iter++, callEntry)
		{
		if ((callEntry->iCallHayes->CallInfo())->iMobileStatus==RMobileCall::EStatusRinging)
			{
			iPhoneGlobals->iNotificationStore->CheckNotification(callEntry->iCallHayes,ERingOccurred);
			break;
			}
		}
	if (iPhoneGlobals->iPhoneStatus.iInitStatus==EPhoneNotInitialised)
		{
		FlowControlSuspend();
		iInit->SpecialStart();
		}
	return KErrNone;
	}

TInt CLineHayes::NotifyIncomingCallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("Line:\tIncoming Call Notification cancelled"));
	ResetNotifyIncomingCall();
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}

TInt CLineHayes::NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* aHookStatus)
	{
	LOGTEXT(_L8("Line:\tHook Change Notification lodged"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(ELineHookChange,aTsyReqHandle,this,aHookStatus);
	return KErrNone;
	}

TInt CLineHayes::NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("Line:\tHook Change Notification cancelled"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}

TInt CLineHayes::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle, RCall::TStatus* aStatus)
	{
	LOGTEXT(_L8("Line:\tStatus Change Notification lodged"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(ELineStatusChange,aTsyReqHandle,this,aStatus);
	return KErrNone;
	}

TInt CLineHayes::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("Line:\tStatus Change Notification cancelled"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}

TInt CLineHayes::NotifyCallAdded(const TTsyReqHandle aTsyReqHandle,TName* aName)
	{
	LOGTEXT(_L8("Line:\tCall Added Notification lodged"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(ENewCallAdded,aTsyReqHandle,this,aName);
	return KErrNone;
	}

TInt CLineHayes::NotifyCallAddedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("Line:\tCall Added Notification cancelled"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}

TInt CLineHayes::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RLine::TCaps* /*aCaps*/)
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CLineHayes::NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

CCallEntry* CLineHayes::CheckNewObject(const TDesC& aName) 
//
//	Server should have caught any identical names and treated them as new handles on existing 
//  objects, and not calling OpenNewObjectL() on the TSY. So panic if there is an identical name,
//  unless it is the name of the pre-allocated call owned by the line in which case the server
//  would not have been aware of it.
	{
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while(callEntry=iter++, callEntry!=NULL)
		{
		if(callEntry->iCallHayes->CheckName(aName))
			{
			if (callEntry->iCallHayes->IsOwnedByTSY())
				return callEntry;
			else
				Panic(ECallNameAlreadyExists);
			}
		}
	return NULL;
	}

TInt CLineHayes::GetInfo(const TTsyReqHandle aTsyReqHandle, RLine::TLineInfo* aLineInfo)
	{
	aLineInfo->iHookStatus = RCall::EHookStatusOn;
	aLineInfo->iStatus = RCall::EStatusIdle;
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while (callEntry=iter++, callEntry)
		{
		TCallInfoTSY* infoTSY = callEntry->iCallHayes->CallInfo();
		if (infoTSY->iHookStatus == RCall::EHookStatusOff)
			{
			aLineInfo->iHookStatus = RCall::EHookStatusOff;
			}
		if (callEntry->iCallHayes->GetCoreCallStatus() == iPhoneGlobals->iPhoneStatus.iLineStatus)
			{
			aLineInfo->iStatus = iPhoneGlobals->iPhoneStatus.iLineStatus;
			}
		}
	GetLastCallName(aLineInfo->iNameOfLastCallAdded);
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineHayes::GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aLineStatus)
//
//	This line may not be the one on which a call is being made, so check to see if any of
//  the calls on this line have the same status as the 'global' line status and if so, this
//  can be assumed to be that line
//
	{
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while(callEntry=iter++, callEntry!=NULL)
		{
		if (callEntry->iCallHayes->GetCoreCallStatus() == iPhoneGlobals->iPhoneStatus.iLineStatus)
			{
			*aLineStatus = iPhoneGlobals->iPhoneStatus.iLineStatus;
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	*aLineStatus = RCall::EStatusIdle;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineHayes::EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aParams)
//
//	Enumerate calls
//
	{
	LOGTEXT(_L8("Line:\tEnumerate Calls"));
	TInt count=0;
	TDblQueIter<CCallEntry> iter(iCalls);
	while (iter++)
		{
		count++;
		}
	*aParams = count;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineHayes::GetHookStatus(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus* aHookStatus)
//
//	Hook is considered ON if no calls exist on this line, or if all calls are idle
//	OFF if at least one is not idle. Should not be more than one active call.
//
	{
	LOGTEXT(_L8("Line:\tGet hook status"));
	RCall::THookStatus hookStatus = RCall::EHookStatusOn;
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while (callEntry=iter++, callEntry)
		{
		if (callEntry->iCallHayes->CallInfo()->iHookStatus == RCall::EHookStatusOff)
			{
			hookStatus = RCall::EHookStatusOff;
			break;	// if any calls are active, line hook status is off
			}
		}
	*aHookStatus = hookStatus;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

void CLineHayes::SetCallsHookStatus(RCall::THookStatus aHookStatus)
//
//	Is it valid to set all calls to a certain hook status at any time?
//
	{
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while (callEntry=iter++, callEntry)
		callEntry->iCallHayes->CallInfo()->iHookStatus = aHookStatus;
	}

TBool CLineHayes::StopMyCallRinging()
	{
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while (callEntry=iter++, callEntry)
		{
		if (callEntry->iCallHayes->CallInfo()->iMobileStatus==RMobileCall::EStatusRinging)
			{
			// EStatusIdle always results in KErrNone return
			(void)callEntry->iCallHayes->ChangeCallStatus(RMobileCall::EStatusIdle);
			return ETrue;
			}
		}
	return EFalse;
	}

void CLineHayes::GetLineStatus(RCall::TStatus& aLineStatus)
	{
	aLineStatus=iPhoneGlobals->iPhoneStatus.iLineStatus;
	}

void CLineHayes::RemoveCall(CCallHayes* aCallHayes)
//
//	When a call closes, it calls this to remove itself from the array
//
	{
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while (callEntry=iter++, callEntry)
		{
		if (callEntry->iCallHayes == aCallHayes)
			{
			callEntry->Deque();
			delete callEntry;	// just deletes list entry, not the call itself
			break;
			}
		}
	}

void CLineHayes::GetLastCallName(TDes& aName) const
	{
	TCallInfoIndex callInfo;
	if (iCalls.IsEmpty())
		{
		aName.Zero();
		return;
		}
	CCallEntry* callEntry = iCalls.Last();
	__ASSERT_ALWAYS(callEntry,Panic(ENewCallDoesNotExist));
	callEntry->iCallHayes->GetCallInfo(&callInfo);
	aName = callInfo.iInfo.iCallName;
	}

void CLineHayes::GetNameOfCallForAnswering(TDes& aName)
	{
	TCallInfoIndex callInfo;
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while (callEntry=iter++, callEntry)
		{
		if (callEntry->iCallHayes->IsForIncomingCall() || callEntry->iCallHayes->IsOwnedByTSY())
			{
			callEntry->iCallHayes->GetCallInfo(&callInfo);
			aName = callInfo.iInfo.iCallName;
			return;
			}
		}
	aName.Zero();
	}

TBool CLineHayes::AnswerIfPossible()
//
//	Finds the call on this line which has been asked to Answer the next incoming call, if it
//	exists, and tells it to answer.
//
	{
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while (callEntry=iter++, callEntry)
		{
		if ((callEntry->iCallHayes->IsForIncomingCall()))
			{
			callEntry->iCallHayes->AnswerImmediately();
			return ETrue;
			}
		}
	return EFalse;
	}

TBool CLineHayes::CheckForOutstandingAnswer()
	{
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while (callEntry=iter++, callEntry)
		{
		if (callEntry->iCallHayes->IsForIncomingCall())
			return ETrue;
		}
	return EFalse;
	}

const CArrayFixFlat<TInt>* CLineHayes::ArrayOfMemberDataSizes(const TInt /*aIpc*/) const
	{
	return iSizeOfMemberData;
	}

void CLineHayes::ResetNotifyIncomingCall()
	{
	iNotifyIncomingCallOutstanding=EFalse;
	}

TBool CLineHayes::IsNotifyIncomingCallOutstanding()
	{
	return iNotifyIncomingCallOutstanding;
	}

//
// Unimplemented Line Request Functions
//

TInt CLineHayes::ExtFunc(const TTsyReqHandle,const TInt, const TDataPackage&)
//
// Extensions aren't supported in this TSY
//
	{
	return KErrNotSupported;
	}


/*
 *  CLineMobile class that implements Multimode ETel Mobile Call requests
 */

CLineMobile::CLineMobile(CATIO* aIo,CATInit* aInit,CPhoneGlobals* aPhoneGlobals) 
			: CLineHayes(aIo, aInit, aPhoneGlobals)
	{}

CLineMobile::~CLineMobile()
	{
	}

CTelObject::TReqMode CLineMobile::ReqModeL(const TInt aIpc)
	{
	// ReqModeL is called from the server's CTelObject::ReqAnalyserL
	// in order to check the type of request it has

	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
//
// No Flow Control NOR Multiple Completion
//
	case EMobileLineGetMobileLineStatus:
		break;

//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobileLineNotifyMobileLineStatusChange:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
//
// Not Supported
//
	default:
		ret=CLineHayes::ReqModeL(aIpc);
		break;
		}

	return ret;
	}


TInt CLineMobile::NumberOfSlotsL(const TInt aIpc)
	{
	// NumberOfSlotsL is called by the server when it is registering a new notification
	// It enables the TSY to tell the server how many buffer slots to allocate for
	// "repost immediately" notifications that may trigger before clients collect them

	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobileLineNotifyMobileLineStatusChange:
		LOGTEXT(_L8("CLineMobile: Registered with 5 slots"));
		numberOfSlots=5;
		break;
	default:
		numberOfSlots = CLineBase::NumberOfSlotsL(aIpc);
		break;
		}
	return numberOfSlots;
	}


TInt CLineMobile::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
					 	  const TDataPackage& aPackage)
	{
	// ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
	// for the TSY to process
	// A request handle, request type and request data are passed to the TSY

	TAny* dataPtr=aPackage.Ptr1();

	// The request data has to extracted from TDataPackage and the TAny* pointers have to
	// be "cast" to the expected request data type

	switch (aIpc)
		{
//
// No Flow Control NOR Multiple Completion
//
	case EMobileLineGetMobileLineStatus:
		return GetMobileLineStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*,dataPtr));

//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobileLineNotifyMobileLineStatusChange:
		return NotifyMobileLineStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*, dataPtr));
//
// Cancels
//
	case EMobileLineNotifyMobileLineStatusChangeCancel:
		return NotifyMobileLineStatusChangeCancel(aTsyReqHandle);
	
	default:
		return KErrNotSupported;
		}
	}

TInt CLineMobile::CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle)
	{
	// CancelService is called by the server when it is "cleaning-up" any still outstanding
	// asynchronous requests before closing a client's sub-session.
	// This will happen if a client closes its R-class handle without cancelling outstanding
	// asynchronous requests.

	switch (aIpc)
		{
	case EMobileLineNotifyMobileLineStatusChange:
		return NotifyMobileLineStatusChangeCancel(aTsyReqHandle);
	default:
		return CLineBase::CancelService(aIpc,aTsyReqHandle);
		}
	}

TInt CLineMobile::GetMobileLineStatus(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus)
	{
	LOGTEXT(_L8("CLineMobile::GetMobileLineStatus called"));
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while(callEntry=iter++, callEntry!=NULL)
		{
		if (callEntry->iCallHayes->GetCoreCallStatus() == iPhoneGlobals->iPhoneStatus.iLineStatus)
			{
			//*aStatus = static_cast<RMobileCall::TMobileCallStatus>(iPhoneGlobals->iPhoneStatus.iLineStatus);
			*aStatus = (RMobileCall::TMobileCallStatus)iPhoneGlobals->iPhoneStatus.iLineStatus;
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	*aStatus = RMobileCall::EStatusIdle;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineMobile::NotifyMobileLineStatusChange(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus)
	{
	LOGTEXT(_L8("CLineMobile::NotifyMobileLineStatusChange lodged"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(EMobileLineStatusChange,aTsyReqHandle,this,aStatus);
	return KErrNone;
	}

TInt CLineMobile::NotifyMobileLineStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CLineMobile::NotifyMobileLineStatusChangeCancel called"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}


//Added for Java Demo 4.4.99
//
// CLineMobileVoice
// Voice Specific Line Functionality
//
CLineMobileVoice* CLineMobileVoice::NewL(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals,const TName& aName)
	{
	CLineMobileVoice* voiceLine=new(ELeave) CLineMobileVoice(aATIO,aInit,aPhoneGlobals);
	TCleanupItem newLineVoiceHayesClose(CloseLine,voiceLine);
	CleanupStack::PushL(newLineVoiceHayesClose);
	voiceLine->ConstructL(aName);
	CleanupStack::Pop();
	return voiceLine;
	}

CLineMobileVoice::CLineMobileVoice(CATIO* aATIO, CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	:CLineMobile(aATIO,aInit,aPhoneGlobals)
	{}

void CLineMobileVoice::ConstructL(const TName& aName)
	{
	CLineHayes::ConstructL(aName);
	TName preAllocName;
	GenerateName(preAllocName);
	CCallMobileVoice* newCall=CCallMobileVoice::NewL(iIo,iInit,iPhoneGlobals,preAllocName);
	TCleanupItem newLineClose(CloseLine,newCall);
	CleanupStack::PushL(newLineClose); 
	newCall->SetNameL(&preAllocName);
	newCall->SetOwnedByTSY();
	newCall->SetOwner(this);	
	iPreAlloc = CCallEntry::NewL(newCall);
	CleanupStack::Pop();
	}

CLineMobileVoice::~CLineMobileVoice()
	{}


CTelObject* CLineMobileVoice::OpenNewObjectByNameL(const TDesC& aName)
//
//	Open a voice call by name. This will be called if the user opens a pre-alloc'ed call by name
//
	{
	CCallEntry* entry = NULL;
	entry=CheckNewObject(aName); //if found in call list, must be a pre-alloc'ed call
	if (!entry)	
		{
		CCallMobileVoice* newCall=CCallMobileVoice::NewL(iIo,iInit,iPhoneGlobals,aName);
		TCleanupItem newLineClose(CloseLine,newCall);
		CleanupStack::PushL(newLineClose); 
		AppendNewCallL(newCall);
		CleanupStack::Pop();
		iPhoneGlobals->iNotificationStore->CheckNotification(this,ECallAdded);
		return newCall;
		}
	else  // this is a pre-allocated call
		{
		TName preAllocatedCallName;	// pre-allocate next call
		GenerateName(preAllocatedCallName);
		CCallMobileVoice* call=CCallMobileVoice::NewL(iIo,iInit,iPhoneGlobals,preAllocatedCallName);
		call->SetOwnedByTSY();
		(void)User::LeaveIfError(call->SetName(&preAllocatedCallName));
		call->SetOwner(this);
		iPreAlloc = CCallEntry::NewL(call);

		CCallHayes* oldpreAllocCall = entry->iCallHayes;
		oldpreAllocCall->SetUnownedByTSY();
		return oldpreAllocCall;
		}
	}

CTelObject* CLineMobileVoice::OpenNewObjectL(TDes& aNewName)
//
//	Open a voice call and return a name
//
	{
	GenerateName(aNewName);
	CCallMobileVoice* newCall=CCallMobileVoice::NewL(iIo,iInit,iPhoneGlobals,aNewName);
	TCleanupItem newLineClose(CloseLine,newCall);
	CleanupStack::PushL(newLineClose); 
	AppendNewCallL(newCall);
	CleanupStack::Pop();
	iPhoneGlobals->iNotificationStore->CheckNotification(this,ECallAdded);
	return newCall;
	}

void CLineMobileVoice::GenerateName(TDes& aName) 
	{
	aName.Append(KVoiceCallName);
	aName.AppendNum(iNameIndex++);
	}

TInt CLineMobileVoice::GetCaps(const TTsyReqHandle aTsyReqHandle,RLine::TCaps* aLineCaps)
	{
	aLineCaps->iFlags = RLine::KCapsEventIncomingCall;
	aLineCaps->iFlags |= RLine::KCapsVoice;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineMobileVoice::GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex* /*aCallInfoIndex*/)
//
//	Provide info about voice call
//
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

//
// CLineMobileData
// Data Specific Line Functionality
//
CLineMobileData* CLineMobileData::NewL(CATIO* aATIO, CATInit* aInit,CPhoneGlobals* aPhoneGlobals,const TName& aName)
	{
	CLineMobileData* dataLine=new(ELeave) CLineMobileData(aATIO,aInit,aPhoneGlobals);
	TCleanupItem newLineDataHayesClose(CloseLine,dataLine);
	CleanupStack::PushL(newLineDataHayesClose);
	dataLine->ConstructL(aName);
	CleanupStack::Pop();
	return dataLine;
	}

CLineMobileData::CLineMobileData(CATIO* aATIO, CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	:CLineMobile(aATIO,aInit,aPhoneGlobals)
	{}

void CLineMobileData::ConstructL(const TName& aName)
//
//	Constructs a call which is to be used only when an incoming call arrives
//	and no client has designated an existing call to answer it. TSY holds responsibility to close 
//	it unless a RING occurs, when the line will add the call to the linked list of calls 
//  and alert any interested clients that a "new" call has been created - if any client then
//	opens a handle on it, the TSY relinquishes any responsibility to close it in the future
//	as the client has that responsibility. When the client opens a handle on it, using OpenByName
//	the line will create a new PreAlloc'ed call. If no client opens a handle, when the line 
//	stops ringing the line will remove the pre-alloc'ed call from the list and keep the pointer
//	to it separately in CLineHayes::iPreAlloc, so any subsequent attempt by a client to 
//  open a handle on it will fail with KErrDoesNotExist or something similar. 
//
	{
	CLineHayes::ConstructL(aName);
	TName preAllocName;
	GenerateName(preAllocName);
	CCallMobileData* newCall=CCallMobileData::NewL(iIo,iInit,iPhoneGlobals,preAllocName);
	TCleanupItem newLineClose(CloseLine,newCall);
	CleanupStack::PushL(newLineClose); 
	newCall->SetNameL(&preAllocName);
	newCall->SetOwnedByTSY();
	newCall->SetOwner(this);	
	iPreAlloc = CCallEntry::NewL(newCall);
	CleanupStack::Pop();
	}

CLineMobileData::~CLineMobileData()
	{}

CTelObject* CLineMobileData::OpenNewObjectByNameL(const TDesC& aName)
//
//	Open a data call by name. This will be called if the user opens a pre-alloc'ed call by name
//
	{
	CCallEntry* entry = NULL;
	entry=CheckNewObject(aName); //if found in call list, must be a pre-alloc'ed call
	if (!entry)	
		{
		CCallMobileData* newCall=CCallMobileData::NewL(iIo,iInit,iPhoneGlobals,aName);
		TCleanupItem newLineClose(CloseLine,newCall);
		CleanupStack::PushL(newLineClose); 
		AppendNewCallL(newCall);
		CleanupStack::Pop();
		iPhoneGlobals->iNotificationStore->CheckNotification(this,ECallAdded);
		return newCall;
		}
	else  // this is a pre-allocated call
		{
		TName preAllocatedCallName;	// pre-allocate next call
		GenerateName(preAllocatedCallName);
		CCallMobileData* call=CCallMobileData::NewL(iIo,iInit,iPhoneGlobals,preAllocatedCallName);
		call->SetOwnedByTSY();
		(void)User::LeaveIfError(call->SetName(&preAllocatedCallName));
		call->SetOwner(this);
		iPreAlloc = CCallEntry::NewL(call);

		CCallHayes* oldpreAllocCall = entry->iCallHayes;
		oldpreAllocCall->SetUnownedByTSY();
		return oldpreAllocCall;
		}
	}

CTelObject* CLineMobileData::OpenNewObjectL(TDes& aNewName)
//
//	Open a data call and return a name
//
	{
	GenerateName(aNewName);
	CCallMobileData* newCall=CCallMobileData::NewL(iIo,iInit,iPhoneGlobals,aNewName);
	TCleanupItem newLineClose(CloseLine,newCall);
	CleanupStack::PushL(newLineClose); 
	AppendNewCallL(newCall);
	CleanupStack::Pop();
	iPhoneGlobals->iNotificationStore->CheckNotification(this,ECallAdded);
	return newCall;
	}

void CLineMobileData::GenerateName(TDes& aName) 
	{
	aName.Append(KDataCallName);
	aName.AppendNum(iNameIndex++);
	}

TInt CLineMobileData::GetCaps(const TTsyReqHandle aTsyReqHandle,RLine::TCaps* aLineCaps)
	{
	aLineCaps->iFlags = RLine::KCapsEventIncomingCall;
	if (iPhoneGlobals->iPhoneStatus.iDataAndFaxFlags & RPhone::KCapsData)
		{
		aLineCaps->iFlags |= RLine::KCapsData;
		}
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineMobileData::GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex* aCallInfoIndex)
//
//	Provide info about data call
//
	{
	LOGTEXT(_L8("DataLine:\tGet Data Call Info"));
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	iter.SetToFirst();
	for (TInt i=0;i<(TInt)(aCallInfoIndex->iIndex);i++)
		{
		iter++;
		}
	callEntry=iter;
	if (callEntry)
		{
		callEntry->iCallHayes->GetCallInfo(aCallInfoIndex);
		aCallInfoIndex->iInfo.iCallCapsFlags |= RCall::KCapsData;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else
		ReqCompleted(aTsyReqHandle,KErrNotFound);
	return KErrNone;
	}

void CLineHayes::SetPreAllocCall()
//
//	If the incoming call hasn't been answered because there were no client-designated "Answerers"
//  on either line, notify any interested client that there is a new call (which is actually the 
//	pre-allocated call).
//
	{
	// EStatusRinging always returns KErrNone
	(void)iPreAlloc->iCallHayes->ChangeCallStatus(RMobileCall::EStatusRinging);
	iCalls.AddLast(*iPreAlloc);		
	iPhoneGlobals->iNotificationStore->CheckNotification(iPreAlloc->iCallHayes,ERingOccurred);
	iPhoneGlobals->iNotificationStore->CheckNotification(this,ECallAdded);
	iPreAlloc = NULL;	// pre-alloc'ed call and "call entry" is now owned by iCalls
	}

void CLineHayes::FreePreAllocCallIfNecessary()
//
// Used when answering a call, to check if whether the the answer request has been given on a call object
// that is not the preallocated call.  If it has then free up the preallocated call.
//
	{
	if(!iPreAlloc)
		{
		ResetPreAllocCall();
		}
	}

void CLineHayes::ResetPreAllocCall()
//
//	Removes the TSY-owned Pre allocated call from the list of calls and returns it to iPreAlloc.
//
	{
	CCallEntry* callEntry;
	TDblQueIter<CCallEntry> iter(iCalls);
	while (callEntry=iter++, callEntry)
		{
		if ((callEntry->iCallHayes->IsOwnedByTSY()))
			{
			callEntry->Deque();
			iPreAlloc = callEntry;
			iPreAlloc->iCallHayes->ResetIsForIncomingCall();
			}
		}
	}


CCallEntry* CCallEntry::NewL(CCallHayes* aCallHayes)
//
//	Call Entry has a pointer to the call, and makes up an entry in the linked list iCalls
//
	{
	return new(ELeave) CCallEntry(aCallHayes);
	}

CCallEntry::CCallEntry(CCallHayes* aCallHayes)
//
//	CCallEntry constructor
//
	: iCallHayes(aCallHayes)
	{}

CCallEntry::~CCallEntry()
//
//	CCallEntry destructor
//
	{}

void CCallEntry::Deque()
//
//	Deque CCallEntry list
//
	{
	iLink.Deque();
	iLink.iPrev=iLink.iNext=NULL;
	}
