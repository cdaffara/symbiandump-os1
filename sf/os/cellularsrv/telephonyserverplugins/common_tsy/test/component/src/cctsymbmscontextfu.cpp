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

/**
 @file The TEFUnit test suite for MBMS Context in the Common TSY.
*/

#include "cctsymbmscontextfu.h"
#include "cctsyactiveretriever.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

#include <pcktcs.h>
#include <pcktlist.h>
#include "pcktretrieve.h"
#include "CMmCommonStaticUtility.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>



CTestSuite* CCTsyMbmsContextFU::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;

    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestOpenNewContext0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestInitialiseContext0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestInitialiseContext0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestInitialiseContext0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestInitialiseContext0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestInitialiseContext0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestSetConfig0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestSetConfig0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestSetConfig0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestSetConfig0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestSetConfig0005L);             
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestActivate0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestActivate0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestActivate0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestActivate0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestActivate0005L);                
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestDeactivate0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestDeactivate0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestDeactivate0004L);  
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestDeactivate0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestDelete0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestDelete0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestDelete0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestClose0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestGetConfig0001L);    
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestGetConfig0002L); 
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestGetConfig0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestGetStatus0001L);    
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestGetLastErrorCause0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestNotifyConfigChanged0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestNotifyConfigChanged0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestNotifyConfigChanged0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestNotifyConfigChanged0004L);    
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestNotifyStatusChange0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestNotifyStatusChange0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyMbmsContextFU, TestNotifyStatusChange0003L);

	END_SUITE;
	}


//
// Actual test cases
//
   
/**
@SYMTestCaseID BA-CTSY-MCF-PONC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::OpenNewContext
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::OpenNewContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/

void CCTsyMbmsContextFU::TestOpenNewContext0001L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	
	
	RPacketService packetService;
    	
	OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );
    
    //-------------------------------------------------------------------------
    // TEST create
    //-------------------------------------------------------------------------    
       
	TInfoName contextId;
    RPacketMbmsContext packetMbmsContext;
    TInt retCode;
	 
    // memory leak check
	__UHEAP_MARK;

    retCode = packetMbmsContext.OpenNewContext(packetService, contextId);
    ASSERT_EQUALS(KErrNone, retCode);
    
    _LIT(KContextName, "MBMS_Context_0");
    
    ASSERT_FALSE(contextId.Compare(KContextName));
    packetMbmsContext.Close();
	
    __UHEAP_MARKEND;
    // end memory leak check
  
    // test maximum number of contexts allowed
    TInt count(0);
    TInt maxAllowed(0);
    	
    TRequestStatus requestStatus;
    packetService.EnumerateMbmsActiveServiceList(requestStatus, count, maxAllowed); 
    User::WaitForRequest( requestStatus );
    
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    TInt err = 0;  
    TInt status = 0;
    for (TInt i = 0; i < maxAllowed; i++)
        {
   		RPacketMbmsContext newPacketMbmsContext;
   		TInfoName contextId;
   		
   		err = newPacketMbmsContext.OpenNewContext(packetService, contextId);
   		ASSERT_EQUALS(KErrNone, err);
   		CleanupClosePushL(newPacketMbmsContext); 			

   		// check the context can be initialised 
   		status = InitMbmsContextL(newPacketMbmsContext, contextId);            
   	    ASSERT_EQUALS(KErrNone, status);	
   		
   	    CleanupStack::Pop();
    	}
	
    RPacketMbmsContext OneMorePacketContext;
 	TInfoName contextId1;
 	
 	err = OneMorePacketContext.OpenNewContext(packetService, contextId1);
    ASSERT_EQUALS(KErrOverflow, err); 
	
	OneMorePacketContext.Close();
	packetService.Close();
	CleanupStack::PopAndDestroy(2, this); //  packetService, this 
    }
   
/**
@SYMTestCaseID BA-CTSY-MCF-PIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::InitialiseContext
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::InitialiseContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestInitialiseContext0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this)); 
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);	

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
 	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	TMockLtsyData1<TInfoName> ltsyExpect(contextId);
	ltsyExpect.SerialiseL(expectData);
 	
 	// data for initialisation of the context 
	TRequestStatus requestStatus;
	iMockLTSY.ExpectL(ECtsyPacketMbmsInitialiseContextReq, expectData, KErrNotSupported);

	//initialisation of the context
	RPacketContext::TDataChannelV2 dataCh;
	TPckg<RPacketContext::TDataChannelV2> pckgInit(dataCh);
    packetContext.InitialiseContext(requestStatus, pckgInit);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------	
	
 	RPacketContext::TDataChannelV2 retDataCh;
 	
 	_LIT(KDataChannelName, "DataChannel_1");
 	retDataCh.iChannelId.Copy(KDataChannelName);
 	
	iMockLTSY.ExpectL(ECtsyPacketMbmsInitialiseContextReq, expectData);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	TMockLtsyData2<TInfoName, RPacketContext::TDataChannelV2> ltsyData(contextId,retDataCh);
	data.Close();
	ltsyData.SerialiseL(data);	
	
	iMockLTSY.CompleteL(ECtsyPacketMbmsInitialiseContextComp, KErrGeneral, data);
	
	//initialisation of the context
	packetContext.InitialiseContext(requestStatus, pckgInit);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::InitialiseContext when result is not cached.
 	//-------------------------------------------------------------------------
	
	iMockLTSY.ExpectL(ECtsyPacketMbmsInitialiseContextReq, expectData);
	iMockLTSY.CompleteL(ECtsyPacketMbmsInitialiseContextComp, KErrNone, data);
	
	//initialisation of the context
	packetContext.InitialiseContext(requestStatus, pckgInit);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	RPacketContext::TDataChannelV2 dataChannel = pckgInit();
    
	// check this
	ASSERT_TRUE(contextId.Compare(KDataChannelName));
	
  	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::InitialiseContext
	// from LTSY.
 	//-------------------------------------------------------------------------
	
	TRequestStatus mockLtsyStatus;	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(ECtsyPacketMbmsInitialiseContextComp, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(5, this); // expectData, data, this, packetService, packetContext	
	}  
	
/**
@SYMTestCaseID BA-CTSY-MCF-PIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketMbmsContext::InitialiseContext
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketMbmsContext::InitialiseContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestInitialiseContext0002L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();
	
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );        

	TInfoName contextId;
    RPacketMbmsContext packetMbmsContext;
    packetMbmsContext.OpenNewContext( packetService, contextId );
    CleanupClosePushL( packetMbmsContext );
    
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);    
        	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::InitialiseContext when result is not cached.
 	//-------------------------------------------------------------------------
 		
	// data for initialisation of the context 
	TRequestStatus requestStatus;
	RPacketContext::TDataChannelV2 dataCh;
	TPckg<RPacketContext::TDataChannelV2> pckgInit(dataCh);
	
	RBuf8 data;
	CleanupClosePushL( data );
	
	TMockLtsyData1< TInfoName> ltsyInit(contextId);
	ltsyInit.SerialiseL(data);	
	iMockLTSY.ExpectL(ECtsyPacketMbmsInitialiseContextReq, data);

	RBuf8 expectData;
	CleanupClosePushL( expectData );	
	
	RPacketContext::TDataChannelV2 retDataCh;
	TMockLtsyData2< TInfoName, RPacketContext::TDataChannelV2 > ltsyData(contextId, retDataCh);
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(ECtsyPacketMbmsInitialiseContextComp, KErrNone, data, 10);

	// initialisation of the context
	packetMbmsContext.InitialiseContext(requestStatus, pckgInit);
	
    // cancel request
	packetMbmsContext.CancelAsyncRequest(EPacketContextInitialiseContext);
	
	User::WaitForRequest(requestStatus);	
	// CTSY has started a request and it is not possible to then cancel 
    // this request. The best thing for the TSY to do in this case is to
    // proceed as though the Cancel never happened.
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	 	

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(3); // expectData, data, this
    }    
   

/**
@SYMTestCaseID BA-CTSY-MCF-PIC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::InitialiseContext with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::InitialiseContext with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestInitialiseContext0003L()
	{
	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );
	
	RBuf8 expectData;
	CleanupClosePushL( expectData );
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RPacketMbmsContext::InitialiseContext
 	//-------------------------------------------------------------------------
	// data for initialisation of the context 
		
	TRequestStatus requestStatus;
	RCall::TCommPort dataCh; // wrong type of data package
	TPckg<RCall::TCommPort> pckgInit( dataCh );
	
	//initialisation of the context
	packetMbmsContext.InitialiseContext( requestStatus, pckgInit );	
	
	User::WaitForRequest( requestStatus );	
	ASSERT_EQUALS( KErrArgument, requestStatus.Int() );
	AssertMockLtsyStatusL();	 	

	CleanupStack::PopAndDestroy( 2 ); // packetMbmsContext, packetService
	CleanupStack::PopAndDestroy( 3, this ); // expectData, data, this
	}   
   
/**
@SYMTestCaseID BA-CTSY-MCF-PIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketMbmsContext::InitialiseContext
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketMbmsContext::InitialiseContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestInitialiseContext0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketMbmsContext packetContext2;
	packetContext2.OpenNewContext(packetService, contextId2);
	CleanupClosePushL(packetContext2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketMbmsContext::InitialiseContext
 	//-------------------------------------------------------------------------
	
	// for the first request
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
    TMockLtsyData1<TInfoName> ltsyExpect(contextId);
	ltsyExpect.SerialiseL(expectData);
	iMockLTSY.ExpectL(ECtsyPacketMbmsInitialiseContextReq, expectData);	
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketContext::TDataChannelV2 dataCh;
	TMockLtsyData2<TInfoName, RPacketContext::TDataChannelV2> ltsyData(contextId, dataCh);
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(ECtsyPacketMbmsInitialiseContextComp, KErrNone, data);	
	
	// for the first request
	RBuf8 expectData2;
	CleanupClosePushL(expectData2);
	
	TMockLtsyData1<TInfoName> ltsyExpect2(contextId2);
	ltsyExpect2.SerialiseL(expectData2);
	iMockLTSY.ExpectL(ECtsyPacketMbmsInitialiseContextReq, expectData2);	
	
	RBuf8 data2;
	CleanupClosePushL(data2);
	
	RPacketContext::TDataChannelV2 dataCh2;
	TMockLtsyData2<TInfoName, RPacketContext::TDataChannelV2> ltsyData2(contextId2, dataCh2);
	ltsyData2.SerialiseL(data2);
	iMockLTSY.CompleteL(ECtsyPacketMbmsInitialiseContextComp, KErrNone, data2);	
	
	//first request
	TRequestStatus requestStatus;
	RPacketContext::TDataChannelV2 dataChannel1;
	TPckg<RPacketContext::TDataChannelV2> pckgInit1(dataChannel1);	
    packetContext.InitialiseContext(requestStatus, pckgInit1);
	
	//second request
	TRequestStatus requestStatus2;
	RPacketContext::TDataChannelV2 dataChannel2;
	TPckg<RPacketContext::TDataChannelV2> pckgInit2(dataChannel2);	
    packetContext2.InitialiseContext(requestStatus2, pckgInit2);	
	
    User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	User::WaitForRequest(requestStatus2);		
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	
	
	AssertMockLtsyStatusL();	 	
	CleanupStack::PopAndDestroy(8, this); //packetContext2, packetContext, packetService, expectData, expectData2, data, data2, this

	}   
   
/**
@SYMTestCaseID BA-CTSY-MCF-PIC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::InitialiseContext with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::InitialiseContext and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestInitialiseContext0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetMbmsContext);
		
	RBuf8 expectData;
	CleanupClosePushL(expectData);
		
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketContext::InitialiseContext
 	//-------------------------------------------------------------------------
    TMockLtsyData1<TInfoName> ltsyExpect(contextId);
	ltsyExpect.SerialiseL(expectData);

	iMockLTSY.ExpectL(ECtsyPacketMbmsInitialiseContextReq, expectData);
		
	TRequestStatus requestStatus;
	RPacketContext::TDataChannelV2 dataCh;
	TPckg<RPacketContext::TDataChannelV2> pckgInit(dataCh);	
	packetMbmsContext.InitialiseContext(requestStatus, pckgInit);
	
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //packetService, packetContext, expectData, this
	}   
 
/**
@SYMTestCaseID BA-CTSY-MCF-PSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::SetConfig
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::SetConfig
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestSetConfig0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);
    
    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    TInfoName contextId;  
    RPacketMbmsContext mbmsContext;
    mbmsContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(mbmsContext);
    
    // initialisation of the context 
 	TInt status = InitMbmsContextL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);  
 
	// data for SetContext
	RPacketMbmsContext::TContextConfigMbmsV1 setConfig;
	TTmgi ttmgi;
	ttmgi.SetServiceId(KMmConfigTMGI1);
	ttmgi.SetMCC(KMmConfigTmgiMMC1);
	ttmgi.SetMNC(KMmConfigTmgiMNC1);
    setConfig.iTmgi = ttmgi;
    setConfig.iMbmsAccessBearer = E2GBearer;
    setConfig.iMbmsServicePriority = EMbmsMediumServicePriority;
    setConfig.iMbmsServiceMode = KBroadcast;
    setConfig.iMbmsSessionFlag = EFalse;

    TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig(setConfig);    
    

    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------
	TMockLtsyData2<RPacketMbmsContext::TContextConfigMbmsV1, TInfoName > expExpect(setConfig, contextId);
    expExpect.SerialiseL(expectData);
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData, KErrNotSupported);

    TRequestStatus requestStatus;    
    mbmsContext.SetConfig(requestStatus, pckgConfig);
    User::WaitForRequest(requestStatus);
    
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);

    TMockLtsyData1<TInfoName > contexName(contextId);
    contexName.SerialiseL(data);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrGeneral, data);
    
    mbmsContext.SetConfig(requestStatus, pckgConfig);
    User::WaitForRequest(requestStatus);
    
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
    AssertMockLtsyStatusL();
    
    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RPacketContext::SetConfig when result is not cached.
    //-------------------------------------------------------------------------
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

    mbmsContext.SetConfig(requestStatus, pckgConfig);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());  
    
    RPacketMbmsContext::TContextConfigMbmsV1 retData = pckgConfig();
    ASSERT_EQUALS(setConfig.iMbmsAccessBearer, retData.iMbmsAccessBearer);  
    ASSERT_EQUALS(setConfig.iMbmsServicePriority, retData.iMbmsServicePriority);  
    ASSERT_EQUALS(setConfig.iMbmsServiceMode, retData.iMbmsServiceMode);  
    ASSERT_EQUALS(setConfig.iMbmsSessionFlag, retData.iMbmsSessionFlag);  
    
    AssertMockLtsyStatusL();
    
    //--------------------------------------------
    // failure to dispatch request for config 
    //--------------------------------------------    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData, KErrNotSupported);

    mbmsContext.SetConfig(requestStatus, pckgConfig);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
   
    AssertMockLtsyStatusL();       
    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RPacketContext::SetConfig
    // from LTSY.
    //-------------------------------------------------------------------------
    TRequestStatus mockLtsyStatus;
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);        
        
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());  
    AssertMockLtsyStatusL();
        
    CleanupStack::PopAndDestroy(2); //packetContext, packetService
    CleanupStack::PopAndDestroy(3, this); // data, expectData, this    
    }  
    
/**
@SYMTestCaseID BA-CTSY-MCF-PSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketMbmsContext::SetConfig
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketMbmsContext::SetConfig
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestSetConfig0002L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);
    
    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    TInfoName contextId;  
    RPacketMbmsContext mbmsContext;
    mbmsContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(mbmsContext);
    
    // initialisation of the context 
 	TInt status = InitMbmsContextL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);  
    
    TRequestStatus requestStatus;      
    RPacketMbmsContext::TContextConfigMbmsV1 setConfig;

    //-------------------------------------------------------------------------
    // TEST C: Cancel completion request of
    // RPacketContext::SetConfig when result is not cached.
    //-------------------------------------------------------------------------
    TMockLtsyData2<RPacketMbmsContext::TContextConfigMbmsV1, TInfoName > expExpect(setConfig, contextId);
    expExpect.SerialiseL(expectData);
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    
    // TODO : KHURAM FIX
    //data for CompleteL    
    TMockLtsyData1<TInfoName > contexName(contextId);
    contexName.SerialiseL(data); 
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data, 10);

    
	// data for SetContext
    TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig(setConfig);  
    mbmsContext.SetConfig(requestStatus, pckgConfig);
    mbmsContext.CancelAsyncRequest(EPacketContextSetConfig);
    User::WaitForRequest(requestStatus);
    
    ASSERT_EQUALS(KErrNone, requestStatus.Int());  
    AssertMockLtsyStatusL();    
    
    CleanupStack::PopAndDestroy(2); //packetContext, packetService
    CleanupStack::PopAndDestroy(3, this); // data, expectData, this    
    } 
        

/**
@SYMTestCaseID BA-CTSY-MCF-PSC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::SetConfig with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::SetConfig with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestSetConfig0003L()
    {
    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);
    
    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    TInfoName contextId;  
    TInfoName hostCid;
    RPacketMbmsContext mbmsContext;
    mbmsContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(mbmsContext);
    
    TRequestStatus requestStatus;
    
    //------------------------------------------------
    //set config for context that was not initialised
    //------------------------------------------------
    
    RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
    TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig(contextConfig);    
    
    mbmsContext.SetConfig(requestStatus, pckgConfig);

    User::WaitForRequest(requestStatus);
    
    ASSERT_EQUALS(KErrNotReady, requestStatus.Int());
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(2); //packetContext packetService
    CleanupStack::PopAndDestroy(2, this); // data, this

    }
     
/**
@SYMTestCaseID BA-CTSY-MCF-PSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketMbmsContext::SetConfig
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketMbmsContext::SetConfig
@SYMTestExpectedResults Pass
@SYMTestType CT
*/     
void CCTsyMbmsContextFU::TestSetConfig0004L()
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
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);


    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);

    RPacketService packetService2;                
    ret = packetService2.Open(phone2);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(packetService2);
    
    // Open first context
    TInfoName contextId;  
    TInfoName hostCid;
    RPacketMbmsContext mbmsContext;
    mbmsContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(mbmsContext);
    
    // Open second context
    TInfoName contextId2; 
    RPacketMbmsContext mbmsContext2;
    mbmsContext2.OpenNewContext(packetService2, contextId2);
    CleanupClosePushL(mbmsContext2);


    // initialisation of the first context 
 	TInt status = InitMbmsContextL(mbmsContext, contextId);
 	ASSERT_EQUALS(KErrNone, status);
  
 	// initialisation of the second context 
	status = InitMbmsContextL(mbmsContext2, contextId2);
 	ASSERT_EQUALS(KErrNone, status);
    
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);   
    
    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting RPacketContext::SetConfig
    //-------------------------------------------------------------------------    
    RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	TTmgi ttmgi;
	ttmgi.SetServiceId(KMmConfigTMGI1);
	ttmgi.SetMCC(KMmConfigTmgiMMC1);
	ttmgi.SetMCC(KMmConfigTmgiMNC1);
    contextConfig.iTmgi = ttmgi;
    contextConfig.iMbmsAccessBearer = E2GBearer;
    contextConfig.iMbmsServicePriority = EMbmsMediumServicePriority;
    contextConfig.iMbmsServiceMode = KBroadcast;
    contextConfig.iMbmsSessionFlag = EFalse;
    TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig(contextConfig);
    // for the first ExpectL    
    TMockLtsyData2<RPacketMbmsContext::TContextConfigMbmsV1, TInfoName > expExpect(contextConfig, contextId);
    expExpect.SerialiseL(expectData);
    
    RPacketMbmsContext::TContextConfigMbmsV1 contextConfig2;
    TTmgi ttmgi2;
	ttmgi2.SetServiceId(KMmConfigTMGI2);
	ttmgi2.SetMCC(KMmConfigTmgiMMC2);
	ttmgi2.SetMCC(KMmConfigTmgiMNC2);
    contextConfig2.iTmgi = ttmgi;
    contextConfig2.iMbmsAccessBearer = E3GBearer;
    contextConfig2.iMbmsServicePriority = EMbmsHighServicePriority;
    contextConfig2.iMbmsServiceMode = KSelectedBroadcast;
    contextConfig2.iMbmsSessionFlag = ETrue;
    TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig2(contextConfig2);
    // for the second ExpectL
    TMockLtsyData2<RPacketMbmsContext::TContextConfigMbmsV1, TInfoName > expExpect2(contextConfig2, contextId2);
    expExpect2.SerialiseL(expectData2);   
    
    // for the first CompleteL
    TMockLtsyData1< TInfoName > ltsyData(contextId);
    ltsyData.SerialiseL(data);
    
    // for the second CompleteL
    TMockLtsyData1< TInfoName > ltsyData2(contextId2);
    ltsyData2.SerialiseL(data2);    
    
    TRequestStatus  requestStatus;    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data, 10);

    TRequestStatus  requestStatus2;    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData2);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data2, 10);    
         
    mbmsContext.SetConfig(requestStatus, pckgConfig);
    mbmsContext2.SetConfig(requestStatus2, pckgConfig2);
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

    RPacketMbmsContext::TContextConfigMbmsV1 ret1 = pckgConfig();
    RPacketMbmsContext::TContextConfigMbmsV1 ret2 = pckgConfig2();
    
    ASSERT_EQUALS(ret1.iMbmsAccessBearer,    contextConfig.iMbmsAccessBearer);  
    ASSERT_EQUALS(ret1.iMbmsServicePriority, contextConfig.iMbmsServicePriority);  
    ASSERT_EQUALS(ret1.iMbmsServiceMode,     contextConfig.iMbmsServiceMode);  
    ASSERT_EQUALS(ret1.iMbmsSessionFlag,     contextConfig.iMbmsSessionFlag);  
    
    ASSERT_EQUALS(ret2.iMbmsAccessBearer,    contextConfig2.iMbmsAccessBearer);  
    ASSERT_EQUALS(ret2.iMbmsServicePriority, contextConfig2.iMbmsServicePriority);  
    ASSERT_EQUALS(ret2.iMbmsServiceMode,     contextConfig2.iMbmsServiceMode);  
    ASSERT_EQUALS(ret2.iMbmsSessionFlag,     contextConfig2.iMbmsSessionFlag);  
    
    User::WaitForRequest(mockLtsyStatus);

    // packetContext, packetService, packetContext2, packetService2
    //one2, telServer2, data2,
    CleanupStack::PopAndDestroy(2); //expectData2, expectData
    CleanupStack::PopAndDestroy(9, this); // data, this         
    }
    
/**
@SYMTestCaseID BA-CTSY-MCF-PSC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::SetConfig with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::SetConfig and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/    
void CCTsyMbmsContextFU::TestSetConfig0005L()
    {
    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);
    
    RBuf8 expectData;
    CleanupClosePushL(expectData);

    //-------------------------------------------------------------------------
    // Test A: Test timeout of RPacketContext::SetConfig
    //-------------------------------------------------------------------------

    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    TInfoName contextId;  
    RPacketMbmsContext mbmsContext;
    mbmsContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(mbmsContext);
    
    // initialisation of the first context 
    TInt status = InitMbmsContextL(mbmsContext, contextId);
    ASSERT_EQUALS(KErrNone, status);

    RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
    TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig(contextConfig);    
    TMockLtsyData2<RPacketMbmsContext::TContextConfigMbmsV1, TInfoName > expExpect(contextConfig, contextId);
    expExpect.SerialiseL(expectData);

    TRequestStatus requestStatus;
    
    TMockLtsyData1<TInfoName > contexName(contextId);
    contexName.SerialiseL(data);
    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    
    mbmsContext.SetConfig(requestStatus, pckgConfig);

    User::WaitForRequest(requestStatus);
    
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
    AssertMockLtsyStatusL();
    
    
    CleanupStack::PopAndDestroy(2); //packetContext packetService, 
    CleanupStack::PopAndDestroy(3, this); // data, expectData, this    
    }      
        
/**
@SYMTestCaseID BA-CTSY-MCF-PA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::Activate
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::Activate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestActivate0001L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL( data );
	
	RBuf8 data2;
	CleanupClosePushL( data2 );
	
	RBuf8 expectData;
	CleanupClosePushL( expectData );	
	
	RBuf8 expectData2;
	CleanupClosePushL( expectData2 );	
	
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );

    //-------------------------------------------------------------------------
    // TEST create
    //-------------------------------------------------------------------------    
       
	TInfoName contextId;
    RPacketMbmsContext packetMbmsContext;
    packetMbmsContext.OpenNewContext( packetService, contextId );
    CleanupClosePushL( packetMbmsContext );
        
    TRequestStatus requestStatus;
	
    //-------------------------------------------------------------------------
    // TEST initialise
    //-------------------------------------------------------------------------
	
	// initialisation of the context 
	TInt status = InitMbmsContextL( packetMbmsContext, contextId );
	ASSERT_EQUALS( KErrNone, status );  
	
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(packetMbmsContext, contextId, contextConfig);

 	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);	  
	
	
	TMockLtsyData2< TInfoName, RPacketMbmsContext::TContextConfigMbmsV1 > ltsyData1(contextId, contextConfig);
	ltsyData1.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------	
 
	iMockLTSY.ExpectL(EPacketContextActivate, expectData, KErrNotSupported);
	
	packetMbmsContext.Activate(requestStatus);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	
 
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
 	TMockLtsyData1 <TInfoName> expData2(contextId);
	expData2.SerialiseL(expectData2);	  
	
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig2;
	TMockLtsyData2< TInfoName, RPacketMbmsContext::TContextConfigMbmsV1 > ltsyData2(contextId, contextConfig2);
	ltsyData2.SerialiseL(data2); 	
 	
	iMockLTSY.ExpectL(EPacketContextActivate, expectData2);
	iMockLTSY.CompleteL(EPacketContextActivate, KErrGeneral, data2);
	
	packetMbmsContext.Activate(requestStatus);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	
 
    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of activate context
    //-------------------------------------------------------------------------	 		    
	iMockLTSY.ExpectL(EPacketContextActivate, expectData);
	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data);
		
	packetMbmsContext.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();    
	ASSERT_EQUALS(KErrNone, requestStatus.Int());    
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::Activate
	// from LTSY.
 	//-------------------------------------------------------------------------
 
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy( 4 ); // data, data2, expectData
	CleanupStack::PopAndDestroy( 3, this ); // packetMbmsContext, packetService, this
    }	
    
/**
@SYMTestCaseID BA-CTSY-MCF-PA-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketMbmsContext::Activate
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketMbmsContext::Activate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestActivate0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);	
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext mbmsContext;
	mbmsContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(mbmsContext);
	
	// initialisation of the context 
	TInt status = InitMbmsContextL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// Configure context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(mbmsContext, contextId, contextConfig);
		
 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketContext::Activate
 	//------------------------------------------------------------------------- 
 	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
 
   	TRequestStatus	requestStatus;	
	TMockLtsyData2< TInfoName, RPacketMbmsContext::TContextConfigMbmsV1 > ltsyData1(contextId, contextConfig);
	ltsyData1.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextActivate, expectData);
	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data, 10);
	
	mbmsContext.Activate(requestStatus);
	mbmsContext.CancelAsyncRequest(EPacketContextActivate);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	// TSY has started a request and it is not possible to then cancel 
    // this request. The best thing for the TSY to do in this case is to
    // proceed as though the Cancel never happened.
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(3); //  this, expectData, data	
	}   
	
/**
@SYMTestCaseID BA-CTSY-MCF-PA-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::Activate with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::Activate with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestActivate0003L()
    {
	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );
	
	// -----------------------------------------------------------------------
	// call Activate without initialisation of the context
	// -----------------------------------------------------------------------
	
	TRequestStatus	requestStatus;
	packetMbmsContext.Activate( requestStatus );
	User::WaitForRequest( requestStatus );
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrArgument, requestStatus.Int() ); 	
 	
	CleanupStack::PopAndDestroy( 3, this ); //packetMbmsContext, packetService, this

    }	 
    
/**
@SYMTestCaseID BA-CTSY-MCF-PA-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketMbmsContext::Activate
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketMbmsContext::Activate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestActivate0004L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );
	
	RBuf8 expectData;
	CleanupClosePushL( expectData );
	
	RBuf8 data2;
	CleanupClosePushL( data2 );
	
	RBuf8 expectData2;
	CleanupClosePushL( expectData2 );

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( telServer2 );

	RMobilePhone phone2;
	ret = phone2.Open( telServer2, KMmTsyPhoneName );
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( phone2 );
	
	// Open PacketService for the first client
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open PacketService for the second client
	RPacketService packetService2;
	packetService2.Open( phone2 );
	CleanupClosePushL( packetService2 );

	// Open first context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );
	
	// initialisation of the first context 
	TInt status = InitMbmsContextL( packetMbmsContext, contextId );
	ASSERT_EQUALS( KErrNone, status );
	AssertMockLtsyStatusL();
	
	
	// Configure context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(packetMbmsContext, contextId, contextConfig);
	ASSERT_EQUALS( KErrNone, status );
	AssertMockLtsyStatusL();
	
	// Open second context
	TInfoName contextId2;	
	RPacketMbmsContext packetMbmsContext2;
	packetMbmsContext2.OpenNewContext( packetService2, contextId2 );
	CleanupClosePushL( packetMbmsContext2 );
	AssertMockLtsyStatusL();
	
	// initialisation of the second context 
	status = InitMbmsContextL( packetMbmsContext2, contextId2 );
	ASSERT_EQUALS( KErrNone, status );
	AssertMockLtsyStatusL();
	
	// Configure context2
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig2;
	status = SetMbmsConfigL(packetMbmsContext2, contextId2, contextConfig2);
	ASSERT_EQUALS( KErrNone, status );
	AssertMockLtsyStatusL();
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketMbmsContext::Activate
 	//-------------------------------------------------------------------------	
		
 	// data for ExpectL for first client
 	TMockLtsyData1 <TInfoName> expData( contextId );
	expData.SerialiseL( expectData );
	
	// data for CompleteL for first client
	TMockLtsyData2< TInfoName, RPacketMbmsContext::TContextConfigMbmsV1 > ltsyData1( contextId, contextConfig );
	ltsyData1.SerialiseL( data );	
	
	// data for ExpectL for second client
 	TMockLtsyData1 <TInfoName> expData2( contextId2 );
	expData2.SerialiseL( expectData2 );
	
	// data for CompleteL for second client
	TMockLtsyData2< TInfoName, RPacketMbmsContext::TContextConfigMbmsV1 > ltsyData2( contextId2, contextConfig2 );
	ltsyData2.SerialiseL( data2 );
	
	// first request
	TRequestStatus	requestStatus;
	iMockLTSY.ExpectL( EPacketContextActivate, expectData );
	iMockLTSY.CompleteL( EPacketContextActivate, KErrNone, data );		
	packetMbmsContext.Activate( requestStatus );
	
	// second request
	TRequestStatus	requestStatus2;
	iMockLTSY.ExpectL( EPacketContextActivate, expectData2 );
	iMockLTSY.CompleteL( EPacketContextActivate, KErrNone, data2 );		
	packetMbmsContext2.Activate( requestStatus2 );	
	
	// wait for first answer
	User::WaitForRequest( requestStatus );
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );		
	
	// wait for the second answer
	User::WaitForRequest( requestStatus2 );
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNone, requestStatus2.Int() );		

	CleanupStack::PopAndDestroy( 11, this ); // ... phone2, telServer2, expectData, data, this
    }

/**
@SYMTestCaseID BA-CTSY-MCF-PA-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::Activate with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::Activate and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestActivate0005L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	RBuf8 data;
	CleanupClosePushL( data );
	
	RBuf8 expectData;
	CleanupClosePushL( expectData );
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );
	
	// initialisation of the context 
	TInt status = InitMbmsContextL( packetMbmsContext, contextId );
	ASSERT_EQUALS( KErrNone, status );		
 
 	// Configure context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(packetMbmsContext, contextId, contextConfig);
	ASSERT_EQUALS( KErrNone, status );
 
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketMbmsContext::Activate
 	//-------------------------------------------------------------------------
 	TMockLtsyData1 <TInfoName> expData( contextId );
	expData.SerialiseL( expectData );
	
 	iMockLTSY.ExpectL( EPacketContextActivate, expectData );
 	
 	TRequestStatus	requestStatus;
	packetMbmsContext.Activate( requestStatus );
	User::WaitForRequest( requestStatus );
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrTimedOut, requestStatus.Int() ); 	

	CleanupStack::PopAndDestroy( 5, this ); // data, expectData, packetMbmsContext, packetService, this
    }    
          
/**
@SYMTestCaseID BA-CTSY-MCF-PD-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::Deactivate
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::Deactivate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestDeactivate0001L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );	
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;		
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );
	
	// initialisation of the context 
	TInt status = InitMbmsContextL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// Configure context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(packetMbmsContext, contextId, contextConfig);
	ASSERT_EQUALS( KErrNone, status );
	
	// activate context 
	status = SetMbmsActiveL(packetMbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//------------------------------------------------------------------------- 	
 	
	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);    
	
    RPacketMbmsContext::CMbmsSession* aSession = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(aSession);	
    	
	CRetrievePcktMbmsSessionList* aSessionlist = CRetrievePcktMbmsSessionList::NewL(packetMbmsContext, *aSession);        
    CleanupStack::PushL(aSessionlist);

    CActiveRetriever::ResetRequestsNumber();
    CActiveRetriever retriever(*aSessionlist);
    
    scheduler.AddRetrieverL(retriever);
 
    aSessionlist->Start( retriever.Status() );	 	
	retriever.Activate();
	scheduler.StartScheduler();
	
	TMockLtsyData2 <TInfoName, RPacketMbmsContext::CMbmsSession*> ltsyData( contextId, aSession);
	ltsyData.SerialiseL( data );
 	iMockLTSY.ExpectL( EPacketContextDeactivate, data, KErrNotFound );		
 	
 	TRequestStatus requestStatus;
	packetMbmsContext.Deactivate(requestStatus);
	User::WaitForRequest( requestStatus );
	ASSERT_EQUALS( KErrNotFound, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------  	
    
	RBuf8 expectData;
	CleanupClosePushL( expectData );
	
	TMockLtsyData1 <TInfoName> ltsyData1( contextId);
    ltsyData1.SerialiseL(expectData);
		
	iMockLTSY.ExpectL( EPacketContextDeactivate, data );
	iMockLTSY.CompleteL( EPacketContextDeactivate, KErrGeneral, expectData  );
	
	packetMbmsContext.Deactivate( requestStatus );
	User::WaitForRequest( requestStatus );
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrGeneral, requestStatus.Int() );

 	
	iMockLTSY.ExpectL( EPacketContextDeactivate, data );
	iMockLTSY.CompleteL( EPacketContextDeactivate, KErrMbmsImpreciseServiceEntries, data );

	packetMbmsContext.Deactivate( requestStatus );
	User::WaitForRequest( requestStatus );
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrMbmsImpreciseServiceEntries, requestStatus.Int() );
	
	
	
	
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketMbmsContext::Deactivate when result is not cached.
 	//-------------------------------------------------------------------------	
	iMockLTSY.ExpectL( EPacketContextDeactivate, data );
	iMockLTSY.CompleteL( EPacketContextDeactivate, KErrNone, expectData );
	
	packetMbmsContext.Deactivate( requestStatus );
	User::WaitForRequest( requestStatus );
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );

  	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketMbmsContext::Deactivate
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated( mockLtsyStatus );	
	iMockLTSY.CompleteL( EPacketContextDeactivate, KErrNone, expectData );
	User::WaitForRequest( mockLtsyStatus );
	ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 4 ); // packetMbmsContext, packetService
	CleanupStack::PopAndDestroy( 3, this ); // data, this
    }          

/**
@SYMTestCaseID BA-CTSY-MCF-PD-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketMbmsContext::Deactivate
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketMbmsContext::Deactivate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestDeactivate0002L()
    {
    // This test should test cancellation of Deactivate
	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );
	
	RBuf8 data;
	CleanupClosePushL( data );
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	// initialisation of the context 
	TInt status = InitMbmsContextL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	
	// Configure context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(packetMbmsContext, contextId, contextConfig);
	ASSERT_EQUALS( KErrNone, status );
	
	// activate context 
	status = SetMbmsActiveL(packetMbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);	

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketMbmsContext::Deactivate
 	//-------------------------------------------------------------------------
	
	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);    
	
    RPacketMbmsContext::CMbmsSession* aSession = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(aSession);	
    	
	CRetrievePcktMbmsSessionList* aSessionlist = CRetrievePcktMbmsSessionList::NewL(packetMbmsContext, *aSession);        
    CleanupStack::PushL(aSessionlist);

    CActiveRetriever::ResetRequestsNumber();
    CActiveRetriever retriever(*aSessionlist);
    
    scheduler.AddRetrieverL(retriever);
 
    aSessionlist->Start( retriever.Status() );	 	
	retriever.Activate();
	scheduler.StartScheduler();
	
	
 	TMockLtsyData2 <TInfoName, RPacketMbmsContext::CMbmsSession* > ltsyData( contextId, aSession);
	ltsyData.SerialiseL( data );	
	
	TMockLtsyData1 <TInfoName> ltsyData1(contextId);
	ltsyData1.SerialiseL(expectData);
	
	iMockLTSY.ExpectL( EPacketContextDeactivate, data );
	iMockLTSY.CompleteL( EPacketContextDeactivate, KErrNone, expectData, 10 );
	
 	TRequestStatus requestStatus;

	packetMbmsContext.Deactivate( requestStatus );
	packetMbmsContext.CancelAsyncRequest( EPacketContextDeactivate );
	User::WaitForRequest( requestStatus );
	
	// CTSY has started a request and it is not possible to then cancel 
    // this request. The best thing for the TSY to do in this case is to
    // proceed as though the Cancel never happened.    
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 4 ); // packetMbmsContext, packetService
	CleanupStack::PopAndDestroy( 3 ); // data, this	
    }
    
/**
@SYMTestCaseID BA-CTSY-MCF-PD-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketMbmsContext::Deactivate
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketMbmsContext::Deactivate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestDeactivate0004L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );
	
	RBuf8 data2;
	CleanupClosePushL( data2 );

	RBuf8 expectData;
	CleanupClosePushL( expectData );
		
	RBuf8 expectData2;
	CleanupClosePushL( expectData2 );
	
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( telServer2 );

	RMobilePhone phone2;
	ret = phone2.Open( iTelServer, KMmTsyPhoneName );
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( phone2 );
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );

	RPacketService packetService2;                
	ret = packetService2.Open( phone2 );
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( packetService2 );
	
	// Open first context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );
	
	// Open second context
	TInfoName contextId2;	
	RPacketMbmsContext packetMbmsContext2;
	packetMbmsContext2.OpenNewContext( packetService2, contextId2 );
	CleanupClosePushL( packetMbmsContext2 );
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated( mockLtsyStatus );

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketMbmsContext::Deactivate
 	//-------------------------------------------------------------------------

	// initialisation of the context 
	TInt status = InitMbmsContextL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// initialisation of the context 
	status = InitMbmsContextL(packetMbmsContext2, contextId2);
	ASSERT_EQUALS(KErrNone, status);
	
	
	// Configure context	
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(packetMbmsContext, contextId, contextConfig);
	ASSERT_EQUALS( KErrNone, status );
	
	status = SetMbmsConfigL(packetMbmsContext2, contextId2, contextConfig);
	ASSERT_EQUALS( KErrNone, status );
	
	// activate context 
	status = SetMbmsActiveL(packetMbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);
		
	status = SetMbmsActiveL(packetMbmsContext2, contextConfig, contextId2);
	ASSERT_EQUALS(KErrNone, status);		
	
	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);    
	
    RPacketMbmsContext::CMbmsSession* aSession = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(aSession);	
    	
	CRetrievePcktMbmsSessionList* aSessionlist = CRetrievePcktMbmsSessionList::NewL(packetMbmsContext, *aSession);        
    CleanupStack::PushL(aSessionlist);

    CActiveRetriever::ResetRequestsNumber();
    CActiveRetriever retriever(*aSessionlist);
    
    scheduler.AddRetrieverL(retriever);
 
    aSessionlist->Start( retriever.Status() );	 	
	retriever.Activate();
	scheduler.StartScheduler();
	TMockLtsyData2 <TInfoName, RPacketMbmsContext::CMbmsSession* > ltsyData( contextId, aSession);
 	ltsyData.SerialiseL( data );
 
    
 	RPacketMbmsContext::CMbmsSession* aSession2 = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(aSession2);	
    	
	CRetrievePcktMbmsSessionList* aSessionlist2 = CRetrievePcktMbmsSessionList::NewL(packetMbmsContext2, *aSession2);        
    CleanupStack::PushL(aSessionlist2);

    CActiveRetriever::ResetRequestsNumber();
    CActiveRetriever retriever1(*aSessionlist2);
    
    scheduler.AddRetrieverL(retriever1);
 
    aSessionlist->Start( retriever1.Status() );	 	
	retriever1.Activate();
	scheduler.StartScheduler();
 	
	TMockLtsyData2 <TInfoName, RPacketMbmsContext::CMbmsSession* > ltsyData2( contextId2, aSession2);
	ltsyData2.SerialiseL( data2 );
		
	iMockLTSY.ExpectL( EPacketContextDeactivate, data );
	
	TMockLtsyData1 <TInfoName> ltsyData3(contextId);
	ltsyData3.SerialiseL(expectData);
	
	iMockLTSY.CompleteL( EPacketContextDeactivate, KErrNone, expectData, 5 );
	
	iMockLTSY.ExpectL( EPacketContextDeactivate, data2 );
	
	TMockLtsyData1 <TInfoName> ltsyData4(contextId2);
	ltsyData4.SerialiseL(expectData2);
	
	iMockLTSY.CompleteL( EPacketContextDeactivate, KErrNone, expectData2, 5 );
	
	TRequestStatus	requestStatus;
	packetMbmsContext.Deactivate( requestStatus );
	
	TRequestStatus	requestStatus2;
	packetMbmsContext2.Deactivate( requestStatus2 );
	
	User::WaitForRequest( requestStatus );
	ASSERT_EQUALS(KErrNone, requestStatus.Int() );
	User::WaitForRequest( requestStatus2 );
	ASSERT_EQUALS( KErrNone, requestStatus2.Int() );

	CleanupStack::PopAndDestroy(15, this); // phone2, telServer2, data, data2, this
    }
       
/**
@SYMTestCaseID BA-CTSY-MCF-PD-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::Deactivate with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::Deactivate and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestDeactivate0005L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketMbmsContext::Deactivate
 	//-------------------------------------------------------------------------
	TRequestStatus	requestStatus;

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);    
		
	RPacketMbmsContext::CMbmsSession* aSession = RPacketMbmsContext::CMbmsSession::NewL();  
	CleanupStack::PushL(aSession);	
	    	
	CRetrievePcktMbmsSessionList* aSessionlist = CRetrievePcktMbmsSessionList::NewL(packetMbmsContext, *aSession);        
	CleanupStack::PushL(aSessionlist);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever retriever(*aSessionlist);
	    
	scheduler.AddRetrieverL(retriever);
	 
	aSessionlist->Start( retriever.Status() );	 	
	retriever.Activate();
	scheduler.StartScheduler();

	TMockLtsyData2 <TInfoName, RPacketMbmsContext::CMbmsSession* > ltsyData( contextId, aSession);
	ltsyData.SerialiseL( data );

	// initialisation of the context 
	TInt status = InitMbmsContextL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
		
	// Configure context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(packetMbmsContext, contextId, contextConfig);
	ASSERT_EQUALS( KErrNone, status );
		
	// activate context 
	status = SetMbmsActiveL(packetMbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);

	iMockLTSY.ExpectL( EPacketContextDeactivate, data );	
	packetMbmsContext.Deactivate( requestStatus );
	
	User::WaitForRequest( requestStatus );
	ASSERT_EQUALS( KErrTimedOut, requestStatus.Int() );
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 4 ); // packetMbmsContext, packetService
	CleanupStack::PopAndDestroy( 2, this ); // data, this
    }               
          
           
/**
@SYMTestCaseID BA-CTSY-MCF-PDE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::Delete
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::Delete
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestDelete0001L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );
	
	RBuf8 outData;
	CleanupClosePushL( outData );	
	
	RBuf8 servData;
	CleanupClosePushL( servData );	
	
	RBuf8 expectData;
	CleanupClosePushL( expectData );
			
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;	
	packetMbmsContext.OpenNewContext( packetService, contextId );	
	CleanupClosePushL( packetMbmsContext );
	
	TRequestStatus requestStatus;
 	TMockLtsyData1<TInfoName > ltsyData( contextId );
	ltsyData.SerialiseL( data );	
		
	//-------------------------------------------------------------------------
	// Send Delete request when ContextStatus is Unknown
 	//-------------------------------------------------------------------------
		
	packetMbmsContext.Delete( requestStatus );
	
	User::WaitForRequest( requestStatus );	
	ASSERT_EQUALS( KErrNotReady, requestStatus.Int() );	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// activate context properly
 	//-------------------------------------------------------------------------
		
	// initialisation of the context 
	TInt status = InitMbmsContextL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
		
	// Configure context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(packetMbmsContext, contextId, contextConfig);
	ASSERT_EQUALS( KErrNone, status );
	
	// activate context 
	status = SetMbmsActiveL(packetMbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// deactivate
	status = SetMbmsDeactiveL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);		
		
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketMbmsContext::Delete
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	
	iMockLTSY.NotifyTerminated( mockLtsyStatus );
	// send completion 
	iMockLTSY.CompleteL( EPacketContextDelete, KErrNone, data );
	// wait for completion
	User::WaitForRequest( mockLtsyStatus );
	ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
 	TMockLtsyData1<TInfoName> expData( contextId );
	expData.SerialiseL( expectData );
	
	iMockLTSY.ExpectL( EPacketContextDelete, expectData, KErrNotSupported );

	packetMbmsContext.Delete( requestStatus );
	
	User::WaitForRequest( requestStatus );	
	ASSERT_EQUALS( KErrNotSupported, requestStatus.Int() );	
	AssertMockLtsyStatusL();
 	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL ( EPacketContextDelete, expectData );
	iMockLTSY.CompleteL( EPacketContextDelete, KErrGeneral, data );

	packetMbmsContext.Delete( requestStatus );
	
	User::WaitForRequest( requestStatus );	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int() );	
	AssertMockLtsyStatusL();
 
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketMbmsContext::Delete when result is not cached.
 	//-------------------------------------------------------------------------		
 	
 	// remove context succefully
	iMockLTSY.ExpectL( EPacketContextDelete, data );
	iMockLTSY.CompleteL( EPacketContextDelete, KErrNone, data );
	packetMbmsContext.Delete( requestStatus );
		
	User::WaitForRequest( requestStatus );	
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );	
	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy( 7, this ); //packetMbmsContext, packetService, expectData, data, this
    }       

/**
@SYMTestCaseID BA-CTSY-MCF-PDE-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketMbmsContext::Delete
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketMbmsContext::Delete
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestDelete0004L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );	
	
	RBuf8 data2;
	CleanupClosePushL( data2 );
	
	RBuf8 expectData;
	CleanupClosePushL( expectData );
	
	RBuf8 expectData2;
	CleanupClosePushL( expectData2 );

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( telServer2 );

	RMobilePhone phone2;
	ret = phone2.Open( iTelServer, KMmTsyPhoneName );
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( phone2 );
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	RPacketService packetService2;                
	ret = packetService2.Open( phone2 );
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( packetService2 );
	
	// Open first context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );
	
	// Open second context
	TInfoName contextId2;	
	RPacketMbmsContext packetMbmsContext2;
	packetMbmsContext2.OpenNewContext( packetService2, contextId2 );
	CleanupClosePushL( packetMbmsContext2 );
	
	// initialise context's first
	TInt status = InitMbmsContextL( packetMbmsContext, contextId );
	ASSERT_EQUALS( KErrNone, status );  
	
	TInt status2 = InitMbmsContextL( packetMbmsContext2, contextId2 );
	ASSERT_EQUALS( KErrNone, status2 );  	
	
	// Configure context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(packetMbmsContext, contextId, contextConfig);
	ASSERT_EQUALS( KErrNone, status );
	
	// Configure context 2
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig2;
	status = SetMbmsConfigL(packetMbmsContext2, contextId2, contextConfig2);
	ASSERT_EQUALS( KErrNone, status );
	
	
	// activate context 
	status = SetMbmsActiveL(packetMbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// activate context 
	status2 = SetMbmsActiveL(packetMbmsContext2, contextConfig2, contextId2);
	ASSERT_EQUALS(KErrNone, status2);	
	
	// deactivate
	status = SetMbmsDeactiveL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	status2 = SetMbmsDeactiveL(packetMbmsContext2, contextId2);
	ASSERT_EQUALS(KErrNone, status2);		
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketMbmsContext::Delete
 	//-------------------------------------------------------------------------
	
 	// data for first CompleteL
 	TMockLtsyData1<TInfoName> ltsyData( contextId );
	ltsyData.SerialiseL( data );
 	
	// data for second CompleteL
 	TMockLtsyData1<TInfoName> ltsyData2( contextId2 );
	ltsyData2.SerialiseL( data2 );	
	
	// for the first request
	TRequestStatus requestStatus;
	TMockLtsyData1<TInfoName> expData( contextId );
	expData.SerialiseL( expectData );
	
	// for the second request
	TRequestStatus requestStatus2;
	TMockLtsyData1<TInfoName> expData2( contextId2 );
	expData2.SerialiseL( expectData2 );
			
	iMockLTSY.ExpectL( EPacketContextDelete, expectData );	 	
 	iMockLTSY.CompleteL( EPacketContextDelete, KErrNone, data );
	iMockLTSY.ExpectL( EPacketContextDelete, expectData2 );
	iMockLTSY.CompleteL( EPacketContextDelete, KErrNone, data2 );
	
	// send first request
	packetMbmsContext.Delete( requestStatus );		
	
	// send second request
	packetMbmsContext2.Delete( requestStatus2 );
	
	// wait for first answer
	User::WaitForRequest( requestStatus );		
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );
		
	// wait for second answer
	User::WaitForRequest( requestStatus2 );		
	ASSERT_EQUALS( KErrNone, requestStatus2.Int() );

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy( 2 ); // expectData2, expectData
	CleanupStack::PopAndDestroy( 9, this ); //packetMbmsContext2, packetMbmsContext, packetService2, packetService, phone2, telServer2, data, data2, this
    }

/**
@SYMTestCaseID BA-CTSY-MCF-PDE-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::Delete with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::Delete and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestDelete0005L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup,this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );
	
	RBuf8 expectData;
	CleanupClosePushL( expectData );
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;	
	packetMbmsContext.OpenNewContext( packetService, contextId );	
	CleanupClosePushL( packetMbmsContext );
	
	// initialise context
	TInt status = InitMbmsContextL( packetMbmsContext, contextId );
	ASSERT_EQUALS( KErrNone, status );  

	// Configure context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(packetMbmsContext, contextId, contextConfig);
	ASSERT_EQUALS( KErrNone, status );
		
	// activate context 
	status = SetMbmsActiveL(packetMbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);
 	
	// deactivate
	status = SetMbmsDeactiveL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status); 	
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketMbmsContext::Delete
 	//------------------------------------------------------------------------- 	
 	TRequestStatus requestStatus; 	
 	TMockLtsyData1<TInfoName> expData( contextId );
	expData.SerialiseL( expectData );
	iMockLTSY.ExpectL( EPacketContextDelete, expectData );

	packetMbmsContext.Delete( requestStatus );
		
	User::WaitForRequest( requestStatus );	
	ASSERT_EQUALS( KErrTimedOut, requestStatus.Int() );	
	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy( 5, this ); //packetMbmsContext, packetService, data, expectData, this
    }          
    
     
/**
@SYMTestCaseID BA-CTSY-MCF-PC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::Close
@SYMTestPriority 
@SYMTestActions Invokes RPacketMbmsContext::Close
@SYMTestExpectedResults Pass
@SYMTestType 
*/
void CCTsyMbmsContextFU::TestClose0001L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup,this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );
	
	RBuf8 expectData;
	CleanupClosePushL( expectData );
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;	
	packetMbmsContext.OpenNewContext( packetService, contextId );	
	CleanupClosePushL( packetMbmsContext );
	
	// initialise context
	TInt status = InitMbmsContextL( packetMbmsContext, contextId );
	ASSERT_EQUALS( KErrNone, status );  

	// Configure context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(packetMbmsContext, contextId, contextConfig);
	ASSERT_EQUALS( KErrNone, status );
			
	// activate context 
	status = SetMbmsActiveL(packetMbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);
 	
	// deactivate
	status = SetMbmsDeactiveL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// delete context
	status = SetMbmsDeleteL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// and finally close context
	TRequestStatus requestStatus; 
	
	packetMbmsContext.Close( );
	
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, KErrNone);
	 	
	CleanupStack::PopAndDestroy( 5, this );			
    }            

/**
@SYMTestCaseID BA-CTSY-MCF-PGC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::GetConfig
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::GetConfig
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestGetConfig0001L()
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
		
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	TRequestStatus	requestStatus;
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	
	TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig(contextConfig); 
	
	TTmgi ttmgi;
	ttmgi.SetServiceId(KMmConfigTMGI1);
	ttmgi.SetMCC(KMmConfigTmgiMMC1);
	ttmgi.SetMCC(KMmConfigTmgiMNC1);
	
	contextConfig.iTmgi = ttmgi;
	contextConfig.iMbmsAccessBearer = E2GBearer;
	contextConfig.iMbmsServicePriority = EMbmsMediumServicePriority;
	contextConfig.iMbmsServiceMode = KBroadcast;
	contextConfig.iMbmsSessionFlag = EFalse;
	
	TInt status = InitMbmsContextL( packetContext, contextId );
	ASSERT_EQUALS( KErrNone, status );  
	
	status = SetMbmsConfigL(packetContext, contextId, contextConfig);
	
	RPacketMbmsContext::TContextConfigMbmsV1 retContextConfig;
		
	TPckg<RPacketMbmsContext::TContextConfigMbmsV1> retPckgConfig(retContextConfig); 

	packetContext.GetConfig(requestStatus, retPckgConfig);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int()); 
	
	ASSERT_EQUALS( contextConfig.iTmgi.GetServiceId(),  contextConfig.iTmgi.GetServiceId());
	ASSERT_EQUALS( contextConfig.iTmgi.GetMCC(),  contextConfig.iTmgi.GetMCC());
	ASSERT_EQUALS( contextConfig.iTmgi.GetMNC(),  contextConfig.iTmgi.GetMNC());
	ASSERT_EQUALS( contextConfig.iMbmsAccessBearer,  contextConfig.iMbmsAccessBearer);
	ASSERT_EQUALS( contextConfig.iMbmsServicePriority,  retContextConfig.iMbmsServicePriority);
	ASSERT_EQUALS( contextConfig.iMbmsServiceMode,  retContextConfig.iMbmsServiceMode);
	ASSERT_EQUALS( contextConfig.iMbmsSessionFlag,  retContextConfig.iMbmsSessionFlag);
		
	CleanupStack::PopAndDestroy(5, this); //  packetService1, this	
    }

/**
@SYMTestCaseID BA-CTSY-MCF-PGC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketMbmsContext::GetConfig
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketMbmsContext::GetConfig
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestGetConfig0002L()
	{
  
	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );
	
	RBuf8 data2;
	CleanupClosePushL( data2 );

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( telServer2 );

	RMobilePhone phone2;
	ret = phone2.Open( iTelServer, KMmTsyPhoneName );
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( phone2 );
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );

	RPacketService packetService2;                
	ret = packetService2.Open( phone2 );
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( packetService2 );
	
	// Open first context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );
	
	// Open second context
	TInfoName contextId2;	
	RPacketMbmsContext packetMbmsContext2;
	packetMbmsContext2.OpenNewContext( packetService2, contextId2 );
	CleanupClosePushL( packetMbmsContext2 );
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated( mockLtsyStatus );

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketMbmsContext::GetConfig
 	//-------------------------------------------------------------------------

	// If this API is flow controlled by Etel or if this API is
	// synchronous, remove this test completely.
	
	TRequestStatus	requestStatus;
 	TMockLtsyData1<TInfoName > ltsyData( contextId );
	ltsyData.SerialiseL( data );
	
	// data for second context
	TRequestStatus	requestStatus2;
 	TMockLtsyData1<TInfoName > ltsyData2( contextId2 );
	ltsyData2.SerialiseL( data2 );
	
	// initialisation of the context 
	TInt status = InitMbmsContextL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	TInt status2 = InitMbmsContextL(packetMbmsContext2, contextId2);
	ASSERT_EQUALS(KErrNone, status2);	
	
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;	
	// activate context 
	status = SetMbmsActiveL(packetMbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig2;	
	// activate context 
	status2 = SetMbmsActiveL(packetMbmsContext2, contextConfig2, contextId2);
	ASSERT_EQUALS(KErrNone, status2);	
	
    // -----------------------------------------------------------------------
    // initialise parameters
    // -----------------------------------------------------------------------		
	
	// first context is selected and high priority 
	TTmgi ttmgi;
	ttmgi.SetServiceId(KMmConfigTMGI1);
	ttmgi.SetMCC(KMmConfigTmgiMMC1);
	ttmgi.SetMCC(KMmConfigTmgiMNC1);	
	
	contextConfig.iTmgi = ttmgi;
	contextConfig.iMbmsSessionFlag = ETrue;
	contextConfig.iMbmsServicePriority = EMbmsHighServicePriority;
	contextConfig.iMbmsServiceMode = KSelectedBroadcast;
	contextConfig.iMbmsAccessBearer = E3GBearer;
	
	// second context
	
	TTmgi ttmgi2;
	ttmgi2.SetServiceId(KMmConfigTMGI2);
	ttmgi2.SetMCC(KMmConfigTmgiMMC2);
	ttmgi2.SetMCC(KMmConfigTmgiMNC2);
	
	contextConfig2.iTmgi = ttmgi2;
	contextConfig2.iMbmsSessionFlag = EFalse;
	contextConfig2.iMbmsServicePriority = EMbmsLowServicePriority;
	contextConfig2.iMbmsServiceMode = KBroadcast;
	contextConfig2.iMbmsAccessBearer = E3GBearer;
	
	status = SetMbmsConfigL( packetMbmsContext, contextId, contextConfig );
	ASSERT_EQUALS(KErrNone, status);  
	
	status2 = SetMbmsConfigL( packetMbmsContext2, contextId2, contextConfig2 );
	ASSERT_EQUALS(KErrNone, status2);
	
    // -----------------------------------------------------------------------
    // get parameters
    // -----------------------------------------------------------------------
    RPacketMbmsContext::TContextConfigMbmsV1 getConfig;	
    RPacketMbmsContext::TContextConfigMbmsV1 getConfig2;	
    
	TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig(getConfig); 
	TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig2(getConfig2); 
	
	// send first request
	packetMbmsContext.GetConfig(requestStatus, pckgConfig);
	// send second request
	packetMbmsContext2.GetConfig(requestStatus2, pckgConfig2);
	
		
	// wait for the both answers
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());     
	User::WaitForRequest( requestStatus2 );
	ASSERT_EQUALS( KErrNone, requestStatus2.Int() );

	ASSERT_EQUALS(KMmConfigTMGI1, getConfig.iTmgi.GetServiceId());  
	ASSERT_TRUE(getConfig.iMbmsSessionFlag);
	ASSERT_EQUALS(EMbmsHighServicePriority, getConfig.iMbmsServicePriority );
	ASSERT_EQUALS(KSelectedBroadcast, getConfig.iMbmsServiceMode);
	ASSERT_EQUALS(E3GBearer, getConfig.iMbmsAccessBearer);

	ASSERT_EQUALS(KMmConfigTMGI2, getConfig2.iTmgi.GetServiceId());  	
    ASSERT_TRUE(!getConfig2.iMbmsSessionFlag);
	ASSERT_EQUALS(EMbmsLowServicePriority, getConfig2.iMbmsServicePriority);
	ASSERT_EQUALS(KBroadcast, getConfig2.iMbmsServiceMode);
	ASSERT_EQUALS(E3GBearer, getConfig2.iMbmsAccessBearer);

	CleanupStack::PopAndDestroy(4); // packetMbmsContext, packetService, packetMbmsContext2, packetService2
	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, data, data2, this
						
	}

/**
@SYMTestCaseID BA-CTSY-MCF-PGC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::GetConfig with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::GetConfig with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestGetConfig0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
		
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	TRequestStatus	requestStatus;
	RPacketContext::TContextConfigGPRS contextConfig;
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(contextConfig); 
	
	TInt status = InitMbmsContextL( packetContext, contextId );
	ASSERT_EQUALS( KErrNone, status );  
	
	packetContext.GetConfig(requestStatus, pckgConfig);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int()); 
	
	CleanupStack::PopAndDestroy(3); //this, packetService packetContext
	}   

/**
@SYMTestCaseID BA-CTSY-MCF-PGS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::GetStatus
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::GetStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestGetStatus0001L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 expectData;
    CleanupClosePushL( expectData );
        
	RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    TInfoName contextId;  
    RPacketMbmsContext mbmsContext;
    mbmsContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(mbmsContext);

    // test 1: context not inited
    RPacketContext::TContextStatus contextStatus;    
    mbmsContext.GetStatus(contextStatus);
    ASSERT_EQUALS(RPacketContext::EStatusUnknown, contextStatus);
    
    // initialisation of the context 
	TInt status = InitMbmsContextL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
		
    // test2: context inited
    mbmsContext.GetStatus(contextStatus);
    ASSERT_EQUALS(RPacketContext::EStatusInactive, contextStatus);
       
    TMockLtsyData1 <TInfoName> expData(contextId);
   	expData.SerialiseL(expectData);	  
   
    RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
   	TMockLtsyData2< TInfoName, RPacketMbmsContext::TContextConfigMbmsV1 > ltsyData1(contextId, contextConfig);
   	ltsyData1.SerialiseL(data);   	
    
    iMockLTSY.ExpectL(EPacketContextActivate, expectData);
	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data);
	
	TRequestStatus  requestStatus;
	mbmsContext.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();    
	ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    	
   	// test3: context activated
	mbmsContext.GetStatus(contextStatus);
	ASSERT_EQUALS(RPacketContext::EStatusActive, contextStatus);
  
    CleanupStack::PopAndDestroy(2); //packetContext, packetService
    CleanupStack::PopAndDestroy(3, this); // data,expectData, this 
    }

/**
@SYMTestCaseID BA-CTSY-MCF-PGLEC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::GetLastErrorCause
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::GetLastErrorCause
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestGetLastErrorCause0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext mbmsContext;
	mbmsContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(mbmsContext);	
	
	RBuf8 data;
	CleanupClosePushL(data);

	// initialise context
	TInt status = InitMbmsContextL( mbmsContext, contextId );
	ASSERT_EQUALS( KErrNone, status );  

	// Configure context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;
	status = SetMbmsConfigL(mbmsContext, contextId, contextConfig);
	ASSERT_EQUALS( KErrNone, status );
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	TInt error;
	status = mbmsContext.GetLastErrorCause(error);
	ASSERT_EQUALS(KErrNone, status);
	ASSERT_EQUALS(KErrNone, error);

	//-------------------------------------------------------------------------
	// TEST B: Emulate some error
 	//-------------------------------------------------------------------------
 		
	// Set context in to Activating state 
	TContextMisc  completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusActivating;    
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData1(contextId, completeMisc);
 	data.Close();
	ltsyData1.SerialiseL(data);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	// Emulate an error 
    completeMisc.iStatus = RPacketContext::EStatusActive;    
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData(contextId, completeMisc);
	data.Close();
	ltsyData.SerialiseL(data);		

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrGeneral, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	// Get the error
	status = mbmsContext.GetLastErrorCause(error);
	ASSERT_EQUALS(KErrNone, status);
	ASSERT_EQUALS(KErrGeneral, error);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // packetContext, mbmsService, data, this

	}

/**
@SYMTestCaseID BA-CTSY-MCF-PNCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::NotifyConfigChanged
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::NotifyConfigChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestNotifyConfigChanged0001L()
	{
	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );
	
	// initialisation of the context 
	TInt status = InitMbmsContextL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated( mockLtsyStatus );
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketMbmsContext::NotifyConfigChanged when result is not cached.
 	//-------------------------------------------------------------------------
	// data for NotifyConfigChanged	
	TRequestStatus requestStatus;
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;	
	TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig( contextConfig );	
	
	//data for ComleteL   
 	RPacketMbmsContext::TContextConfigMbmsV1 configComplete;  	 	
	
 	TTmgi ttmgi;
	ttmgi.SetServiceId(KMmConfigTMGI1);
	ttmgi.SetMCC(KMmConfigTmgiMMC1);
	ttmgi.SetMNC(KMmConfigTmgiMNC1);
	
	configComplete.iTmgi = ttmgi;
	configComplete.iMbmsAccessBearer = E2GBearer;
	configComplete.iMbmsServicePriority = EMbmsMediumServicePriority;
	configComplete.iMbmsServiceMode = KBroadcast;
	configComplete.iMbmsSessionFlag = EFalse;

 	TMockLtsyData2<TInfoName, RPacketMbmsContext::TContextConfigMbmsV1> ltsyData( contextId, configComplete );
	ltsyData.SerialiseL( data );	

	// send request	
	packetMbmsContext.NotifyConfigChanged( requestStatus, pckgConfig );
		
	// sent completion
	iMockLTSY.CompleteL( ECtsyPacketMbmsContextNotifyConfigChanged, KErrNone, data );
	// wait for completion
	User::WaitForRequest( mockLtsyStatus );
		
	// wait for request
	User::WaitForRequest( requestStatus );		
	AssertMockLtsyStatusL();		
    ASSERT_EQUALS( KErrNone, requestStatus.Int() );
	
    ASSERT_EQUALS( configComplete.iTmgi.GetServiceId(),  contextConfig.iTmgi.GetServiceId());
	ASSERT_EQUALS( configComplete.iTmgi.GetMCC(),  contextConfig.iTmgi.GetMCC());
	ASSERT_EQUALS( configComplete.iTmgi.GetMNC(),  contextConfig.iTmgi.GetMNC());
	ASSERT_EQUALS( configComplete.iMbmsAccessBearer,  contextConfig.iMbmsAccessBearer);
	ASSERT_EQUALS( configComplete.iMbmsServicePriority,  contextConfig.iMbmsServicePriority);
	ASSERT_EQUALS( configComplete.iMbmsServiceMode,  contextConfig.iMbmsServiceMode);
	ASSERT_EQUALS( configComplete.iMbmsSessionFlag,  contextConfig.iMbmsSessionFlag);
	
    
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketMbmsContext::NotifyConfigChanged
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated( mockLtsyStatus );
	// sent completion
	iMockLTSY.CompleteL( ECtsyPacketMbmsContextNotifyConfigChanged, KErrNone, data );
	// wait for completion
	User::WaitForRequest( mockLtsyStatus );
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );
	
	CleanupStack::PopAndDestroy( 2 ); // packetService, packetMbmsContext
	CleanupStack::PopAndDestroy( 2, this ); // data, this	
	}
	
/**
@SYMTestCaseID BA-CTSY-MCF-PNCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketMbmsContext::NotifyConfigChanged
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketMbmsContext::NotifyConfigChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestNotifyConfigChanged0002L()
	{
    // This test should test cancellation of NotifyConfigChanged
    // If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated( mockLtsyStatus );
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketMbmsContext::NotifyConfigChanged
 	//------------------------------------------------------------------------- 	
	// data for NotifyConfigChanged	
	TRequestStatus requestStatus;
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;	
	TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig( contextConfig );	
	
	//data for ComleteL    
 	RPacketMbmsContext::TContextConfigMbmsV1 configComplete;  	 	
	
 	TTmgi ttmgi;
	ttmgi.SetServiceId(KMmConfigTMGI1);
	ttmgi.SetMCC(KMmConfigTmgiMMC1);
	ttmgi.SetMNC(KMmConfigTmgiMNC1);
	
	configComplete.iTmgi = ttmgi;
	configComplete.iMbmsAccessBearer = E2GBearer;
	configComplete.iMbmsServicePriority = EMbmsMediumServicePriority;
	configComplete.iMbmsServiceMode = KBroadcast;
	configComplete.iMbmsSessionFlag = EFalse;	

	TMockLtsyData2 <TInfoName, RPacketMbmsContext::TContextConfigMbmsV1> ltsyData( contextId, configComplete );
	ltsyData.SerialiseL( data );	

	// send request	
	packetMbmsContext.NotifyConfigChanged( requestStatus, pckgConfig );	
	//send cancellation
	packetMbmsContext.CancelAsyncRequest( ECtsyPacketMbmsContextNotifyConfigChanged );
		
	// sent completion
	iMockLTSY.CompleteL( ECtsyPacketMbmsContextNotifyConfigChanged, KErrNone, data );
	// wait for completion
	User::WaitForRequest( mockLtsyStatus );
	
	// wait for request
	User::WaitForRequest( requestStatus );		
	AssertMockLtsyStatusL();
	ASSERT_TRUE( KErrCancel == requestStatus.Int() || KErrNone == requestStatus.Int());

	CleanupStack::PopAndDestroy( 2 ); // packetService, packetMbmsContext
	CleanupStack::PopAndDestroy( 2 ); // data, this	
	}

/**
@SYMTestCaseID BA-CTSY-MCF-PNCC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::NotifyConfigChanged with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::NotifyConfigChanged with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestNotifyConfigChanged0003L()
	{
    // This test should test sending bad parameter data for NotifyConfigChanged
    // If this API does not have any parameters, then remove this test completely.

	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup,this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated( mockLtsyStatus );

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RPacketMbmsContext::NotifyConfigChanged
 	//-------------------------------------------------------------------------
	// data for NotifyConfigChanged	
	TRequestStatus requestStatus;
	RPacketMbmsContext::TContextConfigCDMA contextConfig;	
	TPckg<RPacketMbmsContext::TContextConfigCDMA> pckgConfig( contextConfig );	

	// send request	
	packetMbmsContext.NotifyConfigChanged( requestStatus, pckgConfig );

	// wait for request
	User::WaitForRequest(requestStatus );		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNotSupported, requestStatus.Int() );

	// Done !
	CleanupStack::PopAndDestroy( 2 ); // packetMbmsService, packetContext
	CleanupStack::PopAndDestroy( 2, this ); // data, this
	}

/**
@SYMTestCaseID BA-CTSY-MCF-PNCC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketMbmsContext::NotifyConfigChanged
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketMbmsContext::NotifyConfigChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestNotifyConfigChanged0004L()
	{
	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );
	
	RBuf8 data2;
	CleanupClosePushL( data2 );

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( telServer2 );

	RMobilePhone phone2;
	ret = phone2.Open( iTelServer, KMmTsyPhoneName );
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( phone2 );
	
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	RPacketService packetService2;                
	ret = packetService2.Open( phone2 );
	ASSERT_EQUALS( KErrNone, ret );
	CleanupClosePushL( packetService2 );
	
	// Open first context
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;
	packetMbmsContext.OpenNewContext( packetService, contextId );
	CleanupClosePushL( packetMbmsContext );
	
			
	// Open second context
	TInfoName contextId2;	
	RPacketMbmsContext packetMbmsContext2;
	packetMbmsContext2.OpenNewContext( packetService2, contextId2 );
	CleanupClosePushL( packetMbmsContext2 );

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated( mockLtsyStatus );
	
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketMbmsContext::NotifyConfigChanged
 	//-------------------------------------------------------------------------

	// If this API is flow controlled by Etel or if this API is
	// synchronous, remove this test completely.

	// To write this test:
	// If this API has parameters which have different versions, then each
	// client in this test should use a different version of parameter.
	// Otherwise, they should use the same version.
	// All versions of parameters should be used. i.e. If there are three
	// versions of a parameter, there should be three clients, one using
	// each one. If the parameter does not have different versions, use
	// two clients each using the same version.
	
	// data for NotifyConfigChanged	1
	TRequestStatus requestStatus;
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig1;	
	TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig1( contextConfig1 );	
	
	//data for CompleteL 1   
 	RPacketMbmsContext::TContextConfigMbmsV1 configComplete1;	
 	
 	TTmgi ttmgi;
	ttmgi.SetServiceId(KMmConfigTMGI1);
	ttmgi.SetMCC(KMmConfigTmgiMMC1);
	ttmgi.SetMNC(KMmConfigTmgiMNC1);
	
	configComplete1.iTmgi = ttmgi;
	configComplete1.iMbmsAccessBearer = E2GBearer;
	configComplete1.iMbmsServicePriority = EMbmsMediumServicePriority;
	configComplete1.iMbmsServiceMode = KBroadcast;
	configComplete1.iMbmsSessionFlag = EFalse;	
 	TMockLtsyData2<TInfoName, RPacketMbmsContext::TContextConfigMbmsV1> ltsyData( contextId, configComplete1 );
	ltsyData.SerialiseL( data );
		
	// data for NotifyConfigChanged	2
	TRequestStatus requestStatus2;
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig2;	
	TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig2( contextConfig2 );	
	
	//data for CompleteL 2 
 	RPacketMbmsContext::TContextConfigMbmsV1 configComplete2; 
 	
	configComplete2.iTmgi = ttmgi;
	configComplete2.iMbmsAccessBearer = E2GBearer;
	configComplete2.iMbmsServicePriority = EMbmsHighServicePriority;
	configComplete2.iMbmsServiceMode = KSelectedBroadcast;
	configComplete2.iMbmsSessionFlag = ETrue;	
	
 	TMockLtsyData2<TInfoName, RPacketMbmsContext::TContextConfigMbmsV1> ltsyData2( contextId2, configComplete2 );
	ltsyData2.SerialiseL( data2 );
	
	// send first request	
	packetMbmsContext.NotifyConfigChanged( requestStatus, pckgConfig1 );
	
	// send second request	
	packetMbmsContext2.NotifyConfigChanged( requestStatus2, pckgConfig2 );
	
	// sent first completion
	iMockLTSY.CompleteL( ECtsyPacketMbmsContextNotifyConfigChanged, KErrNone, data );
	
	// sent second completion
	iMockLTSY.CompleteL( ECtsyPacketMbmsContextNotifyConfigChanged, KErrNone, data2 );	
	
	
	// wait for completion
	User::WaitForRequest( mockLtsyStatus );
		
	
	// wait for first answer
	User::WaitForRequest( requestStatus );
		
	// wait for second answer
	User::WaitForRequest( requestStatus2 );	
	AssertMockLtsyStatusL();		
	
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );
	ASSERT_EQUALS( configComplete1.iTmgi.GetServiceId(),  contextConfig1.iTmgi.GetServiceId());
	ASSERT_EQUALS( configComplete1.iTmgi.GetMCC(),  contextConfig1.iTmgi.GetMCC());
	ASSERT_EQUALS( configComplete1.iTmgi.GetMNC(),  contextConfig1.iTmgi.GetMNC());
	ASSERT_EQUALS( configComplete1.iMbmsAccessBearer,  contextConfig1.iMbmsAccessBearer);
	ASSERT_EQUALS( configComplete1.iMbmsServicePriority,  contextConfig1.iMbmsServicePriority);
	ASSERT_EQUALS( configComplete1.iMbmsServiceMode,  contextConfig1.iMbmsServiceMode);
	ASSERT_EQUALS( configComplete1.iMbmsSessionFlag,  contextConfig1.iMbmsSessionFlag);
		
		
	ASSERT_EQUALS( KErrNone, requestStatus2.Int() );
	
	ASSERT_EQUALS( configComplete2.iTmgi.GetServiceId(),  contextConfig2.iTmgi.GetServiceId());
	ASSERT_EQUALS( configComplete2.iTmgi.GetMCC(),  contextConfig2.iTmgi.GetMCC());
	ASSERT_EQUALS( configComplete2.iTmgi.GetMNC(),  contextConfig2.iTmgi.GetMNC());
	ASSERT_EQUALS( configComplete2.iMbmsAccessBearer,  contextConfig2.iMbmsAccessBearer);
	ASSERT_EQUALS( configComplete2.iMbmsServicePriority,  contextConfig2.iMbmsServicePriority);
	ASSERT_EQUALS( configComplete2.iMbmsServiceMode,  contextConfig2.iMbmsServiceMode);
	ASSERT_EQUALS( configComplete2.iMbmsSessionFlag,  contextConfig2.iMbmsSessionFlag);
		
					  
	// Done !
	CleanupStack::PopAndDestroy( 9, this ); //packetMbmsContext2, packetMbmsContext, packetService2, packetService, phone2, telServer2, data, data2 this
	}

/**
@SYMTestCaseID 
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::NotifyStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::NotifyStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestNotifyStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext mbmsContext;
	mbmsContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(mbmsContext);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	// data for NotifyStatusChange
	TRequestStatus	requestStatus;
	RPacketContext::TContextStatus  contextStatus; 

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::NotifyStatusChange when result is not cached.
 	//------------------------------------------------------------------------- 	
 	//data for CompleteL    
 	TContextMisc  completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusActive;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData(contextId, completeMisc);
	ltsyData.SerialiseL(data);	
	//send request
	mbmsContext.NotifyStatusChange(requestStatus, contextStatus);	
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	
	// wait for NotifyStatusChange
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(completeMisc.iStatus, contextStatus);	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::NotifyStatusChange
	// from LTSY.
 	//-------------------------------------------------------------------------
 	//data for ComleteL    
    completeMisc.iStatus = RPacketContext::EStatusInactive;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData1(contextId, completeMisc);
	data.Close();
	ltsyData1.SerialiseL(data);	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}
	
/**
@SYMTestCaseID BA-CTSY-MCF-PNSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketMbmsContext::NotifyStatusChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketMbmsContext::NotifyStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestNotifyStatusChange0002L()
	{

// This test should test cancellation of NotifyStatusChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext mbmsContext;
	mbmsContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(mbmsContext);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	// data for NotifyStatusChange
	TRequestStatus	requestStatus;
	RPacketContext::TContextStatus  contextStatus; 

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketContext::NotifyStatusChange
 	//------------------------------------------------------------------------- 	
	//data for ComleteL    
 	TContextMisc  completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusInactive;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData(contextId, completeMisc);
	ltsyData.SerialiseL(data);	
	//send request
	mbmsContext.NotifyStatusChange(requestStatus, contextStatus);	
	// send canceling...
	mbmsContext.CancelAsyncRequest(EPacketContextNotifyStatusChange);
	
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	
	// wait for NotifyStatusChange
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
	
	// Wait for completion of iMockLTSY.NotifyTerminated
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(2); // data, this
	
	}	
	
/**
@SYMTestCaseID BA-CTSY-MCF-PNSC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketMbmsContext::NotifyStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketMbmsContext::NotifyStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyMbmsContextFU::TestNotifyStatusChange0003L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data2;
	CleanupClosePushL(data2);

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
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);
	
	// Open first context
	TInfoName contextId;	
	RPacketMbmsContext mbmsContext;
	mbmsContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(mbmsContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketMbmsContext mbmsContext2;
	mbmsContext2.OpenNewContext(packetService2, contextId2);
	CleanupClosePushL(mbmsContext2);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::NotifyStatusChange
 	//-------------------------------------------------------------------------

	// If this API is flow controlled by Etel or if this API is
	// synchronous, remove this test completely.

	// To write this test:
	// If this API has parameters which have different versions, then each
	// client in this test should use a different version of parameter.
	// Otherwise, they should use the same version.
	// All versions of parameters should be used. i.e. If there are three
	// versions of a parameter, there should be three clients, one using
	// each one. If the parameter does not have different versions, use
	// two clients each using the same version.
	
	//data for ComleteL  for first context
 	TContextMisc  completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusInactive;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData(contextId, completeMisc);
	ltsyData.SerialiseL(data);	
	
	//data for ComleteL  for second context
 	TContextMisc  completeMisc2;
    completeMisc2.iStatus = RPacketContext::EStatusSuspended;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData2(contextId2, completeMisc2);
	ltsyData2.SerialiseL(data2);	
		
	// initialize data for first context's NotifyStatusChange 
	TRequestStatus	requestStatus; 
	RPacketContext::TContextStatus  contextStatus; 
	
	// initialize data for second context's NotifyStatusChange 
	TRequestStatus	requestStatus2;
	RPacketContext::TContextStatus  contextStatus2; 	
		
	//send first request
	mbmsContext.NotifyStatusChange(requestStatus, contextStatus);	
	
	//send second request
	mbmsContext2.NotifyStatusChange(requestStatus2, contextStatus2);	
	
	//send completion for first comtext
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);	
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
		
	// wait for first context NotifyStatusChange
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(completeMisc.iStatus, contextStatus);	
	
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	//send completion for second comtext
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data2);	
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
		
	// wait for second context NotifyStatusChange
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(completeMisc2.iStatus, contextStatus2);	
	
	AssertMockLtsyStatusL();
	
	// Done !
	CleanupStack::PopAndDestroy(4);// packetContext2, packetContext, packetService2, packetService
	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, data, data2, this

	}	
