// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "testformattingsegmentationStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestformattingsegmentationStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
	_LIT(KTestEmsMsg8Bit1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	_LIT(KTestEmsMsg8Bit2,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	_LIT(KTestEmsMsg8Bit3,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	_LIT(KTestEmsMsg16Bit1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	//case 1 - FORMAT STRETCHING OVER THREE PDUS
	CSmsMessage* smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit1,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(smsMessage);

	CEmsFormatIE* object = CEmsFormatIE::NewL();
	object->SetStartPosition(10);
	object->SetFormatLength(261);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	TInt num=smsMessage->NumMessagePDUsL();
	TESTL(num==3);
	CleanupStack::PopAndDestroy(smsMessage);

	//case 2 - FORMAT STRETCHING OVER TWO PDUS
	smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit2,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(10);
	object->SetFormatLength(261);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();
	TESTL(num==2);
	CleanupStack::PopAndDestroy(smsMessage);

	//case 3 - FORMAT STRETCHING OVER MIDDLE PDU
	smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit3,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(134);
	object->SetFormatLength(129);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();
	TESTL(num==3);
	CleanupStack::PopAndDestroy(smsMessage);

	//case 4 - FORMAT STRETCHING OVER THREE PDUS
	smsMessage=CreateSmsMessageL(KTestEmsMsg16Bit1,TSmsDataCodingScheme::ESmsAlphabetUCS2);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(10);
	object->SetFormatLength(261);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();
	TESTL(num==3);
	CleanupStack::PopAndDestroy(smsMessage);

	//case 4 - correct coding of text format with objects and segmentation
	//Regresion test for fixing Defect LEE-5AEF7Z.

	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* emsMessage=CreateSmsMessageL(KLongFormatMsg,alphabet);
	CleanupStack::PushL(emsMessage);

	CEmsFormatIE* object1 = CEmsFormatIE::NewL();
	CleanupStack::PushL(object1);

	object1->SetStartPosition(10);
	object1->SetFormatLength(100);
	object1->SetBold(ETrue);
	object1->SetItalic(ETrue);
	object1->SetUnderline(ETrue);
	object1->SetStrikethrough(ETrue);

	emsMessage->AddEMSInformationElementL(*object1);
	CleanupStack::PopAndDestroy(object1);

	CFbsBitmap* bitmap=NULL;
	CEmsPictureIE* object2=NULL;
	bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);

	TESTL(LoadBitmap(*bitmap,KPicsMBM,1)==KErrNone);

	object2 = CEmsPictureIE::NewL(*bitmap);
	CleanupStack::PushL(object2);
	object2->SetStartPosition(20);

	emsMessage->AddEMSInformationElementL(*object2);

	// now force the encoding
	TInt numMsg = emsMessage->NumMessagePDUsL();
	INFO_PRINTF2(_L("Number of segments = %d "), numMsg);
	//number of IEs, IEIDs, and their position in received message
	const RPointerArray<const CEmsInformationElement>&  emsElements= emsMessage->GetEMSInformationElementsL();
	TInt numElements = emsElements.Count();
	INFO_PRINTF2(_L("Message contains %d Information Elements!"), numElements);

	TInt i=0;
	while(i<numElements)
	{
		const CEmsInformationElement& ie=*emsElements[i];
		switch(ie.Identifier())
		{
			case CSmsInformationElement::ESmsEnhancedTextFormatting:
			{
				CEmsFormatIE& formatIe = (CEmsFormatIE&) ie;
				INFO_PRINTF3(_L("Text formating IE start position: %d length: %d "),formatIe.StartPosition(),formatIe.FormatLength());
				TESTL(formatIe.StartPosition()==10);
				TESTL(formatIe.FormatLength()==100);
			}
				break;
			case CSmsInformationElement::ESmsEnhancedLargePicture:
			{
				CEmsPictureIE& picIe = (CEmsPictureIE&) ie;
				INFO_PRINTF2(_L("Large picture IE start position: %d "),picIe.StartPosition());
				TESTL(picIe.StartPosition()==20);
			}
				break;
			default:
				break;
		}
		i++;
	}

	CleanupStack::PopAndDestroy(object2);
	CleanupStack::PopAndDestroy(bitmap);
	CleanupStack::PopAndDestroy(emsMessage);
	RFbsSession::Disconnect();

		//case 6 - Correct Formatting

	smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit1,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(10);
	object->SetFormatLength(500);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();
	TESTL(num==3);
	CleanupStack::PopAndDestroy(smsMessage);
	
	//case 7 - correct segmentation at PDU boundary
	// Test added for Defect PDEF103591.
	// This tests specifically for the condition when the user data and EMS data together become segmented into more than one PDU (two in
	// this case), and the first PDU gets packed entirely, such that there are zero free units remaining in that PDU.
	// The following data was found to achieve this condition: "XXX[PictureElement]XXXX[SoundClip]"
	// where X is any alphabet character.
	// The choice of the EMS elements and their positioning is crucial for the required segmentaion outcome to be tested.
	// See DEF098401 for further information.
	
	_LIT(KTestProblemMsg, "123 :-) ");   // Represents the same user data format used in DEF098401
	const TInt KPicturePos = 3;
	const TInt KSoundPos = 7;

	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());
	CSmsMessage* emsMessage1=CreateSmsMessageL(KTestProblemMsg, TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(emsMessage1);

	// Add picture
	CFbsBitmap* bitmapP=NULL;
	CEmsPictureIE* objectP=NULL;
	bitmapP = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmapP);

	TESTL(LoadBitmap(*bitmapP, KPicsMBM, 1)==KErrNone);

	objectP = CEmsPictureIE::NewL(*bitmapP);
	CleanupStack::PushL(objectP);
	objectP->SetStartPosition(KPicturePos);

	emsMessage1->AddEMSInformationElementL(*objectP);
	
	// Add predifined sound clip
	CEmsPreDefSoundIE* objectS = CEmsPreDefSoundIE::NewL( CEmsPreDefSoundIE::EClaps);
	CleanupStack::PushL(objectS);
	objectS->SetStartPosition(KSoundPos);

	emsMessage1->AddEMSInformationElementL(*objectS);
	CleanupStack::PopAndDestroy(objectS);

	// Now force the encoding - this performs segmentation of the user data.
	TInt numMsg1(0);
	TRAPD(ret, numMsg1 = emsMessage1->NumMessagePDUsL());
	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("Segmentation of message caused Panic!"), ret);
		}
	
	// Expect 2 PDU's
	TESTL(numMsg1 == 2);

	CleanupStack::PopAndDestroy(objectP);
	CleanupStack::PopAndDestroy(bitmapP);

	CleanupStack::PopAndDestroy(emsMessage1);
	RFbsSession::Disconnect();
	
	return TestStepResult();
	}


