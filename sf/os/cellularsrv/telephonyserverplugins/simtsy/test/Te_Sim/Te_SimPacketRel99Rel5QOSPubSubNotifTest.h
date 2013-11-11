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
//

#ifndef __TE_SIM_PACKET_REL99REL5QOSPUBSUBNOTIF_TEST__
#define __TE_SIM_PACKET_REL99REL5QOSPUBSUBNOTIF_TEST__

#include "SimConstants.h"
#include "Te_SimTestStepBase.h"
#include "Te_SimConstants.h"

class CSimPacketRel99Rel5QOSPubSubNotifTest : public CPhBkAndPacketTestsTestStepBase
	{
public:
	CSimPacketRel99Rel5QOSPubSubNotifTest();
 	virtual TVerdict doTestStepL();

private:
	RPacketContext iFirstPrimaryPacketContext;
	RPacketService iPacketService;
	RPacketQoS iRel99Packetqos;
	RPacketQoS iRel5Packetqos;
	};

#endif // __TE_SIM_PACKET_REL99REL5QOSPUBSUBNOTIF_TEST__
