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

#include "T_RHandleBaseData.h"

/*@{*/
_LIT(KCmdClose,								"Close");
_LIT(KCmdFullName,							"FullName");
_LIT(KCmdName,								"Name");

_LIT(KFldExpected,							"expected");
_LIT(KFldNameParameter,						"nameParameter");

_LIT(KLogErrorExpectedValue,				"Expected Value does not match actual");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_RHandleBaseData::CT_RHandleBaseData()
:	CDataWrapperBase()
	{
	}

CT_RHandleBaseData::~CT_RHandleBaseData()
/**
 * Public destructor
 */
	{
	}

TBool CT_RHandleBaseData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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

	if ( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else if ( aCommand==KCmdFullName )
		{
		DoCmdFullName(aSection);
		}
	else if ( aCommand==KCmdName )
		{
		DoCmdName(aSection);
		}
	else
		{
		ret=CDataWrapperBase::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}

void CT_RHandleBaseData::DoCmdClose()
	{
	GetHandleBase()->Close();
	}

void CT_RHandleBaseData::DoCmdFullName(const TDesC& aSection)
	{
	TFullName	actual;

	TBool	nameParameter=ETrue;
	GetBoolFromConfig(aSection, KFldNameParameter(), nameParameter);

	if ( nameParameter )
		{
		GetHandleBase()->FullName(actual);
		}
	else
		{
		actual=GetHandleBase()->FullName();
		}
	INFO_PRINTF2(_L("FullName: (%S)" ), &actual);

	TPtrC	expected;
	if ( GetStringFromConfig(aSection, KFldExpected, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}
	}

void CT_RHandleBaseData::DoCmdName(const TDesC& aSection)
	{
	TName	actual=GetHandleBase()->Name();
	INFO_PRINTF2(_L("Name: (%S)" ), &actual);

	TPtrC	expected;
	if ( GetStringFromConfig(aSection, KFldExpected, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}
	}
