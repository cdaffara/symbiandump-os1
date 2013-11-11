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

#include "T_MmcSDSessionData.h"

//MMCSD Commands index
/*@{*/
_LIT(KCmdConstructor,					"NewL");
_LIT(KCmdLoadDriver,					"LoadDriver");
_LIT(KCmdUnLoadDriver,					"UnLoadDriver");
_LIT(KCmdDriverOpen,					"DriverOpen");
_LIT(KCmdDestructor,					"~");

_LIT(KLDDName,							"LDD");
_LIT(KStackNumber,						"stacknumber");
/*@}*/



/**
 * Create a new Socket Driver Data wrapper
 *
 * @return					A Socket Driver Data wrapper
 *
 * @leave					System wide error
 */
CT_MmcSDSessionData* CT_MmcSDSessionData::NewL()
	{
	CT_MmcSDSessionData*	ret=new (ELeave) CT_MmcSDSessionData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
 * Construction
 *
 * @return					N/A
 */
CT_MmcSDSessionData::CT_MmcSDSessionData()
:	CT_RBusLogicalChannelData()
,	iMmcSDController(NULL)
	{
	}

/**
 * Second phase construction
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MmcSDSessionData::ConstructL()
	{
	}

/**
 * Public destructor
 *
 * @return					N/A
 */
CT_MmcSDSessionData::~CT_MmcSDSessionData()
	{
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return					pointer to the object that the data wraps
 */
TAny* CT_MmcSDSessionData::GetObject()
	{
	return iMmcSDController;
	}

/**
 * Return a pointer to the handle
 *
 * @return					pointer to the MMC/SD Controller
 */
RHandleBase* CT_MmcSDSessionData::GetHandleBase()
	{
	return iMmcSDController;
	}

/**
 * Return a pointer to the logical channel
 *
 * @return					pointer to the MMC/SD Controller
 */
RBusLogicalChannel* CT_MmcSDSessionData::GetBusLogicalChannel()
	{
	return iMmcSDController;
	}

/**
 * Close and clean
 *
 * @return					void
 */
void CT_MmcSDSessionData::DestroyData()
	{
	delete iMmcSDController;
	iMmcSDController=NULL;
	}

/**
 * Process a command read from the script file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
TBool CT_MmcSDSessionData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdConstructor )
		{
		DoCmdConstructor();
		}
	else if ( aCommand==KCmdLoadDriver )
		{
		DoCmdLoadDriver(aSection);
		}
	else if ( aCommand==KCmdDriverOpen )
		{
		DoCmdDriverOpen(aSection);
		}
	else if ( aCommand==KCmdUnLoadDriver )
		{
		DoCmdUnLoadDriver(aSection);
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else
		{
		ret=CT_RBusLogicalChannelData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}
	return ret;
	}

/**
 * Creates the RBusLogicalChannel derived interface
 *
 * @param aCommand			None
 *
 * @return					None
 */
void CT_MmcSDSessionData::DoCmdConstructor()
	{
	delete iMmcSDController;
	iMmcSDController=NULL;
	TRAPD(err, iMmcSDController=new (ELeave) RMMCSDTestControllerInterface());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RMMCSDTestControllerInterface Creation Error %d"), err);
		SetError(err);
		}
	}

/**
 * Loads the Device Driver
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MmcSDSessionData::DoCmdLoadDriver(const TDesC& aSection)
	{
	TPtrC lddname;
	if(!GetStringFromConfig(aSection, KLDDName(), lddname))
		{
		ERR_PRINTF1(_L("Error in Reading Driver Name from INI file -Load"));
		SetBlockResult(EFail);
		}
	User::LoadLogicalDevice(lddname);
	}

/**
 * Unloads the Device Driver
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MmcSDSessionData::DoCmdUnLoadDriver(const TDesC& aSection)
	{
	TPtrC lddname;
	if(!GetStringFromConfig(aSection, KLDDName(), lddname))
		{
		ERR_PRINTF1(_L("Error in Reading Driver Name from INI file-UnLoad"));
		SetBlockResult(EFail);
		}
	User::FreeLogicalDevice(lddname);
	}

/**
 * Opens the Driver Call
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MmcSDSessionData::DoCmdDriverOpen(const TDesC& aSection)
	{
	TInt err;
	TInt stackNumber = 0;
	GetIntFromConfig(aSection, KStackNumber(), stackNumber);
	err=iMmcSDController->Open(stackNumber,iMmcSDController->VersionRequired());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("Driver Open Call Failure %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("iMmcSDController = %x"), iMmcSDController);
		}
	}

/**
 * Destroys the objects
 *
 * @return					void
 */
void CT_MmcSDSessionData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("CT_MmcSDSessionData::DoCmdDestructor()"));
	DestroyData();
	}

