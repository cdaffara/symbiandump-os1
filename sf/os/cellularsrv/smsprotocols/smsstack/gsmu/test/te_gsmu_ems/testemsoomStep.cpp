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
#include "testemsoomStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestemsoomStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
	TInt ret=KErrNone;
	// test for all new methods
	// format
		CEmsFormatIE* object=NULL;
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,object = CEmsFormatIE::NewL());
		delete object;
		__UHEAP_MARKEND;

	// sounds
		CEmsSoundIE* object2 =NULL;
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,object2 = CEmsSoundIE::NewL(KOasisMelody()));
		__UHEAP_MARKEND;

	// predef sound
		CEmsPreDefSoundIE* object3 = NULL;
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,object3=CEmsPreDefSoundIE::NewL( CEmsPreDefSoundIE::EChordLow));
		__UHEAP_MARKEND;

	// user prompt

		CEmsUserPrompt* prompt = NULL;
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,prompt=CEmsUserPrompt::NewL(2));
		__UHEAP_MARKEND;

	// object distribution

		CEmsObjectDistribution* odi = NULL;
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,odi=CEmsObjectDistribution::NewL(2, CEmsObjectDistribution::EForward));
		__UHEAP_MARKEND;

	// pictures
		CFbsBitmap* bitmap=NULL;
		CEmsPictureIE* pic=NULL;
		FbsStartup();
		User::LeaveIfError(RFbsSession::Connect());

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KPicsMBM,0));

		__UHEAP_SETFAIL(RHeap::EDeterministic,2);
		__UHEAP_MARK;
		TRAP(ret,pic = CEmsPictureIE::NewL(*bitmap));
		__UHEAP_MARKEND;


		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,pic = CEmsPictureIE::NewL(*bitmap));
		__UHEAP_MARKEND;


		CleanupStack::PopAndDestroy(bitmap);
		RFbsSession::Disconnect();

	// animations
		CEmsAnimationIE* anim=NULL;
		CEmsPreDefAnimationIE* predefanim=NULL;

		FbsStartup();
		User::LeaveIfError(RFbsSession::Connect());

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);


		TESTL(!LoadBitmap(*bitmap,KAnimsMBM,0));

		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,anim = CEmsAnimationIE::NewL(*bitmap));
		__UHEAP_MARKEND;

		__UHEAP_SETFAIL(RHeap::EDeterministic,2);
		__UHEAP_MARK;
		TRAP(ret,anim = CEmsAnimationIE::NewL(*bitmap));
		__UHEAP_MARKEND;

		CleanupStack::PopAndDestroy(bitmap);


		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,predefanim = CEmsPreDefAnimationIE::NewL(CEmsPreDefAnimationIE::EWinking));
		__UHEAP_MARKEND;

		RFbsSession::Disconnect();


		// test add to CSmsMessage
		_LIT(KTestEmsMsg8Bit1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
		TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;

		__UHEAP_MARK;
		CSmsMessage* smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit1,alphabet);
		CleanupStack::PushL(smsMessage);
		// format
		object = CEmsFormatIE::NewL();
		object->SetStartPosition(10);
		object->SetFormatLength(1200);
		object->SetBold(ETrue);
		CleanupStack::PushL(object);
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*object));
		smsMessage->ResetEMSL();
		__UHEAP_MARKEND;
		if(ret != KErrNone)smsMessage->AddEMSInformationElementL(*object);
		CleanupStack::PopAndDestroy(object);

		// sounds
		object2 = CEmsSoundIE::NewL(KOasisMelody());
		object2->SetStartPosition(52);
		CleanupStack::PushL(object2);
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*object2));
		smsMessage->ResetEMSL();
		__UHEAP_MARKEND;
		if(ret != KErrNone)smsMessage->AddEMSInformationElementL(*object2);
		CleanupStack::PopAndDestroy(object2);

		CEmsSoundIE* object2a = CEmsSoundIE::NewL(KBarbieMelody());
		object2a->SetStartPosition(52);
		CleanupStack::PushL(object2a);
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*object2a));
		smsMessage->ResetEMSL();
		__UHEAP_MARKEND;
		if(ret != KErrNone)smsMessage->AddEMSInformationElementL(*object2a);
		CleanupStack::PopAndDestroy(object2a);

		// user prompt
		prompt = CEmsUserPrompt::NewL(2);
		prompt->SetStartPosition(52);
		CleanupStack::PushL(prompt);
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*prompt));
		__UHEAP_MARKEND;

		CleanupStack::PopAndDestroy(prompt);

		object3 = CEmsPreDefSoundIE::NewL( CEmsPreDefSoundIE::EChordLow);
		object3->SetStartPosition(5);
		CleanupStack::PushL(object3);
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*object3));
		smsMessage->ResetEMSL();
		__UHEAP_MARKEND;
		if(ret != KErrNone)smsMessage->AddEMSInformationElementL(*object3);
		CleanupStack::PopAndDestroy(object3);

		// object distribution
		odi = CEmsObjectDistribution::NewL(3, CEmsObjectDistribution::EForward);
		odi->SetStartPosition(52);
		CleanupStack::PushL(odi);
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*odi));
		__UHEAP_MARKEND;

		CleanupStack::PopAndDestroy(odi);

	// pictures

		FbsStartup();
		User::LeaveIfError(RFbsSession::Connect());

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KPicsMBM,0));

		pic = CEmsPictureIE::NewL(*bitmap);
		pic->SetStartPosition(5);
		CleanupStack::PushL(pic);
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*pic));
		smsMessage->ResetEMSL();
		__UHEAP_MARKEND;
		if(ret != KErrNone)smsMessage->AddEMSInformationElementL(*pic);
		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KPicsMBM,1));

		pic = CEmsPictureIE::NewL(*bitmap);
		pic->SetStartPosition(5);
		CleanupStack::PushL(pic);
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*pic));
		smsMessage->ResetEMSL();
		__UHEAP_MARKEND;
		if(ret != KErrNone)smsMessage->AddEMSInformationElementL(*pic);
		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KPicsMBM,2));

		pic = CEmsPictureIE::NewL(*bitmap);
		pic->SetStartPosition(5);
		CleanupStack::PushL(pic);
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*pic));
		smsMessage->ResetEMSL();
		__UHEAP_MARKEND;
		if(ret != KErrNone)smsMessage->AddEMSInformationElementL(*pic);
		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		RFbsSession::Disconnect();

	// animations
		FbsStartup();
		User::LeaveIfError(RFbsSession::Connect());

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KAnimsMBM,0));

		anim = CEmsAnimationIE::NewL(*bitmap);
		anim->SetStartPosition(5);
		CleanupStack::PushL(anim);
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*anim));
		smsMessage->ResetEMSL();
		__UHEAP_MARKEND;
		if(ret != KErrNone)smsMessage->AddEMSInformationElementL(*anim);
		CleanupStack::PopAndDestroy(anim);
		CleanupStack::PopAndDestroy(bitmap);

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KAnimsMBM,1));

		anim = CEmsAnimationIE::NewL(*bitmap);
		anim->SetStartPosition(5);
		CleanupStack::PushL(anim);
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*anim));
		smsMessage->ResetEMSL();
		__UHEAP_MARKEND;
		if(ret != KErrNone)smsMessage->AddEMSInformationElementL(*anim);
		CleanupStack::PopAndDestroy(anim);
		CleanupStack::PopAndDestroy(bitmap);

		predefanim = CEmsPreDefAnimationIE::NewL(CEmsPreDefAnimationIE::EWinking);
		predefanim->SetStartPosition(5);
		CleanupStack::PushL(predefanim);
		__UHEAP_FAILNEXT(1);
		__UHEAP_MARK;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*predefanim));
		smsMessage->ResetEMSL();
		__UHEAP_MARKEND;
		if(ret != KErrNone)smsMessage->AddEMSInformationElementL(*predefanim);
		CleanupStack::PopAndDestroy(predefanim);

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

	for(TInt k=1;k<=400;++k)
	{
		RBufReadStream readstream(*aReadBufSeg);
		readstream.Open(*aReadBufSeg,0);
		CleanupClosePushL(readstream);

		RBufWriteStream writestream(*aReadBufSeg);
		writestream.Open(*aReadBufSeg,0);
		CleanupClosePushL(writestream);

		__UHEAP_SETFAIL(RHeap::EDeterministic,k);
		TRAP(ret,writestream << *smsMessage );
		__UHEAP_RESET;
		CleanupStack::PopAndDestroy();  //  writesream
		CleanupStack::PopAndDestroy();  //  readsream
	}


	writestream << *smsMessage ;
	readstream  >> *smsMessage2;
	CleanupStack::PopAndDestroy();  //  writesream
	CleanupStack::PopAndDestroy();  //  readsream
	//
	CleanupStack::PopAndDestroy(aReadBufSeg);
	CleanupStack::PopAndDestroy(smsMessage2);
	CleanupStack::PopAndDestroy(smsMessage);

	__UHEAP_MARKEND;

	return TestStepResult();
	}

