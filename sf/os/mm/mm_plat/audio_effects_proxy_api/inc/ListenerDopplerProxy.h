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
* Description:  This is the definition of the Listener Doppler proxy class.
*
*
*/


#ifndef CLISTENERDOPPLERPROXY_H
#define CLISTENERDOPPLERPROXY_H

// INCLUDES

#include <e32base.h>
#include <ListenerDopplerBase.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <CustomCommandUtility.h>
#include "ListenerDopplerMessageTypes.h"
#include <MListenerDopplerObserver.h>
#include <MAudioEffectObserver.h>

// FORWARD DECLARATION
class CListenerDopplerEventObserver;

// CLASS DECLARATION

/**
*  Interface class to be implemented by objects that receives callbacks from Listener Doppler Event Observer.
*
*  @since 3.0
*/
class MListenerDopplerCallback
    {
    public:

        /**
        * Invoked by the Doppler Event Observer when the doppler object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the doppler data
        */
        virtual void ListenerDopplerEvent( const TDesC8& aDataPckg ) = 0;

    };

/**
*  This is the doppler effect proxy class responsible for handling framework messages.
*
*  @lib DopplerProxy.lib
*  @since 3.0
*/

class CListenerDopplerProxy : public CListenerDoppler,
					 public MListenerDopplerCallback
	{
	public:		// Constructors and destructor

	    /**
        *
        * Factory function for creating the doppler proxy object.
        * @since 3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	pointer to a doppler proxy object
        */
		IMPORT_C static CListenerDopplerProxy* NewL( TMMFMessageDestinationPckg aMessageHandler,
                     						MCustomCommand& aCustomCommand,
                      						CCustomInterfaceUtility* aCustomInterfaceUtility );
		/**
        *
        * Destructor
        */
		virtual ~CListenerDopplerProxy();

	public: // functions from base class

		/**
		* From CAudioEffect
        * Apply effect settings
        * @since 3.0
        */
		IMPORT_C virtual void ApplyL();

	public: // functions from MDopplerCallback

		/**
		* From MDopplerCallback
        * Changes to doppler data has occured
        * @since 3.0
        * @param aBuffer Buffer containing serialized doppler data
        */
		void ListenerDopplerEvent( const TDesC8& aDataPckg );

	private:

		/**
		* Private C++ constructor for this class.
        * @since	3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CListenerDopplerProxy(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
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
	 	// Doppler Event Observer
		CListenerDopplerEventObserver* iDopplerEventObserver;
		// Pointer to the custom interface utility
    	CCustomInterfaceUtility* iCustomInterfaceUtility;

	};

#endif	// of CLISTENERDOPPLERPROXY_H

// End of File
