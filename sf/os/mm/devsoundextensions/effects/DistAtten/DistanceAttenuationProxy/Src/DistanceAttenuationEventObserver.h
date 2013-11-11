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




#ifndef CDISTANCEATTENUATIONEVENTOBSERVER_H
#define CDISTANCEATTENUATIONEVENTOBSERVER_H

// INCLUDES
#include <e32base.h>
#include "DistanceAttenuationProxy.h"


// CLASS DECLARATION

/**
*  DistanceAttenuation event observer provides encapsulates a monitoring request towards the message handler.
*
*  @lib DistanceAttenuationProxy.lib
*  @since 3.0
*/
class CDistanceAttenuationEventObserver : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDistanceAttenuationEventObserver* NewL(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
        								  		  MDistanceAttenuationCallback& aCallback);

        /**
        * Destructor.
        */
        virtual ~CDistanceAttenuationEventObserver();

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
        CDistanceAttenuationEventObserver();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
        				MDistanceAttenuationCallback& aCallback);

        void RunL();

        void DoCancel();

    private:

		// Pointer to custom command utility
		MCustomCommand* iCustomCommand;
		// Message handler handle
		TMMFMessageDestinationPckg iMessageHandler;
		// DistanceAttenuation change event Callbacks
		MDistanceAttenuationCallback* iCallback;
		// Data package received from server
		TEfDistanceAttenuationDataPckg iDataPckgFrom;
		//
		TBool iStopped;
	};

#endif 		// CDISTANCEATTENUATIONEVENTOBSERVER_H

