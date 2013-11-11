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


#include "t_rpositionerdata.h"

/*@{*/
/** Command Literals*/
/**
 * UID of a position module. Uid for "GPS", "A-GPS" or "NB".
 * May also be KParameterValueDefault.
 */
_LIT(KModuleId, "ModuleId");

/** One of _TRequestorType enum values.*/
_LIT(KRequestorType, "RequestorType");

/** One of _TRequestorFormat enum values.*/
_LIT(KRequestorFormat, "RequestorFormat");

/** String data that depends on the requestor type.*/
_LIT(KRequestorData, "RequestorData");

/** KParameterValueAny or a latitude value in degrees.*/
_LIT(KLatitude, "Latitude");

/** KParameterValueAny or a longitude value in degrees*/
_LIT(KLongitude, "Longitude");

/**
 * Maximum accepted error for queried location in meters.
 * Should be bigger than 0.0.
 */
_LIT(KMaximumError, "MaximumError");

/** Maximum accepted accuracy of position in meters.*/
_LIT(KPositionAccuracy, "PositionAccuracy");

/** Altitude in meters.*/
_LIT(KAltitude, "Altitude");

/**
 * Maximum accepted error for queried altitude in meters.
 * Should be bigger than 0.0.
 */
_LIT(KMaximumAltitudeError, "MaximumAltitudeError");

/** Minimum accepted accuracy of the altitude in meters.*/
_LIT(KAltitudeAccuracyLimit, "AltitudeAccuracyLimit");

/**
 * Speed in m/s. Optional parameter. Speed is not checked if the parameter is
 * not available. Give TReal32 values to KSpeed and KMaxSpeedError to check that
 * the speed is in the specified range (KSpeed ± KMaxSpeedError).
 * Give KParameterValueAny to only check that the speed is a positive value.
 */
_LIT(KSpeed, "Speed");

/** Specifies the nunber of interations to get the GPS position.*/
_LIT(KIterations, "Iterations");

/**
 * Maximum allowed error in speed (compared to the reference value KSpeed) in
 * m/s, a TReal32 value, for example "1.0". Should be given always when KSpeed
 * is given.
 */
_LIT(KMaxSpeedError, "MaxSpeedError");

/**
 * A parameter value that means that the parameter is not checked against
 * any reference value.
 */
_LIT(KParameterValueAny, "Any");

/** A parameter value that means that the system default should be used.*/
_LIT(KParameterValueDefault, "Default");

/** A parameter value that means that no value should be used.*/
_LIT(KParameterValueNone, "None");

/** Update interval for position updates in micro seconds.*/
_LIT(KPositionUpdateInterval, "UpdateInterval");

/** Default value is used if KUpdateInterval is not given. 3 seconds.*/
const TInt KDefaultUpdateInterval(3000000);

/** Update time-out for position updates in micro seconds.*/
_LIT(KPositionUpdateTimeout, "UpdateTimeout");

/** Default value is used if KUpdateTimeout is not given. 10 s.*/
const TInt KDefaultUpdateTimeout(10000000);

/** Maximum age for position data in micro seconds.*/
_LIT(KPositionMaxAge, "MaxAge");

/** Default value is used if KPositionMaxAge is not given.*/
const TInt KDefaultMaxAge(0);

/**
 * Accept partial update is set to update options. Boolean value ("TRUE" or
 * "FALSE"). Defaults to "EFalse" if not available. The received position data
 * is verified to be complete only if the parameter value is set.
 */
_LIT(KAcceptPartialUpdates, "AcceptPartialUpdates");

/**
 * Allowed device status. One of _TDeviceStatus values.
 * May also be KParameterValueAny.
 */
_LIT(KExpectedDataQualityStatus, "ExpectedDataQualityStatus");

/** Number of successful position queries.*/
_LIT(KMinimumSuccessfulPositionQueryCount, "MinimumSuccessfullPositionQueryCount");

/** Instance name of an RPositionerServer object.*/
_LIT(KPositionServerInstance, "PositionServerInstance");
/*@}*/

/*@{*/
/** COMMAND NAMES*/
/**
 * Command name which tests setting the type of position module ID: None,
 * Default, GPS or AGPS.
 */
_LIT(KCmdUtilitySetPositionerModuleId, "SetPositionerModuleId");

/**
 * Command name which tests opening a sub-session with the position server with
 * the specified position module ID.
 */
_LIT(KCmdOpen, "Open");

/**
 * Command name which tests closing the client sub-session with the position
 * server
 */
_LIT(KCmdClose, "Close");

/** Command name which tests setting requestor for the client sub-session*/
_LIT(KCmdSetRequestor, "SetRequestor");

/** Command name which tests setting position update options*/
_LIT(KCmdSetUpdateOptions, "SetUpdateOptions");

/** Command name which tests getting the position query from position server.*/
_LIT(KCmdGetPosition, "GetPosition");
/*@}*/

//*@{*/
/**Enumeration Literals*/
_LIT(KERequestorUnknown,  "ERequestorUnknown");
_LIT(KERequestorService,  "ERequestorService");
_LIT(KERequestorContact,  "ERequestorContact");

_LIT(KEFormatUnknown,     "EFormatUnknown");
_LIT(KEFormatApplication, "EFormatApplication");
_LIT(KEFormatTelephone,   "EFormatTelephone");
_LIT(KEFormatUrl,         "EFormatUrl");
_LIT(KEFormatMail,        "EFormatMail");

_LIT(KEDataQualityUnknown,"EDataQualityUnknown");
_LIT(KEDataQualityLoss,   "EDataQualityLoss");
_LIT(KEDataQualityPartial,"EDataQualityPartial");
_LIT(KEDataQualityNormal, "EDataQualityNormal");

_LIT(KEDeviceUnknown,     "EDeviceUnknown");
_LIT(KEDeviceError,       "EDeviceError");
_LIT(KEDeviceDisabled,    "EDeviceDisabled");
_LIT(KEDeviceInactive,    "EDeviceInactive");
_LIT(KEDeviceInitialising,"EDeviceInitialising");
_LIT(KEDeviceStandBy,     "EDeviceStandBy");
_LIT(KEDeviceReady,       "EDeviceReady");
_LIT(KEDeviceActive,      "EDeviceActive");
/*@}*/

const CDataWrapperBase::TEnumEntryTable CT_RPositionerData::iEnumRequestorType[] =
	{
	{KERequestorUnknown,  CRequestorBase::ERequestorUnknown},
	{KERequestorService,  CRequestorBase::ERequestorService},
	{KERequestorContact,  CRequestorBase::ERequestorContact}
	};

const CDataWrapperBase::TEnumEntryTable CT_RPositionerData::iEnumRequestorFormat[] =
	{
	{KEFormatUnknown,     CRequestorBase::EFormatUnknown},
	{KEFormatApplication, CRequestorBase::EFormatApplication},
	{KEFormatTelephone,   CRequestorBase::EFormatTelephone},
	{KEFormatUrl,         CRequestorBase::EFormatUrl},
	{KEFormatMail,        CRequestorBase::EFormatUrl}
	};

const CDataWrapperBase::TEnumEntryTable iEnumDataQualityStatus[] =
	{
	{KEDataQualityUnknown,TPositionModuleStatus::EDataQualityUnknown},
	{KEDataQualityLoss,   TPositionModuleStatus::EDataQualityLoss},
	{KEDataQualityPartial,TPositionModuleStatus::EDataQualityPartial},
	{KEDataQualityNormal, TPositionModuleStatus::EDataQualityNormal},
	};

const CDataWrapperBase::TEnumEntryTable iEnumDeviceStatus[] =
	{
	{KEDeviceUnknown,     TPositionModuleStatus::EDeviceUnknown},
	{KEDeviceError,       TPositionModuleStatus::EDeviceError},
	{KEDeviceDisabled,    TPositionModuleStatus::EDeviceDisabled},
	{KEDeviceInactive,    TPositionModuleStatus::EDeviceInactive},
	{KEDeviceInitialising,TPositionModuleStatus::EDeviceInitialising},
	{KEDeviceStandBy,     TPositionModuleStatus::EDeviceStandBy},
	{KEDeviceReady,       TPositionModuleStatus::EDeviceReady},
	{KEDeviceActive,      TPositionModuleStatus::EDeviceActive},
	};

CT_RPositionerData* CT_RPositionerData::NewL()
	{
	CT_RPositionerData* ret = new (ELeave) CT_RPositionerData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_RPositionerData::CT_RPositionerData()
	:
	iCons(NULL),
	iPositioner(),
	iPositionerModuleId(KPositionNullModuleId),
	iPreviousPosition(),
	iMaxAgeVerifyTime(),
	iPositionerUseModuleId(EFalse),
	iSuccessfulPositionQueryCount(0),	
	iMaxAge(KDefaultMaxAge)
	{
	iCons = GetConsole();
	}

void CT_RPositionerData::ConstructL()
	{
	}

CT_RPositionerData::~CT_RPositionerData()
	{
	delete iCons;
	iCons = NULL;
	}

/** Return pointer to the object that the data wraps*/
TAny* CT_RPositionerData::GetObject()
	{
	return iCons;
	}

/**
 * Process a command read from the ini file
 *
 * @param aCommand	The command to process
 * @param aSection	The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 * @return ETrue if the command is processed
 * @leave System wide error
 */
TBool CT_RPositionerData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool ret = ETrue;

	if (aCommand == KCmdOpen)
		{
		DoCmdOpenL(aSection);
		}
	else if (aCommand == KCmdClose)
		{
		DoCmdClose();
		}
	else if (aCommand == KCmdSetRequestor)
		{
		DoCmdSetRequestor(aSection);
		}
	else if (aCommand == KCmdSetUpdateOptions)
		{
		DoCmdSetUpdateOptions(aSection);
		}
	else if (aCommand == KCmdGetPosition)
		{
		DoCmdGetPositionL(aSection);
		}
	else if (aCommand == KCmdUtilitySetPositionerModuleId)
		{
		DoCmdUtilitySetPositionerModuleIdL(aSection);
		}	
	else
		{
		ERR_PRINTF1(_L("Unknown command."));
		ret = EFalse;
		}
	return ret;
	}

/**
 * Tests position query from position server.
 * @param aSection	The section in the ini containing data for the command
 */
void CT_RPositionerData::DoCmdGetPositionL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::DoCmdGetPositionL"));
	TBool dataOk=ETrue;

	TPtrC positionServerInstanceName;	
	if (!GetStringFromConfig(aSection, KPositionServerInstance, positionServerInstanceName))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPositionServerInstance);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if (dataOk)
		{
		RPositionServer* positionServer = static_cast<RPositionServer*>(GetDataObjectL(positionServerInstanceName));		
		if(!positionServer)
			{
			ERR_PRINTF1(_L("Could not get reference to Position Server."));
			SetError(KErrNotFound);
			}
		}
	
	// VerifyPositionIfNeeded parameters
	TPtrC maximumErrorPtr(KNullDesC);
	if (!GetStringFromConfig(aSection, KMaximumError, maximumErrorPtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMaximumError);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}
	TReal maximumError;
	if(dataOk)
		{
		if(maximumErrorPtr.Compare(KParameterValueAny)!=0)
			{
			if (!GetRealFromConfig(aSection, KMaximumError, maximumError))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMaximumError);
				SetBlockResult(EFail);
				dataOk=EFalse;
				}
			}
		}

	TPtrC expectedLatitudePtr(KNullDesC);
	if (!GetStringFromConfig(aSection, KLatitude, expectedLatitudePtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KLatitude);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}
	TReal latitude;
	if(dataOk)
		{
		if (expectedLatitudePtr.Compare(KParameterValueAny)!=0)
			{
			if (!GetRealFromConfig(aSection, KLatitude, latitude))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KLatitude);
				SetBlockResult(EFail);
				dataOk=EFalse;
				}
			}
		}

	TPtrC expectedLongitudePtr(KNullDesC);
	if (!GetStringFromConfig(aSection, KLongitude, expectedLongitudePtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KLongitude);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}
	TReal longitude;
	if(dataOk)
		{
		if (expectedLongitudePtr.Compare(KParameterValueAny)!=0)
			{
			if (!GetRealFromConfig(aSection, KLongitude, longitude))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KLongitude);
				SetBlockResult(EFail);
				dataOk=EFalse;
				}
			}
		}

	TPtrC expectedAltitudePtr(KNullDesC);
	if (!GetStringFromConfig(aSection, KAltitude, expectedAltitudePtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAltitude);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}
	TReal altitude;
	if(dataOk)
		{
		if (expectedAltitudePtr.Compare(KParameterValueAny)!=0)
			{
			if (!GetRealFromConfig(aSection, KAltitude, altitude))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAltitude);
				SetBlockResult(EFail);
				dataOk=EFalse;
				}
			}
		}

	TPtrC expectedPositionAccuracyPtr(KNullDesC);
	if (!GetStringFromConfig(aSection, KPositionAccuracy, expectedPositionAccuracyPtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPositionAccuracy);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}
	TReal expectedPositionAccuracy;
	if(dataOk)
		{
		if(expectedPositionAccuracyPtr.Compare(KParameterValueAny)!=0)
			{
			if(!GetRealFromConfig(aSection,KPositionAccuracy,expectedPositionAccuracy))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPositionAccuracy);
				SetBlockResult(EFail);
				dataOk=EFalse;
				}
			}
		}

	// VerifyAltitudeIfNeeded parameters
	TPtrC expectedMaximumAltitudeErrorPtr(KNullDesC);
	if (!GetStringFromConfig(aSection, KMaximumAltitudeError, expectedMaximumAltitudeErrorPtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMaximumAltitudeError);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}
	TReal expectedMaximumAltitudeError;
	if(dataOk)
		{
		if (expectedMaximumAltitudeErrorPtr.Compare(KParameterValueAny)!=0)
			{
			if (!GetRealFromConfig(aSection, KMaximumAltitudeError, expectedMaximumAltitudeError))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMaximumAltitudeError);
				SetBlockResult(EFail);
				dataOk=EFalse;
				}
			}
		}

	TPtrC expectedAltitudeAccuracyLimitPtr(KNullDesC);
	if (!GetStringFromConfig(aSection, KAltitudeAccuracyLimit, expectedAltitudeAccuracyLimitPtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAltitudeAccuracyLimit);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}
	TReal altitudeAccuracyLimit;
	if(dataOk)
		{
		if (expectedAltitudeAccuracyLimitPtr.Compare(KParameterValueAny)!=0)
			{
			if (!GetRealFromConfig(aSection, KAltitudeAccuracyLimit, altitudeAccuracyLimit))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAltitudeAccuracyLimit);
				SetBlockResult(EFail);
				dataOk=EFalse;
				}
			}
		}

	// VerifySpeedIfNeeded parameters
	TPtrC expectedSpeedPtr(KNullDesC);
	if (!GetStringFromConfig(aSection, KSpeed, expectedSpeedPtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KSpeed);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}
	TReal maxSpeedError;
	TReal expectedSpeed;
	TBool checkSpeed(EFalse);
	if(dataOk)
		{
		if (expectedSpeedPtr.Compare(KParameterValueAny )!=0)
			{
			if (!GetRealFromConfig(aSection,KMaxSpeedError,maxSpeedError))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMaxSpeedError);
				SetBlockResult(EFail);
				dataOk=EFalse;
				}
			if (!GetRealFromConfig(aSection,KSpeed,expectedSpeed))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KSpeed);
				SetBlockResult(EFail);
				dataOk=EFalse;
				}
			checkSpeed = ETrue;
			}
		}

	// VerifyDataQualityIfNeededL parameters
	TPtrC expectedDataQualityStatusPtr(KNullDesC);
	if (!GetStringFromConfig(aSection, KExpectedDataQualityStatus, expectedDataQualityStatusPtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KExpectedDataQualityStatus);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}
	TPositionModuleStatus::TDataQualityStatus expectedDataQualityStatus(TPositionModuleStatus::EDataQualityUnknown );
	if(dataOk)
		{
		if (expectedDataQualityStatusPtr.Compare(KParameterValueAny)!=0)
			{
			if (!GetIntFromConfig(aSection, KExpectedDataQualityStatus, expectedDataQualityStatus))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KExpectedDataQualityStatus);
				SetBlockResult(EFail);
				dataOk=EFalse;
				}
			}
		}

	// VerifySuccessfulPositionQueryCount parameters
	TInt requiredPositionQueryCount;
	if (!GetIntFromConfig(aSection, KMinimumSuccessfulPositionQueryCount, requiredPositionQueryCount))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMinimumSuccessfulPositionQueryCount);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}

	// Starts position query
	TInt iterations;
	if (!GetIntFromConfig(aSection, KIterations, iterations))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KIterations);
		SetBlockResult(EFail);
		dataOk=EFalse;
		}
	if(dataOk)
		{
		INFO_PRINTF2(_L("Requested iterations: %d"), iterations);

		TPositionSatelliteInfo posInfo;
		TRequestStatus status;

		for (TInt i=0; i<iterations; i++)
			{
			INFO_PRINTF2(_L("ITERATION: %d"), i+1);

			INFO_PRINTF1(_L("NotifyPositionUpdate with TPositionSatelliteInfo"));
			iPositioner.NotifyPositionUpdate(posInfo, status);
			User::WaitForRequest(status);
			INFO_PRINTF2(_L("Request status: %d"), status.Int());

			iMaxAgeVerifyTime.UniversalTime();

			INFO_PRINTF2(_L("Number of satellites: %d"), posInfo.NumSatellitesInView());

			// Check the position data against scripted values
			if (status.Int()==KErrNone)
				{
				TPosition position;
				posInfo.GetPosition(position);
				INFO_PRINTF2(_L("Latitude:  %g"), position.Latitude());
				INFO_PRINTF2(_L("Longitude: %g"), position.Longitude());
				INFO_PRINTF2(_L("Altitude:  %g"), position.Altitude());

				if(maximumErrorPtr.Compare(KParameterValueAny)!=0)
					{
					TCoordinate expectedCoordinate(position.Latitude(), position.Longitude(), position.Altitude());

					if(expectedLatitudePtr.Compare(KParameterValueAny)!=0)
						{
						INFO_PRINTF1(_L("Latitude will be checked in distance calculation"));
						expectedCoordinate.SetCoordinate(latitude ,expectedCoordinate.Longitude());
						}
					if(expectedLongitudePtr.Compare(KParameterValueAny)!=0)
						{
						INFO_PRINTF1(_L("Longitude will be checked in distance calculation"));
						expectedCoordinate.SetCoordinate(expectedCoordinate.Latitude(),longitude);
						}
					if(expectedAltitudePtr.Compare(KParameterValueAny)!=0)
						{
						INFO_PRINTF1(_L("Altitude will be checked in distance calculation"));
						expectedCoordinate.SetCoordinate(expectedCoordinate.Latitude(), expectedCoordinate.Longitude(), altitude);
						}

					if (!VerifyPositionIfNeeded(position, expectedCoordinate, maximumError))
						{
						SetBlockResult(EFail);
						}
					}
				if(expectedPositionAccuracyPtr.Compare(KParameterValueAny)!=0 )
					{
					if (!VerifyHorizontalAccuracyIfNeeded(position, expectedPositionAccuracy))
						{
						SetBlockResult(EFail);
						}
					}

				if (expectedMaximumAltitudeErrorPtr.Compare(KParameterValueAny)!=0)
					{
					if (!VerifyAltitudeIfNeeded(position, altitude, expectedMaximumAltitudeError))
						{
						SetBlockResult(EFail);
						}
					}

				if (expectedAltitudeAccuracyLimitPtr.Compare(KParameterValueAny)!=0)
					{
					if (!VerifyAltitudeAccuracyIfNeeded(position, altitudeAccuracyLimit))
						{
						SetBlockResult(EFail);
						}
					}

				if (!VerifySpeedIfNeeded(position, checkSpeed, maxSpeedError, expectedSpeed ) )
					{
					SetBlockResult(EFail);
					}

				if (expectedDataQualityStatusPtr.Compare(KParameterValueAny)!=0)
					{
					if (!VerifyDataQualityIfNeededL(posInfo, expectedDataQualityStatus, positionServerInstanceName))
						{
						SetBlockResult(EFail);
						}
					}

				if (!VerifyMaxAgeIfNeeded(position))
					{
					SetBlockResult(EFail);
					}

				iPreviousPosition = position;
				INFO_PRINTF1(_L("Successful position query, increasing count"));
				iSuccessfulPositionQueryCount++;
				}
			}

		if (!VerifySuccessfulPositionQueryCount(requiredPositionQueryCount))
			{
			SetBlockResult(EFail);
			}
		}
	INFO_PRINTF1(_L("*END*CT_RPositionerData::DoCmdGetPositionL"));
	}

/**
 * Tests closing the client sub-session with the position server.
 */
void CT_RPositionerData::DoCmdClose()
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::DoCmdClose"));
	iPositioner.Close();
	INFO_PRINTF1(_L("Sub-session with the positioning server closed."));
	INFO_PRINTF1(_L("*END*CT_RPositionerData::DoCmdClose"));
	}

/**
 * Tests opening a sub-session with the position server using the specified 
 * position module ID.
 */
void CT_RPositionerData::DoCmdOpenL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::DoCmdOpenL"));
	TBool dataOk = ETrue;
	TPtrC positionServerInstanceName;	
	if (!GetStringFromConfig(aSection, KPositionServerInstance, positionServerInstanceName))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPositionServerInstance);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if (dataOk)
		{
		RPositionServer* positionServer = static_cast<RPositionServer*>(GetDataObjectL(positionServerInstanceName));		
		if(positionServer)
			{
			if (iPositionerUseModuleId)
				{
				INFO_PRINTF1(_L("Opening sub-session with position server using module ID."));
				TInt error = iPositioner.Open(*positionServer, iPositionerModuleId);				
				if (error!=KErrNone)
					{
					ERR_PRINTF2(_L("Could not open sub-session with position server. Error: %d"), error);
					SetError(error);
					}
				}
			else
				{
				INFO_PRINTF1(_L("Opening sub-session with position server without module ID."));
				TInt error = iPositioner.Open(*positionServer);
				if (error!=KErrNone)
					{
					ERR_PRINTF2(_L("Could not open sub-session with position server. Error: %d"), error);
					SetError(error);
					}
				}			
			}
		else
			{
			ERR_PRINTF1(_L("Could not get reference to Position Server."));
			SetError(KErrNotFound);
			}
		}
	INFO_PRINTF1(_L("*END*CT_RPositionerData::DoCmdOpenL"));
	}

/**
 * Tests setting the requestor for the client sub-session.
 * @param aSection	The section in the INI file containing data for the command.
 */
void CT_RPositionerData::DoCmdSetRequestor(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::DoCmdSetRequestor"));
	TBool dataOk = ETrue;
	TInt requestorType;
	if (!GetEnumFromConfig(aSection, KRequestorType, iEnumRequestorType, requestorType))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRequestorType);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt requestorFormat;
	if (!GetEnumFromConfig(aSection, KRequestorFormat, iEnumRequestorFormat, requestorFormat))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRequestorFormat);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TPtrC16 requestorData;
	if (!GetStringFromConfig(aSection, KRequestorData, requestorData))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRequestorData);		
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		CRequestor::TRequestorType type(requestorType);
		CRequestor::TRequestorFormat format(requestorFormat);
		TInt error = iPositioner.SetRequestor(type, format, requestorData);
		if (error!=KErrNone)
			{
			ERR_PRINTF2(_L("Could not set requestor. Error: %d"), error);
			SetError(error);
			}
		}
	INFO_PRINTF1(_L("*END*CT_RPositionerData::DoCmdSetRequestor"));
	}

/**
 * Tests setting position update options.
 * @param aSection	The section in the INI file containing data for the command.
 */
void CT_RPositionerData::DoCmdSetUpdateOptions(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::DoCmdSetUpdateOptions"));
	TBool dataOk = ETrue;
	TInt updateInterval(KDefaultUpdateInterval);
	if (!GetIntFromConfig(aSection,KPositionUpdateInterval, updateInterval))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPositionUpdateInterval);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt updateTimeout(KDefaultUpdateTimeout);
	if (!GetIntFromConfig(aSection, KPositionUpdateTimeout, updateTimeout))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPositionUpdateTimeout);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	iMaxAge = KDefaultMaxAge;
	if (!GetIntFromConfig(aSection, KPositionMaxAge, iMaxAge))
		{
		ERR_PRINTF1(_L("Position Max Age was not found in INI file, using default value"));
		}

	if(dataOk)
		{
		INFO_PRINTF2(_L("Update interval: %d ms"), updateInterval);
		INFO_PRINTF2(_L("Update time-out: %d ms"), updateTimeout);
		INFO_PRINTF2(_L("Max age: %d ms"), iMaxAge);

		TPositionUpdateOptions updateOptions;

		INFO_PRINTF1(_L("Setting interval 1"));
		TTimeIntervalMicroSeconds updateIntervalTime(updateInterval);
		INFO_PRINTF1(_L("Setting interval 2"));
		updateOptions.SetUpdateInterval(updateIntervalTime);

		INFO_PRINTF1(_L("Setting time out 1"));
		TTimeIntervalMicroSeconds updateTimeoutTime(updateTimeout);
		INFO_PRINTF1(_L("Setting time out 2"));
		updateOptions.SetUpdateTimeOut(updateTimeoutTime);

		INFO_PRINTF1(_L("Setting max age 1"));
		TTimeIntervalMicroSeconds maxAgeTime(iMaxAge);
		INFO_PRINTF1(_L("Setting max age 2"));
		updateOptions.SetMaxUpdateAge(maxAgeTime);

		TBool acceptPartialUpdates(EFalse);
		if (!GetBoolFromConfig(aSection, KAcceptPartialUpdates, acceptPartialUpdates))
			{
			ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAcceptPartialUpdates);
			SetBlockResult(EFail);
			dataOk = EFalse;
			}

		if(dataOk)
			{
			updateOptions.SetAcceptPartialUpdates(acceptPartialUpdates);
			INFO_PRINTF1(_L("Calling iPositioner.SetUpdateOptions"));
			TInt error = iPositioner.SetUpdateOptions(updateOptions);
			if (error!=KErrNone)
				{
				ERR_PRINTF2(_L("Could not set update options. Error: %d"), error);
				SetError(error);
				}
			}
		}
	INFO_PRINTF1(_L("*END*CT_RPositionerData::DoCmdSetUpdateOptions"));
	}

/**
 * Tests setting the type of position module ID: None,Default, GPS or AGPS.
 * @param aSection	The section in the INI file containing data for the command.
 */
void CT_RPositionerData::DoCmdUtilitySetPositionerModuleIdL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*DoCmdUtilitySetPositionerModuleIdL"));
	TBool dataOk = ETrue;

	TPtrC16 moduleIdPtr;
	if (!GetStringFromConfig(aSection, KModuleId, moduleIdPtr))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KModuleId);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if (dataOk)
		{
		if (moduleIdPtr.Compare(KParameterValueDefault)==0)
			{
			TPtrC positionServerInstanceName;	
			if (!GetStringFromConfig(aSection, KPositionServerInstance, positionServerInstanceName))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPositionServerInstance);
				SetBlockResult(EFail);
				dataOk = EFalse;
				}
			if (dataOk)
				{
				INFO_PRINTF1(_L("Default Module ID will be used."));
				iPositionerUseModuleId = ETrue;
				RPositionServer* positionServer = static_cast<RPositionServer*>(GetDataObjectL(positionServerInstanceName));
				if (positionServer)
					{
					TInt error = positionServer->GetDefaultModuleId(iPositionerModuleId);
					if (error!=KErrNone)
						{
						ERR_PRINTF2(_L("Could not get default Module ID. Error: %d"), error);
						SetError(error);
						}
					}
				else
					{
					ERR_PRINTF1(_L("Could not get reference to Position Server."));
					SetError(KErrNotFound);
					}
				}
			}
		else if (moduleIdPtr.Compare(KParameterValueNone)==0)
			{
			INFO_PRINTF1(_L("No module ID will be used."));
			iPositionerUseModuleId = EFalse;
			}
		else
			{
			iPositionerUseModuleId = ETrue;
			TInt id;
			if (!GetIntFromConfig(aSection, KModuleId, id))
				{
				ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KModuleId);
				SetBlockResult(EFail);
				dataOk = EFalse;
				}
			if(dataOk)
				{
				INFO_PRINTF2(_L("The next Module ID will be used: %d"),id);
				iPositionerModuleId.iUid = id;
				}
			}
		}
	INFO_PRINTF1(_L("*END*DoCmdUtilitySetPositionerModuleIdL"));
	}

/**
 * Compares the actual speed with the expected value to determine if it is good
 * enough.
 * @param aPosition The position from the GPS device
 * @param aCheckSpeed Indicates if the speed should be checked
 * @param aMaxError Maximum accepted error for queried location in meters, for
 *  example "100.0". Should be bigger than 0.0.
 * @param aExpectedSpeed Speed in m/s, for example "0.0"
 * @return ETrue If the speed is in the defined range (KSpeed ± KMaxSpeedError)
 */
TBool CT_RPositionerData::VerifySpeedIfNeeded(const TPosition& aPosition, TBool aCheckSpeed, TReal aMaxSpeedError, TReal aExpectedSpeed)
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::VerifySpeedIfNeeded"));
	TBool check=ETrue;
	TReal32 speed(0.0);
	aPosition.Speed(iPreviousPosition, speed);

	if(aCheckSpeed )
		{
		INFO_PRINTF1(_L("Check speed value"));
		INFO_PRINTF2(_L("Max Speed Error: %f"), aMaxSpeedError );
		INFO_PRINTF2(_L("Expected speed: %f"), aExpectedSpeed);

		if (aExpectedSpeed < 0.0 || aMaxSpeedError < 0.0)
			{
			ERR_PRINTF1(_L("Illegal speed parameter values"));
			check=EFalse;
			}

		if(speed < (aExpectedSpeed - aMaxSpeedError ) || speed > (aExpectedSpeed + aMaxSpeedError ) )
			{
			ERR_PRINTF1(_L("Too big speed error") );
			check=EFalse;
			}

		INFO_PRINTF1(_L("Speed check succeeded") );
		}
	else
		{
		// Check speed is a positive value
		INFO_PRINTF1(_L("Check speed value is reasonable") );

		if(speed < 0.0 )
			{
			ERR_PRINTF1(_L("Negative speed value") );
			check=EFalse;
			}
		INFO_PRINTF1(_L("Speed check succeeded") );
		}
	INFO_PRINTF1(_L("*END*CT_RPositionerData::VerifySpeedIfNeeded"));
	return check;
	}

/**
 * Compares the actual GPS position with the expected position to determine if
 * it is good enough.
 *
 * @param aPosition The position from the GPS device
 * @param aExpectedCoordinate GPS coordinate from INI file
 * @param aMaximumError Maximum accepted error for queried location in meters,
 *  for example "100.0". Should be bigger than 0.0.
 * @return ETrue If the distance between the expected position and the actual
 * position is less than the maximum error.
 */
TBool CT_RPositionerData::VerifyPositionIfNeeded(const TPosition& aPosition, TCoordinate aExpectedCoordinate, TReal aMaximumError )
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::VerifyPositionIfNeeded"));
	INFO_PRINTF1(_L("Expected position values"));
	INFO_PRINTF2(_L("Expected latitude: %f"), aExpectedCoordinate.Latitude());
	INFO_PRINTF2(_L("Expected longitude: %f"), aExpectedCoordinate.Longitude());
	INFO_PRINTF2(_L("Expected altitude: %f"), aExpectedCoordinate.Altitude());
	INFO_PRINTF2(_L("Maximum error: %f"), aMaximumError);
	INFO_PRINTF1(_L("Calculating position error"));
	TBool check=ETrue;
	TReal32 distanceError(0.0);

	TInt err = aPosition.Distance(aExpectedCoordinate, distanceError);
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not calculate distance. Error: %d"),err);
		check=EFalse;
		}

	if (aMaximumError <= 0.0)
		{
		ERR_PRINTF1(_L("Maximum error equals or is less than zero."));
		check=EFalse;
		}

	INFO_PRINTF3(_L("Distance: %f. Comparing to the maximum allowed error: %f"), distanceError, aMaximumError);

	if (distanceError > aMaximumError)
		{
		ERR_PRINTF3(_L("Position info error too big. Error: %f, Maximum allowed error: %f"),distanceError, aMaximumError);
		check=EFalse;
		}
	INFO_PRINTF1(_L("*END*CT_RPositionerData::VerifyPositionIfNeeded"));
	return check;
	}

/**
 *  Compares the actual horizontal accuracy with the expected value to determine
 *  if is good enough.
 *
 * @param aPosition The position from the GPS device
 * @param aExpectedPositionAccuracy The expected position accurracy
 * @return ETrue If the horizontal accurracy equals or is bigger than the expected accurracy
 */
TBool CT_RPositionerData::VerifyHorizontalAccuracyIfNeeded(const TPosition& aPosition, TReal aExpectedPositionAccuracy)
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::VerifyHorizontalAccuracyIfNeeded"));
	INFO_PRINTF2(_L("Expected position accuracy limit: %f"), aExpectedPositionAccuracy);
	INFO_PRINTF1(_L("Verifying position accuracy limit") );
	TBool check=ETrue;
	if(aExpectedPositionAccuracy < aPosition.HorizontalAccuracy() )
		{
		ERR_PRINTF3(_L("Position info not accurate enough, accuracy limit: %g, actual accuracy: %g"),aExpectedPositionAccuracy, aPosition.HorizontalAccuracy());
		check=EFalse;
		}
	INFO_PRINTF1(_L("*END*CT_RPositionerData::VerifyHorizontalAccuracyIfNeeded"));
	return check;
	}

/**
 * Compares the actual age of the position data with the expected value to
 * determine if it is not too old.
 *
 * @param aPosition The position from the GPS device
 * @return ETrue if the actual age is bigger than the expected.
 */
TBool CT_RPositionerData::VerifyMaxAgeIfNeeded(const TPosition& aPosition)
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::VerifyMaxAgeIfNeeded"));
	TBool check=ETrue;
	if(iMaxAge!=0)
		{
		TTimeIntervalMicroSeconds maxAgeTime(iMaxAge);
		TTimeIntervalMicroSeconds actualAge = iMaxAgeVerifyTime.MicroSecondsFrom(aPosition.Time());
		INFO_PRINTF2(_L("Maximum Age: %Ld"),maxAgeTime.Int64());
		INFO_PRINTF2(_L("Actual Age : %Ld"),actualAge.Int64());
		if(actualAge > maxAgeTime)
			{
			ERR_PRINTF1(_L("Too old position data"));
			check=EFalse;
			}
		}
	INFO_PRINTF1(_L("*END*CT_RPositionerData::VerifyMaxAgeIfNeeded"));
	return check;
	}

/**
 * Compares the actual altitude with the expected value to determine if it is
 * good enough.
 *
 * @param aPosition The position from the GPS device
 * @param aExpectedAltitude Altitude in meters
 * @param aMaximumAltitudeError Maximum accepted error for queried altitude in meters, for example "10.0". Should be bigger than 0.0.
 * @return ETrue If the altitude error is not bigger than the maximum allowed error
 */
TBool CT_RPositionerData::VerifyAltitudeIfNeeded(const TPosition& aPosition, TReal aExpectedAltitude, TReal aMaximumAltitudeError)
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::VerifyAltitudeIfNeeded"));
	INFO_PRINTF2(_L("Expected altitude: %f"), aExpectedAltitude);
	INFO_PRINTF2(_L("Maximum altitude error: %f"), aMaximumAltitudeError);
	INFO_PRINTF1(_L("Verify altitude"));
	TBool check=ETrue;
	if (aMaximumAltitudeError <= 0.0)
		{
		ERR_PRINTF1(_L("Error: The Maximum Altitude Error equals or is less than zero."));
		check=EFalse;
		}

	TReal altitudeError = aExpectedAltitude - aPosition.Altitude();
	if (altitudeError < 0.0)
		{
		altitudeError = -altitudeError;
		}

	if (altitudeError > aMaximumAltitudeError)
		{
		ERR_PRINTF3(_L("Altitude error too big. Error: %f, Maximum allowed error: %f"), altitudeError, aMaximumAltitudeError);
		check=EFalse;
		}
	INFO_PRINTF1(_L("*END*CT_RPositionerData::VerifyAltitudeIfNeeded"));
	return check;
	}

/**
 * Compares the actual vertical accurracy with the expected value to determine
 * if it is good enough.
 *
 * @param aPosition The position from the GPS device
 * @param aAltitudeAccurracyLimit Minimum accepted accuracy of the altitude in meters, for example "16.123".
 * @return ETrue Check that the reported altitude accuracy is good enough
 */
TBool CT_RPositionerData::VerifyAltitudeAccuracyIfNeeded(const TPosition& aPosition , TReal aAltitudeAccuracyLimit)
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::VerifyAltitudeAccuracyIfNeeded"));
	INFO_PRINTF2(_L("Expected altitude accuracy limit: %f"), aAltitudeAccuracyLimit);
	TBool check=ETrue;
	if (aAltitudeAccuracyLimit < aPosition.VerticalAccuracy())
		{
		ERR_PRINTF3(_L("Position info not accurate enough. Altitude accuracy limit: %f, Actual accuracy: %f"),aAltitudeAccuracyLimit, aPosition.VerticalAccuracy());
		check=EFalse;
		}
	INFO_PRINTF1(_L("Altitude accurate enough."));
	INFO_PRINTF1(_L("*END*CT_RPositionerData::VerifyAltitudeAccuracyIfNeeded"));
	return check;
	}

/**
 * Compares the actual data quality status with the expected value to determine
 * if it is good enough.
 *
 * @param aPosition The position from the GPS device
 * @param aExpectedDataQualityStatus ExpectedDataQualityStatus is one of _TDataQualityStatus enum values:
 *  EDataQualityUnknown, EDataQualityLoss, EDataQualityPartial, EDataQualityNormal
 * @return ETrue If the data quality status is the same as the expected.
 */
TBool CT_RPositionerData::VerifyDataQualityIfNeededL(const TPositionInfo& aPosInfo, TPositionModuleStatus::TDataQualityStatus aExpectedDataQualityStatus, TPtrC aPositionServerInstanceName )
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::VerifyDataQualityIfNeededL"));
	INFO_PRINTF1(_L("Partial updates are not accepted"));
	TBool check=ETrue;
	TPositionModuleStatus posModuleStatus;
	
	RPositionServer* positionServer = static_cast<RPositionServer*>(GetDataObjectL(aPositionServerInstanceName));		
	if(positionServer)
		{
		TInt err = positionServer->GetModuleStatus(posModuleStatus, aPosInfo.ModuleId());
		if (err!=KErrNone)
			{
			ERR_PRINTF2(_L("Could not get module status. Error: %d"), err);
			check=EFalse;
			}
	
		if (posModuleStatus.DataQualityStatus() != aExpectedDataQualityStatus)
			{
			ERR_PRINTF1(_L("Data quality status does not match."));
			ERR_PRINTF3(_L("Expected data quality status: %d, Actual data quality status: %d"), aExpectedDataQualityStatus, posModuleStatus.DataQualityStatus());
			check=EFalse;
			}
		}
	INFO_PRINTF1(_L("*END*CT_RPositionerData::VerifyDataQualityIfNeededL"));
	return check;
	}

/**
 * Compates the postion query count with the expected value to determine if is
 * good enough.
 *
 * @param aRequired Number of position queries.
 * @return ETrue If the position query count is the same as the expected.
 */
TBool CT_RPositionerData::VerifySuccessfulPositionQueryCount(TInt aRequired)
	{
	INFO_PRINTF1(_L("*START*CT_RPositionerData::VerifySuccessfulPositionQueryCount"));
	INFO_PRINTF2(_L("SuccessfulPositionQueryCount = %d"), aRequired);
	TBool check=ETrue;
	if (aRequired <= 0)
		{
		ERR_PRINTF1(_L("Error: Successful position query count equals or is less than zero."));
		check=EFalse;
		}

	if (iSuccessfulPositionQueryCount < aRequired)
		{
		ERR_PRINTF3(_L("Failed, required count: %d, actual count: %d"), aRequired, iSuccessfulPositionQueryCount);
		check=EFalse;
		}

	INFO_PRINTF3(_L("Successful count, required successful position query count: %d, actual: %d"), aRequired, iSuccessfulPositionQueryCount);
	INFO_PRINTF1(_L("*END*CT_RPositionerData::VerifySuccessfulPositionQueryCount"));
	return check;
	}
