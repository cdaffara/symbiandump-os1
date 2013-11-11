/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Definition of CTFDosControlTestCaseParam
*
*/


#ifndef __CTFDOSCONTROLTESTCASEPARAM_H__
#define __CTFDOSCONTROLTESTCASEPARAM_H__

#include <e32base.h>
#include <ctfstubtestcaseparam.h>
#include "tfdostypes.h"

class CTFDosControlTestCaseParam : public CTFStubTestCaseParam
    {
public:
    CTFDosControlTestCaseParam( void );

    void ConstructL( const TTFDosServerControlTestCaseState* aStates, TInt aStateCount );

    virtual ~CTFDosControlTestCaseParam( void );
    
private:

    CTFDosControlTestCaseParam( const CTFDosControlTestCaseParam& aParam );

    CTFDosControlTestCaseParam& operator=( const CTFDosControlTestCaseParam& aParam );
    
public:
    const TTFDosServerControlTestCaseState* States( void ) const;
    TInt StateCount( void ) const;

private:
    TTFDosServerControlTestCaseState* iStates;
    TInt iStateCount;
    };

#endif
