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



#ifndef CPOSSERVERDELAYEDSHUTDOWN_H
#define CPOSSERVERDELAYEDSHUTDOWN_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
 *  Class to handle delayed server shutdown
 */
class CPosServerDelayedShutdown : public CTimer
    {
    private:    // Constructor
        CPosServerDelayedShutdown();

    public:     // Destructor
        ~CPosServerDelayedShutdown();
        static CPosServerDelayedShutdown* NewL();

    public:     // New functions
        void Start(TTimeIntervalMicroSeconds32 aDelay);

    private:
        void ConstructL();
        CPosServerDelayedShutdown( const CPosServerDelayedShutdown& );
        CPosServerDelayedShutdown& operator= ( const CPosServerDelayedShutdown& );

    protected:  // Functions from base classes
        void RunL();

    };

#endif // CPOSSERVERDELAYEDSHUTDOWN_H

// End of File