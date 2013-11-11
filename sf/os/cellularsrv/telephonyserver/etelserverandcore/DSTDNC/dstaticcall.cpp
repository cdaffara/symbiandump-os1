// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "dstaticcall.h"
#include "../DSTD/DDEF.H"

// Literals used in the constructor

//
//	CCallStaticCall
//
CCallStaticCall* CCallStaticCall::NewL(CPhoneFactoryDummyBase* aFac)
//
//	Static function to create new call
//
	{
	CCallStaticCall* call=new(ELeave) CCallStaticCall(aFac);
	CleanupStack::PushL(call);
	call->ConstructL();
	CleanupStack::Pop();
	return call;
	}

CCallStaticCall::CCallStaticCall(CPhoneFactoryDummyBase* aFac)
	:CCallDummyBase(aFac)
	{}

void CCallStaticCall::ConstructL()
//
//	For future use
//
	{
	CCallDummyBase::ConstructL();
	}

CCallStaticCall::~CCallStaticCall()
	{
	}

TInt CCallStaticCall::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
//
//	not supported by the core tsy
//
	{
	return KErrNotSupported;
	}

CTelObject* CCallStaticCall::OpenNewObjectByNameL(const TDesC&)
//
//	open aFax within the context of a call
//
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CCallStaticCall::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

TInt CLineStaticCall::EnumerateCall(const TTsyReqHandle aUid,TInt* aCount)
	{
	*aCount=1;
	ReqCompleted(aUid,KErrNone);
	return KErrNone;
	}

TInt CLineStaticCall::GetCallInfo(const TTsyReqHandle aUid,TCallInfoIndex*)
	{
	ReqCompleted(aUid,KErrNone);
	return KErrNone;
	}

//
//	CLineStaticCall
//
CLineStaticCall* CLineStaticCall::NewL(const TDesC&,CPhoneFactoryDummyBase* aFac)
//
//	Static Function for a New Line
//
	{
	CLineStaticCall* line=new(ELeave) CLineStaticCall(aFac);
	CleanupStack::PushL(line);
	line->ConstructL();
	CleanupStack::Pop();
	return line;
	}

CLineStaticCall::CLineStaticCall(CPhoneFactoryDummyBase* aFac)
	:CLineDummyBase(aFac)
	{}

void CLineStaticCall::ConstructL()
	{
	iCall=CCallStaticCall::NewL(FacPtr());
	}

CLineStaticCall::~CLineStaticCall()
	{
	if (iCall)
		iCall->Close();
	}

CTelObject* CLineStaticCall::OpenNewObjectByNameL(const TDesC&)
//
//	Open subsession extension within context of a line
//
	{
	return NULL;
	}

CTelObject* CLineStaticCall::OpenNewObjectL(TDes&)
	{
	iCall->Open();
	return iCall;
	}

TInt CLineStaticCall::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
//
//	not supported by the core tsy
//
	{
	return KErrNotSupported;
	}

//
//	CPhoneStaticCall
//
CPhoneStaticCall* CPhoneStaticCall::NewL(CPhoneFactoryDummyBase* aFac)
//
//	Create new phone
//
	{
	CPhoneStaticCall* phone=new(ELeave) CPhoneStaticCall(aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CPhoneStaticCall::CPhoneStaticCall(CPhoneFactoryDummyBase* aFac)
	:CPhoneDummyBase(aFac)
	{}

void CPhoneStaticCall::ConstructL()
	{}

CPhoneStaticCall::~CPhoneStaticCall()
	{}

TInt CPhoneStaticCall::EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt*)
	{
	ReqCompleted(aTsyReqHandle,KErrNotSupported);
	return KErrNone;
	}

TInt CPhoneStaticCall::GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneStaticCall::ExtFunc(const TTsyReqHandle,const TInt,const TDataPackage&)
//
//	Not Supported by the Core
//
	{
	return KErrNotSupported;
	}

CTelObject* CPhoneStaticCall::OpenNewObjectByNameL(const TDesC& aName)
//
//	Open New Line within the Context of a Phone
//
	{
	if(aName.Compare(DSTATICCALL_LINE_NAME)==KErrNone)
		return REINTERPRET_CAST(CTelObject*,CLineStaticCall::NewL(aName,FacPtr()));
	return NULL;
	}

CTelObject* CPhoneStaticCall::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

void CPhoneStaticCall::Init()
	{}

//
// StaticCall Phone Factory Functions
//
CPhoneFactoryStaticCall* CPhoneFactoryStaticCall::NewL()
	{
	CPhoneFactoryStaticCall *This = new (ELeave) CPhoneFactoryStaticCall;
	CleanupStack::PushL(This);
	This->ConstructL();
	CleanupStack::Pop();
	return This;
	}

CPhoneFactoryStaticCall::CPhoneFactoryStaticCall()
//
//	Constructor for Phone Factory
//
	{
	iVersion=TVersion(	KTsyEtelMajorVersionNumber,
						KTsyEtelMinorVersionNumber,
						KTsyEtelBuildVersionNumber);
	}

CPhoneFactoryStaticCall::~CPhoneFactoryStaticCall()
//
//	D'tor
//
	{
	iSemaphore.Close();
	}

CPhoneBase* CPhoneFactoryStaticCall::NewPhoneL(const TDesC& aName)
//
//	Create New Instance of Phone
//
	{
	if (aName.Compare(DSTATICCALL_PHONE_NAME)==KErrNone)
		return CPhoneStaticCall::NewL(this);
	return NULL;
	}

TInt CPhoneFactoryStaticCall::GetPhoneInfo(const TInt aIndex, RTelServer::TPhoneInfo& aInfo)
//
//	Get Phone Info
//
	{
	switch (aIndex)
		{
	case 0:
		aInfo.iNetworkType=RTelServer::ENetworkTypeUnknown;
		aInfo.iName=DSTATICCALL_PHONE_NAME;
		aInfo.iNumberOfLines=1;
		return KErrNone;
	default:
		return KErrNotFound;
		}
	}

TInt CPhoneFactoryStaticCall::EnumeratePhones()
	{
	return 1;
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
	CPhoneFactoryStaticCall* factory = NULL;
	TRAP_IGNORE(factory = CPhoneFactoryStaticCall::NewL());
	return factory;
	}
