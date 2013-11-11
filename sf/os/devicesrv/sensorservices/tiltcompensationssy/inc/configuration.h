/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common definitions.
*
*/


#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <sensrvmagneticnorthsensor.h>
#include <sensrvmagnetometersensor.h>
#include <sensrvtypes.h>
#include <sensrvgeneralproperties.h>

#include "common.h"

// DATA TYPES

/** Channel info structure */
struct TChannelInfo
    {
    // Channel id of the channel
    TSensrvChannelId iChannelId;
    
    // Context type of the channel
    TSensrvContextType iContextType;
    
    // Quantity of the channel
    TSensrvQuantity iQuantity;
    
    // Channel type Id of the channel
    TSensrvChannelTypeId iChannelType;
    
    // Location of the sensor related to channel
    const TText8* const iLocation;
    
    // Vendor Id of the sensor related to channel
    const TText8* const iVendorId;
    
    // Data item size delivered in the channel
    TInt iDataItemSize;
    
    // Data type Id
    TSensrvChannelDataTypeId iDataTypeId;
    };

/** Channel properties structure */
struct TChannelProperties
    {
    // Property ID
    TSensrvPropertyId iPropertyId;
    
    // Item index
    TInt iItemIndex;
    
    // Property int value
    TInt iIntValue;
    
    // Property double value
    TReal iRealValue;
    
    // Property buffer value
    const TText8* const iBufValue;
    
    // Property max int value
    TInt iIntMaxValue;
    
    // Property max double value
    TReal iRealMaxValue;
    
    // Property min int value
    TInt iIntMinValue;
    
    // Property min double value
    TReal iRealMinValue;
    
    // Is propery read only
    TBool iReadOnly;
    
    // Property type
    TSensrvPropertyType iPropertyType;
    };

// CONSTANTS

/** Dummy location */
#define NO_LOCATION _S8( "NoLoc" )

/** Dummy vendor ID */
#define VENDOR_NOKIA _S8( "Nokia" )

/** Empty string */
#define NO_BUF_VALUE _S8( "" )

/** Real value not supported */
#define NO_REAL_VALUE (TReal)0
    
/** Supported channels */
enum
    {
    EChannelTilt,
    EChannelLast
    };
    
/** Channel list */
const TChannelInfo KTiltSsyChannels[EChannelLast] =
    {
        // Tilt angle channel
        {
        0,                                   // iChannelId
        ESensrvContextTypeAmbient,               // iContextType
        ESensrvQuantityAngle,               // iQuantity
        KSensrvChannelTypeIdMagneticNorthData,        // iChannelType
        NO_LOCATION,                         // iLocation
        VENDOR_NOKIA,                           // iVendorId
        sizeof( TSensrvMagneticNorthData ),           // iDataItemSize
        TSensrvMagneticNorthData::KDataTypeId         // iDataTypeId
        }
    };
    
/** Tilt channel properties */
enum
    {
    EChannelPropertyTiltAvailability,
    EChannelPropertyTiltDataRate,
    EChannelPropertyTiltMeasureRange,
    EChannelPropertyTiltCalibrationLevel,
    EChannelPropertyTiltDataCompensation,
    EChannelPropertyTiltLast
    };
    
/** Tilt angle channel properties list */
const TChannelProperties KTiltSsyChannelProperties[EChannelPropertyTiltLast] =
    {
        // KSensrvPropIdAvailability
        {
        KSensrvPropIdAvailability,      // iPropertyId
        0,                              // iItemIndex
        ETrue,                          // iIntValue
        NO_REAL_VALUE,
        NO_BUF_VALUE,                   // iBufValue
        ETrue,                          // iIntMaxValue
        NO_REAL_VALUE,
        EFalse,                         // iIntMinValue
        NO_REAL_VALUE,
        ETrue,                          // iReadOnly
        ESensrvIntProperty              // iPropertyType
        },
        
        // KSensrvPropIdDataRate
        {
        KSensrvPropIdDataRate,          // iPropertyId
        0,                              // iItemIndex
        10,                             // iIntValue
        NO_REAL_VALUE,
        NO_BUF_VALUE,                   // iBufValue
        10,                             // iIntMaxValue
        NO_REAL_VALUE,
        1,                              // iIntMinValue
        NO_REAL_VALUE,
        EFalse,                         // iReadOnly
        ESensrvIntProperty              // iPropertyType
        },
        
        // KSensrvPropIdMeasureRange
        {
        KSensrvPropIdMeasureRange,      // iPropertyId
        0,                              // iItemIndex
        0,                              // iIntValue
        NO_REAL_VALUE,
        NO_BUF_VALUE,                   // iBufValue
        359,                            // iIntMaxValue
        NO_REAL_VALUE,
        0,                              // iIntMinValue
        NO_REAL_VALUE,
        ETrue,                          // iReadOnly
        ESensrvIntProperty              // iPropertyType
        },
        
        // KSensrvPropCalibrationLevel
        {
        KSensrvPropCalibrationLevel,    // iPropertyId
        0,                              // iItemIndex
        0,                              // iIntValue
        NO_REAL_VALUE,
        NO_BUF_VALUE,                   // iBufValue
        3,                              // iIntMaxValue
        NO_REAL_VALUE,
        0,                              // iIntMinValue
        NO_REAL_VALUE,
        ETrue,                          // iReadOnly
        ESensrvIntProperty              // iPropertyType
        },
        
        // KSensrvPropIdChannelDataCompensation
        {
        KSensrvPropIdChannelDataCompensation,    // iPropertyId
        0,                              // iItemIndex
        2,                              // iIntValue
        NO_REAL_VALUE,
        NO_BUF_VALUE,                   // iBufValue
        2,                              // iIntMaxValue
        NO_REAL_VALUE,
        0,                              // iIntMinValue
        NO_REAL_VALUE,
        ETrue,                          // iReadOnly
        ESensrvIntProperty              // iPropertyType
        }
    };

#endif // CONFIGURATION_H

// End of File
