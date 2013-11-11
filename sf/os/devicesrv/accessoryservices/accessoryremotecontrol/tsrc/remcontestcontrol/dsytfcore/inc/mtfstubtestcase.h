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
* Description:  Declaration of MTFStubTestCase class
*
*/


#ifndef __MTFSTUBTESTCASE_H__
#define __MTFSTUBTESTCASE_H__

class MTFTestCaseInterface;
class CTFStub;

/**
* Interface that is implemented to add plug-in specific parts to test cases.
*/
NONSHARABLE_CLASS( MTFStubTestCase )
    {
    protected:
        /**
        * Default constructor
        */
        MTFStubTestCase( void );
    
    public:
        /**
        * Destructor is virtual so that framework can delete this object
        */
        virtual ~MTFStubTestCase( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        MTFStubTestCase( const MTFStubTestCase& aCase );

        /**
        * Assignment operator is hidden
        */
        MTFStubTestCase& operator=( const MTFStubTestCase& aCase );
    
    public:
        /**
        * Sets the callback interface. This is called by the framework
        * when an instance of MTFStubTestCase is returned via the plug-in
        * interface
        */
        virtual void SetCallback( MTFTestCaseInterface& aCallback ) = 0;

        /**
        * Returns the test case type. This must return the same type as the
        * CTFStub implementation within a plug-in module.
        */
        virtual TInt Type( void ) = 0;

        /**
        * Writes this object to log at given logger depth.
        */
        virtual void Log( TInt aDepth ) = 0;

        /**
        * Initializes this test case
        */
        virtual void SetupL( void ) = 0;

        /**
        * Runs this test case
        */
        virtual void RunL( void ) = 0;

        /**
        * Releases resources allocated to this test case
        */
        virtual void Teardown( void ) = 0;

        /**
        * Determines if this test case has completed
        */
        virtual TBool IsComplete( void ) = 0;

        /**
        * Sets the stub object of the plug-in module to this test case
        */
        virtual void SetStub( CTFStub& aStub ) = 0;
    };

#endif
