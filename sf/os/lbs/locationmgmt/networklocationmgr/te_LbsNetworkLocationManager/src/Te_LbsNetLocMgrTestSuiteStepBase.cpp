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
 @file Te_LbsNetLocMgrTestSuiteStepBase.cpp
*/

#include "Te_LbsNetLocMgrTestSuiteStepBase.h"
#include "Te_LbsNetLocMgrTestSuiteDefs.h"
#include "LbsInternalInterface.h"
#include "tprocessstartparams.h"
#include "cprocesslaunch.h"
#include "lbsngnlmint.h"
#include "lbsreffnpint.h"
#include <lbs/lbsextendedsatellite.h>

// Device driver constants
const TInt KTestNetLocManagerUid = 0x10282276;

TVerdict CTe_LbsNetLocMgrTestSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	SetTestStepResult(EPass);
	// process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	RLbsNetworkLocationRequests::InitializeL();
	RLbsNetworkLocationResponses::InitializeL();
	RLbsNetworkPositionUpdates::InitializeL();
	
	// initialise the request bus and the property
	RLbsPositionUpdateRequests::InitializeL();
	
	TPositionExtendedSatelliteInfo satInfo;
	RLbsPositionUpdates::InitializeL(satInfo);

	//temporarily set the process supervisor key in cenrep
	//to point to te_NetworkLocationManager
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	TInt err = rep->Set(KProcessSupervisorCategoryKey, KTestNetLocManagerUid);
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(rep);


	TProcessStartParams params;
	_LIT(KDummyFileName, "\\sys\\bin\\t_lbsDummygateway.exe");
	_LIT(KDummyProcessName, "DummyNetworkGateway");
	params.SetProcessFileName(KDummyFileName);
	params.SetProcessName(KDummyProcessName);
	params.SetRendezvousRequired(ETrue);
	TInt r = CProcessLaunch::ProcessLaunch(params);
	if(r != KErrNone)
		{
		INFO_PRINTF1(_L("Launch DummyNetworkGateway failed."));
		SetTestStepResult(EFail);
		}

	// then launch the netlocmanager.exe
	_LIT(KFileName, "\\sys\\bin\\lbsnetlocmanager.exe");
	_LIT(KProcessName, "lbsnetlocmanager");
	params.SetProcessFileName(KFileName);
	params.SetProcessName(KProcessName);
	params.SetRendezvousRequired(ETrue);
	r = CProcessLaunch::ProcessLaunch(params);
	if(r != KErrNone)
		{
		INFO_PRINTF1(_L("Launch netlocmanager failed."));
		SetTestStepResult(EFail);
		}

	// subscribe to the responding data first
	iPositionUpd.OpenL(KLbsNetLocManagerUid);
	CleanupClosePushL(iPositionUpd);
	iPositionUpd.NotifyPositionUpdate(iStatusUpd);	
	INFO_PRINTF1(_L("Subscribe to NetLocManager succeeded."));	

	const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierLS = 
		{
			{KLbsNetLocManagerUidValue},{KLbsLocServerUidValue}
		};

	//initialize a request to netlocmanager
	iPositionReq.OpenL(KChannelIdentifierLS);
	CleanupClosePushL(iPositionReq);
	INFO_PRINTF1(_L("Exit CTe_LbsNetLocMgrTestSuiteStepBase::doTestStepPreambleL"));	


	return TestStepResult();
	}

TVerdict CTe_LbsNetLocMgrTestSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	
		
	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	iPositionReq.Close();
	iPositionUpd.Close();
	CleanupStack::PopAndDestroy(&iPositionReq);		
	CleanupStack::PopAndDestroy(&iPositionUpd);		
	
	RLbsNetworkPositionUpdates::ShutDownL();
	RLbsNetworkLocationResponses::ShutDownL(); 
	RLbsNetworkLocationRequests::ShutDownL(); 
	RLbsPositionUpdateRequests::ShutDownL(); 
	RLbsPositionUpdates::ShutDownL();
	
	return TestStepResult();
	}

CTe_LbsNetLocMgrTestSuiteStepBase::~CTe_LbsNetLocMgrTestSuiteStepBase()
	{
	//reset the process supervisor key in cenrep
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	TInt err = rep->Set(KProcessSupervisorCategoryKey, TInt(KLbsRootUidValue));
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(rep);
	}

CTe_LbsNetLocMgrTestSuiteStepBase::CTe_LbsNetLocMgrTestSuiteStepBase()
	{
	}
