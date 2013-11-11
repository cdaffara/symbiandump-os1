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
* Description:  This is the definition of the Loudness effect class.
*
*/


#ifndef CLOUDNESS_H
#define CLOUDNESS_H

// INCLUDES

#include <e32base.h>
#include <AudioEffectBase.h>
#include <LoudnessData.h>
#include <MCustomInterface.h>

const TUid KUidLoudnessEffect = {0x10207AA8};

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
*  This is the Loudness effect class for managing audio Loudness settings.
*
*  @lib LoudnessUtility.lib
*  @since 3.0
*/

class CLoudness : public CAudioEffect
    {

    public:     //New Functions

        /**
        * Factory function for creating the audio Loudness object.
        * @since 3.0
        * @param aUtility A reference to a convert utility
        * @return pointer to CLoudness object
        */
        IMPORT_C static CLoudness* NewL( CMdaAudioConvertUtility& aUtility );

        /**
        * Factory function for creating the Loudness object.
        * @since 3.0
        * @param aUtility A reference to an audio input stream utility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @return pointer to CLoudness object
        */
        IMPORT_C static CLoudness* NewL( CMdaAudioInputStream& aUtility , TBool aEnable = EFalse );

        /**
        * Factory function for creating the Loudness object.
        * @since 3.0
        * @param aUtility A reference to an audio output stream utility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @return pointer to CLoudness object
        */
        IMPORT_C static CLoudness* NewL( CMdaAudioOutputStream& aUtility , TBool aEnable = EFalse );

        /**
        * Factory function for creating the Loudness object.
        * @since 3.0
        * @param aUtility A reference to an audio player utility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @return pointer to CLoudness object
        */
        IMPORT_C static CLoudness* NewL( CMdaAudioPlayerUtility& aUtility , TBool aEnable = EFalse );

        /**
        * Factory function for creating the Loudness object.
        * @since 3.0
        * @param aUtility A reference to an audio record utility
        * @param aRecordStream ETrue if the effect is to be applied to the recording,
        *                      EFalse if the effect is to be applied only to the playback
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @return pointer to CLoudness object
        */
        IMPORT_C static CLoudness* NewL( CMdaAudioRecorderUtility& aUtility, TBool aRecordStream, TBool aEnable = EFalse );

        /**
        * Factory function for creating the Loudness object.
        * @since 3.0
        * @param aUtility A reference to an audio tone utility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @return pointer to CLoudness object
        */
        IMPORT_C static CLoudness* NewL( CMdaAudioToneUtility& aUtility , TBool aEnable = EFalse );

        /**
        * Factory function for creating the Loudness object.
        * @since 3.0
        * @param aDevSound A reference to a DevSound instance
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @return pointer to CLoudness object
        */
        IMPORT_C static CLoudness* NewL( CMMFDevSound& aDevSound, TBool aEnable = EFalse );

        /**
        * Factory function for creating the Loudness object.
        * @since 3.0
        * @param aUtility A reference to a custom command utility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @return pointer to CLoudness object
        */
        IMPORT_C static CLoudness* NewL( CCustomCommandUtility* aUtility , TBool aEnable = EFalse );

        /**
        * Factory function for creating the Loudness object.
        * @since 3.0
        * @param aCustomInterface A reference to a custom interface
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
         * @return pointer to CLoudness object
        */
        IMPORT_C static CLoudness* NewL( MCustomInterface& aCustomInterface, TBool aEnable = EFalse );

        /**
        * Factory function for creating the Loudness object.
        * @since 3.0
        * @param aUtility A reference to a CMidiClientUtility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
         * @return pointer to CLoudness object
        */
        IMPORT_C static CLoudness* NewL( CMidiClientUtility& aUtility , TBool aEnable = EFalse );

        /**
        * Factory function for creating the Loudness object.
        * @since 3.0
        * @param CDrmPlayerUtility A reference to a CDrmPlayerUtility object
        * @return pointer to CLoudness object
        */
        IMPORT_C static CLoudness* NewL( CDrmPlayerUtility& aUtility , TBool aEnable = EFalse);

        /**
        * Factory function for creating the Loudness object.
        * @since 3.2
        * @param CVideoPlayerUtility A reference to a CVideoPlayerUtility object
        * @return pointer to CLoudness object
        */
        IMPORT_C static CLoudness* NewL( CVideoPlayerUtility& aUtility , TBool aEnable = EFalse);

        /**
        *
        * Destructor
        */
        IMPORT_C virtual ~CLoudness();

    public: // functions from base class

        /*
        * From CAudioEffect
        * Get the unique identifier of the audio effect
        * @since 3.0
        * @return Unique identifier
        */
        IMPORT_C TUid Uid() const;

    protected: // functions from base class

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
        IMPORT_C CLoudness();


    protected:

        // Loudness data structure
        TEfLoudnessData iLoudnessData;
        // Data package sent to server
        TEfLoudnessDataPckg iDataPckgTo;
        // Data package received from server
        TEfLoudnessDataPckg iDataPckgFrom;

    protected:    // Friend classes

        friend class CLoudnessMessageHandler;

    };

#endif  // of CLOUDNESS_H

// End of File
