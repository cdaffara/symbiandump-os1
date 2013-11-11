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
// @file ctlbsclientstep_reset_assistance.h
// 
//

#ifndef __CT_LBS_CLIENT_STEP_RESET_ASSISTANCE_H__
#define __CT_LBS_CLIENT_STEP_RESET_ASSISTANCE_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"
#include "ctlbsclientnotifyposupdao.h"
#include <lbs/lbsadmin.h>

// Literals used
_LIT(KLbsClientStep_ResetAssistance, "LbsClientStep_ResetAssistance");



class CT_LbsClientStep_ResetAssistance : public CT_LbsClientStep, MT_NotifyPosUpdateObserver
	{
public:
	~CT_LbsClientStep_ResetAssistance();

	static CT_LbsClientStep_ResetAssistance* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepL();

    void NotifyPositionUpdateCallback(TRequestStatus& aStatus);

   
private:
	CT_LbsClientStep_ResetAssistance(CT_LbsClientServer& aParent);
	
	void ConstructL();
	void TempSetRequestorL();
	TInt64 DoRequests(TBool aCold, TBool aReset);
	
	
private:

	// To allow changing admin settings.
	CLbsAdmin*	iLbsAdmin;
   
	TBool	iRequestedAssData;
	TBool	iReceivedAssData;
	
	};

#endif //__CT_LBS_CLIENT_STEP_RESET_ASSISTANCE_H__
