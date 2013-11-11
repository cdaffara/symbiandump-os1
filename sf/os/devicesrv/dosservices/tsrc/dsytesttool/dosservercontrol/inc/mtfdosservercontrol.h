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
* Description:  Declaration of MTFDosServerControl class
*
*/


#ifndef __MTFDOSSERVERCONTROL_H__
#define __MTFDOSSERVERCONTROL_H__

#include <e32base.h>

struct TTFDosServerControlTestCaseState;

NONSHARABLE_CLASS( MTFDosServerControl ) 
    {
    public:
        /**
        * Constructor
        */
        MTFDosServerControl( void );

        /**
        * Destructor
        */
        virtual ~MTFDosServerControl( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        MTFDosServerControl( const MTFDosServerControl& aStub );

        /**
        * Assignment operator is hidden
        */
        MTFDosServerControl& operator=( const MTFDosServerControl& aStub );
    
    public:
        /**
        * Calls a DosServer function with given parameters
        */
        virtual void CallDosFunctionL( TTFDosServerControlTestCaseState& aParameter ) = 0;

        /**
        * Notifies the controller about a DosServer event
        */
        virtual void NotifyDosEvent( TInt aEvent, TInt aParameter ) = 0;
    };


/**
* Gets the DosServer control interface
*/
IMPORT_C MTFDosServerControl* GetDosServerControl( void );

#endif
