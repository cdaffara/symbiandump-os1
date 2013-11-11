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

#include "t_raccessoryserverdata.h"

/*@{*/
//LIT's for commands
_LIT( KCmdConnectToAccessoryServer, "Connect");
_LIT( KCmdCloseAccessoryServer, "Disconnect");

/*@}*/

/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_RAccessoryServerData* CT_RAccessoryServerData::NewL()
	{
	CT_RAccessoryServerData* ret=new (ELeave) CT_RAccessoryServerData();
	return ret;
	}


/**
 * Public destructor
 */
CT_RAccessoryServerData::~CT_RAccessoryServerData()
	{
	iServer.Disconnect();	
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_RAccessoryServerData::GetObject()
	{
	return &iServer;
	}


/**
 * Process a command read from the ini file
 *
 * @param aCommand	The command to process
 * @param aSection	The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return	ETrue if the command is processed
 *
 * @leave System wide error
 */
TBool CT_RAccessoryServerData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& /*aSection*/, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;

	if ( aCommand == KCmdConnectToAccessoryServer)
		{
		DoCmdConnect();
		}
	else if ( aCommand == KCmdCloseAccessoryServer)
		{
		DoCmdDisconnect();
		}	
	else
		{
		ret=EFalse;
		}

	return ret;
	}

/**
 * Connects to Accessory Server
 * @return err 		Symbian error code. KErrNone if successful
 */
void CT_RAccessoryServerData::DoCmdConnect()
	{
	INFO_PRINTF1(_L("*START*CT_RAccessoryServerData::DoCmdConnect"));
	// The test verdict, set by the actual test function
	TInt err( KErrNone );
	err = iServer.Connect();

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to connect to accessory server [%d]"), err);
		SetError(err);
		}
	INFO_PRINTF1(_L("*END*CT_RAccessoryServerData::DoCmdConnect"));
	}


/**
 * Closes the connection to Accessory Server
 * @return err 		Symbian error code. KErrNone if successful
 */
void CT_RAccessoryServerData::DoCmdDisconnect()
	{
	INFO_PRINTF1(_L("*START*CT_RAccessoryServerData::DoCmdDisconnect"));
	// The test verdict, set by the actual test function
	iServer.Disconnect();
	INFO_PRINTF1(_L("accessory server was closed"));
	INFO_PRINTF1(_L("*END*CT_RAccessoryServerData::DoCmdDisconnect"));
	}


