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
#include "tstartsafe_appstartapparcinit.h"
#include "testapps.h"


/**
Old Test CaseID 		APPFWK-STARTSAFE-0012
New Test CaseID 		DEVSRVS-SYSSTART-STARTSAFE-0012
 */

TVerdict CAppFwkStartSafeTestStepAppStartAppArcInit::doTestStepL( void )
	{
	
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties* prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );
	
	RProcess proc;
	CleanupClosePushL( proc );
	
	prop->SetFileParamsL( KTestAppGood, KNullDesC );
	prop->SetStartupType( EStartApp );
	prop->SetStartMethod( EDeferredWaitForStart );
	

	TRequestStatus startupTRS;
	TRequestStatus apparcSessionTRS;
	
	INFO_PRINTF1(_L("Starting the Test-App process asynchronously"));
	
	startSafe->StartL( *prop, proc, startupTRS );	

	User::WaitForRequest( startupTRS );
	INFO_PRINTF2( _L("Rendezvous completed with %d"), startupTRS.Int() );	
	
	// StartL should have left if it was unsuccessful, 
	// but ensure that the process's name is that of the test-app.
	TPtrC procNamePtr = proc.FileName().Right( KTestAppGood().Length() );
	TInt nameComparison = procNamePtr.Compare( KTestAppGood() );
	
	if( 0 == nameComparison )
		{
		INFO_PRINTF2( _L("Application \"%S\" started correctly"), &procNamePtr );	
		}
	else
		{
		ERR_PRINTF1( _L("Application-start unsuccessful. Test not completed") );
		
		SetTestStepResult( EFail );
		return TestStepResult();
		}
	
	startSafe->InitAppArcServer( apparcSessionTRS ); // try to initiate apparc, to see if that causes any problems
	User::WaitForRequest( apparcSessionTRS );
	

	if ( ( startupTRS == KErrNone) && ( apparcSessionTRS == KErrNone) )
		{
		SetTestStepResult( EPass );
		INFO_PRINTF1( _L("Test passed") );
		// Dispose of the test-app
		proc.Kill( KErrNone );		
		}
	else
		{
		SetTestStepResult( EFail );
		ERR_PRINTF3( _L("Test failed.startupTRS = %d apparcSessionTRS = %d "), startupTRS.Int(), apparcSessionTRS.Int() );				
		}
	
		
	CleanupStack::PopAndDestroy( 3, startSafe );

	return TestStepResult();	
	}



CAppFwkStartSafeTestStepAppStartAppArcInit::CAppFwkStartSafeTestStepAppStartAppArcInit()
	{
	SetTestStepName(KCTestCaseAppStartAppArcInit);
	}
