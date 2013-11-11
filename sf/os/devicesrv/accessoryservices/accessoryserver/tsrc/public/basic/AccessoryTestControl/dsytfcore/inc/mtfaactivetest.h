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
* Description:  Declaration of MTFAActiveTest class
*
*/


#ifndef __MTFAACTIVETEST_H__
#define __MTFAACTIVETEST_H__

#include <e32base.h>

/**
* Callback interface for test cases that contain active objects.
*/
class MTFAActiveTest 
    {
    public:
        /**
        * Default constructor
        */
        IMPORT_C MTFAActiveTest();

        /**
        * Destructor
        */
        IMPORT_C virtual ~MTFAActiveTest( void );

    private:
        /**
        * Copy constructor is hidden
        */
        MTFAActiveTest( const MTFAActiveTest& aTest );

        /**
        * Assignment operator is hidden
        */
        MTFAActiveTest& operator=( const MTFAActiveTest& aTest );
    
    public:
        /**
        * Called from CActive::RunL
        */
        virtual void ActiveTestRunL( void ) = 0;

        /**
        * Called from CActive::DoCancel
        */
        virtual void ActiveTestCancel( void ) {}
    };

#endif
