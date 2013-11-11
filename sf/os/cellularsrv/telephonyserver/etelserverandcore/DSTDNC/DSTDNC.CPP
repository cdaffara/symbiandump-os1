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

#include "DSTDNC.H"
#include "DNCDEF.H"
#include "../DSTD/DDEF.H"

#include "../TETEL/Tetelcs.h"
 
//
//	CFaxDstdNc
//
CFaxDstdNc* CFaxDstdNc::NewL(CPhoneFactoryDummyBase* aFac)
//
//	Create new instances of Fax
//
	{
	CFaxDstdNc* fax=new(ELeave) CFaxDstdNc(aFac);
	CleanupStack::PushL(fax);
	fax->ConstructL();
	CleanupStack::Pop();
	return fax;
	}

CFaxDstdNc::CFaxDstdNc(CPhoneFactoryDummyBase* aFac)
	:CFaxDummyBase(aFac)
	{}

void CFaxDstdNc::ConstructL()
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

CFaxDstdNc::~CFaxDstdNc()
	{
	iChunk.Close();
	}

RHandleBase* CFaxDstdNc::GlobalKernelObjectHandle()
	{
	return &iChunk;
	}
//
//	CCallDstdNc
//
CCallDstdNc* CCallDstdNc::NewL(CPhoneFactoryDummyBase* aFac)
//
//	Static function to create new call
//
	{
	CCallDstdNc* call=new(ELeave) CCallDstdNc(aFac);
	CleanupStack::PushL(call);
	call->ConstructL();
	CleanupStack::Pop();
	return call;
	}

CCallDstdNc::CCallDstdNc(CPhoneFactoryDummyBase* aFac)
	:CCallDummyBase(aFac)
	{}

void CCallDstdNc::ConstructL()
//
//	For future use
//
	{
	CCallDummyBase::ConstructL();
	iLoanCommPort=EFalse;
	}

CCallDstdNc::~CCallDstdNc()
	{
	}

TInt CCallDstdNc::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
//
//	not supported by the core tsy
//
	{
	return KErrNotSupported;
	}

CTelObject* CCallDstdNc::OpenNewObjectByNameL(const TDesC&)
//
//	open aFax within the context of a call
//
	{
	return REINTERPRET_CAST(CTelObject*,CFaxDstdNc::NewL(FacPtr()));
	}

CTelObject* CCallDstdNc::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

TInt CLineDstdNc::EnumerateCall(const TTsyReqHandle aUid,TInt* aCount)
	{
	*aCount=1;
	ReqCompleted(aUid,KErrNone);
	return KErrNone;
	}

TInt CLineDstdNc::GetCallInfo(const TTsyReqHandle aUid,TCallInfoIndex* aCallInfo)
	{
	aCallInfo->iInfo.iStatus=DSTDNC_CALL_STATUS;
	aCallInfo->iInfo.iCallCapsFlags=DSTDNC_CALL_CAPS;

	ReqCompleted(aUid,KErrNone);
	return KErrNone;
	}

//
//	CLineDstdNc
//
CLineDstdNc* CLineDstdNc::NewL(const TDesC&,CPhoneFactoryDummyBase* aFac)
//
//	Static Function for a New Line
//
	{
	CLineDstdNc* line=new(ELeave) CLineDstdNc(aFac);
	CleanupStack::PushL(line);
	line->ConstructL();
	CleanupStack::Pop();
	return line;
	}

CLineDstdNc::CLineDstdNc(CPhoneFactoryDummyBase* aFac)
	:CLineDummyBase(aFac)
	{}

void CLineDstdNc::ConstructL()
	{}

CLineDstdNc::~CLineDstdNc()
	{}

CTelObject* CLineDstdNc::OpenNewObjectByNameL(const TDesC& aName)
//
//	Open subsession extension within context of a line
//
	{
	if (aName.Compare(DSTDNC_CALL_NAME)==KErrNone)
		return REINTERPRET_CAST(CCallBase*,CCallDstdNc::NewL(FacPtr()));
	return NULL;
	}

CTelObject* CLineDstdNc::OpenNewObjectL(TDes& aNewName)
	{
	aNewName.Append(DSTDNC_MODULE_NAME);
	aNewName.AppendNum(iNameIndex++);
	return REINTERPRET_CAST(CCallBase*,CCallDstdNc::NewL(FacPtr()));
	}

TInt CLineDstdNc::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
//
//	not supported by the core tsy
//
	{
	return KErrNotSupported;
	}

//
//	CPhoneDstdNc
//
CPhoneDstdNc* CPhoneDstdNc::NewL(CPhoneFactoryDummyBase* aFac)
//
//	Create new phone
//
	{
	CPhoneDstdNc* phone=new(ELeave) CPhoneDstdNc(aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CPhoneDstdNc::CPhoneDstdNc(CPhoneFactoryDummyBase* aFac)
	:CPhoneDummyBase(aFac)
	{}

void CPhoneDstdNc::ConstructL()
	{}

CPhoneDstdNc::~CPhoneDstdNc()
	{}

TInt CPhoneDstdNc::EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt* aNumLines)
	{
	*aNumLines=DSTDNC_NUMBER_OF_LINES;	//	just	one	line	on	this	phone
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDstdNc::GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex* aLineInfo)
	{
	if (aLineInfo->iIndex!=0)
		return(KErrArgument);

	aLineInfo->iInfo.iStatus=DSTDNC_LINE_STATUS;
	aLineInfo->iInfo.iName.Copy(DSTDNC_LINE_NAME);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

CTelObject::TReqMode CPhoneDstdNc::ReqModeL(const TInt)
//
//	Request Mode for Phone
//
	{
	CTelObject::TReqMode ret=0;
	return ret;
	}


TInt CPhoneDstdNc::ExtFunc(const TTsyReqHandle aTsyReqHandle, const TInt aIpc, const TDataPackage&)
	{
	switch(aIpc)
		{
	//Only CustomIPC2 is supported by this TSY.

	case ETestCustomIPC2: //IPC 9056
		ReqCompleted(aTsyReqHandle,KErrNone);
		return KErrNone;

	//Custom IPCs 1,3,4,5,6,7,8 & 9 are not supported for various reasons.
	//Typically a TSY will not explicitly list the IPCs it does not support but they are listed
	//here for clarity
	case ETestCustomIPC1: //IPC 1000055 - No support for this IPC in this TSY
	case ETestCustomIPC6: //IPC 1000500 - No support for this IPC in this TSY
	case ETestCustomIPC3: //IPC 1000099 - No support for this IPC in this TSY
	case ETestCustomIPC4: //IPC 1000100 - Should never be supported by a TSY as IPC is outside allocated range
	case ETestCustomIPC5: //IPC 1000499 - Should never be supported by a TSY as IPC is outside allocated range
	case ETestCustomIPC7: //IPC 1000099 - No support for this IPC in this TSY
	case ETestCustomIPC8: //IPC 1000600 - Should never be supported by a TSY as IPC is outside allocated range
	case ETestCustomIPC9: //IPC 999999  - Should never be supported by a TSY as IPC is outside allocated range
		return KErrNotSupported;
		}

	return KErrNotSupported;
	}


CTelObject* CPhoneDstdNc::OpenNewObjectByNameL(const TDesC& aName)
//
//	Open New Line within the Context of a Phone
//
	{
	if (aName.Compare(DSTDNC_LINE_NAME)==KErrNone)
		return REINTERPRET_CAST(CTelObject*,CLineDstdNc::NewL(aName,FacPtr()));
	return NULL;
	}

CTelObject* CPhoneDstdNc::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

void CPhoneDstdNc::Init()
	{}

TSecurityPolicy CPhoneDstdNc::GetRequiredPlatSecCaps(const TInt aIpc)
	{
	switch(aIpc)
		{
	case ETestCustomIPC2:
		return TSecurityPolicy(ECapabilityReadUserData);

	case ETestCustomIPC1: //Not supported by this TSY
	case ETestCustomIPC6: //Not supported by this TSY	
	case ETestCustomIPC3: //Not supported by this TSY
	case ETestCustomIPC4: //Not in range of IPCs allocated to Custom IPCs should be caught by Etel's policy table
	case ETestCustomIPC5: //Not in range of IPCs allocated to Custom IPCs should be caught by Etel's policy table
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
// DSTDNC Phone Factory Functions
//
CPhoneFactoryDstdNc* CPhoneFactoryDstdNc::NewL()
	{
	CPhoneFactoryDstdNc *This = new (ELeave) CPhoneFactoryDstdNc;
	CleanupStack::PushL(This);
	This->ConstructL();
	CleanupStack::Pop();
	return This;
	}

CPhoneFactoryDstdNc::CPhoneFactoryDstdNc()
//
//	Constructor for Phone Factory
//
	{
	iVersion=TVersion(	KTsyEtelMajorVersionNumber,
						KTsyEtelMinorVersionNumber,
						KTsyEtelBuildVersionNumber);
	}

CPhoneFactoryDstdNc::~CPhoneFactoryDstdNc()
//
//	D'tor
//
	{
	}

CPhoneBase* CPhoneFactoryDstdNc::NewPhoneL(const TDesC& aName)
//
//	Create New Instance of Phone
//
	{
	if (aName.Compare(DSTDNC_PHONE_NAME)==KErrNone)
		return CPhoneDstdNc::NewL(this);
	return NULL;
	}

TInt CPhoneFactoryDstdNc::GetPhoneInfo(const TInt aIndex, RTelServer::TPhoneInfo& aInfo)
//
//	Get Phone Info
//
	{
	switch (aIndex)
		{
	case 0:
		aInfo.iNetworkType=DSTDNC_NETWORK_TYPE;
		aInfo.iName=DSTDNC_PHONE_NAME;
		aInfo.iNumberOfLines=DSTDNC_NUMBER_OF_LINES;
		return KErrNone;
	default:
		return KErrNotFound;
		}
	}

TInt CPhoneFactoryDstdNc::EnumeratePhones()
	{
	return (DSTDNC_NUMBER_OF_PHONES);
	}

//
// First Ordinal Functions
//
extern "C"
	{
	IMPORT_C CPhoneFactoryBase* LibEntry();	// Force "Proper Name" export
	}

EXPORT_C CPhoneFactoryBase* LibEntry()
	{
	CPhoneFactoryDstdNc* factory = NULL;
	TRAP_IGNORE(factory = CPhoneFactoryDstdNc::NewL());
	return factory;
	}
