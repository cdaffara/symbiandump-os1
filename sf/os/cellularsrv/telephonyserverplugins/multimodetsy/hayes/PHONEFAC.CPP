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
// Phone Factory Class and DLL entry point
// 
//

#include "PHONEFAC.H"
#include "mSLOGGER.H"
#include "mPHONE.H"

_LIT(KPhoneName,"GsmPhone1");

const TInt KNumberOfPhones=1;


//
// First Ordinal Functions
//
extern "C"
	{
	IMPORT_C CPhoneFactoryBase* LibEntry(void);	// Force "Proper Name" export
	}

EXPORT_C CPhoneFactoryBase* LibEntry()
	{
	return new CPhoneFactoryHayes;
	}

//
// Hayes Phone Factory Functions
//
CPhoneFactoryHayes::CPhoneFactoryHayes()
	{
	iVersion=TVersion(KEtelMajorVersionNumber,KEtelMinorVersionNumber,KEtelBuildVersionNumber);
	}

CPhoneFactoryHayes::~CPhoneFactoryHayes()
	{}

CPhoneBase* CPhoneFactoryHayes::NewPhoneL(const TDesC& aName)
//
//	Return a new phone object
//
	{
	if (aName.CompareF(KPhoneName)!=0)
		{
		User::Leave(KErrNotFound);
		}
	return CPhoneMobile::NewL();
	}

TInt CPhoneFactoryHayes::EnumeratePhones()
//
//	Core TSY only handles one phone
//
	{
	return KNumberOfPhones;
	}

TInt CPhoneFactoryHayes::GetPhoneInfo(const TInt aIndex, RTelServer::TPhoneInfo& aInfo)
//
//	Return name of phone, number of lines and type of network
//
	{
	if(aIndex!=0)
		return KErrNotFound;
	aInfo.iName.Copy(KPhoneName);
	aInfo.iNumberOfLines=KNumberOfLines;
	aInfo.iNetworkType=RTelServer::ENetworkTypeMobileDigital;
	aInfo.iExtensions=(TUint)KETelExtMultimodeV1;
	return KErrNone;
	}

TBool CPhoneFactoryHayes::IsSupported(const TInt aMixin)
/** *  *
 * This method returnes true or false on the question if a specifik kind of functionality excists.  
 * @param aMixin contains information of manufacturer, model, revision and serialnumber. If the phone doesen't support some some of these, an empty string is returned. 
 * @return error code. KErrNone */


	{

	switch (aMixin)
		{
	case KETelFuncMobileNetwork:
	case KETelFuncMobileIdentity:
	case KETelFuncMobilePower:
	case KETelFuncMobileSignal:
	case KETelFuncMobileDataCall:
	case KETelFuncMobileSmsMessaging:
	case KETelFuncMobilePhonebook:
	case KETelFuncMobileSmsStore:
	case KETelFuncMobileOwnNumberStore:
		return ETrue;
	default:
		return EFalse;
		}
	}

