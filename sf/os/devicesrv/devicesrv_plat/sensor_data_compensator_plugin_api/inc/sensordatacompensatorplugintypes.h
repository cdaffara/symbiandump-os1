/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor data compensation plugin type definitions.
*
*/


#ifndef SENSORDATACOMPENSATIONPLUGINTYPES_H
#define SENSORDATACOMPENSATIONPLUGINTYPES_H

#include <sensrvtypes.h>
#include <sensordatacompensationtypes.h>

class CSensorDataCompensatorPlugin;
class MSensorDataCompensatorObserver;

/**
 * Declaration of function pointer for creating CSensorDataCompensatorPlugin instance.
 */
typedef CSensorDataCompensatorPlugin* (*CreateSensorDataCompensatorFuncL)(
    TSensrvChannelDataTypeId,
    TSensorCompensationType aType,
    MSensorDataCompensatorObserver& );

/**
 * Definition of content and package for TCompensationTypeAxisData.
 * Applies to TSensrvAccelerometerAxisData and TSensrvMagnetometerAxisData.
 * If multiple compensation items are provided, the items are combined using
 * matrix multiplication in the compensator that requests the items.
 * Due to the multiplication, plugin must provide the items in priority order
 * (i.e. the most relevant compensation first).
 * 
 */
NONSHARABLE_CLASS(TCompensationTypeAxisData)
    {
    public:
    TInt iAxisX;
    TInt iAxisY;
    TInt iAxisZ;
    };
typedef TPckg<TCompensationTypeAxisData> TCompensationTypeAxisDataPckg;
typedef TPckgBuf<TCompensationTypeAxisData> TCompensationTypeAxisDataPckgBuf;

/**
 * Definition of content and package for TCompensationTypeAngleData.
 * Applies to TSensrvMagneticNorthData. If multiple compensation items are provided,
 * the items are combined by addition in the compensator that requests the items.
 */
NONSHARABLE_CLASS(TCompensationTypeAngleData)
    {
    public:
    TInt iAngle;
    };
typedef TPckg<TCompensationTypeAngleData> TCompensationTypeAngleDataPckg;
typedef TPckgBuf<TCompensationTypeAngleData> TCompensationTypeAngleDataPckgBuf;

/**
 * Definition of content and package for TCompensationTypeDirectionData.
 * Applies to TSensrvTappingData. If multiple compensation items are provided,
 * only the first item is used in the compensator that requests the items.
 */
NONSHARABLE_CLASS(TCompensationTypeDirectionData)
    {
    public:
    /** Clockwise rotation angles supported by tapping axis compensation. */
    enum TRotation
        {
        ECw0   = 0,
        ECw90  = 90,
        ECw180 = 180,
        ECw270 = 270
        };
    /** Tapping axis compensation values. Possible values are defined by TRotation. */
    TInt iTappingAxisX;
    TInt iTappingAxisY;
    TInt iTappingAxisZ;
    };
typedef TPckg<TCompensationTypeDirectionData> TCompensationTypeDirectionDataPckg;
typedef TPckgBuf<TCompensationTypeDirectionData> TCompensationTypeDirectionDataPckgBuf;

#endif  // SENSORDATACOMPENSATIONPLUGINTYPES_H
