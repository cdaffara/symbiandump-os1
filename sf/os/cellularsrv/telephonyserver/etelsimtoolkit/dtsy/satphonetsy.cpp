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
// Dummy phone
// 
//

/**
 @file
*/

#include "sattsy.h"
#include "Etelsat.h"
#include <et_clsvr.h>
#include "Dsatlog.h"
#include <satcs.h>
#include "testdef.h"

#include "coretsy.h"


//
// First Ordinal Functions
//
extern "C"
	{
	IMPORT_C CPhoneFactoryBase* LibEntry();	// Force "Proper Name" export
	}

EXPORT_C CPhoneFactoryBase* LibEntry()
	{
	CPhoneFactoryDSatTsy* factory = NULL;
	TRAP_IGNORE(factory = CPhoneFactoryDSatTsy::NewL());
	return factory;
	}

/**************************************************************************/
//
//	CPhoneFactoryDSatTsy
//
/**************************************************************************/

CPhoneFactoryDSatTsy::CPhoneFactoryDSatTsy()
	{
	iVersion=TVersion(	KTsyEtelMajorVersionNumber,
						KTsyEtelMinorVersionNumber,
						KTsyEtelBuildVersionNumber);
	}

CPhoneFactoryDSatTsy* CPhoneFactoryDSatTsy::NewL()
	{
	CPhoneFactoryDSatTsy *This = new (ELeave) CPhoneFactoryDSatTsy;
	CleanupStack::PushL(This);
	This->ConstructL();
	CleanupStack::Pop();
	return This;
	}

CPhoneFactoryDSatTsy::~CPhoneFactoryDSatTsy()
	{
	LOGDESTROY();
	}

CPhoneBase * CPhoneFactoryDSatTsy::NewPhoneL(const TDesC& aName)
	{
	if (aName.Compare(DSATTSY_PHONE_NAME)==KErrNone)
		return CPhoneDSatTsy::NewL(this);
	return NULL;
	}

TBool CPhoneFactoryDSatTsy::IsSupported(const TInt /*aMixin*/)
	{
	return EFalse;
	}



/**************************************************************************/
//
//	CPhoneDSatTsy
//
/**************************************************************************/

CPhoneDSatTsy* CPhoneDSatTsy::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CPhoneDSatTsy* phone=new(ELeave) CPhoneDSatTsy(aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CPhoneDSatTsy::CPhoneDSatTsy(CPhoneFactoryDummyBase* aFac)
	:CPhoneDummyBase(aFac)
	{}

void CPhoneDSatTsy::ConstructL()
	{
	CPhoneDummyBase::ConstructL();
	LOGTEXT(_L8("CPhoneDSatTsy created"));
	}
  
CPhoneDSatTsy::~CPhoneDSatTsy()
	{
	LOGTEXT(_L8("CPhoneDSatTsy destroyed"));
	}

void CPhoneDSatTsy::Init()
/**
 * Server calls this function once it has created the sub-session
 * it gives the TSY chance to do any initialisation it may need to do for
 * this sub-session
 */
	{}

CTelObject* CPhoneDSatTsy::OpenNewObjectByNameL(const TDesC& aName)
/**
 * Server calls this function when a client is opening an object from the phone
 * for the first time.
 * Multiple clients opening handles to the same sub-session object will be dealt with
 * by the server - i.e. by reference counting
 */
	{


	if(aName.Compare(DSATTSY_SAT_NAME)==KErrNone)
	   return REINTERPRET_CAST(CTelObject*,CSatDSatTsy::NewL(FacPtr()));

	return NULL;
	}

CTelObject* CPhoneDSatTsy::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}


CTelObject::TReqMode CPhoneDSatTsy::ReqModeL(const TInt /*aIpc*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

TInt CPhoneDSatTsy::RegisterNotification(const TInt /*aIpc*/)
	{
	return(KErrNotSupported);
	}

TInt CPhoneDSatTsy::DeregisterNotification(const TInt /*aIpc*/)
	{
	return(KErrNotSupported);
	}

TInt CPhoneDSatTsy::NumberOfSlotsL(const TInt /*aIpc*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}


TInt CPhoneDSatTsy::ExtFunc(const TTsyReqHandle /*aTsyReqHandle*/,const TInt /*aIpc*/,
		 				 const TDataPackage& /*aPackage*/)
	{
	return(KErrNotSupported);
	}

TInt CPhoneDSatTsy::CancelService(const TInt /*aIpc*/,const TTsyReqHandle /*aTsyReqHandle*/)
	{
	return(KErrNotSupported);
	}


CPhoneDSatTsy* CPhoneDSatTsy::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CPhoneDSatTsy*,aPtr);
	}
