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



#ifndef CMMPACKETMESSHANDLERCONTEXTLIST_H
#define CMMPACKETMESSHANDLERCONTEXTLIST_H

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

// INCLUDES
#include <etelpckt.h>
#include "cmmpacketservicemesshandlergsmwcdmaext.h"

// FORWARD DECLARATIONS

class CMmPacketServiceMessHandlerGsmWcdmaExt;
class CMmPacketContextMessHandlerGsmWcdmaExt;


// CLASS DECLARATION

/**
 * Contains information list for Packet Context DOS TSY context objects.
 * This class provides the methods for the Packet Context Message Handler List
 * object (CMmPacketContextList class) to keep track of opened contexts.
 * When a Context object is opened it should be put on the Service's Context
 * List.
 */
class CMmPacketContextMesshandlerList : public CBase
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         */        
        static CMmPacketContextMesshandlerList* NewL( 
            CMmPacketServiceMessHandlerGsmWcdmaExt* const 
                aPacketServiceMessHandlerGsmWcdmaExt );

        /**
         * Default destructor.
         */        
        virtual ~CMmPacketContextMesshandlerList();

        // New functions

        /**
         * Adds context to the context info list.
         *          
         * @param aProxyId
         * @param aContextName
         * @param aHostContextName
         * @return Error value
         */        
        TInt AddObject( const TUint8 aProxyId,
            const TInfoName& aContextName,
            const TInfoName& aHostContextName );

    private: // New functions

        /**
         * C++ default constructor.
         */
        CMmPacketContextMesshandlerList();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private: // Data

        /*
         * A pointer to multimode packet service object.
         * Own.
         */                  
        CMmPacketServiceMessHandlerGsmWcdmaExt* 
            iPacketServiceMessHandlerGsmWcdmaExt;
    };

#endif // CMMPACKETMESSHANDLERCONTEXTLIST_H

//  End of File
