/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the definition of the AudioEqualizer effect message handler class.
*
*
*/


#ifndef CAUDIOEQUALIZERMESSAGEHANDLER_H
#define CAUDIOEQUALIZERMESSAGEHANDLER_H

//  INCLUDES
#include <mmf/common/mmfcontroller.h>
#include <MAudioEqualizerObserver.h>


// FORWARD DECLARATIONS
class CAudioEqualizer;
class CEffectDataQueItem;

// CLASS DECLARATION

/**
*  This class encapsulates the message handler for the Audio Equalizer effect.
*/
class CAudioEqualizerMessageHandler: public CMMFObject,
                             		 public MAudioEqualizerObserver
	{
    public:  // Constructors and destructor

        IMPORT_C static CAudioEqualizerMessageHandler* NewL( TAny* customInterface );

		/**
		* Destructor.
		*/
		virtual ~CAudioEqualizerMessageHandler();

    public:  // New functions

		/**
		* Handle the request message
		* @param aMessage message to be handled
		* @return -
		*/
		virtual void HandleRequest(TMMFMessage& aMessage);

    public:  // Functions from MAudioEqualizerObserver

		/**
		* From MAudioEqualizerObserver
        * Changes to AudioEqualizer data has occured
        * @since 3.0
        * @param aObservedEffect The effect that changed
        * @param aEvent The specific event Id of the effect that changed.
        * @return -
        */
		void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );

	private:

		/**
		* C++ default constructor.
		*/
		CAudioEqualizerMessageHandler( CAudioEqualizer* aAudioEqualizer );

		/**
		* Second phase constructor for this class.
        * @since	3.0
        * @return -
        */
        void ConstructL();

		/**
		* Handle getting the AudioEqualizer object number of bands data.
		* @param aMessage message to be handled
		* @return -
		*/
		void DoGetNumberOfBandsL( TMMFMessage& aMessage );

		/**
		* Handle the request message
		* @param aMessage message to be handled
		* @return -
		*/
		void DoHandleRequestL( TMMFMessage& aMessage );

		/**
		* Handle initializing the AudioEqualizer object with initial data.
		* @param aMessage message to be handled
		* @return -
		*/
		void DoInitializeL( TMMFMessage& aMessage );

		/**
		* Handle initializing the AudioEqualizer object with initial band data.
		* @param aMessage message to be handled
		* @return -
		*/
		void DoInitializeBandL( TMMFMessage& aMessage );

		/**
		* Handle applying the AudioEqualizer data to the AudioEqualizer object
		* @param aMessage message to be handled
		* @return -
		*/
		void DoApplyL( TMMFMessage& aMessage );

		/**
		* Handle applying the AudioEqualizer band data to the AudioEqualizer object
		* @param aMessage message to be handled
		* @return -
		*/
		void DoApplyBandL( TMMFMessage& aMessage );

		/**
		* Handle observation request messages
		* @param aMessage message to be handled
		* @return -
		*/
		void DoObserveL( TMMFMessage& aMessage );


    private:    // Data

    	// The custom interface
        CAudioEqualizer* iAudioEqualizer;
        // Saved message
        TMMFMessage* iMessage;
        // Flag to indicate if registration with the custom interface has occured.
        TBool iRegistered;
        // Queue of effect data to be processed
        TSglQue<CEffectDataQueItem>* iEffectDataQue;

	};

#endif      // CAudioEqualizerMESSAGEHANDLER_H

// End of File
