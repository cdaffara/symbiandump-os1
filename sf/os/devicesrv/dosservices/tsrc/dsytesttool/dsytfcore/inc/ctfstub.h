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
* Description:  Declaration of CTFStub class
*
*/


#ifndef __CTFSTUB_H__
#define __CTFSTUB_H__

#include <e32base.h>

class MTFStubInterface;
class MTFStubTestCase;

/**
* Base class that needs to be implemented in the plug-in modules
*/
class CTFStub : public CBase
    {
    public:
        /**
        * Constructor
        * @param aStubType The plug-in module identifier
        */
        IMPORT_C CTFStub( TInt aStubType );

        /**
        * Destructor
        */
        IMPORT_C virtual ~CTFStub( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFStub( const CTFStub& aStub );

        /**
        * Assignment operator is hidden
        */
        CTFStub& operator=( const CTFStub& aStub );
    
    public:
        /**
        * Called when this stub has been added to the controller.
        */
        virtual void InitializeL( void ) = 0;

        /**
        * Called by the factory to determine if this stub 
        * is an instance of CTFRemoteStub class.
        */
        virtual TBool IsRemoteStub( void ) { return EFalse; }
    
        /**
        * Returns the plug-in specific part from currently active test case
        */
        IMPORT_C MTFStubTestCase* CurrentTestCase( void );

        /**
        * Returns the ID of the currently executing test case
        */
        IMPORT_C TInt CurrentTestCaseID( void );

    public:
        /**
        * Gets the type passed to constructor
        */
        TInt Type( void ) const;

        /**
        * Sets the controller callback interface. This is called by 
        * the framework after the stub has been obtained from the plug-in module.
        */
        void SetStubControl( MTFStubInterface* aStubControl );
    
    private:
        TInt iStubType;
        MTFStubInterface* iStubControl;
    };

#endif
