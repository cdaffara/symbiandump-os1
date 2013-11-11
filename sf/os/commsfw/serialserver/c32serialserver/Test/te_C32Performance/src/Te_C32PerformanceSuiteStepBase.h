/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file 
 @internalComponent
*/

#if (!defined TE_C32PERFORMANCE_STEP_BASE)
#define TE_C32PERFORMANCE_STEP_BASE

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <testexecutestepbase.h>
#else
#include <test/testexecutestepbase.h>
#endif

#include "Te_C32PerformanceSuiteDefs.h"
#include "C32COMM.H"
#include <e32cmn.h>

class TThreadData
/*
  This class is used to store thread details such as thread name,
  thread priority,test result,thread execution time  etc.
  iThreadName:- used to store name of the thread which is fetched from the ini file 
  iTaskName:- used to store name of the test case which is fetched from the ini file
  iThreadPriority:- used to store Priority of the thread which is fetched from the ini file
  iTestResult :- used to indicate the line no of the error occured while running tests.
  iThreadTime :- used to indicate the thread execution time to complete a test.
  iSemaphoreStatus :-  field can be either True or False.If True,semphores in each client 
  thread will be in wait state until all the CSYs are loaded
  iSemaphoreCount :- indicates total no of semaphores used.This value is read from ini file.
*/

{
	
public:
	
	TThreadData(    TPtrC  aThreadName,
					TPtrC  aThreadPriority, 
					TPtrC  aTaskName,
					TBool  aSemaphoreStatus,
					TInt   aSemaphoreCount)  :
				iThreadName(aThreadName),
				iThreadPriority(aThreadPriority),
				iTaskName(aTaskName),
				iSemaphoreStatus(aSemaphoreStatus),
				iThreadTime(0),
				iTestResult(0),
				iSemaphoreCount(aSemaphoreCount)
					{
		
					}
	TBuf<KMaxThreadNameLength> 		iThreadName;
	TBuf<KMaxThreadPriorityLength> 	iThreadPriority;
	TBuf<KMaxTaskNameLength>   		iTaskName;
		
	TBool	 iSemaphoreStatus;
	TInt64   iThreadTime;
	TInt  	 iTestResult;
	TInt     iSemaphoreCount;
	
};

class CTe_C32PerformanceSuiteStepBase : public CTestStep
/*
  This class is used to set the test result
*/
	{
public:
	virtual ~CTe_C32PerformanceSuiteStepBase();
	CTe_C32PerformanceSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
protected:
	//parse the ini file to get thread details
	virtual TInt ProcessNextDescriptionL();
	//create each client thread
	virtual TBool ThreadCreationL();
	//reads semaphore count from the ini file.
	virtual TInt ReadIniTotalSemaphoreCount();
	//initialize comm module
	TInt InitializeCommModule(const TDesC& aCsyName,const TDesC& aPortName0,const TDesC& aPortName1);
	//close RcommServer
	TInt ClosePorts();
	
	//returns the current id
	TInt GetCurrentIdx();
	
public:

    RCommServ commServ; 
    RComm iPortList[2]; 

protected:

	TBuf<KMaxTestExecuteNameLength> iSection; //currect section of the ini file
    
    TInt iTotalNoofSemaphores;//total no of semaphores.
    
    //enum used to return the success/failure result while parsing the ini file
	enum TParseIniFiles
         {
         EFailureIniParsing = 0,         
         ESuccessIniParsing = 1,
         EStopIniParsing 	= 2,
         };
	
private:
    TInt iIdx; //index of the current single step
	};

#endif

