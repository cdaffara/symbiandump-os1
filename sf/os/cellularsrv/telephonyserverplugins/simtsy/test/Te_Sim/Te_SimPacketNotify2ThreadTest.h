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

#ifndef __TE_SIM_PACKET_NOTIFY2THREAD_TEST__
#define __TE_SIM_PACKET_NOTIFY2THREAD_TEST__

#include "Te_SimTestStepBase.h"
#include "Te_SimConstants.h"

class CSimPacketNotify2ThreadTest : public CPhBkAndPacketTestsTestStepBase
	{
public:
	CSimPacketNotify2ThreadTest();
 	virtual TVerdict doTestStepL();

private:
	static TInt GprsThreadEntry(TAny* aPtr);
	static TInt Rel99ThreadEntry(TAny* aPtr);
	void GprsNotifyFunction();
	void Rel99NotifyFunction();

private:
	RPacketContext iGPRSPacketContext;
	RPacketService iGPRSPacketService;
	RPacketContext iRel99PacketContext;
	RPacketService iRel99PacketService;
	
	TBool iMainFlag;
	TBool iThreadFlag;
	RMutex iGprsMutex;
	TBool iNewContextOpened;
	};

#endif // __TE_SIM_PACKET_NOTIFY2THREAD_TEST__
