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
 @file TDBT2Step.cpp
 @internalTechnology
*/
#include "tdbt2step.h"

CTDBT2Step::~CTDBT2Step()
/**
 * Destructor
 */
	{
	}

CTDBT2Step::CTDBT2Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KTDBT2Step);
	}

TVerdict CTDBT2Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	 CTe_LbsBtGpsPsyStepBase::doTestStepPreambleL();
	 if (TestStepResult()!=EPass)
	    return   TestStepResult();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTDBT2Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
	    _LIT( KIndexName0, "TCA1001 - Max age with multiple clients" );
	    _LIT( KIndexName1, "TCA1002 - Timeouts" );
	    _LIT( KIndexName2, "TCA1003 - Datatypes" );
	    _LIT( KIndexName3, "TCA1004 - Partial updates" );
	    _LIT( KIndexName4, "TCA1006 - Canceling requests" );
	    _LIT( KIndexName5, "TCA2001 - Data quality" );
	    
	    TInt stepId;
	    
	    if(!GetIntFromConfig(ConfigSection(),KTe_LbsBtGpsPsySuiteInt, stepId))
	    	{
	    	SetTestStepResult(EFail);
	    	return TestStepResult();
	    	}
	    
	    switch(stepId)
	    	{
	        case 0:
	        	INFO_PRINTF1(KIndexName0);
	        	Sub0TCA1001MaxAgeWithMultipleClientsL();
	            break;
	        case 1:
	        	INFO_PRINTF1(KIndexName1);
	        	Sub1TCA1002TimeoutsL();
	            break;
	        case 2:
	        	INFO_PRINTF1(KIndexName2);
	        	Sub2TCA1003DataTypesL();
	            break;
	        case 3:
	        	INFO_PRINTF1(KIndexName3);
	        	Sub3TCA1004PartialUpdatesL();
	            break;
	        case 4:
	        	INFO_PRINTF1(KIndexName4);
	        	Sub4TCA1006CancelRequestsL();
	            break;
	        case 5:
	        	INFO_PRINTF1(KIndexName5);
	        	Sub5TCA2001DataQualityStatusL();
	        	break;
	    	}
		}
	  return TestStepResult();
	}



TVerdict CTDBT2Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

// Two-phased constructor.
CTDBT2Step* CTDBT2Step::NewL()
    {
    CTDBT2Step* self = new (ELeave) CTDBT2Step();
    return self;
    }

void CTDBT2Step::Sub0TCA1001MaxAgeWithMultipleClientsL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    
    INFO_PRINTF1(KStopTxt);
    
    TRAP( err, RunSubCase0L( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );
    
    INFO_PRINTF1(KEnvCleaningStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KEnvCleaningStopTxt);
    
    _LIT( KErrBTGPSPSYTest, "BT GPS PSY max age test failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYTest, err );
    }

void CTDBT2Step::RunSubCase0L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TBuf<100> message;
    
    TRequestStatus status = KRequestPending;
    TPositionInfo basic;
    TPositionCourseInfo course;
    TPosition pos;
    
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
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Set periodic time interval for position updates" ));
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(3000000) );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, KSettingUpdateFailedTxt, err );
    
    INFO_PRINTF1( _L( "Make two position requests" ));
    
    status = KRequestPending;
    BTGpsPsy.NotifyPositionUpdate( basic, status );
    User::WaitForRequest(status);
    aCommon->FlushEventsL();
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    PrintPositionInformationL( basic );
    
    status = KRequestPending;
    BTGpsPsy.NotifyPositionUpdate( course, status );
    User::WaitForRequest(status);
    aCommon->FlushEventsL();
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    PrintPositionInformationL( course );
    
    basic.GetPosition( pos );        
    TTime basicTime = pos.Time();
    
    course.GetPosition( pos );        
    TTime courseTime = pos.Time();
    
    AssertTrueSecL( courseTime == basicTime, _L( "Not same timestamp, differ %d us" ), 
        courseTime.MicroSecondsFrom( basicTime ).Int64());
    
    INFO_PRINTF1( KStartThreadTenUpdates);
    RThread thread;
    
    TRequestStatus threadStatus;
    
    INFO_PRINTF1( KDisableEmulatorPanic);
    User::SetJustInTime(EFalse);
    
    INFO_PRINTF1( KCreateThread);
    err = thread.Create( _L( "BTGPSTestThread" ), StartThreadL, KDefaultStackSize, KMinHeapSize, KMinHeapSize+KHeapOffset, this );
    AssertTrueSecL( err == KErrNone, KErrorCreatingThread, err );
    CleanupClosePushL( thread );
    
    INFO_PRINTF1(KMakeTestDriverMonitorThread);
    thread.Logon( threadStatus );
    
    INFO_PRINTF1( KStartThread);
    thread.Resume();
    
    User::After( TTimeIntervalMicroSeconds32( 6 * 1000 * 1000 ) );
    
    INFO_PRINTF1( _L( "Make six request, with 1 second delay, at least one should have different timestamp" ));
    
    TInt i = 0;
    for( i = 0; i < 6; i++ )
        {
        
        status = KRequestPending;
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        User::WaitForRequest(status);
        aCommon->FlushEventsL();
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        PrintPositionInformationL( basic );
        
        courseTime = basicTime;
        
        basic.GetPosition( pos );        
        basicTime = pos.Time();
        
        if( i != 0 && basicTime != courseTime )
            break;
        
        User::After( TTimeIntervalMicroSeconds32(1 * 1000 * 1000) );
        }
    
    AssertTrueSecL( i < 6, _L( "Time didn't change, rounds %d" ), i );
    message.Format( _L( "Timestamp changed at round %d" ), i );
    INFO_PRINTF1( message );
    
    if( i == 1 )
        {
        INFO_PRINTF1( _L( "Changed on second round, lets check that the time stays the same for the next request" ));
        status = KRequestPending;
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        User::WaitForRequest(status);
        aCommon->FlushEventsL();
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        PrintPositionInformationL( basic );
        
        courseTime = basicTime;
        
        basic.GetPosition( pos );        
        basicTime = pos.Time();
        
        AssertTrueSecL( basicTime == courseTime, _L( "Time changes always, differs %d us" ), 
            basicTime.MicroSecondsFrom( courseTime ).Int64());
        }
    
    aCommon->FlushEventsL();
    
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(1000000) );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, KSettingUpdateFailedTxt, err );
    
    INFO_PRINTF1( _L( "Make six request with 1s max age, wait 1s between request, always new timestamp" ));
    
    for( i = 0; i < 6; i++ )
        {
        
        status = KRequestPending;
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        User::WaitForRequest(status);
        aCommon->FlushEventsL();
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        PrintPositionInformationL( basic );
        
        courseTime = basicTime;
        
        basic.GetPosition( pos );        
        TTime basicTime = pos.Time();
        
        if( i != 0 && basicTime == courseTime )
            break;
        
        User::After( TTimeIntervalMicroSeconds32(1 * 1000 * 1000) );
        }
    
    AssertTrueSecL( i == 6, _L( "Time didn't change, on round %d" ), i );
    
    INFO_PRINTF1( _L( "Wait for the thread to complete" ));
    User::WaitForRequest(threadStatus);
    
    INFO_PRINTF1( _L( "Check thread exit values" ));
    AssertTrueSecL( thread.ExitType() == EExitKill, _L( "Thread exit type wrong: %d, wanted: 0" ), thread.ExitType() );
    AssertTrueSecL( thread.ExitReason() == 0, _L( "Thread exit reason wrong: %d, wanted: 0" ), thread.ExitReason() );
    
    CleanupStack::PopAndDestroy(3, &posServer); // posServer, BTGpsPsy, thread
    
}

void CTDBT2Step::Sub1TCA1002TimeoutsL()
    {
    TInt err = KErrNone;
    
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

    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSTest, "BT GPS timeout testing failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSTest, err );
    }

void CTDBT2Step::RunSubCase1L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    
    TRequestStatus status = KRequestPending;
    TRequestStatus timeoutStatus = KRequestPending;
    TPositionInfo basic;
    TPositionCourseInfo course;
    
    RPositionServer posServer;
    
    INFO_PRINTF1(KOpenMLFWTxt);
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    RPositioner BTGpsPsy;
    RPositioner BTGpsPsyTimeouted;
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    INFO_PRINTF1( KOpenForTimeoutPSYTxt);
    err = BTGpsPsyTimeouted.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    INFO_PRINTF1( KSetRequestorForTimeoutTxt);
    err = BTGpsPsyTimeouted.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Set timeout time for position updates for the other session" ));
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0));
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(3* 100 * 100) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0));

    
    err = BTGpsPsyTimeouted.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, _L( "Setting the update options to BT GPS timeouting failed, code %d" ), err );
    
    INFO_PRINTF1( KMakePositionRequestTxt);
    
    status = KRequestPending;
    BTGpsPsy.NotifyPositionUpdate( course, status );
    User::WaitForRequest(status);
    
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    PrintPositionInformationL( course );
    
    INFO_PRINTF1( _L( "Make two position requests, other has timeout" ));
    
    timeoutStatus = KRequestPending;
    BTGpsPsyTimeouted.NotifyPositionUpdate( basic, timeoutStatus );
    
    status = KRequestPending;
    BTGpsPsy.NotifyPositionUpdate( course, status );
    
    TTime requestFixTime;
    requestFixTime.UniversalTime();
    
    User::WaitForRequest(timeoutStatus);
    aCommon->FlushEventsL();
    
    User::WaitForRequest(status);
    
    TTime completeFixTime;
    completeFixTime.UniversalTime();
    
    PrintPositionInformationL( basic );
    PrintPositionInformationL( course );
    
    aCommon->FlushEventsL();
    
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    //AssertTrueSecL( timeoutStatus.Int() == KErrTimedOut, _L( "position request didn't timedout with code %d" ), status.Int() );
    
    TInt responseTime = completeFixTime.MicroSecondsFrom( requestFixTime ).Int64();
    
    //The following line is a safety net to check that the position request didn't take too much time
    AssertTrueSecL( responseTime < 4000000, _L( "position request took too long %d" ), responseTime );
    
    CleanupStack::PopAndDestroy(3, &posServer); // posServer, BTGpsPsy, BTGpsPsyTimeouted
    
    }

void CTDBT2Step::Sub2TCA1003DataTypesL()
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
    
    _LIT( KErrBTGPSPSYModInfoAndPosReq, "datatypes testing failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYModInfoAndPosReq, err );
    }

void CTDBT2Step::RunSubCase2L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TBuf<100> message;
    TRequestStatus status = KRequestPending;
    TPositionInfo basic;
    TPositionCourseInfo courseInfo;
    TPositionSatelliteInfo satelliteInfo;
    
    
    RPositionServer posServer;
    
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    RPositioner BTGpsPsy;
    
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();
    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( basic, status );
    
    User::WaitForRequest(status);
    
    aCommon->FlushEventsL();
    
    // if request failed
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    
    PrintPositionInformationL( basic );
    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( courseInfo, status );
    
    User::WaitForRequest(status);
    
    aCommon->FlushEventsL();
    
    PrintPositionInformationL( courseInfo );
    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( satelliteInfo, status );
    
    User::WaitForRequest(status);
    
    aCommon->FlushEventsL();
    
    // if request failed
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    
    PrintPositionInformationL( satelliteInfo );
    
    const TInt KTableSize = 10;
    
    const TPositionFieldId fields[KTableSize] = { EPositionFieldHorizontalSpeed, EPositionFieldHeading, 
        EPositionFieldNMEASentences, EPositionFieldSatelliteNumInView, EPositionFieldSatelliteNumUsed, 
        EPositionFieldSatelliteTime, EPositionFieldSatelliteHorizontalDoP, EPositionFieldSatelliteVerticalDoP, 
        EPositionFieldSatellitePositionDoP, EPositionFieldSatelliteSeaLevelAltitude };
    
    
    for( TInt i = 0; i < KTableSize; i++ )
        {
        
        message.Format( _L( "Round %d" ), i );
        INFO_PRINTF1( message );
        
        HPositionGenericInfo* genericInfo = HPositionGenericInfo::NewL(15*1024, 128);
        CleanupStack::PushL(genericInfo);
        
        genericInfo->SetRequestedField( fields[i] );
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( *genericInfo, status );
        
        User::WaitForRequest(status);
        
        aCommon->FlushEventsL();
        
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        
        if( genericInfo->IsFieldAvailable( fields[i] ) )
            {
            PrintPositionInformationL( genericInfo, fields[i] );
            }
        else
            {
            if( i != ( KTableSize - 1 ) )
                AssertTrueSecL( EFalse, _L( "Field number %d not supported" ), i );
            }
        CleanupStack::PopAndDestroy(genericInfo);
        }
    
    CleanupStack::PopAndDestroy(2, &posServer); // posServer, BTGpsPsy
    
}

void CTDBT2Step::Sub3TCA1004PartialUpdatesL()
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
    
    _LIT( KErrBTGPSPSYModInfoAndPosReq, "BT GPS PSY partial update case failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYModInfoAndPosReq, err );
    }

void CTDBT2Step::RunSubCase3L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TBuf<100> message;
    
    TRequestStatus status = KRequestPending;
    TRequestStatus status2 = KRequestPending;
    TPositionSatelliteInfo satellite;
    TPositionInfo basic;
    TPositionModuleStatus deviceStatus;
    
    RPositionServer posServer;
    
    INFO_PRINTF1(KOpenMLFWTxt);
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    RPositioner BTGpsPsy;
    RPositioner BTGpsPsy2;
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Open BT GPS PSY2" ));
    err = BTGpsPsy2.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Set requestor to BT GPS PSY2" ));
    err = BTGpsPsy2.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, _L("tdBT1 2") );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( KWaitModuleReady );
    
    while( deviceStatus.DeviceStatus() != TPositionModuleStatus::EDeviceReady )
        {
        err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
        AssertTrueSecL( err == KErrNone, KGettingModuleStatusFailedTxt, err );
        aCommon->FlushEventsL();
        User::After((TTimeIntervalMicroSeconds32) 1000000 );
        }
    
    AssertTrueSecL( deviceStatus.DeviceStatus() == TPositionModuleStatus::EDeviceReady, _L( "wrong module status wanted 6, got %d" ), deviceStatus.DeviceStatus() );
    
    INFO_PRINTF1(KMakePositionRequestTxt);
    status = KRequestPending;
    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( satellite, status );
    
    User::WaitForRequest(status);
    
    aCommon->FlushEventsL();
    
    PrintPositionInformationL( satellite );
    
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    
    INFO_PRINTF1( _L( "Make position request with 2" ));
    status2 = KRequestPending;
    
    // request positioninfo
    BTGpsPsy2.NotifyPositionUpdate( basic, status2 );
    
    User::WaitForRequest(status2);
    
    aCommon->FlushEventsL();
    
    PrintPositionInformationL( basic );
    
    AssertTrueSecL( status2.Int() == KErrNone, KPositionRequestFailedTxt, status2.Int() );
    
    
    INFO_PRINTF1( _L( "Set timeout time for position updates for the other session" ));
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetAcceptPartialUpdates( ETrue );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, _L( "Setting the update options to BT GPS timeouting failed, code %d" ), err );
    
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(2000000) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetAcceptPartialUpdates( EFalse );
    
    err = BTGpsPsy2.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, _L( "Setting the update options to BT GPS timeouting failed, code %d" ), err );
    
    
    //iUserInfo->ShowDialog( _L( "Put GPS in such position that the fix is partial, after that press OK" ), EUtfwDialogTypeOk, EFalse );
    // request positioninfo
    
    TInt counter = 0;
    TInt errors = 0;
    status = KRequestPending;
    while( status.Int() != KPositionPartialUpdate )
        {
        if( counter != 0 )
            {
            User::WaitForRequest(status2);
            
            if( status2.Int() != KPositionQualityLoss )
                errors++;
            }
        
        status = KRequestPending;
        status2 = KRequestPending;
        BTGpsPsy.NotifyPositionUpdate( satellite, status );
        BTGpsPsy2.NotifyPositionUpdate( basic, status2 );
        
        User::WaitForRequest(status);
        
        aCommon->FlushEventsL();
        
        if( counter > 20 )
            {
            INFO_PRINTF1( _L( "counter limit full, stop requests" ));
            break;
            }
        
        counter++;
        
        User::After((TTimeIntervalMicroSeconds32) 1000000 );
        }
    
    message.Format( _L( "The partial update was completed with code %d" ), status.Int() );
    INFO_PRINTF1( message );
    message.Format( _L( "The second session got %d return values other than KPositionQualityLoss" ), errors );
    INFO_PRINTF1( message );
    
    User::WaitForRequest(status2);
    
    PrintPositionInformationL( satellite );
    PrintPositionInformationL( basic );
    
    CleanupStack::PopAndDestroy(3, &posServer); // server, psy, psy2
}

void CTDBT2Step::Sub4TCA1006CancelRequestsL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    TRAP( err, RunSubCase4L( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );

    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrTest, "BT GPS PSY cancel case failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrTest, err );
    }

void CTDBT2Step::RunSubCase4L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    
    TRequestStatus status = KRequestPending;
    TRequestStatus trackingStatus = KRequestPending;
    TPositionInfo basic;
    TPositionModuleStatus deviceStatus;
    
    RPositionServer posServer;
    
    INFO_PRINTF1(KOpenMLFWTxt);
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    RPositioner BTGpsPsy;
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( KWaitModuleReady );
    
    while( deviceStatus.DeviceStatus() != TPositionModuleStatus::EDeviceReady )
        {
        err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
        AssertTrueSecL( err == KErrNone, KGettingModuleStatusFailedTxt, err );
        aCommon->FlushEventsL();
        User::After((TTimeIntervalMicroSeconds32) 1000000 );
        }
    
    AssertTrueSecL( deviceStatus.DeviceStatus() == TPositionModuleStatus::EDeviceReady, _L( "wrong module status wanted 6, got %d" ), deviceStatus.DeviceStatus() );
    
    INFO_PRINTF1(KMakePositionRequestTxt);
    status = KRequestPending;
    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( basic, status );
    
    User::WaitForRequest(status);
    
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Make position request to be canceled" ));
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( basic, status );
    
    INFO_PRINTF1( _L( "Make cancel request" ));
    BTGpsPsy.CancelRequest( EPositionerNotifyPositionUpdate );
    
    User::WaitForRequest(status);
    
    AssertTrueSecL( status.Int() == KErrCancel, _L( "position request was not canceled, but: %d" ), status.Int() );
    
    BTGpsPsy.Close();
    /*
    aCommon->FlushEventsL();
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, _L("tdBT2") );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();
    
    User::After((TTimeIntervalMicroSeconds32) 50000 );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Make cancel request" ));
    BTGpsPsy.CancelRequest( EPositionerNotifyPositionUpdate );
    
    BTGpsPsy.Close();

    User::After( 10*1000*1000 );
    
    INFO_PRINTF1( _L( "Test with default proxy session" ));
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer );
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1(KMakePositionRequestTxt);
    trackingStatus = KRequestPending;
    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( basic, trackingStatus );
    
    User::WaitForRequest(trackingStatus);
    
    AssertTrueSecL( trackingStatus.Int() == KErrNone, KPositionRequestFailedTxt, trackingStatus.Int() );
    
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Make position request to be canceled" ));
    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( basic, trackingStatus );
    
    User::After((TTimeIntervalMicroSeconds32) 250000 );
    INFO_PRINTF1( _L( "Make cancel" ));
    
    BTGpsPsy.CancelRequest( EPositionerNotifyPositionUpdate );
    
    User::WaitForRequest(status);
    
    AssertTrueSecL( trackingStatus.Int() == KErrCancel, _L( "position request was not canceled, but: %d" ), 
        trackingStatus.Int() );
    */
    CleanupStack::PopAndDestroy(2, &posServer); // posServer, psy
}

void CTDBT2Step::Sub5TCA2001DataQualityStatusL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    TRAP( err, RunSubCase5L( common ) );

    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStartTxt);
    
    CleanupStack::PopAndDestroy(common); // common
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYModInfoAndPosReq, "BT GPS PSY data quality case failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYModInfoAndPosReq, err );
    }

void CTDBT2Step::RunSubCase5L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TBuf<100> message;
    
    TRequestStatus status = KRequestPending;
    TPositionSatelliteInfo satellite;
    TPositionModuleStatus deviceStatus;
    
    RPositionServer posServer;
    
    INFO_PRINTF1(KOpenMLFWTxt);
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    RPositioner BTGpsPsy;
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( KWaitModuleReady );
    
    while( deviceStatus.DeviceStatus() != TPositionModuleStatus::EDeviceReady )
        {
        err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
        AssertTrueSecL( err == KErrNone, KGettingModuleStatusFailedTxt, err );
        aCommon->FlushEventsL();
        User::After((TTimeIntervalMicroSeconds32) 1000000 );
        }
    
    AssertTrueSecL( deviceStatus.DeviceStatus() == TPositionModuleStatus::EDeviceReady, 
        _L( "wrong module status wanted 6, got %d" ), deviceStatus.DeviceStatus() );
    
    INFO_PRINTF1(KMakePositionRequestTxt);
    status = KRequestPending;
    
    //iUserInfo->ShowDialog( _L( "Make sure the GPS is getting full fix (yellow light is blinking) and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( satellite, status );
    
    User::WaitForRequest(status);
    
    aCommon->FlushEventsL();
    
    PrintPositionInformationL( satellite );
    
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    
    err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
    AssertTrueSecL( err == KErrNone, KGettingModuleStatusFailedTxt, err );
    AssertTrueSecL( ( deviceStatus.DataQualityStatus() == TPositionModuleStatus::EDataQualityNormal ) || ( deviceStatus.DataQualityStatus() == TPositionModuleStatus::EDataQualityPartial ), 
        _L( "wrong data quality status, wanted 3, got %d" ), deviceStatus.DataQualityStatus() );
    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Set partial updates on for position updates for the session" ));
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetAcceptPartialUpdates( ETrue );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, KSettingUpdateFailedTxt, err );
    
    if( deviceStatus.DataQualityStatus() == TPositionModuleStatus::EDataQualityNormal )
        {
        //iUserInfo->ShowDialog( _L( "Put GPS in such position that the fix is partial, after that press OK" ), EUtfwDialogTypeOk, EFalse );
        }
    else if( deviceStatus.DataQualityStatus() == TPositionModuleStatus::EDataQualityPartial )
        {
        //iUserInfo->ShowDialog( _L( "The fix is already partial just press OK" ), EUtfwDialogTypeOk, EFalse );
        }
    
    // request positioninfo
    
    TInt i = 0;
    
    while( deviceStatus.DataQualityStatus() != TPositionModuleStatus::EDataQualityPartial )
        {
        i++;
        status = KRequestPending;
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( satellite, status );
        
        User::WaitForRequest(status);
        
        aCommon->FlushEventsL();
        
        message.Format( _L( "The partial update was completed with code %d" ), status.Int() );
        INFO_PRINTF1( message );
        
        PrintPositionInformationL( satellite );
       
        err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
        AssertTrueSecL( err == KErrNone, KGettingModuleStatusFailedTxt, err );
       
        message.Format( _L( "Request %d completed with code %d, data quality %d" ),
            i, status.Int(), deviceStatus.DataQualityStatus() );
        INFO_PRINTF1( message );
        
        aCommon->FlushEventsL();
        if( i > 10 )
            {
            INFO_PRINTF1( _L( "counter limit full, stop requests" ));
            break;
            }
        }
    
    INFO_PRINTF1( _L( "Set timeout time for position updates for the session" ));
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(2000000) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetAcceptPartialUpdates( EFalse );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, KSettingUpdateFailedTxt, err );
    
    //iUserInfo->ShowDialog( _L( "Put GPS in such position that the fix is lost, after that press OK" ), EUtfwDialogTypeOk, EFalse );
    
    status = KRequestPending;
    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( satellite, status );
    
    User::WaitForRequest(status);
    
    aCommon->FlushEventsL();
    
    message.Format( _L( "The update was completed with code %d" ), status.Int() );
    INFO_PRINTF1( message );
    
    PrintPositionInformationL( satellite );
    aCommon->FlushEventsL();
    
    CleanupStack::PopAndDestroy(2, &posServer); // &posServer);, psy
}

void CTDBT2Step::MakeTenPeriodicUpdatesFromBTGPSPsyL(TInt /*forLogging*/ )
    {
    TInt err = KErrNone;
    
    TRequestStatus status = KRequestPending;
    TPositionInfo basic;
    
    RPositionServer posServer;
    RPositioner BTGpsPsy;
    
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    
    User::LeaveIfError(err);
    
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService , CRequestor::EFormatApplication , _L("tdBT2") );
    
    User::LeaveIfError(err);
    
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval(  TTimeIntervalMicroSeconds(5000000) );
    
    BTGpsPsy.SetUpdateOptions( btGPSOptions );
    
    TTime timeOfTheFix(0);
    TTime previousFixGetTime(0);
    TTime thisFixGetTime(0);
    TInt fixTime = 0;
    TPosition pos;
    
    for( TInt i = 0; i < 20; i++ )
        {
        status = KRequestPending;
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        // and wait until the request is complete
        User::WaitForRequest( status );
        
        User::LeaveIfError( status.Int() );

        if( status.Int() == KPositionQualityLoss )
            User::Leave(KErrTotalLossOfPrecision );
        
        // check that the timestamp is new (no max age in use)
        basic.GetPosition( pos );

        thisFixGetTime.UniversalTime();
        
        timeOfTheFix = pos.Time();
        
        
        PrintPositionInformationForThreadL( basic );
        
        fixTime = thisFixGetTime.MicroSecondsFrom( timeOfTheFix ).Int64();
        
        if( i != 0 && fixTime > 1000000 )
            User::Leave( KErrTooBig );
        
        if( previousFixGetTime == timeOfTheFix )
            User::Leave( KErrDivideByZero );
        
        previousFixGetTime = timeOfTheFix;
        }
    CleanupStack::PopAndDestroy(2, &posServer); // posServer, BTGpsPsy
    }

