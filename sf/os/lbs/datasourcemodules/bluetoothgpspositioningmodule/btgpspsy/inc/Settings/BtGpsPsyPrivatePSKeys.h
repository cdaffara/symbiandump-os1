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



#ifndef BTGPSPSYINTERNALPSKEYS_H
#define BTGPSPSYINTERNALPSKEYS_H

/** @file
 * @publishedPartner
 * @released
 */


/**
* Uid of BT GPS PSY Events
*/
const TUid KPsUidBluetoothGpsPsy = { 0x101FE999 };


/**
* PSY State
*/
const TUint KBluetoothGpsPsyState = 0x01;

/**
* PSY State possible values
*/
enum TBTGPSPSYState
    {
    /** BT GPS PSY not loaded */
    EPSYNotLoaded = -1,     
    /** BT GPS PSY loaded but not connected to GPS device */
    EPSYLoaded,	     	                    
    /** BT GPS PSY loaded and connected to PNOK enabled GPS device */
    EPSYLoadedAndPNOKSupportedBTGPSUsed,   
    /** BT GPS PSY loaded and connected to non PNOK enabled GPS device*/
    EPSYLoadedAndPNOKNotSupportedBTGPSUsed,
    /** BT GPS PSY in stand by state */
    EPSYInStandBy                           
    };

/**
* BT GPS battery level, value between 0-100
*/
const TUint KBluetoothGpsPsyBatteryLevel = 0x02;

/**
* State of the battery in the BT GPS device
*/
const TUint KBluetoothGpsPsyBatteryState = 0x03;

/**
 * State of the BT GPS battery
 */
enum TBTGPSBatteryState
    {
    /** Battery state not available */
    EBatteryStateNA = -1,
    /** Battery state normal */
    EBatteryStateNormal,
    /** Battery low */
    EBatteryStateLow,
    /** Battery full */
    EBatteryStateFull
    };		


/**
* State of the external antenna of the BT GPS
*/
const TUint KBluetoothGpsPsyExtAntennaState = 0x04;

enum TBTGPSExtAntennaState
    {
    /** External antenna no applicable */
    EExtAntennaNA = -1,
    /** External antenna off */
    EExtAntennaOff,
    /** External antenna on */
    EExtAntennaOn
    };


/**
* State of the BT GPS charger
*/
const TUint KBluetoothGpsPsyExtPowerState = 0x05;

/** 
 * State of external power connection
 */
enum TBTGPSExtPowerState
    {
    /** External power no applicable */
    EExtPowerNA = -1,
    /** External power off */
    EExtPowerOff,
    /** External power on */
    EExtPowerOn
    };
	    	

/**
* BT GPS device model name
*/
const TUint KBluetoothGpsPsyVersion = 0x06;

/**
* BT hardware version of the BT GPS device
*/
const TUint KBluetoothGpsPsyBtHwVersion = 0x07;

/**
* BT software version of the BT GPS device
*/
const TUint KBluetoothGpsPsyBtSwVersion = 0x08;

/**
* GPS hardware version of the BT GPS device
*/
const TUint KBluetoothGpsPsyGpsHwVersion = 0x09;

/**
* GPS software version of the BT GPS device
*/
const TUint KBluetoothGpsPsyGpsSwVersion = 0x0a;

/**
* enum for maximun lenght of versions 
*/
enum { KMaximumVersionLength = 32 };
		
/**
* BT GPS version descriptor defination
*/
typedef TBuf16<KMaximumVersionLength> TBTGPSVersion;    		

#endif      // BTGPSPSYINTERNALPSKEYS_H

// End of file
