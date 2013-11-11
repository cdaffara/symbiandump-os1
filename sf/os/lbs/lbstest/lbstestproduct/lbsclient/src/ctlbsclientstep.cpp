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
// @file CT_LbsClientStep.cpp
// This is the class implementation for the LBS Client Test Step Base
// 
//

#include "ctlbsclientstep.h"

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>


#define REFPOS_LAT				52.2
#define REFPOS_LONG				0.2
#define REFPOS_ALT				10
#define REFPOS_HORZ_ACCURACY	100
#define REFPOS_VERT_ACCURACY	100

/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsClientStep::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep::doTestStepPreambleL()"));
	SetTestStepResult(EPass);

	_LIT(KExpectedApiBehaviour, "expected_api_behaviour");
	TInt expectedApiBehaviour;
	if(GetIntFromConfig(ConfigSection(), KExpectedApiBehaviour, expectedApiBehaviour))
		{
		iExpectedApiBehaviour = static_cast<TExpectedApiBehaviour>(expectedApiBehaviour);;
		}
	else
		{
		iExpectedApiBehaviour = EApiVariant1;
		}
	
	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsClientStep::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


CT_LbsClientStep::~CT_LbsClientStep()
	{
	}


CT_LbsClientStep::CT_LbsClientStep(CT_LbsClientServer& aParent) : iParent(aParent)
	{
	}


TInt CT_LbsClientStep::DoNotifyUpdateL(TPositionInfoBase& aPosInfo)
{
	return  GetLocationL(aPosInfo, EDoNotifyPosUpdate);
}

TInt CT_LbsClientStep::DoLastKnownPosL(TPositionInfoBase& aPosInfo)
{
	return GetLocationL(aPosInfo, EDoLastKnownPos);
}


TInt CT_LbsClientStep::GetLocationL(TPositionInfoBase& aPosInfo, TUint32 aNotifyCallType)
{
	// Use wrapper active object for the async call below.
	CT_LbsAsyncWaiter*	waiter = CT_LbsAsyncWaiter::NewL();
	CleanupStack::PushL(waiter);

	// Set requestor, note we use the same values as DUMMY_REQUESTOR so we don't have
	// to include the header file from the test server, to keep code generic when test pos server goes.
		
	// TODO: NOTE for the new server, there is no requirement to SetRequestor so we can
	// remove later.
	User::LeaveIfError(iPositioner.SetRequestor(	CRequestor::ERequestorService,
													CRequestor::EFormatApplication,
													_L("Tom Tom")));

	// Determine the position function to call and get the actual position
	if (EDoNotifyPosUpdate == aNotifyCallType)
		{
		iPositioner.NotifyPositionUpdate(aPosInfo, waiter->iStatus);
		}
	else if (EDoLastKnownPos == aNotifyCallType)
		{
		iPositioner.GetLastKnownPosition(aPosInfo, waiter->iStatus);
		}
		
	else
		User::LeaveIfError(KErrArgument);
		
	// Wait for and process the result.
	waiter->StartAndWait();

	TInt err = waiter->Result();
	if (KErrNone == err)
		{
		INFO_PRINTF1(_L("position obtained successfully"));
	
		const TPositionInfo& posInfo = reinterpret_cast<const TPositionInfo&>(aPosInfo);
		

		TPosition pos;
		posInfo.GetPosition(pos);
		TReal32 lat 	= pos.Latitude();
		TReal32 lng 	= pos.Longitude();
		TReal32 hac  	= pos.HorizontalAccuracy();
		TReal vac		= pos.VerticalAccuracy();

		INFO_PRINTF2(_L("lat: %f"),lat);
		INFO_PRINTF2(_L("lng: %f"),lng);
		INFO_PRINTF2(_L("hac: %f"),hac);
		INFO_PRINTF2(_L("vac: %f"),vac);

		}
	else
		{
		INFO_PRINTF2(_L("position update failed with error %d"), err);
		}

	CleanupStack::PopAndDestroy(waiter);

	return err;
}


	
TInt CT_LbsClientStep::OpenNetSim()
	{
	// Connect to net sim.
	TInt err = iNetSim.ConnectL(NULL);
	if (err)
		{
		return err;
		}


	// Set the reference position, which also be used for any required verification.
	iRefPos.SetCoordinate(REFPOS_LAT, REFPOS_LONG, REFPOS_ALT);
	iRefPos.SetAccuracy(REFPOS_HORZ_ACCURACY, REFPOS_VERT_ACCURACY);
	iRefPos.SetCurrentTime();

	if (!iNetSim.SetReferenceLocation(iRefPos))
		{
		iNetSim.Close();

		return KErrGeneral;
		}	
		

	// Set plugin to use.
	TUid pluginUid;
	if(iParent.iSharedData->iTestModuleInUse)
		{
		pluginUid = TUid::Uid(KSimpleAssistanceDataProviderPluginUidValue);
		}
	else
		{
		pluginUid = TUid::Uid(KSuplAssistanceDataProviderPluginUidValue);
		}
	if (!iNetSim.SetAssistanceDataProvider(pluginUid))
		{
		iNetSim.Close();

		return KErrGeneral;
		}
		
	return err;
	}

void CT_LbsClientStep::SendResetAssistanceData(TLbsAssistanceDataGroup aMask)
	{
	iNetSim.SendResetAssistanceData(aMask);
	}

void CT_LbsClientStep::CloseNetSim()
	{
	iNetSim.Close();	
	}	


