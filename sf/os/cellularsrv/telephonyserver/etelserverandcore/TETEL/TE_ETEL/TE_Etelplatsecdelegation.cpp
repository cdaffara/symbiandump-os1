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
#include "TE_Etelplatsecdelegation.h"
#include "Tetelcs.h"

CTestPlatSecDelegation::CTestPlatSecDelegation()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestPlatSecDelegation"));
	}

enum TVerdict CTestPlatSecDelegation::doTestStepL()
	{
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);

	//Load DSTD TSY
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	
	//Load DSTDNC TSY
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
    
	//INFO_PRINTF1(_L("Test opening a phone from DSTD TSY"));
    RTestPhone phone1;
    ret = phone1.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
    
	//INFO_PRINTF1(_L("Test opening a phone from DSTDNC TSY"));
    RTestPhone phone2;
    ret = phone2.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
    
    //Test calling a series of IPCs on DSTD TSY
    INFO_PRINTF1(_L("Test calling a series of IPCs on DSTD TSY"));
    
    //Test calling IPC1. Should return KErrNone
    ret = phone1.TestCustomIPC(ETestCustomIPC1);  
    TESTCHECK(ret, KErrNone);
    
    //Test calling IPC2. Should return KErrPermissionDenied
    ret = phone1.TestCustomIPC(ETestCustomIPC2);  
    TESTCHECK(ret, KErrPermissionDenied);
    
    //Test calling IPC3. Should return KErrPermissionDenied
    ret = phone1.TestCustomIPC(ETestCustomIPC3);  
    TESTCHECK(ret, KErrPermissionDenied);
    
    //Test calling IPC4. Should return KErrPermissionDenied
    ret = phone1.TestCustomIPC(ETestCustomIPC4);  
    TESTCHECK(ret, KErrPermissionDenied);
    
    //Test calling IPC5. Should return KErrPermissionDenied
    ret = phone1.TestCustomIPC(ETestCustomIPC5);  
    TESTCHECK(ret, KErrPermissionDenied);
        
    //Test calling IPC6. Should return KErrCancel
    ret = phone1.TestCustomIPC(ETestCustomIPC6);  
    TESTCHECK(ret, KErrCancel);
       
    //Test calling IPC7. Should return KErrPermissionDenied
    ret = phone1.TestCustomIPC(ETestCustomIPC7);  
    TESTCHECK(ret, KErrPermissionDenied);
       
    //Test calling IPC8. Should return KErrNotSupported
    ret = phone1.TestCustomIPC(ETestCustomIPC8);  
    TESTCHECK(ret, KErrNotSupported);
        
    //Test calling IPC9. Should return KErrNotSupported
    ret = phone1.TestCustomIPC(ETestCustomIPC9);  
    TESTCHECK(ret, KErrNotSupported);
    
    //Test calling an IPC on DSTDNC TSY while DSTD TSY is still loaded
    INFO_PRINTF1(_L("Test an IPC on DSTDNC TSY"));
    
    //Test calling IPC2. Should return KErrNone
    ret = phone2.TestCustomIPC(ETestCustomIPC2);  
    TESTCHECK(ret, KErrNone);
    
    phone1.Close();
	phone2.Close();
        
    ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);    
    ret=server.UnloadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);    


	return TestStepResult();
	}

