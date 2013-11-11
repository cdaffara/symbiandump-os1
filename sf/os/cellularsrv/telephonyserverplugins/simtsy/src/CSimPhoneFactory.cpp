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
// SIMPHONEFACTORY.CPP
// Phone Factory Class for the Simulator TSY
// This file contains the implementation for the CSimPhoneFactory Class.
// 
//

/**
 @file
*/

#include "CSimPhoneFactory.h"
#include "CSimPhone.h"
#include "../inc/utils.h"

extern "C"
/**
*	This function must be exported as the first ordinal from the DLL.
*	The ETel Server will call this function to create a phone factory class,
*	from which ultimately all other classes are derived.
*/
	{
	IMPORT_C CPhoneFactoryBase* LibEntry(void);
	}

EXPORT_C CPhoneFactoryBase* LibEntry()
/**
*	Create a phone factory class.
*/
	{
	return new CSimPhoneFactory;
	}

CSimPhoneFactory::CSimPhoneFactory()
/**
*	Return the DLL version number.
*/
	{
	iVersion=TVersion(KEtelMajorVersionNumber,KEtelMinorVersionNumber,KEtelBuildVersionNumber);
	}

CSimPhoneFactory::~CSimPhoneFactory()
	{}


CPhoneBase* CSimPhoneFactory::NewPhoneL(const TDesC& aName)
/**
* Creates and returns a pointer to a new phone object.
*
* @param aName Name of the GSM phone ('GsmPhone')
* @return CPhoneBase pointer to the new CSimPhone object, if the phone name is valid
*         NULL otherwise 
* @leave Leave This function may leave for many reasons
*/
	{
	if (aName.CompareF(KPhoneName)!=0)
		{
		User::Leave(KErrNotFound);
		}
	CSimPhone* newPhone= CSimPhone::NewL(iPhoneBaseRef);
	return newPhone;
	}

TInt CSimPhoneFactory::EnumeratePhones()
/**
* Returns the number of phones supported by the TSY. 
*
* @return TInt Number of phone supported.  Only 1 phone is supported.
*/
	{
	return KNumberOfPhones;
	}

TInt CSimPhoneFactory::GetPhoneInfo(const TInt aIndex, RTelServer::TPhoneInfo& aInfo)
/**
* Retrieves phone information
*
* @param aIndex There is only one phone, so the valid index is zero.
* @param aInfo Structure containing name of phone, number of lines and type of network
* @return TInt An error code.
*/
	{
	if(aIndex!=0)
		return KErrNotFound;
	aInfo.iName.Copy(KPhoneName);
	aInfo.iNumberOfLines=KNumberOfLines;
	aInfo.iNetworkType=RTelServer::ENetworkTypeMobileDigital;
	aInfo.iExtensions=(TUint)0;		// Note that the Multimode ETel API extension number should be inserted here.
	return KErrNone;
	}

TBool CSimPhoneFactory::IsSupported(const TInt aMixin)
/**
* Returns a boolean indicating the support of extended functionality.
*
* @param aMixin Limited subset of Muiltimode Etel API functionality
* @return ETrue aMixin functionality is supported
*		  EFalse aMixin functionality is not supported
*/
	{
	switch (aMixin)
		{
	case KETelExtMultimodeV1:  // 3000 is unique reference for Multimode Etel v1.0 API
	case KETelFuncMobileNetwork:
	case KETelFuncMobilePower:
	case KETelFuncMobileSignal:
	case KETelFuncMobileIndicator:
	case KETelFuncMobileDTMF:
	case KETelFuncMobileDataCall:
	case KETelFuncMobileEmergencyCall:
	case KETelFuncMobileSmsMessaging:
	case KETelFuncMobilePhonebook:
	case KETelFuncMobileOwnNumberStore:
	case KETelFuncMobileSmsStore:
//	case KETelFuncMobileSmspStore:
	case KETelExtPcktV1:	//Packet API
	case KETelExtSatV1:
	//security is not supported by alll modules of the tsy
	case RMobileCall::KETelMobileDataCallParamsV1:
	case RMobileCall::KETel3rdPartyCallParamsV1:
		return ETrue;
	case KETelExtMultimodeV2:
	case KETelExtPcktV2:	//Packet API	
	case KETelExtSatV2:	
 	case KEtelFuncMobileMultimediaCallSettings:
	case KEtelFuncMobileNetworkSecurity:
 	case KEtelFuncMobileUSIMApplications:
 		return CheckSupportAgainstVersion(2);
	case KETelExtMultimodeV3:
	case KETelExtPcktV3:	//Packet API	
 		return CheckSupportAgainstVersion(3);	
	case KETelExtMultimodeV4:
 		return CheckSupportAgainstVersion(4);
	case KEtelExtMultimodeV5:
 		return CheckSupportAgainstVersion(5);	
	case KEtelExtMultimodeV6:
 		return CheckSupportAgainstVersion(6);
	case KEtelExtMultimodeV9:
	case KEtelFuncCellInfo:
		return CheckSupportAgainstVersion(9);
	default:
		return EFalse;
		}
	}

TBool CSimPhoneFactory::CheckSupportAgainstVersion(const TInt& aVersion)
	{
	TBool ret = EFalse;
	
	if(iPhoneBaseRef == NULL)
		{
		ret = ETrue;
		}
	else
		{
		CSimPhone* thePhone = static_cast<CSimPhone*>(iPhoneBaseRef);
		if(thePhone->iSimTsyVersion >= aVersion)
			{
			ret = ETrue;
			}
		else
			{
			ret = EFalse;
			}
	}
	
	return ret;
	}
