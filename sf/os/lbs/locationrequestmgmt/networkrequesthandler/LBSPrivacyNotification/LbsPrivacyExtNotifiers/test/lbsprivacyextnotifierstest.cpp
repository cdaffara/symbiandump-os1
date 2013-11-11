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
// This test exercises the data types used to pass data to/from
// the Lbs privacy notification dialogs via the RNotifier API.
// 
//

#include <e32debug.h>
#include <e32test.h>
#include <lbs/lbsprivacyextnotifiers.h>

_LIT(KTestName, "Exercising the data types in LbsPrivacyExtNotifiers.h");
LOCAL_D RTest test(KTestName);


LOCAL_C void TestTLbsPrivacyNotifierParams()
	{
	// First test setting the parameter data.
	TLbsPrivacyNotifierParams params;
	TLbsExternalRequestInfo info;
	
	_LIT8(KRequesterId, "ExtRequest_1");
	_LIT8(KClientName, "ExtClient_1");
	_LIT8(KClientExternalId, "+4401484341822");
	
	info.SetRequesterId(KRequesterId());
	info.SetClientName(KClientName());
	info.SetClientExternalId(KClientExternalId());
	
	params.SetRequesterInfo(info);
	
	// Second test that getting the parameter data works.
	TLbsExternalRequestInfo info2;
	
	params.GetRequesterInfo(info2);

	TLbsRequesterId requesterId;
	TLbsClientName clientName;
	TLbsClientExternalId clientExternalId;
	
	info2.GetRequesterId(requesterId);
	info2.GetClientName(clientName);
	info2.GetClientExternalId(clientExternalId);
	
	test(requesterId == KRequesterId());
	test(clientName == KClientName());
	test(clientExternalId == KClientExternalId());
	}
	
LOCAL_C void TestTLbsPrivacyResponseParams()
	{
	// First test setting the parameter data.
	TLbsPrivacyResponseParams params;
	TLbsPrivacyNotifierResponse response(EResponseAccepted);
	
	params.SetResponseType(response);
	
	// Second test that getting the parameter data works.
	TLbsPrivacyNotifierResponse response2;
	
	params.GetResponseType(response2);
	
	test(response == response2);
	}
	
void doMainL()
	{
	test.Next(_L("Exercising TLbsPrivacyNotifierParams"));
	TestTLbsPrivacyNotifierParams();
	
	test.Next(_L("Exercising TLbsPrivacyNotifierParams"));
	TestTLbsPrivacyResponseParams();
	}

/*
Program entry point.
*/
GLDEF_C int E32Main()
	{
	__UHEAP_MARK;
	
	test.Title();
	test.Start(KTestName);
		
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		return KErrNoMemory;
		}
		
	CActiveScheduler * scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAPD(err, doMainL());
	test.Printf(_L("Test finished with code %d\n"), err);
	test(err == KErrNone);

	delete scheduler;
	delete cleanup;
	
	test.End();
	test.Close();
	
	__UHEAP_MARKEND;
	return 0;	
	}
