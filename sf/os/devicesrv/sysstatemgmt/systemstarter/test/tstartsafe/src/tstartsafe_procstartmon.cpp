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
 @test
 @internalComponent - Internal Symbian test code
*/

#include <startupproperties.h>

#include "startsafe.h"
#include "tstartsafe_procstartmon.h"
#include "testapps.h"
#include "testprocgoodsession.h"


const TInt KStartSafeTestKillCode = 666;
const TInt KStartSafeTestFinalKillCode = 667;
const TInt KThrottleTime = 15000000; //15s
const TInt KPoliteInterval = 1000000; //1s


/**
Old Test CaseID 		APPFWK-STARTSAFE-0011
New Test CaseID 		DEVSRVS-SYSSTART-STARTSAFE-0011
 */

TVerdict CAppFwkStartSafeTestStepProcStartMon::doTestStepL( void )
	{
	
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties* prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );

	RProcess proc;	
	CleanupClosePushL( proc );
	
	// KLaunchServerCommandLineOption makes the process listen for remote control connections
	prop->SetFileParamsL( KTestProcGood, KLaunchServerCommandLineOption ); 
	prop->SetStartupType( EStartProcess );
	prop->SetStartMethod( EWaitForStart ); // EFireAndForget is disallowed
	prop->SetMonitored( ETrue ); // the process death will be monitored
	prop->SetNoOfRetries( 1 );   // Must be non-zero in order to invoke a restart


		
	INFO_PRINTF1( _L("Starting the Test-process") );
		
	TInt tried = 0;	
		
	TRAPD( err, startSafe->StartAndMonitorL( *prop, proc, tried) );
	User::After( KPoliteInterval );

	// Produce the process-name search-term
	TPtrC namePtr( KTestProcGood() );
	namePtr.Set( namePtr.Ptr(), namePtr.Find(_L(".")) );


	// See if StartSafe thinks it's been successful _and that the process can be found. (if, line 73)
	// Then check to see that it has been restarted after being Kill()ed (if, line 85)
	if( (KErrNone == err) && ProcessExists(namePtr) )
		{
		
		INFO_PRINTF2( _L("Process \"%S\" started"), &namePtr );
		
			
		proc.Kill( KStartSafeTestKillCode );
		proc.Close();
		// Process should be restarted here
		User::After( KThrottleTime );
		
		
		if( ProcessExists( namePtr, proc ) )
			{
			INFO_PRINTF2( _L("Process \"%S\" REstarted successfully"), &namePtr );
			
			// RTestProcGoodSession is used to remotely control the process, allowing this process to cancel the monitoring of the target process, 
			// otherwise it keeps being restarted
			RTestProcGoodSession testProc;
			testProc.Connect();
			testProc.CancelMonitor();
			testProc.Close();
			
			proc.Kill( KStartSafeTestFinalKillCode );
		
			SetTestStepResult( EPass );	
			}
		else
			{
			ERR_PRINTF1( _L("Process was _not restarted") );
			
			SetTestStepResult( EFail );	
			}
			
		}
	else 
		{
		ERR_PRINTF2( _L("StartAndMonitorL left with"), err );	
		
		SetTestStepResult( EFail );
		}
		
		
	CleanupStack::PopAndDestroy( 3, startSafe ); // proc, prop, startSafe
	
	return TestStepResult();	
	}

TBool CAppFwkStartSafeTestStepProcStartMon::ProcessExists(const TDesC& aProcessName)
	{
	RProcess process;
	TBool result = EFalse;
	result = ProcessExists(aProcessName, process);
	process.Close(); //Closing a closed process handle, which would be the case if the process we wanted can't be found, is not an error, so no need to check if open
	return result;
	}
	
TBool CAppFwkStartSafeTestStepProcStartMon::ProcessExists(const TDesC& aProcessName, RProcess& aProcess)
	{
	TBool found = EFalse;
	TFullName searchTerm(aProcessName);
	searchTerm += _L("*");
	TFindProcess find(searchTerm);
	TFullName name;
	while(find.Next(name)==KErrNone)
		{
		const TInt err = aProcess.Open(find);
		if (err == KErrNone)
			{
			if (aProcess.ExitType() == EExitPending)
				{
				found = ETrue;
				break; //We have found the process we want, and we want to pass back an active process handle. Breaks to line 150 (return).
				}
			aProcess.Close(); //The process isn't the one we want, so close the handle.
			}
		}
	return found;
	}


CAppFwkStartSafeTestStepProcStartMon::CAppFwkStartSafeTestStepProcStartMon()
	{
	SetTestStepName(KCTestCaseProcStartMon);
	}
