// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// lbsbtgpsconfig unit test step: Test no access is permitted without required capabilities
//

#include "te_lbsbtgpsconfigstepnoaccess.h"


CLbsBtGpsConfigStepNoAccess::CLbsBtGpsConfigStepNoAccess()
	{
	SetTestStepName(KLbsBtGpsConfigStepNoAccess);
	}


CLbsBtGpsConfigStepNoAccess::~CLbsBtGpsConfigStepNoAccess()
	{
	
	}


TVerdict CLbsBtGpsConfigStepNoAccess::doTestStepPreambleL(void)
	{
	CLbsBtGpsConfigStepBase::doTestStepPreambleL();
	
	// Do not open the config API here, do it later within the test step
	iConfig = NULL;
	
	return TestStepResult();
	}


TBool CLbsBtGpsConfigStepNoAccess::RunTestStage(TInt aStage)
	{
	TInt err;
	switch(aStage)
		{

		case 1:
			TRAP(err, iConfig = CLbsBtGpsConfig::NewL(*this));
			if (err != KErrPermissionDenied)
				{
				INFO_PRINTF2(_L("Should not have been allowed to open the API, err=%d"),err);
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF1(_L("Access to BT GPS Config API was not permitted"));
				}
			if (iConfig != NULL)
				{
				INFO_PRINTF1(_L("iConfig should have returned NULL"));
				SetTestStepResult(EFail);
				delete iConfig;
				iConfig = NULL;
				}
			return ETrue;

		case 2:
			// Check that we DID NOT get an update notification following the previous stage
			if (iUpdatesRecieved != 0)
				{
				INFO_PRINTF1(_L("Unexpected update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;
		}
	
	return EFalse;
	}
