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



#ifndef POSITIONINGPLUGINFWINTERNALCRKEYS_H
#define POSITIONINGPLUGINFWINTERNALCRKEYS_H

/**
 * @file PositioningPluginFWInternalCRKeys.h
 * This header file defines central repository keys for publishing PSY static
 * capabilities and basic setup information related to the PSY. Each PSY should 
 * define these values for the specific PSY through central repository.
 * @publishedPartner
 * @released
 * 
 */


/**
 * The version of the Positioning Plug-in Information API implemented in this repository.
 * 
 * Key Type: Integer
 */	
const TUint32 KPSYInterfaceVersion    = 0x00000001;

/**
 * The positioning module priority value is specified as one of the enumerated values of TModulePriority.
 * The preferred priority may be overridden by Location Settings and when factory settings are reset.
 * Location Settings decides the priority of the positioning module if this value is set to EPriorityAuto.
 * 
 * Key Type : Integer
 */
const TUint32 KPSYPriority            = 0x00000002;

/**
 * The module status set to the positioning module when installed to the system.
 * 
 * Key Type: Integer
 * 
 * Values can be
 * - 0: Not available and	
 * - 1: Available
 */
const TUint32 KPSYStatus              = 0x00000003;

/**
 * The estimated time to obtain the initial location fix from the positioning 
 * plug-in after a connection is established. The value should not be a 
 * negative number and is mentioned in microseconds.
 * 
 * Key Type: Long Integer
 */
const TUint32 KPSYTimeToFirstFix      = 0x00000004;

/**
 * The estimated time to obtain a fix when the current position is known.
 * It value should not be a negative number and is mentioned in microseconds.
 * 
 * Key Type: Long Integer
 */
const TUint32 KPSYTimeToNextFix       = 0x00000005;

/**
 * This value specifies the estimated error for the latitude and longitude 
 * components of a position. The value is in meters.
 * 
 * Key Type: Real
 */
const TUint32 KPSYHorizontalAccuracy  = 0x00000006;

/**
 * The value specifies the error in the altitude component.
 * The value is in meters.
 * 
 * Key Type: Real
 */
const TUint32 KPSYVerticalAccuracy    = 0x00000007;

/**
 * This key indicates whether there will be a monetary charge for obtaining a fix.
 * It is specified as one of the value of the enumeration 
 * TPositionQuality::TCostIndicator.
 * 
 * Key Type: Integer
 * 
 * If a positioning module is uncertain about the cost of obtaining a fix, 
 * it should use the value ECostPossible rather than ECostUnknown.
 */
const TUint32 KPSYCostIndicator       = 0x00000008;

/**
 * This key indicates the expected internal power usage involved in obtaining a fix.
 * The power consumption is reported on a simple scale ranging from zero to High.
 * It is specified as one of the values of the enumerator TPositionQuality::TPowerConsumption.
 * 
 * Key Type: Integer
 */
const TUint32 KPSYPowerConsumption    = 0x00000009;

/**
 * Indicates the primary technologies used by this PSY to obtain position information.
 * Its value is a bitmask which is a combination of the values specified in the
 * enumerator TPositionModuleInfo::_TTechnologyType.
 * 
 * Key Type: Integer
 */
const TUint32 KPSYTechnologyType      = 0x0000000a;

/**
 * Indicates whether the positioning hardware used by the PSY is integral or external.
 * Its value is a bitmask which is a combination of the values specified in the
 * enumerator TPositionModuleInfo::_TDeviceLocation.
 * 
 * Key Type: Integer
 */
const TUint32 KPSYDeviceLocation      = 0x0000000b;

/**
 * Indicates the various different types of position information the PSY is able to return.
 * Its value is a bitmask which is a combination of the values specified in the
 * enumerator TPositionModuleInfo::_TCapabilities.
 * 
 * Key Type: Integer
 */
const TUint32 KPSYCapabilities        = 0x0000000c;

/**
 * Indicates the different position data classes supported by this PSY.
 * This is a bitmask specifying which TPositionInfoBase classes can be handled
 * by the PSY. The bitmask is a combination of the value specified in the 
 * enumerator _TPositionInfoClassType.
 * 
 * Key Type: Integer
 *
 * A PSY must always support at least TPositionInfo
 */
const TUint32 KPSYClassesSupported    = 0x0000000d;

/**
 * Indicates whether the PSY is visible to clients of the Location API.
 * 
 * Key Type: Integer
 * 
 * The values can be
 * - 0 : Invisible
 * - 1 : Visible
 *
 * A PSY is set to invisible typically if it is a proxy PSY.
 */
const TUint32 KPSYVisibility          = 0x0000000e;

/**
 * Indicates the resource file name that contains the localized name for this PSY.
 * 
 * Key Type: Integer
 *
 * If the key does not exist or the resource file does not exist then the name
 * mentioned in the ECOM implementation resource file will be used as the PSY
 * name.
 */
const TUint32 KPSYNameResource        = 0x0000000f;

/**
 * The maximum length for the localized resource file name for the key KPSYNameResource.
 */
const TInt KPSYNameResourceMaxLength = 64;

#endif      // POSITIONINGPLUGINFWINTERNALCRKEYS_H