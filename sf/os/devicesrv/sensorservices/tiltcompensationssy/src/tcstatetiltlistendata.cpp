/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CTCStateTiltListenData class implementation.
*
*/
 

#include <sensrvchanneldatatypes.h>
#include <sensrvgeneralproperties.h>
#include <sensrvmagneticnorthsensor.h>
#include <centralrepository.h>

#include "tcstatetiltlistendata.h"
#include "tceventaccelerometer.h"
#include "tceventmagnetometer.h"
#include "tctransactionhandler.h"
#include "tcstatepool.h"
#include "tctransactionflushbuffer.h"
#include "tcutil.h"
#include "common.h"
#include "trace.h"
#include "tceventproperty.h"
#include "tcsendeventtimer.h"
#include "tcprivatecrkeys.h"

const TInt KTiltParamCount( 4 );


// -----------------------------------------------------------------------------
// CTCStateTiltListenData::PrepareArray
// -----------------------------------------------------------------------------
//
// Prepares array for item insertion
static void PrepareArray( RArray<TTiltXYZValue>& aArray, TInt aItemsToInsert, TInt aMaxItems )
    {
    FUNC_LOG;
    
    TInt count = aArray.Count();
    TInt i = count + aItemsToInsert;
    while( i-- > aMaxItems ) // Delete old items from end
        {
        aArray.Remove( --count );
        }
    }

// -----------------------------------------------------------------------------
// CTCStateTiltListenData::InsertArrayItem
// -----------------------------------------------------------------------------
//
// Insert array items
static void InsertArrayItem( RArray<TTiltXYZValue>& aArray, TInt aMaxArrayItems, const TTiltXYZValue& aItem )
    {
    FUNC_LOG;
    
    PrepareArray( aArray, 1, aMaxArrayItems);
    aArray.Insert( aItem, 0 );
    }

// -----------------------------------------------------------------------------
// CTCStateTiltListenData::Average
// -----------------------------------------------------------------------------
//
// Calculate average for array
static void Average( const RArray<TTiltXYZValue>& aArray, TInt16& aAveX, TInt16& aAveY, TInt16& aAveZ )
    {
    FUNC_LOG;
    
    TInt aveX = 0;
    TInt aveY = 0;
    TInt aveZ = 0;

    TInt count = aArray.Count();
    TInt i = 0;
    while( i < count )
        {
        const TTiltXYZValue& val = aArray[ i++ ];
        aveX += val.iX;
        aveY += val.iY;
        aveZ += val.iZ;
        }
    if( count )
        {
        aAveX = aveX / count;
        aAveY = aveY / count;
        aAveZ = aveZ / count;
        }
    }

CTCStateTiltListenData* CTCStateTiltListenData::NewL( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler )
    {
    FUNC_LOG;
    
    CTCStateTiltListenData* self = CTCStateTiltListenData::NewLC(
        aStatePool,
        aTransactionHandler );
    CleanupStack::Pop( self );
    
    return self;
    }

CTCStateTiltListenData* CTCStateTiltListenData::NewLC( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler )
    {
    FUNC_LOG;
    
    CTCStateTiltListenData* self = new( ELeave )CTCStateTiltListenData(
        aStatePool,
        aTransactionHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CTCStateTiltListenData::CTCStateTiltListenData( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler ):
    CTCStateTilt( CTCState::ETCStateListenData, aStatePool, aTransactionHandler )
    {
    FUNC_LOG;

    iPreviousTheta = 0;
    }

void CTCStateTiltListenData::ConstructL()
    {
    FUNC_LOG;
    
    // Load the tiltcompensation library dynamically.    
    TInt err( iTiltLib.Load( KTiltCompensationLib ) );    
    ERROR( err, "Failed to load compensation library!" );           
    // If the compensation library is not available, fail ssy loading as well.
    User::LeaveIfError( err );       
    
    TLibraryFunction libFunc = iTiltLib.Lookup( ECompensateWithParams );
    iCompensationFunc = reinterpret_cast<CompensateFuncWithParams >( libFunc );
    
    if ( !iCompensationFunc )
        {
        User::Leave( KErrNotFound );
        }
      
    // Read the tilt compensation parameters from the Cenrep.
    TReal k( 0.0 );
    TReal m( 0.0 );
    TReal tresholdAngle1( 0.0 );
    TReal tresholdAngle2( 0.0 );
    
    iCenrepObserver = CTCCenrepObserver::NewL( *this );
    CRepository& cenRep = iCenrepObserver->GetCenrepHandle();
    
    iParamsArray.ReserveL( KTiltParamCount );
    
    User::LeaveIfError( cenRep.Get( KTiltCompensationParameterK, k ) );
    iParamsArray.AppendL( k );
    User::LeaveIfError( cenRep.Get( KTiltCompensationParameterM, m ) );
    iParamsArray.AppendL( m );
    User::LeaveIfError( cenRep.Get( KTiltCompensationParameterTresholdAngle1, tresholdAngle1 ) );
    iParamsArray.AppendL( tresholdAngle1 );
    User::LeaveIfError( cenRep.Get( KTiltCompensationParameterTresholdAngle2, tresholdAngle2 ) );
    iParamsArray.AppendL( tresholdAngle2 );
    User::LeaveIfError( cenRep.Get( KTiltCompensationAccWinSize, iSizeOfAccArray ) );
    User::LeaveIfError( cenRep.Get( KTiltCompensationMagWinSize, iSizeOfMagArray ) );
    }

CTCStateTiltListenData::~CTCStateTiltListenData()
    {
    FUNC_LOG;

    if( iTimerToSend )
        {
        delete iTimerToSend;
        iTimerToSend = NULL;
        }
    iTiltLib.Close();
    iParamsArray.Close();
    delete iCenrepObserver;
    }
    
// METHODS

// ----------------------------------------------------------------------------------
// CTCStateTiltListenData::HandleEventL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltListenData::HandleEventL( TTCEventId aId, TTCEvent* aEvent )
    {
    FUNC_LOG;
    
    INFO_2( "Handling event [%S] in state [%S]",
        &TCUtil::EventIdAsDesC( aId ),
        &TCUtil::StateIdAsDesC( this ) );
    
    switch( aId )
        {
        case EEventIdHandleCloseChannel:
            {
            // Stop channel data and switch to closing state
            iTransactionHandler.ProcessTransactionL( ETCTransactionIdStopChannelData );
            iStatePool.ChangeStateL( CTCState::ETCStateClosing );

            // Reset internal state
            ResetState( ETrue );
            break;
            }
        case EEventIdHandleStopChannelData:
            {
            // Stop data listening from accelerometer and magnetometer
            iTransactionHandler.ProcessTransactionL( ETCTransactionIdStopChannelData );
            break;
            }
        case EEventIdHandleForceBufferFull:
            {
            // Set flag that we are forcing the buffer to be filled
            // Still, we need to get fresh values from accelerometer
            // and magnetometer before we can flush the buffer
            Set( EForceBufferFull );
            break;
            }
        case EEventIdHandleAccelerometer:
            {
            ASSERT_DEBUG_TRACE( aEvent, ENullPointer );
            
            // Set new accelerometer data
            TTCEventAccelerometer& event =
                static_cast<TTCEventAccelerometer&>( *aEvent );
            
            // Input data from accelometer
            iInput.iAccelerationVector.iX = event.iData.iAxisX * ( 16 );
            iInput.iAccelerationVector.iY = event.iData.iAxisY * ( 16 );
            iInput.iAccelerationVector.iZ = event.iData.iAxisZ * ( 16 );

            INFO_1( "Tilt compensation raw AccX: %d", iInput.iAccelerationVector.iX );
            INFO_1( "Tilt compensation raw AccY: %d", iInput.iAccelerationVector.iY );
            INFO_1( "Tilt compensation raw AccZ: %d", iInput.iAccelerationVector.iZ );
            
            InsertArrayItem( iArrayAcc, iSizeOfAccArray, TTiltXYZValue(  iInput.iAccelerationVector.iX, iInput.iAccelerationVector.iY, iInput.iAccelerationVector.iZ ) );
            
            Set( EAccelerometerData );
            break;
            }
        case EEventIdHandleMagnetometer:
            {
            ASSERT_DEBUG_TRACE( aEvent, ENullPointer );

            // Set new magnetometer data
            TTCEventMagnetometer& event = static_cast<TTCEventMagnetometer&>( *aEvent );
            
            // Input data from magnetometer
            iInput.iMagneticVector.iX = event.iData.iAxisXCalibrated * ( 5 );
            iInput.iMagneticVector.iY = event.iData.iAxisYCalibrated * ( 5 );
            iInput.iMagneticVector.iZ = event.iData.iAxisZCalibrated * ( 5 );
            
            INFO_1( "Tilt compensation raw MagX: %d", iInput.iMagneticVector.iX );
            INFO_1( "Tilt compensation raw MagY: %d", iInput.iMagneticVector.iY );
            INFO_1( "Tilt compensation raw MagZ: %d", iInput.iMagneticVector.iZ );
           
            InsertArrayItem( iArrayMag, iSizeOfMagArray, TTiltXYZValue( iInput.iMagneticVector.iX, iInput.iMagneticVector.iY, iInput.iMagneticVector.iZ ) );

            Set( EMagnetometerData );
            break;
            }
        case EEventIdHandlePropertyChanged:
            {
            ASSERT_DEBUG_TRACE( aEvent, ENullPointer );
            
            // Adjust sample rate to match current property values
            TTCEventProperty& property = static_cast<TTCEventProperty&>( *aEvent );
            if( property.iProperty.GetPropertyId() == KSensrvPropIdDataRate )
                {
                property.iProperty.GetValue( iHzSamplingRate );
                INFO_1( "Sampling rate changed to: %d", iHzSamplingRate );
                HandleStateEntryL();
                }
            break;
            }
        case EEventIdHandleTimerExpriredSend:
            {
            // Timer has expired, send data if the buffer is filled
            CompleteEventL();
            break;
            }
        default:
            {
            INFO_2( "Event [%S] not handled in [%S] state",
                &TCUtil::EventIdAsDesC( aId ),
                &TCUtil::StateIdAsDesC( this ) );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltListenData::HandleTransactionCompletedL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltListenData::HandleTransactionCompletedL( TTCTransactionId aTransactionId,
    TInt /*aError */)
    {
    FUNC_LOG;

    INFO_2( "Transaction [%S] complete message recieved in [%S] state",
        &TCUtil::TransactionIdAsDesC( aTransactionId ),
        &TCUtil::StateIdAsDesC( this ) );
        
    switch( aTransactionId )
        {
        case ETCTransactionIdStopChannelData:
            {
            // Change state to open
            iStatePool.ChangeStateL( CTCState::ETCStateOpen );
            break;
            }
        default:
            {
            INFO_2( "Transaction [%S] complete message not handled in [%S] state",
                &TCUtil::TransactionIdAsDesC( aTransactionId ),
                &TCUtil::StateIdAsDesC( this ) );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltListenData::HandleStateEntryL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltListenData::HandleStateEntryL()
    {
    FUNC_LOG;
    if( iHzSamplingRate == 0 )
        {
        RSensrvPropertyList list = iTransactionHandler.Properties();
        for( TInt i = 0; i != list.Count(); i++ )
            {
            if( list[ i ].GetPropertyId() == KSensrvPropIdDataRate )
                {
                list[ i ].GetValue( iHzSamplingRate );
                INFO_1( "Sampling rate: %d", iHzSamplingRate );                
                }
            }
        }
    iRequestedDataCount = iTransactionHandler.DataCount();
    if( !iTimerToSend )
        {
        iTimerToSend = CTCSendEventTimer::NewL( iHzSamplingRate, *this );
        }
	iTimerToSend->Start(iHzSamplingRate);
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltListenData::HandleStateExitL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltListenData::HandleStateExitL()
    {
    FUNC_LOG;
    
    if( iTimerToSend )
        {
		iTimerToSend->Cancel();
        }
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltListenData::CalculateTilt
// ----------------------------------------------------------------------------------
//
TBool CTCStateTiltListenData::CalculateTilt()
    {
    FUNC_LOG;
    
    TBool flush( EFalse );
    if( IsSet( EAccelerometerData ) && IsSet( EMagnetometerData ) )
        {
        TTiltCompensationOutput output;
        
        Average ( iArrayAcc, iInput.iAccelerationVector.iX, iInput.iAccelerationVector.iY, iInput.iAccelerationVector.iZ );
        Average ( iArrayMag, iInput.iMagneticVector.iX, iInput.iMagneticVector.iY, iInput.iMagneticVector.iZ );

        INFO_1( "Tilt compensation average MagX: %d", iInput.iMagneticVector.iX );
        INFO_1( "Tilt compensation average MagY: %d", iInput.iMagneticVector.iY );
        INFO_1( "Tilt compensation average MagZ: %d", iInput.iMagneticVector.iZ );
        INFO_1( "Tilt compensation average AccX: %d", iInput.iAccelerationVector.iX );
        INFO_1( "Tilt compensation average AccY: %d", iInput.iAccelerationVector.iY );
        INFO_1( "Tilt compensation average AccZ: %d", iInput.iAccelerationVector.iZ );
        INFO_1( "Tilt compensation iSizeOfAccArray: %d", iSizeOfAccArray );
        INFO_1( "Tilt compensation iSizeOfMagArray: %d", iSizeOfMagArray );
        INFO_1( "Tilt compensation iPreviousTheta: %d", iPreviousTheta );
        INFO_2( "Tilt compensation params: k[%f], m[%f]", iParamsArray[0], iParamsArray[1] );
        INFO_2( "Tilt compensation params: tresholdAng1[%f], tresholdAng2[%f]",
                iParamsArray[2], iParamsArray[3] );
        
        TInt err = iCompensationFunc( iInput, output, iPreviousTheta, iParamsArray );
                    
        iPreviousTheta = output.iTheta;

        ERROR( err, "Tilt compensation failed" );
        if( err == KErrNone )
            {
            INFO_1( "Tilt compensation succesfull. Tilt angle: %d", output.iTheta );
            
            // Write data into data buffer
            TUint8* buffer = iTransactionHandler.DataBuffer();
            if( buffer && ( iWriteCount < iTransactionHandler.DataCount() ) )
                {
                // Move pointer if needed
                buffer += sizeof( TSensrvMagneticNorthData ) * iWriteCount;
                TSensrvMagneticNorthData* tiltData
                  = reinterpret_cast<TSensrvMagneticNorthData*>( buffer );
                iTime.HomeTime();
                tiltData->iTimeStamp = iTime;
                tiltData->iAngleFromMagneticNorth = output.iTheta;
                if( ++iWriteCount == iTransactionHandler.DataCount() ||
                    IsSet( EForceBufferFull ) )
                    {
                    flush = ETrue;
                    }
                    
                // Reset state but not write count
                ResetState( EFalse );
                }
            else
                {
                ERROR_GEN( "Tilt data buffer empty" );
                }
            }
        }
        
    return flush;
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltListenData::CompleteEventL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltListenData::CompleteEventL()
    {
    FUNC_LOG;

    if( CalculateTilt() )
        {
        // If we have received enough data, flush buffer and switch
        // to buffer full state
        TTCTransactionFlushBuffer transaction( iWriteCount );
        iTransactionHandler.ProcessTransactionL( transaction.Id(), &transaction );
        iStatePool.ChangeStateL( CTCState::ETCStateBufferFull );
        
        // Reset state
        ResetState( ETrue );
        }
    else
    	{
    	if( iTimerToSend )
    		iTimerToSend->Start(iHzSamplingRate);
    	}
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltListenData::ResetState
// ----------------------------------------------------------------------------------
//
void CTCStateTiltListenData::ResetState( TBool aWriteCountIncluded )
    {
    FUNC_LOG;
    
    // Reset internal flags
    Clear();
    
    // Reset write count
    if( aWriteCountIncluded )
        {
        iWriteCount = 0;
        }

    // Reset tilt input
    Mem::FillZ( &iInput, sizeof( iInput ) );
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltListenData::SendDataAfterTimer
// ----------------------------------------------------------------------------------
//
void CTCStateTiltListenData::SendDataAfterTimer()
    {
    TRAP_IGNORE( HandleEventL( EEventIdHandleTimerExpriredSend, NULL ) );
    }

// -----------------------------------------------------------------------------
// CTCStateTiltListenData::CenrepValueChanged
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CTCStateTiltListenData::CenrepValueChanged( TUint32 aKey, CRepository& aRepository )
#else
void CTCStateTiltListenData::CenrepValueChanged( TUint32 /*aKey*/, CRepository& aRepository )
#endif
    {
    FUNC_LOG;

    INFO_1( "CTCStateTiltListenData::CenrepValueChanged: aKey %d", aKey );

    // Update all keys to iParamsArray, because it is documented for
    // CCenRepNotifyHandler as follows:
    // "It is not guaranteed that a notification will be received
    // for all keys, if multiple keys are changed in rapid succession"
    TInt index( 0 );
    GetCenrepValue( aRepository, KTiltCompensationParameterK, index );
    GetCenrepValue( aRepository, KTiltCompensationParameterM, ++index );
    GetCenrepValue( aRepository, KTiltCompensationParameterTresholdAngle1, 
        ++index );
    GetCenrepValue( aRepository, KTiltCompensationParameterTresholdAngle2, 
        ++index );
    }

// -----------------------------------------------------------------------------
// CTCStateTiltListenData::GetCenrepValue
// -----------------------------------------------------------------------------
//
void CTCStateTiltListenData::GetCenrepValue( CRepository& aCenRep, 
    TUint32 aKey, TInt aIndex )
    {
    FUNC_LOG;

    TReal value;
    
    TInt err = aCenRep.Get( aKey, value );
    ERROR_1( err, "CTCStateTiltListenData::GetCenrepValue: get err %d", err );
            
    if( err == KErrNone && ( aIndex >= 0 && aIndex < iParamsArray.Count() ) )
        {
        iParamsArray[aIndex] = value;
        }
    }
    
// End of File
