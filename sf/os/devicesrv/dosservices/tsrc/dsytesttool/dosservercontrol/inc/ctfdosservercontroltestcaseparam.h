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
* Description:  Declaration of CTFDosServerControlTestCaseParam class
*
*/


#ifndef __CTFDOSSERVERCONTROLTESTCASEPARAM_H__
#define __CTFDOSSERVERCONTROLTESTCASEPARAM_H__

#include <e32base.h>
#include "ctfstubtestcaseparam.h"
#include "tfdosservertypes.h"

NONSHARABLE_CLASS( CTFDosServerControlTestCaseParam ): public CTFStubTestCaseParam
    {
    public:
        /**
        * Constructor
        */
        CTFDosServerControlTestCaseParam( void );

        /**
        * 2nd phase constructor
        */
        void ConstructL( const TTFDosServerControlTestCaseState* aStates, TInt aStateCount );

        /**
        * Destructor
        */
        virtual ~CTFDosServerControlTestCaseParam( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFDosServerControlTestCaseParam( const CTFDosServerControlTestCaseParam& aParam );

        /**
        * Assignment operator is hidden
        */
        CTFDosServerControlTestCaseParam& operator=( const CTFDosServerControlTestCaseParam& aParam );
    
    public:
        /**
        * Gets the test case states
        */
        const TTFDosServerControlTestCaseState* States( void ) const;

        /**
        * Gets the state count
        */
        TInt StateCount( void ) const;

    private:
        TTFDosServerControlTestCaseState* iStates;
        TInt iStateCount;
    };

#endif
