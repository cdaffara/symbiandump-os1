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
* Description:  Declaration of CTFATest class
*
*/


#ifndef __CTFATEST_H__
#define __CTFATEST_H__

#include <e32base.h>
#include "tftypes.h"

class CTFATestEnvironment;

/**
* Abstract base class for test cases and test suites
*/
class  CTFATest : public CBase
    {
    protected:
        /**
        * Constructor
        * @param aName The name of the test object
        */
        CTFATest( const TDesC& aName );
    
    public:
        /**
        * Destructor
        */
        virtual ~CTFATest( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFATest( const CTFATest& aTest );

        /**
        * Assignment operator is hidden
        */
        CTFATest& operator=( const CTFATest& aTest );
    
    public:
        /**
        * Returns the name
        */
        IMPORT_C const TDesC& Name( void ) const;

        /**
        * Sets the test environment
        */
        IMPORT_C virtual void SetEnvironment( CTFATestEnvironment& aEnvironment );

        /**
        * Gets the test environment
        */
        IMPORT_C CTFATestEnvironment &Environment( void );

    public:
        /**
        * Returns the type of this test object
        */
        virtual TTFATestType Type( void ) const = 0;
    
    private:
        CTFATestEnvironment *iEnvironment ;
        TBuf<KTFAMaxTestNameLength> iName;
    };

#endif
