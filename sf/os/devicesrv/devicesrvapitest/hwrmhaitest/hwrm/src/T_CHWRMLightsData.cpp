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

#include "t_chwrmlightsdata.h"
#include <hwrmlight.h>

/*@{*/
//LIT's for reading params from ini file
_LIT(KTarget,			"target");
_LIT(KLight,			"light");
_LIT(KDuration,			"duration");
_LIT(KOnDuration,		"onduration");
_LIT(KOffDuration,		"offduration");
_LIT(KIntensity,		"intensity");
_LIT(KRestoreState,		"restoreState");
_LIT(KForceNoCCoeEnv,	"forceNoCCoeEnv");
_LIT(KFadein,			"fadein");
_LIT(KFadeout,			"fadeout");
_LIT(KStatus,			"status");
_LIT(KObserver,			"observer");
/*@}*/


/*@{*/
//LIT's for commands
_LIT(KCmdNewL, 				"NewL");
_LIT(KCmdReserveLightL,		"ReserveLightL");
_LIT(KCmdLightBlinkL,		"LightBlinkL");
_LIT(KCmdLightOnL,			"LightOnL");
_LIT(KCmdLightOffL,			"LightOffL");
_LIT(KCmdLightStatus,		"LightStatus");
_LIT(KCmdDestructor,		"~");
/*@}*/

/*@{*/
_LIT(KLogError,				"Error=%d");
_LIT(KLogMissingParameter,	"Missing parameter '%S'");
_LIT(KLogDefaultValueUsed,	"Missing parameter '%S'. Using default value of '%d'");
/*@}*/

//*@{*/
/*LIT's for TLightStatus enumeration*/
_LIT(KELightStatusUnknown,		"ELightStatusUnknown");
_LIT(KELightOn,					"ELightOn");
_LIT(KELightOff,				"ELightOff");
_LIT(KELightBlink,				"ELightBlink");
/*@}*/

const CDataWrapperBase::TEnumEntryTable CT_CHWRMLightsData::iEnumLightStatus[] = 
	{ 
	{ KELightStatusUnknown,		CHWRMLight::ELightStatusUnknown },
	{ KELightOn,				CHWRMLight::ELightOn },
	{ KELightOff,				CHWRMLight::ELightOff },
	{ KELightBlink,				CHWRMLight::ELightBlink }
	};


/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_CHWRMLightsData* CT_CHWRMLightsData::NewL()
	{
	CT_CHWRMLightsData* ret = new (ELeave) CT_CHWRMLightsData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


/**
 * Protected constructor. First phase construction
 */
CT_CHWRMLightsData::CT_CHWRMLightsData()
	: iLight(NULL)
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
void CT_CHWRMLightsData::ConstructL()
	{
	}

/**
 * Public destructor
 */
CT_CHWRMLightsData::~CT_CHWRMLightsData()
	{
	DestroyData();
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_CHWRMLightsData::GetObject()
	{
	return iLight;
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
TBool CT_CHWRMLightsData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;
	TInt	err=KErrNone;

	if (aCommand == KCmdNewL)
		{
		err = DoCmdNewL(aSection);
		}
	else if (aCommand == KCmdReserveLightL)
		{
		err = DoCmdReserveLightL(aSection);
		}
	else if (aCommand == KCmdLightBlinkL)
		{
		err = DoCmdLightBlinkL(aSection);
		}
	else if (aCommand == KCmdLightOnL)
		{
		err = DoCmdLightOnL(aSection);
		}
	else if (aCommand == KCmdLightOffL)
		{
		err = DoCmdLightOffL(aSection);
		}
	else if (aCommand == KCmdLightStatus)
		{
		err = DoCmdLightStatus(aSection);
		}
	else if (aCommand == KCmdDestructor)
		{
		err = DoCmdDestructor();
		}
	else
		{
		ret=EFalse;
		}

	if ( err != KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}

	return ret;
	}

/**
 * Command NewL
 * 
 * @param aSection:		observer
 * 
 * @return
 */
TInt CT_CHWRMLightsData::DoCmdNewL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CHWRMLightsData::DoCmdNewL"));
	DestroyData();

	TInt observer = 0;
	TInt err = KErrNone;
	if (!GetIntFromConfig(aSection, KObserver(), observer))
		{
		// Empty NewL
		TRAP(err, iLight = CHWRMLight::NewL());
		}
	else
		{
		CHWRMLight* client = NULL;
		// Call NewL with null parameter
		TRAP(err, client = CHWRMLight::NewL(NULL));
		delete client;
		client = NULL;
		}
	

	INFO_PRINTF1(_L("*END* CT_CHWRMLightsData::DoCmdNewL"));
	return err;
	}

/**
* Reserves light target exclusively for this client...
* 
* @param aSection:		light			Defines which light should be reserved.
* 						restoreState	If ETrue, the state frozen on last release will be restored upon successful reservation. I.e. if light was blinking when it was released by this
* 										client the last time, it would start blinking again upon successful reservation. For the first reservation of each session this parameter is 
* 										always considered EFalse regardless of what is supplied, as there is no previous frozen state to restore.
* 						forceNoCCoeEnv	If EFalse, then reservation requires that this client is on the foreground at the time of reservation and light target will be automatically 
* 										released and re-reserved based on background/foreground status of the this client. This also implies that CCoeEnv::Static() != NULL is 
* 										required. If ETrue, the client will not require CCoeEnv to be present nor does it automatically reserve/release light by depending on 
* 										foreground/background status of the client. Only trusted clients are allowed to set this flag to ETrue. Client is considered trusted if it has 
* 										nonstandard priority defined in the lights policy.
*/ 
TInt CT_CHWRMLightsData::DoCmdReserveLightL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CHWRMLightsData::DoCmdReserveLightL"));
	TBool dataOk = ETrue;
	
	// Required parameters
	TInt light;
	GETFROMCONFIGMANDATORY(Int, aSection, KTarget, light, KLogMissingParameter, dataOk);
	
	// Optional parameters
	TBool restoreState = EFalse;
	GETFROMCONFIGOPTIONAL(Bool, aSection, KRestoreState, restoreState, KLogDefaultValueUsed);

	TBool forceNoCCoeEnv = ETrue;
	GETFROMCONFIGOPTIONAL(Bool, aSection, KForceNoCCoeEnv, forceNoCCoeEnv, KLogDefaultValueUsed);

	// API Method Call
	TInt err = KErrNone;
	if ( dataOk )
		{
		TRAP(err, iLight->ReserveLightL(light, restoreState, forceNoCCoeEnv));
		}
	
	INFO_PRINTF1(_L("*END* CT_CHWRMLightsData::DoCmdReserveLightL"));
	return err;
	}

/**
* The command blinks the target light(s) of the device for specified duration
* using specified intensity. On- and Off-cycle times of the blinking can also be controlled.
* 
* @param aSection:		light 			Defines which light should be controlled.
* 						duration		Duration of the time the light is set to blink measured in milliseconds.
* 						interval		Value in milliseconds to initialize onDuration and offDuration parameters.
* 						intensity		Intensity of the light.
*/ 
TInt CT_CHWRMLightsData::DoCmdLightBlinkL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CHWRMLightsData::DoCmdLightBlinkL"));
	TBool	dataOk=ETrue;
	
	// Required parameters
	
	TInt light;
	GETFROMCONFIGMANDATORY(Int, aSection, KLight, light, KLogMissingParameter, dataOk);

	// Optional parameters
	
	TInt duration = KHWRMInfiniteDuration;
	GETFROMCONFIGOPTIONAL(Int, aSection, KDuration, duration, KLogDefaultValueUsed);
	
	TInt onDuration = KHWRMDefaultCycleTime;
	GETFROMCONFIGOPTIONAL(Int, aSection, KOnDuration, onDuration, KLogDefaultValueUsed);

	TInt offDuration = KHWRMDefaultCycleTime;
	GETFROMCONFIGOPTIONAL(Int, aSection, KOffDuration, offDuration, KLogDefaultValueUsed);

	TInt intensity = KHWRMDefaultIntensity;
	GETFROMCONFIGOPTIONAL(Int, aSection, KIntensity, intensity, KLogDefaultValueUsed);
	
	// API Method Call
	TInt	err=KErrNone;
	if ( dataOk )
		{
		TRAP(err, iLight->LightBlinkL(light, duration, onDuration, offDuration, intensity));
		}
	
	INFO_PRINTF1(_L("*END* CT_CHWRMLightsData::DoCmdLightBlinkL"));
	return err;
	}

/**
* The command switches the specified target light on for the specified 
* duration using specified intensity.
* 
* @param aSection:		light 			Defines which light should be controlled.
* 						duration		Duration of the time the light is switched on measured in milliseconds.
* 						intensity		Intensity of the light.
* 						fadein			If ETrue, lights will not turn on instantly but instead smoothly fade-in. Note: All devices will not support fade-in, 
* 										in which case device will behave in its default fashion.
*/ 
TInt CT_CHWRMLightsData::DoCmdLightOnL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CHWRMLightsData::DoCmdLightOnL"));
	TBool	dataOk = ETrue;
	
	// Required parameters

	TInt light;
	GETFROMCONFIGMANDATORY(Int, aSection, KLight, light, KLogMissingParameter, dataOk);

	// Optional parameters

	TInt duration = KHWRMInfiniteDuration;
	GETFROMCONFIGOPTIONAL(Int, aSection, KDuration, duration, KLogDefaultValueUsed);
	
	TInt intensity = KHWRMDefaultIntensity;
	GETFROMCONFIGOPTIONAL(Int, aSection, KIntensity, intensity, KLogDefaultValueUsed);
	
	TBool fadein = EFalse;
	GETFROMCONFIGOPTIONAL(Bool, aSection, KFadein, fadein, KLogDefaultValueUsed);

	// API Method Call
	TInt	err=KErrNone;
	if ( dataOk )
		{
		TRAP(err, iLight->LightOnL(light, duration, intensity, fadein));
		}

	INFO_PRINTF1(_L("*END* CT_CHWRMLightsData::DoCmdLightOnL"));
	return err;
	}

/**
 * The command switches the device light off for the specified 
 * target for the specified duration time.
 * 
 * @param aSection:		light 			Defines which light should be controlled.
 * 						duration		Duration of the time the light is switched off measured in milliseconds.
 * 						fadeout			If ETrue, lights will not turn off instantly but instead smoothly fade-out Note: All devices will not support fade-out, 
 * 										in which case device will behave in its default fashion.
 */ 
TInt CT_CHWRMLightsData::DoCmdLightOffL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CHWRMLightsData::DoCmdLightOffL"));
	TBool dataOk = ETrue;
	
	// Required parameters.

	TInt light;
	GETFROMCONFIGMANDATORY(Int, aSection, KLight, light, KLogMissingParameter, dataOk);

	// Optional parameters.

	TInt duration = KHWRMInfiniteDuration;
	GETFROMCONFIGOPTIONAL(Int, aSection, KDuration, duration, KLogDefaultValueUsed);
	
	TBool fadeout = EFalse;
	GETFROMCONFIGOPTIONAL(Bool, aSection, KFadeout, fadeout, KLogDefaultValueUsed);

	// API Method Call
	TInt err = KErrNone;
	if ( dataOk )
		{
		TRAP(err, iLight->LightOffL(light, duration, fadeout));
		}

	INFO_PRINTF1(_L("*END* CT_CHWRMLightsData::DoCmdLightOffL"));
	return err;
	}

/**
 * This method compares the current light status with the expected status.
 * Light status: ELightStatusUnknown, ELightOn, ELightOff, ELightBlink
 * 
 * @param aSection:		expectedStatus		Light status that is expected.
 * 
 * @return
 */ 
TInt CT_CHWRMLightsData::DoCmdLightStatus(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CHWRMLightData::DoCmdLightStatus"));
	TBool dataOk = ETrue;
	TInt err = KErrNone;

	// Required parameters.
	
	TInt light;
	GETFROMCONFIGMANDATORY(Int, aSection, KLight, light, KLogMissingParameter, dataOk);
	
	TInt expectedStatus = CHWRMLight::ELightStatusUnknown;
	if(!GetEnumFromConfig(aSection, KStatus, iEnumLightStatus, expectedStatus))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KStatus);
    	SetBlockResult(EFail);
		}

	if (dataOk)
		{
		TInt status = iLight->LightStatus(light);

		INFO_PRINTF2(_L("Current status is %d"), status);
		INFO_PRINTF2(_L("Expected status status is %d"), expectedStatus);
		
		if (status != expectedStatus)
			{
			err = KErrCorrupt;
			}
		}
	
	INFO_PRINTF1(_L("*END* CT_CHWRMLightData::DoCmdLightStatus"));
	return err;
	}

/**
 * Command destructor.
 */
TInt CT_CHWRMLightsData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START* CT_CHWRMLightsData::DoCmdDestructor"));
	DestroyData();
	INFO_PRINTF1(_L("*END* CT_CHWRMLightsData::DoCmdDestructor"));
	return KErrNone;
	}

/**
 * Destroy an instance of CHWRMLight.
 */
void CT_CHWRMLightsData::DestroyData()
	{
	if(iLight)
		{
		delete iLight;
		iLight = NULL;
		}	
	}
