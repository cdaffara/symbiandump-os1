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
#include "SSYSensorControlStubBase.h"
#include "siftrace.h"
#include "SSYChannelXYZ.h"
#include "sensrvsensorchannels.h"
#include "sensrvchanneldatatypes.h"

#include "SensrvInternalPSKeys.h"
#include <sensrvgeneralproperties.h>

#include <ssycallback.h>
#include <e32property.h>
#include "ssypslistener.h"

//_LIT8( KLocationNew, "New%d" );
_LIT8( KChannelVendorIDNew, "VendorNew" );




//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubBase::CSSYSensorControlStubBase()
// ----------------------------------------------------------------------------------
//
CSSYSensorControlStubBase::CSSYSensorControlStubBase( MSsyCallback* aSsyCallback )
    : iSsyCallback( aSsyCallback ), iChannelArray( 5 )
	{
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::CSSYSensorControlStubBase() - Enter" ) ) );

	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::CSSYSensorControlStubBase() - Return" ) ) );
	}


void CSSYSensorControlStubBase::BaseConstructL()
    {
    iPeriodicForProperty = CPeriodic::NewL( CActive::EPriorityIdle );
    }

//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubBase::~CSSYSensorControlStubBase()
// ----------------------------------------------------------------------------------
//
// Destructor.
CSSYSensorControlStubBase::~CSSYSensorControlStubBase()
	{
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::~CSSYSensorControlStubBase() - Enter" ) ) );
	TInt unloadingCount( 0 );
    TInt err = RProperty::Get( KPSUidSensrvTest, KSensrvSSYUnloadingCount, unloadingCount );
    if( err == KErrNone )
        {
        unloadingCount++;
        RProperty::Set( KPSUidSensrvTest, KSensrvSSYUnloadingCount, unloadingCount );
        }

    if( iPeriodicForProperty )
	    {
	    iPeriodicForProperty->Cancel();
	    }
	delete iPeriodicForProperty;

    iChannelArray.ResetAndDestroy();

    delete iPsListener1;
    delete iPsListener2;
    delete iPsListener3;
    delete iPsListener4;

    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::~CSSYSensorControlStubBase() - return" ) ) );
	}


// ---------------------------------------------------------------------------
// From class CSsySensorControlL
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CSSYSensorControlStubBase::OpenChannelL( TSensrvChannelId aChannelID )
	{
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::OpenChannel() - Enter" ) ) );
	for( TInt i = 0; i < iChannelArray.Count(); i++ )
	    {
	    CSSYChannelBase* channel = iChannelArray[ i ];
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

	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::OpenChannel() - Return" ) ) );
	}



// ---------------------------------------------------------------------------
// From class CSsySensorControlL
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CSSYSensorControlStubBase::CloseChannelL( TSensrvChannelId aChannelID )
	{
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::CloseChannel() - Enter" ) ) );
	for( TInt i = 0; i < iChannelArray.Count(); i++ )
	    {
	    CSSYChannelBase* channel = iChannelArray[ i ];
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
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::CloseChannel() - Return" ) ) );
	}


//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubBase::CheckPropertyDependenciesL()
// ----------------------------------------------------------------------------------
//
void CSSYSensorControlStubBase::CheckPropertyDependenciesL
                                  ( const TSensrvChannelId /*aChannelId*/,
                                    const TSensrvProperty& /*aProperty*/,
                                    RSensrvChannelList& aAffectedChannels  )
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::CheckPropertyDependenciesL()" ) ) );

    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvCheckPropertyDependenciesLeave, leave );
    if( leave )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::CheckPropertyDependenciesL() - CheckPropertyDependenciesL Leave %d" ), leave ) );
        iPeriodicForProperty->Cancel();
        User::Leave( leave );
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvCheckPropertyDependenciesJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::CheckPropertyDependenciesL() - CheckPropertyDependenciesL JAM" ) ) );
        iPeriodicForProperty->Cancel();
        //TRequestStatus status;
        User::After( 1000000000 );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvCheckPropertyDependenciesPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::CheckPropertyDependenciesL() - CheckPropertyDependenciesL PANIC" ) ) );
        iPeriodicForProperty->Cancel();
        User::Panic( _L("TESTPANIC"), 9999 );
        }

    RSensrvChannelList affectedChannels;
    for( TInt i = 0; i < iChannelArray.Count(); i++ )
        {
        CSSYChannelBase* channel = iChannelArray[ i ];
        aAffectedChannels.Append( channel->ChannelId() );
        }

    }

//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubBase::SetPropertyL()
// ----------------------------------------------------------------------------------
//
void CSSYSensorControlStubBase::SetPropertyL(  const TSensrvChannelId aChannelId,
                                    const TSensrvProperty& aProperty )
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - Enter" ) ) );

    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvSetPropertyLeave, leave );
    if( leave )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - SetPropertyL Leave %d" ), leave ) );
        iPeriodicForProperty->Cancel();
        User::Leave( leave );
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvSetPropertyJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - SetPropertyL JAM" ) ) );
        iPeriodicForProperty->Cancel();
        //TRequestStatus status;
        User::After( 1000000000 );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvSetPropertyPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - SetPropertyL PANIC" ) ) );
        iPeriodicForProperty->Cancel();
        User::Panic( _L("TESTPANIC"), 9999 );
        }

    if ( aProperty.GetPropertyId() == KSensrvPropIdDataRate )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - property KSensrvPropIdDataRate" ) ) );
        TInt value( 0 );
        TInt index( aProperty.PropertyItemIndex() );
        aProperty.GetValue( value );
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - value = %d, index = %d" ), value, index ) );

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
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - property KSensrvPropIdMeasureRange" ) ) );
        TReal value( 0 );
        TInt index( aProperty.PropertyItemIndex() );
        aProperty.GetValue( value );
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - value = %f, index = %d" ), value, index ) );

        iPropMeasureRange.SetValue( value );
        iPropMeasureRange.SetMinValue( value );
        iPropMeasureRange.SetMaxValue( value );
        iPropMeasureRange.SetItemIndex( index );

        PropertyChannged( aChannelId, iPropMeasureRange );

        }
    else if( aProperty.GetPropertyId() == KSensrvSensorDescription )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - property KSensrvPropIdMeasureRange" ) ) );
        TBuf8<KSensrvPropertyTextBufferSize> value;
        TInt index( aProperty.PropertyItemIndex() );
        aProperty.GetValue( value );
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - value = %S, index = %d" ), &value, index ) );
        iPropSensorDescription.SetValue( value );
        iPropSensorDescription.SetItemIndex( index );

        PropertyChannged( aChannelId, iPropSensorDescription );
        }
    else if( aProperty.GetPropertyId() == KSensrvPropIdChannelScale )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - property KSensrvPropIdChannelScale" ) ) );
        TSecurityInfo secInfo = aProperty.GetSecurityInfo();
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - Check capability" ) ) );
        if( !secInfo.iCaps.HasCapability( ECapabilityTCB ) )
            {
            COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - Leave KErrPermissionDenied" ) ) );
            User::Leave( KErrPermissionDenied );
            }
        }
    else if( aProperty.GetPropertyId() == KSensrvPropIdAvailability )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - property KSensrvPropIdAvailability" ) ) );
        TInt value( 0 );
        TInt index( aProperty.PropertyItemIndex() );
        aProperty.GetValue( value );
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - value = %d, index = %d" ), value, index ) );

        iPropAvailability.SetPropertyId( KSensrvPropIdAvailability );
        iPropAvailability.SetValue( value );
        iPropAvailability.SetMinValue( 0 );
        iPropAvailability.SetMaxValue( 1 );
        iPropAvailability.SetItemIndex( index );

        PropertyChannged( aChannelId, iPropAvailability );

        }
    // Check capability of all properties
    TSecurityInfo secInfo = aProperty.GetSecurityInfo();
    if( !secInfo.iCaps.HasCapability( ECapabilityWriteDeviceData ) )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - Leave KErrPermissionDenied" ) ) );
        //User::Leave( KErrPermissionDenied );
        }

    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::SetPropertyL() - Return" ) ) );
    }

//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubBase::GetPropertyL()
// ----------------------------------------------------------------------------------
//
void CSSYSensorControlStubBase::GetPropertyL( const TSensrvChannelId /*aChannelId*/,
                                   TSensrvProperty& aProperty )
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - Enter" ) ) );

    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetPropertyLeave, leave );
    if( leave )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - GetPropertyL Leave %d" ), leave ) );
        iPeriodicForProperty->Cancel();
        User::Leave( leave );
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetPropertyJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - GetPropertyL JAM" ) ) );
        iPeriodicForProperty->Cancel();
        //TRequestStatus status;
        //User::WaitForRequest( status );
        User::After( 1000000000 );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetPropertyPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - GetPropertyL PANIC" ) ) );
        iPeriodicForProperty->Cancel();
        User::Panic( _L("TESTPANIC"), 9999 );
        }

    TInt arrayCount( 0 );
    RProperty::Get( KPSUidSensrvTest, KSensrvSetPropertyArray, arrayCount );

    // Data rate property must be provided by all SSYs
    if ( aProperty.GetPropertyId() == KSensrvPropIdDataRate )
        {
        if( arrayCount == 0 )
            {
            COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - property KSensrvPropIdDataRate" ) ) );
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
        else
            {
            if( ESensrvSingleProperty == aProperty.GetArrayIndex() )
                {
                aProperty.SetValue( 1 );
                aProperty.SetMinValue( 0 );
                aProperty.SetMaxValue( 2 );
                aProperty.SetItemIndex( 0 );
                aProperty.SetArrayIndex( ESensrvArrayPropertyInfo );
                }
            else
                {
                aProperty.SetValue( 50 );
                aProperty.SetMinValue( 0 );
                aProperty.SetMaxValue( 0 );
                aProperty.SetItemIndex( 0 );
                aProperty.SetArrayIndex( aProperty.GetArrayIndex() );
                }
            }
        }
    else if( aProperty.GetPropertyId() == KSensrvPropIdMeasureRange )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - property KSensrvPropIdMeasureRange" ) ) );
        TReal value( 0 );
        TInt index( iPropMeasureRange.PropertyItemIndex() );
        iPropMeasureRange.GetValue( value );
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - value = %f, index = %d" ), value, index ) );

        aProperty.SetValue( value );
        aProperty.SetMinValue( value );
        aProperty.SetMaxValue( value );
        aProperty.SetItemIndex( index );

        }
    else if( aProperty.GetPropertyId() == KSensrvSensorDescription )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - property KSensrvPropIdMeasureRange" ) ) );
        TBuf8<KSensrvPropertyTextBufferSize> value;
        TInt index( iPropSensorDescription.PropertyItemIndex() );
        iPropSensorDescription.GetValue( value );
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - value = %S, index = %d" ), &value, index ) );
        aProperty.SetValue( value );
        aProperty.SetItemIndex( index );
        }

    else if ( aProperty.GetPropertyId() == KSensrvPropIdAvailability )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - property KSensrvPropIdAvailability" ) ) );
        TInt value( 0 );
        iPropAvailability.SetMinValue( 0 );
        iPropAvailability.SetMaxValue( 1 );
        iPropAvailability.GetValue( value );
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - value = %f" ), value ) );

		aProperty.SetValue( value );
		aProperty.SetMinValue( 0 );
		aProperty.SetMaxValue( 1 );
        }
    else
        {
        aProperty.SetValue( 666 );
        }

    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - Return" ) ) );
    }

//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubBase::GetAllPropertiesL()
// ----------------------------------------------------------------------------------
//
void CSSYSensorControlStubBase::GetAllPropertiesL( const TSensrvChannelId /*aChannelId*/,
                                        RSensrvPropertyList& aChannelPropertyList)
    {
    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetAllPropertiesL() - Enter" ) ) );

    TInt leave( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetAllPropertiesLeave, leave );
    if( leave )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - GetPropertyL Leave %d" ), leave ) );
        iPeriodicForProperty->Cancel();
        User::Leave( leave );
        }

    TInt jam( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetAllPropertiesJam, jam );
    if( jam )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetPropertyL() - GetPropertyL JAM" ) ) );
        iPeriodicForProperty->Cancel();
        //TRequestStatus status;
        //User::WaitForRequest( status );
        User::After( 1000000000 );
        }

    TInt panic( KErrNone );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetAllPropertiesPanic, panic );
    if( panic )
        {
        COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetAllPropertiesL() - GetAllPropertiesL PANIC" ) ) );
        iPeriodicForProperty->Cancel();
        User::Panic( _L("TESTPANIC"), 9999 );
        }

    TInt propertyCount( 0 );
    TInt arrayCount( 0 );
    RProperty::Get( KPSUidSensrvTest, KSensrvGetAllPropertyCount, propertyCount );
    RProperty::Get( KPSUidSensrvTest, KSensrvSetPropertyArray, arrayCount );

    if( arrayCount == 0 )
        {
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
        }
    else
        {
        TSensrvProperty property;

        property.SetPropertyId( KSensrvPropIdDataRate );
        property.SetArrayIndex( ESensrvArrayPropertyInfo );
        property.SetMinValue( 0 );
        property.SetMaxValue( 2 );
        property.SetValue( 1 );
        property.SetItemIndex( 0 );
        aChannelPropertyList.Append( property );

        property.SetPropertyId( KSensrvPropIdDataRate );
        property.SetArrayIndex( 0 );
        property.SetMinValue( 0 );
        property.SetMaxValue( 0 );
        property.SetValue( 10 );
        property.SetItemIndex( 0 );
        aChannelPropertyList.Append( property );

        property.SetPropertyId( KSensrvPropIdDataRate );
        property.SetArrayIndex( 1 );
        property.SetMinValue( 0 );
        property.SetMaxValue( 0 );
        property.SetValue( 40 );
        property.SetItemIndex( 0 );
        aChannelPropertyList.Append( property );

        property.SetPropertyId( KSensrvPropIdDataRate );
        property.SetArrayIndex( 2 );
        property.SetMinValue( 0 );
        property.SetMaxValue( 0 );
        property.SetValue( 50 );
        property.SetItemIndex( 0 );
        aChannelPropertyList.Append( property );

        property.SetPropertyId( KSensrvPropIdAvailability );
        //property.SetArrayIndex( ESensrvSingleProperty );
        property.SetArrayIndex( 0 );
        property.SetMinValue( 0 );
        property.SetMaxValue( 1 );
        property.SetValue( 1 );
        property.SetItemIndex( 0 );
        aChannelPropertyList.Append( property );
        }

    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::GetAllPropertiesL() - aChannelPropertyList count = %d" ), aChannelPropertyList.Count() ) );
    }


// ---------------------------------------------------------------------------
// CSSYSensorControlStubBase::GetPropertyProviderInterfaceL
// ---------------------------------------------------------------------------
//
void CSSYSensorControlStubBase::GetPropertyProviderInterfaceL( TUid aInterfaceUid,
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
// CSSYSensorControlStubBase::PropertyPeriodicTick
// ---------------------------------------------------------------------------
//
TInt CSSYSensorControlStubBase::PropertyPeriodicTick( TAny* aObject )
    {
    return static_cast<CSSYSensorControlStubBase*>( aObject )->DoPropertyPeriodicTick();
    }

// ---------------------------------------------------------------------------
// CSSYSensorControlStubBase::DoPropertyPeriodicTick
// ---------------------------------------------------------------------------
//
TInt CSSYSensorControlStubBase::DoPropertyPeriodicTick()
    {

    RSensrvChannelList affectedChannels;
    for( TInt i = 0; i < iChannelArray.Count(); i++ )
        {
        CSSYChannelBase* channel = iChannelArray[ i ];
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
// CSSYSensorControlStubBase::PropertyChannged
// ---------------------------------------------------------------------------
//
void CSSYSensorControlStubBase::PropertyChannged( const TSensrvChannelId aChannelId, TSensrvProperty& aProperty )
    {
    RSensrvChannelList affectedChannels;
    for( TInt i = 0; i < iChannelArray.Count(); i++ )
        {
        CSSYChannelBase* channel = iChannelArray[ i ];

        // Ignore the setting channel
        if (aChannelId != channel->ChannelId())
            {
            affectedChannels.Append( channel->ChannelId() );
            }
        }

    iSsyCallback->PropertyChanged( aChannelId, affectedChannels, aProperty );

    affectedChannels.Reset();

    }


//
// ----------------------------------------------------------------------------------
// CSSYSensorControlStubBase::PsValueSet()
// ----------------------------------------------------------------------------------
//
void CSSYSensorControlStubBase::PsValueSet( TUid /*aCategory*/,
                                            TUint aKey,
                                            TInt aValue )
	{
	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::PsValueSet(aKey: %d, aValue: %d) - Enter" ), aKey, aValue ) );

    if(   aKey == KSensrvCreateNewChannelsFirst
       || aKey == KSensrvCreateNewChannelsSecond
       || aKey == KSensrvRemoveNewChannelsFirst
       || aKey == KSensrvRemoveNewChannelsSecond)
        {
        // If value is zero, do nothing
        if (aValue)
            {
            TInt oldAmount = iAddedChannels;
            TInt err(KErrNone);

            // Add or remove new channels and register changes.
            // Both plugins will create similar new channels
        	if(aKey == KSensrvCreateNewChannelsFirst || aKey == KSensrvCreateNewChannelsSecond)
        	    {
        	    COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::PsValueSet() - Adding channels" ) ) );
        	    iAddedChannels += aValue;
                for( TInt i = oldAmount; i < iAddedChannels; i++ )
                    {
                    TBuf8<KSensrvLocationLength> location;
                    GetNewLocString(location, i);
                    TPtrC8 vendorIdPtr( KChannelVendorIDNew() );
                    CSSYChannelXYZ* channel = NULL;
                    TRAP(err, channel = CSSYChannelXYZ::NewL( iSsyCallback, this, ESensrvContextTypeDevice,
                                                                ESensrvQuantityAcceleration, KSensrvChannelTypeIdAccelerometerXYZAxisData,
                                                                location, vendorIdPtr ));

                    if (err == KErrNone)
                        {
                        iChannelArray.Append( channel );
                        }
                    }

        	    }
        	else if (aKey == KSensrvRemoveNewChannelsFirst || aKey == KSensrvRemoveNewChannelsSecond)
        	    {
        	    iAddedChannels -= aValue;

        	    if (iAddedChannels < 0)
        	        {
        	        iAddedChannels = 0;
        	        }

        	    for( TInt i = oldAmount-1; i >= iAddedChannels; i--)
        	        {
        	        // Delete channels starting from newest
        	        TInt last = iChannelArray.Count()-1;
        	        delete iChannelArray[last];
        	        iChannelArray.Remove(last);
        	        }
        	    }

            RSensrvChannelInfoList channelInfoList;
            CleanupClosePushL( channelInfoList );
            for( TInt i = 0; i < iChannelArray.Count(); i++ )
                {
                channelInfoList.Append( (*iChannelArray[i]).ChannelInfo() );
                }

        	// Register new channel set
            iSsyCallback->RegisterChannelsL( channelInfoList );

            // Update ChannelId
            for( TInt i = 0; i < iChannelArray.Count(); i++ )
                {
                (*iChannelArray[i]).SetChannelId( channelInfoList[i].iChannelId );
                }
            CleanupStack::PopAndDestroy( &channelInfoList );
            }
        }
    else if (aKey == KSensrvRegisterZeroChannels)
        {
        // Only react to setting value to zero, otherwise this will loop endlessly
        if (!aValue)
            {
            RSensrvChannelInfoList channelInfoList;
            CleanupClosePushL( channelInfoList );
            RProperty::Set(KPSUidSensrvTest, KSensrvRegisterZeroChannels, KErrArgument); // Initial value
            TRAPD(err, iSsyCallback->RegisterChannelsL( channelInfoList ));

            if (err)
                {
                RProperty::Set(KPSUidSensrvTest, KSensrvRegisterZeroChannels, err);
                }
            else
                {
                // Registering empty list should not work. We need to set some error code
                // to KSensrvRegisterZeroChannels, otherwise test will stall
                RProperty::Set(KPSUidSensrvTest, KSensrvRegisterZeroChannels, -999);
                }
            CleanupStack::PopAndDestroy( &channelInfoList );
            }
        }
    else if (aKey == KSensrvRegisterInvalidChannels)
        {
        // Only react to setting value to zero, otherwise this will loop endlessly
        if (!aValue)
            {
            RSensrvChannelInfoList channelInfoList;
            CleanupClosePushL( channelInfoList );
            TSensrvChannelInfo newInfo;
            channelInfoList.Append(newInfo);
            RProperty::Set(KPSUidSensrvTest, KSensrvRegisterInvalidChannels, KErrArgument); // Initial value
            TRAPD(err, iSsyCallback->RegisterChannelsL( channelInfoList ));

            if (err)
                {
                RProperty::Set(KPSUidSensrvTest, KSensrvRegisterInvalidChannels, err);
                }
            else
                {
                // Registering empty list should not work. We need to set some error code
                // to KSensrvRegisterZeroChannels, otherwise test will stall
                RProperty::Set(KPSUidSensrvTest, KSensrvRegisterInvalidChannels, -999);
                }

            CleanupStack::PopAndDestroy( &channelInfoList );
            }
        }


	COMPONENT_TRACE( ( _L( "SSYStub - CSSYSensorControlStubBase::PsValueSet() - Return" ) ) );
	}

