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
#include "tstartsafe_appasynchstart.h"
#include "testapps.h"

/**
Old Test CaseID 		APPFWK-STARTSAFE-0010
New Test CaseID 		DEVSRVS-SHMA-STARTSAFE-0010
 */


TVerdict CAppFwkStartSafeTestStepAppAsynchStart::doTestStepL()
	{
	
	CStartSafe *startSafe = CStartSafe::NewL();	
	CleanupStack::PushL(startSafe);
	
	CStartupProperties* prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );
	
	RProcess proc;
	CleanupClosePushL( proc );
	
	prop->SetFileParamsL( KTestAppGood, KNullDesC );
	prop->SetStartupType( EStartApp );
	prop->SetStartMethod( EDeferredWaitForStart );
	
	
	INFO_PRINTF1( _L("Start the Test-app asynchronously") );
	
	TRequestStatus commandStatus;
	
	TRAPD( err, startSafe->StartL(*prop, proc, commandStatus) );	
	User::WaitForRequest(commandStatus);

	INFO_PRINTF1( _L("Async call returned. Now check the startup status") );
	
	TPtrC procNamePtr = proc.FileName().Right( KTestAppGood().Length() );
	TInt nameComparison = procNamePtr.Compare( KTestAppGood() );
		
	if ( (commandStatus == KErrNone) && (0 == nameComparison) && (KErrNone == err) )
		{
		INFO_PRINTF2( _L("Application \"%S\" started correctly"), &procNamePtr );
		
		SetTestStepResult( EPass );
		INFO_PRINTF1( _L("Test passed") );
		}
	else 
		{
		SetTestStepResult( EFail );
		ERR_PRINTF3( _L("Test failed. TRS = %d, StartL returned %d"), commandStatus.Int(), err );				
		}

	// Dispose of the test-app.	
	if( 0 == nameComparison )
		{
		proc.Kill( KErrNone );			
		}


	CleanupStack::PopAndDestroy( 3, startSafe ); // proc, prop, startSafe
	
	return TestStepResult();	
	}



CAppFwkStartSafeTestStepAppAsynchStart::CAppFwkStartSafeTestStepAppAsynchStart()
	{
	SetTestStepName( KCTestCaseAppAsynchStart );
	}

