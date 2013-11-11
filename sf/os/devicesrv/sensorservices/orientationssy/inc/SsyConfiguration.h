/*
* Copyright (c) 2007,2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Configurations of Orientation SSY
*
*/


#ifndef __SSYCONFIGURATION_H__
#define __SSYCONFIGURATION_H__


// INCLUDES
#include <sensrvgeneralproperties.h>
#include <sensrvaccelerometersensor.h>
#include <sensrvorientationsensor.h>

// CONSTANTS
_LIT8(KNoManufacturerDefined, "Nokia"); // maxLength is 16
_LIT8(KLocationNormal, "NoLoc");                // maxLength is 16
_LIT8(KLocationCover, "Cover");                  // maxLength is 16

const TInt  KRotationResolutionInDegrees = 15;
const TInt  KMinAngle = 0;
const TInt  KMaxAngle = 359;
const TReal KRotationAccuracy = (TReal)KRotationResolutionInDegrees / (TReal)(KMaxAngle + 1);

const TInt  KNoIntegerValue = 0;
const TReal KNoRealValue = 0;

/**
 * SSsyChannelInfo.
 * Channelinfo structure.
 */
class TSSsyChannelInfo
    {
    public:
    TSensrvChannelId iChannelId;            // Channel id of the channel
    TSensrvContextType iContextType;        // Context type of the channel    
    TSensrvQuantity iQuantity;              // Quantity of the channel
    TSensrvChannelTypeId iChannelType;      // Channel type Id of the channel
    TPtrC8 iLocation;                       // Location of the sensor related to channel
    TPtrC8 iVendorId;                       // Vendor Id of the sensor related to channel
    TInt iDataItemSize;                     // Data item size delivered in the channel
    TSensrvChannelDataTypeId iDataTypeId;   // Data type Id
    };

/**
 * SSyChannelProperties.
 * Channelproperties structure.  
 */
class TSSyChannelProperties
    {
    public:
    TSensrvPropertyId iPropertyId;
    TInt iItemIndex;    

    TInt iIntValue;
    TReal iRealValue;            

    TPtrC8 iBufValue;    

    TInt iIntMaxValue;
    TReal iRealMaxValue;            

    TInt iIntMinValue;        
    TReal iRealMinValue;
   
    TBool iReadOnly;
    TSensrvPropertyType iPropertyType;    
    };

/**
 * KSSsyChannelInfo.
 * Channelinfo table.  
 */
const TSSsyChannelInfo KSSsyChannelInfo[] =
    {
#if 0
        {       
            0,                                   // iChannelId
            ESensrvContextTypeUser,                  // iContextType
            ESensrvQuantityOrientation,          // iQuantity orientation
            KSensrvChannelTypeIdOrientationData, // iChannelType
            TPtrC8(KLocationNormal),             // iLocation
            TPtrC8(KNoManufacturerDefined),      // iVendorId
            sizeof(TSensrvOrientationData),      // iDataItemSize
            TSensrvOrientationData::KDataTypeId  // iDataTypeId
        },
#endif
        {       
            0,                                   // iChannelId
            ESensrvContextTypeUser,                  // iContextType
            ESensrvQuantityRotation,             // iQuantity rotation
            KSensrvChannelTypeIdRotationData,    // iChannelType
            TPtrC8(KLocationNormal),             // iLocation
            TPtrC8(KNoManufacturerDefined),      // iVendorId
            sizeof(TSensrvRotationData),         // iDataItemSize
            TSensrvRotationData::KDataTypeId     // iDataTypeId
        },

    };

/**
 * KSSySensorProperties.
 * Channelproperties table.  
 */
const TSSyChannelProperties KSSySensorProperties[] =
    {        

        {
            KSensrvPropIdAvailability,          // iPropertyId
            -1,                                 // iItemIndex
            ETrue,                              // iValue
            KNoRealValue,
            TPtrC8(KNullDesC8),                 // iBufValue
            ETrue,                              // iMaxValue
            KNoRealValue,
            EFalse,                             // iMinValue
            KNoRealValue,
            ETrue,                              // iReadOnly
            ESensrvIntProperty                  // iPropertyType
        },

        {
            KSensrvPropIdDataRate,              // iPropertyId
            -1,                                 // iItemIndex
            0,                                  // iValue
            KNoRealValue,
            TPtrC8(KNullDesC8),                 // iBufValue
            0,                                  // iMaxValue
            KNoRealValue,
            0,                                  // iMinValue
            KNoRealValue,
            EFalse,                             // iReadOnly. Default value is not readonly, it is set
                                                // to read only when updated.
            ESensrvIntProperty                  // iPropertyType
        },

        {
            KSensrvPropIdChannelDataCompensation,              // iPropertyId
            -1,                                 // iItemIndex
            0,                                  // iValue
            KNoRealValue,
            TPtrC8(KNullDesC8),                 // iBufValue
            2,                                  // iMaxValue
            KNoRealValue,
            0,                                  // iMinValue
            KNoRealValue,
            ETrue,                             // iReadOnly. 
            ESensrvIntProperty                  // iPropertyType
        }
    };

/**
 * KSSyChannelOrientationProperties.
 * Orientation channel properties table.
 */
const TSSyChannelProperties KSSyChannelOrientationProperties[] =
    {

        {
            KSensrvPropIdMeasureRange,          // iPropertyId
            -1,                                 // iItemIndex
            TSensrvOrientationData::EOrientationUndefined,
            KNoRealValue,                       // iValue
            TPtrC8(KNullDesC8),                 // iBufValue
            TSensrvOrientationData::EOrientationDisplayDownwards,
            KNoRealValue,                   // iMaxValue
            TSensrvOrientationData::EOrientationUndefined,
            KNoRealValue,                   // iMinValue
            ETrue,                              // iReadOnly
            ESensrvIntProperty                  // iPropertyType
        },
    };

/**
 * KSSyChannelRotationProperties.
 * Rotation channel properties table.
 */
const TSSyChannelProperties KSSyChannelRotationProperties[] =
    {

        {
            KSensrvPropIdMeasureRange,          // iPropertyId
            -1,                                 // iItemIndex
            KNoIntegerValue,
            (TReal)KMaxAngle,                   // iValue
            TPtrC8(KNullDesC8),                 // iBufValue
            KNoIntegerValue,
            (TReal)KMaxAngle,                   // iMaxValue
            KNoIntegerValue,
            (TReal)KMinAngle,                   // iMinValue
            ETrue,                              // iReadOnly
            ESensrvRealProperty                 // iPropertyType
        },

        {
            KSensrvPropIdChannelAccuracy,       // iPropertyId
            -1,                                 // iItemIndex
            KNoIntegerValue,
            KRotationAccuracy,                  // iValue
            TPtrC8(KNullDesC8),                 // iBufValue
            KNoIntegerValue,
            KRotationAccuracy,                  // iMaxValue
            KNoIntegerValue,
            KRotationAccuracy,                  // iMinValue
            ETrue,                              // iReadOnly
            ESensrvRealProperty                 // iPropertyType
        },
        
    };

// MACROS

#define ARRAY_LENGTH(array) (sizeof( array ) / sizeof( array[ 0 ] ))



#endif // __SSYCONFIGURATION_H__

// End of File
