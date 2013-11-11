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


#include "T_CRepositoryData.h"

/*@{*/
//Command literals 
_LIT(KCmdNewL, 							"NewL");
_LIT(KCmdDestructor, 					"~");
_LIT(KCmdSet,							"Set");
/*@}*/

/*@{*/
//INI Key names literals 
_LIT(KRepUID,							"RepUID");
_LIT(KRepItemID,						"ItemID");
_LIT(KRepSetValue,						"SetValue");
/*@{*/

/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_CRepositoryData* CT_CRepositoryData::NewL()
	{
	CT_CRepositoryData* ret = new (ELeave) CT_CRepositoryData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
 * Private constructor. First phase construction
 */
CT_CRepositoryData::CT_CRepositoryData()
	:
	iRepository(NULL)
	{
	}

/**
 * Second phase construction
 * @internalComponent
 * @return	N/A
 * @pre		None
 * @post	None
 * @leave	system wide error
 */
void CT_CRepositoryData::ConstructL()
	{
	}

/**
 * Public destructor
 */
CT_CRepositoryData::~CT_CRepositoryData()
	{
	DestroyData();
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_CRepositoryData::GetObject()
	{
	return iRepository;
	}

/**
 * Helper Destructor
 */
void CT_CRepositoryData::DestroyData()
	{
	if(iRepository)
		{
		delete iRepository;
		iRepository = NULL;
		}	
	}

/**
 * Process a command read from the Ini file
 * @param aCommand 		   -	The command to process
 * @param aSection		   -	The section get from the *.ini file of the project T_Wlan
 * @param aAsyncErrorIndex -	Command index dor async calls to returns errors to
 * @return TBool		   -    ETrue if the command is process
 * @leave				   -	system wide error
 */
TBool CT_CRepositoryData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool ret = ETrue;
	
	if (aCommand == KCmdNewL)
		{
		DoCmdNewL(aSection);
		}
	else if (aCommand == KCmdDestructor)
		{
		DoCmdDestructor();
		}
	else if (aCommand == KCmdSet)
		{
		DoCmdSet(aSection);
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command."));
		ret=EFalse;
		}
	
	return ret;
	}

/**
 * Create an instance of CRepository
 * @param aSection - Section to read from the ini file
 * @return void
 */
void CT_CRepositoryData::DoCmdNewL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CRepositoryData::DoCmdNewL"));
	DestroyData();
	TBool dataOk = ETrue;
	
	TInt parRepUID = 0;
	if(!GetHexFromConfig(aSection, KRepUID, parRepUID))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRepUID);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
		}

	if(dataOk)
		{
		TUid repUid;
	    repUid.Null();
	    repUid = TUid::Uid(parRepUID);
		TRAPD(error, iRepository = CRepository::NewL(repUid));
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("Could not create CRepository: error %d"), error);
			SetError(error);
			}
		}
	
	INFO_PRINTF1(_L("*END*CT_CRepositoryData::DoCmdNewL"));
	}

/**
 * Delete an instance of CRepository
 * @param
 * @return
 */
void CT_CRepositoryData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START*CT_CRepositoryData::DoCmdDestroyData"));
	DestroyData();
	INFO_PRINTF1(_L("*END*CT_CRepositoryData::DoCmdDestroyData"));
	}

/**
 * Setting the central repository
 * @param aSection - Section to read from the ini file
 * @return void
 */
void CT_CRepositoryData::DoCmdSet(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CRepositoryData::DoCmdSet"));
	TBool dataOk = ETrue;
	
	TInt parRepItemID;
	if(!GetHexFromConfig(aSection, KRepItemID, parRepItemID))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRepItemID);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
		}

	TInt parRepSetValue;
	if(!GetIntFromConfig(aSection, KRepSetValue, parRepSetValue))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRepSetValue);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
		}
	
	if(dataOk)
		{
		TInt error = KErrNone;
		INFO_PRINTF2(_L("ItemId = %d"),parRepItemID);
		INFO_PRINTF2(_L("DataId = %d"),parRepSetValue);
		
		error = iRepository->Set(parRepItemID, parRepSetValue);
		
		if(error != KErrNone)
			{
			ERR_PRINTF3(_L("Setting the central repository parameter %d failed with error %d"),	parRepItemID, error);
			SetError(error);			
			}
		}	
	INFO_PRINTF1(_L("*END* CT_CRepositoryData::DoCmdSet"));
	}
