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
 @file Te_loopbackSuiteStepBase.cpp
 @internalTechnology
*/

#include "Te_loopbackSuiteStepBase.h"
#include "Te_loopbackSuiteDefs.h"

// Device driver constants

TVerdict CTe_loopbackSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{

	for(TInt i = 0; i < KSupportedPorts; i++)
		{
		iPortList[i].name = AllocPortNameL(i);
		}

	SetTestStepResult(EPass);

	TestErrorCodeL( iCommServer.Connect(), _L("Connect to comm server") );
	TestErrorCodeL( iCommServer.LoadCommModule(KCsyName), _L("Loading comm module") );

	return TestStepResult();
	}

TVerdict CTe_loopbackSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	for(TInt i = 0; i < KSupportedPorts; i++)
		{
		delete iPortList[i].name;
		}

	iCommServer.Close();

	SetTestStepResult(EPass);
	return TestStepResult();
	}

CTe_loopbackSuiteStepBase::~CTe_loopbackSuiteStepBase()
	{
	}

CTe_loopbackSuiteStepBase::CTe_loopbackSuiteStepBase()
	{
	}

HBufC* CTe_loopbackSuiteStepBase::AllocPortNameL(TInt aIndex)
	{
	HBufC* buf = HBufC::NewLC(KCommPortFmtStr().Length());
	buf->Des().Format(KCommPortFmtStr, aIndex);

	CleanupStack::Pop();
	return buf;
	}

void CTe_loopbackSuiteStepBase::TestErrorCodeL(TInt aErrCode, const TDesC& aMessage)
	{	
	TestErrorCodeL(aErrCode, KErrNone, aMessage);
	}

void CTe_loopbackSuiteStepBase::TestErrorCodeL(TInt aErrCode, TInt aExpectedErrCode, const TDesC& aMessage)
	{	
	if(aExpectedErrCode == aErrCode)
		{
		INFO_PRINTF3(_L("[%S]. err[%d], as expected. OK."), &aMessage, aErrCode);
		}
	else
		{
		ERR_PRINTF5(_L("Failed: [%S]. err[%d], expected [%d]. Leaving with [%d])."), &aMessage, aErrCode, aExpectedErrCode, aErrCode);
		User::Leave(aErrCode);
		}
	}

TBool CTe_loopbackSuiteStepBase::IsEven(TInt aVal)
	{
		return !(aVal % 2);
	}


TVerdict CTe_loopbackSuiteStepBase::OpenAllAvailablePortsL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_loopbackSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();

	TBuf<50> msg;
	_LIT(KMsgFmt,"Opening comm port <%d>");
	for(TInt i = 0; i < KSupportedPorts; i++)
		{
		msg.Format(KMsgFmt, i);
		TestErrorCodeL(iPortList[i].port.Open(iCommServer, iPortList[i].name->Des(), ECommExclusive, ECommRoleDCE), msg);
		}
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTe_loopbackSuiteStepBase::CloseAllAvailablePortsL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("closing ports..."));
	for(TInt i = 0; i < KSupportedPorts; i++)
		{
		iPortList[i].port.Close();
		}
	INFO_PRINTF1(_L("\t...done"));

	CTe_loopbackSuiteStepBase::doTestStepPostambleL();
	SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

