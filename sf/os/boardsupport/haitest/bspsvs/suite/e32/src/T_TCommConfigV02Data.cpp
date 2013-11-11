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

#include "T_TCommConfigV02Data.h"

/*@{*/
_LIT(KCmdConstructor,						"new");
_LIT(KCmdDestructor,						"~");
_LIT(KCmdiTxShutdownTimeout,				"iTxShutdownTimeout");

_LIT(KFldExpected,							"expected");
_LIT(KFldValue,								"value");

_LIT(KLogError,								"Error=%d");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_TCommConfigV02Data::CT_TCommConfigV02Data()
:	CT_TCommConfigV01Data()
,	iCommConfig2(NULL)
	{
	}

CT_TCommConfigV02Data::~CT_TCommConfigV02Data()
/**
 * Public destructor
 */
	{
	DestroyData();
	}

void CT_TCommConfigV02Data::DestroyData()
	{
	delete iCommConfig2;
	iCommConfig2=NULL;
	}

TDes8* CT_TCommConfigV02Data::Descriptor()
/**
 * Return a pointer to the buffer
 *
 * @return	pointer to the buffer
 */
	{
	return iCommConfig2;
	}

TAny* CT_TCommConfigV02Data::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return iCommConfig2;
	}

TBool CT_TCommConfigV02Data::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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

	if ( aCommand==KCmdConstructor )
		{
		err=DoCmdConstructor();
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdiTxShutdownTimeout )
		{
		DoCmdiTxShutdownTimeout(aSection);
		}
	else
		{
		err=CT_TCommConfigV01Data::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}

	return ret;
	}

TCommConfigV01& CT_TCommConfigV02Data::GetCommConfigV01()
	{
	return (*iCommConfig2)();
	}

TCommConfigV02& CT_TCommConfigV02Data::GetCommConfigV02()
	{
	return (*iCommConfig2)();
	}

TInt CT_TCommConfigV02Data::DoCmdConstructor()
	{
	DestroyData();
	TRAPD(err, iCommConfig2=new (ELeave) TCommConfig2());
	iCommConfig2->FillZ();
	return err;
	}

void CT_TCommConfigV02Data::DoCmdDestructor()
	{
	DestroyData();
	}

void CT_TCommConfigV02Data::DoCmdiTxShutdownTimeout(const TDesC& aSection)
	{
	TInt	actual=GetCommConfigV02().iTxShutdownTimeout;
	INFO_PRINTF2(_L("iTxShutdownTimeout : %d"), actual);

	TInt	expected;
	if( GetIntFromConfig(aSection, KFldExpected(), expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if( GetIntFromConfig(aSection, KFldValue(), value) )
		{
		GetCommConfigV02().iTxShutdownTimeout=value;
		}
	}
