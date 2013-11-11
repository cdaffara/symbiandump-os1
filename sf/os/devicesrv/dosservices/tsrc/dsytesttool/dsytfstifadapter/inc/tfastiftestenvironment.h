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
* Description:  Declaration of CTFAStifTestEnviroment class
*
*/


#ifndef __TFASTIFTESTENVIRONMENT_H__
#define __TFASTIFTESTENVIRONMENT_H__

#include "ctfatestenvironment.h"

NONSHARABLE_CLASS( CTFAStifTestEnvironment ): public CTFATestEnvironment
    {
    public:
        /**
        * Constructo
        */
        CTFAStifTestEnvironment( CTFATestLog& aLogger );

        /**
        * Destructor
        */
        virtual ~CTFAStifTestEnvironment( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFAStifTestEnvironment( const CTFAStifTestEnvironment& aEnv );

        /**
        * Assignment operator is hidden
        */
        CTFAStifTestEnvironment& operator=( const CTFAStifTestEnvironment& aEnv );
    };

#endif
