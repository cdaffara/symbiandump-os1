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
* Description:  Declaration of CTFStubControl class
*
*/


#ifndef __CTFSTUBCONTROL_H__
#define __CTFSTUBCONTROL_H__

#include <e32base.h>
#include "mtfstubinterface.h"
#include "mtfstubcontrol.h"

/**
* Stub controller contains a list of CTFStub objects obtained from the plug-in modules
*/
NONSHARABLE_CLASS( CTFStubControl ): public CBase, public MTFStubInterface, public MTFStubControl
    {
    public:
        /**
        * Creates a stub controller
        */
        static CTFStubControl* NewL( void );

        /**
        * Destructor
        */
        virtual ~CTFStubControl( void );
    
    private:
        /**
        * Default constructor
        */
        CTFStubControl( void );

        /**
        * Copy constructor is hidden
        */
        CTFStubControl( const CTFStubControl& aControl );

        /**
        * Assignment operator is hidden
        */
        CTFStubControl& operator=( const CTFStubControl& aControl );
    
    public:
        /**
        * Sets the currently executing test case
        */
        void SetCurrentTestCase( MTFTestCase* aTestCase );

        /**
        * Returns the stub object that has the given plug-in module identifier
        */
        CTFStub* Stub( TInt aStubType );

        /**
        * Adds a stub to this controller
        */
        void AddStubL( CTFStub& aStub );

        /**
        * Returns the stub count
        */
        TInt Count( void ) const;

        /**
        * Returns the stub object at the given index
        */
        CTFStub* StubAt( TInt aIndex );

        /**
        * Returns the plug-in specific part from the currently executing test case
        */
        MTFStubTestCase* CurrentTestCase( TInt aTestCaseType );

        /**
        * Returns the ID of the currently executing test case
        */
        TInt CurrentTestCaseID( void );
    
    private:
        RPointerArray<CTFStub> iStubs;
        MTFTestCase* iCurrentTestCase;
    };

#endif
