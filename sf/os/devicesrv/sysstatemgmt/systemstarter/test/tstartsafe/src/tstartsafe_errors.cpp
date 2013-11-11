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

#include <startupproperties.h>

#include "tstartsafe_errors.h"
#include "startsafe.h"
#include "testapps.h"



CStartSafeTestStepErrors::CStartSafeTestStepErrors()
	{
	
	SetTestStepName( KStartSafeErrors );	
	}
	
/**
Old Test CaseID 		APPFWK-STARTSAFE-0015
New Test CaseID 		DEVSRVS-SYSSTART-STARTSAFE-0015
 */	
	

TVerdict CStartSafeTestStepErrors::doTestStepL()
	{

	TRAPD( err,
			{	
			NoMonitorFlagL();
			MonitorNullFileNameL();
			BadSyncStartLParamsL();
			BadAsyncStartLParamsL();
			} );


	if( KErrNone != err )
		{
		ERR_PRINTF2( _L("Test failed with leave code %d"), err );
		
		iInterimVerdict = EFail;	
		}	
	
	return TestStepResult();
	}
	
	

/**
 Invoke StartSafe via the StartAndMonitorL() method. Supply a correctly formed CStartupProperties 
 with the exception of its monitor flag.
 The method should leave with KErrArgument.
*/
void CStartSafeTestStepErrors::NoMonitorFlagL()	
	{
	
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties* prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );

	RProcess proc;	
	CleanupClosePushL( proc );
	
	// Erroneous 'monitored' flag before call to StartAndMonitorL().
	prop->SetFileParamsL( KTestProcGood, KNullDesC );
	prop->SetStartupType( EStartProcess );
	prop->SetStartMethod( EWaitForStart ); 
	prop->SetMonitored( EFalse );
	prop->SetNoOfRetries( 0 );

		
	INFO_PRINTF1( _L("Calling StartAndMonitorL with 'monitored' flag set to false") );
		
	TInt tried = 0;	
		
	TRAPD( err, startSafe->StartAndMonitorL( *prop, proc, tried) );

	if( KErrArgument == err )
		{
		
		INFO_PRINTF1( _L("StartSafe correctly returned error code KErrArgument") );
		}
	else
		{
		
		iInterimVerdict = EFail;
		
		ERR_PRINTF2( _L("StartAndMonitorL erroneously returned %d"), err );
		}
		
		
	CleanupStack::PopAndDestroy( 3, startSafe ); // proc, prop, startSafe		
	}
	
	
	

/**
 Invoke StartSafe via the StartAndMonitorL() method. Supply a correctly formed CStartupProperties
 except for its filename parameter which is NULL.
 Attempt two invocations to ensure that both EStartApp and EStartProcess scenarios are tested.
*/	
void CStartSafeTestStepErrors::MonitorNullFileNameL()
	{
	
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties* prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );

	RProcess proc;	
	CleanupClosePushL( proc );

	TRequestStatus trs;
	
	
	
	
	// NULL filename (EStartApp) (iApaStarter::StartAppL() should supply the leave code)
	// Not specified in this test as it is not supplied directly by StartSafe.
	prop->SetFileParamsL( KNullDesC, KNullDesC );
	prop->SetStartupType( EStartApp );
	prop->SetStartMethod( EWaitForStart ); 
	prop->SetMonitored( ETrue );
	prop->SetNoOfRetries( 0 );

		
	INFO_PRINTF1( _L("Calling StartAndMonitorL with NULL filename (EStartApp)") );
		
	TInt tried = 0;	
		
	TRAPD( err, startSafe->StartAndMonitorL( *prop, proc, tried) );

	if( KErrNone != err )
		{
		
		INFO_PRINTF2( _L("StartSafe supplied error code %d"), err );
		}
	else
		{
		
		iInterimVerdict = EFail;
		
		ERR_PRINTF1( _L("StartAndMonitorL erroneously supplied KErrNone") );
		}
		
	
	
	
	// NULL filename (EStartProcess) (RProcess::Create() should supply the leave code)
	// Not specified in this test as it is not supplied directly by StartSafe.
	prop->SetFileParamsL( KNullDesC, KNullDesC );
	prop->SetStartupType( EStartProcess );
	prop->SetStartMethod( EWaitForStart ); 
	prop->SetMonitored( ETrue );
	prop->SetNoOfRetries( 0 );

		
	INFO_PRINTF1( _L("Calling StartAndMonitorL with NULL filename (EStartProcess)") );
		
	tried = 0;	
		
	TRAP( err, startSafe->StartAndMonitorL( *prop, proc, tried) );

	if( KErrNone != err )
		{
		
		INFO_PRINTF2( _L("StartSafe supplied error code %d"), err );
		}
	else
		{
		
		iInterimVerdict = EFail;
		
		ERR_PRINTF1( _L("StartAndMonitorL erroneously supplied KErrNone") );
		}
		
		
		
	CleanupStack::PopAndDestroy( 3, startSafe ); // proc, prop, startSafe		
	}
	
	

/**
 Check several erronious scenarios invoking the synchronous form of SafeStart::StartL()
 Details as commented.
*/	
void CStartSafeTestStepErrors::BadSyncStartLParamsL()
	{
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties* prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );

	RProcess proc;	
	CleanupClosePushL( proc );
	
	
	
	// TStartMethod enumeration out of range.
	const TInt BadStartMethodEnum = 10;
	prop->SetFileParamsL( KTestAppGood, KNullDesC );
	prop->SetStartupType( EStartProcess );
	prop->SetStartMethod( (TStartMethod)BadStartMethodEnum ); 
	prop->SetMonitored( EFalse ); 
	prop->SetNoOfRetries( 0 );   
		
	INFO_PRINTF1( _L("Calling synch StartL (EStartProcess) with TStartMethod out of range") );
		
	TInt tried = 0;	
		
	TRAPD( err, startSafe->StartL( *prop, proc, tried) );

	if( KErrArgument == err )
		{
		INFO_PRINTF1( _L("StartSafe correctly returned error code KErrArgument") );
		}
	else
		{
		iInterimVerdict = EFail;
		
		ERR_PRINTF2( _L("Sync StartL erroneously supplied %d"), err );
		}
		
		
		
	
	// TStartupType enumeration out of range. (EWaitForStart)
	const TInt BadStartupTypeEnum = 10;
	prop->SetFileParamsL( KTestAppGood, KNullDesC );
	prop->SetStartupType( (TStartupType)BadStartupTypeEnum );
	prop->SetStartMethod( EWaitForStart ); 
	prop->SetMonitored( EFalse ); 
	prop->SetNoOfRetries( 0 );   
	
	INFO_PRINTF1( _L("Calling synch StartL (EWaitForStart) with TStartupType out of range") );
		
	tried = 0;	
		
	TRAP( err, startSafe->StartL( *prop, proc, tried) );


	if( KErrArgument == err )
		{
		INFO_PRINTF1( _L("StartSafe correctly returned error code KErrArgument") );
		}
	else
		{
		iInterimVerdict = EFail;
		
		ERR_PRINTF2( _L("Sync StartL erroneously supplied %d"), err );
		}




	// TStartupType enumeration out of range. (EFireAndForget)
	prop->SetFileParamsL( KTestAppGood, KNullDesC );
	prop->SetStartupType( (TStartupType)BadStartupTypeEnum );
	prop->SetStartMethod( EFireAndForget ); 
	prop->SetMonitored( EFalse ); 
	prop->SetNoOfRetries( 0 );   

	INFO_PRINTF1( _L("Calling synch StartL (EFireAndForget) with TStartupType out of range") );
		
	tried = 0;	
		
	TRAP( err, startSafe->StartL( *prop, proc, tried) );


	if( KErrArgument == err )
		{
		INFO_PRINTF1( _L("StartSafe correctly returned error code KErrArgument") );
		}
	else
		{
		iInterimVerdict = EFail;
		
		ERR_PRINTF2( _L("Sync StartL erroneously supplied %d"), err );
		}



		
	// Non-existent filename (EStartProcess)
	_LIT( KNonExistentFilename, "nurdle.exe" );
	prop->SetFileParamsL( KNonExistentFilename, KNullDesC );
	prop->SetStartupType( EStartProcess );
	prop->SetStartMethod( EWaitForStart ); 
	prop->SetMonitored( EFalse ); 
	prop->SetNoOfRetries( 0 );   
	
	INFO_PRINTF1( _L("Calling synch StartL (EStartProcess) with non-existent filename") );
		
	tried = 0;	
		
	TRAP( err, startSafe->StartL( *prop, proc, tried) );


	if( KErrNotFound == err )
		{
		INFO_PRINTF1( _L("StartSafe correctly returned error code KErrNotFound") );
		}
	else
		{
		iInterimVerdict = EFail;
		
		ERR_PRINTF2( _L("Sync StartL erroneously supplied %d"), err );
		}




	// Non-existent filename (EStartApp)	
	prop->SetFileParamsL( KNonExistentFilename, KNullDesC );
	prop->SetStartupType( EStartApp );
	prop->SetStartMethod( EWaitForStart ); 
	prop->SetMonitored( EFalse ); 
	prop->SetNoOfRetries( 0 );   
	
	INFO_PRINTF1( _L("Calling synch StartL (EStartApp) with non-existent filename") );
		
	tried = 0;	
		
	TRAP( err, startSafe->StartL( *prop, proc, tried) );


	if( KErrNotFound == err )
		{
		INFO_PRINTF1( _L("StartSafe correctly returned error code KErrNotFound") );
		}
	else
		{	
		iInterimVerdict = EFail;
		
		ERR_PRINTF2( _L("Sync StartL erroneously supplied %d"), err );
		}
		
	


	// Negative retries (EStartApp)
	prop->SetFileParamsL( KTestAppGood, KNullDesC );
	prop->SetStartupType( EStartApp );
	prop->SetStartMethod( EWaitForStart ); 
	prop->SetMonitored( EFalse ); 
	prop->SetNoOfRetries( -1 );   
		
	INFO_PRINTF1( _L("Calling synch StartL (EStartApp) with negative retries") );
		
	tried = 0;	
		
	TRAP( err, startSafe->StartL( *prop, proc, tried) );

	if( KErrArgument == err )
		{
		INFO_PRINTF1( _L("StartSafe correctly returned error code KErrArgument") );
		}
	else
		{
		iInterimVerdict = EFail;
		
		ERR_PRINTF2( _L("Sync StartL erroneously supplied %d"), err );
		}

		
		

	// Negative retries (EStartProcess)
	prop->SetFileParamsL( KTestAppGood, KNullDesC );
	prop->SetStartupType( EStartProcess );
	prop->SetStartMethod( EWaitForStart ); 
	prop->SetMonitored( EFalse ); 
	prop->SetNoOfRetries( -1 );   
		
	INFO_PRINTF1( _L("Calling synch StartL (EStartProcess) with negative retries") );
		
	tried = 0;	
		
	TRAP( err, startSafe->StartL( *prop, proc, tried) );

	if( KErrArgument == err )
		{
		INFO_PRINTF1( _L("StartSafe correctly returned error code KErrArgument") );
		}
	else
		{
		iInterimVerdict = EFail;
		
		ERR_PRINTF2( _L("Sync StartL erroneously supplied %d"), err );
		}
		
	
		
	CleanupStack::PopAndDestroy( 3, startSafe ); // proc, prop, startSafe			
	}
	
	


/**
 Check several erronious scenarios invoking the asynchronous form of SafeStart::StartL()
 Details as commented.
*/
void CStartSafeTestStepErrors::BadAsyncStartLParamsL()
	{
	
	CStartSafe *startSafe = CStartSafe::NewL();	
	CleanupStack::PushL(startSafe);
	
	CStartupProperties* prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );
	
	RProcess proc;
	CleanupClosePushL( proc );
	

	// TStartupType enumeration out of range.
	const TInt BadStartupTypeEnum = 10;
	prop->SetFileParamsL( KTestAppGood, KNullDesC );
	prop->SetStartupType( (TStartupType)BadStartupTypeEnum );
	prop->SetMonitored( EFalse ); 
	prop->SetNoOfRetries( 0 );   
		
	INFO_PRINTF1( _L("Calling asynch StartL with TStartupType out of range") );
		
	TRequestStatus commandTrs;	
		
	TRAPD( err, startSafe->StartL(*prop, proc, commandTrs) );

	if( KErrArgument == err )
		{
		INFO_PRINTF1( _L("StartL correctly returned leave code KErrArgument") );
		}
	else
		{
		iInterimVerdict = EFail;
		
		ERR_PRINTF2( _L("StartL erroneously supplied %d"), err );
		}




	if( commandTrs == KRequestPending )
		{
		TRequestStatus* completeThisTrs = &commandTrs;

		User::RequestComplete( completeThisTrs, 666 );
		User::WaitForRequest( *completeThisTrs );
		}

	// Non-existent filename (EStartProcess)
	_LIT( KNonExistentFilename, "nurdle.exe" );

	prop->SetFileParamsL( KNonExistentFilename, KNullDesC );
	prop->SetStartupType( EStartProcess );
	prop->SetMonitored( EFalse ); 
	prop->SetNoOfRetries( 0 ); 
			
	INFO_PRINTF1( _L("Calling asynch StartL (EStartProcess) with non-existent filename") );
		
	TRAP( err, startSafe->StartL(*prop, proc, commandTrs) );

	if( KErrNotFound == err )
		{
		INFO_PRINTF1( _L("StartL correctly supplied leave code KErrNotFound") );
		}
	else
		{
		iInterimVerdict = EFail;
		
		ERR_PRINTF2( _L("StartL erroneously supplied %d"), err );
		}	

	if( commandTrs == KRequestPending )
		{
		TRequestStatus* completeThisTrs = &commandTrs;

		User::RequestComplete( completeThisTrs, 666 );
		User::WaitForRequest( *completeThisTrs );
		}
		



	// Non-existent filename. Async. (EStartApp)
	prop->SetFileParamsL( KNonExistentFilename, KNullDesC );
	prop->SetStartupType( EStartApp );
	prop->SetMonitored( EFalse ); 
	prop->SetNoOfRetries( 0 ); 
			
	INFO_PRINTF1( _L("Calling asynch StartL (EStartApp) with non-existent filename") );
		
	TRAP( err, startSafe->StartL(*prop, proc, commandTrs) );
	User::WaitForRequest( commandTrs );
	
	if( KErrNotFound == commandTrs.Int() )
		{
		INFO_PRINTF1( _L("StartL correctly supplied leave code KErrNotFound") );
		}
	else
		{
		iInterimVerdict = EFail;
		
		ERR_PRINTF2( _L("StartL erroneously supplied %d"), err );
		}

	if( commandTrs == KRequestPending )
		{
		TRequestStatus* completeThisTrs = &commandTrs;

		User::RequestComplete( completeThisTrs, 666 );
		User::WaitForRequest( *completeThisTrs );
		}
		
				
		
	CleanupStack::PopAndDestroy( 3, startSafe ); // proc, prop, startSafe
	}
	
	

TVerdict CStartSafeTestStepErrors::doTestStepPostambleL()
	{
	// In case any of the tests were inadvertently successful in starting the test-app
	ProcessFindAndKill( KTestAppGood );
	
	SetTestStepResult( iInterimVerdict );
	
	return TestStepResult();	
	}
	
		
		
TBool CStartSafeTestStepErrors::ProcessFindAndKill( const TDesC& aProcessName )
	{
	
	TBool found = EFalse;
	
	TFullName searchTerm( aProcessName );
	searchTerm += _L("*");
	
	TFindProcess find( searchTerm );
	TFullName name;
	
	while( find.Next(name)==KErrNone )
		{
		
		RProcess process;
		
		const TInt err = process.Open( find );
		
		if ( err == KErrNone )
			{
			if ( process.ExitType() == EExitPending )
				{
				found = ETrue;
				
				process.Kill( KErrCancel );
				process.Close(); 
				
				break;
				}
				
			process.Close(); 
			}
			
		}
		
	return found;
	}

	
	
	
	
