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
* Description:  This is the definition of the Stereo Widening effect class.
*
*/


#ifndef CSTEREOWIDENING_H
#define CSTEREOWIDENING_H

// INCLUDES

#include <e32base.h>
#include <AudioEffectBase.h>
#include <StereoWideningData.h>
#include <MCustomInterface.h>

const TUid KUidStereoWideningEffect = {0x10203836};

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
*  This is the Stereo Widening effect class for managing audio Stereo Widening settings.
*
*  @lib StereoWidening.lib
*  @since 3.0
*/

class CStereoWidening : public CAudioEffect
    {

    public:     //New Functions

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.0
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL();

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.0
        * @param aUtility A reference to a convert utility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @param aInitialLevel Initial value of stereo widening to be used when aEnable = ETrue
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL(CMdaAudioConvertUtility& aUtility, 
                                              TBool aEnable = EFalse,
                                              TInt8 aInitialLevel = 0);

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.0
        * @param aUtility A reference to an audio input stream utility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @param aInitialLevel Initial value of stereo widening to be used when aEnable = ETrue
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL(CMdaAudioInputStream& aUtility,   
                                              TBool aEnable = EFalse,
                                              TInt8 aInitialLevel = 0);

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.0
        * @param aUtility A reference to an audio output stream utility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @param aInitialLevel Initial value of stereo widening to be used when aEnable = ETrue
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL(CMdaAudioOutputStream& aUtility, 
                                              TBool aEnable = EFalse,
                                              TInt8 aInitialLevel = 0);

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.0
        * @param aUtility A reference to an audio player utility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @param aInitialLevel Initial value of stereo widening to be used when aEnable = ETrue
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL(CMdaAudioPlayerUtility& aUtility,
                                              TBool aEnable = EFalse,
                                              TInt8 aInitialLevel = 0);

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.0
        * @param aUtility A reference to an audio record utility
        * @param aRecordStream ETrue if the effect is to be applied to the recording,
        *                      EFalse if the effect is to be applied only to the playback
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @param aInitialLevel Initial value of stereo widening to be used when aEnable = ETrue
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL(CMdaAudioRecorderUtility& aUtility,
                                              TBool aRecordStream,
                                              TBool aEnable = EFalse,
                                              TInt8 aInitialLevel = 0);

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.0
        * @param aUtility A reference to an audio tone utility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @param aInitialLevel Initial value of stereo widening to be used when aEnable = ETrue
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL(CMdaAudioToneUtility& aUtility,
                                              TBool aEnable = EFalse,
                                              TInt8 aInitialLevel = 0);

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.0
        * @param aDevSound A reference to a DevSound instance
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @param aInitialLevel Initial value of stereo widening to be used when aEnable = ETrue
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL(CMMFDevSound& aDevSound,
                                              TBool aEnable = EFalse,
                                              TInt8 aInitialLevel = 0);

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.0
        * @param aUtility A reference to a custom command utility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @param aInitialLevel Initial value of stereo widening to be used when aEnable = ETrue
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL(CCustomCommandUtility* aUtility,
                                              TBool aEnable = EFalse,
                                              TInt8 aInitialLevel = 0);

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.0
        * @param aCustomInterface A reference to a custom interface
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @param aInitialLevel Initial value of stereo widening to be used when aEnable = ETrue
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL(MCustomInterface& aCustomInterface,
                                              TBool aEnable = EFalse,
                                              TInt8 aInitialLevel = 0);

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.0
        * @param aUtility A reference to a CMidiClientUtility
        * @param aEnable Indicate whether the effect will be automatically enabled after creation.
        * @param aInitialLevel Initial value of stereo widening to be used when aEnable = ETrue
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL(CMidiClientUtility& aUtility,
                                              TBool aEnable = EFalse, 
                                              TInt8 aInitialLevel = 0);

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.0
        * @param CDrmPlayerUtility A reference to a CDrmPlayerUtility object
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL(CDrmPlayerUtility& aUtility,
                                              TBool aEnable = EFalse,
                                              TInt8 aInitialLevel = 0);

        /**
        * Factory function for creating the stereo widening object.
        * @since 3.2
        * @param CVideoPlayerUtility A reference to a CVideoPlayerUtility object
        * @return pointer to CStereoWidening object
        */
        IMPORT_C static CStereoWidening* NewL(CVideoPlayerUtility& aUtility,
                                              TBool aEnable = EFalse,
                                              TInt8 aInitialLevel = 0);

        /**
        *
        * Destructor
        */
        IMPORT_C virtual ~CStereoWidening();

        /**
        * Check if continuous level is supported for stereo widening.
        * @since 3.0
        * @return ETrue if continuous level is supported for stereo widening.  If this returns
        *         EFalse, then the adaptation is actually setting the level to the nearest supported discrete
        *         value, when SetStereoWidenlingLevelL is called.
        */
        IMPORT_C TBool IsContinuousLevelSupported() const;

        /**
        * Set stereo widening level.
        * @since 3.0
        * @param aLevel Stereo widening level ranges from 0-100, where 0 is None and 100 is Maximum widening.
        */
        IMPORT_C void SetStereoWideningLevelL( TUint8 aLevel );

        /**
        * Get the stereo widening settings.
        * @since 3.0
        * @return Level of widening
        */
        IMPORT_C TUint8 StereoWideningLevel() const;


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
        IMPORT_C CStereoWidening();

    protected:

        // Balance data structure
        TEfStereoWidening iStereoWideningData;
        // Data package sent to server
        TEfStereoWideningDataPckg iDataPckgTo;
        // Data package received from server
        TEfStereoWideningDataPckg iDataPckgFrom;

    protected:    // Friend classes
        friend class CStereoWideningMessageHandler;

    };

#endif  // of CSTEREOWIDENING_H

// End of File
