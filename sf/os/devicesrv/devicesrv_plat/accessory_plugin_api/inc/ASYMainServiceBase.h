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
* Description:  Pure virtual main service base class for ASY
*
*/


#ifndef CASYMAINSERVICEBASE_H
#define CASYMAINSERVICEBASE_H

// INCLUDES
#include "AsyProxyTypes.h"
#include <AsyCmdTypes.h>
#include <e32std.h>
#include <e32base.h>
#include <ecom/ecom.h>

// CONSTANTS

// MACROS

// DATA TYPES

// Used to pass information about progress status of ASY's creating
// between AccessoryServer and ASY threads
struct TASYThreadGlobalParams
    {
    // Used by ASY thread
    TBool iSignalFlag; // Changed to ETrue after Signal() has been called

    // Used by ASY thread
    TBool iStartlFlag; // Changed to ETrue after StartL() has been called

    // ASY thread writes
    // AccessoryServer thread reads
    TASYThreadStatus iThreadStatus;
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CASYProxyCommsService;
class RAccessoryServer;
class CASYProxyMainService;

// CLASS DECLARATION

/**
*  Pure virtual base class for ASYs, from where ASY are meant to be inherited.
*  ASYProxy creates the instances.
*
*  @lib ASYProxy.dll
*  @since S60 3.1
*/
class CASYMainServiceBase : public CBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CASYMainServiceBase* NewL( CASYProxyCommsService* aCommsService,
                                          TUid iImplementationUidAsyService );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CASYMainServiceBase();

    public: // New functions

        /**
        * ASY's startup function is called after creating ASY
        * @since S60 3.1
        * @param void
        * @return Error code of startup.
        *         KErrNone if successfully
        */
        virtual TInt StartL() = 0;

    public: // Functions from base classes

    protected:  // New functions

        /**
        * Signals Accessory Server that ASY implementation
        * is ready to receive commands.
        * @since S60 3.1
        * @param void
        * @return void
        */
        IMPORT_C void Signal();

        /**
        * Returns reference to Accessory Server created by this thread
        * @since S60 3.1
        * @param void
        * @return Reference to the Accessory Server.
        */
        IMPORT_C RAccessoryServer& Session();

        /**
        * C++ default constructor.
        */
        IMPORT_C CASYMainServiceBase();

    protected: // Functions from base classes

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CASYProxyCommsService* aCommsService );

    public:     // Data

    protected:  // Data

    private:    // Data

        // Pointer to a CASYProxyCommsService object
        CASYProxyCommsService* iCommsService; // Owned

        // Identifier of this instance
        TUid iDtor_ID_Key;

        // Signalling statuses of ASY thread creating
        TASYThreadGlobalParams iGlobalParams;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

        friend class CASYProxyMainService;

    };

#endif // CASYMAINSERVICEBASE_H

// End of File
