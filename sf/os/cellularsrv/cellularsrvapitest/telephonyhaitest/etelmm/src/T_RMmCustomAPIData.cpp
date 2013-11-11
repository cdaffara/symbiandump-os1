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
#include "t_rmmcustomapidata.h"

_LIT(KMobilePhoneKey, 	"RMobilePhone");

_LIT(KChangeMode, 		"ChangeMode");
_LIT(KCheckMode,		"CheckMode");

// Mode Change
_LIT(KCmdSetSystemNetworkMode, 			"SetSystemNetworkMode");
_LIT(KCmdGetCurrentSystemNetworkModes, 	"GetCurrentSystemNetworkModes");
_LIT(KCmdUtilityModeCheck, 				"UtilityModeCheck");
_LIT(KCmdOpen, 							"Open");
_LIT(KCmdClose, 						"Close");

_LIT(KGSM, 		"GSM");
_LIT(KWCDMA, 	"WCDMA");
_LIT(KDUAL, 	"Dual");

const CDataWrapperBase::TEnumEntryTable CT_RMmCustomApiData::iEnumTestCaseMode[] = 
			{ 
			{ KGSM,					RMmCustomAPI::KCapsNetworkModeGsm },	//GSM
			{ KWCDMA,				RMmCustomAPI::KCapsNetworkModeUmts },	//WCDMA(3G)
			{ KDUAL,				RMmCustomAPI::KCapsNetworkModeDual }	//Dual, Phone decides
			};



/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_RMmCustomApiData* CT_RMmCustomApiData::NewL()
	{
	CT_RMmCustomApiData* ret = new(ELeave) CT_RMmCustomApiData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


/**
 * Protected constructor. First phase construction
 */
CT_RMmCustomApiData::CT_RMmCustomApiData()
: 	iActiveCallback(NULL),
	iNetworkMode(0),
	iCustomPhone(NULL)
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
void CT_RMmCustomApiData::ConstructL()
	{
	iCustomPhone = new (ELeave) RMmCustomAPI();
	iActiveCallback = CActiveCallback::NewL(*this);
	}

/**
 * Public destructor
 */
CT_RMmCustomApiData::~CT_RMmCustomApiData()
	{
	if(iCustomPhone)
		{
		delete iCustomPhone;
		iCustomPhone = NULL;
		}
	
	if(iActiveCallback)
		{
		delete iActiveCallback;
	    iActiveCallback = NULL;
		}
	
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_RMmCustomApiData::GetObject()
	{
	return iCustomPhone;
	}

void CT_RMmCustomApiData::RunL(CActive* aActive, TInt aIndex)
    {
    DecOutstanding(); // One of the async calls has completed 
    TInt err = aActive->iStatus.Int(); 
    if( err != KErrNone ) 
    	{ 
        ERR_PRINTF2(_L("RunL Error %d"), err); 
        SetAsyncError( aIndex, err ); 
        } 
    else 
        { 
        INFO_PRINTF1(_L("RunL  completed successfully")); 
        } 
    }

TBool CT_RMmCustomApiData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool ret = ETrue;
	
	if ( aCommand==KCmdUtilityModeCheck )
		{
		DoCmdUtilityModeCheck(aSection);
		}
	else if ( aCommand==KCmdGetCurrentSystemNetworkModes )
		{
		DoCmdGetCurrentSystemNetworkModes(aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdSetSystemNetworkMode )
		{
		DoCmdSetSystemNetworkMode(aSection, aAsyncErrorIndex);
		}
	else if (aCommand==KCmdOpen)
		{
		DoCmdOpen(aSection);
		}
	else if (aCommand==KCmdClose)
		{
		DoCmdClose();
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		ret = EFalse;
		}
	return ret;
	}

void CT_RMmCustomApiData::DoCmdClose()
	{
	INFO_PRINTF1(_L("*START*CT_RMmCustomApiData::DoCmdCloseCustomApi"));
	iCustomPhone->Close();
	INFO_PRINTF1(_L("*END*CT_RMmCustomApiData::DoCmdCloseCustomApi"));
	}

/**
 * Selects the network phone that is currently used.
 * @param aNetworkMode is the new network mode.
 * @return Request status.
 */
void CT_RMmCustomApiData::DoCmdSetSystemNetworkMode(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
    {
    INFO_PRINTF1(_L("*START*CT_RMmCustomApiData::ModeChange"));

    TInt mode = 0;
	if (!GetEnumFromConfig(aSection, KChangeMode, iEnumTestCaseMode, mode))
		{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KChangeMode);
    	SetBlockResult(EFail);
		}  
	else
		{
	    INFO_PRINTF2(_L("Change network mode to: %d"), mode);
		// Setting the network mode, with aNetworkMode
		INFO_PRINTF1(_L("Use RMmCustomAPI to set the phone mode."));
		iCustomPhone->SetSystemNetworkMode(iActiveCallback->iStatus, static_cast<RMmCustomAPI::TNetworkModeCaps>(mode));
	    iActiveCallback->Activate(aAsyncErrorIndex);
	    IncOutstanding();
		}
    INFO_PRINTF1(_L("*END*CT_RMmCustomApiData::ModeChange"));
    }
    
/**
 * Checks that the network mode of phone is the same as defined in parameter.
 */
void CT_RMmCustomApiData::DoCmdGetCurrentSystemNetworkModes(const TInt aAsyncErrorIndex)
	{    
    INFO_PRINTF1(_L("*START*CT_RMmCustomApiData::DoCmdGetCurrentSystemNetworkModes"));

	INFO_PRINTF1(_L("Get the current network mode from RMmCustomAPI."));
	
	iCustomPhone->GetCurrentSystemNetworkModes(iActiveCallback->iStatus, iNetworkMode);
    iActiveCallback->Activate(aAsyncErrorIndex);
    IncOutstanding();
    
    INFO_PRINTF1(_L("*END*CT_RMmCustomApiData::DoCmdGetCurrentSystemNetworkModes"));
	}

/**
 * Checks that the network mode of phone is the same as defined in parameter.
 */
void CT_RMmCustomApiData::DoCmdUtilityModeCheck(const TTEFSectionName& aSection)
	{    
    INFO_PRINTF1(_L("*START*CT_RMmCustomApiData::ModeCheck"));
    TInt mode;
	if (!GetEnumFromConfig(aSection, KCheckMode, iEnumTestCaseMode, mode))
		{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KChangeMode);
    	SetBlockResult(EFail);
		}  
	else
		{
		INFO_PRINTF2(_L("Check network mode to: %d"), mode);
		if ( iNetworkMode != static_cast<RMmCustomAPI::TNetworkModeCaps>(mode))
			{
	        ERR_PRINTF1(_L("Network mode of phone is different to expected."));
	        SetBlockResult(EFail);
			}
		else
			{
		    INFO_PRINTF1(_L("Mode check succeeded"));
			}
		}
    INFO_PRINTF1(_L("*END*CT_RMmCustomApiData::ModeCheck"));
	}


void CT_RMmCustomApiData::DoCmdOpen(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RMmCustomApiData::DoCmdOpen"));
	TPtrC mobilePhoneName;
	TInt error(0);
	if ( !GetStringFromConfig(aSection, KMobilePhoneKey(), mobilePhoneName) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMobilePhoneKey);
		SetBlockResult(EFail);
		}
	else
		{
		RMobilePhone* mobilePhoneObject = static_cast<RMobilePhone*>(GetDataObjectL(mobilePhoneName));
		INFO_PRINTF1(_L("Initializing the custom phone API"));
		error = iCustomPhone->Open(*mobilePhoneObject);
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("Failed to initialize the custom phone API with error %d"), error);
			SetError(error);
			}
		else
			{
			INFO_PRINTF1(_L("Initialize the custom phone API Succeeded"));
			}
		}
	INFO_PRINTF1(_L("*END*CT_RMmCustomApiData::DoCmdOpen"));
	}






