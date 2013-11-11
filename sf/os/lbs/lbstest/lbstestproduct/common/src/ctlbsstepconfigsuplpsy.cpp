// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This implements a test step for configuring the SUPL PSY.
// The following settings are read from the specified ini file section and
// used to configure the PSY:

// * SuplPm = <UID>
//
//   <UID> : Implementation UIDs of the Protocol Module that the SUPL PSY 
//           should use. A value of 0 means use the default PM (SUPL Proxy).
//   e.g.
//   SuplPm=102869ED
// 
//   This specifies that the SUPL PSY should use a test Protocol Module. 
//


#include <centralrepository.h>
#include "ctlbsstepconfigSuplpsy.h"
#include <lbs/test/tlbsutils.h>

_LIT(KSettingSuplPm, "SuplPm");

const TUint32 KSuplPsyRepository = 0x20026FB7;
const TUint32 KSuplPsyPmUid = 0x00000013;

/**
 * Constructor
 */
CT_LbsStep_ConfigSuplPsy::CT_LbsStep_ConfigSuplPsy(CT_LbsServer& aParent)
    : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_ConfigSuplPsy);
	}


/**
Static Constructor
*/
CT_LbsStep_ConfigSuplPsy* CT_LbsStep_ConfigSuplPsy::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_ConfigSuplPsy(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_LbsStep_ConfigSuplPsy::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Test Preamble. CT_LbsStep_ConfigSuplPsy"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_ConfigSuplPsy::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_ConfigSuplPsy::doTestStepL()"));

    CRepository* repos = CRepository::NewL(TUid::Uid(KSuplPsyRepository));
    CleanupStack::PushL(repos);
    TInt suplPm;
    if (GetHexFromConfig(ConfigSection(), KSettingSuplPm, suplPm))
        {
        if (suplPm == 0)
            {
            INFO_PRINTF1(_L("Setting SUPL PSY to use default protocol module"));
            repos->Delete(KSuplPsyPmUid);
            }
        else 
            {
            INFO_PRINTF2(_L("Setting SUPL PSY to use protocol module %x"),suplPm);
            User::LeaveIfError(repos->Set(KSuplPsyPmUid, suplPm));
            }
        }
    CleanupStack::PopAndDestroy(repos);
        
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_ConfigSuplPsy::doTestStepL()"));
	return TestStepResult();
	}


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_LbsStep_ConfigSuplPsy::doTestStepPostambleL()
    {
    INFO_PRINTF1(_L("Test Postamble. CT_LbsStep_ConfigSuplPsy"));
    return TestStepResult();
    }
