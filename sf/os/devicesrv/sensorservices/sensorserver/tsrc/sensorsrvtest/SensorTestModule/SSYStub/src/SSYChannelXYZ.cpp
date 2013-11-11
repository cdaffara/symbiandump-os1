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
* Description:  Stub imlementation
*
*/


// INCLUDE FILES
#include <SsyCallback.h>
#include <sensrvchanneldatatypes.h>
#include <e32property.h>
#include <sensrvgeneralproperties.h>
#include "SSYChannelXYZ.h"
#include "siftrace.h"
#include "SensrvInternalPSKeys.h"

//
// ----------------------------------------------------------------------------------
// CSSYChannelXYZ::CSSYChannelXYZ()
// ----------------------------------------------------------------------------------
//  
CSSYChannelXYZ::CSSYChannelXYZ( MSsyCallback* aSsyCallback, MSsyPropertyProvider* aPropertyProvider )
    : CSSYChannelBase( aSsyCallback, aPropertyProvider )
	{	
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelXYZ::CSSYChannelXYZ() - Enter" ) ) );
	
	
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelXYZ::CSSYChannelXYZ() - Return" ) ) );
	}

// -----------------------------------------------------------------------------
// CSSYChannelXYZ::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSSYChannelXYZ::ConstructL( TSensrvContextType aContextType,
	                     TSensrvQuantity aQuantity, TSensrvChannelTypeId aChannelType,
	                     TDesC8& aLocation, TDesC8& aVendorId )
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelXYZ::ConstructL() - Enter" ) ) );
    
    iChannelInfo.iContextType = aContextType;
    iChannelInfo.iQuantity = aQuantity;
    iChannelInfo.iChannelType = aChannelType;
    iChannelInfo.iLocation = aLocation;
    iChannelInfo.iVendorId = aVendorId;
    iChannelInfo.iDataItemSize = sizeof( TSensrvAccelerometerAxisData );
    iChannelInfo.iChannelDataTypeId = TSensrvAccelerometerAxisData::KDataTypeId;
    
    iDataCounter = 0;
            
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelXYZ::ConstructL() - Return" ) ) );
    }
    
//
// ----------------------------------------------------------------------------------
// CSSYChannelXYZ::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CSSYChannelXYZ* CSSYChannelXYZ::NewL( MSsyCallback* aSsyCallback, MSsyPropertyProvider* aPropertyProvider, 
                                 TSensrvContextType aContextType, TSensrvQuantity aQuantity, 
                                 TSensrvChannelTypeId aChannelType, TDesC8& aLocation, TDesC8& aVendorId )
	{
	CSSYChannelXYZ* self = new ( ELeave ) CSSYChannelXYZ(  aSsyCallback, aPropertyProvider );
	
	CleanupStack::PushL( self );
    self->ConstructL( aContextType, aQuantity, aChannelType, aLocation, aVendorId );
    CleanupStack::Pop( self );
    
    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CSSYChannelXYZ::~CSSYChannelXYZ()
// ----------------------------------------------------------------------------------
//  
// Destructor.
CSSYChannelXYZ::~CSSYChannelXYZ()
	{
	}

//
// ----------------------------------------------------------------------------------
// CSSYChannelXYZ::AppendData()
// ----------------------------------------------------------------------------------
//  
TInt CSSYChannelXYZ::DataObjectSize() const
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelXYZ::DataObjectSize() - Return %d" ), sizeof( TSensrvAccelerometerAxisData ) ) );
    return sizeof( TSensrvAccelerometerAxisData );
    }
//
// ----------------------------------------------------------------------------------
// CSSYChannelXYZ::DoAppendData()
// ----------------------------------------------------------------------------------
//  
void CSSYChannelXYZ::DoAppendData()
    {
    TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelXYZ::DoAppendData() - %S" ), &location ) );
    if( iCounter == 0 )
        {
        iWritePointer = iDataBuffer;
        }
        
    TTime time;
    time.HomeTime();
    //TInt64
    TSensrvAccelerometerAxisData data;
    data.iTimeStamp = time;
    data.iAxisX = iDataCounter;
    data.iAxisY = 100 + iDataCounter;
    data.iAxisZ = 200 + iDataCounter;
    
    TUint8* dataPtr = reinterpret_cast<TUint8*>( &data );
    
    TInt size( sizeof( TSensrvAccelerometerAxisData ) );
    
    for( TInt i = 0; i < size; i++ )
        {
        *iWritePointer++ = *dataPtr++;
        }
    
    iDataCounter++;
    
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelXYZ::DoAppendData() - return" ) ) );
    
    }
    
