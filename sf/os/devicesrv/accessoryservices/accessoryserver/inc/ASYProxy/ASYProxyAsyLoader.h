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
* Description:  Loader of ASYs
*
*/


#ifndef CASYPROXYASYLOADER_H
#define CASYPROXYASYLOADER_H

// INCLUDES
#include "ASYProxyLoaderTimer.h"
#include <ecom/ecom.h>
#include <AsyServiceUids.h>
#include <e32svr.h>
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// Zero Uid is used for non existing handler
#define ZEROHANDLERUID 0x0

enum TASYLoadingStep
    {
    EASYLoadingStepListInit,
    EASYLoadingStepListImplementations,
    EASYLoadingStepOrderImplementations,
    EASYLoadingStepLoadASY,
    EASYLoadingOver
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CASYProxyAsyHandler;
class CASYProxy;

/**
*  Loader class for ASYs
*  Loading steps has been splitted by using AO
*
*  @lib ASYProxy.dll
*  @since S60 3.1
*/
class CASYProxyAsyLoader : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CASYProxyAsyLoader* NewL( CASYProxy* aAsyProxy );

        /**
        * Destructor.
        */
        virtual ~CASYProxyAsyLoader();

    public: // New functions

        /**
        * Issues asynchronous ASY loading steps.
        * @since S60 3.1
        * @param void
        * @return void
        */
        void IssueRequest();

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

        /**
        * From CActive Implements cancellation of an outstanding request.
        */
        void DoCancel();

        /**
        * From CActive Handles an active object's request completion event.
        */
        void RunL();

        /**
        * From CActive Handles RunL's leave cases
        */
        TInt RunError( TInt aError );

    private:

        /**
        * C++ default constructor.
        */
        CASYProxyAsyLoader( CASYProxy* aAsyProxy );

       /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        void Cleanup();
        void ListImplementationsL();
        void DoIssueRequestL();
        void ArrangeImplementationsL();
        void NextStep();

    public:     // Data

    protected:  // Data

    private:    // Data

        // Implementation info array of CASYMainServiceBase instances.
        RImplInfoPtrArray iServiceImplInfoArray;

        // Implementation info array of CASYCommandHandlerBase instances.
        RImplInfoPtrArray iHandlerImplInfoArray;

        // Implementation uid array of CASYCommandHandlerBase instances.
        RArray<TUid> iImplUidsHandlerArray;

        // Status of iServiceImplInfoArray item count.
        TBool iListRet;

        // Index value of ASY while creating ASY.
        TInt iArrayItemCounter;

        // Handles loading steps while proceeding loading of all ASYs.
        TASYLoadingStep iStepToDo;

        // Pointer to timer handling time out during load ASYs.
        CASYProxyLoaderTimer* iTimer; // Owned

        // Pointer to ASYProxy.
        CASYProxy* iAsyProxy; // Not owned

    public:     // Friend classes

        friend void CASYProxyLoaderTimer::RunL();

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CASYPROXYASYLOADER_H

// End of File
