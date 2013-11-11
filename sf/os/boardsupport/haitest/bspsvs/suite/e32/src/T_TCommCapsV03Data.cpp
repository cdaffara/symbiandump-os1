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

#include "T_TCommCapsV03Data.h"

/*@{*/
_LIT(KCmdConstructor,						"new");
_LIT(KCmdDestructor,						"~");
_LIT(KCmdiBreakSupported,					"iBreakSupported");

_LIT(KFldExpected,							"expected");

_LIT(KLogError,								"Error=%d");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_TCommCapsV03Data::CT_TCommCapsV03Data()
:	CT_TCommCapsV02Data()
,	iCommCaps3(NULL)
	{
	}

CT_TCommCapsV03Data::~CT_TCommCapsV03Data()
/**
 * Public destructor
 */
	{
	DestroyData();
	}

void CT_TCommCapsV03Data::DestroyData()
	{
	delete iCommCaps3;
	iCommCaps3=NULL;
	}

TDes8* CT_TCommCapsV03Data::Descriptor()
/**
 * Return a pointer to the buffer
 *
 * @return	pointer to the buffer
 */
	{
	return iCommCaps3;
	}

TAny* CT_TCommCapsV03Data::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return iCommCaps3;
	}

TBool CT_TCommCapsV03Data::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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
	else if ( aCommand==KCmdiBreakSupported )
		{
		DoCmdiBreakSupported(aSection);
		}
	else
		{
		ret=CT_TCommCapsV02Data::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}

	return ret;
	}

TCommCapsV01& CT_TCommCapsV03Data::GetCommCapsV01()
	{
	return (*iCommCaps3)();
	}

TCommCapsV02& CT_TCommCapsV03Data::GetCommCapsV02()
	{
	return (*iCommCaps3)();
	}

TCommCapsV03& CT_TCommCapsV03Data::GetCommCapsV03()
	{
	return (*iCommCaps3)();
	}

TInt CT_TCommCapsV03Data::DoCmdConstructor()
	{
	DestroyData();
	TRAPD(err, iCommCaps3=new (ELeave) TCommCaps3());
	iCommCaps3->FillZ();
	return err;
	}

void CT_TCommCapsV03Data::DoCmdDestructor()
	{
	DestroyData();
	}

void CT_TCommCapsV03Data::DoCmdiBreakSupported(const TDesC& aSection)
	{
	TBool	actual=GetCommCapsV03().iBreakSupported;
	INFO_PRINTF2(_L("iBreakSupported : 0x%X"), actual);

	TBool	expected;
	if ( GetBoolFromConfig(aSection, KFldExpected(), expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}
	}
