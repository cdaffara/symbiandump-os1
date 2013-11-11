/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Channel class of Orientation SSY
*
*/


#ifndef __SSYORIENTATION_H__
#define __SSYORIENTATION_H__


// INCLUDES
#include <sensrvdatalistener.h>
#include <sensrvchannelinfo.h>
#include <sensrvorientationsensor.h>
#include <centralrepository.h>
#include "SsyChannel.h"
#include "SsyTrace.h"
#include "Ssyeventtimer.h"
#include "OrientationCRListener.h"
#include "OrientationConfiguration.h"

// FORWARD DECLARATIONS
class CSensrvChannelFinder;
class CSensrvChannel;
class MSendDataCallBack;
class CSsyOrientationEventTimer;
class TOrientationConfiguration;
class TAngleAndTimerConfig;
class CSsyOrientationCRListener;
class CSensorDataCompensator;

// CONSTANTS
/**
 *   TRotation
 */
enum TRotation
    {
    ERotationAboutXaxis = 0,
    ERotationAboutYaxis,
    ERotationAboutZaxis
    };

// CLASS DECLARATION

/**
*  CSSYOrientation class
*
*  Orientation calculation and sensorserver client class for orientation ssy.
*
*  @lib orientationssy.lib
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CSSYOrientation ) : public CBase, 
                                       public MSensrvDataListener, 
                                       public MSendDataCallBack,
                                       public MCenRepCallBack
    {
    
    public:
        
        /**
        * State of the timer
        */
        enum TTimerState
            {
            EUninitialized = 0,
            EStopped,
            ERunning
            };
        
    public: // From CActive
        void RunL();
        void DoCancel();
        
        /**
        * C++ constructor.
        *
        * @param aChannelType Identifies the type of the channel
        */
        CSSYOrientation( const TSensrvChannelTypeId aChannelType );

        /**
        * Destructor
        */
        virtual ~CSSYOrientation();
        
        /*
         * Second Phase Consructor
         */
        void ConstructL();
        
        /**
        * Create configurations for angles and timers
        */
        TInt CreateConfigurations();
        
        /**
        * Used for creating configuration for angle definitions
        *
        * @param aConfigurationForState the configuration to set.
        */
        TInt SetConfigurationForState( 
        const TOrientationConfiguration&
              aConfigurationForState );
        
        /**
        * Used for getting configuration for angle definitions
        *
        * @param aOrientation to get.
        * @param aConfigurationForState that is returned.
        */
        void GetConfigurationForState( 
        const TSensrvOrientationData::TSensrvDeviceOrientation aOrientation,
             TOrientationConfiguration& aConfigurationForState );
        
        /**
        * From MSensrvDataListener. Indicates data is available in the receiving buffer.
        *
        * @param aChannel Channel which has received the data
        * @param aCount Number of data items received
        * @param aDataLost Number of data items lost
        */
        void DataReceived( CSensrvChannel& aChannel, 
                           TInt aCount, 
                           TInt aDataLost );

        /**
        * From MSensrvDataListener. Data listening failed.
        * 
        * @param aChannel Channel that received error
        * @param aError Error cause
        */
        void DataError( CSensrvChannel& aChannel, 
                        TSensrvErrorSeverity aError );
        
        /** 
        * Returns a pointer to a specified interface - to allow future extension
        * of this class without breaking binary compatibility
        *
        * @since S60 5.0
        * @param aInterfaceUid Identifier of the interface to be retrieved
        * @param aInterface A reference to a pointer that retrieves the specified interface.
        */
        void GetDataListenerInterfaceL( TUid aInterfaceUid, 
                                        TAny*& aInterface );
        
        /**
        * Open channel to sensor server. Used by the channel object.
        */
        void OpenChannelL();

        /**
        * Close channel from sensor server. Used by the channel object.
        */
        void CloseChannelL();

        /**
        * Start listening a channel from sensor server. Used by the channel object.
        */
        void StartListeningL();

        /**
        * Stop listening a channel from sensor server. Used by the channel object.
        */
        void StopListeningL();

        /**
        * Register to get data from the sensor server. Used by the channel object.
        *
        * @param aChannel Channel object where to receive data
        */
        void GetDataFromHardware( CSSYChannel* aChannel );

        /**
        * Gets max data rate from accelerometer SSY
        * 
        * @param aProperty Contains search criteria for property. On
        *        completion, contains found property information. This
        *        property is set to read only as it is not owned by orientation
        *        SSY.
        */
        void GetMaxDataRateL( TSensrvProperty& aProperty );
        
        /**
        * Calculate device rotation based on the received data.
        *
        * @param aAxis1 This is Z-axis when calculating X- or Y-axis rotation.
        *        X-axis when calculating Z-axis rotation.
        * @param aAxis2 This is Y-axis when calculating X- or Z-axis rotation.
        *        X-axis when calculating Y-axis rotation.
        * @param aRotation Defines the third axis
        * @return Rotation of calculated third axis
        */
        TInt CalculateDeviceRotationInDegrees( 
                    const TInt aAxis1, 
                    const TInt aAxis2, 
                    const TRotation aRotation );
        
        /**
        * Calculates the spherical coordinates from the acceleration vector.
        *
        * @param aAxisX X Axis of the vector
        * @param aAxisY Y axis of the vector
        * @param aAxisZ Z axis of the vector
        * @param aZenithAngle The zenithe ange of the vector.
        * @param aAzimuthAngle The azimuth angle of the vector.
        * @return Standard Symbian error codes.
        */
        TInt CalculateSphericalPolarCoordinates( 
                    const TInt aAxisX, 
                    const TInt aAxisY,
                    const TInt aAxisZ,
                    TInt& aZenithAngle,
                    TInt& aAzimuthAngle );
        
        /**
        * Calculate device orientation based on the received data.
        *
        * @param aZenithAngle the calculated zenith angle.
        * @param aAzimuthAngle the calculated azimuth angle.
        * @return The device orientation.
        */
        TSensrvOrientationData::TSensrvDeviceOrientation CalculateDeviceOrientation(
                    const TInt aZenithAngle,
                    const TInt aAzimuthAngle );
        
        TBool IsOrientation(
                    const TInt aZenithAngle,
                    const TInt aAzimuthAngle,
                    TInt aCorrectionToState,
                    TInt aCorrectionToState1,
                    TInt aCorrectionToState2,
                    TInt aCorrectionToState3 );
    
    protected:
    
        // @see MSendDataCallBack
        void SendDataAfterTimer();

    private:

        /** 
        * Channel type. 
        */
        TSensrvChannelTypeId    iChannelType;

        /** 
        * Orientation ssy channel. Not owned. Pointer is assigned 
        * in GetDataFromHardware function.
        */
        CSSYChannel*            iChannel;

        /** 
        * Sensor server channel finder. 
        */
        CSensrvChannelFinder*   iChannelFinder;

        /** 
        * List of channelinfos. 
        */
        RSensrvChannelInfoList  iChannelInfoList;

        /** 
        * Sensor server channel. 
        */
        CSensrvChannel*         iSensorChannel;

        /** 
        * Previous TSensrvOrientationData. 
        */
        TSensrvOrientationData  iOrientationEvent;
        
        /** 
        * Current state of orientation. 
        */
        TSensrvOrientationData  iCurrentOrientationState;
        
        /** 
        * Previous TSensrvRotationData. 
        */
        TSensrvRotationData     iRotationEvent;

        /** 
        * State of the timer
        */
        TTimerState iTimerState;
        
        /** 
        * Pointer to the timer instance, owned
        */
        CSsyOrientationEventTimer* iEventTimer;
        
        /** 
        * The array of orientation that is calculated
        */
        RArray<TInt> iOrientationArray;
        
        /** 
        * The configuration for the current state.
        */
        TOrientationConfiguration iConfigForCurrentState;
        
        /** 
        * The array containing the different configurations.
        */
        RArray<TOrientationConfiguration> iConfigArray;
        
        /** 
        * Pointer to central repository, owned
        */
        CRepository* iRepository;

#ifdef AUTO_ORIENTAION_TEST       
        /** 
        * Pointer to the CR listener object, owned.
        * This is used only for test application. The testing flag must be
        * defined in order to use this.
        */
        CSsyOrientationCRListener* iCRListener;
#endif
    
        /**
        * Boolean indicating is received data first one
        */
        TBool iFirstDataReceived;
        
        /**
        * Sensor compensator.
        */
        CSensorDataCompensator* iCompensator;
    
    };

#endif // __SSYORIENTATION_H__

// End of File
