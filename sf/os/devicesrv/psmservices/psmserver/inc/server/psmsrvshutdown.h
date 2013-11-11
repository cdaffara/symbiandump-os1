/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM server shutdown.
*
*/


#ifndef PSMSRVSHUTDOWN_H
#define PSMSRVSHUTDOWN_H

#include <e32base.h>

class CPsmSrvServer;

/**
 *  Power Save Mode Server shutdown timer
 *
 *  Starts timer to shutdown PSM server
 *
 *  @since S60 5.0
 */
class CPsmSrvShutdown : public CActive
    {
    public:  // Constructors and destructor

        /**
         * Creates a new PSM plug-in loader.
         *
         * @return A pointer to the created object.
         */
        CPsmSrvShutdown( CPsmSrvServer& aServer );

        /**
         * Destructor.
         */
        virtual ~CPsmSrvShutdown();

    public:

        /** 
         * Starts shutdown timer.
         */
        void Start();

        /**
         * Restarts shutdown timer.
         */
        void Restart();

    protected: // From CActive

        void RunL();
        void DoCancel();

    private:    // Data

        CPsmSrvServer& iServer;

        RTimer iTimer;

    };

#endif // PSMSRVSHUTDOWN_H 
