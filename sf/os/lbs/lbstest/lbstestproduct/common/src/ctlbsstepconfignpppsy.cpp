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
// This implements a test step for configuring the NPP PSY.
// The following settings are read from the specified ini file section and
// used to configure the PSY:

// * PsyList = <frozen> <UID1> <UID2> ... <UIDn>
//
//   <frozen> : either 0 or 1, specifying whether the list is frozen or not.
//   <UIDx> : Implementation UIDs of PSY to attempt to use, in hexadecimal form
//            with no leading "0x".
//
//   This specifies an ordered list of implementation UIDs for PSYs that the 
//   NPP PSY should attempt to use.
//
//   e.g. 
//   PsyList=0 101fe98e 20026FB7
// 
//   This specifies an unfrozen list containing the SUPL PSY and the Network PSY. 
//
 
#include <centralrepository.h>
#include "ctlbsstepconfignpppsy.h"
#include <lbs/test/tlbsutils.h>

_LIT(KSettingPsyList, "PsyList");

const TUint32 KNppPsyRepository = 0x10206915;
const TUint32 KNppPsyPsyList = 0x10000001;

/**
 * Constructor
 */
CT_LbsStep_ConfigNppPsy::CT_LbsStep_ConfigNppPsy(CT_LbsServer& aParent)
    : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_ConfigNppPsy);
	}


/**
Static Constructor
*/
CT_LbsStep_ConfigNppPsy* CT_LbsStep_ConfigNppPsy::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_ConfigNppPsy(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_LbsStep_ConfigNppPsy::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Test Preamble. CT_LbsStep_ConfigNppPsy"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_ConfigNppPsy::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_ConfigNppPsy::doTestStepL()"));

    CRepository* repos = CRepository::NewL(TUid::Uid(KNppPsyRepository));
    CleanupStack::PushL(repos);

    TPtrC settingPsyList;
    if (GetStringFromConfig(ConfigSection(), KSettingPsyList, settingPsyList))
        {
        INFO_PRINTF2(_L("Setting NPPs PSY List to '%S'"),&settingPsyList);
        User::LeaveIfError(repos->Set(KNppPsyPsyList, settingPsyList));
        }
    
    CleanupStack::PopAndDestroy(repos);
        
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_ConfigNppPsy::doTestStepL()"));
	return TestStepResult();
	}


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_LbsStep_ConfigNppPsy::doTestStepPostambleL()
    {
    INFO_PRINTF1(_L("Test Postamble. CT_LbsStep_ConfigNppPsy"));
    return TestStepResult();
    }
