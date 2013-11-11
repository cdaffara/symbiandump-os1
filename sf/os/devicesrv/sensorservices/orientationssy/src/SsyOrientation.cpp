/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Orientation class of Orientation SSY
*
*/


// Includes
#include <e32math.h>
#include <sensrvchannelfinder.h>
#include <sensrvgeneralproperties.h>
#include <sensrvaccelerometerproperties.h>
#include <sensordatacompensator.h>
#include <centralrepository.h>
#include "SsyOrientation.h"
#include "Ssyeventtimer.h"
#include "SsyConfiguration.h"               // configuration
#include "SsyTrace.h"
#include "OrientationConfiguration.h"

// Constants

/**
 *  Constants for degrees
 */
const TInt K45Degrees  = 45;
const TInt K90Degrees  = 90;
const TInt K135Degrees = 135;
const TInt K180Degrees = 180;
const TInt K225Degrees = 225;
const TInt K270Degrees = 270;
const TInt K315Degrees = 315;
const TInt K360Degrees = 360;
const TReal KRad2degrees = 57.3; // accurate enough for our purposes. 180° / pi
const TReal KVariationFactor = 0.85;
const TInt KOneSecond = 1;

#ifdef _DEBUG
// ----------------------------------------------------------------------------------
// TraceOrientation()
// Helper function for tracing orientation data.
// ----------------------------------------------------------------------------------
//
void TraceOrientation( const TSensrvOrientationData::TSensrvDeviceOrientation aOrientation,
                               const TInt aXRotation,
                               const TInt aYRotation,
                               const TInt aZRotation )
    {
    switch ( aOrientation )
        {
        case TSensrvOrientationData::EOrientationDisplayUp:
            SSY_TRACE( EExtended, "ORIENTATIONSSY:EOrientationDisplayUp" );
            break;
        case TSensrvOrientationData::EOrientationDisplayDown:
            SSY_TRACE( EExtended, "ORIENTATIONSSY:EOrientationDisplayDown" );
            break;
        case TSensrvOrientationData::EOrientationDisplayLeftUp:
            SSY_TRACE( EExtended, "ORIENTATIONSSY:EOrientationDisplayLeftUp" );
            break;
        case TSensrvOrientationData::EOrientationDisplayRightUp:
            SSY_TRACE( EExtended, "ORIENTATIONSSY:EOrientationDisplayRightUp" );
            break;
        case TSensrvOrientationData::EOrientationDisplayUpwards:
            SSY_TRACE( EExtended, "ORIENTATIONSSY:EOrientationDisplayUpwards" );
            break;
        case TSensrvOrientationData::EOrientationDisplayDownwards:
            SSY_TRACE( EExtended, "ORIENTATIONSSY:EOrientationDisplayDownwards" );
            break;
        default:
            SSY_TRACE( EExtended, "ORIENTATIONSSY:EOrientationUndefined" );
            break;
        }

    SSY_TRACE( EExtended, "ORIENTATIONSSY:aXRotation=%d", aXRotation );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:aYRotation=%d", aYRotation );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:aZRotation=%d", aZRotation );
    }
#endif

// ----------------------------------------------------------------------------------
// CSSYOrientation::CSSYOrientation()
// ----------------------------------------------------------------------------------
// 
CSSYOrientation::CSSYOrientation( const TSensrvChannelTypeId aChannelType ) :
    iChannelType( aChannelType ),
    iChannel( NULL ),
    iChannelFinder( NULL ),
    iSensorChannel( NULL ),
    iEventTimer( NULL ),
    iFirstDataReceived( EFalse )
    {
    SSY_TRACE_IN();
    
    iConfigForCurrentState.iOrientationState = TSensrvOrientationData::EOrientationUndefined;
     
    iTimerState = CSSYOrientation::EUninitialized;
    
    iOrientationEvent.iDeviceOrientation = 
        TSensrvOrientationData::EOrientationUndefined;
        
    iCurrentOrientationState.iDeviceOrientation = 
        TSensrvOrientationData::EOrientationUndefined;

    iRotationEvent.iDeviceRotationAboutZAxis = 
        TSensrvRotationData::KSensrvRotationUndefined;

    iRotationEvent.iDeviceRotationAboutXAxis = 
        TSensrvRotationData::KSensrvRotationUndefined;

    iRotationEvent.iDeviceRotationAboutYAxis = 
        TSensrvRotationData::TSensrvRotationData::KSensrvRotationUndefined;

    iOrientationArray.Reset();
    
    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::~CSSYOrientation()
// ----------------------------------------------------------------------------------
// 
CSSYOrientation::~CSSYOrientation()
    {
    SSY_TRACE_IN();
    
    if( iChannelFinder )
        {
        delete iChannelFinder;
        iChannelFinder = NULL;
        }

    if( iEventTimer )
        {
        delete iEventTimer;
        iEventTimer = NULL;
        }

    if( iSensorChannel )
        {
        delete iSensorChannel;
        iSensorChannel = NULL;
        }
#ifdef AUTO_ORIENTAION_TEST
    if( iCRListener )
        {
        delete iCRListener;
        iCRListener = NULL;
        }
#endif
    if( iRepository )
        {
        delete iRepository;
        iRepository = NULL;
        }

    delete iCompensator;

    iChannel = NULL;

    iChannelInfoList.Reset();
    iOrientationArray.Reset();
    iConfigArray.Reset();

    iChannelInfoList.Close();
    iOrientationArray.Close();
    iConfigArray.Close();

    SSY_TRACE_OUT();
    }

//------------------------
//
//

void CSSYOrientation::ConstructL()
    {
    iRepository = CRepository::NewL( KCRUidOrientationSsySettings );
     User::LeaveIfError(CreateConfigurations());
    
#ifdef AUTO_ORIENTAION_TEST
    iCRListener = CSsyOrientationCRListener::NewL( *this, iRepository );
#endif
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::DataReceived()
// ----------------------------------------------------------------------------------
//
#ifdef _DEBUG
void CSSYOrientation::DataReceived( CSensrvChannel& aChannel, 
                                    TInt aCount, 
                                    TInt aDataLost )
#else
// aCount and aDataLost are not used in UREL builds
void CSSYOrientation::DataReceived( CSensrvChannel& aChannel, 
                                    TInt /*aCount*/, 
                                    TInt /*aDataLost*/ )
#endif
    {
    SSY_TRACE_IN();

    SSY_TRACE( EExtended, "ORIENTATIONSSY:aChannel: %d", aChannel.GetChannelInfo().iChannelId );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:aCount: %d", aCount );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:aDataLost: %d", aDataLost );

    ASSERT_DEBUG_SSY( iSensorChannel == &aChannel );

    
    TBool rotationChanged( EFalse );
    
	if ( !iFirstDataReceived )
		{
        // No need to come here anymore
        iFirstDataReceived = ETrue;
        }
    TPckgBuf<TSensrvAccelerometerAxisData> dataBuf;
    TInt err = iSensorChannel->GetData( dataBuf );

    if ( KErrNone != err )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY: GetData() failed: %d", err );
        return;
        }
        
    if ( !iCompensator )
        {        
        TSensrvChannelInfo channelInfo = iSensorChannel->GetChannelInfo();
        TSensrvChannelDataTypeId dataTypeId( channelInfo.iChannelDataTypeId );
        TRAP( err, iCompensator = CSensorDataCompensator::NewL( dataTypeId, 
                                        ESensorCompensateDeviceOrientation ) );
        if ( err != KErrNone )
            {
            SSY_TRACE( EError, "ORIENTATIONSSY: Unable to create compensator, data won't be compensated! err: %d", err );
            }
        }

    if ( iCompensator )
        {
        err = iCompensator->Compensate( dataBuf );
        if ( err != KErrNone )
            {
            SSY_TRACE( EError, "ORIENTATIONSSY: Data compensation failed! err: %d", err );
            }
        }

    TSensrvAccelerometerAxisData data = dataBuf();
    
    TInt zenith( 0 );
    TInt azimuth( 0 );
    err = CalculateSphericalPolarCoordinates(
         data.iAxisX,
         data.iAxisY,
         data.iAxisZ,
         zenith,
         azimuth );
    
    TSensrvOrientationData::TSensrvDeviceOrientation orientation = 
            CalculateDeviceOrientation( zenith, azimuth );
    
    // positive device rotation about the x-axis
    TInt xRotation = CalculateDeviceRotationInDegrees( 
        data.iAxisZ, data.iAxisY, ERotationAboutXaxis );
    
    // positive device rotation about the y-axis
    TInt yRotation = CalculateDeviceRotationInDegrees( 
        data.iAxisZ, data.iAxisX, ERotationAboutYaxis );
    
    // positive device rotation about the z-axis
    TInt zRotation = CalculateDeviceRotationInDegrees( 
        data.iAxisX, data.iAxisY, ERotationAboutZaxis );
    
    iOrientationEvent.iTimeStamp = data.iTimeStamp;
    iCurrentOrientationState.iTimeStamp = data.iTimeStamp;
    
    // In initial state do this
    if( ( iCurrentOrientationState.iDeviceOrientation == 
          TSensrvOrientationData::EOrientationUndefined )
     && ( iChannelType == KSensrvChannelTypeIdOrientationData ) 
     && ( iTimerState != CSSYOrientation::ERunning )
     && ( err == KErrNone ) )
        {
        GetConfigurationForState( orientation, iConfigForCurrentState );
        iOrientationEvent.iDeviceOrientation = orientation;
        iCurrentOrientationState.iDeviceOrientation = orientation;
        
        
        iOrientationEvent.iTimeStamp = data.iTimeStamp;
        iCurrentOrientationState.iTimeStamp = data.iTimeStamp;
        
        iRotationEvent.iTimeStamp = data.iTimeStamp;
        
        if ( iChannel && iChannel->AppendData( iOrientationEvent, iRotationEvent ) )
            {
            SSY_TRACE( EExtended, "ORIENTATIONSSY: First time data received, set first orientation=%d", orientation );
            iChannel->IssueRequest();
            iChannel = NULL;
            }
        }
    
    // Set rotation to rotation event
    if ( iChannelType == KSensrvChannelTypeIdRotationData )
        {
        if ( iRotationEvent.iDeviceRotationAboutXAxis != xRotation )
            {
            iRotationEvent.iDeviceRotationAboutXAxis = xRotation;
            rotationChanged = ETrue;
            }

        if ( iRotationEvent.iDeviceRotationAboutYAxis != yRotation )
            {
            iRotationEvent.iDeviceRotationAboutYAxis = yRotation;
            rotationChanged = ETrue;
            }

        if ( iRotationEvent.iDeviceRotationAboutZAxis != zRotation )
            {
            iRotationEvent.iDeviceRotationAboutZAxis = zRotation;
            rotationChanged = ETrue;
            }
        }
    
    if( ( iTimerState == CSSYOrientation::ERunning ) 
     && ( orientation != TSensrvOrientationData::EOrientationUndefined ) )
        {
        if( err == KErrNone )
            {
            TRAP_IGNORE( iOrientationArray.AppendL( (TInt)orientation ) );
            }
        }
    
    if( ( orientation != iCurrentOrientationState.iDeviceOrientation )
     && ( orientation != TSensrvOrientationData::EOrientationUndefined )
     && ( iTimerState != CSSYOrientation::ERunning )
     && ( err == KErrNone ) )
        {
        TInt timer = iConfigForCurrentState.GetTimerToOrientation( orientation );
        if( timer == 0 )
            {
            /**
            * Don't set timer to zero seconds, set it rather to one
            * milliseconds.
            */
            timer = KOneSecond;
            }
        SSY_TRACE( EExtended, "ORIENTATIONSSY:Set timer to %d ms", timer );
        TRAP_IGNORE( iOrientationArray.AppendL( (TInt)orientation ) );
        if( !iEventTimer )
            {
            TRAP_IGNORE( iEventTimer = CSsyOrientationEventTimer::NewL( timer, *this ) );
            }
        iTimerState = CSSYOrientation::ERunning;
        }
    
    // append only if somebody is listening
    if ( iChannel )
        {
#ifdef _DEBUG
        SSY_TRACE( EExtended, "ORIENTATIONSSY:data.iAxisX=%d", data.iAxisX );
        SSY_TRACE( EExtended, "ORIENTATIONSSY:data.iAxisY=%d", data.iAxisY );
        SSY_TRACE( EExtended, "ORIENTATIONSSY:data.iAxisZ=%d", data.iAxisZ );        

        TraceOrientation( iOrientationEvent.iDeviceOrientation, 
                          iRotationEvent.iDeviceRotationAboutXAxis, 
                          iRotationEvent.iDeviceRotationAboutYAxis, 
                          iRotationEvent.iDeviceRotationAboutZAxis );
#endif
        // If rotation channel is listened append data to rotation channel
        if ( ( iChannelType == KSensrvChannelTypeIdRotationData ) && rotationChanged )
            {
            iOrientationEvent.iTimeStamp = data.iTimeStamp;
            iRotationEvent.iTimeStamp = data.iTimeStamp;
            if ( iChannel->AppendData( iOrientationEvent, iRotationEvent ) )
                {
                iChannel->IssueRequest();
                iChannel = NULL;
                }
            }
        }
    
    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::CalculateDeviceRotationInDegrees()
// ----------------------------------------------------------------------------------
// 
TInt CSSYOrientation::CalculateDeviceRotationInDegrees( 
    const TInt aAxis1, 
    const TInt aAxis2,
    const TRotation aRotation )
    {
    SSY_TRACE_IN();

    TInt devicetilt( TSensrvRotationData::KSensrvRotationUndefined );
    
    if ( aAxis2 != 0 )
        {
        TReal alpha    = 0;
        TReal relation = (TReal)Abs(aAxis1) / (TReal)Abs(aAxis2);

        // calculate the angle
        Math::ATan( alpha, relation );

        // change radians to degrees
        alpha = alpha * KRad2degrees;

        switch ( aRotation )
            {
            case ERotationAboutXaxis:
                {
                if ( aAxis1 > 0 && aAxis2 > 0 )
                    {
                    // 0-90
                    }
                else if ( aAxis1 > 0 && aAxis2 < 0 )
                    {
                    // 90-180
                    alpha = K90Degrees + ( K90Degrees - alpha );
                    }
                else if ( aAxis1 < 0 && aAxis2 < 0 )
                    {
                    // 180-270
                    alpha += K180Degrees;
                    }
                else
                    {
                    // 270-360
                    alpha = K270Degrees + ( K90Degrees - alpha );
                    }
                break;
                }
// positive device rotation about the x-axis
    
            case ERotationAboutYaxis:
                {
                if ( aAxis1 > 0 && aAxis2 > 0 )
                    {
                    // 90-180
                    alpha += K90Degrees;
                    }
                else if ( aAxis1 > 0 && aAxis2 < 0 )
                    {
                    // 180-270
                    alpha = K270Degrees - alpha;
                    }
                else if ( aAxis1 < 0 && aAxis2 < 0 )
                    {
                    // 270-360
                    alpha += K270Degrees;
                    }
                else
                    {
                    // 0-90
                    alpha = K90Degrees - alpha;
                    }
                break;
                }
            case ERotationAboutZaxis:
                {
                if ( aAxis1 > 0 && aAxis2 > 0 )
                    {
                    // 270-360
                    alpha = K360Degrees - alpha;
                    }
                else if ( aAxis1 > 0 && aAxis2 < 0 )
                    {
                    // 180-270
                    alpha += K180Degrees;
                    }
                else if ( aAxis1 < 0 && aAxis2 < 0 )
                    {
                    // 90-180
                    alpha = K180Degrees - alpha;
                    }
                else
                    {
                    // 0-90
                    }
                break;
                }
            default:
                {
                SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Unknown rotation!" );
                }
            }

        // truncate to integer
        devicetilt = (TInt)alpha;

        // round the result according to KRotationResolutionInDegrees
        devicetilt += ( KRotationResolutionInDegrees >> 1 );
        devicetilt /= KRotationResolutionInDegrees;
        devicetilt *= KRotationResolutionInDegrees;
        devicetilt %= K360Degrees;
        }
  
    SSY_TRACE_OUT();

    return devicetilt;
    }
  
/**
* New implementation
*/
// ----------------------------------------------------------------------------------
// CSSYOrientation::CalculateDeviceOrientation()
// ----------------------------------------------------------------------------------
// 
TSensrvOrientationData::TSensrvDeviceOrientation CSSYOrientation::CalculateDeviceOrientation( 
    const TInt aZenithAngle,
    const TInt aAzimuthAngle )
    {
    SSY_TRACE_IN();

    TSensrvOrientationData::TSensrvDeviceOrientation orientation = 
                                TSensrvOrientationData::EOrientationUndefined;
    
    /**
    * The corrections are to different states are gotten from configurations
    */
    TInt correctionDisplayUp( iConfigForCurrentState.iOrientationDisplayUp.iAngle );
    TInt correctionDisplayDown( iConfigForCurrentState.iOrientationDisplayDown.iAngle );
    TInt correctionDisplayDownwards( iConfigForCurrentState.iOrientationDisplayDownwards.iAngle );
    TInt correctionDisplayRightUp( iConfigForCurrentState.iOrientationDisplayRightUp.iAngle );
    TInt correctionDisplayUpwards( iConfigForCurrentState.iOrientationDisplayUpwards.iAngle );
    TInt correctionDisplayLeftUp( iConfigForCurrentState.iOrientationDisplayLeftUp.iAngle );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation iConfigForCurrentState.iOrientationDisplayUp.iConfigForCurrentState.iOrientationDisplayUp.iAngle=%d", iConfigForCurrentState.iOrientationDisplayUp.iAngle );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation iConfigForCurrentState.iOrientationDisplayUp.iConfigForCurrentState.iOrientationDisplayDown.iAngle=%d", iConfigForCurrentState.iOrientationDisplayDown.iAngle );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation iConfigForCurrentState.iOrientationDisplayUp.iConfigForCurrentState.iOrientationDisplayDownwards.iAngle=%d", iConfigForCurrentState.iOrientationDisplayDownwards.iAngle );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation iConfigForCurrentState.iOrientationDisplayUp.iConfigForCurrentState.iOrientationDisplayRightUp.iAngle=%d", iConfigForCurrentState.iOrientationDisplayRightUp.iAngle );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation iConfigForCurrentState.iOrientationDisplayUp.iConfigForCurrentState.iOrientationDisplayUpwards.iAngle=%d", iConfigForCurrentState.iOrientationDisplayUpwards.iAngle );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation iConfigForCurrentState.iOrientationDisplayUp.iConfigForCurrentState.iOrientationDisplayLeftUp.iAngle=%d", iConfigForCurrentState.iOrientationDisplayLeftUp.iAngle );
    
    /**
    * The current state needs to be compenstated in some cases.
    * This is done whenever the current state is diminished from 45 degrees.
    * This compenstation is saved to the correctionFromSelectedState.
    */

    
    /**
    * Total correction contains the compensation from the configurations
    */
    TInt totalCorrection( K45Degrees - correctionDisplayUpwards );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayUpwards totalCorrection=%d", totalCorrection );
    TInt totalCorrection1( 0 );
    TInt totalCorrection2( 0 );
    TInt totalCorrection3( 0 );
    
    /**
    * If the acceleration vector points upwards the zenith is smaller than
    * 45 + correction
    */
    if( aZenithAngle < totalCorrection )
        {
        orientation = TSensrvOrientationData::EOrientationDisplayUpwards;
        SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayUpwards=%d", orientation );
        }
    
    totalCorrection = K135Degrees + correctionDisplayDownwards;
    

    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation aZenithAngle=%d", aZenithAngle );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayDownwards totalCorrection=%d", totalCorrection );
    
    /**
    * If the acceleration vector points downwards the zenith angle is bigger
    * 135 + correction.
    */
    if( aZenithAngle > totalCorrection )
        {
        orientation = TSensrvOrientationData::EOrientationDisplayDownwards;
        SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayDownwards=%d", orientation );
        }
    
    if(iCurrentOrientationState.iDeviceOrientation
        == TSensrvOrientationData::EOrientationDisplayUpwards ||
        iCurrentOrientationState.iDeviceOrientation
                == TSensrvOrientationData::EOrientationDisplayDownwards)
        {
        totalCorrection = K45Degrees - correctionDisplayRightUp;
        SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayRightUp totalCorrection=%d", totalCorrection );
        totalCorrection1 = K135Degrees + correctionDisplayRightUp;
        SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayRightUp totalCorrection1=%d", totalCorrection1 );
        }
    else
        {
        totalCorrection = K45Degrees - correctionDisplayUpwards;
        SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayRightUp totalCorrection=%d", totalCorrection );
        totalCorrection1 = K135Degrees + correctionDisplayDownwards;
        SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayRightUp totalCorrection1=%d", totalCorrection1 );
        }
    totalCorrection2 = K45Degrees - correctionDisplayRightUp;
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayRightUp totalCorrection2=%d", totalCorrection2 );
    totalCorrection3 = K315Degrees + correctionDisplayRightUp;
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayRightUp totalCorrection3=%d", totalCorrection3 );
    
    /**
    * If the acceleration vector points to the midlle the zenith is between
    * 45 and 135 degrees + correction and azimuth angle is between 45 and
    * 315 degrees + correction.
    */
    if( ( aZenithAngle >= totalCorrection )
     && ( aZenithAngle <= totalCorrection1 )
     && ( ( aAzimuthAngle <= totalCorrection2 ) || ( aAzimuthAngle >= totalCorrection3 ) ) )
        {
        // Azimuth 315 - 45
        orientation = TSensrvOrientationData::EOrientationDisplayRightUp;
        SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayRightUp=%d", orientation );
        }
    
    if(iCurrentOrientationState.iDeviceOrientation
        == TSensrvOrientationData::EOrientationDisplayUpwards ||
        iCurrentOrientationState.iDeviceOrientation
                == TSensrvOrientationData::EOrientationDisplayDownwards)
        {
        totalCorrection = K45Degrees - correctionDisplayUp;
        totalCorrection1 = K135Degrees + correctionDisplayUp;
        }
    else
        {
        totalCorrection = K45Degrees - correctionDisplayUpwards;
        totalCorrection1 = K135Degrees + correctionDisplayDownwards;
        }
    totalCorrection2 = K45Degrees - correctionDisplayUp;
    totalCorrection3 = K135Degrees + correctionDisplayUp;
    /**
    * If the acceleration vector points to the midlle the zenith is between
    * 45 and 135 degrees + correction and azimuth angle is between 45 and
    * 135 degrees + correction.
    */
    if( IsOrientation( aZenithAngle, aAzimuthAngle, totalCorrection,
        totalCorrection1, totalCorrection2, totalCorrection3 ) )
        {
        // Azimuth 45 - 135
        orientation = TSensrvOrientationData::EOrientationDisplayUp;
        SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayUp=%d", orientation );
        }

    if(iCurrentOrientationState.iDeviceOrientation
        == TSensrvOrientationData::EOrientationDisplayUpwards ||
        iCurrentOrientationState.iDeviceOrientation
                == TSensrvOrientationData::EOrientationDisplayDownwards)
        {
        totalCorrection = K45Degrees + correctionDisplayLeftUp;
        totalCorrection1 = K135Degrees - correctionDisplayLeftUp;
        }
    else
        {
        totalCorrection = K45Degrees - correctionDisplayUpwards;
        totalCorrection1 = K135Degrees + correctionDisplayDownwards;
        }
    totalCorrection2 = K135Degrees + correctionDisplayLeftUp;
    totalCorrection3 = K225Degrees - correctionDisplayLeftUp;
    /**
    * If the acceleration vector points to the midlle the zenith is between
    * 135 and 225 degrees + correction and azimuth angle is between 45 and
    * 135 degrees + correction.
    */
    if( IsOrientation( aZenithAngle, aAzimuthAngle, totalCorrection,
        totalCorrection1, totalCorrection2, totalCorrection3 ) )
        {
        // Azimuth 135 - 225
        orientation = TSensrvOrientationData::EOrientationDisplayLeftUp;
        SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayLeftUp=%d", orientation );
        }
    
    totalCorrection = K45Degrees - correctionDisplayUpwards;
    totalCorrection1 = K135Degrees + correctionDisplayDownwards;
    totalCorrection2 = K225Degrees + correctionDisplayDown;
    totalCorrection3 = K315Degrees - correctionDisplayDown;
    /**
    * If the acceleration vector points to the midlle the zenith is between
    * 225 and 315 degrees + correction and azimuth angle is between 45 and
    * 135 degrees + correction.
    */
    if( IsOrientation( aZenithAngle, aAzimuthAngle, totalCorrection,
        totalCorrection1, totalCorrection2, totalCorrection3 ) )
        {
        // Azimuth 225 - 315
        orientation = TSensrvOrientationData::EOrientationDisplayDown;
        SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation EOrientationDisplayDown=%d", orientation );
        }
    
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation orientation=%d", orientation );
    
    SSY_TRACE_OUT();

    return orientation;
    }

TBool CSSYOrientation::IsOrientation(
    const TInt aZenithAngle,
    const TInt aAzimuthAngle,
    TInt aCorrectionToState,
    TInt aCorrectionToState1,
    TInt aCorrectionToState2,
    TInt aCorrectionToState3 )
    {
    TBool retVal( EFalse );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation aZenithAngle=%d", aZenithAngle );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation aZenithAngle=%d", aAzimuthAngle );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation aZenithAngle=%d", aCorrectionToState );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation aZenithAngle=%d", aCorrectionToState1 );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation aZenithAngle=%d", aCorrectionToState2 );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateDeviceOrientation aZenithAngle=%d", aCorrectionToState3 );
    if( ( aZenithAngle > aCorrectionToState )
     && ( aZenithAngle < aCorrectionToState1 ) 
     && ( aAzimuthAngle > aCorrectionToState2 )
     && ( aAzimuthAngle < aCorrectionToState3 ) )
        {
        retVal = ETrue;
        }
    return retVal;
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::CalculateDeviceRotationInDegrees()
// ----------------------------------------------------------------------------------
// 
TInt CSSYOrientation::CalculateSphericalPolarCoordinates( 
    const TInt aAxisX, 
    const TInt aAxisY,
    const TInt aAxisZ,
    TInt& aZenithAngle,
    TInt& aAzimuthAngle )
    {
    SSY_TRACE_IN();
    TInt retVal( KErrNone );
    
    // Calculate the spherial coordinates from carthesian vector coordinates
    // First take the length of the vector formed by x and y.
    TReal srcSqrtZenith = (TReal)( ( aAxisX * aAxisX ) + ( aAxisY * aAxisY ) );
    TReal srcZenith( 0 );
    Math::Sqrt( srcZenith, srcSqrtZenith );
    TReal axisZ( aAxisZ );
    // The zenith angle is arctan( ( x and y length ) / z )
    TReal trgZenith( 0 );
    if(aAxisZ != 0)
        {
        srcZenith = ( srcZenith / axisZ );
        Math::ATan( trgZenith, srcZenith );
        }
    
    TReal trgAzimuth( 0 );
    TReal srcAzimuth( 0 );
    TReal realAxisX( aAxisX );
    TReal realAxisY( aAxisY );
    if(aAxisX != 0 && aAxisY !=0)
        {
        srcAzimuth = ( realAxisY / realAxisX );
        // The azimuth angle is arctan( y / x )
        Math::ATan( trgAzimuth, srcAzimuth );
        }
    if(aAxisY ==0 && aAxisX != 0)
        trgAzimuth = 0;

    TReal zenithAngle = ( trgZenith * KRad2degrees );
    
    if( zenithAngle < 0 )
        {
        // Correct the zenith angle if the result is negative
        zenithAngle = K180Degrees + zenithAngle;
        }
    
    TReal azimuthAngle = ( trgAzimuth * KRad2degrees );
    
    if( ( aAxisX > 0 ) && ( aAxisY < 0 ) )
        {
        // Azimuth is negative, so basically this is minus
        azimuthAngle = azimuthAngle + K360Degrees;
        }
    if( ( aAxisX < 0 ) && ( aAxisY > 0 ) )
        {
        // Azimuth is negative, so basically this is minus
        azimuthAngle = K180Degrees + azimuthAngle;
        }
    if( ( aAxisX < 0 ) && ( aAxisY < 0 ) )
        {
        // This is plus
        azimuthAngle = azimuthAngle + K180Degrees;
        }
        
    TReal roundedAz( 0 );
    Math::Round( roundedAz, azimuthAngle, 0 );
    TReal roundedZen( 0 );
    Math::Round( roundedZen, zenithAngle, 0 );
    
    TInt32 valiAz( 0 );
    TInt32 valiZen( 0 );
    Math::Int( valiZen, roundedZen );
    Math::Int( valiAz, roundedAz );
    aZenithAngle = (TInt)valiZen;
    aAzimuthAngle = (TInt)valiAz;

    if(aAxisZ == 0)
        aZenithAngle = 90;
    if(aAxisX == 0 && aAxisY == 0)
        aAzimuthAngle = 90;
    if(aAxisX == 0 && aAxisY > 0)
        aAzimuthAngle = 90;
    if(aAxisX ==0 && aAxisY < 0)
        aAzimuthAngle = 270;

    // Check that calculated values are in range
    if( ( aZenithAngle > K180Degrees ) 
     || ( aAzimuthAngle > K360Degrees ) 
     || ( aZenithAngle < 0 ) 
     || ( aAzimuthAngle < 0 ) )
        {
        retVal = KErrArgument;
        SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateSphericalPolarCoordinates Could not calculate because some item was null err=%d", retVal );
        }
    
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateSphericalPolarCoordinates aAzimuthAngle=%d", aAzimuthAngle );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateSphericalPolarCoordinates aZenithAngle=%d", aZenithAngle );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateSphericalPolarCoordinates aAxisX=%d", aAxisX );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateSphericalPolarCoordinates aAxisY=%d", aAxisY );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CalculateSphericalPolarCoordinates aAxisZ=%d", aAxisZ );
    
    SSY_TRACE_OUT();
    
    return retVal;
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::DataError()
// ----------------------------------------------------------------------------------
//
#ifdef _DEBUG
void CSSYOrientation::DataError( CSensrvChannel& aChannel, 
                                 TSensrvErrorSeverity aError )
#else
// aError is not used in UREL builds
void CSSYOrientation::DataError( CSensrvChannel& aChannel, 
                                 TSensrvErrorSeverity /*aError*/ )
#endif
    {
    SSY_TRACE_IN();

    ASSERT_DEBUG_SSY( iSensorChannel == &aChannel );

    SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: DataError %d for channelId %d", 
               aError, 
               aChannel.GetChannelInfo().iChannelId );

    if ( iSensorChannel != &aChannel )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Wrong channel!" );
        }

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::OpenChannelL()
// ----------------------------------------------------------------------------------
//
void CSSYOrientation::OpenChannelL()
    {
    SSY_TRACE_IN();

    // if we are opening the channel for the first time
    if ( !iSensorChannel )
        {
        TSensrvChannelInfo channelInfo;
        channelInfo.iChannelType = KSensrvChannelTypeIdAccelerometerXYZAxisData;

        // create channelfinder if it does not exist already
        if ( !iChannelFinder )
            {
            iChannelFinder = CSensrvChannelFinder::NewL();
            }

        iChannelInfoList.Reset();

        // find the accelerometer axis data channel
        iChannelFinder->FindChannelsL( iChannelInfoList, channelInfo );

        // we must find atleast one accelerometer axis channel
        if ( !iChannelInfoList.Count() )
            {
            SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: No KSensrvChannelTypeIdAccelerometerXYZAxisData found!" );
            User::Leave( KErrNotFound );
            }

        // create the actual channel
        iSensorChannel = CSensrvChannel::NewL( iChannelInfoList[0] );
        }

    // Reset orientation values
    
    iTimerState = CSSYOrientation::EUninitialized;
    
    // open the actual channel via sensor server
    iSensorChannel->OpenChannelL();

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::CloseChannelL()
// ----------------------------------------------------------------------------------
//
void CSSYOrientation::CloseChannelL()
    {
    SSY_TRACE_IN();

    if( !iSensorChannel )
        {
        User::Leave( KErrNotFound );
        }

    iSensorChannel->CloseChannel();
    iChannel = NULL;

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::StartListeningL()
// ----------------------------------------------------------------------------------
//  
void CSSYOrientation::StartListeningL()
    {
    SSY_TRACE_IN();

    if ( !iSensorChannel )
        {
        User::Leave( KErrNotFound );
        }

    TInt desiredCount( 1 );
    TInt maximumCount( 1 );
    TInt bufferingPeriod( 0 );

    iSensorChannel->StartDataListeningL( this, desiredCount, maximumCount, bufferingPeriod );

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::StopListeningL()
// ----------------------------------------------------------------------------------
// 
void CSSYOrientation::StopListeningL()
    {
    SSY_TRACE_IN();

    if ( !iSensorChannel )
        {
        User::Leave( KErrNotFound );
        }

    iSensorChannel->StopDataListening();
    iChannel = NULL;

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::GetDataFromHardware()
// ----------------------------------------------------------------------------------
//  
void CSSYOrientation::GetDataFromHardware( CSSYChannel* aChannel )
    {
    SSY_TRACE_IN();
    iChannel = aChannel;
    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::GetMaxDataRateL()
// ----------------------------------------------------------------------------------
//  
void CSSYOrientation::GetMaxDataRateL( TSensrvProperty& aProperty )
    {
    SSY_TRACE_IN();
    // Get data rate property
    TSensrvProperty property;
    iSensorChannel->GetPropertyL( KSensrvPropIdDataRate, KSensrvItemIndexNone, property );

    // Check is it array type of property
    if( ESensrvArrayPropertyInfo == property.GetArrayIndex() )
        {
        // Array type of property, get max property
        TInt maxValueIndex( 0 );
        property.GetMaxValue( maxValueIndex );
        iSensorChannel->GetPropertyL( KSensrvPropIdDataRate,
            KSensrvItemIndexNone, maxValueIndex, property );
        }
    else
        {
        // Single property, use max value as value
        TInt maxValue( 0 );
        property.GetMaxValue( maxValue );
        property.SetValue( maxValue );
        }

    // Copy max data rate to reference parameter
    aProperty = property;

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::SendDataAfterTimer()
// ----------------------------------------------------------------------------------
//
void CSSYOrientation::SendDataAfterTimer()
    {
    SSY_TRACE_IN()
    
    // Timer has fired no send the data to channel if it is a valid state
    iTimerState = CSSYOrientation::EStopped;
    // Stop and delete timer
    if( iEventTimer )
        {
        delete iEventTimer;
        iEventTimer = NULL;
        }
    TInt amountOfOrientations = iOrientationArray.Count() - 1;
    
    // If there are no orientation events don't try counting anything
    if( amountOfOrientations >= 0 )
        {
        TInt lastOrientationOfArray = iOrientationArray[ amountOfOrientations ];
        TInt sumOfCorrectOrientations( 0 );
        
        // Check if the state should be changed to the state of the last orientation
        for( TInt i = 0; iOrientationArray.Count() != i; i++ )
            {
            SSY_TRACE( EExtended, "ORIENTATIONSSY:SendDataAfterTimer::iOrientationArray[ i ]=%d", iOrientationArray[ i ] );
            if( iOrientationArray[ i ] == lastOrientationOfArray )
                {
                sumOfCorrectOrientations++;
                }
            }
        TReal realCountOfArray = (TReal)( iOrientationArray.Count() );
        TReal result = (TReal)(sumOfCorrectOrientations) / realCountOfArray;
        
        SSY_TRACE( EExtended, "ORIENTATIONSSY:SendDataAfterTimer::result=%f", result );
        
        // If variance of the last orientation is in range and current orientation is a new orientation, change the state
        if( ( result > KVariationFactor ) &&
            ( iCurrentOrientationState.iDeviceOrientation != 
            (TSensrvOrientationData::TSensrvDeviceOrientation)lastOrientationOfArray ) )
            {            
            iOrientationEvent.iDeviceOrientation = 
            (TSensrvOrientationData::TSensrvDeviceOrientation)lastOrientationOfArray;
            
            SSY_TRACE( EExtended, "ORIENTATIONSSY:SendDataAfterTimer::Orientation changed to=%d", lastOrientationOfArray );
            iCurrentOrientationState.iDeviceOrientation = 
            (TSensrvOrientationData::TSensrvDeviceOrientation)lastOrientationOfArray;
            
            // Get configurations for the new state
            GetConfigurationForState( 
                iCurrentOrientationState.iDeviceOrientation,
                iConfigForCurrentState );
        
            if( iChannel )
                {
                if( ( iChannelType == KSensrvChannelTypeIdOrientationData )  &&
                    ( iChannel->AppendData( iOrientationEvent, iRotationEvent ) ) )
                    {
                    iChannel->IssueRequest();
                    iChannel = NULL;
                    }
                }
            }
        
        iOrientationArray.Reset();
        }
    
    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::SetConfigurationForState()
// ----------------------------------------------------------------------------------
//
TInt CSSYOrientation::SetConfigurationForState(
 const TOrientationConfiguration& aConfigurationForState )
    {
    SSY_TRACE_IN();
    TBool wasFound( EFalse );
    TInt retVal( KErrNone );
    
    // Check if is in array
    for( TInt i = 0; i != iConfigArray.Count(); i++ )
        {
        TOrientationConfiguration ori = iConfigArray[ i ];
        if( aConfigurationForState.iOrientationState == iConfigArray[ i ].iOrientationState )
            {
            iConfigArray.Remove( i );
            retVal = iConfigArray.Insert( aConfigurationForState, i );
            wasFound = ETrue;
            }
        }
    if( !wasFound )
        {
        retVal = iConfigArray.Append( aConfigurationForState );
        }
	
    SSY_TRACE_OUT();
    return retVal;
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::GetConfigurationForState()
// ----------------------------------------------------------------------------------
//
void CSSYOrientation::GetConfigurationForState( 
 const TSensrvOrientationData::TSensrvDeviceOrientation aOrientation,
 TOrientationConfiguration& aConfigurationForState )
    {
    SSY_TRACE_IN();
    SSY_TRACE( EExtended, "ORIENTATIONSSY: First time data received, set first orientation=%d", (TInt)aOrientation );
    // Check if is in array
    for( TInt i = 0; i != iConfigArray.Count(); i++ )
        {
        TOrientationConfiguration ori = iConfigArray[ i ];
        if( aOrientation == iConfigArray[ i ].iOrientationState )
            {
            aConfigurationForState = iConfigArray[ i ];
            }
        }
    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::CreteConfigurations()
// ----------------------------------------------------------------------------------
//
TInt CSSYOrientation::CreateConfigurations()
    {
    SSY_TRACE_IN();
    
    TInt repValue( 0 );
    TInt retVal( KErrNone );
    
    // This is used in initialisation
    if( iConfigForCurrentState.iOrientationState == TSensrvOrientationData::EOrientationUndefined )
        {
        iConfigForCurrentState.iOrientationDisplayUp.iAngle = 0;
        iConfigForCurrentState.iOrientationDisplayUp.iTimerValueInMilSeconds = 0;
        iConfigForCurrentState.iOrientationDisplayDown.iAngle = 0;
        iConfigForCurrentState.iOrientationDisplayDown.iTimerValueInMilSeconds = 0;
        iConfigForCurrentState.iOrientationDisplayLeftUp.iAngle = 0;
        iConfigForCurrentState.iOrientationDisplayLeftUp.iTimerValueInMilSeconds = 0;
        iConfigForCurrentState.iOrientationDisplayRightUp.iAngle = 0;
        iConfigForCurrentState.iOrientationDisplayRightUp.iTimerValueInMilSeconds = 0;
        iConfigForCurrentState.iOrientationDisplayUpwards.iAngle = 0;
        iConfigForCurrentState.iOrientationDisplayUpwards.iTimerValueInMilSeconds = 0;
        iConfigForCurrentState.iOrientationDisplayDownwards.iAngle = 0;
        iConfigForCurrentState.iOrientationDisplayDownwards.iTimerValueInMilSeconds = 0;
        }
    
    // Configuration for display up
    TOrientationConfiguration configForDisplayUp;
    configForDisplayUp.iOrientationState = TSensrvOrientationData::EOrientationDisplayUp;
    configForDisplayUp.iOrientationDisplayUp.iAngle = 0;
    configForDisplayUp.iOrientationDisplayUp.iTimerValueInMilSeconds = 0;
    
    iRepository->Get( KOriStateDisplayUpToDisplayDownKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUp.iOrientationDisplayDown=%d", repValue );
    configForDisplayUp.iOrientationDisplayDown.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayUpToDisplayDownTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUp.iOrientationDisplayDowniTimerValueInMilSeconds=%d", repValue );
    configForDisplayUp.iOrientationDisplayDown.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayUpToDisplayLeftUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUp.iOrientationDisplayLeftUp=%d", repValue );
    configForDisplayUp.iOrientationDisplayLeftUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayUpToDisplayLeftUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUp.iOrientationDisplayLeftUpiTimerValueInMilSeconds=%d", repValue );
    configForDisplayUp.iOrientationDisplayLeftUp.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayUpToDisplayRightUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUp.iOrientationDisplayRightUp=%d", repValue );
    configForDisplayUp.iOrientationDisplayRightUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayUpToDisplayRightUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUp.iOrientationDisplayRightUpiTimerValueInMilSeconds=%d", repValue );
    configForDisplayUp.iOrientationDisplayRightUp.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayUpToDisplayUpwardsKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUp.iOrientationDisplayUpwards=%d", repValue );
    configForDisplayUp.iOrientationDisplayUpwards.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayUpToDisplayUpwardsTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUp.iOrientationDisplayUpwards.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayUp.iOrientationDisplayUpwards.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayUpToDisplayDownwardsKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUp.iOrientationDisplayDownwards=%d", repValue );
    configForDisplayUp.iOrientationDisplayDownwards.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayUpToDisplayDownwardsTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUp.iOrientationDisplayDownwards.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayUp.iOrientationDisplayDownwards.iTimerValueInMilSeconds = repValue;
    
    retVal = SetConfigurationForState( configForDisplayUp );
    if( retVal != KErrNone)
        {
        SSY_TRACE( EExtended, "ORIENTATIONSSY:SetConfigurationForState(configForDisplayUp)=%d", retVal);
        SSY_TRACE_OUT();
	return retVal;
         }
    
    // Configuration for display down
    TOrientationConfiguration configForDisplayDown;
    configForDisplayDown.iOrientationState = TSensrvOrientationData::EOrientationDisplayDown;
    
    iRepository->Get( KOriStateDisplayDownToDisplayUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDown.iOrientationDisplayUp=%d", repValue );
    configForDisplayDown.iOrientationDisplayUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayDownToDisplayUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDown.iOrientationDisplayUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayDown.iOrientationDisplayUp.iTimerValueInMilSeconds = repValue;
    
    configForDisplayDown.iOrientationDisplayDown.iAngle = 0;
    configForDisplayDown.iOrientationDisplayDown.iTimerValueInMilSeconds = 0;
    
    iRepository->Get( KOriStateDisplayDownToDisplayLeftUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDown.iOrientationDisplayLeftUp=%d", repValue );
    configForDisplayDown.iOrientationDisplayLeftUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayDownToDisplayLeftUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDown.iOrientationDisplayLeftUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayDown.iOrientationDisplayLeftUp.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayDownToDisplayRightUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDown.iOrientationDisplayRightUp=%d", repValue );
    configForDisplayDown.iOrientationDisplayRightUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayDownToDisplayRightUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDown.iOrientationDisplayRightUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayDown.iOrientationDisplayRightUp.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayDownToDisplayUpwardsKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDown.iOrientationDisplayUpwards=%d", repValue );
    configForDisplayDown.iOrientationDisplayUpwards.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayDownToDisplayUpwardsTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDown.iOrientationDisplayUpwards.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayDown.iOrientationDisplayUpwards.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayDownToDisplayDownwardsKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDown.iOrientationDisplayDownwards=%d", repValue );
    configForDisplayDown.iOrientationDisplayDownwards.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayDownToDisplayDownwardsTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDown.iOrientationDisplayDownwards.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayDown.iOrientationDisplayDownwards.iTimerValueInMilSeconds = repValue;

    retVal = SetConfigurationForState( configForDisplayDown);
    if( retVal != KErrNone)
        {
        SSY_TRACE( EExtended, "ORIENTATIONSSY:SetConfigurationForState(configForDisplayDown)=%d", retVal);
        SSY_TRACE_OUT();
	return retVal;
         }
    
    // Configuration for left up
    TOrientationConfiguration configForDisplayLeftUp;
    configForDisplayLeftUp.iOrientationState = TSensrvOrientationData::EOrientationDisplayLeftUp;
    
    iRepository->Get( KOriStateDisplayLeftUpToDisplayUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayLeftUp.iOrientationDisplayUp=%d", repValue );
    configForDisplayLeftUp.iOrientationDisplayUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayLeftUpToDisplayUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayLeftUp.iOrientationDisplayUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayLeftUp.iOrientationDisplayUp.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayLeftUpToDisplayDownKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayLeftUp.iOrientationDisplayDown=%d", repValue );
    configForDisplayLeftUp.iOrientationDisplayDown.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayLeftUpToDisplayDownTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayLeftUp.iOrientationDisplayDown.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayLeftUp.iOrientationDisplayDown.iTimerValueInMilSeconds = repValue;
    
    configForDisplayLeftUp.iOrientationDisplayLeftUp.iAngle = 0;
    configForDisplayLeftUp.iOrientationDisplayLeftUp.iTimerValueInMilSeconds = 0;
    
    iRepository->Get( KOriStateDisplayLeftUpToDisplayRightUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayLeftUp.iOrientationDisplayRightUp=%d", repValue );
    configForDisplayLeftUp.iOrientationDisplayRightUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayLeftUpToDisplayRightUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayLeftUp.iOrientationDisplayRightUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayLeftUp.iOrientationDisplayRightUp.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayLeftUpToDisplayUpwardsKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayLeftUp.iOrientationDisplayUpwards=%d", repValue );
    configForDisplayLeftUp.iOrientationDisplayUpwards.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayLeftUpToDisplayUpwardsTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayLeftUp.iOrientationDisplayUpwards.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayLeftUp.iOrientationDisplayUpwards.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayLeftUpToDisplayDownwardsKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayLeftUp.iOrientationDisplayDownwards=%d", repValue );
    configForDisplayLeftUp.iOrientationDisplayDownwards.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayLeftUpToDisplayDownwardsTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayLeftUp.iOrientationDisplayDownwards.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayLeftUp.iOrientationDisplayDownwards.iTimerValueInMilSeconds = repValue;

    retVal = SetConfigurationForState( configForDisplayLeftUp );
    if( retVal != KErrNone)
        {
        SSY_TRACE( EExtended, "ORIENTATIONSSY:SetConfigurationForState(configForDisplayLeftUp)=%d", retVal);
        SSY_TRACE_OUT();
	return retVal;
         }
    
    // Configuration for right up
    TOrientationConfiguration configForDisplayRightUp;
    configForDisplayRightUp.iOrientationState = TSensrvOrientationData::EOrientationDisplayRightUp;
    
    iRepository->Get( KOriStateDisplayRightUpToDisplayUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayRightUp.iOrientationDisplayUp=%d", repValue );
    configForDisplayRightUp.iOrientationDisplayUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayRightUpToDisplayUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayRightUp.iOrientationDisplayUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayRightUp.iOrientationDisplayUp.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayRightUpToDisplayDownKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayRightUp.iOrientationDisplayDown=%d", repValue );
    configForDisplayRightUp.iOrientationDisplayDown.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayRightUpToDisplayDownTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayRightUp.iOrientationDisplayDown.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayRightUp.iOrientationDisplayDown.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayRightUpToDisplayLeftUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayRightUp.iOrientationDisplayLeftUp=%d", repValue );
    configForDisplayRightUp.iOrientationDisplayLeftUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayRightUpToDisplayLeftUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayRightUp.iOrientationDisplayLeftUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayRightUp.iOrientationDisplayLeftUp.iTimerValueInMilSeconds = repValue;
    
    configForDisplayRightUp.iOrientationDisplayRightUp.iAngle = 0;
    configForDisplayRightUp.iOrientationDisplayRightUp.iTimerValueInMilSeconds = 0;
    
    iRepository->Get( KOriStateDisplayRightUpToDisplayUpwardsKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayRightUp.iOrientationDisplayUpwards=%d", repValue );
    configForDisplayRightUp.iOrientationDisplayUpwards.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayRightUpToDisplayUpwardsTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayRightUp.iOrientationDisplayUpwards.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayRightUp.iOrientationDisplayUpwards.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayRightUpToDisplayDownwardsKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayRightUp.iOrientationDisplayDownwards=%d", repValue );
    configForDisplayRightUp.iOrientationDisplayDownwards.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayRightUpToDisplayDownwardsTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayRightUp.iOrientationDisplayDownwards.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayRightUp.iOrientationDisplayDownwards.iTimerValueInMilSeconds = repValue;

    retVal = SetConfigurationForState( configForDisplayRightUp );
    if( retVal != KErrNone)
        {
        SSY_TRACE( EExtended, "ORIENTATIONSSY:SetConfigurationForState( configForDisplayRightUp)=%d", retVal);
        SSY_TRACE_OUT();
	return retVal;
         }
    
    // Configuration for upwards
    TOrientationConfiguration configForDisplayUpwards;
    configForDisplayUpwards.iOrientationState = TSensrvOrientationData::EOrientationDisplayUpwards;
    
    iRepository->Get( KOriStateDisplayUpwardsToDisplayUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUpwards.iOrientationDisplayUp=%d", repValue );
    configForDisplayUpwards.iOrientationDisplayUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayUpwardsToDisplayUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUpwards.iOrientationDisplayUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayUpwards.iOrientationDisplayUp.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayUpwardsToDisplayDownKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUpwards.iOrientationDisplayDown=%d", repValue );
    configForDisplayUpwards.iOrientationDisplayDown.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayUpwardsToDisplayDownTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUpwards.iOrientationDisplayDown.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayUpwards.iOrientationDisplayDown.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayUpwardsToDisplayLeftUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUpwards.iOrientationDisplayLeftUp=%d", repValue );
    configForDisplayUpwards.iOrientationDisplayLeftUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayUpwardsToDisplayLeftUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUpwards.iOrientationDisplayLeftUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayUpwards.iOrientationDisplayLeftUp.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayUpwardsToDisplayRightUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUpwards.iOrientationDisplayRightUp=%d", repValue );
    configForDisplayUpwards.iOrientationDisplayRightUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayUpwardsToDisplayRightUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUpwards.iOrientationDisplayRightUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayUpwards.iOrientationDisplayRightUp.iTimerValueInMilSeconds = repValue;
    
    configForDisplayUpwards.iOrientationDisplayUpwards.iAngle = 0;
    configForDisplayUpwards.iOrientationDisplayUpwards.iTimerValueInMilSeconds = 0;
    
    iRepository->Get( KOriStateDisplayUpwardsToDisplayDownwardsKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUpwards.iOrientationDisplayDownwards=%d", repValue );
    configForDisplayUpwards.iOrientationDisplayDownwards.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayUpwardsToDisplayDownwardsTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayUpwards.iOrientationDisplayDownwards.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayUpwards.iOrientationDisplayDownwards.iTimerValueInMilSeconds = repValue;

    retVal = SetConfigurationForState( configForDisplayUpwards);
    if( retVal != KErrNone)
        {
        SSY_TRACE( EExtended, "ORIENTATIONSSY:SetConfigurationForState( configForDisplayUpwards)=%d", retVal);
        SSY_TRACE_OUT();
	return retVal;
         }
    
    // Configuration for downwards
    TOrientationConfiguration configForDisplayDownwards;
    configForDisplayDownwards.iOrientationState =
         TSensrvOrientationData::EOrientationDisplayDownwards;
    
    iRepository->Get( KOriStateDisplayDownwardsToDisplayUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDownwards.iOrientationDisplayUp=%d", repValue );
    configForDisplayDownwards.iOrientationDisplayUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayDownwardsToDisplayUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDownwards.iOrientationDisplayUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayDownwards.iOrientationDisplayUp.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayDownwardsToDisplayDownKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDownwards.iOrientationDisplayDown=%d", repValue );
    configForDisplayDownwards.iOrientationDisplayDown.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayDownwardsToDisplayDownTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDownwards.iOrientationDisplayDown.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayDownwards.iOrientationDisplayDown.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayDownwardsToDisplayLeftUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDownwards.iOrientationDisplayLeftUp=%d", repValue );
    configForDisplayDownwards.iOrientationDisplayLeftUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayDownwardsToDisplayLeftUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDownwards.iOrientationDisplayLeftUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayDownwards.iOrientationDisplayLeftUp.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayDownwardsToDisplayRightUpKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDownwards.iOrientationDisplayRightUp=%d", repValue );
    configForDisplayDownwards.iOrientationDisplayRightUp.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayDownwardsToDisplayRightUpTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDownwards.iOrientationDisplayRightUp.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayDownwards.iOrientationDisplayRightUp.iTimerValueInMilSeconds = repValue;
    
    iRepository->Get( KOriStateDisplayDownwardsToDisplayUpwardsKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDownwards.iOrientationDisplayUpwards=%d", repValue );
    configForDisplayDownwards.iOrientationDisplayUpwards.iAngle = repValue;
    
    iRepository->Get( KOriStateDisplayDownwardsToDisplayUpwardsTimerKey, repValue );
    SSY_TRACE( EExtended, "ORIENTATIONSSY:CreteConfigurations::EOrientationDisplayDownwards.iOrientationDisplayUpwards.iTimerValueInMilSeconds=%d", repValue );
    configForDisplayDownwards.iOrientationDisplayUpwards.iTimerValueInMilSeconds = repValue;
    
    configForDisplayDownwards.iOrientationDisplayDownwards.iAngle = 0;
    configForDisplayDownwards.iOrientationDisplayDownwards.iTimerValueInMilSeconds = 0;
    

    retVal = SetConfigurationForState( configForDisplayDownwards);
    if( retVal != KErrNone)
        {
        SSY_TRACE( EExtended, "ORIENTATIONSSY:SetConfigurationForState( configForDisplayDownwards)=%d", retVal);
        SSY_TRACE_OUT();
	return retVal;
        }
    
    if( iConfigForCurrentState.iOrientationState != TSensrvOrientationData::EOrientationUndefined )
        {
        GetConfigurationForState(
        iCurrentOrientationState.iDeviceOrientation, iConfigForCurrentState );
        }
    
    SSY_TRACE_OUT();
    return retVal;
    }

// ----------------------------------------------------------------------------------
// CSSYOrientation::GetDataListenerInterfaceL()
// ----------------------------------------------------------------------------------
//
void CSSYOrientation::GetDataListenerInterfaceL(
    TUid /*aInterfaceUid*/, 
    TAny*& aInterface )
    {
    aInterface = NULL;
    }

// End of File
