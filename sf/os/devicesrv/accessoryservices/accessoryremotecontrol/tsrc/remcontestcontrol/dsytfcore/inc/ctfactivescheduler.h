/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CTFActiveScheduler class
*
*/


#ifndef __CTFACTIVESCHEDULER_H__
#define __CTFACTIVESCHEDULER_H__

#include <e32base.h>

/**
* Active scheduler that does not panic when error occurs in RunL
*/
NONSHARABLE_CLASS( CTFActiveScheduler ): public CActiveScheduler
    {
    public:
        /**
        * Constructor
        */
        CTFActiveScheduler( void );
    
        /**
        * Destructor
        */
        ~CTFActiveScheduler( void );

    private:
        /**
        * Copy constructor is hidden
        */
        CTFActiveScheduler( const CTFActiveScheduler& aAdapter );

        /**
        * Assignment operator is hidden
        */
        CTFActiveScheduler& operator=( const CTFActiveScheduler& aAdapter );

    protected:
        /**
        * Called if RunL leaves
        */
        void Error( TInt aError ) const;
    };

#endif
