// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file
*/
#include <s32file.h>

#include "gsmumsg.h"
#include "gsmubuf.h"
#include "gsmuset.h"
#include "te_gsmuStep.h"
#include "gsmunmspacemobmsg.h"
#include <test/testexecutelog.h>
#include "gsmuieoperations.h"
#include "gsmunonieoperations.h"

//
// Conversion strings for testing
//


_LIT16(K7BitTestData1,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
_LIT16(K7BitTestData2,"|^{}[~]\\");
_LIT16(K7BitTestData3,"\x40\xA3\x24\xA5\xE8\xE9\xF9\xEC\xF2\xE7\x5C\x6E\xD8\xF8\x5C\x72\xC5\xE5\x5F\xC6\xE6\xDF\xC9\x20\x21\x5C\x22\x23\xA4\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F\xA1\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5A\xC4\xD6\xD1\xDC\xA7\xBF\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\xE4\xF6\xF1\xFC\xE0");
_LIT16(K7BitTestData4,"\x40\xA3\x24\xA5\xE8\xE9\xF9\xEC\xF2\xE7\x5C\x6E\xD8\xF8\x5C\x72\xC5\xE5\x5F\xC6\xE6\xDF\xC9\x20\x21\x5C\x22\x23\xA4\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F\xA1\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5A\xC4\xD6\xD1\xDC\xA7\xBF\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\xE4\xF6\xF1\xFC\xE0\x7C\x5E\x7B\x7D\x5B\x7E\x5D\x5C\x5C");
_LIT16(K7BitTestData5,"\x40\xA3\x40\xA3\x24\xA5\xE8\xE9\xF9\xEC\xF2\xE7\x5C\x6E\xD8\xF8\x5C\x72\xC5\xE5\x5F\xC6\xE6\xDF\xC9\x20\x21\x5C\x22\x23\xA4\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F\xA1\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5A\xC4\xD6\xD1\xDC\xA7\xBF\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\xE4\xF6\xF1\xFC\xE0\x24\xA5\xE8\xE9\xF9\xEC\xF2\xE7\x5C\x6E\xD8\xF8\x5C\x72\xC5\xE5\x5F\xC6\xE6\xDF\xC9\x20\x21\x5C");

const TDesC16* K7BitTestDataTable[]=
	{
	&K7BitTestData1,
	&K7BitTestData2,
	&K7BitTestData3,
	&K7BitTestData4,
	NULL
	};

_LIT8(KBinaryTestData1,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
_LIT8(KBinaryTestData2,"|^{}[~]\\");
_LIT8(KBinaryTestData3,"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10");

const TDesC8* KBinaryTestDataTable[]=
	{
	&KBinaryTestData1,
	&KBinaryTestData2,
	&KBinaryTestData3,
	NULL
	};

const TText8 PDUtext[15]={ 0x15 ,0x00 ,0x0c ,0x91 ,0x44 ,0x77 ,0x74 ,0x60 ,0x55 ,0x84 ,0x00 ,0x04 ,0xa7 ,0x01 ,0x78 };

TVerdict CTgsmuStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	TInt ret=iFs.Connect();
	TEST(ret==KErrNone);

	return TestStepResult();
	}

TVerdict CTgsmuStepBase::doTestStepPostambleL()
	{
	iFs.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
	}

void CTgsmuStepBase::NewSmsMessageL(const TSmsUserDataSettings& aUserDataSettings,TInt aNumBufs)
	{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsmessage1=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsmessage1);
    smsmessage1->SetUserDataSettingsL(aUserDataSettings);
	TInt i=0;
	TBuf<27> buf;
	buf.Format(_L("Hello Chris\n"));
	for (; i<aNumBufs; i++)
		{
		buffer->InsertL(i*buf.Length(),buf);
		}

	smsmessage1->SetToFromAddressL(_L("+447747065405"));
	smsmessage1->SetServiceCenterAddressL(_L("+447785016005"));

	TSmsUserDataSettings userdatasettings;
	smsmessage1->UserDataSettings(userdatasettings);
	TEST(aUserDataSettings.TextCompressed()==userdatasettings.TextCompressed());
	TEST(aUserDataSettings.Alphabet()==userdatasettings.Alphabet());

	CArrayFixFlat<TGsmSms>* smsarray=new(ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(smsarray);
	TInt msgref=10;
	smsmessage1->EncodeMessagePDUsL(*smsarray, msgref);
	INFO_PRINTF2(_L("Num messages %d"),smsarray->Count());
	buffer=CSmsBuffer::NewL();
	CSmsMessage* smsmessage2=CSmsMessage::NewL(iFs, (*smsarray)[0],buffer,EFalse,EFalse);
	CleanupStack::PushL(smsmessage2);
	smsmessage2->UserDataSettings(userdatasettings);

	TEST(aUserDataSettings.TextCompressed()==userdatasettings.TextCompressed());
	TEST(aUserDataSettings.Alphabet()==userdatasettings.Alphabet());

	if (smsmessage2->NumMessagePDUsL()>1)
		smsmessage2->DecodeMessagePDUsL(*smsarray);
	for (i=0; i<aNumBufs; i++)
		{
		smsmessage2->Buffer().Extract(buf,i*buf.Length(),buf.Length());
		//INFO_PRINTF2(_L("%S"),&buf);
		}

	CleanupStack::PopAndDestroy(3);
	}



void CTgsmuStepBase::TestSmsHeaderL()
	{
	TSmsUserDataSettings userdatasettings;

	for (TInt alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit; alphabet<=TSmsDataCodingScheme::ESmsAlphabetUCS2; alphabet+=4)
		{
		userdatasettings.SetAlphabet((TSmsDataCodingScheme::TSmsAlphabet) alphabet);
		for (TInt concatenated=0; concatenated<=1; concatenated++)
			{
			userdatasettings.SetTextConcatenated(concatenated);
			for (TInt compressed=0; (compressed==0); compressed++)
				{
				userdatasettings.SetTextCompressed(compressed);
				NewSmsMessageL(userdatasettings,5);
				if (concatenated)
					NewSmsMessageL(userdatasettings,100);
				}
			}
		}
	}

void CTgsmuStepBase::TestDecodePDUL()
	{
	TPtrC8 ptr(PDUtext,sizeof(PDUtext)/sizeof(PDUtext[0]));
	NMobileSmsMessaging::TMobileSmsGsmTpdu pdu(ptr);
	TGsmSms sms;
	sms.SetPdu(pdu);
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsmessage1=CSmsMessage::NewL(iFs, sms,buffer, EFalse,EFalse);
	CleanupStack::PushL(smsmessage1);

	const TInt KLength = 27;
	TBuf<KLength> buf;
	for (TInt i=0; (i*buf.MaxLength())<buffer->Length(); i++)
		{
		TInt length = buffer->Length();
		if( KLength < buffer->Length() )
			length=(i*buf.MaxLength())<buffer->Length()? buf.MaxLength(): buffer->Length()-(i*buf.MaxLength());
		buffer->Extract(buf,i*buf.MaxLength(),length);
		INFO_PRINTF2(_L("%S"),&buf);
		}
	INFO_PRINTF1(_L(""));
	
	CleanupStack::PopAndDestroy();
	}

void CTgsmuStepBase::TestCommandDataHeaderL(void)
/**
 *  @test Test the command data header functionality
 */
	{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsCommand,buffer);
	CleanupStack::PushL(smsMessage);

	// Set some parameters
	smsMessage->SetToFromAddressL(_L("+447747065481"));
	smsMessage->SetServiceCenterAddressL(_L("+447785016005"));

	// Add some command data headers
	_LIT(KCommandDataA,"Command Data A");
	_LIT(KCommandDataB,"Command Data B");
	TBuf8<16> ieiHeaderDataA;
	TBuf8<16> ieiHeaderDataB;
	ieiHeaderDataB.Copy(KCommandDataA);
	ieiHeaderDataB.Copy(KCommandDataB);
	CSmsCommand& smsCommand=(CSmsCommand&)smsMessage->SmsPDU();
	smsCommand.AddInformationElementL(CSmsInformationElement::ESmsIEISCSpecificUse1,ieiHeaderDataA);
	smsCommand.AddInformationElementL(CSmsInformationElement::ESmsIEISCSpecificUse2,ieiHeaderDataB);

	// Attempt encoding PDUs
	CArrayFix<TGsmSms>* smsArray=new(ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(smsArray);
	smsMessage->EncodeMessagePDUsL(*smsArray,ETrue);

	// Reconstruct from PDUs
	CSmsBuffer* buffer2=CSmsBuffer::NewL();
	CSmsMessage* smsMessage2=CSmsMessage::NewL(iFs, (*smsArray)[0],buffer2, EFalse,EFalse);
	CleanupStack::PushL(smsMessage2);

	// Check for command data header
	TEST(smsMessage2->SmsPDU().Type()==CSmsPDU::ESmsCommand);
	CSmsCommand& smsCommand2=(CSmsCommand&)smsMessage2->SmsPDU();
	TEST(smsCommand2.NumInformationElements()==2);
	TEST(smsCommand2.InformationElement(0).Identifier()==CSmsInformationElement::ESmsIEISCSpecificUse1);
	TPtr8 ieiHeaderDataA2=smsCommand2.InformationElement(0).Data();
	TPtr8 ieiHeaderDataB2=smsCommand2.InformationElement(1).Data();
	TEST(Mem::Compare(ieiHeaderDataA2.Ptr(),ieiHeaderDataA2.Size(),ieiHeaderDataA.Ptr(),ieiHeaderDataA.Size())==0);
	TEST(Mem::Compare(ieiHeaderDataB2.Ptr(),ieiHeaderDataB2.Size(),ieiHeaderDataB.Ptr(),ieiHeaderDataB.Size())==0);

	// Tidy up
	CleanupStack::PopAndDestroy(3);	// smsMessage2, smsArray, smsMessage
	}

void CTgsmuStepBase::TestSmsTONandNPIL()
	{
	TPtrC8 ptr(PDUtext,sizeof(PDUtext)/sizeof(PDUtext[0]));
	NMobileSmsMessaging::TMobileSmsGsmTpdu pdu(ptr);
	TGsmSms sms;
	sms.SetPdu(pdu);
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsmessage1=CSmsMessage::NewL(iFs, sms,buffer, EFalse,EFalse);
	CleanupStack::PushL(smsmessage1);

	TGsmSmsTelNumber address;
	smsmessage1->ParsedToFromAddress(address);

	//GLOBAL_CHECKPOINT(( address.iTypeOfAddress.TON() == EGsmSmsTONAlphaNumeric));

	TGsmSmsTypeOfNumber ton =address.iTypeOfAddress.TON() ;
	TGsmSmsNumberingPlanIdentification npi= address.iTypeOfAddress.NPI();

	INFO_PRINTF2(_L("default TON %d"),ton);
	INFO_PRINTF2(_L("default MPI %d"),npi);

	address.iTypeOfAddress.SetTON(EGsmSmsTONNationalNumber);
	address.iTypeOfAddress.SetNPI(EGsmSmsNPINationalNumberingPlan);

	TEST(( address.iTypeOfAddress.TON() == EGsmSmsTONNationalNumber));
	TEST(( address.iTypeOfAddress.NPI() == EGsmSmsNPINationalNumberingPlan));

	address.iTypeOfAddress.SetTON(EGsmSmsTONAlphaNumeric);
	address.iTypeOfAddress.SetNPI(EGsmSmsNPIPrivateNumberingPlan);

	TEST(( address.iTypeOfAddress.TON() == EGsmSmsTONAlphaNumeric));
	TEST(( address.iTypeOfAddress.NPI() == EGsmSmsNPIPrivateNumberingPlan));

	CleanupStack::PopAndDestroy(smsmessage1);
	}

TVerdict CTgsmuStepBase::TestSmsDefaultVersionL(void)
/**
 *  @test Test the current default version (default version is now version 2) allocation
 */
	{
	INFO_PRINTF1(_L("Creating New Message..."));
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsmessage1=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsmessage1);

	TInt ver=-1;

	ver = smsmessage1->Version();
	INFO_PRINTF2(_L("Brand new message, version is %d, expected value is 4 (ESmsMessageV4)"),ver);
	TESTCHECK(ver, CSmsMessage::ESmsMessageV4);

	CleanupStack::PopAndDestroy(1);
	return TestStepResult();
 	}

TVerdict CTgsmuStepBase::TestSmsCurrentVersionL(void)
/**
 *  @test Test the current version allocation (internalise/externalise properly)
 */
	{
	//
	// Create a new message...
	//
	INFO_PRINTF1(_L("Creating New Message..."));
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsmessage1=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsmessage1);

	TInt ver=-1;

	_LIT(KTestFileName,"c:\\smsfp.txt");
	RFileReadStream rTestStream;
	CleanupClosePushL(rTestStream);
	RFileWriteStream wTestStream;
	CleanupClosePushL(wTestStream);
	RFs testFileReadSession;
	CleanupClosePushL(testFileReadSession);
	User::LeaveIfError(testFileReadSession.Connect());
	RFs testFileWriteSession;
	CleanupClosePushL(testFileWriteSession);
	User::LeaveIfError(testFileWriteSession.Connect());

	//
	// Test externalising current version CSmsMessage and check the
	// correct version is set when internalised...
	//
	INFO_PRINTF1(_L("Externalising..."));
	wTestStream.Replace(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage1->ExternalizeWithoutBufferL(wTestStream);
	wTestStream.Close();

	ver=-1;	//resetting
	INFO_PRINTF1(_L("Resetting version..."));
	TInt err = smsmessage1->SetVersion(CSmsMessage::ESmsMessageV0);
	User::LeaveIfError(err);

	INFO_PRINTF1(_L("Internalising..."));
	rTestStream.Open(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage1->InternalizeWithoutBufferL(rTestStream);
	rTestStream.Close();

	INFO_PRINTF1(_L("Checking version..."));
	ver = smsmessage1->Version();
	INFO_PRINTF2(_L("Version is %d, expected value is 4 (ESmsMessageV4)"),ver);
	TESTCHECK(ver, CSmsMessage::ESmsMessageV4);

	//
	// Externalise a V0 CSmsMessage and internalise it...
	//
	INFO_PRINTF1(_L("Changing version..."));
	err = smsmessage1->SetVersion(CSmsMessage::ESmsMessageV0);
	User::LeaveIfError(err);

	INFO_PRINTF1(_L("Externalising new version..."));
	wTestStream.Replace(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage1->ExternalizeWithoutBufferL(wTestStream);
	wTestStream.Close();

	ver=-1;	//resetting
	INFO_PRINTF1(_L("Resetting version..."));
	err = smsmessage1->SetVersion(CSmsMessage::ESmsMessageV1);
	User::LeaveIfError(err);

	INFO_PRINTF1(_L("Internalising new version..."));
	rTestStream.Open(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage1->InternalizeWithoutBufferL(rTestStream);
	rTestStream.Close();

	INFO_PRINTF1(_L("Checking new version..."));
	ver = smsmessage1->Version();
	INFO_PRINTF2(_L("Version is %d, expected value is 0 (ESmsMessageV0)"),ver);
	TESTCHECK(ver, CSmsMessage::ESmsMessageV0);

	//
	// Externalise a V1 CSmsMessage and internalise it...
	//
	INFO_PRINTF1(_L("Changing version..."));
	err = smsmessage1->SetVersion(CSmsMessage::ESmsMessageV1);
	User::LeaveIfError(err);

	INFO_PRINTF1(_L("Externalising new version..."));
	wTestStream.Replace(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage1->ExternalizeWithoutBufferL(wTestStream);
	wTestStream.Close();

	ver=-1;	//resetting
	INFO_PRINTF1(_L("Resetting version..."));
	err = smsmessage1->SetVersion(CSmsMessage::ESmsMessageV0);
	User::LeaveIfError(err);

	INFO_PRINTF1(_L("Internalising new version..."));
	rTestStream.Open(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage1->InternalizeWithoutBufferL(rTestStream);
	rTestStream.Close();

	INFO_PRINTF1(_L("Checking new version..."));
	ver = smsmessage1->Version();
	INFO_PRINTF2(_L("Version is %d, expected value is 1 (ESmsMessageV1)"),ver);
	TESTCHECK(ver, CSmsMessage::ESmsMessageV1);

	//
	// Externalise a V2 CSmsMessage and internalise it...
	//
	INFO_PRINTF1(_L("Changing version..."));
	err = smsmessage1->SetVersion(CSmsMessage::ESmsMessageV2);
	User::LeaveIfError(err);

	INFO_PRINTF1(_L("Externalising new version..."));
	wTestStream.Replace(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage1->ExternalizeWithoutBufferL(wTestStream);
	wTestStream.Close();

	ver=-1;	//resetting
	INFO_PRINTF1(_L("Resetting version..."));
	err = smsmessage1->SetVersion(CSmsMessage::ESmsMessageV1);
	User::LeaveIfError(err);

	INFO_PRINTF1(_L("Internalising new version..."));
	rTestStream.Open(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage1->InternalizeWithoutBufferL(rTestStream);
	rTestStream.Close();

	INFO_PRINTF1(_L("Checking new version..."));
	ver = smsmessage1->Version();
	INFO_PRINTF2(_L("Version is %d, expected value is 2 (ESmsMessageV2)"),ver);
	TESTCHECK(ver, CSmsMessage::ESmsMessageV2);

	//
	// Attempt to set an invalid version number...
	//
	INFO_PRINTF1(_L("Trying to pass an invalid value..."));
	err = smsmessage1->SetVersion(-2);
	TESTCHECK(err, KErrArgument);
		
	//
	// Internalise, externalise and internalise again...
	//
	INFO_PRINTF1(_L("Creating Second Message..."));
	buffer=CSmsBuffer::NewL();
	CSmsMessage* smsmessage2=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsmessage2);

	INFO_PRINTF1(_L("Externalising..."));
	wTestStream.Replace(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage2->ExternalizeWithoutBufferL(wTestStream);
	wTestStream.Close();

	ver=-1;	//resetting
	INFO_PRINTF1(_L("Resetting version..."));
	err = smsmessage2->SetVersion(CSmsMessage::ESmsMessageV1);
	User::LeaveIfError(err);

	INFO_PRINTF1(_L("Internalising..."));
	rTestStream.Open(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage2->InternalizeWithoutBufferL(rTestStream);
	rTestStream.Close();

	INFO_PRINTF1(_L("Checking version..."));
	ver = smsmessage2->Version();
	INFO_PRINTF2(_L("Version is %d, expected value is 4 (ESmsMessageV4)"),ver);
	TESTCHECK(ver, CSmsMessage::ESmsMessageV4);
	
	ver=-1;	//resetting
	INFO_PRINTF1(_L("Resetting version..."));
	err = smsmessage2->SetVersion(CSmsMessage::ESmsMessageV1);
	User::LeaveIfError(err);
		
	INFO_PRINTF1(_L("Externalising..."));
	wTestStream.Replace(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage2->ExternalizeWithoutBufferL(wTestStream);
	wTestStream.Close();

	INFO_PRINTF1(_L("Internalising..."));
	rTestStream.Open(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage2->InternalizeWithoutBufferL(rTestStream);
	rTestStream.Close();

	INFO_PRINTF1(_L("Checking version..."));
	ver = smsmessage2->Version();
	INFO_PRINTF2(_L("Version is %d, expected value is 1 (ESmsMessageV1)"),ver);
	TESTCHECK(ver, CSmsMessage::ESmsMessageV1);

	//
	// Clean up...
	//
	User::LeaveIfError(testFileReadSession.Delete(KTestFileName));

	testFileReadSession.Close();
	testFileWriteSession.Close();
	CleanupStack::PopAndDestroy(smsmessage2);
	CleanupStack::PopAndDestroy(5,smsmessage1);

	return TestStepResult();
 	}

TVerdict CTgsmuStepBase::TestSmsVersionBackwardsL(void)
/**
 *  @test Test the backwards data compatibility
 */
	{
	INFO_PRINTF1(_L("Creating New Message..."));
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsmessage1=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsmessage1);

	TInt ver=-1;

	_LIT(KTestFileName,"c:\\smsfp.txt");
	RFileReadStream rTestStream;
	CleanupClosePushL(rTestStream);
	RFileWriteStream wTestStream;
	CleanupClosePushL(wTestStream);
	RFs testFileReadSession;
	CleanupClosePushL(testFileReadSession);
	User::LeaveIfError(testFileReadSession.Connect());
	RFs testFileWriteSession;
	CleanupClosePushL(testFileWriteSession);
	User::LeaveIfError(testFileWriteSession.Connect());

	INFO_PRINTF1(_L("Simulating an externalised stream without version info..."));
	wTestStream.Replace(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage1->ExternalizeWithoutBufferAndVersionL(wTestStream);
	wTestStream.Close();

	INFO_PRINTF1(_L("Resetting version..."));
	TInt err = smsmessage1->SetVersion(CSmsMessage::ESmsMessageV1);
	User::LeaveIfError(err);

	INFO_PRINTF1(_L("Internalising..."));
	rTestStream.Open(testFileWriteSession,KTestFileName,EFileWrite);
	smsmessage1->InternalizeWithoutBufferL(rTestStream);
	rTestStream.Close();

	INFO_PRINTF1(_L("Checking version..."));
	ver = smsmessage1->Version();
	INFO_PRINTF2(_L("Version is %d, expected value is 0 (ESmsMessageV0)"),ver);
	TESTCHECK(ver, CSmsMessage::ESmsMessageV0)

	User::LeaveIfError(testFileReadSession.Delete(KTestFileName));

	testFileReadSession.Close();
	testFileWriteSession.Close();
	CleanupStack::PopAndDestroy(5,smsmessage1);

	return TestStepResult();
 	}


void CTgsmuStepBase::DoTestConversionL(CSmsAlphabetConverter& aConverter,const TDesC16& aDescriptor)
	{

	TPtrC8 converted8=aConverter.ConvertFromNativeL(aDescriptor);
	TEST(aConverter.UnconvertedNativeCharacters().Length()==0);

	TPtrC converted=aConverter.ConvertToNativeL(converted8);
	TEST(aConverter.UnconvertedUDElements().Length()==0);

	if (converted.Compare(aDescriptor)!=0)
		{
		INFO_PRINTF2(_L("Conversion failed:  '%S'"),&converted);
		TEST(EFalse);
		}
	}

void CTgsmuStepBase::DoTestAlphabetConversionL(enum TSmsDataCodingScheme::TSmsAlphabet aAlphabet,const TDesC16** aBufferTable)
	{
	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	CSmsAlphabetConverter* alphabetConverter=CSmsAlphabetConverter::NewLC(*characterSetConverter,iFs,aAlphabet,EFalse);
	TInt i=0;
	while (aBufferTable[i]!=NULL)
		{
		INFO_PRINTF2(_L("Testing string: '%S'"),aBufferTable[i]);

		DoTestConversionL(*alphabetConverter,*aBufferTable[i]);
		alphabetConverter->ResetUnconvertedNativeCharacters();
		alphabetConverter->ResetUnconvertedUDElements();
		i++;
		}
	CleanupStack::PopAndDestroy(2);
	}

void CTgsmuStepBase::DoBinaryTestConversionL(CSmsAlphabetConverter& aConverter,const TDesC8& aDescriptor)
	{

	TPtrC converted=aConverter.ConvertToNativeL(aDescriptor);
	TEST(aConverter.UnconvertedUDElements().Length()==0);
	TPtrC8 converted8(aConverter.ConvertFromNativeL(converted));
	TEST(aConverter.UnconvertedNativeCharacters().Length()==0);
	TEST(converted8.Compare(aDescriptor)==0);
	}

void CTgsmuStepBase::DoBinaryTestAlphabetConversionL(const TDesC8** aBufferTable)
	{

	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	CSmsAlphabetConverter* alphabetConverter=CSmsAlphabetConverter::NewLC(*characterSetConverter,iFs,TSmsDataCodingScheme::ESmsAlphabetReserved,ETrue);
	TInt i=0;
	while (aBufferTable[i]!=NULL)
		{
		DoBinaryTestConversionL(*alphabetConverter,*aBufferTable[i]);
		alphabetConverter->ResetUnconvertedNativeCharacters();
		alphabetConverter->ResetUnconvertedUDElements();
		i++;
		}
	CleanupStack::PopAndDestroy(2);
	}

void CTgsmuStepBase::DoConversionSplitTestL()
	{

	_LIT8(KSplitTestDataA,"ABCDEFGHIJK\x1b");
	_LIT8(KSplitTestDataB,"\x2fMNOPQRSTUVWXYZ");
	_LIT16(KSplitTestCorrectResult,"ABCDEFGHIJK\\MNOPQRSTUVWXYZ");

	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	CSmsAlphabetConverter* alphabetConverter=CSmsAlphabetConverter::NewLC(*characterSetConverter,iFs,TSmsDataCodingScheme::ESmsAlphabet7Bit,EFalse);

	TPtrC16 resultPtr(NULL,0);
	TBuf16<32> resultBuffer;

	// Do the conversion in parts
	resultPtr.Set(alphabetConverter->ConvertToNativeL(KSplitTestDataA));
	resultBuffer.Append(resultPtr);
	resultPtr.Set(alphabetConverter->ConvertToNativeL(KSplitTestDataB));
	resultBuffer.Append(resultPtr);

	// Check that the result is correct
	TEST(resultBuffer.Compare(KSplitTestCorrectResult)==0);
	TEST(alphabetConverter->UnconvertedUDElements().Length()==0);
	CleanupStack::PopAndDestroy(2);
	}

void CTgsmuStepBase::TestSmsProtocolIdentifierL()
{
    CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	//-- create an object of the 'CSmsPDU' class in order to test
	//--  TSmsProtocolIdentifier::SetTelematicDeviceIndicator() and
	//--  TSmsProtocolIdentifier::SetShortMessageType()
	//-- indirectly, by calling appropriate methods	of CSmsPDU class
	CSmsPDU *SmsPDU = CSmsPDU::NewL(CSmsPDU::ESmsCommand,*characterSetConverter,iFs );
	CleanupStack::PushL(SmsPDU);
	//----- Test TSmsProtocolIdentifier::SetTelematicDeviceIndicator()
	//using TSmsProtocolIdentifier::TSmsTelematicDeviceIndicator;

	const	TSmsProtocolIdentifier::TSmsTelematicDeviceIndicator TelematicDevInd_In=TSmsProtocolIdentifier::ESmsTelematicDevice;
			TSmsProtocolIdentifier::TSmsTelematicDeviceIndicator TelematicDevInd_Out;

	//-- call TSmsProtocolIdentifier::SetTelematicDeviceIndicator()
	SmsPDU->SetTelematicDeviceIndicator(TelematicDevInd_In);

	//-- call TSmsProtocolIdentifier::TelematicDeviceIndicator()
	TelematicDevInd_Out = SmsPDU->TelematicDeviceIndicator();

	if(TelematicDevInd_Out != TelematicDevInd_In)
	{
		INFO_PRINTF1(_L("TSmsProtocolIdentifier::SetTelematicDeviceIndicator() test FAILED !"));
		TEST(EFalse);
	}


	//----- Test TSmsProtocolIdentifier::SetShortMessageType()
	//using TSmsProtocolIdentifier::TSmsShortMessageType;

	const	TSmsProtocolIdentifier::TSmsShortMessageType SmsType_In=TSmsProtocolIdentifier::ESmsReplaceShortMessageType7;
			TSmsProtocolIdentifier::TSmsShortMessageType SmsType_Out;


	//-- call TSmsProtocolIdentifier::SetShortMessageType()
	SmsPDU->SetPIDType(TSmsProtocolIdentifier::ESmsPIDShortMessageType);
	SmsPDU->SetShortMessageType(SmsType_In);


	//-- call TSmsProtocolIdentifier::ShortMessageType()
	SmsType_Out = SmsPDU->ShortMessageType();

	if(SmsType_Out != SmsType_In)
	{
		INFO_PRINTF1(_L("TSmsProtocolIdentifier::SetShortMessageType() test FAILED !"));
		TEST(EFalse);
	}

	CleanupStack::PopAndDestroy(2);
}

void CTgsmuStepBase::TestSmsOptimiseSettingsL()
//this test step added to cover OptimiseSettings.
	{
	INFO_PRINTF1(_L("Test Sms Optimise Settings Started.."));
	CSmsBuffer* optBuffer=CSmsBuffer::NewL();
	CSmsMessage* optSmsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,optBuffer);
	CSmsPDU& optPdu = optSmsMessage->SmsPDU();
	CleanupStack::PushL(optSmsMessage);

	//encoding flag values

	TInt i7BitEncoding=0x00;
	TInt i8BitEncoding=0x04;
	TInt iUCSEncoding=0x08;

	TInt totalTestSteps = 1;
	TInt currentTestStep=0;
	TInt lastError=0;
	TInt origAlpha;
	TInt optimAlpha;

	while(currentTestStep<=totalTestSteps)
		{
		//the tests perform various optimisation scenarios on plain, wide and mixed strings of various lengths
		//the code section is quite long as we want to check for different things according to the input string.
		INFO_PRINTF2(_L("Running Test SubStep %d"),currentTestStep);

		switch(currentTestStep){
		case 0:
			{
			TBuf16<sizeof(K7BitTestData1)> optBuf;
			INFO_PRINTF1(_L("Using 7Bit Encodable String"));
			INFO_PRINTF2(_L("Testing String: %d"),K7BitTestData1);
			optBuf.Copy(K7BitTestData1);
			optBuffer->InsertL(0,optBuf);
			origAlpha = optPdu.Alphabet();//the original encoding setting.
			//check the existing Encoding setting, should be 7Bit/
			TEST((origAlpha==i7BitEncoding));
			if(origAlpha==i7BitEncoding)
				//7BitAlphabet is set
				INFO_PRINTF1(_L("OK:Before Optimising 7Bit Encoding is Set..."));

			else if(origAlpha==i8BitEncoding)
				{
				//8 Bit Alphabet is set
				INFO_PRINTF1(_L("ERROR:Before Optimising 8Bit Encoding is Set..."));
				lastError=1;
				}
			else if(origAlpha==iUCSEncoding)
				{
				//UCS2 is set
				INFO_PRINTF1(_L("ERROR:Before Optimising UCS Encoding is Set..."));
				lastError=2;
				}

			if(lastError)
				break;

			INFO_PRINTF1(_L("Calling Optimise Settings With Optimise SET..."));
			optSmsMessage->OptimizeSettingsL(0x01);
			optimAlpha = optPdu.Alphabet();//get the optimised encoding settings.
			TEST((origAlpha==optimAlpha));

			if((origAlpha && optimAlpha) == i7BitEncoding)
				{
				//correct result, optimised encoding set to 7bit
				INFO_PRINTF1(_L("OK:OptimiseSettings Set 7Bit Encoding:Optimised OK!"));
				}
			else if(optimAlpha==i8BitEncoding)
				{
				//print an error
				INFO_PRINTF1(_L("ERROR:OptimiseSettings Set 8Bit Encoding:Optimise Failed!"));
				lastError=3;
				}
			else if(optimAlpha==iUCSEncoding)
				{
				//print an error
				INFO_PRINTF1(_L("ERROR:OptimiseSettings Set UCS Encoding:Optimise Failed!"));
				lastError=4;
				}
			if(lastError)
				break;

			INFO_PRINTF1(_L("Now Testing with Optimise Flag NOT SET..."));

			optBuffer->Reset();//reset the buffer
			optBuffer->InsertL(0,optBuf);	//rewrite the buffer
			origAlpha = optPdu.Alphabet();//the original encoding setting.
			TEST((origAlpha==i7BitEncoding));

			if(origAlpha==i7BitEncoding)
				//7BitAlphabet is set
				INFO_PRINTF1(_L("OK:Before Optimising 7Bit Encoding is Set..."));

			else if(origAlpha==i8BitEncoding)
				{
				//8 Bit Alphabet is set
				INFO_PRINTF1(_L("ERROR:Before Optimising 8Bit Encoding is Set..."));
				lastError=5;
				}
			else if(origAlpha==iUCSEncoding)
				{
				//UCS2 is set
				INFO_PRINTF1(_L("ERROR:Before Optimising UCS Encoding is Set..."));
				lastError=6;
				}
			if(lastError)
				break;

			INFO_PRINTF1(_L("Calling Optimise Settings With Optimise not SET..."));
			optSmsMessage->OptimizeSettingsL(0x00);//check again with OptimiseFlag not set
			optimAlpha = optPdu.Alphabet();//get the optimised encoding settings.
			TEST(( origAlpha==optimAlpha ));

			if(origAlpha && optimAlpha == i7BitEncoding)
				//correct result, optimised encoding set to 7bit
				INFO_PRINTF1(_L("OK:OptimiseSettings did Not Change Encoding."));

			else if(optimAlpha==i8BitEncoding)
				{
				//print an error
				INFO_PRINTF1(_L("ERROR:OptimiseSettings Set 8Bit Encoding: Do Not Optimise Failed!"));
				lastError=7;
				}
			else if(optimAlpha==iUCSEncoding)
				{
				//print an error
				INFO_PRINTF1(_L("ERROR:OptimiseSettings Set UCS Encoding: Do Not Optimise Failed!"));
				lastError=8;
				}
			if(lastError)
				break;

			//now set to UCS2 and set Optimise flag to True
			INFO_PRINTF1(_L("Calling Optimise Settings With UCS2 Encoding Set..."));
			optBuffer->Reset();
			optBuffer->InsertL(0,optBuf);	//rewrite the buffer
			origAlpha = optPdu.Alphabet();//the original encoding setting.

			if(origAlpha!=iUCSEncoding)
				{
				INFO_PRINTF1(_L("Requesting Encoding Set to UCS2..."));
				optPdu.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabetUCS2);
				origAlpha = optPdu.Alphabet();

				if(origAlpha!=iUCSEncoding)
					{
					INFO_PRINTF1(_L("ERROR:Did Not Set to UCS2 Encoding..."));
					lastError=9;
					}
				else
					INFO_PRINTF1(_L("OK:Set to UCS2 Encoding..."));
				}

			TEST((origAlpha==iUCSEncoding));
			INFO_PRINTF1(_L("Now Testing with Optimise Flag SET..."));
			optSmsMessage->OptimizeSettingsL(0x01);//check again with OptimiseFlag set
			optimAlpha = optPdu.Alphabet();
			TEST((origAlpha==optimAlpha));

			if(origAlpha && optimAlpha == iUCSEncoding)
				{
				//correct result, optimised encoding set to 7bit
				INFO_PRINTF1(_L("OK:OptimiseSettings did Not Change Encoding."));
				}

			else if(optimAlpha==i7BitEncoding)
				{
				//print an error
				INFO_PRINTF1(_L("ERROR:OptimiseSettings Set 7Bit Encoding: Preserve User Setting Failed!"));
				lastError=10;
				}

			else if(optimAlpha==i8BitEncoding)
				{
				//print an error
				INFO_PRINTF1(_L("ERROR:OptimiseSettings Set 8Bit Encoding:  Failed!"));
				lastError=11;
				}
			if(lastError)
				break;

			optBuffer->Reset(); //clear the buffer of the old string
			optPdu.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet7Bit); //reset to default encoding.
			//1st step complete
			INFO_PRINTF2(_L("Completed %d ok!"),currentTestStep);
			}
			break;

		case 1:
			{
			TBuf16<sizeof(K7BitTestData5)> optBuf;
			INFO_PRINTF1(_L("Using 7Bit Encodable String "));
			INFO_PRINTF2(_L("Testing String: %d"),K7BitTestData4);
			optBuf.Copy(K7BitTestData5);
			optBuffer->InsertL(0,optBuf);
			origAlpha = optPdu.Alphabet();//the original encoding setting.
			//check the existing Encoding setting, should be 7Bit/
			TEST((origAlpha==i7BitEncoding));
			if(origAlpha==i7BitEncoding)
				{
				//7BitAlphabet is set
				INFO_PRINTF1(_L("OK:Before Optimising 7Bit Encoding is Set..."));
				}

			else if(origAlpha==i8BitEncoding)
				{
				//8 Bit Alphabet is set
				INFO_PRINTF1(_L("ERROR:Before Optimising 8Bit Encoding is Set..."));
				lastError=12;
				}

			else if(origAlpha==iUCSEncoding)
				{
				//UCS2 is set
				INFO_PRINTF1(_L("ERROR:Before Optimising UCS Encoding is Set..."));
				lastError=13;
				}
			if(lastError)
				break;

			INFO_PRINTF1(_L("Calling Optimise Settings With Optimise SET..."));
			optSmsMessage->OptimizeSettingsL(0x01);
			optimAlpha = optPdu.Alphabet();//get the optimised encoding settings.
			TEST((origAlpha==optimAlpha));

			if(origAlpha && optimAlpha == i7BitEncoding)
				{
				//correct result, optimised encoding set to 7bit
				INFO_PRINTF1(_L("OK:OptimiseSettings Set 7Bit Encoding:Optimised OK!"));
				}

			else if(optimAlpha==i8BitEncoding)
				{
				//print an error
				INFO_PRINTF1(_L("ERROR:OptimiseSettings Set 8Bit Encoding:Optimise Failed!"));
				lastError=14;
				}
			else if(optimAlpha==iUCSEncoding)
				{
				//print an error
				INFO_PRINTF1(_L("ERROR:OptimiseSettings Set UCS Encoding:Optimise Failed!"));
				lastError=15;
				}
			if(lastError)
				break;

			INFO_PRINTF1(_L("Now Testing with Optimise Flag NOT SET..."));

			optBuffer->Reset();//reset the buffer
			optBuffer->InsertL(0,optBuf);	//rewrite the buffer
			origAlpha = optPdu.Alphabet();//the original encoding setting.
			TEST((origAlpha==i7BitEncoding));

			if(origAlpha==i7BitEncoding)
				{
				//7BitAlphabet is set
				INFO_PRINTF1(_L("OK:Before Optimising 7Bit Encoding is Set..."));
				}

			else if(origAlpha==i8BitEncoding)
				{
				//8 Bit Alphabet is set
				INFO_PRINTF1(_L("ERROR:Before Optimising 8Bit Encoding is Set..."));
				lastError=16;
				}

			else if(origAlpha==iUCSEncoding)
				{
				//UCS2 is set
				INFO_PRINTF1(_L("ERROR:Before Optimising UCS Encoding is Set..."));
				lastError=17;
				}
			if(lastError)
				break;

			INFO_PRINTF1(_L("Calling Optimise Settings With Optimise not SET..."));
			optSmsMessage->OptimizeSettingsL(0x00);//check again with OptimiseFlag not set
			optimAlpha = optPdu.Alphabet();//get the optimised encoding settings.
			TEST(( origAlpha==optimAlpha ));

			if(origAlpha && optimAlpha == i7BitEncoding)
				{
				//correct result, optimised encoding set to 7bit
				INFO_PRINTF1(_L("OK:OptimiseSettings did Not Change Encoding."));
				}

			else if(optimAlpha==i8BitEncoding)
				{
				//print an error
				INFO_PRINTF1(_L("ERROR:OptimiseSettings Set 8Bit Encoding: Do Not Optimise Failed!"));
				lastError=18;
				}
			else if(optimAlpha==iUCSEncoding)
				{
				//print an error
				INFO_PRINTF1(_L("ERROR:OptimiseSettings Set UCS Encoding: Do Not Optimise Failed!"));
				lastError=19;
				}
			if(lastError)
				break;

			//now set to UCS2 and set Optimise flag to True
			INFO_PRINTF1(_L("Calling Optimise Settings With UCS2 Encoding Set..."));
			optBuffer->Reset();
			optBuffer->InsertL(0,optBuf);	//rewrite the buffer
			origAlpha = optPdu.Alphabet();//the original encoding setting.

			if(origAlpha!=iUCSEncoding)
				{
				INFO_PRINTF1(_L("Requesting Encoding Set to UCS2..."));
				optPdu.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabetUCS2);
				origAlpha = optPdu.Alphabet();

				if(origAlpha!=iUCSEncoding)
					{
					INFO_PRINTF1(_L("ERROR:Did Not Set to UCS2 Encoding..."));
					lastError=20;
					}

				else
					INFO_PRINTF1(_L("OK:Set to UCS2 Encoding..."));

				if(lastError)
					break;
				}

			TEST((origAlpha==iUCSEncoding));
			INFO_PRINTF1(_L("Now Testing with Optimise Flag SET..."));
			optSmsMessage->OptimizeSettingsL(0x01);//check again with OptimiseFlag set
			optimAlpha = optPdu.Alphabet();
			TEST(( origAlpha==optimAlpha ));

			if((origAlpha && optimAlpha) == iUCSEncoding)
				//correct result, optimised encoding set to 7bit
				INFO_PRINTF1(_L("OK:OptimiseSettings did Not Change Encoding."));

			else if(optimAlpha==i7BitEncoding)
				{
				//print an error
				INFO_PRINTF1(_L("ERROR:OptimiseSettings Set 7Bit Encoding: Preserve User Setting Failed!"));
				lastError=21;
				}
			else if(optimAlpha==i8BitEncoding)
				{
				//print an error
				INFO_PRINTF1(_L("ERROR:OptimiseSettings Set 8Bit Encoding:  Failed!"));
				lastError=22;
				}
			if(lastError)
				break;

			optBuffer->Reset(); //clear the buffer of the old string
			optPdu.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet7Bit); //reset to default encoding.
			//1st step complete
			INFO_PRINTF2(_L("Completed %d ok!"),currentTestStep);
			}
			break;

			//if we wish to add more test substeps for different strings (eg for different encodings)we can do it
			//here, just remember to increase the value of TotalTestSteps to reflect it.

		default:
			break;
		}
		//need to break the while loop if an error condition)

		if(lastError)
			break;
		else
			currentTestStep++;
		}

	//check if an error occurred
	TEST(lastError==0);

	if(!lastError)
		INFO_PRINTF1(_L("Completed Test ok!"));
	CleanupStack::PopAndDestroy(optSmsMessage);
}

void CTgsmuStepBase::TestSetAndGetSchemeL()
	{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);	

	TSmsStatusReportScheme schemeId;
	
	schemeId = smsMessage->Scheme();
	TEST(schemeId == EDefaultScheme);
	

	CSmsSMSCCtrlParameterOperations& operation = (CSmsSMSCCtrlParameterOperations&)(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));
	schemeId = operation.GetScheme();
	TEST(schemeId == EDefaultScheme);
	TEST(smsMessage->Scheme() == EDefaultScheme);
	
	operation.SetSchemeL();
	schemeId = operation.GetScheme();
	TEST(schemeId == EControlParametersScheme);
	TEST(smsMessage->Scheme() == EControlParametersScheme);
	
	operation.ResetSchemeL();
	schemeId = operation.GetScheme();
	TEST(schemeId == EDefaultScheme);
	TEST(smsMessage->Scheme() == EDefaultScheme);
	
	CleanupStack::PopAndDestroy(smsMessage);
	}
	
void CTgsmuStepBase::TestDefaultSetReportAndGetReportL()
	{
	_LIT(KLongText,"YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
		
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);

	TSmsUserDataSettings smsSettings;
	smsSettings.SetAlphabet(alphabet);
	smsSettings.SetTextCompressed(EFalse);
	smsMessage->SetUserDataSettingsL(smsSettings);
	
	smsMessage->SetToFromAddressL(_L("+447747065405"));
	smsMessage->SetServiceCenterAddressL(_L("+447785016005"));
	buffer->InsertL(0,KLongText);

	TSmsStatusReportScheme schemeId;
	
	CSmsSMSCCtrlParameterOperations& operation = (CSmsSMSCCtrlParameterOperations&)(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));	
	operation.SetSchemeL();
	schemeId = operation.GetScheme();
	TEST(schemeId == EControlParametersScheme);
	
	TUint8 selectiveStatus;
	TUint segmentSequenceNum(0);
	TInt  ret;
	
	ret = operation.GetStatusReport(segmentSequenceNum, selectiveStatus);
	TEST(ret == KErrNone);
	TEST(selectiveStatus == 0);
	
	operation.SetDefaultL(0x0A);
	ret = operation.GetStatusReport(segmentSequenceNum, selectiveStatus);
	TEST(ret == KErrNone);
	TEST(selectiveStatus == 0x0A);
	
	segmentSequenceNum = 1;
	ret = operation.GetStatusReport(segmentSequenceNum, selectiveStatus);
	TEST(ret == KErrNone);
	TEST(selectiveStatus ==0x0A);
	
	selectiveStatus = ESmsStatusReportPermanentError;
	segmentSequenceNum = 0;
	ret = operation.SetStatusReportL(segmentSequenceNum, selectiveStatus);
	TEST(ret == KErrNone);
	ret = operation.GetStatusReport(segmentSequenceNum, selectiveStatus);
	TEST(ret == KErrNone);
	TEST(selectiveStatus == ESmsStatusReportPermanentError);
	segmentSequenceNum = 1;
	ret = operation.GetStatusReport(segmentSequenceNum, selectiveStatus);
	TEST(ret == KErrNone);
	TEST(selectiveStatus == 0x0A);
	
	selectiveStatus = ESmsStatusReportTemporaryError;
	segmentSequenceNum = 1;
	operation.SetStatusReportL(segmentSequenceNum, selectiveStatus);
	ret = operation.GetStatusReport(segmentSequenceNum, selectiveStatus);
	TEST(ret == KErrNone);
	TEST(selectiveStatus == ESmsStatusReportTemporaryError);
	
	segmentSequenceNum = 2;
	ret = operation.GetStatusReport(segmentSequenceNum, selectiveStatus);
	TEST(ret == KErrNotFound);
	
	CleanupStack::PopAndDestroy(smsMessage);
	}
	
void CTgsmuStepBase::TestEncodeAndDecodeL()
	{
	_LIT(KLongText,"YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
		
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);

	TSmsUserDataSettings smsSettings;
	smsSettings.SetAlphabet(alphabet);
	smsSettings.SetTextCompressed(EFalse);
	smsMessage->SetUserDataSettingsL(smsSettings);
	
	smsMessage->SetToFromAddressL(_L("+447747065405"));
	smsMessage->SetServiceCenterAddressL(_L("+447785016005"));
	buffer->InsertL(0,KLongText);

	TSmsStatusReportScheme schemeId;
	
	CSmsSMSCCtrlParameterOperations& operation = (CSmsSMSCCtrlParameterOperations&)(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));	
	operation.SetSchemeL();
	schemeId = operation.GetScheme();
	TEST(schemeId == EControlParametersScheme);
	
	TUint8 selectiveStatus;
	TUint segmentSequenceNum(0);
	
	selectiveStatus = ESmsStatusReportPermanentError;
	operation.SetStatusReportL(segmentSequenceNum, selectiveStatus);
	
	selectiveStatus = ESmsStatusReportTemporaryError;
	segmentSequenceNum = 1;
	operation.SetStatusReportL(segmentSequenceNum, selectiveStatus);
	
	
	CArrayFixFlat<TGsmSms>* smsarray=new(ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(smsarray);
	TInt msgref=0;
	smsMessage->EncodeMessagePDUsL(*smsarray, msgref);
	
	smsMessage->DecodeMessagePDUsL(*smsarray);
	
	segmentSequenceNum = 0;
	TInt ret = operation.GetStatusReport(segmentSequenceNum, selectiveStatus);
	TEST(ret == KErrNone);
	TEST(selectiveStatus == ESmsStatusReportPermanentError);
	
	segmentSequenceNum = 1;
	ret = operation.GetStatusReport(segmentSequenceNum, selectiveStatus);
	TEST(ret == KErrNone);
	TEST(selectiveStatus == ESmsStatusReportTemporaryError);
	
	operation.ResetSchemeL();
	
	CleanupStack::PopAndDestroy(smsarray);
	CleanupStack::PopAndDestroy(smsMessage);
	}	

void CTgsmuStepBase::TestExternaliseAndInternaliseL()
	{
	_LIT(KLongText,"YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
		
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);

	TSmsUserDataSettings smsSettings;
	smsSettings.SetAlphabet(alphabet);
	smsSettings.SetTextCompressed(EFalse);
	smsMessage->SetUserDataSettingsL(smsSettings);
	
	smsMessage->SetToFromAddressL(_L("+447747065405"));
	smsMessage->SetServiceCenterAddressL(_L("+447785016005"));
	buffer->InsertL(0,KLongText);

	TSmsStatusReportScheme schemeId;
	
	CSmsTPSRROperations& operation = (CSmsTPSRROperations&)(smsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter));	
	operation.SetSchemeL();
	schemeId = operation.GetScheme();
	TEST(schemeId == ETPSRRScheme);
	
	TSmsFirstOctet::TSmsStatusReportRequest tpsrrStatus;
	TUint segmentSequenceNum(0);
	
	tpsrrStatus = TSmsFirstOctet::ESmsStatusReportRequested;
	operation.SetSegmentStatusReportL(segmentSequenceNum, tpsrrStatus);
		
	tpsrrStatus = TSmsFirstOctet::ESmsStatusReportNotRequested;
	segmentSequenceNum = 1;
	operation.SetSegmentStatusReportL(segmentSequenceNum, tpsrrStatus);
	
	_LIT(KTestFileName,"c:\\smsfp.txt");
	RFileReadStream rTestStream;
	CleanupClosePushL(rTestStream);
	RFileWriteStream wTestStream;
	CleanupClosePushL(wTestStream);
	RFs testFileReadSession;
	CleanupClosePushL(testFileReadSession);
	User::LeaveIfError(testFileReadSession.Connect());
	RFs testFileWriteSession;
	CleanupClosePushL(testFileWriteSession);
	User::LeaveIfError(testFileWriteSession.Connect());

	INFO_PRINTF1(_L("Externalising..."));
	wTestStream.Replace(testFileWriteSession,KTestFileName,EFileWrite);
	smsMessage->ExternalizeL(wTestStream);
	wTestStream.Close();

	INFO_PRINTF1(_L("Internalising..."));
	rTestStream.Open(testFileWriteSession,KTestFileName,EFileWrite);
	smsMessage->InternalizeL(rTestStream);
	rTestStream.Close();
	
	testFileReadSession.Close();
	testFileWriteSession.Close();
	
	schemeId = operation.GetScheme();
	TEST(schemeId == ETPSRRScheme);
	
	segmentSequenceNum = 0;
	tpsrrStatus = (TSmsFirstOctet::TSmsStatusReportRequest)operation.GetStatusReport(segmentSequenceNum);
	TEST(tpsrrStatus == TSmsFirstOctet::ESmsStatusReportRequested);
	
	segmentSequenceNum = 1;
	tpsrrStatus = (TSmsFirstOctet::TSmsStatusReportRequest)operation.GetStatusReport(segmentSequenceNum);
	TEST(tpsrrStatus == TSmsFirstOctet::ESmsStatusReportNotRequested);
	
	CleanupStack::PopAndDestroy(5);
	}
		
void CTgsmuStepBase::TestTPSRRSetAndGetSchemeL()
	{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);	

	TSmsStatusReportScheme schemeId;
	
	schemeId = smsMessage->Scheme();
	TEST(schemeId == EDefaultScheme);
	

	CSmsTPSRROperations& operation = (CSmsTPSRROperations&)(smsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter));
	schemeId = operation.GetScheme();
	TEST(schemeId == EDefaultScheme);
	
	operation.SetSchemeL();
	schemeId = operation.GetScheme();
	TEST(schemeId == ETPSRRScheme);
	
	operation.ResetSchemeL();
	schemeId = operation.GetScheme();
	TEST(schemeId == EDefaultScheme);
	
	CleanupStack::PopAndDestroy(smsMessage);
	}
	
void CTgsmuStepBase::TestTPSRRDefaultSetReportAndGetReportL()
	{
	_LIT(KLongText,"YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
		
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);

	TSmsUserDataSettings smsSettings;
	smsSettings.SetAlphabet(alphabet);
	smsSettings.SetTextCompressed(EFalse);
	smsMessage->SetUserDataSettingsL(smsSettings);
	
	smsMessage->SetToFromAddressL(_L("+447747065405"));
	smsMessage->SetServiceCenterAddressL(_L("+447785016005"));
	buffer->InsertL(0,KLongText);

	TSmsStatusReportScheme schemeId;
	
	CSmsTPSRROperations& operation = (CSmsTPSRROperations&)(smsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter));	
	operation.SetSchemeL();
	schemeId = operation.GetScheme();
	TEST(schemeId == ETPSRRScheme);
	
	TSmsFirstOctet::TSmsStatusReportRequest tpsrrStatus;
	TUint segmentSequenceNum(0);
	tpsrrStatus = (TSmsFirstOctet::TSmsStatusReportRequest)operation.GetStatusReport(segmentSequenceNum);
	TEST(tpsrrStatus == TSmsFirstOctet::ESmsStatusReportNotRequested);
	
	segmentSequenceNum = 9;
	TInt err = operation.GetStatusReport(segmentSequenceNum);
	TEST(err == KErrNotFound);
	
	operation.SetDefaultL(ETrue);
	segmentSequenceNum = 0;
	tpsrrStatus = (TSmsFirstOctet::TSmsStatusReportRequest)operation.GetStatusReport(segmentSequenceNum);
	TEST(tpsrrStatus == TSmsFirstOctet::ESmsStatusReportRequested);
	segmentSequenceNum = 1;
	tpsrrStatus = (TSmsFirstOctet::TSmsStatusReportRequest)operation.GetStatusReport(segmentSequenceNum);
	TEST(tpsrrStatus == TSmsFirstOctet::ESmsStatusReportRequested);
	
	tpsrrStatus = TSmsFirstOctet::ESmsStatusReportNotRequested;
	segmentSequenceNum = 0;
	operation.SetSegmentStatusReportL(segmentSequenceNum, tpsrrStatus);
	tpsrrStatus = (TSmsFirstOctet::TSmsStatusReportRequest)operation.GetStatusReport(segmentSequenceNum);
	TEST(tpsrrStatus == TSmsFirstOctet::ESmsStatusReportNotRequested);
	segmentSequenceNum = 1;
	tpsrrStatus = (TSmsFirstOctet::TSmsStatusReportRequest)operation.GetStatusReport(segmentSequenceNum);
	TEST(tpsrrStatus == TSmsFirstOctet::ESmsStatusReportRequested);
	
	operation.SetLastSegmentStatusReportL(ETrue);
	segmentSequenceNum = 1;
	tpsrrStatus = (TSmsFirstOctet::TSmsStatusReportRequest)operation.GetStatusReport(segmentSequenceNum);
	TEST(tpsrrStatus == TSmsFirstOctet::ESmsStatusReportRequested);
	
	CleanupStack::PopAndDestroy(smsMessage);
	}	
	
void CTgsmuStepBase::TestOnlyOneSchemeExistsL()
	{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);	

	TSmsStatusReportScheme schemeId;
	
	schemeId = smsMessage->Scheme();
	TEST(schemeId == EDefaultScheme);
	
	CSmsTPSRROperations& tpsrrOperation = (CSmsTPSRROperations&)(smsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter));
	tpsrrOperation.SetSchemeL();
	
	CSmsSMSCCtrlParameterOperations& smscOperation = (CSmsSMSCCtrlParameterOperations&)(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));	
	smscOperation.SetSchemeL();
	
	schemeId = tpsrrOperation.GetScheme();
	TEST(schemeId == EControlParametersScheme);
	
	schemeId = smscOperation.GetScheme();
	TEST(schemeId == EControlParametersScheme);
	
	tpsrrOperation.SetSchemeL();
	
	schemeId = smscOperation.GetScheme();
	TEST(schemeId == ETPSRRScheme);
	
	schemeId = tpsrrOperation.GetScheme();
	TEST(schemeId == ETPSRRScheme);
	
	CleanupStack::PopAndDestroy(smsMessage);	
	}
	
void CTgsmuStepBase::TestTPSRREncodeAndDecodeL()
	{
	_LIT(KLongText,"YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
		
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);

	TSmsUserDataSettings smsSettings;
	smsSettings.SetAlphabet(alphabet);
	smsSettings.SetTextCompressed(EFalse);
	smsMessage->SetUserDataSettingsL(smsSettings);
	
	smsMessage->SetToFromAddressL(_L("+447747065405"));
	smsMessage->SetServiceCenterAddressL(_L("+447785016005"));
	buffer->InsertL(0,KLongText);

	TSmsStatusReportScheme schemeId;
	
	CSmsTPSRROperations& operation = (CSmsTPSRROperations&)(smsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter));	
	operation.SetSchemeL();
	schemeId = operation.GetScheme();
	TEST(schemeId == ETPSRRScheme);
	
	TSmsFirstOctet::TSmsStatusReportRequest tpsrrStatus;
	TUint segmentSequenceNum(0);
	
	tpsrrStatus = TSmsFirstOctet::ESmsStatusReportNotRequested;
	operation.SetSegmentStatusReportL(segmentSequenceNum, tpsrrStatus);
	
	operation.SetLastSegmentStatusReportL(ETrue);
	
	
	CArrayFixFlat<TGsmSms>* smsarray=new(ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(smsarray);
	TInt msgref=0;
	smsMessage->EncodeMessagePDUsL(*smsarray, msgref);
	
	smsMessage->DecodeMessagePDUsL(*smsarray);
	
	segmentSequenceNum = 0;
	tpsrrStatus = (TSmsFirstOctet::TSmsStatusReportRequest)operation.GetStatusReport(segmentSequenceNum);
	TEST(tpsrrStatus == TSmsFirstOctet::ESmsStatusReportNotRequested);
	
	segmentSequenceNum = 1;
	tpsrrStatus = (TSmsFirstOctet::TSmsStatusReportRequest)operation.GetStatusReport(segmentSequenceNum);
	TEST(tpsrrStatus == TSmsFirstOctet::ESmsStatusReportRequested);
	
	CleanupStack::PopAndDestroy(smsarray);
	CleanupStack::PopAndDestroy(smsMessage);
	}
	
void CTgsmuStepBase::TestCSmsEditorBufferL()
	{
	// This case tests the class CSmsEditorBuffer by removing text from the buffer and replaces it with other text
	//TInt TestPassInt = 0;
	_LIT(KTestText, "I want a cup of tea");
	TInt lengthKTestText = 19;	
	//Used to check the functions are doing as expected
	_LIT(KTestText1, "I want a cup of ");
	TInt lengthKTestText1 = 16;
	_LIT(KTestText2, "I want a cup of coffee");
	TInt const lengthKTestText2 = 27;
	TInt lengthOfTEA = 3;
		
	
	TBufC<lengthKTestText2> bufferText(KTestText);
	
		
	CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);
	
	CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charFormatLayer);
	
	CRichText* bodyText = CRichText::NewL(paraFormatLayer, charFormatLayer);
	CleanupStack::PushL(bodyText);

	bodyText->InsertL(0,bufferText);
	
	CSmsEditorBuffer* editorBuffer = CSmsEditorBuffer::NewL(*bodyText);
	CleanupStack::PushL(editorBuffer);
	
	//1st test to see if the Length() behaves as expected
	TInt bufLength = editorBuffer->Length();
	TEST(bufLength == lengthKTestText);
	
	//Remove the word "tea" and replace with "coffee"
	TBuf<lengthKTestText2> readOut;
	
	
	//2nd test to see if the Extract() behaves as expected
	editorBuffer->Extract(readOut, 0, lengthKTestText);
	INFO_PRINTF2(_L("The contents of the Editor Buffer is --%S--"),&readOut);
	TEST(readOut == KTestText);
	
	editorBuffer->Extract(readOut, lengthKTestText1, lengthOfTEA);
	INFO_PRINTF2(_L("The Word we are removing is --%S--"),&readOut);
	
	
	//3rd test to see if the DeleteL() behaves as expected
	editorBuffer->DeleteL(lengthKTestText1, lengthOfTEA);	//Deletes the word "tea"	
	bufLength = editorBuffer->Length();
		
	
	editorBuffer->Extract(readOut, 0, bufLength);
	INFO_PRINTF2(_L("The text now reads --%S--"),&readOut);
	TEST(readOut == KTestText1);
	 

	INFO_PRINTF1(_L("Replacing the word -tea- with the word -coffee-"));
	_LIT(KCoffee, "coffee");	
	readOut.Delete(0,readOut.Length());	
	readOut.Append(KCoffee);

	
	//4th test to see if the InsertL() behaves as expected
	editorBuffer->InsertL(lengthKTestText1, readOut);		//Replaces the word "tea" with "coffee"
	bufLength = editorBuffer->Length();
	editorBuffer->Extract(readOut, 0, bufLength);
	INFO_PRINTF2(_L("The Editor Buffer now reads --%S--"),&readOut);
	TEST(readOut == KTestText2);
	
	
	//5th test to see if the Reset() behaves as expected
	editorBuffer->Reset();	
	bufLength = editorBuffer->Length();	
	INFO_PRINTF2(_L("The length of the EditorBuffer after calling Reset is %d"),bufLength);
	TEST(bufLength==0);
	
	CleanupStack::PopAndDestroy(4);
	}
//
//Start Of
//CTgsmuTestAlphabetConversion7Bit
//
TVerdict CTgsmuTestAlphabetConversion7Bit::doTestStepL()
{
	INFO_PRINTF1(_L("Test 7 bit alphabet conversion"));
	DoTestAlphabetConversionL(TSmsDataCodingScheme::ESmsAlphabet7Bit,K7BitTestDataTable);

	return TestStepResult();
}


//
//Start Of
//CTgsmuTestAlphabetConversion8Bit
//
TVerdict CTgsmuTestAlphabetConversion8Bit::doTestStepL()
{

	INFO_PRINTF1(_L("Test 8 bit alphabet conversion"));
	DoTestAlphabetConversionL(TSmsDataCodingScheme::ESmsAlphabet8Bit,K7BitTestDataTable);

	return TestStepResult();
}


//
//Start Of
//CTgsmuTestAlphabetConversionUCS2
//
TVerdict CTgsmuTestAlphabetConversionUCS2::doTestStepL()
{

	INFO_PRINTF1(_L("Test UCS2 alphabet conversion"));
	DoTestAlphabetConversionL(TSmsDataCodingScheme::ESmsAlphabetUCS2,K7BitTestDataTable);

	return TestStepResult();
}

//
//Start Of
//CTgsmuTestAlphabetConversionBinary
//
TVerdict CTgsmuTestAlphabetConversionBinary::doTestStepL()
{
	INFO_PRINTF1(_L("Test Binary alphabet conversion"));
	DoBinaryTestAlphabetConversionL(KBinaryTestDataTable);

	return TestStepResult();
}


//
//Start Of
//CTgsmuTestAlphabetConversionSplit
//
TVerdict CTgsmuTestAlphabetConversionSplit::doTestStepL()
{
	INFO_PRINTF1(_L("Test alphabet conversion with 7 bit"));
	DoConversionSplitTestL();

	return TestStepResult();
}


//
//Start Of
//CTgsmuTestSmsHeader
//
TVerdict CTgsmuTestSmsHeader::doTestStepL()
{
	INFO_PRINTF1(_L("Test new sms header"));
	TestSmsHeaderL();

	return TestStepResult();
}


//
//Start Of
//CTgsmuTestDecodePDUL
//
TVerdict CTgsmuTestDecodePDU::doTestStepL()
{
	INFO_PRINTF1(_L("Test Decode PDU"));
	TestDecodePDUL();

	return TestStepResult();
}

//
//Start Of
//CTgsmuTestCommandDataHeader
//
TVerdict CTgsmuTestCommandDataHeader::doTestStepL()
{
	INFO_PRINTF1(_L("Test command data header"));
	TestCommandDataHeaderL();

	return TestStepResult();
}

//
//Start Of
//CTgsmuTestSmsProtocolIdentifier
//
TVerdict CTgsmuTestSmsProtocolIdentifier::doTestStepL()
{
	INFO_PRINTF1(_L("Test sms protocol Identifier"));
	TestSmsProtocolIdentifierL();
	TestCommandDataHeaderL();

	return TestStepResult();
}


//
//Start Of
//CTgsmuTestSmsTONandNPI
//
TVerdict CTgsmuTestSmsTONandNPI::doTestStepL()
{
	INFO_PRINTF1(_L("Test Ton and Npi"));
	TestSmsTONandNPIL();
	TestCommandDataHeaderL();

	return TestStepResult();
}


//
//Start Of
//CTgsmuTestSmsOptimiseSettings
//
TVerdict CTgsmuTestSmsOptimiseSettings::doTestStepL()
{
	INFO_PRINTF1(_L("Test sms optimise settings"));
	TestSmsOptimiseSettingsL();
	TestCommandDataHeaderL();

	return TestStepResult();
}


//
//Start Of
//CTgsmuTestSmsDefaultVersion
//
TVerdict CTgsmuTestSmsDefaultVersion::doTestStepL()
	{
	INFO_PRINTF1(_L("Test sms default version"));

	return TestSmsDefaultVersionL();
	}


//
//Start Of
//CTgsmuTestSmsCurrentVersion
//
TVerdict CTgsmuTestSmsCurrentVersion::doTestStepL()
	{
	INFO_PRINTF1(_L("Test sms current version"));

	return TestSmsCurrentVersionL();
	}


//
//Start Of
//CTgsmuTestSmsVersionBackwards
//
TVerdict CTgsmuTestSmsVersionBackwards::doTestStepL()
	{
	INFO_PRINTF1(_L("Test sms version backwards"));

	return TestSmsVersionBackwardsL();
	}
	
//
//Start Of
//CTgsmTgsmuTestSMSCCtrlParameterOperations
//
TVerdict CTgsmTgsmuTestSMSCCtrlParameterOperations::doTestStepL()
	{
	INFO_PRINTF1(_L("Test SMSC Control Parameter"));
	TestSetAndGetSchemeL();
	TestDefaultSetReportAndGetReportL();
	TestEncodeAndDecodeL();

	return TestStepResult();
	}
	
//
//Start Of
//CTgsmTgsmuTestTPSRROperations
//
TVerdict CTgsmTgsmuTestTPSRROperations::doTestStepL()
	{
	INFO_PRINTF1(_L("Test SMSC Control Parameter"));
	TestTPSRRSetAndGetSchemeL();
	TestTPSRRDefaultSetReportAndGetReportL();
	TestOnlyOneSchemeExistsL();
	TestExternaliseAndInternaliseL();
	TestTPSRREncodeAndDecodeL();

	return TestStepResult();
	}
	

//
//Start Of
//CTgsmuTestCSmsEditorBuffer
//
TVerdict CTgsmuTestCSmsEditorBuffer::doTestStepL()
	{
	INFO_PRINTF1(_L("Test CSmsEditorBuffer"));
	TestCSmsEditorBufferL();

	return TestStepResult();
	}

TVerdict CTgsmuTestBCDAddress::doTestStepL()
	{
	INFO_PRINTF1(_L("Test BCD Address"));
	TText8 PDUtext[15]={ 0x15 ,0x00 ,0x0c ,0x91 ,0x44 ,0xba ,0xdc ,0xfe ,0x10 ,0x32 ,0x00 ,0x04 ,0xa7 ,0x01 ,0x78 };
	
	TPtrC8 ptr(PDUtext,sizeof(PDUtext)/sizeof(PDUtext[0]));
	NMobileSmsMessaging::TMobileSmsGsmTpdu pdu(ptr);
	TGsmSms sms;
	sms.SetPdu(pdu);
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	RFs fs;
	CSmsMessage* smsmessage1=CSmsMessage::NewL(fs, sms,buffer, EFalse,EFalse);
	CleanupStack::PushL(smsmessage1);

	TGsmSmsTelNumber address;
	smsmessage1->ParsedToFromAddress(address);
	INFO_PRINTF2(_L("To Address : %S"), &address.iTelNumber);
	
	_LIT(KAddress, "44*#abc0123");	
	TESTL(address.iTelNumber.Compare(KAddress)==0);
	
	smsmessage1->SetToFromAddressL(_L("+44AaBbCcDdEe"));
	smsmessage1->ParsedToFromAddress(address);
	INFO_PRINTF2(_L("To Address : %S"), &address.iTelNumber);
	_LIT(KAddress1, "44abc");	
	TESTL(address.iTelNumber.Compare(KAddress1)==0);
	
	CleanupStack::PopAndDestroy();
	
	return TestStepResult();
	}


/**
 *  Tests the APIs for setting and getting the alternative encoding methods.
 * 
 *  @return A TVerdict test result.
 */
TVerdict CTestAlternative7bitEncodingAPIs::doTestStepL()
	{
	//
	// Create an SMS Message object...
	//
	CSmsBuffer*  buffer = CSmsBuffer::NewL();
	CSmsMessage*  smsmessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit, buffer);
	CleanupStack::PushL(smsmessage);

	//
	// Verify the encode methods can be set correctly.
	//
	TESTCHECK(smsmessage->Alternative7bitEncoding(), ESmsEncodingNone);
	TESTCHECK(smsmessage->SetAlternative7bitEncoding(ESmsEncodingNone), KErrNone);
	TESTCHECK(smsmessage->Alternative7bitEncoding(), ESmsEncodingNone);

	TESTCHECK(smsmessage->SetAlternative7bitEncoding(ESmsEncodingTurkishSingleShift), KErrNone);
	TESTCHECK(smsmessage->Alternative7bitEncoding(), ESmsEncodingTurkishSingleShift);
	TESTCHECK(smsmessage->SetAlternative7bitEncoding(ESmsEncodingTurkishLockingShift), KErrNone);
	TESTCHECK(smsmessage->Alternative7bitEncoding(), ESmsEncodingTurkishLockingShift);
	TESTCHECK(smsmessage->SetAlternative7bitEncoding(ESmsEncodingTurkishLockingAndSingleShift), KErrNone);
	TESTCHECK(smsmessage->Alternative7bitEncoding(), ESmsEncodingTurkishLockingAndSingleShift);

//	TESTCHECK(smsmessage->SetAlternative7bitEncoding(ESmsEncodingSpanishSingleShift), KErrNone);
//	TESTCHECK(smsmessage->Alternative7bitEncoding(), ESmsEncodingSpanishSingleShift);

//	TESTCHECK(smsmessage->SetAlternative7bitEncoding(ESmsEncodingPortugueseSingleShift), KErrNone);
//	TESTCHECK(smsmessage->Alternative7bitEncoding(), ESmsEncodingPortugueseSingleShift);
//	TESTCHECK(smsmessage->SetAlternative7bitEncoding(ESmsEncodingPortugueseLockingShift), KErrNone);
//	TESTCHECK(smsmessage->Alternative7bitEncoding(), ESmsEncodingPortugueseLockingShift);
//	TESTCHECK(smsmessage->SetAlternative7bitEncoding(ESmsEncodingPortugueseLockingAndSingleShift), KErrNone);
//	TESTCHECK(smsmessage->Alternative7bitEncoding(), ESmsEncodingPortugueseLockingAndSingleShift);
	
	// Invalid encoders
	TESTCHECK(smsmessage->SetAlternative7bitEncoding((TSmsEncoding) 9999), KErrArgument);
	TESTCHECK(smsmessage->Alternative7bitEncoding(), ESmsEncodingTurkishLockingAndSingleShift);
	TESTCHECK(smsmessage->SetAlternative7bitEncoding(ESmsEncodingNone), KErrNone);
	TESTCHECK(smsmessage->Alternative7bitEncoding(), ESmsEncodingNone);
	TESTCHECK(smsmessage->SetAlternative7bitEncoding((TSmsEncoding) 9999), KErrArgument);
	TESTCHECK(smsmessage->Alternative7bitEncoding(), ESmsEncodingNone);
	
	//
	// Clean up...
	//
	CleanupStack::PopAndDestroy(smsmessage);
	
	return TestStepResult();
	} // CTestAlternative7bitEncodingAPIs::doTestStepL
