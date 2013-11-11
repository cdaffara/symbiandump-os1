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
* Description:
*
*/


#ifndef __CREMCONCONTROLTESTCASEPARAM_H__
#define __CREMCONCONTROLTESTCASEPARAM_H__

#include <e32base.h>
#include "ctfstubtestcaseparam.h"
#include "remcontypes.h"

class CRemConControlTestCaseParam : public CTFStubTestCaseParam
    {
public:
    CRemConControlTestCaseParam( void );
    void ConstructL( const TRemConControlTestCaseState* aStates, TInt aStateCount );
    virtual ~CRemConControlTestCaseParam( void );
    
private:
    CRemConControlTestCaseParam( const CRemConControlTestCaseParam& aParam );
    CRemConControlTestCaseParam& operator=( const CRemConControlTestCaseParam& aParam );
    
public:
    const TRemConControlTestCaseState* States( void ) const;
    TInt StateCount( void ) const;

private:
    TRemConControlTestCaseState* iStates;
    TInt iStateCount;
    };

#endif
