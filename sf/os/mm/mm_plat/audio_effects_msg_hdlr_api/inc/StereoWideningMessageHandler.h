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
* Description:  This is the definition of the StereoWidening effect message handler class.
*
*
*/


#ifndef CSTEREOWIDENINGMESSAGEHANDLER_H
#define CSTEREOWIDENINGMESSAGEHANDLER_H

//  INCLUDES
#include <mmf/common/mmfcontroller.h>
#include <MStereoWideningObserver.h>


// FORWARD DECLARATIONS
class CStereoWidening;
class CEffectDataQueItem;

// CLASS DECLARATION

/**
*  This class encapsulates the message handler for the Stereo Widening effect.
*/
class CStereoWideningMessageHandler: public CMMFObject,
                             		 public MStereoWideningObserver
	{
    public:  // Constructors and destructor

        IMPORT_C static CStereoWideningMessageHandler* NewL( TAny* customInterface );

		/**
		* Destructor.
		*/
		virtual ~CStereoWideningMessageHandler();

    public:  // New functions

		/**
		* Handle the request message
		* @param aMessage message to be handled
		* @return -
		*/
		virtual void HandleRequest(TMMFMessage& aMessage);

    public:  // Functions from MStereoWideningObserver

		/**
		* From MStereoWideningObserver
        * Changes to StereoWidening data has occured
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
		CStereoWideningMessageHandler( CStereoWidening* aStereoWidening );

		/**
		* Second phase constructor for this class.
        * @since	3.0
        * @return -
        */
        void ConstructL();

		/**
		* Handle getting the StereoWidening object number of bands data.
		* @param aMessage message to be handled
		* @return -
		*/
		void DoGetStereoWideningLevelL( TMMFMessage& aMessage );

		/**
		* Handle the request message
		* @param aMessage message to be handled
		* @return -
		*/
		void DoHandleRequestL( TMMFMessage& aMessage );

		/**
		* Handle initializing the StereoWidening object with initial data.
		* @param aMessage message to be handled
		* @return -
		*/
		void DoInitializeL( TMMFMessage& aMessage );

		/**
		* Handle applying the StereoWidening data to the StereoWidening object
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
        CStereoWidening* iStereoWidening;
        // Saved message
        TMMFMessage* iMessage;
        // Flag to indicate if registration with the custom interface has occured.
        TBool iRegistered;
        // Queue of effect data to be processed
        TSglQue<CEffectDataQueItem>* iEffectDataQue;

	};

#endif      // CSTEREOWIDENINGMESSAGEHANDLER_H

// End of File
