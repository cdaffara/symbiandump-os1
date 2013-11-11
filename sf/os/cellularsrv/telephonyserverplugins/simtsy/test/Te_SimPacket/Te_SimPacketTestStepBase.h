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

#ifndef __TE_SIMPACKET_TESTSTEP_BASE__
#define __TE_SIMPACKET_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <etelpckt.h>

#include <e32test.h>
#include <c32comm.h>
#include <e32keys.h>
#include <f32file.h>

#include <etel.h>
#include <etelmm.h>
#include <mmretrieve.h>
#include <pcktcs.h>
#include <etelqos.h>
#include <etelsat.h>
#include <pcktlist.h>
#include "SimTsy.h"

_LIT(KSimtsyName,"SIM");

class CSimPacketTestStepBase : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	static void CleanUp(TAny *aPtr) ;
protected:
	operator TCleanupItem() ;
	void TestConfigParamsL(RPacketQoS::TQoSGPRSNegotiated& aArg1, RPacketQoS::TQoSGPRSNegotiated& aArg2) ;
	void TestConfigParamsL(RPacketContext::TContextConfigGPRS& aArg1, RPacketContext::TContextConfigGPRS& aArg2);
    void SetSimtsyTestNumberL(TInt aTestNumber);
   	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds);

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	RPacketService iPacketService;
	RPacketContext iPacketContext[11]; //SIMTSY supports only 11 contexts
	};

#endif // __TE_SIMPACKET_TESTSTEP_BASE__
