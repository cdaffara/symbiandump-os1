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
* Description:  Declaration of CTFRemoteStub class
*
*/


#ifndef __CTFREMOTESTUB_H__
#define __CTFREMOTESTUB_H__

#include "ctfstub.h"

class CTFRemoteStub : public CTFStub
    {
    public:
        /**
        * Constructor
        * @param aStubType The plug-in module identifier
        */
        IMPORT_C CTFRemoteStub( TInt aStubType );

        /**
        * Destructor
        */
        IMPORT_C ~CTFRemoteStub( void );

    private:
        /**
        * Copy constructor is hidden
        */
        CTFRemoteStub( const CTFStub& aStub );

        /**
        * Assignment operator is hidden
        */
        CTFRemoteStub& operator=( const CTFStub& aStub );
    
    public:
        /**
        * Return ETrue
        */
        TBool IsRemoteStub( void ) { return ETrue; }

        /**
        * Processes a message and completes it. It is not necessary to 
        * complete the message immediately.
        */
        virtual void ProcessMessageAndCompleteL( const RMessage2& aMessage ) = 0;

        /**
        * Notifies the stub that the asynchronous message passed to
        * ProcessMessageAndCompleteL has been completed with given
        * status. This can be used to complete the test case.
        */
        virtual void MessageCompleteL( TInt aResult ) = 0;
    };


#endif
