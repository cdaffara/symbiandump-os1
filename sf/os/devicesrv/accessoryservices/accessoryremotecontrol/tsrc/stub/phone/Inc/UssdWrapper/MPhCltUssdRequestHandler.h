/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Abstract interface for handling the notify events 
*                from the server.
*
*/

#ifndef MPHCLTUSSDREQUESTHANDLER_H
#define MPHCLTUSSDREQUESTHANDLER_H

// INCLUDES
#include <e32std.h>


// CLASS DECLARATION

/**
*  Abstract interface for handling the notify events from the server.
*  User derives his class from this and implements HandleNotifyL().
*
*  @lib phoneclient.lib
*  @since 1.0
*/

class MPhCltUssdRequestHandler
    {
    
    public: // New functions

        /**
        * Called when send completes.
        * 
        * @param aError Error code.
        */    
        virtual void HandleSendEventL( const TInt aError ) = 0;
    };


#endif // MPHCLTUSSDREQUESTHANDLER_H

// End of File
