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
// @file ctlbsx3pstepopenclose.h
// This contains the header file for Location Data Source Tests
// 
//

#ifndef __CT_LBS_X3P_SRC_OPENCLOSE_H__
#define __CT_LBS_X3P_SRC_OPENCLOSE_H__

// LBS includes.
#include <lbscommon.h>

// LBS test includes.
#include "ctlbsx3pstep.h"
#include "ctlbsx3pserver.h"


// Literals used
_LIT(KLbsX3PStep_OpenClose, "LbsX3PStep_OpenClose");


class CT_LbsX3PStep_OpenClose : public CT_LbsX3PStep
	{
public:
	~CT_LbsX3PStep_OpenClose();
	void ConstructL();

	static CT_LbsX3PStep_OpenClose* New(CT_LbsX3PServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsX3PStep_OpenClose(CT_LbsX3PServer& aParent);
	};

#endif //__CT_LBS_X3P_SRC_OPENCLOSE_H__