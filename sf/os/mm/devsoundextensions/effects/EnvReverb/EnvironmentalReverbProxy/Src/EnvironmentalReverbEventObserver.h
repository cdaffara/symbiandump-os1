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




#ifndef CENVIRONMENTALREVERBEVENTOBSERVER_H
#define CENVIRONMENTALREVERBEVENTOBSERVER_H

// INCLUDES
#include <e32base.h>
#include "EnvironmentalReverbProxy.h"


// CLASS DECLARATION

/**
*  EnvironmentalReverb event observer provides encapsulates a monitoring request towards the message handler.
*
*  @lib EnvironmentalReverbProxy.lib
*  @since 3.0
*/
class CEnvironmentalReverbEventObserver : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CEnvironmentalReverbEventObserver* NewL(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
        								  		  MEnvironmentalReverbCallback& aCallback);

        /**
        * Destructor.
        */
        virtual ~CEnvironmentalReverbEventObserver();

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
        CEnvironmentalReverbEventObserver();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
        				MEnvironmentalReverbCallback& aCallback);

        void RunL();

        void DoCancel();

    private:

		// Pointer to custom command utility
		MCustomCommand* iCustomCommand;
		// Message handler handle
		TMMFMessageDestinationPckg iMessageHandler;
		// EnvironmentalReverb change event Callbacks
		MEnvironmentalReverbCallback* iCallback;
		// Data package received from server
		TEfEnvReverbDataPckg iDataPckgFrom;
		//
		TBool iStopped;
	};

#endif 		// CENVIRONMENTALREVERBEVENTOBSERVER_H

