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
 @file Te_manSuiteStepBase.cpp
*/

#include "Te_manSuiteStepBase.h"
#include "Te_manSuiteDefs.h"

// Device driver constants

TVerdict CTe_manSuiteStepBase::doTestStepPreambleL()
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
	INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPreambleL() of the class CTe_manSuiteStepBase!"));
	iSched = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iSched);
		
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_manSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.

	// Remove ActiveScheduler
	CActiveScheduler::Install(NULL);
	delete iSched;
	iSched = NULL;
	
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CTe_manSuiteStepBase::~CTe_manSuiteStepBase()
	{
	}

CTe_manSuiteStepBase::CTe_manSuiteStepBase()
	{
	}

void CTe_manSuiteStepBase::SetNGInterface(MNGTestInterface* aNGInterface)
	{
	iNgInterface = aNGInterface;
	}

void CTe_manSuiteStepBase::WaitAndValidateNgMsg(TLbsNetInternalMsgBase::TLbsNetInternalMsgType aMsgType, TBool aFlag)
	{
	iNgInterface->WaitAndValidateNgMsg(aMsgType, aFlag);
	}

void CTe_manSuiteStepBase::SendNgMsg(const TLbsNetInternalMsgBase& aMsg)
	{
	iNgInterface->SendNgMsg(aMsg);
	}

void CTe_manSuiteStepBase::SetupNGInterface()
	{
	iNgInterface->SetupNGInterface();
	}
