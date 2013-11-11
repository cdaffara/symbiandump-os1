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
* Description:  This is the definition of the Loudness effect message handler class.
*
*
*/


#ifndef CLOUDNESSMESSAGEHANDLER_H
#define CLOUDNESSMESSAGEHANDLER_H

//  INCLUDES
#include <mmf/common/mmfcontroller.h>
#include <MLoudnessObserver.h>


// FORWARD DECLARATIONS
class CLoudness;
class CEffectDataQueItem;

// CLASS DECLARATION

/**
*  This class encapsulates the message handler for the Loudness effect.
*/
class CLoudnessMessageHandler: public CMMFObject,
                             		 public MLoudnessObserver
	{
    public:  // Constructors and destructor

        IMPORT_C static CLoudnessMessageHandler* NewL( TAny* customInterface );

		/**
		* Destructor.
		*/
		virtual ~CLoudnessMessageHandler();

    public:  // New functions

		/**
		* Handle the request message
		* @param aMessage message to be handled
		* @return -
		*/
		virtual void HandleRequest(TMMFMessage& aMessage);

    public:  // Functions from MLoudnessObserver

		/**
		* From MLoudnessObserver
        * Changes to Loudness data has occured
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
		CLoudnessMessageHandler( CLoudness* aLoudness );

		/**
		* Second phase constructor for this class.
        * @since	3.0
        * @return -
        */
        void ConstructL();

		/**
		* Handle getting the Loudness object number of bands data.
		* @param aMessage message to be handled
		* @return -
		*/
		void DoGetLoudnessLevelL( TMMFMessage& aMessage );

		/**
		* Handle the request message
		* @param aMessage message to be handled
		* @return -
		*/
		void DoHandleRequestL( TMMFMessage& aMessage );

		/**
		* Handle initializing the Loudness object with initial data.
		* @param aMessage message to be handled
		* @return -
		*/
		void DoInitializeL( TMMFMessage& aMessage );

		/**
		* Handle applying the Loudness data to the Loudness object
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
        CLoudness* iLoudness;
        // Saved message
        TMMFMessage* iMessage;
        // Flag to indicate if registration with the custom interface has occured.
        TBool iRegistered;
        // Queue of effect data to be processed
        TSglQue<CEffectDataQueItem>* iEffectDataQue;

	};

#endif      // CLOUDNESSMESSAGEHANDLER_H

// End of File
