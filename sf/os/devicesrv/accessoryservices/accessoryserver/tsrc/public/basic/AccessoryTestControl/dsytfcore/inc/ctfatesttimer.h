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
* Description:  Declaration of CTFATestTimer class
*
*/


#ifndef __CTFATESTTIMER_H__
#define __CTFATESTTIMER_H__

#include <e32base.h>

class MTFAActiveTest;

/**
* A timer utility class
*/
class CTFATestTimer : public CTimer
    {
    public:
        /**
        * Creates a new timer.
        * @param aCallback ActiveTestRunL of the callback interface is called when timer completes
        */
        IMPORT_C static CTFATestTimer* NewL( MTFAActiveTest& aCallback );

        /**
        * Destructor
        */
        IMPORT_C ~CTFATestTimer( void );

    private:
        /**
        * Constructor
        */
        CTFATestTimer( MTFAActiveTest& aCallback );

        /**
        * Copy constructor is hidden
        */
        CTFATestTimer( const CTFATestTimer& aTimer );

        /**
        * Assignment operator is hidden
        */
        CTFATestTimer& operator=( const CTFATestTimer& aTimer );
    
    protected:
        /**
        * Called by active scheduler. Calls ActiveTestRunL of the callback interface.
        */
        void RunL( void );
    
    private:
        MTFAActiveTest& iCallback;
    };

#endif
