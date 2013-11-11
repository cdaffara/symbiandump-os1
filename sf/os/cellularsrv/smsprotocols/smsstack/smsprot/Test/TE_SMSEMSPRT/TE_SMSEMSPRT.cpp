// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TE_SMSEMSPRT.h"

_LIT(KBasicSmsText,"abcdefghijklmnopqrstuvwxyz"); // The basic text we send - 26 chars long

TVerdict CTestEmsFormatA::doTestStepL()
	{
	// send a message 26 characters long
	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// create the formatting object
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	object->SetStartPosition(5);
	object->SetFormatLength(20);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Send, receive and compare messages
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsFormatB::doTestStepL()
	{
	TBuf<26*8> buf;

	// create a buffer based on KBasicSmsText
	for (TInt i = 0; i < 8; ++i)
		{
		buf.Append(KBasicSmsText);
		}

	CSmsMessage* smsMessage = CreateSmsMessageLC(buf, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// create the formatting object
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	object->SetStartPosition(120);
	object->SetFormatLength(60);
	object->SetAlignment(CEmsFormatIE::ERight);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Send, receive and compare messages
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsFormatC::doTestStepL()
	{
	TBuf<26*8> buf;

	// create a buffer based on KBasicSmsText
	for (TInt i = 0; i < 8; ++i)
		{
		buf.Append(KBasicSmsText);
		}

	CSmsMessage* smsMessage = CreateSmsMessageLC(buf, TSmsDataCodingScheme::ESmsAlphabet7Bit);

	// create the formatting object
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	object->SetStartPosition(120);
	object->SetFormatLength(60);
	object->SetAlignment(CEmsFormatIE::ERight);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Send, receive and compare messages
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsFormatD::doTestStepL()
	{
	TBuf<26*4> buf;

	// create a buffer based on KBasicSmsText
	for (TInt i = 0; i < 4; ++i)
		{
		buf.Append(KBasicSmsText);
		}

	CSmsMessage* smsMessage = CreateSmsMessageLC(buf, TSmsDataCodingScheme::ESmsAlphabetUCS2);

	// create the formatting object
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	object->SetStartPosition(60);
	object->SetFormatLength(30);
	object->SetAlignment(CEmsFormatIE::ERight);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Send, receive and compare messages
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsFormatE::doTestStepL()
	{
	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// create the formatting object - set the start position to be out of range
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	CleanupStack::PushL(object);

	object->SetStartPosition(60);
	object->SetFormatLength(30);
	object->SetAlignment(CEmsFormatIE::ERight);

	TInt ret = KErrNone;
	TRAP(ret, smsMessage->AddEMSInformationElementL(*object));

	// make sure the return value is not none - the AddEMS... should
	// leave because the start position is out of range
	TEST(ret != KErrNone);

	// Try setting start position to be valid, but the length to be
	// invalid. On sending, the length should be truncated to 16
	object->SetStartPosition(10);
	object->SetFormatLength(30);
	smsMessage->AddEMSInformationElementL(*object);

	SendSmsL(smsMessage, iSocket);

	CleanupStack::PopAndDestroy(object);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsFormatF::doTestStepL()
	{
	TBuf<26*8> buf;

	// create a buffer based on KBasicSmsText
	for (TInt i = 0; i < 8; ++i)
		{
		buf.Append(KBasicSmsText);
		}

	// send a message 26*8 characters long
	CSmsMessage* smsMessage = CreateSmsMessageLC(buf, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// create the formatting object
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	object->SetStartPosition(160);
	object->SetFormatLength(40);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Send, receive and compare messages
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsSoundA::doTestStepL()
	{
	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// create the sound object - EChimes at pos 5
	CEmsPreDefSoundIE* object =
		CEmsPreDefSoundIE::NewL(CEmsPreDefSoundIE::EChimesLow);
	object->SetStartPosition(5);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// create another sound object - EDing at pos 20
	object = CEmsPreDefSoundIE::NewL(CEmsPreDefSoundIE::EDing);
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Send and receive message
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsSoundB::doTestStepL()
	{
	// User melody
	_LIT8(KUserMelody, 	"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
	  "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f");

	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// create one melody at position 20
	CEmsSoundIE* object =
		CEmsSoundIE::NewL(KUserMelody());
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// create another melody at position 5
	object = CEmsSoundIE::NewL(KUserMelody());
	object->SetStartPosition(5);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Send and receive message
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsSoundC::doTestStepL()
	{
	TBuf<26*4> buf;

	// create a 104 byte buffer based on KBasicSmsText
	for (TInt i = 0; i < 4; ++i)
		{
		buf.Append(KBasicSmsText);
		}

	CSmsMessage* smsMessage = CreateSmsMessageLC(buf, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// Create a 128-byte sound - fill it with byte 0x02
	TBuf8<128> soundBuf;
	soundBuf.Fill(TChar(0x02), 128);

	// create first melody at position 20
	CEmsSoundIE* object =
		CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// create another melody at position 60
	object = CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(60);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Send and receive message
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsSoundD::doTestStepL()
	{
	// Create a 130-byte sound - fill it with byte 0x02
	TBuf8<130> soundBuf;
	soundBuf.Fill(TChar(0x02), 130);

	// create melody 130 bytes long - should return error
	TInt ret = KErrNone;

	TRAP(ret,  CEmsSoundIE::NewL(soundBuf));
	TEST(ret != KErrNone);
	return TestStepResult();
	}

TVerdict CTestEmsPicture::doTestStepL()
	{
	// Create a standard message
	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	for (TInt i = 0; i < 3; ++i)
		{
		// Create the picture objects and set the start positions to
		// be at 0, 10, and 20. These pictures are loaded from
		// \ems\pics.mbm and uses the CreatePicture helper
		CEmsPictureIE* picture = EmsTestUtils::CreatePictureL(i);
		picture->SetStartPosition(i * 10);
		CleanupStack::PushL(picture);
		smsMessage->AddEMSInformationElementL(*picture);
		CleanupStack::PopAndDestroy(picture);
		}

	// Send and receive message
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsAnimation::doTestStepL()
	{
	// Create a standard message
	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	for (TInt i = 0; i < 2; ++i)
		{
		// Create the animation object and set the start positions to
		// be at 0 and 10
		CEmsAnimationIE* anim = EmsTestUtils::CreateAnimationL(i);
		anim->SetStartPosition(i * 10);
		CleanupStack::PushL(anim);
		smsMessage->AddEMSInformationElementL(*anim);
		CleanupStack::PopAndDestroy(anim);
		}

	// now create the predefined animation and put it in position 20
	CEmsPreDefAnimationIE* preanim =
		CEmsPreDefAnimationIE::NewL(CEmsPreDefAnimationIE::EDevil);
	preanim->SetStartPosition(20);
	CleanupStack::PushL(preanim);
	smsMessage->AddEMSInformationElementL(*preanim);
	CleanupStack::PopAndDestroy(preanim);

	// Send and receive message
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsUserPromptA::doTestStepL()
	{
	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// Create a 32-byte sound - fill it with byte 0x02
	TBuf8<32> soundBuf;
	soundBuf.Fill(TChar(0x02), 32);

	// create first melody at position 20
	CEmsSoundIE* object =
		CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// create another melody at position 20
	object = CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Now create a UPI at position 20
	CEmsUserPrompt* up = CEmsUserPrompt::NewL(2);
	up->SetStartPosition(20);
	CleanupStack::PushL(up);
	smsMessage->AddEMSInformationElementL(*up);
	CleanupStack::PopAndDestroy(up);

	// Send and receive message
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsUserPromptB::doTestStepL()
	{
	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// Create a 32-byte sound - fill it with byte 0x02
	TBuf8<32> soundBuf;
	soundBuf.Fill(TChar(0x02), 32);

	// create first melody at position 20
	CEmsSoundIE* object =
		CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// create another melody at position 21
	object = CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(21);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Now create a UPI at position 20
	CEmsUserPrompt* up = CEmsUserPrompt::NewL(1);
	up->SetStartPosition(20);
	CleanupStack::PushL(up);
	smsMessage->AddEMSInformationElementL(*up);
	CleanupStack::PopAndDestroy(up);

	// Now create a UPI at position 21
	up = CEmsUserPrompt::NewL(1);
	up->SetStartPosition(21);
	CleanupStack::PushL(up);
	smsMessage->AddEMSInformationElementL(*up);
	CleanupStack::PopAndDestroy(up);

	// Send and receive message
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

void CTestEmsLongMsgBase::StepWithCharSetL(TSmsDataCodingScheme::TSmsAlphabet aAlphabet)
	{
	TBuf<26*4> buf;

	// create a buffer based on KBasicSmsText
	for (TInt i = 0; i < 4; ++i)
		{
		buf.Append(KBasicSmsText);
		}

	CSmsMessage* smsMessage = CreateSmsMessageLC(buf, aAlphabet);

	// Create the format object
	CEmsFormatIE* format = CEmsFormatIE::NewL();
	format->SetStartPosition(30);
	format->SetFormatLength(70);
	format->SetAlignment(CEmsFormatIE::ERight);
	CleanupStack::PushL(format);
	smsMessage->AddEMSInformationElementL(*format);
	CleanupStack::PopAndDestroy(format);

	// Create a 128-byte sound - fill it with byte 0x02
	TBuf8<128> soundBuf;
	soundBuf.Fill(TChar(0x02), 128);
	CEmsSoundIE* sound =
		CEmsSoundIE::NewL(soundBuf);
	sound->SetStartPosition(40);
	CleanupStack::PushL(sound);
	smsMessage->AddEMSInformationElementL(*sound);
	CleanupStack::PopAndDestroy(sound);

	// Load the large bitmap
	CEmsPictureIE* picture = EmsTestUtils::CreatePictureL(1);
	picture->SetStartPosition(50);
	CleanupStack::PushL(picture);
	smsMessage->AddEMSInformationElementL(*picture);
	CleanupStack::PopAndDestroy(picture);

	// Send the message
	SendSmsL(smsMessage, iSocket);
	CleanupStack::PopAndDestroy(smsMessage);
	}

TVerdict CTestEmsLongMessageA::doTestStepL()
	{
	StepWithCharSetL(TSmsDataCodingScheme::ESmsAlphabet8Bit);
	return TestStepResult();
	}

TVerdict CTestEmsLongMessageB::doTestStepL()
	{
	StepWithCharSetL(TSmsDataCodingScheme::ESmsAlphabet7Bit);
	return TestStepResult();
	}

TVerdict CTestEmsLongMessageC::doTestStepL()
	{
	StepWithCharSetL(TSmsDataCodingScheme::ESmsAlphabetUCS2);
	return TestStepResult();
	}

TVerdict CTestEmsUnsupportedIE::doTestStepL()
	{
	TBuf<10> buf;
	buf.Fill('A', 10);

	CSmsMessage* smsMessage = CreateSmsMessageLC(buf, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// Create a 4-byte sound - fill it with byte 0x02
	TBuf8<4> soundBuf;
	soundBuf.Fill(TChar(0x02), 4);
	CEmsSoundIE* object =
		CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(5);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Send and receive message - the received message will be the same
	// as the sent one except there is an additional unsupported IE.
	// The compare will succeed because the text will be the same and
	// the successfully received ems objects will be the same.
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsUnsorted::doTestStepL()
	{
	TBuf<10> buf;
	buf.Fill('A', 10);

	CSmsMessage* smsMessage = CreateSmsMessageLC(buf, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// Create a 4-byte sound - fill it with byte 0x02
	TBuf8<4> soundBuf;
	soundBuf.Fill(TChar(0x02), 4);
	CEmsSoundIE* sound =
		CEmsSoundIE::NewL(soundBuf);
	sound->SetStartPosition(1);
	CleanupStack::PushL(sound);
	smsMessage->AddEMSInformationElementL(*sound);
	CleanupStack::PopAndDestroy(sound);

	// create a predefined animation - start position 2
	CEmsPreDefAnimationIE* preanim =
		CEmsPreDefAnimationIE::NewL(CEmsPreDefAnimationIE::EDevil);
	preanim->SetStartPosition(2);
	CleanupStack::PushL(preanim);
	smsMessage->AddEMSInformationElementL(*preanim);
	CleanupStack::PopAndDestroy(preanim);

	// create a format object - start position 3, length 1
	CEmsFormatIE* format = CEmsFormatIE::NewL();
	format->SetStartPosition(3);
	format->SetFormatLength(1);
	format->SetBold(ETrue);
	CleanupStack::PushL(format);
	smsMessage->AddEMSInformationElementL(*format);
	CleanupStack::PopAndDestroy(format);

	// Create a predefined sound - start position 3
	CEmsPreDefSoundIE* presound =
		CEmsPreDefSoundIE::NewL(CEmsPreDefSoundIE::EChimesLow);
	presound->SetStartPosition(3);
	CleanupStack::PushL(presound);
	smsMessage->AddEMSInformationElementL(*presound);
	CleanupStack::PopAndDestroy(presound);

	// create another format object - start position 6, length 2
	format = CEmsFormatIE::NewL();
	format->SetStartPosition(6);
	format->SetFormatLength(2);
	format->SetItalic(ETrue);
	CleanupStack::PushL(format);
	smsMessage->AddEMSInformationElementL(*format);
	CleanupStack::PopAndDestroy(format);

	// Send and receive message - the received message is encoded in
	// the config file as being out of order. However, it should
	// be decoded OK
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsODIA::doTestStepL()
	{
	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// Create a 32-byte sound - fill it with byte 0x02
	TBuf8<32> soundBuf;
	soundBuf.Fill(TChar(0x02), 32);

	// create first melody at position 20
	CEmsSoundIE* object =
		CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// create another melody at position 20
	object = CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Now create a ODI at position 20
	CEmsObjectDistribution* odi = CEmsObjectDistribution::NewL(2,CEmsObjectDistribution::EForward);
	odi->SetStartPosition(20);
	CleanupStack::PushL(odi);
	smsMessage->AddEMSInformationElementL(*odi);
	CleanupStack::PopAndDestroy(odi);

	// Send and receive message
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsODIB::doTestStepL()
	{
	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// Create a 32-byte sound - fill it with byte 0x02
	TBuf8<32> soundBuf;
	soundBuf.Fill(TChar(0x02), 32);

	// create first melody at position 20
	CEmsSoundIE* object = CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// create another melody at position 21
	object = CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(21);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Now create a ODI at position 20
	CEmsObjectDistribution* odi = CEmsObjectDistribution::NewL(1,CEmsObjectDistribution::EForward);
	odi->SetStartPosition(20);
	CleanupStack::PushL(odi);
	smsMessage->AddEMSInformationElementL(*odi);
	CleanupStack::PopAndDestroy(odi);

	// Now create a ODI at position 21
	odi = CEmsObjectDistribution::NewL(1,CEmsObjectDistribution::EForward);
	odi->SetStartPosition(21);
	CleanupStack::PushL(odi);
	smsMessage->AddEMSInformationElementL(*odi);
	CleanupStack::PopAndDestroy(odi);

	// Send and receive message
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsODIC::doTestStepL()
	{
	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// Create a 32-byte sound - fill it with byte 0x02
	TBuf8<32> soundBuf;
	soundBuf.Fill(TChar(0x02), 32);

	// create first melody at position 20
	CEmsSoundIE* object = CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// create another melody at position 21
	object = CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(21);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Now create a UPI at position 20
	CEmsUserPrompt* up = CEmsUserPrompt::NewL(1);
	up->SetStartPosition(20);
	CleanupStack::PushL(up);
	smsMessage->AddEMSInformationElementL(*up);
	CleanupStack::PopAndDestroy(up);

	// Now create a UPI at position 21
	up = CEmsUserPrompt::NewL(1);
	up->SetStartPosition(21);
	CleanupStack::PushL(up);
	smsMessage->AddEMSInformationElementL(*up);
	CleanupStack::PopAndDestroy(up);

	// Now create a ODI at position 20
	CEmsObjectDistribution* odi = CEmsObjectDistribution::NewL(2,CEmsObjectDistribution::EForward);
	odi->SetStartPosition(20);
	CleanupStack::PushL(odi);
	smsMessage->AddEMSInformationElementL(*odi);
	CleanupStack::PopAndDestroy(odi);

	// Now create a ODI at position 21
	odi = CEmsObjectDistribution::NewL(2,CEmsObjectDistribution::EForward);
	odi->SetStartPosition(21);
	CleanupStack::PushL(odi);
	smsMessage->AddEMSInformationElementL(*odi);
	CleanupStack::PopAndDestroy(odi);

	// Send and receive message
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsODID::doTestStepL()
	{
	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// Create a 32-byte sound - fill it with byte 0x02
	TBuf8<32> soundBuf;
	soundBuf.Fill(TChar(0x02), 32);

	// create first melody at position 20
	CEmsSoundIE* object = CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// create another melody at position 20
	object = CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Now create a ODI at position 20
	CEmsObjectDistribution* odi = CEmsObjectDistribution::NewL(0,CEmsObjectDistribution::EForward);
	odi->SetStartPosition(20);
	CleanupStack::PushL(odi);
	smsMessage->AddEMSInformationElementL(*odi);
	CleanupStack::PopAndDestroy(odi);

	// Send and receive message
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsODIE::doTestStepL()
	{
	TBuf<26*4> buf;

	// create a buffer based on KBasicSmsText
	for (TInt i = 0; i < 4; ++i)
		{
		buf.Append(KBasicSmsText);
		}

	CSmsMessage* smsMessage = CreateSmsMessageLC(buf, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// Create the format object
	CEmsFormatIE* format = CEmsFormatIE::NewL();
	format->SetStartPosition(30);
	format->SetFormatLength(70);
	format->SetAlignment(CEmsFormatIE::ERight);
	CleanupStack::PushL(format);
	smsMessage->AddEMSInformationElementL(*format);
	CleanupStack::PopAndDestroy(format);

	// Create a 128-byte sound - fill it with byte 0x02
	TBuf8<128> soundBuf;
	soundBuf.Fill(TChar(0x02), 128);
	CEmsSoundIE* sound =
		CEmsSoundIE::NewL(soundBuf);
	sound->SetStartPosition(40);
	CleanupStack::PushL(sound);
	smsMessage->AddEMSInformationElementL(*sound);
	CleanupStack::PopAndDestroy(sound);

	// Load the large bitmap
	CEmsPictureIE* picture = EmsTestUtils::CreatePictureL(1);
	picture->SetStartPosition(50);
	CleanupStack::PushL(picture);
	smsMessage->AddEMSInformationElementL(*picture);
	CleanupStack::PopAndDestroy(picture);

	// Now create a ODI in first PDU
	CEmsObjectDistribution* odi = CEmsObjectDistribution::NewL(2,CEmsObjectDistribution::EForward);
	odi->SetStartPosition(30);
	CleanupStack::PushL(odi);
	smsMessage->AddEMSInformationElementL(*odi);
	CleanupStack::PopAndDestroy(odi);

	// And add another ODI in third PDU
	odi = CEmsObjectDistribution::NewL(1,CEmsObjectDistribution::EForward);
	odi->SetStartPosition(49);
	CleanupStack::PushL(odi);
	smsMessage->AddEMSInformationElementL(*odi);
	CleanupStack::PopAndDestroy(odi);

	// Send the message
	SendSmsL(smsMessage, iSocket);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestEmsODIF::doTestStepL()
	{
	CSmsMessage* smsMessage = CreateSmsMessageLC(KBasicSmsText, TSmsDataCodingScheme::ESmsAlphabet8Bit);

	// Create a 32-byte sound - fill it with byte 0x02
	TBuf8<32> soundBuf;
	soundBuf.Fill(TChar(0x02), 32);

	// create first melody at position 20
	CEmsSoundIE* object = CEmsSoundIE::NewL(soundBuf);
	object->SetStartPosition(20);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	// Now create a ODI at position 19
	CEmsObjectDistribution* odi = CEmsObjectDistribution::NewL(1,CEmsObjectDistribution::EForward);
	odi->SetStartPosition(19);
	CleanupStack::PushL(odi);
	smsMessage->AddEMSInformationElementL(*odi);
	CleanupStack::PopAndDestroy(odi);

	// Now create a UPI at position 20
	CEmsUserPrompt* up = CEmsUserPrompt::NewL(1);
	up->SetStartPosition(20);
	CleanupStack::PushL(up);
	smsMessage->AddEMSInformationElementL(*up);
	CleanupStack::PopAndDestroy(up);

	// Send and receive message
	SendReceiveMsgL(*smsMessage);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}
