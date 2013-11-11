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

#include "coretsy.H"
#include "testdef.h"

//#include "etelmm.h"

#include <et_clsvr.h>

void TsyPanic(TTsyPanic aPanic)
	{
	_LIT(KTsyPanic,"Tsy Panic");
	User::Panic(KTsyPanic,aPanic);
	}

TTsyTimer::TTsyTimer()
	{
	iPending=EFalse;
	}

/*******************************************************************/
//
// CFaxDummyBase & CFaxDmmTsy
//
/*******************************************************************/

//	CFaxDmmTsy

CFaxDMmTsy* CFaxDMmTsy::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CFaxDMmTsy* fax=new(ELeave) CFaxDMmTsy(aFac);
	CleanupStack::PushL(fax);
	fax->ConstructL();
	CleanupStack::Pop();
	return fax;
	}

CFaxDMmTsy::CFaxDMmTsy(CPhoneFactoryDummyBase* aFac)
	:CFaxDummyBase(aFac)
	{}

void CFaxDMmTsy::ConstructL()
	{
	TInt r = iChunk.CreateGlobal(KNullDesC,sizeof (RFax::TProgress), sizeof (RFax::TProgress),EOwnerProcess); 
	if (r == KErrNone)
		{
		RFax::TProgress* progress = new(iChunk.Base()) RFax::TProgress;
		progress->iLastUpdateTime = 0;
		progress->iAnswerback.Zero ();
		progress->iPhase = ENotYetStarted;
		progress->iSpeed = 9600;
		progress->iResolution = EFaxNormal;
		progress->iCompression = EModifiedHuffman;
		progress->iECM = 0;
 		progress->iPage = 0;
		progress->iLines = 0;
		}	
	else
		User::Leave(r);
	}

CFaxDMmTsy::~CFaxDMmTsy()
	{
	iChunk.Close();
	}
RHandleBase* CFaxDMmTsy::GlobalKernelObjectHandle()
	{
		return &iChunk;
		
	}


TInt CFaxDMmTsy::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNotSupported;
	}

TInt CFaxDMmTsy::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNotSupported;
	}

/*
TInt CFaxDMmTsy::NumberOfSlotsL(const TInt)
	{
	return KErrNotSupported;
	}
*/

//	CFaxDummyBase
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

CFaxDummyBase* CFaxDummyBase::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CFaxDummyBase*,aPtr);
	}


TInt CFaxDummyBase::WaitForEndOfPageHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iWaitForEndOfPage);
	This(aPtr)->ReqCompleted(This(aPtr)->iWaitForEndOfPage.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::WaitForEndOfPage(const TTsyReqHandle aTsyReqHandle)
	{
	iFac->QueueTimer(iWaitForEndOfPage,aTsyReqHandle,DMMTSY_TIMEOUT,CFaxDummyBase::WaitForEndOfPageHandler,this);
	return KErrNone;
	}

TInt CFaxDummyBase::Read(const TTsyReqHandle aTsyReqHandle,TDes8* aDes)
	{
	iReadParams=aDes;
	iFac->QueueTimer(iRead,aTsyReqHandle,DMMTSY_TIMEOUT,CFaxDummyBase::ReadHandler,this);
	return KErrNone;
	}

TInt CFaxDummyBase::ReadHandler(TAny* aPtr)
	{
	This(aPtr)->iReadParams->Copy(DMMTSY_FAX_DATA);
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

	if (iFac->RemoveTimer(iProgressNotification))
		ReqCompleted(iProgressNotification.iTsyReqHandle,KErrCancel);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::Write(const TTsyReqHandle aTsyReqHandle,TDesC8* aDes)
	{
	if (aDes->Compare(DMMTSY_FAX_DATA)!=KErrNone)
		ReqCompleted(aTsyReqHandle,KErrEtelDataCorrupted);
	else
		iFac->QueueTimer(iWrite,aTsyReqHandle,DMMTSY_TIMEOUT,CFaxDummyBase::WriteHandler,this);
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
	aProgress->iSpeed=DMMTSY_GET_PROGRESS_SPEED;
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
	iFac->QueueTimer(iProgressNotification,aTsyReqHandle,DMMTSY_TIMEOUT,CFaxDummyBase::ProgressNotificationHandler,this);
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
	return KErrEtelNotRecognisedCancelHandle;
	}

/*******************************************************************/
//
// CCallDummyBase
//
/*******************************************************************/

CCallDummyBase::CCallDummyBase(CPhoneFactoryDummyBase* aFac)
	{
	iFac=aFac;
	}

void CCallDummyBase::ConstructL()
	{
	}

CCallDummyBase::~CCallDummyBase()
	{
	iFac->RemoveTimer(iNotifyStatusChange);
	iFac->RemoveTimer(iDial);
	iFac->RemoveTimer(iAnswer);
	iFac->RemoveTimer(iHangUp);
	}

CCallDummyBase* CCallDummyBase::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CCallDummyBase*,aPtr);
	}

CPhoneFactoryDummyBase* CCallDummyBase::FacPtr() const
	{
	return iFac;
	}

/*******************************************************************/
//
// Core ETel API call requests - all these must be implemented by TSY
// even if just to return KErrNotSupported
//
/*******************************************************************/

TInt CCallDummyBase::RelinquishOwnership()
	{
	// Request not implemented in this dummy TSY
	RelinquishOwnershipCompleted(KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::TransferOwnership(const TTsyReqHandle aTsyReqHandle)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::AcquireOwnership(const TTsyReqHandle aTsyReqHandle)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle,RCall::TCaps*)
	{
	iFac->QueueTimer(iNotifyCapsChange,aTsyReqHandle,DMMTSY_TIMEOUT,CCallDummyBase::NotifyCapsChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyHookChange(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus* /*aHookStatus*/)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	iNotifyStatusChangeParams=aStatus;
	iFac->QueueTimer(iNotifyStatusChange,aTsyReqHandle,DMMTSY_TIMEOUT,CCallDummyBase::NotifyStatusChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyDurationChange(const TTsyReqHandle aTsyReqHandle,TTimeIntervalSeconds*)
	{
	iFac->QueueTimer(iNotifyDurationChange,aTsyReqHandle,DMMTSY_TIMEOUT,CCallDummyBase::NotifyDurationChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::GetInfo(const TTsyReqHandle aTsyReqHandle, RCall::TCallInfo* /*aCallInfo*/)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	*aStatus=DMMTSY_CORE_CALL_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetCaps(const TTsyReqHandle aTsyReqHandle,RCall::TCaps*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::LoanDataPort(const TTsyReqHandle aTsyReqHandle, RCall::TCommPort* /*aCommPort*/)
	{
	iFac->QueueTimer(iLoanDataPortTimer,aTsyReqHandle,DMMTSY_TIMEOUT,CCallDummyBase::LoanDataPortHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::RecoverDataPort(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::RecoverDataPortAndRelinquishOwnership()
	{
	// Request not implemented in this dummy TSY
	RecoverDataPortAndRelinquishOwnershipCompleted(KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8* aCallParams, TDesC* aNumber)
	{
	TInt error=KErrNone;

	if(aNumber->Compare(DMMTSY_PHONE_TEL_NUMBER)!=KErrNone)
		error=KErrCorrupt;

	RCall::TCallParamsPckg* paramsPckgV1 = (RCall::TCallParamsPckg*)aCallParams;
	RCall::TCallParams& paramsV1 = (*paramsPckgV1)();

	iDataCallParams.iSpeakerControl = paramsV1.iSpeakerControl;
	iDataCallParams.iSpeakerVolume = paramsV1.iSpeakerVolume;
	iDataCallParams.iInterval = paramsV1.iInterval;
	iDataCallParams.iWaitForDialTone = paramsV1.iWaitForDialTone;
	
	if (paramsV1.ExtensionId()==RMobileCall::KETelMobileCallParamsV1)
		{
		if (iMobileCallParams.iInterval != DMMTSY_CALL_INTERVAL)
			error=KErrCorrupt;

		RMobileCall::TMobileCallParamsV1Pckg* mmParamsPckgV1 = (RMobileCall::TMobileCallParamsV1Pckg*)aCallParams;
		RMobileCall::TMobileCallParamsV1& mmParamsV1 = (*mmParamsPckgV1)();

		iMobileCallParams.iIdRestrict = mmParamsV1.iIdRestrict;
		iMobileCallParams.iCug = mmParamsV1.iCug;
		iMobileCallParams.iAutoRedial = mmParamsV1.iAutoRedial;

		if ((iMobileCallParams.iIdRestrict != DMMTSY_CALL_CLI_RESTRICT_SETTING1) ||
		    (iMobileCallParams.iCug.iExplicitInvoke != DMMTSY_CALL_CUG_INVOKE) ||
			(iMobileCallParams.iCug.iCugIndex != DMMTSY_CALL_CUG_INDEX) ||
		    (iMobileCallParams.iCug.iSuppressOA != DMMTSY_CALL_CUG_OA) ||
		    (iMobileCallParams.iCug.iSuppressPrefCug != DMMTSY_CALL_CUG_PREF) ||
			(iMobileCallParams.iAutoRedial != DMMTSY_CALL_AUTO_REDIAL_FLAG))
			{
			error=KErrCorrupt;
			}
		}
	else if (paramsV1.ExtensionId()==RMobileCall::KETelMobileCallParamsV2)
		{
		if (iMobileCallParams.iInterval != DMMTSY_CALL_INTERVAL)
			error=KErrCorrupt;

		RMobileCall::TMobileCallParamsV2Pckg* mmParamsPckgV2 = (RMobileCall::TMobileCallParamsV2Pckg*)aCallParams;
		RMobileCall::TMobileCallParamsV2& mmParamsV2 = (*mmParamsPckgV2)();

		iMobileCallParams.iIdRestrict = mmParamsV2.iIdRestrict;
		iMobileCallParams.iCug = mmParamsV2.iCug;
		iMobileCallParams.iAutoRedial = mmParamsV2.iAutoRedial;
		iMobileCallParams.iBearerMode = mmParamsV2.iBearerMode;

		if ((iMobileCallParams.iIdRestrict != DMMTSY_CALL_CLI_RESTRICT_SETTING1) ||
		    (iMobileCallParams.iCug.iExplicitInvoke != DMMTSY_CALL_CUG_INVOKE) ||
			(iMobileCallParams.iCug.iCugIndex != DMMTSY_CALL_CUG_INDEX) ||
		    (iMobileCallParams.iCug.iSuppressOA != DMMTSY_CALL_CUG_OA) ||
		    (iMobileCallParams.iCug.iSuppressPrefCug != DMMTSY_CALL_CUG_PREF) ||
			(iMobileCallParams.iAutoRedial != DMMTSY_CALL_AUTO_REDIAL_FLAG) ||
			(iMobileCallParams.iBearerMode != DMMTSY_CALL_NEW_BEARER)) 
			{
			error=KErrCorrupt;
			}
		}
	else if (paramsV1.ExtensionId()==RMobileCall::KETelMobileCallParamsV7)
		{
		if (iMobileCallParams.iInterval != DMMTSY_CALL_INTERVAL)
			{
			error=KErrCorrupt;
			}
		
		RMobileCall::TMobileCallParamsV7Pckg* mmParamsPckgV7 = (RMobileCall::TMobileCallParamsV7Pckg*)aCallParams;
		RMobileCall::TMobileCallParamsV7& mmParamsV7 = (*mmParamsPckgV7)();

		iMobileCallParams.iIdRestrict = mmParamsV7.iIdRestrict;
		iMobileCallParams.iCug = mmParamsV7.iCug;
		iMobileCallParams.iAutoRedial = mmParamsV7.iAutoRedial;
		iMobileCallParams.iBearerMode = mmParamsV7.iBearerMode;
		iMobileCallParams.iCallParamOrigin = mmParamsV7.iCallParamOrigin;
		iMobileCallParams.iSubAddress = mmParamsV7.iSubAddress;
		iMobileCallParams.iBearerCap1 = mmParamsV7.iBearerCap1;
		iMobileCallParams.iBearerCap2 = mmParamsV7.iBearerCap2;
		iMobileCallParams.iBCRepeatIndicator = mmParamsV7.iBCRepeatIndicator;
		iMobileCallParams.iIconId = mmParamsV7.iIconId;
		iMobileCallParams.iAlphaId = mmParamsV7.iAlphaId;
		
		if ((iMobileCallParams.iIdRestrict != DMMTSY_CALL_CLI_RESTRICT_SETTING1) ||
		    (iMobileCallParams.iCug.iExplicitInvoke != DMMTSY_CALL_CUG_INVOKE) ||
			(iMobileCallParams.iCug.iCugIndex != DMMTSY_CALL_CUG_INDEX) ||
		    (iMobileCallParams.iCug.iSuppressOA != DMMTSY_CALL_CUG_OA) ||
		    (iMobileCallParams.iCug.iSuppressPrefCug != DMMTSY_CALL_CUG_PREF) ||
			(iMobileCallParams.iAutoRedial != DMMTSY_CALL_AUTO_REDIAL_FLAG) ||
			(iMobileCallParams.iBearerMode != DMMTSY_CALL_NEW_BEARER) ||
			(iMobileCallParams.iCallParamOrigin != DMMTSY_CALL_PARAM_ORIGIN) ||
			(iMobileCallParams.iSubAddress != DMMTSY_CALL_SUBADDRESS) ||
			(iMobileCallParams.iBearerCap1 != DMMTSY_CALL_BEARER_CAP1) ||
			(iMobileCallParams.iBearerCap2 != DMMTSY_CALL_BEARER_CAP2) ||
			(iMobileCallParams.iBCRepeatIndicator != DMMTSY_CALL_BC_REPEAT_INDICATOR) ||
			(iMobileCallParams.iIconId.iQualifier != DMMTSY_CALL_ICON_ID_QUALIFIER) ||
			(iMobileCallParams.iIconId.iIdentifier != DMMTSY_CALL_ICON_ID_IDENTIFIER) ||
			(iMobileCallParams.iAlphaId != DMMTSY_CALL_ALPHA_ID)) 
			{
			error=KErrCorrupt;		
			}
		}

	if ((paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV1)||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV2)||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV8)||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV1)||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV2)||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV7)||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV8))
		{
		RMobileCall::TMobileDataCallParamsV1Pckg* dataParamsPckgV1 = (RMobileCall::TMobileDataCallParamsV1Pckg*)aCallParams;
		RMobileCall::TMobileDataCallParamsV1& dataParamsV1 = (*dataParamsPckgV1)();

		iDataCallParams.iIdRestrict = dataParamsV1.iIdRestrict;
		iDataCallParams.iCug = dataParamsV1.iCug;
		iDataCallParams.iAutoRedial = dataParamsV1.iAutoRedial;
			
		iDataCallParams.iService = dataParamsV1.iService;
		iDataCallParams.iSpeed = dataParamsV1.iSpeed;
		iDataCallParams.iProtocol = dataParamsV1.iProtocol;
		iDataCallParams.iQoS = dataParamsV1.iQoS;
		iDataCallParams.iRLPVersion = dataParamsV1.iRLPVersion;
		iDataCallParams.iV42bisReq = dataParamsV1.iV42bisReq;
		iDataCallParams.iUseEdge = dataParamsV1.iUseEdge;

		if ((iDataCallParams.iIdRestrict != DMMTSY_CALL_CLI_RESTRICT_SETTING1) ||
			(iDataCallParams.iCug.iExplicitInvoke != DMMTSY_CALL_CUG_INVOKE) ||
			(iDataCallParams.iCug.iCugIndex != DMMTSY_CALL_CUG_INDEX) ||
			(iDataCallParams.iCug.iSuppressOA != DMMTSY_CALL_CUG_OA) ||
			(iDataCallParams.iCug.iSuppressPrefCug != DMMTSY_CALL_CUG_PREF) ||
			(iDataCallParams.iAutoRedial != DMMTSY_CALL_AUTO_REDIAL_FLAG) ||
			(iDataCallParams.iService != DMMTSY_DATA_CALL_SERVICE) ||
		    (iDataCallParams.iSpeed != DMMTSY_DATA_CALL_SPEED) ||
		    (iDataCallParams.iProtocol != DMMTSY_DATA_CALL_PROTOCOL) ||
			(iDataCallParams.iQoS != DMMTSY_DATA_CALL_QOS) || 
			(iDataCallParams.iRLPVersion != DMMTSY_DATA_CALL_RLP) ||
			(iDataCallParams.iV42bisReq != DMMTSY_DATA_CALL_V42BIS) ||
			(iDataCallParams.iUseEdge != DMMTSY_DATA_CALL_EGPRS_REQUIRED))
			
			error=KErrCorrupt;
		}
	
	if ((paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV2) ||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV8))
		{
		RMobileCall::TMobileDataCallParamsV2Pckg* dataParamsPckgV2 = (RMobileCall::TMobileDataCallParamsV2Pckg*)aCallParams;
		RMobileCall::TMobileDataCallParamsV2& dataParamsV2 = (*dataParamsPckgV2)();

		iDataCallParams.iBearerMode = dataParamsV2.iBearerMode;
		if (iDataCallParams.iBearerMode != DMMTSY_PHONE_MULTICALLBEARERMODE_NOTSUPPORTED)
							
			error=KErrCorrupt;
				
		}
	
	if ((paramsV1.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV8))
			{
			RMobileCall::TMobileDataCallParamsV8Pckg* dataParamsPckgV8 = (RMobileCall::TMobileDataCallParamsV8Pckg*)aCallParams;
			RMobileCall::TMobileDataCallParamsV8& dataParamsV8 = (*dataParamsPckgV8)();

			iDataCallParams.iCallParamOrigin = dataParamsV8.iCallParamOrigin;
			iDataCallParams.iSubAddress = dataParamsV8.iSubAddress;
			iDataCallParams.iBearerCap1 = dataParamsV8.iBearerCap1;
			iDataCallParams.iBearerCap2 = dataParamsV8.iBearerCap2;
			iDataCallParams.iBCRepeatIndicator = dataParamsV8.iBCRepeatIndicator;
			iDataCallParams.iIconId.iIdentifier = dataParamsV8.iIconId.iIdentifier;
			iDataCallParams.iIconId.iQualifier = dataParamsV8.iIconId.iQualifier;
			iDataCallParams.iAlphaId = dataParamsV8.iAlphaId;
			
			if ((iDataCallParams.iCallParamOrigin != DMMTSY_CALL_PARAM_ORIGIN) ||
			    (iDataCallParams.iSubAddress != DMMTSY_CALL_SUBADDRESS) ||
			    (iDataCallParams.iBearerCap1 != DMMTSY_CALL_BEARER_CAP1) ||
			    (iDataCallParams.iBearerCap2 != DMMTSY_CALL_BEARER_CAP2) || 
			    (iDataCallParams.iBCRepeatIndicator != DMMTSY_CALL_BC_REPEAT_INDICATOR)||
			    (iDataCallParams.iIconId.iIdentifier != DMMTSY_CALL_ICON_ID_IDENTIFIER) ||
			    (iDataCallParams.iIconId.iQualifier != DMMTSY_CALL_ICON_ID_QUALIFIER) ||
			    (iDataCallParams.iAlphaId != DMMTSY_CALL_ALPHA_ID))
								
				error=KErrCorrupt;
			
			}

	if ((paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV1) || 
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV2) ||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV7) ||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV8))
		{
		RMobileCall::TMobileHscsdCallParamsV1Pckg* hscsdParamsPckgV1 = (RMobileCall::TMobileHscsdCallParamsV1Pckg*)aCallParams;
		RMobileCall::TMobileHscsdCallParamsV1& hscsdParamsV1 = (*hscsdParamsPckgV1)();

		iDataCallParams.iWantedAiur = hscsdParamsV1.iWantedAiur;
		iDataCallParams.iWantedRxTimeSlots = hscsdParamsV1.iWantedRxTimeSlots;
		iDataCallParams.iMaxTimeSlots = hscsdParamsV1.iMaxTimeSlots;
		iDataCallParams.iCodings = hscsdParamsV1.iCodings;
		iDataCallParams.iAsymmetry = hscsdParamsV1.iAsymmetry;
		iDataCallParams.iUserInitUpgrade = hscsdParamsV1.iUserInitUpgrade;

		if ((iDataCallParams.iWantedAiur != DMMTSY_CALL_HCSD_AIUR) ||
		    (iDataCallParams.iWantedRxTimeSlots != DMMTSY_CALL_HCSD_TS) ||
		    (iDataCallParams.iMaxTimeSlots != DMMTSY_CALL_HCSD_MAXTS) ||
		    (iDataCallParams.iCodings != DMMTSY_CALL_HCSD_CODING) ||
			(iDataCallParams.iAsymmetry != DMMTSY_CALL_HCSD_ASYMMETRY) ||
			(iDataCallParams.iUserInitUpgrade != DMMTSY_CALL_HCSD_USER_IMI))

			error=KErrCorrupt;
		}

	if ((paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV2) ||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV7) ||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV8))
		{
		RMobileCall::TMobileHscsdCallParamsV2Pckg* hscsdParamsPckgV2 = (RMobileCall::TMobileHscsdCallParamsV2Pckg*)aCallParams;
		RMobileCall::TMobileHscsdCallParamsV2& hscsdParamsV2 = (*hscsdParamsPckgV2)();

		iDataCallParams.iBearerMode = hscsdParamsV2.iBearerMode;
		
		if ((iDataCallParams.iBearerMode != DMMTSY_CALL_NEW_BEARER))

			error=KErrCorrupt;
		}
	
	if ((paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV7) ||
		(paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV8))
		{
		RMobileCall::TMobileHscsdCallParamsV7Pckg* hscsdParamsPckgV7 = (RMobileCall::TMobileHscsdCallParamsV7Pckg*)aCallParams;
		RMobileCall::TMobileHscsdCallParamsV7& hscsdParamsV7 = (*hscsdParamsPckgV7)();

		iDataCallParams.iCallParamOrigin = hscsdParamsV7.iCallParamOrigin;
		iDataCallParams.iIconId =  hscsdParamsV7.iIconId;
		iDataCallParams.iAlphaId = hscsdParamsV7.iAlphaId;
			
		if ((iDataCallParams.iCallParamOrigin != DMMTSY_CALL_PARAM_ORIGIN) ||
			(iDataCallParams.iAlphaId != DMMTSY_CALL_ALPHA_ID))

			error=KErrCorrupt;
		}

	if (paramsV1.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV8)
		{
		RMobileCall::TMobileHscsdCallParamsV8Pckg* hscsdParamsPckgV8 = (RMobileCall::TMobileHscsdCallParamsV8Pckg*)aCallParams;
		RMobileCall::TMobileHscsdCallParamsV8& hscsdParamsV8 = (*hscsdParamsPckgV8)();
		
		iDataCallParams.iBearerMode = hscsdParamsV8.iBearerMode;
		iDataCallParams.iCallParamOrigin = hscsdParamsV8.iCallParamOrigin;
		iDataCallParams.iIconId.iIdentifier = hscsdParamsV8.iIconId.iIdentifier;
		iDataCallParams.iIconId.iQualifier = hscsdParamsV8.iIconId.iQualifier;
		iDataCallParams.iAlphaId = hscsdParamsV8.iAlphaId;
		iDataCallParams.iSubAddress = hscsdParamsV8.iSubAddress;
		iDataCallParams.iBearerCap1 = hscsdParamsV8.iBearerCap1;
		iDataCallParams.iBearerCap2 = hscsdParamsV8.iBearerCap2;
		iDataCallParams.iBCRepeatIndicator = hscsdParamsV8.iBCRepeatIndicator;
					
		if ((iDataCallParams.iBearerMode != DMMTSY_CALL_NEW_BEARER) ||
			(iDataCallParams.iCallParamOrigin != DMMTSY_CALL_PARAM_ORIGIN) ||
			(iDataCallParams.iIconId.iIdentifier != DMMTSY_CALL_ICON_ID_IDENTIFIER) ||
			(iDataCallParams.iIconId.iQualifier != DMMTSY_CALL_ICON_ID_QUALIFIER) ||
			(iDataCallParams.iAlphaId != DMMTSY_CALL_ALPHA_ID)||
			(iDataCallParams.iSubAddress != DMMTSY_CALL_SUBADDRESS) ||
			(iDataCallParams.iBearerCap1 != DMMTSY_CALL_BEARER_CAP1) ||
			(iDataCallParams.iBearerCap2 != DMMTSY_CALL_BEARER_CAP2) || 
			(iDataCallParams.iBCRepeatIndicator != DMMTSY_CALL_BC_REPEAT_INDICATOR))
			{
			error = KErrCorrupt;
			}
		}
	
	if (error != KErrNone)
		ReqCompleted(aTsyReqHandle,error);
	else
		iFac->QueueTimer(iDial,aTsyReqHandle,DMMTSY_TIMEOUT, CCallDummyBase::DialHandler,this);

	return KErrNone;
	}

TInt CCallDummyBase::Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8*)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8*)
	{
	iFac->QueueTimer(iAnswer,aTsyReqHandle,DMMTSY_TIMEOUT,CCallDummyBase::AnswerHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::HangUp(const TTsyReqHandle aTsyReqHandle)
	{
	// If this is a voice call then any client can hang it up.
	// If this is a data or fax call then only the owning client can hang it up
	if (CheckOwnership(aTsyReqHandle)==CCallBase::EOwnedTrue)
		{
		iFac->QueueTimer(iHangUp,aTsyReqHandle,DMMTSY_TIMEOUT,CCallDummyBase::HangUpHandler,this);
		return KErrNone;
		}
	else
		return KErrEtelNotCallOwner;
	}

TInt CCallDummyBase::GetBearerServiceInfo(const TTsyReqHandle aTsyReqHandle,RCall::TBearerService* aService)
	{
	aService->iBearerSpeed=DMMTSY_CALL_BEARER_SPEED;
	aService->iBearerCaps=DMMTSY_CALL_BEARER_CAPS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetCallParams(const TTsyReqHandle aTsyReqHandle, TDes8* aCallParams)
	{
	RCall::TCallParamsPckg* paramsPckgV1 = (RCall::TCallParamsPckg*)aCallParams;
	RCall::TCallParams& paramsV1 = (*paramsPckgV1)();

	TInt ext = paramsV1.ExtensionId();
	
	switch (ext)
		{
		case RMobileCall::KETelMobileDataCallParamsV1:
		case RMobileCall::KETelMobileHscsdCallParamsV1:
		case RMobileCall::KETelMobileDataCallParamsV2:
		case RMobileCall::KETelMobileHscsdCallParamsV2:
		case RMobileCall::KETelMobileHscsdCallParamsV7:
		case RMobileCall::KETelMobileDataCallParamsV8:
		case RMobileCall::KETelMobileHscsdCallParamsV8:
			{
			paramsV1.iSpeakerControl = iDataCallParams.iSpeakerControl;
			paramsV1.iSpeakerVolume = iDataCallParams.iSpeakerVolume;
			paramsV1.iInterval = iDataCallParams.iInterval;
			paramsV1.iWaitForDialTone = iDataCallParams.iWaitForDialTone;
			break;
			}
		case RMobileCall::KETelMobileCallParamsV1:
		case RMobileCall::KETelMobileCallParamsV2:
		case RMobileCall::KETelMobileCallParamsV7:
		default:
			{
			paramsV1.iSpeakerControl = iMobileCallParams.iSpeakerControl;
			paramsV1.iSpeakerVolume = iMobileCallParams.iSpeakerVolume;
			paramsV1.iInterval = iMobileCallParams.iInterval;
			paramsV1.iWaitForDialTone = iMobileCallParams.iWaitForDialTone;
			break;
			}
		}
	
	if ((ext==RMobileCall::KETelMobileCallParamsV1) ||
	    (ext==RMobileCall::KETelMobileDataCallParamsV1) ||
	    (ext==RMobileCall::KETelMobileHscsdCallParamsV1)) 
		{
		RMobileCall::TMobileCallParamsV1Pckg* mmParamsPckgV1 = (RMobileCall::TMobileCallParamsV1Pckg*)aCallParams;
		RMobileCall::TMobileCallParamsV1& mmParamsV1 = (*mmParamsPckgV1)();

		mmParamsV1.iIdRestrict = iMobileCallParams.iIdRestrict;
		mmParamsV1.iCug = iMobileCallParams.iCug;
		mmParamsV1.iAutoRedial = iMobileCallParams.iAutoRedial;
		}
	else if (ext==RMobileCall::KETelMobileCallParamsV2)
		{
		RMobileCall::TMobileCallParamsV2Pckg* mmParamsPckgV2 = (RMobileCall::TMobileCallParamsV2Pckg*)aCallParams;
		RMobileCall::TMobileCallParamsV2& mmParamsV2 = (*mmParamsPckgV2)();

		mmParamsV2.iIdRestrict = iMobileCallParams.iIdRestrict;
		mmParamsV2.iCug = iMobileCallParams.iCug;
		mmParamsV2.iAutoRedial = iMobileCallParams.iAutoRedial;
		mmParamsV2.iBearerMode = iMobileCallParams.iBearerMode;
		}
	else if ((ext==RMobileCall::KETelMobileDataCallParamsV2) ||
	    	 (ext==RMobileCall::KETelMobileHscsdCallParamsV2)||
	    	 (ext==RMobileCall::KETelMobileDataCallParamsV8) ||
	    	 (ext==RMobileCall::KETelMobileHscsdCallParamsV7)||
	    	 (ext==RMobileCall::KETelMobileHscsdCallParamsV8))
		{
		RMobileCall::TMobileDataCallParamsV2Pckg* mmParamsPckgV2 = (RMobileCall::TMobileDataCallParamsV2Pckg*)aCallParams;
		RMobileCall::TMobileDataCallParamsV2& mmParamsV2 = (*mmParamsPckgV2)();

		mmParamsV2.iIdRestrict = iDataCallParams.iIdRestrict;
		mmParamsV2.iCug = iDataCallParams.iCug;
		mmParamsV2.iAutoRedial = iDataCallParams.iAutoRedial;
		mmParamsV2.iBearerMode = iDataCallParams.iBearerMode;
		}

	if ((ext==RMobileCall::KETelMobileDataCallParamsV1) ||
	    (ext==RMobileCall::KETelMobileHscsdCallParamsV1) ||
		(ext==RMobileCall::KETelMobileDataCallParamsV2) ||
	    (ext==RMobileCall::KETelMobileHscsdCallParamsV2)||
	    (ext==RMobileCall::KETelMobileDataCallParamsV8) ||
	    (ext==RMobileCall::KETelMobileHscsdCallParamsV7) ||
	    (ext==RMobileCall::KETelMobileHscsdCallParamsV8)) 
		{
		RMobileCall::TMobileDataCallParamsV1Pckg* dataParamsPckgV1 = (RMobileCall::TMobileDataCallParamsV1Pckg*)aCallParams;
		RMobileCall::TMobileDataCallParamsV1& dataParamsV1 = (*dataParamsPckgV1)();

		dataParamsV1.iService = iDataCallParams.iService;
		dataParamsV1.iSpeed = iDataCallParams.iSpeed;
		dataParamsV1.iProtocol = iDataCallParams.iProtocol;
		dataParamsV1.iQoS = iDataCallParams.iQoS;
		dataParamsV1.iRLPVersion = iDataCallParams.iRLPVersion;
		dataParamsV1.iV42bisReq = iDataCallParams.iV42bisReq;
		dataParamsV1.iUseEdge = iDataCallParams.iUseEdge;
		}
	if ((ext==RMobileCall::KETelMobileDataCallParamsV8))
		{
		RMobileCall::TMobileDataCallParamsV8Pckg* dataParamsPckgV8 = (RMobileCall::TMobileDataCallParamsV8Pckg*)aCallParams;
		RMobileCall::TMobileDataCallParamsV8& dataParamsV8 = (*dataParamsPckgV8)();

		dataParamsV8.iCallParamOrigin = iDataCallParams.iCallParamOrigin; 
		dataParamsV8.iSubAddress = iDataCallParams.iSubAddress;
		dataParamsV8.iBearerCap1 = iDataCallParams.iBearerCap1;
		dataParamsV8.iBearerCap2 = iDataCallParams.iBearerCap2;
		dataParamsV8.iBCRepeatIndicator = iDataCallParams.iBCRepeatIndicator;
		dataParamsV8.iIconId.iIdentifier = iDataCallParams.iIconId.iIdentifier;
		dataParamsV8.iIconId.iQualifier = iDataCallParams.iIconId.iQualifier;
		dataParamsV8.iAlphaId = iDataCallParams.iAlphaId;
		
		}
	
	if ((ext==RMobileCall::KETelMobileHscsdCallParamsV1) ||
		(ext==RMobileCall::KETelMobileHscsdCallParamsV2) ||
		(ext==RMobileCall::KETelMobileHscsdCallParamsV8))
		{
		RMobileCall::TMobileHscsdCallParamsV1Pckg* hscsdParamsPckgV1 = (RMobileCall::TMobileHscsdCallParamsV1Pckg*)aCallParams;
		RMobileCall::TMobileHscsdCallParamsV1& hscsdParamsV1 = (*hscsdParamsPckgV1)();

		hscsdParamsV1.iWantedAiur = iDataCallParams.iWantedAiur;
		hscsdParamsV1.iWantedRxTimeSlots = iDataCallParams.iWantedRxTimeSlots;
		hscsdParamsV1.iMaxTimeSlots = iDataCallParams.iMaxTimeSlots;
		hscsdParamsV1.iCodings = iDataCallParams.iCodings;
		hscsdParamsV1.iAsymmetry = iDataCallParams.iAsymmetry;
	    hscsdParamsV1.iUserInitUpgrade = iDataCallParams.iUserInitUpgrade;
		}
	
	
	if ((ext==RMobileCall::KETelMobileHscsdCallParamsV8))
		{
		RMobileCall::TMobileHscsdCallParamsV8Pckg* hscsdParamsPckgV8 = (RMobileCall::TMobileHscsdCallParamsV8Pckg*)aCallParams;
		RMobileCall::TMobileHscsdCallParamsV8& hscsdParamsV8 = (*hscsdParamsPckgV8)();
		
		hscsdParamsV8.iSubAddress = iDataCallParams.iSubAddress;
		hscsdParamsV8.iBearerCap1 = iDataCallParams.iBearerCap1;
		hscsdParamsV8.iBearerCap2 = iDataCallParams.iBearerCap2;
		hscsdParamsV8.iBCRepeatIndicator = iDataCallParams.iBCRepeatIndicator;
					
		}


	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetCallDuration(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime)
	{
	*aTime = DMMTSY_CALL_DURATION1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetFaxSettings(const TTsyReqHandle aTsyReqHandle,RCall::TFaxSessionSettings* aSettings)
	{
	aSettings->iFaxId=DMMTSY_MYFAX;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::SetFaxSettings(const TTsyReqHandle aTsyReqHandle,const RCall::TFaxSessionSettings* aSettings)
	{
	if (aSettings->iFaxId.Compare(DMMTSY_MYFAX)!=KErrNone)
		ReqCompleted(aTsyReqHandle,KErrEtelDataCorrupted);
	else
		ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


/*******************************************************************/
//
// Cancellation functions for call requests
//
/*******************************************************************/

TInt CCallDummyBase::AcquireOwnershipCancel(const TTsyReqHandle aTsyReqHandle)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrCancel);
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
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CCallDummyBase::NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrCancel);
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
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CCallDummyBase::NotifyDurationChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyDurationChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyDurationChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CCallDummyBase::LoanDataPortCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iLoanDataPortTimer.iTsyReqHandle)
		{
		iFac->RemoveTimer(iLoanDataPortTimer);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
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
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CCallDummyBase::ConnectCancel(const TTsyReqHandle aTsyReqHandle)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrCancel);
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
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CCallDummyBase::HangUpCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iHangUp.iTsyReqHandle)
		{
		iFac->RemoveTimer(iHangUp);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}


/*******************************************************************/
//
// Asynchronous handler functions - called after a timeout
//
/*******************************************************************/

TInt CCallDummyBase::NotifyCapsChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyCapsChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCapsChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyStatusChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyStatusChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyStatusChange.iTsyReqHandle,KErrNone);
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

TInt CCallDummyBase::LoanDataPortHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iLoanDataPortTimer);
	This(aPtr)->ReqCompleted(This(aPtr)->iLoanDataPortTimer.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

/*******************************************************************/
//
// CLineDummyBase
//
/*******************************************************************/

CLineDummyBase::CLineDummyBase(CPhoneFactoryDummyBase* aFac)
	{
	iFac=aFac;
	}

void CLineDummyBase::ConstructL()
	{
	}

CLineDummyBase::~CLineDummyBase()
	{
	iFac->RemoveTimer(iNotifyIncomingCall);
	iFac->RemoveTimer(iNotifyStatusChange);
	}

CLineDummyBase* CLineDummyBase::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CLineDummyBase*,aPtr);
	}

CPhoneFactoryDummyBase* CLineDummyBase::FacPtr() const
	{
	return iFac;
	}

/*******************************************************************/
//
// Core ETel API line requests - all these must be implemented by TSY
// even if just to return KErrNotSupported
//
/*******************************************************************/

TInt CLineDummyBase::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle,RLine::TCaps* aCaps)
	{
	iCapsParams=aCaps;
	iFac->QueueTimer(iNotifyCapsChange,aTsyReqHandle,DMMTSY_TIMEOUT,CLineDummyBase::NotifyCapsChangeHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	iStatusParams=aStatus;
	iFac->QueueTimer(iNotifyStatusChange,aTsyReqHandle,DMMTSY_TIMEOUT,CLineDummyBase::NotifyStatusChangeHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyIncomingCall(const TTsyReqHandle aTsyReqHandle, TName* aName)
	{
	iNameParams=aName;
	iFac->QueueTimer(iNotifyIncomingCall,aTsyReqHandle,DMMTSY_TIMEOUT,CLineDummyBase::NotifyIncomingCallHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* /*aHookStatus*/)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyCallAdded(const TTsyReqHandle aTsyReqHandle, TName* /*aName*/)
	{
	iFac->QueueTimer(iNotifyCallAdded,aTsyReqHandle,DMMTSY_TIMEOUT,CLineDummyBase::NotifyCallAddedHandler,this);
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
	*aStatus=DMMTSY_CORE_LINE_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::GetHookStatus(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus*)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::EnumerateCall(const TTsyReqHandle aTsyReqHandle, TInt*)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex*)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


/*******************************************************************/
//
// Cancellation functions for line requests
//
/*******************************************************************/

TInt CLineDummyBase::NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyCapsChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyCapsChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CLineDummyBase::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyStatusChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyStatusChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CLineDummyBase::NotifyIncomingCallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyIncomingCall.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyIncomingCall);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CLineDummyBase::NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrCancel);
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
	return KErrEtelNotRecognisedCancelHandle;
	}

/*******************************************************************/
//
// Asynchronous handler functions - called after a timeout
//
/*******************************************************************/

TInt CLineDummyBase::NotifyCapsChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyCapsChange);
	This(aPtr)->iCapsParams->iFlags=DMMTSY_CORE_LINE_CAPS;
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCapsChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyStatusChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyStatusChange);
	*(This(aPtr)->iStatusParams)=DMMTSY_CORE_LINE_STATUS2;
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyStatusChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyIncomingCallHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyIncomingCall);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyIncomingCall.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyCallAddedHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyCallAdded);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCallAdded.iTsyReqHandle,KErrNone);
	return KErrNone;
	}


/*******************************************************************/
//
// CPhoneDummyBase
//
/*******************************************************************/

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
	iFac->RemoveTimer(iNotifyCapsChange);
	}

CPhoneDummyBase* CPhoneDummyBase::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CPhoneDummyBase*,aPtr);
	}

CPhoneFactoryDummyBase* CPhoneDummyBase::FacPtr() const
	{
	return iFac;
	}

/*******************************************************************/
//
// Core ETel API phone requests - all these must be implemented by TSY
// even if just to return KErrNotSupported
//
/*******************************************************************/

TInt CPhoneDummyBase::ControlledInitialisation(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle,RPhone::TCaps*)
	{
	iFac->QueueTimer(iNotifyCapsChange,aTsyReqHandle,DMMTSY_TIMEOUT,CPhoneDummyBase::NotifyCapsChangeHandler,this);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyModemDetected(const TTsyReqHandle aTsyReqHandle,RPhone::TModemDetection* aDetection)
	{
	iDetectionParams=aDetection;
	iFac->QueueTimer(iNotifyPhoneDetected,aTsyReqHandle,DMMTSY_TIMEOUT,CPhoneDummyBase::NotifyPhoneDetectedHandler,this);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetInfo(const TTsyReqHandle aTsyReqHandle, RPhone::TPhoneInfo* /*aPhoneInfo*/)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetStatus(const TTsyReqHandle aTsyReqHandle,RPhone::TStatus* aPhoneStat)
	{
	aPhoneStat->iModemDetected=DMMTSY_CORE_PHONE_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetCaps(const TTsyReqHandle aTsyReqHandle,RPhone::TCaps*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::EnumerateLines(const TTsyReqHandle aTsyReqHandle, TInt*)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetLineInfo(const TTsyReqHandle aTsyReqHandle, TLineInfoIndex* /*aLineInfo*/)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


/*******************************************************************/
//
// Cancellation functions for line requests
//
/*******************************************************************/

TInt CPhoneDummyBase::ControlledInitialisationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrCancel);
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
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CPhoneDummyBase::NotifyModemDetectedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyPhoneDetected.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyPhoneDetected);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

/*******************************************************************/
//
// Asynchronous handler functions - called after a timeout
//
/*******************************************************************/

TInt CPhoneDummyBase::NotifyCapsChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyCapsChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCapsChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyPhoneDetectedHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending(This(aPtr)->iNotifyPhoneDetected);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyPhoneDetected.iTsyReqHandle,KErrNone);
	return KErrNone;
	}


/*******************************************************************/
//
// CPhoneFactoryDummyBase
//
/*******************************************************************/

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

void CPhoneFactoryDummyBase::QueueTimer(TTsyTimer& aTsyTimer, const TTsyReqHandle aTsyReqHandle,TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,
										TInt (*aFunction)(TAny *aPtr), TAny* aPtr)
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

TInt CPhoneFactoryDummyBase::GetPhoneInfo(const TInt aIndex, RTelServer::TPhoneInfo& aInfo)
	{
	if (aIndex==0)
		{
		aInfo.iNetworkType=DMMTSY_NETWORK_TYPE;
		aInfo.iName=DMMTSY_PHONE_NAME;
		aInfo.iNumberOfLines=DMMTSY_NUMBER_OF_LINES;
		return KErrNone;
		}
	else if (aIndex==1)
		{
		aInfo.iNetworkType=DMMTSY_NETWORK_TYPE;
		aInfo.iName=DMMTSY_PHONE_NAMEAUTH1;
		aInfo.iNumberOfLines=DMMTSY_NUMBER_OF_LINES;
		return KErrNone;
		}
	else if (aIndex==2)
		{
		aInfo.iNetworkType=DMMTSY_NETWORK_TYPE;
		aInfo.iName=DMMTSY_PHONE_NAMEAUTH2;
		aInfo.iNumberOfLines=DMMTSY_NUMBER_OF_LINES;
		return KErrNone;
		}
	else if (aIndex==3)
		{
		aInfo.iNetworkType=DMMTSY_NETWORK_TYPE;
		aInfo.iName=DMMTSY_PHONE_NAMEAUTH3;
		aInfo.iNumberOfLines=DMMTSY_NUMBER_OF_LINES;
		return KErrNone;
		}
	else
		return KErrNotFound;
	}

TInt CPhoneFactoryDummyBase::EnumeratePhones()
	{
	return (DMMTSY_NUMBER_OF_PHONES);
	}

/*******************************************************************/
//
// CSubSessionExtDummyBase
//
/*******************************************************************/

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

