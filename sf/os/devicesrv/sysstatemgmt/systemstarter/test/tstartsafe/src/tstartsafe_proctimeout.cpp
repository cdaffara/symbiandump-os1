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
#include "tstartsafe_proctimeout.h"
#include "testappslow.h"
#include "testapps.h"

const TInt KProcTimeoutWaitTime = 300;


/**
Old Test CaseID 		APPFWK-STARTSAFE-0003
New Test CaseID 		DEVSRVS-SYSSTART-STARTSAFE-0003
 */

TVerdict CAppFwkStartSafeTestStepProcTimeout::doTestStepL( void )
	{
	
	CStartSafe *startSafe = CStartSafe::NewL();	
	CleanupStack::PushL(startSafe);
	
	CStartupProperties *prop = CStartupProperties::NewL();
	CleanupStack::PushL(prop);
	
	RProcess proc;
	CleanupClosePushL( proc );
	
	prop->SetFileParamsL( KTestAppSlow, KNullDesC );
	prop->SetStartupType( EStartProcess );
	prop->SetStartMethod( EWaitForStart);
	prop->SetNoOfRetries( 0 );
	prop->SetTimeout( KProcTimeoutWaitTime );
	
	
	TInt tried = 0;
	
	INFO_PRINTF2( _L("Start the process and wait %d milliseconds"), KProcTimeoutWaitTime );	
	
	TRAPD( err, startSafe->StartL(*prop, proc, tried) );
	
	if ( err == KErrTimedOut )
		{
		SetTestStepResult( EPass );
		INFO_PRINTF1( _L("Process timed out.   Test passed") );		
		}
	else
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L("Timeout did not happen - Test Failed") );	
		
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



CAppFwkStartSafeTestStepProcTimeout::CAppFwkStartSafeTestStepProcTimeout()
	{
	SetTestStepName(KCTestCaseProcTimeout);
	}
