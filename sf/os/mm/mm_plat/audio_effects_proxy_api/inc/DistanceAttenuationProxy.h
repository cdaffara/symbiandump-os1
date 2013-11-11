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
* Description:  This is the definition of the DistanceAttenuation proxy class.
*
*
*/


#ifndef CDISTANCEATTENUATIONPROXY_H
#define CDISTANCEATTENUATIONPROXY_H

// INCLUDES

#include <e32base.h>
#include <DistanceAttenuationBase.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <CustomCommandUtility.h>
#include "DistanceAttenuationMessageTypes.h"
#include <MDistanceAttenuationObserver.h>

// FORWARD DECLARATION
class CDistanceAttenuationEventObserver;

// CLASS DECLARATION

/**
*  Interface class to be implemented by objects that receives callbacks from Distance Attenuation Event Observer.
*
*  @since 3.0
*/
class MDistanceAttenuationCallback
    {
    public:

        /**
        * Invoked by the DistanceAttenuation Event Observer when the DistanceAttenuation object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the DistanceAttenuation data
        */
        virtual void DistanceAttenuationEvent( const TDesC8& aDataPckg ) = 0;

    };

/**
*  This is the DistanceAttenuation effect proxy class responsible for handling framework messages.
*
*  @lib DistanceAttenuationProxy.lib
*  @since 3.0
*/

class CDistanceAttenuationProxy : public CDistanceAttenuation,
					         public MDistanceAttenuationCallback
	{
	public:		// Constructors and destructor

	    /**
        *
        * Factory function for creating the DistanceAttenuation proxy object.
        * @since 3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	pointer to a DistanceAttenuation proxy object
        */
		IMPORT_C static CDistanceAttenuationProxy* NewL( TMMFMessageDestinationPckg aMessageHandler,
                     						        MCustomCommand& aCustomCommand,
                      						        CCustomInterfaceUtility* aCustomInterfaceUtility );
		/**
        *
        * Destructor
        */
		virtual ~CDistanceAttenuationProxy();

	public: // functions from base class

		/**
		* From CAudioEffect
        * Apply effect settings
        * @since 3.0
        */
		IMPORT_C virtual void ApplyL();

	public: // functions from MDistanceAttenuationCallback

		/**
		* From MDistanceAttenuationCallback
        * Changes to DistanceAttenuation data has occured
        * @since 3.0
        * @param aBuffer Buffer containing serialized DistanceAttenuation data
        */
		void DistanceAttenuationEvent( const TDesC8& aDataPckg );

	private:

		/**
		* Private C++ constructor for this class.
        * @since	3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CDistanceAttenuationProxy(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
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
	 	// DistanceAttenuation Event Observer
		CDistanceAttenuationEventObserver* iDistanceAttenuationEventObserver;
		// Pointer to the custom interface utility
    	CCustomInterfaceUtility* iCustomInterfaceUtility;



	};

#endif	// of CDISTANCEATTENUATIONPROXY_H

// End of File
