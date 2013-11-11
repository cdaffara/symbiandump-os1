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
* Description:  Declaration of MTFStubInterface class
*
*/


#ifndef __MTFSTUBINTERFACE_H__
#define __MTFSTUBINTERFACE_H__

class MTFStubTestCase;

/**
* Interface to obtain the current plug-in specific test case from 
* the test case controller.
*/
NONSHARABLE_CLASS( MTFStubInterface )
    {
    protected:
        /**
        * Constructor
        */
        MTFStubInterface( void );
    
    public:
        /**
        * Destructor
        */
        virtual ~MTFStubInterface( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        MTFStubInterface( const MTFStubInterface& aInterface );

        /**
        * Assignment operator is hidden
        */
        MTFStubInterface& operator=( const MTFStubInterface& aInterface );
    
    public:
        /**
        * Returns a plug-in specific part of the currently executing test case.
        * @param aTestCaseType The identifier of the plug-in module
        */
        virtual MTFStubTestCase* CurrentTestCase( TInt aTestCaseType ) = 0;

        /**
        * Returns the ID of currently executing test case
        */
        virtual TInt CurrentTestCaseID( void ) = 0;
    };

#endif
