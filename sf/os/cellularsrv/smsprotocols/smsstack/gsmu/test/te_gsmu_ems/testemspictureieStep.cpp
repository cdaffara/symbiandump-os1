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
#include "testemspictureieStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestemspictureieStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
		CFbsBitmap* bitmap=NULL;
		CEmsPictureIE* pic=NULL;
		FbsStartup();
		User::LeaveIfError(RFbsSession::Connect());

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KPicsMBM,0));

		pic = CEmsPictureIE::NewL(*bitmap);
		CleanupStack::PushL(pic);
		pic->SetStartPosition(5);

		CEmsPictureIE* pic2 = static_cast<CEmsPictureIE*>(pic->DuplicateL());
		CleanupStack::PushL(pic2);

		CEmsPictureIE* pic3 = CEmsPictureIE::NewL(*bitmap);
		CleanupStack::PushL(pic3);
		pic3->CopyL(*pic2);
		TEST_CHECKL(pic3->SizeInPixels().iWidth,pic->SizeInPixels().iWidth,_L("picture width is not the same"));
		TEST_CHECKL(pic3->SizeInPixels().iHeight,pic->SizeInPixels().iHeight,_L("picture height is not the same"));
		TEST_CHECKL(pic3->StartPosition(),pic->StartPosition(),_L("Start position is not the same"));

		CFbsBitmap* bitmap3=pic3->GetBitmapL();
		CleanupStack::PushL(bitmap3);

		CFbsBitmap* bitmap0=pic->GetBitmapL();
		CleanupStack::PushL(bitmap0);
		CleanupStack::PopAndDestroy(bitmap0);
		CleanupStack::PopAndDestroy(bitmap3);
		CleanupStack::PopAndDestroy(pic3);
		CleanupStack::PopAndDestroy(pic2);




		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);
		//
		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KPicsMBM,1));

		pic = CEmsPictureIE::NewL(*bitmap);
		CleanupStack::PushL(pic);
		pic->SetStartPosition(5);

		pic2 = (CEmsPictureIE*)pic->DuplicateL();
		CleanupStack::PushL(pic2);

		pic3 = CEmsPictureIE::NewL(*bitmap);
		CleanupStack::PushL(pic3);
		pic3->CopyL(*pic2);
		TEST_CHECKL(pic3->SizeInPixels().iWidth,pic->SizeInPixels().iWidth,_L("picture width is not the same"));
		TEST_CHECKL(pic3->SizeInPixels().iHeight,pic->SizeInPixels().iHeight,_L("picture height is not the same"));
		TEST_CHECKL(pic3->StartPosition(),pic->StartPosition(),_L("Start position is not the same"));

		bitmap3=pic3->GetBitmapL();
		CleanupStack::PushL(bitmap3);

		bitmap0=pic->GetBitmapL();
		CleanupStack::PushL(bitmap0);
		CleanupStack::PopAndDestroy(bitmap0);
		CleanupStack::PopAndDestroy(bitmap3);
		CleanupStack::PopAndDestroy(pic3);
		CleanupStack::PopAndDestroy(pic2);

		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);

		//
		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);
		TESTL(!LoadBitmap(*bitmap,KPicsMBM,2));

		pic = CEmsPictureIE::NewL(*bitmap);
		CleanupStack::PushL(pic);
		pic->SetStartPosition(5);


		pic2 = (CEmsPictureIE*)pic->DuplicateL();
		CleanupStack::PushL(pic2);

		pic3 = CEmsPictureIE::NewL(*bitmap);
		CleanupStack::PushL(pic3);
		pic3->CopyL(*pic2);
		TEST_CHECKL(pic3->SizeInPixels().iWidth,pic->SizeInPixels().iWidth,_L("picture width is not the same"));
		TEST_CHECKL(pic3->SizeInPixels().iHeight,pic->SizeInPixels().iHeight,_L("picture height is not the same"));
		TEST_CHECKL(pic3->StartPosition(),pic->StartPosition(),_L("Start position is not the same"));

		bitmap3=pic3->GetBitmapL();
		CleanupStack::PushL(bitmap3);

		bitmap0=pic->GetBitmapL();
		CleanupStack::PushL(bitmap0);
		CleanupStack::PopAndDestroy(bitmap0);
		CleanupStack::PopAndDestroy(bitmap3);
		CleanupStack::PopAndDestroy(pic3);
		CleanupStack::PopAndDestroy(pic2);

		CleanupStack::PopAndDestroy(pic);
		CleanupStack::PopAndDestroy(bitmap);
		RFbsSession::Disconnect();

	return TestStepResult();
	}


