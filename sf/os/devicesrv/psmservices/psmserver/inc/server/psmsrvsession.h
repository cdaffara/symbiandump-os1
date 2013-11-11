/*
* Copyright (c) 2007 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM server side session.
*
*/


#ifndef PSMSRVSESSION_H
#define PSMSRVSESSION_H

#include <e32base.h>
#include <psmtypes.h>
#include "psmmodechangeobserver.h"

class CPsmManager;
class CPsmSrvServer;
class CPsmMessage;

/**
 *  PSM Session
 *
 *  Server side session for PSM client-server
 *
 *  @since S60 5.0
 */
class CPsmSession : public CSession2
    {
    public:  // Constructors and destructor

        /**
         * Creates a new PSM plug-in loader.
         *
         * @return A pointer to the created object.
         */
        static CPsmSession* NewL( CPsmManager& aPsmManager, CPsmSrvServer& aServer );

        /**
         * Creates a new PSM plug-in loader. Newly created instance is left in the cleanup stack.
         *
         * @return A pointer to the created object.
         */
        static CPsmSession* NewLC( CPsmManager& aPsmManager, CPsmSrvServer& aServer );

        /**
         * Destructor.
         */
        virtual ~CPsmSession();

        /**
         * Service requests
         */
        void ServiceL( const RMessage2& aMessage );
		
		/**
         * ServiceError handling
         */
		void ServiceError(const RMessage2 &aMessage, TInt aError);

    private:

        /**
         * C++ constructor.
         */
        CPsmSession( CPsmManager& aPsmManager, CPsmSrvServer& aServer );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();


        /**
         * Handles settings related requests
         */
        void HandleSettingsRequestL( const RMessage2& aMessage );

        /**
         * Reads config array from message
         */
        void ReadConfigArrayL( RConfigInfoArray& aArray, const RMessage2& aMessage );

    private:    // Data

        /**
         * Reference to server
         */
        CPsmSrvServer& iPsmServer;

        /**
         * Reference to PSM engine 
         */
        CPsmManager& iPsmManager;

        /**
         * Pointer to NotifyModeChange async message
         */
        CPsmMessage* iNotifyModeMessage;

    };

#endif // PSMSRVSESSION_H 
