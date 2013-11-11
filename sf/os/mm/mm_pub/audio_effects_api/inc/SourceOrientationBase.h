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
* Description:  This is the definition of the source orientation effect class.
*
*/


#ifndef CSOURCEORIENTATION_H
#define CSOURCEORIENTATION_H

// INCLUDES

#include <e32base.h>
#include <OrientationBase.h>
#include <MCustomInterface.h>

const TUid KUidSourceOrientationEffect = {0x10203834};

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
*  This is the source orientation effect class for managing source orientation settings.
*
*  @lib SourceOrientationEffect.lib
*  @since 3.0
*/

class CSourceOrientation : public COrientation
    {

    public:     // Constructors and Destructor

        /**
        * Factory function for creating the source orientation object.
        * @since 3.0
        * @param aDevSound A reference to a DevSound instance
        * @return pointer to CSourceOrientation object
        */
        IMPORT_C static CSourceOrientation* NewL( CMMFDevSound& aDevSound );

        /**
        * Factory function for creating the source orientation object.
        * @since 3.0
        * @param aUtility A reference to a convert utility
        * @return pointer to CSourceOrientation object
        */
        IMPORT_C static CSourceOrientation* NewL( CMdaAudioConvertUtility& aUtility );

        /**
        * Factory function for creating the source orientation object.
        * @since 3.0
        * @param aUtility A reference to an audio input stream utility
        * @return pointer to CSourceOrientation object
        */
        IMPORT_C static CSourceOrientation* NewL( CMdaAudioInputStream& aUtility );

        /**
        * Factory function for creating the source orientation object.
        * @since 3.0
        * @param aUtility A reference to an audio output stream utility
        * @return pointer to CSourceOrientation object
        */
        IMPORT_C static CSourceOrientation* NewL( CMdaAudioOutputStream& aUtility );

        /**
        * Factory function for creating the source orientation object.
        * @since 3.0
        * @param aUtility A reference to an audio player utility
        * @return pointer to CSourceOrientation object
        */
        IMPORT_C static CSourceOrientation* NewL( CMdaAudioPlayerUtility& aUtility );

        /**
        * Factory function for creating the source orientation object.
        * @since 3.0
        * @param aUtility A reference to an audio record utility
        * @param aRecordStream ETrue if the effect is to be applied to the recording,
        *                      EFalse if the effect is to be applied only to the playback
        * @return pointer to CSourceOrientation object
        */
        IMPORT_C static CSourceOrientation* NewL( CMdaAudioRecorderUtility& aUtility, TBool aRecordStream );

        /**
        * Factory function for creating the source orientation object.
        * @since 3.0
        * @param aUtility A reference to an audio tone utility
        * @return pointer to CSourceOrientation object
        */
        IMPORT_C static CSourceOrientation* NewL( CMdaAudioToneUtility& aUtility );

        /**
        * Factory function for creating the source orientation object.
        * @since 3.0
        * @param aUtility A reference to a custom command utility
        * @return pointer to CSourceOrientation object
        */
        IMPORT_C static CSourceOrientation* NewL( CCustomCommandUtility* aUtility );

        /**
        * Factory function for creating the source orientation object.
        * @since 3.0
        * @param aCustomInterface A reference to a custom interface
        * @return pointer to CSourceOrientation object
        */
        IMPORT_C static CSourceOrientation* NewL( MCustomInterface& aCustomInterface );

        /**
        * Factory function for creating the source orientation object.
        * @since 3.0
        * @param aUtility A reference to a CMidiClientUtility
        * @return pointer to CSourceOrientation object
        */
        IMPORT_C static CSourceOrientation* NewL( CMidiClientUtility& aUtility );

        /**
        * Factory function for creating the source orientation object.
        * @since 3.0
        * @param CDrmPlayerUtility A reference to a CDrmPlayerUtility object
        * @return pointer to CSourceOrientation object
        */
        IMPORT_C static CSourceOrientation* NewL( CDrmPlayerUtility& aUtility );

        /**
        * Factory function for creating the source orientation object.
        * @since 3.2
        * @param CVideoPlayerUtility A reference to a CVideoPlayerUtility object
        * @return pointer to CSourceOrientation object
        */
        IMPORT_C static CSourceOrientation* NewL( CVideoPlayerUtility& aUtility );

        /**
        *
        * Destructor
        */
        IMPORT_C virtual ~CSourceOrientation();


    public: // functions from base class

        /*
        * From CAudioEffect
        * Get the unique identifier of the audio effect
        * @since 3.0
        * @return Unique identifier
        */
        IMPORT_C TUid Uid() const;

    protected:

        /**
        * Private C++ constructor for this class.
        * @since    3.0
        * @param    aEffectObserver reference to event observer object
        * @return   -
        */
        IMPORT_C CSourceOrientation();

    protected:    // Friend classes

        friend class CSourceOrientationMessageHandler;

    };

#endif  // of CSOURCEORIENTATION_H

// End of File
