// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
//



/**
 @file TDBT1Step.cpp
 @internalTechnology
*/

#include "TDBT1Step.h"

CTDBT1Step::~CTDBT1Step()
/**
 * Destructor
 */
	{
	}

CTDBT1Step::CTDBT1Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KTDBT1Step);
	}

TVerdict CTDBT1Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	 CTe_LbsBtGpsPsyStepBase::doTestStepPreambleL();
	 if (TestStepResult()!=EPass)
	    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTDBT1Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		TInt test;
		if(	!GetIntFromConfig(ConfigSection(),KTe_LbsBtGpsPsySuiteInt,test)
			)
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		SetTestStepResult(EPass);
		StartL(test);
		}
	  return TestStepResult();
	}


TVerdict CTDBT1Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	 CTe_LbsBtGpsPsyStepBase::doTestStepPostambleL();

	return TestStepResult();
	}


// ---------------------------------------------------------
// CTDBT1Step::StartL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CTDBT1Step::StartL(TInt aIndex)
    {
    switch(aIndex)
        {
        case 0:
        	_LIT(KIndexName0, "**** TCB1001 - Check PSY info and position ****");
            INFO_PRINTF1(KIndexName0);
        	Sub0TCB1001PSYInfoAndPositionRequestingL();
            break;
        case 1:
        	_LIT(KIndexName1, "**** TCB1002 - Periodic updates ****");
        	INFO_PRINTF1(KIndexName1);
            Sub1TCB1002PeriodicUpdatesL();
            break;
        case 2:
        	_LIT(KIndexName2, "**** TCB2001 - Module status changes ****" );
        	INFO_PRINTF1(KIndexName2);
            Sub2TCB2001ModuleStatusChangesL();
            break;
        case 3:
        	_LIT(KIndexName3, "**** TCB3002 - BT GPS Availibility ****");
        	INFO_PRINTF1(KIndexName3);
            Sub3TCB3002GPSAvailibilityL();
            break;
        }    
    }


void CTDBT1Step::Sub0TCB1001PSYInfoAndPositionRequestingL()
    {
    TInt err = KErrNone;
    TInt err2 = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    INFO_PRINTF1(KNewlineTxt);
    INFO_PRINTF1(KStopTxt);
    INFO_PRINTF1(KNewlineTxt);
    
    TRAP( err, RunSubCase0L( common ) );

    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    //User::After( 10*1000*1000 );
    //TRAP( err2, RunSubCase0WithDefaultL( common ) );


    INFO_PRINTF1(KNewlineTxt);
    INFO_PRINTF1(KEnvCleaningStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    INFO_PRINTF1(KEnvCleaningStopTxt);
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYModInfoAndPosReq, "BT GPS PSY module info check and position requests case failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYModInfoAndPosReq, err );
    _LIT( KErrBTGPSPSYModInfoAndPosReq2, "BT GPS PSY module info check and position requests as default PSY case failed with code %d" );
    AssertTrueSecL( err2 == KErrNone, KErrBTGPSPSYModInfoAndPosReq2, err2 );
    _LIT(KIndexName0, "Ended: **** TCB1001 - Check PSY info and position ****");
    INFO_PRINTF1(KIndexName0);
    }

void CTDBT1Step::RunSubCase0L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TPositionModuleInfo modInfo;
    
    TRequestStatus status = KRequestPending;
    TPositionInfo basic;
    
    RPositionServer posServer;
    
    INFO_PRINTF1(KOpenMLFWTxt);
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    INFO_PRINTF1(KGetPsyInformationTxt);
    err = posServer.GetModuleInfoById( KbtGPSPsyId, modInfo );    
    AssertTrueSecL( err == KErrNone, KPSYNotRetreivedTxt, err );

    CheckModuleInfoL( modInfo );
    
    RPositioner BTGpsPsy;
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, _L( "Setting the requestor failed for BT GPS PSY, code %d" ), err );
    
    //iUserInfo->ShowDialog( _L( "Select GPS and after that press OK" ), EUtfwDialogTypeOk, EFalse ); 
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( KMakeTenPositionRequestTxt);
    for( TInt i = 0; i < 10; i++ )
        {
        status = KRequestPending;
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        // and wait until the request is complete        
        User::WaitForRequest(status);
        
        INFO_PRINTF1(KWaitOneSecond);
        
        aCommon->FlushEventsL();
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        }
    
    CleanupStack::PopAndDestroy(2, &posServer); // posServer, BTGpsPsy
    }

void CTDBT1Step::RunSubCase0WithDefaultL( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TPositionModuleInfo modInfo;
    
    TRequestStatus status = KRequestPending;
    TPositionInfo basic;
    
    RPositionServer posServer;
    
    INFO_PRINTF1(KOpenMLFWTxt);
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    INFO_PRINTF1(KGetPsyInformationTxt);
    err = posServer.GetModuleInfoById( KbtGPSPsyId, modInfo );    
    AssertTrueSecL( err == KErrNone, KPSYNotRetreivedTxt, err );
    
    CheckModuleInfoL( modInfo );
    
    RPositioner BTGpsPsy;
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, _L( "Setting the requestor failed for BT GPS PSY, code %d" ), err );
    
    INFO_PRINTF1( KMakeTenPositionRequestTxt);
    for( TInt i = 0; i < 10; i++ )
        {
        status = KRequestPending;
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        // and wait until the request is complete        
        User::WaitForRequest(status);
        
        User::After( TTimeIntervalMicroSeconds32(8000000) );
        aCommon->FlushEventsL();
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        
        PrintPositionInformationL( basic );
        }
    
    CleanupStack::PopAndDestroy(2, &posServer); // posServer, BTGpsPsy
    }

void CTDBT1Step::Sub1TCB1002PeriodicUpdatesL()
    {
    TInt err = KErrNone;
    TInt err2 = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
   
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    TRAP( err, RunSubCase1L( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );

    //TRAP( err2, RunSubCase1WithDefaultL( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );

    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPeriodicUpdates, "BT GPS periodic position updating failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPeriodicUpdates, err );
    _LIT( KErrBTGPSPeriodicUpdates2, "BT GPS periodic position updating with default PSY failed with code %d" );
    AssertTrueSecL( err2 == KErrNone, KErrBTGPSPeriodicUpdates2, err2 );
	_LIT(KIndexName1, "Ended: **** TCB1002 - Periodic updates ****");
	INFO_PRINTF1(KIndexName1);
    }

void CTDBT1Step::RunSubCase1L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TBuf<100> debugPrint;
    TBool correctInterval = ETrue;
    TInt timeErrors = KErrNone;
    
    TRequestStatus status = KRequestPending;
    TPositionInfo basic;
    
    RPositionServer posServer;
    
    INFO_PRINTF1(KOpenMLFWTxt);
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    RPositioner BTGpsPsy;
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, _L( "Setting the requestor failed for BT GPS PSY, code %d" ), err );
    
    //iUserInfo->ShowDialog( _L( "Select GPS and after that press OK" ), EUtfwDialogTypeOk, EFalse ); 
    //User::After((TTimeIntervalMicroSeconds32) 10000000 );
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Set periodic time interval for position updates" ));
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(5000000) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, _L( "Setting the update options to BT GPS failed, code %d" ), err );
    
    TTime endFixTime(0);
    TTime startFixTime(0);
    
    INFO_PRINTF1( KMakeTenPositionRequestTxt);
    
    TInt i = 0;
    TPosition position;
    
    for( i = 0; i < 10; i++ )
        {
        status = KRequestPending;
        
        debugPrint.Format( _L( "Request no: %d" ), ( i + 1 ) );
        INFO_PRINTF1( debugPrint );
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        User::WaitForRequest(status);
        User::ResetInactivityTime();
        aCommon->FlushEventsL();
        
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        
        PrintPositionInformationL( basic );
        basic.GetPosition(position);
    	AssertTrueSecL(position.Time().Int64()>0, KNoTimestampTxt, KErrGeneral);
        }

    INFO_PRINTF1( _L( "Make ten periodic position requests and measure time" ));
    TInt interval = 0;

    startFixTime.UniversalTime();
    for( i = 0; i < 10; i++ )
        {
        status = KRequestPending;
        
        debugPrint.Format( _L( "Request no: %d" ), ( i + 1 ) );
        INFO_PRINTF1( debugPrint );
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        User::WaitForRequest(status);
        User::ResetInactivityTime();
        aCommon->FlushEventsL();
        
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        
        PrintPositionInformationL( basic );
        }

    endFixTime.UniversalTime();

    TTimeIntervalMicroSeconds fixTime = endFixTime.MicroSecondsFrom( startFixTime );
    interval = fixTime.Int64();

    if( interval > 55000000 )
        {
        correctInterval = EFalse;
        debugPrint.Format( _L( "The used time for 10 requests was more than 55 seconds: %d" ), interval );
        ERR_PRINTF1(debugPrint);
        }
    
    INFO_PRINTF1( _L( "Set periodic time interval for position updates" ));
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(1000000) );
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, _L( "Setting the update options to BT GPS failed, code %d" ), err );
    
    INFO_PRINTF1( KStartThreadTenUpdates);
    RThread thread;
    
    TRequestStatus threadStatus;
    
    INFO_PRINTF1( KDisableEmulatorPanic);
    User::SetJustInTime(EFalse);
    
    INFO_PRINTF1( KCreateThread);
    err = thread.Create( _L( "BTGPSTestThread" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, this );
    AssertTrueSecL( err == KErrNone, KErrorCreatingThread, err );
    CleanupClosePushL( thread );
    
    INFO_PRINTF1(KMakeTestDriverMonitorThread);
    thread.Logon( threadStatus );
    
    INFO_PRINTF1( KStartThread);
    thread.Resume();

    startFixTime.UniversalTime();    
    INFO_PRINTF1( _L( "Make same time other ten periodic position requests" ));
    for( i = 0; i < 10; i++ )
        {
        status = KRequestPending;
        
        debugPrint.Format( _L( "Request no: %d" ), ( i + 1 ) );
        INFO_PRINTF1( debugPrint );
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        // and wait until the request is complete
        User::WaitForRequest(status);
        
        User::ResetInactivityTime();
        
        aCommon->FlushEventsL();
                
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        }

    endFixTime.UniversalTime();
    
    INFO_PRINTF1( _L( "Wait for the thread to complete" ));
    User::WaitForRequest(threadStatus);
    
    
    fixTime = endFixTime.MicroSecondsFrom( startFixTime );
    interval = fixTime.Int64();

    if( interval > 20000000 )
        {
        correctInterval = EFalse;
        debugPrint.Format( _L( "The used time for 10 requests was more than 20 seconds: %d" ), interval );
        ERR_PRINTF1( debugPrint );
        }

    INFO_PRINTF1( _L( "Check thred exit values" ));
    
    debugPrint.Format( _L( "Test exit type: %d, exit status %d" ), thread.ExitType(), thread.ExitReason() );
    INFO_PRINTF1( debugPrint );
    AssertTrueSecL( thread.ExitType() == EExitKill, _L( "Thread exit type wrong: %d, wanted: 0" ), thread.ExitType() );
    AssertTrueSecL( thread.ExitReason() == 0, _L( "Thread exit reason wrong: %d, wanted: 0" ), thread.ExitReason() );
    
    AssertTrueSecL( correctInterval, _L( "Errors with periodic intervals %d" ), timeErrors );
    
    INFO_PRINTF1( _L( "Put the emulator panic behaviour back to normal" ));
    User::SetJustInTime( ETrue );
    
    CleanupStack::PopAndDestroy(3, &posServer); // posServer, BTGpsPsy, thread
}

void CTDBT1Step::RunSubCase1WithDefaultL( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TBuf<100> debugPrint;
    TBool correctInterval = ETrue;
    TInt timeErrors = KErrNone;
    
    TRequestStatus status = KRequestPending;
    TPositionInfo basic;
    
    RPositionServer posServer;
    
    INFO_PRINTF1(KOpenMLFWTxt);
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    RPositioner BTGpsPsy;
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer );
    CleanupClosePushL(BTGpsPsy);
    
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, _L( "Setting the requestor failed for BT GPS PSY, code %d" ), err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Set periodic time interval for position updates" ));
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(5000000) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, _L( "Setting the update options to BT GPS failed, code %d" ), err );
    
    TTime endFixTime(0);
    TTime startFixTime(0);
    
    INFO_PRINTF1( KMakeTenPositionRequestTxt);
    
    TInt i = 0;

    for( i = 0; i < 10; i++ )
        {
        status = KRequestPending;
        
        debugPrint.Format( _L( "Request no: %d" ), ( i + 1 ) );
        INFO_PRINTF1( debugPrint );
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        User::WaitForRequest(status);
        User::ResetInactivityTime();
        aCommon->FlushEventsL();
        
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        
        PrintPositionInformationL( basic );
        }

    TInt interval = 0;

    INFO_PRINTF1( _L( "Make ten periodic position requests and measure time" ));
    startFixTime.UniversalTime();
    for( i = 0; i < 10; i++ )
        {
        status = KRequestPending;
        
        debugPrint.Format( _L( "Request no: %d" ), ( i + 1 ) );
        INFO_PRINTF1( debugPrint );
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        User::WaitForRequest(status);
        User::ResetInactivityTime();
        aCommon->FlushEventsL();
        
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        
        PrintPositionInformationL( basic );
        }

    endFixTime.UniversalTime();

    TTimeIntervalMicroSeconds fixTime = endFixTime.MicroSecondsFrom( startFixTime );
    interval = fixTime.Int64();

    if( interval > 55000000 )
        {
        correctInterval = EFalse;
        debugPrint.Format( _L( "The used time for 10 requests was more than 55 seconds: %d" ), interval );
        ERR_PRINTF1( debugPrint );
        }
    
    INFO_PRINTF1( _L( "Set periodic time interval for position updates" ));
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(1000000) );
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, _L( "Setting the update options to BT GPS failed, code %d" ), err );
    
    INFO_PRINTF1( _L( "Start the a thread which makes ten periodic position updates" ));
    RThread thread;
    
    TRequestStatus threadStatus;
    
    INFO_PRINTF1( _L( "Disable that panic crashes the emulator" ));
    User::SetJustInTime(EFalse);
    
    INFO_PRINTF1( KCreateThread);
    err = thread.Create( _L( "BTGPSTestThread" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, this );
    AssertTrueSecL( err == KErrNone, KErrorCreatingThread, err );
    CleanupClosePushL( thread );
    
    INFO_PRINTF1(KMakeTestDriverMonitorThread);
    thread.Logon( threadStatus );
    
    INFO_PRINTF1( KStartThread);
    thread.Resume();

    startFixTime.UniversalTime();    
    INFO_PRINTF1( _L( "Make same time other ten periodic position requests" ));
    for( i = 0; i < 10; i++ )
        {
        status = KRequestPending;
        
        debugPrint.Format( _L( "Request no: %d" ), ( i + 1 ) );
        INFO_PRINTF1( debugPrint );
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        // and wait until the request is complete
        User::WaitForRequest(status);
        
        User::ResetInactivityTime();
        
        aCommon->FlushEventsL();
                
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        }

    endFixTime.UniversalTime();
    
    INFO_PRINTF1( _L( "Wait for the thread to complete" ));
    User::WaitForRequest(threadStatus);
    
    
    fixTime = endFixTime.MicroSecondsFrom( startFixTime );
    interval = fixTime.Int64();

    if( interval > 12000000 )
        {
        correctInterval = EFalse;
        debugPrint.Format( _L( "The used time for 10 requests was more than 12 seconds: %d" ), interval );
        ERR_PRINTF1( debugPrint );
        }

    INFO_PRINTF1( _L( "Check thred exit values" ));
    
    debugPrint.Format( _L( "Test exit type: %d, exit status %d" ), thread.ExitType(), thread.ExitReason() );
    INFO_PRINTF1( debugPrint );
    AssertTrueSecL( thread.ExitType() == EExitKill, _L( "Thread exit type wrong: %d, wanted: 0" ), thread.ExitType() );
    AssertTrueSecL( thread.ExitReason() == 0, _L( "Thread exit reason wrong: %d, wanted: 0" ), thread.ExitReason() );
    
    AssertTrueSecL( correctInterval, _L( "Errors with periodic intervals %d" ), timeErrors );
    
    INFO_PRINTF1( _L( "Put the emulator panic behaviour back to normal" ));
    User::SetJustInTime( ETrue );
    
    CleanupStack::PopAndDestroy(3, &posServer); // posServer, BTGpsPsy, thread
}

void CTDBT1Step::Sub2TCB2001ModuleStatusChangesL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    TRAP( err, RunSubCase2L( common ) );

    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYModInfoAndPosReq, "BT GPS PSY module statuses case failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYModInfoAndPosReq, err );
	_LIT(KIndexName2, "Ended: **** TCB2001 - Module status changes ****" );
	INFO_PRINTF1(KIndexName2);
    }

void CTDBT1Step::RunSubCase2L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    
    TBuf<100> message;
    const TInt KRounds = 9;

    TPositionModuleStatus::TDeviceStatus correctStatus[KRounds] = { 
        TPositionModuleStatus::EDeviceInitialising, 
        TPositionModuleStatus::EDeviceStandBy,
        TPositionModuleStatus::EDeviceReady,
        TPositionModuleStatus::EDeviceActive, 
        TPositionModuleStatus::EDeviceReady, 
        TPositionModuleStatus::EDeviceStandBy, 
        TPositionModuleStatus::EDeviceActive, 
        TPositionModuleStatus::EDeviceReady, 
        TPositionModuleStatus::EDeviceInactive };

    TRequestStatus status = KRequestPending;
    TRequestStatus moduleStatus = KRequestPending;
    TPositionInfo basic;
    TPositionModuleStatus deviceStatus;
    TPositionModuleStatusEvent btGPSEvent( TPositionModuleStatusEvent::EEventDeviceStatus );
    
    RPositionServer posServer;
    
    INFO_PRINTF1(KOpenMLFWTxt);
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    RPositioner BTGpsPsy;
    
    /* INACTIVE CHECKING AND OPENING */
    
    INFO_PRINTF1( KCheckModuleActive);
    aCommon->FlushEventsL();
    err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
    AssertTrueSecL( deviceStatus.DeviceStatus() == TPositionModuleStatus::EDeviceInactive, _L( "wrong module status wanted 3, got %d" ), deviceStatus.DeviceStatus() );
    
    INFO_PRINTF1(KStartListeningModuleEvents);
    moduleStatus = KRequestPending;
    posServer.NotifyModuleStatusEvent( btGPSEvent, moduleStatus );
    
    INFO_PRINTF1(KOpenPSYTxt);
    User::LeaveIfError(BTGpsPsy.Open( posServer, KbtGPSPsyId ));
    CleanupClosePushL(BTGpsPsy);
    
    INFO_PRINTF1(KSetRequestorTxt);
    User::LeaveIfError(BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 ));
    
    
    INFO_PRINTF1(KWaitModuleReportInitializing);
    // and wait until the request is complete

    // round 0: init
    // round 1: standby
    // round 2: ready
    // round 3: active
    // round 4: ready
    // round 5: standby
    // round 6: active
    // round 7: ready
    // round 8: inactive

    for( TInt i = 0; i < KRounds; i++ )
        {

        aCommon->FlushEventsL();

        message.Format(KRoundNumber, i );
        INFO_PRINTF1(message);

        User::WaitForRequest( moduleStatus );
        aCommon->FlushEventsL();

        INFO_PRINTF1(KCheckModuleStatus);
        btGPSEvent.GetModuleStatus( deviceStatus );
        if( deviceStatus.DeviceStatus() != correctStatus[i] )
            {
            message.Format(KWrongModuleStatus, correctStatus[i], deviceStatus.DeviceStatus() );
            INFO_PRINTF1(message);
            User::Leave(KErrGeneral);
            }
        
        if( i < 8 )
            {
            INFO_PRINTF1(KContinueListeningModuleEvents);
            moduleStatus = KRequestPending;
            posServer.NotifyModuleStatusEvent( btGPSEvent, moduleStatus );
            }

        // wait for the request
        if( i == 3 || i == 6 )
            {
            INFO_PRINTF1(KWaitRequestComplete);
            User::WaitForRequest(status);
            PrintPositionInformationL(basic);
            }
        // make request
        if( i == 2 || i == 5 )
            {
            INFO_PRINTF1(KMakeRequest);
            aCommon->FlushEventsL();
            // request positioninfo
            status = KRequestPending;
            BTGpsPsy.NotifyPositionUpdate( basic, status );
            }

        if( i == 7 )
            {
            aCommon->FlushEventsL();
            CleanupStack::PopAndDestroy(&BTGpsPsy);
            }
        }        
    
    CleanupStack::PopAndDestroy(&posServer);
}

void CTDBT1Step::Sub3TCB3002GPSAvailibilityL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    TRAP( err, RunSubCase3L( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );

    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYModInfoAndPosReq, "BT GPS PSY availibility case failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYModInfoAndPosReq, err );
	_LIT(KIndexName3, "Ended: **** TCB3002 - BT GPS Availibility ****");
	INFO_PRINTF1(KIndexName3);
    }

void CTDBT1Step::RunSubCase3L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TBuf<100> errorMessage;
    
    TRequestStatus status = KRequestPending;
    TPositionInfo basic;
    TPositionModuleStatus deviceStatus;
    
    RPositionServer posServer;
    
    INFO_PRINTF1(KOpenMLFWTxt);
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    RPositioner BTGpsPsy;
    
    
    /* Check that bt gps psy is available */
    INFO_PRINTF1( _L( "Check module is inactive" ));
    aCommon->FlushEventsL();
    err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
    AssertTrueSecL( err == KErrNone, KGettingModuleStatusFailedTxt, err );
    AssertTrueSecL( deviceStatus.DeviceStatus() == TPositionModuleStatus::EDeviceInactive, _L( "wrong module status wanted 3, got %d" ), deviceStatus.DeviceStatus() );
    
    
    //iUserInfo->ShowDialog( _L( "Check BT GPS is off and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    
    INFO_PRINTF1( _L( "BT GPS is off" ) );
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    INFO_PRINTF1( _L( "Check module is in standby" ) );
    TInt counter = 0;
    while( deviceStatus.DeviceStatus() != TPositionModuleStatus::EDeviceStandBy &&
        counter < 40)
        {
        counter++;
        User::After( (TTimeIntervalMicroSeconds32) 1000000  );
        aCommon->FlushEventsL();
        err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
        AssertTrueSecL(ETrue, _L("Device status = %d"), deviceStatus.DeviceStatus());
        }
    
    AssertTrueSecL( err == KErrNone, KGettingModuleStatusFailedTxt, err );
    AssertTrueSecL( deviceStatus.DeviceStatus() == TPositionModuleStatus::EDeviceStandBy, 
        _L( "wrong module status wanted 1, got %d" ), deviceStatus.DeviceStatus() );
    
    //iUserInfo->ShowDialog( _L( "Put BT GPS on and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    
    User::After( TTimeIntervalMicroSeconds32(500000) );
    
    TInt i = 0;
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, _L( "Setting the requestor failed for BT GPS PSY, code %d" ), err );
    
    
    INFO_PRINTF1( _L( "Check module is ready" ) );
    
    i = 0;
    while( i < 12 && deviceStatus.DeviceStatus() != TPositionModuleStatus::EDeviceReady )
        {
        User::After( TTimeIntervalMicroSeconds32(6000000) );
        aCommon->FlushEventsL();
        err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
        i++;
        }
    
    AssertTrueSecL( err == KErrNone, KGettingModuleStatusFailedTxt, err );
    AssertTrueSecL( deviceStatus.DeviceStatus() == TPositionModuleStatus::EDeviceReady, 
        _L( "wrong module status wanted 6, got %d" ), deviceStatus.DeviceStatus() );
    
    
    INFO_PRINTF1( _L( "Make position request" ));
    i = 0;
    status = KRequestPending;
    while( status.Int() != KErrNone && i < 12 )
        {
        status = KRequestPending;
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        // and wait until the request is complete
        User::WaitForRequest(status);
        
        aCommon->FlushEventsL();

        PrintPositionInformationL( basic );
        
        User::After( TTimeIntervalMicroSeconds32(3000000) );
        i++;
        }

    errorMessage.Format( _L( "The position request succeeded without quality loss at round %d" ), i );
    INFO_PRINTF1( errorMessage );
    
    // if request failed
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    
    INFO_PRINTF1( _L( "BT GPS working OK" ) );
    
    //iUserInfo->ShowDialog( _L( "Position request succeeded, turn off the bt gps and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    
    aCommon->FlushEventsL();
    
    i = 0;
    while( i < 12 && deviceStatus.DeviceStatus() != TPositionModuleStatus::EDeviceStandBy )
        {
        User::After( TTimeIntervalMicroSeconds32(2000000) );
        aCommon->FlushEventsL();
        err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
        i++;
        }
    
    AssertTrueSecL( err == KErrNone, KGettingModuleStatusFailedTxt, err );
    AssertTrueSecL( deviceStatus.DeviceStatus() == TPositionModuleStatus::EDeviceStandBy, 
        _L( "wrong module status wanted 1, got %d" ), deviceStatus.DeviceStatus() );
    
    INFO_PRINTF1( _L( "GPS turned off" ));
    
    //iUserInfo->ShowDialog( _L( "PSY in error state, turn on the bt gps and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    
    INFO_PRINTF1( _L( "Make position request" ));

    aCommon->FlushEventsL();
    
    i = 0;
    status = KRequestPending;
    while( status.Int() != KErrNone && i < 12 )
        {
        status = KRequestPending;
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        // and wait until the request is complete
        User::WaitForRequest(status);
        
        aCommon->FlushEventsL();

        if( status.Int() == KErrNone )
            PrintPositionInformationL( basic );
        
        User::After( TTimeIntervalMicroSeconds32(3000000) );
        i++;
        }
    
    errorMessage.Format( _L( "The position request succeeded without quality loss at round %d" ), i );
    INFO_PRINTF1( errorMessage );

    // if request failed
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    
    INFO_PRINTF1( _L( "BT GPS is working OK" ) );
    
    //iUserInfo->ShowDialog( _L( "Move bt gps out of range and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    
    INFO_PRINTF1( _L( "Start moving the device out of range" ));
    
    while( status.Int() == KErrNone )
        {
        status = KRequestPending;
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        // and wait until the request is complete
        User::WaitForRequest(status);
        
        aCommon->FlushEventsL();
        
        User::After( TTimeIntervalMicroSeconds32(600000) );
        }
    
    AssertTrueSecL( status.Int() != KErrNone, _L( "position request success with code %d" ), status.Int() );
    
    INFO_PRINTF1( _L( "Start waiting error status" ));
    i = 0;
    while( i < 12 && deviceStatus.DeviceStatus() != TPositionModuleStatus::EDeviceStandBy )
        {
        aCommon->FlushEventsL();
        User::After( TTimeIntervalMicroSeconds32(1000000) );
        err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
        i++;
        errorMessage.Format( _L( "Round %d for error waiting" ), i );
        INFO_PRINTF1( errorMessage );
        }
    
    INFO_PRINTF1( _L( "Check module is in error state" ));
    
    AssertTrueSecL( err == KErrNone, KGettingModuleStatusFailedTxt, err );
    AssertTrueSecL( deviceStatus.DeviceStatus() == TPositionModuleStatus::EDeviceStandBy, 
        _L( "wrong module status wanted 1, got %d" ), deviceStatus.DeviceStatus() );
    
    //iUserInfo->ShowDialog( _L( "Move bt gps back to range and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    
    INFO_PRINTF1( _L( "Make position request" ));
    i= 0;
    while( status.Int() != KErrNone && i < 12 )
        {
        status = KRequestPending;
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        // and wait until the request is complete
        User::WaitForRequest(status);
        
        aCommon->FlushEventsL();
        
        User::After( TTimeIntervalMicroSeconds32(600000) );
        }
    
    // if request failed
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    PrintPositionInformationL( basic );
    
    errorMessage.Format( _L( "The position request succeeded without quality loss at round %d" ), i );
    INFO_PRINTF1( errorMessage );
    
    INFO_PRINTF1( _L( "Set periodic time interval for position updates" ));
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(2000000) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, _L( "Setting the update options to BT GPS failed, code %d" ), err );
    
    INFO_PRINTF1( _L( "Start periodic session" ));
    status = KRequestPending;    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( basic, status );
    // and wait until the request is complete
    User::WaitForRequest(status);
    PrintPositionInformationL( basic );
    
    //iUserInfo->ShowDialog( _L( "Start moving out of bt gps psy range after pressing OK" ), EUtfwDialogTypeOk, EFalse );
    
    while( status.Int() == KErrNone )
        {
        status = KRequestPending;    
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        // and wait until the request is complete
        User::WaitForRequest(status);
        aCommon->FlushEventsL();
        }
    
    AssertTrueSecL( status.Int() != KErrNone, _L( "position request success with code %d" ), status.Int() );
    
    INFO_PRINTF1( _L( "Check module has error" ) );
    err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
    AssertTrueSecL( err == KErrNone, KGettingModuleStatusFailedTxt, err );
    AssertTrueSecL( deviceStatus.DeviceStatus() == TPositionModuleStatus::EDeviceStandBy, 
        _L( "wrong module status wanted 1, got %d" ), deviceStatus.DeviceStatus() );
    
    //iUserInfo->ShowDialog( _L( "Start moving back to bt gps psy range after pressing OK" ), EUtfwDialogTypeOk, EFalse );
    
    while( status.Int() != KErrNone )
        {
        status = KRequestPending;    
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        // and wait until the request is complete
        User::WaitForRequest(status);
        }
    
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    PrintPositionInformationL( basic );
    
    CleanupStack::PopAndDestroy(2, &posServer); // posServer, bbtgpspsy
    }

void CTDBT1Step::MakeTenPeriodicUpdatesFromBTGPSPsyL(TInt /*forLogging*/ )
    {
    TInt err = KErrNone;
    TBuf<100> debugPrint;
    
    TRequestStatus status = KRequestPending;
    TPositionInfo basic;
    
    RPositionServer posServer;
    
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    
    RPositioner BTGpsPsy;
    
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    
    if( err != KErrNone )
        {
        User::Leave( err );
        }
    
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService , CRequestor::EFormatApplication , _L("tdBT1Thread") );
    
    if( err != KErrNone )
        {
        User::Leave( err );
        }
    
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval(  TTimeIntervalMicroSeconds(1000000) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    User::LeaveIfError( err );
    
    TTime previousFixGetTime(0);
    TTime thisFixGetTime(0);
    
    TInt interval = 0;
    
    PrintForThreadL( _L( "Start making requests" ) );
    TInt errors = KErrNone;

    thisFixGetTime.UniversalTime();
    
    for( TInt i = 0; i < 10; i++ )
        {
        status = KRequestPending;
        
        debugPrint.Format( _L( "THREAD: Request no: %d" ), ( i + 1 ) );
        PrintForThreadL( debugPrint );
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        // and wait until the request is complete
        User::WaitForRequest( status );
        
        User::ResetInactivityTime();
        
        debugPrint.Format( _L( "Request complete with code %d" ), status.Int() );
        PrintForThreadL( debugPrint );
        
        //PrintPositionInformationForThreadL( basic );
        
        // if request failed
        if( status.Int() != KErrNone )
            {
            errors++;
            } 
        }

    // take end time
    previousFixGetTime.UniversalTime();
    
    TTimeIntervalMicroSeconds fixTime = previousFixGetTime.MicroSecondsFrom( thisFixGetTime );
    interval = fixTime.Int64();

    debugPrint.Format( _L( "Execution time %d" ), interval );
    PrintForThreadL( debugPrint );
    if( interval > 15*1000*1000 )
        {
        User::Leave( KErrTotalLossOfPrecision );
        }

    // if request failed
    if( errors )
        {
        debugPrint.Format( _L( "Completion errors %d" ), errors );
        PrintForThreadL( debugPrint );
        User::Leave( KErrCompletion );
        }
    
    PrintForThreadL( _L( "Thread finnished" ) );
    
    CleanupStack::PopAndDestroy(2, &posServer); // posServer, BTGpsPsy
}

