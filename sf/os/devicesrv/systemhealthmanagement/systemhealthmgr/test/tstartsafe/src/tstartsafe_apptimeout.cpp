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
#include "tstartsafe_apptimeout.h"
#include "testapps.h"


const TInt KWaitTime = 300;

/**
Old Test CaseID 		APPFWK-STARTSAFE-0004
New Test CaseID 		DEVSRVS-SHMA-STARTSAFE-0004
 */

TVerdict CAppFwkStartSafeTestStepAppTimeout::doTestStepL( void )
	{
	
	CStartSafe *startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties *prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );
	
	RProcess proc;
	CleanupClosePushL( proc );
	
	prop->SetFileParamsL( KTestAppSlow, KNullDesC );
	prop->SetStartupType( EStartApp );
	prop->SetStartMethod( EWaitForStart );
	prop->SetNoOfRetries( 0 );
	prop->SetTimeout( KWaitTime );
	

	TInt tried = 0;
			
	
	INFO_PRINTF2( _L("Start the app and wait for %d milliseconds"), KWaitTime );	

	TRAPD( err, startSafe->StartL(*prop, proc, tried) );
	
	if ( KErrTimedOut == err )
		{
		SetTestStepResult( EPass );
		INFO_PRINTF1( _L("Timed out - Test passed") );
		}
	else
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L("No time-out - Test failed") );
		
		TPtrC procNamePtr = proc.FileName().Right( KTestAppSlow().Length() );
		// Check that the process is the app we think it is.
		if( 0 == procNamePtr.Compare( KTestAppSlow()) )
			{
			proc.Kill( KErrNone );	
			}
			
		}
		
		
	CleanupStack::PopAndDestroy( 3, startSafe );

	return TestStepResult();	
	}



CAppFwkStartSafeTestStepAppTimeout::CAppFwkStartSafeTestStepAppTimeout()
	{
	SetTestStepName(KCTestCaseAppTimeout);
	}
