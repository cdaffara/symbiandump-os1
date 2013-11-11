// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The TEFUnit test suite for PacketContext in the Common TSY.
// 
//

#include "cctsypacketcontextfu.h"
#include "cctsypacketservicesfu.h"

#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/mltsydispatchpacketservicesinterface.h>

CTestSuite* CCTsyPacketContextFU::CreateSuiteL(const TDesC& aName)
    {
    SUB_SUITE;

    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestUnit00027L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestUnit00028L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestUnit00029L);

    END_SUITE;
    }


//
// Actual test cases
//

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00027
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::Delete
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Delete
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestUnit00027L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);
    
    RBuf8 expectData;
    CleanupClosePushL(expectData);
        
    RPacketService packetService;
    OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);    
   
    // Open new context
    TInfoName contextId;    
    RPacketContext packetContext;   
    packetContext.OpenNewContext(packetService, contextId); 
    CleanupClosePushL(packetContext);
 
    TInfoName primaryContext;
    TInfoName secondaryContext;
    primaryContext.Copy(KFirstAllocatedContextName);
    secondaryContext.Copy(KDefaultContextName);    
    
    //-------------------------------------------------------------------------
    // SEND Delete request when ContextStatus is Unknown
    //-------------------------------------------------------------------------
    TRequestStatus requestStatus;   
    packetContext.Delete(requestStatus);
    
    User::WaitForRequest(requestStatus);    
    ASSERT_EQUALS(KErrNotReady, requestStatus.Int());   
    AssertMockLtsyStatusL();
       
    // Set context in Active state
    SetContextActiveL(contextId);
  
    //----------------------------------
    // Send an unsupported request
    //----------------------------------
    
    RPacketContext::TDataVolume volume; 
    packetContext.GetDataVolumeTransferred(requestStatus, volume);
    User::WaitForRequest(requestStatus);        
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    AssertMockLtsyStatusL();
    

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RPacketContext::Delete
    // from LTSY.
    //-------------------------------------------------------------------------    
    TMockLtsyData1 <TInfoName > ltsyData(contextId);
    data.Close();
    ltsyData.SerialiseL(data);
    
    TRequestStatus mockLtsyStatus;
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    // send completion 
    iMockLTSY.CompleteL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, KErrNone, data);

    // wait for completion
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------
    //TMockLtsyData1 <TInfoName> expData(contextId);
    TMockLtsyData1<TInfoName> expData(primaryContext);
    expectData.Close();
    expData.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, expectData, KErrNotSupported);

    packetContext.Delete(requestStatus);
    
    User::WaitForRequest(requestStatus);    
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());   
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------
    iMockLTSY.ExpectL (MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, expectData);
    iMockLTSY.CompleteL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, KErrGeneral, data);

    packetContext.Delete(requestStatus);
    
    User::WaitForRequest(requestStatus);    
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());    
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RPacketContext::Delete when result is not cached.
    //-------------------------------------------------------------------------
    iMockLTSY.ExpectL (MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, expectData);
    iMockLTSY.CompleteL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, KErrNone, data);

    packetContext.Delete(requestStatus);
        
    User::WaitForRequest(requestStatus);    
    ASSERT_EQUALS(KErrNone, requestStatus.Int());   
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST D: failure to modify context parameters when context deleted
    //-------------------------------------------------------------------------
    packetContext.ModifyActiveContext(requestStatus);
    User::WaitForRequest(requestStatus);        
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotReady, requestStatus.Int());
 
    iMockLTSY.ExpectL (MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, expectData);
    CleanupStack::PopAndDestroy(5, this); //packetContext, packetService, expectData, data, this 
    
    }

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00028
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::Delete
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::Delete
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestUnit00028L()
    {                   
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);    
    
    RBuf8 data2;
    CleanupClosePushL(data2);
    
    RBuf8 expectData;
    CleanupClosePushL(expectData);
    
    RBuf8 expectData2;
    CleanupClosePushL(expectData2);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(iTelServer,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);
    
    RPacketService packetService;
    OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);
    
    RPacketService packetService2;                
    ret = packetService2.Open(phone2);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(packetService2);
    
    // Open first context
    TInfoName contextId;    
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
    
    // Open second context
    TInfoName contextId2;   
    RPacketContext packetContext2;
    packetContext2.OpenNewContext(packetService2, contextId2);
    CleanupClosePushL(packetContext2);
    
    // Set first context in Active state
    SetContextActiveL(contextId);
    
    // Set second context in Active state
    SetContextActiveL(contextId2);

    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting RPacketContext::Delete
    //-------------------------------------------------------------------------
    
    // data for first CompleteL
    TMockLtsyData1 <TInfoName > ltsyData(contextId);
    ltsyData.SerialiseL(data);
    
    // data for second CompleteL
    TMockLtsyData1 <TInfoName > ltsyData2(contextId2);
    ltsyData2.SerialiseL(data2);    
   
    // for the first request
    TRequestStatus requestStatus;
    TMockLtsyData1 <TInfoName> expData(contextId);
    expData.SerialiseL(expectData);
    
    // for the second request
    TRequestStatus requestStatus2;
    TMockLtsyData1 <TInfoName> expData2(contextId2);
    expData2.SerialiseL(expectData2);
            
    iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, expectData);        
    iMockLTSY.CompleteL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, KErrNone, data);
    iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, expectData2);    
    iMockLTSY.CompleteL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, KErrNone, data2);
    
    // send first request
    packetContext.Delete(requestStatus);        
    
    // send second request
    packetContext2.Delete(requestStatus2);
    
    // wait for first answer
    User::WaitForRequest(requestStatus);        
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
        
    // wait for second answer
    User::WaitForRequest(requestStatus2);       
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

    AssertMockLtsyStatusL();
    iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, expectData2);
    iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, expectData);
    CleanupStack::PopAndDestroy(2); // expectData2, expectData
    CleanupStack::PopAndDestroy(9, this); //packetContext2, packetContext, packetService2, packetService, phone2, telServer2, data,data2 this  
    }

/**
@SYMTestCaseID BA-CTSYD-DIS-PACKETSERVICES-UN00029
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::Delete with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Delete and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestUnit00029L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);
    
    RBuf8 expectData;
    CleanupClosePushL(expectData);
    
    RPacketService packetService;
    OpenPacketServiceLC(packetService, RPacketService::EStatusUnattached);
    
    // Open new context
    TInfoName contextId;    
    RPacketContext packetContext;   
    packetContext.OpenNewContext(packetService, contextId); 
    CleanupClosePushL(packetContext);
    
    // Set context in Active state
    SetContextActiveL(contextId);
    
    //-------------------------------------------------------------------------
    // Test A: Test timeout of RPacketContext::Delete
    //-------------------------------------------------------------------------     
    TRequestStatus requestStatus;   
    TMockLtsyData1 <TInfoName> expData(contextId);
    expData.SerialiseL(expectData);    
    iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, expectData);
    
    packetContext.Delete(requestStatus);
        
    User::WaitForRequest(requestStatus);    
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());   
    AssertMockLtsyStatusL();
    
    iMockLTSY.ExpectL(MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, expectData);
    CleanupStack::PopAndDestroy(5, this); //packetContext, packetService, data, expectData, this
    }
