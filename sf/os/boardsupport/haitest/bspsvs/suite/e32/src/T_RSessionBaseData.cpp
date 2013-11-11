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

#include "T_RSessionBaseData.h"

/*@{*/
_LIT(KCmdShareAuto,							"ShareAuto");
_LIT(KCmdShareProtected,					"ShareProtected");

_LIT(KLogError,								"Error=%d");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_RSessionBaseData::CT_RSessionBaseData()
:	CT_RHandleBaseData()
	{
	}

CT_RSessionBaseData::~CT_RSessionBaseData()
/**
 * Public destructor
 */
	{
	}

TBool CT_RSessionBaseData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
	{
	TBool	ret=ETrue;
	TInt	err=KErrNone;

	if ( aCommand==KCmdShareAuto )
		{
		err=DoCmdShareAuto();
		}
	else if ( aCommand==KCmdShareProtected )
		{
		err=DoCmdShareProtected();
		}
	else
		{
		ret=CT_RHandleBaseData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}

	return ret;
	}

TInt CT_RSessionBaseData::DoCmdShareAuto()
	{
	return GetSessionBase()->ShareAuto();
	}

TInt CT_RSessionBaseData::DoCmdShareProtected()
	{
	return GetSessionBase()->ShareProtected();
	}
