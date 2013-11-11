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


#include "T_BTServiceSecurityData.h"


//Constant Literals used.

//Parameters
_LIT(KAuthentication,									"authentication");
_LIT(KAuthorisation,									"authorisation");
_LIT(KDenied,											"denied");
_LIT(KEncryption,										"encryption");
_LIT(KPasskeyminlength,									"passkeyminlength");
_LIT(KUid,												"uid");
_LIT(KServiceSecurity,									"btservicesecurity");

// Unfortunate fact of life
// authorisation 	TRUE = 2
// encryption		TRUE = 4
// denied 			TRUE = 8
// Therefore this additional parameters are neaded so that TRUE can be verified
_LIT(KAuthorisationInt,									"authorisation_int");
_LIT(KEncryptionInt,									"encryption_int");
_LIT(KDeniedInt,										"denied_int");


//TBTServiceSecurity
_LIT(KCmdConstruct, 									"Construct");
_LIT(KCmdSetAuthentication, 							"SetAuthentication");
_LIT(KCmdAuthenticationRequired, 						"AuthenticationRequired");
_LIT(KCmdSetAuthorisation, 								"SetAuthorisation");
_LIT(KCmdAuthorisationRequired, 						"AuthorisationRequired");
_LIT(KCmdSetDenied, 									"SetDenied");
_LIT(KCmdDenied, 										"Denied");
_LIT(KCmdSetEncryption, 								"SetEncryption");
_LIT(KCmdEncryptionRequired, 							"EncryptionRequired");
_LIT(KCmdSetPasskeyMinLength, 							"SetPasskeyMinLength");
_LIT(KCmdPasskeyMinLength, 								"PasskeyMinLength");
_LIT(KCmdSetUid, 										"SetUid");
_LIT(KCmdUid, 											"Uid");
_LIT(KCmdDistructor,									"~");

/*@}*/

CT_BTServiceSecurityData* CT_BTServiceSecurityData::NewL()
	{
	CT_BTServiceSecurityData*	ret=new (ELeave) CT_BTServiceSecurityData();
	return ret;
	}

CT_BTServiceSecurityData::CT_BTServiceSecurityData()
:   iBTServiceSecurity(NULL)
	{
	}
	
CT_BTServiceSecurityData::~CT_BTServiceSecurityData()
	{
	Cleanup();
	}
	
void CT_BTServiceSecurityData::Cleanup()
	{
	delete iBTServiceSecurity;	
	iBTServiceSecurity=NULL;	
	}
	
inline TCleanupOperation CT_BTServiceSecurityData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_BTServiceSecurityData::CleanupOperation(TAny* aAny)
	{
	TBTServiceSecurity* arg=static_cast<TBTServiceSecurity*>(aAny);
	delete arg;
	}


TBool CT_BTServiceSecurityData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool								retVal=ETrue;

	if ( aCommand==KCmdConstruct )
		{
		TBTSSConstruct(aSection);
		}					
	else if ( aCommand==KCmdSetAuthentication )
		{		
		TBTSSSetAuthentication(aSection);
		}
	else if ( aCommand==KCmdAuthenticationRequired )
		{				
		TBTSSAuthenticationRequired(aSection);
		}
	else if ( aCommand==KCmdSetAuthorisation )
		{		
		TBTSSSetAuthorisation(aSection);
		}
	else if ( aCommand==KCmdAuthorisationRequired )
		{				
		TBTSSAuthorisationRequired(aSection);	
		}	
	else if ( aCommand==KCmdSetDenied )
		{		
		TBTSSSetDenied(aSection);
		}
	else if ( aCommand==KCmdDenied )
		{						
		TBTSSDenied(aSection);
		}				
	else if ( aCommand==KCmdSetEncryption )
		{		
		TBTSSSetEncryption(aSection);	
		}
	else if ( aCommand==KCmdEncryptionRequired )
		{				
		TBTSSEncryptionRequired(aSection);
		}		
	else if ( aCommand==KCmdSetPasskeyMinLength )
		{		
		TBTSSSetPasskeyMinLength(aSection);
		}
	else if ( aCommand==KCmdPasskeyMinLength )
		{			
		TBTSSPasskeyMinLength(aSection);	
		}	
	else if ( aCommand==KCmdSetUid )
		{		
		TBTSSSetUid(aSection);
		}
	else if ( aCommand==KCmdUid )
		{				
		TBTSSUid(aSection);
		}		
	else if ( aCommand==KCmdDistructor )
		{				
		Cleanup();
		}	
	else
		{
		retVal=EFalse;
		}
	return retVal;
	}
	
void CT_BTServiceSecurityData::TBTSSConstruct(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTServiceSecurity Constructor Call"));
	TPtrC myServiceSecurityName;
	TBTServiceSecurity*	myServiceSecurity=NULL;
	if ( !GetStringFromConfig(aSection, KServiceSecurity(), myServiceSecurityName) )
		{
		//Use default constructor
		Cleanup();
		TRAPD( err, iBTServiceSecurity = new (ELeave) TBTServiceSecurity() );
		SetError( err);
		}
	else
		{
		TRAPD( err, myServiceSecurity=static_cast<TBTServiceSecurity*>(GetDataObjectL(myServiceSecurityName)) );	
		
		if ( (!err) && (myServiceSecurity!=NULL) )
			{
			Cleanup();
			TRAP( err, iBTServiceSecurity = new (ELeave) TBTServiceSecurity(*myServiceSecurity) );
		    SetError( err);
			}	
		else
			{
			ERR_PRINTF2(_L("TBTSSConstructFromServiceSecurity TSockAddr is NULL and err = %d"), err);
			SetBlockResult(EFail);				
			}	
		}
	}	
	
	
void CT_BTServiceSecurityData::TBTSSSetAuthentication(const TDesC& aSection)
	{		
	TBool	authentication;
	if( GetBoolFromConfig(aSection, KAuthentication(), authentication))
		{
		INFO_PRINTF2(_L("TBTServiceSecurity SetAuthentication (%d)"), authentication);
		iBTServiceSecurity->SetAuthentication(authentication);				
		}
	else
		{
		ERR_PRINTF1(_L("GetBoolFromConfig failed"));
		SetBlockResult(EFail);				
		}		
	}	
	
void CT_BTServiceSecurityData::TBTSSAuthenticationRequired(const TDesC& aSection)
	{		
	TBool	authentication;
	TBool authenticationReceived = iBTServiceSecurity->AuthenticationRequired();
	INFO_PRINTF2(_L("TBTServiceSecurity Authentication: actual   (%d)" ), authenticationReceived);
	if( GetBoolFromConfig(aSection, KAuthentication(), authentication))
		{
		INFO_PRINTF2(_L("TBTServiceSecurity Authentication: expected (%d)" ), authentication);
		if(authentication != authenticationReceived)
			{
			ERR_PRINTF1(_L("Authentication is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetBoolFromConfig failed"));
		SetBlockResult(EFail);	
		}
	}	
	
void CT_BTServiceSecurityData::TBTSSSetAuthorisation(const TDesC& aSection)
	{		
	TBool	authorisation;
	if( GetBoolFromConfig(aSection, KAuthorisation(), authorisation))
		{
		INFO_PRINTF2(_L("TBTServiceSecurity SetAuthorisation (%d)"), authorisation);
		iBTServiceSecurity->SetAuthorisation(authorisation);				
		}
	else
		{
		ERR_PRINTF1(_L("GetBoolFromConfig failed"));
		SetBlockResult(EFail);		
		}		
	}	
	
void CT_BTServiceSecurityData::TBTSSAuthorisationRequired(const TDesC& aSection)
	{		
	TBool	authorisation;
	TBool authorisationReceived = iBTServiceSecurity->AuthorisationRequired();
	INFO_PRINTF2(_L("TBTServiceSecurity Authorisation: actual   (%d)" ), authorisationReceived);
	
	if( GetBoolFromConfig(aSection, KAuthorisation(), authorisation))
		{
		INFO_PRINTF2(_L("TBTServiceSecurity Authorisation: expected (%d)" ), authorisation);
		if(authorisation != authorisationReceived)
			{
			ERR_PRINTF1(_L("Authorisation is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		// Try reading the Int version
		if (GetIntFromConfig(aSection, KAuthorisationInt(), authorisation))
			{
			INFO_PRINTF2(_L("TBTServiceSecurity Authorisation: expected (%d)" ), authorisation);
			if(authorisation != authorisationReceived)
				{
				ERR_PRINTF1(_L("Authorisation is not as expected!"));
				SetBlockResult(EFail);				
				}
			}
		else
			{
			ERR_PRINTF1(_L("GetBoolFromConfig and GetIntFromConfig failed"));
			SetBlockResult(EFail);	
			}
		}
	}
	
void CT_BTServiceSecurityData::TBTSSSetDenied(const TDesC& aSection)
	{
	TBool	denied;
	if( GetBoolFromConfig(aSection, KDenied(), denied))
		{
		INFO_PRINTF2(_L("TBTServiceSecurity SetDenied (%d)"), denied);
		iBTServiceSecurity->SetDenied(denied);				
		}
	else
		{
		ERR_PRINTF1(_L("GetBoolFromConfig failed"));
		SetBlockResult(EFail);	
		}
	}	
	
void CT_BTServiceSecurityData::TBTSSDenied(const TDesC& aSection)
	{		
	TBool denied;
	TBool deniedReceived = iBTServiceSecurity->Denied();
	INFO_PRINTF2(_L("TBTServiceSecurity Denied: actual   (%d)" ), deniedReceived);
	
	if( GetBoolFromConfig(aSection, KDenied(), denied))
		{
		INFO_PRINTF2(_L("TBTServiceSecurity Denied: expected (%d)" ), denied);
		if(denied != deniedReceived)
			{
			ERR_PRINTF1(_L("Denied is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		// Try reading the Int version
		if (GetIntFromConfig(aSection, KDeniedInt(), denied))
			{
			INFO_PRINTF2(_L("TBTServiceSecurity Denied: expected (%d)" ), denied);
			if(denied != deniedReceived)
				{
				ERR_PRINTF1(_L("Denied is not as expected!"));
				SetBlockResult(EFail);				
				}
			}
		else
			{
			ERR_PRINTF1(_L("GetBoolFromConfig and GetIntFromConfig failed"));
			SetBlockResult(EFail);	
			}
		}
	}	
	
void CT_BTServiceSecurityData::TBTSSSetEncryption(const TDesC& aSection)
	{	
	TBool	encryption;
	if( GetBoolFromConfig(aSection, KEncryption(), encryption))
		{
		INFO_PRINTF2(_L("TBTServiceSecurity SetEncryption (%d)"), encryption);	
		iBTServiceSecurity->SetEncryption(encryption);				
		}
	else
		{
		ERR_PRINTF1(_L("GetBoolFromConfig failed"));
		SetBlockResult(EFail);	
		}
	}	
	
void CT_BTServiceSecurityData::TBTSSEncryptionRequired(const TDesC& aSection)
	{		
	TBool	encryption;
	TBool encryptionReceived = iBTServiceSecurity->EncryptionRequired();
	INFO_PRINTF2(_L("TBTServiceSecurity Encryption: actual   (%d)" ), encryptionReceived);	
	if( GetBoolFromConfig(aSection, KEncryption(), encryption))
		{
		INFO_PRINTF2(_L("TBTServiceSecurity Encryption: expected (%d)" ), encryption);
		if(encryption != encryptionReceived)
			{
			ERR_PRINTF1(_L("Encryption is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		// Try reading the Int version
		if (GetIntFromConfig(aSection, KEncryptionInt(), encryption))
			{
			INFO_PRINTF2(_L("TBTServiceSecurity Encryption: expected (%d)" ), encryption);
			if(encryption != encryptionReceived)
				{
				ERR_PRINTF1(_L("Encryption is not as expected!"));
				SetBlockResult(EFail);				
				}
			}
		else
			{
			ERR_PRINTF1(_L("GetBoolFromConfig and GetIntFromConfig failed"));
			SetBlockResult(EFail);	
			}
		}
	}	
	
void CT_BTServiceSecurityData::TBTSSSetPasskeyMinLength(const TDesC& aSection)
	{		
	TInt	passkeyMinLength;
	if( GetIntFromConfig(aSection, KPasskeyminlength(), passkeyMinLength))
		{
		INFO_PRINTF2(_L("TBTServiceSecurity SetPasskeyMinLength (%d)"), passkeyMinLength);	
		iBTServiceSecurity->SetPasskeyMinLength(passkeyMinLength);				
		}
	else
		{
		ERR_PRINTF1(_L("GetIntFromConfig failed"));
		SetBlockResult(EFail);	
		}
	}	
	
void CT_BTServiceSecurityData::TBTSSPasskeyMinLength(const TDesC& aSection)
	{		
	TInt	passkeyMinLength;
	TInt passkeyMinLengthReceived = static_cast<TInt>(iBTServiceSecurity->PasskeyMinLength());
	INFO_PRINTF2(_L("TBTServiceSecurity PasskeyMinLength: actual   (%d)" ), passkeyMinLengthReceived);	
	if( GetIntFromConfig(aSection, KPasskeyminlength(), passkeyMinLength))
		{
		INFO_PRINTF2(_L("TBTServiceSecurity PasskeyMinLength: expected (%d)" ), passkeyMinLength);
		if(passkeyMinLength != passkeyMinLengthReceived)
			{
			ERR_PRINTF1(_L("PasskeyMinLength is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetIntFromConfig failed"));
		SetBlockResult(EFail);	
		}		
	}	
	
void CT_BTServiceSecurityData::TBTSSSetUid(const TDesC& aSection)
	{	
	TInt	tmp;
	if( GetIntFromConfig(aSection, KUid(), tmp))
		{
		TUid uid=TUid::Uid(tmp);
		INFO_PRINTF2(_L("TBTServiceSecurity SetUid (%d)"), uid.iUid);	
		iBTServiceSecurity->SetUid(uid);				
		}
	else
		{
		ERR_PRINTF1(_L("GetIntFromConfig failed"));
		SetBlockResult(EFail);	
		}		
	}
void CT_BTServiceSecurityData::TBTSSUid(const TDesC& aSection)
	{		
	TInt	tmp;
	TUid uidReceived = iBTServiceSecurity->Uid();
	INFO_PRINTF2(_L("TBTServiceSecurity Uid: actual   (%i)" ), uidReceived.iUid );	
	if( GetIntFromConfig(aSection, KUid(), tmp))
		{
		TUid uid=TUid::Uid(tmp);
		INFO_PRINTF2(_L("TBTServiceSecurity Uid: expected (%i)" ), uid.iUid );
		if(uid != uidReceived)
			{
			ERR_PRINTF1(_L("Uid is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		ERR_PRINTF1(_L("GetIntFromConfig failed"));
		SetBlockResult(EFail);	
		}		
	}

TAny* CT_BTServiceSecurityData::GetObject()
	{
	return iBTServiceSecurity;
	}

void CT_BTServiceSecurityData::SetObjectL(TAny* aObject)
	{
	delete iBTServiceSecurity;
	iBTServiceSecurity = static_cast<TBTServiceSecurity*>(aObject);
	}

void CT_BTServiceSecurityData::DisownObjectL()
	{
	iBTServiceSecurity=NULL;
	}

void CT_BTServiceSecurityData::CopyObjectL(TAny* aObjectPtr)
	{
	Cleanup();
	TBTServiceSecurity* tmp = static_cast<TBTServiceSecurity*>(aObjectPtr);
	iBTServiceSecurity =new (ELeave) TBTServiceSecurity(*tmp);	
	}
