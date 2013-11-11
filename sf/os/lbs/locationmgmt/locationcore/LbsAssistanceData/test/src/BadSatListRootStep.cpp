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
// Example CTestStep derived implementation
// 
//

/**
 @file BadSatListRootStep.cpp
*/
#include "BadSatListRootStep.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define  iBadSatListBuilder iServer.iBadSatListBuilder
#define  iBadSatArrayBuilder iServer.iBadSatArrayReader

#define  iBadSatListReader iServer.iBadSatListReader
#define  iBadSatArrayReader iServer.iBadSatArrayReader

CBadSatListRootStep::~CBadSatListRootStep()
/**
 * Destructor
 */
	{
	}

CBadSatListRootStep::CBadSatListRootStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KBadSatListRootStep);
	}

TVerdict CBadSatListRootStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}


TVerdict CBadSatListRootStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		INFO_PRINTF1(_L("Open root property"));
	  	iBadSatListBuilder.OpenL();
	  	iBadSatListReader.OpenL();

        // Check that the properties are empty
		//
		if(iBadSatListBuilder.FieldExists(TBadSatList::EBadSatIdArray))
			{
			ERR_PRINTF1(_L("Root Builder not empty"));
			SetTestStepResult(EFail);
			}
		if(iBadSatListReader.FieldExists(TBadSatList::EBadSatIdArray))
			{
			ERR_PRINTF1(_L("Root Reader not empty"));
			SetTestStepResult(EFail);			
			}
		}
	  return TestStepResult();
	}


TVerdict CBadSatListRootStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
