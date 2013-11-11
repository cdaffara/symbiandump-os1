// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "te_etelbase.h"
#include "te_etelpanicafterrequest.h"
#include "et_struc.h"
#include "et_struct.h"
#include "et_clsvr.h"
#include "etelext.h"

const TUint KDefaultHeapSizeThread = 0x4000; //< default heap size for the test Threads

class RTestPanicAfterRequestPhone: public RPhone
	{
public:
	RTestPanicAfterRequestPhone():RPhone()
		{
		
		}
	void GetSomeData(TRequestStatus& aStatus)
		{
		// Send an asynch request for GetLineInfo
		TIpcArgs args;
		args.Set(1,EIsaDesTobeRead);
		TLineInfoIndex info;
		info.iIndex=0;
		TPtr8 ptr(REINTERPRET_CAST(TText8*,&info),sizeof(TLineInfoIndex),sizeof(TLineInfoIndex));
		args.Set(0,&ptr);
		SendReceive(EEtelPhoneGetLineInfo,args,aStatus);
		
		}
	
	};

CTestPanicAfterRequest::CTestPanicAfterRequest()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestPanicAfterRequest"));
	}


TVerdict CTestPanicAfterRequest::doTestStepL()
	{
	INFO_PRINTF1(_L("Client sends a request to Etel server, and then exits"));

	RThread panicThread;
	TRequestStatus stat;
	TInt res=panicThread.Create(_L("RequestAndPanicThread"),RequestAndPanicThread,KDefaultStackSize,KDefaultHeapSizeThread,KDefaultHeapSizeThread,this);

	panicThread.Logon(stat);
	
	panicThread.Resume();

	User::WaitForRequest(stat);
	User::After(10000); // Give ETEL time to panic....
	return TestStepResult();
	}

TInt CTestPanicAfterRequest::RequestAndPanicThread(TAny* /*aArg*/)
	{
	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return KErrGeneral;
	//CTestPanicAfterRequest* stepTest = reinterpret_cast<CTestPanicAfterRequest*> (aArg);
    RTelServer server;
	TInt ret = server.Connect();
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	RTestPanicAfterRequestPhone phone;
	ret = phone.Open(server,DSTD_SLOW_PHONE_NAME);
	TRequestStatus status;
	phone.GetSomeData(status);

	server.Close();
	delete cleanup;
	
	return KErrNone;
	}
