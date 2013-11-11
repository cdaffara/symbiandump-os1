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

#include "T_DataSubConQosGenericParamSet.h"

/*@{*/
///	Constant Literals used.
_LIT(KConstructor,							"Constructor");
_LIT(KDestructor,							"~");
_LIT(KGetDownLinkAveragePacketSize,			"GetDownLinkAveragePacketSize");
_LIT(KGetDownLinkBandwidth,					"GetDownLinkBandwidth");
_LIT(KGetDownLinkDelay,						"GetDownLinkDelay");
_LIT(KGetDownLinkDelayVariation,			"GetDownLinkDelayVariation");
_LIT(KGetDownLinkMaximumBurstSize,			"GetDownLinkMaximumBurstSize");
_LIT(KGetDownLinkMaximumPacketSize,			"GetDownLinkMaximumPacketSize");
_LIT(KGetDownLinkPriority,					"GetDownLinkPriority");
_LIT(KGetHeaderMode,						"GetHeaderMode");
_LIT(KGetName,								"GetName");
_LIT(KGetUpLinkAveragePacketSize,			"GetUpLinkAveragePacketSize");
_LIT(KGetUpLinkBandwidth,					"GetUpLinkBandwidth");
_LIT(KGetUpLinkDelay,						"GetUpLinkDelay");
_LIT(KGetUpLinkDelayVariation,				"GetUpLinkDelayVariation");
_LIT(KGetUpLinkMaximumBurstSize,			"GetUpLinkMaximumBurstSize");
_LIT(KGetUpLinkMaximumPacketSize,			"GetUpLinkMaximumPacketSize");
_LIT(KGetUpLinkPriority,					"GetUpLinkPriority");
_LIT(KSetDownLinkAveragePacketSize,			"SetDownLinkAveragePacketSize");
_LIT(KSetDownLinkBandwidth,					"SetDownLinkBandwidth");
_LIT(KSetDownLinkDelay,						"SetDownLinkDelay");
_LIT(KSetDownLinkDelayVariation,			"SetDownLinkDelayVariation");
_LIT(KSetDownLinkMaximumBurstSize,			"SetDownLinkMaximumBurstSize");
_LIT(KSetDownLinkMaximumPacketSize,			"SetDownLinkMaximumPacketSize");
_LIT(KSetDownLinkPriority,					"SetDownLinkPriority");
_LIT(KSetHeaderMode,						"SetHeaderMode");
_LIT(KSetName,								"SetName");
_LIT(KSetUpLinkAveragePacketSize,			"SetUpLinkAveragePacketSize");
_LIT(KSetUpLinkBandwidth,					"SetUpLinkBandwidth");
_LIT(KSetUpLinkDelay,						"SetUpLinkDelay");
_LIT(KSetUpLinkDelayVariation,				"SetUpLinkDelayVariation");
_LIT(KSetUpLinkMaximumBurstSize,			"SetUpLinkMaximumBurstSize");
_LIT(KSetUpLinkMaximumPacketSize,			"SetUpLinkMaximumPacketSize");
_LIT(KSetUpLinkPriority,					"SetUpLinkPriority");


_LIT(KSubConParameterFamily,				"subconparameterfamily");
_LIT(KParamSetType,							"paramsettype");
_LIT(KUseGetGenericSet,						"usegetgenericset");
_LIT(KExpectedValue,						"expectedvalue");
_LIT(KParameterValue,						"parametervalue");
/*@}*/

CT_DataSubConQosGenericParamSet* CT_DataSubConQosGenericParamSet::NewL()
	{
	CT_DataSubConQosGenericParamSet*	ret=new (ELeave) CT_DataSubConQosGenericParamSet();
	return ret;
	}


CT_DataSubConQosGenericParamSet::CT_DataSubConQosGenericParamSet()
:	CT_DataCommsBase()
,	iSubConQosGenericParamSet(NULL)
	{
	}


CT_DataSubConQosGenericParamSet::~CT_DataSubConQosGenericParamSet()
	{
	//delete iSubConQosGenericParamSet;
	//iSubConQosGenericParamSet = NULL;
	}


TBool CT_DataSubConQosGenericParamSet::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;

	if ( aCommand==KConstructor )
		{
		DoCmdConstructorL(aSection);
		}
	else if( aCommand==KDestructor )
		{
		DoCmdDestructor();
		}
	else if( aCommand==KGetDownLinkAveragePacketSize )
		{
		DoCmdGetDownLinkAveragePacketSize(aSection);
		}
	else if( aCommand==KGetDownLinkBandwidth )
		{
		DoCmdGetDownLinkBandwidth(aSection);
		}
	else if( aCommand==KGetDownLinkDelay )
		{
		DoCmdGetDownLinkDelay(aSection);
		}
	else if( aCommand==KGetDownLinkDelayVariation )
		{
		DoCmdGetDownLinkDelayVariation(aSection);
		}
	else if( aCommand==KGetDownLinkMaximumBurstSize )
		{
		DoCmdGetDownLinkMaximumBurstSize(aSection);
		}
	else if( aCommand==KGetDownLinkMaximumPacketSize )
		{
		DoCmdGetDownLinkMaximumPacketSize(aSection);
		}
	else if( aCommand==KGetDownLinkPriority )
		{
		DoCmdGetDownLinkPriority(aSection);
		}
	else if( aCommand==KGetHeaderMode )
		{
		DoCmdGetHeaderMode(aSection);
		}
	else if( aCommand==KGetName )
		{
		DoCmdGetName(aSection);
		}
	else if( aCommand==KGetUpLinkAveragePacketSize )
		{
		DoCmdGetUpLinkAveragePacketSize(aSection);
		}
	else if( aCommand==KGetUpLinkBandwidth )
		{
		DoCmdGetUpLinkBandwidth(aSection);
		}
	else if( aCommand==KGetUpLinkDelay )
		{
		DoCmdGetUpLinkDelay(aSection);
		}
	else if( aCommand==KGetUpLinkDelayVariation )
		{
		DoCmdGetUpLinkDelayVariation(aSection);
		}
	else if( aCommand==KGetUpLinkMaximumBurstSize )
		{
		DoCmdGetUpLinkMaximumBurstSize(aSection);
		}
	else if( aCommand==KGetUpLinkMaximumPacketSize )
		{
		DoCmdGetUpLinkMaximumPacketSize(aSection);
		}
	else if( aCommand==KGetUpLinkPriority )
		{
		DoCmdGetUpLinkPriority(aSection);
		}
	else if( aCommand==KSetDownLinkAveragePacketSize )
		{
		DoCmdSetDownLinkAveragePacketSize(aSection);
		}
	else if( aCommand==KSetDownLinkBandwidth )
		{
		DoCmdSetDownLinkBandwidth(aSection);
		}
	else if( aCommand==KSetDownLinkDelay )
		{
		DoCmdSetDownLinkDelay(aSection);
		}
	else if( aCommand==KSetDownLinkDelayVariation )
		{
		DoCmdSetDownLinkDelayVariation(aSection);
		}
	else if( aCommand==KSetDownLinkMaximumBurstSize )
		{
		DoCmdSetDownLinkMaximumBurstSize(aSection);
		}
	else if( aCommand==KSetDownLinkMaximumPacketSize )
		{
		DoCmdSetDownLinkMaximumPacketSize(aSection);
		}
	else if( aCommand==KSetDownLinkPriority )
		{
		DoCmdSetDownLinkPriority(aSection);
		}
	else if( aCommand==KSetHeaderMode )
		{
		DoCmdSetHeaderMode(aSection);
		}
	else if( aCommand==KSetName )
		{
		DoCmdSetName(aSection);
		}
	else if( aCommand==KSetUpLinkAveragePacketSize )
		{
		DoCmdSetUpLinkAveragePacketSize(aSection);
		}
	else if( aCommand==KSetUpLinkBandwidth )
		{
		DoCmdSetUpLinkBandwidth(aSection);
		}
	else if( aCommand==KSetUpLinkDelay )
		{
		DoCmdSetUpLinkDelay(aSection);
		}
	else if( aCommand==KSetUpLinkDelayVariation )
		{
		DoCmdSetUpLinkDelayVariation(aSection);
		}
	else if( aCommand==KSetUpLinkMaximumBurstSize )
		{
		DoCmdSetUpLinkMaximumBurstSize(aSection);
		}
	else if( aCommand==KSetUpLinkMaximumPacketSize )
		{
		DoCmdSetUpLinkMaximumPacketSize(aSection);
		}
	else if( aCommand==KSetUpLinkPriority )
		{
		DoCmdSetUpLinkPriority(aSection);
		}
	else
		{
		ret = EFalse;
		}

	return ret;
	}


TAny* CT_DataSubConQosGenericParamSet::GetObject()
	{
	return iSubConQosGenericParamSet;
	}


void CT_DataSubConQosGenericParamSet::DoCmdConstructorL( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet Constructor

Ini file options:
	subconparameterfamily	- a CSubConParameterFamily dictionary entry
	paramsettype			- parameter set type to be used
	usegetgenericset		- whether to use GetGenericSet() instead of NewL()

@internalComponent
*/
	{
	TPtrC		subConParameterFamilyName;

	if( !GetStringFromConfig( aSection, KSubConParameterFamily(), subConParameterFamilyName) )
		{
		MissingParamError( KSubConParameterFamily() );
		}
	else
		{
		CSubConParameterFamily*	subConParameterFamily = static_cast<CSubConParameterFamily*>(GetDataObjectL(subConParameterFamilyName));

		if (subConParameterFamily == NULL)
			{
			ERR_PRINTF1(_L("CSubConQosGenericParamSet Constructor - CSubConParameterFamily is NULL"));
			SetBlockResult(EFail);
			}

		TPtrC										paramSetTypeName;
		CSubConParameterFamily::TParameterSetType	paramSetType = CSubConParameterFamily::ERequested;

		if (GetStringFromConfig( aSection, KParamSetType(), paramSetTypeName) )
			{
			if(ParameterSetType(paramSetTypeName, paramSetType))
				{
				INFO_PRINTF3(_L("CSubConQosGenericParamSet Constructor - Using paramsettype %S (%d)"), &paramSetTypeName, paramSetType);
				}
			else
				{
				TInt	paramSetTypeInt;
				if( GetIntFromConfig( aSection, KParamSetType(), paramSetTypeInt) )
					{
					paramSetType = (CSubConParameterFamily::TParameterSetType)paramSetTypeInt;
					INFO_PRINTF2(_L("CSubConQosGenericParamSet Constructor - Using paramsettype %d"), paramSetType);
					}
				else
					{
					ERR_PRINTF2(_L("CSubConQosGenericParamSet Constructor - Given paramsettype (%S) is invalid"), &paramSetTypeName);
					SetBlockResult(EFail);
					}
				}
			}

		TBool		useGetGenericSet = ETrue;

		if (GetBoolFromConfig( aSection, KUseGetGenericSet(), useGetGenericSet) )
			{
			if (useGetGenericSet)
				{
				INFO_PRINTF3(_L("Calling %S -> GetGenericSet( %d )"), &subConParameterFamilyName, paramSetType );
				TRAPD (err, iSubConQosGenericParamSet = (CSubConQosGenericParamSet*)subConParameterFamily->GetGenericSet(paramSetType));

				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("GetGenericSet() failed with error %d"), err);
					SetError(err);
					}
				}
			else
				{
				INFO_PRINTF3(_L("Calling CSubConQosGenericParamSet::NewL( %S, %d )"), &subConParameterFamilyName, paramSetType );
				TRAPD (err, iSubConQosGenericParamSet = CSubConQosGenericParamSet::NewL(*subConParameterFamily, paramSetType));

				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("CSubConQosGenericParamSet::NewL() failed with error %d"), err);
					SetError(err);
					}
				}
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdDestructor()
/**
Test code for CSubConQosGenericParamSet Destructor

Ini file options:
	none

@internalComponent
*/
	{
	if( iSubConQosGenericParamSet )
		{
		INFO_PRINTF1(_L("Calling CSubConQosGenericParamSet Destructor"));
	//	delete iSubConQosGenericParamSet;
	//	iSubConQosGenericParamSet = NULL;
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetDownLinkAveragePacketSize( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetDownLinkAveragePacketSize

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetDownLinkAveragePacketSize();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetDownLinkAveragePacketSize() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetDownLinkBandwidth( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetDownlinkBandwidth

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetDownlinkBandwidth();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetDownlinkBandwidth() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetDownLinkDelay( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetDownLinkDelay

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetDownLinkDelay();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetDownLinkDelay() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetDownLinkDelayVariation( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetDownLinkDelayVariation

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetDownLinkDelayVariation();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetDownLinkDelayVariation() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}

void CT_DataSubConQosGenericParamSet::DoCmdGetDownLinkMaximumBurstSize( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetDownLinkMaximumBurstSize

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetDownLinkMaximumBurstSize();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetDownLinkMaximumBurstSize() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetDownLinkMaximumPacketSize( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetDownLinkMaximumPacketSize

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetDownLinkMaximumPacketSize();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetDownLinkMaximumPacketSize() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetDownLinkPriority( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetDownLinkPriority

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetDownLinkPriority();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetDownLinkPriority() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetHeaderMode( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetHeaderMode

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TBool		expectedHeaderMode;

	if(!GetBoolFromConfig( aSection, KExpectedValue(), expectedHeaderMode))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TBool actualHeaderMode = iSubConQosGenericParamSet->GetHeaderMode();

		if (expectedHeaderMode != actualHeaderMode)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetHeaderMode() error - expected header mode = %d, actual header mode = %d"), expectedHeaderMode, actualHeaderMode);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetName( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetName

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TPtrC		expectedName;

	if(!GetStringFromConfig( aSection, KExpectedValue(), expectedName))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TName	actualName = iSubConQosGenericParamSet->GetName();

		if (expectedName != actualName)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetName() error - expected name = %S, actual name = %S"), &expectedName, &actualName);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetUpLinkAveragePacketSize( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetUpLinkAveragePacketSize

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetUpLinkAveragePacketSize();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetUpLinkAveragePacketSize() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetUpLinkBandwidth( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetUplinkBandwidth

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetUplinkBandwidth();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetUplinkBandwidth() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetUpLinkDelay( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetUpLinkDelay

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetUpLinkDelay();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetUpLinkDelay() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetUpLinkDelayVariation( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetUpLinkDelayVariation

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetUpLinkDelayVariation();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetUpLinkDelayVariation() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetUpLinkMaximumBurstSize( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetUpLinkMaximumBurstSize

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetUpLinkMaximumBurstSize();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetUpLinkMaximumBurstSize() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetUpLinkMaximumPacketSize( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetUpLinkMaximumPacketSize

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetUpLinkMaximumPacketSize();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetUpLinkMaximumPacketSize() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdGetUpLinkPriority( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::GetUpLinkPriority

Ini file options:
	expectedvalue - Expected parameter value

@internalComponent
*/
	{
	TInt		expectedParamValue;

	if(!GetIntFromConfig( aSection, KExpectedValue(), expectedParamValue))
		{
		MissingParamError( KExpectedValue() );
		}
	else
		{
		TInt actualParamValue = iSubConQosGenericParamSet->GetUpLinkPriority();

		if (expectedParamValue != actualParamValue)
			{
			ERR_PRINTF3(_L("CSubConQosGenericParamSet::GetUpLinkPriority() error - expected parameter value = %d, actual parameter value = %d"), expectedParamValue, actualParamValue);
			SetBlockResult(EFail);
			}
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetDownLinkAveragePacketSize( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetDownLinkAveragePacketSize

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetDownLinkAveragePacketSize(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetDownLinkBandwidth( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetDownlinkBandwidth

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetDownlinkBandwidth(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetDownLinkDelay( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetDownLinkDelay

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetDownLinkDelay(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetDownLinkDelayVariation( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetDownLinkDelayVariation

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetDownLinkDelayVariation(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetDownLinkMaximumBurstSize( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetDownLinkMaximumBurstSize

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetDownLinkMaximumBurstSize(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetDownLinkMaximumPacketSize( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetDownLinkMaximumPacketSize

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetDownLinkMaximumPacketSize(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetDownLinkPriority( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetDownLinkPriority

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetDownLinkPriority(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetHeaderMode( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetHeaderMode

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TBool		headerValue;

	if(!GetBoolFromConfig( aSection, KParameterValue(), headerValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetHeaderMode(headerValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetName( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetName

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TPtrC		name;

	if(!GetStringFromConfig( aSection, KParameterValue(), name))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetName(name);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetUpLinkAveragePacketSize( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetUpLinkAveragePacketSize

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetUpLinkAveragePacketSize(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetUpLinkBandwidth( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetUplinkBandwidth

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetUplinkBandwidth(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetUpLinkDelay( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetUpLinkDelay

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetUpLinkDelay(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetUpLinkDelayVariation( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetUpLinkDelayVariation

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetUpLinkDelayVariation(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetUpLinkMaximumBurstSize( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetUpLinkMaximumBurstSize

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetUpLinkMaximumBurstSize(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetUpLinkMaximumPacketSize( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetUpLinkMaximumPacketSize

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig( aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetUpLinkMaximumPacketSize(paramValue);
		}
	}


void CT_DataSubConQosGenericParamSet::DoCmdSetUpLinkPriority( const TDesC& aSection )
/**
Test code for CSubConQosGenericParamSet::SetUpLinkPriority

Ini file options:
	parametervalue - Parameter value to be set

@internalComponent
*/
	{
	TInt		paramValue;

	if(!GetIntFromConfig(aSection, KParameterValue(), paramValue))
		{
		MissingParamError( KParameterValue() );
		}
	else
		{
		iSubConQosGenericParamSet->SetUpLinkPriority(paramValue);
		}
	}
