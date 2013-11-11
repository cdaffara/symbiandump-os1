// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <lbs/lbsgpstimingmeasurement.h>

//-----------------------------------------------------------------------------
// TGpsTimingMeasurementData
//-----------------------------------------------------------------------------

/**
 * Default Constructor
 */
EXPORT_C TGpsTimingMeasurementData::TGpsTimingMeasurementData()
 : iTimingDataFlags(EGpsTimingDataTypeUndefined), iData1(0), iData2(-1), iData3(-1), iData4(-1)  
    {
    }

/**
Sets the data type to be stored.
@param aDataType data type to be stored
*/
EXPORT_C void TGpsTimingMeasurementData::SetDataType(TGpsTimingDataType aDataType)
    {
    __ASSERT_DEBUG(aDataType > EGpsTimingDataTypeUndefined && aDataType < EGpsTimingDataTypeMax, User::Invariant());

    // clear the "undefined" flag
    iTimingDataFlags &= ~EGpsTimingDataTypeUndefined;

    // set the data type flag
    if (aDataType == EGpsTimingDataTypeUtran)
        {
        iTimingDataFlags |= ETimingDataFlagsUtranTimingData;
        }
    else
        {
        iTimingDataFlags |= ETimingDataFlagsGsmTimingData;
        }
    }

/**
Returns the data type to be stored.
@see TGpsTimingMeasurementDataType
@return data type stored
*/
EXPORT_C TGpsTimingMeasurementData::TGpsTimingDataType TGpsTimingMeasurementData::DataType() const
    {
    if (iTimingDataFlags & ETimingDataFlagsUtranTimingData)
        {
        return EGpsTimingDataTypeUtran;
        }
    else if (iTimingDataFlags & ETimingDataFlagsGsmTimingData)
        {
        return EGpsTimingDataTypeGsm;
        }
    else // something has gone wrong
        {
        return EGpsTimingDataTypeUndefined;
        }
    }

// UTran Data Type Setters

/**
Sets the UTran Network Mode
Note data type must have been set to EGpsTimingDataTypeUtran
@see TGpsTimingMeasurementData::SetDataType()
@param aNetworkMode indicates FDD or TDD mode
*/
EXPORT_C void TGpsTimingMeasurementData::SetNetworkMode(TNetworkMode aNetworkMode)
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsUtranTimingData, User::Invariant());
    if (aNetworkMode == ENetworkModeFdd)
        {
        iTimingDataFlags |= ETimingDataFlagsUtranModeIsFdd;
        }
    else 
        {
        iTimingDataFlags &= ~ETimingDataFlagsUtranModeIsFdd;
        }
    }


/**
Sets the UTran GPS Timing Of Cell Ms Part data element
Note data type must have been set to EGpsTimingDataTypeUtran
Valid values are in the range 0..16383

@see TGpsTimingMeasurementData::SetDataType()
@param aGpsTocMs the GPS Timing Of Cell Ms Part to assign
*/
EXPORT_C void TGpsTimingMeasurementData::SetGPSTimingOfCellMsPart(TInt aGpsTocMs)
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsUtranTimingData, User::Invariant());
    iData2 = aGpsTocMs;
    }

/**
Sets the UTran GPS Timing Of Cell LS Part data element
Note data type must have been set to EGpsTimingDataTypeUtran
Valid values are in the range 0..4294967295

@see TGpsTimingMeasurementData::SetDataType()
@param aGpsTocLs the GPS Timing Of Cell LS Part to assign
*/
EXPORT_C void TGpsTimingMeasurementData::SetGPSTimingOfCellLsPart(TUint aGpsTocLs)
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsUtranTimingData, User::Invariant());
    iData1 = aGpsTocLs;
    }

/**
Sets the UTran Reference Identity data element
Note data type must have been set to EGpsTimingDataTypeUtran
Valid values are in the range 0..511 for FDD mode
                              0..127 for TDD mode

@see TGpsTimingMeasurementData::SetDataType()
@param aRefId the Reference Identity to assign
*/
EXPORT_C void TGpsTimingMeasurementData::SetReferenceIdentity(TInt aRefId)
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsUtranTimingData, User::Invariant());
    iData3 = aRefId;
    }

/**
Sets the UTran SFN data element
Note data type must have been set to EGpsTimingDataTypeUtran
Valid values are in the range 0..4095

@see TGpsTimingMeasurementData::SetDataType()
@param aSfn the SFN value to assign
*/
EXPORT_C void TGpsTimingMeasurementData::SetSfn(TInt aSfn)
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsUtranTimingData, User::Invariant());
    iData4 = aSfn;
    }


// UTran Data Type Accessors

/**
Returns the UTran Network Mode
Note data type must be EGpsTimingDataTypeUtran
@see TGpsTimingMeasurementData::DataType()
@return the UTran Network Mode
*/
EXPORT_C TGpsTimingMeasurementData::TNetworkMode TGpsTimingMeasurementData::NetworkMode() const
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsUtranTimingData, User::Invariant());
    if (iTimingDataFlags & ETimingDataFlagsUtranModeIsFdd)
        {
        return ENetworkModeFdd;
        }
    else 
        {
        return ENetworkModeTdd;
        }
    }

/**
Returns the GPS Timing Of Cell Ms Part data
Note data type must be EGpsTimingDataTypeUtran
Valid values are in the range 0..16383

@see TGpsTimingMeasurementData::DataType()
@return the GPS Timing Of Cell Ms Part data
        -ve value indicates that the parameter has not been set
*/
EXPORT_C TInt TGpsTimingMeasurementData::GPSTimingOfCellMsPart() const
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsUtranTimingData, User::Invariant());
    return iData2;
    }

/**
Returns the GPS Timing Of Cell LS Part data
Note data type must be EGpsTimingDataTypeUtran
Valid values are in the range 0..4294967295

@see TGpsTimingMeasurementData::DataType()
@return the GPS Timing Of Cell LS Part data
*/
EXPORT_C TUint TGpsTimingMeasurementData::GPSTimingOfCellLsPart() const
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsUtranTimingData, User::Invariant());
    return iData1;
    }

/**
Returns the Reference Identity data
Note data type must be EGpsTimingDataTypeUtran
Valid values are in the range 0..511 for FDD mode
                              0..127 for TDD mode

@see TGpsTimingMeasurementData::DataType()
@return the Reference Identity data
        -ve value indicates that the parameter has not been set
*/
EXPORT_C TInt TGpsTimingMeasurementData::ReferenceIdentity() const
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsUtranTimingData, User::Invariant());
    return iData3;
    }

/**
Returns the SFN data
Note data type must be EGpsTimingDataTypeUtran
Valid values are in the range 0..4095

@see TGpsTimingMeasurementData::DataType()
@return the SFN data
        -ve value indicates that the parameter has not been set
*/
EXPORT_C TInt  TGpsTimingMeasurementData::Sfn() const
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsUtranTimingData, User::Invariant());
    return iData4;
    }


// GSM Data Type Setters

/**
Sets the GSM Reference Frame MSB data element
Note data type must have been set to EGpsTimingDataTypeGsm
Valid values are in the range 0..63 

@see TGpsTimingMeasurementData::SetDataType()
@param aRefFrameMsb the Reference Frame MSB value to assign
*/
EXPORT_C void TGpsTimingMeasurementData::SetReferenceFrameMsb(TInt aRefFrameMsb)
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsGsmTimingData, User::Invariant());
    iData1 = (TUint)aRefFrameMsb;
    }

/**
Sets the GSM Gps Tow Subms data element
Note data type must have been set to EGpsTimingDataTypeGsm
Valid values are in the range 0..9999 (optional)

@see TGpsTimingMeasurementData::SetDataType()
@param aGpsTowSubms the Gps Tow Subms value to assign
*/
EXPORT_C void TGpsTimingMeasurementData::SetGpsTowSubms(TInt aGpsTowSubms)
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsGsmTimingData, User::Invariant());
    iData2 = aGpsTowSubms;
    }

/**
Sets the GSM Delta Tow data element
Note data type must have been set to EGpsTimingDataTypeGsm
Valid values are in the range 0..127  (optional) 

@see TGpsTimingMeasurementData::SetDataType()
@param aDeltaTow the GSM Delta Tow value to assign
*/
EXPORT_C void TGpsTimingMeasurementData::SetDeltaTow(TInt aDeltaTow)
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsGsmTimingData, User::Invariant());
    iData3 = aDeltaTow;
    }

/**
Sets the GSM Gps Reference Time Uncertainty data element
Note data type must have been set to EGpsTimingDataTypeGsm
Valid values are in the range 0..127  (optional) 

@see TGpsTimingMeasurementData::SetDataType()
@param aGpsRefTimeUncert the GSM Gps Reference Time Uncertainty value to assign
*/
EXPORT_C void TGpsTimingMeasurementData::SetGpsReferenceTimeUncertainty(TInt aGpsRefTimeUncert)
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsGsmTimingData, User::Invariant());
    iData4 = aGpsRefTimeUncert;
    }

// GSM Data Type Accessors

/**
Returns the Reference Frame MSB data
Note data type must be EGpsTimingDataTypeGsm
Valid values are in the range 0..63 

@see TGpsTimingMeasurementData::DataType()
@return the Reference Frame MSB data
*/
EXPORT_C TInt  TGpsTimingMeasurementData::ReferenceFrameMsb() const
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsGsmTimingData, User::Invariant());
    return static_cast<TInt>(iData1);
    }

/**
Returns the Gps Tow Subms data
Note data type must be EGpsTimingDataTypeGsm
Valid values are in the range 0..9999 (optional)

@see TGpsTimingMeasurementData::DataType()
@return the Gps Tow Subms data
        -ve value indicates that the optional parameter has not been set
*/
EXPORT_C TInt  TGpsTimingMeasurementData::GpsTowSubms() const
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsGsmTimingData, User::Invariant());
    return iData2;
    }

/**
Returns the GSM Gps Tow Subms data
Note data type must be EGpsTimingDataTypeGsm
Valid values are in the range 0..127  (optional) 

@see TGpsTimingMeasurementData::DataType()
@return the Gps Tow Subms data
        -ve value indicates that the optional parameter has not been set
*/
EXPORT_C TInt  TGpsTimingMeasurementData::DeltaTow() const
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsGsmTimingData, User::Invariant());
    return iData3;
    }

/**
Returns the GSM Gps Reference Time Uncertainty data
Note data type must be EGpsTimingDataTypeGsm
Valid values are in the range 0..127  (optional) 

@see TGpsTimingMeasurementData::DataType()
@return the Gps Reference Time Uncertainty data
        -ve value indicates that the optional parameter has not been set
*/
EXPORT_C TInt  TGpsTimingMeasurementData::GpsReferenceTimeUncertainty() const
    {
    __ASSERT_DEBUG(iTimingDataFlags & ETimingDataFlagsGsmTimingData, User::Invariant());
    return iData4;
    }
