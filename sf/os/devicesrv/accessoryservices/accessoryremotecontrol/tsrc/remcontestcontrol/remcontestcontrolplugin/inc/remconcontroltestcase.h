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



#ifndef __CREMCONCONTROLTESTCASE_H__
#define __CREMCONCONTROLTESTCASE_H__

#include "mtfaactivetest.h"
#include "ctfstubtestcase.h"
#include "remcontypes.h"

class CTFATestTimer;
class CRemConControlTestCaseParam;

#define TF_REMCON_CONTROL_TEST_CASE_L( aCaseId, aCaseStates ) \
    case aCaseId: \
        { \
        CRemConControlTestCaseParam* param = new ( ELeave ) CRemConControlTestCaseParam(); \
        CleanupStack::PushL( param ); \
        param->ConstructL( aCaseStates, sizeof ( aCaseStates ) / sizeof ( TRemConControlTestCaseState ) ); \
        returnValue = new ( ELeave ) CRemConControlTestCase( param ); \
        CleanupStack::Pop(); \
        CleanupStack::PushL( returnValue ); \
        STATIC_CAST( CRemConControlTestCase*, returnValue )->ConstructL(); \
        CleanupStack::Pop(); \
        } \
        break

class CRemConControlTestCase : public CTFStubTestCase, public MTFAActiveTest
    {
public:
    CRemConControlTestCase( CRemConControlTestCaseParam* aParameters );
        void ConstructL( void );
        ~CRemConControlTestCase( void );

private:
    CRemConControlTestCase( const CRemConControlTestCase& aStub );
    CRemConControlTestCase& operator=( const CRemConControlTestCase& aStub );
    
public:		

	
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
    void CallCurrentApiFunctionL( void );
    TInt CurrentStateIndex( void ) const;
    CRemConControlTestCaseParam& Parameters( void );
    TTFRemConFunction CurrentDosFunction( void ) const;
    TInt CurrentArg1( void ) const;
    CTFATestTimer* Timer( void );

private:
    
private:
    CRemConControlTestCaseParam* iParameters;
    TRemConControlTestCaseState iCurrentState;
    TInt iCurrentStateIndex;
    CTFATestTimer* iTimer;
    TBool iCleanupWait;
    TInt  iStoredArg1;
    TInt iStoredArg2;
    };

#endif
