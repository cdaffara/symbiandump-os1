/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSITIONERREGISTRY_H
#define CPOSITIONERREGISTRY_H

#include <e32base.h>
#include <lbscommon.h>

// FORWARD DECLARATIONS
class MPosStatusObserver;
class MPosParameterObserver;
class MPositionerStatus;
class CPosPsyExtension;

// CLASS DECLARATION

/**
* Class used to keep track of all CPositioner instances. All such instances 
* should register/de-register themselves once to this class.
* 
*/
class CPositionerRegistry : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Singleton constructor.
        *
        * @param aImplementationUid PSY implementation UID
        * @return the one and only CPositionerRegistry instance.
        */
        static CPositionerRegistry* InstanceL(
        /* IN  */       MPosStatusObserver* aStatusObserver
        );

        /**
        * Singleton destructor.
        *
        * @param aImplementationUid PSY implementation UID
        */
        void Release();

    public:  // New functions

        /**
        * Registers a positioner as user of this PSY.
        *
        * @param aImplementationUid the implementation UID for the PSY.
        * @return the CPosPsyExtension instance, common for all CPositioner
        *         instances with the same implementation UID.
        */
        CPosPsyExtension* RegisterPositionerL(
        /* IN  */       TUid aImplementationUid
        );

        /**
        * De-registers a positioner as user of this PSY.
        *
        * @param aImplementationUid the implementationUid for the PSY.
        */
        void DeregisterPositioner(
        /* IN  */       TUid aImplementationUid
        );
        
        static TInt						iInstanceCount;

    private:  // New functions

        TInt Find(TUid aImplementationUid);

    private:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CPositionerRegistry(MPosStatusObserver* aStatusObserver);

        /**
        * Destructor.
        */
        ~CPositionerRegistry();

        // By default, prohibit copy constructor
        CPositionerRegistry( const CPositionerRegistry& );
        // Prohibit assigment operator
        CPositionerRegistry& operator= ( const CPositionerRegistry& );

    private: // Data

        RPointerArray<CPosPsyExtension> iPsyExtensions;
        TInt                            iRefCount;
        MPosStatusObserver*             iStatusObserver;
        static CPositionerRegistry* 	iInstance;
        
    };  

#endif  // CPOSITIONERREGISTRY_H

// End of File
