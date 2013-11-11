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

#include "T_DataSubConParameterFamily.h"

/*@{*/
///	Constant Literals used.
_LIT(KConstructor,							"Constructor");
_LIT(KDestructor,							"~");

_LIT(KSubConParameterBundle,				"subconparameterbundle");
/*@}*/

CT_DataSubConParameterFamily* CT_DataSubConParameterFamily::NewL()
	{
	CT_DataSubConParameterFamily*	ret=new (ELeave) CT_DataSubConParameterFamily();
	return ret;
	}


CT_DataSubConParameterFamily::CT_DataSubConParameterFamily()
:	CT_DataCommsBase()
,	iSubConParameterFamily(NULL)
	{
	}


CT_DataSubConParameterFamily::~CT_DataSubConParameterFamily()
	{
	iSubConParameterFamily = NULL;
	}


TBool CT_DataSubConParameterFamily::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;

	if ( aCommand==KConstructor )
		{
		DoCmdConstructorL( aSection );
		}
	else if( aCommand==KDestructor )
		{
		DoCmdDestructor();
		}
	else
		{
		ret = EFalse;
		}

	return ret;
	}


TAny* CT_DataSubConParameterFamily::GetObject()
	{
	return iSubConParameterFamily;
	}


void CT_DataSubConParameterFamily::DoCmdConstructorL( const TDesC& aSection )
/**
Test code for CSubConParameterFamily Constructor

Ini file options:
	subconparameterbundle	- a RSubConParameterBundle dictionary entry

@internalComponent
*/
	{
	TPtrC		subConParameterBundleName;

	if( !GetStringFromConfig(aSection, KSubConParameterBundle(), subConParameterBundleName) )
		{
		MissingParamError( KSubConParameterBundle() );
		}
	else
		{
		RSubConParameterBundle*	subConParameterBundle = static_cast<RSubConParameterBundle*>(GetDataObjectL(subConParameterBundleName));

		if (subConParameterBundle == NULL)
			{
			ERR_PRINTF1(_L("CSubConParameterFamily Constructor - RSubConParameterBundle is NULL"));
			SetBlockResult(EFail);
			}

		INFO_PRINTF2(_L("Calling CSubConParameterFamily::NewL( %S )"), &subConParameterBundleName);
		TRAPD (err, iSubConParameterFamily = CSubConParameterFamily::NewL(*subConParameterBundle, KSubConQoSFamily));

		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("CSubConParameterFamily::NewL() failed with error %d"), err);
			SetError(err);
			}
		}
	}

void CT_DataSubConParameterFamily::DoCmdDestructor()
/**
Test code for CSubConParameterFamily Destructor

Ini file options:
	none

@internalComponent
*/
	{
	if( iSubConParameterFamily )
		{
		INFO_PRINTF1(_L("Calling CSubConParameterFamily Destructor"));
		delete iSubConParameterFamily;
		iSubConParameterFamily = NULL;
		}
	else
		{
		ERR_PRINTF1(_L("CSubConParameterFamily object invalid, can't call destructor"));
		SetBlockResult(EFail);
		}
	}
