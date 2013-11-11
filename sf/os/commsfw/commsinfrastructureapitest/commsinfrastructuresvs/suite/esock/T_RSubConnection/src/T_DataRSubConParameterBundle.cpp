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



#include "T_DataRSubConParameterBundle.h"


_LIT(KConstructor,							"Constructor");
_LIT(KFindFamily,							"FindFamily");
_LIT(KClose,								"Close");
_LIT(KDestructor,							"~");

_LIT(KSubConParameterFamily,				"subconparameterfamily");


CT_DataRSubConParameterBundle* CT_DataRSubConParameterBundle::NewL()
	{
	CT_DataRSubConParameterBundle*	ret=new (ELeave) CT_DataRSubConParameterBundle();
	return ret;
	}


CT_DataRSubConParameterBundle::CT_DataRSubConParameterBundle()
:	CT_DataCommsBase()
,	iSubConParameterBundle(NULL)
	{
	}


CT_DataRSubConParameterBundle::~CT_DataRSubConParameterBundle()
	{
	delete iSubConParameterBundle;	
	}


TBool CT_DataRSubConParameterBundle::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;
	
	//-------------------------------------------------------------------------
	// RSubConParameterBundle (Currently only used for other classes Copy constructor(s))
	//-------------------------------------------------------------------------		

	if ( aCommand==KConstructor )
		{
		DoCmdConstructorL();
		}
	else if ( aCommand==KFindFamily )
		{
		DoCmdFindFamily( aSection );
		}
	else if ( aCommand==KClose )
		{
		DoCmdClose();
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
	

TAny* CT_DataRSubConParameterBundle::GetObject()
	{
	return iSubConParameterBundle;
	}
	

void CT_DataRSubConParameterBundle::DoCmdConstructorL()
/**
Test code for RSubConParameterBundle Constructor

Ini file options:
	none

@internalComponent
*/
	{
	if( iSubConParameterBundle )
		{
		delete iSubConParameterBundle;
		iSubConParameterBundle = NULL;	
		}

	INFO_PRINTF1(_L("Calling RSubConParameterBundle Constructor"));
	TRAPD(err, iSubConParameterBundle = new (ELeave) RSubConParameterBundle());
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("RSubConParameterBundle Constructor failed with error %d"), err);
		SetError(err);
		}
	}

void CT_DataRSubConParameterBundle::DoCmdFindFamily( const TDesC& aSection )
/**
Test code for RSubConParameterBundle FindFamily

Ini file options:
	subconparameterfamily	- a CSubConParameterFamily dictionary entry

@internalComponent
*/
	{
	TPtrC		subConParameterFamilyName;

	if( !GetStringFromConfig(aSection, KSubConParameterFamily(), subConParameterFamilyName) )
		{
		MissingParamError( KSubConParameterFamily() );
		}
	else
		{
		CSubConParameterFamily*	subConParameterFamily = static_cast<CSubConParameterFamily*>(GetDataObjectL(subConParameterFamilyName));

		if (subConParameterFamily == NULL)
			{
			ERR_PRINTF1(_L("CSubConParameterBundle DoCmdFindFamily - CSubConParameterFamily is NULL"));
			SetBlockResult(EFail);
			}

		INFO_PRINTF1(_L("Calling RSubConParameterBundle FindFamily"));
		TRAPD (err, subConParameterFamily = iSubConParameterBundle->FindFamily(KSubConQoSFamily));

		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("FindFamily() failed with error %d"), err);
			SetError(err);
			}
		}
	}

void CT_DataRSubConParameterBundle::DoCmdClose()
/**
Test code for RSubConParameterBundle Close

Ini file options:
	none

@internalComponent
*/
	{
	INFO_PRINTF1(_L("Calling RSubConParameterBundle Close"));
	iSubConParameterBundle->Close();
	}

void CT_DataRSubConParameterBundle::DoCmdDestructor()
/**
Test code for RSubConParameterBundle Destructor

Ini file options:
	none

@internalComponent
*/
	{
	if( iSubConParameterBundle )
		{
		INFO_PRINTF1(_L("Calling RSubConParameterBundle Destructor"));
		delete iSubConParameterBundle;
		iSubConParameterBundle = NULL;
		}
	else
		{
		ERR_PRINTF1(_L("RSubConParameterBundle object invalid, can't call destructor"));
		SetBlockResult(EFail);
		}
	}


