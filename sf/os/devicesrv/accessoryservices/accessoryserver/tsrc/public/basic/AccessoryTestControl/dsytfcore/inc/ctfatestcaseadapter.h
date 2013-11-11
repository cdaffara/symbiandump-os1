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
* Description:  Declaration of CTFATestCaseAdapter class
*
*/


#ifndef __CTFATESTCASEADAPTER_H__
#define __CTFATESTCASEADAPTER_H__

#include <e32base.h>
#include "mtfatestcaseadapter.h"

class CTFATestCase;

/**
* Base class for test case adapters. This is implemented in the adapter DLL.
*/
//NONSHARABLE_CLASS( CTFATestCaseAdapter ): public CBase, public MTFATestCaseAdapter
class CTFATestCaseAdapter : public CBase, public MTFATestCaseAdapter
    {
    protected:
        /**
        * Constructor
        * @param aTestCase The test case wrapped into this adapter
        */
        IMPORT_C CTFATestCaseAdapter( CTFATestCase& aTestCase );
    
    public:
        /**
        * Destructor
        */
        IMPORT_C ~CTFATestCaseAdapter( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFATestCaseAdapter( const CTFATestCaseAdapter& aAdapter );

        /**
        * Assignment operator is hidden
        */
        CTFATestCaseAdapter& operator=( const CTFATestCaseAdapter& aAdapter );
    
    public:
        /**
        * Returns the type of test case wrapped into this adapter
        */
        IMPORT_C TTFATestType Type( void ) const;
    
    protected:
        /**
        * The wrapped test case is available to subclasses
        */
        CTFATestCase& iTestCase;
    };

#endif
