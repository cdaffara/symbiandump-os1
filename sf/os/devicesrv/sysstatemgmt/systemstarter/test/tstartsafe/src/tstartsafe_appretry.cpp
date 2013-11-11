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
#include <s32file.h>
#include <hal.h>

#include "testapps.h"
#include "startsafe.h"
#include "tstartsafe_appretry.h"
#include "testapprvafterretry.h"


// KDontRvCount is defined in testapprvafterretry.h
// the application will fail to rendezvous KDontRvCount times
// but will then acquiesce and rendezvous with KErrNone
const TInt KTries = KDontRvCount;
const TInt KStartSafeRetryTestTimeout = 600; //400;

/**
Old Test CaseID 		APPFWK-STARTSAFE-0008
New Test CaseID 		DEVSRVS-SYSSTART-STARTSAFE-0008
 */

TVerdict CAppFwkStartSafeTestStepAppRetry::doTestStepL( void )
	{
	TInt threadHandles_Before;
	TInt processHandles_Before;
	TInt threadHandles_After;
	TInt processHandles_After;
		
	RThread().HandleCount(processHandles_Before, threadHandles_Before);
	
	CStartSafe *startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties *prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );
	

	// Write the retry number to a file to be read, updated 
	// and eventually deleted by the test-app.
	RFs fs;	
	CleanupClosePushL( fs );
	
	RFileWriteStream writeStream;
	CleanupClosePushL( writeStream );
	
	if( KErrNone != fs.Connect()
	 || KErrNone != writeStream.Replace(fs, KDontRvCountFile, EFileWrite) )
		{
		User::Leave( KErrCouldNotConnect );	
		}	
					
	writeStream.WriteInt8L( KDontRvCount );
	writeStream.CommitL();		
	
	CleanupStack::PopAndDestroy( 2, &fs );
	
	prop->SetFileParamsL( KTestAppRvAfterRetry, KNullDesC );
	prop->SetStartupType( EStartApp );	
	prop->SetStartMethod( EWaitForStart );
	prop->SetNoOfRetries( KDontRvCount ); 
	prop->SetTimeout( KStartSafeRetryTestTimeout );
	
	RProcess proc;
	CleanupClosePushL( proc );
	
	TInt tried = 0;
	TInt nTicksInMicroseconds = 0;
	HAL::Get( HAL::ENanoTickPeriod, nTicksInMicroseconds );
	INFO_PRINTF2( _L("System nano-ticks in uSec = %d"), nTicksInMicroseconds );
	
	TUint nTicks = User::NTickCount();	
	
	INFO_PRINTF2( _L("Invoke StartSafe on %S"), &KTestAppRvAfterRetry() );
	TRAPD( err, startSafe->StartL(*prop, proc, tried) );
	
	nTicks = ( User::NTickCount() - nTicks );
	INFO_PRINTF2( _L("milliseconds after = %d"), nTicks * (nTicksInMicroseconds / 1000) );
	

	TPtrC procNamePtr = proc.FileName().Right( KTestAppRvAfterRetry().Length() );
	TInt nameComparison = procNamePtr.Compare( KTestAppRvAfterRetry() );
		
	if ( (KTries !=tried) || (KErrNone != err) || (0 != nameComparison) )
		{
		SetTestStepResult( EFail );	
		ERR_PRINTF3( _L("Test failed. Retries %d. StartL error code %d "), tried, err );
		}
	else
		{
		SetTestStepResult( EPass );	
		INFO_PRINTF2( _L("Test passed. App launched after %d unsuccessful tries"), tried );
		}

	// Dispose of the test-app.	
	if( 0 == nameComparison )
		{
		proc.Kill( KErrNone );			
		}
		
					
	CleanupStack::PopAndDestroy( 3, startSafe ); // proc, prop, startSafe
	
	RThread().HandleCount(processHandles_After, threadHandles_After);
	TEST(processHandles_After == processHandles_Before);
	TEST(threadHandles_After == threadHandles_Before);
	
	return TestStepResult();	
	}



CAppFwkStartSafeTestStepAppRetry::CAppFwkStartSafeTestStepAppRetry()
	{
	SetTestStepName( KCTestCaseAppRetry );
	}
