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
* Description:  Declaration of MTFTestCaseInterface class
*
*/


#ifndef __MTFTESTCASEINTERFACE_H__
#define __MTFTESTCASEINTERFACE_H__

#include <e32base.h>

class CTFATestLog;

/**
* MTFTestCaseInterface is a callback interface that is given to stub test case implementations
* when they are initialized.
*/
NONSHARABLE_CLASS( MTFTestCaseInterface)
    {
    protected:
        /**
        * Default constructor
        */
        MTFTestCaseInterface( void );
    
    public:
        /**
        * Destructor
        */
        virtual ~MTFTestCaseInterface( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        MTFTestCaseInterface( const MTFTestCaseInterface& aInterface );

        /**
        * Assignment operator is hidden
        */
        MTFTestCaseInterface& operator=( const MTFTestCaseInterface& aInterface );
    
    public:
        /**
        * Called by the stub test case implementation to notify that the test is complete.
        * This can be called multiple times, but the first error result will become the test
        * result.
        * @param aTestResult The test result
        */
        virtual void TestComplete( TInt aTestResult ) = 0;

        /**
        * Returns the test case logger.
        */
        virtual CTFATestLog& Logger( void ) = 0;
    };

#endif
