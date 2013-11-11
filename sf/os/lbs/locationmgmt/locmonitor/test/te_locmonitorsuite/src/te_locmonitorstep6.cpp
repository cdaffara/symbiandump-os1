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
 @file te_locmonitorstep6.cpp
*/
 
#include "te_locmonitorstep6.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep6::~CTe_LocMonitorStep6()
	{
	}

CTe_LocMonitorStep6::CTe_LocMonitorStep6()
	{
	SetTestStepName(KLocMonitorStep6);
	}

TVerdict CTe_LocMonitorStep6::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// Check that a subsession can't be opened
// if the session wasn't connected first
//
TVerdict CTe_LocMonitorStep6::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		TRequestStatus status;
 		RLbsLocMonitorSession locMonSession;
 		
 		// This should panic due to the session not
 		// being connected.
 		RLbsLocMonitorAreaPositioner areaPositioner;
 		areaPositioner.OpenL(locMonSession);
		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep6::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
