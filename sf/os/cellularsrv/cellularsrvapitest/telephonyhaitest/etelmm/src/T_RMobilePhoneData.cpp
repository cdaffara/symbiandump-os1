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


#include <cdbcols.h>
#include <e32base.h>
#include <e32def.h>

//	User Includes
#include "t_rmobilephonedata.h"


_LIT(KTelServerKey,			"RTelServer");
_LIT(KPhone,				"Phone");

/*@{*/
//LIT's for commands
_LIT(KCmdOpen, 				"Open");
_LIT(KCmdClose, 			"Close");
_LIT(KCmdInitialise,		"Initialise");
/*}@*/

/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_RMobilePhoneData* CT_RMobilePhoneData::NewL()
	{
	CT_RMobilePhoneData*	ret=new (ELeave) CT_RMobilePhoneData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


/**
 * Protected constructor. First phase construction
 */
CT_RMobilePhoneData::CT_RMobilePhoneData()
:iMobilePhone(NULL)
	{
	}

/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
void CT_RMobilePhoneData::ConstructL()
	{
	iMobilePhone = new (ELeave) RMobilePhone();
	}

/**
 * Public destructor
 */
CT_RMobilePhoneData::~CT_RMobilePhoneData()
	{
	if(iMobilePhone)
		{
		delete iMobilePhone;
		iMobilePhone = NULL;
		}	
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_RMobilePhoneData::GetObject()
	{
	return iMobilePhone;
	}

TBool CT_RMobilePhoneData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool ret = ETrue;
	
	if ( aCommand==KCmdOpen )
		{
		DoCmdOpen(aSection);
		}
	else if ( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else if ( aCommand==KCmdInitialise )
		{
		DoCmdInitialise();
		}
	else
		{
		ret = EFalse;
		ERR_PRINTF1(_L("Unknown command"));
		}
	
	return ret;
	}

//== Mobile Line functions
void CT_RMobilePhoneData::DoCmdOpen(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RMobilePhoneData::DoCmdOpen"));
	// Check that first phone is available and log phone name.
	RTelServer::TPhoneInfo info;
	// Reading phone info for the first available phone
	TInt error(0);
	TBool dataOk = ETrue;
	TInt parPhone = 0;
	if ( !GetIntFromConfig(aSection, KPhone(), parPhone) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPhone);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TPtrC telServerName;
	if ( !GetStringFromConfig(aSection, KTelServerKey(), telServerName) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KTelServerKey);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if ( dataOk )
		{
		RTelServer* telServerObject = static_cast<RTelServer*>(GetDataObjectL(telServerName));
		INFO_PRINTF1(_L("Check if phone info was found."));
		error = telServerObject->GetPhoneInfo(parPhone, info);
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("Failed to read phone info for phone KFirstPhone with error %d"), error);
			SetError(error);
			}
		else
			{
			// Connect to RMobilePhone interface.
			INFO_PRINTF1(_L("Opening connection to phone"));
			error = iMobilePhone->Open(*telServerObject, info.iName);
			if (error != KErrNone)
				{
				ERR_PRINTF2(_L("Connection to phone failed with error %d"), error);
				SetError(error);
				}
			else
				{
				INFO_PRINTF1(_L("Opening connection to phone sucessfull"));
				}
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMobilePhoneData::DoCmdOpen"));
	}

void CT_RMobilePhoneData::DoCmdInitialise()
	{
	TInt error(0);
	INFO_PRINTF1(_L("*START*CT_RMobilePhoneData::DoCmdInitialise"));
	INFO_PRINTF1(_L("Initialising the phone"));
	error = iMobilePhone->Initialise();
	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to initalise the phone with error %d"), error);
        SetError(error);
		}
	INFO_PRINTF1(_L("*END*CT_RMobilePhoneData::DoCmdInitialise"));
	}

void CT_RMobilePhoneData::DoCmdClose()
	{
	INFO_PRINTF1(_L("*START*CT_RMobilePhoneData::DoCmdClosePhone"));
	INFO_PRINTF1(_L("Closing mobile phone session..."));
	iMobilePhone->Close();
	INFO_PRINTF1(_L("*END*CT_RMobilePhoneData::DoCmdInitialise"));
	}

