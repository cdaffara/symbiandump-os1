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
#include "t_rtelserverdata.h"

/*@{*/
//LIT's for commands
_LIT(KCmdConnect,					"Connect");
_LIT(KCmdClose, 					"Close");
/*}@*/

/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_RTelServerData* CT_RTelServerData::NewL()
	{
	CT_RTelServerData* ret = new (ELeave) CT_RTelServerData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


/**
 * Protected constructor. First phase construction
 */
CT_RTelServerData::CT_RTelServerData()
:	iTelServer(NULL)
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
void CT_RTelServerData::ConstructL()
	{
	iTelServer = new (ELeave) RTelServer();
	}

/**
 * Public destructor
 */
CT_RTelServerData::~CT_RTelServerData()
	{
	if(iTelServer)
		{
		delete iTelServer;
		iTelServer = NULL;
		}	
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_RTelServerData::GetObject()
	{
	return iTelServer;
	}

TBool CT_RTelServerData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& /*aSection*/, const TInt /*aAsyncErrorIndex*/)
	{
	TBool ret = ETrue;
	
	if ( aCommand==KCmdConnect )
		{
		DoCmdConnect();
		}
	else if ( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else
		{
		ret = EFalse;
		ERR_PRINTF1(_L("Unknown command"));
		}
	
	return ret;
	}


void CT_RTelServerData::DoCmdConnect()
	{
	INFO_PRINTF1(_L("*START*CT_RTelServerData::DoCmdOpenTelServer"));
	
	TInt error(0);
	if(iTelServer->Handle())
		{
		ERR_PRINTF1(_L("Handle to the telephony server exists"));
		SetBlockResult(EFail);
		}
	else
		{
		// Connect to RTelServer server. This will also start the server if needed.
		INFO_PRINTF1(_L("Connecting to the telephony server (ETel)"));
		error = iTelServer->Connect();
		// Report if loading failed.
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("Failed to load TSY with error %d"), error);
			SetError(error);
			}
		else
			{
			INFO_PRINTF1(_L("The root server is up for sure and telephony server is started."));
			}
		}
	INFO_PRINTF1(_L("*END*CT_RTelServerData::DoCmdOpenTelServer"));
	}

void CT_RTelServerData::DoCmdClose()
	{
	INFO_PRINTF1(_L("*START*CT_RTelServerData::DoCmdCloseTelServer"));
	INFO_PRINTF1(_L("Closing telephony server session..."));
	iTelServer->Close();
	INFO_PRINTF1(_L("*END*CT_RTelServerData::DoCmdCloseTelServer"));
	}

