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
// Symbian OS includes
// 
//

#include "Te_EtelPacketTestSteps.h"

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"

#include "testdef.h"

#include "Te_etelpckt_defs.h"

CTestConnectEtelLoadTSY::CTestConnectEtelLoadTSY(RTelServer& aTelServer): iTelServer(aTelServer)
	{
	}

// Do Test step
enum TVerdict CTestConnectEtelLoadTSY::doTestStepL()
	{
	TPtrC tsyName;

	TInt ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed connect phone"));
		User::Leave(ret);
		}

	GetStringFromConfig(ConfigSection(), _L("name"), tsyName);
   	ret=iTelServer.LoadPhoneModule(tsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failed load phone module %S"),&tsyName);
		iTelServer.Close();
		User::Leave(ret);
		}

	return TestStepResult();
	}
