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
#include <hal.h>

#include "startsafe.h"
#include "tstartsafe_proctimesucc.h"
#include "testappslow.h"
#include "testapps.h"


const TInt KProcTimeSuccTimeoutMargin = 1600; //400; // milliseconds
const TInt KProcTimeSuccWaitTime = KTestAppSlowTimeoutInMilliSeconds + KProcTimeSuccTimeoutMargin;

/**
Old Test CaseID 		APPFWK-STARTSAFE-0005
New Test CaseID 		DEVSRVS-SHMA-STARTSAFE-0005
 */

TVerdict CAppFwkStartSafeTestStepProcTimeSucc::doTestStepL()
	{
	
	CStartSafe *startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties *prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );
	
	RProcess proc;
	CleanupClosePushL( proc );
	
	prop->SetFileParamsL( KTestAppSlow, KNullDesC );
	prop->SetStartupType( EStartProcess );	
	prop->SetStartMethod( EWaitForStart );
	prop->SetNoOfRetries( 0 );
	prop->SetTimeout( KProcTimeSuccWaitTime );

	
	INFO_PRINTF1( _L("Invoke StartSafe on the test-app") );
	
	TInt tried = 0;
	TInt nTicksInMicroseconds = 0;
	HAL::Get( HAL::ENanoTickPeriod, nTicksInMicroseconds );
	INFO_PRINTF2( _L("System nano-ticks in uSec = %d"), nTicksInMicroseconds );
	
	TUint nTicks = User::NTickCount();	
		
	TRAPD( err, startSafe->StartL(*prop, proc, tried) );
	
	nTicks = ( User::NTickCount() - nTicks );
	INFO_PRINTF2( _L("milliseconds after = %d"), nTicks * (nTicksInMicroseconds / 1000) );
	INFO_PRINTF2( _L("Of which test-app tardiness accounts for = %dms"), KTestAppSlowTimeoutInMilliSeconds );
		
	// Prepare to check that the process is the app we think it is.
	TPtrC procNamePtr = proc.FileName().Right( KTestAppSlow().Length() );
	TInt nameComparison = procNamePtr.Compare( KTestAppSlow() );
		
	if( (KErrNone == err) && (0 == nameComparison) )
		{
		SetTestStepResult( EPass );
		INFO_PRINTF2( _L("Process \"%S\" started correctly"), &procNamePtr );
		INFO_PRINTF1( _L("Test passed") );
		}
	else
		{
		ERR_PRINTF2( _L("Test Failed StartSafe returned %d"), err );
		INFO_PRINTF2( _L("Process name returned \"%S\""), &procNamePtr );
		
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



CAppFwkStartSafeTestStepProcTimeSucc::CAppFwkStartSafeTestStepProcTimeSucc()
	{
	SetTestStepName(KCTestCaseProcTimeSucc);
	}
