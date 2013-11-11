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
#include "testcorruptedinformationelementsStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestcorruptedinformationelementsStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("Testing corrupted information elements"));

	// Test too big melody buffer
	TInt ret;
	TRAP(ret,CEmsSoundIE::NewL(KTooBigMelody()));
	TESTL(ret==KErrCorrupt);

	// Test corrupted pictures

	CFbsBitmap* bitmap=NULL;
	CEmsPictureIE* pic=NULL;
	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());

	// Test too big bitmap 32X40
	bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	TESTL(LoadBitmap(*bitmap,KPicsMBM,3)==KErrNone);
	TRAP(ret,pic = CEmsPictureIE::NewL(*bitmap));
	TESTL(ret==KErrTooBig);
	CleanupStack::PopAndDestroy(bitmap);


	// Test too big bitmap 32X33
	bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	TESTL(LoadBitmap(*bitmap,KPicsMBM,4)==KErrNone);
	TRAP(ret,pic = CEmsPictureIE::NewL(*bitmap));
	TESTL(ret==KErrTooBig);
	CleanupStack::PopAndDestroy(bitmap);

	// Test OK bitmap 32X31
	bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	TESTL(LoadBitmap(*bitmap,KPicsMBM,5)==KErrNone);
	TRAP(ret,pic = CEmsPictureIE::NewL(*bitmap));
	CleanupStack::PushL(pic);
	TESTL(ret==KErrNone);
	CleanupStack::PopAndDestroy(pic);
	CleanupStack::PopAndDestroy(bitmap);


	// Test incorrect width 31X32
	bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	TESTL(LoadBitmap(*bitmap,KPicsMBM,6)==KErrNone);
	TRAP(ret,pic = CEmsPictureIE::NewL(*bitmap));
	TESTL(ret==KErrCorrupt);
	CleanupStack::PopAndDestroy(bitmap);

	// Test corrupt animations

	// Test too big animation 33X8
	bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	TESTL(LoadBitmap(*bitmap,KAnimsMBM,2)==KErrNone);
	TRAP(ret,CEmsAnimationIE::NewL(*bitmap));
	TESTL(ret==KErrCorrupt);
	CleanupStack::PopAndDestroy(bitmap);

	// Test too big animation 64X17
	bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	TESTL(LoadBitmap(*bitmap,KAnimsMBM,3)==KErrNone);
	TRAP(ret,CEmsAnimationIE::NewL(*bitmap));
	TESTL(ret==KErrTooBig);
	CleanupStack::PopAndDestroy(bitmap);


	RFbsSession::Disconnect();

	return TestStepResult();
	}


