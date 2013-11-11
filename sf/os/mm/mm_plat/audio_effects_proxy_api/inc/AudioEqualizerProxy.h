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
* Description:  This is the definition of the Audio Equalizer proxy class.
*
*
*/


#ifndef CAUDIOEQUALIZERPROXY_H
#define CAUDIOEQUALIZERPROXY_H

// INCLUDES

#include <e32base.h>
#include <AudioEqualizerBase.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <CustomCommandUtility.h>
#include "AudioEqualizerMessageTypes.h"
#include <MAudioEqualizerObserver.h>
#include <MAudioEffectObserver.h>

// FORWARD DECLARATION
class CAudioEqualizerEventObserver;

// CLASS DECLARATION

/**
*  Interface class to be implemented by objects that receives callbacks from Audio Equalizer Event Observer.
*
*  @since 3.0
*/
class MAudioEqualizerCallback
    {
    public:

        /**
        * Invoked by the AudioEqualizer Event Observer when the AudioEqualizer object changes state
        * @since 3.0
        * @param aBuffer Buffer containing the AudioEqualizer data
        */
        virtual void AudioEqualizerEventL( const TDesC8& aDataPckg ) = 0;

    };

/**
*  This is the AudioEqualizer effect proxy class responsible for handling framework messages.
*
*  @lib AudioEqualizerProxy.lib
*  @since 3.0
*/

class CAudioEqualizerProxy : public CAudioEqualizer,
					         public MAudioEqualizerCallback
	{
	public:		// Constructors and destructor

	    /**
        *
        * Factory function for creating the AudioEqualizer proxy object.
        * @since 3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	pointer to a AudioEqualizer proxy object
        */
		IMPORT_C static CAudioEqualizerProxy* NewL( TMMFMessageDestinationPckg aMessageHandler,
                     						        MCustomCommand& aCustomCommand,
                      						        CCustomInterfaceUtility* aCustomInterfaceUtility );
		/**
        *
        * Destructor
        */
		virtual ~CAudioEqualizerProxy();

	public: // functions from base class

		/**
		* From CAudioEffect
        * Apply effect settings
        * @since 3.0
        */
		IMPORT_C virtual void ApplyL();

	public: // functions from MAudioEqualizerCallback

		/**
		* From MAudioEqualizerCallback
        * Changes to AudioEqualizer data has occured
        * @since 3.0
        * @param aBuffer Buffer containing serialized AudioEqualizer data
        */
		void AudioEqualizerEventL( const TDesC8& aDataPckg );

	private:

		/**
		* Private C++ constructor for this class.
        * @since	3.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CAudioEqualizerProxy(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
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
	 	// AudioEqualizer Event Observer
		CAudioEqualizerEventObserver* iAudioEqualizerEventObserver;
		// Pointer to the custom interface utility
    	CCustomInterfaceUtility* iCustomInterfaceUtility;

	};

#endif	// of CAudioEqualizerPROXY_H

// End of File
