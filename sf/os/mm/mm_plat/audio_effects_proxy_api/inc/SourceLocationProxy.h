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


#ifndef CSOURCELOCATIONPROXY_H
#define CSOURCELOCATIONPROXY_H

// INCLUDES

#include <e32base.h>
#include <SourceLocationBase.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <CustomCommandUtility.h>
#include "SourceLocationMessageTypes.h"
#include <MSourceLocationObserver.h>
#include <MAudioEffectObserver.h>

// FORWARD DECLARATION
class CSourceLocationEventObserver;

// CLASS DECLARATION

/**
*  Interface class to be implemented by objects that receives callbacks from SourceLocation Event Observer.
*
*  @since 3.0
*/
class MSourceLocationCallback
    {
    public:

        /**
        * Invoked by the SourceLocation Event Observer when the bassboost object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the bassboost data
        */
        virtual void SourceLocationEvent( const TDesC8& aDataPckg ) = 0;

    };

/**
*  This is the bassboost effect proxy class responsible for handling framework messages.
*
*  @lib SourceLocationProxy.lib
*  @since 3.0
*/

class CSourceLocationProxy : public CSourceLocation,
					 public MSourceLocationCallback
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
		IMPORT_C static CSourceLocationProxy* NewL( TMMFMessageDestinationPckg aMessageHandler,
                     						MCustomCommand& aCustomCommand,
                      						CCustomInterfaceUtility* aCustomInterfaceUtility );
		/**
        *
        * Destructor
        */
		virtual ~CSourceLocationProxy();

	public: // functions from base class

		/**
		* From CAudioEffect
        * Apply effect settings
        * @since 3.0
        */
		IMPORT_C virtual void ApplyL();

	public: // functions from MSourceLocationCallback

		/**
		* From MSourceLocationCallback
        * Changes to bassboost data has occured
        * @since 3.0
        * @param aBuffer Buffer containing serialized bassboost data
        */
		void SourceLocationEvent( const TDesC8& aDataPckg );

	private:

		/**
		* Private C++ constructor for this class.
        * @since	3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CSourceLocationProxy(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
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
	 	// SourceLocation Event Observer
		CSourceLocationEventObserver* iSourceLocationEventObserver;
		// Pointer to the custom interface utility
    	CCustomInterfaceUtility* iCustomInterfaceUtility;

	};

#endif	// of CSOURCELOCATIONPROXY_H

// End of File
