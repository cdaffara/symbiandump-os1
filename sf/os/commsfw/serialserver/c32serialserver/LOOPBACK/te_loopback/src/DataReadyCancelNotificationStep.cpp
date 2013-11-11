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
 @file DataReadyCancelNotificationStep.cpp
 @internalTechnology
*/
#include "DataReadyCancelNotificationStep.h"
#include "Te_loopbackSuiteDefs.h"

CDataReadyCancelNotificationStep::~CDataReadyCancelNotificationStep()
/**
 * Destructor
 */
	{
	}

CDataReadyCancelNotificationStep::CDataReadyCancelNotificationStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KDataReadyCancelNotificationStep);
	}

TVerdict CDataReadyCancelNotificationStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return OpenAllAvailablePortsL();
	}


TVerdict CDataReadyCancelNotificationStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		_LIT8(KTestOutBuf,"test message");
		HBufC* InBuf = HBufC::NewLC(KTestOutBuf().Length() / 2);
		TPtr ptr = InBuf->Des();
		TPtr8 inBuf(ptr.Collapse());
		
		TBuf<50> msg;
		_LIT(KOperReading, "Data available request canceled in port <%d>");
		_LIT(KOperWriting, "Written port <%d>");
		TRequestStatus status;
		for(TInt i = 0; i < KSupportedPorts; i++)
			{
			if(IsEven(i))
				{
				iPortList[i].port.Write(status, KTestOutBuf);
				msg.Format(KOperWriting, i);
				User::WaitForRequest(status);
				TestErrorCodeL(status.Int(), msg);
				}
			else
				{
				iPortList[i].port.NotifyDataAvailableCancel();
				msg.Format(KOperReading, i);
				}
			}
		CleanupStack::PopAndDestroy();

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict CDataReadyCancelNotificationStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return CloseAllAvailablePortsL();
	}

