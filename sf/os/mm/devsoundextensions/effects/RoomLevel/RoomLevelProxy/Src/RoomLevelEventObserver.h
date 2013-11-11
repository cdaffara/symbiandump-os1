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




#ifndef CROOMLEVELEVENTOBSERVER_H
#define CROOMLEVELEVENTOBSERVER_H

// INCLUDES
#include <e32base.h>
#include "RoomLevelProxy.h"


// CLASS DECLARATION

/**
*  RoomLevel event observer provides encapsulates a monitoring request towards the message handler.
*
*  @lib RoomLevelProxy.lib
*  @since 3.0
*/
class CRoomLevelEventObserver : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CRoomLevelEventObserver* NewL(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
        								  		  MRoomLevelCallback& aCallback);

        /**
        * Destructor.
        */
        virtual ~CRoomLevelEventObserver();

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
        CRoomLevelEventObserver();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
        				MRoomLevelCallback& aCallback);

        void RunL();

        void DoCancel();

    private:

		// Pointer to custom command utility
		MCustomCommand* iCustomCommand;
		// Message handler handle
		TMMFMessageDestinationPckg iMessageHandler;
		// RoomLevel change event Callbacks
		MRoomLevelCallback* iCallback;
		// Data package received from server
		TEfRoomLevelDataPckg iDataPckgFrom;
		//
		TBool iStopped;
	};

#endif 		// CROOMLEVELEVENTOBSERVER_H

