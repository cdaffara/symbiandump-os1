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
* Description:  Declaration of CTFStubTestCaseParam class
*
*/


#ifndef __CTFSTUBTESTCASEPARAM_H__
#define __CTFSTUBTESTCASEPARAM_H__

#include <e32base.h>

/**
* Root class for plug-in specific test case parameters
*/
class CTFStubTestCaseParam : public CBase
    {
    public:
        /**
        * Default constructor
        */
        IMPORT_C CTFStubTestCaseParam( void );

        /**
        * Destructor
        */
        IMPORT_C virtual ~CTFStubTestCaseParam( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFStubTestCaseParam( const CTFStubTestCaseParam& aParam );

        /**
        * Assignment operator is hidden
        */
        CTFStubTestCaseParam& operator=( const CTFStubTestCaseParam& aParam );
    };

#endif
