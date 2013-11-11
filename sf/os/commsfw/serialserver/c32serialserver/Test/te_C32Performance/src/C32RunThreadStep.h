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
#if (!defined C32RUNTHREAD_STEP_H)
#define C32RUNTHREAD_STEP_H

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <testexecutestepbase.h>
#else
#include <test/testexecutestepbase.h>
#endif

#include "Te_C32PerformanceSuiteStepBase.h"

#include "CPerformTasks.h"

class C32PerformanceThread : public CBase
/*
This class is to used to instantiate multithreaded  tests.
*/
{
public:
	C32PerformanceThread( TThreadData& aTThreadData ) :
		iTThreadData( aTThreadData )
		{
		}
	virtual ~C32PerformanceThread()
		{
		delete iPerformTest;
     	}

   static TInt RunL(TAny* aNetPerformanceThread);
   	
   static TInt iCountSemaphores; //to get the semaphore count

private:
   
   void InitTestDataL();
   void RunAllTests();
   
   CPerformTestTasks* iPerformTest;
   TThreadData& iTThreadData;

};

class CC32RunThreadStep : public CTe_C32PerformanceSuiteStepBase
/*
This function is used to parse ini file to populate thread details and spawn client threads
*/
	{
public:
	CC32RunThreadStep();
	~CC32RunThreadStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	
	TUint iTaskCounter;

protected:
	//validate ini file format 
	TInt ProcessNextDescriptionL();
	//spawns each client thread
	TBool ThreadCreationL();
	//reads NumofThreads field from ini file
	TInt ReadIniTotalSemaphoreCount();
	//sets thread priority
	TThreadPriority ParseThreadPriority(TPtrC athreadPriority);
	
private:
	TThreadData* iThreadData[KMaxTestClientThreads]; 
	TRequestStatus iRequestStatus[KMaxTestClientThreads];
	C32PerformanceThread* iNetPerformanceThread[KMaxTestClientThreads];
	
	};

_LIT(KC32RunThreadStep,"CC32RunThreadStep");

#endif

