// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TE_EtelBase.h"
#include "TE_Etelmemdataport.h"

CTestMemDataPort::CTestMemDataPort()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestMemDataPort"));
	}

enum TVerdict CTestMemDataPort::doTestStepL()
	{
	INFO_PRINTF1(_L("TestDataPortCase3"));
    RTelServer server;
    
    TInt  ret = server.Connect();
    TESTCHECK(ret, KErrNone);
    
    ret = server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
	
	ret = call.OpenNewCall(server,DSTDNC_CALL_FULLNAME);
    TESTCHECK(ret, KErrNone);
	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
    
    ret = call.Dial(dialString);
    TESTCHECK(ret, KErrNone);
	RCall::TCommPort commInfo;
	
	ret = call.LoanDataPort(commInfo);
    TESTCHECK(ret, KErrNone);
	
	call.Close();
	ServerClose(server);

	return TestStepResult();
	}
