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

#include "Te_SimDataTestStepBase.h"

TVerdict CSimDataTestStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, 0);
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed resetting"));
		User::Leave(ret);
		}

	ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KSimtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}

	ret=iPhone.Open(iTelServer,KPhoneName) ;
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open phone module"));
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}
	ret=iPhone.Initialise();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to initialise the phone"));
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}

	RPhone::TCaps phoneCaps;
	ret = iPhone.GetCaps(phoneCaps) ;
	if ((KErrNone != ret) || !(phoneCaps.iFlags & RPhone::KCapsData))
		{
		INFO_PRINTF2(_L("This phone does not support data calls (caps=0x%x)"), phoneCaps.iFlags);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}

	return TestStepResult();
	}

TVerdict CSimDataTestStepBase::doTestStepPostambleL()
	{
	iPhone.Close();
	iTelServer.UnloadPhoneModule(KSimtsyName);
	iTelServer.Close();

	__UHEAP_MARKEND;
	return TestStepResult();
	}
