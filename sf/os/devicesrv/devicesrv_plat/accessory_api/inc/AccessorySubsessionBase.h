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


#ifndef RACCESSORYSUBSESSIONBASE_H
#define RACCESSORYSUBSESSIONBASE_H

#include <e32base.h>

// FORWARD DECLARATIONS
class TAccPolGenericID;
class RAccessoryServer;
class CAccPolAccessoryPolicy;


// CLASS DECLARATION

/**
*  Base class for Accessory Server sub-sessions.
*
*  This class is not intended for user derivation.
*
*  @lib AccClient.lib
*  @since S60 3.1
*/
class RAccessorySubsessionBase : public RSubSessionBase
    {
    public:

        /**
        * Creates a new sub-session within an existing session.
        * Pure virtual function which derived classes must implement.
        *
        * @since S60 3.1
        * @param aSession The session to which this sub-session will belong.
        * @return KErrNone if successful, system-wide error code if failed.
        */
        virtual TInt CreateSubSession( RAccessoryServer& aSession ) = 0;

        /**
        * Closes the sub-session.
        * Pure virtual function which derived classes must implement.
        *
        * @since S60 3.1
        * @return KErrNone if successful, system-wide error code if failed.
        * @see CreateSubSession()
        */
        virtual TInt CloseSubSession() = 0;

    protected:

        /**
        * C++ default constructor.
        */
        IMPORT_C RAccessorySubsessionBase();

};

#endif // RACCESSORYSUBSESSIONBASE_H
