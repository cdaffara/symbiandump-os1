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

#include <commsdattypesv1_1.h>
#include "CALL.H"		// Header file for this source file
#include "NOTIFY.H"
#include "ATDIAL.H"
#include "ATANSWER.H"
#include "ATCONNCT.H"
#include "ATHANGUP.H"
#include "ATO.H"
#include "ATNOCARR.H"
#include "LINE.H"
#include "ATINIT.H"
#include "mSLOGGER.H"	// for LOGTEXT untilities
#include "ATIO.H"		// For CATIO class
#include "PHONE.H"
#include <et_clsvr.h>
#include "ATBASE.H"
#include "Matstd.h"		// For AT command string constants 
#include "set_cbst.h"		// for CATSetCBST class
#include "et_struct.h"


//
// CAcquireEntry class
//
CAcquireEntry* CAcquireEntry::NewL(const TTsyReqHandle aTsyReqHandle)
//
// Create new request entry
//
	{
	return new(ELeave) CAcquireEntry(aTsyReqHandle);
	}

CAcquireEntry::CAcquireEntry(const TTsyReqHandle aTsyReqHandle)
//
// Constructor
//
	{
	iTsyReqHandle=aTsyReqHandle;
	}

CAcquireEntry::~CAcquireEntry()
//
// Destructor
//
	{}

void CAcquireEntry::Deque()
//
// Deque List
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
//	Static function to create new acquire owner list
//
	{
	CAcquireOwnerList* self=new(ELeave) CAcquireOwnerList();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CAcquireOwnerList::ConstructL()
	{
	iAcquireList.SetOffset(_FOFF(CAcquireEntry,iLink));
	}

CAcquireEntry* CAcquireOwnerList::FindByTsyReqHandle(const TTsyReqHandle aTsyReqHandle)
//
//	Searches for client interested in taking ownership of call, by its TsyReqHandle
//
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
//	CHeartbeatRunner - periodic class to count seconds from beginning of call
//
CHeartbeatRunner* CHeartbeatRunner::NewL(CCallHayes* aCallHayes,CNotifications* aNotificationStore)
	{
	CHeartbeatRunner* self=new (ELeave) CHeartbeatRunner(aCallHayes,aNotificationStore);
	CleanupStack::PushL(self);
    self->ConstructL();
	CleanupStack::Pop();
    return self;
	}

CHeartbeatRunner::CHeartbeatRunner(CCallHayes* aCallHayes,CNotifications* aNotificationStore)
	:iCallHayes(aCallHayes), iNotificationStore(aNotificationStore)
	{}

void CHeartbeatRunner::ConstructL()
	{
	iHeartbeat=CHeartbeat::NewL(0); // neutral priority
	}

CHeartbeatRunner::~CHeartbeatRunner()
	{
	if (iHeartbeat != NULL)
		iHeartbeat->Cancel();
    delete iHeartbeat;
	}

void CHeartbeatRunner::Start()
	{
	iStartTime.UniversalTime();
	iHeartbeat->Start(ETwelveOClock,this);
	}

void CHeartbeatRunner::Stop()
	{
	iHeartbeat->Cancel();
	iTicks=0;
	}

TTimeIntervalSeconds CHeartbeatRunner::CallDuration() const
	{
	TTimeIntervalSeconds duration(iTicks);
	return duration;
	}

// private functions

void CHeartbeatRunner::Beat()
	{
	iTicks++;
	iNotificationStore->CheckNotification(iCallHayes,ETimePeriodElapsed);
	}

void CHeartbeatRunner::Synchronize()
	{
	TInt ticksMissed = 0;
	TTime desiredTime = iStartTime + TTimeIntervalMicroSeconds(iTicks * KCallTimerInterval);
	TTime currentTime; // set current time to now
	currentTime.UniversalTime();
	TTimeIntervalMicroSeconds missedTime = currentTime.MicroSecondsFrom(desiredTime);
	// Calculate the ticks missed (quickly!)
	TInt64 missedTimeInt = missedTime.Int64(); // convert the missed time interval to an Int64
	ticksMissed = I64INT(missedTimeInt / KCallTimerInterval);
	// The following loop increments the ticks missed by the same amount, but takes much longer
	// while (desiredTime < currentTime)
	//	{
	//	desiredTime = desiredTime - TTimeIntervalMicroSeconds(iTickInterval);
	//	ticksMissed++;
	//	}
	iTicks = iTicks + ticksMissed;
	LOGTEXT3(_L8("Heartbeat function synchronising - from %d to %d"),iTicks-ticksMissed,iTicks);
	if (ticksMissed!=0)
		iNotificationStore->CheckNotification(iCallHayes,ETimePeriodElapsed);
	}

//
// CCallHayes - General Call Functionality
//
void CCallHayes::CloseCall(TAny* aObj)
//
// Utility func for cleanup stack
//
	{
	((CObject*)aObj)->Close();
	}

CCallHayes::CCallHayes(CATIO* aIo,CATInit* aInit,CPhoneGlobals* aPhoneGlobals) 
			: iPhoneGlobals(aPhoneGlobals),iIo(aIo),iInit(aInit)
	{}

void CCallHayes::ConstructL(const TName& aName)
//
//	Initialise Call Information
//
	{
	LOGTEXT(_L8("Entered CCallHayes::ConstructL()"));
	iCallInfo.iCallName = aName;
	iCallInfo.iBearerService.iBearerCaps = RCall::KBearerCapsCompressionUnknown | RCall::KBearerCapsProtocolUnknown;
	iCallInfo.iBearerService.iBearerSpeed = RCall::EBearerDataUnknown;
	iCallInfo.iHookStatus = RCall::EHookStatusOn;
	iCallInfo.iClientPanicOccurred = ENoPanicOccurred;
	//	Read default call preferences from database
	GetDefaultCallParams();
	iCallInfo.iLoanedToClient = EFalse;
	if (iPhoneGlobals->iPhoneStatus.iLineStatus == RCall::EStatusRinging)
		{
		iCallInfo.iMobileStatus = RMobileCall::EStatusRinging;
		}
	else
		{
		iCallInfo.iMobileStatus = RMobileCall::EStatusIdle;
		}

	iQuickInit=CATQuickInit::NewL(iIo,this,iPhoneGlobals);
	iWaitForNoCarrier=CATWaitForNoCarrier::NewL(iIo,this,iPhoneGlobals);
	iList=CAcquireOwnerList::NewL();
	iCallTimer = CHeartbeatRunner::NewL(this,iPhoneGlobals->iNotificationStore);
	LOGTEXT2(_L8("iCallTimer = %x"),iCallTimer);
	}

CCallHayes::~CCallHayes()
	{
	LOGTEXT(_L8("Entered CCallHayes destructor"));
	if (Owner())
		REINTERPRET_CAST(CLineHayes*,Owner())->RemoveCall(this);
	iPhoneGlobals->iNotificationStore->RemoveClientFromLastEvents(this);
	delete iWaitForNoCarrier;
	delete iList;
	delete iQuickInit;
	delete iCallTimer;
	}

CTelObject::TReqMode CCallHayes::ReqModeL(const TInt aIpc)
	{
	TReqMode reqMode = CCallBase::ReqModeL(aIpc);
	if ((reqMode & KReqModeFlowControlObeyed || aIpc==EEtelCallAnswer) && iPhoneGlobals->iPhoneStatus.iDataPortLoaned)
		{
		LOGTEXT2(_L8("ReqModeL Leaving with KErrInUse as data port is loaned (aIpc=%d)"),aIpc);
		User::Leave(KErrInUse);
		}
	return reqMode;
	}

TInt CCallHayes::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}
TInt CCallHayes::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}


void CCallHayes::Init()
//
//	Only CPhoneHayes::Init() is implemented as that is called first
//
	{}


TInt CCallHayes::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RCall::TCaps* aCaps)
	{
	LOGTEXT(_L8("Call:\tCaps Change Notification lodged"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(ECallCaps,aTsyReqHandle,this,aCaps);
	return KErrNone;
	}

TInt CCallHayes::NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("Call:\tCaps Change Notification cancelled"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}

TInt CCallHayes::NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* aHookStatus)
	{
	LOGTEXT(_L8("Call:\tHook Change Notification lodged"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(ECallHookChange,aTsyReqHandle,this,aHookStatus);
	return KErrNone;
	}

TInt CCallHayes::NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("Call:\tHook Change Notification cancelled"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}

TInt CCallHayes::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	LOGTEXT(_L8("Call:\tStatus Change Notification lodged"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(ECallStatusChange,aTsyReqHandle,this,aStatus);
	return KErrNone;
	}

TInt CCallHayes::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("Call:\tStatus Change Notification cancelled"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}

TInt CCallHayes::NotifyDurationChange(const TTsyReqHandle aTsyReqHandle,TTimeIntervalSeconds* aTime)
	{
	LOGTEXT(_L8("Call:\tDuration Change Notification lodged"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(ECallDurationChange,aTsyReqHandle,this,aTime);
	return KErrNone;
	}

TInt CCallHayes::NotifyDurationChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("Call:\tDuration Change Notification cancelled"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}

TInt CCallHayes::GetInfo(const TTsyReqHandle aTsyReqHandle, RCall::TCallInfo* aCallInfo)
	{
	aCallInfo->iCallName = iCallInfo.iCallName;
	aCallInfo->iStatus = GetCoreCallStatus();
	aCallInfo->iHookStatus = iCallInfo.iHookStatus;
	aCallInfo->iLineName = Owner()->Name();
	GetCallDuration(aCallInfo->iDuration);
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallHayes::GetStatus(const TTsyReqHandle aTsyReqHandle, RCall::TStatus* aCallStatus)
    {
	LOGTEXT(_L8("Call:\tGetStatus() called"));
	*aCallStatus = GetCoreCallStatus();
	ReqCompleted(aTsyReqHandle,KErrNone);
    return KErrNone;
    }

TInt CCallHayes::TransferOwnership(const TTsyReqHandle aTsyReqHandle)
//
// Transfer call ownership
//
	{
	LOGTEXT(_L8("Call:\tTransferOwnership called"));
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
		LOGTEXT(_L8("Call:\tTransferOwnership successful"));
		(void)SetOwnership(entry->iTsyReqHandle);
		ReqCompleted(entry->iTsyReqHandle,KErrNone);
		iList->Remove(entry);
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CCallHayes::AcquireOwnership(const TTsyReqHandle aTsyReqHandle)
//
//	Acquire call Ownership (server has already checked client does not own it)
//	If call is unowned and idle, this request is completed with CallNotActive.
//  A call must never be owned and idle concurrently.
//
	{
	LOGTEXT(_L8("Call:\tAcquireOwnership called"));
	if (CheckOwnership(aTsyReqHandle)==CCallBase::EOwnedUnowned)
		{
		LOGTEXT(_L8("Call:\tAcquireOwnership unsuccessful as call is not owned already"));
		ReqCompleted(aTsyReqHandle,KErrEtelCallNotActive);
		}
	else 
		{
		if(iList->iAcquireList.IsEmpty())
			// List is empty. Client is the first one to request ownership of the call.
			{
			CAcquireEntry* entry=NULL;
			TRAPD(err,entry=CAcquireEntry::NewL(aTsyReqHandle));
			if(err==KErrNone)
				iList->iAcquireList.AddLast(*entry);
			else
				return err;
			}
		else
			// List is not empty. Another client has already requested to acquire ownership of the call.
			// Only one client can be waiting to acquire ownership at any one time.
			return KErrInUse;
		}
	return KErrNone;
	}

TInt CCallHayes::AcquireOwnershipCancel(const TTsyReqHandle aTsyReqHandle)
//
//	Cancel Acquire call Ownership
//
	{
	CAcquireEntry* entry=iList->FindByTsyReqHandle(aTsyReqHandle);
	__ASSERT_ALWAYS(entry!=NULL,Panic(EAcquirerNotFound));
	if (entry != NULL)
		{
		iList->Remove(entry);
		ReqCompleted(aTsyReqHandle, KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle, KErrNotFound);
		}					
	return KErrNone;		
	}
	
TInt CCallHayes::GetBearerServiceInfo(const TTsyReqHandle aTsyReqHandle,RCall::TBearerService* aBearerService)
	{
	*aBearerService = iCallInfo.iBearerService;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallHayes::GetCallParams(const TTsyReqHandle aTsyReqHandle, TDes8* aParams)
//
// Call parameters are only set when connecting a call, so there have no meaning when a 
// call is not in progress.
//
	{
	if (RMobileCall::EStatusIdle != iCallInfo.iMobileStatus  &&
		RMobileCall::EStatusUnknown != iCallInfo.iMobileStatus )
		{
		TPckg<RCall::TCallParams>* paramsPckg = (TPckg<RCall::TCallParams>*)aParams;
		RCall::TCallParams& callparams = (*paramsPckg)();

		//
		// Configure basic TCallParams parameters
		callparams.iSpeakerControl = iCallInfo.iSpeakerControl;
		callparams.iSpeakerVolume = iCallInfo.iSpeakerVolume;
		callparams.iInterval = iCallInfo.iInterval;
		callparams.iWaitForDialTone = iCallInfo.iWaitForDialTone;
	
		//
		// Configure additional params as required
		if(callparams.ExtensionId()==RMobileCall::KETelMobileCallParamsV1)
			{
			//
			// Configure RMobileCall::TMobileCallParamsV1 parameters
			RMobileCall::TMobileCallParamsV1Pckg* pckg = (RMobileCall::TMobileCallParamsV1Pckg*)aParams;
			RMobileCall::TMobileCallParamsV1& params = (*pckg)();
			params.iIdRestrict=iCallInfo.iCallParams.iIdRestrict;
			params.iCug=iCallInfo.iCallParams.iCug;
			params.iAutoRedial=iCallInfo.iCallParams.iAutoRedial;
			}
		else if(callparams.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV1)
			{
			//
			// Configure RMobileCall::TMobileDataCallParamsV1 parameters
			RMobileCall::TMobileDataCallParamsV1Pckg* pckg = (RMobileCall::TMobileDataCallParamsV1Pckg*)aParams;
			RMobileCall::TMobileDataCallParamsV1& params = (*pckg)();
			params.iService=iCallInfo.iCallParams.iService;
			params.iSpeed=iCallInfo.iCallParams.iSpeed;
			params.iProtocol=iCallInfo.iCallParams.iProtocol;
			params.iQoS=iCallInfo.iCallParams.iQoS;
			params.iRLPVersion=iCallInfo.iCallParams.iRLPVersion;
			params.iModemToMSWindowSize=iCallInfo.iCallParams.iModemToMSWindowSize;
			params.iMSToModemWindowSize=iCallInfo.iCallParams.iMSToModemWindowSize;
			params.iAckTimer=iCallInfo.iCallParams.iAckTimer;
			params.iRetransmissionAttempts=iCallInfo.iCallParams.iRetransmissionAttempts;
			params.iResequencingPeriod=iCallInfo.iCallParams.iResequencingPeriod;
			params.iV42bisReq=iCallInfo.iCallParams.iV42bisReq;
			params.iV42bisCodewordsNum=iCallInfo.iCallParams.iV42bisCodewordsNum;
			params.iV42bisMaxStringLength=iCallInfo.iCallParams.iV42bisMaxStringLength;
			}
		else if(callparams.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV1)
			{
			//
			// Configure RMobileCall::TMobileHscsdCallParamsV1 parameters
			RMobileCall::TMobileHscsdCallParamsV1Pckg* pckg = (RMobileCall::TMobileHscsdCallParamsV1Pckg*)aParams;
			RMobileCall::TMobileHscsdCallParamsV1& params = (*pckg)();
			params.iWantedAiur=iCallInfo.iCallParams.iWantedAiur;
			params.iWantedRxTimeSlots=iCallInfo.iCallParams.iWantedRxTimeSlots;
			params.iMaxTimeSlots=iCallInfo.iCallParams.iMaxTimeSlots;
			params.iCodings=iCallInfo.iCallParams.iCodings;
			params.iAsymmetry=iCallInfo.iCallParams.iAsymmetry;
			params.iUserInitUpgrade=iCallInfo.iCallParams.iUserInitUpgrade;
			}
		
		//
		// Complete the clients request
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else 
		{
		ReqCompleted(aTsyReqHandle,KErrUnknown);
		}
	return KErrNone;
	}

TInt CCallHayes::GetCallDuration(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime)
	{
	if ( RMobileCall::EStatusConnected != iCallInfo.iMobileStatus && 
		 RMobileCall::EStatusDisconnecting != iCallInfo.iMobileStatus)
		{
		ReqCompleted(aTsyReqHandle,KErrEtelCallNotActive);
		return KErrNone;
		}
	*aTime = iCallTimer->CallDuration();
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

void CCallHayes::GetCallDuration(TTimeIntervalSeconds& aTime) const
	{
	aTime = iCallTimer->CallDuration();
	}

void CCallHayes::GetDefaultCallParams()
	{
	iPhoneGlobals->iConfiguration->GetIntervalPref(iCallInfo.iInterval);
	iPhoneGlobals->iConfiguration->GetSpeakerSettingPref(iCallInfo.iSpeakerControl);
	iPhoneGlobals->iConfiguration->GetSpeakerVolumePref(iCallInfo.iSpeakerVolume);
	iPhoneGlobals->iConfiguration->GetWaitForDialTonePref(iCallInfo.iWaitForDialTone);
	}

void CCallHayes::SetCallParams(const TDesC8* aParams)
	{
	if ((*aParams).Length()==0)
		// Always returns KErrNone
		GetDefaultCallParams();
	else
		{
		TPckg<RCall::TCallParams>* paramsPckg = (TPckg<RCall::TCallParams>*)aParams;
		RCall::TCallParams& callparams = (*paramsPckg)();
		iCallInfo.iSpeakerControl = callparams.iSpeakerControl;
		iCallInfo.iSpeakerVolume = callparams.iSpeakerVolume;
		iCallInfo.iInterval = callparams.iInterval;
		iCallInfo.iWaitForDialTone = callparams.iWaitForDialTone; 
		}
	}

TInt CCallHayes::ValidateRequest(const TTsyReqHandle aTsyReqHandle, RCall::TStatus aLineStatus)
//
//  Validating a request
//
	{
	CCallBase::TCallOwnership owned = CheckOwnership(aTsyReqHandle);
	if (owned==CCallBase::EOwnedFalse)	// call owned by another client
		{
		return KErrEtelNotCallOwner;
		}
	if (iPhoneGlobals->iPhoneStatus.iLineStatus != aLineStatus) 
		{
		if (aLineStatus==RCall::EStatusIdle)
		// implies that this call is already active (assuming an owned call is 
		// an active call) or ringing or unknown
			{
			return KErrInUse;
			}
		else if (aLineStatus==RCall::EStatusRinging)
			{
			return KErrNotReady;
			}
		}
	return KErrNone;
	}


RCall::TStatus CCallHayes::GetCoreCallStatus()
	{
	RCall::TStatus coreStatus;
	if (iCallInfo.iMobileStatus <= RMobileCall::EStatusDisconnecting)
		//coreStatus = static_cast<RCall::TStatus>(iCallInfo.iMobileStatus);
		coreStatus = (RCall::TStatus)iCallInfo.iMobileStatus;
	else
		switch (iCallInfo.iMobileStatus)
		{
		case RMobileCall::EStatusReconnectPending:
		case RMobileCall::EStatusHold:
			coreStatus = RCall::EStatusConnected;
			break;
		case RMobileCall::EStatusWaitingAlternatingCallSwitch:
			coreStatus = RCall::EStatusIdle;
			break;
		default:
			coreStatus = RCall::EStatusUnknown;
			break;
		}
	return coreStatus;
	}

TInt CCallHayes::ChangeCallStatus(RMobileCall::TMobileCallStatus aCallStatus)
	{
	if (iCallInfo.iMobileStatus != aCallStatus)
		{
		iCallInfo.iMobileStatus = aCallStatus;
		
		if (aCallStatus == RMobileCall::EStatusIdle)
			{
			iCallInfo.iHookStatus = RCall::EHookStatusOn;
			iPhoneGlobals->iNotificationStore->CheckNotification(this,EBecomeIdle);
			}
		else if (aCallStatus != RMobileCall::EStatusUnknown && aCallStatus != RMobileCall::EStatusRinging)
			{
			iCallInfo.iHookStatus = RCall::EHookStatusOff;
			}
		if (aCallStatus == RMobileCall::EStatusConnected)
			{
			iPhoneGlobals->iNotificationStore->CheckNotification(this,EConnected);
			iIo->Cancel();
			TCommConfig aConfigPckg;
			TInt ret = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeConnect);
			if (ret==KErrNone)
				ret = iIo->ConfigurePort(aConfigPckg);
//			if (!(iIo->ReadPending()))
//				iIo->Read();
			iCallInfo.iTimeCallBegan.UniversalTime();
			return ret;
			}
		}
	return KErrNone;
	}

void CCallHayes::ChangeLineStatus(RCall::TStatus aLineStatus)
	{
	iPhoneGlobals->iPhoneStatus.iLineStatus = aLineStatus;
	}

void CCallHayes::SetToIdle()
	{
	iWaitForNoCarrier->StopWait();
	// Always returns KErrNone
	(void)SetUnowned();
	iPhoneGlobals->iPhoneStatus.iMode = RPhone::EModeIdle;
	ChangeLineStatus(RCall::EStatusIdle);	// line status should be changed first because 
											// ChangeCallStatus is the function which sends
											// the new event to the Notification Handler,
											// and this will complete both line and call status
											// notifications, taking the new statuses from the
											// CLineHayes and CCallHayes objects.
	// EStatusIdle always results in KErrNone return value
	(void)ChangeCallStatus(RMobileCall::EStatusIdle);
	StopCallTicker();
	}

void CCallHayes::SetToIdleAndCompleteReq(TTsyReqHandle aTsyReqHandle,TInt aStatus)
	{
	SetToIdle();
	ReqCompleted(aTsyReqHandle, aStatus);
	}

void CCallHayes::GetCallInfo(TCallInfoIndex* aCallInfoIndex)
//
//	Copied field by field since TCallInfoTSY is different to TCallInfoIndex
//
	{
	aCallInfoIndex->iInfo.iCallName = iCallInfo.iCallName;
	aCallInfoIndex->iInfo.iStatus = GetCoreCallStatus();
	aCallInfoIndex->iInfo.iCallCapsFlags = 0;
	}

TBool CCallHayes::CheckName(const TDesC& aName) const
//
//	Return TRUE if name is the same as the name of this call
	{
	if (iCallInfo.iCallName.CompareF(aName))	
		return EFalse;		
	else 
		return ETrue;
	}

TCallInfoTSY* CCallHayes::CallInfo()
	{
	return &iCallInfo;
	}

void CCallHayes::StartCallTicker() const
	{
	iCallTimer->Start();
	}

void CCallHayes::StopCallTicker() const
	{
	iCallTimer->Stop();
	}

void CCallHayes::ResetIsForIncomingCall()
	{
	iIsForIncomingCall=EFalse;
	}

TBool CCallHayes::IsForIncomingCall() const
	{
	return iIsForIncomingCall;
	}

void CCallHayes::SetOwnedByTSY() 
	{
	iIsOwnedByTSY=ETrue;
	}

void CCallHayes::SetUnownedByTSY()
	{
	iIsOwnedByTSY=EFalse;
	}

TBool CCallHayes::IsOwnedByTSY() const
	{
	return iIsOwnedByTSY;
	}


//
//	Functions which are supported in one line but not the other
//
TInt CCallHayes::LoanDataPort(const TTsyReqHandle,RCall::TCommPort*)
	{
	return KErrNotSupported;
	}

TInt CCallHayes::LoanDataPortCancel(const TTsyReqHandle)
	{
	return KErrNotSupported;
	}

TInt CCallHayes::RecoverDataPort(const TTsyReqHandle)
	{
	return KErrNotSupported;
	}

TInt CCallHayes::RecoverDataPortAndRelinquishOwnership()
	{
	return KErrNotSupported;
	}

TInt CCallHayes::GetFaxSettings(const TTsyReqHandle,RCall::TFaxSessionSettings*)
	{	
	return KErrNotSupported;
	}

TInt CCallHayes::SetFaxSettings(const TTsyReqHandle,const RCall::TFaxSessionSettings*)
	{	
	return KErrNotSupported;
	}

CTelObject* CCallHayes::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CCallHayes::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

TInt CCallHayes::CheckAndSetRegistrationParams(const TInt /*aIpc*/,const TDes8* /*aDes1*/,const TDes8* /*aDes2*/)
	{
	return KErrNone;
	}

TInt CCallHayes::ExtFunc(const TTsyReqHandle,const TInt, const TDataPackage&)
//
// Unsupported in this TSY
//
	{
	return KErrNotSupported;
	}

/*
 *  CCallMobile class that implements Multimode ETel Mobile Call requests
 */

CCallMobile::CCallMobile(CATIO* aIo,CATInit* aInit,CPhoneGlobals* aPhoneGlobals) 
			: CCallHayes(aIo, aInit, aPhoneGlobals)
	{
	}

CCallMobile::~CCallMobile()
	{
	}

CTelObject::TReqMode CCallMobile::ReqModeL(const TInt aIpc)
	{
	// ReqModeL is called from the server's CTelObject::ReqAnalyserL
	// in order to check the type of request it has

	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
//
// No Flow Control NOR Multiple Completion
//
	case EMobileCallGetMobileCallCaps:
	case EMobileCallGetMobileCallStatus:
		break;
	
	case EMobileCallGetMobileDataCallCaps: 
		break;
//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobileCallNotifyMobileCallStatusChange:
	case EMobileCallNotifyMobileCallCapsChange:

		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
//
// 
//
	default:
		ret=CCallHayes::ReqModeL(aIpc);
		break;
		}

	return ret;
	}


TInt CCallMobile::NumberOfSlotsL(const TInt aIpc)
	{
	// NumberOfSlotsL is called by the server when it is registering a new notification
	// It enables the TSY to tell the server how many buffer slots to allocate for
	// "repost immediately" notifications that may trigger before clients collect them

	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobileCallNotifyMobileCallStatusChange:
	case EMobileCallNotifyMobileCallCapsChange:
		LOGTEXT(_L8("CCallMobile: Registered with 5 slots"));
		numberOfSlots=5;
		break;
	default:
		numberOfSlots = CCallBase::NumberOfSlotsL(aIpc);
		break;
		}
	return numberOfSlots;
	}


TInt CCallMobile::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
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
	case EMobileCallGetMobileCallCaps:
		return GetMobileCallCaps(aTsyReqHandle, aPackage.Des1n());

	case EMobileCallGetMobileCallStatus:
		return GetMobileCallStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*,dataPtr));
//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobileCallNotifyMobileCallStatusChange:
		return NotifyMobileCallStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*, dataPtr));

	case EMobileCallNotifyMobileCallCapsChange:
		return NotifyMobileCallCapsChange(aTsyReqHandle, aPackage.Des1n());

//
// Cancels
//
	case EMobileCallNotifyMobileCallStatusChangeCancel:
		return NotifyMobileCallStatusChangeCancel(aTsyReqHandle);
	
	case EMobileCallNotifyMobileCallCapsChangeCancel:
		return NotifyMobileCallCapsChangeCancel(aTsyReqHandle);

	default:
		return KErrNotSupported;
		}
	}

TInt CCallMobile::CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle)
	{
	// CancelService is called by the server when it is "cleaning-up" any still outstanding
	// asynchronous requests before closing a client's sub-session.
	// This will happen if a client closes its R-class handle without cancelling outstanding
	// asynchronous requests.

	switch (aIpc)
		{
	case EMobileCallNotifyMobileCallStatusChange:
		return NotifyMobileCallStatusChangeCancel(aTsyReqHandle);
	case EMobileCallNotifyMobileCallCapsChange:
		return NotifyMobileCallCapsChangeCancel(aTsyReqHandle);
	default:
		return CCallBase::CancelService(aIpc,aTsyReqHandle);
		}
	}

TInt CCallMobile::GetMobileCallCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
	{
	LOGTEXT(_L8("CCallMobile:	:GetMobileCallCaps called"));
	
	RMobileCall::TMobileCallCapsV1Pckg *capsPckg = REINTERPRET_CAST(RMobileCall::TMobileCallCapsV1Pckg *,aCaps);
	RMobileCall::TMobileCallCapsV1& caps = (*capsPckg)();
	// Always returns KErrNone
	(void)CollateCurrentMobileCaps(aTsyReqHandle, &caps.iCallControlCaps);
	caps.iCallEventCaps=0;  // no call events are supported

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallMobile::GetCaps(const TTsyReqHandle aTsyReqHandle,RCall::TCaps* aCallCaps)
	{
	(void)CollateCurrentCoreCaps(aTsyReqHandle, reinterpret_cast<TUint32*>(&aCallCaps->iFlags));
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TBool CCallMobile::CollateCurrentMobileCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCallCaps)
	{
	CollateCoreCaps(aTsyReqHandle, aCallCaps);
	if (iMobileCaps.iCallControlCaps != *aCallCaps)
		{
		iMobileCaps.iCallControlCaps = *aCallCaps;
		return ETrue;
		}
	return EFalse;
	}

TBool CCallMobile::CollateCurrentCoreCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCallCaps)
	{
	CollateCoreCaps(aTsyReqHandle, aCallCaps);
	if (iCaps.iFlags != *aCallCaps)
		{
		iCaps.iFlags = *aCallCaps;
		return ETrue;
		}
	return EFalse;
	}

TInt CCallMobile::NotifyMobileCallCapsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
	{
	LOGTEXT(_L8("CCallMobile::NotifyMobileCallCapsChange lodged"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(ECallMobileCaps,aTsyReqHandle,this,aCaps);
	return KErrNone;
	}

TInt CCallMobile::NotifyMobileCallCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallMobile::NotifyMobileCallCapsChangeCancel called"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}
	
TInt CCallMobile::GetMobileCallStatus(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus)
	{
	LOGTEXT(_L8("CCallMobile::GetMobileCallStatus called"));
	*aStatus=iCallInfo.iMobileStatus;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallMobile::NotifyMobileCallStatusChange(const TTsyReqHandle aTsyReqHandle,RMobileCall::TMobileCallStatus* aStatus)
	{
	LOGTEXT(_L8("CCallMobile::NotifyMobileCallStatusChange lodged"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(EMobileCallStatusChange,aTsyReqHandle,this,aStatus);
	return KErrNone;
	}

TInt CCallMobile::NotifyMobileCallStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CCallMobile::NotifyMobileCallStatusChangeCancel called"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}

//
// CCallMobileVoice - Voice Specific Call Functionality
//

CCallMobileVoice* CCallMobileVoice::NewL(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals,const TName& aName)
	{
	CCallMobileVoice* voiceCall=new(ELeave) CCallMobileVoice(aATIO,aInit,aPhoneGlobals);
	TCleanupItem newCallVoiceHayesClose(CloseCall,voiceCall);
	CleanupStack::PushL(newCallVoiceHayesClose);
	voiceCall->ConstructL(aName);
	CleanupStack::Pop();
	return voiceCall;
	}

CCallMobileVoice::CCallMobileVoice(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	: CCallMobile(aATIO,aInit,aPhoneGlobals)
	{}

void CCallMobileVoice::ConstructL(const TName& aName)
	{
	CCallHayes::ConstructL(aName);
	iCallInfo.iLineOwnerName = KVoiceLineName;
	iDialVoice=CATDialVoice::NewL(iIo,this,iInit,iPhoneGlobals);
	iAnswerVoice=CATAnswerVoice::NewL(iIo,this,iInit,iPhoneGlobals);
	iHangUpVoice=CATHangUpVoice::NewL(iIo,this,iInit,iPhoneGlobals);
	}

CCallMobileVoice::~CCallMobileVoice()
//
//	Removes itself from array of calls in CLineMobileVoice
//
	{
	delete iDialVoice;
	delete iAnswerVoice;
	delete iHangUpVoice;
	}

void CCallMobileVoice::CollateCoreCaps(const TTsyReqHandle /*aTsyReqHandle*/, TUint32* aCallCaps)
	{
	*aCallCaps = RCall::KCapsVoice;

	// Voice calls can be manipulated by any client, not just owner so set as many caps as possible
	if (iPhoneGlobals->iPhoneStatus.iModemDetected==RPhone::EDetectedPresent)
		{
		*aCallCaps |= (RCall::KCapsDial | RCall::KCapsConnect);

		TInt ret=KErrNone;
		if (!iIsForIncomingCall)	
		{
			if (REINTERPRET_CAST(CPhoneHayes*,Owner()->Owner())->CheckForOutstandingAnswer())
				ret=KErrEtelAnswerAlreadyOutstanding;
		}
		else
			ret=KErrEtelAnswerAlreadyOutstanding;
		
		if (ret==KErrNone && (iCallInfo.iMobileStatus==RMobileCall::EStatusIdle || iCallInfo.iMobileStatus==RMobileCall::EStatusRinging))
			*aCallCaps |= RCall::KCapsAnswer;

		if (iCallInfo.iMobileStatus==RMobileCall::EStatusConnected)
			*aCallCaps |= RCall::KCapsHangUp;
		}
	}

TInt CCallMobileVoice::Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams,TDesC* aTelNumber)
//
//	Dial a voice call
//
	{
	SetCallParams(aCallParams);
	LOGTEXT(_L8("VoiceCall:\tSubmitting Dial command for Voice call"));
	if (iPhoneGlobals->iPhoneStatus.iLineStatus == RCall::EStatusIdle)	
		{
		(void)SetOwnership(aTsyReqHandle);
		iDialVoice->ExecuteCommand(aTsyReqHandle,aTelNumber,&iCallInfo);
		return KErrNone;
		}
	else 
		{
		return KErrInUse;
		}
	}

TInt CCallMobileVoice::DialCancel(const TTsyReqHandle aTsyReqHandle)
/**
 *	Cancel the dial request if possible
 */
	{
	LOGTEXT(_L8("VoiceCall:\tSubmitting Cancel Dial command for Voice call"));
	iDialVoice->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}

TInt CCallMobileVoice::AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams)
/**
 *	If there is no waiting call, will wait for one and then answer it. If there is, it will
 *	answer immediately.
 */
 	{
	CCallBase::TCallOwnership owned = CheckOwnership(aTsyReqHandle);
	TInt ret=KErrNone;
	if (owned==CCallBase::EOwnedFalse)	// call owned by another client
		{
		ret=KErrEtelNotCallOwner;
		}
	else 
		{
		if (!iIsForIncomingCall)	
			{
			if (REINTERPRET_CAST(CPhoneHayes*,Owner()->Owner())->CheckForOutstandingAnswer())
				ret=KErrEtelAnswerAlreadyOutstanding;
			}
		else
			ret=KErrEtelAnswerAlreadyOutstanding;
		}
	if (ret==KErrNone)
		{
		CLineHayes* line = STATIC_CAST(CLineHayes*,Owner());
		STATIC_CAST(CPhoneHayes*,line->Owner())->CancelOtherRingingCall(line);
		line->FreePreAllocCallIfNecessary();
		SetCallParams(aCallParams);
		if (iCallInfo.iMobileStatus==RMobileCall::EStatusRinging)
			{
			(void)SetOwnership(aTsyReqHandle);
			LOGTEXT(_L8("VoiceCall:\tSubmitting Answer command"));
			iAnswerVoice->ExecuteCommand(aTsyReqHandle,NULL,&iCallInfo);
			}
		else	// This call is now a client-designated Incoming Call object.
			{
			iIsForIncomingCall=ETrue;
			iAnswerTsyReqHandle = aTsyReqHandle;
			}
		return KErrNone;
		}
	ReqCompleted(aTsyReqHandle,ret);
	return KErrNone;
	}


TInt CCallMobileVoice::AnswerIncomingCallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("VoiceCall:\tSubmitting Cancel Answer command"));
	if (iIsForIncomingCall)
		{
		iIsForIncomingCall=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		iAnswerVoice->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}

void CCallMobileVoice::AnswerImmediately()
	{
	(void)SetOwnership(iAnswerTsyReqHandle);
	// EStatusRinging always results in KErrNone return value
	(void) ChangeCallStatus(RMobileCall::EStatusRinging);
	iPhoneGlobals->iNotificationStore->CheckNotification(this,ERingOccurred);
	iIsForIncomingCall=EFalse;
	// this must be after the check notification as CNotifications
	// asks line for info about the call which Is For Incoming Call
	LOGTEXT(_L8("VoiceCall:\tSubmitting Answer command"));
	iAnswerVoice->ExecuteCommand(iAnswerTsyReqHandle,NULL,&iCallInfo);
	}		

TInt CCallMobileVoice::Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8* /*aCallParams*/)
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CCallMobileVoice::ConnectCancel(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CCallMobileVoice::HangUp(const TTsyReqHandle aTsyReqHandle)
	{
	if (CheckOwnership(aTsyReqHandle)==CCallBase::EOwnedFalse)
		{
		ReqCompleted(aTsyReqHandle,KErrEtelNotCallOwner);
		return KErrNone;
		}	
	if ((iCallInfo.iMobileStatus!=RMobileCall::EStatusIdle) &&
		(iCallInfo.iMobileStatus!=RMobileCall::EStatusDisconnecting))
		{
		LOGTEXT(_L8("VoiceCall:\tSubmitting Hang Up command"));
		iHangUpVoice->ExecuteCommand(aTsyReqHandle,NULL,&iCallInfo);
		return KErrNone;
		}
	else
		{
		// Call not in a state to be terminated - but return KErrNone
		ReqCompleted(aTsyReqHandle,KErrNone);
		return KErrNone;
		}
	}

TInt CCallMobileVoice::HangUpCancel(const TTsyReqHandle aTsyReqHandle)
//
//	Cancel the hang up command. 
//
	{
	LOGTEXT(_L8("VoiceCall:\tSubmitting Cancel Hang Up command"));
	iHangUpVoice->Stop(aTsyReqHandle);	
	return KErrNone;
	}

TInt CCallMobileVoice::RelinquishOwnership()
	{
//
//	Called by server to tell TSY to either pass ownership on to another interested client
//	or hang up immediately. If call is currently connecting, RelinquishOwnershipComplete 
//	will be called once cancelling has finished.
//
	LOGTEXT(_L8("VoiceCall:\tRelinquish Ownership"));
	if(iList->iAcquireList.IsEmpty()) 
		{
		if (iDialVoice->IsPreConnectInProgress())
			{
			iCallInfo.iClientPanicOccurred = EPanicOccurredWithoutDataPortLoan;
			return KErrNone;
			}
		switch(iCallInfo.iMobileStatus)
			{
		case RMobileCall::EStatusConnected:
			iCallInfo.iClientPanicOccurred = EPanicOccurredWithoutDataPortLoan;
			iHangUpVoice->ExecuteCommand(0, NULL, &iCallInfo);
			break;
		case RMobileCall::EStatusDialling:
		case RMobileCall::EStatusConnecting:
		case RMobileCall::EStatusAnswering:
		case RMobileCall::EStatusDisconnecting:
			iCallInfo.iClientPanicOccurred = EPanicOccurredWithoutDataPortLoan;
			break;
		default:
			if (iPhoneGlobals->iPhoneStatus.iInitStatus==EPhoneInitialising)
				iCallInfo.iClientPanicOccurred = EPanicOccurredWithoutDataPortLoan;
			else
				RelinquishOwnershipCompleted(KErrNone);
			break;
			}
		return KErrNone;
		}
	CAcquireEntry* entry=iList->iAcquireList.First();
	if (entry) 
		{
		(void)SetOwnership(entry->iTsyReqHandle);
		ReqCompleted(entry->iTsyReqHandle,KErrNone);
		iList->Remove(entry);
		}
	RelinquishOwnershipCompleted(KErrNone);
	return KErrNone;
	}

TInt CCallMobileVoice::RecoverDataPortAndRelinquishOwnership()
	{
	RecoverDataPortAndRelinquishOwnershipCompleted(KErrNotSupported);
	return KErrNone;
	}

TInt CCallMobileVoice::LoanDataPort(const TTsyReqHandle aTsyReqHandle, RCall::TCommPort* /* aCommPort*/)
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CCallMobileVoice::LoanDataPortCancel(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CCallMobileVoice::RecoverDataPort(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}


//
//
// CCallMobileData - Data Specific Call Functionality
//
//

CCallMobileData* CCallMobileData::NewL(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals,const TName& aName)
	{
	CCallMobileData* dataCall=new(ELeave) CCallMobileData(aATIO,aInit,aPhoneGlobals);
	TCleanupItem newCallDataHayesClose(CloseCall,dataCall);
	CleanupStack::PushL(newCallDataHayesClose);
	dataCall->ConstructL(aName);
	CleanupStack::Pop();
	return dataCall;
	}

CCallMobileData::CCallMobileData(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	: CCallMobile(aATIO,aInit,aPhoneGlobals)
	{}

void CCallMobileData::ConstructL(const TName& aName)
	{
	CCallHayes::ConstructL(aName);
	iCallInfo.iLineOwnerName = KDataLineName;
	iDialData=CATDialData::NewL(iIo,this,iInit,iPhoneGlobals);
	iAnswerData=CATAnswerData::NewL(iIo,this,iInit,iPhoneGlobals);
	iSetCBST=CATSetCBST::NewL(iIo,this,iInit,iPhoneGlobals);
	iConnectData=CATConnectData::NewL(iIo,this,iInit,iPhoneGlobals);
	iHangUpData=CATHangUpData::NewL(iIo,this,iInit,iPhoneGlobals);
	iATSetToOnlineDataMode=CATSetToOnlineDataMode::NewL(iIo,this,iInit,iPhoneGlobals);
	}

CCallMobileData::~CCallMobileData()
//
//	Removes itself from array of calls in CLineMobileData
//
	{
	delete iDialData;
	delete iSetCBST;
	delete iAnswerData;
	delete iConnectData;
	delete iHangUpData;
	delete iATSetToOnlineDataMode;
	}

TInt CCallMobileData::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
					 		  const TDataPackage& aPackage)
/**
 * Process the IPC locally, otherwise delegate to our base class.
 */	
	{
	switch (aIpc)
		{
	case EMobileCallGetMobileDataCallCaps: 
		return GetMobileDataCallCaps(aTsyReqHandle, aPackage.Des1n());
	default:
		// Delegate the processing of the IPC to our base class
		return CCallMobile::ExtFunc(aTsyReqHandle,aIpc,aPackage);		
		}
	}


TInt CCallMobileData::GetMobileDataCallCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
	{
	LOGTEXT(_L8("CCallMobileData::GetMobileDataCallCaps called"));

	//
	// This is a synchronous client request so this method must complete quickly.
	// The call caps returned are those which were discovered during the 
	// intialisation of the TSY (see ATINIT.CPP).
	TPckg<RMobileCall::TMobileCallDataCapsV1>* pckg = (TPckg<RMobileCall::TMobileCallDataCapsV1>*)aCaps;
	RMobileCall::TMobileCallDataCapsV1& caps = (*pckg)();
	caps=iPhoneGlobals->iCallDataCaps;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

void CCallMobileData::CollateCoreCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCallCaps)
	{
	*aCallCaps = RCall::KCapsData;
	if (iPhoneGlobals->iPhoneStatus.iModemDetected==RPhone::EDetectedPresent)
		{
		CCallBase::TCallOwnership owner = CheckOwnership(aTsyReqHandle);
		if (ValidateRequest(aTsyReqHandle,RCall::EStatusIdle)==KErrNone)
			*aCallCaps |= (RCall::KCapsDial | RCall::KCapsConnect);
		TInt ret=KErrNone;
		if (owner==CCallBase::EOwnedFalse)	// call owned by another client
			ret=KErrEtelNotCallOwner;
		else 
			{
			if (!iIsForIncomingCall)	
				{
				if (REINTERPRET_CAST(CPhoneHayes*,Owner()->Owner())->CheckForOutstandingAnswer())
					ret=KErrEtelAnswerAlreadyOutstanding;
				}
			else
				ret=KErrEtelAnswerAlreadyOutstanding;
			}
		if (ret==KErrNone && (iCallInfo.iMobileStatus==RMobileCall::EStatusIdle || iCallInfo.iMobileStatus==RMobileCall::EStatusRinging))
			*aCallCaps |= RCall::KCapsAnswer;
		if ((owner==CCallBase::EOwnedTrue || owner==CCallBase::EOwnedPriorityClient)
			&& iCallInfo.iMobileStatus==RMobileCall::EStatusConnected)
			{
			*aCallCaps |= RCall::KCapsHangUp;
			if (iCallInfo.iLoanedToClient==EFalse)
				*aCallCaps |= RCall::KCapsLoanDataPort;
			else
				*aCallCaps |= RCall::KCapsRecoverDataPort;
			}
		}
	}

TInt CCallMobileData::AssembleCBSTSetString(TBuf8<KGenericBufferSize>& aTxBuffer)
/**
 * This funciton assembles a +CBST= string ready to be sent to the modem using 
 * the values from CCallHayes::iCallInfo.
 * This utility funciton is used by objects created and owned by this class.
 * This has been done to reduce code size.
 * @return KErrNone if and only if CBST string assembled
 */
	{
	TInt ret(KErrNone);		// Return value of this function
	
	TInt speed(0); // these are the names that etsi use
	TInt name(0);  // these are the names that etsi use
	TInt ce(0); // these are the names that etsi use
	TBool speedSet(ETrue);
	TBool nameSet(ETrue);
	TBool ceSet(ETrue);

	switch (iCallInfo.iCallParams.iService)
		{
		case RMobileCall::EServiceDataCircuitAsync:
			name = 0;
			break;
		case RMobileCall::EServiceDataCircuitAsyncRdi:
			name = 4;
			break;
		case RMobileCall::EServiceDataCircuitSync:
			name = 1;
			break;
		case RMobileCall::EServiceDataCircuitSyncRdi:
			name = 5;
			break;
		case RMobileCall::EServicePADAsyncUDI:
			name = 2;
			break;
		case RMobileCall::EServicePADAsyncRDI:
			name = 6;
			break;
		case RMobileCall::EServicePacketAccessSyncUDI:
			name = 3;
			break;
		case RMobileCall::EServicePacketAccessSyncRDI:
			name = 7;
			break;
		case RMobileCall::EServiceUnspecified:  // taking the default value of 0
		default:
			nameSet=EFalse;
			break;
		}

	switch(iCallInfo.iCallParams.iQoS)
		{
		case RMobileCall::EQoSTransparent:
			ce = 0;
			break;
		case RMobileCall::EQosTransparentPreferred:
			ce = 2;
			break;
		case RMobileCall::EQoSNonTransparent:
			ce = 1;
			break;
		case RMobileCall::EQosNonTransparentPreferred:
			ce = 3;
			break;
		case RMobileCall::EQoSUnspecified:
		default:
			ceSet=EFalse;
			break;
		}

	switch(iCallInfo.iCallParams.iSpeed)
		{
		case RMobileCall::ESpeedAutobauding:
			speed = 0;
			break;

		case RMobileCall::ESpeed2400:
			{
			if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV22bis)
				speed = 4;
			else if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV120)
				speed = 36;
			else if ((iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV110) || 
				(iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolX31FlagStuffing))
				speed = 68;
			else
				speedSet=EFalse;
			break;
			}
		case RMobileCall::ESpeed4800:
			{
			if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV32)
				speed = 6;
			else if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV120)
				speed = 38;
			else if ((iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV110) || 
				(iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolX31FlagStuffing))
				speed = 70;
			else
				speedSet=EFalse;
			break;
			}
		case RMobileCall::ESpeed9600:
			{
			if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV32)
				speed = 7;
			else if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV34)
				speed = 12;
			else if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV120)
				speed = 39;
			else if ((iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV110) || 
				(iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolX31FlagStuffing))
				speed = 71;
			else
				speedSet=EFalse;
			break;
			}
		case RMobileCall::ESpeed14400:
			{
			if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV34)
				speed = 14;
			else if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV120)
				speed = 43;
			else if ((iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV110) || 
				(iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolX31FlagStuffing))
				speed = 75;
			else
				speedSet=EFalse;
			break;
			}
		case RMobileCall::ESpeed19200:
			{
			if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV34)
				speed = 15;
			else if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV120)
				speed = 47;
			else if ((iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV110) || 
				(iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolX31FlagStuffing))
				speed = 79;
			else
				speedSet=EFalse;
			break;
			}
		case RMobileCall::ESpeed28800:
			{
			if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV34)
				speed = 16;
			else if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV120)
				speed = 48;
			else if ((iCallInfo.iCallParams.iProtocol ==RMobileCall:: EProtocolV110) || 
				(iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolX31FlagStuffing))
				speed = 80;
			else
				speedSet=EFalse;
			break;
			}
		case RMobileCall::ESpeed38400:
			{
			if ((iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV110) || 
				(iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolX31FlagStuffing))
				speed = 81;
			else if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV120)
				speed = 49;
			else
				speedSet=EFalse;
			break;
			}
		case RMobileCall::ESpeed48000:
			if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV120)
				speed = 50;
			else if ((iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV110) || 
				(iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolX31FlagStuffing))
				speed = 82;
			else
				speedSet=EFalse;
			break;
		case RMobileCall::ESpeed56000:
			if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV120)
				speed = 51;
			else if ((iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolV110) || 
				(iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolX31FlagStuffing))
				speed = 83;
			else if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolBitTransparent)
				speed = 115;
			else
				speedSet=EFalse;
			break;
		case RMobileCall::ESpeed64000:
			if (iCallInfo.iCallParams.iProtocol == RMobileCall::EProtocolBitTransparent)
				speed = 116;
			else
				speedSet=EFalse;
			break;

		case RMobileCall::ESpeed33600:  // this baud rate is not currently used in the etsi table
		case RMobileCall::ESpeedUnspecified:
		default:
			speedSet=EFalse;
			break;
		}

	//
	// Try to assemble as much of the +CBST= command as we can.
	// If we do not have a <speed> value we can not send the command.
	// If we do not have a <ce> value then we can send a command giving only <speed> and <name>.
	// etc...
	if(speedSet)
		{
		aTxBuffer.Zero();		// Clear tx buffer

		// Append +CBST= and the <speed> parameter
		aTxBuffer.Append(KDataCallBearerServiceType);
		aTxBuffer.AppendNum(speed);
		
		if(nameSet)
			{
			// Append the <name> parameter
			aTxBuffer.Append(TChar(',')); 
			aTxBuffer.AppendNum(name);

			if(ceSet)
				{
				// Append the <ce> parameter
				aTxBuffer.Append(TChar(',')); 
				aTxBuffer.AppendNum(ce); 
				}
			}

		// Append final \r char to AT command
		aTxBuffer.Append(TChar('\r'));
		}
	else
		{
		// We have not have enough info to create a +CBST= string 
		ret=KErrArgument;
		}

	return ret;
	}



TInt CCallMobileData::Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams,TDesC* aTelNumber)
//
//	Dial a data call
//
	{
	TInt ret(KErrNone);

	SetDataCallParams(aCallParams);	

	ret=ValidateRequest(aTsyReqHandle,RCall::EStatusIdle);
	if (ret==KErrNone)
		{
		(void)SetOwnership(aTsyReqHandle);
		LOGTEXT(_L8("DataCall:\tSubmitting Dial command for Data call"));
		iDialData->ExecuteCommand(aTsyReqHandle,aTelNumber,&iCallInfo);
		}
	else
		ReqCompleted(aTsyReqHandle,ret);		// An error occurred

	return KErrNone;	// Real return code is returned to client thru request completion
	}


TInt CCallMobileData::DialCancel(const TTsyReqHandle aTsyReqHandle)
//
//	Cancel the dial request if possible
//
	{
	LOGTEXT(_L8("DataCall:\tSubmitting Cancel Dial command for Data call"));
	iDialData->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}


void CCallMobileData::SetDataCallParams(const TDesC8* aCallParams)
/**
 * Utility function which unpacks the call parameters supplied by the client
 * and saves their values in iPhoneGlobals.
 * If the client did not supply parameters then default ones are used.
 */
	{
	if(aCallParams->Length()==0)
		{
		//
		// Client did not supply parameters, so we use default ones
		LOGTEXT(_L8("CCallMobileData::SetDataCallParams  Using default parameters"));
		CCallHayes::SetCallParams(aCallParams); 

		//
		// Ensure the speed parameter is undefined so that we prevent the
		// +CBST=... command being sent to the phone when dialling a CSD data call.
		iCallInfo.iCallParams.iService = RMobileCall::EServiceUnspecified;
		}
	else
		{
		//
		// Use call parameters that client supplied us
		LOGTEXT(_L8("CCallMobileData::SetDataCallParams  Using clients parameters"));
		RCall::TCallParamsPckg* paramsPckgV1 = (RCall::TCallParamsPckg*)aCallParams;
		RCall::TCallParams& paramsV1 = (*paramsPckgV1)();

		CCallHayes::SetCallParams(aCallParams); 
		if (paramsV1.ExtensionId()==RMobileCall::KETelMobileCallParamsV1)
			{
			RMobileCall::TMobileCallParamsV1Pckg* mmParamsPckgV1 = (RMobileCall::TMobileCallParamsV1Pckg*)aCallParams;
			RMobileCall::TMobileCallParamsV1& mmParamsV1 = (*mmParamsPckgV1)();
			iCallInfo.iCallParams.iIdRestrict = mmParamsV1.iIdRestrict;
			iCallInfo.iCallParams.iCug = mmParamsV1.iCug;
			iCallInfo.iCallParams.iAutoRedial = mmParamsV1.iAutoRedial;
			}
		
		if((paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV1) ||
		   (paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV1))
			{
			RMobileCall::TMobileDataCallParamsV1Pckg* dataParamsPckgV1 = (RMobileCall::TMobileDataCallParamsV1Pckg*)aCallParams;
			RMobileCall::TMobileDataCallParamsV1& dataParamsV1 = (*dataParamsPckgV1)();	
			iCallInfo.iCallParams.iService=dataParamsV1.iService;
			iCallInfo.iCallParams.iSpeed=dataParamsV1.iSpeed;
			iCallInfo.iCallParams.iProtocol=dataParamsV1.iProtocol;
			iCallInfo.iCallParams.iQoS=dataParamsV1.iQoS;
			iCallInfo.iCallParams.iRLPVersion=dataParamsV1.iRLPVersion;
			iCallInfo.iCallParams.iModemToMSWindowSize=dataParamsV1.iModemToMSWindowSize;
			iCallInfo.iCallParams.iMSToModemWindowSize=dataParamsV1.iMSToModemWindowSize;
			iCallInfo.iCallParams.iAckTimer=dataParamsV1.iAckTimer;
			iCallInfo.iCallParams.iRetransmissionAttempts=dataParamsV1.iRetransmissionAttempts;
			iCallInfo.iCallParams.iResequencingPeriod=dataParamsV1.iResequencingPeriod;
			iCallInfo.iCallParams.iV42bisReq=dataParamsV1.iV42bisReq;
			iCallInfo.iCallParams.iV42bisCodewordsNum=dataParamsV1.iV42bisCodewordsNum;
			iCallInfo.iCallParams.iV42bisMaxStringLength=dataParamsV1.iV42bisMaxStringLength;
			}

		if (paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV1)
			{
			RMobileCall::TMobileHscsdCallParamsV1Pckg* hscsdParamsPckgV1 = (RMobileCall::TMobileHscsdCallParamsV1Pckg*)aCallParams;
			RMobileCall::TMobileHscsdCallParamsV1& hscsdParamsV1 = (*hscsdParamsPckgV1)();
			iCallInfo.iCallParams.iWantedAiur = hscsdParamsV1.iWantedAiur;
			iCallInfo.iCallParams.iWantedRxTimeSlots = hscsdParamsV1.iWantedRxTimeSlots;
			iCallInfo.iCallParams.iMaxTimeSlots = hscsdParamsV1.iMaxTimeSlots;
			iCallInfo.iCallParams.iCodings = hscsdParamsV1.iCodings;
			iCallInfo.iCallParams.iAsymmetry = hscsdParamsV1.iAsymmetry;
			iCallInfo.iCallParams.iUserInitUpgrade = hscsdParamsV1.iUserInitUpgrade;
			}
		}
	}

TInt CCallMobileData::AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams)
/**
 * If there is no waiting call, will wait for one and then answer it.
 * If there is a waiting call, will answer immediatly.
 * Regardless of if there is a waiting call or not, the +CBST= command is sent immediatly.
 */
	{
	const CCallBase::TCallOwnership owned = CheckOwnership(aTsyReqHandle);
	TInt ret=KErrNone;
	if (owned==CCallBase::EOwnedFalse)	// call owned by another client
		{
		ret=KErrEtelNotCallOwner;
		}
	else 
		{
		if (!iIsForIncomingCall)	
			{
			if (REINTERPRET_CAST(CPhoneHayes*,Owner()->Owner())->CheckForOutstandingAnswer())
				ret=KErrEtelAnswerAlreadyOutstanding;
			}
		else
			ret=KErrEtelAnswerAlreadyOutstanding;
		}

	if (ret==KErrNone)
		{
		CLineHayes* line = STATIC_CAST(CLineHayes*,Owner());
		STATIC_CAST(CPhoneHayes*,line->Owner())->CancelOtherRingingCall(line);
		line->FreePreAllocCallIfNecessary();
		SetDataCallParams(aCallParams);	
		if (iCallInfo.iMobileStatus==RMobileCall::EStatusRinging)
			{
			(void)SetOwnership(aTsyReqHandle);
			LOGTEXT(_L8("DataCall:\tSubmitting Answer command"));

			// Start the 'send CBST to phone' AT machine and pass the 
			// CATAnswerData object, so that the call is answered after 
			// the CBST is sent.
			iSetCBST->ExecuteCommand(aTsyReqHandle,iAnswerData,&iCallInfo);
			}
		else	// This call is now a client-designated Incoming Call object.
			{
			iIsForIncomingCall=ETrue;
			iAnswerTsyReqHandle = aTsyReqHandle;

			// Start the 'send CBST to phone' AT machine. This state machine 
			// will not complete any client request.
			iSetCBST->ExecuteCommand(aTsyReqHandle,NULL,&iCallInfo);
			}
		return KErrNone;
		}

	ReqCompleted(aTsyReqHandle,ret);
	return KErrNone;
	}


TInt CCallMobileData::AnswerIncomingCallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("DataCall:\tSubmitting Cancel Answer command"));
	if (iIsForIncomingCall)
		{
		iIsForIncomingCall=EFalse;
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		iAnswerData->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}

void CCallMobileData::AnswerImmediately()
	{
	(void)SetOwnership(iAnswerTsyReqHandle);
	// EStatusRinging always results in KErrNone return
	(void) ChangeCallStatus(RMobileCall::EStatusRinging);	// new 14/1/99
	iPhoneGlobals->iNotificationStore->CheckNotification(this,ERingOccurred);
	iIsForIncomingCall=EFalse;	// this must be after the check notification as CNotifications
								// asks line for info about the call which Is For Incoming Call
	LOGTEXT(_L8("DataCall:\tSubmitting Answer command"));
	iAnswerData->ExecuteCommand(iAnswerTsyReqHandle,NULL,&iCallInfo);
	}		

TInt CCallMobileData::Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams)
//
//	Connect to an already dialled call
//
	{
	TInt ret = ValidateRequest(aTsyReqHandle,RCall::EStatusIdle);
	if (ret==KErrNone)
		{
		(void)SetOwnership(aTsyReqHandle);
		SetCallParams(aCallParams);
		LOGTEXT(_L8("DataCall:\tSubmitting Immediate Connect command"));
		iConnectData->ExecuteCommand(aTsyReqHandle,NULL,&iCallInfo);
		}
	else
		ReqCompleted(aTsyReqHandle,ret);
	return KErrNone;
	}

TInt CCallMobileData::ConnectCancel(const TTsyReqHandle aTsyReqHandle)
//
//	Cancel immediate connect command
//
	{
	LOGTEXT(_L8("DataCall:\tSubmitting Cancel Connect command"));
	iConnectData->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}

TInt CCallMobileData::HangUp(const TTsyReqHandle aTsyReqHandle)
//
//	Terminate a data call. If someone else owns call complete with error, but if no-one owns
//	it send an ATH anyway, as the connection may have failed and the user wants to ensure
//	call has hung up. Call Start() instead of ExecuteCommand() because there is no need to
//	initialise before doing this or to send the escape sequence - the hang up sequence drops
//	DTR anyway.
//
	{
	if (CheckOwnership(aTsyReqHandle)==CCallBase::EOwnedFalse)
		{
		ReqCompleted(aTsyReqHandle,KErrEtelNotCallOwner);
		return KErrNone;
		}	
	if ((iCallInfo.iMobileStatus!=RMobileCall::EStatusIdle) &&
		(iCallInfo.iMobileStatus!=RMobileCall::EStatusDisconnecting))
		{
		LOGTEXT(_L8("DataCall:\tSubmitting Hang Up command"));
		iHangUpData->ExecuteCommand(aTsyReqHandle,NULL,&iCallInfo);
		return KErrNone;
		}
	else
		{
		// Call not in a state to be terminated - but return KErrNone
		ReqCompleted(aTsyReqHandle,KErrNone);
		return KErrNone;
		}
	}

TInt CCallMobileData::HangUpCancel(const TTsyReqHandle aTsyReqHandle)
//
//	Cancel the hang up command. 
//
	{
	LOGTEXT(_L8("DataCall:\tSubmitting Cancel Hang Up command"));
	iHangUpData->Stop(aTsyReqHandle);	
	return KErrNone;
	}

TInt CCallMobileData::RelinquishOwnership()
//
//	Called by server to tell TSY to either pass ownership on to another interested client
//	or hang up immediately. If call is currently connecting, RelinquishOwnershipComplete 
//	will be called once cancelling has finished.
//
	{
	LOGTEXT(_L8("DataCall:\tRelinquish Ownership"));
	if(iList->iAcquireList.IsEmpty()) 
		{
		if (iDialData->IsPreConnectInProgress() ||
			iConnectData->IsPreConnectInProgress() ||
			iAnswerData->IsPreConnectInProgress())
			{
			iCallInfo.iClientPanicOccurred = EPanicOccurredWithoutDataPortLoan;
			return KErrNone;
			}
		switch(iCallInfo.iMobileStatus)
			{
		case RMobileCall::EStatusConnected:
			iCallInfo.iClientPanicOccurred = EPanicOccurredWithoutDataPortLoan;
			//iHangUpData->ExecuteCommand(0,NULL,&iCallInfo);
			iQuickInit->CancelAndHangUp(&iCallInfo,iHangUpData);
			break;
		case RMobileCall::EStatusDialling:
		case RMobileCall::EStatusConnecting:
		case RMobileCall::EStatusAnswering:
		case RMobileCall::EStatusDisconnecting:
			iCallInfo.iClientPanicOccurred = EPanicOccurredWithoutDataPortLoan;
			break;
		default:
			if (iPhoneGlobals->iPhoneStatus.iInitStatus==EPhoneInitialising)
				iCallInfo.iClientPanicOccurred = EPanicOccurredWithoutDataPortLoan;
			else
				RelinquishOwnershipCompleted(KErrNone);
			break;
			}
		return KErrNone;
		}
	CAcquireEntry* entry=iList->iAcquireList.First();
	if (entry) 
		{
		(void)SetOwnership(entry->iTsyReqHandle);
		ReqCompleted(entry->iTsyReqHandle,KErrNone);
		iList->Remove(entry);
		}
	RelinquishOwnershipCompleted(KErrNone);
	return KErrNone;
	}

TInt CCallMobileData::RecoverDataPortAndRelinquishOwnership()
//
//	Called by server to tell TSY to either pass ownership on to another interested client
//	after regaining control of the port and performing a swift initialisation,
//	or hang up immediately
//
	{
	LOGTEXT(_L8("DataPort Recover and Relinquish Ownership"));
	iCallInfo.iLoanedToClient=EFalse;
	iPhoneGlobals->iPhoneStatus.iDataPortLoaned = EFalse;
	if(iList->iAcquireList.IsEmpty())
		{
		(void)SetUnowned();
		iCallInfo.iClientPanicOccurred = EPanicOccurredDuringDataPortLoan;
		iIo->Read();
		iHangUpData->ExecuteCommand(0,NULL,&iCallInfo);
		return KErrNone;
		}
	CAcquireEntry* entry=iList->iAcquireList.First();
	if (entry) 
		{
		(void)SetOwnership(entry->iTsyReqHandle);
		ReqCompleted(entry->iTsyReqHandle,KErrNone);
		iList->Remove(entry);
		iQuickInit->StartQuickInit();
		}
	RecoverDataPortAndRelinquishOwnershipCompleted(KErrNone);
	return KErrNone;
	}

TInt CCallMobileData::LoanDataPort(const TTsyReqHandle aTsyReqHandle, RCall::TCommPort* aCommPort)
//
//	Get the port info
//
	{
	LOGTEXT(_L8(" DataPort Loan"));
	if (CheckOwnership(aTsyReqHandle)!=CCallBase::EOwnedTrue)
		{
		ReqCompleted(aTsyReqHandle,KErrEtelNotCallOwner);
		return KErrNone;
		}	
	if (iCallInfo.iMobileStatus != RMobileCall::EStatusConnected)
		{
		ReqCompleted(aTsyReqHandle,KErrEtelCallNotActive);
		return KErrNone;	
		}
	TInt ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameCsyName),aCommPort->iCsy);
	if (!ret)
		{
		ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNamePortName),aCommPort->iPort);
		}
	if (ret)
		{
		ReqCompleted(aTsyReqHandle,ret);
		return KErrNone;
		}
	iCallInfo.iLoanedToClient = ETrue;
	iPhoneGlobals->iPhoneStatus.iDataPortLoaned = ETrue;
	iPhoneGlobals->iNotificationStore->CheckNotification(this,EDataPortLoaned);
	iWaitForNoCarrier->StopWait();
	if (iPhoneGlobals->iPhoneStatus.iMode==RPhone::EModeOnlineCommand)
		iATSetToOnlineDataMode->ExecuteCommand(aTsyReqHandle,NULL,&iCallInfo);
	else
		{
		iIo->Cancel();
		TCommConfig aConfigPckg;
		TInt ret = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeFull);
		if (ret==KErrNone)
			ret = iIo->ConfigurePort(aConfigPckg);
		ReqCompleted(aTsyReqHandle,ret);
		}
	return KErrNone;
	}

TInt CCallMobileData::LoanDataPortCancel(const TTsyReqHandle aTsyReqHandle)
	{
	iATSetToOnlineDataMode->CancelCommand(aTsyReqHandle);
	return KErrNone;
	}

TInt CCallMobileData::RecoverDataPort(const TTsyReqHandle aTsyReqHandle)
//
//	Take back control of data port
//	Check that call had been loaned to client
//
	{
	if (iCallInfo.iLoanedToClient==EFalse)
		{
		ReqCompleted(aTsyReqHandle,KErrEtelPortNotLoanedToClient);	
		return KErrNone;
		}	
	TCallOwnership owner = CheckOwnership(aTsyReqHandle);
	if (owner!=CCallBase::EOwnedTrue && owner!=CCallBase::EOwnedPriorityClient)
		{
		ReqCompleted(aTsyReqHandle,KErrEtelNotCallOwner);
		return KErrNone;
		}	
	LOGTEXT(_L8("DataPort Recover"));
	iCallInfo.iLoanedToClient=EFalse;
	iPhoneGlobals->iPhoneStatus.iDataPortLoaned = EFalse;
	LOGTEXT2(_L8("Comm signals : %x"),iIo->Signals());
 	iIo->ResetReadAndWriteBuffers();	// in case client has left RxBuffer non-empty
	LOGTEXT2(_L8("Comm signals after ResetBuffers : %x"),iIo->Signals());
	FlowControlSuspend();			// Defect fix for MPO-4ZECUN
	iQuickInit->StartQuickInit();
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}
