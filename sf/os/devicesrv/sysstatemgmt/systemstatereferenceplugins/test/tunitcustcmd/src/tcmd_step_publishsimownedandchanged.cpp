// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:Unit test case to test CompareToLastStoredImsi()
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "tcmd_step_publishsimownedandchanged.h"
#include "ssmcustomcmdfactory.h"

// class under test
#include "cmdpublishSimOwnedAndChanged.h"

// helper class
#include "activestopper.h"
#include "ssmdebug.h"
#include <ssm/ssmuiproviderdll.h>
#include <ssm/startupdomainpskeys.h>
#include <ssm/starterdomaincrkeys.h>
#include <e32property.h>
#include <centralrepository.h>


/**
* Used for storing the IMSI code of current SIM card. This is needed for
* determining if SIM card has changed from the previous boot.
* Key value is an descriptor containing the IMSI code.
*/
const TUint32 KSSMStoredImsi = 0x00000001;
const TUid KCRUidSSMVals = {0x2001d2aa};

CCustomCmdTestPublishSimOwnedAndChanged::~CCustomCmdTestPublishSimOwnedAndChanged()
    {
	delete iSimCustomCmd;
	}

CCustomCmdTestPublishSimOwnedAndChanged::CCustomCmdTestPublishSimOwnedAndChanged()
    {
    SetTestStepName(KTCCustomCmdTestPublishSimOwnedAndChanged);
    }

TVerdict CCustomCmdTestPublishSimOwnedAndChanged::doTestStepL()
    {
    INFO_PRINTF1(_L("Entering test for publish startup mode custom command"));
    __UHEAP_MARK;
    
    TRAPD(err, doTestRetrieveSubscriberIdFromCentrepL());
    TEST(err == KErrNone);
    
    __UHEAP_MARKEND;
    INFO_PRINTF1(_L("Leaving test for publish startup mode custom command"));
    return TestStepResult();
    }

/**
Test CaseID         DEVSRVS-SSREFPLUGINS-CUSTCMD-31
 */

void CCustomCmdTestPublishSimOwnedAndChanged::doTestRetrieveSubscriberIdFromCentrepL()
    {
    INFO_PRINTF1(_L("Setting the Subscriber ID in Central Repository"));   
       
    _LIT(KSubscriberID1 , "1");
    _LIT(KSubscriberID2 , "2");
    RMobilePhone::TMobilePhoneSubscriberId aFirstValue(KSubscriberID1);
    RMobilePhone::TMobilePhoneSubscriberId aSecondValue(KSubscriberID2);
    DEBUGPRINT2A("Current IMSI: %S", &aFirstValue);
    CRepository* repository = CRepository::NewLC(KCRUidSSMVals);
    User::LeaveIfError(repository->Set(KSSMStoredImsi, aFirstValue));
    INFO_PRINTF1(_L("Retrieving the value from Certrep and setting the Subscriber ID again"));  
    TInt simChangedValue = 0;
    simChangedValue=iSimCustomCmd->CompareToLastStoredImsi(aFirstValue);
    TEST(!simChangedValue);
    INFO_PRINTF1(_L("Setting the Subscriber ID to a new value and comparing with the old one"));
    simChangedValue=iSimCustomCmd->CompareToLastStoredImsi(aSecondValue);
    TEST(simChangedValue);
    CleanupStack::PopAndDestroy(repository);
    }

