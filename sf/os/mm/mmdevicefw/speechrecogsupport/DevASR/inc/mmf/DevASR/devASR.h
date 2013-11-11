// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __DEVASR_H__
#define __DEVASR_H__

//  INCLUDES
#include <e32base.h>
#include <mmf/common/speechrecognitiondatadevasr.h>
#include <mmf/devasr/devasrcommon.h>

// FORWARD DECLARATION
class CSRSAlgorithmManager;
class TMMFPrioritySettings;

/**
*  Interface for the DevASR callbacks that are implemented by DevASR observers.
*
*  @publishedAll
*  @released
*  @since 8.0
*/

class MDevASRObserver
	{
    public:

        /**
        * DevASR notifies object implementing this interface whenever a spontaneous or non-spontaneous
		* event occurs in DevASR.
        * @since 8.0
        * @param aEvent DevASR event code
        * @param aError DevASR error code
        */
		virtual void DevASREvent(TDevASREvent aEvent, TDevASRError aError)=0;

		/**
        * DevASR notifies object implementing this interface when feature vectors are available.
        * @since 8.0
        * @param aFV Buffer containing the feature vector data
        * @param aSNR Signal-to-noise ratio
		* @param aPosition Indicates whether the feature vector is the first, middle, or end.
        */
		virtual void FeatureVectorDataRcvd(const TDesC16& aFV, TInt32 aSNR, TInt32 aPosition)=0;

        /**
        * DevASR calls this method to send a message in response to a custom command.
        * @since 8.0
        * @param aMsg Buffer containing the message
        */
		virtual void DevASRMessage(TDesC8& aMsg)=0;
    };

/**
*  Interface Class for the DevASR API.
*
*  @publishedAll
*  @released
*  @since 8.0
*/
class CDevASR : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aObserver The observer to call back when events occur
        *
        * @return pointer to object of type CDevASR
        */
        IMPORT_C static CDevASR* NewL(MDevASRObserver& aObserver);
        
        /**
        * Two-phased constructor.
        *
        * @param aObserver The observer to call back when events occur        
        *
        * @return pointer to object of type CDevASR
        */
        IMPORT_C static CDevASR* NewLC(MDevASRObserver& aObserver);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CDevASR();

    public: // New functions

		/**
        * Cancels the current or started tasks.
        * @since 8.0
        */
        IMPORT_C void Cancel();

		/**
        * Use to request a custom interface implementation from the component.
		* through the DevASR API.
        * @since 8.0
        * @param aInterfaceId - Interface UID, defined with the custom interface.
		* @return a pointer to the interface implementation, or NULL if the requested interface is
		* not implemented. The return value must be cast to the correct type by the user.
        */
		IMPORT_C TAny* CustomInterface(TUid aInterfaceId);

		/**
        * This method is used to indicate the end of a recognition session. All loaded grammars,
		* lexicon, and models become invalid after this call.
        * @since 8.0
        */
		IMPORT_C void EndRecSession();


		/**
        Retreive the properties of the underlying speech recognition engine.

        @param  aPropertyId
		        An array of identifiers being querried
        @param  aPropertyValue
		        An array of values corresponding to the querried identifiers

        @since  8.0
        */
        IMPORT_C void GetEnginePropertiesL(const RArray<TInt>& aPropertyId, RArray<TInt>& aPropertyValue);

		/**
        * Initializes the front-end module in the speech recognition engine.
		* The frontend module used during training/recognition functions is started
		* as a result. This method is intended to be used in conjunction with InitTrainBE().
        * @since 8.0
        * @param aMode - Mode of the recognizer
        */
        IMPORT_C void InitFrontEnd(TRecognizerMode aMode);

		/**
        * Perform any necessary initialization or allocation of the audio resource
		* for playing a voice sample. A buffer to hold audio data to play is allocated.
		* If allocation fails, this method will leave. Client is responsible for filling
		* the buffer with their audio data and invoke Play() on DevASR.
        * @since 8.0
        * @param aPlayDuration - Specifies the duration of playback in microseconds
		* @return A reference to the buffer to be filled with source audio data.
        */
		IMPORT_C TDes8& InitPlayL(TTimeIntervalMicroSeconds32 aPlayDuration);

		/**
        * Initialize the recognition engine back-end. The module responsible for recognition
		* function is started as a result. This method must be used before any recognition
		* operations and intended to be used in conjunction with InitFrontEnd().
        * @since 8.0
        * @param aResult - a reference to an object where the recognition result will be written
        */
        IMPORT_C void InitRecognizerBE(CSDResultSet& aResult);

		/**
        * Initializes the back-end module in the speech recognition engine.
		* The back-end module responsible for training functions is started as a result.
		* This method is intended to be used in conjunction with InitFrontEnd().
        * @since 8.0
        * @param aModel	- Buffer in which the newly created model will be written
        */
        IMPORT_C void InitTrainBE(CSDModel& aModel);

		/**
        * Load the specified recognizer parameter(s). These parameters are used to alter the
        * recognizer's default parameters. The parameters are specified as attribute-value pairs.
        * @since 8.0
        * @param aParameterId - An array of parameter identifiers
        * @param aParameterValue - An array of parameter values
        */
        IMPORT_C void LoadEngineParametersL(const RArray<TInt>& aParameterId, const RArray<TInt>& aParameterValue);

		/**
        * Load the specified grammar into the recognizer.
        * @since 8.0
        * @param aGrammar - A reference to a grammar in an internal format
        */
        IMPORT_C void LoadGrammar(const CSDGrammar& aGrammar);

		/**
        * Load the specified grammar into the recognizer.
        * @since 8.0
        * @param aLexicon - A reference to a lexicon in an internal format
        */
        IMPORT_C void LoadLexicon(const CSDLexicon& aLexicon);

		/**
        * Load the specified Lexicon into the recognizer.
        * @since 8.0
        * @param aModels - A reference to a model bank
        */
        IMPORT_C void LoadModels(const CSDModelBank& aModels);

		/**
        * Request DevASR to play the contents of its own buffer. The buffer was previously
		* allocated by invoking method InitPlay().
        * @since 8.0
        */
        IMPORT_C void Play();

		/**
        * Request to begin recording.
        * @since 8.0
        * @param aRecordDuration - Length of time to record, expressed in microseconds
        */
        IMPORT_C void Record(TTimeIntervalMicroSeconds32 aRecordDuration);

		/**
        * Use to set the priority of the sound device
        * @since 8.0
        * @param aPrioritySettings Priority settings structure

        */
		IMPORT_C void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);

		/**
        * This method is used to start a recognition session with the recognition engine.
        * @since 8.0
		* @param aMode Mode of the recognizer
		* @return result code of operation
        */
        IMPORT_C TInt StartRecSession(TRecognizerMode aMode);

		/**
        * Request to unload the specified rule in the grammar from recognizer.
        * @since 8.0
        * @param aGrammarID - Identifer of the grammar to operate on
		* @param aRuleID - Identifier of the rule to remove
        */
        IMPORT_C void UnloadRule(TGrammarID aGrammarID, TRuleID aRuleID);

		/**
        * Retreive the raw audio data accumulated during recording.
        * @since 8.0
		* @param aBuffer Reference to a buffer containing the data
		* @param aDuration Duration of the utterance
        */
        IMPORT_C void GetUtteranceData(TDes8* aBuffer, TTimeIntervalMicroSeconds32& aDuration);


    private:

        /**
        * C++ default constructor.
        */
        CDevASR();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(MDevASRObserver& aObserver);

    private:    // Data
        // Pointer to algorithm manager
   		CSRSAlgorithmManager* iSRSAlgorithmManager;
    };

#endif // __DEVASR_H__

// End of File
