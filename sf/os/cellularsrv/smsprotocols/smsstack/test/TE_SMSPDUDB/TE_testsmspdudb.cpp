
// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#include "TE_smspdudbbase.h"
#include "TE_testsmspdudb.h"


TVerdict CTestSmsPduDbMessage::doTestStepL()
/*
 * implementation of Message test case
 */
{
	CSmsPduDatabase* db = CSmsPduDatabase::NewL(iFs, KSmsPduDbDefaultSection, KSmsPduDbTestFilename, KSmsPduDbDefaultComponent);
	CleanupStack::PushL(db);

	RPointerArray<CSmsPduDbMessage> array;
	CleanupResetAndDestroyPushL(array);

	db->GetMessageL(array, CSmsPDU::ESmsSubmit);

	const TInt count = array.Count();
	TInt i = 0;
	for (i = 0; i < count; i++)
	{
	PrintMessage(i, *array[i]);
	}

	CleanupStack::PopAndDestroy(&array);
	CleanupStack::PopAndDestroy(db);

	return TestStepResult();
}


TVerdict CTestSmsPduDbPdu::doTestStepL()
/*
 * implementation of Pdu test case
 */
{
	CSmsPduDatabase* db = CSmsPduDatabase::NewL(iFs, KSmsPduDbDefaultSection, KSmsPduDbTestFilename, KSmsPduDbDefaultComponent);
	CleanupStack::PushL(db);

	RPointerArray<CSmsPduDbPdu> array;
	CleanupResetAndDestroyPushL(array);

	db->GetPduL(array, CSmsPDU::ESmsSubmit);
	TInt count = array.Count();
	TInt i = 0;
	for (i = 0; i < count; i++)
	{
	const CSmsPduDbPdu& pdu = *array[i];
	PrintPdu(i, pdu);
	}
	array.ResetAndDestroy();
	db->GetPduL(array, CSmsPDU::ESmsDeliver);
	count = array.Count();

	for (i = 0; i < count; i++)
	{
	const CSmsPduDbPdu& pdu = *array[i];
	PrintPdu(i, pdu);
	}

	CleanupStack::PopAndDestroy(&array);
	CleanupStack::PopAndDestroy(db);

	return TestStepResult() ;
}


TVerdict CTestSmsPduDbConcatSegment::doTestStepL()
/*
 * implementation of Concat Segment test case
 */
{
	// The test case was empty in old t_smspdudb.exe

	return TestStepResult();
}


TVerdict CTestSmsPduDbConcat::doTestStepL()
/*
 * implementation of Concat test case
 */
{
	CSmsPduDatabase* db = CSmsPduDatabase::NewL(iFs, KSmsPduDbDefaultSection, KSmsPduDbTestFilename, KSmsPduDbDefaultComponent);
	CleanupStack::PushL(db);

	RPointerArray<CSmsPduDbConcat> array;
	CleanupResetAndDestroyPushL(array);

	db->GetConcatL(array, CSmsPDU::ESmsSubmit);
	TInt count = array.Count();
	TInt i = 0;
	for (i = 0; i < count; i++)
	{
	const CSmsPduDbConcat& concat = *array[i];
	PrintConcat(i, concat);
	}

	array.ResetAndDestroy();

	db->GetConcatL(array, CSmsPDU::ESmsDeliver);
	count = array.Count();

	for (i = 0; i < count; i++)
	{
	const CSmsPduDbConcat& concat = *array[i];
	PrintConcat(i, concat);
	}

	CleanupStack::PopAndDestroy(&array);
	CleanupStack::PopAndDestroy(db);

	return TestStepResult();
}


TVerdict CTestSmsPduDbParse::doTestStepL()
/*
 * implementation of Parse test case
 */
{
	CSmsPduDatabase* db = CSmsPduDatabase::NewL(iFs, KSmsPduDbDefaultSection, KSmsPduDbTestFilename, KSmsPduDbDefaultComponent);
	CleanupStack::PushL(db);

	db->RewriteFileL(KSmsPduDbTestFilename, KSmsPduDbDefaultComponent, KSmsPduDbTestOutput);


	CleanupStack::PopAndDestroy(db);

	return TestStepResult();
}


TVerdict CTestSmsPduDbCompile::doTestStepL()
/*
 * implementation of Compile test case
 */
	{
	CSmsPduDatabase* db = CSmsPduDatabase::NewL(iFs);
	CleanupStack::PushL(db);

	const CTestConfig& configFile = db->ConfigFile();

	const TInt sectionCount = configFile.Sections().Count();

	for (TInt section = 0; section < sectionCount; section++)
		{
		const CTestConfigSection& configSection = configFile[section];
		const TInt itemCount = configSection.Items().Count();

		for (TInt item = 0; item < itemCount; item++)
			{
			const CTestConfigItem& configItem = configSection[item];
			CSmsPduDbPdu* pdu = NULL;

			TRAPD(err, pdu = db->PduFactoryL(configItem));
			if (err != KErrNone)
				{
				DisplayCompileErrorL(configSection, configItem, err);
				TEST(EFalse);
				continue;
				}

			CleanupStack::PushL(pdu);

			if (pdu->iDecodeError > KErrNone)
				{
				DisplayCompileErrorL(configSection, configItem, KErrArgument);
				TEST(EFalse);
				}
			else if (pdu->iSmsMessageError > KErrNone)
				{
				DisplayCompileErrorL(configSection, configItem, KErrCorrupt);
				TEST(EFalse);
				}

			CleanupStack::PopAndDestroy(pdu);
			}
		}

	CleanupStack::PopAndDestroy(db);

	return TestStepResult();
	}


TVerdict CTestSmsPduDbTools::doTestStepL()
/*
 * implementation of Tools test case
 */
{
	TGsmSms pdu;
	TSmsFirstOctet fo(iSmsStackTestUtils->FirstDeliverOctet(EFalse, EFalse, EFalse, ETrue));
	TSmsDataCodingScheme dcs;
	TSmsProtocolIdentifier pid;

	TGsmSmsTelNumber oa;
	oa.iTelNumber.Copy(_L("HelloHello"));
	oa.iTypeOfAddress.SetNPI(EGsmSmsNPIUnknown);
	oa.iTypeOfAddress.SetTON(EGsmSmsTONAlphaNumeric);

	TTime scts;
	scts.UniversalTime();
	TInt offsetToUTCInQtrHours =  ((User::UTCOffset()).Int()/900);

	CCnvCharacterSetConverter* conv = CCnvCharacterSetConverter::NewL();
	CleanupStack::PushL(conv);

	CSmsUserData* ud = CSmsUserData::NewL(*conv, iSmsStackTestUtils->iFs, fo, dcs);
	CleanupStack::PushL(ud);

	iSmsStackTestUtils->DeliverWithoutUserDataL(pdu, fo, oa, pid, dcs, scts, offsetToUTCInQtrHours);
	iSmsStackTestUtils->AppendUserDataL(pdu, *ud);
	iSmsStackTestUtils->PrintAndTestDecodeL(pdu, dcs, pid, *ud, ETrue, KErrNone);

	fo = iSmsStackTestUtils->FirstDeliverOctet(ETrue, ETrue, ETrue, ETrue);
	iSmsStackTestUtils->DeliverWithoutUserDataL(pdu, fo, oa, pid, dcs, scts, offsetToUTCInQtrHours);
	iSmsStackTestUtils->AppendUserDataL(pdu, *ud);
	iSmsStackTestUtils->PrintAndTestDecodeL(pdu, dcs, pid, *ud, ETrue, KErrNone);

	iSmsStackTestUtils->DeliverWithoutUserDataL(pdu, fo, oa, pid, dcs, scts, offsetToUTCInQtrHours);
	iSmsStackTestUtils->AppendUserDataL(pdu, *ud, 0x99);
	iSmsStackTestUtils->PrintAndTestDecodeL(pdu, dcs, pid, *ud, ETrue, KErrGsmuDecoding);

	ud->SetBodyL(_L8("Dundee United SPL Champions 1983"));
	iSmsStackTestUtils->DeliverWithoutUserDataL(pdu, fo, oa, pid, dcs, scts, offsetToUTCInQtrHours);
	iSmsStackTestUtils->AppendUserDataL(pdu, *ud);
	iSmsStackTestUtils->PrintAndTestDecodeL(pdu, dcs, pid, *ud, ETrue, KErrNone);

	TPckgBuf<TInt32> buf;
	buf() = 200;

	ud->AddInformationElementL(CSmsInformationElement::ESmsIEIConcatenatedShortMessages16BitReference, buf);
	iSmsStackTestUtils->DeliverWithoutUserDataL(pdu, fo, oa, pid, dcs, scts, offsetToUTCInQtrHours);
	iSmsStackTestUtils->AppendUserDataL(pdu, *ud);
	iSmsStackTestUtils->PrintAndTestDecodeL(pdu, dcs, pid, *ud, ETrue, KErrNone);


	CleanupStack::PopAndDestroy(2);

	return TestStepResult();
}


TVerdict CTestSmsServiceCenterTimeStamp::doTestStepL()
/*
 * implementation of Tools test case
 */
{
	iSmsStackTestUtils->CreateTSmsServiceCenterTimeStampL();
	iSmsStackTestUtils->EncodeTSmsServiceCenterTimeStampL();
	iSmsStackTestUtils->DecodeTSmsServiceCenterTimeStampL();

	return TestStepResult();
}


TVerdict CTestSmsValidityPeriod::doTestStepL()
/*
 * implementation of Tools test case
 */
{
	iSmsStackTestUtils->CreateTSmsValidityPeriodL();
	iSmsStackTestUtils->EncodeTSmsValidityPeriodL();
	iSmsStackTestUtils->DecodeTSmsValidityPeriodL();

	return TestStepResult();
}
