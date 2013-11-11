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

#include "t_rpropertydata.h"
#include <e32property.h>
#include <hwrmpowerstatesdkpskeys.h>

/*@{*/
//LIT's for commands
_LIT(KCmdNewL,						"NewL");
_LIT(KCmdGet,						"Get");
_LIT(KCmdAttach,					"Attach");
_LIT(KCmdSubscribe,				"Subscribe");
_LIT(KCmdDestructor,				"~");
/*@}*/

/*@{*/
//LIT's for reading params from ini file
_LIT(KCategory,					"category");
_LIT(KKey,						"key");
/*@}*/

//*@{*/
/*Enumeration Literals*/
_LIT(KKPSUidHWRMPowerState,		"KPSUidHWRMPowerState");
_LIT(KKHWRMBatteryLevel,		"KHWRMBatteryLevel");
_LIT(KKHWRMBatteryStatus,		"KHWRMBatteryStatus");
_LIT(KKHWRMChargingStatus,		"KHWRMChargingStatus");
/*@}*/

const CDataWrapperBase::TEnumEntryTable CT_RPropertyData::iEnumPropertyKeys[] = 
	{ 
	{ KKPSUidHWRMPowerState,	KPSUidHWRMPowerState.iUid },	/* = 0x10205041*/
	{ KKHWRMBatteryLevel,		KHWRMBatteryLevel },			/* = 0x00000001*/
	{ KKHWRMBatteryStatus,		KHWRMBatteryStatus },			/* = 0x00000002*/
	{ KKHWRMChargingStatus,		KHWRMChargingStatus}			/* = 0x00000003*/
	};


/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_RPropertyData* CT_RPropertyData::NewL()
	{
	CT_RPropertyData* ret = new (ELeave) CT_RPropertyData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
 * Protected constructor. First phase construction
 */
CT_RPropertyData::CT_RPropertyData()
	: iProperty(NULL), iActiveCallback(NULL)
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
void CT_RPropertyData::ConstructL()
	{
	iActiveCallback = CActiveCallback::NewL(*this);
	}

/**
 * Public destructor
 */
CT_RPropertyData::~CT_RPropertyData()
	{
	DestroyData();

	if (iActiveCallback)
		{
		delete iActiveCallback;
		iActiveCallback = NULL;
		}
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_RPropertyData::GetObject()
	{
	return &iProperty;
	}


/**
 * Process a command read from the ini file.
 *
 * @param aCommand	The command to process
 * @param aSection	The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return	ETrue if the command is processed
 *
 * @leave System wide error
 */
TBool CT_RPropertyData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
{
	TBool	ret = ETrue;

	if (aCommand == KCmdNewL)
		{
		DoCmdNewL();
		}
	else if (aCommand == KCmdGet)
		{
		DoCmdGet(aSection);
		}
	else if (aCommand == KCmdAttach)
		{
		DoCmdAttach(aSection);
		}
	else if (aCommand == KCmdSubscribe)
		{
		DoCmdSubscribe(aAsyncErrorIndex);
		}
	else if (aCommand == KCmdDestructor)
		{
		DoCmdDestructor();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

/*
 * RunL method for management Active callbacks
 * @param aActive	param to review which active call back is being finished
 * @param aIndex    not used
 */
void CT_RPropertyData::RunL(CActive* aActive, TInt /*aIndex*/)
	{
	INFO_PRINTF1(_L("*START* CT_RPropertyData::RunL"));
	
	if (aActive == iActiveCallback)
		{
		INFO_PRINTF1(_L("active call back for Subscribe."));
		
		DecOutstanding();
		TInt err = aActive->iStatus.Int();
		
		if(err == KErrNone)
			{
			INFO_PRINTF1(_L("Asynchronous task has completed. RunL  called"));
			}
		else
			{
			ERR_PRINTF2(_L("RProperty::RunL failed with error: %d"), err);
			SetError(err);
			}
		}
	
	INFO_PRINTF1(_L("*END* CT_RPropertyData::RunL"));
	}

/**
 * Create an instance of RProperty
 */
 void CT_RPropertyData::DoCmdNewL()
 	{
 	INFO_PRINTF1(_L("*START* CT_RPropertyData::DoCmdNewL"));
 	DestroyData();

 	iProperty = new (ELeave)RProperty();

 	INFO_PRINTF1(_L("*END* CT_RPropertyData::DoCmdNewL"));
 	}

/**
 * Gets an integer property. The function gets the integer value of the specified property.
 * 
 * @param aSection:		category		The UID that identifies the property category.
 * 						key				The property sub-key, i.e. the key that identifies the specific property within the category.
 */ 
void CT_RPropertyData::DoCmdGet(const TTEFSectionName& aSection)
	{
 	INFO_PRINTF1(_L("*START* CT_RPropertyData::DoCmdGet"));

 	TBool dataOk = ETrue;
 	
 	TInt categoryTInt;
	if(!GetEnumFromConfig(aSection, KCategory, iEnumPropertyKeys, categoryTInt))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCategory);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
		}
 	
 	TInt key;
	if(!GetEnumFromConfig(aSection, KKey, iEnumPropertyKeys, key))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KKey);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
		}
 	
	TInt error = KErrNone;
	
	if (dataOk)
		{
		TInt value = -1;
		
		/***** API method call *****/
		error = iProperty->Get( TUid::Uid(categoryTInt), key, value );
		
		INFO_PRINTF2(_L("Property key: %d"), key);
		INFO_PRINTF2(_L("Property value: %d"), value);
		}
	
	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("RProperty::Get failed with error: %d"), error);
		SetError(error);
		}
	
 	INFO_PRINTF1(_L("*END* CT_RPropertyData::DoCmdGet"));
	}

/**
 * Attaches to the specified property. 
 * 
 * @param aSection:		category			The UID that identifies the property category.
 * 						key					The property sub-key, i.e. the key that identifies the specific property within the category.
 */ 
void CT_RPropertyData::DoCmdAttach(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_RPropertyData::DoCmdAttach"));
	TBool dataOk = ETrue;

 	TInt categoryTInt;
	if(!GetEnumFromConfig(aSection, KCategory, iEnumPropertyKeys, categoryTInt))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCategory);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
		}
 	
 	TInt key;
	if(!GetEnumFromConfig(aSection, KKey, iEnumPropertyKeys, key))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KKey);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
		}

	TInt err = KErrNone;

	if (dataOk)
		{
		/***** API method call *****/
		err = iProperty->Attach(TUid::Uid(categoryTInt), key);
		}

	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("RProperty::Attach failed with error: %d"), err);
		SetError(err);
		}

	INFO_PRINTF1(_L("*END* CT_RPropertyData::DoCmdAttach"));
	}

/**
 * Subscribes to a property.
 * 
 * @param aAsyncErrorIndex
 */ 
void CT_RPropertyData::DoCmdSubscribe(const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START* CT_RPropertyData::DoCmdSubscribe"));
	
	/***** API method call *****/
	iProperty->Subscribe(iActiveCallback->iStatus);
	
	iActiveCallback->Activate(aAsyncErrorIndex);
	IncOutstanding();
	
	INFO_PRINTF1(_L("*END* CT_RPropertyData::DoCmdSubscribe"));
	}


/**
 * Command destructor.
 */
void CT_RPropertyData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START* CT_RPropertyData::DoCmdDestructor"));
	DestroyData();
	
	if (iActiveCallback)
		{
		delete iActiveCallback;
		iActiveCallback = NULL;
		}
	INFO_PRINTF1(_L("*END* CT_RPropertyData::DoCmdDestructor"));
	}

 /**
  * Destroy an instance of RProperty.
  */
 void CT_RPropertyData::DestroyData()
 	{
	if(iProperty)
		{
		delete iProperty;
		iProperty = NULL;
		}
 	}
