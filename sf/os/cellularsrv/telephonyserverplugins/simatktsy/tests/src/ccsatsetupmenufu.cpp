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
 @file The TEFUnit test suite for SetUpMenu in the SAT.
*/

#include <cstktsy/bertlv_defs.h>
#include <etel.h>
#include <etelmm.h>
#include <satcs.h>
#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <test/mockltsyindicatorids.h>
#include <test/tmockltsydata.h>

#include "ccsatsetupmenufu.h"

static const TInt KDelay = 10;

CTestSuite* CCSatSetUpMenuFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatSetUpMenuFU, TestNotifySetUpMenuPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpMenuFU, TestNotifySetUpMenuPCmd0001bL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpMenuFU, TestNotifySetUpMenuPCmd0001cL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpMenuFU, TestNotifySetUpMenuPCmd0001dL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpMenuFU, TestNotifySetUpMenuPCmd0001eL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpMenuFU, TestNotifySetUpMenuPCmd0001fL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpMenuFU, TestNotifySetUpMenuPCmd0001gL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpMenuFU, TestNotifySetUpMenuPCmd0001hL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpMenuFU, TestNotifySetUpMenuPCmd0001iL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpMenuFU, TestNotifySetUpMenuPCmd0001jL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpMenuFU, TestNotifySetUpMenuPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpMenuFU, TestNotifySetUpMenuPCmd0004L);
	
	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-SUM-SNSUMPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpMenuPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpMenuPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpMenuFU::TestNotifySetUpMenuPCmd0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	RBuf8 data;
	CleanupClosePushL(data);

	TUint8 KPCmdNumber = 1;

	// prepare tlv buffer for serialisation
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KSetUpMenu); //ETLV_TypeOfCommand
	tlv.AddByte(0); //ETLV_CommandQualifier
	
	tlv.AddTag(KTlvDeviceIdentityTag); 
	tlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	tlv.AddByte(KDisplay); //ETLV_DestinationDeviceIdentity
	
	tlv.AddTag(KTlvAlphaIdentifierTag);
	_LIT8(KSetupMenuTest8,"Menu AlphaIdent");
	tlv.AddData(KSetupMenuTest8);//ETLV_AlphaIdentifier

	tlv.AddTag(KTlvItemTag);
	tlv.AddByte(77); //item identifier
	tlv.AddData(_L8("Item AlphaIdent1"));
	
	TPtrC8 tlvDsc = tlv.End();
	TDesC8* tlvDscPtr = &tlvDsc;

	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	// tell Mock about forthcoming RSat::NotifySetUpMenuPCmd
	TUint8 pcmdCode = KSetUpMenu;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	data.Close();
 
	RSat::TSetUpMenuV2 setupMenu;
	RSat::TSetUpMenuV2Pckg setpPck(setupMenu);
	TRequestStatus stat;
	iSat.NotifySetUpMenuPCmd(stat, setpPck);

	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, KErrGeneral, data, KDelay);

	User::WaitForRequest(stat);
	ASSERT_EQUALS(KErrGeneral, stat.Int());	

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifySetUpMenuPCmd from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	    

	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, KErrNone, data, KDelay);
	data.Close();
	
	RSat::TAdditionalInfo addInfo;
	addInfo.Zero();
	addInfo.Append(RSat::KNoSpecificMeProblem);
	
	PrepareTerminalResponseMockDataL( KPCmdNumber, KSetUpMenu, 0,
			addInfo, RSat::KMeUnableToProcessCmd, KNullDesC8);	
	
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// test C - Successful completion request of
	// RSat::NotifySetFramesPCmd() - implemented in 0001b()-0001i() functions
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-SUM-SNSUMPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpMenuPCmd - SET UP MENU (help request support) and ENVELOPE MENU SELECTION
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpMenuPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpMenuFU::TestNotifySetUpMenuPCmd0001bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	// Expected Sequence 2.1 
	//(SET UP MENU and MENU SELECTION, with Help Request, Replace and Remove a Toolkit Menu)
	//See ETSI TS 102 384 [26] in subclause 27.22.4.8.2.4.2, Expected Sequence 2.1.

	TUint8 cmdNumber = 1;
	const TInt KNumItems = 4;	
	
	TUint8 ids[KNumItems] = {1, 2, 3, 4};
	TPtrC8 idNames[KNumItems] = {_L8("Item 1"), _L8("Item 2"), _L8("Item 3"), _L8("Item 4")};
	
	SetUpMenuL( cmdNumber, KHelpAvailabilityMask, _L8("Toolkit Menu"), KNumItems, ids, idNames );
		
	//generate the terminal response
	RSat::TSetUpMenuRspV1 resp;
	RSat::TSetUpMenuRspV1Pckg respPck(resp);
	
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(cmdNumber);
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, KHelpAvailabilityMask, RSat::ESetUpMenu,
			 respPck, resp.iAdditionalInfo, resp.iGeneralResult);	
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();
	AssertMockLtsyStatusL();

	// select 2
	SelectMenuItemL(ids[1], ETrue);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this);
	}


/**
@SYMTestCaseID BA-CSAT-SUM-SNSUMPC-0001c
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpMenuPCmd - SET UP MENU (next action support) and ENVELOPE MENU SELECTION
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpMenuPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpMenuFU::TestNotifySetUpMenuPCmd0001cL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	// Expected Sequence 3.1 
	//(SET UP MENU, next action indicator "Send SM", "Set Up Call", "LaunchBrowser", 
	//"Provide Local Information", successful)
	//See ETSI TS 102 384 [26] in subclause 27.22.4.8.3.4.2, Expected Sequence 3.1.

	TUint8 cmdNumber = 1;
	TUint8 noPref(0);	// no selection preferences
	const TInt KNumItems = 4;	
	
	TUint8 ids[KNumItems] = {1, 2, 3, 4};
	TPtrC8 idNames[KNumItems] = {_L8("Item 1"), _L8("Item 2"), _L8("Item 3"), _L8("Item 4")};
	TUint8 actions[KNumItems] = {KSendShortMessage, KSetUpCall, KLaunchBrowser, KProvideLocalInformation};
	
	SetUpMenuL( cmdNumber, noPref, _L8("Toolkit Menu"), KNumItems, ids, idNames, actions );
		
	//generate the terminal response
	RSat::TSetUpMenuRspV1 resp;
	RSat::TSetUpMenuRspV1Pckg respPck(resp);
	
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(cmdNumber);
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();
	AssertMockLtsyStatusL();

	// select 2
	SelectMenuItemL(ids[1], ETrue);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this); 
	}



/**
@SYMTestCaseID BA-CSAT-SUM-SNSUMPC-0001d
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpMenuPCmd - SET UP MENU (display of icons) and ENVELOPE MENU SELECTION
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpMenuPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpMenuFU::TestNotifySetUpMenuPCmd0001dL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	// Expected Sequence 4.1A 
	//(SET UP MENU, BASIC ICON NOT SELF EXPLANATORY in ALPHA ID and ITEMS DATA OBJECTS, successful)
	//See ETSI TS 102 384 [26] in subclause 27.22.4.8.4.4.2, Expected Sequence 4.1A.

	TUint8 cmdNumber = 1;
	TUint8 noPref(0);	// no preferences
	const TInt KNumItems = 3;	
	
	TUint8 ids[KNumItems] = {1, 2, 3};
	TPtrC8 idNames[KNumItems] = {_L8("Item 1"), _L8("Item 2"), _L8("Item 3")};
	TUint8 iconList[KNumItems] = {5, 5, 5};
		
	SetUpMenuL( cmdNumber, noPref, _L8("Toolkit Menu"), KNumItems, ids, idNames, NULL,
		RSat::ENotSelfExplanatory, 1, RSat::ENotSelfExplanatory, iconList );
      
	//generate the terminal response
	RSat::TSetUpMenuRspV1 resp;
	RSat::TSetUpMenuRspV1Pckg respPck(resp);
	
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(cmdNumber);
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();
	AssertMockLtsyStatusL();

	// select 2
	SelectMenuItemL(ids[1], ETrue);
	AssertMockLtsyStatusL();
	
	//Expected Sequence 4.1B 
	//(SET UP MENU, BASIC ICON NOT SELF EXPLANATORY in ALPHA ID and ITEMS DATA OBJECTS, 
	//requested icon could not be displayed)
	//See ETSI TS 102 384 [26] in subclause 27.22.4.8.4.4.2, Expected Sequence 4.1B.

	SetUpMenuL( cmdNumber, noPref, _L8("Toolkit Menu"), KNumItems, ids, idNames, NULL,
		RSat::ENotSelfExplanatory, 1, RSat::ENotSelfExplanatory, iconList );
      
	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed;
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();
	AssertMockLtsyStatusL();

	// select 2
	SelectMenuItemL(ids[1], ETrue);
	AssertMockLtsyStatusL();

	//Expected Sequence 4.2A (SET UP MENU, BASIC ICON SELF EXPLANATORY in ALPHA ID and ITEMS
	//DATA OBJECTS, successful)
	//See ETSI TS 102 384 [26] in subclause 27.22.4.8.4.4.2, Expected Sequence 4.2A.

	SetUpMenuL( cmdNumber, noPref, _L8("Toolkit Menu"), KNumItems, ids, idNames, NULL,
		RSat::ESelfExplanatory, 1, RSat::ESelfExplanatory, iconList );
      
	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, respPck,
			 resp.iAdditionalInfo, resp.iGeneralResult);
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();
	AssertMockLtsyStatusL();

	// select 2
	SelectMenuItemL(ids[1], ETrue);
	AssertMockLtsyStatusL();
	
	//Expected Sequence 4.2B (SET UP MENU, BASIC ICON SELF EXPLANATORY in ALPHA ID and ITEMS
	//DATA OBJECTS, requested icon could not be displayed)
	//See ETSI TS 102 384 [26] in subclause 27.22.4.8.4.4.2, Expected Sequence 4.2B.

	SetUpMenuL( cmdNumber, noPref, _L8("Toolkit Menu"), KNumItems, ids, idNames, NULL,
		RSat::ESelfExplanatory, 1, RSat::ESelfExplanatory, iconList );
      
	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed;
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();
	AssertMockLtsyStatusL();

	// select 2
	SelectMenuItemL(ids[1], ETrue);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(1, this);
	}


/**
@SYMTestCaseID BA-CSAT-SUM-SNSUMPC-0001e
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpMenuPCmd - SET UP MENU (soft keys support) and ENVELOPE MENU SELECTION
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpMenuPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpMenuFU::TestNotifySetUpMenuPCmd0001eL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	// Expected Sequence 5.1 (SET UP MENU, SOFT KEY PREFERRED, successful)
	//See ETSI TS 102 384 [26] in subclause 27.22.4.8.5.4.2, Expected Sequence 5.1.

	TUint8 cmdNumber = 1;
	const TInt KNumItems = 2;	
	
	TUint8 ids[KNumItems] = {1, 2};
	TPtrC8 idNames[KNumItems] = {_L8("Item 1"), _L8("Item 2")};
	
	SetUpMenuL( cmdNumber, KSelectionUsingSoftKey, _L8("Toolkit Menu"), KNumItems, ids, idNames );
		
	//generate the terminal response
	RSat::TSetUpMenuRspV1 resp;
	RSat::TSetUpMenuRspV1Pckg respPck(resp);
	
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(cmdNumber);
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, KSelectionUsingSoftKey, RSat::ESetUpMenu,
			 respPck, resp.iAdditionalInfo, resp.iGeneralResult);	
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();

	// select second item	
	SelectMenuItemL(ids[1]);   

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this);
	}


/**
@SYMTestCaseID BA-CSAT-SUM-SNSUMPC-0001f
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpMenuPCmd - SET UP MENU (help request support) and ENVELOPE MENU SELECTION
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpMenuPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpMenuFU::TestNotifySetUpMenuPCmd0001fL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	// Check SATTSY for handling error value
	
	TUint8 cmdNumber = 1;
	const TInt KNumItems = 4;	
	
	TUint8 ids[KNumItems] = {1, 2, 3, 4};
	TPtrC8 idNames[KNumItems] = {_L8("Item 1"), _L8("Item 2"), _L8("Item 3"), _L8("Item 4")};
	
	SetUpMenuL( cmdNumber, KHelpAvailabilityMask, _L8("Toolkit Menu"), KNumItems, ids, idNames );
		
	//generate the terminal response
	RSat::TSetUpMenuRspV1 resp;
	RSat::TSetUpMenuRspV1Pckg respPck(resp);
	
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(cmdNumber);
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, KHelpAvailabilityMask, RSat::ESetUpMenu,
			 respPck, resp.iAdditionalInfo, resp.iGeneralResult);	
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();
	AssertMockLtsyStatusL();

	// select 2
	// now tell Mock to return KErrUnknown when dispatching forthcoming RSat::MenuSelection()
	SelectMenuItemL(ids[1], ETrue, KErrUnknown);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this);
	}


/**
@SYMTestCaseID BA-CSAT-SUM-SNSUMPC-0001g
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpMenuPCmd - SET UP MENU (UCS2 display in Cyrillic) and ENVELOPE MENU SELECTION
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpMenuPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpMenuFU::TestNotifySetUpMenuPCmd0001gL()
	{
	//"hello" in russian
	_LIT8(KMenu, 
	"\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15");

	//"hello1", "hello2" etc... in russian
	TPtrC8 idNames[4] = 
	{_L8("\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15\x00\x31"), 
	_L8("\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15\x00\x32"),
	_L8("\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15\x00\x33"),
	_L8("\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15\x00\x34")};

	//"hello5", "hello6" in russian
	TPtrC8 idNames2[2] = 
	{_L8("\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15\x00\x35"), 
	_L8("\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15\x00\x36")};
	
	CheckForeignSetUpMenuL(KMenu, idNames, idNames2);							   
	}


/**
@SYMTestCaseID BA-CSAT-SUM-SNSUMPC-0001h
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpMenuPCmd - SET UP MENU (UCS2 display in Chinese) and ENVELOPE MENU SELECTION
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpMenuPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpMenuFU::TestNotifySetUpMenuPCmd0001hL()
	{
	//"Toolkit menu" in chinese
	_LIT8(KMenu, "\x80\x5D\xE5\x51\x77\x7B\xB1\x53\x55");

	//"Item 1", "Item 2" etc. in chinese
	TPtrC8 idNames[4] = 
	{_L8("\x80\x98\x79\x76\xEE\x4E\x00"), 
	_L8("\x80\x98\x79\x76\xEE\x4E\x8C"),
	_L8("\x80\x98\x79\x76\xEE\x4E\x09"),
	_L8("\x80\x98\x79\x76\xEE\x56\xDB")};

	//"One", "two" in chinese
	TPtrC8 idNames2[2] = 
	{_L8("\x80\x4E\x00"), 
	 _L8("\x80\x4E\x8C")};
	
	CheckForeignSetUpMenuL(KMenu, idNames, idNames2);							   
	}


/**
@SYMTestCaseID BA-CSAT-SUM-SNSUMPC-0001i
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpMenuPCmd - SET UP MENU (UCS2 display in Katakana) and ENVELOPE MENU SELECTION
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpMenuPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpMenuFU::TestNotifySetUpMenuPCmd0001iL()
	{
	//Menu Header and menu items use characters in Katakana	
	_LIT8(KMenu, "\x80\x00\x38\x00\x30\x30\xEB\x00\x30");	

	TPtrC8 idNames[4] = 
	{_L8("\x80\x00\x38\x00\x30\x30\xEB\x00\x31"), 
	_L8("\x80\x00\x38\x00\x30\x30\xEB\x00\x32"),
	_L8("\x80\x00\x38\x00\x30\x30\xEB\x00\x33"),
	_L8("\x80\x00\x38\x00\x30\x30\xEB\x00\x34")};

	//second menu items use characters in Katakana
	TPtrC8 idNames2[2] = 
	{_L8("\x80\x00\x38\x00\x30\x30\xEB\x00\x35"), 
	_L8("\x80\x00\x38\x00\x30\x30\xEB\x00\x36")};
	
	CheckForeignSetUpMenuL(KMenu, idNames, idNames2);							   
	}


/**
* auxiliary function for testing different types of menu alignment
* @param aAlign - type of alignment for tvl text formatting
*/    
void CCSatSetUpMenuFU::CheckMenuAlignmentL(TUint8 aAlign)
	{
	TUint8 cmdNumber = 1;
	TUint8 noPref = 0;	
	const TInt KNumItems = 3;
	_LIT8(KMenu1, "Toolkit Menu 1");
	
	TUint8 ids[KNumItems] = {1, 2, 3};
	TPtrC8 idNames[KNumItems] = {_L8("Item 1"), _L8("Item 2"), _L8("Item 3")};
	TUint8 alphaFormatting[RSat::KTextAttributeBufSize] = 
		{
		0, 	//formatting position
		14, //text length
		aAlign, 
		0xB4 //Dark Green Foreground(0x04), Bright Yellow Background(0xB0)
		};
	TUint8 itemsFormatting[KNumItems][RSat::KTextAttributeBufSize] = 
		{{ 0, 6, aAlign ,0xB4 },
		 { 0, 6, aAlign ,0xB4 },
		 { 0, 6, aAlign ,0xB4 }};

	//Text attributes are not parsed in CSatNotifySetUpMenu::CompleteNotifyL()
	
	ERR_PRINTF2(_L("<font color=Orange>$CSATKnownFailure: defect id = %d</font>"), 6);
	
	SetUpMenuL( cmdNumber, noPref, KMenu1, KNumItems, ids, idNames, NULL,
		RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 
		alphaFormatting, itemsFormatting );

		
	//generate the terminal response
	RSat::TSetUpMenuRspV1 resp;
	RSat::TSetUpMenuRspV1Pckg respPck(resp);
	
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(cmdNumber);
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);	
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();

	// select second item
	SelectMenuItemL(ids[1]);   

	AssertMockLtsyStatusL();

	// second menu
	TUint8 ids2[KNumItems] = {4, 5, 6};
	TPtrC8 idNames2[KNumItems] = {_L8("Item 4"), _L8("Item 5"), _L8("Item 6") };
	_LIT8(KMenu2, "Toolkit Menu 2");
	
	SetUpMenuL( cmdNumber, noPref, KMenu2, KNumItems, ids2, idNames2 );

	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);	
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();

	// select second item	
	SelectMenuItemL(ids2[1]);   

	AssertMockLtsyStatusL();	
	}


/**
@SYMTestCaseID BA-CSAT-SUM-SNSUMPC-0001j
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpMenuPCmd - SET UP MENU (normal) and ENVELOPE MENU SELECTION
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpMenuPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpMenuFU::TestNotifySetUpMenuPCmd0001jL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	// Expected Sequence 1.1 
	//(SET UP MENU and MENU SELECTION, without Help Request, Replace and Remove a Toolkit Menu)
	// See ETSI TS 102 384 [26] in subclause 27.22.4.8.1.4.2, Expected Sequence 1.1.

	TUint8 cmdNumber = 1;
	TUint8 noPref(0);	// no selection preferences
	const TInt KNumItems = 4;

	TUint8 ids[KNumItems] = {1, 2, 3, 4};
	TPtrC8 idNames[KNumItems] = {_L8("Item 1"), _L8("Item 2"), _L8("Item 3"), _L8("Item 4")};
	
	SetUpMenuL( cmdNumber, noPref, _L8("Toolkit Menu"), KNumItems, ids, idNames );
		
	//generate the terminal response
	RSat::TSetUpMenuRspV1 resp;
	RSat::TSetUpMenuRspV1Pckg respPck(resp);
	
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(cmdNumber);
	
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();

	// select second item	
	SelectMenuItemL(ids[1]);
    
    // making second menu instead of first
	const TInt KNumItems2 = 2;

	TUint8 ids2[KNumItems2] = {11, 12};
	TPtrC8 idNames2[KNumItems2] = {_L8("One"), _L8("Two")};
	
	SetUpMenuL( cmdNumber, noPref, _L8("Toolkit Menu"), KNumItems2, ids2, idNames2 );
    	
	//generate the terminal response	
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, 
			RSat::ESetUpMenu, respPck, resp.iAdditionalInfo, resp.iGeneralResult);
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();

	// select second item	
	SelectMenuItemL(ids2[1]);

	AssertMockLtsyStatusL();

    // remove the menu    
	SetUpMenuL( cmdNumber, noPref, _L8("Toolkit Menu"), 0, NULL, NULL );
			
	//generate the terminal response	
	GenerateTerminalResponseL(cmdNumber,KSetUpMenu, noPref, RSat::ESetUpMenu, 
			 respPck, resp.iAdditionalInfo, resp.iGeneralResult);
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();
	AssertMockLtsyStatusL();

	//Expected Sequence 1.2 
	//(SET UP MENU, Large Menu with many items or with large items or with Large Alpha Identifier)
	//See ETSI TS 102 384 [26] in subclause 27.22.4.8.1.4.2, Expected Sequence 1.2.

	// setup large menu
	const TInt KNumItems3 = 30;

	TUint8 ids3[KNumItems3] = 
	 {0x50, 0x4f, 0x4e, 0x4d, 0x4c, 0x4b, 0x4a, 0x49, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41,
	  0x40, 0x3f, 0x3e, 0x3d, 0x3c, 0x3b, 0x3a, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33};
	TPtrC8 idNames3[KNumItems3] = 
	 {_L8("Zero"), _L8("One"), _L8("Two"), _L8("Three"), _L8("Four"), _L8("Five"), _L8("Six"), _L8("Seven"),
	 _L8("Eight"), _L8("Nine"), _L8("Alpha"), _L8("Bravo"), _L8("Charlie"), _L8("Delta"), _L8("Echo"),
	 _L8("Fox-trot"), _L8("Black"), _L8("Brown"), _L8("Red"), _L8("Orange"), _L8("Yellow"), _L8("Green"),
	 _L8("Blue"), _L8("Violet"), _L8("Grey"), _L8("White"), _L8("milli"), _L8("micro"), _L8("nano"), _L8("pico")};
	
	SetUpMenuL( cmdNumber, noPref, _L8("LargeMenu1"), KNumItems3, ids3, idNames3 );
		
	//generate the terminal response	
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();	

	// select "Orange"	0x3d
	SelectMenuItemL(ids3[20]);

	AssertMockLtsyStatusL();

	// setup another large menu	
	const TInt KNumItems4 = 7;	

	TUint8 ids4[KNumItems4] = {0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9};
	TPtrC8 idNames4[KNumItems4] = 
	 {_L8("1 Call Forward Unconditional"), 
	  _L8("2 Call Forward On User Busy"), 
	  _L8("3 Call Forward On No Reply"), 
	  _L8("4 Call Forward On User Not Reachable"), 
	  _L8("5 Barring Of All Outgoing Calls"), 
	  _L8("6 Barring Of All Outgoing Int Calls"), 
	  _L8("7 CLI Presentation")};
	
	SetUpMenuL( cmdNumber, noPref, _L8("LargeMenu2"), KNumItems4, ids4, idNames4 );
		
	//generate the terminal response	
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();	

	// select 0xfb
	SelectMenuItemL(ids4[4]);

	AssertMockLtsyStatusL();

	// one more large menu
	const TInt KNumItems5 = 1;	

	_LIT8(KSetupMenuTest8,"The SIM shall supply a set of menu items, which shall be integrated with the\
menu system (or other MMI facility) in order to give the user the opportunity to\
choose one of these menu items at his own discretion. Each item comprises a sh");
	
	TUint8 ids5[KNumItems5] = {0x01};
	TPtrC8 idNames5[KNumItems5] = {_L8("Y")};
	
	SetUpMenuL( cmdNumber, noPref, KSetupMenuTest8, KNumItems5, ids5, idNames5 );
		
	//generate the terminal response	
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();	

	// select 1
	SelectMenuItemL(ids5[0]);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this); 
	}


/**
@SYMTestCaseID BA-CSAT-SUM-SNSUMPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifySetUpMenuPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifySetUpMenuPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpMenuFU::TestNotifySetUpMenuPCmd0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	RBuf8 data;
	CleanupClosePushL(data);

	TUint8 pcmdCode = KSetUpMenu;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);

	RSat::TSetUpMenuV2 setupMenu;
	RSat::TSetUpMenuV2Pckg setpPck(setupMenu);
	TRequestStatus stat;
	iSat.NotifySetUpMenuPCmd(stat, setpPck);
	
	iSat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);
	
	User::WaitForRequest(stat);
	ASSERT_EQUALS(KErrCancel, stat.Int());

	// There is no implementation for RSat::NotifySetUpMenuPCmdCancel(), declared in etelsat.h
	ERR_PRINTF2(_L("<font color=Orange>$CSATKnownFailure: defect id = %d</font>"), 3);
	ASSERT_TRUE(EFalse);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}



/**
@SYMTestCaseID BA-CSAT-SUM-SNSUMPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifySetUpMenuPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifySetUpMenuPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpMenuFU::TestNotifySetUpMenuPCmd0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);

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
	
	TUint8 KPCmdNumber = 1;

	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KSetUpMenu); //ETLV_TypeOfCommand
	tlv.AddByte(0); //ETLV_CommandQualifier
	
	tlv.AddTag(KTlvDeviceIdentityTag); 
	tlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	tlv.AddByte(KDisplay); //ETLV_DestinationDeviceIdentity
	
	tlv.AddTag(KTlvAlphaIdentifierTag);
	_LIT8(KSetupMenuTest8,"Menu AlphaIdent");
	_LIT16(KSetupMenuTest16,"Menu AlphaIdent");
	tlv.AddData(KSetupMenuTest8);//ETLV_AlphaIdentifier

	const TUint8 KItemId = 77; 
	_LIT8(KItem8, "Item AlphaIdent1");	
	_LIT16(KItem16, "Item AlphaIdent1");	
	
	tlv.AddTag(KTlvItemTag);
	tlv.AddByte(KItemId); //item identifier
	tlv.AddData(KItem8);	
	
	TPtrC8 tlvDsc = tlv.End();
	TDesC8* tlvDscPtr = &tlvDsc;

	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	// tell Mock about forthcoming RSat::NotifySetUpMenuPCmd
	TUint8 pcmdCode = KSetUpMenu;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	data.Close();
 
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, KErrNone, data, KDelay);
	
	RSat::TSetUpMenuV2 setupMenu;
	RSat::TSetUpMenuV2 setupMenu2;
	RSat::TSetUpMenuV2Pckg setpPck(setupMenu);
	RSat::TSetUpMenuV2Pckg setpPck2(setupMenu2);
	TRequestStatus stat;
	TRequestStatus stat2;
	iSat.NotifySetUpMenuPCmd(stat, setpPck);
	sat2.NotifySetUpMenuPCmd(stat2, setpPck2);
	
	User::WaitForRequest(stat);
	ASSERT_EQUALS(KErrNone, stat.Int());

	ASSERT_EQUALS(KPCmdNumber, setupMenu.PCmdNumber());
	ASSERT_EQUALS(RSat::ENoSelectionPreference, setupMenu.iPreference);		
	ASSERT_EQUALS((TInt)EFalse, setupMenu.IsRemoveMenu());
				
	RSat::TItem item;
	ASSERT_EQUALS( KErrNone, setupMenu.GetItem( 1, item));
	ASSERT_EQUALS( KItemId, (item.iItemId));
	ASSERT_EQUALS(0, item.iItemString.Compare(KItem16));					
	ASSERT_EQUALS((TUint)1, setupMenu.NumberOfItems());		
	ASSERT_EQUALS(RSat::ENoHelpAvailable, setupMenu.iHelp);
	ASSERT_EQUALS( RSat::ENoIconId, setupMenu.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::ENoIconId, setupMenu.iIconListQualifier);

	ASSERT_EQUALS(0, setupMenu.iAlphaId.iAlphaId.Compare(KSetupMenuTest16));	
	ASSERT_EQUALS(RSat::EAlphaIdProvided, setupMenu.iAlphaId.iStatus);

	User::WaitForRequest(stat2);
	ASSERT_EQUALS(KErrNone, stat2.Int());

	ASSERT_EQUALS(KPCmdNumber, setupMenu2.PCmdNumber());
	ASSERT_EQUALS(RSat::ENoSelectionPreference, setupMenu2.iPreference);		
	ASSERT_EQUALS((TInt)EFalse, setupMenu2.IsRemoveMenu());
	ASSERT_EQUALS( KErrNone, setupMenu2.GetItem( 1, item));
	ASSERT_EQUALS( KItemId, item.iItemId);
	ASSERT_EQUALS(0, item.iItemString.Compare(KItem16));					
	ASSERT_EQUALS((TUint)1, setupMenu2.NumberOfItems());		
	ASSERT_EQUALS(RSat::ENoHelpAvailable, setupMenu2.iHelp);
	ASSERT_EQUALS( RSat::ENoIconId, setupMenu2.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::ENoIconId, setupMenu2.iIconListQualifier);
	ASSERT_EQUALS(0, setupMenu2.iAlphaId.iAlphaId.Compare(KSetupMenuTest16));	
	ASSERT_EQUALS(RSat::EAlphaIdProvided, setupMenu2.iAlphaId.iStatus);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); 	
	}


/**
* auxiliary function for testing different fonts in set up menu
* @param aFontParam - type of font for tvl text formatting
*/    
void CCSatSetUpMenuFU::CheckMenuFontL(TUint8 aFontParam)
	{
	TUint8 cmdNumber = 1;
	TUint8 noPref = 0;	
	const TInt KNumItems = 3;
	_LIT8(KMenu1, "Toolkit Menu 1");
	
	TUint8 ids[KNumItems] = {1, 2, 3};
	TPtrC8 idNames[KNumItems] = {_L8("Item 1"), _L8("Item 2"), _L8("Item 3")};
	TUint8 alphaFormatting[RSat::KTextAttributeBufSize] = 
		{
		0, 	//formatting position
		14, //text length
		aFontParam, 
		0xB4 //Dark Green Foreground(0x04), Bright Yellow Background(0xB0)
		};
	TUint8 itemsFormatting[KNumItems][RSat::KTextAttributeBufSize] = 
		{{ 0, 6, aFontParam ,0xB4 },
		 { 0, 6, aFontParam ,0xB4 },
		 { 0, 6, aFontParam ,0xB4 }};
	
	SetUpMenuL( cmdNumber, noPref, KMenu1, KNumItems, ids, idNames, NULL,
		RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 
		alphaFormatting, itemsFormatting );

		
	//generate the terminal response
	RSat::TSetUpMenuRspV1 resp;
	RSat::TSetUpMenuRspV1Pckg respPck(resp);
	
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(cmdNumber);
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);	
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();

	// select second item	
	SelectMenuItemL(ids[1]);   

	AssertMockLtsyStatusL();

	// second menu	
	TUint8 ids2[KNumItems] = {4, 5, 6};
	TPtrC8 idNames2[KNumItems] = {_L8("Item 4"), _L8("Item 5"), _L8("Item 6") };
	_LIT8(KMenu2, "Toolkit Menu 2");
	
	alphaFormatting[2] = 0x00;	// normal font
	itemsFormatting[0][2] = itemsFormatting[1][2] = itemsFormatting[2][2] = 0x00;

	SetUpMenuL( cmdNumber, noPref, KMenu2, KNumItems, ids2, idNames2, NULL,
		RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 
		alphaFormatting, itemsFormatting );

	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);	
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();

	// select second item	
	SelectMenuItemL(ids2[1]);   

	AssertMockLtsyStatusL();
	
	// first menu again
	alphaFormatting[2] = aFontParam;
	itemsFormatting[0][2] = itemsFormatting[1][2] = itemsFormatting[2][2] = aFontParam;
	
	SetUpMenuL( cmdNumber, noPref, KMenu1, KNumItems, ids, idNames, NULL,
		RSat::EIconQualifierNotSet, 0, RSat::EIconQualifierNotSet, NULL, 
		alphaFormatting, itemsFormatting );

	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);	
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();

	// select second item	
	SelectMenuItemL(ids[1]);   

	AssertMockLtsyStatusL();

	// third menu	
	TUint8 ids3[KNumItems] = {7, 8, 9};
	TPtrC8 idNames3[KNumItems] = {_L8("Item 7"), _L8("Item 8"), _L8("Item 9") };
	_LIT8(KMenu3, "Toolkit Menu 3");
	
	SetUpMenuL( cmdNumber, noPref, KMenu3, KNumItems, ids3, idNames3 );

	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu,
			 respPck, resp.iAdditionalInfo, resp.iGeneralResult);	
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();

	// select second item	
	SelectMenuItemL(ids3[1]);   

	AssertMockLtsyStatusL();
	}


/**
* End Proactive Sim Session 
*/    
void CCSatSetUpMenuFU::EndProactiveSimSessionL()
	{
	RBuf8 data;
	CleanupClosePushL(data);	
	
	TRequestStatus reqStatus;
	TRequestStatus mockLtsyStatus;
	
	TUint8 proactiveCommandType = KEndOfTheProactiveSession;
	
	//this is as a result of the NotifyProactiveSimSessionEnd call down to the LTSY,
	//this tells the LTSY we are interested in a Sim Session End event
	//CTSY -> LTSY
	TMockLtsyData1<TUint8> proactiveCommandTypeDataPack(proactiveCommandType);
	proactiveCommandTypeDataPack.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	data.Close();
	
	//the client side call to be notifed when the Proactive Sim Session has ended.
	iSat.NotifyProactiveSimSessionEnd(reqStatus);

	//sometime later the Sim Session ends, indicator up
	//LTSY -> CTSY

	TUint8 statusWord1 = KSw1NormalEnding;
	TUint8 statusWord2 = KSw2NormalEnding;

	TMockLtsyData2<TUint8,TUint8> notifyProactiveSimSessionEndDataPack(statusWord1,statusWord2);
	notifyProactiveSimSessionEndDataPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatNotifyProactiveSimSessionEndIndId,KErrNone,data, KDelay);
	
	//wait for the client side to be notified
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(1);
	}


/**
* prepare and call Set Sup Menu proactive command
* @param aCmdNum - command number
* @param aCmdQual - command details
* @param aAlpha - alpha identifier
* @param aNumItems - number of menu items
* @param aItemIds - array of menu items identifier
* @param aItemNames - array of menu items names
* @param aAction - array of next actions
* @param aIconQual - type of icon
* @param aIcon - icon itself
* @param aIconListQual - type of icons for menu items
* @param aIconList- array of icons for menu items
* @param aAlphaFormating- array of KTextAttributeBufSize bytes for alpha identifier formatting
* @param aItemFormating- array for item strings formating
*/    
void CCSatSetUpMenuFU::SetUpMenuL(TUint8 aCmdNum, 
						    TUint8 aCmdQual,
						   const TDesC8& aAlpha,  
						   TUint8 aNumItems, 
						   TUint8 aItemIds[], 
						   TPtrC8 aItemNames[],
						   TUint8 aAction[],
						   RSat::TIconQualifier aIconQual, 
						   TUint8 aIcon,						   
						   RSat::TIconQualifier aIconListQual,
						   TUint8 aIconList[],
						   TUint8 aAlphaFormating[], 
						   TUint8 aItemFormating[][RSat::KTextAttributeBufSize])
	{
	RBuf8 data;
	CleanupClosePushL(data);	

	// tell Mock about forthcoming RSat::NotifySetUpMenuPCmd
	TUint8 pcmdCode = KSetUpMenu;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data);
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(aCmdNum);//ETLV_CommandNumber
	tlv.AddByte(KSetUpMenu); //ETLV_TypeOfCommand	
	tlv.AddByte(aCmdQual); //ETLV_CommandQualifier
	
	tlv.AddTag(KTlvDeviceIdentityTag); 
	tlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	tlv.AddByte(KDisplay); //ETLV_DestinationDeviceIdentity

	if(0 < aAlpha.Size())
		{			
		tlv.AddTag(KTlvAlphaIdentifierTag);
		tlv.AddData(aAlpha);//ETLV_AlphaIdentifier
		}

	for( TInt i = 0; i < aNumItems; i++)
		{
		tlv.AddTag(KTlvItemTag);
		tlv.AddByte(aItemIds[i]); //item identifier
		tlv.AddData(aItemNames[i]);
		}

	if(aAction)
		{
		tlv.AddTag(KTlvItemsNextActionIndicatorTag);
				
		for( TInt i = 0; i < aNumItems; i++)
			{
			tlv.AddByte(aAction[i]); 
			}
		}

	// constants for coding icon details
	const TUint8 KSelfExp = 0x00;
	const TUint8 KNotSelfExp = 0x01;	
	
	if(RSat::EIconQualifierNotSet != aIconQual)
		{
		tlv.AddTag(KTlvIconIdentifierTag);
		tlv.AddByte((RSat::ESelfExplanatory == aIconQual) ? KSelfExp : KNotSelfExp);
		tlv.AddByte(aIcon); 
		}

	if(RSat::EIconQualifierNotSet != aIconListQual)
		{
		tlv.AddTag(KTlvItemIconIdentifierListTag);
		tlv.AddByte((RSat::ESelfExplanatory == aIconListQual) ? KSelfExp : KNotSelfExp);
		
		for( TInt i = 0; i < aNumItems; i++)
			{
			tlv.AddByte(aIconList[i]); 
			}		
		}

	if(aAlphaFormating)
		{
		tlv.AddTag(0x50);		
		tlv.AddByte(aAlphaFormating[0]); 	// text start
		tlv.AddByte(aAlphaFormating[1]); 	// text length
		tlv.AddByte(aAlphaFormating[2]); 	// formatting
		tlv.AddByte(aAlphaFormating[3]); 	// colors
		}
		
	if(aItemFormating)
		{
		tlv.AddTag(0x51);		
		
		for(TInt i = 0; i < aNumItems; i++)
			{
			tlv.AddByte(aItemFormating[i][0]); 
			tlv.AddByte(aItemFormating[i][1]); 
			tlv.AddByte(aItemFormating[i][2]); 
			tlv.AddByte(aItemFormating[i][3]); 				
			}
		}
				
	TPtrC8 tlvDsc = tlv.End();
	TDesC8* tlvDscPtr = &tlvDsc;
	
	data.Close();
	
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,KErrNone,data, KDelay);
	
	RSat::TSetUpMenuV5 setupMenu;
	RSat::TSetUpMenuV5Pckg setpPck(setupMenu);
	TRequestStatus requestStatus;
	
	iSat.NotifySetUpMenuPCmd(requestStatus, setpPck);
	
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(aCmdNum, setupMenu.PCmdNumber());
	
	if ( aCmdQual & KSelectionUsingSoftKey )
		{
		ASSERT_EQUALS(RSat::ESoftKeyPreferred, setupMenu.iPreference);
		}
	else
		{
		ASSERT_EQUALS(RSat::ENoSelectionPreference, setupMenu.iPreference);
		}	
		
	if(0 < aNumItems)	
		{
		ASSERT_EQUALS((TInt)EFalse, setupMenu.IsRemoveMenu());

		RBuf16 data16;
		CleanupClosePushL(data16);	
		
		for(TInt i = 0; i < aNumItems; i++)
			{
			RSat::TItem item;
			RSat::TTextAttribute attr;			
			ASSERT_EQUALS( KErrNone, setupMenu.GetItem( i + 1, item, attr));
			ASSERT_EQUALS( aItemIds[i], item.iItemId);

			data16.CreateL(aItemNames[i].Length());
			
			TSatUtility::SetAlphaId( aItemNames[i], data16);
			
			ASSERT_EQUALS(0, item.iItemString.Compare(data16));				
						
			if(aItemFormating)
				{
				ASSERT_EQUALS(RSat::ETextAttributeProvided, attr.iStatus);		
				ASSERT_EQUALS(aItemFormating[i][0], attr.iTextAttributeData[0]);	
				ASSERT_EQUALS(aItemFormating[i][1], attr.iTextAttributeData[1]);	
				ASSERT_EQUALS(aItemFormating[i][2], attr.iTextAttributeData[2]);	
				ASSERT_EQUALS(aItemFormating[i][3], attr.iTextAttributeData[3]);			
				}
			}
		CleanupStack::PopAndDestroy(1);					
		}
	else
		{
		ASSERT_EQUALS((TInt)ETrue, setupMenu.IsRemoveMenu());
		}
	
	ASSERT_EQUALS((TUint)aNumItems, setupMenu.NumberOfItems());	
	
	if ( aCmdQual & KHelpAvailabilityMask )
		{
		ASSERT_EQUALS(RSat::EHelpAvailable, setupMenu.iHelp);
		}
	else
		{
		ASSERT_EQUALS(RSat::ENoHelpAvailable, setupMenu.iHelp);
		}	
	
	if(aAlphaFormating)
		{
		ASSERT_EQUALS(RSat::ETextAttributeProvided, setupMenu.iTextAttribute.iStatus);		
		ASSERT_EQUALS(aAlphaFormating[0], setupMenu.iTextAttribute.iTextAttributeData[0]);	
		ASSERT_EQUALS(aAlphaFormating[1], setupMenu.iTextAttribute.iTextAttributeData[1]);	
		ASSERT_EQUALS(aAlphaFormating[2], setupMenu.iTextAttribute.iTextAttributeData[2]);	
		ASSERT_EQUALS(aAlphaFormating[3], setupMenu.iTextAttribute.iTextAttributeData[3]);			
		}
	else
		{
		ASSERT_EQUALS(RSat::ETextAttributeNotSet, setupMenu.iTextAttribute.iStatus);		
		}

	if(RSat::EIconQualifierNotSet != aIconQual)
		{			
		ASSERT_EQUALS( aIconQual, setupMenu.iIconId.iQualifier);
		ASSERT_EQUALS( aIcon, setupMenu.iIconId.iIdentifier);
		}
	else
		{
		ASSERT_EQUALS( RSat::ENoIconId, setupMenu.iIconId.iQualifier);
		}
	
	if(RSat::EIconQualifierNotSet != aIconListQual)
		{
		RSat::TItem item;
		TInt iconId;
		
		for( TInt i = 0; i < aNumItems; i++)
			{
			setupMenu.GetItemIcon(i+1, item, iconId);
			ASSERT_EQUALS((TInt)aIconList[i], iconId);
			}
		}
	else
		{
		ASSERT_EQUALS( RSat::ENoIconId, setupMenu.iIconListQualifier);
		}

	if(aAction)
		{
		RSat::TItem item;
		TInt action;
		
		for( TInt i = 0; i < aNumItems; i++)
			{
			setupMenu.GetItem(i+1, item, action);
			ASSERT_EQUALS((TInt)aAction[i], action);
			}
		}

	RBuf16 data16;
	CleanupClosePushL(data16);	
	
	data16.CreateL(aAlpha.Length());
		
	TSatUtility::SetAlphaId( aAlpha, data16);
	
	ASSERT_EQUALS(0, setupMenu.iAlphaId.iAlphaId.Compare(data16));	
	ASSERT_EQUALS(RSat::EAlphaIdProvided, setupMenu.iAlphaId.iStatus);

	CleanupStack::PopAndDestroy(2);
	}
	

/**
* prepare and make select menu call
* @param aItemId - item to select
* @param aRequestHelp - if help requested
* @param aErrorForExpect - error code to pass and expect
*/    
void CCSatSetUpMenuFU::SelectMenuItemL(TUint8 aItemId, TBool aRequestHelp, TInt aErrorForExpect )
	{
	RBuf8 data;
	CleanupClosePushL(data);	

	TTlv menuSelTlv;
	menuSelTlv.Begin( KBerTlvMenuSelectionTag );
	menuSelTlv.AddTag( KTlvDeviceIdentityTag );
	menuSelTlv.AddByte( KKeypad );
	menuSelTlv.AddByte( KSim );
	menuSelTlv.AddTag( KTlvItemIdentifierTag );
	menuSelTlv.AddByte( aItemId );
	if(aRequestHelp)	
		{
		menuSelTlv.AddTag( KTlvHelpRequestTag );
		}

    TPtrC8 menuSelDsc = menuSelTlv.End();
    TDesC8* menuSelPtr = &menuSelDsc;
    TMockLtsyData1<TDesC8*> menuSelDescPck(menuSelPtr);
    menuSelDescPck.SerialiseL(data);

	iMockLTSY.ExpectL(MLtsyDispatchSatMenuSelectionEnvelope::KLtsyDispatchSatMenuSelectionEnvelopeApiId, data, aErrorForExpect);	

	RSat::TMenuSelectionV1 menuSel;
	RSat::TMenuSelectionV1Pckg menuSelPckg(menuSel);
	
	menuSel.iItemId = aItemId;
	menuSel.iHelp = aRequestHelp ? RSat::EHelpRequested : RSat::EHelpNotRequested;

	TRequestStatus stat;

    iSat.MenuSelection(stat, menuSelPckg);
    User::WaitForRequest(stat);
    ASSERT_EQUALS(aErrorForExpect, stat.Int());
	
	CleanupStack::PopAndDestroy(1);
	}


/**
* auxiliary function for testing different languages in set up menu
* @param aTitle - menu header
* @param aItemNames - first menu strings
* @param aItemNames - second menu strings
*/    
void CCSatSetUpMenuFU::CheckForeignSetUpMenuL(const TDesC8& aTitle,  
						   TPtrC8 aItemNames[],
						   TPtrC8 aItemNames2[])
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	//(SET UP MENU and MENU SELECTION, without Help Request, Replace and Remove a Toolkit Menu)

	TUint8 cmdNumber = 1;
	TUint8 noPref(0);	// no selection preferences
	const TInt KNumItems = 4;
	
	TUint8 ids[KNumItems] = {1, 2, 3, 4};
	
	SetUpMenuL( cmdNumber, noPref, aTitle, KNumItems, ids, aItemNames );
		
	//generate the terminal response
	RSat::TSetUpMenuRspV1 resp;
	RSat::TSetUpMenuRspV1Pckg respPck(resp);
	
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(cmdNumber);
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu, 
			respPck, resp.iAdditionalInfo, resp.iGeneralResult);	
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();

	// select second item	
	SelectMenuItemL(ids[1]);
    
    // making second menu instead of first
	const TInt KNumItems2 = 2;

	TUint8 ids2[KNumItems2] = {11, 12};
	
	SetUpMenuL( cmdNumber, noPref, aTitle, KNumItems2, ids2, aItemNames2 );
    	
	//generate the terminal response	
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu,
			 respPck, resp.iAdditionalInfo, resp.iGeneralResult);	
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();

	// select second item	
	SelectMenuItemL(ids2[1]);

	AssertMockLtsyStatusL();

    // remove the menu    
	SetUpMenuL( cmdNumber, noPref, aTitle, 0, NULL, NULL );
	
	//generate the terminal response	
	GenerateTerminalResponseL(cmdNumber, KSetUpMenu, noPref, RSat::ESetUpMenu,
			 respPck, resp.iAdditionalInfo, resp.iGeneralResult);	
	AssertMockLtsyStatusL();

	// now end session		
	EndProactiveSimSessionL();
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this);	
	}

