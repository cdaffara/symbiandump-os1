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
 @file te_suplposmsgpluginsuitestepbase.cpp
*/

#include "te_suplproxypmsuitestepbase.h"
#include "te_suplproxypmsuitedefs.h"

#include "cprocesslaunch.h"
#include "tprocessstartparams.h"



_LIT(KNetGatewayExeName, "te_sppm_dummynetgateway.exe");



TVerdict CTe_SuplProxyPMSuiteStepBase::doTestStepPreambleL()
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
	    
	// Start the dummy network gateway to load the SuplProxyProtocolModule
	User::LeaveIfError(StartNetGateway());
    _LIT(KStartNG, "Dummy network gateway started successfully");
    INFO_PRINTF1(KStartNG);
	
	// Load the POS message plugin [test code acts as the SUPL FW]
	RImplInfoPtrArray pluginArray;
	
	// Load the POS Message plugin
	const TUid aPOSMsgIntefaceUid =   {0x102073D4};
	REComSession::ListImplementationsL(aPOSMsgIntefaceUid, pluginArray);
	// Number of POS message plugins found [should always be 1]
	TInt countPOSMsgPlugins = pluginArray.Count();
	if (countPOSMsgPlugins < 1)
	   {
           _LIT(KPluginError, "POS Message plugin not found");
	       ERR_PRINTF1(KPluginError);
	       SetTestStepResult(EFail);
	       return TestStepResult();
	   }
	else
	    {
            _LIT(KPosMsgPlugin, "POS Message plugin found");
            INFO_PRINTF1(KPosMsgPlugin);
            
            const CImplementationInformation* pluginInformation = pluginArray[0];
            TUid  pluginUid = pluginInformation->ImplementationUid();
            // Load the POS message ECOM plugin implementation
            LoadSuplPosMessagePluginL();
	    }
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_SuplProxyPMSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	
	User::LeaveIfError(StopNetGateway());
		
    //Shutdown NRH<->NG & SystemController interface 
    ShutDownInterfacesL();
	
    // Remove ActiveScheduler
	CActiveScheduler::Install(NULL);
	delete iSched;
	iSched = NULL;
    
    return TestStepResult();
	
	}

void CTe_SuplProxyPMSuiteStepBase::LoadSuplPosMessagePluginL()
    {
    // Load the POS Message plugin
    const TUid posMsgPluginImplUid   =   {0x102871EF};
    iSuplPosHandler =  COMASuplPosHandlerBase::NewL(posMsgPluginImplUid);
    }



void CTe_SuplProxyPMSuiteStepBase::SetProtocolModuleSettingsL()
    {
    CLbsAdmin* adminHandle = CLbsAdmin::NewL();
    CleanupStack::PushL(adminHandle);
    
    // Set the implementation Uid of the << suplproxyprotocol module >> plugin
    // to be used for these demo tests.
    const TUid protModuleImplUid = { 0x102871EC };
    
    TInt err = adminHandle->ResetToDefault();
    
    // Make the networkgateway always use the suplproxyprotocolmodule
    err = adminHandle->Set(KLbsSettingHomeProtocolModule, protModuleImplUid);
    User::LeaveIfError(err);
    
    err = adminHandle->Set(KLbsSettingRoamingProtocolModule, protModuleImplUid);
    User::LeaveIfError(err);    
    
    CleanupStack::PopAndDestroy(adminHandle);

    }

TInt CTe_SuplProxyPMSuiteStepBase::SetupLbsAdminSettingsL()
    {
          
    SetProtocolModuleSettingsL();
          
    CLbsAdmin* adminHandle = CLbsAdmin::NewL();
	CleanupStack::PushL(adminHandle);

	// Load all the protocol modules
	TLbsAdminProtocolModuleLoadingInfo loadInfo;
	TInt loadingStrategy = TLbsAdminProtocolModuleLoadingInfo::EProtocolModuleLoadingAll;

	loadInfo.SetLoadingStrategy(loadingStrategy);
	TInt errLoading = adminHandle->Set(KLbsProtocolModuleLoading, loadInfo);
        
	CleanupStack::PopAndDestroy(adminHandle);

	return errLoading;
          
    }



/* Starts the LbsNetGateway executable 
*/
TInt CTe_SuplProxyPMSuiteStepBase::StartNetGateway()
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
void CTe_SuplProxyPMSuiteStepBase::InitialiseInterfacesL()
    {    
    // Initialize SystemController interface
    RLbsProcessSupervisor::InitializeL();
    }

//Shutdown NRH<->NG & SystemController interfaces
void CTe_SuplProxyPMSuiteStepBase::ShutDownInterfacesL()
    {
    // Shutdown system controller interface
    RLbsProcessSupervisor::ShutDownL();
    }


/* Forces the shutdown of the LbsNetGateway executable.
*/
TInt CTe_SuplProxyPMSuiteStepBase::StopNetGateway()
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
        _LIT(KGatewayProcessPattern, "te_sppm_dummynetgateway.exe*");
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

CTe_SuplProxyPMSuiteStepBase::~CTe_SuplProxyPMSuiteStepBase()
	{
	if (iSuplPosHandler)
	    {
	    delete iSuplPosHandler;
	    }
    iSuplPosHandler = NULL;
    REComSession::FinalClose();
	}

CTe_SuplProxyPMSuiteStepBase::CTe_SuplProxyPMSuiteStepBase()
	{
	}



TReal CTe_SuplProxyPMSuiteStepBase::SuplVersion()
    {
    return (static_cast<TReal>(1.0));
    }

void CTe_SuplProxyPMSuiteStepBase::TerminateSession()
    {
    
    }

void CTe_SuplProxyPMSuiteStepBase::SetTestCasePropertyL(const TUid TestServerUid, const TUint KeyType, TInt aIndex)
    {
    // The Property is used the modify the behaviour of the plugins [eg. ref loc plugin] for different
    // test cases
    TInt err = RProperty::Set(TestServerUid, KeyType, aIndex);
    if (err != KErrNone)
        {
        User::Leave(err);
        }
    }

