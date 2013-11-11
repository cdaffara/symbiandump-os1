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

#include "dstdiden.h"
#include "DDEF.H"
#include "didendef.h"

//
// CCallDstdIden
//
CCallDstdIden* CCallDstdIden::NewL(CPhoneFactoryDummyBase* aFac)
//
// Create new instance of CCallDstdIden
//
	{
	CCallDstdIden* call=new(ELeave)CCallDstdIden(aFac);
	CleanupStack::PushL(call);
	call->ConstructL();
	CleanupStack::Pop();
	return call;
	}

CCallDstdIden::CCallDstdIden(CPhoneFactoryDummyBase* aFac)
	:CCallDummyBase(aFac)
	{}

CCallDstdIden::~CCallDstdIden()
	{}

TInt CCallDstdIden::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CCallDstdIden::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	return REINTERPRET_CAST(CTelObject*,CFaxDstdIden::NewL(FacPtr()));
	}

CTelObject* CCallDstdIden::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CCallDstdIden::ReqModeL(const TInt aIpc)
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
		ret=KReqModeFlowControlObeyed;
		break;
	default:
		User::Leave(0); // Server error = Should not call this
		break;
		}
	return ret;
	}



	
//
//	CFaxDstdIden
//
CFaxDstdIden* CFaxDstdIden::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CFaxDstdIden* fax=new(ELeave) CFaxDstdIden(aFac);
	CleanupStack::PushL(fax);
	fax->ConstructL();
	CleanupStack::Pop();
	return fax;
	}

CFaxDstdIden::CFaxDstdIden(CPhoneFactoryDummyBase* aFac)
	:CFaxDummyBase(aFac)
	{}

void CFaxDstdIden::ConstructL()
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

CFaxDstdIden::~CFaxDstdIden()
	{
	iChunk.Close();
	}

RHandleBase* CFaxDstdIden::GlobalKernelObjectHandle()
	{
		return &iChunk;
	}

//
//	CLineDstdIden
//
CLineDstdIden* CLineDstdIden::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CLineDstdIden* line=new(ELeave) CLineDstdIden(aFac);
	CleanupStack::PushL(line);
	line->ConstructL();
	CleanupStack::Pop();
	return line;
	}

CLineDstdIden::CLineDstdIden(CPhoneFactoryDummyBase* aFac)
	:CLineDummyBase(aFac)
	{
	iFac=aFac;
	}

CLineDstdIden::~CLineDstdIden()
	{}

TInt CLineDstdIden::EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aCount)
	{
	*aCount=iNumberOfCalls;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDstdIden::GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex* aCallInfo)
	{
	aCallInfo->iInfo.iStatus=DSTD_CALL_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDstdIden::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CLineDstdIden::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	return REINTERPRET_CAST(CTelObject*,CCallDstdIden::NewL(iFac));
	}

CTelObject* CLineDstdIden::OpenNewObjectL(TDes& aNewName)
	{
	aNewName.Append(DSTDIDEN_MODULE_NAME);
	aNewName.AppendNum(iNameIndex++);
	return REINTERPRET_CAST(CTelObject*,CCallDstdIden::NewL(iFac));
	}

//
//	CPhoneDstdIden
//
CPhoneDstdIden* CPhoneDstdIden::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CPhoneDstdIden* phone=new(ELeave) CPhoneDstdIden(aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CPhoneDstdIden::CPhoneDstdIden(CPhoneFactoryDummyBase* aFac)
	:CPhoneDummyBase(aFac)
	{}

CPhoneDstdIden::~CPhoneDstdIden()
	{}

TInt CPhoneDstdIden::EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt* aNumLines)
	{
	*aNumLines=DSTD_NUMBER_OF_LINES;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDstdIden::GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex* aLineInfo)
	{
	if (aLineInfo->iIndex!=0)
		return(KErrArgument);

	aLineInfo->iInfo.iStatus=DSTD_LINE_STATUS;
	aLineInfo->iInfo.iName.Copy(DSTD_LINE_NAME);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

CTelObject::TReqMode CPhoneDstdIden::ReqModeL(const TInt aIpc)
//
//	Request Mode for Phone
//
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
	case EEtelPhoneNotifyModemDetected:
		ret=KReqModeMultipleCompletionEnabled;
		break;
	case EEtelPhoneEnumerateLines:
	case EEtelPhoneGetLineInfo:
	case EEtelPhoneGetCaps:
	case EEtelPhoneGetStatus:
	case EEtelPhoneGetInfo:
		ret=KReqModeFlowControlObeyed;
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return ret;

	}

TInt CPhoneDstdIden::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CPhoneDstdIden::OpenNewObjectByNameL(const TDesC& aName)
	{
	if (aName.Compare(DSTD_LINE_NAME)==KErrNone)
		return REINTERPRET_CAST(CTelObject*,CLineDstdIden::NewL(FacPtr()));
	return NULL;
	}

CTelObject* CPhoneDstdIden::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

void CPhoneDstdIden::Init()
	{}

//
//	FlowControl	Phone
//

//
//	CFaxFcIden
//
CFaxFcIden* CFaxFcIden::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CFaxFcIden* fax=new(ELeave) CFaxFcIden(aFac);
	CleanupStack::PushL(fax);
	fax->ConstructL();
	CleanupStack::Pop();
	return fax;
	}

CFaxFcIden::CFaxFcIden(CPhoneFactoryDummyBase* aFac)
	:CFaxDummyBase(aFac)
	{}

void CFaxFcIden::ConstructL()
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
	
RHandleBase* CFaxFcIden::GlobalKernelObjectHandle()
	{
		return &iChunk;
	}

CFaxFcIden::~CFaxFcIden()
	{
	iChunk.Close();
	}

//
//	CCallFcIden
//
CCallFcIden* CCallFcIden::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CCallFcIden* call=new(ELeave) CCallFcIden(aFac);
	CleanupStack::PushL(call);
	call->ConstructL();
	CleanupStack::Pop();
	return call;
	}

CCallFcIden::CCallFcIden(CPhoneFactoryDummyBase* aFac)
	:CCallDummyBase(aFac)
	{}

CCallFcIden::~CCallFcIden()
	{}



TInt CCallFcIden::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CCallFcIden::OpenNewObjectByNameL(const TDesC& )
	{
	return REINTERPRET_CAST(CTelObject*,CFaxFcIden::NewL(FacPtr()));
	}

CTelObject* CCallFcIden::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

//
//	CLineFcIden
//
CLineFcIden* CLineFcIden::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CLineFcIden* line=new(ELeave)CLineFcIden(aFac);
	CleanupStack::PushL(line);
	line->ConstructL();
	CleanupStack::Pop();
	return line;
	}

CLineFcIden::CLineFcIden(CPhoneFactoryDummyBase* aFac)
	:CLineDummyBase(aFac)
	{}

CLineFcIden::~CLineFcIden()
	{}

TInt CLineFcIden::EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aCount)
	{
	*aCount=iNumberOfCalls;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineFcIden::GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex* aCallInfo)
	{
	aCallInfo->iInfo.iStatus=DSTDFC_CALL_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineFcIden::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CLineFcIden::OpenNewObjectByNameL(const TDesC& aName)
	{
	if (aName.Compare(DSTDFC_CALL_NAME)==KErrNone)
		return REINTERPRET_CAST(CTelObject*,CCallFcIden::NewL(FacPtr()));
	return NULL;
	}

CTelObject* CLineFcIden::OpenNewObjectL(TDes& aNewName)
	{
	aNewName.Append(DSTDIDEN_MODULE_NAME);
	aNewName.AppendNum(iNameIndex++);
	return REINTERPRET_CAST(CTelObject*,CCallFcIden::NewL(FacPtr()));
	}

//
//	CPhoneFcIden
//
CPhoneFcIden* CPhoneFcIden::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CPhoneFcIden* phone=new(ELeave) CPhoneFcIden(aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CPhoneFcIden::CPhoneFcIden(CPhoneFactoryDummyBase* aFac)
	:CPhoneDummyBase(aFac)
	{}

void CPhoneFcIden::Init()
	{
	FlowControlSuspend();
	FacPtr()->QueueTimer(iOpenPhone,0,DSTDFC_OPEN_PHONE_TIMEOUT,CPhoneFcIden::OpenPhoneHandler,this);
	}

TInt CPhoneFcIden::OpenPhoneHandler(TAny* aPtr)
	{
	REINTERPRET_CAST(CPhoneFcIden*,aPtr)->FlowControlResume();
	REINTERPRET_CAST(CPhoneFcIden*,aPtr)->FacPtr()->ResetPending(REINTERPRET_CAST(CPhoneFcIden*,aPtr)->iOpenPhone );
	return KErrNone;
	}

CPhoneFcIden::~CPhoneFcIden()
	{
	FacPtr()->RemoveTimer(iOpenPhone);
	}

TInt CPhoneFcIden::EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt* aNumLines)
	{
	*aNumLines=DSTDFC_NUMBER_OF_LINES;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneFcIden::GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex* aLineInfo)
	{
	if (aLineInfo->iIndex!=0)
		return(KErrArgument);

	aLineInfo->iInfo.iStatus=DSTDFC_LINE_STATUS;
	aLineInfo->iInfo.iName.Copy(DSTDFC_LINE_NAME);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneFcIden::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CPhoneFcIden::OpenNewObjectByNameL(const TDesC& aName)
	{
	if (aName.Compare(DSTDFC_LINE_NAME)==KErrNone)
		return REINTERPRET_CAST(CTelObject*,CLineFcIden::NewL(FacPtr()));
	return NULL;
	}

CTelObject* CPhoneFcIden::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

//
//	CFaxPmIden
//
CFaxPmIden* CFaxPmIden::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CFaxPmIden* fax=new(ELeave) CFaxPmIden(aFac);
	CleanupStack::PushL(fax);
	fax->ConstructL();
	CleanupStack::Pop();
	return fax;
	}

CFaxPmIden::CFaxPmIden(CPhoneFactoryDummyBase* aFac)
	:CFaxDummyBase(aFac)
	{}

void CFaxPmIden::ConstructL()
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
	
RHandleBase* CFaxPmIden::GlobalKernelObjectHandle()
	{
		return &iChunk;
	}

CFaxPmIden::~CFaxPmIden()
	{
	iChunk.Close();
	}

//
//	CCallPmIden
//
CCallPmIden* CCallPmIden::NewL(TInt& aNumberOfCalls,CPhoneFactoryDummyBase* aFac)
	{
	CCallPmIden* call=new(ELeave) CCallPmIden(aNumberOfCalls,aFac);
	CleanupStack::PushL(call);
	call->ConstructL();
	CleanupStack::Pop();
	return call;
	}

CCallPmIden::CCallPmIden(TInt& aNumberOfCalls,CPhoneFactoryDummyBase* aFac)
	: CCallDummyBase(aFac),
	iNumberOfCalls(aNumberOfCalls)
	{}

CCallPmIden::~CCallPmIden()
	{
	iNumberOfCalls--;
	}

TInt CCallPmIden::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CCallPmIden::OpenNewObjectByNameL(const TDesC& )
	{
	return REINTERPRET_CAST(CTelObject*,CFaxPmIden::NewL(FacPtr()));
	}
CTelObject* CCallPmIden::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}


//
//	CLinePmIden
//
CLinePmIden* CLinePmIden::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CLinePmIden* line=new(ELeave) CLinePmIden(aFac);
	CleanupStack::PushL(line);
	line->ConstructL();
	CleanupStack::Pop();
	return line;
	}

CLinePmIden::CLinePmIden(CPhoneFactoryDummyBase* aFac)
	:CLineDummyBase(aFac)
	{}

void CLinePmIden::ConstructL()
	{
	CLineDummyBase::ConstructL();
	iNumberOfCalls=0;
	iCallName.SetLength(0);
	}

CLinePmIden::~CLinePmIden()
	{}

TInt CLinePmIden::EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aCount)
	{
	*aCount=iNumberOfCalls;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLinePmIden::GetCallInfo(const TTsyReqHandle aUid,TCallInfoIndex* aCallInfo)
	{
	aCallInfo->iInfo.iStatus=	DSTDPM_CALL_STATUS;
	aCallInfo->iInfo.iCallName.Copy(iCallName);

	ReqCompleted(aUid,KErrNone);
	return KErrNone;
	}

TInt CLinePmIden::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CLinePmIden::OpenNewObjectByNameL(const TDesC& aName)
	{
	iNumberOfCalls++;
	iCallName.Copy(aName);
	return REINTERPRET_CAST(CTelObject*,CCallPmIden::NewL(iNumberOfCalls,FacPtr()));
	}
CTelObject* CLinePmIden::OpenNewObjectL(TDes& aNewName)
	{
	iNumberOfCalls++;
	aNewName.Append(DSTDIDEN_MODULE_NAME);
	aNewName.AppendNum(iNameIndex++);
	iCallName.Copy(aNewName);
	return REINTERPRET_CAST(CTelObject*,CCallPmIden::NewL(iNumberOfCalls,FacPtr()));
	}

//
//	CPhonePmIden
//
CPhonePmIden* CPhonePmIden::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CPhonePmIden* phone=new(ELeave) CPhonePmIden(aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CPhonePmIden::CPhonePmIden(CPhoneFactoryDummyBase* aFac)
	:CPhoneDummyBase(aFac)
	{}

void CPhonePmIden::Init()
	{}

CPhonePmIden::~CPhonePmIden()
	{}

TInt CPhonePmIden::EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt* aNumLines)
	{
	*aNumLines=DSTDPM_NUMBER_OF_LINES;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhonePmIden::GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex* aLineInfo)
	{
	if (aLineInfo->iIndex!=0)
		return(KErrArgument);

	aLineInfo->iInfo.iStatus=DSTDPM_LINE_STATUS;
	aLineInfo->iInfo.iName.Copy(DSTDPM_LINE_NAME);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhonePmIden::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
	{
	return KErrNotSupported;
	}

CTelObject* CPhonePmIden::OpenNewObjectByNameL(const TDesC& aName)
	{
	if (aName.Compare(DSTDPM_LINE_NAME)==KErrNone)
		return REINTERPRET_CAST(CTelObject*,CLinePmIden::NewL(FacPtr()));
	return NULL;
	}
CTelObject* CPhonePmIden::OpenNewObjectL(TDes& /*aNewName*/)
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

EXPORT_C CPhoneFactoryBase* LibEntry()
	{
	CPhoneFactoryDstdIden* factory = NULL;
	TRAP_IGNORE(factory = CPhoneFactoryDstdIden::NewL());
	return factory;
	}

//
//	DSTDIDEN Phone	Factory	Functions
//

CPhoneFactoryDstdIden* CPhoneFactoryDstdIden::NewL()
	{
	CPhoneFactoryDstdIden *This = new (ELeave) CPhoneFactoryDstdIden;
	CleanupStack::PushL(This);
	This->ConstructL();
	CleanupStack::Pop();
	return This;
	}

CPhoneFactoryDstdIden::CPhoneFactoryDstdIden()
	{
	iVersion=TVersion(	KTsyEtelMajorVersionNumber,
						KTsyEtelMinorVersionNumber,
						KTsyEtelBuildVersionNumber);
	}

CPhoneFactoryDstdIden::~CPhoneFactoryDstdIden()
	{}

CPhoneBase* CPhoneFactoryDstdIden::NewPhoneL(const TDesC& aName)
	{
	if (aName.Compare(DSTD_PHONE_NAME)==KErrNone)
		return CPhoneDstdIden::NewL(this);
	if (aName.Compare(DSTDFC_PHONE_NAME)==KErrNone)
		return CPhoneFcIden::NewL(this);
	if (aName.Compare(DSTDPM_PHONE_NAME)==KErrNone)
		return CPhonePmIden::NewL(this);
	return NULL;
	}

TInt CPhoneFactoryDstdIden::GetPhoneInfo(const TInt aIndex,RTelServer::TPhoneInfo& aInfo)
	{
	switch(aIndex)
		{
	case 0:
		aInfo.iNetworkType=DSTD_NETWORK_TYPE;
		aInfo.iName=DSTD_PHONE_NAME;	// same as DSTD's phone name
		aInfo.iNumberOfLines=DSTD_NUMBER_OF_LINES;
		return KErrNone;
	default:
		return KErrNotFound;
		}
	}

TInt CPhoneFactoryDstdIden::EnumeratePhones()
	{
	TVersion version(KTsyEtelMajorVersionNumber,
					KTsyEtelMinorVersionNumber,
					KTsyEtelBuildVersionNumber);
	if (!QueryVersionSupported(version))
		return(KErrNotSupported);

	return (DSTDIDEN_NUMBER_OF_PHONES);
	}

