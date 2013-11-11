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

/**
 @file
*/

#include "TE_EtelBase.h"
#include "TE_Etelduplicatephonename.h"

CTestDuplicatePhoneName::CTestDuplicatePhoneName()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestDupliPhoneName"));
	}

enum TVerdict CTestDuplicatePhoneName::doTestStepL()
//
// With throu' tsy module if already loaded tsy with the same name
//
	{
	INFO_PRINTF1(_L("Test Duplicate Phone Name Case 1"));
	INFO_PRINTF1(_L("With throu' tsy module if already loaded tsy with the same name"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);

	TInt  originalNumPhones;
	ret = server.EnumeratePhones(originalNumPhones);
    TESTCHECK(ret, KErrNone);

   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	TInt num;
	ret=server.EnumeratePhones(num);
    TESTCHECK(ret, KErrNone);

	TESTCHECK(num, DSTD_NUMBER_OF_PHONES + originalNumPhones);

	//
	// no longer rejects loading this module with new server functionality
	//
   	ret=server.LoadPhoneModule(DBAD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	// check that is so
	ret=server.EnumeratePhones(num);
    TESTCHECK(ret, KErrNone);
	TESTCHECK(num, DSTD_NUMBER_OF_PHONES + DBAD_NUMBER_OF_PHONES + originalNumPhones);
 
	ret=server.UnloadPhoneModule(DBAD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
  	
	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	ret=server.EnumeratePhones(num);
    TESTCHECK(ret, KErrNone);
    TESTCHECK(num, DSTD_NUMBER_OF_PHONES + DSTDNC_NUMBER_OF_PHONES + originalNumPhones);
	
   	ret=server.LoadPhoneModule(DBAD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	ret=server.EnumeratePhones(num);
    TESTCHECK(ret, KErrNone);
    TESTCHECK(num, DSTD_NUMBER_OF_PHONES + DSTDNC_NUMBER_OF_PHONES +
	            DBAD_NUMBER_OF_PHONES + originalNumPhones);

	ret=server.UnloadPhoneModule(DBAD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	ServerClose(server);

	return TestStepResult();
	}


CTestOpenCallNoName::CTestOpenCallNoName()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenCallNoName"));
	}

enum TVerdict CTestOpenCallNoName::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Open Call Without Name"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RLine line;
	line.Open(server,DSTDPM_LINE_FULLNAME);
	TInt num;
	ret=line.EnumerateCall(num);
    TESTCHECK(ret, KErrNone);
    TESTCHECK(num, 0);

	RCall call;
	ret = call.OpenNewCall(line);
    TESTCHECK(ret, KErrNone);
	ret=line.EnumerateCall(num);
    TESTCHECK(ret, KErrNone);
    TESTCHECK(num, 1);

	RLine::TCallInfo info;
	ret=line.GetCallInfo(0,info);
    TESTCHECK(ret, KErrNone);

	// the first time so the name of this call is DSTD0
    TESTCHECKSTR(info.iCallName, _L("DSTD0"));

	RCall call2;
	ret=call2.OpenNewCall(line); // another default name
    TESTCHECK(ret, KErrNone);
	ret=line.EnumerateCall(num);
    TESTCHECK(ret, KErrNone);
    TESTCHECK(num, 2);

	ret=line.GetCallInfo(1,info);
    TESTCHECK(ret, KErrNone);

	// this time name of this call is DSTD1
    TESTCHECKSTR(info.iCallName, _L("DSTD1"));
	line.Close();
	call2.Close();
	call.Close();
    ServerClose(server);

	return TestStepResult();
	}

CTestVersion::CTestVersion()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestVersion"));
	}

enum TVerdict CTestVersion::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Etel Version"));

	RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);

	TVersion version = server.Version();
	INFO_PRINTF2(_L("Version number of ETel = %d"),version.iBuild);
    TESTCHECK(version.iBuild, KEtelBuildVersionNumber);
	ServerClose(server);

	return TestStepResult();
	}

CTestLoadTsySamePhoneName::CTestLoadTsySamePhoneName()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestLoadTsySmPhName"));
	}

enum TVerdict CTestLoadTsySamePhoneName::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Load Tsys With Same Phone Names"));

	RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);

	TInt  originalNumPhones;
	ret = server.EnumeratePhones(originalNumPhones);
    TESTCHECK(ret, KErrNone);

	INFO_PRINTF1(_L("Loading DSTD first then DSTDIDEN"));
	ret = server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
	ret = server.LoadPhoneModule(DSTDIDEN_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	TInt  phoneIndex = originalNumPhones;

	RTelServer::TPhoneInfo info;
	ret = server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(info.iName, DSTD_PHONE_NAME);
	phoneIndex++;

	ret = server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(info.iName, DSTDFC_PHONE_NAME);
	phoneIndex++;

	ret = server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(info.iName, DSTDPM_PHONE_NAME);
	phoneIndex++;

	ret = server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(info.iName, DSTD_SLOW_PHONE_NAME);
	phoneIndex++;

	ret = server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	_LIT(KNewPhoneName,"DSTDID-Dummy");
	TESTCHECKSTR(info.iName, KNewPhoneName);
	INFO_PRINTF2(_L("Identical phone name renamed %S"),&info.iName);
	phoneIndex++;

	ret = server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
	
	ret = server.UnloadPhoneModule(DSTDIDEN_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
//other way round
	INFO_PRINTF1(_L("Loading DSTDIDEN first then DSTD"));

	ret = server.LoadPhoneModule(DSTDIDEN_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
	ret = server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	phoneIndex = originalNumPhones;

	ret = server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(info.iName, DSTD_PHONE_NAME);
	phoneIndex++;

	ret = server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	_LIT(KNewPhoneName2,"DSTD-Dummy");
	TESTCHECKSTR(info.iName, KNewPhoneName2);
	INFO_PRINTF2(_L("Identical phone name renamed %S"),&info.iName);
	phoneIndex++;

	ret = server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(info.iName, DSTDFC_PHONE_NAME);
	phoneIndex++;

	ret = server.GetPhoneInfo(phoneIndex,info);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(info.iName, DSTDPM_PHONE_NAME);
	phoneIndex++;

	ret = server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
	ret = server.UnloadPhoneModule(DSTDIDEN_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	ServerClose(server);
	
	return TestStepResult();
	}


CTestOpenPhoneSamePhoneName::CTestOpenPhoneSamePhoneName()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpPhoneSmPhName"));
	}

/**
Tests opening of two phones with the same phone name.
Loads DSTD tsy and then DSTDIDEN tsy.
DSTDIDEN contains one phone which has the same name
as the first phone in DSTD. These are opened and then 
line names in each phone are retrieved. 

NOTE: 	When an additional TSY is loaded which contains
		a phone which has the same name as a phone in an already
		loaded TSY, the phone is renamed (it is prefixed with
		the TSY name.)  Therefore, this test does not actually
		force the opening of two phones with the same name.
		See test step: TestLoadTsySamePhoneName

NOTE:	In startupmode 0, there is an additional phone called 
		"GSMPhone1" in DSTD.
*/
enum TVerdict CTestOpenPhoneSamePhoneName::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Open Phones With Same Phone Names"));

	RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);

	INFO_PRINTF1(_L("Loading DSTD..."));	
	
	ret = server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	TInt numPhonesInFirstTSY;
	ret = server.EnumeratePhones(numPhonesInFirstTSY);
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF2(_L("Enumerate phones found %d phones"), numPhonesInFirstTSY);
	
	// Contains 1 phone, same name as first phone in DSTD
	INFO_PRINTF1(_L("Loading DSTDIDEN..."));	
	ret = server.LoadPhoneModule(DSTDIDEN_MODULE_NAME); 
	TESTCHECK(ret, KErrNone);

	RTelServer::TPhoneInfo info1;
	
	TInt numPhonesTotal;
	ret = server.EnumeratePhones(numPhonesTotal);
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF2(_L("Enumerate phones found %d phones"), numPhonesTotal);
	
	ret = server.GetPhoneInfo(0,info1);
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF2(_L("Opening phone: %S"), &info1.iName);
	
	RPhone dstdPhone;
	ret = dstdPhone.Open(server,info1.iName);
	TESTCHECK(ret, KErrNone);

	RTelServer::TPhoneInfo info2;
	
	// Get info from first phone in DSTIDEN TSY
	// Phones indexed from 0, so numPhonesFirstInTSY = first phone in DSTDID.
	ret = server.GetPhoneInfo(numPhonesInFirstTSY,info2); 
	TESTCHECK(ret, KErrNone);

	INFO_PRINTF2(_L("Opening phone: %S"), &info2.iName);
	RPhone dstdidenPhone;
	ret = dstdidenPhone.Open(server,info2.iName);
	TESTCHECK(ret, KErrNone);

	dstdPhone.Close();
	
	ret = dstdPhone.Open(server,info1.iName);
	TESTCHECK(ret, KErrNone);

	TInt numLines1;
	ret = dstdPhone.EnumerateLines(numLines1);
	TESTCHECK(ret, KErrNone);
	
	TInt numLines2;
	ret = dstdidenPhone.EnumerateLines(numLines2);
	TESTCHECK(ret, KErrNone);
	
	RPhone::TLineInfo lineinfo;
	RLine line;
	
	TInt i=0;
	
	INFO_PRINTF1(_L("Lines in first phone:"));
	for (i=0; i<numLines1; i++)
		{
		ret = dstdPhone.GetLineInfo(i,lineinfo);
		TESTCHECK(ret, KErrNone);

		TName linename(info1.iName);	// set linename first to the phone name, plus doublecolon
		_LIT(KDoubleColon,"::");
		linename.Append(KDoubleColon);
		linename.Append(lineinfo.iName);
		INFO_PRINTF2(_L("Line name = %S"),&linename);
		
		ret = line.Open(server,linename);
		TESTCHECK(ret, KErrNone);
		line.Close();
		}

	INFO_PRINTF1(_L("Lines in second phone:"));
	for (i=0; i<numLines2; i++)	// now for the phone in DSTDID.TSY - open and close its lines
		{
		ret = dstdidenPhone.GetLineInfo(i,lineinfo);
		TESTCHECK(ret, KErrNone);

		TName linename(info2.iName); // set linename first to the phone name, plus doublecolon
		_LIT(KDoubleColon,"::");
		linename.Append(KDoubleColon);
		linename.Append(lineinfo.iName);
		INFO_PRINTF2(_L("Line name = %S"),&linename);
		
		line.Open(server,linename);
		TESTCHECK(ret, KErrNone);	// open and close each line in turn
		line.Close();
		}
					
	dstdidenPhone.Close();
	dstdPhone.Close();

	ServerClose(server);

	return TestStepResult();
	}
		
CTestOpenPhoneModeIdentPhoneName::CTestOpenPhoneModeIdentPhoneName()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpPhModSmPhName"));
	}

enum TVerdict CTestOpenPhoneModeIdentPhoneName::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Loading Phone Module With 2 Identical Phone Names"));

	RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
	
	ret = server.LoadPhoneModule(DBAD_IDEN_MODULE_NAME);
	TESTCHECK(ret, KErrEtelDuplicatePhoneName);

	ServerClose(server);

	return TestStepResult();
	}

CTestStaticCallPoolTsy::CTestStaticCallPoolTsy()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestStatCallPoolTsy"));
	}

enum TVerdict CTestStaticCallPoolTsy::doTestStepL()
	{
	_LIT(KStaticCallTsy,"DStaticCall.TSY");

	INFO_PRINTF1(_L("Test a TSY that allocates call objects from a static pool."));

	RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
	
	ret = server.LoadPhoneModule(KStaticCallTsy);
	TESTCHECK(ret, KErrNone);

	RPhone phone;
	
	ret = phone.Open(server,DSTATICCALL_PHONE_NAME);
	TESTCHECK(ret, KErrNone);

	RLine line;
	
	ret = line.Open(phone,DSTATICCALL_LINE_NAME);
	TESTCHECK(ret, KErrNone);

	TName name;
	RCall call;
	
	ret = call.OpenNewCall(line,name);
	TESTCHECK(ret, KErrNone);
	call.Close();

	ret = call.OpenNewCall(line,name);
	TESTCHECK(ret, KErrNone);
	call.Close();
	line.Close();
	phone.Close();

	ServerClose(server);

	return TestStepResult();
	}
