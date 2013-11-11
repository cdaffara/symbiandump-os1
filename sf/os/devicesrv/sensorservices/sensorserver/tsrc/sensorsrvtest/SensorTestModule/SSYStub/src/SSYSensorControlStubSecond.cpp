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
* Description:  Stub imlementation
*
*/


// INCLUDE FILES
#include "SSYSensorControlStubSecond.h"
#include "siftrace.h"
#include "SSYChannelXYZ.h"
#include "sensrvsensorchannels.h"
#include "sensrvchanneldatatypes.h"

#include "SensrvInternalPSKeys.h"

#include <ssycallback.h>
#include <e32property.h>




//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubSecond::CSSYSensorControlStubSecond()
// ----------------------------------------------------------------------------------
//  
CSSYSensorControlStubSecond::CSSYSensorControlStubSecond( MSsyCallback* aSsyCallback )
    : CSSYSensorControlStubBase( aSsyCallback )
	{	
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSecond::CSSYSensorControlStubSecond() - Enter" ) ) );
		
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSecond::CSSYSensorControlStubSecond() - Return" ) ) );
	}

// -----------------------------------------------------------------------------
// CSSYSensorControlStubSecond::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSSYSensorControlStubSecond::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSecond::ConstructL() - Enter" ) ) );
    
    BaseConstructL();
    
    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvLeaveSecondStubConstruction, leave );
    if( leave != KErrNone )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSecond::ConstructL() - Leave %d" ), leave ) );
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
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSecond::ConstructL() - delay %d second" ), delay ) );
        User::After( delay * 1000000 ); // microseconds
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSecond::ConstructL() - continue construction" ) ) );
        }
    
    CreateChannelsL();  
      
    RSensrvChannelInfoList channelInfoList;
    CleanupClosePushL( channelInfoList );

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

    CleanupStack::PopAndDestroy( &channelInfoList );
    
    iPsListener1 = CSsyPsListener::NewL(*this, KPSUidSensrvTest, KSensrvCreateNewChannelsSecond);
    iPsListener2 = CSsyPsListener::NewL(*this, KPSUidSensrvTest, KSensrvRemoveNewChannelsSecond);
            
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubSecond::ConstructL() - Return" ) ) );
    }
    
//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubSecond::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CSSYSensorControlStubSecond* CSSYSensorControlStubSecond::NewL( MSsyCallback* aSsyCallback )
	{
	CSSYSensorControlStubSecond* self = new ( ELeave ) CSSYSensorControlStubSecond( aSsyCallback );
	
	CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubSecond::~CSSYSensorControlStubSecond()
// ----------------------------------------------------------------------------------
//  
// Destructor.
CSSYSensorControlStubSecond::~CSSYSensorControlStubSecond()
	{
	
	}
	

//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubSecond::CreateChannelsL()
// ----------------------------------------------------------------------------------
//  
void CSSYSensorControlStubSecond::CreateChannelsL()
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
        location.Format( KLocationSecond, i );
        TPtrC8 vendorIdPtr( KChannelVendorIDSecond() );
        CSSYChannelXYZ* channel = CSSYChannelXYZ::NewL( iSsyCallback, this, ESensrvContextTypeDevice, 
                                                    ESensrvQuantityAcceleration, KSensrvChannelTypeIdAccelerometerXYZAxisData, 
                                                    location, vendorIdPtr );
        
        iChannelArray.Append( channel );
        }
    }


//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubSecond::NewLocString()
// ----------------------------------------------------------------------------------
//  
void CSSYSensorControlStubSecond::GetNewLocString(TDes8& aTarget, TInt aId) const
    {
    aTarget.Format( KNewLocStringBaseSecond, aId );
    }

