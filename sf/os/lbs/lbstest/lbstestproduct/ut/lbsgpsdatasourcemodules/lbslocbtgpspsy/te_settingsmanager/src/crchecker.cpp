// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// CCRChecker implementation
//



/**
 @file crchecker.cpp
 @internalTechnology
*/

#include "crchecker.h"
#include "BTGPSSettingsApi.h"

CCRChecker::CCRChecker()
	{
	// No implementation required
	}

CCRChecker::~CCRChecker()
	{
	delete iSettings;
	}

CCRChecker* CCRChecker::NewLC()
	{
	CCRChecker* self = new (ELeave)CCRChecker();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCRChecker* CCRChecker::NewL()
	{
	CCRChecker* self=CCRChecker::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

void CCRChecker::ConstructL()
	{
	//Construct Central Repositary Setting
	iSettings = CRepository::NewL(KCRUidBtGpsPsy);
	}

TInt CCRChecker::CheckCommonRepositorykeys()
	{
	TInt err = KErrNone;

	// version of the Positioning Plug-in Information API implemented in this repository.	
	err = CheckRepository(KPSYInterfaceVersion);
	if (err != KErrNone){return err;}

	// The positioning module priority value 
	err = CheckRepository(KPSYPriority);
	if (err != KErrNone){return err;}

	/*
	 * The module status set to the positioning module when installed to the system.
	 * - 0: Not available and	
	 * - 1: Available
	 */
	err = CheckRepository(KPSYStatus);
	if (err != KErrNone){return err;}

	// The estimated time to obtain the initial location fix 
	err = CheckRepository(KPSYTimeToFirstFix);
	if (err != KErrNone){return err;}

	// The estimated time to obtain a fix when the current position is known.
	err = CheckRepository(KPSYTimeToNextFix);
	if (err != KErrNone){return err;}

	// This value specifies the estimated error for the latitude and longitude 
	err = CheckRepositoryRealKey(KPSYHorizontalAccuracy);
	if (err != KErrNone){return err;}

	// The value specifies the error in the altitude component.
	err = CheckRepositoryRealKey(KPSYVerticalAccuracy);
	if (err != KErrNone){return err;}

	// This key indicates whether there will be a monetary charge for obtaining a fix.
	err = CheckRepository(KPSYCostIndicator);
	if (err != KErrNone){return err;}

	// This key indicates the expected internal power usage involved in obtaining a fix.
	err = CheckRepository(KPSYPowerConsumption);
	if (err != KErrNone){return err;}

	// Indicates the primary technologies used by this PSY to obtain position information.
	err = CheckRepository(KPSYTechnologyType);
	if (err != KErrNone){return err;}

	// Indicates whether the positioning hardware used by the PSY is integral or external.
	err = CheckRepository(KPSYDeviceLocation);
	if (err != KErrNone){return err;}

	// Indicates the various different types of position information the PSY is able to return.
	err = CheckRepository(KPSYCapabilities);
	if (err != KErrNone){return err;}

	// Indicates the different position data classes supported by this PSY.
	err = CheckRepository(KPSYClassesSupported);
	if (err != KErrNone){return err;}

	/*
	 * Indicates whether the PSY is visible to clients of the Location API.
	 * The values can be
	 * - 0 : Invisible
	 * - 1 : Visible
	 */
	err = CheckRepository(KPSYVisibility);
	if (err != KErrNone){return err;}

	// Indicates the resource file name that contains the localized name for this PSY.
	err = CheckRepository(KPSYNameResource, _L("\\resource\\nlabtgpspsy_name.rsc"));
	return err;
	}

TInt CCRChecker::CheckSpecificRepositorykeys()
	{
	TInt err = KErrNone;

	// Nightmode setting key. 
	err = CheckRepository(KBluetoothGpsPsyNightModeState);
	if (err != KErrNone){return err;}

	/**
	 * Bluetooth GPS device address validity key
	 *
	 * 0 = address invalid.
	 * 
	 * 1 = address is valid and doesn't support Nokia proprietary NMEA sentences.
	 * 
	 * 2 = address is valid and support Nokia proprietary NMEA sentences.
	 */
	err = CheckRepository(KBluetoothGpsPsyDeviceAddressValidity);
	if (err != KErrNone){return err;}

	// Delay between the messages send to the GPS device in microseconds. 
	err = CheckRepository(KBluetoothGpsPsyMsgSendDelay);
	if (err != KErrNone){return err;}

	// The user equivalent range error used in GPS data error calculations in meters
	err = CheckRepositoryRealKey(KBluetoothGpsPsyUserEquivalentRangeError);
	if (err != KErrNone){return err;}

	/*
	 * The number of test NMEA sentences needed to receive during the 
	 * timeIntervalForDeviceTypeDetect of time.
	 */
	err = CheckRepository(KBluetoothGpsPsyNumTestNmeaSentences);
	if (err != KErrNone){return err;}

	// Delay between the sending of initialisation messages to Nomal GPS in microseconds
	err = CheckRepository(KBluetoothGpsPsyNormalGpsInitMsgDelay);
	if (err != KErrNone){return err;}

	/*
	 * The number of messages to repeately send to
	 * the GPS in initialisation to set it to full power mode
	 */
	err = CheckRepository(KBluetoothGpsPsyNumSetFullPowerMsg);
	if (err != KErrNone){return err;}

	// Delay for waiting PNOK compatible BT GPS response PNOK polling message
	err = CheckRepository(KBluetoothGpsPsyPnokPollingDelay);
	if (err != KErrNone){return err;}

	// Time between the reconnection attempts in microseconds
	err = CheckRepository(KBluetoothGpsPsyReconnectionTimeout);
	if (err != KErrNone){return err;}

	// The inactivity time in microseconds
	err = CheckRepository(KBluetoothGpsPsyInactiveDealy);
	if (err != KErrNone){return err;}

	/**
	 * The power saving mode usage switch
	 *
	 * 0 = off
	 *
	 * 1 = on
	 */
	err = CheckRepository(KBluetoothGpsPsyUtilisePowerSaving);
	if (err != KErrNone){return err;}

	// The satellite information(GSV sentences) life time
	err = CheckRepository(KBluetoothGpsPsySateInfoLifeTime);
	if (err != KErrNone){return err;}

	// The size of NMEA data buffer
	err = CheckRepository(KBluetoothGpsPsyNmeaBufferSize);
	return err;
	}

TInt CCRChecker::CheckRepository(TUint32 aKey)
	{
	TInt err = KErrNone;
	TInt value = 0;
	err |= iSettings->Get(aKey, value);
	if (KErrNone != err)
		{
		return err;
		}
	switch (aKey)
		{
		case KPSYInterfaceVersion:
			if (value < 0)
				{
				err = KErrCorrupt;
				}
			break;
		case KPSYPriority:
			if(value < 0)
				{
				err = KErrCorrupt;
				}
			break;

		case KPSYStatus:
			if ((value != 0)&&(value != 1))
				{
				err = KErrCorrupt;
				}
			break;

		case KPSYTimeToFirstFix:
			if (value < 0)
				{
				err = KErrCorrupt;
				}
			break;

		case KPSYTimeToNextFix:
			if (value < 0)
				{
				err = KErrCorrupt;
				}
			break;

		case KPSYCostIndicator:
			if ((value!=TPositionQuality::ECostUnknown) && (value!=TPositionQuality::ECostZero)
					&& (value!=TPositionQuality::ECostPossible) && (value!=TPositionQuality::ECostCharge))
				{
				err = KErrCorrupt;
				}
			break;

		case KPSYPowerConsumption:
			if ((value!=TPositionQuality::EPowerUnknown) && (value!=TPositionQuality::EPowerZero)
					&& (value!=TPositionQuality::EPowerLow) && (value!=TPositionQuality::EPowerMedium)
					&& (value!=TPositionQuality::EPowerHigh))
				{
				err = KErrCorrupt;
				}
			break;

		case KPSYTechnologyType:
			if ((value!=TPositionModuleInfo::ETechnologyUnknown) && (value!=TPositionModuleInfo::ETechnologyAssisted)
					&& (value!=TPositionModuleInfo::ETechnologyNetwork) && (value!=TPositionModuleInfo::ETechnologyTerminal))
				{
				err = KErrCorrupt;
				}
			break;

		case KPSYDeviceLocation:
			if ((value!=TPositionModuleInfo::EDeviceUnknown)&&(value!=TPositionModuleInfo::EDeviceExternal)
					&& (value!=TPositionModuleInfo::EDeviceInternal))
				{
				err = KErrCorrupt;
				}
			break;

		case KPSYCapabilities:
			if (value!=TPositionModuleInfo::ECapabilityNone+TPositionModuleInfo::ECapabilityHorizontal
					+TPositionModuleInfo::ECapabilityVertical+TPositionModuleInfo::ECapabilitySpeed
					+TPositionModuleInfo::ECapabilityDirection+TPositionModuleInfo::ECapabilitySatellite
					+TPositionModuleInfo::ECapabilityNmea)
				{
				err = KErrCorrupt;
				}
			break;

		case KPSYClassesSupported:
			if ((value!=EPositionModuleInfoUnknownClass) && (value!=EPositionModuleInfoClass)
					&& (value!=EPositionModuleInfoFirstCustomClass))
				{
				if(value!=15)//btpsy actual value
					{
					err = KErrCorrupt;
					}
				}
			break;

		case KPSYVisibility:
			err = CheckRepository(KBluetoothGpsPsyNightModeState);
			if ((value != TBTGPSSettingsApi::ENightModeNA) && (value != TBTGPSSettingsApi::ENightModeOff)
					&& (value != TBTGPSSettingsApi::ENightModeOn))
				{
				err = KErrCorrupt;
				}
			break;
		case KBluetoothGpsPsyDeviceAddress:
			if (value <0)
				{
				err = KErrCorrupt;
				}
			break;

		case KBluetoothGpsPsyDeviceAddressValidity:
			if ((value != TBTGPSSettingsApi::EBtDeviceInvalid) && (value != TBTGPSSettingsApi::EBtDeviceNonNokGps)
					&& (value != TBTGPSSettingsApi::EBtDeviceNokGps))
				{
				err = KErrCorrupt;
				}
			break;

		case KBluetoothGpsPsyMsgSendDelay:
			if (value <0)
				{
				err = KErrCorrupt;
				}
			break;
			
		case KBluetoothGpsPsyNumTestNmeaSentences:
			if (value <0)
				{
				err = KErrCorrupt;
				}
			break;
			
		case KBluetoothGpsPsyNormalGpsInitMsgDelay:
			if (value <0)
				{
				err = KErrCorrupt;
				}
			break;
			
		case KBluetoothGpsPsyNumSetFullPowerMsg:
			if (value <0)
				{
				err = KErrCorrupt;
				}
			break;
			
		case KBluetoothGpsPsyPnokPollingDelay:
			if (value <0)
				{
				err = KErrCorrupt;
				}
			break;
			
		case KBluetoothGpsPsyReconnectionTimeout:
			if (value <0)
				{
				err = KErrCorrupt;
				}
			break;
			
		case KBluetoothGpsPsyInactiveDealy:
			if (value <0)
				{
				err = KErrCorrupt;
				}
			break;
			
		case KBluetoothGpsPsyUtilisePowerSaving:
			if ((value != 0)&&(value != 1))
				{
				err = KErrCorrupt;
				}
			break;
			
		case KBluetoothGpsPsySateInfoLifeTime:
			if (value <0)
				{
				err = KErrCorrupt;
				}
			break;
			
		case KBluetoothGpsPsyNmeaBufferSize:
			if (value <0)
				{
				err = KErrCorrupt;
				}
			break;
		}
	return err;
	}


TInt CCRChecker::CheckRepositoryRealKey(TUint32 aKey)
	{
	TInt err = KErrNone;
	TReal value = 0;
	err |= iSettings->Get(aKey, value);
	if (KErrNone != err)
		{
		return err;
		}
	switch (aKey)
		{
		case KPSYHorizontalAccuracy:
			if (value < 0)
				{
				err = KErrCorrupt;
				}
			break;

		case KPSYVerticalAccuracy:
			if (value < 0)
				{
				err = KErrCorrupt;
				}
			break;
			
		case KBluetoothGpsPsyUserEquivalentRangeError:
			if (value <0)
				{
				err = KErrCorrupt;
				}
			break;
		}
	return err;
	}

TInt CCRChecker::CheckRepository(TUint32 aKey, const TDesC& aValue)
	{
	TInt err = KErrNone;
	TBuf<60> descValue;
	err = iSettings->Get(aKey, descValue);
	if (KErrNone == err)
		{
		if (aValue.Compare(descValue) != 0)
			{
			err = KErrCorrupt;
			}
		}
	return err;
	}
