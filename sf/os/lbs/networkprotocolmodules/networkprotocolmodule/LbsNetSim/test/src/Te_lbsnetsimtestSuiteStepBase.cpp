// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_lbsnetsimtestSuiteStepBase.cpp
*/

#include "Te_lbsnetsimtestSuiteStepBase.h"
#include "Te_lbsnetsimtestSuiteDefs.h"

// Device driver constants

TVerdict CTe_lbsnetsimtestSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{

	// process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPreambleL() of the class CTe_lbsnetsimtestSuiteStepBase!"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_lbsnetsimtestSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPostambleL() of the class CTe_lbsnetsimtestSuiteStepBase!"));
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CTe_lbsnetsimtestSuiteStepBase::~CTe_lbsnetsimtestSuiteStepBase()
	{
	delete iAs;
	}

CTe_lbsnetsimtestSuiteStepBase::CTe_lbsnetsimtestSuiteStepBase() :
	iTest(NULL), iGateway(NULL)
	{
	iAs = new (ELeave) CActiveScheduler();
  	CActiveScheduler::Install(iAs);
	}


//
// Test base class
CAOTest_Base::CAOTest_Base(CTe_lbsnetsimtestSuiteStepBase* aBase, TInt aDelay, TBool aStepMode) :
	CActive(EPriorityNormal), iBase(aBase), iDelay(aDelay), iStepMode(aStepMode)
	{
	CActiveScheduler::Add(this);
	}
CAOTest_Base::~CAOTest_Base()
	{
	}
void CAOTest_Base::Activate()
	{
	TRequestStatus* pStatus = &iStatus;
	User::RequestComplete(pStatus, KErrNone);
	SetActive();
	}
void CAOTest_Base::CAOTest_Base::Connected()
	{
	}
void CAOTest_Base::Disconnected()
	{
	}
void CAOTest_Base::NotifyRegisterLcsMoLr(const TDesC&)
	{
	ASSERT(EFalse);
	}
void CAOTest_Base::NotifyReleaseLcsMoLr(TInt)
	{
	ASSERT(EFalse);
	}
void CAOTest_Base::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse&)
	{
	ASSERT(EFalse);
	}
void CAOTest_Base::NotifyMeasurementReportLocation(const TPositionInfo&)
	{
	ASSERT(EFalse);
	}
void CAOTest_Base::NotifyMeasurementControlLocation(const TPositionInfo&, const RLbsAssistanceDataBuilderSet&, const TLbsNetPosRequestQuality&)
	{
	ASSERT(EFalse);
	}
void CAOTest_Base::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup&)
	{
	ASSERT(EFalse);
	}
void CAOTest_Base::NotifyMeasurementReportControlFailure(TInt)
	{
	ASSERT(EFalse);
	}
void CAOTest_Base::NotifyFacilityLcsMoLrResult(TInt, const TPositionInfo&)
	{
	ASSERT(EFalse);
	}
void CAOTest_Base::NotifyError(MLbsNetSimTestObserver::EFunction, TInt)
	{
	ASSERT(EFalse);
	}


//
// Gateway base class
CAOGateway_Base::CAOGateway_Base(CTe_lbsnetsimtestSuiteStepBase* aBase) :
	CActive(EPriorityNormal), iBase(aBase)
	{
	CActiveScheduler::Add(this);
	}
CAOGateway_Base::~CAOGateway_Base()
	{
	}
void CAOGateway_Base::Activate()
	{
	TRequestStatus* pStatus = &iStatus;
	User::RequestComplete(pStatus, KErrNone);
	SetActive();
	}
void CAOGateway_Base::ProcessRegisterLcsLocationNotification(const TLbsExternalRequestInfo&, const TLbsNetPosRequestPrivacy&)
	{
	ASSERT(EFalse);
	}
void CAOGateway_Base::ProcessReleaseLcsLocationNotification(TInt)
	{
	ASSERT(EFalse);
	}
void CAOGateway_Base::ProcessFacilityLcsMoLrResult(TInt, const TPositionInfoBase*)
	{
	ASSERT(EFalse);
	}
void CAOGateway_Base::ProcessReleaseLcsMoLr()
	{
	ASSERT(EFalse);
	}
void CAOGateway_Base::ProcessMeasurementControlLocation(const TPositionInfoBase&, const RLbsAssistanceDataBuilderSet&, const TLbsNetPosRequestQuality&)
	{
	ASSERT(EFalse);
	}
void CAOGateway_Base::ProcessMeasurementControlLocationError(TInt)
	{
	ASSERT(EFalse);
	}
void CAOGateway_Base::ProcessResetAssistanceData(TLbsAssistanceDataGroup /*aMask*/)
	{
	ASSERT(EFalse);
	}
void CAOGateway_Base::ProcessError(TInt)
	{
	ASSERT(EFalse);
	}



