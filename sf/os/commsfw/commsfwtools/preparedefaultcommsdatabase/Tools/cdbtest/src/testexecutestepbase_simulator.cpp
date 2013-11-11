// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This contains CTestStep which is the base class for all test steps.
// This is a simulator for the TestExecuteStepBase.cpp file contained 
// within TestExecute and is designed for use with the TOOLS2 platform. 
// 
//

/**
 @file testexecutestepbase_simulator.cpp
*/
#include "testexecutestepbase.h"

CTestStep::CTestStep()
    {
    }

CTestStep::~CTestStep()
    {
    }


// Set a user-friendly test step name referred for a test step object from the script file
void CTestStep::SetTestStepName(const TDesC& aStepName)
    {
    iTestStepName.Copy(aStepName);
    }

// Set the actual test step error obtained to execution of test step to compare the expected error with the actual
void CTestStep::SetTestStepError(const TInt& aErrorCode)
    {
    iTestStepError = aErrorCode;
    if (aErrorCode < 0)
        {
        SetTestStepResult(EFail); 
        }
    }

// Test Step virtuals - Test Implementation methods
TVerdict CTestStep::doTestStepL()
    {
	return EFail;
    } 

TVerdict CTestStep::doTestStepPreambleL()
    {
	return EPass;
    }

TVerdict CTestStep::doTestStepPostambleL()
    {
	return EPass;
    }

// Retrieve the test case name/id
TBool CTestStep::TestCaseName(TDes& aTestCaseName)
    {
	return EFalse;
    }

void CTestStep::testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine)
 //< TEST(a)
     {
     if(TestStepResult() == EPass && !aCondition)            // If not true, change to fail!
         {
         SetTestStepResult(EFail);
        }
     }
 
void CTestStep::testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine, TBool aIgnoreStepResult)
 //< TEST(a)
     {
     if (aIgnoreStepResult)
         {
         if (!aCondition)
             {
             SetTestStepResult(EFail);
             }
         }
     else
         {
         testBooleanTrue(aCondition, aFile, aLine);
         }
     }


void CTestStep::testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine)
 //< TESTL(a)
     {
     testBooleanTrue(aCondition, aFile, aLine);
     if(!aCondition)
         User::Leave(TEST_ERROR_CODE);   // leave with standard error code
     }
