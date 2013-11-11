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
 @file TDBT3Step.cpp
 @internalTechnology
*/
#include "TDBT3Step.h"

#include <ecom\ecom.h>

#include "tdbt3mposstatusobserver.h"
#include "tdbt3mposparameterobserver.h"



CTDBT3Step::~CTDBT3Step()
/**
 * Destructor
 */
	{
	}

CTDBT3Step::CTDBT3Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KTDBT3Step);
	}

TVerdict CTDBT3Step::doTestStepPreambleL()
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


TVerdict CTDBT3Step::doTestStepL()
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
		if(!GetIntFromConfig(ConfigSection(),KTe_LbsBtGpsPsySuiteInt,test)
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



TVerdict CTDBT3Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}


// ================= MEMBER FUNCTIONS =======================

// TYPE DEFINITIONS
struct TPositionerConstructParams
    {
    TUid iImplementationUid;
    MPosStatusObserver* iStatusObserver;
    MPosParameterObserver* iParamObserver;
    };


// ---------------------------------------------------------
// CTDBT3Step::StartL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CTDBT3Step::StartL(TInt aIndex)
    {
    switch(aIndex)
        {
        case 0:
        	_LIT( KIndexName0, "**** TCPM2001 - Out of memory and heap consumption ****" );
            INFO_PRINTF1(KIndexName0);
        	Sub0TCPM2001OutOfMemoryL();
            break;
        case 1:
        	_LIT( KIndexName1, "**** TCPM1002 - Fast open/close sequencing ****" );
        	INFO_PRINTF1(KIndexName1);
        	Sub1TCPM1002FastOpenCloseL();
            break;
        case 2:
        	_LIT( KIndexName2, "**** TCPM1004 - Change system time vs. max age ****" );
        	INFO_PRINTF1(KIndexName2);
        	Sub2TCPM1004SystemTimeChangeL();
            break;
        case 3:
        	_LIT( KIndexName3, "**** TCPM1003 - Five clients parallel requesting ****" );
        	INFO_PRINTF1(KIndexName3);
        	Sub3TCPM1003FiveParallelSessionsL();
            break;
        case 4:
        	_LIT( KIndexName4, "**** TCPM1001 - Timeout performance ****" );
        	INFO_PRINTF1(KIndexName4);
        	Sub4TCPM1001TimeoutPerformanceL();
            break;
        case 5:
        	_LIT( KIndexName5, "**** TCS1001 - Change of position when power off ****" );
        	INFO_PRINTF1(KIndexName5);
        	Sub5TCS1001PositionJumpingL();
            break;
        case 6:
        	_LIT( KIndexName6, "**** TCS1002 - Crossing zero points ****" );
        	INFO_PRINTF1(KIndexName6);
        	Sub6TCS1002CrossingZeroPointsL();
            break;
        case 7:
        	_LIT( KIndexName7, "**** TCS2001 - Satellite data checking ****" );
        	INFO_PRINTF1(KIndexName7);
        	Sub7TCS2001SatelliteDataL();
            break;
        case 8:

#if SERVICE_TESTS
        	_LIT( KIndexName8, "**** TCS2002 - Service Db Oom testing ****" );
        	INFO_PRINTF1(KIndexName8);
        	TestServiceDbOomL();
#endif
            break;
        }    
    }


void CTDBT3Step::Sub0TCPM2001OutOfMemoryL()
    {
    TInt err = KErrNone;
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    //INFO_PRINTF1( _L( "Set global privacy to accept all without notifications" ) );
    //common->SetupGlobalPrivacyL( TPosPrivacy::EAcceptAll, EFalse );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    TRAP( err, RunSubCase0L( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    //User::After( 10*1000*1000 );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYTest, "BT GPS PSY out of memory test failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYTest, err );
	_LIT( KIndexName0, "Ended: **** TCPM2001 - Out of memory and heap consumption ****" );
    INFO_PRINTF1(KIndexName0);
    }

void CTDBT3Step::RunSubCase0L( CLcfCommonProcedures* aCommon )
    {
    
#ifdef __WINS__
    TcOomInPsyLoadL( KbtGPSPsyId );
    TcOomWhenRequestingLocationL(KbtGPSPsyId, aCommon);
#else 
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    TcHeapUsageL( KbtGPSPsyId, common );
    CleanupStack::PopAndDestroy(common);

#endif
    
    }

void CTDBT3Step::Sub1TCPM1002FastOpenCloseL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    //INFO_PRINTF1( _L( "Set global privacy to accept all without notifications" ) );
    //common->SetupGlobalPrivacyL( TPosPrivacy::EAcceptAll, EFalse );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    TRAP( err, RunSubCase1L( common, KbtGPSPsyId ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYTest, "BT GPS PSY fast open/close test failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYTest, err );
	_LIT( KIndexName1, "Ended: **** TCPM1002 - Fast open/close sequencing ****" );
	INFO_PRINTF1(KIndexName1);
    }

void CTDBT3Step::RunSubCase1L( CLcfCommonProcedures* aCommon, const TUid aPsyUid )
    {
    TPositionerConstructParams params;
    
    Tt3MPosParameterObserver paramObserver;
    Tt3MPosStatusObserver statusObserver;
    
    params.iImplementationUid = aPsyUid;
    params.iParamObserver = &paramObserver;
    params.iStatusObserver = &statusObserver;
    
    CPositioner* positioner = NULL;
    
    TInt err = KErrNone;
    TInt err1 = KErrNone;
    TInt err2 = KErrNone;
    _LIT(KCPosNoMLFWTxt, "Try to create and destroy CPositioner instance without using MLFW");
    INFO_PRINTF1(KCPosNoMLFWTxt);
    
    TRAP(err, positioner = CPositioner::NewL(&params));
    delete positioner;
    
    AssertTrueSecL( err == KErrNone, KOpenFailedTxt, err );
    
    TRequestStatus status = KRequestPending;
    TPositionInfo posInfo;
    _LIT(KCreateReqCancelWithoutMLFWTxt, "Try to create, request position, cancel request and destroy CPositioner instance without using MLFW");
    INFO_PRINTF1(KCreateReqCancelWithoutMLFWTxt);
    
    TRAP(err, positioner = CPositioner::NewL(&params));
    TRAP(err1, positioner->NotifyPositionUpdate(posInfo, status));
    TRAP(err2, positioner->CancelNotifyPositionUpdate());
    aCommon->AsyncWait(status);
    delete positioner;
    
    AssertTrueSecL( err == KErrNone, KOpenFailedTxt, err );
    AssertTrueSecL( err1 == KErrNone, KRequestFailedTxt, err1 );
    _LIT(KCancelFailedTxt, "Cancel failed, got %d ");
    AssertTrueSecL( err2 == KErrNone, KCancelFailedTxt, err2 );
    _LIT(KReqCancelFailed, "Request canceling failed, got %d ");
    AssertTrueSecL( status.Int() == KErrCancel, KReqCancelFailed, status.Int() );
    
    status = KRequestPending;
    _LIT(KWait10STxt, "Wait 10s");
    INFO_PRINTF1(KWait10STxt);
    User::After(1*1000*1000);
    
    _LIT(KNoMLFWTestTxt, "Try to create, request position, wait request and destroy CPositioner instance without using MLFW");
    INFO_PRINTF1(KNoMLFWTestTxt);
    TRAP(err, positioner = CPositioner::NewL(&params));
    TRAP(err1, positioner->NotifyPositionUpdate(posInfo, status));
    aCommon->AsyncWait(status);
    delete positioner;
    
    PrintPositionInformationL( posInfo );
    
    _LIT(KOpenOrRequestFailedTxt, "Open or request failed, got %d ");
    AssertTrueSecL( err == KErrNone, KOpenOrRequestFailedTxt, err );
    AssertTrueSecL( err1 == KErrNone, KRequestFailedTxt, err1 );
    AssertTrueSecL( status.Int() == KErrNone, KRequestFailedTxt, status.Int() );
    
    _LIT(KTestEndedTxt, "Test ended");
    INFO_PRINTF1(KTestEndedTxt);
    
    }

void CTDBT3Step::Sub2TCPM1004SystemTimeChangeL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    //INFO_PRINTF1( _L( "Set global privacy to accept all without notifications" ) );
    //common->SetupGlobalPrivacyL( TPosPrivacy::EAcceptAll, EFalse );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    TRAP( err, RunSubCase2L( common, KbtGPSPsyId ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );

    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStartTxt);
    
    CleanupStack::PopAndDestroy(common); 
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYTest, "BT GPS PSY system time change test failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYTest, err );
	_LIT( KIndexName2, "Ended: **** TCPM1004 - Change system time vs. max age ****" );
	INFO_PRINTF1(KIndexName2);
    }

void CTDBT3Step::RunSubCase2L( CLcfCommonProcedures* aCommon, const TUid aPsyUid )
    {
    TInt err = KErrNone;
    TRequestStatus status = KRequestPending;
    TPositionInfo basic;
    
    RPositionServer posServer;
    
    INFO_PRINTF1(KOpenMLFWTxt);
    err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    RPositioner BTGpsPsy;
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer, aPsyUid );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, _L("tdBT3") );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();
    
    INFO_PRINTF1(KMakePositionRequestTxt);
    
    status = KRequestPending;
    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( basic, status );
    
    // and wait until the request is complete        
    User::WaitForRequest(status);
    
    aCommon->FlushEventsL();
    // if request failed
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    
    PrintPositionInformationL( basic );
    
    _LIT(KChangeSysTimeTxt, "Change system time");
    INFO_PRINTF1(KChangeSysTimeTxt);
    TPosition position;
    TTime fixTime;
    basic.GetPosition( position );
    fixTime = position.Time();
    
    fixTime -= TTimeIntervalMicroSeconds(25*1000*1000);
    
    err = User::SetHomeTime( fixTime );
    _LIT(KSetSysTimeFailedTxt, "Setting the system time failed, code %d" );
    AssertTrueSecL( err == KErrNone, KSetSysTimeFailedTxt, err );
    
    // get the fix time to the original
    fixTime = position.Time();
    
    INFO_PRINTF1( _L( "Set periodic time interval for position updates" ));
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(5*1000*1000) );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, KSettingUpdateFailedTxt, err );
    
    INFO_PRINTF1(KMakePositionRequestTxt);
    
    status = KRequestPending;
    TPositionInfo basic2;
    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( basic2, status );
    
    // and wait until the request is complete        
    User::WaitForRequest(status);
    
    aCommon->FlushEventsL();
    // if request failed
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    
    PrintPositionInformationL( basic2 );
    
    basic2.GetPosition( position );
    
    AssertTrueSecL( fixTime != position.Time(), _L( "The position info is the same" ), err );
    
    CleanupStack::PopAndDestroy(2, &posServer); // posServer, BTGpsPsy
    }

void CTDBT3Step::Sub3TCPM1003FiveParallelSessionsL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    //INFO_PRINTF1( _L( "Set global privacy to accept all without notifications" ) );
    //common->SetupGlobalPrivacyL( TPosPrivacy::EAcceptAll, EFalse );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    TRAP( err, RunSubCase3L( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );

    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYTest, "BT GPS PSY five parallel sessions test failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYTest, err );
	_LIT( KIndexName3, "Ended: **** TCPM1003 - Five clients parallel requesting ****" );
	INFO_PRINTF1(KIndexName3);
    }

void CTDBT3Step::RunSubCase3L( CLcfCommonProcedures* aCommon )
    {

#define SECOND_THREAD
#define THIRD_THREAD
#define FOURTH_THREAD
#define FIFTH_THREAD

    // Workaround for winscw 
#ifdef __WINSCW__
    RPositionServer posServer;
    
    TInt err = posServer.Connect();
    User::LeaveIfError(err);
    CleanupClosePushL(posServer);
#endif
    
    //iUserInfo->ShowDialog( _L( "The test ready to start, press OK to continue" ), EUtfwDialogTypeOk, EFalse );
    Sub3BasicInfoRequestingL( aCommon );
    User::After( 5*1000*1000 );
    //iUserInfo->ShowDialog( _L( "The first phase done, press OK to continue" ), EUtfwDialogTypeOk, EFalse );
    Sub3SatelliteInfoRequestingL( aCommon );
    User::After( 5*1000*1000 );
    //iUserInfo->ShowDialog( _L( "The second phase done, press OK to continue" ), EUtfwDialogTypeOk, EFalse );
    Sub3SatelliteInfoRequestingPrintBasicInfoL( aCommon );
    User::After( 5*1000*1000 );
    //iUserInfo->ShowDialog( _L( "The third phase done, press OK to continue" ), EUtfwDialogTypeOk, EFalse );
    Sub3SatelliteInfoRequestingNoPrintingL( aCommon );
    //iUserInfo->ShowDialog( _L( "The test finished, press OK to continue" ), EUtfwDialogTypeOk, EFalse );
#ifdef __WINSCW__
    CleanupStack::PopAndDestroy(&posServer);
#endif
    }

void CTDBT3Step::Sub3BasicInfoRequestingL( CLcfCommonProcedures* aCommon )
    {

    TInt err = KErrNone;
    TBuf<100> message;
    TInt threadNumber1 = 1;


#ifdef SECOND_THREAD
    TInt threadNumber2 = threadNumber1 + 1;
#endif

#ifdef THIRD_THREAD
    TInt threadNumber3 = threadNumber2 + 1;
#endif

#ifdef FOURTH_THREAD
    TInt threadNumber4 = threadNumber3 + 1;
#endif

#ifdef FIFTH_THREAD
    TInt threadNumber5 = threadNumber4 + 1;
#endif

    TInt *pointer1 = &threadNumber1;

#ifdef SECOND_THREAD
    TInt *pointer2 = &threadNumber2;
#endif

#ifdef THIRD_THREAD
    TInt *pointer3 = &threadNumber3;
#endif

#ifdef FOURTH_THREAD
    TInt *pointer4 = &threadNumber4;
#endif

#ifdef FIFTH_THREAD
    TInt *pointer5 = &threadNumber5;
#endif
    
    INFO_PRINTF1( KStartThreadTenUpdates);
    RThread thread1;

#ifdef SECOND_THREAD
    RThread thread2;
#endif

#ifdef THIRD_THREAD
    RThread thread3;
#endif

#ifdef FOURTH_THREAD
    RThread thread4;
#endif

#ifdef FIFTH_THREAD
    RThread thread5;
#endif
    
    TRequestStatus threadStatus1;
    
#ifdef SECOND_THREAD
    TRequestStatus threadStatus2;
#endif

#ifdef THIRD_THREAD
    TRequestStatus threadStatus3;
#endif

#ifdef FOURTH_THREAD
    TRequestStatus threadStatus4;
#endif

#ifdef FIFTH_THREAD
    TRequestStatus threadStatus5;
#endif
    
    aCommon->FlushEventsL();

    INFO_PRINTF1( KDisableEmulatorPanic);
    User::SetJustInTime(EFalse);
    
    INFO_PRINTF1( _L( "Create the thread1" ));
    err = thread1.Create( _L( "BTGPSTestThread1" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer1 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 1 with code %d" ), err );
    CleanupClosePushL( thread1 );
    
#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Create the thread2" ));
    err = thread2.Create( _L( "BTGPSTestThread2" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer2 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 2 with code %d" ), err );
    CleanupClosePushL( thread2 );
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Create the thread3" ));
    err = thread3.Create( _L( "BTGPSTestThread3" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer3 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 3 with code %d" ), err );
    CleanupClosePushL( thread3 );
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Create the thread4" ));
    err = thread4.Create( _L( "BTGPSTestThread4" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer4 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 4 with code %d" ), err );
    CleanupClosePushL( thread4 );
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Create the thread5" ));
    err = thread5.Create( _L( "BTGPSTestThread5" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer5 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 5 with code %d" ), err );
    CleanupClosePushL( thread5 );
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 1" ));
    thread1.Logon( threadStatus1 );

#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 2" ));
    thread2.Logon( threadStatus2 );
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 3" ));
    thread3.Logon( threadStatus3 );
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 4" ));
    thread4.Logon( threadStatus4 );
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 5" ));
    thread5.Logon( threadStatus5 );
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Start the thread 1" ));
    thread1.Resume();
    
#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Start the thread 2" ));
    thread2.Resume();
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Start the thread 3" ));
    thread3.Resume();
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Start the thread 4" ));
    thread4.Resume();
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Start the thread 5" ));
    thread5.Resume();
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Wait for the thread 1 to complete" ));
    

    while( threadStatus1 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }

    User::WaitForRequest(threadStatus1);

    aCommon->FlushEventsL();

#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 2 to complete" ));
    
    while( threadStatus2 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }
    
    User::WaitForRequest(threadStatus2);
    
    aCommon->FlushEventsL();
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 3 to complete" ));
    
    while( threadStatus3 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }

    User::WaitForRequest(threadStatus3);
    
    aCommon->FlushEventsL();
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 4 to complete" ));
    
    while( threadStatus4 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }

    User::WaitForRequest(threadStatus4);
    
    aCommon->FlushEventsL();
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 5 to complete" ));
    
    while( threadStatus5 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }
    User::WaitForRequest(threadStatus5);

    aCommon->FlushEventsL();
#endif

    message.Format( _L( "Thread 1 exit status: %d" ), threadStatus1.Int() );
    INFO_PRINTF1(message );

#ifdef SECOND_THREAD
    message.Format( _L( "Thread 2 exit status: %d" ), threadStatus2.Int() );
    INFO_PRINTF1(message );
#endif

#ifdef THIRD_THREAD
    message.Format( _L( "Thread 3 exit status: %d" ), threadStatus3.Int() );
    INFO_PRINTF1(message );
#endif

#ifdef FOURTH_THREAD    
    message.Format( _L( "Thread 4 exit status: %d" ), threadStatus4.Int() );
    INFO_PRINTF1(message );
#endif

#ifdef FIFTH_THREAD
    message.Format( _L( "Thread 5 exit status: %d" ), threadStatus5.Int() );
    INFO_PRINTF1(message );
#endif

    aCommon->FlushEventsL();

    message.Format( _L( "Thread 1 exit type: %d, reason: %d" ), thread1.ExitType(), thread1.ExitReason() );
    INFO_PRINTF1(message );

#ifdef SECOND_THREAD
    message.Format( _L( "Thread 2 exit type: %d, reason: %d" ), thread2.ExitType(), thread2.ExitReason() );
    INFO_PRINTF1(message );
#endif

#ifdef THIRD_THREAD
    message.Format( _L( "Thread 3 exit type: %d, reason: %d" ), thread3.ExitType(), thread3.ExitReason() );
    INFO_PRINTF1(message );
#endif

#ifdef FOURTH_THREAD
    message.Format( _L( "Thread 4 exit type: %d, reason: %d" ), thread4.ExitType(), thread4.ExitReason() );
    INFO_PRINTF1(message );
#endif

#ifdef FIFTH_THREAD
    message.Format( _L( "Thread 5 exit type: %d, reason: %d" ), thread5.ExitType(), thread5.ExitReason() );
    INFO_PRINTF1(message );
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Check thread 1 exit values" ));
    AssertTrueSecL( thread1.ExitType() == EExitKill, _L( "Thread 1 exit type wrong: %d, wanted: 0" ), thread1.ExitType() );
    AssertTrueSecL( thread1.ExitReason() == 0, _L( "Thread 1 exit reason wrong: %d, wanted: 0" ), thread1.ExitReason() );
    
#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Check thread 2 exit values" ));
    AssertTrueSecL( thread2.ExitType() == EExitKill, _L( "Thread 2 exit type wrong: %d, wanted: 0" ), thread2.ExitType() );
    AssertTrueSecL( thread2.ExitReason() == 0, _L( "Thread 2 exit reason wrong: %d, wanted: 0" ), thread2.ExitReason() );
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Check thread 3 exit values" ));
    AssertTrueSecL( thread3.ExitType() == EExitKill, _L( "Thread exit type wrong: %d, wanted: 0" ), thread3.ExitType() );
    AssertTrueSecL( thread3.ExitReason() == 0, _L( "Thread 3 exit reason wrong: %d, wanted: 0" ), thread3.ExitReason() );
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Check thread 4 exit values" ));
    AssertTrueSecL( thread4.ExitType() == EExitKill, _L( "Thread 4 exit type wrong: %d, wanted: 0" ), thread4.ExitType() );
    AssertTrueSecL( thread4.ExitReason() == 0, _L( "Thread 4 exit reason wrong: %d, wanted: 0" ), thread4.ExitReason() );
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Check thread 5 exit values" ));
    AssertTrueSecL( thread5.ExitType() == EExitKill, _L( "Thread 5 exit type wrong: %d, wanted: 0" ), thread5.ExitType() );
    AssertTrueSecL( thread5.ExitReason() == 0, _L( "Thread 5 exit reason wrong: %d, wanted: 0" ), thread5.ExitReason() );
#endif

    aCommon->FlushEventsL();

    CleanupStack::PopAndDestroy(); // thread x 1

#ifdef SECOND_THREAD
    CleanupStack::PopAndDestroy(); // thread x 2
#endif

#ifdef THIRD_THREAD
    CleanupStack::PopAndDestroy(); // thread x 3
#endif

#ifdef FOURTH_THREAD
    CleanupStack::PopAndDestroy(); // thread x 4
#endif

#ifdef FIFTH_THREAD
    CleanupStack::PopAndDestroy(); // thread x 5
#endif
    
    User::SetJustInTime(ETrue);

    }

    
void CTDBT3Step::Sub3SatelliteInfoRequestingL( CLcfCommonProcedures* aCommon )
    {
    
    TInt err = KErrNone;
    TBuf<100> message;
    TInt threadNumber6 = 6;


#ifdef SECOND_THREAD
    TInt threadNumber7 = threadNumber6 + 1;
#endif

#ifdef THIRD_THREAD
    TInt threadNumber8 = threadNumber7 + 1;
#endif

#ifdef FOURTH_THREAD
    TInt threadNumber9 = threadNumber8 + 1;
#endif

#ifdef FIFTH_THREAD
    TInt threadNumber10 = threadNumber9 + 1;
#endif

    TInt *pointer6 = &threadNumber6;

#ifdef SECOND_THREAD
    TInt *pointer7 = &threadNumber7;
#endif

#ifdef THIRD_THREAD
    TInt *pointer8 = &threadNumber8;
#endif

#ifdef FOURTH_THREAD
    TInt *pointer9 = &threadNumber9;
#endif

#ifdef FIFTH_THREAD
    TInt *pointer10 = &threadNumber10;
#endif
    
    INFO_PRINTF1( _L( "Start the a thread which makes ten periodic position updates" ));
    RThread thread6;

#ifdef SECOND_THREAD
    RThread thread7;
#endif

#ifdef THIRD_THREAD
    RThread thread8;
#endif

#ifdef FOURTH_THREAD
    RThread thread9;
#endif

#ifdef FIFTH_THREAD
    RThread thread10;
#endif
    
    TRequestStatus threadStatus6;
    
#ifdef SECOND_THREAD
    TRequestStatus threadStatus7;
#endif

#ifdef THIRD_THREAD
    TRequestStatus threadStatus8;
#endif

#ifdef FOURTH_THREAD
    TRequestStatus threadStatus9;
#endif

#ifdef FIFTH_THREAD
    TRequestStatus threadStatus10;
#endif
    
    aCommon->FlushEventsL();

    INFO_PRINTF1( _L( "Disable that panic crashes the emulator" ));
    User::SetJustInTime(EFalse);
    
    INFO_PRINTF1( _L( "Create the thread6" ));
    err = thread6.Create( _L( "BTGPSTestThread6" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer6 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 6 with code %d" ), err );
    CleanupClosePushL( thread6 );
    
#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Create the thread7" ));
    err = thread7.Create( _L( "BTGPSTestThread7" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer7 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 7 with code %d" ), err );
    CleanupClosePushL( thread7 );
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Create the thread8" ));
    err = thread8.Create( _L( "BTGPSTestThread8" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer8 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 8 with code %d" ), err );
    CleanupClosePushL( thread8 );
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Create the thread9" ));
    err = thread9.Create( _L( "BTGPSTestThread9" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer9 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 9 with code %d" ), err );
    CleanupClosePushL( thread9 );
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Create the thread10" ));
    err = thread10.Create( _L( "BTGPSTestThread10" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer10 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 10 with code %d" ), err );
    CleanupClosePushL( thread10 );
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 6" ));
    thread6.Logon( threadStatus6 );

#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 7" ));
    thread7.Logon( threadStatus7 );
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 8" ));
    thread8.Logon( threadStatus8 );
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 9" ));
    thread9.Logon( threadStatus9 );
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 10" ));
    thread10.Logon( threadStatus10 );
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Start the thread 6" ));
    thread6.Resume();
    
#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Start the thread 7" ));
    thread7.Resume();
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Start the thread 8" ));
    thread8.Resume();
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Start the thread 9" ));
    thread9.Resume();
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Start the thread 10" ));
    thread10.Resume();
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Wait for the thread 6 to complete" ));
    

    while( threadStatus6 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }

    User::WaitForRequest(threadStatus6);

    aCommon->FlushEventsL();

#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 7 to complete" ));
    
    while( threadStatus7 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }
    
    User::WaitForRequest(threadStatus7);
    
    aCommon->FlushEventsL();
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 8 to complete" ));
    
    while( threadStatus8 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }

    User::WaitForRequest(threadStatus8);
    
    aCommon->FlushEventsL();
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 9 to complete" ));
    
    while( threadStatus9 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }

    User::WaitForRequest(threadStatus9);
    
    aCommon->FlushEventsL();
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 10 to complete" ));
    
    while( threadStatus10 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }
    User::WaitForRequest(threadStatus10);

    aCommon->FlushEventsL();
#endif

    message.Format( _L( "Thread 6 exit status: %d" ), threadStatus6.Int() );
    INFO_PRINTF1(message );

#ifdef SECOND_THREAD
    message.Format( _L( "Thread 7 exit status: %d" ), threadStatus7.Int() );
    INFO_PRINTF1(message );
#endif

#ifdef THIRD_THREAD
    message.Format( _L( "Thread 8 exit status: %d" ), threadStatus8.Int() );
    INFO_PRINTF1(message );
#endif

#ifdef FOURTH_THREAD    
    message.Format( _L( "Thread 9 exit status: %d" ), threadStatus9.Int() );
    INFO_PRINTF1(message );
#endif

#ifdef FIFTH_THREAD
    message.Format( _L( "Thread 10 exit status: %d" ), threadStatus10.Int() );
    INFO_PRINTF1(message );
#endif

    aCommon->FlushEventsL();

    message.Format( _L( "Thread 6 exit type: %d, reason: %d" ), thread6.ExitType(), thread6.ExitReason() );
    INFO_PRINTF1(message );

#ifdef SECOND_THREAD
    message.Format( _L( "Thread 7 exit type: %d, reason: %d" ), thread7.ExitType(), thread7.ExitReason() );
    INFO_PRINTF1(message );
#endif

#ifdef THIRD_THREAD
    message.Format( _L( "Thread 8 exit type: %d, reason: %d" ), thread8.ExitType(), thread8.ExitReason() );
    INFO_PRINTF1(message );
#endif

#ifdef FOURTH_THREAD
    message.Format( _L( "Thread 9 exit type: %d, reason: %d" ), thread9.ExitType(), thread9.ExitReason() );
    INFO_PRINTF1(message );
#endif

#ifdef FIFTH_THREAD
    message.Format( _L( "Thread 10 exit type: %d, reason: %d" ), thread10.ExitType(), thread10.ExitReason() );
    INFO_PRINTF1(message );
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Check thread 6 exit values" ));
    AssertTrueSecL( thread6.ExitType() == EExitKill, _L( "Thread 6 exit type wrong: %d, wanted: 0" ), thread6.ExitType() );
    AssertTrueSecL( thread6.ExitReason() == 0, _L( "Thread 6 exit reason wrong: %d, wanted: 0" ), thread6.ExitReason() );
    
#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Check thread 7 exit values" ));
    AssertTrueSecL( thread7.ExitType() == EExitKill, _L( "Thread 7 exit type wrong: %d, wanted: 0" ), thread7.ExitType() );
    AssertTrueSecL( thread7.ExitReason() == 0, _L( "Thread 7 exit reason wrong: %d, wanted: 0" ), thread7.ExitReason() );
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Check thread 8 exit values" ));
    AssertTrueSecL( thread8.ExitType() == EExitKill, _L( "Thread exit type wrong: %d, wanted: 0" ), thread8.ExitType() );
    AssertTrueSecL( thread8.ExitReason() == 0, _L( "Thread 8 exit reason wrong: %d, wanted: 0" ), thread8.ExitReason() );
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Check thread 9 exit values" ));
    AssertTrueSecL( thread9.ExitType() == EExitKill, _L( "Thread 9 exit type wrong: %d, wanted: 0" ), thread9.ExitType() );
    AssertTrueSecL( thread9.ExitReason() == 0, _L( "Thread 9 exit reason wrong: %d, wanted: 0" ), thread9.ExitReason() );
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Check thread 10 exit values" ));
    AssertTrueSecL( thread10.ExitType() == EExitKill, _L( "Thread 10 exit type wrong: %d, wanted: 0" ), thread10.ExitType() );
    AssertTrueSecL( thread10.ExitReason() == 0, _L( "Thread 10 exit reason wrong: %d, wanted: 0" ), thread10.ExitReason() );
#endif

    aCommon->FlushEventsL();

    CleanupStack::PopAndDestroy(); // thread x 1

#ifdef SECOND_THREAD
    CleanupStack::PopAndDestroy(); // thread x 2
#endif

#ifdef THIRD_THREAD
    CleanupStack::PopAndDestroy(); // thread x 3
#endif

#ifdef FOURTH_THREAD
    CleanupStack::PopAndDestroy(); // thread x 4
#endif

#ifdef FIFTH_THREAD
    CleanupStack::PopAndDestroy(); // thread x 5
#endif
    
    User::SetJustInTime(ETrue);
    }
    
void CTDBT3Step::Sub3SatelliteInfoRequestingPrintBasicInfoL( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TBuf<100> message;
    TInt threadNumber11 = 11;


#ifdef SECOND_THREAD
    TInt threadNumber12 = threadNumber11 + 1;
#endif

#ifdef THIRD_THREAD
    TInt threadNumber13 = threadNumber12 + 1;
#endif

#ifdef FOURTH_THREAD
    TInt threadNumber14 = threadNumber13 + 1;
#endif

#ifdef FIFTH_THREAD
    TInt threadNumber15 = threadNumber14 + 1;
#endif

    TInt *pointer11 = &threadNumber11;

#ifdef SECOND_THREAD
    TInt *pointer12 = &threadNumber12;
#endif

#ifdef THIRD_THREAD
    TInt *pointer13 = &threadNumber13;
#endif

#ifdef FOURTH_THREAD
    TInt *pointer14 = &threadNumber14;
#endif

#ifdef FIFTH_THREAD
    TInt *pointer15 = &threadNumber15;
#endif
    
    INFO_PRINTF1( KStartThreadTenUpdates);
    RThread thread11;

#ifdef SECOND_THREAD
    RThread thread12;
#endif

#ifdef THIRD_THREAD
    RThread thread13;
#endif

#ifdef FOURTH_THREAD
    RThread thread14;
#endif

#ifdef FIFTH_THREAD
    RThread thread15;
#endif
    
    TRequestStatus threadStatus11;
    
#ifdef SECOND_THREAD
    TRequestStatus threadStatus12;
#endif

#ifdef THIRD_THREAD
    TRequestStatus threadStatus13;
#endif

#ifdef FOURTH_THREAD
    TRequestStatus threadStatus14;
#endif

#ifdef FIFTH_THREAD
    TRequestStatus threadStatus15;
#endif
    
    aCommon->FlushEventsL();

    INFO_PRINTF1( KDisableEmulatorPanic);
    User::SetJustInTime(EFalse);
    
    INFO_PRINTF1( _L( "Create the thread11" ));
    err = thread11.Create( _L( "BTGPSTestThread11" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer11 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 11 with code %d" ), err );
    CleanupClosePushL( thread11 );
    
#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Create the thread12" ));
    err = thread12.Create( _L( "BTGPSTestThread12" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer12 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 12 with code %d" ), err );
    CleanupClosePushL( thread12 );
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Create the thread13" ));
    err = thread13.Create( _L( "BTGPSTestThread13" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer13 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 13 with code %d" ), err );
    CleanupClosePushL( thread13 );
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Create the thread14" ));
    err = thread14.Create( _L( "BTGPSTestThread14" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer14 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 14 with code %d" ), err );
    CleanupClosePushL( thread14 );
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Create the thread15" ));
    err = thread15.Create( _L( "BTGPSTestThread15" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer15 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 15 with code %d" ), err );
    CleanupClosePushL( thread15 );
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 11" ));
    thread11.Logon( threadStatus11 );

#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 12" ));
    thread12.Logon( threadStatus12 );
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 13" ));
    thread13.Logon( threadStatus13 );
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 14" ));
    thread14.Logon( threadStatus14 );
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 15" ));
    thread15.Logon( threadStatus15 );
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Start the thread 11" ));
    thread11.Resume();
    
#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Start the thread 12" ));
    thread12.Resume();
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Start the thread 13" ));
    thread13.Resume();
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Start the thread 14" ));
    thread14.Resume();
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Start the thread 15" ));
    thread15.Resume();
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Wait for the thread 11 to complete" ));
    

    while( threadStatus11 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }

    User::WaitForRequest(threadStatus11);

    aCommon->FlushEventsL();

#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 12 to complete" ));
    
    while( threadStatus12 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }
    
    User::WaitForRequest(threadStatus12);
    
    aCommon->FlushEventsL();
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 13 to complete" ));
    
    while( threadStatus13 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }

    User::WaitForRequest(threadStatus13);
    
    aCommon->FlushEventsL();
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 14 to complete" ));
    
    while( threadStatus14 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }

    User::WaitForRequest(threadStatus14);
    
    aCommon->FlushEventsL();
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 15 to complete" ));
    
    while( threadStatus15 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }
    User::WaitForRequest(threadStatus15);

    aCommon->FlushEventsL();
#endif

    message.Format( _L( "Thread 11 exit status: %d" ), threadStatus11.Int() );
    INFO_PRINTF1(message );

#ifdef SECOND_THREAD
    message.Format( _L( "Thread 12 exit status: %d" ), threadStatus12.Int() );
    INFO_PRINTF1(message );
#endif

#ifdef THIRD_THREAD
    message.Format( _L( "Thread 13 exit status: %d" ), threadStatus13.Int() );
    INFO_PRINTF1(message );
#endif

#ifdef FOURTH_THREAD    
    message.Format( _L( "Thread 14 exit status: %d" ), threadStatus14.Int() );
    INFO_PRINTF1(message );
#endif

#ifdef FIFTH_THREAD
    message.Format( _L( "Thread 15 exit status: %d" ), threadStatus15.Int() );
    INFO_PRINTF1(message );
#endif

    aCommon->FlushEventsL();

    message.Format( _L( "Thread 11 exit type: %d, reason: %d" ), thread11.ExitType(), thread11.ExitReason() );
    INFO_PRINTF1(message );

#ifdef SECOND_THREAD
    message.Format( _L( "Thread 12 exit type: %d, reason: %d" ), thread12.ExitType(), thread12.ExitReason() );
    INFO_PRINTF1(message );
#endif

#ifdef THIRD_THREAD
    message.Format( _L( "Thread 13 exit type: %d, reason: %d" ), thread13.ExitType(), thread13.ExitReason() );
    INFO_PRINTF1(message );
#endif

#ifdef FOURTH_THREAD
    message.Format( _L( "Thread 14 exit type: %d, reason: %d" ), thread14.ExitType(), thread14.ExitReason() );
    INFO_PRINTF1(message );
#endif

#ifdef FIFTH_THREAD
    message.Format( _L( "Thread 15 exit type: %d, reason: %d" ), thread15.ExitType(), thread15.ExitReason() );
    INFO_PRINTF1(message );
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Check thread 11 exit values" ));
    AssertTrueSecL( thread11.ExitType() == EExitKill, _L( "Thread 11 exit type wrong: %d, wanted: 0" ), thread11.ExitType() );
    AssertTrueSecL( thread11.ExitReason() == 0, _L( "Thread 11 exit reason wrong: %d, wanted: 0" ), thread11.ExitReason() );
    
#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Check thread 12 exit values" ));
    AssertTrueSecL( thread12.ExitType() == EExitKill, _L( "Thread 12 exit type wrong: %d, wanted: 0" ), thread12.ExitType() );
    AssertTrueSecL( thread12.ExitReason() == 0, _L( "Thread 12 exit reason wrong: %d, wanted: 0" ), thread12.ExitReason() );
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Check thread 13 exit values" ));
    AssertTrueSecL( thread13.ExitType() == EExitKill, _L( "Thread exit type wrong: %d, wanted: 0" ), thread13.ExitType() );
    AssertTrueSecL( thread13.ExitReason() == 0, _L( "Thread 13 exit reason wrong: %d, wanted: 0" ), thread13.ExitReason() );
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Check thread 14 exit values" ));
    AssertTrueSecL( thread14.ExitType() == EExitKill, _L( "Thread 14 exit type wrong: %d, wanted: 0" ), thread14.ExitType() );
    AssertTrueSecL( thread14.ExitReason() == 0, _L( "Thread 14 exit reason wrong: %d, wanted: 0" ), thread14.ExitReason() );
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Check thread 15 exit values" ));
    AssertTrueSecL( thread15.ExitType() == EExitKill, _L( "Thread 15 exit type wrong: %d, wanted: 0" ), thread15.ExitType() );
    AssertTrueSecL( thread15.ExitReason() == 0, _L( "Thread 15 exit reason wrong: %d, wanted: 0" ), thread15.ExitReason() );
#endif

    aCommon->FlushEventsL();

    CleanupStack::PopAndDestroy(); // thread x 1

#ifdef SECOND_THREAD
    CleanupStack::PopAndDestroy(); // thread x 2
#endif

#ifdef THIRD_THREAD
    CleanupStack::PopAndDestroy(); // thread x 3
#endif

#ifdef FOURTH_THREAD
    CleanupStack::PopAndDestroy(); // thread x 4
#endif

#ifdef FIFTH_THREAD
    CleanupStack::PopAndDestroy(); // thread x 5
#endif
    
    User::SetJustInTime(ETrue);
    
    }

void CTDBT3Step::Sub3SatelliteInfoRequestingNoPrintingL( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TBuf<100> message;
    TInt threadNumber16 = 16;


#ifdef SECOND_THREAD
    TInt threadNumber17 = threadNumber16 + 1;
#endif

#ifdef THIRD_THREAD
    TInt threadNumber18 = threadNumber17 + 1;
#endif

#ifdef FOURTH_THREAD
    TInt threadNumber19 = threadNumber18 + 1;
#endif

#ifdef FIFTH_THREAD
    TInt threadNumber20 = threadNumber19 + 1;
#endif

    TInt *pointer16 = &threadNumber16;

#ifdef SECOND_THREAD
    TInt *pointer17 = &threadNumber17;
#endif

#ifdef THIRD_THREAD
    TInt *pointer18 = &threadNumber18;
#endif

#ifdef FOURTH_THREAD
    TInt *pointer19 = &threadNumber19;
#endif

#ifdef FIFTH_THREAD
    TInt *pointer20 = &threadNumber20;
#endif
    
    INFO_PRINTF1( _L( "Start the a thread which makes ten periodic position updates" ));
    RThread thread16;

#ifdef SECOND_THREAD
    RThread thread17;
#endif

#ifdef THIRD_THREAD
    RThread thread18;
#endif

#ifdef FOURTH_THREAD
    RThread thread19;
#endif

#ifdef FIFTH_THREAD
    RThread thread20;
#endif
    
    TRequestStatus threadStatus16;
    
#ifdef SECOND_THREAD
    TRequestStatus threadStatus17;
#endif

#ifdef THIRD_THREAD
    TRequestStatus threadStatus18;
#endif

#ifdef FOURTH_THREAD
    TRequestStatus threadStatus19;
#endif

#ifdef FIFTH_THREAD
    TRequestStatus threadStatus20;
#endif
    
    aCommon->FlushEventsL();

    INFO_PRINTF1( _L( "Disable that panic crashes the emulator" ));
    User::SetJustInTime(EFalse);
    
    INFO_PRINTF1( _L( "Create the thread16" ));
    err = thread16.Create( _L( "BTGPSTestThread16" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer16 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 16 with code %d" ), err );
    CleanupClosePushL( thread16 );
    
#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Create the thread17" ));
    err = thread17.Create( _L( "BTGPSTestThread17" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer17 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 17 with code %d" ), err );
    CleanupClosePushL( thread17 );
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Create the thread18" ));
    err = thread18.Create( _L( "BTGPSTestThread18" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer18 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 18 with code %d" ), err );
    CleanupClosePushL( thread18 );
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Create the thread19" ));
    err = thread19.Create( _L( "BTGPSTestThread19" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer19 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 19 with code %d" ), err );
    CleanupClosePushL( thread19 );
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Create the thread20" ));
    err = thread20.Create( _L( "BTGPSTestThread20" ), StartThreadL, KDefaultStackSize, 
        KMinHeapSize, KMinHeapSize+KHeapOffset, pointer20 );
    AssertTrueSecL( err == KErrNone, _L( "Error when creating thread 20 with code %d" ), err );
    CleanupClosePushL( thread20 );
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 16" ));
    thread16.Logon( threadStatus16 );

#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 17" ));
    thread17.Logon( threadStatus17 );
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 18" ));
    thread18.Logon( threadStatus18 );
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 19" ));
    thread19.Logon( threadStatus19 );
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Make the test driver to monitor the thread 20" ));
    thread20.Logon( threadStatus20 );
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Start the thread 16" ));
    thread16.Resume();
    
#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Start the thread 17" ));
    thread17.Resume();
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Start the thread 18" ));
    thread18.Resume();
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Start the thread 19" ));
    thread19.Resume();
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Start the thread 20" ));
    thread20.Resume();
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Wait for the thread 16 to complete" ));
    

    while( threadStatus16 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }

    User::WaitForRequest(threadStatus16);

    aCommon->FlushEventsL();

#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 17 to complete" ));
    
    while( threadStatus17 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }
    
    User::WaitForRequest(threadStatus17);
    
    aCommon->FlushEventsL();
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 18 to complete" ));
    
    while( threadStatus18 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }

    User::WaitForRequest(threadStatus18);
    
    aCommon->FlushEventsL();
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 19 to complete" ));
    
    while( threadStatus19 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }

    User::WaitForRequest(threadStatus19);
    
    aCommon->FlushEventsL();
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Wait for the thread 20 to complete" ));
    
    while( threadStatus20 == KRequestPending )
        {
        aCommon->FlushEventsL();
        User::ResetInactivityTime();
        User::After(5*1000*1000);
        User::ResetInactivityTime();
        }
    User::WaitForRequest(threadStatus20);

    aCommon->FlushEventsL();
#endif

    message.Format( _L( "Thread 16 exit status: %d" ), threadStatus16.Int() );
    INFO_PRINTF1(message );

#ifdef SECOND_THREAD
    message.Format( _L( "Thread 17 exit status: %d" ), threadStatus17.Int() );
    INFO_PRINTF1(message );
#endif

#ifdef THIRD_THREAD
    message.Format( _L( "Thread 18 exit status: %d" ), threadStatus18.Int() );
    INFO_PRINTF1(message );
#endif

#ifdef FOURTH_THREAD    
    message.Format( _L( "Thread 19 exit status: %d" ), threadStatus19.Int() );
    INFO_PRINTF1(message );
#endif

#ifdef FIFTH_THREAD
    message.Format( _L( "Thread 20 exit status: %d" ), threadStatus20.Int() );
    INFO_PRINTF1(message );
#endif

    aCommon->FlushEventsL();

    message.Format( _L( "Thread 16 exit type: %d, reason: %d" ), thread16.ExitType(), thread16.ExitReason() );
    INFO_PRINTF1(message );

#ifdef SECOND_THREAD
    message.Format( _L( "Thread 17 exit type: %d, reason: %d" ), thread17.ExitType(), thread17.ExitReason() );
    INFO_PRINTF1(message );
#endif

#ifdef THIRD_THREAD
    message.Format( _L( "Thread 18 exit type: %d, reason: %d" ), thread18.ExitType(), thread18.ExitReason() );
    INFO_PRINTF1(message );
#endif

#ifdef FOURTH_THREAD
    message.Format( _L( "Thread 19 exit type: %d, reason: %d" ), thread19.ExitType(), thread19.ExitReason() );
    INFO_PRINTF1(message );
#endif

#ifdef FIFTH_THREAD
    message.Format( _L( "Thread 20 exit type: %d, reason: %d" ), thread20.ExitType(), thread20.ExitReason() );
    INFO_PRINTF1(message );
#endif

    aCommon->FlushEventsL();
    
    INFO_PRINTF1( _L( "Check thread 16 exit values" ));
    AssertTrueSecL( thread16.ExitType() == EExitKill, _L( "Thread 16 exit type wrong: %d, wanted: 0" ), thread16.ExitType() );
    AssertTrueSecL( thread16.ExitReason() == 0, _L( "Thread 16 exit reason wrong: %d, wanted: 0" ), thread16.ExitReason() );
    
#ifdef SECOND_THREAD
    INFO_PRINTF1( _L( "Check thread 17 exit values" ));
    AssertTrueSecL( thread17.ExitType() == EExitKill, _L( "Thread 17 exit type wrong: %d, wanted: 0" ), thread17.ExitType() );
    AssertTrueSecL( thread17.ExitReason() == 0, _L( "Thread 17 exit reason wrong: %d, wanted: 0" ), thread17.ExitReason() );
#endif

#ifdef THIRD_THREAD
    INFO_PRINTF1( _L( "Check thread 18 exit values" ));
    AssertTrueSecL( thread18.ExitType() == EExitKill, _L( "Thread exit type wrong: %d, wanted: 0" ), thread18.ExitType() );
    AssertTrueSecL( thread18.ExitReason() == 0, _L( "Thread 18 exit reason wrong: %d, wanted: 0" ), thread18.ExitReason() );
#endif

#ifdef FOURTH_THREAD
    INFO_PRINTF1( _L( "Check thread 19 exit values" ));
    AssertTrueSecL( thread19.ExitType() == EExitKill, _L( "Thread 19 exit type wrong: %d, wanted: 0" ), thread19.ExitType() );
    AssertTrueSecL( thread19.ExitReason() == 0, _L( "Thread 19 exit reason wrong: %d, wanted: 0" ), thread19.ExitReason() );
#endif

#ifdef FIFTH_THREAD
    INFO_PRINTF1( _L( "Check thread 20 exit values" ));
    AssertTrueSecL( thread20.ExitType() == EExitKill, _L( "Thread 20 exit type wrong: %d, wanted: 0" ), thread20.ExitType() );
    AssertTrueSecL( thread20.ExitReason() == 0, _L( "Thread 20 exit reason wrong: %d, wanted: 0" ), thread20.ExitReason() );
#endif

    aCommon->FlushEventsL();

    CleanupStack::PopAndDestroy(); // thread x 1

#ifdef SECOND_THREAD
    CleanupStack::PopAndDestroy(); // thread x 2
#endif

#ifdef THIRD_THREAD
    CleanupStack::PopAndDestroy(); // thread x 3
#endif

#ifdef FOURTH_THREAD
    CleanupStack::PopAndDestroy(); // thread x 4
#endif

#ifdef FIFTH_THREAD
    CleanupStack::PopAndDestroy(); // thread x 5
#endif
    
    User::SetJustInTime(ETrue);
    
    }

void CTDBT3Step::Sub4TCPM1001TimeoutPerformanceL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    //INFO_PRINTF1( _L( "Set global privacy to accept all without notifications" ) );
    //common->SetupGlobalPrivacyL( TPosPrivacy::EAcceptAll, EFalse );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    TRAP( err, RunSubCase4L( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYTest, "BT GPS PSY timeout performance test failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYTest, err );
	_LIT( KIndexName4, "Ended: **** TCPM1001 - Timeout performance ****" );
	INFO_PRINTF1(KIndexName4);
    }

void CTDBT3Step::RunSubCase4L( CLcfCommonProcedures* aCommon )
    {
    TInt timeoutValues[4] = { 15*100*1000, 13*100*1000, 11*100*1000, 10*100*1000 };
    for( TInt i = 0; i < 4; i++ )
        {
        aCommon->FlushEventsL();
        Sub4TimeoutRequestsL(timeoutValues[i], aCommon );
        User::After( 5*1000*1000 );
        }
    }

void CTDBT3Step::Sub4TimeoutRequestsL( TInt aTimeOutValue, CLcfCommonProcedures* aCommon )
    {
    
    TInt err = KErrNone;
    TBuf<100> message;
    
    TRequestStatus status = KRequestPending;
    TPositionSatelliteInfo satellite;
    
    RPositionServer posServer;
    RPositioner BTGpsPsy;
    
    err = posServer.Connect();
    User::LeaveIfError(err);
    CleanupClosePushL(posServer);
    
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    User::LeaveIfError(err);
    CleanupClosePushL(BTGpsPsy);
    
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService , CRequestor::EFormatApplication , _L("tdBT3") );
    User::LeaveIfError(err);
    
    
    status = KRequestPending;
    
    // request positioninfo
    BTGpsPsy.NotifyPositionUpdate( satellite, status );
    
    // and wait until the request is complete
    User::WaitForRequest( status );
    
    aCommon->FlushEventsL();
    
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
    
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(aTimeOutValue) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetAcceptPartialUpdates( EFalse );
    
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, _L( "set update options failed with code %d" ), err );
    
    TInt timeOuted = 0;
    TInt qualityLosses = 0;
    
    for( TInt i = 0; i < 100; i++ )
        {
        status = KRequestPending;
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( satellite, status );
        
        // and wait until the request is complete
        User::WaitForRequest( status );
        
        aCommon->FlushEventsL();
        
        if( status.Int() == KErrTimedOut )
            timeOuted++;
        else if( status.Int() == KPositionQualityLoss )
            qualityLosses++;
        else
            User::LeaveIfError( status.Int() );
        }
    
    message.Format( _L( "Timed out requests %d" ), timeOuted );
    INFO_PRINTF1(message);
    
    message.Format( _L( "Timed out request percentage %f" ), timeOuted/100.0 );
    INFO_PRINTF1(message);
    
    message.Format( _L( "Quality lost requests %d" ), qualityLosses );
    INFO_PRINTF1(message);
    
    CleanupStack::PopAndDestroy(2); // server, psy
    
    }

void CTDBT3Step::Sub5TCS1001PositionJumpingL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    //INFO_PRINTF1( _L( "Set global privacy to accept all without notifications" ) );
    //common->SetupGlobalPrivacyL( TPosPrivacy::EAcceptAll, EFalse );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    TRAP( err, RunSubCase5L( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYTest, "BT GPS PSY position change while power off test failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYTest, err );
	_LIT( KIndexName5, "Ended: **** TCS1001 - Change of position when power off ****" );
	INFO_PRINTF1(KIndexName5);
    }

void CTDBT3Step::RunSubCase5L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TRequestStatus status = KRequestPending;
    TPositionSatelliteInfo basic;
    
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
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, _L("tdBT3") );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();

    INFO_PRINTF1( _L( "Set periodic time interval for position updates" ));
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetAcceptPartialUpdates( ETrue );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, KSettingUpdateFailedTxt, err );
    
    INFO_PRINTF1(KMakePositionRequestTxt);
    
    while( status.Int() != KErrNone )
        {
        status = KRequestPending;
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        // and wait until the request is complete        
        User::WaitForRequest(status);
        
        aCommon->FlushEventsL();
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        
        PrintPositionInformationL( basic );
        }

    //iUserInfo->ShowDialog( _L( "Shutdown the GPS and change the simulated position. After that, press OK" ), EUtfwDialogTypeOk, EFalse );

    while( status.Int() != KErrNone )
        {
        status = KRequestPending;
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        // and wait until the request is complete        
        User::WaitForRequest(status);
        
        aCommon->FlushEventsL();
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        
        PrintPositionInformationL( basic );
        }

    aCommon->FlushEventsL();

    INFO_PRINTF1( _L( "Set periodic time interval for position updates" ));
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetAcceptPartialUpdates( EFalse );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, KSettingUpdateFailedTxt, err );

    //iUserInfo->ShowDialog( _L( "Shutdown the GPS and change the simulated position again. After that, press OK" ), EUtfwDialogTypeOk, EFalse );

    while( status.Int() != KErrNone )
        {
        status = KRequestPending;
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        // and wait until the request is complete        
        User::WaitForRequest(status);
        
        aCommon->FlushEventsL();
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        
        PrintPositionInformationL( basic );
        }

    aCommon->FlushEventsL();

    CleanupStack::PopAndDestroy(2); // server, psy

    }

void CTDBT3Step::Sub6TCS1002CrossingZeroPointsL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    //INFO_PRINTF1( _L( "Set global privacy to accept all without notifications" ) );
    //common->SetupGlobalPrivacyL( TPosPrivacy::EAcceptAll, EFalse );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    TRAP( err, RunSubCase6L( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYTest, "BT GPS PSY crossing zero points test failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYTest, err );
	_LIT( KIndexName6, "Ended: **** TCS1002 - Crossing zero points ****" );
	INFO_PRINTF1(KIndexName6);
    }

void CTDBT3Step::RunSubCase6L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TRequestStatus status = KRequestPending;
    TPositionSatelliteInfo basic;
    
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
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, _L("tdBT3") );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();

    INFO_PRINTF1( _L( "Set periodic time interval for position updates" ));
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetAcceptPartialUpdates( EFalse );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, KSettingUpdateFailedTxt, err );
    
    INFO_PRINTF1(KMakePositionRequestTxt);
    
    TBool endReached = EFalse;

    TPosition position;
    TReal64 latitude = 0;
    TReal64 longitude = 0;
    TInt32 intLongitude = 0;
    TInt32 intLatitude = 0;

    while( !endReached )
        {
        status = KRequestPending;
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        // and wait until the request is complete        
        User::WaitForRequest(status);
        
        aCommon->FlushEventsL();
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        
        PrintPositionInformationL( basic );

        basic.GetPosition( position );
        latitude = position.Latitude();
        longitude = position.Longitude();

        Math::Int( intLongitude, longitude );
        Math::Int( intLatitude, latitude );

        if( intLongitude == 60 && intLongitude == 60 )
            {
            endReached = ETrue;
            }
        }

    CleanupStack::PopAndDestroy(2); //server, psy
    }

void CTDBT3Step::Sub7TCS2001SatelliteDataL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    //INFO_PRINTF1( _L( "Set global privacy to accept all without notifications" ) );
    //common->SetupGlobalPrivacyL( TPosPrivacy::EAcceptAll, EFalse );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    TRAP( err, RunSubCase7L( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYTest, "BT GPS PSY satellite data test failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYTest, err );
	_LIT( KIndexName7, "Ended: **** TCS2001 - Satellite data checking ****" );
	INFO_PRINTF1(KIndexName7);
    }

void CTDBT3Step::RunSubCase7L( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    TRequestStatus status = KRequestPending;
    TPositionSatelliteInfo basic;
    
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
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, _L("tdBT3") );
    AssertTrueSecL( err == KErrNone, KSettingRequestorFailedTxt, err );
    
    aCommon->FlushEventsL();

    INFO_PRINTF1( _L( "Set periodic time interval for position updates" ));
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetAcceptPartialUpdates( EFalse );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    AssertTrueSecL( err == KErrNone, KSettingUpdateFailedTxt, err );
    
    INFO_PRINTF1(KMakePositionRequestTxt);
    
    for( TInt i = 0; i < 10; i++ )
        {
        status = KRequestPending;
        
        // request positioninfo
        BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        // and wait until the request is complete        
        User::WaitForRequest(status);
        
        aCommon->FlushEventsL();
        // if request failed
        AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
        
        PrintPositionInformationL( basic );

        aCommon->FlushEventsL();
        }

    CleanupStack::PopAndDestroy(2); //server, psy
    }

void CTDBT3Step::MakeTenPeriodicUpdatesFromBTGPSPsyL( TInt forLogging )
    {
    TBuf<80> filePath;
    TBuf<80>message;
    TInt err = KErrNone;
    
    TRequestStatus status = KRequestPending;
    TPositionSatelliteInfo satellite;
    TPositionInfo basic;
    
    filePath.Format( KThreadNumberLogFile, forLogging );
    
    RPositionServer posServer;
    RPositioner BTGpsPsy;
    
    err = posServer.Connect();
    User::LeaveIfError(err);
    CleanupClosePushL(posServer);
    
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    User::LeaveIfError(err);
    CleanupClosePushL(BTGpsPsy);
    
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService , CRequestor::EFormatApplication , _L("tdBT3") );
    User::LeaveIfError(err);
    
    TPositionUpdateOptions btGPSOptions;
    btGPSOptions.SetUpdateInterval( TTimeIntervalMicroSeconds(1000000) );
    btGPSOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetMaxUpdateAge( TTimeIntervalMicroSeconds(0) );
    btGPSOptions.SetAcceptPartialUpdates( ETrue );
    
    err = BTGpsPsy.SetUpdateOptions( btGPSOptions );
    User::LeaveIfError(err);
    
    TInt partialQuality = 0;
    TInt qualityLosses = 0;
    TInt totalTime = 0;
    TTime previousFixGetTime(0);
    TTime thisFixGetTime(0);
    TTime otherTime(0);
    TInt lostTime = 0;
    TInt fixTime = 0;
    
    previousFixGetTime.UniversalTime();
    
    for( TInt i = 0; i < 100; i++ )
        {
        status = KRequestPending;
        
        //PrintForThreadL( _L( "Request position\r\n" ), filePath );
        
        otherTime.UniversalTime();
        lostTime += otherTime.MicroSecondsFrom( previousFixGetTime ).Int64();
        
        // request positioninfo
        if( forLogging > 5 )
            BTGpsPsy.NotifyPositionUpdate( satellite, status );
        else
            BTGpsPsy.NotifyPositionUpdate( basic, status );
        
        // and wait until the request is complete
        User::WaitForRequest( status );
        
        // check that the timestamp is new (no max age in use)
        thisFixGetTime.UniversalTime();
        
        message.Format( _L( "Request completion value: %d\r\n"), status.Int() );
        PrintForThreadL( message, filePath );
        User::LeaveIfError( status.Int() );
        
        if( status.Int() == KPositionQualityLoss )
            qualityLosses++;
        if( status.Int() == KPositionPartialUpdate )
            partialQuality++;
        
        if( forLogging < 6 )
            PrintPositionInformationForThreadL( basic, filePath );
        else if( forLogging < 11 )
            PrintPositionInformationForThreadL( satellite, filePath );
        else if( forLogging < 16 )
            PrintPositionInformationForThreadL( (TPositionInfo) satellite, filePath );
        
        fixTime = thisFixGetTime.MicroSecondsFrom( previousFixGetTime ).Int64();
        totalTime += fixTime;
        
        message.Format( _L( "%d\r\n" ), fixTime );
        PrintForThreadL( message, filePath );        
        
        previousFixGetTime = thisFixGetTime;
        
        }
    
    message.Format( _L( "Total runtime was %d microseconds\r\n" ), totalTime );
    PrintForThreadL( message, filePath );
    
    message.Format( _L( "Average fix time was %d microseconds\r\n" ), (totalTime/100) );
    PrintForThreadL( message, filePath );
    
    message.Format( _L( "Quality lost fixes %d\r\n"), qualityLosses );
    PrintForThreadL( message, filePath );
    
    message.Format( _L( "Partial fixes %d\r\n"), partialQuality );
    PrintForThreadL( message, filePath );
    
    message.Format( _L( "Lost time %d\r\n"), lostTime );
    PrintForThreadL( message, filePath );
    
    PrintForThreadL( _L( "Thread completed\r\n" ), filePath );
    
    CleanupStack::PopAndDestroy(2); // posServer, BTGpsPsy
    }
    
/*
void CTDBT3Step::PrintPositionInformationL( HPositionGenericInfo *aInfo, TPositionFieldId aId )
    {
    
    TInt err = KErrNone;
    
    TReal32 value = 0;
    TUint8 number = 0;
    TInt8 amount = 0;
    TTime time(0);
    TBuf<200> message;
    _LIT(KFieldExtractingTxt, "Field extracting...");
    INFO_PRINTF1(KFieldExtractingTxt);
    
    switch( aId )
        {
        
        case EPositionFieldHorizontalSpeed:
            {
            err = aInfo->GetValue( aId, value );
            AssertTrueSecL( err == KErrNone, KGetValueFailedTxt, err );
            message.Format( _L( "EPositionFieldHorizontalSpeed was %f" ), value );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldHeading:
            {
            err = aInfo->GetValue( aId, value );
            AssertTrueSecL( err == KErrNone, KGetValueFailedTxt, err );
            message.Format( _L( "EPositionFieldHeading was %f" ), value );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldNMEASentences:
            {
            err = aInfo->GetValue( aId, number );
            AssertTrueSecL( err == KErrNone, KGetValueFailedTxt, err );
            INFO_PRINTF1( _L( "Field printing" ) );
            message.Format( _L( "EPositionFieldNMEASentences was %d" ), number );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatelliteNumInView:
            {
            err = aInfo->GetValue( aId, amount );
            AssertTrueSecL( err == KErrNone, KGetValueFailedTxt, err );
            message.Format( _L( "EPositionFieldSatelliteNumInView was %d" ), amount );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatelliteNumUsed:
            {
            err = aInfo->GetValue( aId, amount );
            AssertTrueSecL( err == KErrNone, KGetValueFailedTxt, err );
            message.Format( _L( "EPositionFieldSatelliteNumUsed was %d" ), amount );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatelliteTime:
            {
            err = aInfo->GetValue( aId, time );
            time.FormatL( message, _L( "EPositionFieldSatelliteTime %D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B" ) );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatelliteHorizontalDoP:
            {
            err = aInfo->GetValue( aId, value );
            message.Format( _L( "EPositionFieldSatelliteHorizontalDoP was %f" ), value );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatelliteVerticalDoP:
            {
            err = aInfo->GetValue( aId, value );
            message.Format( _L( "EPositionFieldSatelliteVerticalDoP was %f" ), value );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatellitePositionDoP:
            {
            err = aInfo->GetValue( aId, value );
            message.Format( _L( "EPositionFieldSatellitePositionDoP was %f" ), value );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatelliteSeaLevelAltitude:
            {
            err = aInfo->GetValue( aId, value );
            message.Format( _L( "EPositionFieldSatelliteSeaLevelAltitude was %f" ), value );
            INFO_PRINTF1( message );
            break;
            }
        }
    }
*/
/* In this test case instance of CPositioner is created directly without
   using MLFW at all. This way memory allocation failures can be caused
   in PSY, to see the effect at each possible phase.
   
   #1 Prepare for CPositioner constructon by creating necessary interface
      instances.
   #2 Start a loop
   #3 Turn heap failure tool on, so that n:th allocation will fail
      starting from value 1.
   #4 Try to create CPositioner instance
   #5 Check that leave code was KErrNone or KErrNoMemory
   #6 Increase failure rate by one step, and go for next round, 
      if leave code was KErrNoMemory
   #7 Create an instance of CPositioner
   #8 Create an instance of CPositioner
   #9 Create an instance of CPositioner, after this delete all instances and check that no heap is reserved
*/
void CTDBT3Step::TcOomInPsyLoadL(const TUid aPsyUid)
    {
    // #1
    TPositionerConstructParams params;
    
    Tt3MPosParameterObserver paramObserver;
    Tt3MPosStatusObserver statusObserver;
    
    params.iImplementationUid = aPsyUid;
    params.iParamObserver = &paramObserver;
    params.iStatusObserver = &statusObserver;
    
    CPositioner* positioner = NULL;
    
    __UHEAP_MARK;
    
    TInt failureRate = 1;
    TInt err = KErrNone;
    
    // #2
    do
        {
        LogL(_L("Set heap failure rate to %d"), failureRate);
        
        INFO_PRINTF1(_L("Try to create CPositioner instance without using MLFW"));
        
        // #3 Set heap failure, this can only be set after log writings.
        __UHEAP_FAILNEXT(failureRate);
        
        // #4
        TRAP(err, positioner = CPositioner::NewL(&params));
        
        // Disble heap failure, because otherwise writing to log file will fail
        __UHEAP_RESET;
        
        LogL(_L("Leave code: %d"), err);
        
        // #5
        if (err != KErrNoMemory && err != KErrNone)
            {
            LogErrorL(_L("KErrNoMemory (-4) is only allowed error code, now received %d"), err);
            
            User::Leave(KErrGeneral);
            }
        
        // #6
        failureRate++;
        
        } while (failureRate < 100 && err == KErrNoMemory);
        
        delete positioner; // TEMP
        
        
        // #7
        INFO_PRINTF1(_L("Create CPositioner instance"));
        
        positioner = CPositioner::NewL(&params);
        
        CleanupStack::PushL(positioner);
        
        // #8
        CPositioner* positioner2 = CPositioner::NewL(&params);
        
        CleanupStack::PushL(positioner2);
        
        // #9
        CPositioner* positioner3 = CPositioner::NewL(&params);
        
        CleanupStack::PushL(positioner3);
        
        CleanupStack::PopAndDestroy(3); // positioner, positioner2, positioner3
        
        //We need to have EComSession closed to free all global variables.
        REComSession::FinalClose();
        
        __UHEAP_MARKEND;
        
        INFO_PRINTF1(_L("Test completed succesfully."));
    }


// This test case tests what happens when OOM occurs when making a
// location request.
// 
// #1 Create CPositioner instance directly without using MLFW
// #2 Start a loop
//   #3 Set heap failure on (with rate 1 at first round)
//   #4 Request for location by calling NotifyPositionUpdate
//   #5 Wait until request has completed
//   #6 Disable heap failure
//   #7 Check that return value was KErrNone or KErrNoMemory
//   #8 Increase failure rate, and go on new round if error
//      code was KErrNoMemory and failure rate is < 100
// #9 Delete CPositioner instance
// #10 Fail test if last location request did not succeed.
//   
void CTDBT3Step::TcOomWhenRequestingLocationL(const TUid aPsyUid, CLcfCommonProcedures* aCommon)
    {
    __UHEAP_MARK;
    
    // #1
    TPositionerConstructParams params;
    
    Tt3MPosParameterObserver paramObserver;
    Tt3MPosStatusObserver statusObserver;
    
    params.iImplementationUid = aPsyUid;
    params.iParamObserver = &paramObserver;
    params.iStatusObserver = &statusObserver;
    
    INFO_PRINTF1(_L("Create CPositioner instance"));
    
    CPositioner* positioner = NULL;
    
    
    TInt failureRate = 1;
    TRequestStatus status;
    
    // #2
    do
        {
        positioner = CPositioner::NewL(&params);
        CleanupStack::PushL(positioner);

        LogL(_L("Set heap failure rate to %d"), failureRate);
        
        INFO_PRINTF1(_L("Try to request location directly"));
        
        // #3 Set heap failure, this can only be set after log writings.
        __UHEAP_FAILNEXT(failureRate);
        

        // #4
        status = KRequestPending;
        
        TPositionInfo posInfo;
        
        TRAPD(err, positioner->NotifyPositionUpdate(posInfo, status));
        
        if (err != KErrNone)
            {
            User::__DbgSetAllocFail(RHeap::EUser,RHeap::ENone,1);
            
            LogErrorL(_L("Leave in NotifyPositionUpdate with code %d!"));
            
            User::Leave(KErrGeneral);
            }
        
        // #5
        aCommon->AsyncWait(status);
        
        // Disble heap failure, because otherwise writing to log file will fail
        __UHEAP_RESET;
        
        if (status != KErrNoMemory && status != KErrNone)
            {
            LogErrorL(_L("KErrNoMemory (-4) is only allowed error code, now received %d"), status.Int());
            
            User::Leave(KErrGeneral);
            }
        else if (status == KErrNone)
            {
            PrintPositionInformationL(posInfo);
            }
        
        LogL(_L("Error code: %d"), status.Int());
        
        // #6
        failureRate++;
        
        CleanupStack::PopAndDestroy(positioner);
        
        } while (failureRate < 100 && status == KErrNoMemory);
        
        INFO_PRINTF1(_L("Delete created CPositioner instance."));
        
        
        if (status != KErrNone)
            {
            ERR_PRINTF1(_L("Last location request should have succeeded."));
            User::Leave(KErrGeneral);
            }
        
        //We need to have EComSession closed to free all global variables.
        REComSession::FinalClose();
        
        __UHEAP_MARKEND;
        
        INFO_PRINTF1(_L("Test completed succesfully."));    
    }



void CTDBT3Step::DisplayHeapUsageL(const TInt aOrigHUsage, TInt& aPrevHUsage, TInt& aMaxHUsage, CLcfCommonProcedures* aCommon)
    {
    TInt heapSize = 0;
    
    TInt currentHUsage = aCommon->GetHeapUsageL(heapSize); 
    
    if (aMaxHUsage < currentHUsage)
        {
        aMaxHUsage = currentHUsage;
        }
    
    INFO_PRINTF1(KNullDesC);
    LogL(_L("%5d B, grew by %5d bytes now, %5d bytes since start (size %d)"), 
        currentHUsage, currentHUsage-aPrevHUsage, currentHUsage-aOrigHUsage, heapSize);
    INFO_PRINTF1(KNullDesC);
    
    
    aPrevHUsage = currentHUsage;
    }


// This test case measures heap usage of given PSY, when three connections
// are opened and location fix is requested using each conneciton at the
// same time.
//
// MLFW is not used in this test, because otherwise it would be impossible
// to distinguish PSY heap usage from EPos Location Server heap usage. 
// Instead CPositioner instances are created directly.
//
// Heap consumption is measured and displayed after each step below.
//
// #1 Create CPositioner instance #1
// #2 Create CPositioner instance #2
// #3 Create CPositioner instance #3
// #4 Create CPositioner instance #4
// #5 Create CPositioner instance #5
// #6 Make location request with intance #1
// #7 Make location request with intance #2
// #8 Make location request with intance #3
// #9 Make location request with intance #4
// #10 Make location request with intance #5
// #11 Wait that request#1 completes
// #12 Wait that request#2 completes
// #13 Wait that request#3 completes
// #14 Wait that request#4 completes
// #15 Wait that request#5 completes
// #16 Delete all CPositioner instances
// #17 Check that memory consumption did not exceed limits at any point
//
void CTDBT3Step::TcHeapUsageL(const TUid aPsyUid, CLcfCommonProcedures* aCommon)
    {
    
    __UHEAP_MARK;
    
    TPositionerConstructParams params;
    
    Tt3MPosParameterObserver paramObserver;
    Tt3MPosStatusObserver statusObserver;
    
    params.iImplementationUid = aPsyUid;
    params.iParamObserver = &paramObserver;
    params.iStatusObserver = &statusObserver;
    
    TInt heapUsage;
    TInt originalHeapUsage;
    TInt previousHeapUsage;
    TInt maxHeapUsage = 0;
    TInt heapSize;
    
    heapUsage = aCommon->GetHeapUsageL(heapSize);
    originalHeapUsage = heapUsage;
    previousHeapUsage = heapUsage;
    maxHeapUsage = heapUsage;
    
    
    LogL(_L("Heap usage at beginning: %d bytes, size %d bytes"), heapUsage, heapSize);
    
    // #1
    INFO_PRINTF1(_L("Create CPositioner instance"));
    
    CPositioner* positioner = CPositioner::NewL(&params);
    
    CleanupStack::PushL(positioner);
    
    INFO_PRINTF1(_L("Heap usage after one open:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #2
    CPositioner* positioner2 = CPositioner::NewL(&params);
    
    CleanupStack::PushL(positioner2);
    
    INFO_PRINTF1(_L("Heap usage after two opens:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #3
    CPositioner* positioner3 = CPositioner::NewL(&params);
    
    CleanupStack::PushL(positioner3);
    
    INFO_PRINTF1(_L("Heap usage after three opens:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #4
    CPositioner* positioner4 = CPositioner::NewL(&params);
    
    CleanupStack::PushL(positioner4);
    
    INFO_PRINTF1(_L("Heap usage after four opens:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #5
    CPositioner* positioner5 = CPositioner::NewL(&params);
    
    CleanupStack::PushL(positioner5);
    
    INFO_PRINTF1(_L("Heap usage after five opens:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // Request location
    
    INFO_PRINTF1(_L("Request location with positioner instance #1"));
    
    TRequestStatus status = KRequestPending;
    TRequestStatus status2 = KRequestPending;
    TRequestStatus status3 = KRequestPending;
    TRequestStatus status4 = KRequestPending;
    TRequestStatus status5 = KRequestPending;
    TPositionInfo posInfo;
    TPositionInfo posInfo2;
    TPositionInfo posInfo3;
    TPositionInfo posInfo4;
    TPositionInfo posInfo5;
    
    aCommon->FlushEventsL();
    
    // #6 Location request #1
    positioner->NotifyPositionUpdate(posInfo, status);
    
    INFO_PRINTF1(_L("Heap usage after requesting location once:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #7 Location request #2
    positioner2->NotifyPositionUpdate(posInfo2, status2);
    
    INFO_PRINTF1(_L("Heap usage after requesting location twice:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #8 Location request #3
    positioner3->NotifyPositionUpdate(posInfo3, status3);
    
    INFO_PRINTF1(_L("Heap usage after requesting location three times:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #9 Location request #4
    positioner4->NotifyPositionUpdate(posInfo4, status4);
    
    INFO_PRINTF1(_L("Heap usage after requesting location four times:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #10 Location request #5
    positioner5->NotifyPositionUpdate(posInfo5, status5);
    
    INFO_PRINTF1(_L("Heap usage after requesting location five times:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    
    INFO_PRINTF1(_L("Wait for location request completions"));
    
    aCommon->FlushEvents();
    
    // #11
    aCommon->AsyncWait(status);
    
    INFO_PRINTF1(_L("Heap usage after receiving location fix #1:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #12
    aCommon->AsyncWait(status2);
    
    INFO_PRINTF1(_L("Heap usage after receiving location fix #2:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #13
    aCommon->AsyncWait(status3);
    
    INFO_PRINTF1(_L("Heap usage after receiving location fix #3:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #14
    aCommon->AsyncWait(status4);
    
    INFO_PRINTF1(_L("Heap usage after receiving location fix #4:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #15
    aCommon->AsyncWait(status5);
    
    INFO_PRINTF1(_L("Heap usage after receiving location fix #5:"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #16
    INFO_PRINTF1(_L("Delete all CPositioner instances"));
    
    CleanupStack::PopAndDestroy(5); // positioner, positioner2, positioner3
    
    
    INFO_PRINTF1(_L("Heap usage after deleting all CPositioner instances"));
    DisplayHeapUsageL(originalHeapUsage, previousHeapUsage, maxHeapUsage, aCommon);
    
    // #17
    
    // Max memory consumption is 4KB + 1KB for each additional connection
    // --> 6 kB in this test case
    const TInt KMaxHeapUsage = 12 * 1024;
    
    // We must calculate how much heap usage has increased since start of test
    maxHeapUsage -= originalHeapUsage;
    
    AssertTrueSecL(
        maxHeapUsage <= KMaxHeapUsage, 
        _L("Too large heap usage (allowed %d bytes, consumed %d bytes)"), 
        KMaxHeapUsage, maxHeapUsage);
    
    LogL(_L("Largest detected heap usage: %d bytes (%.1f kB)"), maxHeapUsage, TReal32(maxHeapUsage)/1024.0);
    
    __UHEAP_MARKEND;
    
    INFO_PRINTF1(_L("Test completed succesfully"));
    }
    
void CTDBT3Step::LogL(TRefByValue<const TDesC> aMsg, ...)
    {
    VA_LIST list;
    VA_START(list, aMsg);
    iLogBuf.FormatList(aMsg, list);
    
    INFO_PRINTF1(iLogBuf);
    }

void CTDBT3Step::LogErrorL(TRefByValue<const TDesC> aMsg, ...)
    {
    VA_LIST list;
    VA_START(list, aMsg);
    iLogBuf.FormatList(aMsg, list);
    
    ERR_PRINTF1(iLogBuf);
    }

#if SERVICE_TESTS
void CTDBT3Step::TestServiceDbOomL()
    {

    TInt err = KErrNone;

    CPosServiceDb* serviceDb = NULL;
    CPosServiceIdArray* serviceArray = NULL;
    CPosService* service = NULL;
    
    __UHEAP_MARK;

#if 1
    serviceDb = CPosServiceDb::OpenL();
    CleanupStack::PushL(serviceDb);
    serviceArray = serviceDb->ListServicesL();
    CleanupStack::PushL(serviceArray);
    service = serviceDb->OpenServiceL( (*serviceArray)[1] );
    CleanupStack::PushL(service);

    LogL( _L("Service id was: %S"), &(service->Id()) );
    LogL( _L("Service name was: %S"), &(service->Name()) );

    TPosPrivacy privacy(  TPosPrivacy::EAcceptAll, EFalse );
    service->SetPrivacyL( privacy );

#else
    
    serviceDb = TestServiceDbOpenOomLC();
    
    serviceArray = TestServiceArrayOpenOomLC( serviceDb );
    
    LogL( _L( "Services in db: %d" ), serviceArray->Count() );

    if( serviceArray->Count() < 2 )
        {
        INFO_PRINTF1(_L("No services in the db"));
        User::Leave( KErrGeneral );
        }

    service = TestServiceLOpenOomLC( serviceDb, serviceArray );

    LogL( _L("Service id was: %S"), &(service->Id()) );
    LogL( _L("Service name was: %S"), &(service->Name()) );


    TestSetPrivacyLOomL( service );
#endif

    TRAP( err, TestCommitServiceLOomL( serviceDb, service ) );

#if 0
    serviceDb->CloseServiceL( service->DbItemId() );
    CleanupStack::PopAndDestroy(3);
    serviceDb = CPosServiceDb::OpenL();
    CleanupStack::PushL(serviceDb);
    serviceArray = serviceDb->ListServicesL();
    CleanupStack::PushL(serviceArray);
    service = serviceDb->OpenServiceL( (*serviceArray)[1] );
    CleanupStack::PushL(service);
#endif

    TestFunctionalityAfterNotReadyL( serviceDb, serviceArray );
        
    //if( err != KErrNone )
    //    {
    //    TestCloseServiceLOomL( serviceDb, service->DbItemId() );
    //    }

    // TestDeleteServicesLOomL( serviceDb, serviceArray );

    CleanupStack::PopAndDestroy(3); // serviceDb, serviceArray, service
        
    __UHEAP_MARKEND;
        
    INFO_PRINTF1(_L("Test completed succesfully."));
	_LIT( KIndexName8, "Ended: **** TCS2002 - Service Db Oom testing ****" );
	INFO_PRINTF1(KIndexName8);
    }

CPosServiceDb* CTDBT3Step::TestServiceDbOpenOomLC()
    {

    TInt failureRate = 1;
    TInt err = KErrNone;
    CPosServiceDb* serviceDb = NULL;

    __UHEAP_MARK;
    
    // #2
    do
        {
        LogL(_L("Set heap failure rate to %d"), failureRate);
        
        INFO_PRINTF1(_L("Try to open service Db"));
        
        // #3 Set heap failure, this can only be set after log writings.
        __UHEAP_FAILNEXT(failureRate);
        
        TRAP( err, serviceDb = CPosServiceDb::OpenL() );
        
        // Disble heap failure, because otherwise writing to log file will fail
        __UHEAP_RESET;
        
        
        // SERVICE DB CHECKING
        if ( err != KErrNoMemory && err != KErrNone )
            {
            LogErrorL(_L("KErrNoMemory (-4) is only allowed error code, now received %d"), err );
            
            User::Leave( KErrGeneral );
            }
        else if ( err == KErrNone )
            {
            CleanupStack::PushL( serviceDb );
            INFO_PRINTF1(_L("Db opened successfully"));
            }
        
        LogL(_L("Error code: %d"), err );
        
        // #6
        failureRate++;
        
        } while (failureRate < 2000 && err == KErrNoMemory );

    if ( err != KErrNone )
        {
        INFO_PRINTF1Error(_L("The opening of the service Db did not succeed!"));
        User::Leave(KErrGeneral);
        }
    else
        {
        return serviceDb;
        }
    }

CPosServiceIdArray* CTDBT3Step::TestServiceArrayOpenOomLC( CPosServiceDb* aServiceDb )
    {

    TInt failureRate = 1;
    TInt err = KErrNone;
    CPosServiceIdArray* serviceArray = NULL;

    do
        {
        LogL(_L("Set heap failure rate to %d"), failureRate);

        INFO_PRINTF1(_L("Try to get service ids"));
        
        // #3 Set heap failure, this can only be set after log writings.
        __UHEAP_FAILNEXT(failureRate);
        
        TRAP( err, serviceArray = aServiceDb->ListServicesL() );

        // Disble heap failure, because otherwise writing to log file will fail
        __UHEAP_RESET;
        
        // SERVICE ARRAY CHECKING
        if ( err != KErrNoMemory && err != KErrNone)
            {
            LogErrorL(_L("KErrNoMemory (-4) is only allowed error code, now received %d"), err );
            
            User::Leave( KErrGeneral );
            }
        else if ( err == KErrNone )
            {
            CleanupStack::PushL( serviceArray );
            INFO_PRINTF1(_L("Array opened successfully"));
            }
        
        LogL(_L("Error code: %d"), err );
        
        // #6
        failureRate++;
        
        } while (failureRate < 2000 && err == KErrNoMemory );

    if ( err != KErrNone )
        {
        INFO_PRINTF1Error(_L("The id array retrieval did not succeed!"));
        User::Leave(KErrGeneral);
        }
    else
        {
        return serviceArray;
        }
    }

CPosService* CTDBT3Step::TestServiceLOpenOomLC( CPosServiceDb* aServiceDb, 
                                                                   CPosServiceIdArray* aServiceArray )
    {

    TInt failureRate = 1;
    TInt err = KErrNone;
    CPosService* service = NULL;
    
    do
        {
        LogL(_L("Set heap failure rate to %d"), failureRate);
        INFO_PRINTF1(_L("Try to open service"));
        
        // #3 Set heap failure, this can only be set after log writings.
        __UHEAP_FAILNEXT(failureRate);
        
        TRAP( err, service = aServiceDb->OpenServiceL( (*aServiceArray)[1] ) );
        
        // Disble heap failure, because otherwise writing to log file will fail
        __UHEAP_RESET;
        
        // SERVICE CHECKING
        if ( err != KErrNoMemory && err != KErrNone)
            {
            LogErrorL(_L("KErrNoMemory (-4) is only allowed error code, now received %d"), err );
            
            User::Leave( KErrGeneral );
            }
        else if ( err == KErrNone )
            {
            CleanupStack::PushL( service );
            INFO_PRINTF1(_L("Service opened successfully"));
            }
        
        LogL(_L("Error code: %d"), err );
        
        // #6
        failureRate++;
        
        } while (failureRate < 2000 && err == KErrNoMemory );
            
    if ( err != KErrNone )
        {
        INFO_PRINTF1Error(_L("The opening of the service did not succeed!"));
        User::Leave(KErrGeneral);
        }
    else
        {
        return service; // serviceDb
        }
    }

CPosService* CTDBT3Step::TestServiceLXOpenOomLC( CPosServiceDb* aServiceDb, 
                                                                   CPosServiceIdArray* aServiceArray )
    {

    TInt failureRate = 1;
    TInt err = KErrNone;
    CPosService* service = NULL;
    
    do
        {
        LogL(_L("Set heap failure rate to %d"), failureRate);
        INFO_PRINTF1(_L("Try to open service"));
        
        // #3 Set heap failure, this can only be set after log writings.
        __UHEAP_FAILNEXT(failureRate);
        
        TRAP( err, service = aServiceDb->OpenServiceLX( (*aServiceArray)[1] ) );
        
        // Disble heap failure, because otherwise writing to log file will fail
        __UHEAP_RESET;
        
        // SERVICE CHECKING
        if ( err != KErrNoMemory && err != KErrNone)
            {
            LogErrorL(_L("KErrNoMemory (-4) is only allowed error code, now received %d"), err );
            
            User::Leave( KErrGeneral );
            }
        else if ( err == KErrNone )
            {
            CleanupStack::PushL( service );
            INFO_PRINTF1(_L("Service opened successfully"));
            }
        
        LogL(_L("Error code: %d"), err );
        
        // #6
        failureRate++;
        
        } while (failureRate < 2000 && err == KErrNoMemory );
            
    if ( err != KErrNone )
        {
        INFO_PRINTF1Error(_L("The opening of the service did not succeed!"));
        User::Leave(KErrGeneral);
        }
    else
        {
        return service; // serviceDb
        }
    }


void CTDBT3Step::TestSetPrivacyLOomL( CPosService* aService )
    {

    TInt failureRate = 1;
    TInt err = KErrNone;
    
    do
        {
        LogL(_L("Set heap failure rate to %d"), failureRate);
        INFO_PRINTF1(_L("Try to set privacy"));
        
        // #3 Set heap failure, this can only be set after log writings.
        __UHEAP_FAILNEXT(failureRate);

        TPosPrivacy privacy(  TPosPrivacy::EAcceptAll, EFalse );
        
        TRAP( err, aService->SetPrivacyL( privacy ) );
        
        // Disble heap failure, because otherwise writing to log file will fail
        __UHEAP_RESET;
        
        // SERVICE CHECKING
        if ( err != KErrNoMemory && err != KErrNone)
            {
            LogErrorL(_L("KErrNoMemory (-4) is only allowed error code, now received %d"), err );
            
            User::Leave( KErrGeneral );
            }
        else if ( err == KErrNone )
            {
            INFO_PRINTF1(_L("Privacy set successfully"));
            }
        
        LogL(_L("Error code: %d"), err );
        
        // #6
        failureRate++;
        
        } while (failureRate < 2000 && err == KErrNoMemory );
            
    if ( err != KErrNone )
        {
        INFO_PRINTF1Error(_L("The setting of the privacy did not succeed!"));
        User::Leave(KErrGeneral);
        }
    }

void CTDBT3Step::TestCommitServiceLOomL( CPosServiceDb* aServiceDb, 
                                                                   CPosService* aService )
    {

    TInt failureRate = 1;
    TInt err = KErrNone;

    do
        {
        LogL(_L("Set heap failure rate to %d"), failureRate);
        INFO_PRINTF1(_L("Try to commit service"));
        
        // #3 Set heap failure, this can only be set after log writings.
        __UHEAP_FAILNEXT(failureRate);
        
        TRAP( err, aServiceDb->CommitServiceL( *aService ) );
        
        // Disble heap failure, because otherwise writing to log file will fail
        __UHEAP_RESET;
        
        // SERVICE CHECKING
        if ( err != KErrNoMemory && err != KErrNone /* && err != KErrNotReady */ )
            {
            LogErrorL(_L("KErrNoMemory (-4) is only allowed error code, now received %d"), err );
            
            User::Leave( KErrGeneral );
            }
        else if ( err == KErrNone )
            {
            INFO_PRINTF1(_L("Service commited successfully"));
            }
        
        LogL(_L("Error code: %d"), err );
        
        // #6
        failureRate++;
        
        } while (failureRate < 2000 &&  ( err == KErrNoMemory /*|| err == KErrNotReady*/ ) );
            
    if ( err != KErrNone )
        {
        INFO_PRINTF1Error(_L("The commiting of the service did not succeed!"));
        User::Leave(KErrGeneral);
        }
    }

void CTDBT3Step::TestCloseServiceLOomL( CPosServiceDb* aServiceDb, 
                                                                   TPosServiceItemId aServiceId )
    {

    TInt failureRate = 1;
    TInt err = KErrNone;

    do
        {
        LogL(_L("Set heap failure rate to %d"), failureRate);
        INFO_PRINTF1(_L("Try to close service"));
        
        // #3 Set heap failure, this can only be set after log writings.
        __UHEAP_FAILNEXT(failureRate);
        
        TRAP( err, aServiceDb->CloseServiceL( aServiceId ) );
        
        // Disble heap failure, because otherwise writing to log file will fail
        __UHEAP_RESET;
        
        // SERVICE CHECKING
        if ( err != KErrNoMemory && err != KErrNone)
            {
            LogErrorL(_L("KErrNoMemory (-4) is only allowed error code, now received %d"), err );
            
            User::Leave( KErrGeneral );
            }
        else if ( err == KErrNone )
            {
            INFO_PRINTF1(_L("Service closed successfully"));
            }
        
        LogL(_L("Error code: %d"), err );
        
        // #6
        failureRate++;
        
        } while (failureRate < 2000 && err == KErrNoMemory );
            
    if ( err != KErrNone )
        {
        INFO_PRINTF1Error(_L("The closing of the service did not succeed!"));
        User::Leave(KErrGeneral);
        }
    }

void CTDBT3Step::TestDeleteServicesLOomL( CPosServiceDb* aServiceDb, 
                                                                   CPosServiceIdArray* aServiceArray )
    {

    TInt failureRate = 1;
    TInt err = KErrNone;
    
    do
        {
        LogL(_L("Set heap failure rate to %d"), failureRate);
        INFO_PRINTF1(_L("Try to delete services"));
        
        // #3 Set heap failure, this can only be set after log writings.
        __UHEAP_FAILNEXT(failureRate);
        
        TRAP( err, aServiceDb->DeleteServicesL( *aServiceArray ) );
        
        // Disble heap failure, because otherwise writing to log file will fail
        __UHEAP_RESET;
        
        // SERVICE CHECKING
        if ( err != KErrNoMemory && err != KErrNone)
            {
            LogErrorL(_L("KErrNoMemory (-4) is only allowed error code, now received %d"), err );
            
            User::Leave( KErrGeneral );
            }
        else if ( err == KErrNone )
            {
            INFO_PRINTF1(_L("Service deleted successfully"));
            }
        
        LogL(_L("Error code: %d"), err );
        
        // #6
        failureRate++;
        
        } while (failureRate < 2000 && err == KErrNoMemory );
            
    if ( err != KErrNone )
        {
        INFO_PRINTF1Error(_L("The deleting of the service did not succeed!"));
        User::Leave(KErrGeneral);
        }
    }

void CTDBT3Step::TestFunctionalityAfterNotReadyL( CPosServiceDb* aServiceDb, 
                                                                   CPosServiceIdArray* aServiceArray )
    {

    TInt error = TestFunctionsAfterNotReadyL( aServiceDb, aServiceArray );
    LogL( _L( "Error code before Close %d" ), error );
            
    // CLOSE SERVICE AND TRY ALL AGAIN AFTER CLOSE
    
    INFO_PRINTF1( _L( "CloseServiceL" ));
    TRAPD( err, aServiceDb->CloseServiceL( (*aServiceArray)[1] ));
    LogL( _L( "Leave code %d" ), err );
    
    // RETRY
    if( error == KErrNone )
        TestFunctionsAfterNotReadyL( aServiceDb, aServiceArray );
    else
        TestFunctionsWithDifferentServiceL( aServiceDb, aServiceArray );
    }

TInt CTDBT3Step::TestFunctionsAfterNotReadyL( CPosServiceDb* aServiceDb, 
                                                                  CPosServiceIdArray* aServiceArray )
    {

    CPosService* serviceL = NULL;
    CPosService* serviceLC = NULL;
    CPosServiceIdArray* array = NULL;
    
    INFO_PRINTF1( _L( "OpenServiceL" ));
    TRAPD( err, serviceL = aServiceDb->OpenServiceL( (*aServiceArray)[1] ) );
    CleanupStack::PushL(serviceL);
    LogL( _L( "Leave code %d" ), err );
    if( err == KErrNone )
        {        
        CleanupStack::PopAndDestroy();
        aServiceDb->CloseServiceL( (*aServiceArray)[1] );
        }
    else
        CleanupStack::Pop();
    
    INFO_PRINTF1( _L( "OpenServiceLX" ));
    TRAP( err, serviceLC = aServiceDb->OpenServiceLX( (*aServiceArray)[1] ) ; CleanupStack::Pop() );
    CleanupStack::PushL(serviceLC);
    LogL( _L( "Leave code %d" ), err );
    if( err == KErrNone )
        {
        CleanupStack::PopAndDestroy();
        aServiceDb->CloseServiceL( (*aServiceArray)[1] );
        }
    else
        CleanupStack::Pop();
    
    INFO_PRINTF1( _L( "ReadServiceL" ));
    TRAP( err, serviceL = aServiceDb->ReadServiceL( (*aServiceArray)[1] ));
    LogL( _L( "Leave code %d" ), err );
    if( err == KErrNone )
        {
        delete serviceL;
        serviceL = NULL;
        }
    
    INFO_PRINTF1( _L( "ReadServiceLC" ));
    TRAP( err, serviceL = aServiceDb->ReadServiceLC( (*aServiceArray)[1] ) ; CleanupStack::Pop() );
    
    if( err == KErrNone )
        CleanupStack::PushL( serviceL );
    
    LogL( _L( "Leave code %d" ), err );
    
    if( err == KErrNone )
        CleanupStack::PopAndDestroy(); // serviceL
    
    CPosService* service = CPosService::NewLC();
    service->SetIdL( _L( "+3585092345" ),CPosRequestor::EIdFormatPhoneNumber );
    service->SetNameL( _L( "Kalle Test" ) );
    
    TPosPrivacy privacy( TPosPrivacy::EAlwaysAsk, EFalse );
    service->SetPrivacyL( privacy );
    
    INFO_PRINTF1( _L( "AddNewServiceL" ));
    TRAP( err, aServiceDb->AddNewServiceL( *service ));
    LogL( _L( "Leave code %d" ), err );
    
    CleanupStack::PopAndDestroy(); // service
    
    INFO_PRINTF1( _L( "FindLC" ));
    TRAP( err, array = aServiceDb->FindLC( _L( "Location" ), KPosServiceInfoAllAttr ) ; CleanupStack::Pop() );
    LogL( _L( "Leave code %d" ), err );
    
    if( err == KErrNone )
        {
        CleanupStack::PushL(array);
        LogL( _L( "FindLC found %d matches" ), array->Count() );
        CleanupStack::PopAndDestroy();
        array = NULL;
        }
    
    INFO_PRINTF1( _L( "SortArrayL" ));
    TRAP( err, array = aServiceDb->SortArrayL( *aServiceArray, EPosAsc ));
    CleanupStack::PushL( array );
    LogL( _L( "Leave code %d" ), err );
    
    if( err == KErrNone )
        CleanupStack::PopAndDestroy();
    else
        CleanupStack::Pop();
    
    array = NULL;
    
    INFO_PRINTF1( _L( "SortArrayLC" ));
    TRAP( err, array = aServiceDb->SortArrayLC( *aServiceArray, EPosDesc ); CleanupStack::Pop() );
    CleanupStack::PushL( array );
    LogL( _L( "Leave code %d" ), err );
    
    if( err == KErrNone )
        CleanupStack::PopAndDestroy();
    else
        CleanupStack::Pop();
    
    array = NULL;
    
    INFO_PRINTF1( _L( "MatchPhoneNumberL" ));
    TRAP( err, array = aServiceDb->MatchPhoneNumberL( _L( "0505550123" ) ));
    CleanupStack::PushL( array );
    LogL( _L( "Leave code %d" ), err );
    
    if( err == KErrNone )
        CleanupStack::PopAndDestroy();
    else
        CleanupStack::Pop();
    
    array = NULL;
    
    if( aServiceDb->CompressRequired() )
        INFO_PRINTF1( _L( "Compress needed" ));
    else
        INFO_PRINTF1( _L( "Compress NOT needed" ));
    
    INFO_PRINTF1( _L( "CompressL" ));
    TRAP( err, aServiceDb->CompressL() );
    LogL( _L( "Leave code %d" ), err );
    
    INFO_PRINTF1( _L( "DeleteServiceL" ));
    TRAP( err, aServiceDb->DeleteServiceL( (*aServiceArray)[1] ));
    LogL( _L( "Leave code %d" ), err );
    
    INFO_PRINTF1( _L( "ListServicesL" ));
    TRAP( err, array = aServiceDb->ListServicesL() );
    if( err == KErrNone )
        {
        CleanupStack::PushL(array);
        aServiceArray = array;
        }
    
    LogL( _L( "Leave code %d" ), err );
    
    if( err == KErrNone )
        {
        LogL( _L( "List found %d services" ), array->Count() );
        }
    
    INFO_PRINTF1( _L( "DeleteServicesL" ));
    TRAP( err, aServiceDb->DeleteServicesL( *aServiceArray ));
    LogL( _L( "Leave code %d" ), err );
    
    return err;

    }

void CTDBT3Step::TestFunctionsWithDifferentServiceL( CPosServiceDb* aServiceDb, 
                                                                  CPosServiceIdArray* aServiceArray )
    {

    CPosService* serviceL = NULL;
    CPosService* serviceLC = NULL;
    CPosServiceIdArray* array = NULL;

    CPosService* service = CPosService::NewLC();
    service->SetIdL( _L( "+3585072323" ),CPosRequestor::EIdFormatPhoneNumber );
    service->SetNameL( _L( "Kalle Test2" ) );

    TPosPrivacy privacy2( TPosPrivacy::EAcceptAll, EFalse );
    service->SetPrivacyL( privacy2 );

    TPosServiceItemId serviceId;

    INFO_PRINTF1( _L( "AddNewServiceL" ));
    TRAPD( err, serviceId = aServiceDb->AddNewServiceL( *service ));
    LogL( _L( "Leave code %d" ), err );
    if( err != KErrNone )
        serviceId = (*aServiceArray)[1];

    INFO_PRINTF1( _L( "OpenServiceL" ));
    TRAP( err, serviceL = aServiceDb->OpenServiceL( serviceId ));
    CleanupStack::PushL(serviceL);
    LogL( _L( "Leave code %d" ), err );
    if( err == KErrNone )
        {        
        CleanupStack::PopAndDestroy();
        aServiceDb->CloseServiceL( serviceId );
        }
    else
        CleanupStack::Pop();
    
    INFO_PRINTF1( _L( "OpenServiceLX" ));
    TRAP( err, serviceLC = aServiceDb->OpenServiceLX( serviceId ) ; CleanupStack::Pop() );
    CleanupStack::PushL(serviceLC);
    LogL( _L( "Leave code %d" ), err );
    if( err == KErrNone )
        {
        CleanupStack::PopAndDestroy();
        aServiceDb->CloseServiceL( serviceId );
        }
    else
        CleanupStack::Pop();
    
    INFO_PRINTF1( _L( "ReadServiceL" ));
    TRAP( err, serviceL = aServiceDb->ReadServiceL( serviceId ));
    CleanupStack::PushL(serviceLC);
    LogL( _L( "Leave code %d" ), err );
    if( err == KErrNone )
        CleanupStack::PopAndDestroy(); // serviceL
    else
        CleanupStack::Pop();
    
    INFO_PRINTF1( _L( "ReadServiceLC" ));
    TRAP( err, serviceL = aServiceDb->ReadServiceLC( serviceId ) ; CleanupStack::Pop() );

    if( err == KErrNone )
        CleanupStack::PushL( serviceL );

    LogL( _L( "Leave code %d" ), err );
    
    if( err == KErrNone )
        CleanupStack::PopAndDestroy(); // serviceL

    INFO_PRINTF1( _L( "FindLC" ));
    TRAP( err, array = aServiceDb->FindLC( _L( "Location" ), KPosServiceInfoAllAttr ) ; CleanupStack::Pop() );
    LogL( _L( "Leave code %d" ), err );

    if( err == KErrNone )
        {
        CleanupStack::PushL(array);
        LogL( _L( "FindLC found %d matches" ), array->Count() );
        CleanupStack::PopAndDestroy();
        array = NULL;
        }

    INFO_PRINTF1( _L( "SortArrayL" ));
    TRAP( err, array = aServiceDb->SortArrayL( *aServiceArray, EPosAsc ));
    CleanupStack::PushL( array );
    LogL( _L( "Leave code %d" ), err );

    if( err == KErrNone )
        CleanupStack::PopAndDestroy();
    else
        CleanupStack::Pop();

    array = NULL;

    INFO_PRINTF1( _L( "SortArrayLC" ));
    TRAP( err, array = aServiceDb->SortArrayLC( *aServiceArray, EPosDesc ); CleanupStack::Pop() );
    CleanupStack::PushL( array );
    LogL( _L( "Leave code %d" ), err );

    if( err == KErrNone )
        CleanupStack::PopAndDestroy();
    else
        CleanupStack::Pop();

    array = NULL;
    
    INFO_PRINTF1( _L( "MatchPhoneNumberL" ));
    TRAP( err, array = aServiceDb->MatchPhoneNumberL( _L( "0505550123" ) ));
    CleanupStack::PushL( array );
    LogL( _L( "Leave code %d" ), err );

    if( err == KErrNone )
        CleanupStack::PopAndDestroy();
    else
        CleanupStack::Pop();

    array = NULL;
        
    if( aServiceDb->CompressRequired() )
        INFO_PRINTF1( _L( "Compress needed" ));
    else
        INFO_PRINTF1( _L( "Compress NOT needed" ));

    INFO_PRINTF1( _L( "CompressL" ));
    TRAP( err, aServiceDb->CompressL() );
    LogL( _L( "Leave code %d" ), err );

    INFO_PRINTF1( _L( "DeleteServiceL" ));
    TRAP( err, aServiceDb->DeleteServiceL( serviceId ));
    LogL( _L( "Leave code %d" ), err );

    INFO_PRINTF1( _L( "ListServicesL" ));
    TRAP( err, array = aServiceDb->ListServicesL() );
    if( err == KErrNone )
        {
        CleanupStack::PushL(array);
        aServiceArray = array;
        }
    
    LogL( _L( "Leave code %d" ), err );
    
    if( err == KErrNone )
        {
        LogL( _L( "List found %d services" ), array->Count() );
        }
    
    INFO_PRINTF1( _L( "DeleteServicesL" ));
    TRAP( err, aServiceDb->DeleteServicesL( *aServiceArray ));
    LogL( _L( "Leave code %d" ), err );

    CleanupStack::PopAndDestroy(2, service); // service, array

}

#endif

// End of File
