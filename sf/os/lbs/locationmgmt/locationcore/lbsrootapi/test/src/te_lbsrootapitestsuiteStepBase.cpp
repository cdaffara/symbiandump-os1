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
 @file Te_LbsRootApiTestSuiteStepBase.cpp
*/

#include "te_lbsrootapitestsuiteStepBase.h"
#include "te_lbsrootapiTestSuiteDefs.h"
#include "lbsnetinternalapi.h"
#include "LbsInternalInterface.h"
#include "lbsrootcenrepdefs.h"
#include "tprocessstartparams.h"
#include "cprocesslaunch.h"

// Device driver constants


TVerdict CTe_LbsRootApiTestSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{

	// Initialise the properties here, as this needs to be done for every test step.
	INFO_PRINTF1(_L("Initializing interfaces"));

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LbsRootApiTestSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("In Postamble. Check cenrep is OK"));

	return TestStepResult();
	}

CTe_LbsRootApiTestSuiteStepBase::~CTe_LbsRootApiTestSuiteStepBase()
	{
    iPanicRequest.Close();
    iPanicOnCloseDownRequest.Close();
    iSecondCloseDownRequest.Close();	
	}

CTe_LbsRootApiTestSuiteStepBase::CTe_LbsRootApiTestSuiteStepBase()
	{
	}

/**
 * Read in whatever is in the cenrep file so we can check at the
 * at end of the test that it has been restored.
 * Do this by using the 'partial match' meshanism to read the Ids first
 * of all entries which match a pattern (unlikely to be any) then of 
 * all which don't match it. Doing this for int, real, 8- and 16- bit
 * strings means that we end up with a list of all the Ids in 
 * the cenrep file.
 * We then get the values, and store them so that they can be checked 
 * when the cenrep file is restored at the end of the test.
 */

void CTe_LbsRootApiTestSuiteStepBase::SetTestRepositorySettingsL(TInt aTestStep)
	{
	CRepository* rep = CRepository::NewL(KLbsCenRepUid);
	CleanupStack::PushL(rep);
	
	// clean out any existing entries
	TUint32 errorKey;
	User::LeaveIfError(rep->Delete(KLbsSystemTransientKey));
	User::LeaveIfError(rep->Delete(KLbsRootProcessTransientKey));
	User::LeaveIfError(rep->Delete(KLbsRootProcessNameFirstKey, 0xFFFFF000, errorKey));
	User::LeaveIfError(rep->Delete(KLbsRootProcessUidFirstKey, 0xFFFFF000, errorKey));
	User::LeaveIfError(rep->Delete(KLbsRootProcessIsServerFirstKey, 0xFFFFF000, errorKey));
	
	// For testing, set LbsRoot to 'transient' mode
	User::LeaveIfError(rep->Set(KLbsRootProcessTransientKey, EFalse));
	User::LeaveIfError(rep->Set(KLbsSystemTransientKey, ETrue));
	
	// set the entries for the 'dummy' processes to 
	// be used for the test
	TUint nameKey = KLbsRootProcessNameFirstKey;
	TUint uidKey = KLbsRootProcessUidFirstKey;
	TUint isProcessKey = KLbsRootProcessIsServerFirstKey;
	
	switch(aTestStep)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			default:
				{
				User::LeaveIfError(rep->Set(nameKey++, KNrhName));
				User::LeaveIfError(rep->Set(uidKey++, TInt(KNrhUid.iUid)));
				User::LeaveIfError(rep->Set(isProcessKey++, EFalse));
	
				User::LeaveIfError(rep->Set(nameKey++, KAgpsName));
				User::LeaveIfError(rep->Set(uidKey++, TInt(KAgpsUid.iUid)));
				User::LeaveIfError(rep->Set(isProcessKey++, EFalse));
	
				User::LeaveIfError(rep->Set(nameKey++, KNgName));
				User::LeaveIfError(rep->Set(uidKey++, TInt(KNgUid.iUid)));
				User::LeaveIfError(rep->Set(isProcessKey++, EFalse));
				break;
				}
// cases 10-18 yet to be ported 				
			case 19:
				{
				// Force a panic on the first process to be started
				User::LeaveIfError(rep->Set(nameKey++, KPanickingProcessName));
				User::LeaveIfError(rep->Set(uidKey++, TInt(KPanickingUid.iUid)));
				User::LeaveIfError(rep->Set(isProcessKey++, EFalse));
	
				User::LeaveIfError(rep->Set(nameKey++, KAgpsName));
				User::LeaveIfError(rep->Set(uidKey++, TInt(KAgpsUid.iUid)));
				User::LeaveIfError(rep->Set(isProcessKey++, EFalse));
	
				User::LeaveIfError(rep->Set(nameKey++, KNgName));
				User::LeaveIfError(rep->Set(uidKey++, TInt(KNgUid.iUid)));
				User::LeaveIfError(rep->Set(isProcessKey++, EFalse));
				break;			
				}
		
			case 20:
				{
				// Force a panic on the second process to be started
				User::LeaveIfError(rep->Set(nameKey++, KNrhName));
				User::LeaveIfError(rep->Set(uidKey++, TInt(KNrhUid.iUid)));
				User::LeaveIfError(rep->Set(isProcessKey++, EFalse));

				User::LeaveIfError(rep->Set(nameKey++, KPanickingProcessName));
				User::LeaveIfError(rep->Set(uidKey++, TInt(KPanickingUid.iUid)));
				User::LeaveIfError(rep->Set(isProcessKey++, EFalse));
	
				User::LeaveIfError(rep->Set(nameKey++, KNgName));
				User::LeaveIfError(rep->Set(uidKey++, TInt(KNgUid.iUid)));
				User::LeaveIfError(rep->Set(isProcessKey++, EFalse));
				break;			
				}
		
		}
	
	CleanupStack::PopAndDestroy(rep);
	}
	
void CTe_LbsRootApiTestSuiteStepBase::StartLbsRootL()
	{
   	TProcessStartParams params;
   	_LIT(KLbsRootFileName, "\\sys\\bin\\lbsroot.exe");
   	_LIT(KLbsRootProcessName, "lbsroot.exe");
   	params.SetProcessFileName(KLbsRootFileName);
   	params.SetProcessName(KLbsRootProcessName);
   	params.SetRendezvousRequired(ETrue);
   	TInt r = CProcessLaunch::ProcessLaunch(params);
   	User::LeaveIfError(r);	
	}
	
TInt CTe_LbsRootApiTestSuiteStepBase::KillLbsRoot()
	{
	TInt err(KErrNone);
	_LIT(KMatchPattern, "lbsroot*");
	TFindProcess findProc(KMatchPattern);
	TFullName fullName;
	err = findProc.Next(fullName);
	if (KErrNone != err)
		{
		return err;
		}
		
	RProcess root;
	err = root.Open(fullName);
	if (KErrNone == err)
		{
		root.Kill(KErrNone);				
		root.Close();
		}
	return err;	
	}
