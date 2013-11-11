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
* Description:  CTCChannel class declaration.
*
*/



#ifndef TCCHANNELTILT_H
#define TCCHANNELTILT_H

// INCLUDES
#include <sensrvdatalistener.h>
#include <sensrvpropertylistener.h>

#include "tcchannel.h"

// FORWARD DECLARATIONS
class CSensrvChannel;
class CSensorDataCompensator;
// CLASS DECLARATION

/**
*  Tilt channel.
*  Tilt channel uses acclerometer data with magnetometer data
*  to calculate 2D data from 3D vectors.
* 
*  @lib None.
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTCChannelTilt ): public CTCChannel,
    public MSensrvDataListener, public MSensrvPropertyListener
    {
    public:
    
        /**
        * Two phased constructor.
        *
        * @param aCallback SSY callback.
        * @return CTCChannelTilt
        */
        static CTCChannelTilt* NewL( MSsyCallback& aCallback );
        static CTCChannelTilt* NewLC( MSsyCallback& aCallback );
    
        // Default C++ destructor
        ~CTCChannelTilt();
    
    public: // From base classes

        // @see CTCChannel
        void RegisterChannelL( RSensrvChannelInfoList& aChannels );
        
        // @see CTCChannel
        void OpenChannelL();

        // @see CTCChannel
        void CloseChannelL();
        
        // @see CTCChannel
        void RegisterStateL( CTCState::TCStateId aStateId,
            RStateArray& aStateArray );
            
        // @see CActive
        void RunL();
        
        // @see MSsyPropertyProvider
        void CheckPropertyDependenciesL( const TSensrvChannelId aChannelId,
            const TSensrvProperty& aProperty,
            RSensrvChannelList& aAffectedChannels );

        // @see MSsyPropertyProvider
        void SetPropertyL( const TSensrvChannelId aChannelId,
            const TSensrvProperty& aProperty );
                                
        // @see MSsyPropertyProvider
        void GetPropertyL( const TSensrvChannelId aChannelId, 
            TSensrvProperty& aProperty );

        // @see MSsyPropertyProvider
        void GetAllPropertiesL( const TSensrvChannelId aChannelId, 
            RSensrvPropertyList& aChannelPropertyList );
            
        // @see MSsyPropertyProvider
        void GetPropertyProviderInterfaceL( TUid aInterfaceUid, 
            TAny*& aInterface );
            
        // @see MSsyChannelDataProvider
        void StartChannelDataL( const TSensrvChannelId aChannelId, 
            TUint8* aBuffer, TInt aCount );

        // @see MSsyChannelDataProvider
        void StopChannelDataL( const TSensrvChannelId aChannelId );

        // @see MSsyChannelDataProvider
        void ForceBufferFilledL( const TSensrvChannelId aChannelId );
        
        // @see MSsyChannelDataProvider
        void GetChannelDataProviderInterfaceL( TUid aInterfaceUid, 
            TAny*& aInterface );
        
        // @see MTCTransactionHandler
        void ProcessTransactionL( TTCTransactionId aId,
            TTCTransaction* aTransaction = NULL );
            
        // @see MTCTransactionHandler
        const RSensrvPropertyList& Properties() const;
        
        // @see MSensorDataListener
        void DataReceived( CSensrvChannel& aChannel, 
            TInt aCount, 
            TInt aDataLost );

        // @see MSensorDataListener
        void DataError( CSensrvChannel& aChannel, 
            TSensrvErrorSeverity aError );
            
        // @see MSensorDataListener
        void GetDataListenerInterfaceL( TUid aInterfaceUid, 
            TAny*& aInterface );
        
        // @see MSensrvPropertyListener
        void PropertyChanged( CSensrvChannel& aChannel, 
            const TSensrvProperty& aChangedProperty );
        
        // @see MSensrvPropertyListener
        void PropertyError( CSensrvChannel& aChannel, 
            TSensrvErrorSeverity aError );
            
        // @see MSensrvPropertyListener
        void SetPropertySuccessIndicationChanged( 
            TSetPropertySuccessIndicator aIndication );
            
        // @see MSensrvPropertyListener
        void GetPropertyListenerInterfaceL( TUid aInterfaceUid, 
            TAny*& aInterface );
        
    private: // New methods
    
        // Opens channel
        void DoOpenChannelL();
        
        // Closes channel
        void DoCloseChannel();
        
        // Start channel data
        void DoStartChannelDataL();
        
        // Close channel data
        void DoStopChannelData();
        
        // Find and create accelerometer channel
        void FindAndCreateAccelerometerChannelL();

        // Find and create magnetometer channel
        void FindAndCreateMagnetometerChannelL();
        
        // Get property
        TSensrvProperty* Property( TSensrvPropertyId aId );

    private:

        CTCChannelTilt( MSsyCallback& aCallback );
        
        void ConstructL();
        
    private: // Data
        
        TBool iIsSetFromListened;
        
        /** Accelerometer channel, owned */
        CSensrvChannel* iAccelerometerChannel;
        
        /** Magnetometer channel, owned */
        CSensrvChannel* iMagnetometerChannel;
        
        /** Tilt channel properties */
        RSensrvPropertyList iProperties;
        
        /** Maximum value for data rate */
        TInt iMaximumDataRate;
        
        /** Check which channel has the lower rate */
        TBool iIsAccelometerLower;
        
        /** Sensor data accelerometer compensator instance, owned.*/
        CSensorDataCompensator* iAccelerometerCompensator;

        /** Sensor data magnetometer compensator instance, owned.*/
        CSensorDataCompensator* iMagnetometerCompensator;
    };

#endif // TCCHANNELTILT_H

// End of File
