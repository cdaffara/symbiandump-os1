// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __CT_LBS_X3P_ACC_REF_POS_H__
#define __CT_LBS_X3P_ACC_REF_POS_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsx3p.h>

// LBS test includes.
#include "ctlbshybridx3pstep.h"
#include "ctlbshybridx3pserver.h"
#include <lbs/test/lbsnetprotocolproxy.h>

// Literals used
_LIT(KLbsX3pAccurateReferencePosition, "LbsX3pAccurateReferencePosition");

class CT_LbsX3pAccRefPos :	public	CT_LbsHybridX3PStep
	{
public:
	~CT_LbsX3pAccRefPos();

	static CT_LbsX3pAccRefPos* New(CT_LbsHybridX3PServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsX3pAccRefPos(CT_LbsHybridX3PServer& aParent);
	void ConstructL();		

private:
	TLbsNetSessionId iSessionId;
    CNetProtocolProxy* iProxy;
	RLbsTransmitPositionServer iServer;
	RLbsTransmitPosition iTransmitter;
	};

#endif //__CT_LBS_X3P_ACC_REF_POS_H__
