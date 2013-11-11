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
* Description:  PSM client-server session class.
*
*/


#ifndef PSMSESSION_H
#define PSMSESSION_H

#include <e32base.h>
#include <psmtypes.h>
#include "psmclientserver.h"

/**
 * Main session class for PSM server. This is only for PSM client's 
 * internal use. R-classes are hidden to actual clients.
 * 
 * @since S60 5.0
 */
NONSHARABLE_CLASS ( RPsmServer ) : public RSessionBase
    {
    public:
        /**
         * Creates a session with a server.
         */
        TInt Connect();

        /**
         * Send message to server
         *
         * @param aRequest The function number identifying the request.
         * @param aArgs A set of up to arguments
         * @return KErrNone if connect succesfully, otherwise system wide error
         */
        TInt SendReceiveSync( TPsmServerRequest aRequest, const TIpcArgs& aArgs ) const;

        /**
         * Send asynchronous message to server
         *
         * @param aRequest The function number identifying the request.
         * @param aArgs A set of up to arguments
         * @param aStatus The request status object used to contain the completion status of the request.
         */
        void SendReceiveAsync( TPsmServerRequest aRequest, 
                               const TIpcArgs& aArgs, 
                               TRequestStatus& aStatus ) const;

    private:

        /**
         * Return server location
         *
         * @return location of server
         */
        TFullName ServerLocation() const;

        /**
         * Return version of server
         *
         * @return version of server
         */
        TVersion ServerVersion() const;

        /**
         * Starts server
         *
         * @return KErrNone if started succesfully, otherwise system wide error
         */
        TInt StartServer() const;

    };

#endif // PSMSESSION_H 
