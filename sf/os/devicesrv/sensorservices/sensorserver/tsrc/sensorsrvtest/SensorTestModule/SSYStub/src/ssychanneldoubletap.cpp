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
* Description:  Stub imlementation
*
*/


// INCLUDE FILES
#include <SsyCallback.h>
#include <sensrvchanneldatatypes.h>
#include <e32property.h>
#include <sensrvgeneralproperties.h>
#include "ssychanneldoubletap.h"
#include "siftrace.h"
#include "SensrvInternalPSKeys.h"

//
// ----------------------------------------------------------------------------------
// CSSYChannelDoubleTap::CSSYChannelDoubleTap()
// ----------------------------------------------------------------------------------
//  
CSSYChannelDoubleTap::CSSYChannelDoubleTap( MSsyCallback* aSsyCallback, MSsyPropertyProvider* aPropertyProvider )
    : CSSYChannelBase( aSsyCallback, aPropertyProvider )
	{	
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelDoubleTap::CSSYChannelDoubleTap() - Enter" ) ) );
	
	
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelDoubleTap::CSSYChannelDoubleTap() - Return" ) ) );
	}

// -----------------------------------------------------------------------------
// CSSYChannelDoubleTap::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSSYChannelDoubleTap::ConstructL( TSensrvContextType aContextType,
	                     TSensrvQuantity aQuantity, TSensrvChannelTypeId aChannelType,
	                     TDesC8& aLocation, TDesC8& aVendorId )
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelDoubleTap::ConstructL() - Enter" ) ) );
    
    iChannelInfo.iContextType = aContextType;
    iChannelInfo.iQuantity = aQuantity;
    iChannelInfo.iChannelType = aChannelType;
    iChannelInfo.iLocation = aLocation;
    iChannelInfo.iVendorId = aVendorId;
    iChannelInfo.iDataItemSize = sizeof( TSensrvTappingData );
    iChannelInfo.iChannelDataTypeId = 5;
    
    iDataCounter = 0;
            
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelDoubleTap::ConstructL() - Return" ) ) );
    }
    
//
// ----------------------------------------------------------------------------------
// CSSYChannelDoubleTap::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CSSYChannelDoubleTap* CSSYChannelDoubleTap::NewL( MSsyCallback* aSsyCallback, MSsyPropertyProvider* aPropertyProvider, 
                                 TSensrvContextType aContextType, TSensrvQuantity aQuantity, 
                                 TSensrvChannelTypeId aChannelType, TDesC8& aLocation, TDesC8& aVendorId )
	{
	CSSYChannelDoubleTap* self = new ( ELeave ) CSSYChannelDoubleTap(  aSsyCallback, aPropertyProvider );
	
	CleanupStack::PushL( self );
    self->ConstructL( aContextType, aQuantity, aChannelType, aLocation, aVendorId );
    CleanupStack::Pop( self );
    
    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CSSYChannelDoubleTap::~CSSYChannelDoubleTap()
// ----------------------------------------------------------------------------------
//  
// Destructor.
CSSYChannelDoubleTap::~CSSYChannelDoubleTap()
	{
	}

//
// ----------------------------------------------------------------------------------
// CSSYChannelDoubleTap::AppendData()
// ----------------------------------------------------------------------------------
//  
TInt CSSYChannelDoubleTap::DataObjectSize() const
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelDoubleTap::DataObjectSize() - Return %d" ), sizeof( TSensrvTappingData ) ) );
    return sizeof( TSensrvTappingData );
    }
//
// ----------------------------------------------------------------------------------
// CSSYChannelDoubleTap::DoAppendData()
// ----------------------------------------------------------------------------------
//  
void CSSYChannelDoubleTap::DoAppendData()
    {
    TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelDoubleTap::DoAppendData() - %S" ), &location ) );
    if( iCounter == 0 )
        {
        iWritePointer = iDataBuffer;
        }
        
    TTime time;
    time.HomeTime();
    //TInt64
    
    TSensrvTappingData data;
    data.iTimeStamp = time;
    data.iDirection = iDataCounter;
    
    TUint8* dataPtr = reinterpret_cast<TUint8*>( &data );
    
    TInt size( sizeof( TSensrvTappingData ) );
    
    for( TInt i = 0; i < size; i++ )
        {
        *iWritePointer++ = *dataPtr++;
        }
    
    iDataCounter++;
    
    if (iDataCounter > 0x3F) // 0x3F means all possible direction bits set
        {
        iDataCounter = 0;
        }
    
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYChannelDoubleTap::DoAppendData() - return" ) ) );
    
    }
    
