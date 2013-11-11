// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TE_SIM_PACKET_GPRSREL99NOTIFYPROFILECHANGE_TEST__
#define __TE_SIM_PACKET_GPRSREL99NOTIFYPROFILECHANGE_TEST__

#include "SimConstants.h"
#include "Te_SimTestStepBase.h"
#include "Te_SimConstants.h"

class CSimPacketGPRSRel99NotifyProfileChangeTest : public CPhBkAndPacketTestsTestStepBase
	{
public:
	CSimPacketGPRSRel99NotifyProfileChangeTest();
 	virtual TVerdict doTestStepL();

private:
	RPacketContext iGPRSPacketContext;
	RPacketService iGPRSPacketService;
	RPacketQoS iGPRSPacketqos;
	RPacketContext iRel99PacketContext;
	RPacketService iRel99PacketService;
	RPacketQoS iRel99Packetqos;
	};

#endif // __TE_SIM_PACKET_GPRSREL99NOTIFYPROFILECHANGE_TEST__
