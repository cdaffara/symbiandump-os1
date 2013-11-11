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
* Description:  Sensor data compensator class definition.
*
*/


#ifndef C_SENSORDATACOMPENSATOR_H
#define C_SENSORDATACOMPENSATOR_H

#include <sensrvtypes.h>
#include <sensordatacompensationtypes.h>

/**
 * Class provides operations to compensate sensor data.
 *
 * Usage:
 *
 * @code
 * #include <sensordatacompensator.h>
 * #include <sensrvaccelerometersensor.h>
 *
 * // Client creates the compensator instance as a member.
 * // This compensates the axis data received from accelerometer.
 * // The data is compensated according to both device and UI rotation.
 * // See sensrvtypes.h and sensordatacompensationtypes.h for more information.
 * iDataCompensator = CSensorDataCompensator::NewL(
 *                        KSensrvChannelTypeIdAccelerometerXYZAxisData,
 *                        ESensorCompensateDeviceAndUIOrientation );
 *
 * // Data compensation is done in client's MSensrvDataListener::DataReceived() implementation 
 * // where 'iSensorChannel' contains the accelerometer channel instance.
 * TPckgBuf<TSensrvAccelerometerAxisData> dataPckgBuf;
 * // Get data from accelerometer ( iSensorChannel->GetData( dataPckgBuf ) )
 * TInt err( iDataCompensator->Compensate( dataPckgBuf ) );
 *
 * // Client deletes the compensator instance in destructor.
 * delete iDataCompensator;
 *
 * @endcode
 *
 * @lib sensordatacompensator.lib
 * @since S60 5.1
 */

NONSHARABLE_CLASS( CSensorDataCompensator ): public CBase
    {
    public:

    /**
    * Two-phase constructor
    *
    * @since S60 5.1
    * @param aDataType Specifies the data type to be compensated.
    * @param aType Specifies the type of compensation. See TSensorCompensationType.
    * @return CSensorDataCompensator* Pointer to created object
    */
    IMPORT_C static CSensorDataCompensator* NewL( TSensrvChannelDataTypeId aDataType,
        TSensorCompensationType aType );

    public:

    /**
    * Compensates sensor data.
    *
    * @since S60 5.1
    * @param aData Reference to descriptor where sensor data locates.
    *              On return contains compensated data if compensation successful.
    *              The following packages can be used in compensating;
    *              TSensrvAccelerometerAxisData
    *              TSensrvMagnetometerAxisData
    *              TSensrvMagneticNorthData
    *              TSensrvTappingData               
    * @return TInt KErrNone - Compensation successful.
    *              KErrArgument - Input aData does not match data type.
    *              KErrNotSupported - Compensation not supported for data type.
    *              KErrNotReady - Not ready for compensation.
    *              KErrCorrupt - Configured compensation values are invalid.
    */
    virtual TInt Compensate( TDes8& aData ) = 0;

    /**
    * Used to get the compensated channel data type.
    *
    * @since S60 5.1
    * @return TSensrvChannelDataTypeId
    *              The compensated data type.
    */
    virtual TSensrvChannelDataTypeId GetChannelDataType( ) const = 0;

    /**
    * Used to get the compensation type.
    *
    * @since S60 5.1
    * @return TSensorCompensationType
    *              The used compensation type.
    */
    virtual TSensorCompensationType GetCompensationType( ) const = 0;

    };


#endif // C_SENSORDATACOMPENSATOR_H
