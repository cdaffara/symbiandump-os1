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

#ifndef __CT_LBS_HYBRID_UE_ASSISTED_X3P_NO_GPS_H__
#define __CT_LBS_HYBRID_UE_ASSISTED_X3P_NO_GPS_H__

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
_LIT(KLbsHybridUEAssistedX3PNoGPS, "LbsHybridUEAssistedX3PNoGPS");

class CT_LbsHybridUEAssistedX3PNoGPS :	public	CT_LbsHybridX3PStep
	{
public:
	~CT_LbsHybridUEAssistedX3PNoGPS();

	static CT_LbsHybridUEAssistedX3PNoGPS* New(CT_LbsHybridX3PServer& aParent);
	virtual TVerdict doTestStepL();

private:
	CT_LbsHybridUEAssistedX3PNoGPS(CT_LbsHybridX3PServer& aParent);
	void ConstructL();		

private:
	TLbsNetSessionId iSessionId;
    CNetProtocolProxy* iProxy;
	RLbsTransmitPositionServer iServer;
	RLbsTransmitPosition iTransmitter;
	};

#endif //__CT_LBS_HYBRID_UE_ASSISTED_X3P_NO_GPS_H__
