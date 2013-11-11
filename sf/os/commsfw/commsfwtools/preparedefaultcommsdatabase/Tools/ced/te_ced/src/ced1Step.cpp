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
 @file ced1Step.cpp
 @internalTechnology
*/
#include "ced1Step.h"
#include "Te_cedSuiteDefs.h"

#include <e32base.h>
#include <metadatabase.h>
#include <commsdat.h>
#include <commsdattypesv1_1.h>

using namespace CommsDat;
using namespace Meta;

Cced1Step::~Cced1Step()
/**
 * Destructor
 */
	{
	}

Cced1Step::Cced1Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kced1Step);
	}

TVerdict Cced1Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cced1Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    SetTestStepResult(EFail);
    
    /**
     * Creating the session with the Central Repository.
     */
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(cmdbSession);
    
    /**
     * Creating an IAP record in the clients' memory area try to find a given
     * record in the DB. This query should fail.
     */
    
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRecord);
    
    /**
     * Existing RecordIDs in the IAP table are 1, 2, 3 after the append with
     * the CED.
     */
    iapRecord->SetRecordId(3);
    
    TRAPD(leavingError, iapRecord->LoadL(*cmdbSession));
    INFO_PRINTF2(_L("LoadL for IAPRecord returns %d"), leavingError);
    
    if (!leavingError)
    //The LoadL didn't leave, which means that we have a record with
        {	
        if ( TPtrC(KCDTypeNameLANService) == iapRecord->iServiceType.GetL() )
		    if ( TPtrC(KCDTypeNameLANBearer) == iapRecord->iBearerType.GetL() )
		        if ( 3 == iapRecord->iNetworkWeighting )
                    SetTestStepResult(EPass);
        }
    
    CleanupStack::PopAndDestroy(2, cmdbSession);
    
    return TestStepResult();
	}



TVerdict Cced1Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_cedSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
