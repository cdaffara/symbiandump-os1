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
#include "testcsmsmessageemsgetapiStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestcsmsmessageemsgetapiStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
		_LIT(KTestEmsMsg8Bit1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit1,alphabet);
	CleanupStack::PushL(smsMessage);
	// format
		CEmsFormatIE* object = CEmsFormatIE::NewL();
		object->SetStartPosition(100);
		object->SetFormatLength(120);
		object->SetBold(ETrue);
		CleanupStack::PushL(object);
		smsMessage->AddEMSInformationElementL(*object);
		CleanupStack::PopAndDestroy(object);
	// sounds
		CEmsSoundIE* object2 = CEmsSoundIE::NewL(KOasisMelody());
		object2->SetStartPosition(52);
		CleanupStack::PushL(object2);
		smsMessage->AddEMSInformationElementL(*object2);
		CleanupStack::PopAndDestroy(object2);

		CEmsSoundIE* object2a = CEmsSoundIE::NewL(KBarbieMelody());
		object2a->SetStartPosition(52);
		CleanupStack::PushL(object2a);
		smsMessage->AddEMSInformationElementL(*object2a);
		CleanupStack::PopAndDestroy(object2a);
		// user prompt
		CEmsUserPrompt* prompt = CEmsUserPrompt::NewL(2);
		prompt->SetStartPosition(52);
		CleanupStack::PushL(prompt);
		smsMessage->AddEMSInformationElementL(*prompt);
		CleanupStack::PopAndDestroy(prompt);

		CEmsPreDefSoundIE* object3 = CEmsPreDefSoundIE::NewL( CEmsPreDefSoundIE::EChordLow);
		object3->SetStartPosition(5);
		CleanupStack::PushL(object3);
		smsMessage->AddEMSInformationElementL(*object3);
		CleanupStack::PopAndDestroy(object3);

		// object distribution
		CEmsObjectDistribution* odi = CEmsObjectDistribution::NewL(3, CEmsObjectDistribution::EForward);
		odi->SetStartPosition(52);
		CleanupStack::PushL(odi);
		smsMessage->AddEMSInformationElementL(*odi);
		CleanupStack::PopAndDestroy(odi);

	// pictures
		CFbsBitmap* bitmap=NULL;
		CEmsPictureIE* pic=NULL;
		FbsStartup();
		User::LeaveIfError(RFbsSession::Connect());

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KPicsMBM,0));

		pic = CEmsPictureIE::NewL(*bitmap);
		pic->SetStartPosition(5);
		CleanupStack::PushL(pic);
		smsMessage->AddEMSInformationElementL(*pic);
		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KPicsMBM,1));

		pic = CEmsPictureIE::NewL(*bitmap);
		pic->SetStartPosition(5);
		CleanupStack::PushL(pic);
		smsMessage->AddEMSInformationElementL(*pic);
		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KPicsMBM,2));

		pic = CEmsPictureIE::NewL(*bitmap);
		pic->SetStartPosition(5);
		CleanupStack::PushL(pic);
		smsMessage->AddEMSInformationElementL(*pic);
		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		RFbsSession::Disconnect();
	// animations
		CEmsAnimationIE* anim=NULL;

		FbsStartup();
		User::LeaveIfError(RFbsSession::Connect());

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KAnimsMBM,0));

		anim = CEmsAnimationIE::NewL(*bitmap);
		anim->SetStartPosition(5);
		CleanupStack::PushL(anim);
		smsMessage->AddEMSInformationElementL(*anim);
		CleanupStack::PopAndDestroy(anim);
		CleanupStack::PopAndDestroy(bitmap);

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KAnimsMBM,1));

		anim = CEmsAnimationIE::NewL(*bitmap);
		anim->SetStartPosition(5);
		CleanupStack::PushL(anim);
		smsMessage->AddEMSInformationElementL(*anim);
		CleanupStack::PopAndDestroy(anim);
		CleanupStack::PopAndDestroy(bitmap);

		RFbsSession::Disconnect();


	CSmsMessage* smsMessage2=CreateSmsMessageL(KTestEmsMsg8Bit1,alphabet);
	CleanupStack::PushL(smsMessage2);

	CBufSeg* aReadBufSeg = CBufSeg::NewL(KSmsMaxEMSLength);
	CleanupStack::PushL(aReadBufSeg);

	RBufReadStream readstream(*aReadBufSeg);
	readstream.Open(*aReadBufSeg,0);
	CleanupClosePushL(readstream);

	RBufWriteStream writestream(*aReadBufSeg);
	writestream.Open(*aReadBufSeg,0);
	CleanupClosePushL(writestream);

	writestream << *smsMessage ;
	readstream  >> *smsMessage2;
	CleanupStack::PopAndDestroy();  //  writesream
	CleanupStack::PopAndDestroy();  //  readsream
	//
	CleanupStack::PopAndDestroy(aReadBufSeg);

	const RPointerArray<const CEmsInformationElement>&  emsElements= smsMessage2->GetEMSInformationElementsL();
	INFO_PRINTF2(_L("Number of IE elements in the smsMessage %d"),emsElements.Count());
	CleanupStack::PopAndDestroy(smsMessage2);
	CleanupStack::PopAndDestroy(smsMessage);

	return TestStepResult();
	}


