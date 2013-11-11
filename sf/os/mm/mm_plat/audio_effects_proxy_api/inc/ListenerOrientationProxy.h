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
* Description:  This is the definition of the bassboost proxy class.
*
*
*/


#ifndef CLISTENERORIENTATIONPROXY_H
#define CLISTENERORIENTATIONPROXY_H

// INCLUDES

#include <e32base.h>
#include <ListenerOrientationBase.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <CustomCommandUtility.h>
#include "ListenerOrientationMessageTypes.h"
#include <MListenerOrientationObserver.h>
#include <MAudioEffectObserver.h>

// FORWARD DECLARATION
class CListenerOrientationEventObserver;

// CLASS DECLARATION

/**
*  Interface class to be implemented by objects that receives callbacks from ListenerOrientation Event Observer.
*
*  @since 3.0
*/
class MListenerOrientationCallback
    {
    public:

        /**
        * Invoked by the ListenerOrientation Event Observer when the bassboost object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the bassboost data
        */
        virtual void ListenerOrientationEvent( const TDesC8& aDataPckg ) = 0;

    };

/**
*  This is the bassboost effect proxy class responsible for handling framework messages.
*
*  @lib ListenerOrientationProxy.lib
*  @since 3.0
*/

class CListenerOrientationProxy : public CListenerOrientation,
					 public MListenerOrientationCallback
	{
	public:		// Constructors and destructor

	    /**
        *
        * Factory function for creating the bassboost proxy object.
        * @since 3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	pointer to a bassboost proxy object
        */
		IMPORT_C static CListenerOrientationProxy* NewL( TMMFMessageDestinationPckg aMessageHandler,
                     						MCustomCommand& aCustomCommand,
                      						CCustomInterfaceUtility* aCustomInterfaceUtility );
		/**
        *
        * Destructor
        */
		virtual ~CListenerOrientationProxy();

	public: // functions from base class

		/**
		* From CAudioEffect
        * Apply effect settings
        * @since 3.0
        */
		IMPORT_C virtual void ApplyL();

	public: // functions from MListenerOrientationCallback

		/**
		* From MListenerOrientationCallback
        * Changes to bassboost data has occured
        * @since 3.0
        * @param aBuffer Buffer containing serialized bassboost data
        */
		void ListenerOrientationEvent( const TDesC8& aDataPckg );

	private:

		/**
		* Private C++ constructor for this class.
        * @since	3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CListenerOrientationProxy(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
					 CCustomInterfaceUtility* aCustomInterfaceUtility);

		/**
		* Second phase constructor for this class.
        * @since	3.0
        * @return -
        */
		void ConstructL();

		/**
		* Start the active observer.
        * @since	3.0
		* @param -
        * @return -
        */
        void StartObserver();

	private:

		// Pointer to custom command utility
		MCustomCommand* iCustomCommand;
		// Message handler handle
		TMMFMessageDestinationPckg iMessageHandler;
	 	// ListenerOrientation Event Observer
		CListenerOrientationEventObserver* iListenerOrientationEventObserver;
		// Pointer to the custom interface utility
    	CCustomInterfaceUtility* iCustomInterfaceUtility;

	};

#endif	// of CLISTENERORIENTATIONPROXY_H

// End of File
