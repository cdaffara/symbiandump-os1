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



#ifndef __CTFACCESSORYTESTCASECONTROL_H__
#define __CTFACCESSORYTESTCASECONTROL_H__

#include "mtfaactivetest.h"
#include <ctfstubtestcase.h>
#include "TfAccessoryTestControlTypes.h"

class CTFATestTimer;
class CTFAccessoryTestCaseParamControl;

#define TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( aCaseId, aCaseStates ) \
    case aCaseId: \
        { \
        CTFAccessoryTestCaseParamControl* param = new ( ELeave ) CTFAccessoryTestCaseParamControl(); \
        CleanupStack::PushL( param ); \
        param->ConstructL( aCaseStates, sizeof ( aCaseStates ) / sizeof ( TTFAccessoryTestCaseStateControl ) ); \
        returnValue = new ( ELeave ) CTFAccessoryTestCaseControl( param ); \
        CleanupStack::Pop(); \
        CleanupStack::PushL( returnValue ); \
        STATIC_CAST( CTFAccessoryTestCaseControl*, returnValue )->ConstructL(); \
        CleanupStack::Pop(); \
        } \
        break

class CTFAccessoryTestCaseControl : public CTFStubTestCase, public MTFAActiveTest
    {
public:
    CTFAccessoryTestCaseControl( CTFAccessoryTestCaseParamControl* aParameters );
        void ConstructL( void );
        ~CTFAccessoryTestCaseControl( void );

private:
    CTFAccessoryTestCaseControl( const CTFAccessoryTestCaseControl& aStub );
    CTFAccessoryTestCaseControl& operator=( const CTFAccessoryTestCaseControl& aStub );
    
public:		

	void CheckRequest( TRequestStatus aStatus, TInt64 aValue, TUint32& aCapability, TInt* aErr = NULL );		
    void CheckObjectRequest( TRequestStatus aStatus, TAccValueTypeObject& aValue, TUint32& aCapability );       
	
public:
        void ActiveTestRunL( void );

protected:
        void Log( TInt aDepth );
        void InitL( void );
        void RunL( void );
        void Teardown( void );

protected:
    void DoCompleteTest( TInt aResult );
    void StartNextState( TInt aResult );
    void CallCurrentAccessoryFunctionL( void );
    TInt CurrentStateIndex( void ) const;
    CTFAccessoryTestCaseParamControl& Parameters( void );
    TTFAccessoryFunction CurrentAccessoryFunction( void ) const;
    TInt CurrentArg1( void ) const;
    CTFATestTimer* Timer( void );

private:
        TInt CheckResult( TInt aResult, TBool aIsEvent );
    
private:
    CTFAccessoryTestCaseParamControl* iParameters;
    TTFAccessoryTestCaseStateControl iCurrentState;
    TInt iCurrentStateIndex;
    CTFATestTimer* iTimer;
    TBool iCleanupWait;
    TBool iStatusReceive;
    TInt  iStoredArg1;
    TInt64 iStoredArg2;
    };

#endif
