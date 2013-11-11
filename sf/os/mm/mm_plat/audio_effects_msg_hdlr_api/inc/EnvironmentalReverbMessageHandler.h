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
* Description:  This is the definition of the EnvironmentalReverb effect message handler class.
*
*
*/


#ifndef CENVIRONMENTALREVERBMESSAGEHANDLER_H
#define CENVIRONMENTALREVERBMESSAGEHANDLER_H

//  INCLUDES
#include <mmf/common/mmfcontroller.h>
#include <MEnvironmentalReverbObserver.h>


// FORWARD DECLARATIONS
class CEnvironmentalReverb;
class CEffectDataQueItem;

// CLASS DECLARATION

/**
*  This class encapsulates the message handler for the Environmental Reverb effect.
*/
class CEnvironmentalReverbMessageHandler: public CMMFObject,
                             		 	  public MEnvironmentalReverbObserver
	{
    public:  // Constructors and destructor

        IMPORT_C static CEnvironmentalReverbMessageHandler* NewL( TAny* customInterface );

		/**
		* Destructor.
		*/
		virtual ~CEnvironmentalReverbMessageHandler();

    public:  // New functions

		/**
		* Handle the request message
		* @param aMessage message to be handled
		* @return -
		*/
		virtual void HandleRequest(TMMFMessage& aMessage);

    public:  // Functions from MEnvironmentalReverbObserver

		/**
		* From MEnvironmentalReverbObserver
        * Changes to EnvironmentalReverb data has occured
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
		CEnvironmentalReverbMessageHandler( CEnvironmentalReverb* aEnvironmentalReverb );

		/**
		* Second phase constructor for this class.
        * @since	3.0
        * @return -
        */
        void ConstructL();

		/**
		* Handle the request message
		* @param aMessage message to be handled
		* @return -
		*/
		void DoHandleRequestL( TMMFMessage& aMessage );

		/**
		* Handle initializing the EnvironmentalReverb object with initial data.
		* @param aMessage message to be handled
		* @return -
		*/
		void DoInitializeL( TMMFMessage& aMessage );

		/**
		* Handle applying the EnvironmentalReverb data to the EnvironmentalReverb object
		* @param aMessage message to be handled
		* @return -
		*/
		void DoApplyL( TMMFMessage& aMessage );

		/**
		* Handle observation request messages
		* @param aMessage message to be handled
		* @return -
		*/
		void DoObserveL( TMMFMessage& aMessage );




    private:    // Data

    	// The custom interface
        CEnvironmentalReverb* iEnvironmentalReverb;
        // Saved message
        TMMFMessage* iMessage;
        // Flag to indicate if registration with the custom interface has occured.
        TBool iRegistered;
        // Queue of effect data to be processed
        TSglQue<CEffectDataQueItem>* iEffectDataQue;

	};

#endif      // CENVIRONMENTALREVERBMESSAGEHANDLER_H

// End of File
