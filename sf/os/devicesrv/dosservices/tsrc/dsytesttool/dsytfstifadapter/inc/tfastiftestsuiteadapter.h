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
* Description:  Declaration of CTFAStifTestSuiteAdapter class
*
*/


#ifndef __TFASTIFTESTSUITEADAPTER_H__
#define __TFASTIFTESTSUITEADAPTER_H__

#include "ctfatestsuiteadapter.h"

class CTFATestSuite;

NONSHARABLE_CLASS( CTFAStifTestSuiteAdapter ): public CTFATestSuiteAdapter
    {
    public:
        /**
        * Creates a new suite adapter
        */
        static CTFAStifTestSuiteAdapter* NewLC( CTFATestSuite& aTestSuite, CTFAEntryPoint* aEntryPoint );

        /**
        * Destructor
        */
        virtual ~CTFAStifTestSuiteAdapter( void );
    
    protected:
        /**
        * Constructor
        */
        CTFAStifTestSuiteAdapter( CTFATestSuite& aTestSuite, CTFAEntryPoint* aEntryPoint );

    private:
        /**
        * Copy constructor is hidden
        */
        CTFAStifTestSuiteAdapter( const CTFAStifTestSuiteAdapter& aAdapter );

        /**
        * Assignment operator is hidden
        */
        CTFAStifTestSuiteAdapter& operator=( const CTFAStifTestSuiteAdapter& aAdapter );
    };

#endif
