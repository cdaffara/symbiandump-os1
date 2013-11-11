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
// Te_SimPacketR99R5QOSTest.h
// 
//

#ifndef __TE_SIM_PACKET_R99R5QOS_TEST__
#define __TE_SIM_PACKET_R99R5QOS_TEST__

#include "SimConstants.h"
#include "Te_SimTestStepBase.h"
#include "Te_SimConstants.h"

class CSimPacketRel99Rel5QOSTest : public CPhBkAndPacketTestsTestStepBase
	{
public:
	CSimPacketRel99Rel5QOSTest();
 	virtual TVerdict doTestStepL();

private:
	void TestEquals(const RPacketQoS::TQoSR99_R4Negotiated& aProfile1, 
					const RPacketQoS::TQoSR99_R4Negotiated& aProfile2);

	RPacketContext iFirstPrimaryPacketContext;
	RPacketService iPacketService;
	RPacketQoS iRel99Packetqos;
	RPacketQoS iRel5Packetqos;
	};

#endif // __TE_SIM_PACKET_R99R5QOS_TEST__
