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
// @file ctlbsclientsteplastknownpos.h
// This contains the header file for  Last Known Position Tests
// 
//

#ifndef __CT_LBS_CLIENT_STEP_LAST_KNOWN_POS_H__
#define __CT_LBS_CLIENT_STEP_LAST_KNOWN_POS_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"

// Literals used
_LIT(KLbsClientStep_LastKnownPos, "LbsClientStep_LastKnownPos");

class CT_LbsClientStep_LastKnownPos : public CT_LbsClientStep
	{
public:
	~CT_LbsClientStep_LastKnownPos();

	static CT_LbsClientStep_LastKnownPos* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepL();
	void SwitchOnselfLocateAPIL();
	
private:
    void ConstructL();
    
	CT_LbsClientStep_LastKnownPos(CT_LbsClientServer& aParent);
	};

#endif //__CT_LBS_CLIENT_STEP_LAST_KNOWN_POS_H__
