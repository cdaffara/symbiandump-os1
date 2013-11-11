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
#include "testsamepositionStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestsamepositionStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
		_LIT(KTestEmsMsg8Bit1,"");
		_LIT(KTestEmsMsg8Bit2,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

//case 1
		CSmsMessage* smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit1,TSmsDataCodingScheme::ESmsAlphabet8Bit);
		CleanupStack::PushL(smsMessage);


// sounds
		CEmsSoundIE* object2 = CEmsSoundIE::NewL(KItchyMelody());
		object2->SetStartPosition(0);
		CleanupStack::PushL(object2);
		smsMessage->AddEMSInformationElementL(*object2);
		CleanupStack::PopAndDestroy(object2);

		CEmsSoundIE* object2a = CEmsSoundIE::NewL(KBarbieMelody());
		object2a->SetStartPosition(0);
		CleanupStack::PushL(object2a);
		smsMessage->AddEMSInformationElementL(*object2a);
		CleanupStack::PopAndDestroy(object2a);


		CEmsPreDefSoundIE* object3 = CEmsPreDefSoundIE::NewL( CEmsPreDefSoundIE::EChordLow);
		object3->SetStartPosition(0);
		CleanupStack::PushL(object3);
		smsMessage->AddEMSInformationElementL(*object3);
		CleanupStack::PopAndDestroy(object3);

		CEmsFormatIE* object = CEmsFormatIE::NewL();
		object->SetStartPosition(0);
		object->SetFormatLength(261);
		object->SetBold(ETrue);
		CleanupStack::PushL(object);
		smsMessage->AddEMSInformationElementL(*object);
		CleanupStack::PopAndDestroy(object);


// pictures
		CFbsBitmap* bitmap=NULL;
		CEmsPictureIE* pic=NULL;
		FbsStartup();
		User::LeaveIfError(RFbsSession::Connect());

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(LoadBitmap(*bitmap,KPicsMBM,0)==KErrNone);

		pic = CEmsPictureIE::NewL(*bitmap);
		pic->SetStartPosition(0);
		CleanupStack::PushL(pic);
		smsMessage->AddEMSInformationElementL(*pic);
		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(LoadBitmap(*bitmap,KPicsMBM,1)==KErrNone);

		pic = CEmsPictureIE::NewL(*bitmap);
		pic->SetStartPosition(0);
		CleanupStack::PushL(pic);
		smsMessage->AddEMSInformationElementL(*pic);
		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(LoadBitmap(*bitmap,KPicsMBM,2)==KErrNone);

		pic = CEmsPictureIE::NewL(*bitmap);
		pic->SetStartPosition(0);
		CleanupStack::PushL(pic);
		smsMessage->AddEMSInformationElementL(*pic);
		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		RFbsSession::Disconnect();

		TInt num=smsMessage->NumMessagePDUsL();
		TESTL(num==4);
		CleanupStack::PopAndDestroy(smsMessage);

//case 2
		smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit2,TSmsDataCodingScheme::ESmsAlphabet8Bit);
		CleanupStack::PushL(smsMessage);


// sounds
		object2 = CEmsSoundIE::NewL(KOasisMelody());
		object2->SetStartPosition(10);
		CleanupStack::PushL(object2);
		smsMessage->AddEMSInformationElementL(*object2);
		CleanupStack::PopAndDestroy(object2);

		object2a = CEmsSoundIE::NewL(KItchyMelody());
		object2a->SetStartPosition(10);
		CleanupStack::PushL(object2a);
		smsMessage->AddEMSInformationElementL(*object2a);
		CleanupStack::PopAndDestroy(object2a);
		// user prompt
		CEmsUserPrompt* prompt = CEmsUserPrompt::NewL(2);
		prompt->SetStartPosition(10);
		CleanupStack::PushL(prompt);
		smsMessage->AddEMSInformationElementL(*prompt);
		CleanupStack::PopAndDestroy(prompt);

		object3 = CEmsPreDefSoundIE::NewL( CEmsPreDefSoundIE::EChordLow);
		object3->SetStartPosition(11);
		CleanupStack::PushL(object3);
		smsMessage->AddEMSInformationElementL(*object3);
		CleanupStack::PopAndDestroy(object3);

		// object distribution
		CEmsObjectDistribution* odi = CEmsObjectDistribution::NewL(3, CEmsObjectDistribution::EForward);
		odi->SetStartPosition(10);
		CleanupStack::PushL(odi);
		smsMessage->AddEMSInformationElementL(*odi);
		CleanupStack::PopAndDestroy(odi);

// pictures

		FbsStartup();
		User::LeaveIfError(RFbsSession::Connect());

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(LoadBitmap(*bitmap,KPicsMBM,0)==KErrNone);

		pic = CEmsPictureIE::NewL(*bitmap);
		pic->SetStartPosition(11);
		CleanupStack::PushL(pic);
		smsMessage->AddEMSInformationElementL(*pic);
		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(LoadBitmap(*bitmap,KPicsMBM,1)==KErrNone);

		pic = CEmsPictureIE::NewL(*bitmap);
		pic->SetStartPosition(11);
		CleanupStack::PushL(pic);
		smsMessage->AddEMSInformationElementL(*pic);
		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(LoadBitmap(*bitmap,KPicsMBM,2)==KErrNone);

		pic = CEmsPictureIE::NewL(*bitmap);
		pic->SetStartPosition(11);
		CleanupStack::PushL(pic);
		smsMessage->AddEMSInformationElementL(*pic);
		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		RFbsSession::Disconnect();

		num=smsMessage->NumMessagePDUsL();
		INFO_PRINTF2(_L("Number of segments of smsMessage %d"),num);
		CleanupStack::PopAndDestroy(smsMessage);
		TESTL(num==6);

	return TestStepResult();
	}


