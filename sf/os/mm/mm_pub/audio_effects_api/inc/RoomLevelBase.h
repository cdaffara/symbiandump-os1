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
* Description:  This is the definition of the RoomLevel effect class.
*
*/


#ifndef CROOMLEVEL_H
#define CROOMLEVEL_H

// INCLUDES

#include <e32base.h>
#include <AudioEffectBase.h>
#include <RoomLevelData.h>
#include <MCustomInterface.h>

const TUid KUidRoomLevelEffect = {0x10207B3C};

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
class  CMidiClientUtility;
class CEnvironmentalReverb;
class CDrmPlayerUtility;
class CVideoPlayerUtility;

// CLASS DECLARATION

/**
*  This is the Environmental RoomLevel effect class for managing RoomLevel settings.
*
*  @lib RoomLevelEffect.lib
*  @since 3.0
*/

class CRoomLevel : public CAudioEffect
    {

    public:     //New Functions
        /**
        * Factory function for creating the audio RoomLevel object.
        * @since 3.0
        * @param aUtility A reference to a convert utility
        * @param aReverb A reference to a environmental reverb utility
        * @return pointer to CRoomLevel object
        */
        IMPORT_C static CRoomLevel* NewL( CMdaAudioConvertUtility& aUtility, CEnvironmentalReverb& aReverb);

        /**
        * Factory function for creating the audio RoomLevel object.
        * @since 3.0
        * @param aUtility A reference to an audio input stream utility
        * @param aReverb A reference to a environmental reverb utility
        * @return pointer to CRoomLevel object
        */
        IMPORT_C static CRoomLevel* NewL( CMdaAudioInputStream& aUtility, CEnvironmentalReverb& aReverb );

        /**
        * Factory function for creating the audio RoomLevel object.
        * @since 3.0
        * @param aUtility A reference to an audio output stream utility
        * @param aReverb A reference to a environmental reverb utility
        * @return pointer to CRoomLevel object
        */
        IMPORT_C static CRoomLevel* NewL( CMdaAudioOutputStream& aUtility, CEnvironmentalReverb& aReverb);

        /**
        * Factory function for creating the audio RoomLevel object.
        * @since 3.0
        * @param aUtility A reference to an audio player utility
        * @param aReverb A reference to a environmental reverb utility
        * @return pointer to CRoomLevel object
        */
        IMPORT_C static CRoomLevel* NewL( CMdaAudioPlayerUtility& aUtility, CEnvironmentalReverb& aReverb );

        /**
        * Factory function for creating the audio RoomLevel object.
        * @since 3.0
        * @param aUtility A reference to an audio record utility
        * @param aRecordStream ETrue if the effect is to be applied to the recording,
        *                      EFalse if the effect is to be applied only to the playback
        * @param aReverb A reference to a environmental reverb utility
        * @return pointer to CRoomLevel object
        */
        IMPORT_C static CRoomLevel* NewL( CMdaAudioRecorderUtility& aUtility, TBool aRecordStream, CEnvironmentalReverb& aReverb);

        /**
        * Factory function for creating the audio RoomLevel object.
        * @since 3.0
        * @param aUtility A reference to an audio tone utility
        * @param aReverb A reference to a environmental reverb utility
        * @return pointer to CRoomLevel object
        */
        IMPORT_C static CRoomLevel* NewL( CMdaAudioToneUtility& aUtility, CEnvironmentalReverb& aReverb );

        /**
        * Factory function for creating the audio RoomLevel object.
        * @since 3.0
        * @param aDevSound A reference to a DevSound instance
        * @param aReverb A reference to a environmental reverb utility
        * @return pointer to CRoomLevel object
        */
        IMPORT_C static CRoomLevel* NewL( CMMFDevSound& aDevSound, CEnvironmentalReverb& aReverb);

        /**
        * Factory function for creating the audio RoomLevel object.
        * @since 3.0
        * @param aUtility A reference to a custom command utility
        * @param aReverb A reference to a environmental reverb utility
        * @return pointer to CRoomLevel object
        */
        IMPORT_C static CRoomLevel* NewL( CCustomCommandUtility* aUtility, CEnvironmentalReverb& aReverb );

        /**
        * Factory function for creating the audio RoomLevel object.
        * @since 3.0
        * @param aCustomInterface A reference to a custom interface
        * @param aReverb A reference to a environmental reverb utility
        * @return pointer to CRoomLevel object
        */
        IMPORT_C static CRoomLevel* NewL( MCustomInterface& aCustomInterface, CEnvironmentalReverb& aReverb );

        /**
        * Factory function for creating the audio RoomLevel object.
        * @since 3.0
        * @param aUtility A reference to a CMidiClientUtility object
        * @param aReverb A reference to a environmental reverb utility
        * @return pointer to CRoomLevel object
        */
        IMPORT_C static CRoomLevel* NewL( CMidiClientUtility& aUtility, CEnvironmentalReverb& aReverb);

        /**
        * Factory function for creating the RoomLevel object.
        * @since 3.0
        * @param CDrmPlayerUtility A reference to a CDrmPlayerUtility object
        * @return pointer to CRoomLevel object
        */
        IMPORT_C static CRoomLevel* NewL( CDrmPlayerUtility& aUtility, CEnvironmentalReverb& aReverb );

        /**
        * Factory function for creating the RoomLevel object.
        * @since 3.2
        * @param CVideoPlayerUtility A reference to a CVideoPlayerUtility object
        * @return pointer to CRoomLevel object
        */
        IMPORT_C static CRoomLevel* NewL( CVideoPlayerUtility& aUtility, CEnvironmentalReverb& aReverb );

        /**
        *
        * Destructor
        */
        IMPORT_C virtual ~CRoomLevel();

        /**
        * Gets the RoomLevel current level in mB
        * @since 3.0
        * @return RoomLevel current level
        */
        IMPORT_C TInt32 Level() const;

        /**
        * Gets the RoomLevel current level maximum and minimum in mB
        * @since 3.0
        * @param aMin Minimum current level
        * @param aMax Maximum current level
        * @return -
        */
        IMPORT_C void LevelRange( TInt32& aMin, TInt32& aMax );

        /**
        * Sets the RoomLevel level, it will leave if aRoomLevel is not within range of Min and Max
        * @since 3.0
        * @param aRoomLevelLevel The RoomLevel level in mB
        */
        IMPORT_C void SetRoomLevelL( TInt32 aRoomLevel );


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
        * @since 3.0
        * @return   -
        */
        IMPORT_C CRoomLevel();

    protected:
        CEnvironmentalReverb* iClientReverb;
        // RoomLevel data structure
        TEfRoomLevel iRoomLevelData;
        // Data package sent to server
        TEfRoomLevelDataPckg iDataPckgTo;
        // Data package received from server
        TEfRoomLevelDataPckg iDataPckgFrom;

    friend class CRoomLevelMessageHandler;
//    friend class CEnvironmentalReverbImpl;

    };

#endif  // of CROOMLEVEL_H

// End of File
