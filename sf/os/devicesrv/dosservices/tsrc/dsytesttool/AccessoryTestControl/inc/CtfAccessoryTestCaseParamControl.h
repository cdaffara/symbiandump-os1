/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __CTFACCESSORYTESTCASEPARAMCONTROL_H__
#define __CTFACCESSORYTESTCASEPARAMCONTROL_H__

#include <e32base.h>
#include <ctfstubtestcaseparam.h>
#include "TfAccessoryTestControlTypes.h"

class CTFAccessoryTestCaseParamControl : public CTFStubTestCaseParam
    {
public:
    CTFAccessoryTestCaseParamControl( void );

    void ConstructL( const TTFAccessoryTestCaseStateControl* aStates, TInt aStateCount );

    virtual ~CTFAccessoryTestCaseParamControl( void );
    
private:

    CTFAccessoryTestCaseParamControl( const CTFAccessoryTestCaseParamControl& aParam );

    CTFAccessoryTestCaseParamControl& operator=( const CTFAccessoryTestCaseParamControl& aParam );
    
public:
    const TTFAccessoryTestCaseStateControl* States( void ) const;
    TInt StateCount( void ) const;

private:
    TTFAccessoryTestCaseStateControl* iStates;
    TInt iStateCount;
    };

#endif
