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
#include "SSYSensorControlStubSis.h"
#include "siftrace.h"
#include "SSYChannelXYZ.h"
#include "sensrvsensorchannels.h"
#include "sensrvchanneldatatypes.h"

#include "SensrvInternalPSKeys.h"

#include <ssycallback.h>
#include <e32property.h>




//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubSis::CSSYSensorControlStubSis()
// ----------------------------------------------------------------------------------
//  
CSSYSensorControlStubSis::CSSYSensorControlStubSis( MSsyCallback* aSsyCallback )
    : CSSYSensorControlStubBase( aSsyCallback )
	{	
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSis::CSSYSensorControlStubSis() - Enter" ) ) );
		
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSis::CSSYSensorControlStubSis() - Return" ) ) );
	}

// -----------------------------------------------------------------------------
// CSSYSensorControlStubSis::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSSYSensorControlStubSis::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSis::ConstructL() - Enter" ) ) );
    
    BaseConstructL();
    
    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvLeaveSecondStubConstruction, leave );
    if( leave != KErrNone )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSis::ConstructL() - Leave %d" ), leave ) );
        User::Leave( leave );
        }
        
    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvStubConstructionJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubFirst::ConstructL() - Construction JAM" ) ) );
        TRequestStatus status; 
        User::WaitForRequest( status );
        }
        
    TInt delay( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvStubConstructionDelay, delay );
    
    if( delay > 0 )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSis::ConstructL() - delay %d second" ), delay ) );
        User::After( delay * 1000000 ); // microseconds
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSis::ConstructL() - continue construction" ) ) );
        }
    
    CreateChannelsL();  
      
    RSensrvChannelInfoList channelInfoList( 5 );
    for( TInt i = 0; i < iChannelArray.Count(); i++ )
        {
        channelInfoList.Append( (*iChannelArray[i]).ChannelInfo() );
        }
    
    iSsyCallback->RegisterChannelsL( channelInfoList );
    
    // Update ChannelId
    for( TInt i = 0; i < iChannelArray.Count(); i++ )
        {
        (*iChannelArray[i]).SetChannelId( channelInfoList[i].iChannelId );
        }
        
    channelInfoList.Reset();
    
            
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSis::ConstructL() - Return" ) ) );
    }
    
//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubSis::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CSSYSensorControlStubSis* CSSYSensorControlStubSis::NewL( MSsyCallback* aSsyCallback )
	{
	CSSYSensorControlStubSis* self = new ( ELeave ) CSSYSensorControlStubSis( aSsyCallback );
	
	CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubSis::~CSSYSensorControlStubSis()
// ----------------------------------------------------------------------------------
//  
// Destructor.
CSSYSensorControlStubSis::~CSSYSensorControlStubSis()
	{
	
	}
	

//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubSis::CreateChannelsL()
// ----------------------------------------------------------------------------------
//  
void CSSYSensorControlStubSis::CreateChannelsL()
    {
    TInt count;
    TInt err( KErrNone );
    err = RProperty::Get( KPSUidSensrvTest, KSensrvSecondStubChannelCount, count );
    if( err )
        {
        count = 3;
        }
    for( TInt i = 0; i < count; i++ )
        {
        TBuf8<KSensrvLocationLength> location;
        location.Format( KLocationSis, i );
        TPtrC8 vendorIdPtr( KChannelVendorIDSis() );
        CSSYChannelXYZ* channel = CSSYChannelXYZ::NewL( iSsyCallback, this, ESensrvContextDevice, 
                                                    ESensrvQuantityAcceleration, KSensrvChannelTypeIdAccelerometerXYZAxisData, 
                                                    location, vendorIdPtr );
        
        iChannelArray.Append( channel );
        }
    }

void CSSYSensorControlStubSis::GetNewLocString(TDes8& aTarget, TInt aId) const
    {
    aTarget.Format( KNewLocStringBaseSis, aId );
    }


