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
* Description:  This is the definition of the Audio Equalizer effect class.
*
*/


#ifndef CAUDIOEQUALIZER_H
#define CAUDIOEQUALIZER_H

// INCLUDES

#include <e32base.h>
#include <AudioEffectBase.h>
#include <AudioEqualizerData.h>
#include <MCustomInterface.h>


const TUid KUidAudioEqualizerEffect = {0x1020382A};

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
*  This is the Equalizer effect class for managing audio equalizer settings.
*
*  @lib AudioEqualizerEffect.lib
*  @since 3.0
*/

class CAudioEqualizer : public CAudioEffect
	{

	public:		//New Functions

	    /**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @return pointer to CAudioEqualizer object
        */
		IMPORT_C static CAudioEqualizer* NewL();

	    /**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param aUtility A reference to a convert utility
        * @return pointer to CAudioEqualizer object
        */
		IMPORT_C static CAudioEqualizer* NewL( CMdaAudioConvertUtility& aUtility );

	    /**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param aUtility A reference to an audio input stream utility
        * @return pointer to CAudioEqualizer object
        */
		IMPORT_C static CAudioEqualizer* NewL( CMdaAudioInputStream& aUtility );

	    /**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param aUtility A reference to an audio output stream utility
        * @return pointer to CAudioEqualizer object
        */
		IMPORT_C static CAudioEqualizer* NewL( CMdaAudioOutputStream& aUtility );

	    /**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param aUtility A reference to an audio player utility
        * @return pointer to CAudioEqualizer object
        */
		IMPORT_C static CAudioEqualizer* NewL( CMdaAudioPlayerUtility& aUtility );

	    /**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param aUtility A reference to an audio record utility
        * @param aRecordStream ETrue if the effect is to be applied to the recording,
        *                      EFalse if the effect is to be applied only to the playback
        * @return pointer to CAudioEqualizer object
        */
		IMPORT_C static CAudioEqualizer* NewL( CMdaAudioRecorderUtility& aUtility, TBool aRecordStream );

	    /**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param aUtility A reference to an audio tone utility
        * @return pointer to CAudioEqualizer object
        */
		IMPORT_C static CAudioEqualizer* NewL( CMdaAudioToneUtility& aUtility );

	    /**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param aDevSound A reference to a DevSound instance
        * @return pointer to CAudioEqualizer object
        */
		IMPORT_C static CAudioEqualizer* NewL( CMMFDevSound& aDevSound );

		/**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param aUtility A reference to a custom command utility
        * @return pointer to CAudioEqualizer object
        */
		IMPORT_C static CAudioEqualizer* NewL( CCustomCommandUtility* aUtility );

		/**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param aCustomInterface A reference to a custom interface
        * @return pointer to CAudioEqualizer object
        */
		IMPORT_C static CAudioEqualizer* NewL( MCustomInterface& aCustomInterface );
		
		
		/**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param CMidiClientUtility A reference to a CMidiClientUtility object
        * @return pointer to CAudioEqualizer object
        */		
		IMPORT_C static CAudioEqualizer* NewL( CMidiClientUtility& aUtility );

		/**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param CDrmPlayerUtility A reference to a CDrmPlayerUtility object
        * @return pointer to CAudioEqualizer object
        */		
		IMPORT_C static CAudioEqualizer* NewL( CDrmPlayerUtility& aUtility );

		/**
        * Factory function for creating the audio equalizer object.
        * @since 3.2
        * @param CVideoPlayerUtility A reference to a CVideoPlayerUtility object
        * @return pointer to CAudioEqualizer object
        */		
		IMPORT_C static CAudioEqualizer* NewL( CVideoPlayerUtility& aUtility );

		/**
        *
        * Destructor
        */
		IMPORT_C virtual ~CAudioEqualizer();

		/**
        * Get the band level in mB for the specified band
        * @since 3.0
        * @param aBand Frequency Band
        * @return Returns the band level in mB for the specified band
        */
		IMPORT_C TInt32 BandLevel( TUint8 aBand ) const;

		/**
        * Get the band width in Hz for the specified band.
        * @since 3.0
        * @param aBand Frequency Band
        * @return The band width in Hz for the specified band
        */
		IMPORT_C TInt32 BandWidth( TUint8 aBand ) const;

		/**
        * Get the center frequency in Hz for a given band
        * @since 3.0
        * @param aBand Frequency Band
        * @return The center frequency in Hz for a given band.
        */
		IMPORT_C TInt32 CenterFrequency( TUint8 aBand ) const;

		/**
        * Get the cross-over frequency between the given frequency band (aBand) and the next band
        * @since 3.0
        * @param aBand Frequency Band
        * @return Crossover frequency.
        */
		IMPORT_C TInt32 CrossoverFrequency( TUint8 aBand ) const;

		/**
        * Get the dB range in mB for the equalizer
        * @since 3.0
        * @param aMin Minimum level in dB
        * @param aMin Maximum level in dB
        */
		IMPORT_C void DbLevelLimits( TInt32& aMin, TInt32& aMax );

		/**
        * Get number of equalizer bands.
        * @since 3.0
        * @param -
        * @return The number of equalizer bands.
        */
		IMPORT_C TUint8 NumberOfBands() const;

		/**
        * Sets the equalizer band level value in mB, ranging from Min to Max
        * @since 3.0
        * @param aBand Frequency Band
        * @param aLevel band level in dB, ranges from DbLevelLimits()
        * @return -
        */
		IMPORT_C void SetBandLevelL( TInt8 aBand, TInt32 aLevel );

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
        * @return	-
        */
		IMPORT_C CAudioEqualizer();

		/**
		* Create a descriptor containing data for the specified band(s).
        * @since 3.0
        * @param aBand The frequency band data to package. Valid if aAllBands = EFalse.
        * @param aAllBands Indicate if data for all bands is requested.
        * @return A descriptor containing data for specified band.
        */
		IMPORT_C const TDesC8& DoBandDataL( TUint8 aBand, TBool aAllBands );

		/**
		* Unpack band data from the specified descriptor.
        * @since 3.0
        * @param aNumberOfBands Number of bands contained in the descriptor.
        * @param aBandDataBuffer Descriptor containing band data
        */
        IMPORT_C void SetBandDataL( TInt aNumberOfBands, const TDesC8& aBandDataBuffer );

	protected:

		// Equalizer data structure
		TEfAudioEqualizer iAudioEqualizerData;
		// Equalizer Band data structure
		RArray<TEfAudioEqualizerBand> iBandsData;
		// Data package sent to server
		TEfAudioEqualizerDataPckg iDataPckgTo;
		// Band Data package sent to server
		TEfAudioEqualizerBandDataPckg iBandDataPckgTo;
		// Bitmask used to indicate change in band data
		TUint32 iBandChange;

		HBufC8* iBandDataBuf;

    protected:    // Friend classes

		friend class CAudioEqualizerMessageHandler;
		friend class CAudioEqualizerUtility;

	};

#endif	// of CAUDIOEQUALIZER_H

// End of File
