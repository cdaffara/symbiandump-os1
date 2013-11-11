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
// This file is used to spawn different client threads
// 
//

/**
 @file
 @internalComponent
*/

#include "Te_C32PerformanceSuiteStepBase.h"
#include "C32RunThreadStep.h"
#include "Te_C32PerformanceSuiteDefs.h"

#include "CUSBMTTests.h"
#include "CDummyMTTests.h"
#include "CECUARTMTTests.h"
#include "CLoopbackMTTests.h"
#include "CDummyCsy1MTTests.h"
#include "CDummyCsy2MTTests.h"
#include "CDummyCsy3MTTests.h"

TInt C32PerformanceThread::iCountSemaphores = 0;

CC32RunThreadStep::~CC32RunThreadStep()
/**
 * Destructor
 */
	{
	for (int k =0 ; k<iTaskCounter; k++ )
		{
		 delete iThreadData[k];
		 delete iNetPerformanceThread[k];
		}
	}

CC32RunThreadStep::CC32RunThreadStep()
/**
 * Constructor 
 */
	{
	SetTestStepName(KC32RunThreadStep);
	}

TVerdict CC32RunThreadStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_C32PerformanceSuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CC32RunThreadStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

TInt CC32RunThreadStep::ReadIniTotalSemaphoreCount()
/**
 * @return - TInt code
 * Override of base class virtual
 * Finds semaphore count from the ini file.
 */
	{
	TInt  semaphoreCount = 0;
	TInt  count		     = 0;
	TBool findField      = ETrue;
	TBool semStatus;
		
	while(findField)
		{
		//parse thread details from ini files
		iSection.Copy(KThreadSection);
		iSection.AppendNum(count++);
		
		findField = GetBoolFromConfig(iSection, KSemaphore, semStatus);
		if(findField)
		   {
	   	   if(semStatus)
	   	 	   {
	   	 	   //increment if semaphore field is set to True.
	   	 	   semaphoreCount++;
	   	 	   }
	   	   }
	    else
	       {
	       findField = EFalse;
	       }
	    }

	return semaphoreCount;	
	}

TInt CC32RunThreadStep::ProcessNextDescriptionL()
/**
 * @return - TInt
 * Implementation of ProcessNextDescriptionL virtual function
 * This function is used to parse ini file to populate thread details
 * returns EFailureIniParsing if any fields are missing in the ini file
 * returns EStopIniParsing if there are no fields to parse
 * returns ESuccessIniParsing if parsing of ini file is success
 */
	 {
	  	  
	  TUint noofFields  = 0;
	  iTaskCounter = GetCurrentIdx();
	  
	  //parse thread details from ini files
	  iSection.Copy(KThreadSection);
	  iSection.AppendNum(GetCurrentIdx());
	  
	  TPtrC threadName;
	  TBool semStatus;
	  TPtrC taskName;
	  TPtrC threadPriority;
	 	
	  
	  //this field is used to fetch the thread name from the ini file		    
	  if(GetStringFromConfig(iSection, KThreadName, threadName) == EFalse)
	  	{
	     noofFields++;
	    }
	  // configure the priority of the thread
	  if(GetStringFromConfig(iSection, KThreadPriority, threadPriority) == EFalse)
	  	{
	  	 noofFields++;
	   	}
	  //this field is used to fetch the test case name from the ini file
	  if(GetStringFromConfig(iSection, KTaskName, taskName) == EFalse)
	  	{
	     noofFields++;
	    }
	  /*
	    this field can be either True or False.If True,semphores in client 
	  	thread will be in wait state until all the CSYs are loaded
	  */
	  if(GetBoolFromConfig(iSection, KSemaphore, semStatus) == EFalse)
	  	{
	     noofFields++;
	    }
	  /* 
	    stop parsing the ini file if there are no tags left in the ini file
	    return EStopIniParsing.
	  */   
	  if (noofFields == KTotalIniFieldsNo )
	  	{
	  	 return EStopIniParsing;	
	  	}
	  // return EFailureIniParsing if any fields are missing.
	  if( noofFields > 0 )
	  	{
	  	 return EFailureIniParsing;
	  	}
	   //store values fetched from ini file to TThreadData class.	  
	   iThreadData[iTaskCounter] = new(ELeave) TThreadData
	  								( threadName,threadPriority,taskName,
	  								  semStatus,iTotalNoofSemaphores);	
		
	  return ESuccessIniParsing;
	 }
TBool CC32RunThreadStep::ThreadCreationL()
 /**
 * @return - TBool
 * Implementation of ThreadCreationL virtual function
 * This function is used to spawn each client thread and result will be logged to 
 * a HTML file.
 * returns EFalse if a thread panics or test case fails.
 */	  	 
 	 {
 	 //create a global semaphore     
                       
     RSemaphore clientSemaphore;
     CleanupClosePushL(clientSemaphore);
     
     TInt result = clientSemaphore.CreateGlobal(KSemaphoreName, 0);
     
     //create threads based on the information given in ini file    
         
	 for (TInt n = 0 ; n<iTaskCounter; n++ )
		 {
		 iNetPerformanceThread[n] = new(ELeave) C32PerformanceThread( *iThreadData[n] );
		 TName name;
		 name.Copy( iThreadData[n]->iThreadName );
		 RThread thread;
		 const TUint KStackSize=0x1000;
		 TInt nErr;
		 if ( (nErr = thread.Create(name, C32PerformanceThread::RunL, KStackSize, NULL, iNetPerformanceThread[n])) == KErrNone)
			 {
			 thread.SetPriority(ParseThreadPriority(iThreadData[n]->iThreadPriority));
			 thread.Logon(iRequestStatus[n]);
			 thread.Resume();
			 thread.Close();
			 }
		 else
			 {
			 INFO_PRINTF3(_L(" Failed to create thread = %S .Failed with error = %d  "),&iThreadData[n]->iThreadName,nErr);	 	
			 }
		 }
	  	  
	  	   
	  INFO_PRINTF1(_L("------------------------ C32 Performance Results -------------------------------- "));
		  	
	  for (TInt j=0 ; j<iTaskCounter; j++ )
		 {
		 //wait for the thread to complete
		 User::WaitForRequest( iRequestStatus[j] );

		 //print the results to a log file	
    	 INFO_PRINTF2(_L(" Thread Name 			   	 = %S  "),&iThreadData[j]->iThreadName);
    	 INFO_PRINTF2(_L(" Task Name 			   	 = %S  "),&iThreadData[j]->iTaskName);
    	 INFO_PRINTF2(_L(" Time taken to execute the thread = %ld microseconds "), iThreadData[j]->iThreadTime);
    	 INFO_PRINTF2(_L(" Error occured in line no         = %d. Expected value was 0 "), iThreadData[j]->iTestResult);
    	 INFO_PRINTF2(_L(" Thread terminated value          = %d. Expected value was 0 "),iRequestStatus[j].Int());
    	
    	 INFO_PRINTF1(_L("--------------------------------------------------------------------------------- "));	
		}
		
	  CleanupStack::PopAndDestroy();	
      
      /*
        iTestResult is used to indicate the line no of the error occured in each test case.
        return EFalse,if a test case failed or a thread paniced
      */
      for (TInt j=0 ; j<iTaskCounter; j++ )
		  {	
	   		if((iThreadData[j]->iTestResult > 0) || (iRequestStatus[j].Int() > 0 ))
    	 		{
    	 		return EFalse;
    	 	    }
		  }
     return ETrue;
     }
TThreadPriority CC32RunThreadStep::ParseThreadPriority(TPtrC athreadPriority)
/**
 * @return - TThreadPriority
 * Implementation of ParseThreadPriority  function
 * This function is used to set priority of each client thread
 */	
	{
 
	 TThreadPriority priority = EPriorityNormal;
	 
	 if (athreadPriority.Compare(KEPriorityNull)==0)
	 { priority = EPriorityNull;  }
	 else if(athreadPriority.Compare(KEPriorityMuchLess)==0)
	 { priority = EPriorityMuchLess;	}
	 else if(athreadPriority.Compare(KEPriorityLess)==0)
	 { priority = EPriorityLess;	}
	 else if(athreadPriority.Compare(KEPriorityNormal)==0)
	 { priority = EPriorityNormal;	}
	 else if(athreadPriority.Compare(KEPriorityMore)==0)
	 { priority = EPriorityMore;	}
	 else if(athreadPriority.Compare(KEPriorityMuchMore)==0)
	 { priority = EPriorityMuchMore;	}
	 else if(athreadPriority.Compare(KEPriorityRealTime)==0)
	 { priority = EPriorityRealTime;	}
	 else if(athreadPriority.Compare(KEPriorityAbsoluteVeryLow)==0)
	 { priority = EPriorityAbsoluteVeryLow;	}
	 else if(athreadPriority.Compare(KEPriorityAbsoluteLow)==0)
	 { priority = EPriorityAbsoluteLow;	}
	 else if(athreadPriority.Compare(KEPriorityAbsoluteBackground)==0)
	 { priority = EPriorityAbsoluteBackground;	}
	 else if(athreadPriority.Compare(KEPriorityAbsoluteForeground)==0)
	 { priority = EPriorityAbsoluteForeground;	}
	 else if(athreadPriority.Compare(KEPriorityAbsoluteHigh)==0)
	 { priority = EPriorityAbsoluteHigh;	}
		
	 return priority;
	}

	  
TInt C32PerformanceThread::RunL(TAny* aNetPerformanceThread)
/**
 * @return - TInt
 * Implementation of RunL function
 * The RunL function is invoked each time when you create a client thread.
 * A global semaphore is implemented to block the client threads until all the CSYs are loaded.
 * iCountSemaphores variable is used to count the no of semaphores used.
 * InitTestDataL() is used to load the CSY from the corresponding test class.
 * RunAllTests() is used to perform read/write operation in each client thread once all the semaphore
 * are signalled.  
 */	
	
	{
	
	C32PerformanceThread *perThread = static_cast<C32PerformanceThread*>(aNetPerformanceThread);
   
    TInt  totalNoofSemaphores = perThread->iTThreadData.iSemaphoreCount;
    TBool semStatus           = perThread->iTThreadData.iSemaphoreStatus; 
    
    //search for the global semaphore        
    RSemaphore clientSemaphore;
		 	
	TFindSemaphore findTimeServer(KSemaphoreName);
	 
	TFullName name;
	TInt result1 = findTimeServer.Next(name);

	clientSemaphore.OpenGlobal(name);
	
	/*
	active scheduler is installed in each thread so that the test code doesn't
	need to install it if required. 
	*/   	
	CTrapCleanup* pCleanup = CTrapCleanup::New(); 
	CActiveScheduler* pScheduler = new CActiveScheduler;
	TInt ret = KErrNone;
	if ( pCleanup && pScheduler )
		{
      	 //install active scheduler for each thread     		
      	 CActiveScheduler::Install(pScheduler );
      	    	
		 TRAPD(err,perThread->InitTestDataL());
		 if(err)
		    {
		    User::Leave(KErrAbort);
		    }
      	  
      	 //signal all the semaphores if all the CSYs are loaded.
      	 if(totalNoofSemaphores == (iCountSemaphores+1))
        	{
        	 RDebug::Printf("signal all the semaphores if all the CSYs are loaded = %d",totalNoofSemaphores); 
        	 clientSemaphore.Signal(iCountSemaphores);
        	}
         //check whether the Semaphore status is True.
         else if(semStatus)
        	{
        	 //count no of semaphores required. 
	  		 iCountSemaphores++;
	  		 //block the client thread
	  		 clientSemaphore.Wait();
        	}
        //invoke RunAllTests() once all the semaphores are signalled.
        RDebug::Printf("run all MT tests"); 
        perThread->RunAllTests();
  
        }
	delete pScheduler;
    delete pCleanup;	
   
	return ret;
	}

void C32PerformanceThread::InitTestDataL()
/**
 * @return - void
 * Implementation of InitTestData function
 * New Tests are added here
 */	
	{
	
	 if(iTThreadData.iTaskName == _L("LoopbackTests"))
	 	{
        iPerformTest = new (ELeave)CLoopbackMTTests(iTThreadData);
      	}
	 else if(iTThreadData.iTaskName == _L("DummyTests"))
	 	{
	  	iPerformTest = new (ELeave)CDummyMTTests(iTThreadData);
	  	}
     else if(iTThreadData.iTaskName == _L("DummyTests1"))
	 	{
	  	iPerformTest = new (ELeave)CDummyCsy1MTTests(iTThreadData);
	  	}
     else if(iTThreadData.iTaskName == _L("DummyTests2"))
	 	{
	  	iPerformTest = new (ELeave)CDummyCsy2MTTests(iTThreadData);
        }
     else if(iTThreadData.iTaskName == _L("DummyTests3"))
	 	{
	  	iPerformTest = new (ELeave)CDummyCsy3MTTests(iTThreadData);
       	}
     else if(iTThreadData.iTaskName == _L("EcuartTests"))
	 	{
	  	iPerformTest = new (ELeave)CECUARTMTTests(iTThreadData);
        }
     else if(iTThreadData.iTaskName == _L("USBTests"))
	 	{
	  	iPerformTest = new (ELeave)CUSBMTTests(iTThreadData);
        }
     else
        {
        //leave occurs if no matching is found. 
        User::Leave(KErrAbort);	
        }   
        
     CleanupStack::PushL(iPerformTest);
     iPerformTest->InitL();
     CleanupStack::Pop(iPerformTest);
	 
	 }
void C32PerformanceThread::RunAllTests()
/**
 * @return - void
 * Implementation of RunAllTests function
 * Run all the tests
 */	
	{
	iPerformTest->PerformTask();
	}



