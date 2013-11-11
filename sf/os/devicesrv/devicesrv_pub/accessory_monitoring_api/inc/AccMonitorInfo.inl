/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Inline functions to access CAccMonitorInfo
*
*/


// -----------------------------------------------------------------------------
// CAccMonitorInfo::Reset
// Other items commented in header.
// -----------------------------------------------------------------------------
//
inline void CAccMonitorInfo::Reset()
    {
    iAccCapabilityArray.Reset();
    iAccessoryDeviceType = 0;
    iAccessoryPhysicalConnection = 0;
    iDeviceAddress = 0;
    iAccessoryID = 0;
    iReserved1 = 0;
    iReserved2 = 0;
    }

// -----------------------------------------------------------------------------
// CAccMonitorInfo::AccDeviceType
// Other items commented in header.
// -----------------------------------------------------------------------------
//
inline TAccMonCapability CAccMonitorInfo::AccDeviceType() const 
    {
    return iAccessoryDeviceType;
    }

// -----------------------------------------------------------------------------
// CAccMonitorInfo::AccPhysicalConnection
// Other items commented in header.
// -----------------------------------------------------------------------------
//
inline TAccMonCapability CAccMonitorInfo::AccPhysicalConnection() const 
    {
    return iAccessoryPhysicalConnection;
    }

// -----------------------------------------------------------------------------
// CAccMonitorInfo::AccDeviceAddress
// Other items commented in header.
// -----------------------------------------------------------------------------
//
inline TUint64 CAccMonitorInfo::AccDeviceAddress() const 
    {
    return iDeviceAddress;
    }

// -----------------------------------------------------------------------------
// CAccMonitorInfo::AccCapabilityAtIndex
// Other items commented in header.
// -----------------------------------------------------------------------------
//
inline TAccMonCapability CAccMonitorInfo::AccCapabilityAtIndex( TInt aIndex ) const 
    {
    return iAccCapabilityArray[ aIndex ]; 
    }

// -----------------------------------------------------------------------------
// CAccMonitorInfo::Count
// Other items commented in header.
// -----------------------------------------------------------------------------
//
inline TInt CAccMonitorInfo::Count() const 
    {
    return iAccCapabilityArray.Count();
    }

// -----------------------------------------------------------------------------
// CAccMonitorInfo::Exists
// Other items commented in header.
// -----------------------------------------------------------------------------
//
inline TBool CAccMonitorInfo::Exists( TAccMonCapability aCapability ) const 
    {
    TBool retValue( ETrue );
    if( iAccCapabilityArray.Find( aCapability ) != KErrNotFound )
        { 
        retValue = ETrue;
        }
    else
        {
        retValue = EFalse;  
        }
    return retValue;
    }

//End of file
