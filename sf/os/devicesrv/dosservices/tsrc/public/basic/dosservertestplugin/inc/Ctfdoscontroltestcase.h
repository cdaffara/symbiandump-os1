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
*     Definition of CTFDosServerControlTestCase
*
*/


#ifndef __CTFDOSSERVERCONTROLTESTCASE_H__
#define __CTFDOSSERVERCONTROLTESTCASE_H__

#include <mtfaactivetest.h>
#include <ctfstubtestcase.h>
#include "tfdostypes.h"

class CTFATestTimer;
class CTFDosControlTestCaseParam;

#define TF_DOS_SERVER_CONTROL_TEST_CASE_L( aCaseId, aCaseStates ) \
    case aCaseId: \
        { \
        CTFDosControlTestCaseParam* param = new ( ELeave ) CTFDosControlTestCaseParam(); \
        CleanupStack::PushL( param ); \
        param->ConstructL( aCaseStates, sizeof ( aCaseStates ) / sizeof ( TTFDosServerControlTestCaseState ) ); \
        returnValue = new ( ELeave ) CTFDosControlTestCase( param ); \
        CleanupStack::Pop(); \
        CleanupStack::PushL( returnValue ); \
        STATIC_CAST( CTFDosControlTestCase*, returnValue )->ConstructL(); \
        CleanupStack::Pop(); \
        } \
        break

class CTFDosControlTestCase : public CTFStubTestCase, public MTFAActiveTest
    {
public:
    CTFDosControlTestCase( CTFDosControlTestCaseParam* aParameters );
        void ConstructL( void );
        ~CTFDosControlTestCase( void );
    
private:
    CTFDosControlTestCase( const CTFDosControlTestCase& aStub );
    CTFDosControlTestCase& operator=( const CTFDosControlTestCase& aStub );
    
public:
        void NotifyDosEvent( TInt aEvent, TInt aParameter );

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
    void CallCurrentDosFunctionL( void );
    TInt CurrentStateIndex( void ) const;
    CTFDosControlTestCaseParam& Parameters( void );
    TTFDosFunction CurrentDosFunction( void ) const;
    TInt CurrentArg1( void ) const;
    CTFATestTimer* Timer( void );

private:
        TInt CheckResult( TInt aResult );
    
private:
    CTFDosControlTestCaseParam* iParameters;
    TTFDosServerControlTestCaseState iCurrentState;
    TInt iCurrentStateIndex;
    CTFATestTimer* iTimer;
    TBool iCleanupWait;
    TInt iStoredArg1;
    TInt iStoredArg2;
    };

#endif
