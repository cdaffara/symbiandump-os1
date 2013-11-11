/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the definition of the Distance Attenuation effect class.
*
*/


#ifndef CDISTANCEATTENUATION_H
#define CDISTANCEATTENUATION_H

// INCLUDES

#include <e32base.h>
#include <AudioEffectBase.h>
#include <DistanceAttenuationData.h>
#include <MCustomInterface.h>


const TUid KUidDistanceAttenuationEffect = {0x1020382C};

// FORWARD DELCARATION
class CMdaAudioConvertUtility;
class CMdaAudioPlayerUtility;
class CMdaAudioRecorderUtility;
class CMdaAudioInputStream;
class CMdaAudioOutputStream;
class CMdaAudioToneUtility;
class CCustomCommandUtility;
class CCustomInterfaceUtility;
class CMMFDevSound;
class CMidiClientUtility;
class CDrmPlayerUtility;
class CVideoPlayerUtility;

// CLASS DECLARATION

/**
*  This is the Distance Attenuation effect class for managing audio Distance Attenuation settings.
*
*  @lib DistanceAttenuationEffect.lib
*  @since 3.0
*/

class CDistanceAttenuation : public CAudioEffect
    {

    public:     //New Functions

        /**
        * Factory function for creating the distance attenuation object.
        * @since 3.0
        * @param aUtility A reference to a convert utility
        * @return pointer to CDistanceAttenuation object
        */
        IMPORT_C static CDistanceAttenuation* NewL( CMdaAudioConvertUtility& aUtility );

        /**
        * Factory function for creating the distance attenuation object.
        * @since 3.0
        * @param aUtility A reference to an audio input stream utility
        * @return pointer to CDistanceAttenuation object
        */
        IMPORT_C static CDistanceAttenuation* NewL( CMdaAudioInputStream& aUtility );

        /**
        * Factory function for creating the distance attenuation object.
        * @since 3.0
        * @param aUtility A reference to an audio output stream utility
        * @return pointer to CDistanceAttenuation object
        */
        IMPORT_C static CDistanceAttenuation* NewL( CMdaAudioOutputStream& aUtility );

        /**
        * Factory function for creating the distance attenuation object.
        * @since 3.0
        * @param aUtility A reference to an audio player utility
        * @return pointer to CDistanceAttenuation object
        */
        IMPORT_C static CDistanceAttenuation* NewL( CMdaAudioPlayerUtility& aUtility );

        /**
        * Factory function for creating the distance attenuation object.
        * @since 3.0
        * @param aUtility A reference to an audio record utility
        * @param aRecordStream ETrue if the effect is to be applied to the recording,
        *                      EFalse if the effect is to be applied only to the playback
        * @return pointer to CDistanceAttenuation object
        */
        IMPORT_C static CDistanceAttenuation* NewL( CMdaAudioRecorderUtility& aUtility, TBool aRecordStream );

        /**
        * Factory function for creating the distance attenuation object.
        * @since 3.0
        * @param aDevSound A reference to a DevSound instance
        * @return pointer to CDistanceAttenuation object
        */
        IMPORT_C static CDistanceAttenuation* NewL( CMMFDevSound& aDevSound );

        /**
        * Factory function for creating the distance attenuation object.
        * @since 3.0
        * @param aUtility A reference to an audio tone utility
        * @return pointer to CDistanceAttenuation object
        */
        IMPORT_C static CDistanceAttenuation* NewL( CMdaAudioToneUtility& aUtility );

        /**
        * Factory function for creating the distance attenuation object.
        * @since 3.0
        * @param aUtility A reference to a custom command utility
        * @return pointer to CDistanceAttenuation object
        */
        IMPORT_C static CDistanceAttenuation* NewL( CCustomCommandUtility* aUtility );

        /**
        * Factory function for creating the distance attenuation object.
        * @since 3.0
        * @param aCustomInterface A reference to a custom interface
        * @return pointer to CDistanceAttenuation object
        */
        IMPORT_C static CDistanceAttenuation* NewL( MCustomInterface& aCustomInterface );

        /**
        * Factory function for creating the distance attenuation object.
        * @since 3.0
        * @param aUtility A reference to a CMidiClientUtility
        * @return pointer to CDistanceAttenuation object
        */
        IMPORT_C static CDistanceAttenuation* NewL( CMidiClientUtility& aUtility );


        /**
        * Factory function for creating the distance attenuation object.
        * @since 3.0
        * @param CDrmPlayerUtility A reference to a CDrmPlayerUtility object
        * @return pointer to CDistanceAttenuation object
        */
        IMPORT_C static CDistanceAttenuation* NewL( CDrmPlayerUtility& aUtility );

        /**
        * Factory function for creating the distance attenuation object.
        * @since 3.2
        * @param CVideoPlayerUtility A reference to a CVideoPlayerUtility object
        * @return pointer to CDistanceAttenuation object
        */
        IMPORT_C static CDistanceAttenuation* NewL( CVideoPlayerUtility& aUtility );

        /**
        *
        * Destructor
        */
        IMPORT_C virtual ~CDistanceAttenuation();

        /**
        * Gets the Distance Attenuation of the listener.
        * @since 3.0
        * @param aRMin The source-to-listener distance below which the sound level is constant
        * @param aRMax The source-to-listener distance above which the sound level is constant or zero
        * @param aMuteAfterMax Indicate if the sound is muted beyond RMax
        * @param aRollOffFactor Multiplier factor to source-to-listener distance
        * @param aRoomRollOffFactor Multiplier factor to source-to-listener distance for Room effect.
        * @return -
        */
        IMPORT_C void DistanceAttenuation( TInt32& aRMin, TInt32& aRMax, TBool& aMuteAfterMax,
                                           TUint32& aRollOffFactor, TUint32& aRoomRollOffFactor );

        /**
        * Get roll off factor maximum value.
        * @since 3.0
        * @param aRollOffFactor The maximum multiplier factor to source-to-listener distance for Room effect.
        * @return -
        */
        IMPORT_C void RollOffFactorMax( TUint32& aRollOfFactorMax );

        /**
        * Get room roll off factor maximum value.
        * @since 3.0
        * @param aRollOffFactor The maximum multiplier factor to source-to-listener distance for Room effect.
        * @return -
        */
        IMPORT_C void RoomRollOffFactorMax( TUint32& aRoomRollOfFactorMax );

        /**
        * Sets the Distance Attenuation
        * @since 3.0
        * @param aRMin The source-to-listener distance below which the sound level is constant
        * @param aRMax The source-to-listener distance above which the sound level is constant or zero
        * @param aMuteAfterMax Indicate if the sound is muted beyond RMax
        * @param aRollOffFactor Multiplier factor to source-to-listener distance
        * @param aRoomRollOffFactor Multiplier factor to source-to-listener distance for Room effect.
        * @return -
        */
        IMPORT_C void SetDistanceAttenuationL( TInt32 aRMin, TInt32 aRMax, TBool aMuteAfterMax,
                                               TUint32 aRollOffFactor, TUint32 aRoomRollOffFactor );

    public: // functions from base class

        /*
        * From CAudioEffect
        * Get the unique identifier of the audio effect
        * @since 3.0
        * @return Unique identifier
        */
        IMPORT_C TUid Uid() const;

    protected:  // Functions from base classes

        /**
        * From CAudioEffect
        * Create a package of the effect data
        * @since 3.0
        * @return A descriptor containing the effect data.
        */
        IMPORT_C const TDesC8& DoEffectData();

        /**
        * From CAudioEffect
        * Internal function to unpack effect data
        * @since 3.0
        * @param aEffectDataBuffer Descriptor containing packed effect data
        * @return -
        */
        IMPORT_C void SetEffectData( const TDesC8& aEffectDataBuffer );

    protected:

        /**
        * Private C++ constructor for this class.
        * @since    3.0
        * @param    aEffectObserver reference to event observer object
        * @return   -
        */
        IMPORT_C CDistanceAttenuation();

    protected:

        // Distance Attenuation data structure
        TEfDistanceAttenuation iDistanceAttenuationData;
        // Data package sent to server
        TEfDistanceAttenuationDataPckg iDataPckgTo;
        // Data package received from server
        TEfDistanceAttenuationDataPckg iDataPckgFrom;

   protected:    // Friend classes

        friend class CDistanceAttenuationMessageHandler;

    };

#endif  // of CDISTANCEATTENUATION_H

// End of File
