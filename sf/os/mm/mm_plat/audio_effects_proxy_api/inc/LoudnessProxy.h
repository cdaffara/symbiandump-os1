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
* Description:  This is the definition of the Loudness proxy class.
*
*
*/


#ifndef CLOUDNESSPROXY_H
#define CLOUDNESSPROXY_H

// INCLUDES

#include <e32base.h>
#include <LoudnessBase.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <CustomCommandUtility.h>
#include "LoudnessMessageTypes.h"
#include <MLoudnessObserver.h>

// FORWARD DECLARATION
class CLoudnessEventObserver;

// CLASS DECLARATION

/**
*  Interface class to be implemented by objects that receives callbacks from Loudness Event Observer.
*
*  @since 3.0
*/
class MLoudnessCallback
    {
    public:

        /**
        * Invoked by the Loudness Event Observer when the Loudness object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the Loudness data
        */
        virtual void LoudnessEvent( const TDesC8& aDataPckg ) = 0;

    };

/**
*  This is the Loudness effect proxy class responsible for handling framework messages.
*
*  @lib LoudnessProxy.lib
*  @since 3.0
*/

class CLoudnessProxy : public CLoudness,
					         public MLoudnessCallback
	{
	public:		// Constructors and destructor

	    /**
        *
        * Factory function for creating the Loudness proxy object.
        * @since 3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	pointer to a Loudness proxy object
        */
		IMPORT_C static CLoudnessProxy* NewL( TMMFMessageDestinationPckg aMessageHandler,
                     						        MCustomCommand& aCustomCommand,
                      						        CCustomInterfaceUtility* aCustomInterfaceUtility );
		/**
        *
        * Destructor
        */
		virtual ~CLoudnessProxy();

	public: // functions from base class

		/**
		* From CAudioEffect
        * Apply effect settings
        * @since 3.0
        */
		IMPORT_C virtual void ApplyL();

	public: // functions from MLoudnessCallback

		/**
		* From MLoudnessCallback
        * Changes to Loudness data has occured
        * @since 3.0
        * @param aBuffer Buffer containing serialized Loudness data
        */
		void LoudnessEvent( const TDesC8& aDataPckg );

	private:

		/**
		* Private C++ constructor for this class.
        * @since	3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CLoudnessProxy(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
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
	 	// Loudness Event Observer
		CLoudnessEventObserver* iLoudnessEventObserver;
		// Pointer to the custom interface utility
    	CCustomInterfaceUtility* iCustomInterfaceUtility;



	};

#endif	// of CLOUDNESSPROXY_H

// End of File
