// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LBSGPSTIMINGMEASUREMENT_H__
#define __LBSGPSTIMINGMEASUREMENT_H__

#include <e32std.h>
#include <e32math.h>

/**
This class is used to store timing assistance measurement data.

@publishedPartner
@prototype
 */
class TGpsTimingMeasurementData
    {
public:
    
    /**
     * Specifies the type of timing data supplied
     */
    typedef TUint TGpsTimingDataType;
    enum _TGpsTimingDataType
        {
        EGpsTimingDataTypeUndefined,
        EGpsTimingDataTypeUtran,
        EGpsTimingDataTypeGsm,
        
        EGpsTimingDataTypeMax
        };

    /**
     * Specifies the network mode of operation.
     * Currently specific to UTRAN networks.
     */
    typedef TUint TNetworkMode;
    enum _TUtranMode
        {
        ENetworkModeUndefined,
        ENetworkModeFdd,
        ENetworkModeTdd,
        
        ENetworkModeMax
        };
    
public:
    IMPORT_C TGpsTimingMeasurementData();

    // Generic Data Type Setters/Accessors
    IMPORT_C void SetDataType(TGpsTimingDataType aDataType); 
    IMPORT_C TGpsTimingDataType DataType() const; 
    
    // UTran Data Type Setters
    IMPORT_C void  SetNetworkMode(TNetworkMode aNetworkMode);
    IMPORT_C void  SetGPSTimingOfCellMsPart(TInt aGpsTocMs);
    IMPORT_C void  SetGPSTimingOfCellLsPart(TUint aGpsTocLs);
    IMPORT_C void  SetReferenceIdentity(TInt aRefId);
    IMPORT_C void  SetSfn(TInt aSfn);

    // UTran Data Type Accessors
    IMPORT_C TNetworkMode NetworkMode() const;
    IMPORT_C TInt  GPSTimingOfCellMsPart() const;
    IMPORT_C TUint GPSTimingOfCellLsPart() const;
    IMPORT_C TInt  ReferenceIdentity() const;
    IMPORT_C TInt  Sfn() const;
    
    // GSM Data Type Setters
    IMPORT_C void  SetReferenceFrameMsb(TInt aRefFrameMsb);
    IMPORT_C void  SetGpsTowSubms(TInt aGpsTowSubms);
    IMPORT_C void  SetDeltaTow(TInt aDeltaTow);
    IMPORT_C void  SetGpsReferenceTimeUncertainty(TInt aGpsRefTimeUncert);
    
    // GSM Data Type Accessors
    IMPORT_C TInt  ReferenceFrameMsb() const;
    IMPORT_C TInt  GpsTowSubms() const;
    IMPORT_C TInt  DeltaTow() const;
    IMPORT_C TInt  GpsReferenceTimeUncertainty() const;
    
private:

    /** class internal flags */
    typedef TUint TTimingDataFlags;
    enum _TTimingDataFlags
        {
        ETimingDataFlagsUtranTimingData   = 1,
        ETimingDataFlagsGsmTimingData     = 2,
        ETimingDataFlagsUtranModeIsFdd    = 4
        };
    
    /** Gps Time of Week in milli seconds */
    TTimingDataFlags iTimingDataFlags;
    
    /**  Data containers 
     * The data containers hold different data depending on the 
     * network protocol in use. See constructor definition.
     */
    TUint iData1;
    TInt  iData2;
    TInt  iData3;
    TInt  iData4;

    };


#endif // __LBSGPSTIMINGMEASUREMENT_H__
