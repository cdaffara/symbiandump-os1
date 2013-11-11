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
// Example CTestStep derived implementation
// 
//

/**
 @file LbsConnectDisconnectTest.cpp
*/
#include "lbsregisterlcsmolrtestnotestapi.h"
#include "Te_lbsnetsimtestSuiteDefs.h"


_LIT(KDestination, "");


CLbsRegisterLcsMoLrTestNoTestAPI::~CLbsRegisterLcsMoLrTestNoTestAPI()
/**
 * Destructor
 */
	{
	}

CLbsRegisterLcsMoLrTestNoTestAPI::CLbsRegisterLcsMoLrTestNoTestAPI()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KRegisterLcsMoLrNoTestApiTest);
	}

TVerdict CLbsRegisterLcsMoLrTestNoTestAPI::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	__UHEAP_MARK;
	
	iState = EStart;
	iGateway = new (ELeave) CAOGateway(this); // Stopped during test
	iState = EGatewayRegisterLcsMoLr;
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsRegisterLcsMoLrTestNoTestAPI::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		iGateway->Activate();
		iAs->Start();

		if (iGateway->Finished() && (State() == EFinished))
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}
		}
		
	  return TestStepResult();
	}



TVerdict CLbsRegisterLcsMoLrTestNoTestAPI::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	delete iGateway;
	
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
	
//
// CAOGateway
CLbsRegisterLcsMoLrTestNoTestAPI::CAOGateway::CAOGateway(CLbsRegisterLcsMoLrTestNoTestAPI* aBase) :
	CAOGateway_Base(aBase),
	iDestination(KDestination), iFacilityLcsMoLrResult(EFalse), iMeasurementControlLocation(EFalse)
	{
	iLbsNetSim = new(ELeave) RLbsNetSim();
	iLbsNetSim->ConnectL(this);
	}
	
CLbsRegisterLcsMoLrTestNoTestAPI::CAOGateway::~CAOGateway()
	{
	Cancel();

	iLbsNetSim->Close();
	delete iLbsNetSim;
	}
	
void CLbsRegisterLcsMoLrTestNoTestAPI::CAOGateway::RunL()
	{
	switch (iBase->State())
		{
		case EError:
			{
			ASSERT(EFalse);
			}
		case EGatewayRegisterLcsMoLr:
			{
			iLbsNetSim->RegisterLcsMoLrL(iDestination);
			iBase->SetState(EError);
			break;
			}
		case EGatewayMeasurementReportLocation:
			{
			TPositionInfo position;
			iLbsNetSim->MeasurementReportLocationL(position);
			iBase->SetState(EError);
			break;
			}
		case EGatewayReleaseLcsMoLr:
			{
			iLbsNetSim->ReleaseLcsMoLrL(KErrNone);
			iBase->SetState(EFinished);
			Activate();
			break;
			}
		case EFinished:
			{
			CActiveScheduler::Stop();
			break;
			}
			
		} // switch
	}
	
void CLbsRegisterLcsMoLrTestNoTestAPI::CAOGateway::DoCancel()
	{
	ASSERT(EFalse);
	}
	
TBool CLbsRegisterLcsMoLrTestNoTestAPI::CAOGateway::Finished()
	{
	return (iFacilityLcsMoLrResult && iMeasurementControlLocation);
	}
	
void CLbsRegisterLcsMoLrTestNoTestAPI::CAOGateway::ProcessFacilityLcsMoLrResult(TInt, const TPositionInfoBase*)
	{
	iFacilityLcsMoLrResult = ETrue;
	iBase->SetState(EGatewayReleaseLcsMoLr);
	Activate();
	}
void CLbsRegisterLcsMoLrTestNoTestAPI::CAOGateway::ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	iMeasurementControlLocation = ETrue;
	iBase->SetState(EGatewayMeasurementReportLocation);
	Activate();
	}
