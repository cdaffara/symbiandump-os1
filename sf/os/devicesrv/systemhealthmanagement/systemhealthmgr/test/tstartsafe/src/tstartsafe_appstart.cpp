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
#include "tstartsafe_appstart.h"
#include "testapps.h"

/**
Old Test CaseID 		APPFWK-STARTSAFE-0002
New Test CaseID 		DEVSRVS-SHMA-STARTSAFE-0002
 */

TVerdict CAppFwkStartSafeTestStepAppStart::doTestStepL()
	{
	
	CStartSafe *startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties *prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );
	
	RProcess proc;
	CleanupClosePushL( proc );
	
	prop->SetFileParamsL( KTestAppGood, KNullDesC );
	prop->SetStartupType( EStartApp );
	prop->SetStartMethod( EFireAndForget );
	// Tell StartSafe we intend to monitor in order that it provides the process handle 
	// (so we can Kill() it)
	prop->SetMonitored( ETrue );
	
	
	TInt tried = 0;
	
	INFO_PRINTF1( _L("Start the Test Application") );
	TRAPD( err, startSafe->StartL(*prop, proc, tried) );
	
	// Originally there was a delay (2s) here. TKAS

	// Ensure that the process's name is that of the test-app.
	TPtrC procNamePtr = proc.FileName().Right( KTestAppGood().Length() );
	TInt nameComparison = procNamePtr.Compare( KTestAppGood() );
		
	if ( (KErrNone == err) && (0 == nameComparison) )
		{
		INFO_PRINTF2( _L("Application \"%S\" started correctly"), &procNamePtr );
		
		SetTestStepResult( EPass );
		INFO_PRINTF1( _L("Test passed") );
		}
	else 
		{
		ERR_PRINTF1( _L("Test failed") );
		
		SetTestStepResult( EFail );
		}	

	// Dispose of the test-app.	
	if( 0 == nameComparison )
		{
		proc.Kill( KErrNone );			
		}	


	CleanupStack::PopAndDestroy( 3, startSafe ); 

	return TestStepResult();	
	}



CAppFwkStartSafeTestStepAppStart::CAppFwkStartSafeTestStepAppStart()
	{
	SetTestStepName( KCTestCaseAppStart );
	}
