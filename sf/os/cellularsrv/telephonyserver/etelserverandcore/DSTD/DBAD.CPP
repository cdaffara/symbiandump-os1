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

#include <e32base.h>
#include "ET_PHONE.H"
#include "DDEF.H"
#include "DBAD.H"
#include "DBDEF.H"
#include "../DSTDNC/DNCDEF.H"


//
//	First Ordinal Functions
//
extern	"C"
	{
	IMPORT_C CPhoneFactoryBase* LibEntry();	//	Force "Proper Name" export
	}

EXPORT_C CPhoneFactoryBase* LibEntry()
	{
	return new CPhoneFactoryBad;
	}

//
//	Bad Phone Factory Functions
//
CPhoneFactoryBad::CPhoneFactoryBad()
	{
	iVersion=TVersion(KTsyEtelMajorVersionNumber,
					  KTsyEtelMinorVersionNumber,
					  KTsyEtelBuildVersionNumber);
	}

CPhoneFactoryBad::~CPhoneFactoryBad()
	{}

CPhoneBase* CPhoneFactoryBad::NewPhoneL(const TDesC&)
//
// Only pretended to have two phones!
//
	{
	User::Leave(KErrGeneral);
	return NULL;
	}

TInt CPhoneFactoryBad::GetPhoneInfo(const TInt aIndex,RTelServer::TPhoneInfo& aInfo)
//
// Pretending There are Two Phones DSTD and DSTDNC
//
	{
	switch(aIndex)
		{
	case 0:
		aInfo.iNetworkType=DSTDPM_NETWORK_TYPE;
		aInfo.iName=DSTD_PHONE_NAME;
		aInfo.iNumberOfLines=DSTDPM_NUMBER_OF_LINES;
		return KErrNone;
	case 1:
		aInfo.iNetworkType=DSTDNC_NETWORK_TYPE;
		aInfo.iName=DSTDNC_PHONE_NAME;
		aInfo.iNumberOfLines=DSTDNC_NUMBER_OF_LINES;
		return KErrNone;
	default:
		return KErrNotFound;
		}
	}

TInt CPhoneFactoryBad::EnumeratePhones()
//
// Return as Two Phones
//
	{
	return (DBAD_NUMBER_OF_PHONES);
	}

TBool CPhoneFactoryBad::IsSupported(const TInt)
	{
	return EFalse;
	}
