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
 @file TDBT6Step.cpp
 @internalTechnology
*/
#include <lbs.h>

#include "TDBT6Step.h"

CTDBT6Step::~CTDBT6Step()
/**
 * Destructor
 */
	{
	}

CTDBT6Step::CTDBT6Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KTDBT6Step);
	}

TVerdict CTDBT6Step::doTestStepPreambleL()
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


TVerdict CTDBT6Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		/*TInt test;
		if(!GetIntFromConfig(ConfigSection(),KTe_LbsBtGpsPsySuiteInt,test)
			)
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}*/
		SetTestStepResult(EPass);
		ConnectNeverPairedBeforeDeviceStepL();
		LocationRequestTestL();
		//StartL(test);
		}
	  return TestStepResult();
	}



TVerdict CTDBT6Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

// ---------------------------------------------------------
// CTDBT6Step::StartL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CTDBT6Step::StartL(const TInt aIndex)
    {
    switch(aIndex)
        {
        case 0:
        	_LIT(KIndexName0, "**** Display fix on screen ****");
        	INFO_PRINTF1(KIndexName0);
        	ConnectNeverPairedBeforeDeviceStepL();
        	LocationRequestTestL();
            break;
        } 
    }

TInt CTDBT6Step::StartBTPSYWatch(TAny* /*parameter*/)
    {
    // Create cleanupstack for this thread
    CTrapCleanup* cleanup = CTrapCleanup::New();
    CActiveScheduler* newScheduler;
    
    // Create active scheduler
    TRAPD(err, newScheduler = new(ELeave) CActiveScheduler();
    CActiveScheduler::Install(newScheduler););
    
    if(KErrNone != err)
    	{
    	return err;
    	}
    
    // Run P&S watchers
    TRAP(err, RunBTPSYWatchL(););
    
    // Delete active scheduler
    delete newScheduler;
    
    // Delete cleanup stack
    delete cleanup;
    
    return err; 
    }

void CTDBT6Step::RunBTPSYWatchL()
	{
    CBTPSYPropertyWatchArray* propertyArray;
    propertyArray = CBTPSYPropertyWatchArray::NewLC();
    propertyArray->WatchL();
    
    CActiveScheduler::Start();
    
    CleanupStack::PopAndDestroy(propertyArray);
	}

void CTDBT6Step::LocationRequestTestL()
	{
	// connect to server
	RPositionServer posServer;
	User::LeaveIfError(posServer.Connect());
	CleanupClosePushL(posServer);

	// open positioner
	RPositioner positioner;
	User::LeaveIfError(positioner.Open(posServer, KbtGPSPsyId));
	CleanupClosePushL(positioner);

	INFO_PRINTF1( KStartThreadTenUpdates);
	RThread thread;

	TRequestStatus threadStatus;

	INFO_PRINTF1( KDisableEmulatorPanic);
	User::SetJustInTime(EFalse);

	INFO_PRINTF1( KCreateThread);

	TInt err = thread.Create( _L( "BTGPSTestThread" ), StartBTPSYWatch, KDefaultStackSize, 
			KMinHeapSize, KMinHeapSize+KHeapOffset, (TAny *)(&Logger()), EOwnerProcess);



	AssertTrueSecL( err == KErrNone, KErrorCreatingThread, err );
	CleanupClosePushL( thread );

	INFO_PRINTF1(KMakeTestDriverMonitorThread);
	thread.Logon( threadStatus );

	INFO_PRINTF1( KStartThread);
	thread.Resume();

	// Set requestor
	positioner.SetRequestor(
			CRequestor::ERequestorService, 
			CRequestor::EFormatApplication, 
			KtdBT6);

	// make location request
	TRequestStatus reqStatus;
	TPositionInfo posInfo;


	do{
	positioner.NotifyPositionUpdate(posInfo, reqStatus);

	// and wait until the request is complete        
	User::WaitForRequest(reqStatus);

	User::LeaveIfError(reqStatus.Int());
	}while(reqStatus.Int() == 1);

	PrintPositionInformationL(posInfo);
	
	//
	InvertNightModeL();
	User::After(2000000);
	InvertNightModeL();
	User::After(2000000);
	// the following line allows human interactions.
	User::After(120000000);

	CleanupStack::PopAndDestroy(&thread);
	CleanupStack::PopAndDestroy(&positioner);
	CleanupStack::PopAndDestroy(&posServer);
    }

void CTDBT6Step::InvertNightModeL()
	{
	CRepository* settings = CRepository::NewLC(KCRUidBtGpsPsy);
	TInt nightMode = KErrNotFound;
	settings->Get(KBluetoothGpsPsyNightModeState, nightMode);
	switch(nightMode)
		{
		case 0: INFO_PRINTF1(_L("Night mode was off"));
			SetNightModeL(ETrue);
			INFO_PRINTF1(_L("Now, the indicators (the lights) should be lit on the bt dongle"));
			break;
		case 1: INFO_PRINTF1(_L("Night mode was on"));
			INFO_PRINTF1(_L("Now, the indicators (the lights) should be switched off on  the bt dongle"));
			SetNightModeL(EFalse);
			break;
		default: 
			delete settings;
			User::Leave(nightMode);
		}
	CleanupStack::PopAndDestroy(settings);
	}

void CTDBT6Step::SetNightModeL(TBool aMode)
    {
    if(aMode)
        {
        INFO_PRINTF1(_L("Set night mode on"));
        }
    else
        {
        INFO_PRINTF1(_L("Set night mode off"));
        }
    
    CRepository* settings = CRepository::NewLC(KCRUidBtGpsPsy);
    if(aMode)
        {
        User::LeaveIfError(settings->Set(KBluetoothGpsPsyNightModeState, 1));
        }
    else
        {
        User::LeaveIfError(settings->Set(KBluetoothGpsPsyNightModeState, 0));
        }
    CleanupStack::PopAndDestroy(settings);
    }



void CTDBT6Step::ConnectNeverPairedBeforeDeviceStepL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    TRAP( err, ConnectNeverPairedBeforeDeviceL( common ) );

    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYModInfoAndPosReq, "BT GPS PSY module statuses case failed with code %d" );
    INFO_PRINTF2( KErrBTGPSPSYModInfoAndPosReq, err );
    }

void CTDBT6Step::ConnectNeverPairedBeforeDeviceL( CLcfCommonProcedures* aCommon )
    {
    TInt err = KErrNone;
    
    TBuf<100> message;
    const TInt KRounds = 8;

    TPositionModuleStatus::TDeviceStatus correctStatus[KRounds] = { 
        TPositionModuleStatus::EDeviceInitialising, 
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
    
    RPositioner bTGpsPsy;
    
    /* INACTIVE CHECKING AND OPENING */
    
    INFO_PRINTF1( KCheckModuleActive);
    aCommon->FlushEventsL();
    err = posServer.GetModuleStatus( deviceStatus, KbtGPSPsyId );
    AssertTrueSecL( deviceStatus.DeviceStatus() == TPositionModuleStatus::EDeviceInactive, _L( "wrong module status wanted 3, got %d" ), deviceStatus.DeviceStatus() );
    
    INFO_PRINTF1( KStartListeningModuleEvents);
    moduleStatus = KRequestPending;
    posServer.NotifyModuleStatusEvent( btGPSEvent, moduleStatus );
    
    INFO_PRINTF1(KOpenPSYTxt);
    User::LeaveIfError(bTGpsPsy.Open( posServer, KbtGPSPsyId ));
    CleanupClosePushL(bTGpsPsy);
    
    INFO_PRINTF1(KSetRequestorTxt);
    User::LeaveIfError(bTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 ));
    
    
    INFO_PRINTF1(KWaitModuleReportInitializing);
    // and wait until the request is complete

    // round 0: init
    // round 1: ready
    // round 2: active
    // round 3: ready
    // round 4: standby
    // round 5: active
    // round 6: ready
    // round 7: inactive

    for( TInt i = 0; i < KRounds; i++ )
        {

        aCommon->FlushEventsL();

        message.Format( KRoundNumber, i );
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
        
        if( i < KRounds - 1 )
            {
            INFO_PRINTF1(KContinueListeningModuleEvents);
            moduleStatus = KRequestPending;
            posServer.NotifyModuleStatusEvent( btGPSEvent, moduleStatus );
            }

        // wait for the request
        if( i == 2 || i == 5 )
            {
            INFO_PRINTF1(KWaitRequestComplete);
            User::WaitForRequest(status);
            PrintPositionInformationL(basic);
            }
        // make request
        if( i == 1 || i == 4 )
            {
            INFO_PRINTF1(KMakeRequest);
            aCommon->FlushEventsL();
            // request positioninfo
            status = KRequestPending;
            bTGpsPsy.NotifyPositionUpdate( basic, status );
            }

        if( i == 6 )
            {
            aCommon->FlushEventsL();
            CleanupStack::PopAndDestroy(&bTGpsPsy);
            }
        }        
    
    CleanupStack::PopAndDestroy(&posServer);
}
