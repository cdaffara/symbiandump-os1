/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef CMMSUBMESSHANDLERBASE_H
#define CMMSUBMESSHANDLERBASE_H

//  INCLUDES
#include <e32base.h>

//  FORWARD DECLARATIONS
class CIsiMsg;

// CLASS DECLARATION
/**
 * CMmSubMessHandlerBase a base class for all
 * Message Handler subsystems.
 */
class CMmSubMessHandlerBase : public CBase
    {
    public: // Constructors and destructor

        /**
         * Message for CustomMessageHandler received
         *
         * @param aIsiMsg Pointer to an incoming message
         * @return Error value
         */
        virtual TInt MessageReceivedL( CIsiMsg* aIsiMsg ) = 0;
        
        /**
         * Destructor
         */
        virtual ~CMmSubMessHandlerBase() { return; };

    protected:

        /**
         * C++ default constructor.
         */
        CMmSubMessHandlerBase() { return; };
    };

#endif // CMMSUBMESSHANDLERBASE_H

