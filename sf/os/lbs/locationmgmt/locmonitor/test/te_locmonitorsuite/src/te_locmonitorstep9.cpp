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
 @file te_locmonitorstep9.cpp
*/
 
#include "te_locmonitorstep9.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep9::~CTe_LocMonitorStep9()
	{
	}

CTe_LocMonitorStep9::CTe_LocMonitorStep9()
	{
	SetTestStepName(KLocMonitorStep9);
	}

TVerdict CTe_LocMonitorStep9::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// This test checks that a SetLastKnowPosition request issued
// on a subsession not previously opened panics.
//
TVerdict CTe_LocMonitorStep9::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		TPositionInfo position;
		
 		RLbsLocMonitorAreaPositioner areaPositioner;
		// This should panic due to the subsession not being opened 		
 		areaPositioner.SetLastKnownPosition(position);
 		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep9::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
