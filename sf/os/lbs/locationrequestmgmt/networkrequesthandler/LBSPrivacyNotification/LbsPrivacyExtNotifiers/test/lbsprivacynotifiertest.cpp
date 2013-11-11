// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class implementation of CLbsPrivacyNotifierTest
// 
//

#include <e32debug.h>
#include <e32test.h>
#include <lbs/lbsloccommon.h>
#include "LbsPrivacyNotifier.h"
#include "lbsprivacynotifiertest.h"

// Test
_LIT(KTestName, "LBS Privacy Notifier Test");
LOCAL_D RTest test(KTestName);

CLbsPrivacyNotifierTest* CLbsPrivacyNotifierTest::NewL()
	{
	CLbsPrivacyNotifierTest* self = new (ELeave) CLbsPrivacyNotifierTest;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CLbsPrivacyNotifierTest::~CLbsPrivacyNotifierTest()
	{
	delete iNotifyNotifier;
	delete iVerifyNotifier;
	}
	
void CLbsPrivacyNotifierTest::DoDialogTestL()
	{
	TInt error;
	TLbsExternalRequestInfo info;
	iNotifyNotifier->DisplayL(error, info);
	CActiveScheduler::Start();
	
	iVerifyNotifier->DisplayL(error, info);
	CActiveScheduler::Start();
	}
	
void CLbsPrivacyNotifierTest::OnNotificationDialogResponse(TInt aErr, const TLbsPrivacyNotifierResponse& aResponse)
	{
	RDebug::Printf("Received dialog response; aErr = %d, aResponse = %d", aErr, aResponse);
	}

CLbsPrivacyNotifierTest::CLbsPrivacyNotifierTest()
	{
	
	}
	
void CLbsPrivacyNotifierTest::ConstructL()
	{
	iNotifyNotifier = CLbsPrivacyNotifier::NewL(EPrivacyDialogNotification, 20000000);
	iNotifyNotifier->SetObserver(this);	
	iVerifyNotifier = CLbsPrivacyNotifier::NewL(EPrivacyDialogVerification, 20000000);
	iVerifyNotifier->SetObserver(this);	
	}
	

void doMainL()
	{
	CLbsPrivacyNotifierTest* test = CLbsPrivacyNotifierTest::NewL();
	CleanupStack::PushL(test);
	test->DoDialogTestL();
	CleanupStack::PopAndDestroy(test);
	}

GLDEF_C int E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(KTestName);
		
	CActiveScheduler* rootScheduler = new CActiveScheduler;
	CActiveScheduler::Install(rootScheduler);
	CTrapCleanup* theCleanup=CTrapCleanup::New();

	TRAPD(ret,doMainL());	
	test.Printf(_L("\nTest Completion code: %d\n"), ret);	
	test(ret==KErrNone);
	
	delete theCleanup;	
	delete rootScheduler;
	
	test.End();
	test.Close();
		
	__UHEAP_MARKEND;
	return 0;
	}
