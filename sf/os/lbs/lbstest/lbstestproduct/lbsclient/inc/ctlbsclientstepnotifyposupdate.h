// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file CT_LbsClientStep_NotifyPosUpdate.h
// This contains the header file for Notify Position Update Tests
// 
//

#ifndef __CT_LBS_CLIENT_STEP_NOTIFY_POS_UPDATE_H__
#define __CT_LBS_CLIENT_STEP_NOTIFY_POS_UPDATE_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"
#include "ctlbsclientnotifyposupdao.h"
#include <lbs/lbsadmin.h>

_LIT(KNumPositions, "num_positions");

// Literals used
_LIT(KLbsClientStep_NotifyPosUpdate, "LbsClientStep_NotifyPosUpdate");



class CT_LbsClientStep_NotifyPosUpdate : public CT_LbsClientStep, MT_NotifyPosUpdateObserver
	{
public:
	~CT_LbsClientStep_NotifyPosUpdate();

	static CT_LbsClientStep_NotifyPosUpdate* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepL();

    void NotifyPositionUpdateCallback(TRequestStatus& aStatus);

   
private:
	CT_LbsClientStep_NotifyPosUpdate(CT_LbsClientServer& aParent);
	
	void ConstructL();
	void TempSetRequestorL();
	
	
private:

	// To allow changing admin settings.
	CLbsAdmin*	iLbsAdmin;
   
	TBool	iRequestedAssData;
	TBool	iReceivedAssData;
	
	};

#endif //__CT_LBS_CLIENT_STEP_NOTIFY_POS_UPDATE_H__
