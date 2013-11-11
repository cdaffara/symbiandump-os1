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

#include "Te_SimPacketCorruptedConfigFileTest.h"

CSimPacketCorruptedConfigFileTest::CSimPacketCorruptedConfigFileTest() 
	{ 
	SetTestStepName(_L("CorruptedConfigFileTest"));
	}

TVerdict CSimPacketCorruptedConfigFileTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketCorruptedConfigFileTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(4);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet object"));
	TName contextName;
	TName contextNameCompare;
	TESTL(iFirstPrimaryPacketContext.OpenNewContext(iPacketService, contextName)==KErrNone);
	contextNameCompare.Append(KSimPrimaryPacketContextName);
	contextNameCompare.AppendNum(1);
	TESTL(contextName.Compare(contextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened Context object"));

	TName qosName;
	TESTL(iPacketqos.OpenNewQoS(iFirstPrimaryPacketContext, qosName)==KErrNone);

	INFO_PRINTF1(_L("Check that the log config.txt contains 31 instances of 'WARNING ERROR IN CONFIGURATION FILE PARSING.'"));

	iPacketqos.Close();
	iFirstPrimaryPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

