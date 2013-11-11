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
* Description:  Declaration of MTFATestSuiteAdapter class
*
*/


#ifndef __MTFATESTSUITEADAPTER_H__
#define __MTFATESTSUITEADAPTER_H__

#include "mtfatestadapter.h"

/**
* Test suite adapter interface is implemented by the adapter libraries
*/
//NONSHARABLE_CLASS( MTFATestSuiteAdapter ): public MTFATestAdapter
class MTFATestSuiteAdapter : public MTFATestAdapter
    {
    protected:
        /**
        * Constructor
        */
        IMPORT_C MTFATestSuiteAdapter( void );

    public:
        /**
        * Destructor
        */
        IMPORT_C virtual ~MTFATestSuiteAdapter( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        MTFATestSuiteAdapter( const MTFATestSuiteAdapter& aAdapter );

        /**
        * Assignment operator is hidden
        */
        MTFATestSuiteAdapter& operator=( const MTFATestSuiteAdapter& aAdapter );
    
    public:
        /**
        * Adds a test case adapter to this suite
        */
        virtual void AddL( MTFATestAdapter& aAdapter ) = 0;
    };

#endif
