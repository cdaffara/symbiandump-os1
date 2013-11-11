/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Unit test code for RStarterSession class
*
*/
#ifndef __MT_RSTARTERSESSION_H__
#define __MT_RSTARTERSESSION_H__

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
// Function pointer related internal definitions
#ifndef __GCC32__
#define GETPTR
#else
#define GETPTR &
#endif  
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}

//  EXTERNAL INCLUDES
#include <stiftestmodule.h>

//  INTERNAL INCLUDES
#include <starterclient.h>

// FORWARD DECLARATIONS
class TCaseInfo;
class MT_RStarterSession;
// DESCRIPTION
// This a Test Module interface template
// that does not really do anything.
typedef TInt ( MT_RStarterSession::*TestFunction )( TTestResult& );
//  CLASS DEFINITION
/**
 *
 */
NONSHARABLE_CLASS( MT_RStarterSession )
	: public CTestModuleBase
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_RStarterSession* NewL(CTestModuleIf& aTestModuleIf);
        /**
         * Destructor
         */
        ~MT_RStarterSession();

    private:    // Constructors and destructors

        MT_RStarterSession( CTestModuleIf& aTestModuleIf );
        
        /**
        * Function returning test case name and pointer to test case function
        */
        const TCaseInfo Case( const TInt aCaseNumber ) const;

   	public: // Functions from base classes

        // @see CTestModuleBase
        virtual TInt GetTestCasesL( const TFileName& aTestCaseFile,
        RPointerArray<TTestCaseInfo>& aTestCases );

        // @see CTestModuleBase
        virtual TInt RunTestCaseL( const TInt aCaseNumber,
        const TFileName& aTestCaseFile,
		TTestResult& aResult );
            
    private:    // New methods

        void SetupL();
        void Teardown();

        TInt T_RStarterSession_Connect(TTestResult& aResult);
        TInt T_RStarterSession_SetState(TTestResult& aResult);
        TInt T_RStarterSession_SetStateToNormal(TTestResult& aResult);
        TInt T_RStarterSession_SetStateToAlarm(TTestResult& aResult);
        TInt T_RStarterSession_SetStateToCharging(TTestResult& aResult);
        TInt T_RStarterSession_SetStateToOffLine(TTestResult& aResult);
        TInt T_RStarterSession_SetStateToBTSap(TTestResult& aResult);
        TInt T_RStarterSession_Reset(TTestResult& aResult);
        TInt T_RStarterSession_Shutdown(TTestResult& aResult);
        TInt T_RStarterSession_ResetNetwork(TTestResult& aResult);
        TInt T_RStarterSession_IsRTCTimeValid(TTestResult& aResult);
        TInt T_RStarterSession_ActivateRfForEmergencyCall(TTestResult& aResult);
        TInt T_RStarterSession_DeactivateRfAfterEmergencyCall(TTestResult& aResult);
        TInt T_RStarterSession_EndSplashScreen(TTestResult& aResult);

    private:    // Data
		TestFunction    iMethod;
        RStarterSession iRStarterSession;

    };

// Function pointer related internal definitions

// Hack around known GCC bug.
#ifndef __GCC32__
    #define GETPTR
#else
    #define GETPTR &
#endif


// An internal structure containing a test case name and
// the pointer to function doing the test
class TCaseInfoInternal
    {
    public:
        const TText* iCaseName;
        TestFunction iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
    };

// An internal structure containing a test case name and
// the pointer to function doing the test
class TCaseInfo
    {
    public:
        TPtrC iCaseName;
        TestFunction iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
    TCaseInfo( const TText* a ) : iCaseName( ( TText* ) a )
        {
        };
    };

#endif      //  __MT_RSTARTERSESSION_H__

// End of file
