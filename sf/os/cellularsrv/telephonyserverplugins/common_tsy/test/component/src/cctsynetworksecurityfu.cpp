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
// The TEFUnit test suite for NetworkSecurity in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsynetworksecurityfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyNetworkSecurityFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkSecurityFU, TestGetNetworkSecurityLevel0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkSecurityFU, TestGetNetworkSecurityLevel0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkSecurityFU, TestGetCipheringIndicatorStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkSecurityFU, TestGetCipheringIndicatorStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkSecurityFU, TestGetCipheringIndicatorStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkSecurityFU, TestNotifyCipheringIndicatorStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkSecurityFU, TestNotifyNetworkSecurityLevelChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkSecurityFU, TestNotifyNetworkSecurityLevelChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkSecurityFU, TestNotifyNetworkSecurityLevelChange0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-NTWS-MGNSL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetNetworkSecurityLevel
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetNetworkSecurityLevel
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkSecurityFU::TestGetNetworkSecurityLevel0001L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    
    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RMobilePhone::GetNetworkSecurityLevel when result is not cached.
    //-------------------------------------------------------------------------

    // Get default networkSecurity
    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneNetworkSecurity networkSecurity;
    
    iPhone.GetNetworkSecurityLevel(requestStatus, networkSecurity);
    
    User::WaitForRequest(requestStatus);
    
    AssertMockLtsyStatusL();

    ASSERT_EQUALS(KErrNone, requestStatus.Int()); 
    ASSERT_EQUALS(RMobilePhone::ECipheringOff, networkSecurity);


    // change and get new networkSecurity
    TRequestStatus mockLtsyStatus; 
    iMockLTSY.NotifyTerminated(mockLtsyStatus); 

    // Prepare data for passing to CTSY through MockLTSY using RMobilePhone::NotifyNetworkSecurityLevelChange 
    networkSecurity = RMobilePhone::ECipheringGSM;
    
    TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSecurity> networkSecData(networkSecurity); 
    networkSecData.SerialiseL(data); 
        
    // set up necessary data in CTSY using NotifyNetworkSecurityLevelChange 
    iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkSecurityLevelChange, KErrNone, data);         

    User::WaitForRequest(mockLtsyStatus); 
    
    iPhone.GetNetworkSecurityLevel(requestStatus, networkSecurity);
    User::WaitForRequest(requestStatus); 
    
    AssertMockLtsyStatusL(); 
    
    ASSERT_EQUALS(KErrNone, requestStatus.Int()); 
    ASSERT_EQUALS(RMobilePhone::ECipheringGSM, networkSecurity) 


    //-------------------------------------------------------------------------
    // TEST D: RMobilePhone::GetNetworkSecurityLevel again, this time CTSY
    // will get result from the cache.
    //-------------------------------------------------------------------------

    iPhone.GetNetworkSecurityLevel(requestStatus, networkSecurity);
    
    User::WaitForRequest(requestStatus);
    
    AssertMockLtsyStatusL();

    ASSERT_EQUALS(KErrNone, requestStatus.Int()); 
    ASSERT_EQUALS(RMobilePhone::ECipheringGSM, networkSecurity);

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2, this); // data, this
    }



/**
@SYMTestCaseID BA-CTSY-NTWS-MGNSL-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetNetworkSecurityLevel
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetNetworkSecurityLevel
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkSecurityFU::TestGetNetworkSecurityLevel0004L()
    {
                        
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(iTelServer,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);

    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting RMobilePhone::GetNetworkSecurityLevel
    //-------------------------------------------------------------------------

    // setting ECipheringGSM
    
    TRequestStatus mockLtsyStatus; 
    iMockLTSY.NotifyTerminated(mockLtsyStatus); 

    // Prepare data for passing to CTSY through MockLTSY using RMobilePhone::NotifyNetworkSecurityLevelChange 
    RMobilePhone::TMobilePhoneNetworkSecurity networkSecurity = RMobilePhone::ECipheringGSM;
    
    TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSecurity> networkSecData(networkSecurity); 
    networkSecData.SerialiseL(data); 
        
    // set up necessary data in CTSY using NotifyNetworkSecurityLevelChange 
    iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkSecurityLevelChange, KErrNone, data);         

    User::WaitForRequest(mockLtsyStatus); 

    AssertMockLtsyStatusL();

    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int()); 


    // getting
    TRequestStatus requestStatus1;
    RMobilePhone::TMobilePhoneNetworkSecurity networkSecurity1;
    iPhone.GetNetworkSecurityLevel(requestStatus1, networkSecurity1);

    TRequestStatus requestStatus2;
    RMobilePhone::TMobilePhoneNetworkSecurity networkSecurity2;
    phone2.GetNetworkSecurityLevel(requestStatus2, networkSecurity2);
    
    User::WaitForRequest(requestStatus1);

    ASSERT_EQUALS(KErrNone, requestStatus1.Int());
    ASSERT_TRUE(networkSecurity1 == RMobilePhone::ECipheringGSM);

    User::WaitForRequest(requestStatus2);

    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(networkSecurity2 == RMobilePhone::ECipheringGSM);
    

    AssertMockLtsyStatusL();

    // Done !
    CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

    }




/**
@SYMTestCaseID BA-CTSY-NTWS-MGCS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCipheringIndicatorStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCipheringIndicatorStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkSecurityFU::TestGetCipheringIndicatorStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);


	const RMobilePhone::TMobileCallCipheringIndicator KStatus(RMobilePhone::ECipheringDisplayOn);

    //
    // IMPORTANT NOTE!!!
    // Test route where CipheringIndicatorForcedOff() set to TRUE can't be checked
    // because it depends on file z:\private\101f7989\operatorVariants.ini
    // which can't be changed in automatic test scripts.
    //
    
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
 	
	TRequestStatus requestStatus;
	RMobilePhone::TMobileCallCipheringIndicator indiStatus;

	// Queue response
	iMockLTSY.ExpectL(EMobilePhoneGetCipheringIndicatorStatus, KErrNotSupported);
	// Post request
	iPhone.GetCipheringIndicatorStatus(requestStatus, indiStatus);
	
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());


	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetCipheringIndicatorStatus);
	
	TMockLtsyData1<RMobilePhone::TMobileCallCipheringIndicator> indiDataB(indiStatus);
	indiDataB.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetCipheringIndicatorStatus, KErrGeneral, data);
	
	iPhone.GetCipheringIndicatorStatus(requestStatus, indiStatus);
	
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());


 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetCipheringIndicatorStatus when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetCipheringIndicatorStatus);
	
	indiStatus = KStatus;
	TMockLtsyData1<RMobilePhone::TMobileCallCipheringIndicator> indiDataC(indiStatus);
	data.Close();
	indiDataC.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetCipheringIndicatorStatus, KErrNone, data);

	iPhone.GetCipheringIndicatorStatus(requestStatus, indiStatus);
	
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE(indiStatus == KStatus);
	
    //TEST E
	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus); 
    iMockLTSY.CompleteL(EMobilePhoneGetCipheringIndicatorStatus, KErrNone, data);         
    User::WaitForRequest(mockLtsyStatus); 
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWS-MGCS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetCipheringIndicatorStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetCipheringIndicatorStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkSecurityFU::TestGetCipheringIndicatorStatus0002L()
	{

// This test should test cancellation of GetCipheringIndicatorStatus
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetCipheringIndicatorStatus
 	//-------------------------------------------------------------------------
	const RMobilePhone::TMobileCallCipheringIndicator KStatus(RMobilePhone::ECipheringDisplayOn);
 	
	// Expect call down to LTSY for this IPC
	iMockLTSY.ExpectL(EMobilePhoneGetCipheringIndicatorStatus);

	// Queue response to be delayed to give time for cancel to come through
	// Return data is needed because CTSY does not pass the cancel to LTSY

	RMobilePhone::TMobileCallCipheringIndicator indiStatus(KStatus);
	TMockLtsyData1<RMobilePhone::TMobileCallCipheringIndicator> indiData(indiStatus);
	indiData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetCipheringIndicatorStatus, KErrNone, data, 10);

	TRequestStatus requestStatus;
	RMobilePhone::TMobileCallCipheringIndicator indi;
	
	iPhone.GetCipheringIndicatorStatus(requestStatus, indi);
	
	iPhone.CancelAsyncRequest(EMobilePhoneGetCipheringIndicatorStatus);
	
	User::WaitForRequest(requestStatus);
    
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWS-MGCS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetCipheringIndicatorStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetCipheringIndicatorStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkSecurityFU::TestGetCipheringIndicatorStatus0004L()
	{				
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetCipheringIndicatorStatus
 	//-------------------------------------------------------------------------

	const RMobilePhone::TMobileCallCipheringIndicator KStatus(RMobilePhone::ECipheringDisplayOn);
 	
	// Expect call down to LTSY for this IPC
	iMockLTSY.ExpectL(EMobilePhoneGetCipheringIndicatorStatus);

	RMobilePhone::TMobileCallCipheringIndicator indiStatus(KStatus);
	TMockLtsyData1<RMobilePhone::TMobileCallCipheringIndicator> indiData(indiStatus);
	indiData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetCipheringIndicatorStatus, KErrNone, data);

	// Expect another call down to LTSY for the second client
	iMockLTSY.ExpectL(EMobilePhoneGetCipheringIndicatorStatus);

	iMockLTSY.CompleteL(EMobilePhoneGetCipheringIndicatorStatus, KErrNone, data);


	// First client makes request
	TRequestStatus requestStatus1;
	RMobilePhone::TMobileCallCipheringIndicator indi1;
	
	iPhone.GetCipheringIndicatorStatus(requestStatus1, indi1);


	TRequestStatus requestStatus2;
	RMobilePhone::TMobileCallCipheringIndicator indi2;
	
	phone2.GetCipheringIndicatorStatus(requestStatus2, indi2);

	// Check result for first client
	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());
	ASSERT_TRUE(indi1 == KStatus);

	// Check result for second client
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_TRUE(indi2 == KStatus);

	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}




/**
@SYMTestCaseID BA-CTSY-NTWS-MNCIS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCipheringIndicatorStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCipheringIndicatorStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkSecurityFU::TestNotifyCipheringIndicatorStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RMobilePhone::TMobileCallCipheringIndicator indi;
    TRequestStatus requestStatus;
    
    iPhone.NotifyCipheringIndicatorStatus(requestStatus, indi);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWS-MNNSLC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyNetworkSecurityLevelChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyNetworkSecurityLevelChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkSecurityFU::TestNotifyNetworkSecurityLevelChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);


	// TEST C: Successful completion request of
	// RMobilePhone::NotifyNetworkSecurityLevelChange when result is not cached.

    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneNetworkSecurity networkSec;    
    iPhone.NotifyNetworkSecurityLevelChange(requestStatus, networkSec);

    RMobilePhone::TMobilePhoneNetworkSecurity networkSecurity = RMobilePhone::ECipheringGSM;   
    TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSecurity> networkSecData(networkSecurity); 
    networkSecData.SerialiseL(data);         
    iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkSecurityLevelChange, KErrNone, data);  

    User::WaitForRequest(requestStatus); 
    
    ASSERT_EQUALS(KErrNone, requestStatus.Int()); 
    ASSERT_EQUALS(RMobilePhone::ECipheringGSM, networkSec); 
        
	AssertMockLtsyStatusL();
	
    iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkSecurityLevelChange); 		

    // notify without change
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkSecurityLevelChange, KErrNone, data);  
    User::WaitForRequest(mockLtsyStatus);    
	       

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWS-MNNSLC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyNetworkSecurityLevelChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyNetworkSecurityLevelChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkSecurityFU::TestNotifyNetworkSecurityLevelChange0002L()
	{

// This test should test cancellation of NotifyNetworkSecurityLevelChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

   	OpenPhoneL();


	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyNetworkSecurityLevelChange
 	//-------------------------------------------------------------------------
 	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneNetworkSecurity networkSec;    
    iPhone.NotifyNetworkSecurityLevelChange(requestStatus, networkSec);

    RMobilePhone::TMobilePhoneNetworkSecurity networkSecurity = RMobilePhone::ECipheringGSM;   
    TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSecurity> networkSecData(networkSecurity); 
    networkSecData.SerialiseL(data);         
    
    iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkSecurityLevelChange); 	

    iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkSecurityLevelChange, KErrNone, data, 10);         

    User::WaitForRequest(requestStatus); 
      
    // This defect was fixed. For more information see defect description   
    //ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 20401);
    
    ASSERT_EQUALS(KErrCancel, requestStatus.Int()); 
 	
	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL(); 	
    
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWS-MNNSLC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyNetworkSecurityLevelChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyNetworkSecurityLevelChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkSecurityFU::TestNotifyNetworkSecurityLevelChange0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyNetworkSecurityLevelChange
 	//-------------------------------------------------------------------------
    TRequestStatus requestStatus1;
    RMobilePhone::TMobilePhoneNetworkSecurity networkSec1;    
    iPhone.NotifyNetworkSecurityLevelChange(requestStatus1, networkSec1);

    TRequestStatus requestStatus2;
    RMobilePhone::TMobilePhoneNetworkSecurity networkSec2;    
    phone2.NotifyNetworkSecurityLevelChange(requestStatus2, networkSec2);

    RMobilePhone::TMobilePhoneNetworkSecurity networkSecurity = RMobilePhone::ECipheringGSM;   
    TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSecurity> networkSecData(networkSecurity); 
    networkSecData.SerialiseL(data);         
    iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkSecurityLevelChange, KErrNone, data);         
    
    User::WaitForRequest(requestStatus1); 
    
    ASSERT_EQUALS(KErrNone, requestStatus1.Int()); 
    ASSERT_EQUALS(RMobilePhone::ECipheringGSM, networkSec1); 

    User::WaitForRequest(requestStatus2); 
    
    ASSERT_EQUALS(KErrNone, requestStatus2.Int()); 
    ASSERT_EQUALS(RMobilePhone::ECipheringGSM, networkSec2); 
    
    AssertMockLtsyStatusL();  
    
    // This defect was fixed. For more information see defect description    
    //ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 20401);   
            
	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}



