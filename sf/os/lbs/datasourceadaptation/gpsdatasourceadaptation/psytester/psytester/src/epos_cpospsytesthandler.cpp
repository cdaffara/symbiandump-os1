// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



// INCLUDE FILES
#include "epos_cpospsytesthandler.h"
#include "EPos_CPosResourceReader.h"

#include <psytester.rsg>

// CONSTANTS

_LIT(KPosResourceFile, "\\resource\\psytester.rsc");
// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYTestHandler::CPosPSYTestHandler() :
	CActive(CActive::EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

// EPOC default constructor can leave.
void CPosPSYTestHandler::ConstructL()
    {
	ReadResourceValuesL(iTestParameters);

	iPSYTester = CPosPSYTester::NewL();

    iPSYEComHandler = CPosPSYEComHandler::NewL();
	UpdatePSYListL();
	
	// Exclude the PSYs specified in the resource
	// file from listing
	ExcludePSYsL();
		
	iConsoleManager = CPosPSYConsoleManager::NewL(this);
    iConsoleManager->StartL();	

	// iTestAll = EFalse;
    }

// Two-phased constructor.
CPosPSYTestHandler* CPosPSYTestHandler::NewL()
    {
    CPosPSYTestHandler* self = new (ELeave) CPosPSYTestHandler;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYTestHandler::~CPosPSYTestHandler()
    {
    Cancel();

	delete iPSYTester;
    delete iPSYEComHandler;
    iInfoArray.Reset();
	iInfoArray.Close();
    delete iConsoleManager;
    }

// ---------------------------------------------------------
// CPosPSYTestHandler::StartPSYTesterL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestHandler::StartPSYTesterL(
    TInt aIndex)
    {
    if (!IsActive())
        {
        TPosPSYInfo psyInfo;
        iPSYIndex = aIndex;

        if (aIndex == 0)
            {
            iTestAll = ETrue;
            psyInfo.SetPSYBaseInfo(iInfoArray[aIndex]);
            iPSYEComHandler->GetSpecificPSYInformationL(psyInfo);
            iPSYTester->StartTestsL(psyInfo, iStatus, iTestParameters);
            SetActive();
			iConsoleManager->SetTestState(ERunAll);
            }
        else if (aIndex <= iInfoArray.Count())
            {
            iTestAll = EFalse;
            if (!iTestAll)
                {
				iConsoleManager->SetTestState(ETestsRunning);
                }
            psyInfo.SetPSYBaseInfo(iInfoArray[aIndex-1]);
            iPSYEComHandler->GetSpecificPSYInformationL(psyInfo);
            iPSYTester->StartTestsL(psyInfo, iStatus, iTestParameters);
            SetActive();
            }
        else
            {
			iConsoleManager->SetTestState(EIndexError);
            }
        }
    }

// ---------------------------------------------------------
// CPosPSYTestHandler::InfoList
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
RArray<TPosPSYBaseInfo> CPosPSYTestHandler::InfoList()
    {
    return iInfoArray;
    }

// ---------------------------------------------------------
// CPosPSYTestHandler::UpdatePSYListL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestHandler::UpdatePSYListL()
    {
    iInfoArray.Reset();
    iPSYEComHandler->GetPsyInfoListL(iInfoArray);
    }


// ---------------------------------------------------------
// CPosPSYTestHandler::ExcludePSYs
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestHandler::ExcludePSYsL()
    {
    
	  //+ Read the list of PSYs to be excluded
	  //+ from the resource file
	  CPosResourceReader* resources = CPosResourceReader::NewLC(KPosResourceFile);
	  
	  HBufC* excludeList = resources->ReadHBufCL(R_POS_PSY_TESTER_PSY_UID_EXCLUDES_LIST);
	    
	  if(excludeList)
	  {
	  	  TPtr buffer = excludeList->Des();

		  TLex uidList(buffer);
		  while(!uidList.Eos())
		  {
			TPtrC token = uidList.NextToken();
			TLex uid(token);
			TUint val;
			TInt err;
			err = uid.Val(val, EHex);
			if(KErrNone == err)
			{
				for(TInt arrIndex = 0; arrIndex < iInfoArray.Count(); arrIndex++)
				{
					if(iInfoArray[arrIndex].Uid().iUid == val)
					{
						iInfoArray.Remove(arrIndex);
						break;
					}
				}
			}
		  } 
		  
		  delete excludeList;
	  	
	  }
	  CleanupStack::PopAndDestroy(resources); 
     }


// ---------------------------------------------------------
// CPosPSYTestHandler::TestsRunning
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYTestHandler::TestsRunning()
    {
	return IsActive();
    }


// ---------------------------------------------------------
// CPosPSYTestHandler::ReadResourceValuesL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CPosPSYTestHandler::ReadResourceValuesL(
	TPosPSYTestParameters& aTestParameters)
	{
	
	CPosResourceReader* resources = CPosResourceReader::NewLC(KPosResourceFile);
	
	aTestParameters.SetNumberOfLocationRequests(
		resources->ReadInt16L(R_POS_PSY_TESTER_NR_LOCATION_REQUESTS));
    aTestParameters.SetNumberOfPartialUpdateRequests(
		resources->ReadInt16L(R_POS_PSY_TESTER_NR_PARTIAL_UPDATE_REQUESTS));
    aTestParameters.SetHeapSizePercentageLevel(
		resources->ReadInt16L(R_POS_PSY_TESTER_HEAP_SIZE_PERCENTAGE_LEVEL));
	aTestParameters.SetHorizontalAccuracyThreshold(
		resources->ReadInt16L(R_POS_PSY_TESTER_HORIZONTAL_ACCURACY_THRESHOLD));
	aTestParameters.SetMaxAgeMaxTestTime(
		resources->ReadInt16L(R_POS_PSY_TESTER_MAX_AGE_MAX_TEST_TIME));
	aTestParameters.SetMaxHorizontalAccuracy(
		resources->ReadInt32L(R_POS_PSY_TESTER_MAX_HORIZONTAL_ACCURACY));
	aTestParameters.SetMaxTimeToFirstFix(
		resources->ReadInt32L(R_POS_PSY_TESTER_MAX_TIME_TO_FIRST_FIX));
	aTestParameters.SetMaxVerticalAccuracy(
		resources->ReadInt16L(R_POS_PSY_TESTER_MAX_VERTICAL_ACCURACY));
	aTestParameters.SetNumberOfMemoryRequests(
		resources->ReadInt16L(R_POS_PSY_TESTER_NR_MEMORY_REQUESTS));
	aTestParameters.SetNumberOfMultipleSessionCancelTestInstances(
		resources->ReadInt16L(R_POS_PSY_TESTER_NR_MULTIPLE_SESSION_CANCEL_TEST_INSTANCES));
	aTestParameters.SetNumberOfMultipleSessionTestInstances(
		resources->ReadInt16L(R_POS_PSY_TESTER_NR_MULTIPLE_SESSION_TEST_INSTANCES));
	aTestParameters.SetNumberOfStressTestInstances(
		resources->ReadInt16L(R_POS_PSY_TESTER_NR_STRESS_TEST_INSTANCES));
	aTestParameters.SetResonableHorizontalAccuracy(
		resources->ReadInt16L(R_POS_PSY_TESTER_RESONABLE_HORIZONTAL_ACCURACY));
	aTestParameters.SetResonableVertictalAccuracy(
		resources->ReadInt16L(R_POS_PSY_TESTER_RESONABLE_VERTICAL_ACCURACY));
	aTestParameters.SetTimeToFixThreshold(
		resources->ReadInt16L(R_POS_PSY_TESTER_TIME_TO_NEXT_FIX_THRESHOLD));
	aTestParameters.SetVerticalAccuracyThreshold(
		resources->ReadInt16L(R_POS_PSY_TESTER_VERTICAL_ACCURACY_THRESHOLD));
		
	CleanupStack::PopAndDestroy(resources);
	}

// ---------------------------------------------------------
// CPosPSYTestHandler::GetTestParameters
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestHandler::GetTestParameters(
	TPosPSYTestParameters& aTestParameters)
	{
	aTestParameters = iTestParameters;
	}

// ---------------------------------------------------------
// CPosPSYTestHandler::SetTestParameters
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestHandler::SetTestParameters(
	TPosPSYTestParameters aTestParameters)
	{
	iTestParameters = aTestParameters;
	}
// ---------------------------------------------------------
// CPosPSYTestHandler::RunL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestHandler::RunL()
    {
    if (iStatus == KErrNone)
        {
        iPSYIndex++;
        if (iTestAll && iPSYIndex < iInfoArray.Count())
            {
            TPosPSYInfo psyInfo;
            psyInfo.SetPSYBaseInfo(iInfoArray[iPSYIndex]);
            iPSYEComHandler->GetSpecificPSYInformationL(psyInfo);
            iPSYTester->StartTestsL(psyInfo, iStatus, iTestParameters);
            SetActive();
            }
        else
            {
            iConsoleManager->SetTestState(ETestsCompletedCorrect);
            }
        }
    else
        {
		iConsoleManager->SetTestState(ETestsCompletedWithError);
        }
    }

// ---------------------------------------------------------
// CPosPSYTestHandler::DoCancel
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYTestHandler::DoCancel()
    {
    iPSYTester->Cancel();
    } 

// ---------------------------------------------------------
// CPosPSYTestHandler::RunError
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPSYTestHandler::RunError(
	TInt /*aErr*/)
    {
    Cancel();
    iPSYTester->Cancel();
    return KErrNone;
    }

//  End of File
