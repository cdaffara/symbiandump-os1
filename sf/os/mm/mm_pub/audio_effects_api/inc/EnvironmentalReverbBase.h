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
* Description:  This is the definition of the Environmental Reverb effect class.
*
*/


#ifndef CENVIRONMENTALREVERB_H
#define CENVIRONMENTALREVERB_H

// INCLUDES

#include <e32base.h>
#include <AudioEffectBase.h>
#include <EnvironmentalReverbData.h>
#include <MCustomInterface.h>

const TUid KUidEnvironmentalReverbEffect = {0x10203837};

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
*  This is the Environmental Reverb effect class for managing reverb settings.
*
*  @lib EnvironmentalReverbEffect.lib
*  @since 3.0
*/

class CEnvironmentalReverb : public CAudioEffect
    {

    public:     //New Functions

        /**
        * Factory function for creating the audio reverb object.
        * @since 3.0
        * @return pointer to CEnvironmentalReverb object
        */
        IMPORT_C static CEnvironmentalReverb* NewL();

        /**
        * Factory function for creating the audio reverb object.
        * @since 3.0
        * @param aUtility A reference to a convert utility
        * @return pointer to CEnvironmentalReverb object
        */
        IMPORT_C static CEnvironmentalReverb* NewL( CMdaAudioConvertUtility& aUtility );

        /**
        * Factory function for creating the audio reverb object.
        * @since 3.0
        * @param aUtility A reference to an audio input stream utility
        * @return pointer to CEnvironmentalReverb object
        */
        IMPORT_C static CEnvironmentalReverb* NewL( CMdaAudioInputStream& aUtility );

        /**
        * Factory function for creating the audio reverb object.
        * @since 3.0
        * @param aUtility A reference to an audio output stream utility
        * @return pointer to CEnvironmentalReverb object
        */
        IMPORT_C static CEnvironmentalReverb* NewL( CMdaAudioOutputStream& aUtility );

        /**
        * Factory function for creating the audio reverb object.
        * @since 3.0
        * @param aUtility A reference to an audio player utility
        * @return pointer to CEnvironmentalReverb object
        */
        IMPORT_C static CEnvironmentalReverb* NewL( CMdaAudioPlayerUtility& aUtility );

        /**
        * Factory function for creating the audio reverb object.
        * @since 3.0
        * @param aUtility A reference to an audio record utility
        * @param aRecordStream ETrue if the effect is to be applied to the recording,
        *                      EFalse if the effect is to be applied only to the playback
        * @return pointer to CEnvironmentalReverb object
        */
        IMPORT_C static CEnvironmentalReverb* NewL( CMdaAudioRecorderUtility& aUtility, TBool aRecordStream );

        /**
        * Factory function for creating the audio reverb object.
        * @since 3.0
        * @param aUtility A reference to an audio tone utility
        * @return pointer to CEnvironmentalReverb object
        */
        IMPORT_C static CEnvironmentalReverb* NewL( CMdaAudioToneUtility& aUtility );

        /**
        * Factory function for creating the audio reverb object.
        * @since 3.0
        * @param aDevSound A reference to a DevSound instance
        * @return pointer to CEnvironmentalReverb object
        */
        IMPORT_C static CEnvironmentalReverb* NewL( CMMFDevSound& aDevSound );

        /**
        * Factory function for creating the audio reverb object.
        * @since 3.0
        * @param aUtility A reference to a custom command utility
        * @return pointer to CEnvironmentalReverb object
        */
        IMPORT_C static CEnvironmentalReverb* NewL( CCustomCommandUtility* aUtility );

        /**
        * Factory function for creating the audio reverb object.
        * @since 3.0
        * @param aCustomInterface A reference to a custom interface
        * @return pointer to CEnvironmentalReverb object
        */
        IMPORT_C static CEnvironmentalReverb* NewL( MCustomInterface& aCustomInterface );

        /**
        * Factory function for creating the audio reverb object.
        * @since 3.0
        * @param aUtility A reference to a CMidiClientUtility object
        * @return pointer to CEnvironmentalReverb object
        */
        IMPORT_C static CEnvironmentalReverb* NewL( CMidiClientUtility& aUtility );

        /**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param CDrmPlayerUtility A reference to a CDrmPlayerUtility object
        * @return pointer to CAudioEqualizer object
        */
        IMPORT_C static CEnvironmentalReverb* NewL( CDrmPlayerUtility& aUtility );

        /**
        * Factory function for creating the audio equalizer object.
        * @since 3.2
        * @param CVideoPlayerUtility A reference to a CVideoPlayerUtility object
        * @return pointer to CAudioEqualizer object
        */
        IMPORT_C static CEnvironmentalReverb* NewL( CVideoPlayerUtility& aUtility );

        /**
        *
        * Destructor
        */
        IMPORT_C virtual ~CEnvironmentalReverb();

        /**
        * Gets the decay HF Ratio in hundredths
        * @since 3.0
        * @return decay HF Ratio
        */
        IMPORT_C TUint32 DecayHFRatio() const;

        /**
        * Gets the reverb decay HF Ratio minimum and maximum in hundredths.
        * @since 3.0
        * @param aMin Minimum decay HF Ratio
        * @param aMax Maximum decay HF Ratio
        */
        IMPORT_C void DecayHFRatioRange(TUint32& aMin, TUint32& aMax);

        /**
        * Gets the decay time in milliseconds
        * @since 3.0
        * @return decay time
        */
        IMPORT_C TUint32 DecayTime() const;

        /**
        * Gets the allowable reverb decay time range in milliseconds.
        * @since 3.0
        * @param aMin Minimum decay time in milliseconds
        * @param aMax Maximum decay time in milliseconds
        */
        IMPORT_C void DecayTimeRange(TUint32& aMin, TUint32& aMax);

        /**
        * Gets the density current value as a percentage in hundredths
        * @since 3.0
        * @return density value
        */
        IMPORT_C TUint32 Density() const;

        /**
        * Gets the diffusion current value as a percentage in hundredths.
        * @since 3.0
        * @return diffusion value
        */
        IMPORT_C TUint32 Diffusion() const;

        /**
        * Gets the reverb reflections delay in ms.
        * @since 3.0
        * @return reverb reflections delay
        */
        IMPORT_C TUint32 ReflectionsDelay() const;

        /**
        * Gets the reverb reflections delay maximum in milliseconds.
        * @since 3.0
        * @return reverb reflections delay maximum
        */
        IMPORT_C TUint32 ReflectionsDelayMax() const;

        /**
        * Gets the reverb reflections level in mB
        * @since 3.0
        * @return Reverb reflections level
        */
        IMPORT_C TInt32 ReflectionsLevel() const;

        /**
        * Gets the reverb reflections level maximum and minimum in mB
        * @since 3.0
        * @param aMin Minimum reflections level
        * @param aMax Maximum reflections level
        */
        IMPORT_C void ReflectionLevelRange( TInt32& aMin, TInt32& aMax );

        /**
        * Gets the reverb delay in milliseconds
        * @since 3.0
        * @return reverb delay
        */
        IMPORT_C TUint32 ReverbDelay() const;

        /**
        * Gets the reverb delay maximum in milliseconds
        * @since 3.0
        * @return reverb delay maximum
        */
        IMPORT_C TUint32 ReverbDelayMax() const;

        /**
        * Gets the reverb current level in mB
        * @since 3.0
        * @return reverb current level
        */
        IMPORT_C TInt32 ReverbLevel() const;

        /**
        * Gets the reverb current level maximum and minimum in mB
        * @since 3.0
        * @param aMin Minimum current level
        * @param aMax Maximum current level
        * @return -
        */
        IMPORT_C void ReverbLevelRange( TInt32& aMin, TInt32& aMax );

        /**
        * Gets the room HF level current ratio
        * @since 3.0
        * @return room HF level ratio
        */
        IMPORT_C TInt32 RoomHFLevel() const;

        /**
        * Gets the room HF level maximum and minimum ratios
        * @since 3.0
        * @param aMin Minimum current room HF level
        * @param aMax Maximum current room HF level
        * @return -
        */
        IMPORT_C void RoomHFLevelRange( TInt32& aMin, TInt32& aMax );

        /**
        * Gets the room level current value in mB
        * @since 3.0
        * @return room level value
        */
        IMPORT_C TInt32 RoomLevel() const;

        /**
        * Gets the room level maximum and minimum in mB
        * @since 3.0
        * @param aMin Minimum current room level
        * @param aMax Maximum current room level
        * @return -
        */
        IMPORT_C void RoomLevelRange( TInt32& aMin, TInt32& aMax );

        /**
        * Sets the decay HF Ratio in hundredths
        * @since 3.0
        * @param aDecayHFRatio The decay high frequence ratio in hundredths
        * @return -
        */
        IMPORT_C void SetDecayHFRatioL( TUint32 aDecayHFRatio );

        /**
        * Sets the decay time in millisecond
        * @since 3.0
        * @param aDecayTime Decay time in ms
        */
        IMPORT_C void SetDecayTimeL( TUint32 aDecayTime );

        /**
        * Sets the density value as percentage in hundredths
        * @since 3.0
        * @param aDensity The density.
        */
        IMPORT_C void SetDensityL( TUint32 aDensity );

        /**
        * Sets the diffusion value as a percentage in hundredths
        * @since 3.0
        * @param aDiffusion The diffusion.
        */
        IMPORT_C void SetDiffusionL( TUint32 aDiffusion );

        /**
        * Sets the reverb reflections delay
        * @since 3.0
        * @param aRefectionsDelay The reflection delay in ms.
        */
        IMPORT_C void SetReflectionsDelayL( TUint32 aReflectionsDelay );

        /**
        * Sets the reverb reflections level in milli-dB
        * @since 3.0
        * @param aRefectionsLevel The reflection level in mB
        */
        IMPORT_C void SetReflectionsLevelL( TInt32 aReflectionsLevel );

        /**
        * Sets the reverb delay
        * @since 3.0
        * @param aReverbDelay The reverb delay in ms
        */
        IMPORT_C void SetReverbDelayL( TUint32 aReverbDelay );

        /**
        * Sets the reverb level
        * @since 3.0
        * @param aReverbLevel The reverb level in mB
        */
        IMPORT_C void SetReverbLevelL( TInt32 aReverbLevel );

        /**
        * Sets the room HF level ratio
        * @since 3.0
        * @param aRoomHFLevel The room high frequency ratio
        */
        IMPORT_C void SetRoomHFLevelL( TInt32 aRoomHFLevel );

        /**
        * Sets the room level value in milli-dB
        * @since 3.0
        * @param aRoomLevel The room level
        */
        IMPORT_C void SetRoomLevelL( TInt32 aRoomLevel );

        /**
        * Gets the total delay maximum in milliseconds
        * @since 3.0
        * @return reverb delay maximum
        */
        IMPORT_C TUint32 DelayMax() const;

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
        IMPORT_C CEnvironmentalReverb();

    protected:

        // Reverb data structure
        TEfEnvironmentalReverb iReverbData;
        // Data package sent to server
        TEfEnvReverbDataPckg iDataPckgTo;
        // Data package received from server
        TEfEnvReverbDataPckg iDataPckgFrom;

    protected:    // Friend classes

        friend class CEnvironmentalReverbMessageHandler;
        friend class CEnvironmentalReverbUtility;

    };

#endif  // of CENVIRONMENTALREVERB_H

// End of File
