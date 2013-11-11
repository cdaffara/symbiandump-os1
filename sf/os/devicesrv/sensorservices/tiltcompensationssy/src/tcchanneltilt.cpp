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
* Description:  CTCChannelTilt class implementation.
*
*/


#include <ssycallback.h>
#include <sensrvsensorchannels.h>
#include <sensrvchanneldatatypes.h>
#include <sensrvmagnetometersensor.h>
#include <sensordatacompensator.h>

#include "tcchanneltilt.h"
#include "tcstatetiltidle.h"
#include "tcstatetiltopen.h"
#include "tcstatetiltlistendata.h"
#include "tcstatetiltbufferfull.h"
#include "tcstatetiltclosing.h"
#include "tceventaccelerometer.h"
#include "tceventmagnetometer.h"
#include "tceventproperty.h"
#include "tctransactionflushbuffer.h"
#include "tcutil.h"
#include "configuration.h"
#include "common.h"
#include "trace.h"

// LOCAL DEFINITIONS

const TInt KDesiredCount = 1;
const TInt KMaximumCount = 1;
const TInt KBufferingPeriod = 0;
const TInt KMaximumDataRate = 10;

CTCChannelTilt* CTCChannelTilt::NewL( MSsyCallback& aCallback )
    {
    FUNC_LOG;
    
    CTCChannelTilt* self = CTCChannelTilt::NewLC( aCallback );
    CleanupStack::Pop( self );
    
    return self;
    }

CTCChannelTilt* CTCChannelTilt::NewLC( MSsyCallback& aCallback )
    {
    FUNC_LOG;
    
    CTCChannelTilt* self = new( ELeave ) CTCChannelTilt( aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

CTCChannelTilt::~CTCChannelTilt()
    {
    FUNC_LOG;
    
    if( iAccelerometerChannel )
        {
        delete iAccelerometerChannel;
        iAccelerometerChannel = NULL;
        }
    if( iMagnetometerChannel )
        {
        delete iMagnetometerChannel;
        iMagnetometerChannel = NULL;
        }
    
    if( iCurrentTransaction )
        {
        delete iCurrentTransaction;
        iCurrentTransaction = NULL;
        }
    iProperties.Close();
    
    delete iMagnetometerCompensator;
    delete iAccelerometerCompensator;
    }

CTCChannelTilt::CTCChannelTilt( MSsyCallback& aCallback ):
    CTCChannel( aCallback )
    {
    FUNC_LOG;
    }
    
void CTCChannelTilt::ConstructL()
    {
    FUNC_LOG;
    
    iIsSetFromListened = EFalse;
    
    BaseConstructL();
    // Initialize properties
    for( TInt i = 0; i < EChannelPropertyTiltLast; i++ )
        {
        switch( KTiltSsyChannelProperties[ i ].iPropertyType )
            {
            case ESensrvIntProperty:
                {
                TSensrvProperty property( KTiltSsyChannelProperties[ i ].iPropertyId,
                    KTiltSsyChannelProperties[ i ].iItemIndex,
                    KTiltSsyChannelProperties[ i ].iIntValue,
                    KTiltSsyChannelProperties[ i ].iIntMaxValue,
                    KTiltSsyChannelProperties[ i ].iIntMinValue,
                    KTiltSsyChannelProperties[ i ].iReadOnly,
                    KTiltSsyChannelProperties[ i ].iPropertyType );
                iProperties.AppendL( property );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// METHODS

// ----------------------------------------------------------------------------------
// CTCChannelTilt::RegisterChannelL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::RegisterChannelL( RSensrvChannelInfoList& aChannels )
    {
    FUNC_LOG;

    // Register tilt angle channel
    TChannelInfo infoStruct = KTiltSsyChannels[ EChannelTilt ];
    TSensrvChannelInfo info( infoStruct.iChannelId,
        infoStruct.iContextType,
        infoStruct.iQuantity,
        infoStruct.iChannelType,
        TPtrC8( infoStruct.iLocation ),
        TPtrC8( infoStruct.iVendorId ),
        infoStruct.iDataItemSize,
        infoStruct.iDataTypeId );
    aChannels.AppendL( info );
    
    // Store channel info
    iChannelInfo = info;
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::OpenChannelL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::OpenChannelL()
    {
    FUNC_LOG;
    
    // Find and create accelerometer and magnetometer channels
    FindAndCreateAccelerometerChannelL();
    
    FindAndCreateMagnetometerChannelL();
    
    // Get current state and send open channel event
    CTCState* current = CurrentState();
    if( current )
        {
        current->HandleEventL( EEventIdHandleOpenChannel );
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::CloseChannel
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::CloseChannelL()
    {
    FUNC_LOG;

    // Get current state and send close channel event
    CTCState* current = CurrentState();
    if( current )
        {
        current->HandleEventL( EEventIdHandleCloseChannel );
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::RegisterStateL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::RegisterStateL( CTCState::TCStateId aStateId,
    RStateArray& aStateArray )
    {
    FUNC_LOG;

    switch( aStateId )
        {
        case CTCState::ETCStateIdle:
            {
            CTCStateTiltIdle* state = CTCStateTiltIdle::NewL( *this, *this );
            CleanupStack::PushL( state );
            aStateArray.AppendL( state );
            CleanupStack::Pop( state );
            break;
            }
        case CTCState::ETCStateOpen:
            {
            CTCStateTiltOpen* state = CTCStateTiltOpen::NewL( *this, *this );
            CleanupStack::PushL( state );
            aStateArray.AppendL( state );
            CleanupStack::Pop( state );
            break;
            }
        case CTCState::ETCStateListenData:
            {
            CTCStateTiltListenData* state = CTCStateTiltListenData::NewL( *this, *this );
            CleanupStack::PushL( state );
            aStateArray.AppendL( state );
            CleanupStack::Pop( state );
            break;
            }
        case CTCState::ETCStateBufferFull:
            {
            CTCStateTiltBufferFull* state = CTCStateTiltBufferFull::NewL( *this, *this );
            CleanupStack::PushL( state );
            aStateArray.AppendL( state );
            CleanupStack::Pop( state );
            break;
            }
        case CTCState::ETCStateClosing:
            {
            CTCStateTiltClosing* state = CTCStateTiltClosing::NewL( *this, *this );
            CleanupStack::PushL( state );
            aStateArray.AppendL( state );
            CleanupStack::Pop( state );
            break;
            }
        default:
            {
            ERROR_GEN( "Oops. Invalid state" );
            User::Leave( KErrGeneral );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::RunL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::RunL()
    {
    FUNC_LOG;
    
    TInt status = iStatus.Int();
    
    INFO_2( "Transaction [%S] completed with code [%d]",
        &TCUtil::TransactionIdAsDesC( iCurrentTransactionId ), status );
    
    switch( iCurrentTransactionId )
        {
        case ETCTransactionIdOpenChannel:
            {
            // Channel opened, finnish request to sensor server
            iCallback.ChannelOpened( iChannelInfo.iChannelId, status, this, this );
            break;
            }
        case ETCTransactionIdCloseChannel:
            {
            // Channel closed, finnish request to sensor server
            iCallback.ChannelClosed( iChannelInfo.iChannelId );
            break;
            }
        case ETCTransactionIdStopChannelData:
            {
            // Reset data
            iData = NULL;
            iCount = 0;
            break;
            }
        case ETCTransactionIdFlushBuffer:
            {
            ASSERT_DEBUG_TRACE( iCurrentTransaction, ENullPointer );
            
            // Reset data
            iData = NULL;
            iCount = 0;
                
            // Buffer filled and needs to be flushed
            TTCTransactionFlushBuffer& transaction =
                static_cast<TTCTransactionFlushBuffer&>( *iCurrentTransaction );
            iCallback.BufferFilled( iChannelInfo.iChannelId,
                transaction.iDataCount,
                iData,
                iCount );
                
            INFO_2( "Received new data pointer [0x%x] and data count [%d]",
                iData,
                iCount );
            break;
            }
        default:
            {
            // Nothing to do
            ERROR_GEN( "Incorrect transaction ID" );
            break;
            }
        }

    // Clean up transaction
    if( iCurrentTransaction )
        {
        delete iCurrentTransaction;
        iCurrentTransaction = NULL;
        }

    // Inform current state that transaction has been completed
    CTCState* current = CurrentState();
    if( current )
        {
        TTCTransactionId currentTransactionId = iCurrentTransactionId;
        iCurrentTransactionId = ETCTransactionIdNull;
        current->HandleTransactionCompletedL( currentTransactionId, status );
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::CheckPropertyDependenciesL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::CheckPropertyDependenciesL( const TSensrvChannelId /*aChannelId*/,
    const TSensrvProperty& /*aProperty*/,
    RSensrvChannelList& /*aAffectedChannels */)
    {
    FUNC_LOG;
    
    // No depencies to other channels
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::SetPropertyL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::SetPropertyL( const TSensrvChannelId aChannelId,
    const TSensrvProperty& aProperty )
    {
    FUNC_LOG;

    // Check that we have a correct channel ID
    if( aChannelId == iChannelInfo.iChannelId )
        {
        TInt err = KErrNotFound;
        TSensrvProperty* property = Property( aProperty.GetPropertyId() );
        if( property )
            {
            // Check that property is not read only
            INFO_1( "Property id [%d] found from supported properties list",
                property->GetPropertyId() );
            
            if( !property->ReadOnly() )
                {
                INFO_1( "Property id [%d] supports writing",
                    property->GetPropertyId() );
                
                // Check that value is correct
                switch( property->PropertyType() )
                    {
                    case ESensrvIntProperty:
                        {
                        INFO_1( "Maximum data rate for tilt channel [%d]",
                                iMaximumDataRate );
                        // Get new value
                        TInt value = 0;
                        aProperty.GetValue( value );
                        
                        // Get allowed min and max
                        TInt min = 0;
                        TInt max = 0;
                        property->GetMinValue( min );
                        property->GetMaxValue( max );
                        if( value >= min && value <= max && value <= iMaximumDataRate )
                            {
                            property->SetValue( value );
                                INFO_2( "New value [%d] set for property id [%d]",
                                value,
                                property->GetPropertyId() );
                            err = KErrNone;
                            }
                        else
                            {
                            ERROR_GEN_2( "Value [%d] incorrect for property id [%d]",
                                value,
                                property->GetPropertyId() );
                            err = KErrArgument;
                            }
                        break;
                        }
                    case ESensrvRealProperty:
                    // Fall through
                    case ESensrvBufferProperty:
                    // Fall through
                    default:
                        {
                        // Other property types than Int not supported
                        err = KErrArgument;
                        break;
                        }
                    }
                }
            else
                {
                if( ( property->GetPropertyId() == KSensrvPropCalibrationLevel ) &&
                    iIsSetFromListened )
                    {
                    // Check that value is correct
                    switch( property->PropertyType() )
                        {
                        case ESensrvIntProperty:
                            {
                            // Get new value
                            TInt value = 0;
                            aProperty.GetValue( value );
                            
                            // Get allowed min and max
                            property->SetValue( value );
                                    INFO_2( "New value [%d] set for property id [%d]",
                                    value,
                                    property->GetPropertyId() );
                            iIsSetFromListened = EFalse;
                            err = KErrNone;
                            break;
                            }
                        case ESensrvRealProperty:
                        // Fall through
                        case ESensrvBufferProperty:
                        // Fall through
                        default:
                            {
                            // Other property types than Int not supported
                            err = KErrArgument;
                            break;
                            }
                        }
                    }
                else
                    {
                    // Cannot modify read only property
                    ERROR_GEN_1( "Property id [%d] does not support writing",
                        property->GetPropertyId() );
                    err = KErrArgument;
                    }
                }
            }
        
        ERROR_1( err, "Property set for id [%d] failed", aProperty.GetPropertyId() );
        User::LeaveIfError( err );
        
        TSensrvProperty property1;
        property1.SetPropertyId( KSensrvPropCalibrationLevel );
        GetPropertyL( iChannelInfo.iChannelId, property1 );
        RSensrvChannelList affectedChannels;
        affectedChannels.AppendL( iChannelInfo.iChannelId );
        iCallback.PropertyChanged( iChannelInfo.iChannelId, affectedChannels, property1 );
        affectedChannels.Close();
        
        // Inform current state that property has changed
        CTCState* current = CurrentState();
        if( current )
            {
            TTCEventProperty event( aProperty );
            current->HandleEventL( event.Id(), &event );
            }
        }
    else
        {
        ERROR_GEN_1( "Incorrect channel id [%d]", aChannelId );
        User::Leave( KErrNotSupported );
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::GetPropertyL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::GetPropertyL( const TSensrvChannelId aChannelId, 
    TSensrvProperty& aProperty )
    {
    FUNC_LOG;

    // Check that we have a correct channel ID
    if( aChannelId == iChannelInfo.iChannelId )
        {
        // Check if property exists
        TSensrvProperty* property = Property( aProperty.GetPropertyId() );
        if( property )
            {
            // Property found, set new values
            aProperty = *property;
            }
        else
            {
            ERROR_GEN_1( "Property id [%d] not supported", aProperty.GetPropertyId() );
            User::Leave( KErrNotFound );
            }
        }
    else
        {
        ERROR_GEN_1( "Incorrect channel ID: %d", aChannelId );
        User::Leave( KErrNotSupported );
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::GetAllPropertiesL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::GetAllPropertiesL( const TSensrvChannelId aChannelId, 
    RSensrvPropertyList& aChannelPropertyList )
    {
    FUNC_LOG;

    // Check that we have a correct channel ID
    if( aChannelId == iChannelInfo.iChannelId )
        {
        // Get all properties
        TInt count = iProperties.Count();
        for( TInt i = 0; i < count; i++ )
            {
            aChannelPropertyList.AppendL( iProperties[i] );
            }
        }
    else
        {
        ERROR_GEN_1( "Incorrect channel ID: %d", aChannelId );
        User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CTCChannelTilt::GetPropertyProviderInterfaceL
// -----------------------------------------------------------------------------
//
void CTCChannelTilt::GetPropertyProviderInterfaceL( TUid aInterfaceUid, 
    TAny*& aInterface )
    {
    FUNC_LOG;

    aInterface = NULL;
    
	if ( aInterfaceUid.iUid == KSsyPropertyProviderInterface1.iUid )
		{
		aInterface = reinterpret_cast<TAny*>(
			static_cast<MSsyChannelDataProvider*>( this ) );
		}
    }
    
// ----------------------------------------------------------------------------------
// CTCChannelTilt::StartChannelDataL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::StartChannelDataL( const TSensrvChannelId aChannelId, 
    TUint8* aBuffer,
    TInt aCount )
    {
    FUNC_LOG;
    
    // Check that we have a correct channel ID
    if( aChannelId == iChannelInfo.iChannelId )
        {
        // Store data
        iData = aBuffer;
        iCount = aCount;

        // Get current state and send start channel data event
        CTCState* current = CurrentState();
        if( current )
            {
            current->HandleEventL( EEventIdHandleStartChannelData );
            }
        }
    else
        {
        ERROR_GEN_1( "Incorrect channel id [%d]", aChannelId );
        User::Leave( KErrNotSupported );
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::StopChannelDataL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::StopChannelDataL( const TSensrvChannelId aChannelId )
    {
    FUNC_LOG;

    // Check that we have a correct channel ID
    if( aChannelId == iChannelInfo.iChannelId )
        {
        // Get current state and send stop channel data event
        CTCState* current = CurrentState();
        if( current )
            {
            current->HandleEventL( EEventIdHandleStopChannelData );
            }
        }
    else
        {
        ERROR_GEN_1( "Incorrect channel id [%d]", aChannelId );
        User::Leave( KErrNotSupported );
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::ForceBufferFilledL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::ForceBufferFilledL( const TSensrvChannelId aChannelId )
    {
    FUNC_LOG;

    // Check that we have a correct channel ID
    if( aChannelId == iChannelInfo.iChannelId )
        {
        // Get current state and send stop channel data event
        CTCState* current = CurrentState();
        if( current )
            {
            current->HandleEventL( EEventIdHandleForceBufferFull );
            }
        }
    else
        {
        ERROR_GEN_1( "Incorrect channel id [%d]", aChannelId );
        User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CTCChannelTilt::GetChannelDataProviderInterfaceL
// -----------------------------------------------------------------------------
//
void CTCChannelTilt::GetChannelDataProviderInterfaceL( TUid aInterfaceUid, 
    TAny*& aInterface )
    {
    FUNC_LOG;

    aInterface = NULL;
    
	if ( aInterfaceUid.iUid == KSsyChannelDataProviderInterface1.iUid )
		{
		aInterface = reinterpret_cast<TAny*>(
			static_cast<MSsyChannelDataProvider*>( this ) );
		}
    }
    
// ----------------------------------------------------------------------------------
// CTCChannelTilt::ProcessTransactionL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::ProcessTransactionL( TTCTransactionId aId,
    TTCTransaction* aTransaction )
    {
    FUNC_LOG;
    
    ASSERT_DEBUG_TRACE( !IsActive(), EAlreadyServingTransaction );
    
    INFO_1( "Handling transaction [%S]", &TCUtil::TransactionIdAsDesC( aId ) );
    
    // Store current transaction id
    iCurrentTransactionId = aId;

    switch( aId )
        {
        case ETCTransactionIdOpenChannel:
            {
            // Open channel
            // Need to trap to send code to sensor server
            TRAPD( err, DoOpenChannelL() );
            
            // Complete transaction
            iStatus = KRequestPending;
						if( !IsActive() )
							{
							SetActive();	
							}
						CompleteTransaction( err );
            break;
            }
        case ETCTransactionIdCloseChannel:
            {
            // Close channel
            DoCloseChannel();

            // Complete transaction
            iStatus = KRequestPending;
						if( !IsActive() )
							{
							SetActive();
							}
            CompleteTransaction( KErrNone );
            break;
            }
        case ETCTransactionIdStartChannelData:
            {
            // Start channel data
            DoStartChannelDataL();

            // Complete transaction
            iStatus = KRequestPending;
						if( !IsActive() )
							{
							SetActive();	
							}
            CompleteTransaction( KErrNone );
            break;
            }
        case ETCTransactionIdStopChannelData:
            {
            // Start channel data
            DoStopChannelData();

            // Complete transaction
            iStatus = KRequestPending;
						if( !IsActive() )
							{
							SetActive();	
							}
            CompleteTransaction( KErrNone );
            break;
            }
        case ETCTransactionIdFlushBuffer:
            {
            ASSERT_DEBUG_TRACE( aTransaction, ENullPointer );
            
            // Store current transaction, complete and finalize transaction
            if( iCurrentTransaction )
                {
                delete iCurrentTransaction;
                iCurrentTransaction = NULL;
                }
            TTCTransactionFlushBuffer& transaction =
                static_cast<TTCTransactionFlushBuffer&>( *aTransaction );
            iCurrentTransaction = new( ELeave )
                TTCTransactionFlushBuffer( transaction.iDataCount );
            
            // Complete transaction
            iStatus = KRequestPending;
						if( !IsActive() )
							{
							SetActive();	
							}
            CompleteTransaction( KErrNone );
            break;
            }
        default:
            {
            ERROR_GEN( "Should not be here! Incorrect transaction ID" );
            break;  
            }
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::Properties
// ----------------------------------------------------------------------------------
//
const RSensrvPropertyList& CTCChannelTilt::Properties() const
    {
    FUNC_LOG;
    
    return iProperties;
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::DataReceived
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::DataReceived( CSensrvChannel& aChannel, 
    TInt aCount, 
    TInt /*aDataLost */)
    {
    FUNC_LOG;

    // Check data type
    const TSensrvChannelInfo& info = aChannel.GetChannelInfo();
    switch( info.iChannelType )
        {
        case KSensrvChannelTypeIdAccelerometerXYZAxisData:
            {
            INFO_1( "Received KSensrvChannelTypeIdAccelerometerXYZAxisData data. Data count [%d]", aCount );
            if( aCount )
                {
                TPckgBuf<TSensrvAccelerometerAxisData> dataPckgBuf;
                TInt err = aChannel.GetData( dataPckgBuf );
                ERROR( err, "GetData failed." );
                if( err == KErrNone )
                    {
                    const TSensrvAccelerometerAxisData& axisData = dataPckgBuf();
                    INFO_3( "GetData succesfull. Accelerometer [X: %d], [Y: %d], [Z: %d]",
                         axisData.iAxisX,
                         axisData.iAxisY,
                         axisData.iAxisZ );
                    
                    // Compensate data before sending.                    
                    if ( !iAccelerometerCompensator )
                        {
                        INFO( "No compensator, creating" );
                        TRAP( err, iAccelerometerCompensator = CSensorDataCompensator::NewL( 
                                                       KSensrvChannelTypeIdAccelerometerXYZAxisData,
                                                       ESensorCompensateDeviceAndUIOrientation ) );
                        ERROR( err, "Unable to create compensator, data won't be compensated!" );
                        }                    
                    if ( iAccelerometerCompensator )
                        {
                        err = iAccelerometerCompensator->Compensate( dataPckgBuf );
                        ERROR( err, "Data compensation failed, err=%d" );
                        }
                    
                    INFO_3( "Compensated data Accelerometer [X: %d], [Y: %d], [Z: %d]",
                         axisData.iAxisX,
                         axisData.iAxisY,
                         axisData.iAxisZ );

                    // Send new accelerometer data event to current state
                    CTCState* current = CurrentState();
                    if( current )
                        {
                        TTCEventAccelerometer event( axisData );
                        TRAP_IGNORE( current->HandleEventL( event.Id(), &event ) );
                        }
                    }
                }
            break;
            }
        case KSensrvChannelTypeIdMagnetometerXYZAxisData:
            {
            INFO_1( "Received KSensrvChannelTypeIdMagnetometerXYZAxisData data. Data count [%d]", aCount );
            if( aCount )
                {
                TPckgBuf<TSensrvMagnetometerAxisData> dataPckgBuf;
                TInt err = aChannel.GetData( dataPckgBuf );
                ERROR( err, "GetData failed." );
                if( err == KErrNone )
                    {
                    const TSensrvMagnetometerAxisData& axisData = dataPckgBuf();
                    INFO_3( "GetData succesfull. Magnetometer [X: %d], [Y: %d], [Z: %d]",
                         axisData.iAxisXCalibrated,
                         axisData.iAxisYCalibrated,
                         axisData.iAxisZCalibrated );
                    
                    // Compensate data before sending.
                    if ( !iMagnetometerCompensator )
                        {
                        INFO( "No compensator, creating" );
                        TRAP( err, iMagnetometerCompensator = CSensorDataCompensator::NewL( 
                                                        KSensrvChannelTypeIdMagnetometerXYZAxisData,
                                                        ESensorCompensateDeviceAndUIOrientation ) );
                        ERROR( err, "Unable to create compensator, data won't be compensated!" );                                                        
                        }                        
                    if ( iMagnetometerCompensator )
                        {
                        err = iMagnetometerCompensator->Compensate( dataPckgBuf );
                        ERROR( err, "Data compensation failed, err=%d" );
                        }
                    INFO_3( "Compensated data Magnetometer [X: %d], [Y: %d], [Z: %d]",
                         axisData.iAxisXCalibrated,
                         axisData.iAxisYCalibrated,
                         axisData.iAxisZCalibrated );

                    // Send new magnetometer data event to current state
                    CTCState* current = CurrentState();
                    if( current )
                        {
                        TTCEventMagnetometer event( axisData );
                        TRAP_IGNORE( current->HandleEventL( event.Id(), &event ) );
                        }
                    }
                }
            break;
            }
        default:
            {
            ERROR_GEN( "Incorrect channel data received." );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::DataError
// ----------------------------------------------------------------------------------
//
#ifdef _DEBUG
void CTCChannelTilt::DataError( CSensrvChannel& /*aChannel*/, 
    TSensrvErrorSeverity aError )

#else
void CTCChannelTilt::DataError( CSensrvChannel& /*aChannel*/, 
    TSensrvErrorSeverity /*aError*/ )
#endif
    {
    FUNC_LOG;
    
    INFO_1( "Data error [%d]", aError );
    
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::GetDataListenerInterfaceL()
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::GetDataListenerInterfaceL( TUid /*aInterfaceUid*/, 
    TAny*& aInterface )
    {
    FUNC_LOG;
    
    aInterface = NULL;
    }


// ----------------------------------------------------------------------------------
// CTCChannelTilt::DoOpenChannelL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::DoOpenChannelL()
    {
    FUNC_LOG;
    
    ASSERT_DEBUG_TRACE( iAccelerometerChannel, EAccelerometerChannelNotCreated );
    ASSERT_DEBUG_TRACE( iMagnetometerChannel, EMagnetometerChannelNotCreated );
    iMaximumDataRate = 10;

    iAccelerometerChannel->OpenChannelL();
    iMagnetometerChannel->OpenChannelL();
    TSensrvProperty property;
    /*iAccelerometerChannel->GetPropertyL( 
        KSensrvPropIdDataRate, KSensrvItemIndexNone, property );
    property.GetValue( iMaximumDataRate );*/
    INFO_1( "Maximum data rate accelerometer %d", iMaximumDataRate );
    INFO( "Listen property changes in accelometer channel" );
    iAccelerometerChannel->SetPropertyListenerL( this );
    TSensrvProperty property1;
    iMagnetometerChannel->GetPropertyL( 
        KSensrvPropIdDataRate, KSensrvItemIndexNone, property1 );
    TInt dataRateForMagChannel( 0 );
    property1.GetValue( dataRateForMagChannel );
    INFO_1( "Maximum data rate magnetometer %d", dataRateForMagChannel );
    if( dataRateForMagChannel < iMaximumDataRate )
        {
        iMaximumDataRate = dataRateForMagChannel;
        }
    iMagnetometerChannel->SetPropertyListenerL( this );
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::DoOpenChannel
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::DoCloseChannel()
    {
    FUNC_LOG;
    
    ASSERT_DEBUG_TRACE( iAccelerometerChannel, EAccelerometerChannelNotCreated );
    ASSERT_DEBUG_TRACE( iMagnetometerChannel, EMagnetometerChannelNotCreated );

    iAccelerometerChannel->CloseChannel();
    iMagnetometerChannel->CloseChannel();
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::DoStartChannelDataL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::DoStartChannelDataL()
    {
    FUNC_LOG;
    
    ASSERT_DEBUG_TRACE( iAccelerometerChannel, EAccelerometerChannelNotCreated );
    ASSERT_DEBUG_TRACE( iMagnetometerChannel, EMagnetometerChannelNotCreated );

    iAccelerometerChannel->StartDataListeningL( this,
        KDesiredCount,
        KMaximumCount,
        KBufferingPeriod );
    
    iMagnetometerChannel->StartDataListeningL( this,
        KDesiredCount,
        KMaximumCount,
        KBufferingPeriod );
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::DoStopChannelData
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::DoStopChannelData()
    {
    FUNC_LOG;
    
    ASSERT_DEBUG_TRACE( iAccelerometerChannel, EAccelerometerChannelNotCreated );
    ASSERT_DEBUG_TRACE( iMagnetometerChannel, EMagnetometerChannelNotCreated );

    iAccelerometerChannel->StopDataListening();
    iMagnetometerChannel->StopDataListening();
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::FindAndCreateAccelerometerChannelL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::FindAndCreateAccelerometerChannelL()
    {
    FUNC_LOG;
    
    if( !iAccelerometerChannel )
        {
        // Create accelerometer channel
        TSensrvChannelInfo channelInfo;
        if( !FindChannelL( KSensrvChannelTypeIdAccelerometerXYZAxisData,
            KNullDesC8,
            KNullDesC8,
            channelInfo ) )
            {
            ERROR_GEN( "Accelerometer XYZ axis data channel not found." );
            User::Leave( KErrNotFound );
            }
            
        INFO( "Opening accelerometer channel..." );
        iAccelerometerChannel = CSensrvChannel::NewL( channelInfo );
        INFO( "Opening accelerometer channel... Success" );
        
        // Get data rate for accelometer channel
        
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::FindAndCreateMagnetometerChannelL
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::FindAndCreateMagnetometerChannelL()
    {
    FUNC_LOG;
    
    if( !iMagnetometerChannel )
        {
        // Create magnetometer channel
        TSensrvChannelInfo channelInfo;
        if( !FindChannelL( KSensrvChannelTypeIdMagnetometerXYZAxisData,
            KNullDesC8,
            KNullDesC8,
            channelInfo ) )
            {
            ERROR_GEN( "Magnetometer XYZ axis data channel not found." );
            User::Leave( KErrNotFound );
            }
            
        INFO( "Opening magnetometer channel..." );
        iMagnetometerChannel = CSensrvChannel::NewL( channelInfo );
        INFO( "Opening magnetometer channel... Success" );
        
         // Get data rate for magnetometer channel
        
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::Property
// ----------------------------------------------------------------------------------
//
TSensrvProperty* CTCChannelTilt::Property( TSensrvPropertyId aId )
    {
    FUNC_LOG;
    
    TSensrvProperty* property = NULL;
    TInt count = iProperties.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iProperties[ i ].GetPropertyId() == aId )
            {
            property = &iProperties[ i ];
            break;
            }
        }
        
    return property;
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::PropertyChanged
// ----------------------------------------------------------------------------------
//
void CTCChannelTilt::PropertyChanged( CSensrvChannel& aChannel, 
    const TSensrvProperty& aChangedProperty )
    {
    FUNC_LOG;
    if( aChangedProperty.GetPropertyId() == KSensrvPropIdDataRate &&
      ( ( aChannel.GetChannelInfo().iChannelDataTypeId
        == KSensrvChannelTypeIdAccelerometerXYZAxisData ) ||
      ( aChannel.GetChannelInfo().iChannelDataTypeId
        == KSensrvChannelTypeIdMagnetometerXYZAxisData ) ) )
        {
        TInt valueOfProp( 0 );
        aChangedProperty.GetValue( valueOfProp );
        if( ( valueOfProp < iMaximumDataRate )
         && ( valueOfProp < KMaximumDataRate ) && ( valueOfProp != 0 ) )
            {
            // Inform current state that property has changed
            iMaximumDataRate = valueOfProp;
            TSensrvProperty property;
            TInt count = iProperties.Count();
            for( TInt i = 0; i < count; i++ )
                {
                if( iProperties[ i ].GetPropertyId() == KSensrvPropIdDataRate )
                    {
                    property = iProperties[ i ];
                    break;
                    }
                }
            property.SetValue( valueOfProp );
            TRAP_IGNORE( SetPropertyL( iChannelInfo.iChannelId, property ) );
            }
        }
    if( aChangedProperty.GetPropertyId() == KSensrvPropCalibrationLevel &&
      ( aChannel.GetChannelInfo().iChannelDataTypeId
        == KSensrvChannelTypeIdMagnetometerXYZAxisData ) )
        {
        iIsSetFromListened = ETrue;
        TInt valueOfProp( 0 );
        aChangedProperty.GetValue( valueOfProp );
        // Inform current state that property has changed
        TSensrvProperty property;
        TInt count = iProperties.Count();
        for( TInt i = 0; i < count; i++ )
            {
            if( iProperties[ i ].GetPropertyId() == KSensrvPropCalibrationLevel )
                {
                property = iProperties[ i ];
                break;
                }
            }
        property.SetValue( valueOfProp );
        TRAP_IGNORE( SetPropertyL( iChannelInfo.iChannelId, property ) );
        }
    }

// ----------------------------------------------------------------------------------
// CTCChannelTilt::PropertyChanged
// ----------------------------------------------------------------------------------
//
#ifdef _DEBUG
void CTCChannelTilt::PropertyError( CSensrvChannel& /*aChannel*/, 
    TSensrvErrorSeverity aError )
#else
// not used in UREL builds
void CTCChannelTilt::PropertyError( CSensrvChannel& /*aChannel*/, 
    TSensrvErrorSeverity /*aError*/ )
#endif
    {
    FUNC_LOG;
    INFO_1( "Property error [%d]", aError );
    }

// -----------------------------------------------------------------------------
// CTCChannelTilt::SetPropertySuccessIndicationChanged
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CTCChannelTilt::SetPropertySuccessIndicationChanged( TSetPropertySuccessIndicator aIndication )
#else
// not used in UREL builds
void CTCChannelTilt::SetPropertySuccessIndicationChanged( TSetPropertySuccessIndicator /*aIndication*/ )
#endif
    {
    FUNC_LOG;
    
    INFO_1( "TSetPropertySuccessIndicator [%d]", aIndication );
    }

// -----------------------------------------------------------------------------
// CTCChannelTilt::GetPropertyListenerInterfaceL
// -----------------------------------------------------------------------------
//
void CTCChannelTilt::GetPropertyListenerInterfaceL( TUid /*aInterfaceUid*/, 
    TAny*& aInterface )
    {
    FUNC_LOG;
    aInterface = NULL;
    }

// End of File
