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
#include "testemsanimationieStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestemsanimationieStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
		CEmsAnimationIE* anim=NULL;

		FbsStartup();
		User::LeaveIfError(RFbsSession::Connect());

		CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KAnimsMBM,0));

		anim = CEmsAnimationIE::NewL(*bitmap);
		CleanupStack::PushL(anim);
		anim->SetStartPosition(5);

		CEmsAnimationIE* anim2 = static_cast<CEmsAnimationIE*>(anim->DuplicateL());
		CleanupStack::PushL(anim2);

		CEmsAnimationIE* anim3 = CEmsAnimationIE::NewL(*bitmap);
		CleanupStack::PushL(anim3);
		anim3->CopyL(*anim2);


		CFbsBitmap* bitmap3=anim3->GetBitmapL();
		CleanupStack::PushL(bitmap3);

		CFbsBitmap* bitmap0=anim->GetBitmapL();
		CleanupStack::PushL(bitmap0);

		TEST_CHECKL(bitmap0->SizeInPixels().iWidth,bitmap3->SizeInPixels().iWidth,_L("picture width is not the same"));
		TEST_CHECKL(bitmap0->SizeInPixels().iHeight,bitmap3->SizeInPixels().iHeight,_L("picture height is not the same"));
		TEST_CHECKL(anim3->StartPosition(),anim->StartPosition(),_L("Start position is not the same"));


		CleanupStack::PopAndDestroy(bitmap0);
		CleanupStack::PopAndDestroy(bitmap3);
		CleanupStack::PopAndDestroy(anim3);
		CleanupStack::PopAndDestroy(anim2);



		CleanupStack::PopAndDestroy(anim);
		CleanupStack::PopAndDestroy(bitmap);

		bitmap = new (ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap);

		TESTL(!LoadBitmap(*bitmap,KAnimsMBM,1));
		anim = CEmsAnimationIE::NewL(*bitmap);
		CleanupStack::PushL(anim);
		anim->SetStartPosition(5);
		CleanupStack::PopAndDestroy(anim);
		CleanupStack::PopAndDestroy(bitmap);

		RFbsSession::Disconnect();

	return TestStepResult();
	}


