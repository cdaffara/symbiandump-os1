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
* Description:  Declaration of MTFTestCase class
*
*/


#ifndef __MTFTESTCASE_H__
#define __MTFTESTCASE_H__

#include "tftypes.h"

class MTFStubTestCase;

/**
* Interface for test cases that are divided into different plug-in modules.
*/
NONSHARABLE_CLASS( MTFTestCase )
    {
    protected:
        /**
        * Default constructor
        */
        MTFTestCase( void );
    
    public:
        /**
        * Destructor
        */
        virtual ~MTFTestCase( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        MTFTestCase( const MTFTestCase& aCase );

        /**
        * Assignement operator is hidden
        */
        MTFTestCase& operator=( const MTFTestCase& aCase );
    
    public:
        /**
        * Returns the plug-in specific part that has the given type
        * @param aTestCaseType The type
        */
        virtual MTFStubTestCase* TestCase( TInt aTestCaseType ) = 0;

        /**
        * Returns the test type. This is the same function
        * as in CTFATest abstract base class.
        */
        virtual TTFATestType Type( void ) const = 0;

        /**
        * Returns the test case ID of this test case
        */
        virtual TInt TestCaseID( void ) const = 0;
    };

#endif
