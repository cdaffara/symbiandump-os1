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
 @file The TEFUnit test suite for SetUpEventList in the SAT.
*/

#include "ccsatsetupeventlistfu.h"
#include <satcs.h>                  // Etel SAT IPC definitions
#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <test/mockltsyindicatorids.h>


static const TUint8 KDelay = 10;

static const TUint8 KPcmdNumber = 1;
//channel 1, Link established or Packet data service activated	
static const TUint8 KChannelStatusByte1 = 0x81;

static const TInt KSubAddressLength = 100;

// these constants are used to show that they are NOT supported by TSY
static const TUint8 KDisplayParamsChanges_missing	              = 0x0C;
static const TUint8 KLocalConnection_missing               	  = 0x0D;

//ETLV_CommandNumber
static const TUint8 KTestCommandNumber = 0x01;
//ETLV_CommandQualifier	
static const TUint8 KTestCommandQualifier = 0x00;



CTestSuite* CCSatSetUpEventListFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestNotifySetUpEventListPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestNotifySetUpEventListPCmd0001bL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestNotifySetUpEventListPCmd0001cL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestNotifySetUpEventListPCmd0001dL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestNotifySetUpEventListPCmd0001eL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestNotifySetUpEventListPCmd0001fL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestNotifySetUpEventListPCmd0001gL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestNotifySetUpEventListPCmd0001hL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestNotifySetUpEventListPCmd0001iL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestNotifySetUpEventListPCmd0001jL);
	
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestNotifySetUpEventListPCmd0002L);	
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestNotifySetUpEventListPCmd0004L);	
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpEventListFU, TestEventDownload0001L);	
	
	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-SUEL-SNSUELPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpEventListPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestNotifySetUpEventListPCmd0001L()
	{	
	static const TUint8 events[] = {KEventUserActivity};
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));	
	OpenPhoneL();
	OpenSatL();
	
	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifySetUpEventListPCmd 
 	//-------------------------------------------------------------------------
	
	// see TestNotifySetUpEventListPCmd0001bL - TestNotifySetUpEventListPCmd0001kL
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifySetUpEventListPCmd
	// from LTSY.
 	//-------------------------------------------------------------------------
	// prepare TLV for
	TTlv eventTlv;	
	PrepareTlv( eventTlv, events, sizeof(events)/sizeof(TUint8));	
	const TDesC8& tlv = eventTlv.End();			
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// post Complete with KErrNone	
	CompleteMockDispatchSatPcmdL(tlv, KErrNone);
	
	// as TerminalResponse is called from CTSY
	// Prepare here ExpectL for it
	TBuf8<1> resultAddInfo;
	resultAddInfo.Append(RSat::KNoSpecificMeProblem); 	
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KMeUnableToProcessCmd, resultAddInfo);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	
	//Prepare data for ExpectL needed by NotifySetUpEventListPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSetUpEventList);
				
	// Post Notify itself
	RSat::TSetUpEventListV1 eventList;
	RSat::TSetUpEventListV1Pckg eventListPck(eventList);
	iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
			
	// post Complete with KErrUnknown, the request must be left pending
	CompleteMockDispatchSatPcmdL(tlv, KErrUnknown);
	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// post next Complete with KErrNone to complete request successfully
	CompleteMockDispatchSatPcmdL(tlv, KErrNone);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// ---TERMINAL RESPONSE---
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KSuccess, KNullDesC8);			
	// call TerminalRsp
	RSat::TSetUpEventListRspV1 resp;
	RSat::TSetUpEventListRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this

	}


/**
@SYMTestCaseID BA-CSAT-SUEL-SNSUELPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifySetUpEventListPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestNotifySetUpEventListPCmd0002L()
	{
	static const TUint8 events[] = {KEventUserActivity};
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifySetUpEventListPCmd
 	//-------------------------------------------------------------------------
	
	//Prepare data for ExpectL needed by NotifySetUpEventListPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSetUpEventList);
	
	TRequestStatus requestStatus;					
	// Post Notify itself
	RSat::TSetUpEventListV1 eventList;
	RSat::TSetUpEventListV1Pckg eventListPck(eventList);
	iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);
	// cancel Notify
	iSat.CancelAsyncRequest(ESatNotifySetUpEventListPCmd);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());		
		
	// Now post Complete after cancel
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	 
	TTlv eventTlv;	
	PrepareTlv( eventTlv, events, sizeof(events)/sizeof(TUint8));	
	CompleteMockDispatchSatPcmdL(eventTlv.End(), KErrNone);
	// as TerminalResponse is called from CTSY
	// Prepare here ExpectL for it
	TBuf8<1> resultAddInfo;
	resultAddInfo.Append(RSat::KNoSpecificMeProblem); 	
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KMeUnableToProcessCmd, resultAddInfo);	

	// wait for CompleteMockDispatchSatPcmdL
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // this
	}


/**
@SYMTestCaseID BA-CSAT-SUEL-SNSUELPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifySetUpEventListPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestNotifySetUpEventListPCmd0004L()
	{
	static const TUint8 events[] = {KEventUserActivity};
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);		
	RSat sat2;	
	User::LeaveIfError(sat2.Open(phone2));
	CleanupClosePushL(sat2);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	//Prepare data for ExpectL needed by NotifySetUpEventListPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSetUpEventList);
			
	// Post first Notify 
	RSat::TSetUpEventListV1 eventList;
	RSat::TSetUpEventListV1Pckg eventListPck(eventList);
	iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);
	
	// Post second Notify 
	RSat::TSetUpEventListV1 eventList2;
	RSat::TSetUpEventListV1Pckg eventListPck2(eventList2);
	sat2.NotifySetUpEventListPCmd(requestStatus2, eventListPck2);
		
	// prepare TLV for the first client
	TTlv eventTlv;	
	PrepareTlv( eventTlv, events, sizeof(events)/sizeof(TUint8));
	
	// post Complete for
	CompleteMockDispatchSatPcmdL(eventTlv.End(), KErrNone);		    
			    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS((TUint)RSat::KUserActivity, eventList.iEvents);	
	ASSERT_EQUALS(RSat::EUpdateEventList, eventList.iType);
	
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS((TUint)RSat::KUserActivity, eventList2.iEvents);	
	ASSERT_EQUALS(RSat::EUpdateEventList, eventList2.iType);
	
	// ---TERMINAL RESPONSE from the first client---
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KSuccess, KNullDesC8);			
	// call TerminalRsp
	RSat::TSetUpEventListRspV1 resp;
	RSat::TSetUpEventListRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);
	AssertMockLtsyStatusL();	
	// ---ENVELOPE: EVENT DOWNLOAD from the first client---
	PrepareAndCallEventDownloadL(KEventUserActivity);
		
	// ---TERMINAL RESPONSE from the second client---
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KSuccess, KNullDesC8);		
	sat2.TerminalRsp(requestStatus2, RSat::ESetUpEventList, respPckg);
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	AssertMockLtsyStatusL();		
	// ---ENVELOPE: EVENT DOWNLOAD from the second client---
	PrepareAndCallEventDownloadL(KEventUserActivity, &sat2, EFalse, KErrNone);	
	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(4, this); // ...,  this
	}


/**
@SYMTestCaseID BA-CSAT-SUEL-SNSUELPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Expected Sequence 1.1 (SET UP EVENT LIST, User Activity)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestNotifySetUpEventListPCmd0001bL()
	{
	static const TUint8 events[] = {KEventUserActivity};
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhoneL();
	OpenSatL();
	
	TRequestStatus requestStatus;	
	
 	//-------------------------------------------------------------------------
	// Expected Sequence 1.1 (SET UP EVENT LIST, User Activity)
	// see ETSI TS 102 384 V6.5.0 
 	//-------------------------------------------------------------------------
	
	//Prepare data for ExpectL needed by NotifySetUpEventListPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSetUpEventList);
			
	// Post Notify itself
	RSat::TSetUpEventListV1 eventList;
	RSat::TSetUpEventListV1Pckg eventListPck(eventList);
	iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);
		
	// prepare TLV for
	TTlv eventTlv;
	PrepareTlv( eventTlv, events, sizeof(events)/sizeof(TUint8));	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(eventTlv.End(), KErrNone);		    
			    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS((TUint)RSat::KUserActivity, eventList.iEvents);	
	ASSERT_EQUALS(RSat::EUpdateEventList, eventList.iType);
	
	// ---TERMINAL RESPONSE---
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KSuccess, KNullDesC8);			
	// call TerminalRsp
	RSat::TSetUpEventListRspV1 resp;
	RSat::TSetUpEventListRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);
	AssertMockLtsyStatusL();
	
	// ---ENVELOPE: EVENT DOWNLOAD---
	PrepareAndCallEventDownloadL(KEventUserActivity);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); //  this
	} 


/**
@SYMTestCaseID BA-CSAT-SUEL-SNSUELPC-0001c
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test for KEventIdleScreenAvailable and  KEventLanguageSelection events
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestNotifySetUpEventListPCmd0001cL()
	{	
	static const TUint8 events[] = {KEventIdleScreenAvailable, KEventLanguageSelection};
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhoneL();
	OpenSatL();
	
	TRequestStatus requestStatus;	
	
 	//-------------------------------------------------------------------------
	// Test for KEventIdleScreenAvailable and  KEventLanguageSelection events
 	//-------------------------------------------------------------------------
	
	//Prepare data for ExpectL needed by NotifySetUpEventListPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSetUpEventList);
			
	// Post Notify itself
	RSat::TSetUpEventListV1 eventList;
	RSat::TSetUpEventListV1Pckg eventListPck(eventList);
	iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);
		
	// prepare TLV for
	TTlv eventTlv;	
	PrepareTlv( eventTlv, events, sizeof(events)/sizeof(TUint8));	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(eventTlv.End(), KErrNone);		    
			    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	// create constant to compare with data got in NotifySetUpEventListPCmd 
	TUint eventsFlag = RSat::KIdleScreenAvailable + RSat::KLanguageSelection; // as we asked for  KEventIdleScreenAvailable and KEventLanguageSelection
	ASSERT_EQUALS(eventsFlag, eventList.iEvents);	
	ASSERT_EQUALS(RSat::EUpdateEventList, eventList.iType);
	
	// ---TERMINAL RESPONSE---
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KSuccess, KNullDesC8);			
	// call TerminalRsp
	RSat::TSetUpEventListRspV1 resp;
	RSat::TSetUpEventListRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);
	AssertMockLtsyStatusL();
	
	// ---ENVELOPE: KEventLanguageSelection---
	PrepareAndCallEventDownloadL(KEventLanguageSelection);
	// ---ENVELOPE: KEventIdleScreenAvailable---
	PrepareAndCallEventDownloadL(KEventIdleScreenAvailable);	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); //  this
	}



/**
@SYMTestCaseID BA-CSAT-SUEL-SNSUELPC-0001d
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test for KEventBrowserTermination, KEventDataAvailable and  KEventChannelStatus events
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestNotifySetUpEventListPCmd0001dL()
	{	
	static const TUint8 events[] = {KEventBrowserTermination, KEventDataAvailable, KEventChannelStatus};
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhoneL();
	OpenSatL();
	
	TRequestStatus requestStatus;	
	
 	//-------------------------------------------------------------------------
	// Test for KEventBrowserTermination, KEventDataAvailable and  KEventChannelStatus events
 	//-------------------------------------------------------------------------
	
	//Prepare data for ExpectL needed by NotifySetUpEventListPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSetUpEventList);
			
	// Post Notify itself
	RSat::TSetUpEventListV1 eventList;
	RSat::TSetUpEventListV1Pckg eventListPck(eventList);
	iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);
		
	// prepare TLV for
	TTlv eventTlv;	
	PrepareTlv( eventTlv, events, sizeof(events)/sizeof(TUint8));	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(eventTlv.End(), KErrNone);		    
			    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	TUint eventsFlag = RSat::KBrowserTermination + RSat::KDataAvailable + RSat::KChannelStatus; 
	ASSERT_EQUALS(eventsFlag, eventList.iEvents);	
	ASSERT_EQUALS(RSat::EUpdateEventList, eventList.iType);
	
	// ---TERMINAL RESPONSE---
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KSuccess, KNullDesC8);			
	// call TerminalRsp
	RSat::TSetUpEventListRspV1 resp;
	RSat::TSetUpEventListRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);
	AssertMockLtsyStatusL();
	
	// ---ENVELOPE: KEventBrowserTermination---
	PrepareAndCallEventDownloadL(KEventBrowserTermination);
	// ---ENVELOPE: KEventDataAvailable---
	PrepareAndCallEventDownloadL(KEventDataAvailable);	
	// ---ENVELOPE: KEventChannelStatus---
	PrepareAndCallEventDownloadL(KEventChannelStatus);	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); //  this
	}

/**
@SYMTestCaseID BA-CSAT-SUEL-SNSUELPC-0001e
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test for KEventMTCall, KEventCallConnected, KEventCallDisconnected, KEventLocationStatus and  KEventAccessTechnologyChange events
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestNotifySetUpEventListPCmd0001eL()
	{	
	static const TUint8 eventListComplete[] = {	KEventMTCall, 
												KEventCallConnected, 
												KEventCallDisconnected, 
												KEventLocationStatus, 
												KEventAccessTechnologyChange};
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhoneL();
	OpenSatL();
	
	TRequestStatus requestStatus;	
	
 	//-------------------------------------------------------------------------
	// Test for KEventMTCall, KEventCallConnected, KEventCallDisconnected, KEventLocationStatus, 
	// and  KEventAccessTechnologyChange events.  TSY is responsible for these events
	// and they shouldn't be reported to ETel
 	//-------------------------------------------------------------------------
	
	//Prepare data for ExpectL needed by NotifySetUpEventListPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSetUpEventList);
			
	// Post Notify itself
	RSat::TSetUpEventListV1 eventList;
	RSat::TSetUpEventListV1Pckg eventListPck(eventList);
	iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);
		
	// prepare TLV for
	TTlv eventTlv;	
	PrepareTlv( eventTlv, eventListComplete, sizeof(eventListComplete)/sizeof(TUint8));	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(eventTlv.End(), KErrNone);		    
			    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	// user receives empty event list
	TUint eventsFlag = 0; 
	ASSERT_EQUALS(eventsFlag, eventList.iEvents);	
	ASSERT_EQUALS(RSat::EUpdateEventList, eventList.iType);	
	
	// ---TERMINAL RESPONSE---
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KSuccess, KNullDesC8);			
	// call TerminalRsp
	RSat::TSetUpEventListRspV1 resp;
	RSat::TSetUpEventListRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); //  this
	} 

/**
@SYMTestCaseID BA-CSAT-SUEL-SNSUELPC-0001f
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Tests for all possible flow of CSatNotifySetUpEventList::TerminalResponseL
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestNotifySetUpEventListPCmd0001fL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	// test all possible parameters for CSatNotifySendUssd::CreateTerminalRespL()
	
	DoTestTerminalResponseL();	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); //this
	}


/**
@SYMTestCaseID BA-CSAT-SUEL-SNSUELPC-0001g
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test send ENVELOPE for events handled by TSY (just for coverage!)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestNotifySetUpEventListPCmd0001gL()
	{	
	static const TUint8 eventListComplete[] = {	KEventMTCall, 
													KEventCallConnected,
													KEventCallDisconnected, 
													KEventLocationStatus, 
													KEventAccessTechnologyChange};
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhoneL();
	OpenSatL();
	
	TRequestStatus requestStatus;	
 	//-------------------------------------------------------------------------
	// Test ENVELOPE for events handled by TSY:
	//KEventMTCall, KEventCallConnected, KEventCallDisconnected, KEventLocationStatus, 
	//and  KEventAccessTechnologyChange. 
	// (just for coverage!)
 	//-------------------------------------------------------------------------
	
	//Prepare data for ExpectL needed by NotifySetUpEventListPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSetUpEventList);
			
	// Post Notify itself
	RSat::TSetUpEventListV1 eventList;
	RSat::TSetUpEventListV1Pckg eventListPck(eventList);
	iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);
	
	// prepare TLV for
	TTlv eventTlv;	
	PrepareTlv( eventTlv, eventListComplete, sizeof(eventListComplete)/sizeof(TUint8));	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(eventTlv.End(), KErrNone);		    
			    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS((TUint)0, eventList.iEvents);	
	ASSERT_EQUALS(RSat::EUpdateEventList, eventList.iType);
	
	// ---TERMINAL RESPONSE---
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KSuccess, KNullDesC8);			
	// call TerminalRsp
	RSat::TSetUpEventListRspV1 resp;
	RSat::TSetUpEventListRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);
	AssertMockLtsyStatusL();
		
	// ENVELOPE for KEventMTCall
	EnvelopeFromTsyL(KEventMTCall);
	
	// ENVELOPE for KEventCallConnected	
	EnvelopeFromTsyL(KEventCallConnected);
	
	// ENVELOPE for KEventCallDisconnected
	EnvelopeFromTsyL(KEventCallDisconnected);
	
	// ENVELOPE for KEventLocationStatus
	EnvelopeFromTsyL(KEventLocationStatus);
	
	// ENVELOPE for KEventAccessTechnologyChange
	EnvelopeFromTsyL(KEventAccessTechnologyChange);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); //  this
	} 

/**
@SYMTestCaseID BA-CSAT-SUEL-SNSUELPC-0001h
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test send Unsolicited ENVELOPE  for events handled by TSY (just for coverage!)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestNotifySetUpEventListPCmd0001hL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhoneL();
	OpenSatL();
	
	TRequestStatus requestStatus;	
 	//-------------------------------------------------------------------------
	// Test Unsolicited ENVELOPE for events handled by TSY:
	//KEventMTCall, KEventCallConnected, KEventCallDisconnected, KEventLocationStatus, 
	//and  KEventAccessTechnologyChange. 
	// (just for coverage!)
 	//-------------------------------------------------------------------------
	
	// Unsolicited ENVELOPE for KEventMTCall
	EnvelopeFromTsyL(KEventMTCall, EFalse);
	
	// Unsolicited ENVELOPE for KEventCallConnected	
	EnvelopeFromTsyL(KEventCallConnected, EFalse);
	
	// Unsolicited ENVELOPE for KEventCallDisconnected
	EnvelopeFromTsyL(KEventCallDisconnected, EFalse);
	
	// Unsolicited ENVELOPE for KEventLocationStatus
	EnvelopeFromTsyL(KEventLocationStatus, EFalse);
	
	// Unsolicited ENVELOPE for KEventAccessTechnologyChange
	EnvelopeFromTsyL(KEventAccessTechnologyChange, EFalse);
	
	// Unsolicited ENVELOPE for KEventUserActivity
	EnvelopeFromTsyL(KEventUserActivity, EFalse);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); //  this
	} 

/**
@SYMTestCaseID BA-CSAT-SUEL-SNSUELPC-0001i
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test send Unsolicited ENVELOPE and  failure to dispatch request to LTSY for events handled by ETEL (just for coverage!)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestNotifySetUpEventListPCmd0001iL()
	{	
	static const TUint8 events[] = {KEventUserActivity, 
									KEventIdleScreenAvailable, 
									KEventBrowserTermination,
									KEventLanguageSelection,						
									KEventDataAvailable, 
									KEventChannelStatus};
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	// ---ENVELOPE:  KEventUserActivity ---
	PrepareAndCallEventDownloadL(KEventUserActivity, NULL, ETrue, KErrNone);	
	
	// ---ENVELOPE:  KEventIdleScreenAvailable ---
	PrepareAndCallEventDownloadL(KEventIdleScreenAvailable, NULL, ETrue, KErrNone);
	
	// ---ENVELOPE:  KEventBrowserTermination ---
	PrepareAndCallEventDownloadL(KEventBrowserTermination, NULL, ETrue, KErrNone);
	
	// ---ENVELOPE:  KEventLanguageSelection ---
	PrepareAndCallEventDownloadL(KEventLanguageSelection, NULL, ETrue, KErrNone);
	
	// ---ENVELOPE:  KEventDataAvailable ---
	PrepareAndCallEventDownloadL(KEventDataAvailable, NULL, ETrue, KErrNone);
	
	// ---ENVELOPE:  KEventChannelStatus ---
	PrepareAndCallEventDownloadL(KEventChannelStatus, NULL, ETrue, KErrNone);
	
	
	//-------------------------------------------------------------------------	
	// Failure to dispatch request to LTSY
	// Like "Test A" of 0001 tests for ENVELOPE command
	//-------------------------------------------------------------------------
	//Prepare data for ExpectL needed by NotifySetUpEventListPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSetUpEventList);
	
	TRequestStatus requestStatus;			
	// Post Notify itself
	RSat::TSetUpEventListV1 eventList;
	RSat::TSetUpEventListV1Pckg eventListPck(eventList);
	iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);
		
	// prepare TLV for
	TTlv eventTlv;
	PrepareTlv( eventTlv, events, sizeof(events)/sizeof(TUint8));
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(eventTlv.End(), KErrNone);		    
			    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	TUint eventsFlag = 	RSat::KUserActivity + RSat::KIdleScreenAvailable + 
						RSat::KBrowserTermination + RSat::KLanguageSelection + 
						RSat::KDataAvailable + RSat::KChannelStatus;
	ASSERT_EQUALS(eventsFlag, eventList.iEvents);	
	ASSERT_EQUALS(RSat::EUpdateEventList, eventList.iType);
	
	// ---TERMINAL RESPONSE---
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KSuccess, KNullDesC8);			
	// call TerminalRsp
	RSat::TSetUpEventListRspV1 resp;
	RSat::TSetUpEventListRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);
	AssertMockLtsyStatusL();
	
	// ---ENVELOPE: KEventUserActivity---
	PrepareAndCallEventDownloadL(KEventUserActivity, NULL, EFalse, KErrUnknown);
	
	// ---ENVELOPE: KEventIdleScreenAvailable---
	PrepareAndCallEventDownloadL(KEventIdleScreenAvailable, NULL, EFalse, KErrUnknown);
		
	// ---ENVELOPE: KEventBrowserTermination---
	PrepareAndCallEventDownloadL(KEventBrowserTermination, NULL, EFalse, KErrUnknown);
			
	// ---ENVELOPE: KEventLanguageSelection---
	PrepareAndCallEventDownloadL(KEventLanguageSelection, NULL, EFalse, KErrUnknown);
				
	// ---ENVELOPE: KEventDataAvailable---
	PrepareAndCallEventDownloadL(KEventDataAvailable, NULL, EFalse, KErrUnknown);
					
	// ---ENVELOPE: KEventChannelStatus---
	PrepareAndCallEventDownloadL(KEventChannelStatus, NULL, EFalse, KErrUnknown);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); //this
	} 



/**
@SYMTestCaseID BA-CSAT-SUEL-SNSUELPC-0001j
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Some tests for coverage of CSatNotifySetUpEventList::CompleteNotifyL()
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpEventListPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestNotifySetUpEventListPCmd0001jL()
	{	
	static const  TUint8 eventListComplete[] = {KEventUserActivity, KEventUserActivity};
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhoneL();
	OpenSatL();
	
	TRequestStatus requestStatus;	
	TRequestStatus mockStatus;
	
 	
	//Prepare data for ExpectL needed by NotifySetUpEventListPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSetUpEventList);			
	// Post Notify itself
	RSat::TSetUpEventListV1 eventList;
	RSat::TSetUpEventListV1Pckg eventListPck(eventList);
	iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);		
	
	//-------------------------------------------------------------------------
	//Pass TLV-buffer from LTSY to TSY without KTlvEventListTag tag
	//-------------------------------------------------------------------------	
	TTlv eventTlv;
	eventTlv.Begin(KBerTlvProactiveSimCommandTag);
	eventTlv.AddTag(KTlvCommandDetailsTag);
	eventTlv.AddByte(KTestCommandNumber);//ETLV_CommandNumber
	eventTlv.AddByte(KSetUpEventList); //ETLV_TypeOfCommand
	eventTlv.AddByte(KTestCommandQualifier); //ETLV_CommandQualifier	
	eventTlv.AddTag(KTlvDeviceIdentityTag); 
	eventTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	eventTlv.AddByte(KMe);	
		
	iMockLTSY.NotifyTerminated(mockStatus);
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(eventTlv.End(), KErrNone);		
	// as TerminalResponse is called from CTSY (in this case)
	// Prepare here ExpectL for it
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KErrorRequiredValuesMissing, KNullDesC8);	
	// wait for complete of iMockLTSY.NotifyTerminated(mockStatus) because TSY doesn't inform client 
	// about errors from LTSY
	User::WaitForRequest(mockStatus);
	ASSERT_EQUALS(KErrNone, mockStatus.Int());
	AssertMockLtsyStatusL();	
	
	//-------------------------------------------------------------------------
	//Pass event from LTSY to TSY with invalid event code
	//-------------------------------------------------------------------------	
	// prepare TLV with invalid event code
	TUint8 wrongEventList[] = {0xFF};
	PrepareTlv( eventTlv, wrongEventList, sizeof(wrongEventList)/sizeof(TUint8));
	
	iMockLTSY.NotifyTerminated(mockStatus);
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(eventTlv.End(), KErrNone);
	// as TerminalResponse is called from CTSY (in this case)
	// Prepare here ExpectL for it
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KCmdBeyondMeCapabilities, KNullDesC8);
	// wait for complete of iMockLTSY.NotifyTerminated(mockStatus) because TSY doesn't inform client 
	// about errors from LTSY
	User::WaitForRequest(mockStatus);
	ASSERT_EQUALS(KErrNone, mockStatus.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	//Pass TLV-buffer from LTSY to TSY without EvenList (remove all events)
	//-------------------------------------------------------------------------	
	PrepareTlv( eventTlv, NULL, 0);
	eventTlv.AddTag(KTlvEventListTag); 
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(eventTlv.End(), KErrNone);		
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS((TUint) 0, eventList.iEvents);	
	ASSERT_EQUALS(RSat::ERemoveExistingEventList, eventList.iType);
	
	// ---TERMINAL RESPONSE---
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KSuccess, KNullDesC8);			
	// call TerminalRsp
	RSat::TSetUpEventListRspV1 resp;
	RSat::TSetUpEventListRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);
	AssertMockLtsyStatusL();		
		
	//-------------------------------------------------------------------------
	//Pass event list from LTSY to TSY with two identical events 
	//-------------------------------------------------------------------------	
	// Post Notify again
	PrepareMockLDataWithExpL(KSetUpEventList);			
	iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);	
	// prepare TLV
	PrepareTlv( eventTlv, eventListComplete, sizeof(eventListComplete)/sizeof(TUint8));
	
	iMockLTSY.NotifyTerminated(mockStatus);
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(eventTlv.End(), KErrNone);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS((TUint) RSat::KUserActivity, eventList.iEvents);	
	ASSERT_EQUALS(RSat::EUpdateEventList, eventList.iType);
	
	// ---TERMINAL RESPONSE---
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KSuccess, KNullDesC8);			
	// call TerminalRsp
	TerminalResponseL(respPckg);
	AssertMockLtsyStatusL();
		
	// ---ENVELOPE: EVENT DOWNLOAD---
	PrepareAndCallEventDownloadL(KEventUserActivity);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); //  this
	} 


/**
@SYMTestCaseID BA-CSAT-SUEL-SED-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::EventDownload
@SYMTestPriority High
@SYMTestActions Invokes RSat::EventDownload
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpEventListFU::TestEventDownload0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));	
	OpenPhoneL();
	OpenSatL();
	
	TRequestStatus requestStatus;	
	 
	//-------------------------------------------------------------------------
	// TEST Unsolicited call of RSat::EventDownload
	// without getting notifications from CTSY
 	//-------------------------------------------------------------------------	
	RSat::TEventDownloadBaseV2 			 baseEventInfo;
	RSat::TEventDownloadBaseV2Pckg 		 baseEventInfoPckg(baseEventInfo);
	iSat.EventDownload(requestStatus, RSat::KIdleScreenAvailable, baseEventInfoPckg);		
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int())	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); //  this

	// All other EventDownload tests are the part of NotifySetUpEventListPCmd tests	
	}


//----------------------------------------------------------------------------------------------------------
// Utility functions
//----------------------------------------------------------------------------------------------------------

/**
*  Prepare data for ExpectL() and post ExpectL.	
* @param aPCmdNumber - code of Proactive SIM commands	 
*/
void CCSatSetUpEventListFU::PrepareMockLDataWithExpL( TUint8 aCommand )
	{
	TPckg<TUint8> pcmdCodePckg(aCommand);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, pcmdCodePckg);
	}

/**
* prepare tlv buffer 
* @param aTlv - tlv buffer to prepare
* @param aEventList -  List of codes of events  
* @param aNumberOfEvents - number of events
*/ 
void CCSatSetUpEventListFU::PrepareTlv(TTlv& aTlv, const TUint8* aEventList, TUint8 aNumberOfEvents)
	{	
	aTlv.Begin(KBerTlvProactiveSimCommandTag);
	aTlv.AddTag(KTlvCommandDetailsTag);
	aTlv.AddByte(KTestCommandNumber);//ETLV_CommandNumber
	aTlv.AddByte(KSetUpEventList); //ETLV_TypeOfCommand
	aTlv.AddByte(KTestCommandQualifier); //ETLV_CommandQualifier	
	aTlv.AddTag(KTlvDeviceIdentityTag); 
	aTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	aTlv.AddByte(KMe);
	
	if (0 < aNumberOfEvents)
		{
		aTlv.AddTag(KTlvEventListTag);
		
		for (TInt i = 0; i < aNumberOfEvents; i++)
			{
			aTlv.AddByte(aEventList[i]);
			}
		}	
	}


/**
 * prepare mock for processing of proactive command
 * @param aCompleteTlv - tlv buffer to send
 * @param aError - result for request completion
 */ 
void CCSatSetUpEventListFU::CompleteMockDispatchSatPcmdL( TPtrC8 aCompleteTlv, TInt aError )
	{
	RBuf8 data;
	CleanupClosePushL(data);	

	TDesC8* tlvPtr = &aCompleteTlv;	
	TMockLtsyData1<TDesC8*> dspTxtDscPack(tlvPtr);
	dspTxtDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aError, data, KDelay);
	
	CleanupStack::PopAndDestroy(&data);
	}



/**
 * prepare data and call  iMockLTSY.ExpectL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, data);	
 * @param aPCmdNumber - command number	 
 * @param aGeneralResult - result of terminal response
 * @param aResultAddInfo - result's additional info
 * @param aErrorForExpect -  error, expected from LTSY
 */ 
void CCSatSetUpEventListFU::PrepareTerminalResponseMockDataWithExpL(
		TUint8 aPCmdNumber,		
		RSat::TPCmdResult aGeneralResult,
		const TDesC8& aResultAddInfo,
		TInt aErrorForExpect)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 terminalRsp;
	CleanupClosePushL(terminalRsp);
	terminalRsp.CreateL(KTlvMaxSize);
		
	TTlv tlv;
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(aPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KSetUpEventList); //ETLV_TypeOfCommand
	tlv.AddByte(KTestCommandQualifier); //ETLV_CommandQualifier 0x00 -- according to Test requirement

	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KMe);
	tlv.AddByte(KSim);
		
	tlv.AddTag(KTlvResultTag);
	tlv.AddByte(aGeneralResult);
	if (aResultAddInfo.Length())
		{
		tlv.AddByte( ( TUint8 ) aResultAddInfo[0] );
		}
	
	terminalRsp.Append(tlv.GetDataWithoutTopLevelTag());	
	
	TDesC8* terminalRspPtr = &terminalRsp;	
	TMockLtsyData1<TDesC8*> termRespData(terminalRspPtr);
	termRespData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, data, aErrorForExpect);
	
	CleanupStack::PopAndDestroy(&terminalRsp);
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * prepare data and call iSat.TerminalRsp(reqStatus, aRSatTypeOfCommand, aRspPckg);
 * @param aRspPckg - data for response 
 * @param aExpectedError - error, which is expected from CTSY 
 */ 
void CCSatSetUpEventListFU::TerminalResponseL(const TDesC8& aRspPckg, TInt aExpectedError)
	{
	TRequestStatus reqStatus;
	iSat.TerminalRsp(reqStatus, RSat::ESetUpEventList, aRspPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(aExpectedError, reqStatus.Int());
	}

/**
 * prepare data and call iSat.EventDownload;
 * @param aEvent - code of event 
 */ 
void CCSatSetUpEventListFU::PrepareAndCallEventDownloadL(const TUint8 aEvent)
	{
	PrepareAndCallEventDownloadL(	aEvent, 
									NULL,  		// aClientSat,
									EFalse,		// aIsUnsolicited,
									KErrNone); 	//aErrorForExpect)	
	}

/**
 * prepare data and call iSat.EventDownload;
 * @param aEvent - code of event
 * @param aUseOwnSat - if interal sat (iSat) is used
 * @param aClientSat - pointer to sat object (if aUseOwnSat == EFalse)
 * @param aIsUnsolicited - if this command is unsolicited one
 * @param aErrorForExpect -  error, expected from LTSY 
 */ 
void CCSatSetUpEventListFU::PrepareAndCallEventDownloadL( const TUint8 aEvent,														
														  const RSat* aClientSat,
														  TBool aIsUnsolicited,
														  TInt  aErrorForExpect)
	{	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 eventData;
	CleanupClosePushL(eventData);
	
	RSat::TEventDownloadBaseV2 			 baseEventInfo;
	RSat::TEventDownloadBaseV2Pckg 		 baseEventInfoPckg(baseEventInfo);
	
	// BY DEFAULT use base event info
	eventData.CreateL(baseEventInfoPckg);
	
	TRequestStatus requestStatus;	
	// Create envelope
	TTlv envelope;
	envelope.Begin(KBerTlvEventDownloadTag);
	// Event list
	envelope.AddTag(KTlvEventListTag);
	envelope.AddByte(aEvent);
	// Device identities
	envelope.AddTag(KTlvDeviceIdentityTag);	
	envelope.AddByte( (aEvent ==KEventIdleScreenAvailable)? KDisplay : KMe);	
	envelope.AddByte(KSim);
		
	RSat::TEventList singleEvent( RSat::KUserActivity);	
	switch (aEvent)
		{
		case KEventUserActivity:
			{		  
			singleEvent = RSat::KUserActivity; 								
			break;
			}	
		
		 case KEventIdleScreenAvailable:
			 {			
			 singleEvent = RSat::KIdleScreenAvailable;				 				 
			 break;
			 }
		
		 case KEventBrowserTermination:
			 {
			 singleEvent = RSat::KBrowserTermination;
			 
			 // change default event data
			 RSat::TBrowserTerminationEventV2	 browserTermination;
			 RSat::TBrowserTerminationEventV2Pckg browserTerminationPckg(browserTermination);
			 browserTermination.iCause = RSat::EUserTermination;
			 // use special (browserTermination) event info
			 eventData.Close();
			 eventData.CreateL(browserTerminationPckg);			 
			 
			 // Cause
		     envelope.AddTag( KTlvBrowserTerminationCauseTag );
		     envelope.AddByte( KBrowserTerminationUser );				 
			 break;
			 }
		
		 case KEventLanguageSelection:
			 {
			 singleEvent = RSat::KLanguageSelection; 
			 
			 // change default event data
			 RSat::TLanguageSelectionEventV2      lang;
			 RSat::TLanguageSelectionEventV2Pckg  langPck(lang);			 
			 //data for event download	
			 TUint languageCode(0xABCD); // some language code	
			 lang.iLanguage = languageCode;			 
			 // use special (lang) event info
			 eventData.Close();
			 eventData.CreateL(langPck);		
			 
			 // Language
			 envelope.AddTag( KTlvLanguageTag );
			 envelope.AddByte( TUint8( ( languageCode >> 8 ) & 0x7f ) );// MSB
			 envelope.AddByte( TUint8( ( languageCode ) & 0x7f ) );// LSB			 
			 break;
			 }
		
		 case KEventDataAvailable:
			 {
			 singleEvent = RSat::KDataAvailable;
			 
			 // change default event data
			 RSat::TDataAvailableEventV2	 	 dataAvailable;
			 RSat::TDataAvailableEventV2Pckg  	 dataAvailablePckg(dataAvailable);			 
			 TUint8 length = 0x22; // some length
			 //data for event download	
			 dataAvailable.iStatus.Append(KChannelStatusByte1);
			 dataAvailable.iLength = length;
			 // use special (dataavailable) event info
			 eventData.Close();
			 eventData.CreateL(dataAvailablePckg);	
			 
			 // Channel status
			 envelope.AddTag( KTlvChannelStatusTag );
			 envelope.AddByte( KChannelStatusByte1 );
			 // Channel data length
			 envelope.AddTag( KTlvChannelDataLengthTag );
			 envelope.AddByte( length );
		     break;
		     }
		   	
		 case KEventChannelStatus:
		   	 {
		   	 singleEvent = RSat::KChannelStatus;
		   	
		   	 // change default event data
		   	 RSat::TChannelStatusEventV2			 channelStatus;
		   	 RSat::TChannelStatusEventV2Pckg 	 channelStatusPckg(channelStatus);
		     //data for event download	
		   	 channelStatus.iStatus.Append(KChannelStatusByte1);		   	
		   	 // use special (channel status) event info	
		   	 eventData.Close();
		   	 eventData.CreateL(channelStatusPckg);
		  		  
		     // Channel status
		     envelope.AddTag( KTlvChannelStatusTag );
		     envelope.AddByte( KChannelStatusByte1 );
		     break;
		   	 }

		 case KLocalConnection_missing:
			 {
			 singleEvent = RSat::KCardReaderStatus;
			 break;
			 }
			 
		 case KDisplayParamsChanges_missing:
			 {
			 singleEvent = RSat::KCardReaderStatus;
			 break;
			 }
			 
		 case KEventCardReaderStatus:
			 {
			 singleEvent = RSat::KCardReaderStatus;
			 break;
			 }
		         
		 // Not reported to the client
		 case KEventMTCall:
		 case KEventCallConnected:
		 case KEventCallDisconnected:
		 case KEventLocationStatus:
		 case KEventAccessTechnologyChange:
		 default:
			 {
			 // we have nothing to do here
			 // so let's go out
			 CleanupStack::PopAndDestroy(&eventData);
			 CleanupStack::PopAndDestroy(&data);
			 return;
			 }
			 
		}// end Switch
		
	// if this request is NOT Unsolicited
	if (!aIsUnsolicited)
		{
		TDesC8* envelopePtr = &const_cast<TDesC8&>(envelope.End());
		TMockLtsyData1<TDesC8*> eventDownloadExpLtsyData(envelopePtr);
		eventDownloadExpLtsyData.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId,
							data, aErrorForExpect);
		}
	
	// this if-else just because of multiple client test
	if (aClientSat)
		{
		// it means that this request is from the second client
		aClientSat->EventDownload(requestStatus, singleEvent, eventData);		
		}
	else
		{		
		//ETel calls event download
		iSat.EventDownload(requestStatus, singleEvent, eventData);
		}
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(aErrorForExpect, requestStatus.Int())
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&eventData);
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * prepare data and force TSY to send ENVELOPE
 * @param aEvent - code of event
 * @param aIsEnvelopeSent- ETrue if ENVELOPE is  really sent to LTSY
 */ 
void CCSatSetUpEventListFU::EnvelopeFromTsyL(const TUint8 aEvent, TBool aIsEnvelopeSent)
	{	
	static const TUint8 someId = 111;
	
	switch (aEvent)
		{
		 case KEventMTCall:
			 {
			 // ---Prepare and call CompleteMTCallL with correct data-----------------------------				 
			 //Type of Number (TON) and numbering plan identification (NPI).
			 //MSB - 1, Type of Number - 011 (Network Specific Number), Numbering Plan - 1001 (Private numbering plan) 10111001 = B9 Hex
			 const TUint8 KTonAndNpi = (0x80) | (RMobilePhone::ENetworkSpecificNumber << 4) | (RMobilePhone::EPrivateNumberPlan);			 			 	
			 TBuf8<KAddrMaxLength> address;
			 address.Append(KTonAndNpi);
			 address.Append(_L8("1234567")); //dummy address 			 	
			 TBuf8<KSubAddressLength> subAddress;
			 subAddress.Append(_L8("12345678910")); //dummy subAddress 	
			 CompleteMTCallL(someId, &address, &subAddress, aIsEnvelopeSent);			 
			 
			 // ---Prepare and call CompleteMTCallL with NULL-address and subAddress lenght ------
			 address.Zero();	
			 subAddress.Zero();
			 address.Append(KTonAndNpi);			
			 CompleteMTCallL(someId, &address, &subAddress, aIsEnvelopeSent);		 
			 
			 // ---Prepare and call CompleteMTCallL without KTonAndNpi ----------------------------
			 address.Zero();			 
			 CompleteMTCallL(someId, &address, &subAddress, aIsEnvelopeSent);			 
			 break;
			 }
			 
		 case KEventCallConnected:
			 {			 			
			 // ---Prepare and call CompleteCallConnectedL with nearEnd = EFalse --------------------
			 TBool nearEnd = EFalse;
			 CompleteCallConnectedL(someId, nearEnd, aIsEnvelopeSent);
			 
			 // ---Prepare and call CompleteCallConnectedL with nearEnd = ETrue --------------------
			 nearEnd = ETrue;
			 CompleteCallConnectedL(someId, nearEnd, aIsEnvelopeSent);			 
			 break;
			 }
			 
		 case KEventCallDisconnected:
			 {
			 // ---Prepare and call CompleteCallDisconnectedL with nearEnd = EFalse and cause given---
			 TBool nearEnd = EFalse;
			 TBuf8 <KMaxCauseLength> cause;
			 cause.Append(_L8("1234567"));			 
			 CompleteCallDisconnectedL(someId, nearEnd, &cause, aIsEnvelopeSent);
			 
			 // ---Prepare and call CompleteCallDisconnectedL with nearEnd = ETrue and without cause--
			 nearEnd = ETrue;
			 cause.Zero();			 
			 CompleteCallDisconnectedL(someId, nearEnd, &cause, aIsEnvelopeSent);			 
			 break;
			 }
			 
		 case KEventLocationStatus:
			 {
			 // ---Prepare and call CompleteLocationStatusL with  KLocationStatusNormalService--------
			 TUint8 locationStatus = KLocationStatusNormalService;
			 TBuf8<KCountryCodeLength> countryCode(_L8("926")); // code of some Moscow ME operator
			 TUint16 locationAreaCode = KLocationAreaCode;
			 TUint16 cellId = KCellId;
			 CompleteLocationStatusL(locationStatus, countryCode, locationAreaCode, cellId, aIsEnvelopeSent);
			 
			 // ---Prepare and call CompleteLocationStatusL with  KLocationStatusLimitedService--------
			 locationStatus = KLocationStatusLimitedService;			
			 CompleteLocationStatusL(locationStatus, countryCode, locationAreaCode, cellId, aIsEnvelopeSent);
			 
			 // ---Prepare and call CompleteLocationStatusL with  another countryCode------------------
			 countryCode.Copy(_L8("124"));			
			 CompleteLocationStatusL(locationStatus, countryCode, locationAreaCode, cellId, aIsEnvelopeSent);	
			 
			 // ---Prepare and call CompleteLocationStatusL with  another locationAreaCode-------------
			 locationAreaCode = 333; // some dummy code			
			 CompleteLocationStatusL(locationStatus, countryCode, locationAreaCode, cellId, aIsEnvelopeSent);

			 // ---Prepare and call CompleteLocationStatusL with  another cellId-----------------------
			 cellId = 444; // some dummy id	
			 CompleteLocationStatusL(locationStatus, countryCode, locationAreaCode, cellId, aIsEnvelopeSent);
			 
			 // ---Prepare and call CompleteLocationStatusL with  the same data------------------------
			 CompleteLocationStatusL(locationStatus, countryCode, locationAreaCode, cellId, EFalse);			 
			 break;
			 }
			 
		 case KEventAccessTechnologyChange:
			 {
			 TUint8 aAccTechChange(0);
			 CompleteAccessTechnologyChangeL(aAccTechChange, aIsEnvelopeSent);
			 break;
			 }
			 
		 default:
			 {
			 break;
			 }
			 
		}// end of swith
	}


/**
 * Invokes CSatEventDownloadTsy::CompleteMTCallL()
 * @param aTransactionId - id of transaction
 * @param aAddress - address for complete
 * @param aSubAddress - subaddress for complete
 * @param aIsEnvelopeSent- ETrue if ENVELOPE is  really sent to LTSY
 */ 
void CCSatSetUpEventListFU::CompleteMTCallL(TUint8 aTransactionId, 
											TDesC8* aAddress,
											TDesC8* aSubAddress,
											TBool aIsEnvelopeSent)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
		
	// call TSY's CompleteMTCallL
	TMockLtsyData3<TUint8,TDesC8*,TDesC8*> mtCallPack(aTransactionId, aAddress, aSubAddress);
	mtCallPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatMtCallIndId, KErrNone, data, KDelay); // use KDelay to have time for preparing MockL data 
	data.Close();
	
	if (aIsEnvelopeSent)
		{
		//Prepare data MockL data for TSY's ENVELOPE
		// Create envelope
		TTlv envelope;
		envelope.Begin(KBerTlvEventDownloadTag);
		// Event list
		envelope.AddTag ( KTlvEventListTag );
		envelope.AddByte( KEventMTCall );
		// Device identities
		envelope.AddTag ( KTlvDeviceIdentityTag );
		envelope.AddByte( KNetwork );
		envelope.AddByte( KSim );	
		// Transaction identifier
		envelope.AddTag ( KTlvTransactionIdentifierTag );
		envelope.AddByte(aTransactionId);
		
		if (aAddress->Length())
			{
			envelope.AddTag ( KTlvAddressTag );
			envelope.AddData( *aAddress );
			}
		
		if (aSubAddress->Length())
			{
			envelope.AddTag( KTlvSubaddressTag );
			envelope.AddData( *aSubAddress );
			}
		
		TDesC8* envelopePtr = &const_cast<TDesC8&>(envelope.End());
		TMockLtsyData1<TDesC8*> eventDownloadExpLtsyData(envelopePtr);
		eventDownloadExpLtsyData.SerialiseL(data);		
		iMockLTSY.ExpectL(MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId,data);
		}
	
	// wait for CompleteMTCallL
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	CleanupStack::PopAndDestroy(&data);
	AssertMockLtsyStatusL();
	}

/**
 * Invokes CSatEventDownloadTsy::CompleteCallConnectedL()
 * @param aTransactionId - id of transaction
 * @param aNearEnd - indicates phone (KMe) or network (KNetwork)
 * @param aIsEnvelopeSent- ETrue if ENVELOPE is  really sent to LTSY
 */
void CCSatSetUpEventListFU::CompleteCallConnectedL(TUint8 aTransactionId, TBool aNearEnd, 
												TBool aIsEnvelopeSent)
	{
	 RBuf8 data;
	 CleanupClosePushL(data);
	 
	 TRequestStatus requestStatus; 
	
	 TMockLtsyData2<TUint8, TBool> callConnectedTechPack(aTransactionId, aNearEnd);
	 callConnectedTechPack.SerialiseL(data);
	 iMockLTSY.CompleteL(KMockLtsyDispatchSatCallConnectedIndId, KErrNone, data, KDelay); // use KDelay to have time for preparing MockL data
	 data.Close();
	 
	 if (aIsEnvelopeSent)
		 {	 
		 // Create envelope
		 TTlv envelope;
		 envelope.Begin( KBerTlvEventDownloadTag );
		 
		 // Event list
		 envelope.AddTag(KTlvEventListTag);
		 envelope.AddByte(KEventCallConnected);
		 // Device identities
		 envelope.AddTag(KTlvDeviceIdentityTag);
		 envelope.AddByte(aNearEnd ? KMe : KNetwork);
		 envelope.AddByte(KSim);
		 // Transaction identifier
		 envelope.AddTag(KTlvTransactionIdentifierTag);
		 envelope.AddByte(aTransactionId);
		 
		 TDesC8* envelopePtr = &const_cast<TDesC8&>(envelope.End());
		 TMockLtsyData1<TDesC8*> eventDownloadExpLtsyData(envelopePtr);
		 eventDownloadExpLtsyData.SerialiseL(data);		
		 iMockLTSY.ExpectL(MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId,data);
		 }
	 
	 // wait for CompleteMTCallL
	 TRequestStatus mockLtsyStatus;
	 iMockLTSY.NotifyTerminated(mockLtsyStatus); 
	 User::WaitForRequest(mockLtsyStatus);
	 ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	 
	 CleanupStack::PopAndDestroy(&data);
	 AssertMockLtsyStatusL();
	}	

/**
 * Invokes CSatEventDownloadTsy::CompleteCallDisconnectedL()
 * @param aTransactionId - id of transaction
 * @param aNearEnd - indicates phone (KMe) or network (KNetwork)
 * @param aCause - the initiator of disconnect 
 * @param aIsEnvelopeSent- ETrue if ENVELOPE is  really sent to LTSY
 */
void CCSatSetUpEventListFU::CompleteCallDisconnectedL(TUint8 aTransactionId, TBool aNearEnd, 
													TDesC8* aCause, TBool aIsEnvelopeSent)
	{
	
	RBuf8 data;
	CleanupClosePushL(data);
		 
	TRequestStatus requestStatus; 
	
	TMockLtsyData3<TUint8,TBool,TDesC8*> callDisconnectedLtsyData(aTransactionId, aNearEnd, aCause);
	callDisconnectedLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCallDisconnectedIndId, KErrNone, data,  KDelay); // use KDelay to have time for preparing MockL data
	data.Close();
	
	if (aIsEnvelopeSent)
		{	
	    // Create envelope data
		TTlv envelope;
		envelope.Begin(KBerTlvEventDownloadTag);
		// Event list
		envelope.AddTag(KTlvEventListTag);
		envelope.AddByte(KEventCallDisconnected);
		// Device identities
		envelope.AddTag(KTlvDeviceIdentityTag);
		envelope.AddByte(aNearEnd ? KMe : KNetwork);
		envelope.AddByte(KSim);
		// Transaction identifier
		envelope.AddTag(KTlvTransactionIdentifierTag);
		envelope.AddByte(aTransactionId);    
		// Cause
		if (aCause->Length() != 0)
			{
			envelope.AddTag (KTlvCauseTag);
			envelope.AddData(*aCause);
			}
		
		TDesC8* envelopePtr = &const_cast<TDesC8&>(envelope.End());
		TMockLtsyData1<TDesC8*> eventDownloadExpLtsyData(envelopePtr);
		eventDownloadExpLtsyData.SerialiseL(data);		
		iMockLTSY.ExpectL(MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId,data);
		}
	// wait for CompleteMTCallL
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		 
	CleanupStack::PopAndDestroy(&data);
	AssertMockLtsyStatusL();	
	}



/**
 * Invokes CSatEventDownloadTsy::CompleteLocationStatusL()
 * @param aLocationStatus - status
 * @param aCountryCode - operator's code
 * @param aLocationAreaCode - location area code
 * @param aCellId - cell id
 * @param aIsEnvelopeSent- ETrue if ENVELOPE is  really sent to LTSY
 */
void CCSatSetUpEventListFU::CompleteLocationStatusL(TUint8 	aLocationStatus,
													TDes8&	aCountryCode,
													TUint16 aLocationAreaCode,
													TUint16 aCellId,
													TBool aIsEnvelopeSent)
	{	
	RBuf8 data;
	CleanupClosePushL(data);		 
	TRequestStatus requestStatus;
	TDesC8* countryCodePtr = &aCountryCode;
	TMockLtsyData4<TUint8, TDesC8*, TUint16, TUint16 >locationStatusData( aLocationStatus, 
																			countryCodePtr,
																			aLocationAreaCode,
																			aCellId);	
	locationStatusData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatLocationStatusIndId, KErrNone, data,  KDelay); // use KDelay to have time for preparing MockL data
	data.Close();
	
	if (aIsEnvelopeSent)
		{
		// Create envelope data
		TTlv envelope;
		envelope.Begin  ( KBerTlvEventDownloadTag );
		// Event list
		envelope.AddTag ( KTlvEventListTag );
		envelope.AddByte( KEventLocationStatus );
		// Device identities
		envelope.AddTag ( KTlvDeviceIdentityTag );
		envelope.AddByte( KMe );
		envelope.AddByte( KSim );
		// Locationstatus
		envelope.AddTag ( KTlvLocationStatusTag );
		envelope.AddByte( TUint8(aLocationStatus) );

		// Location information, this is included only if status indicates 
		// normal service
		if ( KLocationStatusNormalService == aLocationStatus )
			{
			envelope.AddTag ( KTlvLocationInformationTag );
			envelope.AddData( aCountryCode );        
			// Mobile country & network codes
			envelope.AddByte( TUint8( aLocationAreaCode >> 8 ) );
			envelope.AddByte( TUint8( aLocationAreaCode ) );
			envelope.AddByte( TUint8( aCellId >> 8 ) ); 
			envelope.AddByte( TUint8( aCellId ) );
			}
	
		TDesC8* envelopePtr = &const_cast<TDesC8&>(envelope.End());
		TMockLtsyData1<TDesC8*> eventDownloadExpLtsyData(envelopePtr);
		eventDownloadExpLtsyData.SerialiseL(data);		
		iMockLTSY.ExpectL(MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId,data);
		} 
	
	// wait for CompleteMTCallL
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		 
	CleanupStack::PopAndDestroy(&data);
	AssertMockLtsyStatusL();	
	}
/**
 * Invokes CSatEventDownloadTsy::CompleteAccessTechnologyChangeL()
 * @param aAccTechChange - Access technology
 * @param aIsEnvelopeSent- ETrue if ENVELOPE is  really sent to LTSY
 */
void CCSatSetUpEventListFU::CompleteAccessTechnologyChangeL(TUint8 aAccTechChange, TBool aIsEnvelopeSent)
	{
	RBuf8 data;
	CleanupClosePushL(data);
		 
	TRequestStatus requestStatus;
	
	TMockLtsyData1<TUint8> accTechData( aAccTechChange);	
	accTechData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatAccessTechnologyChangeIndId, KErrNone, data,  KDelay); // use KDelay to have time for preparing MockL data
	data.Close();
	
	if (aIsEnvelopeSent)
		{
		TTlv envelope;
		envelope.Begin  ( KBerTlvEventDownloadTag );
		// event list
		envelope.AddTag ( KTlvEventListTag );
		envelope.AddByte( KEventAccessTechnologyChange );
		// device identities
		envelope.AddTag ( KTlvDeviceIdentityTag );
		envelope.AddByte( KMe );
		envelope.AddByte( KSim );	
		envelope.AddTag( KTlvAccessTechnologyTag );
		envelope.AddByte( aAccTechChange );	
		
		TDesC8* envelopePtr = &const_cast<TDesC8&>(envelope.End());
		TMockLtsyData1<TDesC8*> eventDownloadExpLtsyData(envelopePtr);
		eventDownloadExpLtsyData.SerialiseL(data);		
		iMockLTSY.ExpectL(MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId,data);
		}
	// wait for CompleteMTCallL
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		 
	CleanupStack::PopAndDestroy(&data);
	AssertMockLtsyStatusL();	
	}


/**
 * Tests all possible "TERMINAL RESPONSE" cases. 
 * RSat::TerminalRsp should complets with KErrNone for all valid responses and KErrCorrupt 
 * for those that can not be used with "SET UP EVENT LIST" command  
 */
void CCSatSetUpEventListFU::DoTestTerminalResponseL()
	{
	static const struct 
		{
		RSat::TPCmdResult			iGeneralResult;
		RSat::TAdditionalInfoType	iInfoType;
		TBool						iIntentionallyOmmitAdditionalInfo; // skip AdditionalInfo
		TUint8						iAdditionalInfo;
		TInt						iExpectedResult;
		} 
	KResponsesToTest[] = 
		{
			{
			// 0x01 - Command performed with partial comprehension;
			RSat::KPartialComprehension,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},
			{
			// 0x02 - Command performed, with missing information;
			RSat::KMissingInformation,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},
			{
			// 0x03 - REFRESH performed with additional EFs read;
			RSat::KRefreshAdditionEFRead,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{ 
			// 0x04 - Command performed successfully, but requested icon could not be displayed;
			RSat::KSuccessRequestedIconNotDisplayed,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x05 - Command performed, but modified by call control by NAA;
			RSat::KModifiedByCallControl,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x06 - Command performed successfully, limited service;
			RSat::KSuccessLimitedService,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x07 - Command performed with modification;
			RSat::KPerformedWithModifications,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x08 - REFRESH performed but indicated NAA was not active;
			RSat::KRefreshUSIMNotActive,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x09 - Command performed successfully, tone not played;
			RSat::KPlayTonePerformedSuccessfully,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x10 - Proactive UICC session terminated by the user;
			RSat::KPSessionTerminatedByUser,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x11 - Backward move in the proactive UICC session requested by the user;
			RSat::KBackwardModeRequestedByUser,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt			
			},
			{
			// 0x12 - No response from user;
			RSat::KNoResponseFromUser,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt			
			},
			{
			// 0x13 - Help information required by the user;
			RSat::KHelpRequestedByUser,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt			
			},
			{
			// 0x14 - reserved for GSM/3G.
			RSat::KUssdTransactionTerminatedByUser,
			RSat::KNoAdditionalInfo, 
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x20 - terminal currently unable to process command;
			RSat::KMeUnableToProcessCmd,
			RSat::KMeProblem,
			EFalse,
			0xAE, // dummy additional info
			KErrNone
			},
			{
			// 0x20 without additional information (erroneous)
			RSat::KMeUnableToProcessCmd,
			RSat::KMeProblem, 
			ETrue, // Intentional error !
			0,
			KErrCorrupt
			},	
			
			{
			// 0x21 - Network currently unable to process command;
			RSat::KNetworkUnableToProcessCmd,
			RSat::KSatNetworkErrorInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x22 - User did not accept the proactive command;
			RSat::KPCmdNotAcceptedByUser,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x23 - User cleared down call before connection or network release;
			RSat::KCallClearedBeforeConnectionOrReleased,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x24 - Action in contradiction with the current timer state;
			RSat::KContradictionWithTimerState,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x25 - Interaction with call control by NAA, temporary problem;
			RSat::KInteractionWithCCTemporaryError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x26 - Launch browser generic error code;
			RSat::KLaunchBrowserError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x27 - MMS temporary problem.
			RSat::KMMSTemporaryProblem,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x30 - Command beyond terminal's capabilities;
			RSat::KCmdBeyondMeCapabilities,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},			
			{
			// 0x31 - Command type not understood by terminal;
			RSat::KCmdTypeNotUnderstood,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},
			{ 
			// 0x32 - Command data not understood by terminal;
			RSat::KCmdDataNotUnderstood,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},
			{
			// 0x33 - Command number not known by terminal;
			RSat::KCmdNumberNotKnown,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},
			{
			// 0x34 - reserved for GSM/3G;
			RSat::KSsReturnError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x35 - reserved for GSM/3G;
			RSat::KSmsRpError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{ 
			// 0x36 - Error, required values are missing;
			RSat::KErrorRequiredValuesMissing,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},			
			{
			// 0x37 - reserved for GSM/3G;
			RSat::KUssdReturnError,
			RSat::KSatNetworkErrorInfo,
			EFalse,
			0,
			KErrCorrupt
			},
			{
			// 0x38 - MultipleCard commands error;
			RSat::KMultipleCardCmdsError,
			RSat::KMeProblem,
			EFalse,
			0,
			KErrCorrupt
			},
			{
			// 0x39 - Interaction with call control by NAA, permanent problem;
			RSat::KInteractionWithCCPermanentError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			
			{
			// 0x3A - Bearer Independent Protocol error;
			RSat::KBearerIndepProtocolError,
			RSat::KMeProblem, 
			EFalse,
			RSat::KNoSpecificBIPError,
			KErrCorrupt
			},
			{
			// 0x3A without additional information (erroneous)
			RSat::KBearerIndepProtocolError,
			RSat::KMeProblem, 
			ETrue, // Intentional error
			0,
			KErrCorrupt
			},
			{
			// 0x3B - Access Technology unable to process command;
			RSat::KAccessTechUnableProcessCmd,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x3C - Frames error;
			RSat::KFramesError,
			RSat::KMeProblem,
			EFalse,
			RSat::KNoSpecificCauseCanBeGiven,
			KErrCorrupt
			},
			{
			// 0x3D - MMS Error.
			RSat::KMMSError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			}
		};	

	static const TUint8 events[] = {KEventUserActivity};
	
	const TInt KNumberOfResponsesToTest = sizeof(KResponsesToTest)/ sizeof(KResponsesToTest[0]);
	
	TRequestStatus requestStatus;
	// prepare TLV for
	TTlv eventTlv;	
	PrepareTlv( eventTlv, events, sizeof(events)/sizeof(TUint8));
	const TDesC8& tlvEnd = eventTlv.End();
	
	TBuf8<1> resultAddInfo;			
	RSat::TSetUpEventListV1 eventList;
	RSat::TSetUpEventListV1Pckg eventListPck(eventList);
	
		

	//Prepare data for ExpectL needed by NotifySetUpEventListPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSetUpEventList);						
	// Post Notify itself		
	iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);		
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(tlvEnd, KErrNone);		    
					    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());				
	// ------------------------------------------------------------------------------------
	// Tell Mock to return KErrUnknown when dispatching forthcoming RSat::TerminalRsp()
	// something like  "Test A" of 001 test for TerminalResponse	
	//-------------------------------------------------------------------------------------
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber, RSat::KSuccess, KNullDesC8, KErrUnknown);	
	
	// call TerminalRsp
	RSat::TSetUpEventListRspV1 resp;
	RSat::TSetUpEventListRspV1Pckg respPckg(resp);		
	resp.SetPCmdNumber(KPcmdNumber);
	resp.iGeneralResult	= RSat::KSuccess;
	resp.iInfoType 		= RSat::KNoAdditionalInfo;	
	TerminalResponseL(respPckg, KErrUnknown);		
	AssertMockLtsyStatusL();
	
	// ---------------------------------------------------------------
	//Execute All cases for terminal response
	// ---------------------------------------------------------------
		
	for(TInt i = 0; i < KNumberOfResponsesToTest; i++)
		{	
		//Prepare data for ExpectL needed by NotifySetUpEventListPCmd
		//and post that ExpectL
		PrepareMockLDataWithExpL(KSetUpEventList);						
		// Post Notify itself		
		iSat.NotifySetUpEventListPCmd(requestStatus, eventListPck);		
		// post Complete with KErrNone
		CompleteMockDispatchSatPcmdL(tlvEnd, KErrNone);		    
						    
		User::WaitForRequest(requestStatus);
		ASSERT_EQUALS(KErrNone, requestStatus.Int());				
		
		// now test a "TERMINAL RESPONSE" specific to this loop 		
		resultAddInfo.Zero();				
		if ((	RSat::KNoAdditionalInfo	==	KResponsesToTest[i].iInfoType ||
				RSat::KMeProblem		==	KResponsesToTest[i].iInfoType	) && 
				!KResponsesToTest[i].iIntentionallyOmmitAdditionalInfo)
			{			
			resultAddInfo.Append(KResponsesToTest[i].iAdditionalInfo);
			}
					
		PrepareTerminalResponseMockDataWithExpL(KPcmdNumber,
												KResponsesToTest[i].iGeneralResult, 
												resultAddInfo);
		// call TerminalRsp
		RSat::TSetUpEventListRspV1 resp;
		RSat::TSetUpEventListRspV1Pckg respPckg(resp);
		resp.iGeneralResult = KResponsesToTest[i].iGeneralResult;
		resp.iInfoType 		= KResponsesToTest[i].iInfoType;
		if(!KResponsesToTest[i].iIntentionallyOmmitAdditionalInfo)
			{
			resp.iAdditionalInfo.Append(KResponsesToTest[i].iAdditionalInfo);
			}
		
		resp.SetPCmdNumber(KPcmdNumber);	
		TerminalResponseL(respPckg, KResponsesToTest[i].iExpectedResult);
		AssertMockLtsyStatusL();
			
		// ---ENVELOPE: EVENT DOWNLOAD---
		// There is No ENVELOPE sent in case of errors!					
		}
	
	}


