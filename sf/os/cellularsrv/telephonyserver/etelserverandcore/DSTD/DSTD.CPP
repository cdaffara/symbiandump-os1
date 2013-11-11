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

#include "DSTD.H"
#include "DDEF.H"
#include "../TETEL/Tetelcs.h"

//
// CCallDstd
//
CCallDstd* CCallDstd::NewL(CPhoneFactoryDummyBase* aFac)
//
// Create new instance of CCallDstd
//
	{
	CCallDstd* call=new(ELeave)CCallDstd(aFac);
	CleanupStack::PushL(call);
	call->ConstructL();
	CleanupStack::Pop();
	return call;
	}

CCallDstd::CCallDstd(CPhoneFactoryDummyBase* aFac)
	:CCallDummyBase(aFac)
	{}

CCallDstd::~CCallDstd()
	{}

TInt CCallDstd::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CCallDstd::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	return REINTERPRET_CAST(CTelObject*,CFaxDstd::NewL(FacPtr()));
	}

CTelObject* CCallDstd::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CCallDstd::ReqModeL(const TInt aIpc)
//
//	Basic Request Mode for Call
//
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
	case EEtelCallRecoverDataPort:
	case EEtelCallAcquireOwnership:
		break;
	case EEtelCallNotifyStatusChange:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
	case EEtelCallNotifyHookChange:
		ret=KReqModeMultipleCompletionEnabled;
		break;

	case EEtelCallGetStatus:
	case EEtelCallGetCaps:
	case EEtelCallLoanDataPort:
	case EEtelCallDial:
	case EEtelCallConnect:
	case EEtelCallAnswer:
	case EEtelCallHangUp:
	case EEtelCallTransferOwnership:
	case EEtelCallGetBearerServiceInfo:
	case EEtelCallGetOwnershipStatus:
	case EEtelCallGetFaxSettings:
	case EEtelCallSetFaxSettings:
	case EEtelCallReferenceCount:
	case EEtelAdoptFaxSharedHeaderFile:
		ret=KReqModeFlowControlObeyed;
		break;
	default:
		User::Leave(0); // Server error = Should not call this
		break;
		}
	return ret;
	}



	
//
//	CFaxDstd
//
CFaxDstd* CFaxDstd::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CFaxDstd* fax=new(ELeave) CFaxDstd(aFac);
	CleanupStack::PushL(fax);
	fax->ConstructL();
	CleanupStack::Pop();
	return fax;
	}

CFaxDstd::CFaxDstd(CPhoneFactoryDummyBase* aFac)
	:CFaxDummyBase(aFac)
	{}

void CFaxDstd::ConstructL()
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

CFaxDstd::~CFaxDstd()
	{
	iChunk.Close();
	}

RHandleBase* CFaxDstd::GlobalKernelObjectHandle()
	{	
	return &iChunk;
	}


//
//	CLineDstd
//
CLineDstd* CLineDstd::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CLineDstd* line=new(ELeave) CLineDstd(aFac);
	CleanupStack::PushL(line);
	line->ConstructL();
	CleanupStack::Pop();
	return line;
	}

CLineDstd::CLineDstd(CPhoneFactoryDummyBase* aFac)
	:CLineDummyBase(aFac)
	{
	iFac=aFac;
	}

CLineDstd::~CLineDstd()
	{}

TInt CLineDstd::EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aCount)
	{
	*aCount=iNumberOfCalls;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDstd::GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex* aCallInfo)
	{
	aCallInfo->iInfo.iStatus=DSTD_CALL_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDstd::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CLineDstd::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	return REINTERPRET_CAST(CTelObject*,CCallDstd::NewL(iFac));
	}

CTelObject* CLineDstd::OpenNewObjectL(TDes& aNewName)
	{
	aNewName.Append(DSTD_MODULE_NAME);
	aNewName.AppendNum(iNameIndex++);
	return REINTERPRET_CAST(CTelObject*,CCallDstd::NewL(iFac));
	}

//
//	CPhoneDstd
//
CPhoneDstd* CPhoneDstd::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CPhoneDstd* phone=new(ELeave) CPhoneDstd(aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CPhoneDstd::CPhoneDstd(CPhoneFactoryDummyBase* aFac)
	:CPhoneDummyBase(aFac)
	{}

CPhoneDstd::~CPhoneDstd()
	{}

TInt CPhoneDstd::EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt* aNumLines)
	{
	*aNumLines=DSTD_NUMBER_OF_LINES;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDstd::GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex* aLineInfo)
	{
	
	if (aLineInfo->iIndex!=0)
		return(KErrArgument);

	aLineInfo->iInfo.iStatus=DSTD_LINE_STATUS;
	aLineInfo->iInfo.iName.Copy(DSTD_LINE_NAME);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

CTelObject::TReqMode CPhoneDstd::ReqModeL(const TInt aIpc)
//
//	Request Mode for Phone
//
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
	case ETestCustomIPC1:
	case ETestCustomIPC2:
	case ETestCustomIPC3:
	case ETestCustomIPC4:
	case ETestCustomIPC5:
	case ETestCustomIPC6:
	case ETestCustomIPC7:
	case ETestCustomIPC8:
	case ETestCustomIPC9:
		break;
	case EEtelPhoneNotifyModemDetected:
		ret=KReqModeMultipleCompletionEnabled;
		break;
	case EEtelPhoneEnumerateLines:
	case EEtelPhoneGetLineInfo:
	case EEtelPhoneGetCaps:
	case EEtelPhoneGetStatus:
	case EEtelPhoneGetInfo:
	case ETestEtelUnicodeSetAndNarrowGet:
	case ETestEtelNarrowSetAndGet:
	case ETestEtelNarrowSetAndUnicodeGet:
	case ETestEtelUnicodeSetAndGet:
	case ETestEtelDoubleUnicodeGet:
	case EETelPhoneSetEmergencyClient:
		ret=KReqModeFlowControlObeyed;
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return ret;

	}

TInt CPhoneDstd::ExtFunc(const TTsyReqHandle aTsyReqHandle, const TInt aIpc, const TDataPackage& aPackage)
	{

	switch (aIpc)
		{
	case ETestEtelNarrowSetAndGet:
		return TestNarrowSetAndGet(aTsyReqHandle, aPackage.Des1n(),aPackage.Des2n());
			
	case ETestEtelUnicodeSetAndNarrowGet:
		return TestUnicodeSetAndNarrowGet(aTsyReqHandle, aPackage.Des1n(),aPackage.Des2u());
	
	case ETestEtelUnicodeSetAndGet:
		return TestUnicodeSetAndGet(aTsyReqHandle, aPackage.Des1u(),aPackage.Des2u());
			
	case ETestEtelNarrowSetAndUnicodeGet:
		return TestNarrowSetAndUnicodeGet(aTsyReqHandle, aPackage.Des1n(),aPackage.Des2u());
		
	case ETestEtelDoubleUnicodeGet:
	    return TestDoubleUnicodeGet(aTsyReqHandle, aPackage.Des1u(), aPackage.Des2u());
	    
	//Custom IPC 1,2 &6 are supported by this TSY.
	case ETestCustomIPC1: //IPC 1000055
	case ETestCustomIPC2: //IPC 1000056
		ReqCompleted(aTsyReqHandle,KErrNone);
		return KErrNone;
	
	case EETelPhoneSetEmergencyClient:
		ReqCompleted(aTsyReqHandle,KErrNone);
		return KErrNone;

	case ETestCustomIPC6:
		return KErrCancel; //IPC 100500 (potentially a cancel request for IPC 9000)

	//Custom IPCs 3,4,5,7,8 & 9 are not supported for various reasons.
	//Typically a TSY will not explicitly list the IPCs it does not support but they are listed
	//here for clarity
	case ETestCustomIPC3: //IPC 1000099 - No support for this IPC in this TSY
	case ETestCustomIPC4: //IPC 1000100 - No support for this IPC in this TSY
	case ETestCustomIPC5: //IPC 1000499 - No support for this IPC in this TSY
	case ETestCustomIPC7: //IPC 1000099 - No support for this IPC in this TSY
	case ETestCustomIPC8: //IPC 1001000 - Should never be supported by a TSY as IPC is outside allocated range
	case ETestCustomIPC9: //IPC 999999  - Should never be supported by a TSY as IPC is outside allocated range
		return KErrNotSupported;
		}

	return KErrNotSupported;
	}

CTelObject* CPhoneDstd::OpenNewObjectByNameL(const TDesC& aName)
	{
	if (aName.Compare(DSTD_LINE_NAME)==KErrNone)
		return REINTERPRET_CAST(CTelObject*,CLineDstd::NewL(FacPtr()));
	return NULL;
	}

CTelObject* CPhoneDstd::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

void CPhoneDstd::Init()
	{}


TInt CPhoneDstd::TestNarrowSetAndGet(const TTsyReqHandle aTsyReqHandle, const TDesC8* aDes1, TDes8* aDes2)
	{
	if (aDes1->Compare(DSTD_NARROW)!=KErrNone)
		{
		TInt  extendedError = (KErrLicenseeSpecificCorrupt << 16) |
			                  (KErrCorrupt & 0x0000ffff);
		
		ReqCompleted(aTsyReqHandle,extendedError);
		}
	else
		{
		TInt  extendedError = (KErrLicenseeSpecificOkay << 16) |
			                  (KErrNone & 0x0000ffff);
		
		*aDes2=DSTD_NARROW;
		ReqCompleted(aTsyReqHandle,extendedError);
		}
	return KErrNone;	
	}

TInt CPhoneDstd::TestUnicodeSetAndNarrowGet(const TTsyReqHandle aTsyReqHandle, TDes8* aDes1, const TDesC16* aDes2)
	{
	if (aDes2->Compare(DSTD_UNICODE)!=KErrNone)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		*aDes1=DSTD_NARROW;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;	
	}

TInt CPhoneDstd::TestUnicodeSetAndGet(const TTsyReqHandle aTsyReqHandle, const TDesC16* aDes1, TDes16* aDes2)
	{
	if (aDes1->Compare(DSTD_UNICODE)!=KErrNone)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		*aDes2=DSTD_UNICODE;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;	
	}

TInt CPhoneDstd::TestNarrowSetAndUnicodeGet(const TTsyReqHandle aTsyReqHandle, const TDesC8* aDes1 , TDes16* aDes2)
	{
	if (aDes1->Compare(DSTD_NARROW)!=KErrNone)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		*aDes2=DSTD_UNICODE;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;	
	}

TInt CPhoneDstd::TestDoubleUnicodeGet(const TTsyReqHandle aTsyReqHandle, TDes16* aDes1 , TDes16* aDes2)
    {
    *aDes1=DSTD_UNICODE;
    *aDes2=DSTD_UNICODE;
    ReqCompleted(aTsyReqHandle,KErrNone);

    return KErrNone;    
    }

TSecurityPolicy CPhoneDstd::GetRequiredPlatSecCaps(const TInt aIpc)
	{
	switch(aIpc)
		{
	case ETestCustomIPC1:
		return TSecurityPolicy(ECapabilityNetworkServices);

	case ETestCustomIPC2:
		return TSecurityPolicy(ECapabilityTCB);

	case ETestCustomIPC6:
		return TSecurityPolicy(ECapabilityReadUserData, ECapabilityWriteUserData);

	case ETestCustomIPC3: //Not supported by this TSY
	case ETestCustomIPC4: //Not supported by this TSY
	case ETestCustomIPC5: //Not supported by this TSY
	case ETestCustomIPC7: //Not supported by this TSY
	case ETestCustomIPC8: //Not in range of IPCs allocated to Custom IPCs should be caught by Etel's policy table
	case ETestCustomIPC9: //Not in range of IPCs allocated to Custom IPCs should be caught by Etel's policy table
		return TSecurityPolicy(TSecurityPolicy::EAlwaysFail);

	default:
		//For any non-Custom IPCs return a policy that will always cause the capability check to pass
		return TSecurityPolicy(TSecurityPolicy::EAlwaysPass);
		}
	}	

//
//	FlowControl	Phone
//

//
//	CFaxFc
//
CFaxFc* CFaxFc::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CFaxFc* fax=new(ELeave) CFaxFc(aFac);
	CleanupStack::PushL(fax);
	fax->ConstructL();
	CleanupStack::Pop();
	return fax;
	}

CFaxFc::CFaxFc(CPhoneFactoryDummyBase* aFac)
	:CFaxDummyBase(aFac)
	{}

void CFaxFc::ConstructL()
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

CFaxFc::~CFaxFc()
	{
	iChunk.Close();
	}

RHandleBase* CFaxFc::GlobalKernelObjectHandle()
	{		
	return &iChunk;
	}

//
//	CCallFc
//
CCallFc* CCallFc::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CCallFc* call=new(ELeave) CCallFc(aFac);
	CleanupStack::PushL(call);
	call->ConstructL();
	CleanupStack::Pop();
	return call;
	}

CCallFc::CCallFc(CPhoneFactoryDummyBase* aFac)
	:CCallDummyBase(aFac)
	{}

CCallFc::~CCallFc()
	{}



TInt CCallFc::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CCallFc::OpenNewObjectByNameL(const TDesC& )
	{
	return REINTERPRET_CAST(CTelObject*,CFaxFc::NewL(FacPtr()));
	}

CTelObject* CCallFc::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

//
//	CLineFc
//
CLineFc* CLineFc::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CLineFc* line=new(ELeave)CLineFc(aFac);
	CleanupStack::PushL(line);
	line->ConstructL();
	CleanupStack::Pop();
	return line;
	}

CLineFc::CLineFc(CPhoneFactoryDummyBase* aFac)
	:CLineDummyBase(aFac)
	{}

CLineFc::~CLineFc()
	{}

TInt CLineFc::EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aCount)
	{
	*aCount=iNumberOfCalls;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineFc::GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex* aCallInfo)
	{
	aCallInfo->iInfo.iStatus=DSTDFC_CALL_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineFc::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CLineFc::OpenNewObjectByNameL(const TDesC& aName)
	{
	if (aName.Compare(DSTDFC_CALL_NAME)==KErrNone)
		return REINTERPRET_CAST(CTelObject*,CCallFc::NewL(FacPtr()));
	return NULL;
	}

CTelObject* CLineFc::OpenNewObjectL(TDes& aNewName)
	{
	aNewName.Append(DSTD_MODULE_NAME);
	aNewName.AppendNum(iNameIndex++);
	return REINTERPRET_CAST(CTelObject*,CCallFc::NewL(FacPtr()));
	}

//
//	CPhoneFc
//
CPhoneFc* CPhoneFc::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CPhoneFc* phone=new(ELeave) CPhoneFc(aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CPhoneFc::CPhoneFc(CPhoneFactoryDummyBase* aFac)
	:CPhoneDummyBase(aFac)
	{}

void CPhoneFc::Init()
	{
	FlowControlSuspend();
	FacPtr()->QueueTimer(iOpenPhone,0,DSTDFC_OPEN_PHONE_TIMEOUT,CPhoneFc::OpenPhoneHandler,this);
	}

TInt CPhoneFc::OpenPhoneHandler(TAny* aPtr)
	{
	REINTERPRET_CAST(CPhoneFc*,aPtr)->FlowControlResume();
	REINTERPRET_CAST(CPhoneFc*,aPtr)->FacPtr()->ResetPending(REINTERPRET_CAST(CPhoneFc*,aPtr)->iOpenPhone );
	return KErrNone;
	}

CPhoneFc::~CPhoneFc()
	{
	FacPtr()->RemoveTimer(iOpenPhone);
	}

TInt CPhoneFc::EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt* aNumLines)
	{
	*aNumLines=DSTDFC_NUMBER_OF_LINES;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneFc::GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex* aLineInfo)
	{
	if (aLineInfo->iIndex!=0)
		return(KErrArgument);

	aLineInfo->iInfo.iStatus=DSTDFC_LINE_STATUS;
	aLineInfo->iInfo.iName.Copy(DSTDFC_LINE_NAME);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneFc::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CPhoneFc::OpenNewObjectByNameL(const TDesC& aName)
	{
	if (aName.Compare(DSTDFC_LINE_NAME)==KErrNone)
		return REINTERPRET_CAST(CTelObject*,CLineFc::NewL(FacPtr()));
	return NULL;
	}

CTelObject* CPhoneFc::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

//
//	CFaxPm
//
CFaxPm* CFaxPm::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CFaxPm* fax=new(ELeave) CFaxPm(aFac);
	CleanupStack::PushL(fax);
	fax->ConstructL();	
	CleanupStack::Pop();
	return fax;
	}

CFaxPm::CFaxPm(CPhoneFactoryDummyBase* aFac)
	:CFaxDummyBase(aFac)
	{}

void CFaxPm::ConstructL()
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

CFaxPm::~CFaxPm()
	{
	iChunk.Close();
	}
	
RHandleBase* CFaxPm::GlobalKernelObjectHandle()
	{	
	return &iChunk;
	}


//
//	CCallPm
//
CCallPm* CCallPm::NewL(TInt& aNumberOfCalls,CPhoneFactoryDummyBase* aFac)
	{
	CCallPm* call=new(ELeave) CCallPm(aNumberOfCalls,aFac);
	CleanupStack::PushL(call);
	call->ConstructL();
	CleanupStack::Pop();
	return call;
	}

CCallPm::CCallPm(TInt& aNumberOfCalls,CPhoneFactoryDummyBase* aFac)
	: CCallDummyBase(aFac),
	iNumberOfCalls(aNumberOfCalls)
	{}

CCallPm::~CCallPm()
	{
	iNumberOfCalls--;
	}

TInt CCallPm::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CCallPm::OpenNewObjectByNameL(const TDesC& )
	{
	return REINTERPRET_CAST(CTelObject*,CFaxPm::NewL(FacPtr()));
	}
CTelObject* CCallPm::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}


//
//	CLinePm
//
CLinePm* CLinePm::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CLinePm* line=new(ELeave) CLinePm(aFac);
	CleanupStack::PushL(line);
	line->ConstructL();
	CleanupStack::Pop();
	return line;
	}

CLinePm::CLinePm(CPhoneFactoryDummyBase* aFac)
	:CLineDummyBase(aFac)
	{}

void CLinePm::ConstructL()
	{
	CLineDummyBase::ConstructL();
	iNumberOfCalls=0;
	iCallName.SetLength(0);
	}

CLinePm::~CLinePm()
	{}

TInt CLinePm::EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aCount)
	{
	*aCount=iNumberOfCalls;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLinePm::GetCallInfo(const TTsyReqHandle aUid,TCallInfoIndex* aCallInfo)
	{
	aCallInfo->iInfo.iStatus=	DSTDPM_CALL_STATUS;
	aCallInfo->iInfo.iCallName.Copy(iCallName);

	ReqCompleted(aUid,KErrNone);
	return KErrNone;
	}

TInt CLinePm::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CLinePm::OpenNewObjectByNameL(const TDesC& aName)
	{
	iNumberOfCalls++;
	iCallName.Copy(aName);
	return REINTERPRET_CAST(CTelObject*,CCallPm::NewL(iNumberOfCalls,FacPtr()));
	}
CTelObject* CLinePm::OpenNewObjectL(TDes& aNewName)
	{
	iNumberOfCalls++;
	aNewName.Append(DSTD_MODULE_NAME);
	aNewName.AppendNum(iNameIndex++);
	iCallName.Copy(aNewName);
	return REINTERPRET_CAST(CTelObject*,CCallPm::NewL(iNumberOfCalls,FacPtr()));
	}

//
//	CPhonePm
//
CPhonePm* CPhonePm::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CPhonePm* phone=new(ELeave) CPhonePm(aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CPhonePm::CPhonePm(CPhoneFactoryDummyBase* aFac)
	:CPhoneDummyBase(aFac)
	{}

void CPhonePm::Init()
	{}

CPhonePm::~CPhonePm()
	{}

TInt CPhonePm::EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt* aNumLines)
	{
	*aNumLines=DSTDPM_NUMBER_OF_LINES;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhonePm::GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex* aLineInfo)
	{
	if (aLineInfo->iIndex!=0)
		return(KErrArgument);

	aLineInfo->iInfo.iStatus=DSTDPM_LINE_STATUS;
	aLineInfo->iInfo.iName.Copy(DSTDPM_LINE_NAME);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhonePm::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CPhonePm::OpenNewObjectByNameL(const TDesC& aName)
	{
	if (aName.Compare(DSTDPM_LINE_NAME)==KErrNone)
		return REINTERPRET_CAST(CTelObject*,CLinePm::NewL(FacPtr()));
	return NULL;
	}
CTelObject* CPhonePm::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

//
//	First Ordinal Functions
//
extern	"C"
	{
	IMPORT_C CPhoneFactoryBase* LibEntry();	//	Force "Proper Name" export
	}

void CPhoneFactoryDstd::CloseObj(TAny* aObj)
/**
*	A utility function for cleaning up the stack.
*
* @param aObj a pointer to the CObject to close
*/
	{
	((CObject*)aObj)->Close();
	}

EXPORT_C CPhoneFactoryBase* LibEntry()
	{
	CPhoneFactoryDstd* factory = NULL;
	TRAP_IGNORE(factory = CPhoneFactoryDstd::NewL());
	return factory;
	}

//
//	DSTD Phone	Factory	Functions
//

CPhoneFactoryDstd* CPhoneFactoryDstd::NewL()
	{
	CPhoneFactoryDstd *phoneFac = new (ELeave) CPhoneFactoryDstd;
	TCleanupItem newPhoneFacClose(CloseObj,phoneFac);
	CleanupStack::PushL(newPhoneFacClose);
	phoneFac->ConstructL();
	CleanupStack::Pop();
	return phoneFac;
	}

CPhoneFactoryDstd::CPhoneFactoryDstd()
	{
	iVersion=TVersion(	KTsyEtelMajorVersionNumber,
						KTsyEtelMinorVersionNumber,
						KTsyEtelBuildVersionNumber);
	}

CPhoneFactoryDstd::~CPhoneFactoryDstd()
	{}

CPhoneBase* CPhoneFactoryDstd::NewPhoneL(const TDesC& aName)
	{
	if (aName.Compare(DSTD_PHONE_NAME)==KErrNone)
		return CPhoneDstd::NewL(this);
	if (aName.Compare(DSTDFC_PHONE_NAME)==KErrNone)
		return CPhoneFc::NewL(this);
	if (aName.Compare(DSTDPM_PHONE_NAME)==KErrNone)
		return CPhonePm::NewL(this);
	if (aName.Compare(DSTD_SLOW_PHONE_NAME)==KErrNone)
		return CPhoneDstdSlow::NewL(this);
	return NULL;
	}

TInt CPhoneFactoryDstd::GetPhoneInfo(const TInt aIndex,RTelServer::TPhoneInfo& aInfo)
	{
	switch(aIndex)
		{
	case 0:
		aInfo.iNetworkType=DSTD_NETWORK_TYPE;
		aInfo.iName=DSTD_PHONE_NAME;
		aInfo.iNumberOfLines=DSTD_NUMBER_OF_LINES;
		return KErrNone;
	case 1:
		aInfo.iNetworkType=DSTDFC_NETWORK_TYPE;
		aInfo.iName=DSTDFC_PHONE_NAME;
		aInfo.iNumberOfLines=DSTDFC_NUMBER_OF_LINES;
		return KErrNone;
	case 2:
		aInfo.iNetworkType=DSTDPM_NETWORK_TYPE;
		aInfo.iName=DSTDPM_PHONE_NAME;
		aInfo.iNumberOfLines=DSTDPM_NUMBER_OF_LINES;
		return KErrNone;
	case 3:
		aInfo.iNetworkType=DSTDPM_NETWORK_TYPE;
		aInfo.iName=DSTD_SLOW_PHONE_NAME;
		aInfo.iNumberOfLines=DSTDPM_NUMBER_OF_LINES;
		return KErrNone;
	default:
		return KErrNotFound;
		}
	}

TInt CPhoneFactoryDstd::EnumeratePhones()
	{
	TVersion version(KTsyEtelMajorVersionNumber,
					KTsyEtelMinorVersionNumber,
					KTsyEtelBuildVersionNumber);
	if (!QueryVersionSupported(version))
		return(KErrNotSupported);

	return (DSTD_NUMBER_OF_PHONES);
	}


//
//	CPhoneDstdSlow
//
CPhoneDstdSlow* CPhoneDstdSlow::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CPhoneDstdSlow* phone=new(ELeave) CPhoneDstdSlow(aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CPhoneDstdSlow::CPhoneDstdSlow(CPhoneFactoryDummyBase* aFac)
	:CPhoneDstd(aFac)
	{}

CPhoneDstdSlow::~CPhoneDstdSlow()
	{}

TInt CPhoneDstdSlow::GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex* aLineInfo)
	{
	User::After(1000);
	return CPhoneDstd::GetLineInfo(aTsyReqHandle,aLineInfo);
	}

