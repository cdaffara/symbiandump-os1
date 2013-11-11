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




#ifndef CLISTENERDOPPLEREVENTOBSERVER_H
#define CLISTENERDOPPLEREVENTOBSERVER_H

// INCLUDES
#include <e32base.h>
#include "ListenerDopplerProxy.h"


// CLASS DECLARATION

/**
*  Doppler event observer provides encapsulates a monitoring request towards the message handler.
*
*  @lib DopplerProxy.lib
*  @since 3.0
*/
class CListenerDopplerEventObserver : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CListenerDopplerEventObserver* NewL(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
        								  MListenerDopplerCallback& aCallback);

        /**
        * Destructor.
        */
        virtual ~CListenerDopplerEventObserver();

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
        CListenerDopplerEventObserver();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
        				MListenerDopplerCallback& aCallback);

        void RunL();

        void DoCancel();

    private:

		// Pointer to custom command utility
		MCustomCommand* iCustomCommand;
		// Message handler handle
		TMMFMessageDestinationPckg iMessageHandler;
		// Doppler change event Callbacks
		MListenerDopplerCallback* iCallback;
		// Data package received from server
		TEfDopplerDataPckg iDataPckgFrom;
		//
		TBool iStopped;
	};

#endif 		// CLISTENERDOPPLEREVENTOBSERVER_H

