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
* Description:  Declaration of MTFStubControl class
*
*/


#ifndef __MTFSTUBCONTROL_H__
#define __MTFSTUBCONTROL_H__

class MTFTestCase;
class CTFStub;

/**
* Interface to the stub controller implementation
*/
NONSHARABLE_CLASS( MTFStubControl )
    {
    protected:
        /**
        * Default constructor
        */
        MTFStubControl( void );
    
    public:
        /**
        * Destructor
        */
        virtual ~MTFStubControl( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        MTFStubControl( const MTFStubControl& aControl );

        /**
        * Assignment operator is hidden
        */
        MTFStubControl& operator=( const MTFStubControl& aControl );
    
    public:
        /**
        * Sets the currently executing test case
        */
        virtual void SetCurrentTestCase( MTFTestCase* aTestCase ) = 0;

        /**
        * Gets the stub which has the given plug-in module identifier
        */
        virtual CTFStub* Stub( TInt aStubType ) = 0;
    };

#endif
