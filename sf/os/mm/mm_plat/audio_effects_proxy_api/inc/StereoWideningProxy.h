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
* Description:  This is the definition of the StereoWidening proxy class.
*
*
*/


#ifndef CSTEREOWIDENINGPROXY_H
#define CSTEREOWIDENINGPROXY_H

// INCLUDES

#include <e32base.h>
#include "StereoWideningMessageTypes.h"
#include <StereoWideningBase.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <CustomCommandUtility.h>
#include <MStereoWideningObserver.h>

// FORWARD DECLARATION
class CStereoWideningEventObserver;

// CLASS DECLARATION

/**
*  Interface class to be implemented by objects that receives callbacks from Stereo Widening Event Observer.
*
*  @since 3.0
*/
class MStereoWideningCallback
    {
    public:

        /**
        * Invoked by the StereoWidening Event Observer when the StereoWidening object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the StereoWidening data
        */
        virtual void StereoWideningEvent( const TDesC8& aDataPckg ) = 0;

    };

/**
*  This is the StereoWidening effect proxy class responsible for handling framework messages.
*
*  @lib StereoWideningProxy.lib
*  @since 3.0
*/

class CStereoWideningProxy : public CStereoWidening,
					         public MStereoWideningCallback
	{
	public:		// Constructors and destructor

	    /**
        *
        * Factory function for creating the StereoWidening proxy object.
        * @since 3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	pointer to a StereoWidening proxy object
        */
		IMPORT_C static CStereoWideningProxy* NewL( TMMFMessageDestinationPckg aMessageHandler,
                     						        MCustomCommand& aCustomCommand,
                      						        CCustomInterfaceUtility* aCustomInterfaceUtility );
		/**
        *
        * Destructor
        */
		virtual ~CStereoWideningProxy();

	public: // functions from base class

		/**
		* From CAudioEffect
        * Apply effect settings
        * @since 3.0
        */
		IMPORT_C virtual void ApplyL();

	public: // functions from MStereoWideningCallback

		/**
		* From MStereoWideningCallback
        * Changes to StereoWidening data has occured
        * @since 3.0
        * @param aBuffer Buffer containing serialized StereoWidening data
        */
		void StereoWideningEvent( const TDesC8& aDataPckg );

	private:

		/**
		* Private C++ constructor for this class.
        * @since	3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CStereoWideningProxy(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
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
	 	// StereoWidening Event Observer
		CStereoWideningEventObserver* iStereoWideningEventObserver;
		// Pointer to the custom interface utility
    	CCustomInterfaceUtility* iCustomInterfaceUtility;



	};

#endif	// of CSTEREOWIDENINGPROXY_H

// End of File
