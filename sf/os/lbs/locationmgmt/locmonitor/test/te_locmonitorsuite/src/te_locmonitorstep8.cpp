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
 @file te_locmonitorstep8.cpp
*/
 
#include "te_locmonitorstep8.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep8::~CTe_LocMonitorStep8()
	{
	}

CTe_LocMonitorStep8::CTe_LocMonitorStep8()
	{
	SetTestStepName(KLocMonitorStep8);
	}

TVerdict CTe_LocMonitorStep8::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// This test checks that a GetLastKnowPositionArea request issued
// on a subsession not previously opened panics.
//
TVerdict CTe_LocMonitorStep8::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		TPositionInfo position;
 		TPositionAreaInfo posAreaInfo;
 		TPosAreaReqParams posAreaReqParams;
 		TRequestStatus status;
 		
 		RLbsLocMonitorAreaPositioner areaPositioner;
		// This should panic due to the subsession not being opened 		
 		areaPositioner.GetLastKnownPositionArea(position,posAreaInfo,posAreaReqParams,status);
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep8::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
