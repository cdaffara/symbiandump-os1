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
* Description:  This is the definition of the Source Doppler Effect message 
*                handler class.
*
*
*/


#ifndef CSOURCEDOPPLERMESSAGEHANDLER_H
#define CSOURCEDOPPLERMESSAGEHANDLER_H

//  INCLUDES
#include <mmf/common/mmfcontroller.h>
#include <MSourceDopplerObserver.h>


// FORWARD DECLARATIONS
class CSourceDoppler;
class CEffectDataQueItem;

// CLASS DECLARATION

/**
*  This class encapsulates the message handing function of the doppler effect.
*  All custom commands send by the Doppler Proxy are routed to this class. The
*  custom commands are dispatched to the appropriate handler.
*/
class CSourceDopplerMessageHandler: public CMMFObject,
                             public MSourceDopplerObserver
	{
    public:  // Constructors and destructor

        IMPORT_C static CSourceDopplerMessageHandler* NewL( TAny* customInterface );

		/**
		* Destructor.
		*/
		virtual ~CSourceDopplerMessageHandler();

    public:  // New functions

		/**
		* Handle the request message
		* @param aMessage message to be handled
		* @return -
		*/
		virtual void HandleRequest(TMMFMessage& aMessage);

    public:  // Functions from MSourceDopplerObserver

		/**
		* From MSourceDopplerObserver
        * Changes to SourceDoppler data has occured
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
		CSourceDopplerMessageHandler( CSourceDoppler* aDoppler );

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
		* Handle initializing the doppler object with initial data.
		* @param aMessage message to be handled
		* @return -
		*/
		void DoInitializeL( TMMFMessage& aMessage );

		/**
		* Handle applying the doppler data to the doppler object
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
        CSourceDoppler* iDoppler;
        // Saved observation message
        TMMFMessage* iMessage;
        // Flag to indicate if registration with the custom interface has occured.
        TBool iRegistered;
        // Queue of effect data to be processed
        TSglQue<CEffectDataQueItem>* iEffectDataQue;

	};

#endif      // CSOURCEDOPPLERMESSAGEHANDLER_H

// End of File
