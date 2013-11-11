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
* Description:  Creator and user of ASY threads
*
*/


#ifndef CASYPROXYMAINSERVICE_H
#define CASYPROXYMAINSERVICE_H

// INCLUDES
#include "AccSrvProxy.h"
#include <AsyMainServiceBase.h>
#include <AsyServiceUids.h>
#include <AsyCmdTypes.h>
#include <e32std.h>
#include <e32base.h>

// CONSTANTS

// Common first part of unique thread's name
_LIT(KASYThreadName, "ASYThread - ");

// MACROS

// DATA TYPES
struct TASYProxyThreadParam
    {
    RSemaphore* iSemaphore;     // Semaphore for thread
    TUid iThreadMainServiceUid; // MainService Uid
    TUid iThreadHandlerUid;     // Handler Uid
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CASYProxyASYHandler;

/**
*  Creates ASY to it's own thread
*    CASYMainServiceBase -object to it's own thread
*    CASYCommandHandlerBase -object to it's own thread
*
*  @lib ASYProxy.dll
*  @since S60 3.1
*/
class CASYProxyMainService : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CASYProxyMainService* NewL( CASYProxyASYHandler* aAsyHandler );

        /**
        * Destructor.
        */
        virtual ~CASYProxyMainService();

    public: // New functions

        /**
        * Thread's entry point
        * @since S60 3.1
        * @param aParameter Pointer of parameter passed to thread
        *        Please see the TASYProxyThreadParam struct for more info
        * @return Error code of thread creating
        *         KErrNone if succesfully
        */
        static TInt ThreadEntryPoint( TAny* aParameter );

        /**
        * Starts the thread creating
        * @since S60 3.1
        * @param aMainServiceUid Implementation uid for CASYMainServiceBase -object
        * @param aHandlerUid Implementation uid for CASYCommandHandlerBase -object
        * @param aHeapSize Heap memory size for ASY thread
        * @param aStackSize Stack memory size for ASY thread
        * @return void
        */
        void StartThreadL( const TUid aMainServiceUid,
                           const TUid aHandlerUid,
                           const TInt aHeapSize,
                           const TInt aStackSize );

        // Add Cancel for this method

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CASYProxyMainService( CASYProxyASYHandler* aAsyHandler );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * From CActive Handles an active object's request completion event.
        */
        void RunL();

        /**
        * From CActive Handles RunL's leave cases
        */
        TInt RunError( TInt aError );

        /**
        * From CActive Implements cancellation of an outstanding request.
        */
        void DoCancel();

        /**
        * Creates CActiveScheduler object and begins ASY creating
        */
        static TInt DoStartThreadL( const TUid aMainServiceUid,
                                    const TUid aHandlerUid );

        /**
        * Create ASYs
        */
        static TInt CreateAsyServicesLC( const TUid aMainServiceUid,
                                         const TUid aHandlerUid );

    public:     // Data

    protected:  // Data

    private:    // Data

        // Handle to created thread
        RThread iThread;

        // Pointer to ASY handler
        CASYProxyASYHandler* iAsyHandler; // Not owned

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
    };

#endif // CASYPROXYMAINSERVICE_H

// End of File
