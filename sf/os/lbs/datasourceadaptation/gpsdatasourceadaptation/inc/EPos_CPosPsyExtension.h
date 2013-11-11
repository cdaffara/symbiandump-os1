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



#ifndef CPOSPSYEXTENSION_H
#define CPOSPSYEXTENSION_H

#include <lbs/epos_mpositionerstatus.h>

// FORWARD DECLARATIONS
class MPosStatusObserver;

// CLASS DECLARATION

/**
* Class used to extend the CPositioner member data list. It holds PSY specific
* information and services. Only one instance per module should exist.
*/
class CPosPsyExtension : public CBase, public MPositionerStatus
    {
    public:  // Constructors and destructor

        /**
        * C++ constructor.
        *
        * @param aImplementationUid The UID of the PSY implementation.
        * @param aStatusObserver The server object watching for status updates.
        */
        CPosPsyExtension(
        /* IN  */       TUid aImplementationUid,
        /* IN  */       MPosStatusObserver* aStatusObserver
        );

        /**
        * Destructor.
        */
        ~CPosPsyExtension();

    public:  // Functions from base classes

        /**
        * From MPositionerStatus. 
        * Reports that the positioner status has changed.
        *
        * @param aStatus The new status.
        */
        void ReportStatus(
        /* IN  */       const TPositionModuleStatus& aStatus
        );

    public:  // New functions

        /**
        * Registers a positioner as user of this PSY.
        */
        void RegisterPositioner();

        /**
        * De-registers a positioner as user of this PSY.
        *
        * @return remaining number of registred positioners.
        */
        TInt UnregisterPositioner();

    private:  // Constructors

        // By default, prohibit copy constructor
        CPosPsyExtension( const CPosPsyExtension& );
        // Prohibit assigment operator
        CPosPsyExtension& operator= ( const CPosPsyExtension& );

    public:  // Data

        TUid                    iImplementationUid;
        MPosStatusObserver*     iStatusObserver;

    private:  // Data

        TUint                   iRefCount;

    };  

#endif  // CPOSPSYEXTENSION_H

// End of File
