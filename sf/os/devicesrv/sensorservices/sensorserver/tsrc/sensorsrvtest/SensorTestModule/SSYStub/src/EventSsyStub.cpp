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
#include "EventSsyStub.h"
#include "siftrace.h"
#include "EventChannel.h"

#include "SensrvInternalPSKeys.h"
#include <sensrvgeneralproperties.h>
#include "sensrvtestchanneldatatypes.h"

#include <ssycallback.h>
#include <e32property.h>

_LIT8( KLocationEvent, "Event%d" );
_LIT8( KChannelVendorIDEvent, "VendorEvent" );


//
// ----------------------------------------------------------------------------------
// CEventSsyStub::CEventSsyStub()
// ----------------------------------------------------------------------------------
//
CEventSsyStub::CEventSsyStub( MSsyCallback* aSsyCallback )
    : iSsyCallback( aSsyCallback ), iChannelArray( 5 )
	{
	COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::CEventSsyStub() - Enter" ) ) );

	COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::CEventSsyStub() - Return" ) ) );
	}


void CEventSsyStub::ConstructL()
    {
    iPeriodicForProperty = CPeriodic::NewL( CActive::EPriorityIdle );

    for( TInt i = 0; i < 2; i++ )
        {
        TBuf8<KSensrvLocationLength> location;
        location.Format( KLocationEvent, i );
        TPtrC8 vendorIdPtr( KChannelVendorIDEvent() );
        TSensrvChannelInfo channelInfo;
        channelInfo.iContextType = ESensrvContextTypeDevice;
        channelInfo.iQuantity = KSensrvTestEventChannelType;
        channelInfo.iChannelType = KSensrvTestEventChannelType;
        channelInfo.iLocation = location;
        channelInfo.iVendorId = vendorIdPtr;
        channelInfo.iDataItemSize = sizeof( TSensrvTestEventData );
        channelInfo.iChannelDataTypeId = 5;
        CEventChannel* channel = CEventChannel::NewL( iSsyCallback, this, channelInfo );

        iChannelArray.Append( channel );
        }

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

    }

//
// ----------------------------------------------------------------------------------
// CEventSsyStub::NewL()
// ----------------------------------------------------------------------------------
//
// Two-phased constructor.
CEventSsyStub* CEventSsyStub::NewL( MSsyCallback* aSsyCallback )
	{
	CEventSsyStub* self = new ( ELeave ) CEventSsyStub( aSsyCallback );

	CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
	}




//
// ----------------------------------------------------------------------------------
// CEventSsyStub::~CEventSsyStub()
// ----------------------------------------------------------------------------------
//
// Destructor.
CEventSsyStub::~CEventSsyStub()
	{
	COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::~CEventSsyStub() - Enter" ) ) );

    if( iPeriodicForProperty )
	    {
	    iPeriodicForProperty->Cancel();
	    }
	delete iPeriodicForProperty;

    iChannelArray.ResetAndDestroy();
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::~CEventSsyStub() - return" ) ) );
	}


// ---------------------------------------------------------------------------
// From class CSsySensorControlL
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CEventSsyStub::OpenChannelL( TSensrvChannelId aChannelID )
	{
	COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::OpenChannel() - Enter" ) ) );
	for( TInt i = 0; i < iChannelArray.Count(); i++ )
	    {
	    CEventChannel* channel = iChannelArray[ i ];
	    if( channel->ChannelId() == aChannelID )
	        {
	        User::LeaveIfError( channel->OpenChannel() );

	        //Start to generate property changed notifications from OpenChannel (stop it when channel is closed)
        	TInt intervalInMillisecond( 0 );
            RProperty::Get( KPSUidSensrvTest, KSensrvPropertyChanged, intervalInMillisecond );
            if( intervalInMillisecond > 0 && !iPeriodicForProperty->IsActive() )
                {

                iPeriodicForProperty->Start( TTimeIntervalMicroSeconds32( intervalInMillisecond*1000 ),
                                            TTimeIntervalMicroSeconds32( intervalInMillisecond*1000 ),
                                            TCallBack( PropertyPeriodicTick, this ) );
                }
	        break;
	        }
	    }

	COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::OpenChannel() - Return" ) ) );
	}



// ---------------------------------------------------------------------------
// From class CSsySensorControlL
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CEventSsyStub::CloseChannelL( TSensrvChannelId aChannelID )
	{
	COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::CloseChannel() - Enter" ) ) );
	for( TInt i = 0; i < iChannelArray.Count(); i++ )
	    {
	    CEventChannel* channel = iChannelArray[ i ];
	    if( channel->ChannelId() == aChannelID )
	        {
	        User::LeaveIfError( channel->CloseChannel() );
	        break;
	        }
	    }

	if( iPeriodicForProperty->IsActive() )
        {
        iPeriodicForProperty->Cancel();
        }
	COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::CloseChannel() - Return" ) ) );
	}


//
// ----------------------------------------------------------------------------------
// CEventSsyStub::CheckPropertyDependenciesL()
// ----------------------------------------------------------------------------------
//
void CEventSsyStub::CheckPropertyDependenciesL
                                  ( const TSensrvChannelId /*aChannelId*/,
                                    const TSensrvProperty& /*aProperty*/,
                                    RSensrvChannelList& aAffectedChannels  )
    {
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::CheckPropertyDependenciesL()" ) ) );

    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvCheckPropertyDependenciesLeave, leave );
    if( leave )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::CheckPropertyDependenciesL() - CheckPropertyDependenciesL Leave %d" ), leave ) );
        iPeriodicForProperty->Cancel();
        User::Leave( leave );
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvCheckPropertyDependenciesJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::CheckPropertyDependenciesL() - CheckPropertyDependenciesL JAM" ) ) );
        iPeriodicForProperty->Cancel();
        //TRequestStatus status;
        User::After( 1000000000 );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvCheckPropertyDependenciesPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::CheckPropertyDependenciesL() - CheckPropertyDependenciesL PANIC" ) ) );
        iPeriodicForProperty->Cancel();
        User::Panic( _L("TESTPANIC"), 9999 );
        }

    RSensrvChannelList affectedChannels;
    for( TInt i = 0; i < iChannelArray.Count(); i++ )
        {
        CEventChannel* channel = iChannelArray[ i ];
        aAffectedChannels.Append( channel->ChannelId() );
        }

    }

//
// ----------------------------------------------------------------------------------
// CEventSsyStub::SetPropertyL()
// ----------------------------------------------------------------------------------
//
void CEventSsyStub::SetPropertyL(  const TSensrvChannelId aChannelId,
                                    const TSensrvProperty& aProperty )
    {
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - Enter" ) ) );

    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvSetPropertyLeave, leave );
    if( leave )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - SetPropertyL Leave %d" ), leave ) );
        iPeriodicForProperty->Cancel();
        User::Leave( leave );
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvSetPropertyJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - SetPropertyL JAM" ) ) );
        iPeriodicForProperty->Cancel();
        //TRequestStatus status;
        User::After( 1000000000 );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvSetPropertyPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - SetPropertyL PANIC" ) ) );
        iPeriodicForProperty->Cancel();
        User::Panic( _L("TESTPANIC"), 9999 );
        }

    if ( aProperty.GetPropertyId() == KSensrvPropIdDataRate )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - property KSensrvPropIdDataRate" ) ) );
        TInt value( 0 );
        TInt index( aProperty.PropertyItemIndex() );
        aProperty.GetValue( value );
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - value = %d, index = %d" ), value, index ) );

        TInt intervalInMillisecond( 1000 / value );
        if ( intervalInMillisecond < 1 )
            {
            intervalInMillisecond = 1;
            }
        RProperty::Set( KPSUidSensrvTest, KSensrvSamplingInterval, intervalInMillisecond );
        iPropDataRate.SetValue( value );
        iPropDataRate.SetItemIndex( index );

        PropertyChannged( aChannelId, iPropDataRate );
        }

    else if( aProperty.GetPropertyId() == KSensrvPropIdMeasureRange )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - property KSensrvPropIdMeasureRange" ) ) );
        TReal value( 0 );
        TInt index( aProperty.PropertyItemIndex() );
        aProperty.GetValue( value );
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - value = %f, index = %d" ), value, index ) );

        iPropMeasureRange.SetValue( value );
        iPropMeasureRange.SetMinValue( value );
        iPropMeasureRange.SetMaxValue( value );
        iPropMeasureRange.SetItemIndex( index );

        PropertyChannged( aChannelId, iPropMeasureRange );

        }
    else if( aProperty.GetPropertyId() == KSensrvSensorDescription )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - property KSensrvPropIdMeasureRange" ) ) );
        TBuf8<KSensrvPropertyTextBufferSize> value;
        TInt index( aProperty.PropertyItemIndex() );
        aProperty.GetValue( value );
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - value = %S, index = %d" ), &value, index ) );
        iPropSensorDescription.SetValue( value );
        iPropSensorDescription.SetItemIndex( index );

        PropertyChannged( aChannelId, iPropSensorDescription );
        }

    else if( aProperty.GetPropertyId() == KSensrvPropIdAvailability )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - property KSensrvPropIdAvailability" ) ) );
        TInt value( 0 );
        TInt index( aProperty.PropertyItemIndex() );
        aProperty.GetValue( value );
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - value = %d, index = %d" ), value, index ) );

		iPropAvailability.SetValue( value );
		iPropAvailability.SetItemIndex( index );

        PropertyChannged( aChannelId, iPropAvailability );
        }

    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::SetPropertyL() - Return" ) ) );
    }

//
// ----------------------------------------------------------------------------------
// CEventSsyStub::GetPropertyL()
// ----------------------------------------------------------------------------------
//
void CEventSsyStub::GetPropertyL( const TSensrvChannelId /*aChannelId*/,
                                   TSensrvProperty& aProperty )
    {
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - Enter" ) ) );

    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetPropertyLeave, leave );
    if( leave )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - GetPropertyL Leave %d" ), leave ) );
        iPeriodicForProperty->Cancel();
        User::Leave( leave );
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetPropertyJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - GetPropertyL JAM" ) ) );
        iPeriodicForProperty->Cancel();
        //TRequestStatus status;
        //User::WaitForRequest( status );
        User::After( 1000000000 );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetPropertyPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - GetPropertyL PANIC" ) ) );
        iPeriodicForProperty->Cancel();
        User::Panic( _L("TESTPANIC"), 9999 );
        }

    // Data rate property must be provided by all SSYs
    if ( aProperty.GetPropertyId() == KSensrvPropIdDataRate )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - property KSensrvPropIdDataRate" ) ) );
        TInt intervalInMillisecond(0);
        RProperty::Get( KPSUidSensrvTest, KSensrvSamplingInterval, intervalInMillisecond );

        if (!intervalInMillisecond)
            {
            intervalInMillisecond = 1;
            }

        TInt value( 1000 / intervalInMillisecond );

        // Ensure that there is at least some rate
        if ( value < 1)
            {
            value = 1;
            }
        aProperty.SetValue( value );
        aProperty.SetMinValue( value );
        aProperty.SetMaxValue( value );
        aProperty.SetItemIndex( iPropDataRate.PropertyItemIndex() );
        }
    else if( aProperty.GetPropertyId() == KSensrvPropIdMeasureRange )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - property KSensrvPropIdMeasureRange" ) ) );
        TReal value( 0 );
        TInt index( iPropMeasureRange.PropertyItemIndex() );
        iPropMeasureRange.GetValue( value );
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - value = %f, index = %d" ), value, index ) );

        aProperty.SetValue( value );
        aProperty.SetMinValue( value );
        aProperty.SetMaxValue( value );
        aProperty.SetItemIndex( index );

        }
    else if( aProperty.GetPropertyId() == KSensrvSensorDescription )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - property KSensrvPropIdMeasureRange" ) ) );
        TBuf8<KSensrvPropertyTextBufferSize> value;
        TInt index( iPropSensorDescription.PropertyItemIndex() );
        iPropSensorDescription.GetValue( value );
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - value = %S, index = %d" ), &value, index ) );
        aProperty.SetValue( value );
        aProperty.SetItemIndex( index );
        }

    else if( aProperty.GetPropertyId() == KSensrvPropIdAvailability )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - property KSensrvPropIdDataRate" ) ) );
        TInt value(0);

		RProperty::Get( KPSUidSensrvTest, KSensrvPropIdAvailability, value );

        aProperty.SetValue( value );
        aProperty.SetMinValue( 0 );
        aProperty.SetMaxValue( 1 );
        aProperty.SetItemIndex( iPropAvailability.PropertyItemIndex() );
        }

    else
        {
        aProperty.SetValue( 666 );
        }

    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - Return" ) ) );
    }

//
// ----------------------------------------------------------------------------------
// CEventSsyStub::GetAllPropertiesL()
// ----------------------------------------------------------------------------------
//
void CEventSsyStub::GetAllPropertiesL( const TSensrvChannelId /*aChannelId*/,
                                        RSensrvPropertyList& aChannelPropertyList)
    {
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetAllPropertiesL() - Enter" ) ) );

    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetAllPropertiesLeave, leave );
    if( leave )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - GetPropertyL Leave %d" ), leave ) );
        iPeriodicForProperty->Cancel();
        User::Leave( leave );
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetAllPropertiesJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetPropertyL() - GetPropertyL JAM" ) ) );
        iPeriodicForProperty->Cancel();
        //TRequestStatus status;
        //User::WaitForRequest( status );
        User::After( 1000000000 );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetAllPropertiesPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetAllPropertiesL() - GetAllPropertiesL PANIC" ) ) );
        iPeriodicForProperty->Cancel();
        User::Panic( _L("TESTPANIC"), 9999 );
        }

    TInt propertyCount( 0 );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetAllPropertyCount, propertyCount );

    if( propertyCount == 0 )
        {
        aChannelPropertyList.Append( iPropDataRate );
        aChannelPropertyList.Append( iPropMeasureRange );
        aChannelPropertyList.Append( iPropSensorDescription );
        aChannelPropertyList.Append( iPropAvailability );
        }

    else
        {
        for( TInt i = 0; i < propertyCount; i++ )
            {
            TSensrvProperty property;
            property.SetMinValue( i );
            property.SetMaxValue( i );
            property.SetValue( i );
            property.SetItemIndex( i );

            aChannelPropertyList.Append( property );
            }
        }

    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventSsyStub::GetAllPropertiesL() - aChannelPropertyList count = %d" ), aChannelPropertyList.Count() ) );
    }

// ---------------------------------------------------------------------------
// CEventSsyStub::GetPropertyProviderInterfaceL
// ---------------------------------------------------------------------------
//
void CEventSsyStub::GetPropertyProviderInterfaceL( TUid aInterfaceUid,
	                                                 TAny*& aInterface )
    {
    aInterface = NULL;

	  if ( aInterfaceUid.iUid == KSsyPropertyProviderInterface1.iUid )
		    {
		    aInterface = reinterpret_cast<TAny*>(
			     static_cast<MSsyPropertyProvider*>( this ) );
		    }
    }
  
// ---------------------------------------------------------------------------
// CEventSsyStub::PropertyPeriodicTick
// ---------------------------------------------------------------------------
//
TInt CEventSsyStub::PropertyPeriodicTick( TAny* aObject )
    {
    return static_cast<CEventSsyStub*>( aObject )->DoPropertyPeriodicTick();
    }

// ---------------------------------------------------------------------------
// CEventSsyStub::DoPropertyPeriodicTick
// ---------------------------------------------------------------------------
//
TInt CEventSsyStub::DoPropertyPeriodicTick()
    {

    RSensrvChannelList affectedChannels;
    for( TInt i = 0; i < iChannelArray.Count(); i++ )
        {
        CEventChannel* channel = iChannelArray[ i ];
        affectedChannels.Append( channel->ChannelId() );
        }


    _LIT8( KSSYTest,"KKK" );
    TSensrvProperty changedProperty;
    changedProperty.SetValue( KSSYTest );

    iSsyCallback->PropertyChanged( affectedChannels[0], affectedChannels, changedProperty );

    affectedChannels.Reset();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CEventSsyStub::PropertyChannged
// ---------------------------------------------------------------------------
//
void CEventSsyStub::PropertyChannged( const TSensrvChannelId aChannelId, TSensrvProperty& aProperty )
    {
    RSensrvChannelList affectedChannels;
    for( TInt i = 0; i < iChannelArray.Count(); i++ )
        {
        CEventChannel* channel = iChannelArray[ i ];
        affectedChannels.Append( channel->ChannelId() );
        }

    iSsyCallback->PropertyChanged( aChannelId, affectedChannels, aProperty );

    affectedChannels.Reset();

    }

