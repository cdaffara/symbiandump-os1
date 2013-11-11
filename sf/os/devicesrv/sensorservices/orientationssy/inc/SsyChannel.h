/*
* Copyright (c) 2007,2008 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __SSYCHANNEL_H__
#define __SSYCHANNEL_H__

// INCLUDES
#include <ssychanneldataprovider.h>  // ssy plugin API data provider
#include <ssypropertyprovider.h>     // ssy plugin API property provider
#include <sensrvchannelinfo.h>       // sensor server channelinfo
#include <sensrvchanneldatatypes.h>  // sensor server datatypes
#include <sensrvorientationsensor.h>  // sensor server datatypes

// CONSTANTS
enum TChannelState
    {
    EChannelIdle              = 0x0001,
    EChannelOpening           = 0x0002,
    EChannelOpen              = 0x0004,
    EChannelClosing           = 0x0008,
    EChannelListening         = 0x0010,
    EChannelDataReceived      = 0x0020,
    EChannelStopListening     = 0x0040,
    EChannelForceBufferFilled = 0x0080,
    EChannelBufferFilled      = 0x0100
    };

const TInt KChannelCanBeOpened   = EChannelIdle;

const TInt KChannelCanBeListened = EChannelOpen;

const TInt KChannelCanBeFilled   = EChannelListening |                                           
                                   EChannelDataReceived;

const TInt KChannelCanBeStopped  = EChannelListening |
                                   EChannelDataReceived |
                                   EChannelForceBufferFilled |
                                   EChannelBufferFilled;

const TInt KChannelCanBeClosed   = EChannelOpening |
                                   EChannelOpen |
                                   EChannelListening |
                                   EChannelDataReceived |
                                   EChannelStopListening |
                                   EChannelForceBufferFilled |
                                   EChannelBufferFilled;

// FORWARD DECLARATIONS
class CSSYProperty;
class MSsyCallback;
class CSSYOrientation;


// CLASS DECLARATION

/**
 *  CSSYChannel channel class
 *
 *  Channel class for orientation ssy.
 *
 *  @lib orientationssy.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CSSYChannel ) : public CActive, 
                                   public MSsyChannelDataProvider,
                                   public MSsyPropertyProvider
    {
    public:

        /**
         * Two phase constructor.
         *
         * @param aProperty Properties of this channel
         * @param aCallBack Callback class from sensor server
         * @param aChannelInfo Informnation os this channel
         */
        static CSSYChannel* NewL( CSSYProperty* aProperty,
                                  MSsyCallback* const aCallBack,                                  
                                  const TSensrvChannelInfo& aChannelInfo );

        /**
         * Destructor.
         */
        virtual ~CSSYChannel();

        /**
         * Issue a request into the active scheduler.
         */
        void IssueRequest( TInt aError = KErrNone );

        /**
         * Append data into the client buffer.
         *
         * @param aOrientationData Orientation channel data
         * @param aRotationData Rotation channel data
         * @return TBool True if appending data is successfull
         */
        TBool AppendData( const TSensrvOrientationData& aOrientationData, 
                          const TSensrvRotationData& aRotationData );

        /**
         * Open the channel. Called by the controller.
         *
         * @return KErrNone if channel is opened successfully
         */
        TInt OpenChannel();

        /**
         * Close the channel. Called by the controller.
         *
         * @return KErrNone if channel is closed successfully
         */
        TInt CloseChannel();

        /**
         * Return the channelId of the channel.
         *
         * @return Channel identifier of this channel
         */
        TSensrvChannelId ChannelId();

        /**
         * Set the channel id for the channel after getting it from the client.
         * Called by the controller.
         *
         * @param aChannelId New channel identifier of this channel
         */
        void SetChannelId( const TSensrvChannelId aChannelId );

    private:

        /**
         *  From CActive. Cancels any pending request
         */
        void DoCancel();

        /**
         *  From CActive. Handles completed request
         */
        void RunL();
     
        /**
         *  From CActive. Handles errors
         */
        TInt RunError( TInt aError );
     
        /**
         * From MSsyChannelDataProvider. Starts asynchronous data listening.
         */
        void StartChannelDataL( const TSensrvChannelId aChannelId,
                                TUint8* aBuffer, TInt aCount );
        /**
         * From MSsyChannelDataProvider. Stops asynchronous data listening.
         */
        void StopChannelDataL( const TSensrvChannelId aChannelId );

        /**
         * From MSsyChannelDataProvider. Forces SSY to call BufferFilled()
         * regardless of how many data items have been written to buffer.
         */
        void ForceBufferFilledL( const TSensrvChannelId aChannelId );    

                                            
        /**
         * From MSsyChannelDataProvider.
         */
        void GetChannelDataProviderInterfaceL( TUid aInterfaceUid, 
                                               TAny*& aInterface );
        /**
         * From MSsyPropertyProvider. Check if property value affects other sensor
         * channels already open.
         */
        void CheckPropertyDependenciesL( const TSensrvChannelId aChannelId,
                                         const TSensrvProperty& aProperty,
                                         RSensrvChannelList& aAffectedChannels );

        /**
         * From MSsyPropertyProvider. Set property for the channel.
         */
        void SetPropertyL( const TSensrvChannelId aChannelId,
                           const TSensrvProperty& aProperty );

        /**
         * From MSsyPropertyProvider. Get property for the channel.
         */
        void GetPropertyL( const TSensrvChannelId aChannelId, 
                           TSensrvProperty& aProperty );

        /**
         * From MSsyPropertyProvider. Get all properties for the channel.
         */
        void GetAllPropertiesL( const TSensrvChannelId aChannelId, 
                                RSensrvPropertyList& aChannelPropertyList );
                                
        /**
         * From MSsyPropertyProvider. Get all properties for the channel.
         */
        void GetPropertyProviderInterfaceL( TUid aInterfaceUid, 
                                            TAny*& aInterface );
                                            
        /**
         * C++ constructor.
         */
        CSSYChannel( CSSYProperty* aProperty,
                     MSsyCallback* const aCallBack,                  
                     const TSensrvChannelInfo& aChannelInfo );

        /**
         * Symbian 2nd phase constructor.
         */            
        void ConstructL();

        /**
         * Return the channelinfo of the channel.
         */
        TSensrvChannelInfo& ChannelInfo();            
    
        /**
         *   Set the state of the channel.
         */
        void SetChannelState( const TChannelState aChannelState );        

    private: // data

        /** 
         * Orientation handler. 
         */
        CSSYOrientation*   iOrientationHandler;

        /** 
         * Channel specific properties. 
         */
        CSSYProperty*      iChannelProperties;

		/** 
         * Sensor specific properties common for all the channels. 
         */
        CSSYProperty*      iSensorProperties;

        /** 
         * Ssy callback class. 
         */
        MSsyCallback*      iCallback;

        /** 
         * Channel state. 
         */
        TChannelState      iChannelState;

        /** 
         * Channelinfo. 
         */
        TSensrvChannelInfo iChannelInfo;

        /** 
         * Receive buffer for data. 
         */
        TUint8*            iBuffer;

        /** 
         * Amount of data to be received. 
         */
        TInt               iCount;

        /** 
         * Amount of data that is currently received. 
         */
        TInt               iCountReceived;

        /** 
         * Pointer into the buffer where we are writing currently. 
         */
        TUint8*            iWritePointer;

};

#endif // __SSYCHANNEL_H__

// End of File
