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


#include "T_RSocketServData.h"
#include <commdb.h>						// CommDB
#include <wdbifwlansettings.h>			// CWLanSettings

/*@{*/
//LIT's for commands
_LIT(KCmdConnectSocketServ, 	"Connect");
_LIT(KCmdCloseSocketServ, 		"Close");
/*}@*/

CT_RSocketServData* CT_RSocketServData::NewL()
	{
	CT_RSocketServData * ret = new (ELeave) CT_RSocketServData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_RSocketServData::~CT_RSocketServData()
	{
	if (iSocketServ)
		{
		delete iSocketServ;
		iSocketServ = NULL;
		}
	
	}

CT_RSocketServData::CT_RSocketServData()
	:
	iSocketServ(NULL)
	{
	}

void CT_RSocketServData::ConstructL()
	{
	iSocketServ = new (ELeave)RSocketServ();
	}

TAny* CT_RSocketServData::GetObject()
	{
	return iSocketServ;
	}

TBool CT_RSocketServData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& /*aSection*/, const TInt /*aAsyncErrorIndex*/)
	{
	TBool ret = ETrue;
	
	if(aCommand == KCmdConnectSocketServ)
		{
		DoCmdConnect();
		}
	else if(aCommand == KCmdCloseSocketServ)
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

/**
 * Connects to Socket Server.
 * Sets TEF error if not successful.
 */
void CT_RSocketServData::DoCmdConnect()
	{
	INFO_PRINTF1(_L("*START* CT_RSocketServData::DoCmdConnect"));
	TInt err(KErrNone);
	err = iSocketServ->Connect();	
	if(err == KErrNone)
 		{
 		INFO_PRINTF1(_L("Connected to Socket Server"));
 		}
	else
		{		
		ERR_PRINTF1(_L("iSocketServ->Connect() Fail"));
		ERR_PRINTF2(_L("Failed to connect to Socket server [%d]"), err);
		SetError(err);
		}
	
	
	INFO_PRINTF1(_L("*END* CT_RSocketServData::DoCmdConnect"));
	}

/**
 * Closes Socket Server.
 * Sets TEF error if not successful.
 */
void CT_RSocketServData::DoCmdClose()
	{
	INFO_PRINTF1(_L("*START* CT_RSocketServData::DoCmdClose"));
	iSocketServ->Close();
	INFO_PRINTF1(_L("*END* CT_RSocketServData::DoCmdClose"));
	}

