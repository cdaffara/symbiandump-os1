// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Telephony Watchers Unit Test main test code.
// 
//

/**
 @file
*/

#include <e32uid.h>
#include <f32file.h>
#include <bafindf.h>
#include <cdbcols.h>

#include "indicatorwatcher.h"
#include "signalstrengthwatcher.h"

#include "TE_TelWatchersUnitBase.h"
#include "TE_TelWatchersUnitTest.h"

//
// Constants...
//
const TInt  KExpectedCallStateChanges(4);
const TInt  KExpectedBatteryChargerStateChanges(4);
const TInt  KExpectedNetworkAvailabilityStateChanges(4);
const TInt  KSimulatedCommDBChanges(10);

/**
 *  Each test step initialises it's own name
 */
CTelWatchersUnitTestCallStateChangesWithPublishSubscribe::CTelWatchersUnitTestCallStateChangesWithPublishSubscribe()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestCallStateChangesWithPublishSubscribe"));
	} // CTelWatchersUnitTestCallStateChangesWithPublishSubscribe::CTelWatchersUnitTestCallStateChangesWithPublishSubscribe


/**
 *  Simple destructor.
 */
CTelWatchersUnitTestCallStateChangesWithPublishSubscribe::~CTelWatchersUnitTestCallStateChangesWithPublishSubscribe()
	{
	// NOP
	} // CTelWatchersUnitTestCallStateChangesWithPublishSubscribe::~CTelWatchersUnitTestCallStateChangesWithPublishSubscribe


/**
 *  This test monitors the call state notifications and checks they
 *  are as expected and valid.
 */	
enum TVerdict CTelWatchersUnitTestCallStateChangesWithPublishSubscribe::doTestStepL()
	{
	SetSimTsyTestNumberL(0);

	RProperty propCurrCallState; //- property that reflects current call state	
	TInt propVal = KErrUnknown;  //- value of the property

	INFO_PRINTF2(_L("Wait for %d call state changes with publish and subscribe"),
		         KExpectedCallStateChanges);

	//-- attach to KUidCurrentCall property
	TESTL(propCurrCallState.Attach(KUidSystemCategory,KUidCurrentCall.iUid) == KErrNone);

	//
	// Install the Indicator and Signal Strength watcher...
	//
	TInt dummyWatcherParams;
	CIndicatorWatcher*  indicatorWatcher = CIndicatorWatcher::NewL((TAny*)&dummyWatcherParams);

	CleanupStack::PushL(indicatorWatcher);

	CSignalStrengthWatcher*  signalStrengthWatcher = CSignalStrengthWatcher::NewL((TAny*)&dummyWatcherParams);
	CleanupStack::PushL(signalStrengthWatcher);

	//
	// Prepare indicator watcher to return random call state
	// values...
	//
  	TESTL(RProperty::Set(KUidSystemCategory, KUidTestProp_CallStateChange.iUid, 0) == KErrNone);
  	
	//
	// Wait for the change events...
	//
	CWatcherObserverPS* watcher = new(ELeave) CWatcherObserverPS;
	CleanupStack::PushL(watcher);
	
	watcher->BindProperty(&propCurrCallState);

	TInt  counter;

	for (counter = 0;  counter < KExpectedCallStateChanges;  counter++)
		{
		//
		// Prepares a notify request and starts the scheduler. Stops the
		// scheduler when notify is detected.
		//
        watcher->WaitForPropertyChangeL();

        //-- get property value after it has been updated
       TESTL(watcher->GetPropertyValue(propVal) == KErrNone);

		//
		// Print the state...
		//
		switch (propVal)
			{
			case ESACallNone:
				{
				INFO_PRINTF1(_L("  Call None."));
				}
				break;

			case ESACallVoice:
				{
				INFO_PRINTF1(_L("  Call Voice."));
				}
				break;

			case ESACallFax:
				{
				INFO_PRINTF1(_L("  Call Fax."));
				}
				break;

			case ESACallData:
				{
				INFO_PRINTF1(_L("  Call Data."));
				}
				break;

			case ESACallAlerting:
				{
				INFO_PRINTF1(_L("  Call Alerting."));
				}
				break;

			case ESACallRinging:
				{
				INFO_PRINTF1(_L("  Call Ringing."));
				}
				break;

			case ESACallAlternating:
				{
				INFO_PRINTF1(_L("  Call Alternating."));
				}
				break;

			default:
				{
				INFO_PRINTF2(_L("Failed: Unknown state returned (%d)!"), propVal);
		        SetTestStepResult(EFail);
				}
				break;
			}
		}

	CleanupStack::PopAndDestroy(watcher);
	CleanupStack::PopAndDestroy(signalStrengthWatcher);
	CleanupStack::PopAndDestroy(indicatorWatcher);

	return TestStepResult();
	} // CTelWatchersUnitTestCallStateChangesWithPublishSubscribe::doTestStepL

/**
 *  Each test step initialises it's own name
 */
CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe::CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestBatteryChargerChangesWithPublishSubscribe"));
	} // CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe::CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe


/**
 *  Simple destructor.
 */
CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe::~CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe()
	{
	// NOP
	} // CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe::~CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe


/**
 *  This test monitors the battery charger notifications and checks they
 *  are as expected and valid.
 */	
enum TVerdict CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe::doTestStepL()
	{
	SetSimTsyTestNumberL(0);

	RProperty propChargerState;  //- property that reflects phone charger state
	TInt propVal = KErrUnknown;  //- value of the property

	INFO_PRINTF2(_L("Wait for %d battery charger changes with publish and subscribe"),
		         KExpectedBatteryChargerStateChanges);

	//-- attach to KUidChargerStatus property
	TESTL(propChargerState.Attach(KUidSystemCategory,KUidChargerStatus.iUid) == KErrNone);

	//
	// Install the Indicator and Signal Strength watcher...
	//
    TInt dummyWatcherParams;
	CIndicatorWatcher*  indicatorWatcher = CIndicatorWatcher::NewL((TAny*)&dummyWatcherParams);

	CleanupStack::PushL(indicatorWatcher);

	CSignalStrengthWatcher*  signalStrengthWatcher = CSignalStrengthWatcher::NewL((TAny*)&dummyWatcherParams);
	CleanupStack::PushL(signalStrengthWatcher);

	//
	// Prepare indicator watcher to return random battery values...
	//
	//
	TESTL(RProperty::Set(KUidSystemCategory, KUidTestProp_CallStateChange.iUid, 1) == KErrNone);

	//
	// Wait for the change events...
	//	
	CWatcherObserverPS* watcher = new(ELeave) CWatcherObserverPS;
	CleanupStack::PushL(watcher);
	watcher->BindProperty( &propChargerState );
	TInt counter;
	
	for (counter = 0;  counter < KExpectedBatteryChargerStateChanges;  counter++)
		{
		//
		// Prepares a notify request and starts the scheduler. Stops the scheduler
		// when notify is detected.
		//
		watcher->WaitForPropertyChangeL();

        //-- get property value after it has been updated
        TESTL(watcher->GetPropertyValue(propVal) == KErrNone);

		//
		// Print state...
		//
		switch (propVal)
			{
			case ESAChargerConnected:
				{
				INFO_PRINTF1(_L("  Charger connected."));
				}
				break;

			case ESAChargerDisconnected:
				{
				INFO_PRINTF1(_L("  Charger disconnected."));
				}
				break;

			case ESAChargerNotCharging:
				{
				INFO_PRINTF1(_L("  Not charging."));
				}
				break;

			default:
				{
				INFO_PRINTF2(_L("Failed: Unknown state returned (%d)!"), propVal);
		        SetTestStepResult(EFail);
				}
				break;
			}
		}

	CleanupStack::PopAndDestroy(watcher);
	CleanupStack::PopAndDestroy(signalStrengthWatcher);
	CleanupStack::PopAndDestroy(indicatorWatcher);

	return TestStepResult();
	} // CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe::doTestStepL

/**
 *  Each test step initialises it's own name
 */
CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe::CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestNetworkAvailabilityChangesWithPublishSubscribe"));
	} // CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe::CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe


/**
 *  Simple destructor.
 */
CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe::~CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe()
	{
	// NOP
	} // CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe::~CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe


/**
 *  This test monitors the network availablity notifications and checks they
 *  are as expected and valid.
 */	
enum TVerdict CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe::doTestStepL()
	{
	SetSimTsyTestNumberL(0);

	RProperty propNetworkState;  //- property that reflects phone charger state
	TInt propVal = KErrUnknown;  //- value of the property
	
	INFO_PRINTF2(_L("Wait for %d network availability changes with publish and subscribe"),
		         KExpectedNetworkAvailabilityStateChanges);

	//-- attach to KUidCurrentCall property
	TESTL(propNetworkState.Attach(KUidSystemCategory,KUidNetworkStatus.iUid) == KErrNone);

	//
	// Install the Indicator and Signal Strength watcher...
	//
    TInt dummyWatcherParams;
	CIndicatorWatcher*  indicatorWatcher = CIndicatorWatcher::NewL((TAny*)&dummyWatcherParams);

	CleanupStack::PushL(indicatorWatcher);

	CSignalStrengthWatcher*  signalStrengthWatcher = CSignalStrengthWatcher::NewL((TAny*)&dummyWatcherParams);
	CleanupStack::PushL(signalStrengthWatcher);

	//
	// Prepare indicator watcher to return random network state
	// values...
	//
	TESTL(RProperty::Set(KUidSystemCategory, KUidTestProp_CallStateChange.iUid, 2) == KErrNone);

	//
	// Wait for the change events...
	//
	CWatcherObserverPS* watcher = new(ELeave) CWatcherObserverPS;
	CleanupStack::PushL(watcher);
	watcher->BindProperty( &propNetworkState );
	TInt counter;

	for (counter = 0;  counter < KExpectedNetworkAvailabilityStateChanges;  counter++)
		{
		//
		// Prepares a notify request and starts the scheduler. Stops the
		// scheduler when notify is detected.
		//
		watcher->WaitForPropertyChangeL();

        //-- get property value after it has been updated
        TESTL(watcher->GetPropertyValue(propVal) == KErrNone);

		//
		// Print state...
		//
		switch(propVal)
			{
			case ESANetworkAvailable:
				{
				INFO_PRINTF1(_L("  Network available."));
				}
				break;

			case ESANetworkUnAvailable:
				{
				INFO_PRINTF1(_L("  Network unavailable."));
				}
				break;
				
			default:
				{
				INFO_PRINTF2(_L("Failed: Unknown state returned (%d)!"), propVal);
		        SetTestStepResult(EFail);
				}
				break;
			}
		}
	
	CleanupStack::PopAndDestroy(watcher);
	CleanupStack::PopAndDestroy(signalStrengthWatcher);
	CleanupStack::PopAndDestroy(indicatorWatcher);

	return TestStepResult();
	} // CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe::doTestStepL

/**
 *  Each test step initialises it's own name
 */
CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe::CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestSimulteCommDBModemChangeWithPublishSubscribe"));
	} //CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe::CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe


/**
 *  Simple destructor.
 */
CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe::~CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe()
	{
	// NOP
	} // CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe::~CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe


/**
 *  This test issues a number of CommDB table update requests to the Watcher and
 *  checks that the CommDB table is re-read.
 */	
enum TVerdict CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe::doTestStepL()
	{	
	SetSimTsyTestNumberL(0);

	INFO_PRINTF2(_L("Simulate %d CommDB default modem changes with publish and subscribe"),
		         KSimulatedCommDBChanges);
	
	RProperty propMdmRecordChanged;   //-- modem record change property (debug only)
	RProperty propMdmTableRefreshed;  //-- modem table re-read property (debug only)

	TESTL(propMdmRecordChanged.Attach(KUidSystemCategory, KUidTestProp_ModemRecordChanged.iUid) == KErrNone);
	TESTL(propMdmTableRefreshed.Attach(KUidSystemCategory, KUidTestProp_ModemTableRefreshed.iUid) == KErrNone);

	//
	// Install the Indicator and Signal Strength watcher...
	//
    TInt dummyWatcherParams;
	CIndicatorWatcher*  indicatorWatcher = CIndicatorWatcher::NewL((TAny*)&dummyWatcherParams);
	CleanupStack::PushL(indicatorWatcher);

	CSignalStrengthWatcher*  signalStrengthWatcher = CSignalStrengthWatcher::NewL((TAny*)&dummyWatcherParams);
	CleanupStack::PushL(signalStrengthWatcher);

	//
	// Allow a few seconds for the Active Objects to start...
	//
	PauseToRunActiveSchedulerL(10);

	//
	// Wait for the change events...
	//
	CWatcherObserverPS* watcher = new(ELeave) CWatcherObserverPS;
	CleanupStack::PushL(watcher);
	TInt  counter;

	for (counter = 0;  counter < KSimulatedCommDBChanges;  counter++)
		{
		//
		// Send some comm db modem record change events
		//
		TTime  time;
		time.UniversalTime();

        TESTL(RProperty::Set(KUidSystemCategory, KUidCommDbModemPhoneServicesAndSMSChange.iUid, I64LOW(time.Int64())) == KErrNone );

		//
		// Now wait for an ack that the change was spotted...
		//
        watcher->BindProperty(&propMdmRecordChanged);
        watcher->WaitForPropertyChangeL();

		INFO_PRINTF1(_L("  CommDB change has been spotted."));

		//
		// Now wait for an ack that the modem table has been re-read ok
		//
        watcher->BindProperty(&propMdmTableRefreshed);
        watcher->WaitForPropertyChangeL();
        
		INFO_PRINTF1(_L("  Modem table has been re-read."));

		//
		// Now wait a few seconds before trying again
		//
		PauseToRunActiveSchedulerL(10);
		}

	CleanupStack::PopAndDestroy(watcher);
	CleanupStack::PopAndDestroy(signalStrengthWatcher);
	CleanupStack::PopAndDestroy(indicatorWatcher);

	return TestStepResult();
	} // CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe::doTestStepL


/**
 *  Each test step initialises it's own name
 */
CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe::CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestCommDBChangeWhenWatcherDisabledWithPublishSubscribe"));
	} // CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe::CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe


/**
 *  Simple destructor.
 */
CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe::~CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe()
	{
	// NOP
	} // CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe::~CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe


/**
 *  This test checks that the Watcher notices a CommDB change whilst the Watcher
 *  are disabled.
 */	
enum TVerdict CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe::doTestStepL()
	{
	SetSimTsyTestNumberL(0);

	INFO_PRINTF1(_L("Test sending a CommDB change when the watcher is disabled with publish and subscribe"));

	RProperty propPhoneWatcherDisable;//-- Phone watchers disabling/reset (debug only)
	RProperty propMdmRecordChanged;   //-- modem record change property   (debug only)
	RProperty propMdmTableRefreshed;  //-- modem table re-read property (debug only)

	TESTL(propPhoneWatcherDisable.Attach(KUidSystemCategory, KUidTestProp_WatchersDisable.iUid) == KErrNone);
	TESTL(propMdmRecordChanged.Attach(KUidSystemCategory, KUidTestProp_ModemRecordChanged.iUid) == KErrNone);
	TESTL(propMdmTableRefreshed.Attach(KUidSystemCategory, KUidTestProp_ModemTableRefreshed.iUid) == KErrNone);

	//
	// Install the Indicator and Signal Strength watcher...
	//
    TInt dummyWatcherParams;
	CIndicatorWatcher*  indicatorWatcher = CIndicatorWatcher::NewL((TAny*)&dummyWatcherParams);
	CleanupStack::PushL(indicatorWatcher);

	CSignalStrengthWatcher*  signalStrengthWatcher = CSignalStrengthWatcher::NewL((TAny*)&dummyWatcherParams);
	CleanupStack::PushL(signalStrengthWatcher);

	//
	// Allow a few seconds for the Active Objects to start...
	//
	PauseToRunActiveSchedulerL(10);


	//
	// To wait for the change events...
	//
	CWatcherObserverPS* watcher = new(ELeave) CWatcherObserverPS();
	CleanupStack::PushL(watcher);

	//
	// Disable the watcher by setting the property...
	//
	TESTL(propPhoneWatcherDisable.Set(ETrue) == KErrNone );
    
	INFO_PRINTF1(_L("  Watcher has been disabled."));

	//
	// Send some Comm DB modem record change events...
	//
	TTime  time;
	time.UniversalTime();

	TESTL(RProperty::Set(KUidSystemCategory, KUidCommDbModemPhoneServicesAndSMSChange.iUid, I64LOW(time.Int64())) == KErrNone );
    
	//
	// Now wait for an ack that the change was spotted...
	//
	watcher->BindProperty(&propMdmRecordChanged);
	watcher->WaitForPropertyChangeL();
	INFO_PRINTF1(_L("  CommDB change has been spotted."));

	//
	// Now wait for an ack that the modem table has been re-read ok
	//
	watcher->BindProperty(&propMdmTableRefreshed);
	watcher->WaitForPropertyChangeL();
	INFO_PRINTF1(_L("  Modem table has been re-read."));

	//
	// Now wait a few seconds before trying again...
	//
	PauseToRunActiveSchedulerL(10);

	CleanupStack::PopAndDestroy(watcher);
	CleanupStack::PopAndDestroy(signalStrengthWatcher);
	CleanupStack::PopAndDestroy(indicatorWatcher);

	return TestStepResult();
	} // CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe::doTestStepL
