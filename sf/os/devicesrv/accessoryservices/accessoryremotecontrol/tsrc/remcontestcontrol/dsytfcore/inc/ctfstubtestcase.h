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
* Description:  Declaration of CTFStubTestCase class
*
*/


#ifndef __CTFSTUBTESTCASE_H__
#define __CTFSTUBTESTCASE_H__

#include <e32base.h>
#include "mtfstubtestcase.h"

class CTFATestLog;

/**
* Base class for plug-in specific test case implementations
*/
class CTFStubTestCase : public CBase, public MTFStubTestCase
    {
    public:
        /**
        * Constructor
        * @param aTestCaseType The plug-in module identifier
        */
        IMPORT_C CTFStubTestCase( TInt aTestCaseType );

        /**
        * Destructor
        */
        IMPORT_C virtual ~CTFStubTestCase( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFStubTestCase( const CTFStubTestCase& aCase );

        /**
        * Assignment operator is hidden
        */
        CTFStubTestCase& operator=( const CTFStubTestCase& aCase );
    
    public:
        /**
        * Sets the callback interface that is used to complete the plug-in specific test case
        */
        IMPORT_C virtual void SetCallback( MTFTestCaseInterface& aCallback );

        /**
        * Returns the type passed into constructor
        */
        IMPORT_C virtual TInt Type( void );

        /**
        * Initializes this test case. The subclass can add initialization
        * code into the InitL method, which is called from this method.
        */
        IMPORT_C virtual void SetupL( void );

        /**
        * Returns ETrue if CompleteTest has been called. EFalse otherwise
        */
        IMPORT_C virtual TBool IsComplete( void );

        /**
        * Stores the stub into iStub variable accessible to subclasses
        */
        IMPORT_C virtual void SetStub( CTFStub& aStub );

    protected:
        /**
        * Called by subclasses to complete the test
        */
        IMPORT_C virtual void CompleteTest( TInt aTestResult );

        /**
        * Returns the test case logger
        */
        IMPORT_C virtual CTFATestLog& Logger( void );

    protected:
        /**
        * Logs this test case
        */
        virtual void Log( TInt aDepth ) = 0;

        /**
        * Subclass initialization code
        */
        virtual void InitL( void ) = 0;

        /**
        * Runs the test case
        */
        virtual void RunL( void ) = 0;

        /**
        * Closes this test case
        */
        virtual void Teardown( void ) = 0;

    protected:
        /**
        * Stub object is accessible to subclasses
        */
        CTFStub* iStub;
    
    private:
        MTFTestCaseInterface* iCallback;
        TInt iTestCaseType;
        TBool iComplete;
    };

#endif
