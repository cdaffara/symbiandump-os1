/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: ?Description
*
*/


#ifndef RACCESSORYSERVER_H
#define RACCESSORYSERVER_H

//  INCLUDES
#include <e32std.h>


// CLASS DECLARATION

/**
*  RAccessoryServer is a session class for The Accessory Server.
*
*  This class is not intended for user derivation.
*
*  @lib AccClient.lib
*  @since S60 3.1
*/
class RAccessoryServer : public RSessionBase
    {
    public:

        /**
        * C++ default constructor.
        */
        IMPORT_C RAccessoryServer();

    public:

        /**
        * Creates a session with a server.
        * RAccessoryServer does not allowe more that a single connection
        * to the Accessory Server per client thread.
        *
        * @since S60 3.1
        * @return KErrNone if successful, system-wide error code if failed.
        *         Specifically:
        *         KErrNotFound, if Server is not running
        *         KErrAlreadyExists, if connection allready exist
        */
        IMPORT_C TInt Connect();

        /**
        * Closes a session with a server.
        * All the related sub-sessions must be closed before closing the session.
        *
        * @since S60 3.1
        * @return void
        */
        IMPORT_C void Disconnect();

        /**
        * Get version information about the server.
        *
        * @since S60 3.1
        * @return Version information about the server
        */
        IMPORT_C TVersion Version() const;

    };

#endif      // RACCESSORYSERVER_H

// End of File

