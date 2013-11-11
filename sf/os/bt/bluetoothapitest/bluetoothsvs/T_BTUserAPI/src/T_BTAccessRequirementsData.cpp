/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "T_BTAccessRequirementsData.h"

/*@{*/
//Parameters
_LIT(KAuthentication,										"authentication");
_LIT(KAuthorisation,										"authorisation");
_LIT(KEncryption,											"encryption");
_LIT(KDenied,												"denied");
_LIT(KPasskeyMinLength,										"passkeyminlength");
_LIT(KExpected, 											"expected");

//Commands
_LIT(KCmdTBTAccessRequirements,								"TBTAccessRequirements");
_LIT(KCmdSetAuthentication,									"SetAuthentication");
_LIT(KCmdSetAuthorisation,									"SetAuthorisation");
_LIT(KCmdSetEncryption,										"SetEncryption");
_LIT(KCmdSetDenied,											"SetDenied");
_LIT(KCmdSetPasskeyMinLength,								"SetPasskeyMinLength");
_LIT(KCmdAuthenticationRequired,							"AuthenticationRequired");
_LIT(KCmdAuthorisationRequired,								"AuthorisationRequired");
_LIT(KCmdEncryptionRequired,								"EncryptionRequired");
_LIT(KCmdDenied,											"Denied");
_LIT(KCmdPasskeyMinLength,									"PasskeyMinLength");

//	Logging
_LIT(KLogMissingParameter,			"Missing parameter '%S'");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_BTAccessRequirementsData* CT_BTAccessRequirementsData::NewL()
	{
	CT_BTAccessRequirementsData*	ret=new (ELeave) CT_BTAccessRequirementsData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_BTAccessRequirementsData::CT_BTAccessRequirementsData()
:	iData(NULL)
	{
	}

void CT_BTAccessRequirementsData::ConstructL()
	{
	}

CT_BTAccessRequirementsData::~CT_BTAccessRequirementsData()
	{
	DestroyData();
	}

//	Service methods
TAny* CT_BTAccessRequirementsData::GetObject()
	{
	return iData;
	}

void CT_BTAccessRequirementsData::SetObjectL(TAny* aAny)
	{
	//called when loading up persistent data
	DestroyData();
	iData = static_cast<TBTAccessRequirements*> (aAny);
	}


void CT_BTAccessRequirementsData::DisownObjectL()
	{
	iData=NULL;
	}

void CT_BTAccessRequirementsData::DestroyData()
	{
	delete iData;
	iData=NULL;	
	}

inline TCleanupOperation CT_BTAccessRequirementsData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_BTAccessRequirementsData::CleanupOperation(TAny* aAny)
	{
	TBTAccessRequirements* arg=static_cast<TBTAccessRequirements*>(aAny);
	delete arg;
	}

TBool CT_BTAccessRequirementsData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;
	if ( aCommand==KCmdTBTAccessRequirements )
		{
		DoCmdTBTAccessRequirements();
		}
	else if ( aCommand==KCmdSetAuthentication )
		{
		DoCmdSetAuthentication(aSection);
		}
	else if ( aCommand==KCmdSetAuthorisation )
		{
		DoCmdSetAuthorisation(aSection);
		}
	else if ( aCommand==KCmdSetEncryption )
		{
		DoCmdSetEncryption(aSection);
		}
	else if ( aCommand==KCmdSetDenied )
		{
		DoCmdSetDenied(aSection);
		}
	else if ( aCommand==KCmdSetPasskeyMinLength )
		{
		DoCmdSetPasskeyMinLength(aSection);
		}
	else if ( aCommand==KCmdAuthenticationRequired )
		{
		DoCmdAuthenticationRequired(aSection);
		}
	else if ( aCommand==KCmdAuthorisationRequired )
		{
		DoCmdAuthorisationRequired(aSection);
		}
	else if ( aCommand==KCmdEncryptionRequired )
		{
		DoCmdEncryptionRequired(aSection);
		}
	else if ( aCommand==KCmdDenied )
		{
		DoCmdDenied(aSection);
		}
	else if ( aCommand==KCmdPasskeyMinLength )
		{
		DoCmdPasskeyMinLength(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test TBTAccessRequirements()
*/
void CT_BTAccessRequirementsData::DoCmdTBTAccessRequirements()
	{
	INFO_PRINTF1(_L("TBTAccessRequirements Constructor Call"));
	DestroyData();
	iData = new (ELeave) TBTAccessRequirements();
	}


/**
Test SetAuthentication()
*/
void CT_BTAccessRequirementsData::DoCmdSetAuthentication(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTAccessRequirements SetAuthentication Call"));

	TBool	authentication = EFalse;
	if(GetBoolFromConfig(aSection, KAuthentication(), authentication) )
		{
		iData->SetAuthentication(authentication);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KAuthentication());
		SetBlockResult(EFail);
		}
	}


/**
Test SetAuthorisation()
*/
void CT_BTAccessRequirementsData::DoCmdSetAuthorisation(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTAccessRequirements SetAuthorisation Call"));

	TBool	authorisation = EFalse;
	if(GetBoolFromConfig(aSection, KAuthorisation(), authorisation) )
		{
		iData->SetAuthorisation(authorisation);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KAuthorisation());
		SetBlockResult(EFail);
		}
	}


/**
Test SetEncryption()
*/
void CT_BTAccessRequirementsData::DoCmdSetEncryption(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTAccessRequirements SetEncryption Call"));

	TBool	encryption = EFalse;
	if(GetBoolFromConfig(aSection, KEncryption(), encryption) )
		{
		iData->SetEncryption(encryption);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KEncryption());
		SetBlockResult(EFail);
		}
	}


/**
Test SetDenied()
*/
void CT_BTAccessRequirementsData::DoCmdSetDenied(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTAccessRequirements SetDenied Call"));

	TBool	denied = EFalse;
	if(GetBoolFromConfig(aSection, KDenied(), denied) )
		{
		iData->SetDenied(denied);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KDenied());
		SetBlockResult(EFail);
		}
	}


/**
Test SetPasskeyMinLength()
*/
void CT_BTAccessRequirementsData::DoCmdSetPasskeyMinLength(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTAccessRequirements SetPasskeyMinLength Call"));

	TInt	passkeyMinLength = EFalse;
	if(GetIntFromConfig(aSection, KPasskeyMinLength(), passkeyMinLength) )
		{
		TInt result = iData->SetPasskeyMinLength(passkeyMinLength);
		if(result != KErrNone)
			{
			ERR_PRINTF2(_L("**** TBTAccessRequirements SetPasskeyMinLength failed with error %d"), result);
			SetError(result);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KPasskeyMinLength());
		SetBlockResult(EFail);
		}
	}


/**
Test AuthenticationRequired()
*/
void CT_BTAccessRequirementsData::DoCmdAuthenticationRequired(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTAccessRequirements AuthenticationRequired Call"));
	
	TBool authentification = iData->AuthenticationRequired();
	INFO_PRINTF2(_L("TBTAccessRequirements AuthenticationRequired result: actual = %d"), authentification);
	
	TBool	expected = EFalse;
	if(GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		if( expected != authentification )
			{
			ERR_PRINTF1(_L("AuthenticationRequired not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}


/**
Test AuthorisationRequired()
*/
void CT_BTAccessRequirementsData::DoCmdAuthorisationRequired(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTAccessRequirements AuthorisationRequired Call"));
	
	TBool authorisation = iData->AuthorisationRequired();
	INFO_PRINTF2(_L("TBTAccessRequirements AuthorisationRequired result: actual = %d"), authorisation);
	
	TBool	expected = EFalse;
	if(GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		if( expected != authorisation )
			{
			ERR_PRINTF1(_L("AuthorisationRequired not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}


/**
Test EncryptionRequired()
*/
void CT_BTAccessRequirementsData::DoCmdEncryptionRequired(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTAccessRequirements EncryptionRequired Call"));
	
	TBool encryption = iData->EncryptionRequired();
	INFO_PRINTF2(_L("TBTAccessRequirements EncryptionRequired result: actual = %d"), encryption);
	
	TBool	expected = EFalse;
	if(GetBoolFromConfig(aSection, KExpected(), expected))
		{
		if( expected != encryption )
			{
			ERR_PRINTF1(_L("EncryptionRequired not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}


/**
Test Denied()
*/
void CT_BTAccessRequirementsData::DoCmdDenied(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTAccessRequirements Denied Call"));
	
	TBool denied = iData->Denied();
	INFO_PRINTF2(_L("TBTAccessRequirements Denied result: actual = %d"), denied);
	
	TBool	expected = EFalse;
	if(GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		if( expected != denied )
			{
			ERR_PRINTF1(_L("Denied not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}


/**
Test PasskeyMinLength()
*/
void CT_BTAccessRequirementsData::DoCmdPasskeyMinLength(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTAccessRequirements PasskeyMinLength Call"));
	
	TUint passkeyMinLength = iData->PasskeyMinLength();
	INFO_PRINTF2(_L("TBTAccessRequirements PasskeyMinLength result: actual = %d"), passkeyMinLength);
	
	TInt	expected = EFalse;
	if(GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if( expected != passkeyMinLength )
			{
			ERR_PRINTF1(_L("PasskeyMinLength not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}

