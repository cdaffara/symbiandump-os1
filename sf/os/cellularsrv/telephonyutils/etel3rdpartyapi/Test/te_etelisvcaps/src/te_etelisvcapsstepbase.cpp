// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of base class for each capability test in Etel3rdParty.
// 
//

/**
 @file 
 @internalTechnology
*/

#include "te_etelisvcapsstepbase.h"
#include "te_etelisvcapsdefs.h"
#include "te_capscallao.h"
#include "callrunner/callrunnercommon.h"

// define the maximum line length in config file
const TInt KCapsMaxConfigLaneLength = 255;


_LIT(KTe_CapTCB, 			"TCB");
_LIT(KTe_CapCommDD, 		"CommDD");
_LIT(KTe_CapPowerMgmt, 		"PowerMgmt");
_LIT(KTe_CapMultimediaDD, 	"MultimediaDD");
_LIT(KTe_CapReadDeviceData, "ReadDeviceData");
_LIT(KTe_CapWriteDeviceData,"WriteDeviceData");
_LIT(KTe_CapDRM, 			"DRM");
_LIT(KTe_CapTrustedUI, 		"TrustedUI");
_LIT(KTe_CapProtServ, 		"ProtServ");
_LIT(KTe_CapDiskAdmin, 		"DiskAdmin");
_LIT(KTe_CapNetworkControl, "NetworkControl");
_LIT(KTe_CapAllFiles, 		"AllFiles");
_LIT(KTe_CapSwEvent, 		"SwEvent");
_LIT(KTe_CapNetworkServices,"NetworkServices");
_LIT(KTe_CapLocalServices, 	"LocalServices");
_LIT(KTe_CapReadUserData, 	"ReadUserData");
_LIT(KTe_CapWriteUserData, 	"WriteUserData");
_LIT(KTe_CapLocation, 		"Location");
_LIT(KTe_CapSurroundingsDD, "SurroundingsDD");
_LIT(KTe_CapUserEnvironment,"UserEnvironment");


/**
Entry structure in capability mapping table. 
*/
struct TCapabilityEntry
	{
	const TDesC* iName;
	TCapability iValue;
	};

/**
Capability mapping table. 
Use to map strings into integer value.
*/
TCapabilityEntry capabilityMapping[ECapability_Limit+1] =
	{
	{&KTe_CapTCB, 				ECapabilityTCB},
	{&KTe_CapCommDD, 			ECapabilityCommDD},
	{&KTe_CapPowerMgmt, 		ECapabilityPowerMgmt},
	{&KTe_CapMultimediaDD, 		ECapabilityMultimediaDD},
	{&KTe_CapReadDeviceData,	ECapabilityReadDeviceData},
	{&KTe_CapWriteDeviceData, 	ECapabilityWriteDeviceData},
	{&KTe_CapDRM, 				ECapabilityDRM},
	{&KTe_CapTrustedUI, 		ECapabilityTrustedUI},
	{&KTe_CapProtServ, 			ECapabilityProtServ},
	{&KTe_CapDiskAdmin, 		ECapabilityDiskAdmin},
	{&KTe_CapNetworkControl, 	ECapabilityNetworkControl},
	{&KTe_CapAllFiles, 			ECapabilityAllFiles},
	{&KTe_CapSwEvent, 			ECapabilitySwEvent},
	{&KTe_CapNetworkServices, 	ECapabilityNetworkServices},
	{&KTe_CapLocalServices, 	ECapabilityLocalServices},
	{&KTe_CapReadUserData, 		ECapabilityReadUserData},
	{&KTe_CapWriteUserData, 	ECapabilityWriteUserData},
	{&KTe_CapLocation, 			ECapabilityLocation},
	{&KTe_CapSurroundingsDD, 	ECapabilitySurroundingsDD},
	{&KTe_CapUserEnvironment, 	ECapabilityUserEnvironment},
	{NULL, ECapability_Limit}
	};


/**
Implementation of CTestStep virtual method.
Performs common test initialization.

@return EPass or EFail
 */
TVerdict CTe_CapsTestSuiteStepBase::doTestStepPreambleL()
	{
	// Load the data from config table
	TBuf<KCapsMaxConfigLaneLength> buffer;
	TPtrC theString = buffer.Ptr();
	
	if( !GetStringFromConfig(ConfigSection(),KReqCapabilitiesStr, theString) ||
		!GetIntFromConfig(ConfigSection(),KNoOfCallsRequiredStr,iNoOfCallsRequired) ||
		!GetIntFromConfig(ConfigSection(),KSimTsyConfigNumberStr,iSimTsyConfigNumber) ||
		!GetBoolFromConfig(ConfigSection(),KAsynchRequestStr,iAsyncRequeset) ) 
		{
		// Leave if there's any error.
		INFO_PRINTF1(_L("Unable to load data from TEF config file"));
		User::Leave(KErrNotFound);
		}
	
	// optional element, default false
	if (iNoOfCallsRequired)
		{
		if (!GetBoolFromConfig(ConfigSection(),KExternalCallstStr,iExternalCalls) )
			{
			iExternalCalls = EFalse;
			}
		}
	
	if (iNoOfCallsRequired < 0 || iNoOfCallsRequired > 2)
		{
		// the system allows maximum of 2 calls
		User::Leave(KErrArgument);	
		}
		
   	// optional element, default false
   	if (!GetBoolFromConfig(ConfigSection(),KNotifierStr,iNotifier) )
   		{
		iNotifier = EFalse;
		}
	
	// Ensure the SIMTSY test number is correctly set up before we start
	User::LeaveIfError(iTestNumberProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
	User::LeaveIfError(iTestNumberProperty.Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, iSimTsyConfigNumber));

	// start the etel3rd party, telephony service
	iTelephony = CTelephony::NewL();
	
	// create timer
	iTimer = CCapsTimer::NewL();
		
	// parse capabilities string and convert it into number representation
	ParseCapabilities(theString);
	
	// create calls required before test starts
	if (iExternalCalls)
		{
		InvokeCallsExternallyL();
		}
	else
		{
		InvokeCallsInternallyL();
		}

	return TestStepResult();
	}

/**
The method creates separate application (callRunner) to establish 
the calls in separate process.

@param aNoOfCalls number of calls to create.
*/	
void CTe_CapsTestSuiteStepBase::InvokeCallsExternallyL()
	{
	_LIT(KCallRunnerPath, "callrunner.exe");
	TBuf<1> buffer;
		
	// the buffer is a command line argument. 
	// It specifies the number of calls to create
	buffer.AppendNum(iNoOfCallsRequired);
	
	User::LeaveIfError(iProcess.Create(KCallRunnerPath, buffer));
	
	// make sure the calls are created before we proceed with a test case
	iProcess.SetPriority(EPriorityHigh);
	iProcess.Resume();
	   
	// attach to the property for further notification on test case end. 
	User::LeaveIfError(iCallRunnerProperty.Attach(KCallRunnerPropertyUid, KCallRunnerPropertyKey));
	
	// make sure the calls are created before we proceed with a test case
	TRequestStatus status;
	TInt value;	
	do 
		{
		iCallRunnerProperty.Subscribe(status);
		User::WaitForRequest(status);
		iCallRunnerProperty.Get(value);
		} while (value != ECallRunnerStarted);
	}

/**
The method creates required calls on the internall CTelephony object.

@param aNoOfCalls number of calls to create.
*/
void CTe_CapsTestSuiteStepBase::InvokeCallsInternallyL()	
	{
	// set up local data necessary for dialing/answering a call;
	CTelephony::TCallStatusV1 status;
	CTelephony::TCallStatusV1Pckg statusPckg(status);
	
	_LIT(KGoodNumber,"1789");
	
	// create the calls only when NetworkSevice capability is present
	if (IsCapNetworkServicesPresent())
		{
		if (iNoOfCallsRequired)
			{
			// create the first call
			
			//Set the call parameters for call 1
			CTelephony::TCallParamsV1 callParams1;
			callParams1.iIdRestrict = CTelephony::ESendMyId;
			CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	

			TTestCallParams testCallParams1;
			testCallParams1.iLine = CTelephony::EVoiceLine;
			testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
			testCallParams1.iTelAddress.iTelNumber = KGoodNumber;
			testCallParams1.iExpStat = KErrNone;

			// Create active DialNewCallAct objects
			CTestDialNewCallAct* testDialNewCallAct = CTestDialNewCallAct::NewLC(this, iTelephony);
			testDialNewCallAct->TestDialNewCall(testCallParams1);
			CActiveScheduler::Start();
			
			User::LeaveIfError(iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg));
			TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
			TEST1(testCallParams1.iCallId == CTelephony::EISVCall1, ETrue);
			
			if (iNoOfCallsRequired == 2)
				{
				// create the second call
				
				//Set the call parameters for call 2
				CTelephony::TCallParamsV1 callParams2;
				callParams2.iIdRestrict = CTelephony::ESendMyId;
				CTelephony::TCallParamsV1Pckg callParamsPckg2(callParams2);	

				TTestCallParams testCallParams2;
				testCallParams2.iLine = CTelephony::EVoiceLine;
				testCallParams2.iCallParamsV1Pckg = &callParamsPckg2;
				testCallParams2.iTelAddress.iTelNumber = KGoodNumber;
				testCallParams2.iExpStat = KErrNone;
				
				// Put on hold first
				CTestHoldAct* TestHoldAct = CTestHoldAct::NewLC(this,iTelephony);	
				TestHoldAct->TestHold(testCallParams1);
				CActiveScheduler::Start();	
		
				User::LeaveIfError(iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg));
				TEST1(status.iStatus == CTelephony::EStatusHold, ETrue);
				
				// Create active DialNewCallAct objects
				testDialNewCallAct->TestDialNewCall(testCallParams2);
				CActiveScheduler::Start();
				
				User::LeaveIfError(iTelephony->GetCallStatus(testCallParams2.iCallId,  statusPckg));
				TEST1(status.iStatus == CTelephony::EStatusConnected, ETrue);
				TEST1(testCallParams2.iCallId == CTelephony::EISVCall2, ETrue);
				
				CleanupStack::PopAndDestroy(TestHoldAct);
				}
			CleanupStack::PopAndDestroy(testDialNewCallAct);
			}
		} 
	}

/**
Implementation of CTestStep base class virtual
Performs test cleanup. 

@return EPass or EFail 
*/
TVerdict CTe_CapsTestSuiteStepBase::doTestStepPostambleL()
	{
	if (iExternalCalls)
		{
		// Publish the new value to inform callRunner to close the process
		TInt err = iCallRunnerProperty.Set(KCallRunnerPropertyUid, KCallRunnerPropertyKey, ECallRunnerCloseDown);
		iCallRunnerProperty.Close();
		iProcess.Close();
		}
	
	iTestNumberProperty.Close();
	
	delete iTelephony;
	iTelephony = NULL;
	
	delete iTimer;
	iTimer = NULL;
	
	return TestStepResult();
	}


/**
The method verifies if the current process has required capabilities. 

The list of checked capabilities is passed in as an argument.
If argument not specified (NULL) the required capability list is taken from 
TEF test configuration data.

@param aReqCap Optional argument. Lists of the capabilities to check against. 
				If not provided, the TEF ini file is used to identify required capabilities 
*/
TBool CTe_CapsTestSuiteStepBase::IsCapabilitySufficient(RArray<TCapability> *aReqCap)
	{
	// choose the correct array to check against
	RArray<TCapability>& localCapArray = (aReqCap != NULL)? *aReqCap : iReqCap;
	
	for (TInt i = localCapArray.Count() - 1; i >= 0 ; i--)
		{
		if (localCapArray[i] == ECapability_None)
			{
			return ETrue;
			}
		if (!RProcess().HasCapability(localCapArray[i]))
			{
			return EFalse;
			}
		}
	return ETrue;
	}

/**
The method verifies if the current process has NetworkServices capability. 
*/
TBool CTe_CapsTestSuiteStepBase::IsCapNetworkServicesPresent()
	{
	return RProcess().HasCapability(ECapabilityNetworkServices);
	}

/**
The method parses the capability line from TEF configuration file. 
*/
void CTe_CapsTestSuiteStepBase::ParseCapabilities(TPtrC aString)
	{
	TBuf<255> strToParse(aString);
	TBuf<255> token;
	TInt position = 0;
	TInt length = aString.Length();
	
	// identify required API capability
	while(1)
		{
		position = strToParse.Find(_L(";"));
		if (position != KErrNotFound)
			{
			token = strToParse.Left(position);
			strToParse = strToParse.Mid(position + 1);
			token.TrimRight();
			AddCapability(token);
			}
		else
			{
			strToParse.TrimAll();
			AddCapability(strToParse);
			break;			
			}
		}
	
	// print out the capability of executable
	INFO_PRINTF1(_L("Executable capability:"));
	for (TInt i = 0; capabilityMapping[i].iName != NULL; i++)
		{
		if (RProcess().HasCapability(capabilityMapping[i].iValue))
			{
			INFO_PRINTF2(_L("\t%S"), capabilityMapping[i].iName);
			}
		}
	INFO_PRINTF1(_L("End of capability list"));

	}
	
/**
The method update the local capability list with the capability passed as arg. 
It performs also string conversion into capability numerical value. 

@param aCapability String representation of capability
*/
void CTe_CapsTestSuiteStepBase::AddCapability(TPtrC aCapability)
	{
	TInt i;
	for (i = 0 ; i < ECapability_Limit ; i++)
		{
		if (aCapability.Match(*capabilityMapping[i].iName) == 0)
			{
			iReqCap.Append(static_cast<TCapability> (capabilityMapping[i].iValue));
			break;
			}
		}
	}

/**
The generic version of capability test step. 
It is using template methods: IssueRequestL and CancelRequestL 
to process specific test case.

*/	
TVerdict CTe_CapsTestSuiteStepBase::doTestStepL()
	{
	// Check if possible to make a test.
	// The condition verifies if the test requires calls to be established in this process. 
	// If calls required, the capability of the process are checked for NetworkServices.
	if (!iExternalCalls && 
		iNoOfCallsRequired > 0 && 
		!IsCapNetworkServicesPresent() )
		{
		// Skip the test, as this is unable to verify the test in such capability constraints
		INFO_PRINTF1(_L("Test case skipped. Unable to create calls without NetworkService capability."));
		return TestStepResult();
		}
		
	// Create internal AO to handle the asynchronous calls
	CAOCommon *clientAO = new CAOCommon(*iTimer);
	CleanupStack::PushL(clientAO);
	TRequestStatus& status = clientAO->GetInternalStatusObject();
	   
  
	// Issue the request for particular test		
	status = KErrNone;
	TEST1(IssueRequestL(status) == KErrNone, ETrue);
	
	// some of the APIs are synchronous, therefore do not activate AO
	if (iAsyncRequeset)
		{
		// as notifiers are returning KRequestPending and hangs until 
		// requested event occurs, set up the timer to stop the request.
		if (iNotifier)
			iTimer->After(100000);
		
		// set active object active and run scheduler
		clientAO->SetupAO();
		CActiveScheduler::Start();
		}
		
	// verify results
	if (IsCapabilitySufficient())
		{
		if (iNotifier)
			{
			TEST1(status.Int() == KRequestPending, ETrue);
			}
		else
			{
			TEST1(status.Int() == KErrNone, ETrue);
			}
		}
	else
		{
		TEST1(status == KErrPermissionDenied, ETrue);
		}
	

	// clear outstanding request, if any
	if (iAsyncRequeset && status == KRequestPending)
		{
		CancelRequestL();
		CActiveScheduler::Start();
		}
	
	CleanupStack::PopAndDestroy(clientAO);
	return TestStepResult();
	}	

/**
CAOCommon constructor

@param aCapsTimer Reference to the timer activated for a test case
*/
CAOCommon::CAOCommon(CCapsTimer& aCapsTimer)
	:CActive(CActive::EPriorityStandard), iCapsTimer(aCapsTimer)
	{
	CActiveScheduler::Add(this);
	}

/**
CAOCommon RunL implementation
*/
void CAOCommon::RunL()
	{
	// Reset the timer if it is running
	if (iCapsTimer.IsActive())
		{
		iCapsTimer.Cancel();
		}
	CActiveScheduler::Stop();
	}

/**
CAOCommon DoCancel implementation
*/

void CAOCommon::DoCancel()
	{
	if (iCapsTimer.IsActive())
		{
		iCapsTimer.Cancel();
		}
	}

/**
The method sets the AO active.
*/	
void CAOCommon::SetupAO()
	{
	SetActive();
	}

/**
The method return the reference to internal status object. 
*/
TRequestStatus& CAOCommon::GetInternalStatusObject()
	{
	return iStatus;
	}

/**
CCapsTimer constructor;
*/
CCapsTimer::CCapsTimer()
	:CTimer(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

/**
The methods creates the CCapsTimer object. 
*/
CCapsTimer* CCapsTimer::NewL()
	{
	CCapsTimer* me = new (ELeave) CCapsTimer();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

/**
Second-phase constructor
*/
void CCapsTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

/**
CCapsTimer RunL implementation
*/
void CCapsTimer::RunL()
	{
	CActiveScheduler::Stop();
	return;
	}



	

