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
* Description:  Declaration of CTFAActiveTest class
*
*/


#ifndef __CTFAACTIVETEST_H__
#define __CTFAACTIVETEST_H__

#include <e32base.h>

class MTFAActiveTest;

/**
* Provides the Symbian active object functionality to classes
* that cannot inherit from CActive.
*/
class CTFAActiveTest : public CActive
    {
    public:
        /**
        * Constructor
        * @param aCallback ActiveTestRunL of the callback is called from RunL of this object
        */
        IMPORT_C CTFAActiveTest( MTFAActiveTest& aCallback );

        /**
        * Destructor
        */
        IMPORT_C virtual ~CTFAActiveTest( void );

    private:
        /**
        * Copy constructor is hidden
        */
        CTFAActiveTest( const CTFAActiveTest& aTimer );

        /**
        * Assignment operator is hidden
        */
        CTFAActiveTest& operator=( const CTFAActiveTest& aTimer );
    
    protected:
        /**
        * Calls ActiveTestRunL of callback interface
        */
        void RunL( void );

        /**
        * Calls ActiveTestCancel of callback interface
        */
        virtual void DoCancel( void );
    
    public:
        /**
        * Returns the status of this object
        */
        IMPORT_C TRequestStatus& Status( void );

        /**
        * Calls SetActive of this object
        */
        IMPORT_C void DoSetActive( void );
    
    private:
        MTFAActiveTest& iCallback;
    };

#endif
