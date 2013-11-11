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

#include "T_HALData.h"

/*@{*/
_LIT(KDeviceId,								"deviceid");
_LIT(KHalEValue,							"halEValue");
_LIT(KValue,								"value");
_LIT(KExpected,								"expected");

_LIT(KCmdSet,								"Set");
_LIT(KCmdGet,								"Get");

_LIT(KLogEnumNotFound,						"Enum '%S' not found");
_LIT(KLogError,								"Error=%d");
_LIT(KLogMissingParameter,					"Missing parameter '%S'");
_LIT(KLogActualValue,						"Actual Value '%d' 0x%x");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_HALData::CT_HALData(const THalTableLookup* aTable)
:	CDataWrapperBase()
,	iTable(aTable)
,	iValue(-1)
	{
	}

CT_HALData::~CT_HALData()
/**
 * Public destructor
 */
	{
	}

TAny* CT_HALData::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return NULL;
	}

const CT_HALData::THalTableLookup* CT_HALData::LookUp(const TDesC& aValue)
	{
	const THalTableLookup*	ret=NULL;
	TInt					index=0;
	while ( (iTable[index].iHalFunctionSetPrepare!=NULL) &&
			(iTable[index].iHalFunctionGetValidation!=NULL) && (ret==NULL) )
		{
		if ( iTable[index].iHalString==aValue )
			{
			ret=&iTable[index];
			}
		else
			{
			++index;
			}
		}
	return ret;
	}

TBool CT_HALData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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

	if ( aCommand==KCmdSet )
		{
		err=DoCmdSet(aSection);
		}
	else if ( aCommand==KCmdGet)
		{
		err=DoCmdGet(aSection);
		}
	else
		{
		ret=CDataWrapperBase::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}

	return ret;
	}

TInt CT_HALData::DoCmdSet(const TDesC& aSection)
	{
	TInt	err=KErrNone;

	//	Do we have a device id
	TInt	deviceId=0;
	TBool	hasDeviceId=GetIntFromConfig(aSection, KDeviceId(), deviceId);

	//	Get the HAL variable to set
	TPtrC	enumString;
	if ( GetStringFromConfig(aSection, KHalEValue(), enumString) )
		{
		const THalTableLookup*	entry=LookUp(enumString);

		if ( entry==NULL )
			{
			//	HAL variable not valid
			ERR_PRINTF2(KLogEnumNotFound, &enumString);
			SetBlockResult(EFail);
			}
		else
			{
			TInt	value=0;
			//	Read data from ini file for the value to set
			if ( entry->iHalFunctionSetPrepare(this, aSection, value) )
				{
				//	Set the value
				if ( hasDeviceId )
					{
					err=HAL::Set(deviceId, entry->iHalAttribute, value);
					}
				else
					{
					err=HAL::Set(entry->iHalAttribute, value);
					}

				if ( err==KErrNone )
					{
					//	Set was successful so store it locally
					entry->iHalFunctionStore(this, value);
					}
				}
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KHalEValue());
		SetBlockResult(EFail);
		}

	return err;
	}

TInt CT_HALData::DoCmdGet(const TDesC& aSection)
	{
	TInt	err=KErrNone;

	//	Do we have a device id
	TInt	deviceId=0;
	TBool	hasDeviceId=GetIntFromConfig(aSection, KDeviceId(), deviceId);

	//	Get the HAL variable to set
	TPtrC	enumString;
	if ( GetStringFromConfig(aSection, KHalEValue(), enumString) )
		{
		const THalTableLookup*	entry=LookUp(enumString);

		if ( entry==NULL )
			{
			//	HAL variable not valid
			ERR_PRINTF2(KLogEnumNotFound, &enumString);
			SetBlockResult(EFail);
			}
		else
			{
			//	HAL::Get testing works within a range (needed if we request the full palette)
			//	Get the range of values to get
			TInt	valueStart=0;
			TInt	valueEnd=0;
			if ( entry->iHalFunctionGetPrepare(this, aSection, valueStart, valueEnd) )
				{
				for ( iValue=valueStart; (iValue<=valueEnd) && (err==KErrNone); ++iValue )
					{
					//	Get the value
					TInt	value=iValue;
					if ( hasDeviceId )
						{
						err=HAL::Get(deviceId, entry->iHalAttribute, value);
						}
					else
						{
						err=HAL::Get(entry->iHalAttribute, value);
						}

					if ( err==KErrNone )
						{
						//	Get was successful validate the returned value and store it locally
						INFO_PRINTF3(KLogActualValue, value, value);
						entry->iHalFunctionGetValidation(this, aSection, value, entry->iHalForceValidation);
						entry->iHalFunctionStore(this, value);
						}
					}
				}
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KHalEValue());
		SetBlockResult(EFail);
		}

	return err;
	}

//	Prepare the value we are setting as a TInt
TBool CT_HALData::SetPrepareInt(CDataWrapperBase* aThis, const TDesC& aSection, TInt& aValue)
	{
	TBool	ret=aThis->GetIntFromConfig(aSection, KValue(), aValue);
	if ( !ret )
		{
		aThis->ERR_PRINTF2(KLogMissingParameter, &KValue());
		aThis->SetBlockResult(EFail);
		}

	return ret;
	}

//	Prepare the value we are setting as a TUint
TBool CT_HALData::SetPrepareUint(CDataWrapperBase* aThis, const TDesC& aSection, TInt& aValue)
	{
	TUint	uint=aValue;
	TBool	ret=aThis->GetUintFromConfig(aSection, KValue(), uint);
	if ( !ret )
		{
		aThis->ERR_PRINTF2(KLogMissingParameter, &KValue());
		aThis->SetBlockResult(EFail);
		}

	aValue=uint;
	return ret;
	}

//	Validate the returned value from a HAL::Get as a TBool
void CT_HALData::GetValidationBool(CDataWrapperBase* aThis, const TDesC& aSection, TInt aValue, TBool aForceValidation)
	{
	TBool	expectedValue;
	TBool	ret=aThis->GetBoolFromConfig(aSection, KExpected(), expectedValue);
	if ( !ret )
		{
		if ( aForceValidation )
			{
			aThis->ERR_PRINTF2(KLogMissingParameter, &KExpected());
			aThis->SetBlockResult(EFail);
			}
		}
	else
		{
		if ( aValue!=expectedValue )
			{
			aThis->SetBlockResult(EFail);
			aThis->ERR_PRINTF3(_L("Retrived Value(%d) != expectedValue(%d)"), aValue, expectedValue);
			}
		}
	}

//	Validate the returned value from a HAL::Get as a TInt
void CT_HALData::GetValidationInt(CDataWrapperBase* aThis, const TDesC& aSection, TInt aValue, TBool aForceValidation)
	{
	TInt	expectedValue;
	TBool	ret=aThis->GetIntFromConfig(aSection, KExpected(), expectedValue);
	if ( !ret )
		{
		if ( aForceValidation )
			{
			aThis->ERR_PRINTF2(KLogMissingParameter, &KExpected());
			aThis->SetBlockResult(EFail);
			}
		}
	else
		{
		if ( aValue!=expectedValue )
			{
			aThis->SetBlockResult(EFail);
			aThis->ERR_PRINTF3(_L("Retrived Value(%d) != expectedValue(%d)"), aValue, expectedValue);
			}
		}
	}

//	Validate the returned value from a HAL::Get as a TUint
void CT_HALData::GetValidationUint(CDataWrapperBase* aThis, const TDesC& aSection, TInt aValue, TBool aForceValidation)
	{
	TUint	expectedValue;
	TBool	ret=aThis->GetUintFromConfig(aSection, KExpected(), expectedValue);
	if ( !ret )
		{
		if ( aForceValidation )
			{
			aThis->ERR_PRINTF2(KLogMissingParameter, &KExpected());
			aThis->SetBlockResult(EFail);
			}
		}
	else
		{
		if ( (TUint)aValue!=expectedValue )
			{
			aThis->SetBlockResult(EFail);
			aThis->ERR_PRINTF3(_L("Retrived Value(%d) != expectedValue(%d)"), aValue, expectedValue);
			}
		}
	}
