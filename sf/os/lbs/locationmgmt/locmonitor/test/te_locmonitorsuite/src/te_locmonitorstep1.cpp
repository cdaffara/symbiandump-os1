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
 @file te_locmonitorstep1.cpp
*/
 
#include "te_locmonitorstep1.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep1::~CTe_LocMonitorStep1()
	{
	}

CTe_LocMonitorStep1::CTe_LocMonitorStep1()
	{
	SetTestStepName(KLocMonitorStep1);
	}

TVerdict CTe_LocMonitorStep1::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// This test checks that it is possible to connect a session and
// subsession with the Location Monitor and then close both.
//
TVerdict CTe_LocMonitorStep1::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		
 		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect());
 		
 		RLbsLocMonitorAreaPositioner areaPositioner;
 		areaPositioner.OpenL(locMonSession);
 		
 		areaPositioner.Close();
 		locMonSession.Close();
 		
 		// Reopen the session and subsession
 		User::LeaveIfError(locMonSession.Connect());
 		areaPositioner.OpenL(locMonSession);

 		areaPositioner.Close();
 		locMonSession.Close(); 		
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep1::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
