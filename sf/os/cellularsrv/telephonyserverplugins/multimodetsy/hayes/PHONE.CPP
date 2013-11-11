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
#include "PHONE.H"
#include "LINE.H"
#include "CALL.H"
#include "NOTIFY.H"
#include "mSLOGGER.H"
#include "ATINIT.H"
#include "ATERROR.H"
#include "mPHBOOK.H"
#include "mnetwork.h"
#include "ATIO.H"
#include "Matstd.h"		// for KXXStorage constants
#include "et_struct.h"

#if defined (__WINS__)
_LIT(KPDDName,"ECDRV");
_LIT(KLDDName,"ECOMM");
#else
_LIT(KPDDName,"EUART1");
#if defined (PDD2_NAME)
_LIT(KPDD2Name,"EUART2");
#endif
_LIT(KLDDName,"ECOMM");
#endif


//
//	CPhoneGlobals
//
CPhoneGlobals* CPhoneGlobals::NewL(TBool aExplicit) 
	{
	CPhoneGlobals* self = new(ELeave)CPhoneGlobals();
	CleanupStack::PushL(self);
	self->ConstructL(aExplicit);
	CleanupStack::Pop();
	return self;
	}

void CPhoneGlobals::ConstructL(TBool aExplicit)
	{
	iPhoneStatus.iModemDetected = RPhone::EDetectedUnknown;
	iPhoneStatus.iDataAndFaxFlags = RPhone::KCapsUnknown;
	iPhoneStatus.iWaitForCarrierTime = KDefaultSecondsToWaitForCarrier;
	iPhoneStatus.iRegistrationStatus = RMobilePhone::ERegistrationUnknown;
	iConfiguration=CTsyConfig::NewL(aExplicit);	
	iNotificationStore=CNotifications::NewL();
	}

CPhoneGlobals::~CPhoneGlobals()
	{
	delete iConfiguration;	 
	delete iNotificationStore;
	}

TBool CPhoneGlobals::IsWriteAccess(TStorageType aStorageType)
	{
	if ((aStorageType.Compare(KMEStorage)==KErrNone) || 
		(aStorageType.Compare(KSMStorage)==KErrNone) || 
		(aStorageType.CompareF(KTAStorage)==KErrNone))
		return ETrue;
	else
		return EFalse;
	}


void CPhoneGlobals::CheckForChangeOfNetwork()
//
// Changes in network registration may imply a change of operator, which involves issuing
// more AT commands.
//
	{
	if (iATNetworkInfo && iPhoneStatus.iNetworkChanged &&
		((iPhoneStatus.iMode==RPhone::EModeIdle)||
		 (iPhoneStatus.iMode==RPhone::EModeOnlineCommand)))
		{
		LOGTEXT(_L8("CPhoneGlobals: Update CurrentNetworkInfo"));
		iATNetworkInfo->CheckOperator();
		}
	}

//
// Character set conversion between the ME encoding (see +CSCS) and Unicode
//
// TO DO: Add appropriate use of CHARCONV converters
// "GSM"	=> KCharacterSetIdentifierSms7Bit
// "IRA"	=> KCharacterSetIdentifierAscii
// "8859-1"	=> KCharacterSetISO88591

TInt CPhoneGlobals::ConvertFromUnicode(TDes8& aMEString, const TDesC16& aUnicode) const
	{
	aMEString.Copy(aUnicode);
	return KErrNone;
	}

TInt CPhoneGlobals::ConvertToUnicode(TDes16& aUnicode, const TDesC8& aMEString) const
	{
	aUnicode.Copy(aMEString);
	return KErrNone;
	}



//
// CPhoneHayes
//
void CPhoneHayes::ClosePhone(TAny* aObj)
//
// Utility func for cleanup stack
//
	{
	((CObject*)aObj)->Close();
	}

CPhoneHayes* CPhoneHayes::NewL()
	{
	CPhoneHayes* phone=new(ELeave) CPhoneHayes();
	TCleanupItem newPhoneHayesClose(ClosePhone,phone);
	CleanupStack::PushL(newPhoneHayesClose);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

void CPhoneHayes::ConstructL()
//
// Creation of Global Params
//
	{
	LOGTEXTREL(_L8("---------- New Log ----------"));		// Added this to keep log looking like it used to

	// Add description of component build to log flie
#if defined(__WINS__)
	LOGTEXTREL(_L8("Platform: WINS"));
#elif defined(__MARM_ARMI__)
	LOGTEXTREL(_L8("Platform: ARMI"));
#elif defined(__MARM_ARM4__)
	LOGTEXTREL(_L8("Platform: ARM4"));
#elif defined(__MARM_THUMB__)
	LOGTEXTREL(_L8("Platform: THUMB"));
#else
	LOGTEXTREL(_L8("Platform: unknown"));
#endif
#if defined (_DEBUG)
	LOGTEXTREL(_L8("Variant: DEBUG"));
#else
	LOGTEXTREL(_L8("Variant: RELEASE"));
#endif

	LOGTEXT(_L8("--- CPhoneHayes::ConstructL() ---"));	

	LOGTEXT(_L8("Loading Serial drivers"));

	TInt r=User::LoadPhysicalDevice(KPDDName);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		User::Leave(r);
#if defined (PDD2_NAME)
	r=User::LoadPhysicalDevice(KPDD2Name);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		User::Leave(r);
#endif
	r=User::LoadLogicalDevice(KLDDName);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		User::Leave(r);


		
	iDefaultDataLineInfo.iStatus = RCall::EStatusUnknown;
	iDefaultDataLineInfo.iLineCapsFlags = (	RLine::KCapsData|RLine::KCapsEventIncomingCall);
	iDefaultDataLineInfo.iName = KDataLineName;
	iDefaultFaxLineInfo.iStatus = RCall::EStatusUnknown;
	iDefaultFaxLineInfo.iLineCapsFlags = (	RLine::KCapsFax|RLine::KCapsEventIncomingCall);
	iDefaultFaxLineInfo.iName = KFaxLineName;
	iDefaultVoiceLineInfo.iStatus = RCall::EStatusUnknown;
	iDefaultVoiceLineInfo.iLineCapsFlags = (RLine::KCapsVoice|RLine::KCapsEventIncomingCall);
	iDefaultVoiceLineInfo.iName = KVoiceLineName;
	iSizeOfMemberData = new(ELeave) CArrayFixFlat<TInt>(1);
	}

CPhoneHayes::~CPhoneHayes()
//
//	iIo must be deleted after pointers to objects which used it
//
	{
	LOGTEXT(_L8("Entered CPhoneHayes destructor"));
	if (iPhoneGlobals != NULL)
		iPhoneGlobals->iNotificationStore->RemoveClientFromLastEvents(this);
	delete iInit;
	delete iErrorHandler;
	delete iWaitForCall;
	delete iPhoneGlobals;
	if (iSizeOfMemberData!=NULL)
		iSizeOfMemberData->Reset();
	delete iSizeOfMemberData;
	if (iIo!=NULL)
		{
		iIo->Cancel();
		iIo->Disconnect();
		delete iIo;
		}
	LOGTEXT(_L8("--- CPhoneHayes::~CPhoneHayes() ---"));
	}


TInt CPhoneHayes::MultimodeInitL(TBool aExplicit)
	{
	TFileName csy;
	TName port;
	
	if(!aExplicit && !iPhoneGlobals)
		{
		iPhoneGlobals = CPhoneGlobals::NewL(aExplicit);
		}
		
	LOGTEXT(_L8("Getting CSY from CommDB"));
	(void)User::LeaveIfError(iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameCsyName),csy));

	LOGTEXT(_L8("Getting PORT from CommDB"));
	(void)User::LeaveIfError(iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNamePortName),port));

	if(!iIo)
		iIo=CATIO::NewL(csy,port,iPhoneGlobals->iPhoneStatus.iPortAccess);
	
	if(!iWaitForCall)
		iWaitForCall=CATWaitForCall::NewL(iIo,this,iPhoneGlobals);
	
	if(!iErrorHandler)
		iErrorHandler = CATErrorHandler::NewL(iPhoneGlobals,iWaitForCall);
	
	iIo->SetErrorHandler(iErrorHandler);

	if(!iInit)
		iInit=CATInit::NewL(iIo,this,iPhoneGlobals);
	
	FlowControlSuspend();
	iInit->SpecialStart();
	
	return KErrNone;
	}
	
TInt CPhoneHayes::ExtFunc(const TTsyReqHandle,const TInt, const TDataPackage&)
//
// Extensions aren't supported in this TSY
//
	{
	return KErrNotSupported;
	}

TInt CPhoneHayes::CheckAndSetRegistrationParams(const TInt /*aIpc*/,const TDes8* /*aDes1*/,const TDes8* /*aDes2*/)
	{
	return KErrNotSupported;
	}

//
// Implemented Phone Functions
//
CTelObject* CPhoneHayes::OpenNewObjectByNameL(const TDesC& aName)
//
//	Open a new line. Opens fax line even if phone does not support it, as that information
//	may not be available (init sequence may not have reached that far.)
//
	{
	if (!aName.CompareF(KDataLineName))
		{
		__ASSERT_ALWAYS(iDataLine==NULL,Panic(ELineAlreadyExists));
		iDataLine=CLineMobileData::NewL(iIo,iInit,iPhoneGlobals,aName);
		if (iPhoneGlobals->iPhoneStatus.iLineStatus == RCall::EStatusUnknown)
			iPhoneGlobals->iPhoneStatus.iLineStatus = RCall::EStatusIdle;
		return iDataLine;
		}
	
	else if (!aName.CompareF(KVoiceLineName)) //Added for Java Demo 4.4.99
		{
		__ASSERT_ALWAYS(iVoiceLine==NULL,Panic(ELineAlreadyExists));
		iVoiceLine=CLineMobileVoice::NewL(iIo,iInit,iPhoneGlobals,aName);
		if (iPhoneGlobals->iPhoneStatus.iLineStatus == RCall::EStatusUnknown)
			iPhoneGlobals->iPhoneStatus.iLineStatus = RCall::EStatusIdle;
		return iVoiceLine;
		}
	else
		{
		User::Leave(KErrNotFound);
		return NULL;
		}
	}

CTelObject* CPhoneHayes::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CPhoneHayes::ReqModeL(const TInt aIpc)
	{
	TReqMode reqMode = CPhoneBase::ReqModeL(aIpc);
	if ((reqMode & KReqModeFlowControlObeyed) && iPhoneGlobals->iPhoneStatus.iDataPortLoaned)
		{
		LOGTEXT2(_L8("ReqModeL Leaving with KErrInUse as data port is loaned (aIpc=%d)"),aIpc);
		User::Leave(KErrInUse);
		}

	return reqMode;
	}

TInt CPhoneHayes::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}
TInt CPhoneHayes::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

void CPhoneHayes::Init()
//
//	Automatic start-up initialise function, doesn't call an AT command on completion
//	Re-implemented because modem must be initialised before any RING comes in
//
	{
	}

TInt CPhoneHayes::ControlledInitialisation(const TTsyReqHandle aTsyReqHandle)
/*
 * If the phone is already initialised, then there's nothing to do.  However, if for some
 * reason the phone has not been successfully initialised, but the port is not available
 * (e.g. it may be loaned) then just return KErrAccessDenied.
 * If none of the cases above apply then proceed with the initialisation as usual.
 */
	{
 	if(iPhoneGlobals->iPhoneStatus.iInitStatus==EPhoneInitialised) // This also fixes defect MPO-4ZECUN
 		{
 		LOGTEXT(_L8("CPhoneHayes::ControlledInitialisation\tPhone has been initialised - Completing request."));
 		ReqCompleted(aTsyReqHandle,KErrNone);
 		return KErrNone;
 		}
 
 	if(iPhoneGlobals->iPhoneStatus.iPortAccess==EPortAccessDenied)
 		{
 		LOGTEXT(_L8("CPhoneHayes::ControlledInitialisation\tPort Access Denied flag detected"));
 		ReqCompleted(aTsyReqHandle,KErrAccessDenied);
 		return KErrNone;
 		}

 	if(iInit==NULL)
 		{
 		LOGTEXT(_L8("CPhoneHayes::ControlledInitialisation\tPort Access Denied flag detected"));
 		ReqCompleted(aTsyReqHandle,KErrHardwareNotAvailable);
 		return KErrNone;
 		}

	TInt aError;
	if (iInit->JustInitialised(aError))
		ReqCompleted(aTsyReqHandle,aError);
	else
		iInit->SpecialStart(aTsyReqHandle);
	
	return KErrNone;
	}

TInt CPhoneHayes::ControlledInitialisationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	iInit->StopInit(aTsyReqHandle);
	return KErrNone;
	}

TInt CPhoneHayes::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RPhone::TCaps* /*aCaps*/)
	{
//	iPhoneGlobals->iNotificationStore->RegisterNotification(EPhoneGeneral,aTsyReqHandle,this,aPhoneInfo);
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CPhoneHayes::NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
//	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneHayes::NotifyModemDetected(const TTsyReqHandle aTsyReqHandle, RPhone::TModemDetection* aDetection)
//
//	This request will be completed when the phone's connection status changes
//
	{
	LOGTEXT(_L8("Phone:\tDetection Change Notification lodged"));
	iPhoneGlobals->iNotificationStore->RegisterNotification(EModemDetection,aTsyReqHandle,this,aDetection);
	return KErrNone;
	}

TInt CPhoneHayes::NotifyModemDetectedCancel(const TTsyReqHandle aTsyReqHandle)
//
//	Cancel outstanding modem detection notification, by TSY handle
//
	{
	LOGTEXT(_L8("Phone:\tDetection Change Notification cancelled"));
	iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
	return KErrNone;
	}

TInt CPhoneHayes::GetInfo(const TTsyReqHandle aTsyReqHandle, RPhone::TPhoneInfo* aPhoneInfo)
	{
	aPhoneInfo->iDetection = iPhoneGlobals->iPhoneStatus.iModemDetected;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneHayes::GetCaps(const TTsyReqHandle aTsyReqHandle, RPhone::TCaps* aCaps)
//
//	Get the phone capabilities
//
	{
	LOGTEXT(_L8("Phone:\tExecuting Get Caps"));
	aCaps->iFlags = iPhoneGlobals->iPhoneStatus.iDataAndFaxFlags;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneHayes::GetStatus(const TTsyReqHandle aTsyReqHandle,RPhone::TStatus* aStatus)
//
//	Get the phone status
//
	{
	LOGTEXT(_L8("Phone:\tExecuting Get Status"));
	aStatus->iMode = iPhoneGlobals->iPhoneStatus.iMode;
	aStatus->iModemDetected = iPhoneGlobals->iPhoneStatus.iModemDetected;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneHayes::EnumerateLines(const TTsyReqHandle aTsyReqHandle, TInt* aParams)
//
//	Enumerate the lines
//
	{
	LOGTEXT(_L8("Phone:\tSubmitting Enumerate Lines"));
	*aParams = KNumberOfLines;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneHayes::GetLineInfo(const TTsyReqHandle aTsyReqHandle, TLineInfoIndex* aParams)
//
//	TLineInfoIndex specifies which of the two lines' info is requested. If that line has not
//	been created yet, default info is passed back.
//
	{
	LOGTEXT(_L8("Phone:\tGet Line Info"));
	 if (aParams->iIndex==KDataLineIndex)
		{
		if (iDataLine!=NULL)
			{
			aParams->iInfo.iStatus = iPhoneGlobals->iPhoneStatus.iLineStatus;
			aParams->iInfo.iName = iDataLine->iLineName;
			aParams->iInfo.iLineCapsFlags = (RLine::KCapsData|RLine::KCapsEventIncomingCall);
			}
		else
			{
			aParams->iInfo = iDefaultDataLineInfo;
			}
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else if (aParams->iIndex==KVoiceLineIndex)
		{
		if (iVoiceLine!=NULL)
			{
			aParams->iInfo.iStatus = iPhoneGlobals->iPhoneStatus.iLineStatus;
			aParams->iInfo.iName = iVoiceLine->iLineName;
			aParams->iInfo.iLineCapsFlags = (RLine::KCapsVoice|RLine::KCapsEventIncomingCall);
			}
		else
			{
			aParams->iInfo = iDefaultVoiceLineInfo;
			}
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNotFound);
		}
	return KErrNone;
	}

void CPhoneHayes::RemoveLine(CLineHayes* aLineHayes)
//
//	When a line closes, it calls this to remove its pointer from CPhoneHayes
//
	{
	if (aLineHayes == iDataLine)
		iDataLine=NULL;
	if (aLineHayes == iVoiceLine)
		iVoiceLine=NULL;
	}

void CPhoneHayes::StartWaitForRing()
	{
	iWaitForCall->StartWait();
	}

void CPhoneHayes::SetCallRinging(TInt aIndex)
//
//	If a call has had AnswerIncomingCall() called on it, this will be used to answer immediately.
//	If a NotifyIncomingCall has been called on a line, use PreAlloc call on it and complete notify.
//
//	Returns ETrue if a call has begun to answer, otherwise EFalse
	{
	_LIT16(KNokiaMatchString,"*Nokia*");
	TBool nokiaPhone=(iPhoneGlobals->iPhoneId.iManufacturer.MatchF(KNokiaMatchString)==0);

// If its not a Nokia Phone or its an incoming voice or fax call (i.e. anything other than data)
// then do the proper thing...
	if((!nokiaPhone)||(aIndex==KVoiceLineIndex)||(aIndex==KFaxLineIndex))
		{
		CLineHayes* line=NULL;
		switch (aIndex)
			{
		case KDataLineIndex:
			line=iDataLine;
			break;
		case KVoiceLineIndex:
			line=iVoiceLine;
			break;
		default:
			return;
			};

		if (line==NULL)
			{
			LOGTEXT(_L8("No line has been opened"));
			return;
			}
		LOGTEXT(_L8("Calling AnswerIfPossible on line"));
		if (line->AnswerIfPossible())
				return;
		LOGTEXT(_L8("Calling SetPreAllocCall on line"));
		line->SetPreAllocCall();
		return;
		}
	else
		{
// Otherwise we need to handle the Nokia's ambiguous data call signal: it could actually be
// data or fax
		LOGTEXT(_L8("SetCallRinging()\tDetected an incoming data call on a Nokia phone"));
		SetAmbiguousDataFaxCallRinging();
		return;
		}
	}

void CPhoneHayes::SetAmbiguousDataFaxCallRinging()
//
// Answer or set a call or line as ringing when the string from the modem could either
// indicate it is a data call or a fax call.  The algorithm below has to make a decision
// based on ETel clients behaviour as to whether the incoming call should be treated as
// data or fax.
//
	{
	if((iDataLine)&&(iDataLine->AnswerIfPossible()))	// First priority: if we're waiting for a Data call, answer it
		{
		LOGTEXT(_L8("SetAmbiguousDataFaxCallRinging()\tInterpretting as data call"));
		return;
		}
	// So both lines MIGHT exist.  It's then down to Notify on incoming call notifications,
// and we'll make a priority call in favour of data...
// First ensure that either a Data line or a Fax line does exist (Nokia 7110 fix: returns
// +CRING: REL ASYNC for a voice call (hence a voice line is created). This response
// however, is correctly treated as an incoming indication for a Data or Fax call).
	if (iDataLine)
		{
		LOGTEXT(_L8("SetAmbiguousDataFaxCallRinging()\tA DataLine has been found. Now checking for an outstanding Notification"));
		
		if(iDataLine->IsNotifyIncomingCallOutstanding())
			{
			LOGTEXT(_L8("SetAmbiguousDataFaxCallRinging()\tNotify: SetPreAllocCall on DataLine"));
			iDataLine->SetPreAllocCall();
			}
		}
	else 
		LOGTEXT(_L8("SetAmbiguousDataFaxCallRinging()\tNo DataLine has been found; this may be a voice call"));
	}

void CPhoneHayes::StopRinging()
	{
	if (iDataLine)
		{
		(void)iDataLine->StopMyCallRinging();
		iDataLine->ResetPreAllocCall();	// this may not revert the call to PreAlloc status as
										// the call may have been opened by a client but not
										// answered.
		}
	if (iVoiceLine)
		{
		(void)iVoiceLine->StopMyCallRinging();
		iVoiceLine->ResetPreAllocCall();	// ditto
		}
	}

void CPhoneHayes::StopRingCounter() const
	{
	iPhoneGlobals->iNotificationStore->RemoveEventFromLastEvents(ERingOccurred);
	iWaitForCall->ResetRingCounter();
	}

void CPhoneHayes::SetHookStatus(RCall::THookStatus aHookStatus)
	{
	if (iDataLine)
		iDataLine->SetCallsHookStatus(aHookStatus);
	
	}

TBool CPhoneHayes::CheckForOutstandingAnswer() const
//
//	Returns TRUE if any call in the system has AnswerIncomingCall() outstanding on it.
//
	{
	TBool check=EFalse;
	if (iDataLine)
		check = iDataLine->CheckForOutstandingAnswer();
	
	if (!check && iVoiceLine)
		check = iVoiceLine->CheckForOutstandingAnswer();
	return check;
	}

void CPhoneHayes::CancelOtherRingingCall(CLineHayes* aLine) const
	{
	if (iDataLine && aLine!=iDataLine)
		{
		(void)iDataLine->StopMyCallRinging();
		iDataLine->ResetPreAllocCall();	
		}
	if (iVoiceLine && aLine!=iVoiceLine)
		{
		(void)iVoiceLine->StopMyCallRinging();
		iVoiceLine->ResetPreAllocCall();	
		}
	}

const CArrayFixFlat<TInt>* CPhoneHayes::ArrayOfMemberDataSizes(const TInt /*aIpc*/) const
	{
	return iSizeOfMemberData;
	}
