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
// @file CT_LbsClientStep_SrvConnectClose.h
// This contains the header file for Server Connect Close Tests
// 
//

#ifndef __CT_LBS_CLIENT_STEP_SRV_CONNECT_CLOSE_H__
#define __CT_LBS_CLIENT_STEP_SRV_CONNECT_CLOSE_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"

// Literals used
_LIT(KLbsClientStep_SrvConnectClose, "LbsClientStep_SrvConnectClose");

class CT_LbsClientStep_SrvConnectClose : public CT_LbsClientStep
	{
public:
	~CT_LbsClientStep_SrvConnectClose();

	static CT_LbsClientStep_SrvConnectClose* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsClientStep_SrvConnectClose(CT_LbsClientServer& aParent);
	};

#endif //__CT_LBS_CLIENT_STEP_SRV_CONNECT_CLOSE_H__
