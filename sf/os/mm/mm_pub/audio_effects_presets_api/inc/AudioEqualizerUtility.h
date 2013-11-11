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
* Description:  This is the definition of the audio equalizer utility class.
*
*/



#ifndef CAUDIOEQUALIZERUTILITY_H
#define CAUDIOEQUALIZERUTILITY_H

//  INCLUDES
#include <e32base.h>
#include <e32svr.h>
#include <AudioEqualizerBase.h>
#include <BassBoostBase.h>
#include <centralrepository.h>
#include <AudioEqualizerUtilityData.h>

// FORWARD DECLARATIONS
class TEfAudioEqualizerUtilityPreset;
class CAudioEqualizer;
class CBassBoost;
class CMdaAudioConvertUtility;
class CMdaAudioPlayerUtility;
class CMdaAudioRecorderUtility;
class CMdaAudioInputStream;
class CMdaAudioOutputStream;
class CMdaAudioToneUtility;
class CCustomCommandUtility;
class CCustomInterfaceUtility;
class CMMFDevSound;
class CRepository;
class CMidiClientUtility;
class CDrmPlayerUtility;
class CVideoPlayerUtility;

// CLASS DECLARATION

/**
*  This is the Equalizer Utility class for managing audio equalizer presets.
*
*  @lib AudioEqualizerUtility.lib
*  @since 3.0
*/

class CAudioEqualizerUtility : public CBase
    {
    public:  // Constructors and destructor


        /**
		* Factory function for creating the audio equalizer utility object.
		* @since 3.0
		* @param aUtility A reference to a Devsound Instance
		* @return pointer to CAudioEqualizerUtility object
        */
        IMPORT_C static CAudioEqualizerUtility* NewL(CMMFDevSound& aDevSound);

		/**
        * Factory function for creating the audio equalizer utility object.
        * @since 3.0
        * @param aUtility A reference to a convert utility
        * @return pointer to CAudioEqualizerUtility object
        */
		IMPORT_C static CAudioEqualizerUtility* NewL(CMdaAudioConvertUtility& aUtility);

		/**
        * Factory function for creating the audio equalizer utility object.
        * @since 3.0
        * @param aUtility A reference to a audio input stream utility
        * @return pointer to CAudioEqualizerUtility object
        */
		IMPORT_C static CAudioEqualizerUtility* NewL(CMdaAudioInputStream& aUtility);

		/**
		* Factory function for creating the audio equalizer utility object.
		* @since 3.0
		* @param aUtility A reference to a audio output stream utility
		* @return pointer to CAudioEqualizerUtility object
        */
		IMPORT_C static CAudioEqualizerUtility* NewL(CMdaAudioOutputStream& aUtility);

		/**
		* Factory function for creating the audio equalizer utility object.
		* @since 3.0
		* @param aUtility A reference to a audio player utility
		* @return pointer to CAudioEqualizerUtility object
        */
		IMPORT_C static CAudioEqualizerUtility* NewL(CMdaAudioPlayerUtility& aUtility);

		/**
		* Factory function for creating the audio equalizer utility object.
		* @since 3.0
		* @param aUtility A reference to a audio recorder utility
		* @return pointer to CAudioEqualizerUtility object
        */
		IMPORT_C static CAudioEqualizerUtility* NewL(CMdaAudioRecorderUtility& aUtility);

		/**
		* Factory function for creating the audio equalizer utility object.
		* @since 3.0
		* @param aUtility A reference to a audio tone utility
		* @return pointer to CAudioEqualizerUtility object
        */
		IMPORT_C static CAudioEqualizerUtility* NewL(CMdaAudioToneUtility& aUtility);

		/**
		* Factory function for creating the audio equalizer utility object.
		* @since 3.0
		* @param aUtility A reference to custom command utility
		* @return pointer to CAudioEqualizerUtility object
        */
		IMPORT_C static CAudioEqualizerUtility* NewL(CCustomCommandUtility* aUtility);

		/**
		* Factory function for creating the audio equalizer utility object.
		* @since 3.0
		* @param aUtility A reference to custom interface utility
		* @return pointer to CAudioEqualizerUtility object
        */
		IMPORT_C static CAudioEqualizerUtility* NewL(MCustomInterface& aCustomInterface);
		
		/**
		* Factory function for creating the audio equalizer utility object.
		* @since 3.0
		* @param aUtility A reference to CMidiClientUtility
		* @return pointer to CAudioEqualizerUtility object
        */
		IMPORT_C static CAudioEqualizerUtility* NewL(CMidiClientUtility& aUtility);
		
		/**
        * Factory function for creating the audio equalizer object.
        * @since 3.0
        * @param CDrmPlayerUtility A reference to a CDrmPlayerUtility object
        * @return pointer to CAudioEqualizer object
        */		
		IMPORT_C static CAudioEqualizerUtility* NewL( CDrmPlayerUtility& aUtility );

		/**
        * Factory function for creating the audio equalizer object.
        * @since 3.2
        * @param CVideoPlayerUtility A reference to a CVideoPlayerUtility object
        * @return pointer to CAudioEqualizer object
        */		
		IMPORT_C static CAudioEqualizerUtility* NewL( CVideoPlayerUtility& aUtility );

		 /**
        * Destructor.
        */
        virtual ~CAudioEqualizerUtility();

		/**
        * Apply the Preset Index value
        * @since 3.0
        * @param aPreset Preset Value
        */
		IMPORT_C void ApplyPresetL(TInt aPreset);

		/**
        * Disable the Equalizer
        * @since 3.0
        */
		IMPORT_C void DisableEqualizerL();

		/**
        * Create a Preset with the given Name and Equalizer Settings
        * @since 3.0
        * @param aName Name of the Preset
        * @param aAudioEqualizer Equalizer Settings for the Preset
        */
		IMPORT_C void CreatePresetL(TDesC& aName, CAudioEqualizer& aAudioEqualizerUI);

		/**
        * Delete a Preset with the given index in the Central Repository
        * @since 3.0
        * @param aPresetIndex Index of the Preset in the Array
        */
		IMPORT_C void DeletePresetL(TInt aPresetIndex);

		/**
        * Retrieves a Preset with the given index from the Central Repository
        * @since 3.0
        * @param aPresetIndex Index of the Preset in the Array
        */
		IMPORT_C  const TDesC& GetPresetL(TInt aPresetIndex);

		/**
        * Get the reference to the Equalizer Object
        * @since 3.0
        * @return Reference to the Equalizer Object.
        */
		IMPORT_C CAudioEqualizer& Equalizer();

		/**
		* Get the reference to the BassBoost Object
		* @since 3.0
		* @return Reference to the CBassBoost Object.
		*/
		IMPORT_C CBassBoost& BassBoost();

		/**
        * Modify a Preset with the given Name and Equalizer Settings
        * @since 3.0
        * @param aPresetIndex Array Index of the Preset
        * @param aName Name of the Preset
        * @param aAudioEqualizer Equalizer Settings for the Preset
        */
		IMPORT_C void ModifyPresetL(TInt aPresetIndex, TDesC& aName, CAudioEqualizer& aAudioEqualizerUI);

		/**
        * Get the number of Pre Defined Presets
        * @since 3.0
        * @return Number of Pre Defined Presets
        */
		IMPORT_C TUint32 NumberOfPreDefinedPresets() const;

		/**
        * Get all the Presets defined in the Central Repository
        * @since 3.0
        * @return Array of all Presets.
        */
		IMPORT_C TArray<TEfAudioEqualizerUtilityPreset> Presets();

		/**
		 * Reset a System Defined Preset with the given index in the Array returned from Presets()
		 * @since 3.0
		 * @param aPresetIndex Index of the Preset in the Array
		 */
		IMPORT_C void ResetPresetL(TInt aPresetIndex);

    private:

        /**
        * C++ default constructor.
        */
        CAudioEqualizerUtility();

        /**
        * By default Symbian 2nd phase constructor is private.
        */

        /**
		* Second Phase Costructor for DevSound Instance
		* @since 3.0
		* @param aDevSound DevSound Referece
		*/
        void ConstructL(CMMFDevSound& aDevSound);

		/**
		* Second Phase Costructor for Audio Convert Utility
		* @since 3.0
		* @param aUtility CMdaAudioConvertUtility Referece
		*/
		void ConstructL(CMdaAudioConvertUtility& aUtility);

		/**
		* Second Phase Costructor for CMdaAudioInputStream
		* @since 3.0
		* @param aUtility CMdaAudioInputStream Referece
		*/
		void ConstructL(CMdaAudioInputStream& aUtility);

		/**
		* Second Phase Costructor for CMdaAudioOutputStream
		* @since 3.0
		* @param aUtilityCMdaAudioOutputStream Referece
		*/
		void ConstructL(CMdaAudioOutputStream& aUtility);

		/**
		* Second Phase Costructor for CMdaAudioPlayerUtility
		* @since 3.0
		* @param aUtility CMdaAudioPlayerUtility Referece
		*/
		void ConstructL(CMdaAudioPlayerUtility& aUtility);

		/**
		* Second Phase Costructor for CMdaAudioRecordUtility
		* @since 3.0
		* @param aUtility CMdaAudioRecordUtility Referece
		*/
		void ConstructL(CMdaAudioRecorderUtility& aUtility);

		/**
		* Second Phase Costructor for CMdaAudioToneUtility
		* @since 3.0
		* @param aUtility CMdaAudioToneUtility Referece
		*/
		void ConstructL(CMdaAudioToneUtility& aUtility);

		/**
		* Second Phase Costructor for MCustomCommand
		* @since 3.0
		* @param aUtility MCustomCommand Referece
		*/
		void ConstructL(CCustomCommandUtility* aUtility);

		/**
		* Second Phase Costructor for MCustomInterface
		* @since 3.0
		* @param aUtility MCustomInterface Referece
		*/
		void ConstructL(MCustomInterface& aCustomInterface);

		/**
		* Second Phase Costructor for CMidiClientUtility
		* @since 3.0
		* @param aUtility CMidiClientUtility Referece
		*/
		void ConstructL(CMidiClientUtility& aUtility);
		
		/**
		* Second Phase Costructor for CDrmPlayerUtility
		* @since 3.0
		* @param aUtility CDrmPlayerUtility Referece
		*/
		void ConstructL(CDrmPlayerUtility& aUtility);		

		/**
		* Second Phase Costructor for CVideoPlayerUtility
		* @since 3.2
		* @param aUtility CVideoPlayerUtility Referece
		*/
		void ConstructL(CVideoPlayerUtility& aUtility);		

		/**
		* Function for Updating the Array from the Central Repository
		* @since 3.0
		* @param aUtility MCustomInterface Referece
		*/
		void UpdateFullPresetArrayFromCenRep(RArray<TEfAudioEqualizerUtilityPreset> &aFullPresetArray);

		/**
		* Function of Updating the Preset Array for the User
		* @since 3.0
		* @param aUtility MCustomInterface Referece
		*/
		void UpdatePresetArray(RArray<TEfAudioEqualizerUtilityPreset>& aPresetArray,const RArray<TEfAudioEqualizerUtilityPreset> &aFullPresetArray);


	private: // Data

		// Array for Storing the Current State of the Repository
		// which has been compacted after the transaction
		// with the Central Repository
		RArray<TEfAudioEqualizerUtilityPreset> iPresetArray;

		// Array Which contains the full information from the
		// Central Repository except the Description Values
		RArray<TEfAudioEqualizerUtilityPreset> iFullPresetArray;

		// Pointer to the Equalizer Object
		CAudioEqualizer* iAudioEqualizer;

		// Pointer to the Equalizer Object
		// which is returned to the UI for Manipulation
		CAudioEqualizer* iTransAudioEqualizer;

		// Pointer to the Bass Boost Ojbect
		CBassBoost* iBassBoost;

		// Pointer to the Preset Repository
		CRepository* iPresetRepository;
   };

#endif      // CAUDIOEQUALIZERUTILITY_H

// End of File
