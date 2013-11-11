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
* Description:  Server's side session class
*
*/


#ifndef CACCSRVMAINSESSION_H
#define CACCSRVMAINSESSION_H

// INCLUDES
#include "AccServer.h"
#include "AccSrvSubMode.h"
#include "AccSrvsubSettings.h"
#include "AccSrvSubSingleConnection.h"
#include "AccSrvSubConnection.h"
#include "AccSrvSubControl.h"
#include "AccSrvSubBTControl.h"
#include "AccSrvSubAudioControl.h"
#include "AccSrvSubAsyComms.h"
#include "AsyProxyAPI.h"
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Server's side session class
*
*  AccServer.exe
*  @since S60 3.1
*/
NONSHARABLE_CLASS(  CAccSrvMainSession ) : public CSession2
    {

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since S60 3.1
        * @param aClient A client's thread
        * @param aServer Reference to server
        * @param aProxy Pointer to an ASYProxy's API
        * @param aConnectionController Pointer to an connection controller object
        * @param aServerModel Pointer to an server model
        * @return Pointer to a new session
        */
        static CAccSrvMainSession* NewL(
                          RThread& aClient,
                          CAccServer& aServer,
                          CASYProxyAPI* aProxy,
                          CAccSrvConnectionController* aConnectionController,
                          CAccSrvServerModel* aServerModel );

        /**
        * Two-phased constructor.
        * @since S60 3.1
        * @param aClient A client's thread
        * @param aServer Reference to server
        * @param aProxy Pointer to an ASYProxy's API
        * @param aConnectionController Pointer to an connection controller object
        * @param aServerModel Pointer to an server model
        * @return Pointer to a new session
        */
        static CAccSrvMainSession* NewLC(
                          RThread& aClient,
                          CAccServer& aServer,
                          CASYProxyAPI* aProxy,
                          CAccSrvConnectionController* aConnectionController,
                          CAccSrvServerModel* aServerModel );

        /**
        * Destructor.
        */
        ~CAccSrvMainSession();

    public: // New functions

    public: // Functions from base classes

        /**
        * Override of CSession2::ServiceL().
        * @since S60 3.1
        * @param aMessage Message from client
        * @return void
        */
        void ServiceL( const RMessage2& aMessage );

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CAccSrvMainSession( CAccSrvConnectionController* aConnectionController,
                            CAccSrvServerModel* aServerModel );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CAccServer* aServer, CASYProxyAPI* aProxy );

        void NewSubSessionL( const RMessage2& aMessage );
        void CloseSubSessionL( const RMessage2& aMessage );
        void DispatchMessageL( const RMessage2& aMessage );
        void WriteL( const RMessage2& aMessage, TInt aFunction, TInt aErr );

    public:     // Data

    protected:  // Data

    private:    // Data

        // Pointer to a server
        CAccServer* iAccServer;  // Not owned

        // Pointer to an object index
        CObjectIx* iObjectIx;    // Owned

        // Pointer to an object container
        CObjectCon* iObjectCon;  // Owned

        // Pointer to an ASYProxy's API
        CASYProxyAPI* iProxy;    // Not owned

        CAccSrvConnectionController* iConnectionController; // Not owned
        CAccSrvServerModel*          iServerModel;          // Not Owned

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCSRVMAINSESSION_H

// End of File
