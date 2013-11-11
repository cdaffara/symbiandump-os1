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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <e32base.h>
#include <e32debug.h>
#include <startupdomaindefs.h>
#include <f32file.h>
#include <domainmanager.h>
#include <s32file.h>
#include <startup.hrh>
#include "appfwk_sysstart_test_stfp.h"

_LIT(KStateTransFailurePolicyPanic, "CStateTransFailurePolicy");
	
CStateTransFailurePolicy* CStateTransFailurePolicy::NewL(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId)
	{
	RDebug::Print(_L("CStateTransFailurePolicy::NewL - entry"));
	CStateTransFailurePolicy* self = NewLC(aHierarchyId, aDomainId);
	CleanupStack::Pop(self);
	RDebug::Print(_L("CStateTransFailurePolicy::NewL - exit"));
	return self;
	}
		
CStateTransFailurePolicy* CStateTransFailurePolicy::NewLC(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId)
	{
	RDebug::Print(_L("CStateTransFailurePolicy::NewLC - entry"));
	CStateTransFailurePolicy* self = new (ELeave) CStateTransFailurePolicy(aHierarchyId, aDomainId);
	CleanupStack::PushL(self);
	self->ConstructL();
	RDebug::Print(_L("CStateTransFailurePolicy::NewLC - exit"));
	return self;	
	}
	
void CStateTransFailurePolicy::Start()
	{
	RDebug::Print(_L("CStateTransFailurePolicy::Start - entry"));
		
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KStateTransFailurePolicyPanic,1));
			
	TRAPD(err,PerformActionL());	
	if (err)
		{
		User::Panic(KStateTransFailurePolicyPanic,err);
		}
		
	RDebug::Print(_L("CStateTransFailurePolicy::Start - exit"));
	}
		
CStateTransFailurePolicy::CStateTransFailurePolicy(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId) :
	CDmDomain(aHierarchyId, aDomainId)
	{
	}
	
void CStateTransFailurePolicy::RunL()
	{
	RDebug::Print(_L("CStateTransFailurePolicy::RunL - entry"));
			
	PerformActionL();
	
	RDebug::Print(_L("CStateTransFailurePolicy::RunL - exit"));
	}

/**
 This function performs the following actions when called as part
 of the state transition:
 
 - connect to file server
 - acknowledge state transition
 - read in the result file (for debug purposes)
 - disconnect from the file server
 - request notification of further transitions
 */
void CStateTransFailurePolicy::PerformActionL()
	{
	RDebug::Print(_L("CStateTransFailurePolicy::PerformActionL - entry"));
	
	// get the current state
	iState = GetState();
	User::LeaveIfError(iFs.Connect());
		
	// register a request for further transition notifications
	RDebug::Print(_L("CStateTransFailurePolicy: Request Transition Notification - current state: %d"), iState);
	RequestTransitionNotification();
	
	if(IsTestableState())
		{
		// acknowledge the state transition
		AckStateL();
		
		// for debug only - print out results to epocwind
		ReadResultFileL();
		}
	else
		{
		AcknowledgeLastState(KErrNone);
		}
		
	iFs.Close();
	
	RDebug::Print(_L("CStateTransFailurePolicy::PerformActionL - exit"));
	}

/**
  Checks that the state is testable. This caters for the situation where 
  new states have been added to the startup resource file.
  */
TBool CStateTransFailurePolicy::IsTestableState()
	{
	TBool rc = EFalse;
	
	switch (iState)
		{
		case EStartupStateCriticalStatic:
		case EStartupStateCriticalDynamic: 
		case EStartupStateNonCritical: 
			rc = ETrue;
			break;
		default:
			RDebug::Print(_L("CStateTransFailurePolicy::IsTestableState - State not tested - %d"), iState);
			break;
		}
	
	return rc;
	}
	
/**
  Acknowledges the state transition. Dependant on the state of the system,
  the acknowledgement is either success (KErrNone) or failure (KErrNotFound).
  
  The system state is defined by the setup script, which is run prior to the
  system booting.
  */
void CStateTransFailurePolicy::AckStateL()
	{
	TFileName myAckFileName = _L("c:\\myAckFileNameUndefined.txt");
	
	switch (iState)
		{
		case EStartupStateCriticalStatic:
			RDebug::Print(_L("CStateTransFailurePolicy: current state = Critical Static"));	
			iCSCount++;
		 	myAckFileName = _L("z:\\sysstarttest\\TAckCriticalStatic.txt");
			break;
		case EStartupStateCriticalDynamic: 
			RDebug::Print(_L("CStateTransFailurePolicy: current state = Critical Dymamic"));
			iCDCount++;
		 	myAckFileName = _L("z:\\sysstarttest\\TAckCriticalDynamic.txt");
			break;
		case EStartupStateNonCritical: 
			RDebug::Print(_L("CStateTransFailurePolicy: current state = Non Critical"));
			iNCCount++;
		 	myAckFileName = _L("z:\\sysstarttest\\TAckNonCritical.txt");
			break;
		default:
			User::Panic(KStateTransFailurePolicyPanic,iState);
			break;
		}
	
	// If file is not present, Open() fails with KErrNotFound (-1)
	// which is then used as the error code to fail the state transition
	// i.e. this is the mechanism to control which transition is failed
	// e.g. TAckCriticalDynamic.txt does not exist, then the critical static to
	//      critical dynamic transition is failed
	RFile myAckFile;
	TInt ackCode = myAckFile.Open(iFs,myAckFileName,EFileRead);
	
	if (ackCode)
		{
		switch (iState)
			{
			case EStartupStateCriticalStatic: 
				iCSFailCount++;
				break;
			case EStartupStateCriticalDynamic: 
				iCDFailCount++;
				break;
			case EStartupStateNonCritical: 
				iNCFailCount++;
				break;
			default:
				User::Panic(KStateTransFailurePolicyPanic,iState);
				break;
			}
		}
	
	// Write results at this point to ensure that an indication of the state
	// being reached is in the results file.
	WriteResultFileL();
	
	RDebug::Print(_L("CStateTransFailurePolicy: acknowledgement sent: %d"), ackCode);
	myAckFile.Close();
	AcknowledgeLastState(ackCode);
	}

/**
  Write the results to the result file, to be later extracted by
  test execute test case.
  
  The format is:
  Critical Static count=value1
  Critical Static Fail count=value2
  Critical Dynamic count=value3
  Critical Dynamic Fail count=value4
  Non Critical count=value5
  Non Critical Fail count=value6
  '=' is used as the deliminator.
  */
void CStateTransFailurePolicy::WriteResultFileL()
	{
	RFileWriteStream writer;
	writer.PushL(); // writer on cleanup stack
	
	User::LeaveIfError(writer.Replace(iFs, KStfpResultFile, EFileWrite));
	
	// write data to file
	writer << _L("Critical Static count=");
	writer.WriteUint8L(iCSCount);
	writer << _L("\n");
	writer << _L("Critical Static Fail count=");
	writer.WriteUint8L(iCSFailCount);	
	writer << _L("\n");
	writer << _L("Critical Dynamic count=");
	writer.WriteUint8L(iCDCount);
	writer << _L("\n");
	writer << _L("Critical Dynamic Fail count=");
	writer.WriteUint8L(iCDFailCount);
	writer << _L("\n");
	writer << _L("Non Critical count=");
	writer.WriteUint8L(iNCCount);
	writer << _L("\n");	
	writer << _L("Non Critical Fail count=");
	writer.WriteUint8L(iNCFailCount);
	writer << _L("\n");
		
	writer.CommitL();
	CleanupStack::PopAndDestroy(&writer);
	}


/**
  Read the results from the result file.
  
  The expected format is:
  Critical Static count=value1
  Critical Static Fail count=value2
  Critical Dynamic count=value3
  Critical Dynamic Fail count=value4
  Non Critical count=value5
  Non Critical Fail count=value6
  '=' is used as the deliminator.
  */
void CStateTransFailurePolicy::ReadResultFileL()
	{
	RFileReadStream reader;
		
	reader.PushL();
	
	User::LeaveIfError(reader.Open(iFs,KStfpResultFile,EFileWrite));
		
	TChar delimChar('=');
	TBuf8<255> aDes;
	reader.ReadL(aDes, delimChar);
	TInt csCount = reader.ReadInt8L();
	RDebug::Print(_L("CStateTransFailurePolicy::ReadResultFileL - csCount - %d"), csCount);
	
	reader.ReadL(aDes, delimChar);
	TInt csFailCount = reader.ReadInt8L();
	RDebug::Print(_L("CStateTransFailurePolicy::ReadResultFileL - csFailCount - %d"), csFailCount);
	
	reader.ReadL(aDes, delimChar);
	TInt cdCount = reader.ReadInt8L();
	RDebug::Print(_L("CStateTransFailurePolicy::ReadResultFileL - cdCount - %d"), cdCount);
	
	reader.ReadL(aDes, delimChar);
	TInt cdFailCount = reader.ReadInt8L();
	RDebug::Print(_L("CStateTransFailurePolicy::ReadResultFileL - cdFailCount - %d"), cdFailCount);
	
	reader.ReadL(aDes, delimChar);
	TInt ncCount = reader.ReadInt8L();
	RDebug::Print(_L("CStateTransFailurePolicy::ReadResultFileL - ncCount - %d"), ncCount);
	
	reader.ReadL(aDes, delimChar);
	TInt ncFailCount = reader.ReadInt8L();
	RDebug::Print(_L("CStateTransFailurePolicy::ReadResultFileL - ncFailCount - %d"), ncFailCount);

	CleanupStack::PopAndDestroy(&reader);
	}

LOCAL_C void StartSystemL(void);

GLDEF_C TInt E32Main(void)
	{
	RDebug::Print(_L("CStateTransFailurePolicy::E32Main - entry"));
	__UHEAP_MARK; // mark heap state

	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();
	TRAPD(error, StartSystemL());
    delete TheTrapCleanup;

	__UHEAP_MARKEND; // check no memory leak

	RDebug::Print(_L("CStateTransFailurePolicy::E32Main - exit"));
	return error;
	}

LOCAL_C void StartSystemL(void)
	{
	RDebug::Print(_L("CStateTransFailurePolicy::StartSystemL - entry"));
	CActiveScheduler* theScheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(theScheduler);
	CActiveScheduler::Install(theScheduler);

	CStateTransFailurePolicy *starter = CStateTransFailurePolicy::NewLC(KDmHierarchyIdStartup, KAppServicesDomain3);
	starter->Start();
	CActiveScheduler::Start();
	
	User::Panic(KStateTransFailurePolicyPanic,1);

	CleanupStack::PopAndDestroy(&starter);
	CleanupStack::PopAndDestroy(&theScheduler);
	RDebug::Print(_L("CStateTransFailurePolicy::StartSystemL - exit"));
	}
