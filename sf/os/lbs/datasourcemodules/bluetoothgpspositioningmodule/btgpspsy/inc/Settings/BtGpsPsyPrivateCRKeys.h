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



#ifndef BTGPSPSYINTERNALCRKEYS_H
#define BTGPSPSYINTERNALCRKEYS_H

/** @file
 * 
 * @publishedPartner
 * @released
 */


/**
* BT GPS PSY Setting Storage CenRep UID
*/
const TUid KCRUidBtGpsPsy = { 0x101FE999 };

/**
* Nightmode setting key. Valid settings are:
* 
* TBTGPSSettingsApi::ENightModeOff = 0
* TBTGPSSettingsApi::ENightModeOn = 1
*/
const TUint32 KBluetoothGpsPsyNightModeState = 0x10000000;

/**
* Bluetooth GPS device address key
*/
const TUint32 KBluetoothGpsPsyDeviceAddress = 0x10000007;

/**
* Bluetooth GPS device address validity key
*
* 0 = address invalid.
* 
* 1 = address is valid and doesn't support Nokia proprietary NMEA sentences.
* 
* 2 = address is valid and support Nokia proprietary NMEA sentences.
*/
const TUint32 KBluetoothGpsPsyDeviceAddressValidity = 0x10000008;

/**
* Delay between the messages send to the GPS device in microseconds. 
*/
const TUint32 KBluetoothGpsPsyMsgSendDelay = 0x11000001;

/**
* The user equivalent range error used in GPS data error calculations in meters
*/
const TUint32 KBluetoothGpsPsyUserEquivalentRangeError = 0x11000002;

/**
* The number of test NMEA sentences needed to receive during the 
* timeIntervalForDeviceTypeDetect of time.
*/
const TUint32 KBluetoothGpsPsyNumTestNmeaSentences = 0x11000003;

/**
* Delay between the sending of initialisation messages to Nomal GPS in microseconds
*/
const TUint32 KBluetoothGpsPsyNormalGpsInitMsgDelay = 0x11000004;

/**
* The number of messages to repeately send to
* the GPS in initialisation to set it to full power mode
*/
const TUint32 KBluetoothGpsPsyNumSetFullPowerMsg = 0x11000005;

/**
* Delay for waiting PNOK compatible BT GPS response PNOK polling message
*/
const TUint32 KBluetoothGpsPsyPnokPollingDelay = 0x11000006;

/**
* Time between the reconnection attempts in microseconds
*/
const TUint32 KBluetoothGpsPsyReconnectionTimeout = 0x11000007;

/**
* The inactivity time in microseconds
*/
const TUint32 KBluetoothGpsPsyInactiveDealy = 0x11000008;

/**
* The power saving mode usage switch
*
* 0 = off
*
* 1 = on
*/
const TUint32 KBluetoothGpsPsyUtilisePowerSaving = 0x11000009;

/**
* The satellite information(GSV sentences) life time
*/
const TUint32 KBluetoothGpsPsySateInfoLifeTime = 0x1100000a;

/**
* The size of NMEA data buffer
*/
const TUint32 KBluetoothGpsPsyNmeaBufferSize = 0x1100000b;

#endif      // BTGPSPSYINTERNALCRKEYS_H

// End of file