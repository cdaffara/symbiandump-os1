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

#ifndef __TE_GPRS_TESTSTEP_BASE__
#define __TE_GPRS_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <in_sock.h>
#include <etelpckt.h>

#include "Te_Gprscfgfile.h"

_LIT(KtsyName,"MM");
_LIT(KPhoneName,"GsmPhone1");
_LIT(KTGPRSConfigFile,"Te_gprs.ini"); 

class CGprsTestStep : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	void TestConfigParams(RPacketContext::TContextConfigGPRS& aArg1, RPacketContext::TContextConfigGPRS& aArg2);

protected:
	RTelServer iTelServer;
	RPhone iPhone;
	RSocketServ iSocketServer;
	RConnection	iConnection;
	CConfigParams* iTestConfigParam;
	RPacketService iPacketService;
	RPacketContext iPacketContext[20];

private:
	CActiveScheduler* iTestScheduler;
	};

#endif // __TE_GPRS_TESTSTEP_BASE__
