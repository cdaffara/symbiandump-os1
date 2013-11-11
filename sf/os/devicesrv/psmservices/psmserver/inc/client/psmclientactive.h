/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM client active object class.
*
*/


#ifndef PSMCLIENTACTIVE_H
#define PSMCLIENTACTIVE_H

#include <e32base.h>

// FORWARD DECLARATIONS
class MPsmClientObserver;

/**
 *  Active object for PSM client. Active objects are hidden from actual clients
 *
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CPsmClientActive ) : public CActive
    {

    public:

        /**
         * C++ constructor.
         *
         * @param aObserver Observer for the completion of this active object
         * @param aInfo Contains information from the asynchronous request this 
         *              object is pending.
         */
        CPsmClientActive( MPsmClientObserver& aObserver );

        /**
         * Destructor.
         */
        virtual ~CPsmClientActive();

        /**
         * Reference to TRequestStatus
         */
        TRequestStatus& RequestStatus() { return iStatus; };

        /**
         * Sets this active object to active
         */
        void SetActive();

    protected: // From CActive

        void RunL();
        void DoCancel();

    private: // data

        MPsmClientObserver& iObserver;

    };

#endif // PSMCLIENTACTIVE_H 
