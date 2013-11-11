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
// @file CT_LbsClientUpdateOptions.h
// This contains the header file for  Update Options Tests
// 
//

#ifndef __CT_LBS_CLIENT_UPDATEOPTIONS_H__
#define __CT_LBS_CLIENT_UPDATEOPTIONS_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"
#include <lbs/lbsadmin.h>


// Literals used
_LIT(KLbsClientStep_UpdateOptions, "LbsClientStep_UpdateOptions");

class CT_LbsClientStep_UpdateOptions : public CT_LbsClientStep
	{
public:
	~CT_LbsClientStep_UpdateOptions();

	static CT_LbsClientStep_UpdateOptions* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepL();
		
private:
	CT_LbsClientStep_UpdateOptions(CT_LbsClientServer& aParent);
	
	void CT_LbsClientStep_UpdateOptions::TempSetRequestorL();
	TBool CT_LbsClientStep_UpdateOptions::CompareUpdateOptions(TPositionUpdateOptions aPosA, TPositionUpdateOptions aPosB);
	
	TBool VerifyFirstPosTime(const TTime& aTimeRequested, const TPositionInfo& aPosInfo, const TPositionUpdateOptions& aUpdateOpts);
	TBool VerifyPosTime(const TTime& aTimeFirstPosReceived, const TPositionUpdateOptions& aUpdateOpts, TPositionInfo& aPosInfo, TInt aPosCount);

	
private:

	// to allow changing admin settings:
	CLbsAdmin*	iLbsAdmin;

	TBool	iRequestedAssData;
	TBool	iReceivedAssData;
		
	};

#endif //__CT_LBS_CLIENT_UPDATEOPTIONS_H__
