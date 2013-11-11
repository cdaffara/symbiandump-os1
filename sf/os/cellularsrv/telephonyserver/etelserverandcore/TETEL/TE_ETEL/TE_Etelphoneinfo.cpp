// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TE_EtelBase.h"
#include "TE_Etelphoneinfo.h"

CTestPhoneInfo::CTestPhoneInfo()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestPhoneInfo"));
	}

enum TVerdict CTestPhoneInfo::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Phone Info"));

    RTelServer server;
	TInt ret = server.Connect();
    TESTCHECK(ret, KErrNone);

	TInt  originalNumPhones;
	ret = server.EnumeratePhones(originalNumPhones);
    TESTCHECK(ret, KErrNone);

   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	TInt numPhones;
	ret=server.EnumeratePhones(numPhones);
    TESTCHECK(ret, KErrNone);
	INFO_PRINTF2(_L("EnumeratePhones returned %d"),numPhones);

	INFO_PRINTF4(_L("Should equal %d + %d + %d"), DSTDNC_NUMBER_OF_PHONES, DSTD_NUMBER_OF_PHONES,
		originalNumPhones);
	TESTCHECK(numPhones, (DSTDNC_NUMBER_OF_PHONES + DSTD_NUMBER_OF_PHONES + originalNumPhones));

	RTelServer::TPhoneInfo info;
	TName tsyName;
	TInt  phoneIndex = originalNumPhones;

	// first phone
	ret=server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECK(info.iNetworkType, DSTD_NETWORK_TYPE);
	TESTCHECKSTR(info.iName, DSTD_PHONE_NAME);
	TESTCHECK(info.iNumberOfLines, DSTD_NUMBER_OF_LINES);
	
	ret=server.GetTsyName(phoneIndex,tsyName);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(tsyName, DSTD_MODULE_NAME);
	phoneIndex++;
	
	// second phone
	ret=server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECK(info.iNetworkType, DSTDFC_NETWORK_TYPE);
	TESTCHECKSTR(info.iName, DSTDFC_PHONE_NAME);
	TESTCHECK(info.iNumberOfLines, DSTDFC_NUMBER_OF_LINES);

	ret=server.GetTsyName(phoneIndex,tsyName);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(tsyName, DSTD_MODULE_NAME);
	phoneIndex++;

	// third phone
	ret=server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECK(info.iNetworkType, DSTDPM_NETWORK_TYPE);
	TESTCHECKSTR(info.iName, DSTDPM_PHONE_NAME);
	TESTCHECK(info.iNumberOfLines, DSTDPM_NUMBER_OF_LINES);

	ret=server.GetTsyName(phoneIndex,tsyName);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(tsyName, DSTD_MODULE_NAME);
	phoneIndex++;

	// fourth phone
	ret=server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECK(info.iNetworkType, DSTDPM_NETWORK_TYPE);
	TESTCHECKSTR(info.iName, DSTD_SLOW_PHONE_NAME);
	TESTCHECK(info.iNumberOfLines, DSTDPM_NUMBER_OF_LINES);

	ret=server.GetTsyName(phoneIndex,tsyName);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(tsyName, DSTD_MODULE_NAME);
	phoneIndex++;

	// fifth phone
	ret=server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECK(info.iNetworkType, DSTDNC_NETWORK_TYPE);
	TESTCHECKSTR(info.iName, DSTDNC_PHONE_NAME);
	TESTCHECK(info.iNumberOfLines, DSTDNC_NUMBER_OF_LINES);

	ret=server.GetTsyName(phoneIndex,tsyName);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(tsyName, DSTDNC_MODULE_NAME);
	phoneIndex++;

	// sixth phone
	ret=server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNotFound);

	ret=server.GetTsyName(phoneIndex,tsyName);
	TESTCHECK(ret, KErrNotFound);
	phoneIndex++;

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
	ret=server.UnloadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

    ret = server.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	ret=server.EnumeratePhones(numPhones);
    TESTCHECK(ret, KErrNone);
	TESTCHECK(numPhones, (DSTD_NUMBER_OF_PHONES + DSTDNC_NUMBER_OF_PHONES + originalNumPhones));

	phoneIndex = originalNumPhones;

	ret=server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECK(info.iNetworkType, DSTDNC_NETWORK_TYPE);
	TESTCHECKSTR(info.iName, DSTDNC_PHONE_NAME);
	TESTCHECK(info.iNumberOfLines, DSTDNC_NUMBER_OF_LINES);
	phoneIndex++;

	ret=server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECK(info.iNetworkType, DSTD_NETWORK_TYPE);
	TESTCHECKSTR(info.iName, DSTD_PHONE_NAME);
	TESTCHECK(info.iNumberOfLines, DSTD_NUMBER_OF_LINES);
	phoneIndex++;

	ret=server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECK(info.iNetworkType, DSTDFC_NETWORK_TYPE);
	TESTCHECKSTR(info.iName, DSTDFC_PHONE_NAME);
	TESTCHECK(info.iNumberOfLines, DSTDFC_NUMBER_OF_LINES);
	phoneIndex++;

	ret=server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECK(info.iNetworkType, DSTDPM_NETWORK_TYPE);
	TESTCHECKSTR(info.iName, DSTDPM_PHONE_NAME);
	TESTCHECK(info.iNumberOfLines, DSTDPM_NUMBER_OF_LINES);
	phoneIndex++;

	ret=server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECK(info.iNetworkType, DSTDPM_NETWORK_TYPE);
	TESTCHECKSTR(info.iName, DSTD_SLOW_PHONE_NAME);
	TESTCHECK(info.iNumberOfLines, DSTDPM_NUMBER_OF_LINES);
	phoneIndex++;

	ret=server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNotFound);
	phoneIndex++;

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
	ret=server.UnloadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}
