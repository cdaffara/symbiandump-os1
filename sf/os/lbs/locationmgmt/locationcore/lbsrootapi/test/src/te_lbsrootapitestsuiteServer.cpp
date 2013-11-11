// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file te_networkrequesthandlerTestSuiteServer.cpp
*/
#include <centralrepository.h>
#include "lbsrootcenrepdefs.h"

#include "te_lbsrootapitestsuiteServer.h"
#include "lbsrootapitest0step.h"
#include "lbsrootapitest1step.h"
#include "lbsrootapitest2step.h"
#include "lbsrootapitest3step.h"
#include "lbsrootapitest4step.h"
#include "lbsrootapitest5step.h"
#include "lbsrootapitest6step.h"
#include "lbsrootapitest7step.h"
#include "lbsrootapitest8step.h"
#include "lbsrootapitest9step.h"
//#include "lbsrootapiTest10Step.h"
//#include "lbsrootapiTest11Step.h"
//#include "lbsrootapiTest12Step.h"
//#include "lbsrootapiTest13Step.h"
#include "lbsrootapitest19step.h"
#include "lbsrootapitest20step.h"
#include "lbsrootapitest21step.h"
#include "lbsrootapitest22step.h"
#include "lbsrootapitest23step.h"
#include "lbsrootapitest24step.h"
#include "lbsrootapitest25step.h"
#include "lbsrootapitest26step.h"
#include "lbsrootapitest27step.h"
#include "lbsrootapitest28step.h"
#include "lbsrootapitest29step.h"
#include "lbsrootapitest31step.h"
#include "tprocessstartparams.h"
#include "cprocesslaunch.h"

_LIT(KServerName,"te_lbsrootapiTestSuite");
CTe_LbsRootApiTestSuite* CTe_LbsRootApiTestSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_LbsRootApiTestSuite * server = new (ELeave) CTe_LbsRootApiTestSuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	
	CleanupStack::Pop(server);
	return server;
	}



const TInt KRootApiTestSearchInt = 1234; 
_LIT16(KRootApiTestSearchDesC, "SearchDesC");
const TReal KRootApiTestSearchReal = 5.678;

void ReadCenrepIdsL(CRepository* aCenRep,
					RArray<TUint32>& aIntIds,
					RArray<TUint32>& aStringIds,
					RArray<TUint32>& aRealIds)
	{
		
		
	RArray<TUint32> neqIds;
		
	aIntIds.Reset();
	aCenRep->FindEqL(0, 0, KRootApiTestSearchInt, aIntIds);
	aCenRep->FindNeqL(0, 0, KRootApiTestSearchInt, neqIds);
	for(TInt i = 0; i < neqIds.Count(); ++i)
		{
		TInt intVal;
		TInt err = aCenRep->Get(neqIds[i], intVal);
		if(err != KErrArgument)
			{
			aIntIds.Append(neqIds[i]);			
			}
		}
	neqIds.Reset();
			
	aStringIds.Reset();
	aCenRep->FindEqL(0, 0, KRootApiTestSearchDesC, aStringIds);
	aCenRep->FindNeqL(0, 0, KRootApiTestSearchDesC, neqIds);
	for(TInt i = 0; i < neqIds.Count(); ++i)
		{
		TBuf<100> stringVal;
		TInt err = aCenRep->Get(neqIds[i], stringVal);
		if(err != KErrArgument)
			{
			aStringIds.Append(neqIds[i]);			
			}
		}
	neqIds.Reset();
   
	aRealIds.Reset();
	aCenRep->FindEqL(0, 0, KRootApiTestSearchReal, aRealIds);
	aCenRep->FindNeqL(0, 0, KRootApiTestSearchReal, neqIds);
	for(TInt i = 0; i < neqIds.Count(); ++i)
		{
		TReal realVal;
		TInt err = aCenRep->Get(neqIds[i], realVal);
		if(err != KErrArgument)
			{
			aRealIds.Append(neqIds[i]);			
			}
		}
	neqIds.Reset();
	
	}
	
void ReadCenrepIntValsL(CRepository* aCenRep,
				const RArray<TUint32>& aIntIds, 
						RArray<TInt>& aIntVals)
	{
	aIntVals.Reset();
	for(TInt i = 0; i < aIntIds.Count(); ++i)
		{
		TInt val;
		aCenRep->Get(aIntIds[i], val);
		aIntVals.Append(val);
		}
	}
	
void ReadCenrepRealValsL(CRepository* aCenRep,
				const RArray<TUint32>& aRealIds, 
						RArray<TReal>& aRealVals)
	{
	aRealVals.Reset();
	for(TInt i = 0; i < aRealIds.Count(); ++i)
		{
		TReal val;
		aCenRep->Get(aRealIds[i], val);
		aRealVals.Append(val);
		}
	}
	
void ReadCenrepStringValsL(CRepository* aCenRep,
				const RArray<TUint32>& aStringIds, 
			RArray<TLbsProcessName>& aStringVals)
	{
	aStringVals.Reset();
	for(TInt i = 0; i < aStringIds.Count(); ++i)
		{
		TBuf<100> stringVal;
		aCenRep->Get(aStringIds[i], stringVal);
		aStringVals.Append(stringVal);
		}
	}
	
// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
   
	RArray<TUint32> startIntIds;
	RArray<TUint32> startStringIds;
	RArray<TUint32> startRealIds;
		
	RArray<TInt>	startIntVals;
	RArray<TLbsProcessName>	startStringVals;
	RArray<TReal>	startRealVals;
		
	CleanupClosePushL(startIntIds);	
	CleanupClosePushL(startStringIds);	
	CleanupClosePushL(startRealIds);	
	CleanupClosePushL(startIntVals);	
	CleanupClosePushL(startStringVals);	
	CleanupClosePushL(startRealVals);	
   	
   	// Kick off the root process here, so that the startup/closedown 
   	// properties are there when required (as in the 'real' system).
   	// We can't go the whole hog and kick the Root Process off at startup, 
   	// as we need to make sure the cenrep initialisation will tell
   	// the system to start itself in transient mode.
   	// Do the mods to the cenrep file first.
   
	// First reset the cenrep file, just in case anyone 
	// else has been messing with it.
	CRepository* rep = CRepository::NewL(KLbsCenRepUid);
	CleanupStack::PushL(rep);
	rep->Reset();
	CleanupStack::PopAndDestroy(rep);

   	rep = CRepository::NewL(KLbsCenRepUid);
   	CleanupStack::PushL(rep);
   	
	ReadCenrepIdsL(rep, startIntIds, startStringIds, startRealIds);
	ReadCenrepIntValsL(rep, startIntIds, startIntVals);
	ReadCenrepStringValsL(rep, startStringIds, startStringVals);
	ReadCenrepRealValsL(rep, startRealIds, startRealVals);	
		
   	// clean out any existing entries
   	User::LeaveIfError(rep->Delete(KLbsSystemTransientKey));
   	User::LeaveIfError(rep->Set(KLbsSystemTransientKey, ETrue));
   	User::LeaveIfError(rep->Delete(KLbsRootProcessTransientKey));
   	User::LeaveIfError(rep->Set(KLbsRootProcessTransientKey, EFalse));
   		
	CleanupStack::PopAndDestroy(rep);

   	// Leave the hooks in for platform security
   #if (defined __DATA_CAGING__)
   	RProcess().DataCaging(RProcess::EDataCagingOn);
   	RProcess().DataCaging(RProcess::ESecureApiOn);
   #endif

	User::SetJustInTime(EFalse);
   	CActiveScheduler* sched=NULL;
   	sched=new(ELeave) CActiveScheduler;
   	CActiveScheduler::Install(sched);
   	CTe_LbsRootApiTestSuite* server = NULL;
   	// Create the CTestServer derived server
   	TRAPD(err,server = CTe_LbsRootApiTestSuite::NewL());
   	if(!err)
   		{
   		// Sync with the client and enter the active scheduler
   		RProcess::Rendezvous(KErrNone);
   		sched->Start();
   		}
	User::SetJustInTime(ETrue);
	
	rep = CRepository::NewL(KLbsCenRepUid);
	CleanupStack::PushL(rep);
	rep->Reset();
	CleanupStack::PopAndDestroy(rep);
	
	
	rep = CRepository::NewL(KLbsCenRepUid);
	CleanupStack::PushL(rep);
	
	RArray<TUint32> endIntIds;
	RArray<TUint32> endStringIds;
	RArray<TUint32> endRealIds;
		
	RArray<TInt>	endIntVals;
	RArray<TLbsProcessName>	endStringVals;
	RArray<TReal>	endRealVals;
	
	CleanupClosePushL(endIntIds);	
	CleanupClosePushL(endStringIds);	
	CleanupClosePushL(endRealIds);	
	CleanupClosePushL(endIntVals);	
	CleanupClosePushL(endStringVals);	
	CleanupClosePushL(endRealVals);	
	
	ReadCenrepIdsL(rep, endIntIds, endStringIds, endRealIds);
	ReadCenrepIntValsL(rep, endIntIds, endIntVals);
	ReadCenrepStringValsL(rep, endStringIds, endStringVals);
	ReadCenrepRealValsL(rep, endRealIds, endRealVals);
	
	// For each type, check the number is th same, and then that the values 
	// are the same as before the test
	TInt i;
	TInt count = startIntIds.Count();
	if(endIntIds.Count() != count)
		{
		User::Invariant();
		}
	// Check the Ids (keys) are the same
	for(i = 0; i < count; ++i)
		{
		TInt key1 = endIntIds[i];
		TInt key2 = startIntIds[i];
		if(startIntIds[i] != endIntIds[i])
			{
			User::Invariant();
			}
		}
	// Check the values are the same
	for(i = 0; i < count; ++i)
		{
		TInt val1 = endIntVals[i];
		TInt val2 = startIntVals[i];
		if(startIntVals[i] != endIntVals[i])
			{
			User::Invariant();
			}
		}
	
	count = startRealIds.Count();
	if(endRealIds.Count() != count)
		{
		User::Invariant();
		}
	for(i = 0; i < count; ++i)
		{
		TReal val1 = endRealVals[i];
		TReal val2 = startRealVals[i];
		if(startRealVals[i] != endRealVals[i])
			{
			User::Invariant();
			}
		}
	
	count = startStringIds.Count();
	if(endStringIds.Count() != count)
		{
		User::Invariant();
		}
	for(i = 0; i < count; ++i)
		{
		TBuf<100> str1;
		TBuf<100> str2;
		str1.Copy(endStringVals[i]);
		str2.Copy(startStringVals[i]);
		if(str1.Compare(str2) != 0)
			{
			User::Invariant();
			}
		}
	
	CleanupStack::PopAndDestroy(&endRealVals);
	CleanupStack::PopAndDestroy(&endStringVals);
	CleanupStack::PopAndDestroy(&endIntVals);
	CleanupStack::PopAndDestroy(&endRealIds);
	CleanupStack::PopAndDestroy(&endStringIds);
	CleanupStack::PopAndDestroy(&endIntIds);
	
	CleanupStack::PopAndDestroy(rep);
	
	CleanupStack::PopAndDestroy(&startRealVals);
	CleanupStack::PopAndDestroy(&startStringVals);
	CleanupStack::PopAndDestroy(&startIntVals);
	CleanupStack::PopAndDestroy(&startRealIds);
	CleanupStack::PopAndDestroy(&startStringIds);
	CleanupStack::PopAndDestroy(&startIntIds);		
	
   	delete server;
   	delete sched;
   	}
   
   
GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CTe_LbsRootApiTestSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
    if(aStepName == KLbsRootApiTest0Step)
        return new CLbsRootApiTest0Step();
    if(aStepName == KLbsRootApiTest1Step)
        return new CLbsRootApiTest1Step();
    if(aStepName == KLbsRootApiTest2Step)
        return new CLbsRootApiTest2Step();
    if(aStepName == KLbsRootApiTest3Step)
        return new CLbsRootApiTest3Step();
    if(aStepName == KLbsRootApiTest4Step)
        return new CLbsRootApiTest4Step();
    if(aStepName == KLbsRootApiTest5Step)
        return new CLbsRootApiTest5Step();
    if(aStepName == KLbsRootApiTest6Step)
        return new CLbsRootApiTest6Step();
    if(aStepName == KLbsRootApiTest7Step)
        return new CLbsRootApiTest7Step();
    if(aStepName == KLbsRootApiTest8Step)
        return new CLbsRootApiTest8Step();
    if(aStepName == KLbsRootApiTest9Step)
        return new CLbsRootApiTest9Step();
//    if(aStepName == KLbsRootApiTest10Step)
//        return new CLbsRootApiTest10Step();
//    if(aStepName == KLbsRootApiTest11Step)
//        return new CLbsRootApiTest11Step();
//   if(aStepName == KLbsRootApiTest12Step)
//       return new CLbsRootApiTest12Step();
//   if(aStepName == KLbsRootApiTest13Step)
//        return new CLbsRootApiTest13Step();
	if(aStepName == KLbsRootApiTest19Step)
		return new CLbsRootApiTest19Step();
	if(aStepName == KLbsRootApiTest20Step)
		return new CLbsRootApiTest20Step();
	if(aStepName == KLbsRootApiTest21Step)
		return new CLbsRootApiTest21Step();
	if(aStepName == KLbsRootApiTest22Step)
		return new CLbsRootApiTest22Step();
	if(aStepName == KLbsRootApiTest23Step)
		return new CLbsRootApiTest23Step();
	if(aStepName == KLbsRootApiTest24Step)
		return new CLbsRootApiTest24Step();
	if(aStepName == KLbsRootApiTest25Step)
		return new CLbsRootApiTest25Step();
	if(aStepName == KLbsRootApiTest26Step)
		return new CLbsRootApiTest26Step();
	if(aStepName == KLbsRootApiTest27Step)
		return new CLbsRootApiTest27Step();
	if(aStepName == KLbsRootApiTest28Step)
		return new CLbsRootApiTest28Step();
	if(aStepName == KLbsRootApiTest29Step)
		return new CLbsRootApiTest29Step();
	if(aStepName == KLbsRootApiTest31Step)
		return new CLbsRootApiTest31Step();
    
	return NULL;
	}
