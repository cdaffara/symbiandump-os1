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
* Description:  Communication channel to Accessory server
*
*/


#ifndef CASYPROXYCOMMSSERVICE_H
#define CASYPROXYCOMMSSERVICE_H

// INCLUDES
#include "ASYProxyCommandObserver.h"
#include "AccessoryAsyComms.h"
#include <AsyCmdTypes.h>
#include <e32std.h>
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CASYProxyCommsStack;
class CASYCommandHandlerBase;
class CASYMainServiceBase;

// CLASS DECLARATION

/**
*  Creates communication channel to Accessory server
*    using RAccessoryAsyComms -client.
*  Creates CASYCommandHandlerBase -object, handler part of ASY.
*
*  @lib ASYProxy.dll
*  @since S60 3.1
*/
class CASYProxyCommsService : public CBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CASYProxyCommsService* NewL( const TUid aHandlerUid,
                                            const TUid aMainServiceUid );

        /**
        * Destructor.
        */
        virtual ~CASYProxyCommsService();

    public: // New functions

        /**
        * Creates CASYCommandHandlerBase -object.
        * @since S60 3.1
        * @param aASYProxyCommandObserver Pointer to Observer object
        * @return void
        */
        void CreateAsyCommandHandlerL(
                    CASYProxyCommandObserver* aASYProxyCommandObserver );

        /**
        * Gets session to Accessory server
        * @since S60 3.1
        * @param void
        * @return Session to server
        */
        RAccessoryServer& GetServerSession();

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CASYProxyCommsService( const TUid aHandlerUid );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TUid aMainServiceUid );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        void Destroy( TInt aTrId );
        void UpdateAsyCommsStackL();

    public:     // Data

        // Pointer to a ASYMainService
        CASYMainServiceBase* iAsyMainServiceBase; // Not owned

    protected:  // Data

    private:    // Data

        // Client API of Accessory server
        RAccessoryAsyComms iAsyComms;

        // Implementation uid of ASY's handler object to be created
        TUid iHandlerUid;

        // Pointer to a commands stack object
        CASYProxyCommsStack* iAsyProxyCommsStack; // Owned

        // Pointer to a handler object of ASY.
        //  This class creates
        //  CASYProxyCommandObserver deletes
        CASYCommandHandlerBase *iAsyCmdHndlr;

        // Accessory server's client
        RAccessoryServer iServer;

        // Dummy observer for zero ( non existing ) handler
        // This AO is needed for obligatory CActiveScheduler::Start()
        CASYProxyCommandObserver* iZeroObserver;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

        friend class CASYProxyCommandObserver;
    };

#endif // CASYPROXYCOMMSSERVICE_H

// End of File
