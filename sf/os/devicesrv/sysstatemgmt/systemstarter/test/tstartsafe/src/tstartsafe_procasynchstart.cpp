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
#include "tstartsafe_procasynchstart.h"
#include "testapps.h"


/**
Old Test CaseID 		APPFWK-STARTSAFE-0009
New Test CaseID 		DEVSRVS-SYSSTART-STARTSAFE-0009
 */	

TVerdict CAppFwkStartSafeTestStepProcAsynchStart::doTestStepL( void )
	{
	
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties* prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );
	
	RProcess proc;
	CleanupClosePushL( proc );
	
	prop->SetFileParamsL( KTestAppGood, KNullDesC );
	prop->SetStartupType( EStartProcess );
	prop->SetStartMethod( EDeferredWaitForStart );
	

	TRequestStatus commandStatus;
	
	INFO_PRINTF1(_L( "Start the Test-App process asynchronously") );
	
	TRAPD( err, startSafe->StartL(*prop, proc, commandStatus) );	
	User::WaitForRequest( commandStatus );
	
	// Ensure that the process's name is that of the test-app.
	TPtrC procNamePtr = proc.FileName().Right( KTestAppGood().Length() );
	TInt nameComparison = procNamePtr.Compare( KTestAppGood() );
	
	
	if ( (KErrNone == err) && (commandStatus == KErrNone) && (0 == nameComparison) )
		{
		INFO_PRINTF2( _L("Process \"%S\" started correctly"), &procNamePtr );
		INFO_PRINTF1( _L("Test passed") );
		
		SetTestStepResult( EPass );
		}
	else
		{
		SetTestStepResult( EFail );
		
		ERR_PRINTF4( _L("Test failed. StartL returned %d TRS completed with %d Process name resolved to \"%S\""), 
						err, commandStatus.Int(), &procNamePtr );				
		}

	
	// Dispose of the test-app.	
	if( 0 == nameComparison )
		{
		proc.Kill( KErrNone );			
		}
		
		
	CleanupStack::PopAndDestroy( 3, startSafe );
	
	return TestStepResult();	
	}



CAppFwkStartSafeTestStepProcAsynchStart::CAppFwkStartSafeTestStepProcAsynchStart()
	{
	SetTestStepName(KCTestCaseProcAsynchStart);
	}

