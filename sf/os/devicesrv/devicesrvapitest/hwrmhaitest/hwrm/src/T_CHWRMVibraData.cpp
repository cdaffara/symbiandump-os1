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

#include "t_chwrmvibradata.h"
#include <hwrmvibra.h>

/*@{*/
//LIT's for commands
_LIT( KCmdNewL,					"NewL");
_LIT( KCmdDestructor,				"~");
_LIT( KCmdReserveVibraL,			"ReserveVibraL");
_LIT( KCmdStartVibraL,				"StartVibraL");
_LIT( KCmdVibraStatus,				"VibraStatus");
/*@}*/

/*@{*/
//LIT's for reading params from ini file
_LIT( KDuration,		"duration");
_LIT( KIntensity,		"intensity");
_LIT( KRestoreState,	"restoreState");
_LIT( KForceNoCCoeEnv,	"forceNoCCoeEnv");
_LIT( KStatus,			"status");
_LIT( KObserver,		"observer");
/*@}*/

/*@{*/
_LIT(KLogError,				"Error=%d");
_LIT(KLogMissingParameter,	"Missing parameter '%S'");
_LIT(KLogDefaultValueUsed,	"Missing parameter '%S'. Using default value of '%d'");
/*@}*/

//*@{*/
/*LIT's for TLightStatus enumeration*/
_LIT(KEVibraStatusUnknown,		"EVibraStatusUnknown");
_LIT(KEVibraStatusNotAllowed,	"EVibraStatusNotAllowed");
_LIT(KEVibraStatusStopped,		"EVibraStatusStopped");
_LIT(KEVibraStatusOn,			"EVibraStatusOn");
/*@}*/

const CDataWrapperBase::TEnumEntryTable CT_CHWRMVibraData::iEnumVibraStatus[] = 
	{ 
	{ KEVibraStatusUnknown,		CHWRMVibra::EVibraStatusUnknown },
	{ KEVibraStatusNotAllowed,	CHWRMVibra::EVibraStatusNotAllowed },
	{ KEVibraStatusStopped,		CHWRMVibra::EVibraStatusStopped },
	{ KEVibraStatusOn,			CHWRMVibra::EVibraStatusOn }
	};


/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_CHWRMVibraData* CT_CHWRMVibraData::NewL()
	{
	CT_CHWRMVibraData* ret=new (ELeave) CT_CHWRMVibraData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


/**
 * Protected constructor. First phase construction
 */
CT_CHWRMVibraData::CT_CHWRMVibraData():
	iVibra(NULL)
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
void CT_CHWRMVibraData::ConstructL()
	{
	}

/**
 * Public destructor
 */
CT_CHWRMVibraData::~CT_CHWRMVibraData()
	{
	DestroyData();	
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_CHWRMVibraData::GetObject()
	{
	return iVibra;
	}

/**
 * Process a command read from the ini file
 *
 * @param aCommand	The command to process
 * @param aSection	The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return	ETrue if the command is processed
 *
 * @leave System wide error
 */
TBool CT_CHWRMVibraData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;
	TInt	err = KErrNone;

	if (aCommand == KCmdNewL)
		{
		err = DoCmdNewL(aSection);
		}
	else if (aCommand == KCmdReserveVibraL)
		{
		err = DoCmdReserveVibraL(aSection);
		}
	else if (aCommand == KCmdStartVibraL)
		{
		err = DoCmdStartVibraL(aSection);
		}
	else if (aCommand == KCmdVibraStatus)
		{
		err = DoCmdVibraStatus(aSection);
		}
	else if (aCommand == KCmdDestructor)
		{
		err = DoCmdDestructor();
		}
	else
		{
		ret=EFalse;
		}

	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}

	return ret;
	}

/**
 * Create an instance of CHWRMVibra
 * 
 * @param aSection:		observer
 * 
 * @return
 */
TInt CT_CHWRMVibraData::DoCmdNewL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CHWRMVibraData::DoCmdNewL"));
	DestroyData();

	TInt err = KErrNone;
	TInt observer = 0;
	if (!GetIntFromConfig(aSection, KObserver(), observer))
		{
		// Empty NewL
		TRAP(err, iVibra = CHWRMVibra::NewL());
		}
	else
		{
		CHWRMVibra* client = NULL;
		// Call NewL with null parameter
		TRAP(err, client = CHWRMVibra::NewL(NULL));
		delete client;
		client = NULL;
		}

	INFO_PRINTF1(_L("*END* CT_CHWRMVibraData::DoCmdNewL"));
	return err;
	}

/**
 * Reserves vibration feature exclusively for this client.
 * 
 * @param aSection:		restoreState	If ETrue, the state frozen on last release will be restored upon successful reservation. I.e. if vibra was on when it was released by this client
 * 										the last time, it would continue the vibrating upon successful reservation. For the first reservation of each session this parameter is always 
 * 										considered EFalse regardless of what is supplied, as there is no previous frozen state to restore.
 * 						forceNoCCoeEnv	If EFalse, then reservation requires that this client has the keyboard focus at the time of reservation and vibra will be automatically released 
 * 										and re-reserved based on the keyboard focus status of the this client. This also implies that CCoeEnv::Static() != NULL is required. If ETrue, 
 * 										the client will not require CCoeEnv to be present nor does it automatically reserve/release vibra by depending on foreground/background status of 
 * 										the client. Only trusted clients are allowed to set this flag to ETrue. Client is considered trusted if it has nonstandard priority defined in the 
 * 										vibra policy.
 * 
 * @return
 */ 
TInt CT_CHWRMVibraData::DoCmdReserveVibraL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CHWRMVibraData::DoCmdReserveVibraL"));
	TBool dataOk = ETrue;
	
	TBool restoreState = EFalse;
	GETFROMCONFIGMANDATORY(Bool, aSection, KRestoreState(), restoreState, KLogMissingParameter, dataOk);

	TBool forceNoCCoeEnv = ETrue;
	GETFROMCONFIGMANDATORY(Bool, aSection, KForceNoCCoeEnv(), forceNoCCoeEnv, KLogMissingParameter, dataOk);

	TInt err = KErrNone;
	if ( dataOk )
		{
		TRAP(err, iVibra->ReserveVibraL(restoreState, forceNoCCoeEnv));
		}

	INFO_PRINTF1(_L("*END* CT_CHWRMVibraData::DoCmdReserveVibraL"));
	return err;
	}

/**
 * Starts the device vibration feature.
 * 
 * @param aSection:		duration		Duration of the vibration measured in milliseconds.
 * 						intensity		Intensity of the vibra in decimal is -100 to 100, which shows the percentage of the vibra motor full rotation speed.
 * 
 * @return
 */ 
TInt CT_CHWRMVibraData::DoCmdStartVibraL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CHWRMVibraData::DoCmdStartVibraL"));
	
	TInt duration = 0;
	GETFROMCONFIGOPTIONAL(Int, aSection, KDuration(), duration, KLogDefaultValueUsed);
	
	TInt intensity = KHWRMVibraMaxIntensity;
	GETFROMCONFIGOPTIONAL(Int, aSection, KIntensity(), intensity, KLogDefaultValueUsed);
	
	TInt err = KErrNone;
	TRAP(err, iVibra->StartVibraL(duration,intensity));
	
	INFO_PRINTF1(_L("*END* CT_CHWRMVibraData::DoCmdStartVibraL"));
	return err;
	}

/**
 * This method prints the current vibra status.
 * Vibra status: EVibraStatusUnknown, EVibraStatusNotAllowed, EVibraStatusStopped, EVibraStatusOn
 * 
 * @param aSection:		expectedStatus		Vibra status that is expected.
 * 
 * @return
 */ 
TInt CT_CHWRMVibraData::DoCmdVibraStatus(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CHWRMVibraData::DoCmdVibraStatus"));
	TInt err = KErrNone;
	
	TInt expectedStatus = CHWRMVibra::EVibraStatusUnknown;
	if(!GetEnumFromConfig(aSection, KStatus, iEnumVibraStatus, expectedStatus))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KStatus);
    	SetBlockResult(EFail);
		}

	TInt status = iVibra->VibraStatus();

	INFO_PRINTF2(_L("Current status is %d"), status);
	INFO_PRINTF2(_L("Expected status status is %d"), expectedStatus);

	if (status != expectedStatus)
		{
		err = KErrCorrupt;
		}

	INFO_PRINTF1(_L("*END* CT_CHWRMVibraData::DoCmdVibraStatus"));
	return err;
	}

/**
 * Command destructor
 */
TInt CT_CHWRMVibraData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START* CT_CHWRMVibraData::DoCmdDestructor"));
	DestroyData();
	INFO_PRINTF1(_L("*END* CT_CHWRMVibraData::DoCmdDestructor"));
	return KErrNone;
	}

/**
 * Destroy an instance of CHWRMVibra
 */
void CT_CHWRMVibraData::DestroyData()
	{
	if(iVibra)
		{
		delete iVibra;
		iVibra = NULL;
		}
	}
