/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


/**
 @file te_locmonitorstep5.cpp
*/
 
#include "te_locmonitorstep5.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep5::~CTe_LocMonitorStep5()
	{
	}

CTe_LocMonitorStep5::CTe_LocMonitorStep5()
	{
	SetTestStepName(KLocMonitorStep5);
	}

TVerdict CTe_LocMonitorStep5::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// This test checks that attempting to call
// RLbsLocMonitorSession::WipeOutDatabase without
// previously connecting the session results in a panic
//
TVerdict CTe_LocMonitorStep5::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		TRequestStatus status;
 		RLbsLocMonitorSession locMonSession;
 		// This should panic due to the session not
 		// being connected.
		locMonSession.WipeOutDatabase(status); 
		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep5::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
