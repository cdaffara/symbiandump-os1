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
 @file te_locmonitorstep16.cpp
*/
 
#include "te_locmonitorstep16.h"
#include "te_locmonitorsuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTe_LocMonitorStep16::~CTe_LocMonitorStep16()
	{
	}

CTe_LocMonitorStep16::CTe_LocMonitorStep16()
	{
	SetTestStepName(KLocMonitorStep16);
	}

TVerdict CTe_LocMonitorStep16::doTestStepPreambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// This test checks calling
// RLbsLocMonitorSession::WipeOutDatabase 
// results in a request to delete the DB.
TVerdict CTe_LocMonitorStep16::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		TRequestStatus status;

 		RLbsLocMonitorSession locMonSession;
 		User::LeaveIfError(locMonSession.Connect());
 
 		iLocMonDbListener->ListenForLocMonDbFeedback();
 		locMonSession.WipeOutDatabase(status);
 		iLocMonDbListener->WaitForDbFeedback();
		
		TESTL(iLocMonDbListener->iDbData.iDbDeleted);

		User::WaitForRequest(status);
		TESTL(KErrNone == status.Int());
		}

	  return TestStepResult();
	}

TVerdict CTe_LocMonitorStep16::doTestStepPostambleL()
	{
	CTe_LocMonitorStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
