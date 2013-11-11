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
* Description:   Definition of the event observer class
*
*/




#ifndef CLOUDNESSEVENTOBSERVER_H
#define CLOUDNESSEVENTOBSERVER_H

// INCLUDES
#include <e32base.h>
#include "LoudnessProxy.h"


// CLASS DECLARATION

/**
*  Loudness event observer provides encapsulates a monitoring request towards the message handler.
*
*  @lib LoudnessProxy.lib
*  @since 3.0
*/
class CLoudnessEventObserver : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CLoudnessEventObserver* NewL(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
        								  		  MLoudnessCallback& aCallback);

        /**
        * Destructor.
        */
        virtual ~CLoudnessEventObserver();

    public: // New functions

        /**
        * Start event monitoring
        * @since 3.0
        */
        void Start();

       	/**
        * Stop event monitoring
        * @since 3.0
        */
        void Stop();


    private:

        /**
        * C++ default constructor.
        */
        CLoudnessEventObserver();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
        				MLoudnessCallback& aCallback);

        void RunL();

        void DoCancel();

    private:

		// Pointer to custom command utility
		MCustomCommand* iCustomCommand;
		// Message handler handle
		TMMFMessageDestinationPckg iMessageHandler;
		// Loudness change event Callbacks
		MLoudnessCallback* iCallback;
		// Data package received from server
		TEfLoudnessDataPckg iDataPckgFrom;
		//
		TBool iStopped;
	};

#endif 		// CLOUDNESSEVENTOBSERVER_H

