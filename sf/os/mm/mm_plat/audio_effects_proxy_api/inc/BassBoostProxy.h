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


#ifndef CBASSBOOSTPROXY_H
#define CBASSBOOSTPROXY_H

// INCLUDES

#include <e32base.h>
#include <BassBoostBase.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <CustomCommandUtility.h>
#include "BassBoostMessageTypes.h"
#include <MBassBoostObserver.h>
#include <MAudioEffectObserver.h>

// FORWARD DECLARATION
class CBassBoostEventObserver;

// CLASS DECLARATION

/**
*  Interface class to be implemented by objects that receives callbacks from BassBoost Event Observer.
*
*  @since 3.0
*/
class MBassBoostCallback
    {
    public:

        /**
        * Invoked by the BassBoost Event Observer when the bassboost object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the bassboost data
        */
        virtual void BassBoostEvent( const TDesC8& aDataPckg ) = 0;

    };

/**
*  This is the bassboost effect proxy class responsible for handling framework messages.
*
*  @lib BassBoostProxy.lib
*  @since 3.0
*/

class CBassBoostProxy : public CBassBoost,
					 public MBassBoostCallback
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
		IMPORT_C static CBassBoostProxy* NewL( TMMFMessageDestinationPckg aMessageHandler,
                     						MCustomCommand& aCustomCommand,
                      						CCustomInterfaceUtility* aCustomInterfaceUtility );
		/**
        *
        * Destructor
        */
		virtual ~CBassBoostProxy();

	public: // functions from base class

		/**
		* From CAudioEffect
        * Apply effect settings
        * @since 3.0
        */
		IMPORT_C virtual void ApplyL();

	public: // functions from MBassBoostCallback

		/**
		* From MBassBoostCallback
        * Changes to bassboost data has occured
        * @since 3.0
        * @param aBuffer Buffer containing serialized bassboost data
        */
		void BassBoostEvent( const TDesC8& aDataPckg );

	private:

		/**
		* Private C++ constructor for this class.
        * @since	3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CBassBoostProxy(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
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
	 	// BassBoost Event Observer
		CBassBoostEventObserver* iBassBoostEventObserver;
		// Pointer to the custom interface utility
    	CCustomInterfaceUtility* iCustomInterfaceUtility;

	};

#endif	// of CBASSBOOSTPROXY_H

// End of File
