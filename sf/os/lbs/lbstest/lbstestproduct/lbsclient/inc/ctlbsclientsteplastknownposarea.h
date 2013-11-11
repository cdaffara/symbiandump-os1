/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file CT_LbsClientStep_LastKnownPosArea.h
* This contains the header file for Last Known Position Area Tests
*
*/


#ifndef __CT_LBS_CLIENT_STEP_LAST_KNOWN_POS_AREA_H__
#define __CT_LBS_CLIENT_STEP_LAST_KNOWN_POS_AREA_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"
#include "ctlbsclientnotifyposupdao.h"
#include <lbs/lbsadmin.h>


// Literals used
_LIT(KLbsClientStep_LastKnownPosArea, "LbsClientStep_LastKnownPosArea");


class CT_LbsClientStep_LastKnownPosArea : public CT_LbsClientStep, MT_NotifyPosUpdateObserver
	{
public:
	~CT_LbsClientStep_LastKnownPosArea();
	static CT_LbsClientStep_LastKnownPosArea* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepL();
    void NotifyPositionUpdateCallback(TRequestStatus& aStatus);
   
private:
	CT_LbsClientStep_LastKnownPosArea(CT_LbsClientServer& aParent);
	void Validate(TPositionInfo& aExpectedPosition, TPositionInfo& aActualPosition, TPositionAreaExtendedInfo& aActualArea, 
			TPositionAreaInfo::TArea aArea, TBool aMccExpected, TBool aMncExpected, TBool aLacExpected, TBool aCidExpected);
	TInt VerifyMatchLevel(TPositionAreaExtendedInfo& aExpected, TPositionAreaExtendedInfo& aActual);
	TInt DoLastKnownPosAreaL(TPositionInfoBase& aPosInfo, TPositionAreaExtendedInfo& aMatchLevel);
	void ConstructL();
	};

#endif //__CT_LBS_CLIENT_STEP_LAST_KNOWN_POS_AREA_H__
