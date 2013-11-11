/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The implementation for Accessory Monitoring API virtual
*        functions.
*
*/


// INCLUDES
#include <AccMonitorInfo.h>
#include "acc_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAccMonitorInfo::CAccMonitorImpl
// 
// Default C++ constructor.
// ---------------------------------------------------------------------------
CAccMonitorInfo::CAccMonitorInfo()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorInfo::CAccMonitorInfo()" );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorInfo::CAccMonitorInfo - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitorInfo::NewL
// 
// Default C++ constructor.
// ---------------------------------------------------------------------------
EXPORT_C CAccMonitorInfo* CAccMonitorInfo::NewL()
    {
    CAccMonitorInfo* self = CAccMonitorInfo::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccMonitorInfo::NewL
// 
// Default C++ constructor.
// ---------------------------------------------------------------------------  
EXPORT_C CAccMonitorInfo* CAccMonitorInfo::NewL( const CAccMonitorInfo* aAccMonInfo )
    {
    CAccMonitorInfo* self = CAccMonitorInfo::NewLC( aAccMonInfo );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccMonitorInfo::NewLC
// 
// Default C++ constructor.
// ---------------------------------------------------------------------------
EXPORT_C CAccMonitorInfo* CAccMonitorInfo::NewLC()
    {
    CAccMonitorInfo* self = new ( ELeave ) CAccMonitorInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CAccMonitorInfo::NewLC
// 
// Default C++ constructor.
// ---------------------------------------------------------------------------  
EXPORT_C CAccMonitorInfo* CAccMonitorInfo::NewLC( const CAccMonitorInfo* aAccMonInfo )
    {
    CAccMonitorInfo* self = new ( ELeave ) CAccMonitorInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aAccMonInfo );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccMonitorInfo::ConstructL
// 
// Costructs an instance of the container.
// ---------------------------------------------------------------------------
void CAccMonitorInfo::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorInfo::ConstructL()" );
    Reset();
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorInfo::ConstructL - return" );
    }
 
// ---------------------------------------------------------------------------
// CAccMonitorInfo::ConstructL
// 
// Costructs an instance of the container.
// ---------------------------------------------------------------------------
void CAccMonitorInfo::ConstructL( const CAccMonitorInfo* aAccMonInfo )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorInfo::ConstructL()" );
    
    CopyL( aAccMonInfo );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorInfo::ConstructL - return" );
    }
    
// ---------------------------------------------------------------------------
// CAccMonitorInfo::~CAccMonitorInfo
// 
// Default C++ constructor.
// ---------------------------------------------------------------------------
CAccMonitorInfo::~CAccMonitorInfo()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorInfo::~CAccMonitorInfo()" );
    iAccCapabilityArray.Close();
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorInfo::~CAccMonitorInfo - return" );
    }
    
// -----------------------------------------------------------------------------
// CAccMonitorInfo::Exists
// Other items commented in header.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CAccMonitorInfo::Exists( RAccMonCapabilityArray& aCapabilityArray ) const 
    {
    TBool retVal( ETrue ); 
    TInt count = aCapabilityArray.Count();
    for( TInt i = 0; i != count; i++ )
        {
        if( ( iAccCapabilityArray.Find( aCapabilityArray[ i ] ) ) == KErrNotFound )
            {
            retVal = EFalse;
            }
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CAccMonitorInfo::Copy
// Other items commented in header.
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccMonitorInfo::CopyL( const CAccMonitorInfo* aAccMonInfo )
    {
    if( aAccMonInfo )
        {
        if ( this != aAccMonInfo )
            {
            Reset();
            TInt count = aAccMonInfo->Count();
            for( TInt i = 0; i != count; i++ )
                {
                iAccCapabilityArray.AppendL( aAccMonInfo->AccCapabilityAtIndex( i ) );
                }
            iAccessoryDeviceType = aAccMonInfo->iAccessoryDeviceType;
            iAccessoryPhysicalConnection = aAccMonInfo->iAccessoryPhysicalConnection;
            iDeviceAddress = aAccMonInfo->iDeviceAddress;
            iAccessoryID = aAccMonInfo->iAccessoryID;
            iReserved1 = aAccMonInfo->iReserved1;
            iReserved2 = aAccMonInfo->iReserved2;
            }
        }
    else
        {
        User::Leave( KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
// CAccMonitorInfo::CompareL
// Other items commented in header.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CAccMonitorInfo::CompareL( const CAccMonitorInfo* aAccMonInfo ) const
    {
    TBool retValue( ETrue );
    if( aAccMonInfo )
        {
        RAccMonCapabilityArray capArray;
        CleanupClosePushL( capArray );
        TInt count = aAccMonInfo->Count();
        for( TInt i = 0; i != count; i++ )
            {
            capArray.AppendL( aAccMonInfo->AccCapabilityAtIndex( i ) );
            }
            if( ( iAccessoryDeviceType != aAccMonInfo->iAccessoryDeviceType ) ||
            ( iAccessoryPhysicalConnection != aAccMonInfo->iAccessoryPhysicalConnection ) ||
            ( iDeviceAddress != aAccMonInfo->iDeviceAddress ) ||
            ( iReserved1 != aAccMonInfo->iReserved1 ) ||
            ( iReserved2 != aAccMonInfo->iReserved2 ) ||
            ( Count() != aAccMonInfo->Count() ) ||
            ( !( Exists( capArray ) ) )
            )
                {
                retValue = EFalse;
                }
        CleanupStack::PopAndDestroy( &capArray );
        }
    else
        {
        User::Leave( KErrGeneral );
        }
    
    return retValue;
    }

// -----------------------------------------------------------------------------
// CAccMonitorInfo::operator=
// Other items commented in header.
// -----------------------------------------------------------------------------
//
void CAccMonitorInfo::operator=( CAccMonitorInfo aAccMonInfo )
    {
    // Should not be used, because array can't be copied
    Reset();
    TInt count = aAccMonInfo.Count();
    for( TInt i = 0; i != count; i++ )
        {
        iAccCapabilityArray.Append( aAccMonInfo.AccCapabilityAtIndex( i ) );
        }
    iAccessoryDeviceType = aAccMonInfo.iAccessoryDeviceType;
    iAccessoryPhysicalConnection = aAccMonInfo.iAccessoryPhysicalConnection;
    iDeviceAddress = aAccMonInfo.iDeviceAddress;
    iAccessoryID = aAccMonInfo.iAccessoryID;
    iReserved1 = aAccMonInfo.iReserved1;
    iReserved2 = aAccMonInfo.iReserved2;
    }

// End of file
