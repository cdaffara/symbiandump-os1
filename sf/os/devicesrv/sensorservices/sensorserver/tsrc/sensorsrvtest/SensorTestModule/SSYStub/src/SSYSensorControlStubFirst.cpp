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
#include "SSYSensorControlStubFirst.h"
#include "siftrace.h"
#include "SSYChannelXYZ.h"
#include "ssychanneldoubletap.h"
#include "sensrvsensorchannels.h"
#include "sensrvchanneldatatypes.h"

#include "SensrvInternalPSKeys.h"

#include <ssycallback.h>
#include <e32property.h>




//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubFirst::CSSYSensorControlStubFirst()
// ----------------------------------------------------------------------------------
//  
CSSYSensorControlStubFirst::CSSYSensorControlStubFirst( MSsyCallback* aSsyCallback )
    : CSSYSensorControlStubBase( aSsyCallback )
	{	
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubFirst::CSSYSensorControlStubFirst() - Enter" ) ) );
		
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubFirst::CSSYSensorControlStubFirst() - Return" ) ) );
	}

// -----------------------------------------------------------------------------
// CSSYSensorControlStubFirst::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSSYSensorControlStubFirst::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubFirst::ConstructL() - Enter" ) ) );
    
    BaseConstructL();

    // Set KSensrvCheckPluginLoaded to indicate plugin is loaded (triggered by channel open)
    TInt err = RProperty::Set( KPSUidSensrvTest, KSensrvCheckPluginLoaded, 1 ); 
    if( err )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubFirst::CreateChannelsL() - KSensrvCheckPluginLoaded set err %d" ), err ) );
        }
        
    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvLeaveFirstStubConstruction, leave );
    if( leave != KErrNone )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubFirst::ConstructL() - Leave %d" ), leave ) );
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
            
    iPsListener1 = CSsyPsListener::NewL(*this, KPSUidSensrvTest, KSensrvCreateNewChannelsFirst);
    iPsListener2 = CSsyPsListener::NewL(*this, KPSUidSensrvTest, KSensrvRemoveNewChannelsFirst);
    iPsListener3 = CSsyPsListener::NewL(*this, KPSUidSensrvTest, KSensrvRegisterZeroChannels);
    iPsListener4 = CSsyPsListener::NewL(*this, KPSUidSensrvTest, KSensrvRegisterInvalidChannels);
    
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubFirst::ConstructL() - Return" ) ) );
    }
    
//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubFirst::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CSSYSensorControlStubFirst* CSSYSensorControlStubFirst::NewL( MSsyCallback* aSsyCallback )
	{
	CSSYSensorControlStubFirst* self = new ( ELeave ) CSSYSensorControlStubFirst( aSsyCallback );
	
	CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubFirst::~CSSYSensorControlStubFirst()
// ----------------------------------------------------------------------------------
//  
// Destructor.
CSSYSensorControlStubFirst::~CSSYSensorControlStubFirst()
	{
    
	}

//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubFirst::CreateChannelsL()
// ----------------------------------------------------------------------------------
//  
void CSSYSensorControlStubFirst::CreateChannelsL()
    {
    TInt countXYZ(0);
    TInt countTap(0);
    TInt err( KErrNone );
    TPtrC8 vendorIdPtr( KChannelVendorIDFirst() );
    
    // XYZ channels
    err = RProperty::Get( KPSUidSensrvTest, KSensrvFirstStubChannelCount, countXYZ );
    if( err )
        {
        countXYZ = 3;
        }
    for( TInt i = 0; i < countXYZ; i++ )
        {
        TBuf8<KSensrvLocationLength> location;
        location.Format( KLocationFirst, i );
        CSSYChannelXYZ* channel = CSSYChannelXYZ::NewL( iSsyCallback, this, ESensrvContextTypeDevice, 
                                                    ESensrvQuantityAcceleration, KSensrvChannelTypeIdAccelerometerXYZAxisData, 
                                                    location, vendorIdPtr );
        
        iChannelArray.Append( channel );
        }

    // Doubletap channels        
    err = RProperty::Get( KPSUidSensrvTest, KSensrvFirstStubDoubleTappingChannelCount, countTap );    
    if (err)
        {
        countTap = 0;
        }
        
    for( TInt i = countXYZ; i < countTap + countXYZ; i++ )
        {
        TBuf8<KSensrvLocationLength> location;
        location.Format( KLocationFirst, i );
        CSSYChannelDoubleTap* channel = CSSYChannelDoubleTap::NewL( iSsyCallback, this, ESensrvContextTypeDevice, 
                                                    ESensrvQuantityTapping, KSensrvChannelTypeIdAccelerometerDoubleTappingData, 
                                                    location, vendorIdPtr );
        
        iChannelArray.Append( channel );
        }
        
    }

//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubFirst::NewLocString()
// ----------------------------------------------------------------------------------
//  
void CSSYSensorControlStubFirst::GetNewLocString(TDes8& aTarget, TInt aId) const
    {
    aTarget.Format( KNewLocStringBaseFirst, aId );
    }

