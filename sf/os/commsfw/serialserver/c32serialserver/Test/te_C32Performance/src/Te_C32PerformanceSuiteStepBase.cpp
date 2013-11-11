// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file 
 @internalComponent
*/

#include "Te_C32PerformanceSuiteStepBase.h"
#include "Te_C32PerformanceSuiteDefs.h"

// Device driver constants

TVerdict CTe_C32PerformanceSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_C32PerformanceSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_C32PerformanceSuiteStepBase::doTestStepL()
/**
 * @return - TVerdict
 * Implementation of doTestStepL virtual function
 * This function is used to set the test result (pass or fail).
 * Invokes ProcessNextDescriptionL() to validate ini file format.
 * Invokes ThreadCreationL() to create client threads
 * Takes care of multiple runs. 
 */
	{
    
    SetTestStepResult(EPass);
            
    TInt parseIniFile = ESuccessIniParsing;
    
    iTotalNoofSemaphores = ReadIniTotalSemaphoreCount();
        
    //iterate ProcessNextDescriptionL() until the last tag in the ini 
    //file is read.
    for (iIdx = 0;parseIniFile; iIdx++)
    	{
    	parseIniFile = ProcessNextDescriptionL();
    	//stop parsing the ini file if there are no tags to read
    	if(parseIniFile == EStopIniParsing)
    		{
    		break;
    		}
    	//set the test result to EFail if any fields are missing in the ini file 	
    	if(parseIniFile == EFailureIniParsing)
    		{
    		INFO_PRINTF1(_L("Error in Parsing the Ini file"));
    		SetTestStepResult(EFail);
    		return TestStepResult();
    		}	
    	}
   	//set the test result to EFail if ThreadCreationL() fails
    if(iIdx>0)
 		{
 		//invoke ThreadCreationL() to create client threads
 		TBool result = ThreadCreationL();
 		if (result == EFalse)
 			{
 			SetTestStepResult(EFail);
 			}
 			
 		}
   	//set the test result to EFail if there are no tags in the ini file
    if (iIdx==0)
        {
        INFO_PRINTF1(_L("Each test step must have at least one valid description."));
        INFO_PRINTF1(_L("This test step doesn't have any."));
        SetTestStepResult(EFail);
        }
  
	return TestStepResult();
	}
TInt CTe_C32PerformanceSuiteStepBase::GetCurrentIdx()
/**
 * Returns index of the current single step
 */
    {
    return iIdx;
    }	

CTe_C32PerformanceSuiteStepBase::~CTe_C32PerformanceSuiteStepBase()
	{
	}

CTe_C32PerformanceSuiteStepBase::CTe_C32PerformanceSuiteStepBase()
	{
	}
	
TInt CTe_C32PerformanceSuiteStepBase::ProcessNextDescriptionL()
	{
	return ESuccessIniParsing;
	}
TBool CTe_C32PerformanceSuiteStepBase::ThreadCreationL() 
	{
	return ETrue;
	}
TInt CTe_C32PerformanceSuiteStepBase::ReadIniTotalSemaphoreCount()
	{
	return 0;
	}
	
TInt CTe_C32PerformanceSuiteStepBase::InitializeCommModule(const TDesC& aCsyName,const TDesC& aPortName0,const TDesC& aPortName1)
	{
	//Creating a RcommSession, connecting to the C32 server	
   	TInt ret=commServ.Connect();
	RDebug::Printf("In CTe_C32PerformanceSuiteStepBase::InitializeCommModule().Connect Comm Server = %d",ret); 

	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect RCommServ"));
		return 0;
		}
	//load comm module		
	ret=commServ.LoadCommModule(aCsyName);
	RDebug::Printf("In CTe_C32PerformanceSuiteStepBase::InitializeCommModule().Load Comm Module= %d",ret); 

	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to Load Comm Module"));
		return 0;
		}
		
	//Open port for reading and writing
	ret=iPortList[0].Open(commServ,aPortName0,ECommShared, ECommRoleDTE);
	RDebug::Printf("In CTe_C32PerformanceSuiteStepBase::InitializeCommModule().Open port for reading = %d",ret); 
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open port"));
		return 0;
		}
	
	ret=iPortList[1].Open(commServ,aPortName1,ECommShared, ECommRoleDTE);
	RDebug::Printf("In CTe_C32PerformanceSuiteStepBase::InitializeCommModule().Open port for writing, = %d",ret); 
	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open port"));
		return 0;
		} 	
		
	return 0;	
	}
	
TInt CTe_C32PerformanceSuiteStepBase::ClosePorts()
	{
	//close comm ports
	iPortList[0].Close();
	iPortList[1].Close();
	commServ.Close();
	return 0;	
	}	

