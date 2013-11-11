// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#include <e32uid.h>
#include "te_activitymanager.h"

/*@{*/
//	Commands
_LIT(KCmdDestructor,					 "~");
_LIT(KCmdNewL,							 "NewL");
_LIT(KCmdStart,							 "Start");
_LIT(KCmdSetInactivityTimeout,			 "SetInactivityTimeout");
_LIT(KCmdUserActivity,					 "UserActivity");
_LIT(KCmdSetExpectedEventUserInActivity, "SetExpectedEventUserInActivity");
_LIT(KCmdSetExpectedEventUserActivity,	 "SetExpectedEventUserActivity");
_LIT(KCmdCheckExpectedEvents,	 		 "CheckExpectedEvents");
_LIT(KCmdOOMTest,	 		 	 		 "OOMTest");
_LIT(KCmdCallbacksOff, 		 	 		 "CallBacksOff");
_LIT(KCmdSetSlowCallbacks,		 	 	 "SetSlowCallbacks");
_LIT(KCancel,	 			 	 		 "Cancel");
_LIT(KCmdDoCmdDummyMethod,	 	 	 	 "DummyMethod");
_LIT(KCmdUDEB_Only,			 	 		 "UDEBOnly");
_LIT(KCmdUREL_Only,			 	 		 "URELOnly");
_LIT(KExpectedPanic,			 		 "ExpectedPanic");
_LIT(KCmdDoCmdInjectActivity,	 		 "InjectActivity");

//	Parameters
_LIT(KPriority,							"priority");
_LIT(KInterval,							"interval");
_LIT(KInactiveEvents,					"inactiveEvents");
_LIT(KActiveEvents,						"activeEvents");
_LIT(KInt, 								"int");
_LIT(KString, 							"string");

// CActive Mapping
_LIT(KCActivePriorityIdle,		"EPriorityIdle");
_LIT(KCActivePriorityLow,		"EPriorityLow");
_LIT(KCActivePriorityStandard,	"EPriorityStandard");
_LIT(KCActivePriorityUserInput,	"EPriorityUserInput");
_LIT(KCActivePriorityHigh,		"EPriorityHigh");

/*@}*/

const TInt64 KMsInOneSecond = 1000000;
const TInt   KActivityManagerDllUid3 = 0x100056e4;

//
// Construction/Destruction
//

/**
 * Two phase constructor
 */
CTeActivityManager* CTeActivityManager::NewL()
	{
	CTeActivityManager*	ret=new (ELeave) CTeActivityManager();
	return ret;
	}

/**
 * Constructor. First phase construction
 */
CTeActivityManager::CTeActivityManager(): 
	iCallbackMethods(ETrue), iSlowCallbacks(EFalse), iDoCommands(ETrue)
	{
	}

/**
 * Destructor
 */
CTeActivityManager::~CTeActivityManager()
	{
	DestroyData();
	}

void CTeActivityManager::DestroyData()
	{
	delete iUserActivityManager;
	iUserActivityManager=NULL;
	}
	
TAny* CTeActivityManager::GetObject() 
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	Pointer to the object that the data wraps
 */
	{
	return &iUserActivityManager;
	};

	
void CTeActivityManager::UDEB_Only()
	{
#ifdef _DEBUG
	iDoCommands=ETrue;
#else
	iDoCommands=EFalse;
#endif
	}

void CTeActivityManager::UREL_Only()
	{
#ifdef _DEBUG
	iDoCommands=EFalse;
#else
	iDoCommands=ETrue;
#endif
	}

void CTeActivityManager::ExpectedPanic(const TTEFSectionName& aSection)
	{
	if (!iDoCommands)
		{
	  	TPtrC category;
		TInt reason;
		GetStringFromConfig(aSection, KString(), category);
		GetIntFromConfig(aSection, KInt(), reason);
		User::Panic(category,reason);
		}
	}
	
TBool CTeActivityManager::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
	{
	TBool retVal=ETrue;

	if (aCommand == KCmdUDEB_Only)
		{
		UDEB_Only();
		}
	else if (aCommand == KCmdUREL_Only)
		{
		UREL_Only();
		}
	else if (aCommand == KExpectedPanic)
		{
		ExpectedPanic(aSection);
		}
	else if (!iDoCommands) 
		{
		//Don't process other commands
		return retVal;	
		}
	else if (aCommand == KCmdDestructor)
		{
		DoCmdDestructor();
		}
	else if (aCommand == KCmdNewL)
		{
		DoCmdNew(aSection);
		}
	else if (aCommand == KCmdStart)
		{
		DoCmdStart(aSection);
		}
	else if (aCommand == KCmdSetInactivityTimeout)
		{
		DoCmdSetInactivityTimeout(aSection);
		}		
	else if (aCommand == KCmdUserActivity)
		{
		DoCmdUserActivity();
		}	
	else if (aCommand == KCmdSetExpectedEventUserInActivity)
		{
		DoCmdSetExpectedResultInactivity(aSection);
		}		
	else if (aCommand == KCmdSetExpectedEventUserActivity)
		{
		DoCmdSetExpectedResultActivity(aSection);
		}	
	else if (aCommand == KCmdCheckExpectedEvents)
		{
		DoCmdCheckExpectedEvents(aSection);
		}	
	else if (aCommand == KCmdOOMTest)
		{
		OomTest();
		}	
	else if (aCommand == KCmdCallbacksOff)
		{
		DoCmdSetCallbackOff();
		}
	else if (aCommand == KCmdSetSlowCallbacks)
		{
		DoCmdSetSlowCallbacks();
		}
	else if (aCommand == KCancel)
		{
		DoCmdCancel();
		}	
	else if (aCommand == KCmdDoCmdDummyMethod)
		{
		DoCmdDummyMethod();
		}
	else if (aCommand == KCmdDoCmdInjectActivity)
		{
		DoCmdActivityInjectTest(aSection);
		}
	else
		{
		retVal=EFalse;
		}

	return retVal;
	}

void CTeActivityManager::DoCmdNew(const TTEFSectionName& aSection)
	{
	// Read the CActive::Priority from the config file
 	CActive::TPriority priority = CActive::EPriorityStandard;
  	TPtrC readPriority;
	if ( GetStringFromConfig(aSection, KPriority(), readPriority) )
		{
		if ( !MapAndReturnCActivePriority(priority ,readPriority ) )
			{	
			SetError(KErrGeneral);
			}
		}
		
	// Create the class object under test			
	TRAPD(err, iUserActivityManager = CUserActivityManager::NewL(priority));

	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CUserActivityManager::NewL failed with error %d"), err);
		SetError(err);
		}

	// Ensure that inactivity timers are at zero. This also simulates user activity,
  	User::ResetInactivityTime();
    // Set the start time, for test reporting purposes.
	iStartTime.HomeTime();
	}
	
void CTeActivityManager::DoCmdDestructor()
	{
	DestroyData();
	}	

void CTeActivityManager::DoCmdSetCallbackOff()
	{
	iCallbackMethods = EFalse;
	}

void CTeActivityManager::DoCmdSetSlowCallbacks()
	{
	iSlowCallbacks = ETrue;
	}

void CTeActivityManager::DoCmdCancel()
	{
	iUserActivityManager->Cancel();
	}

void CTeActivityManager::DoCmdStart(const TTEFSectionName& aSection)
	{
	// Get the expected interval period from the config file.
   	TInt interval;
  	GetIntFromConfig(aSection, KInterval(), interval);
  	  	
  	// If callbacks are in use
  	if (iCallbackMethods)
  		{
  		iUserActivityManager->Start(interval,
 	   	       			        	TCallBack(HandleInactiveEvent,this),
    		       			    	TCallBack(HandleActiveEvent,this));	
  		}
  	else
	  	{
	  	iUserActivityManager->Start(interval,
 	   	      			        	TCallBack(NULL,this),
    		       			    	TCallBack(NULL,this));	
	  	}
	
	// Set an outstanding event. This corresponds with the OUTSTANDING command in the script
    IncOutstanding();		       			    
	}

void CTeActivityManager::DoCmdSetInactivityTimeout(const TTEFSectionName& aSection)
	{
	// Get the expected interval period from the config file.
   	TInt interval;
  	GetIntFromConfig(aSection, KInterval(), interval);
   	iUserActivityManager->SetInactivityTimeout(interval);
	}
		
void CTeActivityManager::DoCmdUserActivity()
	{
  	// Ensure that inactivity timers are at zero. This also simulates user activity,
   	User::ResetInactivityTime();
   	}		
		
void CTeActivityManager::DoCmdSetExpectedResultInactivity(const TTEFSectionName& aSection)
	{
	// Get the expected interval period from the config file.
   	TInt interval;
  	GetIntFromConfig(aSection, KInterval(), interval);
  	
  	__ASSERT_ALWAYS(interval > 0,User::Panic(_L("Interval not valid in DoCmdSetExpectedResultInactivity"),KErrArgument));
   	
   	iExpectedIntervalMicroSecondsInactivity = interval * KMsInOneSecond;
   	}	

void CTeActivityManager::DoCmdSetExpectedResultActivity(const TTEFSectionName& aSection)
	{
	// Get the expected interval period from the config file.
   	TInt interval;
  	GetIntFromConfig(aSection, KInterval(), interval);
  	
  	__ASSERT_ALWAYS(interval > 0,User::Panic(_L("Interval not valid in DoCmdSetExpectedResultActivity"),KErrArgument));
   	
   	iExpectedIntervalMicroSecondsActivity = interval * KMsInOneSecond;
   	}	
   	
void CTeActivityManager::DoCmdCheckExpectedEvents(const TTEFSectionName& aSection)
	{
	TInt inactiveEventsExpected;
	TInt activeEventsExpected;
	// Get the number of expected inactive and active events
	GetIntFromConfig(aSection, KInactiveEvents(), inactiveEventsExpected);
  	GetIntFromConfig(aSection, KActiveEvents(), activeEventsExpected);
  	
  	__ASSERT_ALWAYS(inactiveEventsExpected >= 0,User::Panic(_L("inactiveEventsExpected not valid in DoCmdCheckExpectedEvents"),KErrArgument));
  	__ASSERT_ALWAYS(activeEventsExpected >= 0,User::Panic(_L("activeEventsExpected not valid in DoCmdCheckExpectedEvents"),KErrArgument));

	// Check that the number of inactive events matches expectations.
	if (inactiveEventsExpected != iInactiveEvents)
		{
		INFO_PRINTF3(_L("Inactive events mismatch. Expected: %d, Actual: %d"),inactiveEventsExpected,iInactiveEvents);
		SetError(KErrGeneral);
		}
		
	// Check that the number of active events matches expectations.
	if (activeEventsExpected != iActiveEvents)
		{
		INFO_PRINTF3(_L("Active events mismatch. Expected: %d, Actual: %d"),activeEventsExpected,iActiveEvents);
		SetError(KErrGeneral);
		}
   	}	   	
		
void CTeActivityManager::ProcessTestResult(TActivityType event,TTime aCurrentTime)
	{
	// Get the microseconds that have elapsed from start time to the current time.
	TTimeIntervalMicroSeconds microSecondsInterval = aCurrentTime.MicroSecondsFrom(iStartTime);
	
	TTimeIntervalMicroSeconds expectedIntervalMicroSeconds;
	if (event == EEventInactivity)
		{
		expectedIntervalMicroSeconds = iExpectedIntervalMicroSecondsInactivity;
		}
	else
		{
		expectedIntervalMicroSeconds = iExpectedIntervalMicroSecondsActivity;
		}
		
	// Specify a range of time that the active/inactive event may fall into. Assume +- 10% of expected is valid.
	TTimeIntervalMicroSeconds lowestAcceptableInterval(expectedIntervalMicroSeconds.Int64() * 0.9);	
	TTimeIntervalMicroSeconds highestAcceptableInterval(expectedIntervalMicroSeconds.Int64() * 1.1);
	
	// Track event count
	if (event == EEventInactivity)
		{
		INFO_PRINTF1(_L("CTeActivityManager::HandleInactiveEvent"));
		iInactiveEvents++;
		}
	else
		{
	 	INFO_PRINTF1(_L("CTeActivityManager::HandleActiveEvent"));
	 	iActiveEvents++;
		}

	// Check if expected result is in a range expected.	
	if (microSecondsInterval > lowestAcceptableInterval && microSecondsInterval < highestAcceptableInterval)
		{
		// Range is acceptable
		INFO_PRINTF3(_L("Range is acceptable. Expected: %Ld, Actual: %Ld"),expectedIntervalMicroSeconds.Int64(),microSecondsInterval.Int64());
		}
	else
		{
		INFO_PRINTF3(_L("Range is not acceptable. Expected: %Ld, Actual: %Ld"),expectedIntervalMicroSeconds.Int64(),microSecondsInterval.Int64());
		SetError(KErrGeneral);
		}
	}		
		    
// Slow handlers
void CTeActivityManager::SlowCallbacks(TBool aSlowCallbacks)
	{
		if (aSlowCallbacks)
			{
			RTimer timer;
			timer.CreateLocal();
			TRequestStatus status;
			timer.After(status,4000000);
			User::WaitForRequest(status);
			timer.Close();
			}
	}

// Handles inactive event
TInt CTeActivityManager::HandleInactiveEvent(TAny* aPtr)
    {
    // Get the current time immediately when the event occurs
	TTime currentTime;
	currentTime.HomeTime();
	
    // Code to be executed in case of user inactivity
	CTeActivityManager* activityManager = static_cast<CTeActivityManager*>(aPtr);
	
	activityManager->ProcessTestResult(EEventInactivity,currentTime);
	
	// Decrement the outstanding event
	activityManager->DecOutstanding();
	SlowCallbacks(activityManager->iSlowCallbacks);
	return KErrNone;
    }

// Handles active event
TInt CTeActivityManager::HandleActiveEvent(TAny* aPtr)
    {
    // Get the current time immediately when the event occurs
	TTime currentTime;
	currentTime.HomeTime();
	
    // Code to be executed in case of user activity after an inactivity period.
	CTeActivityManager* activityManager = static_cast<CTeActivityManager*>(aPtr);
	
	activityManager->ProcessTestResult(EEventActivity,currentTime);
	SlowCallbacks(activityManager->iSlowCallbacks);
	return KErrNone;
    }

/**
MapToState - Map the priority from config file to CActive::TPriority
 @param aPriority return value, mapped to CActive::TPriority
 @param aReadPriority parameter read from config file
*/
TBool CTeActivityManager::MapAndReturnCActivePriority(CActive::TPriority& aPriority, TDesC& aReadPriority)
	{
	TBool	ret = ETrue;
	if( !(aReadPriority.Compare(KCActivePriorityIdle)) )
	 	{
		aPriority = CActive::EPriorityIdle;
		}
	else if( !( aReadPriority.Compare(KCActivePriorityLow)) )
	    {
	    aPriority = CActive::EPriorityLow;
	    }
	else if( !(aReadPriority.Compare(KCActivePriorityStandard)) )
	    {
		aPriority = CActive::EPriorityStandard;
	    }
	else if( !( aReadPriority.Compare(KCActivePriorityUserInput)) )
	    {
	    aPriority = CActive::EPriorityUserInput;
	    }
	else if( !( aReadPriority.Compare(KCActivePriorityHigh)) )
	    {
	    aPriority = CActive::EPriorityHigh;
	    }	    
	else
		{
		ret = EFalse;
		}

	return ret;
	}
	
void CTeActivityManager::OomTest()
	{	
	TInt error;
	TInt count = 0;

	do
		{
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::EFailNext, ++count);
		User::__DbgMarkStart(RHeap::EUser);
		
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);
		
		TRAP(error, OomTestImpL());
		
		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
		
		__ASSERT_ALWAYS(endProcessHandleCount == startProcessHandleCount,User::Panic(_L("Process Handle count mismatch"),KErrGeneral));
		__ASSERT_ALWAYS(endThreadHandleCount == startThreadHandleCount,User::Panic(_L("Thread Handle count mismatch"),KErrGeneral));
		
		User::__DbgMarkEnd(RHeap::EUser, 0);
		} while(error == KErrNoMemory);

	__ASSERT_ALWAYS(error==KErrNone,User::Panic(_L("OOM Test failed"),error));

	User::__DbgSetAllocFail(RHeap::EUser, RHeap::ENone, 1);
	}	
	
// Handles inactive event
TInt CTeActivityManager::ErrorTestingHandleInactiveEvent(TAny* /*aPtr*/)
    {
    CActiveScheduler::Stop();
	return KErrNone;
    }

// Handles active event
TInt CTeActivityManager::ErrorTestingHandleActiveEvent(TAny* /*aPtr*/)
    {
    CActiveScheduler::Stop();
	return KErrNone;
    }    
    
void CTeActivityManager::OomTestImpL()	
	{
	// Create the class object under test			
	CUserActivityManager *userActivityManager = CUserActivityManager::NewL(CActive::EPriorityStandard);
	CleanupStack::PushL(userActivityManager);

	// Ensure that inactivity timers are at zero. This also simulates user activity,
	User::ResetInactivityTime();
  	  	
  	userActivityManager->Start(2,
 	   	       			       TCallBack(ErrorTestingHandleInactiveEvent,this),
    		       			   TCallBack(ErrorTestingHandleActiveEvent,this));
    
    CActiveScheduler::Start();
      		       			   
  	// Set inactivity timeout while waiting for user activity
    userActivityManager->SetInactivityTimeout(3);
    
    // Cause a user activity event.
	User::ResetInactivityTime();
	
	CActiveScheduler::Start();
	
 	// Set inactivity timeout while waiting for user inactivity
    userActivityManager->SetInactivityTimeout(5);
    
    CActiveScheduler::Start();
    
    CleanupStack::PopAndDestroy(userActivityManager);	       			   
	}

void CTeActivityManager::DoCmdDummyMethod()
	{
	TUidType uidType(KDynamicLibraryUid,KSharedLibraryUid,TUid::Uid(KActivityManagerDllUid3));
	
	RLibrary library;
	TInt err = library.Load(_L("activitymanager.dll"),uidType);
	
	__ASSERT_ALWAYS(err==KErrNone,User::Panic(_L("Failed to load activitymanaer.dll, error: %d."),err));

	// Get the ordinal for a dummy function
#ifdef __WINSCW__
	// Dummy1 is at ordinal 1 for winscw	
	TLibraryFunction ordinalfunction = library.Lookup(1); 
#else
	// Dummy1 is at ordinal 3 for eabi
	TLibraryFunction ordinalfunction = library.Lookup(3); 
#endif	
	
	__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in activitymanager.dll."),KErrGeneral));
	
	// Call dummy method
	(*ordinalfunction)();
	}

TInt CTeActivityManager::ActivityInjectTestL(TAny* aAny)
	{
	__UHEAP_MARK;
	
	CTeActivityManager *teActivityManager = reinterpret_cast<CTeActivityManager*>(aAny);
	User::After((teActivityManager->iInterval) * KMsInOneSecond);
	
	// Simulate the user activity here.
	User::ResetInactivityTime();
	RDebug::Print(_L("Activity inject complete"));
	
	__UHEAP_MARKEND;
	// Terminate the thread
	User::Exit(KErrNone);
	return KErrNone;
	}

void CTeActivityManager::DoCmdActivityInjectTest(const TTEFSectionName& aSection)
	{
	// Get the expected interval period from the config file.
   	GetIntFromConfig(aSection, KInterval(), iInterval);
	
   	// Start a new thread to allow the insertion of user activity at the desired time. A new
   	// thread is required since the script processing and the test code occur in the same
   	// thread, which does not allow insertion of the activity event in the script at the desired
   	// time. But, since threads are time sliced, starting another thread allows
   	// the new thread to insert the activity event at the appropriate time, even during script/
   	// test code execution.
	INFO_PRINTF1(_L("Starting thread for  activity inject test..."));
	RThread thread;
	TInt err=thread.Create(_L("Activity inject test "),ActivityInjectTestL,KDefaultStackSize*8,KMinHeapSize,0x100000,this);
	__ASSERT_ALWAYS(err==KErrNone,User::Panic(_L("Thread create failure."),KErrGeneral));
	thread.Resume();
	thread.Close();
	}
