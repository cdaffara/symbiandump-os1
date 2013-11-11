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

#include "pckttsy.h"
#include "etelpckt.h"
#include <et_clsvr.h>
#include "dpcktlog.h"
#include <pcktcs.h>
#include "testdef.h"

#include "coretsy.h"


extern "C"
/**
 * First ordinal function - Force "Proper Name" export
 */
	{
	IMPORT_C CPhoneFactoryBase* LibEntry();	
	}

EXPORT_C CPhoneFactoryBase* LibEntry()
/**
 * Ordinal function.
 */
	{
	return new CPhoneFactoryDGprsTsy;
	}

/**************************************************************************/
//
//	CPhoneFactoryDGprsTsy
//
/**************************************************************************/

CPhoneFactoryDGprsTsy::CPhoneFactoryDGprsTsy()
	{
	iVersion=TVersion(	KTsyEtelMajorVersionNumber,
						KTsyEtelMinorVersionNumber,
						KTsyEtelBuildVersionNumber);
	}

CPhoneFactoryDGprsTsy::~CPhoneFactoryDGprsTsy()
	{
	LOGDESTROY();
	}

CPhoneBase * CPhoneFactoryDGprsTsy::NewPhoneL(const TDesC& aName)
	{
	LOGTEXT(_L8("CPhoneFactoryDGprsTsy: NewPhoneL()"));

	if (aName.Compare(DPCKTTSY_PHONE_NAME)==KErrNone)
		return CPhoneDGprsTsy::NewL(this);
	return NULL;
	}

TBool CPhoneFactoryDGprsTsy::IsSupported(const TInt /*aMixin*/)
	{
	return EFalse;
	}



/**************************************************************************/
//
//	CPhoneDGprsTsy
//
/**************************************************************************/

CPhoneDGprsTsy* CPhoneDGprsTsy::NewL(CPhoneFactoryDummyBase* aFac)
/**
 * NewL method - Standard 2 phase constructor.
 */
	{
	LOGTEXT(_L8("CPhoneDGprsTsy: NewL()"));

	CPhoneDGprsTsy* phone=new(ELeave) CPhoneDGprsTsy(aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CPhoneDGprsTsy::CPhoneDGprsTsy(CPhoneFactoryDummyBase* aFac)
	:CPhoneDummyBase(aFac)
	{}

void CPhoneDGprsTsy::ConstructL()
/**
 * ConstructL method
 */
	{
	CPhoneDummyBase::ConstructL();
	LOGTEXT(_L8("CPhoneDGprsTsy::ConstructL()"));
	}
  
CPhoneDGprsTsy::~CPhoneDGprsTsy()
/** 
 * Destructor
 */
	{
	LOGTEXT(_L8("CPhoneDGprsTsy::~CPhoneDGprsTsy"));
	}

void CPhoneDGprsTsy::Init()
/** 
 * Server calls this function once it has created the sub-session. It gives the TSY a chance 
 * to do any initialisation it may need to do for this sub-session.
 */
	{}

CTelObject* CPhoneDGprsTsy::OpenNewObjectByNameL(const TDesC& aName)
/**
 * Server calls this function when a client is opening an object from the phone for the 
 * first time. Multiple clients opening handles to the same sub-session object will be 
 * dealt with by the server - i.e. by reference counting.
 */
	{
	LOGTEXT(_L8("CPhoneDGprsTsy: OpenNewObjectByNameL() called"));

	if(aName.Compare(DPCKTTSY_PACKET_NAME)==KErrNone)
	   return REINTERPRET_CAST(CTelObject*,CGprsDGprsTsy::NewL(FacPtr()));

	return NULL;
	}

CTelObject* CPhoneDGprsTsy::OpenNewObjectL(TDes& /*aNewName*/)
/**
 * This method is not supported in this dummy TSY.
 */
	{
	LOGTEXT(_L8("CPhoneDGprsTsy: OpenNewObjectL, Not supported()"));

	User::Leave(KErrNotSupported);
	return NULL;
	}


CTelObject::TReqMode CPhoneDGprsTsy::ReqModeL(const TInt /*aIpc*/)
	{
	return 0;
	}

TInt CPhoneDGprsTsy::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNotSupported;
	}

TInt CPhoneDGprsTsy::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNotSupported;
	}

TInt CPhoneDGprsTsy::NumberOfSlotsL(const TInt /*aIpc*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}


TInt CPhoneDGprsTsy::ExtFunc(const TTsyReqHandle /*aTsyReqHandle*/,const TInt /*aIpc*/,
		 				 const TDataPackage& /*aPackage*/)
	{
	return KErrNotSupported;
	}

TInt CPhoneDGprsTsy::CancelService(const TInt /*aIpc*/,const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return KErrNotSupported;
	}


CPhoneDGprsTsy* CPhoneDGprsTsy::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CPhoneDGprsTsy*,aPtr);
	}
