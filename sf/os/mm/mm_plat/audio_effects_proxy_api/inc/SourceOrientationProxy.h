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


#ifndef CSOURCEORIENTATIONPROXY_H
#define CSOURCEORIENTATIONPROXY_H

// INCLUDES

#include <e32base.h>
#include <SourceOrientationBase.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <CustomCommandUtility.h>
#include "SourceOrientationMessageTypes.h"
#include <MSourceOrientationObserver.h>
#include <MAudioEffectObserver.h>

// FORWARD DECLARATION
class CSourceOrientationEventObserver;

// CLASS DECLARATION

/**
*  Interface class to be implemented by objects that receives callbacks from SourceOrientation Event Observer.
*
*  @since 3.0
*/
class MSourceOrientationCallback
    {
    public:

        /**
        * Invoked by the SourceOrientation Event Observer when the bassboost object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the bassboost data
        */
        virtual void SourceOrientationEvent( const TDesC8& aDataPckg ) = 0;

    };

/**
*  This is the bassboost effect proxy class responsible for handling framework messages.
*
*  @lib SourceOrientationProxy.lib
*  @since 3.0
*/

class CSourceOrientationProxy : public CSourceOrientation,
					 public MSourceOrientationCallback
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
		IMPORT_C static CSourceOrientationProxy* NewL( TMMFMessageDestinationPckg aMessageHandler,
                     						MCustomCommand& aCustomCommand,
                      						CCustomInterfaceUtility* aCustomInterfaceUtility );
		/**
        *
        * Destructor
        */
		virtual ~CSourceOrientationProxy();

	public: // functions from base class

		/**
		* From CAudioEffect
        * Apply effect settings
        * @since 3.0
        */
		IMPORT_C virtual void ApplyL();

	public: // functions from MSourceOrientationCallback

		/**
		* From MSourceOrientationCallback
        * Changes to bassboost data has occured
        * @since 3.0
        * @param aBuffer Buffer containing serialized bassboost data
        */
		void SourceOrientationEvent( const TDesC8& aDataPckg );

	private:

		/**
		* Private C++ constructor for this class.
        * @since	3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CSourceOrientationProxy(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
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
	 	// SourceOrientation Event Observer
		CSourceOrientationEventObserver* iSourceOrientationEventObserver;
		// Pointer to the custom interface utility
    	CCustomInterfaceUtility* iCustomInterfaceUtility;

	};

#endif	// of CSOURCEORIENTATIONPROXY_H

// End of File
