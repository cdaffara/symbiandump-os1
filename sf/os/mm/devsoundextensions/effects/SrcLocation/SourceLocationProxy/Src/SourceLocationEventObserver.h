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




#ifndef CSOURCELOCATIONEVENTOBSERVER_H
#define CSOURCELOCATIONEVENTOBSERVER_H

// INCLUDES
#include <e32base.h>
#include "SourceLocationProxy.h"


// CLASS DECLARATION

/**
*  SourceLocation event observer provides encapsulates a monitoring request towards the message handler.
*
*  @lib SourceLocationProxy.lib
*  @since 3.0
*/
class CSourceLocationEventObserver : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSourceLocationEventObserver* NewL(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
        								  MSourceLocationCallback& aCallback);

        /**
        * Destructor.
        */
        virtual ~CSourceLocationEventObserver();

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
        CSourceLocationEventObserver();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
        				MSourceLocationCallback& aCallback);

        void RunL();

        void DoCancel();

    private:

		// Pointer to custom command utility
		MCustomCommand* iCustomCommand;
		// Message handler handle
		TMMFMessageDestinationPckg iMessageHandler;
		// SourceLocation change event Callbacks
		MSourceLocationCallback* iCallback;
		// Data package received from server
		TEfLocationDataPckg iDataPckgFrom;
		//
		TBool iStopped;
	};

#endif 		// CSOURCELOCATIONEVENTOBSERVER_H

