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
//

/**
 @file te_lbsnetworkprivacysuitestepbase.cpp
*/

#include "te_lbsnetworkprivacysuitestepbase.h"
#include "te_lbsnetworkprivacysuitedefs.h"
#include <lbs/lbsnetprotocolbase.h>
#include "cprocesslaunch.h"
#include "tprocessstartparams.h"
#include "lbsnrhngmsgs.h"

_LIT(KNetGatewayExeName, "te_dummynetgateway.exe");



TVerdict CTe_LbsNetworkPrivacySuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	//process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
		
	// Construct a new ActiveScheduler and install it
	iSched = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iSched);
	
	//Initialigation of NRH<->NG & SystemController interfaces  
	InitialiseInterfacesL();
		
	// Start NetGateway
	User::LeaveIfError(StartNetGateway());
	
	// Create Network Privacy Client    
	iPosNetworkPrivacy  = CPosNetworkPrivacy::NewL();
			
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LbsNetworkPrivacySuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
     
	// Delete Network Privacy Client
	delete iPosNetworkPrivacy;
	iPosNetworkPrivacy = NULL;
        
   //Stop Netgateway     
    User::LeaveIfError(StopNetGateway());
         
    //Shutdown NRH<->NG & SystemController interface 
    ShutDownInterfacesL();
                
    // Remove ActiveScheduler
	CActiveScheduler::Install(NULL);
	delete iSched;
	iSched = NULL;
    
    return TestStepResult();
	}

CTe_LbsNetworkPrivacySuiteStepBase::~CTe_LbsNetworkPrivacySuiteStepBase()
	{
	}

CTe_LbsNetworkPrivacySuiteStepBase::CTe_LbsNetworkPrivacySuiteStepBase()
	{
	}

void CTe_LbsNetworkPrivacySuiteStepBase::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	INFO_PRINTF1(_L("CTe_LbsNetworkPrivacySuiteStepBase::ProcessNetChannelMessage"));
	if (aChannelId != RLbsNetChannel::EChannelNRH2NG)
		{
		INFO_PRINTF2(_L("Unexpected message channel id: %d"), aChannelId);
		SetTestStepResult(EFail);
		}
		
	INFO_PRINTF2(_L("Message Type (%d)"), aMessage.Type());	
	switch (aMessage.Type())
		{
		case TLbsNetLocationRequestMsg::ELocationRequest:
			{
			INFO_PRINTF1(_L("Unexpected message type!\n"));
			break;
			}
		case TLbsNetLocationRequestMsg::ELocationResponse:
			{
			INFO_PRINTF1(_L("Unexpected message type!\n"));
			break;
			}
		case TLbsNetLocationRequestMsg::ETransmitLocationRequest:
			{
			INFO_PRINTF1(_L("Unexpected message type!\n"));
			break;
			}
		case TLbsNetLocationRequestMsg::ETransmitLocationCancel:
			{
			INFO_PRINTF1(_L("Unexpected message type!\n"));
			break;
			}			
		case TLbsNetLocationRequestMsg::EPrivacyRequest:
			{
			INFO_PRINTF1(_L("CTe_LbsNetworkPrivacySuiteStepBase::ProcessNetChannelMessage:Privacy Request Received"));
			break;
			}
		case TLbsNetLocationRequestMsg::EPrivacyResponse:
			{
			INFO_PRINTF1(_L("Unexpected message type!\n"));
			break;
			}
		case TLbsNetLocationRequestMsg::ESessionComplete:
			{
			INFO_PRINTF1(_L("CTe_LbsNetworkPrivacySuiteStepBase::ProcessNetChannelMessage:SessionComplete received"));
	        break;
			}
		default:
			{
			INFO_PRINTF1(_L("Unknown message type!"));
			break;
			}
		}
	}


/* Starts the LbsNetGateway executable 
*/
TInt CTe_LbsNetworkPrivacySuiteStepBase::StartNetGateway()
	{
	// use the process launch code
	TProcessStartParams params;
	_LIT(KDummyProcessName, "dummylbsnetgateway");
	params.SetProcessFileName(KNetGatewayExeName);
	params.SetProcessName(KDummyProcessName);
	params.SetRendezvousRequired(ETrue);
	TInt r = CProcessLaunch::ProcessLaunch(params);
	return r;
	}

//Initialigation of NRH-NG & SystemController interfaces 
void CTe_LbsNetworkPrivacySuiteStepBase::InitialiseInterfacesL()
	{
	// Initialise NRH<->NG interface
	RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNG2NRH);
	RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNRH2NG);	
	
	// Initialize SystemController interface
	RLbsProcessSupervisor::InitializeL();
	}

//Shutdown NRH<->NG & SystemController interfaces
void CTe_LbsNetworkPrivacySuiteStepBase::ShutDownInterfacesL()
	{
	// Shutdown system controller interface
	RLbsProcessSupervisor::ShutDownL();

	// Shutdown NRH-NG interface
	RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNG2NRH);
	RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNRH2NG);
	}

/* Forces the shutdown of the LbsNetGateway executable.
*/
TInt CTe_LbsNetworkPrivacySuiteStepBase::StopNetGateway()
	{
	// Shutdown the network gateway via the process 
	// supervisor api that LbsRoot uses.
	TRAPD(err, iProcessSupervisor.OpenL(KDummyNetworkGatewayUid));
	if (err == KErrNone)
		{
		// Find the network gateway manager process
		RProcess process;
		TFullName fullName;
		TFindProcess findProcess;
		_LIT(KGatewayProcessPattern, "te_dummynetgateway.exe*");
		TBool runningInstanceFound = EFalse;
		
		findProcess.Find(KGatewayProcessPattern);
		while (err = findProcess.Next(fullName), err == KErrNone)
			{
			TInt processOpenErr = process.Open(findProcess);
			if (processOpenErr == KErrNone)
				{
				TExitType exitType = process.ExitType();
				if (exitType == EExitPending)
					{
					// found a running instance of the process,
					// so assume everything is ok;
					runningInstanceFound = ETrue;
					break;
					}
				}
				process.Close();
			}
		
		if (runningInstanceFound)
			{
			TRequestStatus status;
			process.Logon(status);
			iProcessSupervisor.CloseDownProcess();
			User::WaitForRequest(status);
			process.Close();
			iProcessSupervisor.Close();
			}
		}
	return err;
	}

