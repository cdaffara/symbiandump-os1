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
// Test step definitions for the SmsMessaging functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestsmsmessaging.h"
#include "pcktcs.h"
#include "cctsyinidata.h"


	
CCTSYIntegrationTestSmsMessagingBase::CCTSYIntegrationTestSmsMessagingBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr),
	iSmsMessagingTsyTestHelper(*this), iSmsStoreTsyTestHelper(*this), iNetworkTsyTestHelper(*this),
	iPacketServiceTsyTestHelper(*this), iSimTsyTestHelper(*this), iCallControlTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestSmsMessagingBase::~CCTSYIntegrationTestSmsMessagingBase()
/*
 * Destructor
 */
	{
	}
	
void CCTSYIntegrationTestSmsMessagingBase::GetSmsServiceCentreAddressL(TPtrC &aServiceCentreAddress)
/*
 * Gets the service centre address
 * 
 * @param aServiceCentreAddress a reference to an descriptor that will hold the service address
 */
	{
	DEBUG_PRINTF1(_L("Attempting to get Service Centre Address"));
	
	RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// wait until we are on the network
	CHECK_EQUALS_L( iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone ), KErrNone, 
			_L("Network is unavailable") );
			
	// Determine the network we are on
	RMobilePhone::TMobilePhoneNetworkInfoV1 netInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg netInfoPkg(netInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	TInt err = iNetworkTsyTestHelper.GetCurrentNetworkL( phone, netInfoPkg, area, operatorName );
	
	CHECK_EQUALS_L( err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	// Identify the network from the short name, read the service centre number from ini file
	const TDesC* network = NULL;
	switch ( operatorName )
	{
	case EOperatorO2: 
		network = &KNetworkO2;
		break;
	case EOperatorVodafone:
		network = &KNetworkVodafone;
		break;
	case EOperatorNTN:
		network = &KNetworkNTN;
		break;
	case EOperatorOrange:
		network = &KNetworkOrange;
		break;
	case EOperatorTMobile:
		network = &KNetworkTMobile;
		break;
	case EOperatorUnknown:
	default:
		CHECK_TRUE_L( EFail, _L("Failed to identify current network"));
		break;
	}	
	
	// Get the service centre from the ini file
	TBool gotServiceCentre = GetStringFromConfig( KServiceCentreSection, *network, aServiceCentreAddress );
	
	CHECK_TRUE_L( gotServiceCentre, _L("Failed to read service centre from INI file"));
	}


void CCTSYIntegrationTestSmsMessagingBase::SetSmsBearerL(RMobileSmsMessaging &aSmsMessaging, RMobileSmsMessaging::TMobileSmsBearer aBearer)
/**
 * Ensure the SMS Bearer is set to the specified bearer.
 * CTSY does not support Getting the Sms Bearer.  Neither does it support being notified of a change
 * in the Sms Bearer.
 * 
 * @param aSmsMessaging a reference to the messeging session
 * @param aBearer the bearer
 */
	{
	
	TExtEtelRequestStatus setBearerStatus(aSmsMessaging, EMobileSmsMessagingSetMoSmsBearer);
	CleanupStack::PushL(setBearerStatus);
	aSmsMessaging.SetMoSmsBearer(setBearerStatus, aBearer);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(setBearerStatus, ETimeLong), KErrNone, 
		_L("RMobileSmsMessaging::SetMoSmsBearer timed-out"));

	//setBearerStatus
	CleanupStack::PopAndDestroy(&setBearerStatus);
	}


void CCTSYIntegrationTestSmsMessagingBase::GetSmsSendAttributesL(RMobileSmsMessaging::TMobileSmsSendAttributesV1 &aSendSmsAttributes,TBool aCallToSelf)
/**
 * Get the attributes for sending an Sms Message
 * 
 * @param aSendSmsAttributes the message attribute.
 * @param aCallToSelf ETrue if the calling number will be the ONStore number, EFalse if the number will be the SmsReceptor in the ini file.
 *
 */
	{
	RMobilePhone::TMobileAddress destinationNumber;
	if(aCallToSelf)
		{
		destinationNumber.iTelNumber = iOwnNumbersCache.OwnNumbers().OwnVoiceNumber();
		if(destinationNumber.iTelNumber.Length()==0)
			{
			DEBUG_PRINTF1(_L("Since ONStore fails, we read this from the ini file."));
			TPtrC mobileNumber;
			GetStringFromConfig(KIniOwnNumSection, KIniOwnVoiceNumber1, mobileNumber);
			destinationNumber.iTelNumber=mobileNumber;
			}
		}
	else
		{
		//Read the destination and number from ini file	
		TPtrC mobileNumber;
		GetStringFromConfig(ConfigSection(), _L("SmsReceptor"), mobileNumber);
		destinationNumber.iTelNumber=mobileNumber;
		}
	
	DEBUG_PRINTF2(_L("Calling to %S"),&destinationNumber.iTelNumber);
	destinationNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	destinationNumber.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
	aSendSmsAttributes.iDestination = destinationNumber;
		
	//Read the Service Centre from ini file based on network 
	RMobilePhone::TMobileAddress serviceCentre;
	TPtrC serviceCentreNumber;
	GetSmsServiceCentreAddressL(serviceCentreNumber);
	serviceCentre.iTelNumber = serviceCentreNumber;
	serviceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	serviceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
	aSendSmsAttributes.iGsmServiceCentre = serviceCentre;
	aSendSmsAttributes.iMore = EFalse;
	aSendSmsAttributes.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
	aSendSmsAttributes.iSubmitReport.SetLength(0);
	
	//Set iFlags to show which attributes are pressent
	aSendSmsAttributes.iFlags = RMobileSmsMessaging::KGsmServiceCentre | RMobileSmsMessaging::KSmsDataFormat | RMobileSmsMessaging::KRemotePartyInfo | RMobileSmsMessaging::KMoreToSend;
	
	}


RMobileSmsMessaging::TMobileSmsGsmTpdu CCTSYIntegrationTestSmsMessagingBase::CreateValidSmsMessageL(TDes8 &aText, 
		RMobileSmsMessaging::TMobileSmsSendAttributesV1& aSendSmsAttributes, 
		TSmsStoreTsyTestHelper::TDataCodingScheme aDataCodingScheme,
		TBool aIsToSelf,
		TBool aStatusReportRequest)
/**
 * Create an SMS PDU from a string
 * 
 * @param aText the message body.
 * @param aSendSmsAttributes the message attribute.
 * @param aDataCodingScheme the data coding scheme
 * @param aIsToSelf ETrue if the message should be sent into the own sim number, EFalse will be send to a ini file specific number
 * @param aStatusReportRequest will be true if the TP-SRI bit (status request indicator) should be on.
 *
 * @return - The constructed message PDU.
 * 
 */
	{

	GetSmsSendAttributesL(aSendSmsAttributes,aIsToSelf);

	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu;
	TPtrC16 pNumber = aSendSmsAttributes.iDestination.iTelNumber;
	iSmsStoreTsyTestHelper.CreateValidSmsPdu(aText, pNumber, pdu, aDataCodingScheme, aStatusReportRequest);

	return pdu;
	}


TBool CCTSYIntegrationTestSmsMessagingBase::ComparePDUs(RMobileSmsMessaging::TMobileSmsGsmTpdu &aRecieved, RMobileSmsMessaging::TMobileSmsGsmTpdu &aSent)
/*
 * Helper function, which will compare two PDU's, a Deliver and submit and return false if they are not the same
 * It compare the message length, the message class and the message text.
 * 
 * @param aRecieved the PDU that was recieved.
 * @param aSent the PDU that was sent.
 *
 * @return - Returns ETrue iff both PDU contains the same information.
 * 
 */
	{
	TUint recivedSize = aRecieved.Length();
	TUint sentSize = aSent.Length();
	ASSERT_TRUE(recivedSize>3,_L("CCTSYIntegrationTestSmsMessagingBase::ComparePDUs illeagal PDU length"));
	ASSERT_TRUE(sentSize>2,_L("CCTSYIntegrationTestSmsMessagingBase::ComparePDUs illeagal PDU length"));
	if((recivedSize<=3)||(sentSize<=2))
		{
		return EFalse;
		}
	TUint sizeOfRecvTPOA = (TUint)aRecieved[1]/2 + 2; //number is given in 4 bits, another bit is for the length and another is for the type
	TUint sizeOfSentTPOA = (TUint)aSent[2]/2 + 2; //number is given in 4 bits, another bit is for the length and another is for the type
	
	ASSERT_TRUE(recivedSize>sizeOfRecvTPOA+12,_L("CCTSYIntegrationTestSmsMessagingBase::ComparePDUs illeagal PDU length"));
	ASSERT_TRUE(sentSize>sizeOfSentTPOA+7,_L("CCTSYIntegrationTestSmsMessagingBase::ComparePDUs illeagal PDU length"));
	if((recivedSize<=sizeOfRecvTPOA+12)||(sentSize<=sizeOfSentTPOA+7))
		{
		return EFalse;
		}
	ASSERT_EQUALS(aRecieved[sizeOfRecvTPOA+2],aSent[sizeOfSentTPOA+3],_L("RMobileSmsMessaging::ReceiveMessage did not returned the correct class message"));
	ASSERT_EQUALS(aRecieved[sizeOfRecvTPOA+10],aSent[sizeOfSentTPOA+5],_L("RMobileSmsMessaging::ReceiveMessage did not returned the same message length"));
	if((aRecieved[sizeOfRecvTPOA+2]!=aSent[sizeOfSentTPOA+3])||(aRecieved[sizeOfRecvTPOA+10]!=aSent[sizeOfSentTPOA+5]))
		{
		return EFalse;
		}
	TInt offset= 5-sizeOfSentTPOA+sizeOfRecvTPOA;
	TBuf<200> sentext;
	ASSERT_TRUE(iSmsStoreTsyTestHelper.ToText(sentext,aSent,EFalse),_L("RMobileSmsMessaging::ReceiveMessage sent unreadable text"));
	TBuf<200> rectext;
	ASSERT_TRUE(iSmsStoreTsyTestHelper.ToText(rectext,aRecieved,ETrue),_L("RMobileSmsMessaging::ReceiveMessage got unreadable text"));
	ASSERT_EQUALS(recivedSize,sentSize+offset,_L("RMobileSmsMessaging::ReceiveMessage got the wrong message length"));
	ASSERT_EQUALS_DES16(rectext,sentext,_L("RMobileSmsMessaging::ReceiveMessage did not returned the same text"));
	if(rectext!=sentext)
		{
		return EFalse;
		}
	return ETrue;
	}

CCTSYIntegrationTestSmsMessaging0001::CCTSYIntegrationTestSmsMessaging0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0001::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0001::~CCTSYIntegrationTestSmsMessaging0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0001
 * @SYMFssID BA/CTSY/SMSM-0001
 * @SYMTestCaseDesc Get message store information.
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::GetMessageStoreInfo
 * @SYMTestExpectedResults Pass - Number of message stores and message store info. returned are correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify number of message stores and message store details are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL( KMainServer, KMainPhone, KMainSmsMessaging);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RMobileSmsMessaging::EnumerateMessageStores returns aCount=1
	TInt numOfMessages = 0;
	smsMessaging.EnumerateMessageStores(numOfMessages);
	ASSERT_TRUE(numOfMessages > 0,_L("RMobileSmsMessaging::EnumerateMessageStores returned with error."));

	// Check RMobileSmsMessaging::GetMessageStoreInfo with aIndex=0 returns 
	// TMobilePhoneStoreInfoV1::iCaps of 
	//	RMobilePhoneStore::KCapsIndividualEntry 
	//     | RMobilePhoneStore::KCapsDeleteAll 
	//     | RMobilePhoneStore::KCapsReadAccess 
	//     | RMobilePhoneStore::KCapsWriteAccess 
	//     | RMobilePhoneStore::KCapsNotifyEvent 
	//     | RMobilePhoneStore::KCapsWholeStore 
	TInt index = 0;
	TExtEtelRequestStatus smsStoreInfoRequest(smsMessaging,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(smsStoreInfoRequest);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 smsStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg smsStorePkg(smsStoreInfo);
	smsMessaging.GetMessageStoreInfo(smsStoreInfoRequest, index, smsStorePkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(smsStoreInfoRequest, ETimeLong),
				KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	TUint32 expectedCaps = RMobilePhoneStore::KCapsIndividualEntry | RMobilePhoneStore::KCapsDeleteAll 
							   | RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess 
							   | RMobilePhoneStore::KCapsNotifyEvent | RMobilePhoneStore::KCapsWholeStore;
	ASSERT_BITS_SET(smsStoreInfo.iCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetSecurityCaps completed with incorrect caps."));


	// Check RMobileSmsMessaging::GetMessageStoreInfo with aIndex=0 returns TMobilePhoneStoreInfoV1::iName = KETelIccSmsStore
	// Check RMobileSmsMessaging::GetMessageStoreInfo with aIndex=0 returns TMobilePhoneStoreInfoV1::iTotalEntries > 0
	// Check RMobileSmsMessaging::GetMessageStoreInfo with aIndex=0 returns TMobilePhoneStoreInfoV1::iType = RMobilePhoneStore::EShortMessageStore
	// Check RMobileSmsMessaging::GetMessageStoreInfo with aIndex=0 returns TMobilePhoneStoreInfoV1::iUsedEntries >= 0
	DEBUG_PRINTF2(_L("SMS store name is %S"), &smsStoreInfo.iName);
	ASSERT_EQUALS_DES16(smsStoreInfo.iName, KETelIccSmsStore, _L("RMobileSmsMessaging::GetMessageStoreInfo returned with incorrect name."));
	ASSERT_TRUE(smsStoreInfo.iTotalEntries > 0, _L("RMobileSmsMessaging::GetMessageStoreInfo returned with incorrect total entries."));
	ASSERT_EQUALS(smsStoreInfo.iType, RMobilePhoneStore::EShortMessageStore, _L("RMobileSmsMessaging::GetMessageStoreInfo returned with incorrect type."));
	ASSERT_TRUE(smsStoreInfo.iUsedEntries >= 0, _L("RMobileSmsMessaging::GetMessageStoreInfo returned with incorrect used entries."));


	// Check RMobileSmsMessaging::GetMessageStoreInfo with aIndex=1 returns KErrArgument
	index = 1;
	TExtEtelRequestStatus smsStoreInfoRequest2(smsMessaging,EMobilePhoneGetPhoneStoreInfo);
	CleanupStack::PushL(smsStoreInfoRequest2);
	smsMessaging.GetMessageStoreInfo(smsStoreInfoRequest2, index, smsStorePkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(smsStoreInfoRequest2, ETimeLong),
		KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out."));
	ASSERT_EQUALS(smsStoreInfoRequest2.Int(), KErrArgument, 
		_L("RMobilePhone::GetPhoneStoreInfo returned with error."));			

	//
	// TEST END
	//

    StartCleanup();
	
	// smsStoreInfoRequest
	// smsStoreInfoRequest2
	CleanupStack::PopAndDestroy(2, &smsStoreInfoRequest);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0001");
	}



CCTSYIntegrationTestSmsMessaging0002::CCTSYIntegrationTestSmsMessaging0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0002::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0002::~CCTSYIntegrationTestSmsMessaging0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0002
 * @SYMFssID BA/CTSY/SMSM-0002
 * @SYMTestCaseDesc Set the SMS receive mode.
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::SetReceiveMode, RMobileSmsMessaging::NotifyReceiveModeChange
 * @SYMTestExpectedResults Pass - Receive mode is set when the mode is supported. KErrNotSupported is returned otherwise.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify receive mode change notification completes and receive mode is set correctly.
 *
 * @return - TVerdict code
 * 
 * $CTSYKnownFailure - When changing the receive mode AND a notification is requested of  its change 
 * (CTSY only supports nofifications when setting the receive mode to EReceiveUnstoredClientAck) 
 * a panic is raised and the board reboots. Adding logging into
 * the CSY it was found that line 617 of cmmsmstsy.cpp when
 * It was found that the panic is not caused when this line is executed, but a
 * little while after.
 */
	{

	//
	// SET UP
	//
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL( KMainServer, KMainPhone, KMainSmsMessaging);
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//


	// ===  Check default receive mode ===

	// Check RMobileSmsMessaging::GetReceiveMode returns aReceiveMode=RMobileSmsMessaging::EReceiveUnstoredClientAck
	DEBUG_PRINTF1(_L("Checking RMobileSmsMessaging::GetReceiveMode returns aReceiveMode=RMobileSmsMessaging::EReceiveUnstoredClientAck"));
	RMobileSmsMessaging::TMobileSmsReceiveMode receiveMode;	
	RMobileSmsMessaging::TMobileSmsReceiveMode getReceiveMode;	
	ASSERT_EQUALS(smsMessaging.GetReceiveMode(receiveMode), KErrNone,
		_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	ASSERT_EQUALS(receiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
		_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));
	

	// ===  Set receive mode to unsupported modes and check receive mode not changed ==

	// Set receive mode to EReceiveModeUnspecified with RMobileSmsMessaging::SetReceiveMode with aReceiveMode=EReceiveModeUnspecified 
	DEBUG_PRINTF1(_L("Setting receive mode to EReceiveModeUnspecified"));
	TExtEtelRequestStatus receiveModeRequest(smsMessaging,EMobileSmsMessagingSetReceiveMode);
	CleanupStack::PushL(receiveModeRequest);
	receiveMode = RMobileSmsMessaging::EReceiveModeUnspecified;
	smsMessaging.SetReceiveMode(receiveModeRequest, receiveMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveModeRequest, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SetReceiveMode timed out."));

	// Check KErrNotSupported is returned
	ASSERT_EQUALS(receiveModeRequest.Int(), KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect status."));

	// Check RMobileSmsMessaging::GetReceiveMode returns aReceiveMode=EReceiveUnstoredClientAck
	ASSERT_EQUALS(smsMessaging.GetReceiveMode(getReceiveMode), KErrNone,
		_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	ASSERT_EQUALS(getReceiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
		_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));

	
	
	
	
	

	// Set receive mode to EReceiveUnstoredPhoneAck with RMobileSmsMessaging::SetReceiveMode with aReceiveMode=EReceiveUnstoredPhoneAck 
	DEBUG_PRINTF1(_L("Setting receive mode to EReceiveUnstoredPhoneAck"));
	TExtEtelRequestStatus receiveModeRequest2(smsMessaging,EMobileSmsMessagingSetReceiveMode);
	CleanupStack::PushL(receiveModeRequest2);
	receiveMode = RMobileSmsMessaging::EReceiveUnstoredPhoneAck;
	smsMessaging.SetReceiveMode( receiveModeRequest2, receiveMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveModeRequest2, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SetReceiveMode timed out."));

	// Check KErrNotSupported is returned
	ASSERT_EQUALS(receiveModeRequest2.Int(), KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect status."));
		
	// Check RMobileSmsMessaging::GetReceiveMode returns aReceiveMode=EReceiveUnstoredClientAck
	ASSERT_EQUALS(smsMessaging.GetReceiveMode(getReceiveMode), KErrNone,
		_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	ASSERT_EQUALS(getReceiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
		_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));

		
	
	
	

	
	// Set receive mode to EReceiveStored with RMobileSmsMessaging::SetReceiveMode with aReceiveMode=EReceiveStored 
	DEBUG_PRINTF1(_L("Setting receive mode to EReceiveStored"));
	TExtEtelRequestStatus receiveModeRequest3(smsMessaging,EMobileSmsMessagingSetReceiveMode);
	CleanupStack::PushL(receiveModeRequest3);
	receiveMode = RMobileSmsMessaging::EReceiveStored;
	smsMessaging.SetReceiveMode( receiveModeRequest3, receiveMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveModeRequest3, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SetReceiveMode timed out."));
	
	// Check KErrNotSupported is returned
	ASSERT_EQUALS(receiveModeRequest3.Int(), KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect status."));

	// Check RMobileSmsMessaging::GetReceiveMode returns aReceiveMode=EReceiveUnstoredClientAck
	ASSERT_EQUALS(smsMessaging.GetReceiveMode(getReceiveMode), KErrNone,
		_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	ASSERT_EQUALS(getReceiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
		_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));


	
	
	
	
	
	// Set receive mode to EReceiveEither with RMobileSmsMessaging::SetReceiveMode with aReceiveMode=EReceiveEither 
	DEBUG_PRINTF1(_L("Setting receive mode to EReceiveEither"));
	TExtEtelRequestStatus receiveModeRequest4(smsMessaging,EMobileSmsMessagingSetReceiveMode);
	CleanupStack::PushL(receiveModeRequest4);
	receiveMode = RMobileSmsMessaging::EReceiveEither;
	smsMessaging.SetReceiveMode( receiveModeRequest4, receiveMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveModeRequest4, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SetReceiveMode timed out."));

	// Check KErrNotSupported is returned
	ASSERT_EQUALS(receiveModeRequest4.Int(), KErrNotSupported, 
		_L("RMobilePhone::GetPhoneStoreInfo completed with incorrect status."));

	// Check RMobileSmsMessaging::GetReceiveMode returns aReceiveMode=EReceiveUnstoredClientAck
	ASSERT_EQUALS(smsMessaging.GetReceiveMode(getReceiveMode), 
		KErrNone, _L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	ASSERT_EQUALS(getReceiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
		_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));



	
	
	
	// ===  Set receive mode to EReceiveUnstoredClientAck ===

	// ===  CTSY always completes notification even though the receive mode hasn't changed, we have to assume this is correct behaviour for now. ===
	DEBUG_PRINTF1(_L("Checking for Notification"));
	RMobileSmsMessaging::TMobileSmsReceiveMode notifyReceiveMode;	
	TExtEtelRequestStatus notifyRequestStatus(smsMessaging, EMobileSmsMessagingNotifyReceiveModeChange);
	CleanupStack::PushL(notifyRequestStatus); 
	smsMessaging.NotifyReceiveModeChange(notifyRequestStatus, notifyReceiveMode);

	// Set receive mode to  with RMobileSmsMessaging::SetReceiveMode with aReceiveMode=EReceiveUnstoredClientAck 
	DEBUG_PRINTF1(_L("Setting receive mode to EReceiveUnstoredClientAck"));
	TExtEtelRequestStatus receiveModeRequest5(smsMessaging,EMobileSmsMessagingSetReceiveMode);
	CleanupStack::PushL(receiveModeRequest5);
	receiveMode = RMobileSmsMessaging::EReceiveUnstoredClientAck;
	smsMessaging.SetReceiveMode( receiveModeRequest5, receiveMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveModeRequest5, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SetReceiveMode timed out."));
	
	// Check RMobileSmsMessaging::NotifyReceiveModeChange completes wtih aReceiveMode=EReceiveUnstoredClientAck
	iSmsMessagingTsyTestHelper.WaitForMobileSmsMessagingNotifyReceiveModeChange( smsMessaging, 
				notifyRequestStatus,
				notifyReceiveMode, 
				RMobileSmsMessaging::EReceiveUnstoredClientAck,
				KErrNone );	

	// Check RMobileSmsMessaging::GetReceiveMode returns aReceiveMode=EReceiveUnstoredClientAck
	ASSERT_EQUALS(smsMessaging.GetReceiveMode(getReceiveMode), KErrNone,
					_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	ASSERT_EQUALS(getReceiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
					_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));

	
	
	
	
	// ===  Cancel setting the receive mode ===
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10013);	
	ASSERT_EQUALS(1,0, _L("Test leaving because it will cause TSY crash, remove this when defect is fixed"));
	TEST_CHECK_POINT_L(_L("test leaving..."));
	
	// Set receive mode to  with RMobileSmsMessaging:: with aReceiveMode=EReceiveUnstoredClientAck 
	DEBUG_PRINTF1(_L("Setting receive mode to EReceiveUnstoredClientAck"));
	TExtEtelRequestStatus receiveModeRequest6(smsMessaging,EMobileSmsMessagingSetReceiveMode);
	CleanupStack::PushL(receiveModeRequest6);
	receiveMode = RMobileSmsMessaging::EReceiveUnstoredClientAck;
	smsMessaging.SetReceiveMode(receiveModeRequest6, receiveMode);


	// Cancel the request with CancelAsyncRequest(EMobileSmsMessagingSetReceiveMode)
 	DEBUG_PRINTF1(_L("Cancel EMobileSmsMessagingSetReceiveMode"));
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingSetReceiveMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveModeRequest6, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SetReceiveMode returned with error."));
	ASSERT_TRUE(receiveModeRequest6.Int() == KErrNone || receiveModeRequest6.Int() == KErrCancel, 
			_L("RMobileSmsMessaging::SetReceiveMode request completed with error."));


	// Check RMobileSmsMessaging::GetReceiveMode returns aReceiveMode=EReceiveUnstoredClientAck
	ASSERT_EQUALS(smsMessaging.GetReceiveMode(getReceiveMode), KErrNone,
		_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	ASSERT_EQUALS(getReceiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
		_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));
	

	//
	// TEST END
	//

    StartCleanup();
	
	//receiveModeRequest
	//receiveModeRequest2
	//receiveModeRequest3
	//receiveModeRequest4
	//notifyRequestStatus	
	//receiveModeRequest5
	//receiveModeRequest6
	CleanupStack::PopAndDestroy(7);				
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0002");
	}



CCTSYIntegrationTestSmsMessaging0003::CCTSYIntegrationTestSmsMessaging0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0003::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0003::~CCTSYIntegrationTestSmsMessaging0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0003
 * @SYMFssID BA/CTSY/SMSM-0003
 * @SYMTestCaseDesc Send an SMS message in each bearer mode.
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::SendMessage, RMobileSmsMessaging::GetMoSmsBearer, RMobileSmsMessaging::NotifyMoSmsBearerChange
 * @SYMTestExpectedResults Pass - SMS sent successfully using all supported bearers.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify bearer can be set and SMS can be sent.
 *
 * @return - TVerdict code
 * 
 */
	{

	//
	// SET UP
	//
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer, KMainPhone, KMainSmsMessaging);

    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);

	// Ensure RMobileSmsMessaging::GetMoSmsBearer returns aBearer= ESmsBearerCircuitOnly 
	RMobileSmsMessaging::TMobileSmsBearer bearer = RMobileSmsMessaging::ESmsBearerCircuitOnly;
	SetSmsBearerL(smsMessaging, bearer);


	//
	// SET UP END
	//

	StartTest();
	
	//
	// TEST START
	//
	
	
	// Send an SMS message with RMobileSmsMessaging::SendMessage. Check no error is returned in aMsgAttributes.iSubmitReport 
	TBuf8<140> text(_L8("Hello from CTSY"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes);	

	
	// Send an SMS message with RMobileSmsMessaging::SendMessage. Check no error is returned in aMsgAttributes.iSubmitReport 
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
		_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));
	
		
	//Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	
		

	
	
	// Set the MO SMS bearer to ESmsBearerPacketOnly with RMobileSmsMessaging::SetMoSmsBearer
	DEBUG_PRINTF1(_L("Set the MO SMS bearer to ESmsBearerPacketOnly"));
	bearer = RMobileSmsMessaging::ESmsBearerPacketOnly;
	SetSmsBearerL(smsMessaging, bearer);
	
	
	//Check the status of the registration
	DEBUG_PRINTF1(_L("Checking the state of the packet service"));
	RPacketService &packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	TExtEtelRequestStatus getNtwkRegStatus(packetService, EPacketGetNtwkRegStatus);
	CleanupStack::PushL(getNtwkRegStatus);	
	RPacketService::TRegistrationStatus registrationStatus;
	packetService.GetNtwkRegStatus(getNtwkRegStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNtwkRegStatus, ETimeMedium),
			KErrNone, _L("RPacketService::GetNtwkRegStatus timed out."));
	DEBUG_PRINTF2(_L("NETWORK REGISTRATION STATUS = %d"), registrationStatus);

	//Lets get the status of the packet service
	TExtEtelRequestStatus statusRequestStatus(packetService, EPacketGetStatus);
	CleanupStack::PushL(statusRequestStatus);
	RPacketService::TStatus packetStatus;
	TInt err=packetService.GetStatus(packetStatus);
	ASSERT_EQUALS(err, KErrNone, _L("RPacketService::GetStatus returned error."));
	DEBUG_PRINTF2(_L("Packet status returned %d"), packetStatus);
	
	// Attach a packet service here: 
	DEBUG_PRINTF1(_L("Set attatch mode to RPacketService::EAttachWhenNeeded"));
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);	
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenNeeded);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), 
		KErrNone, _L("RPacketService::SetAttachMode timed-out"));
	ASSERT_EQUALS(setAttachModeStatus.Int(), KErrNone, 
		_L("RPacketService::SetAttachMode returned with an error"));

	
	//Setup a notifier to be notified of a change in packet service status change
	TExtEtelRequestStatus notifyStatusChange(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusChange);
	RPacketService::TStatus notifiedPacketStatus;
	packetService.NotifyStatusChange(notifyStatusChange, notifiedPacketStatus);
	
	
	//Attach
	TExtEtelRequestStatus attachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(attachModeStatus);	
	packetService.Attach(attachModeStatus);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(attachModeStatus, ETimeLong), 
		KErrNone, _L("RPacketService::Attach timed-out"));
	ASSERT_EQUALS(attachModeStatus.Int(), KErrNone, 
		_L("RPacketService::Attach returned with an error"));

	
	// Check RPacketService::NotifyStatusChange() completes wtih aStatus=EStatusAttached
	RPacketService::TStatus expectedStatus=RPacketService::EStatusAttached;
	iPacketServiceTsyTestHelper.WaitForPacketServiceNotifyStatusChange( 
			packetService, 
			notifyStatusChange,
			notifiedPacketStatus, 
			expectedStatus,
			KErrNone );	
	
	//Send SMS
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes2;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg2(sendSmsAttributes2);
	GetSmsSendAttributesL(sendSmsAttributes2);
	TExtEtelRequestStatus sendSmsRequestStatus2(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus2);
	smsMessaging.SendMessage(sendSmsRequestStatus2, pdu, sendSmsAttributesPckg2);
    // $CTSYProblem - Sending a SMS message over packetService times out.
    //  	- The message is sent from CTSY to LTSY.  From the logs it is apparent
    //      - that the message is trying to be sent three times.  The sending is
    // 		- unsuccessful.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus2, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus2.Int(), KErrGeneral,
		_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Detach from the packet service
	TExtEtelRequestStatus detatchPacketStatus(packetService, EPacketContextDeactivate);
	CleanupStack::PushL(detatchPacketStatus);	
	packetService.Detach(detatchPacketStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(detatchPacketStatus, ETimeLong), 
			KErrNone, _L("RPacketService::Detach timed-out"));
	ASSERT_EQUALS(detatchPacketStatus.Int(), KErrNone, 
			_L("RPacketService::Detach returned with an error"));


	
	
	// Set the MO SMS bearer to ESmsBearerPacketPreferred with RMobileSmsMessaging::SetMoSmsBearer 
	DEBUG_PRINTF1(_L("Set the MO SMS bearer to ESmsBearerPacketPreferred"));
	bearer = RMobileSmsMessaging::ESmsBearerPacketPreferred;
	SetSmsBearerL(smsMessaging, bearer);

	// Send an SMS message with RMobileSmsMessaging::SendMessage. Check no error is returned in aMsgAttributes.iSubmitReport
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes3;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg3(sendSmsAttributes3);
	GetSmsSendAttributesL(sendSmsAttributes3);
	TExtEtelRequestStatus sendSmsRequestStatus3(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus3);
	smsMessaging.SendMessage(sendSmsRequestStatus3, pdu, sendSmsAttributesPckg3);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus3, ETimeMedium),
                  KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus3.Int(), KErrNone, 
                  _L("RMobileSmsMessaging::SendMessage completed with incorrect status"));
	
	//Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes3.iSubmitReport.Length() > 0, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes3.iFlags,wantedBits,KNoUnwantedBits, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));		
	






	// Set the MO SMS bearer to ESmsBearerCircuitPreferred with RMobileSmsMessaging::SetMoSmsBearer 
	DEBUG_PRINTF1(_L("Set the MO SMS bearer to ESmsBearerCircuitPreferred"));
	bearer = RMobileSmsMessaging::ESmsBearerCircuitPreferred;
	SetSmsBearerL(smsMessaging, bearer);

	// Send an SMS message with RMobileSmsMessaging::SendMessage. Check no error is returned in aMsgAttributes.iSubmitReport 
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes4;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg4(sendSmsAttributes4);
	GetSmsSendAttributesL(sendSmsAttributes4);
	TExtEtelRequestStatus sendSmsRequestStatus4(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus4);
	smsMessaging.SendMessage(sendSmsRequestStatus4, pdu, sendSmsAttributesPckg4);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus4, ETimeMedium),
                  KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus4.Int(), KErrNone, 
                  _L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	//Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes4.iSubmitReport.Length() > 0, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes4.iFlags,wantedBits,KNoUnwantedBits, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));

	//
	// TEST END
	//

    //StartCleanup();
	/*	sendSmsRequestStatus
	   	getNtwkRegStatus
	   	statusRequestStatus
    	setAttachModeStatus
    	notifyStatusChange
    	attachModeStatus
    	sendSmsRequestStatus2
    	detatchPacketStatus
    	sendSmsRequestStatus3
    	sendSmsRequestStatus4 */
	
    CleanupStack::PopAndDestroy(10, &sendSmsRequestStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0003");
	}



CCTSYIntegrationTestSmsMessaging0004::CCTSYIntegrationTestSmsMessaging0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0004::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0004::~CCTSYIntegrationTestSmsMessaging0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0004
 * @SYMFssID BA/CTSY/SMSM-0004
 * @SYMTestCaseDesc Store a valid service centre address and send message using that service centre.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify service centre address stored and message sent successfully.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Send an SMS message using RMobileSmsMessaging::SendMessage with a valid service centre number in TMobileSmsAttributesV1::iGsmServiceCentre (ensure TMobileSmsAttributeFlags is set correctly). Check no error is returned in aMsgAttributes.iSubmitReport 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0004");
	}



CCTSYIntegrationTestSmsMessaging0005::CCTSYIntegrationTestSmsMessaging0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0005::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0005::~CCTSYIntegrationTestSmsMessaging0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0005
 * @SYMFssID BA/CTSY/SMSM-0005
 * @SYMTestCaseDesc Send message with different validity periods
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::StoreSmspListL
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Store SMS parameters with RMobileSmsMessaging::StoreSmspListL with one entry with TMobileSmspEntryV1::iValidityPeriod of 1 day 

	// Send an SMS message with RMobileSmsMessaging::SendMessage. Check no error is returned in aMsgAttributes.iSubmitReport 

	// Store SMS parameters with RMobileSmsMessaging::StoreSmspListL with one entry with TMobileSmspEntryV1::iValidityPeriod of 3 days 

	// Send an SMS message with RMobileSmsMessaging::SendMessage. Check no error is returned in aMsgAttributes.iSubmitReport 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0005");
	}



CCTSYIntegrationTestSmsMessaging0006::CCTSYIntegrationTestSmsMessaging0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0006::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0006::~CCTSYIntegrationTestSmsMessaging0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0006
 * @SYMFssID BA/CTSY/SMSM-0006
 * @SYMTestCaseDesc Send SMS with request for status report.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// wait until we are on the network
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Send an SMS message containing also a request for a status report with RMobileSmsMessaging::SendMessage.
	TBuf8<140> text(_L8("BA-CTSY-INT-SMSM-0006"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes, 
			TSmsStoreTsyTestHelper::ENoDataCodingScheme, EFalse, ETrue);	
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL( KMainServer, KMainPhone, KMainSmsMessaging);
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
		_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

    // Check no error is returned in aMsgAttributes.iSubmitReport 
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	sendSmsRequestStatus
	CleanupStack::PopAndDestroy(&sendSmsRequestStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0006");
	}



CCTSYIntegrationTestSmsMessaging0007::CCTSYIntegrationTestSmsMessaging0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0007::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0007::~CCTSYIntegrationTestSmsMessaging0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0007
 * @SYMFssID BA/CTSY/SMSM-0007
 * @SYMTestCaseDesc Send an empty SMS message.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// wait until we are on the network
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Send an empty SMS message with RMobileSmsMessaging::SendMessage.
	TBuf8<140> text(_L8(""));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes);	
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL( KMainServer, KMainPhone, KMainSmsMessaging);
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
		_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));
    
    // Check no error is returned in aMsgAttributes.iSubmitReport 
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	sendSmsRequestStatus
	CleanupStack::PopAndDestroy(&sendSmsRequestStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0007");
	}



CCTSYIntegrationTestSmsMessaging0008::CCTSYIntegrationTestSmsMessaging0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0008::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0008::~CCTSYIntegrationTestSmsMessaging0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0008
 * @SYMFssID BA/CTSY/SMSM-0008
 * @SYMTestCaseDesc Send a 160 character SMS when using MMI language of Default 7-bit alphabet
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::SetMoSmsBearer
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message sent.
 *
 * @return - TVerdict code
 * 
 * $CTSYProblem - There is nothing wrong with the procedure:  I used a notifer to for 
 *  a change in the bearer mode.  The notifier is not supported in CTSY: CTSY returns 
 *  KErrNotSupported for CMmSmsTsy::NotifyMoSmsBearerChange
 */
	{

	//
	// SET UP
	//
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL( KMainServer, KMainPhone, KMainSmsMessaging);

    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Send a 160 character SMS message with RMobileSmsMessaging::SendMessage encoded using default 7bit alphabet. Check no error is returned in aMsgAttributes.iSubmitReport 
	TBuf8<160> text(_L8("Hello Mobile. This is a 160 Character message.  It is going to fill up the entire length of of a PDU.  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes);	

	//Send the PDU
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);

	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
		_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	//Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			



	
	// ===  Send another SMS immediately after ===

	// Send a 160 character SMS message with RMobileSmsMessaging::SendMessage encoded using default 7bit alphabet.  Check no error is returned in aMsgAttributes.iSubmitReport	
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes2;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg2(sendSmsAttributes2);
	GetSmsSendAttributesL(sendSmsAttributes2);
	TExtEtelRequestStatus sendSmsRequestStatus2(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus2);
	sendSmsAttributes.iSubmitReport.SetLength(0);
	smsMessaging.SendMessage(sendSmsRequestStatus2, pdu, sendSmsAttributesPckg2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus2, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus2.Int(), KErrNone, 
		_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));
	
	//Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes2.iSubmitReport.Length() > 0, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	ASSERT_BITS_SET(sendSmsAttributes2.iFlags,wantedBits,KNoUnwantedBits, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			


	// ===  Send an SMS to an international number ===

	// Send a 160 character SMS message with RMobileSmsMessaging::SendMessage encoded using default 7bit alphabet to an international number. Check no error is returned in aMsgAttributes.iSubmitReport 
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes3;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg3(sendSmsAttributes3);
	GetSmsSendAttributesL(sendSmsAttributes3);
	sendSmsAttributes3.iDestination.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	TExtEtelRequestStatus sendSmsRequestStatus3(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus3);
	sendSmsAttributes.iSubmitReport.SetLength(0);
	smsMessaging.SendMessage(sendSmsRequestStatus3, pdu, sendSmsAttributesPckg3);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus3, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus3.Int(), KErrNone, 
		_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));
	
	//Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes3.iSubmitReport.Length() > 0, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	ASSERT_BITS_SET(sendSmsAttributes3.iFlags,wantedBits,KNoUnwantedBits, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			
	
	
	
	// ===  Cancel sending an SMS message ===

	// Set the bearer to RMobileSmsMessaging::ESmsBearerCircuitPreferred with RMobileSmsMessaging::SetMoSmsBearer 
	RMobileSmsMessaging::TMobileSmsBearer bearer;
	TExtEtelRequestStatus notifyBearerChangeStatus(smsMessaging, EMobileSmsMessagingNotifyMoSmsBearerChange);
	CleanupStack::PushL(notifyBearerChangeStatus);
	smsMessaging.NotifyMoSmsBearerChange(notifyBearerChangeStatus, bearer);
	
	TExtEtelRequestStatus setBearerStatus(smsMessaging, EMobileSmsMessagingSetMoSmsBearer);
	CleanupStack::PushL(setBearerStatus);
	smsMessaging.SetMoSmsBearer(setBearerStatus, RMobileSmsMessaging::ESmsBearerCircuitPreferred);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setBearerStatus, ETimeLong), KErrNone, 
			_L("RMobileSmsMessaging::SetMoSmsBearer timed-out"));
	
	// Check RMobileSmsMessaging::NotifyMoSmsBearerChange completes with KErrNotSupported
	bearer = RMobileSmsMessaging::ESmsBearerCircuitPreferred;
	iSmsMessagingTsyTestHelper.WaitForMobileSmsMessagingNotifyMoSmsBearerChange(smsMessaging,
			notifyBearerChangeStatus, bearer, RMobileSmsMessaging::ESmsBearerCircuitPreferred, KErrNotSupported);


	
	// Send an SMS message with RMobileSmsMessaging::SendMessage 
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes4;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg4(sendSmsAttributes4);
	GetSmsSendAttributesL(sendSmsAttributes4);	
	TExtEtelRequestStatus sendSmsRequestStatus4(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus4);
	sendSmsAttributes.iSubmitReport.SetLength(0);
	smsMessaging.SendMessage(sendSmsRequestStatus4, pdu, sendSmsAttributesPckg4);

	
	// Cancel the request immediately with CancelAsyncRequest(EMobileSmsMessagingSendMessage) 
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingSendMessage);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus4, ETimeMedium), KErrNone, 
			_L("RMobileSmsMessaging::SendMessage timed-out"));

	
	// Check RMobileSmsMessaging::SendMessage completed with KErrCancel or KErrNone
	ASSERT_TRUE(sendSmsRequestStatus4.Int() == KErrNone || sendSmsRequestStatus4.Int() == KErrCancel, 
			_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));
	
	
	//
	// TEST END
	//

    StartCleanup();
	
    // sendSmsRequestStatus
    // sendSmsRequestStatus2
    // sendSmsRequestStatus3
    // notifyBearerChangeStatus
    // setBearerStatus
    // sendSmsRequestStatus4

	CleanupStack::PopAndDestroy(6);
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0008");
	}



CCTSYIntegrationTestSmsMessaging0009::CCTSYIntegrationTestSmsMessaging0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0009::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0009::~CCTSYIntegrationTestSmsMessaging0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0009
 * @SYMFssID BA/CTSY/SMSM-0009
 * @SYMTestCaseDesc Send a 160 character SMS when using MMI language of UCS2 alphabet & message content is default 7-bit alphabet
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Send a 160 character SMS message with RMobileSmsMessaging::SendMessage encoded using MMI language of UCS2 alphabet & message content is default 7-bit alphabet. Check no error is returned in aMsgAttributes.iSubmitReport 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0009");
	}



CCTSYIntegrationTestSmsMessaging0010::CCTSYIntegrationTestSmsMessaging0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0010::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0010::~CCTSYIntegrationTestSmsMessaging0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0010
 * @SYMFssID BA/CTSY/SMSM-0010
 * @SYMTestCaseDesc Send an SMS of length greater than 160 characters encoded with default 7-bit alphabet
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Send a 220 character SMS message with RMobileSmsMessaging::SendMessage encoded using default 7bit alphabet to the board. Check no error is returned in aMsgAttributes.iSubmitReport 

	// Check the full message is received with RMobileSmsMessaging::ReceiveMessage

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0010");
	}



CCTSYIntegrationTestSmsMessaging0011::CCTSYIntegrationTestSmsMessaging0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0011::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0011::~CCTSYIntegrationTestSmsMessaging0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0011
 * @SYMFssID BA/CTSY/SMSM-0011
 * @SYMTestCaseDesc Send an SMS encoded with 7-bit alphabet which is over 140 Bytes
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - SMS sent successfully
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Send an SMS message that contains over 140 bytes of content encoded with the default 7 bit alphabet with RMobileSmsMessaging::SendMessage 

	// Check no error is returned in aMsgAttributes.iSubmitReport

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0011");
	}



CCTSYIntegrationTestSmsMessaging0012::CCTSYIntegrationTestSmsMessaging0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0012::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0012::~CCTSYIntegrationTestSmsMessaging0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0012
 * @SYMFssID BA/CTSY/SMSM-0012
 * @SYMTestCaseDesc Send an SMS over 70 characters in length encoded with UCS-2 alphabet
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Send an SMS over 70 characters in length encoded with UCS-2 alphabet with RMobileSmsMessaging::SendMessage 

	// Check no error is returned in aMsgAttributes.iSubmitReport

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0012");
	}



CCTSYIntegrationTestSmsMessaging0013::CCTSYIntegrationTestSmsMessaging0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0013::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0013::~CCTSYIntegrationTestSmsMessaging0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0013
 * @SYMFssID BA/CTSY/SMSM-0013
 * @SYMTestCaseDesc Send an SMS with over 140 bytes of content when UCS-2 alphabet & default & extended default 7-bit alphabet is used to encode it
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Aim is to send a long message with mixed UCS-2 and default and extended default 7 bit alphabets ===

	// Send the SMS message with RMobileSmsMessaging::SendMessage 

	// Check no error is returned in aMsgAttributes.iSubmitReport

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0013");
	}



CCTSYIntegrationTestSmsMessaging0014::CCTSYIntegrationTestSmsMessaging0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0014::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0014::~CCTSYIntegrationTestSmsMessaging0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0014
 * @SYMFssID BA/CTSY/SMSM-0014
 * @SYMTestCaseDesc Send an EMS (Enhanced Messaging Service) 
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  For encoding an EMS see spec 3GPP TS 23.040, GSM 04.11 ===

	// ===  Check the Telephony and STIF code we got to see if someone has already written code ===

	// ===  to encode an EMS.   (***remove this comment from your test when you have done it) ===

	// Send an EMS (e.g. one containing formatted text) using RMobileSmsMessaging::SendMessage 

	// Check no error is returned in aMsgAttributes.iSubmitReport

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0014");
	}



CCTSYIntegrationTestSmsMessaging0015::CCTSYIntegrationTestSmsMessaging0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0015::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0015::~CCTSYIntegrationTestSmsMessaging0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0015
 * @SYMFssID BA/CTSY/SMSM-0015
 * @SYMTestCaseDesc Send an SMS during a call and during active packet connection
 * @SYMTestPriority High
 * @SYMTestActions RCall::NotifyStatusChange, RPacketContext::NotifyStatusChange, RPacketContext::Activate, RCall::HangUp, RPacketContext::Deactivate, RCall::Dial
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Get voice line 1. 
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer,KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KVoiceLine,KCall1 );	

	// wait until we are on the network
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Part one - Send SMS during CS call ===

	// post notifier for RCall::NotifyStatusChange
	RCall::TStatus callStatus;
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus (call1,&RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);

	// Dial a number that answers with RCall::Dial 
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse,			  
			_L("CTestStep::GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	call1.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
			_L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  
			_L("RCall::Dial returned with an error"));
    
	// Check RCall::NotifyStatusChange completes with EStatusConnected.
	const TInt wantedStatus = KErrNone;
	RCall::TStatus expectedCallStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 wantedStatus);
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
    
	// Send the SMS message with RMobileSmsMessaging::SendMessage 
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer, KMainPhone, KMainSmsMessaging);
	TBuf8<140> text(_L8("BA-CTSY-INT-SMSM-0015.1"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes);	
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
		_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Hang up call with RCall::HangUp 
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1,&RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone,  
			_L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone,  
			_L("RCall::HangUp returned an error"));

	// Check RCall::NotifyStatusChange completes with EStatusIdle.
	expectedCallStatus = RCall::EStatusIdle;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 wantedStatus);

	// ===  Part two - Send SMS when packet context is active ===

	// Activate a primary packet context with RPacketContext::Activate 
	// Check RPacketContext::NotifyStatusChange completes with EStatusActive
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
    iPacketServiceTsyTestHelper.ActivatePrimaryPacketContextL(phone,packetContext);

	// Send the SMS message with RMobileSmsMessaging::SendMessage 
	TBuf8<140> text2(_L8("BA-CTSY-INT-SMSM-0015.2"));
	pdu = CreateValidSmsMessageL(text2, sendSmsAttributes);	
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
		_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(packetContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Deactivate packet context with RPacketContext::Deactivate 
	TExtEtelRequestStatus contextDeactivateStatus(packetContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	packetContext.Deactivate(contextDeactivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));

	// Check RPacketContext::NotifyStatusChange completes with EStatusInactive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTsyTestHelper.WaitForPacketContextNotifyStatusChange(
				packetContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	contextDeactivateStatus
	//	notifyContextStChStatus
	//	hangUpStatus
	//	sendSmsRequestStatus
	//	dialStatus
	//	notifyStatusChangeStatus
	CleanupStack::PopAndDestroy(6,&notifyStatusChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0015");
	}



CCTSYIntegrationTestSmsMessaging0016::CCTSYIntegrationTestSmsMessaging0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0016::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0016::~CCTSYIntegrationTestSmsMessaging0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0016
 * @SYMFssID BA/CTSY/SMSM-0016
 * @SYMTestCaseDesc Send SMS when out of coverage
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyNetworkRegistrationStatusChange
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify message is not sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Remove the aerial to ensure phone is in a black spot 
	DisplayUserInteractionPromptL(_L("Remove the aerial to ensure phone is in a black spot."), ETimeMedium);

	// Ensure RMobilePhone::GetNetworkRegistrationStatus returns ERegistrationUnknown | ENotRegisteredNoService | ENotRegisteredSearching 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	TExtEtelRequestStatus getNetworkRegistrationStatusStatus(phone,EMobilePhoneGetNetworkRegistrationStatus);
	CleanupStack::PushL(getNetworkRegistrationStatusStatus);
    RMobilePhone::TMobilePhoneRegistrationStatus phoneRegistrationStatus;
    phone.GetNetworkRegistrationStatus(getNetworkRegistrationStatusStatus,phoneRegistrationStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getNetworkRegistrationStatusStatus, ETimeShort), KErrNone, 
		_L("RMobilePhone::GetNetworkRegistrationStatus timed-out"));
    CHECK_TRUE_L(((phoneRegistrationStatus == RMobilePhone::ERegistrationUnknown) ||
                  (phoneRegistrationStatus == RMobilePhone::ENotRegisteredNoService) ||
                  (phoneRegistrationStatus == RMobilePhone::ENotRegisteredSearching)),
                 _L("RMobilePhone::GetNetworkRegistrationStatus returned with incorrect status."));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Attempt to send the SMS message with RMobileSmsMessaging::SendMessage 
	TBuf8<140> text(_L8("BA-CTSY-INT-SMSM-0016.1"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes);	
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer, KMainPhone, KMainSmsMessaging);
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
		_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check an error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

    // Post Network status change Notifyer to the network
	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus;
	TExtEtelRequestStatus reqStatus(phone, EMobilePhoneNotifyNetworkRegistrationStatusChange );
	CleanupStack::PushL(reqStatus);
	phone.NotifyNetworkRegistrationStatusChange(reqStatus, registrationStatus);
    
    
	// Put the aerial back  
	DisplayUserInteractionPromptL(_L("Put the aerial back."), ETimeMedium);

	// Check RMobilePhone::NotifyNetworkRegistrationStatusChange completes with aStatus=ERegisteredOnHomeNetwork
    iNetworkTsyTestHelper.WaitForMobilePhoneNotifyNetworkRegistrationStatusChange(phone,
                                                                                  reqStatus,
                                                                                  registrationStatus,
                                                                                  RMobilePhone::ERegisteredOnHomeNetwork,
                                                                                  KErrNone );

	// Send the SMS message again with RMobileSmsMessaging::SendMessage 
	TBuf8<140> text2=_L8("BA-CTSY-INT-SMSM-0016.2");
	pdu = CreateValidSmsMessageL(text2, sendSmsAttributes);	
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
		_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
		_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));		

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	reqStatus
	//	sendSmsRequestStatus
	//	getNetworkRegistrationStatusStatus
	CleanupStack::PopAndDestroy(3,&getNetworkRegistrationStatusStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0016");
	}



CCTSYIntegrationTestSmsMessaging0017::CCTSYIntegrationTestSmsMessaging0017(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0017::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0017::~CCTSYIntegrationTestSmsMessaging0017()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0017::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0017
 * @SYMFssID BA/CTSY/SMSM-0017
 * @SYMTestCaseDesc Receive a class 2 SMS message
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify class 2 message is received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that the SMS store on the SIM is not full 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL( KMainServer, KMainPhone, KMainSmsMessaging);
	CHECK_TRUE_L(iSimTsyTestHelper.EnsureEmptyStoreSpaceL(smsStore,KETelIccSmsStore,storeInfo,phone)>=0,_L("TSimTsyTestHelper::EnsureEmptyStoreSpaceL returned an error"));

	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 
	RMobileSmsMessaging::TMobileSmsReceiveMode receiveMode;	
	CHECK_EQUALS_L(smsMessaging.GetReceiveMode(receiveMode), KErrNone,
			_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	CHECK_EQUALS_L(receiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
			_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 
	TExtEtelRequestStatus receiveSmsRequestStatus(smsMessaging,EMobileSmsMessagingReceiveMessage);
	CleanupStack::PushL(receiveSmsRequestStatus);
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receivedSmsAttributes;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receivedSmsAttributesPckg(receivedSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	smsMessaging.ReceiveMessage(receiveSmsRequestStatus, gsmMsg, receivedSmsAttributesPckg);

	// Send a class 2 SMS message with RMobileSmsMessaging::SendMessage to the board 
	TBuf8<140> text(_L8("bAA Hello Mobile AAb"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes,TSmsStoreTsyTestHelper::EClass2DataCodingScheme,ETrue);	
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveSmsRequestStatus, ETimeVeryLong),
			KErrNone, _L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_TRUE(ComparePDUs(gsmMsg,pdu),_L("RMobileSmsMessaging::ReceiveMessage return wrong message"));
	
	// Check RMobileSmsMessaging::ReceiveMessage completes with valid store name in aMsgAttributes.iStore
	ASSERT_TRUE(receivedSmsAttributes.iStore.Length()>0,_L("RMobileSmsMessaging::ReceiveMessage returned an invalid store name"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	sendSmsRequestStatus
	//	receiveSmsRequestStatus
	CleanupStack::PopAndDestroy(2,&receiveSmsRequestStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0017::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0017");
	}



CCTSYIntegrationTestSmsMessaging0018::CCTSYIntegrationTestSmsMessaging0018(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0018::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0018::~CCTSYIntegrationTestSmsMessaging0018()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0018::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0018
 * @SYMFssID BA/CTSY/SMSM-0018
 * @SYMTestCaseDesc Receive a class 2 EMS message
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify class 2 EMS message is received
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that the SMS store on the SIM is not full 

	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 

	// ===  Send a class 2 EMS message - See spec 3GPP TS 23.040, GSM 04.11 ===

	// Send a class 2 EMS message with RMobileSmsMessaging::SendMessage to the board 

	// Check no error is returned in aMsgAttributes.iSubmitReport

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct originator in aMsgAttributes.iOriginator
	// Check RMobileSmsMessaging::ReceiveMessage completes with aMsgAttributes.iStatus=RMobileSmsMessaging::EMtMessageStored
	// Check RMobileSmsMessaging::ReceiveMessage completes with valid store name in aMsgAttributes.iStore

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0018::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0018");
	}



CCTSYIntegrationTestSmsMessaging0019::CCTSYIntegrationTestSmsMessaging0019(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0019::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0019::~CCTSYIntegrationTestSmsMessaging0019()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0019::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0019
 * @SYMFssID BA/CTSY/SMSM-0019
 * @SYMTestCaseDesc Receive unsupported and invalid SMS types 
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - SMS sent successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify phone does not crash
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 

	// ===  Send a SMS message with corrupt headers ===

	// Send a SMS message containing corrupt headers with RMobileSmsMessaging::SendMessage to the board 

	// Check no error is returned in aMsgAttributes.iSubmitReport

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	// Check RMobileSmsMessaging::ReceiveMessage completes with aMsgAttributes.iStatus=EMtMessageUnstoredPhoneAck
	// Check RMobileSmsMessaging::ReceiveMessage completes with KErrNone

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0019::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0019");
	}



CCTSYIntegrationTestSmsMessaging0020::CCTSYIntegrationTestSmsMessaging0020(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0020::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0020::~CCTSYIntegrationTestSmsMessaging0020()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0020::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0020
 * @SYMFssID BA/CTSY/SMSM-0020
 * @SYMTestCaseDesc Receive SMS during CS call and when packet context is active
 * @SYMTestPriority High
 * @SYMTestActions RCall::HangUp, RPacketContext::Deactivate, RCall::Dial, RCall::NotifyStatusChange, RMobileSmsMessaging::SendMessage, RPacketContext::NotifyStatusChange, RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - SMS received
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Get call 1 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KVoiceLine,KCall1 );	

	// Get voice line 1 
	RMobileLine& line1 = iEtelSessionMgr.GetLineL(KMainServer,KMainPhone, KVoiceLine);

	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer, KMainPhone, KMainSmsMessaging);
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	RMobileSmsMessaging::TMobileSmsReceiveMode receiveMode;	
	CHECK_EQUALS_L(smsMessaging.GetReceiveMode(receiveMode), KErrNone,
			_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	CHECK_EQUALS_L(receiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
			_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Part one - Receive SMS during CS call ===

	// post notifier for RCall::NotifyStatusChange
	RCall::TStatus callStatus;
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus (call1,&RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);

	// Dial a number that answers with RCall::Dial 
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse,			  
			_L("CTestStep::GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	call1.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
			_L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  
			_L("RCall::Dial returned with an error"));

	// Check RCall::NotifyStatusChange completes with EStatusConnected.
	const TInt wantedStatus = KErrNone;
	RCall::TStatus expectedCallStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
			 notifyStatusChangeStatus,
			 callStatus,
			 expectedCallStatus,
			 wantedStatus);

	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 
	TExtEtelRequestStatus receiveSmsRequestStatus(smsMessaging,EMobileSmsMessagingReceiveMessage);
	CleanupStack::PushL(receiveSmsRequestStatus);
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receivedSmsAttributes;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receivedSmsAttributesPckg(receivedSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	smsMessaging.ReceiveMessage(receiveSmsRequestStatus, gsmMsg, receivedSmsAttributesPckg);

	// Send the SMS message with RMobileSmsMessaging::SendMessage 
	TBuf8<140> text(_L8("BA-CTSY-INT-SMSM-0020.1"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes,TSmsStoreTsyTestHelper::ENoDataCodingScheme,ETrue);	
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveSmsRequestStatus, ETimeVeryLong),
			KErrNone, _L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_TRUE(ComparePDUs(gsmMsg,pdu),_L("RMobileSmsMessaging::ReceiveMessage return wrong message"));

	// Hang up call with RCall::HangUp 
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1,&RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone,  
			_L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone,  
			_L("RCall::HangUp returned an error"));

	// Check RCall::NotifyStatusChange completes with EStatusIdle.
	expectedCallStatus = RCall::EStatusIdle;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 wantedStatus);

	// ===  Part two - Receive SMS when packet context is active ===

	// Activate a primary packet context with RPacketContext::Activate 
	// Check RPacketContext::NotifyStatusChange completes with EStatusActive
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
    iPacketServiceTsyTestHelper.ActivatePrimaryPacketContextL(phone,packetContext);

	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 
	smsMessaging.ReceiveMessage(receiveSmsRequestStatus, gsmMsg, receivedSmsAttributesPckg);

	// Send the SMS message with RMobileSmsMessaging::SendMessage 
	TBuf8<140> text2(_L8("BA-CTSY-INT-SMSM-0020.2"));
	pdu = CreateValidSmsMessageL(text2, sendSmsAttributes,TSmsStoreTsyTestHelper::ENoDataCodingScheme,ETrue);	
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveSmsRequestStatus, ETimeVeryLong),
			KErrNone, _L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_TRUE(ComparePDUs(gsmMsg,pdu),_L("RMobileSmsMessaging::ReceiveMessage return wrong message"));
    
	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(packetContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Deactivate packet context with RPacketContext::Deactivate 
	TExtEtelRequestStatus contextDeactivateStatus(packetContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	packetContext.Deactivate(contextDeactivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));

	// Check RPacketContext::NotifyStatusChange completes with EStatusInactive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTsyTestHelper.WaitForPacketContextNotifyStatusChange(
				packetContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	contextDeactivateStatus
	//	notifyContextStChStatus
	//	hangUpStatus
	//	sendSmsRequestStatus
	//	receiveSmsRequestStatus
	//	dialStatus
	//	notifyStatusChangeStatus
	CleanupStack::PopAndDestroy(7,&notifyStatusChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0020::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0020");
	}



CCTSYIntegrationTestSmsMessaging0021::CCTSYIntegrationTestSmsMessaging0021(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0021::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0021::~CCTSYIntegrationTestSmsMessaging0021()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0021::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0021
 * @SYMFssID BA/CTSY/SMSM-0021
 * @SYMTestCaseDesc Receive an SMS encoded in default 7-bit alphabet
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - SMS received
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 

	// Send an SMS message encoded iwth default 7-bit alphabet with RMobileSmsMessaging::SendMessage to the board 

	// Check no error is returned in aMsgAttributes.iSubmitReport

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct originator in aMsgAttributes.iOriginator
	// Check RMobileSmsMessaging::ReceiveMessage completes with aMsgAttributes.iStatus=EMtMessageUnstoredPhoneAck

	// Repeat this test sending and receiving an SMS of maximum length. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0021::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0021");
	}



CCTSYIntegrationTestSmsMessaging0022::CCTSYIntegrationTestSmsMessaging0022(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0022::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0022::~CCTSYIntegrationTestSmsMessaging0022()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0022::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0022
 * @SYMFssID BA/CTSY/SMSM-0022
 * @SYMTestCaseDesc Receive an SMS encoded in UCS2 alphabet
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - SMS received
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 

	// Send an SMS message encoded in the UCS2 alphabet with RMobileSmsMessaging::SendMessage to the board 

	// Check no error is returned in aMsgAttributes.iSubmitReport

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct originator in aMsgAttributes.iOriginator
	// Check RMobileSmsMessaging::ReceiveMessage completes with aMsgAttributes.iStatus=EMtMessageUnstoredPhoneAck

	// Repeat this test sending and receiving an SMS of maximum length. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0022::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0022");
	}



CCTSYIntegrationTestSmsMessaging0023::CCTSYIntegrationTestSmsMessaging0023(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0023::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0023::~CCTSYIntegrationTestSmsMessaging0023()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0023::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0023
 * @SYMFssID BA/CTSY/SMSM-0023
 * @SYMTestCaseDesc Receive a concatenated SMS
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - SMS received
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify messages are received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 

	// ===  Send a long SMS message made up of 3 individual SMSs ===

	// Send first part of the SMS message with RMobileSmsMessaging::SendMessage specifying aMsgAttributes.iMore=ETrue 

	// Check no error is returned in aMsgAttributes.iSubmitReport

	// Send second part of the SMS message with RMobileSmsMessaging::SendMessage specifying aMsgAttributes.iMore=ETrue 

	// Check no error is returned in aMsgAttributes.iSubmitReport

	// Send third part of the SMS message with RMobileSmsMessaging::SendMessage specifying aMsgAttributes.iMore=EFalse 

	// Check no error is returned in aMsgAttributes.iSubmitReport

	// ===  Check that each part is received correctly ===

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	// Check RMobileSmsMessaging::ReceiveMessage completes with aMsgAttributes.iStatus=EMtMessageUnstoredPhoneAck

	// Repost RMobileSmsMessaging::ReceiveMessage immediately to wait for next incoming SMS 

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	// Check RMobileSmsMessaging::ReceiveMessage completes with aMsgAttributes.iStatus=EMtMessageUnstoredPhoneAck

	// Repost RMobileSmsMessaging::ReceiveMessage immediately to wait for next incoming SMS 

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	// Check RMobileSmsMessaging::ReceiveMessage completes with aMsgAttributes.iStatus=EMtMessageUnstoredPhoneAck

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0023::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0023");
	}



CCTSYIntegrationTestSmsMessaging0024::CCTSYIntegrationTestSmsMessaging0024(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0024::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0024::~CCTSYIntegrationTestSmsMessaging0024()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0024::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0024
 * @SYMFssID BA/CTSY/SMSM-0024
 * @SYMTestCaseDesc Receive a class 0 SMS message
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - SMS received
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL( KMainServer, KMainPhone, KMainSmsMessaging);
	RMobileSmsMessaging::TMobileSmsReceiveMode receiveMode;	
	ASSERT_EQUALS(smsMessaging.GetReceiveMode(receiveMode), KErrNone,
		_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	ASSERT_EQUALS(receiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
		_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 
	TExtEtelRequestStatus receiveSmsRequestStatus(smsMessaging,EMobileSmsMessagingReceiveMessage);
	CleanupStack::PushL(receiveSmsRequestStatus);
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receivedSmsAttributes;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receivedSmsAttributesPckg(receivedSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	smsMessaging.ReceiveMessage(receiveSmsRequestStatus, gsmMsg, receivedSmsAttributesPckg);

	// Send a class 0 SMS message with RMobileSmsMessaging::SendMessage to the board 
	TBuf8<140> text(_L8("cAA Hello Mobile AAc"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes,TSmsStoreTsyTestHelper::EClass0DataCodingScheme,ETrue);	
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveSmsRequestStatus, ETimeVeryLong),
			KErrNone, _L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_TRUE(ComparePDUs(gsmMsg,pdu),_L("RMobileSmsMessaging::ReceiveMessage return wrong message"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	sendSmsRequestStatus
	//	receiveSmsRequestStatus
	CleanupStack::PopAndDestroy(2,&receiveSmsRequestStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0024::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0024");
	}



CCTSYIntegrationTestSmsMessaging0025::CCTSYIntegrationTestSmsMessaging0025(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0025::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0025::~CCTSYIntegrationTestSmsMessaging0025()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0025::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0025
 * @SYMFssID BA/CTSY/SMSM-0025
 * @SYMTestCaseDesc Receive and store a class 1 SMS message
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::AckSmsStored, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::Write, RMobileSmsMessaging::ReceiveMessage, RMobilePhoneStore::Read
 * @SYMTestExpectedResults Pass - SMS message is received and stored.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message received and stored in SMS store.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhoneStore::GetInfo on the SMS store returns iCaps in set of KCapsReadAccess | KCapsWriteAccess 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);
	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_EQUALS_L(getInfoStatus.Int(), KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(storeInfo.iCaps, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess) ,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect store caps"));

	// Ensure that the SMS store is not full 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL( KMainServer, KMainPhone, KMainSmsMessaging);
	TInt clearSpace=iSimTsyTestHelper.EnsureEmptyStoreSpaceL(smsStore,KETelIccSmsStore,storeInfo,phone);
	CHECK_TRUE_L(clearSpace>=0,_L("TSimTsyTestHelper::TSimTsyTestHelper::EnsureEmptyStoreSpaceL returned an error"));

	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 
	DEBUG_PRINTF1(_L("Checking RMobileSmsMessaging::GetReceiveMode returns aReceiveMode=RMobileSmsMessaging::EReceiveUnstoredClientAck"));
	RMobileSmsMessaging::TMobileSmsReceiveMode receiveMode;	
	ASSERT_EQUALS(smsMessaging.GetReceiveMode(receiveMode), KErrNone,
		_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	ASSERT_EQUALS(receiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
		_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 
	TExtEtelRequestStatus receiveSmsRequestStatus(smsMessaging,EMobileSmsMessagingReceiveMessage);
	CleanupStack::PushL(receiveSmsRequestStatus);
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receivedSmsAttributes;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receivedSmsAttributesPckg(receivedSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	smsMessaging.ReceiveMessage(receiveSmsRequestStatus, gsmMsg, receivedSmsAttributesPckg);

	// Send a class 1 SMS message with RMobileSmsMessaging::SendMessage to the board 
	TBuf8<140> text(_L8("eAA Hello Mobile"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes,TSmsStoreTsyTestHelper::EClass1DataCodingScheme,ETrue);	
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveSmsRequestStatus, ETimeVeryLong),
			KErrNone, _L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_TRUE(ComparePDUs(gsmMsg,pdu),_L("RMobileSmsMessaging::ReceiveMessage return wrong message"));

	// post notifier for RMobilePhoneStore::NotifyStoreEvent
	TExtEtelRequestStatus notifyStoreEventStatus (smsStore,EMobilePhoneStoreNotifyStoreEvent);	
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 storeEvent;
	TInt index;
	smsStore.NotifyStoreEvent(notifyStoreEventStatus, storeEvent, index);

	// Write the received SMS message to the SMS store using RMobilePhoneStore::Write 
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);
	
 	entry.iServiceCentre.iTypeOfNumber =  receivedSmsAttributes.iGsmServiceCentre.iTypeOfNumber; 
 	entry.iServiceCentre.iNumberPlan = receivedSmsAttributes.iGsmServiceCentre.iNumberPlan; 
 	entry.iServiceCentre.iTelNumber = receivedSmsAttributes.iGsmServiceCentre.iTelNumber;  
	entry.iMsgStatus = RMobileSmsStore::EStoredMessageUnread;
	entry.iMsgData=gsmMsg;
	entry.iIndex = clearSpace;
	TExtEtelRequestStatus writeStatus(smsStore, EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);
	smsStore.Write(writeStatus,pckgEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),KErrNone, _L("RMobileSmsStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileSmsStore::Write returned an error"));
	

	// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreEntryAdded
	RMobilePhoneStore::TMobileStoreEvent expectedEvent = RMobilePhoneStore::KStoreEntryAdded ;
	iSmsStoreTsyTestHelper.WaitForSmsStoreNotifyStoreEvent( smsStore,
			notifyStoreEventStatus,
			storeEvent,
			expectedEvent,
			index);

	// Acknowledge that the SMS has been decoded and stored with RMobileSmsMessaging::AckSmsStored sending the delivery report in aMsgData 
	TExtEtelRequestStatus ackStatus(smsMessaging, EMobileSmsMessagingAckSmsStored);	
	CleanupStack::PushL(ackStatus);
	RMobileSmsMessaging::TMobileSmsGsmTpdu ackPdu;
	ackPdu.SetLength(0);
	TUint8 messageTypeIndicator=0x00; //0 deliver and deliver report; 1 submit and submit report; 2 status report and command
	TUint8 userDataHeaderIndicator=0x00; //0 no header; 0x40 has header
	ackPdu.Append(messageTypeIndicator | userDataHeaderIndicator);
	TUint8 tpParameterIndicator=0x00;
	ackPdu.Append(tpParameterIndicator);
	smsMessaging.AckSmsStored(ackStatus,ackPdu);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(ackStatus, ETimeVeryLong),KErrNone, _L("RMobilePhone::AckSmsStored timed out"));
	ASSERT_EQUALS(ackStatus.Int(), KErrNone, _L("RMobilePhone::AckSmsStored returned an error"));
	
	// Read the SMS message that has just been stored and check that the contents is the same as that received 
	RMobileSmsStore::TMobileGsmSmsEntryV1 readEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg readEntryPckg(readEntry);
	readEntry.iIndex = clearSpace;
	readEntry.iMsgData.Zero();
	readEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;
	TExtEtelRequestStatus readStatus (smsStore,EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);	
	smsStore.Read(readStatus,readEntryPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),KErrNone,_L("RMobileSmsStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileSmsStore::Read returned an error"));
	ASSERT_EQUALS(readEntry.iMsgStatus,RMobileSmsStore::EStoredMessageUnread,_L("RMobileSmsStore::Read returned incorrect data"));
	ASSERT_EQUALS(readEntry.iServiceCentre.iTypeOfNumber,receivedSmsAttributes.iGsmServiceCentre.iTypeOfNumber,_L("RMobileSmsStore::Read returned incorrect data"));
	ASSERT_EQUALS(readEntry.iServiceCentre.iNumberPlan,receivedSmsAttributes.iGsmServiceCentre.iNumberPlan,_L("RMobileSmsStore::Read returned incorrect data"));
	ASSERT_EQUALS_DES16(readEntry.iServiceCentre.iTelNumber,receivedSmsAttributes.iGsmServiceCentre.iTelNumber,_L("RMobileSmsStore::Read returned incorrect data"));
	ASSERT_EQUALS_DES8(readEntry.iMsgData,gsmMsg,_L("RMobileSmsStore::Read returned incorrect data"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	readStatus
	//	ackStatus
	//	writeStatus
	//	notifyStoreEventStatus
	//	sendSmsRequestStatus
	//	receiveSmsRequestStatus
	//	getInfoStatus
	CleanupStack::PopAndDestroy(7,&getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0025::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0025");
	}



CCTSYIntegrationTestSmsMessaging0026::CCTSYIntegrationTestSmsMessaging0026(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0026::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0026::~CCTSYIntegrationTestSmsMessaging0026()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0026::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0026
 * @SYMFssID BA/CTSY/SMSM-0026
 * @SYMTestCaseDesc Receive and store a class 2 SMS message
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::AckSmsStored, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::Write, RMobileSmsMessaging::ReceiveMessage, RMobilePhoneStore::Read
 * @SYMTestExpectedResults Pass - SMS message is received and stored.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message received and stored in SMS store.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Ensure RMobilePhoneStore::GetInfo on the SMS store returns iCaps in set of KCapsReadAccess | KCapsWriteAccess 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);
	TExtEtelRequestStatus getInfoStatus (smsStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg (storeInfo);
	smsStore.GetInfo(getInfoStatus,storeInfoPckg);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	CHECK_EQUALS_L(getInfoStatus.Int(), KErrNone, _L("RMobileSmsStore::GetInfo returned an error"));
	CHECK_BITS_SET_L(storeInfo.iCaps, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess) ,KNoUnwantedBits, _L("RMobilePhoneStore::GetInfo returned incorrect store caps"));

	// Ensure that the SMS store is not full 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL( KMainServer, KMainPhone, KMainSmsMessaging);
    TInt clearSpace=iSimTsyTestHelper.EnsureEmptyStoreSpaceL(smsStore,KETelIccSmsStore,storeInfo,phone);
	CHECK_TRUE_L(clearSpace>=0,_L("TSimTsyTestHelper::TSimTsyTestHelper::EnsureEmptyStoreSpaceL returned an error"));

	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 
	DEBUG_PRINTF1(_L("Checking RMobileSmsMessaging::GetReceiveMode returns aReceiveMode=RMobileSmsMessaging::EReceiveUnstoredClientAck"));
	RMobileSmsMessaging::TMobileSmsReceiveMode receiveMode;	
	ASSERT_EQUALS(smsMessaging.GetReceiveMode(receiveMode), KErrNone,
		_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	ASSERT_EQUALS(receiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
		_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));
    iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 
	TExtEtelRequestStatus receiveSmsRequestStatus(smsMessaging,EMobileSmsMessagingReceiveMessage);
	CleanupStack::PushL(receiveSmsRequestStatus);
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receivedSmsAttributes;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receivedSmsAttributesPckg(receivedSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	smsMessaging.ReceiveMessage(receiveSmsRequestStatus, gsmMsg, receivedSmsAttributesPckg);

	// Send a class 2 SMS message with RMobileSmsMessaging::SendMessage to the board 
	TBuf8<140> text(_L8("BA-CTSY-INT-SMSM-0026.1"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes,TSmsStoreTsyTestHelper::EClass2DataCodingScheme,ETrue);	
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveSmsRequestStatus, ETimeVeryLong),
			KErrNone, _L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_TRUE(ComparePDUs(gsmMsg,pdu),_L("RMobileSmsMessaging::ReceiveMessage return wrong message"));


	// post notifier for RMobilePhoneStore::NotifyStoreEvent
	TExtEtelRequestStatus notifyStoreEventStatus (smsStore,EMobilePhoneStoreNotifyStoreEvent);	
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 storeEvent;
	TInt index;
	smsStore.NotifyStoreEvent(notifyStoreEventStatus, storeEvent, index);

	// Write the received SMS message to the SMS store using RMobilePhoneStore::Write 
	RMobileSmsStore::TMobileGsmSmsEntryV1 entry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg pckgEntry(entry);
 	entry.iServiceCentre.iTypeOfNumber =  receivedSmsAttributes.iGsmServiceCentre.iTypeOfNumber; 
 	entry.iServiceCentre.iNumberPlan = receivedSmsAttributes.iGsmServiceCentre.iNumberPlan; 
 	entry.iServiceCentre.iTelNumber = receivedSmsAttributes.iGsmServiceCentre.iTelNumber;  
	entry.iMsgStatus = RMobileSmsStore::EStoredMessageUnread;
	entry.iMsgData=gsmMsg;
	entry.iIndex = clearSpace;
	TExtEtelRequestStatus writeStatus(smsStore, EMobilePhoneStoreWrite);	
	CleanupStack::PushL(writeStatus);
	smsStore.Write(writeStatus,pckgEntry);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(writeStatus, ETimeMedium),KErrNone, _L("RMobileSmsStore::Write timed out"));
	ASSERT_EQUALS(writeStatus.Int(), KErrNone, _L("RMobileSmsStore::Write returned an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreEntryAdded
	RMobilePhoneStore::TMobileStoreEvent expectedEvent = RMobilePhoneStore::KStoreEntryAdded ;
	iSmsStoreTsyTestHelper.WaitForSmsStoreNotifyStoreEvent( smsStore,
			notifyStoreEventStatus,
			storeEvent,
			expectedEvent,
			index);

	// Acknowledge that the SMS has been decoded and stored with RMobileSmsMessaging::AckSmsStored sending the delivery report in aMsgData 
	TExtEtelRequestStatus ackStatus(smsMessaging, EMobileSmsMessagingAckSmsStored);	
	CleanupStack::PushL(ackStatus);
	RMobileSmsMessaging::TMobileSmsGsmTpdu ackPdu;
	ackPdu.SetLength(0);
	TUint8 messageTypeIndicator=0x00; //0 deliver and deliver report; 1 submit and submit report; 2 status report and command
	TUint8 userDataHeaderIndicator=0x00; //0 no header; 0x40 has header
	ackPdu.Append(messageTypeIndicator | userDataHeaderIndicator);
	TUint8 tpParameterIndicator=0x00;
	ackPdu.Append(tpParameterIndicator);
	smsMessaging.AckSmsStored(ackStatus,ackPdu);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(ackStatus, ETimeVeryLong),KErrNone, _L("RMobilePhone::AckSmsStored timed out"));
	ASSERT_EQUALS(ackStatus.Int(), KErrNone, _L("RMobilePhone::AckSmsStored returned an error"));

	// Read the SMS message that has just been stored and check that the contents is the same as that received 
	RMobileSmsStore::TMobileGsmSmsEntryV1 readEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg readEntryPckg(readEntry);
	readEntry.iIndex = clearSpace;
	readEntry.iMsgData.Zero();
	readEntry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;
	TExtEtelRequestStatus readStatus (smsStore,EMobilePhoneStoreRead);	
	CleanupStack::PushL(readStatus);	
	smsStore.Read(readStatus,readEntryPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(readStatus, ETimeMedium),KErrNone,_L("RMobileSmsStore::Read timed out"));
	ASSERT_EQUALS(readStatus.Int(), KErrNone, _L("RMobileSmsStore::Read returned an error"));
	ASSERT_EQUALS(readEntry.iMsgStatus,RMobileSmsStore::EStoredMessageUnread,_L("RMobileSmsStore::Read returned incorrect data"));
	ASSERT_EQUALS(readEntry.iServiceCentre.iTypeOfNumber,receivedSmsAttributes.iGsmServiceCentre.iTypeOfNumber,_L("RMobileSmsStore::Read returned incorrect data"));
	ASSERT_EQUALS(readEntry.iServiceCentre.iNumberPlan,receivedSmsAttributes.iGsmServiceCentre.iNumberPlan,_L("RMobileSmsStore::Read returned incorrect data"));
	ASSERT_EQUALS_DES16(readEntry.iServiceCentre.iTelNumber,receivedSmsAttributes.iGsmServiceCentre.iTelNumber,_L("RMobileSmsStore::Read returned incorrect data"));
	ASSERT_EQUALS_DES8(readEntry.iMsgData,gsmMsg,_L("RMobileSmsStore::Read returned incorrect data"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	readStatus
	//	ackStatus
	//	writeStatus
	//	notifyStoreEventStatus
	//	sendSmsRequestStatus
	//	receiveSmsRequestStatus
	//	getInfoStatus
	CleanupStack::PopAndDestroy(7,&getInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0026::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0026");
	}



CCTSYIntegrationTestSmsMessaging0027::CCTSYIntegrationTestSmsMessaging0027(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0027::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0027::~CCTSYIntegrationTestSmsMessaging0027()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0027::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0027
 * @SYMFssID BA/CTSY/SMSM-0027
 * @SYMTestCaseDesc Receive a class 3 SMS message
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - SMS message is received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL( KMainServer, KMainPhone, KMainSmsMessaging);
	RMobileSmsMessaging::TMobileSmsReceiveMode receiveMode;	
	CHECK_EQUALS_L(smsMessaging.GetReceiveMode(receiveMode), KErrNone,
			_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	CHECK_EQUALS_L(receiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
			_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 
	TExtEtelRequestStatus receiveSmsRequestStatus(smsMessaging,EMobileSmsMessagingReceiveMessage);
	CleanupStack::PushL(receiveSmsRequestStatus);
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receivedSmsAttributes;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receivedSmsAttributesPckg(receivedSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	smsMessaging.ReceiveMessage(receiveSmsRequestStatus, gsmMsg, receivedSmsAttributesPckg);

	// Send a class 3 SMS message with RMobileSmsMessaging::SendMessage to the board 
	TBuf8<140> text(_L8("bAA Hello Mobile AAb"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes,TSmsStoreTsyTestHelper::EClass3DataCodingScheme,ETrue);	
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveSmsRequestStatus, ETimeVeryLong),
			KErrNone, _L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_TRUE(ComparePDUs(gsmMsg,pdu),_L("RMobileSmsMessaging::ReceiveMessage return wrong message"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	sendSmsRequestStatus
	//	receiveSmsRequestStatus
	CleanupStack::PopAndDestroy(2,&receiveSmsRequestStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0027::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0027");
	}



CCTSYIntegrationTestSmsMessaging0028::CCTSYIntegrationTestSmsMessaging0028(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0028::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0028::~CCTSYIntegrationTestSmsMessaging0028()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0028::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0028
 * @SYMFssID BA/CTSY/SMSM-0028
 * @SYMTestCaseDesc Receive a class 0 SMS message when the SMS store is full
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - SMS message is received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL( KMainServer, KMainPhone, KMainSmsMessaging);
	RMobileSmsMessaging::TMobileSmsReceiveMode receiveMode;	
	CHECK_EQUALS_L(smsMessaging.GetReceiveMode(receiveMode), KErrNone,
			_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	CHECK_EQUALS_L(receiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
			_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));

	// Ensure that the SMS store is full 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);
    CHECK_TRUE_L(iSmsStoreTsyTestHelper.EnsureStoreFullL(smsStore),_L("TSmsStoreTsyTestHelper::EnsureStoreFullL returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Receive a class 0 message when the SMS store is full ===

	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 
	TExtEtelRequestStatus receiveSmsRequestStatus(smsMessaging,EMobileSmsMessagingReceiveMessage);
	CleanupStack::PushL(receiveSmsRequestStatus);
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receivedSmsAttributes;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receivedSmsAttributesPckg(receivedSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	smsMessaging.ReceiveMessage(receiveSmsRequestStatus, gsmMsg, receivedSmsAttributesPckg);

	// Send a class 0 SMS message with RMobileSmsMessaging::SendMessage to the board 
	TBuf8<140> text1(_L8("BA-CTSY-INT-SMSM-0028.1"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text1, sendSmsAttributes,TSmsStoreTsyTestHelper::EClass0DataCodingScheme,ETrue);	
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveSmsRequestStatus, ETimeVeryLong),
			KErrNone, _L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_TRUE(ComparePDUs(gsmMsg,pdu),_L("RMobileSmsMessaging::ReceiveMessage return wrong message"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	sendSmsRequestStatus
	//	receiveSmsRequestStatus
	CleanupStack::PopAndDestroy(2,&receiveSmsRequestStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0028::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0028");
	}



CCTSYIntegrationTestSmsMessaging0029::CCTSYIntegrationTestSmsMessaging0029(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0029::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0029::~CCTSYIntegrationTestSmsMessaging0029()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0029::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0029
 * @SYMFssID BA/CTSY/SMSM-0029
 * @SYMTestCaseDesc Receive class 2 SMS when store is full and resume SMS reception
 * @SYMTestPriority High
 * @SYMTestActions RMobileSmsMessaging::NackSmsStored, RMobileSmsMessaging::AckSmsStored, RMobileSmsMessaging::ResumeSmsReception, RMobileSmsMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - Acknowledgements can be sent to the network and SMS reception can be resumed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure that the SMS store is not full 
	RMobileSmsStore& smsStore = iEtelSessionMgr.GetSmsStoreL(KMainServer,KMainPhone,KMainSmsMessaging,KIccSmsStore);
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL( KMainServer, KMainPhone, KMainSmsMessaging);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	TInt clearSpace=iSimTsyTestHelper.EnsureEmptyStoreSpaceL(smsStore,KETelIccSmsStore,storeInfo,phone);
	CHECK_TRUE_L(clearSpace>=0,_L("TSimTsyTestHelper::TSimTsyTestHelper::EnsureEmptyStoreSpaceL returned an error"));
	
	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 
	RMobileSmsMessaging::TMobileSmsReceiveMode receiveMode;	
	CHECK_EQUALS_L(smsMessaging.GetReceiveMode(receiveMode), KErrNone,
		_L("RMobileSmsMessaging::GetReceiveMode returned with an error."));
	CHECK_EQUALS_L(receiveMode, RMobileSmsMessaging::EReceiveUnstoredClientAck,
		_L("RMobileSmsMessaging::GetReceiveMode returned with incorrect mode."));
    iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(phone);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Part 1 - Receive 1 SMS then tell network that memory is full ===

	// ===  Receive a class 2 SMS message ===

	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 
	TExtEtelRequestStatus receiveSmsRequestStatus(smsMessaging,EMobileSmsMessagingReceiveMessage);
	CleanupStack::PushL(receiveSmsRequestStatus);
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receivedSmsAttributes;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receivedSmsAttributesPckg(receivedSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	smsMessaging.ReceiveMessage(receiveSmsRequestStatus, gsmMsg, receivedSmsAttributesPckg);

	// Send a class 2 SMS message with RMobileSmsMessaging::SendMessage to the board 
	TBuf8<140> text(_L8("first 29"));
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu = CreateValidSmsMessageL(text, sendSmsAttributes,TSmsStoreTsyTestHelper::EClass2DataCodingScheme,ETrue);	
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	TUint wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveSmsRequestStatus, ETimeVeryLong),
			KErrNone, _L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_TRUE(ComparePDUs(gsmMsg,pdu),_L("RMobileSmsMessaging::ReceiveMessage return wrong message"));
	
	// ===  Tell the network that this message has been received and decoded ===

	// ===  successfully but now the memory is full ===

	// Acknowledge that the SMS has been decoded and stored with RMobileSmsMessaging::AckSmsStored sending the delivery report in aMsgData and indicating store is full with aFull=ETrue
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10045);
	ASSERT_EQUALS(1,0, _L("Test leaving because it will cause TSY crash, remove this when defect is fixed"));
	TEST_CHECK_POINT_L(_L("test leaving..."));
	TExtEtelRequestStatus ackStatus(smsMessaging, EMobileSmsMessagingAckSmsStored);	
	CleanupStack::PushL(ackStatus);
	RMobileSmsMessaging::TMobileSmsGsmTpdu ackPdu;
	ackPdu.SetLength(0);
	TUint8 messageTypeIndicator=0x00; //0 deliver and deliver report; 1 submit and submit report; 2 status report and command
	TUint8 userDataHeaderIndicator=0x00; //0 no header; 0x40 has header
	ackPdu.Append(messageTypeIndicator | userDataHeaderIndicator);
	TUint8 tpParameterIndicator=0x00;
	ackPdu.Append(tpParameterIndicator);
	smsMessaging.AckSmsStored(ackStatus,ackPdu,ETrue );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(ackStatus, ETimeVeryLong),KErrNone, _L("RMobilePhone::AckSmsStored timed out"));
	ASSERT_EQUALS(ackStatus.Int(), KErrNone, _L("RMobilePhone::AckSmsStored returned an error"));

	// ===  Send a message to the board but check it is not received until ===
	// ===  SMS reception is resumed ===

	// Ensure that the SMS store is not full 
    ASSERT_TRUE(iSimTsyTestHelper.EnsureEmptyStoreSpaceL(smsStore,KETelIccSmsStore,storeInfo,phone)>=0,_L("TSimTsyTestHelper::EnsureEmptyStoreSpaceL returned an error"));

	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 
	smsMessaging.ReceiveMessage(receiveSmsRequestStatus, gsmMsg, receivedSmsAttributesPckg);

	// Send a class 2 SMS message with RMobileSmsMessaging::SendMessage to the board 
	TBuf8<140> text2(_L8("this is the 2nd 29"));
	pdu = CreateValidSmsMessageL(text2, sendSmsAttributes,TSmsStoreTsyTestHelper::EClass2DataCodingScheme,ETrue);
    
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Wait for 1 minute  
	User::After(60*KOneSecond);
	
	// Check RMobileSmsMessaging::ReceiveMessage is still pending
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(),KRequestPending,_L("RMobileSmsMessaging::ReceiveMessage is not pending"));

	// ===  Resume SMS and check message is received ===
    
	// Resume SMS reception with RMobileSmsMessaging::ResumeSmsReception 
	TExtEtelRequestStatus resumeReceptionRequestStatus(smsMessaging, EMobileSmsMessagingResumeSmsReception);
	CleanupStack::PushL(resumeReceptionRequestStatus);
	smsMessaging.ResumeSmsReception(resumeReceptionRequestStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(resumeReceptionRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::ResumeSmsReception timed out."));
	ASSERT_EQUALS(resumeReceptionRequestStatus.Int(), KErrNone, 
    _L("RMobileSmsMessaging::ResumeSmsReception completed with incorrect status"));
    
	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveSmsRequestStatus, ETimeVeryLong),
			KErrNone, _L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
    DEBUG_PRINTF1(_L("BEFORE WAIT"));
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_TRUE(ComparePDUs(gsmMsg,pdu),_L("RMobileSmsMessaging::ReceiveMessage return wrong message"));

	// ===  Part 2 attempt to receive an SMS but tell the network memory is full. Then resume SMS reception and receive the message again ===

	// ===  Receive an SMS message ===

	// Ensure that the SMS store is not full 
    ASSERT_TRUE(iSimTsyTestHelper.EnsureEmptyStoreSpaceL(smsStore,KETelIccSmsStore,storeInfo,phone)>=0,_L("TSimTsyTestHelper::EnsureEmptyStoreSpaceL returned an error"));

	// Call RMobileSmsMessaging::ReceiveMessage to wait for an incoming SMS 
	smsMessaging.ReceiveMessage(receiveSmsRequestStatus, gsmMsg, receivedSmsAttributesPckg);

	// Send a class 2 SMS message with RMobileSmsMessaging::SendMessage to the board 
	TBuf8<140> text3(_L8("and this is the third 29 sms"));
	pdu = CreateValidSmsMessageL(text3, sendSmsAttributes,TSmsStoreTsyTestHelper::EClass2DataCodingScheme,ETrue);	
	smsMessaging.SendMessage(sendSmsRequestStatus, pdu, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_EQUALS(sendSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));

	// Check no error is returned in aMsgAttributes.iSubmitReport
	ASSERT_TRUE(sendSmsAttributes.iSubmitReport.Length() > 0, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1.iSubmitReport has zero length."));
	wantedBits = RMobileSmsMessaging::KGsmSubmitReport; 
	ASSERT_BITS_SET(sendSmsAttributes.iFlags,wantedBits,KNoUnwantedBits, 
			_L("RMobileSmsMessaging::TMobileSmsSendAttributesV1 has no submit report in iFlags."));			

	// Check RMobileSmsMessaging::ReceiveMessage is no longer pending
	ASSERT_TRUE(receiveSmsRequestStatus.Int()!=KRequestPending,_L("RMobileSmsMessaging::ReceiveMessage still pending"));

	// ===  Send negative acknowledgement telling network that memory is full ===

	// Return a negative acknowledgment with RMobileSmsMessaging::NackSmsStored indicating storage is full 
	TExtEtelRequestStatus nackStatus(smsMessaging, EMobileSmsMessagingNackSmsStored);
	CleanupStack::PushL(nackStatus);
	RMobileSmsMessaging::TMobileSmsGsmTpdu nackPdu;
	nackPdu.SetLength(0);
	messageTypeIndicator=0x00; //0 deliver and deliver report; 1 submit and submit report; 2 status report and command
	userDataHeaderIndicator=0x00; //0 no header; 0x40 has header
	nackPdu.Append(messageTypeIndicator | userDataHeaderIndicator);
	TUint8 failureCause=0xD0; //SIM SMS storage full
	nackPdu.Append(failureCause);
	tpParameterIndicator=0x00;
	nackPdu.Append(tpParameterIndicator);
	smsMessaging.NackSmsStored(nackStatus,nackPdu,KErrGsmSMSSimSMSStorageFull);

	// Check RMobileSmsMessaging::ReceiveMessage is still pending
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(),KRequestPending,_L("RMobileSmsMessaging::ReceiveMessage is not pending"));

	// ===  Resume SMS and check message is received ===

	// Resume SMS reception with RMobileSmsMessaging::ResumeSmsReception 
	smsMessaging.ResumeSmsReception(resumeReceptionRequestStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(resumeReceptionRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::ResumeSmsReception timed out."));
	ASSERT_EQUALS(resumeReceptionRequestStatus.Int(), KErrNone, 
		_L("RMobileSmsMessaging::ResumeSmsReception completed with incorrect status"));

	// Check RMobileSmsMessaging::ReceiveMessage completes with the correct message data in aMsgData
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveSmsRequestStatus, ETimeVeryLong),
			KErrNone, _L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_EQUALS(receiveSmsRequestStatus.Int(), KErrNone, 
			_L("RMobileSmsMessaging::ReceiveMessage returned with error."));	
	ASSERT_TRUE(ComparePDUs(gsmMsg,pdu),_L("RMobileSmsMessaging::ReceiveMessage return wrong message"));


	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	//	nackStatus
	//	resumeReceptionRequestStatus
	//	ackStatus
	//	sendSmsRequestStatus
	//	receiveSmsRequestStatus
	CleanupStack::PopAndDestroy(5,&receiveSmsRequestStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0029::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0029");
	}



CCTSYIntegrationTestSmsMessaging0030::CCTSYIntegrationTestSmsMessaging0030(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0030::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0030::~CCTSYIntegrationTestSmsMessaging0030()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0030::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0030
 * @SYMFssID BA/CTSY/SMSM-0030
 * @SYMTestCaseDesc Send an SMS message without FDN check
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Message sent.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that RMobileSmsMessaging::GetReceiveMode returns aReceiveMode = EReceiveUnstoredClientAck 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Send an SMS message using RMobileSmsMessaging::SendMessageNoFdnCheck 

	// Check no error is returned in aMsgAttributes.iSubmitReport

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0030::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0030");
	}



CCTSYIntegrationTestSmsMessaging0031::CCTSYIntegrationTestSmsMessaging0031(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0031::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0031::~CCTSYIntegrationTestSmsMessaging0031()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0031::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0031
 * @SYMFssID BA/CTSY/SMSM-0031
 * @SYMTestCaseDesc Get SMS messaging caps.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Correct caps returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify caps returned are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer, KMainPhone, KMainSmsMessaging);
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Check RMobileSmsMessaging::GetCaps returns aCaps.iSmsMode caps in set of RMobileSmsMessaging::KCapsGsmSms
	RMobileSmsMessaging::TMobileSmsCapsV1 smsCaps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPkg(smsCaps);
	ASSERT_EQUALS(smsMessaging.GetCaps(smsCapsPkg), KErrNone, 
		_L("RMobileSmsMessaging::GetCaps returned an error."));
	TUint32 expectedMinCaps = RMobileSmsMessaging::KCapsGsmSms;
	ASSERT_BITS_SET(smsCaps.iSmsMode, expectedMinCaps, KNoUnwantedBits,
		_L("RMobileSmsMessaging::GetCaps miniumum capabilites not found:RMobileSmsMessaging::KCapsGsmSms."));
	
	// Check RMobileSmsMessaging::GetCaps returns aCaps.iSmsControl caps in set of RMobileSmsMessaging::KCapsReceiveUnstoredClientAck | RMobileSmsMessaging::KCapsSendWithAck | RMobileSmsMessaging::KCapsGetSmspList | RMobileSmsMessaging::KCapsSetSmspList
	expectedMinCaps = RMobileSmsMessaging::KCapsReceiveUnstoredClientAck | 
					RMobileSmsMessaging::KCapsSendWithAck |
				    RMobileSmsMessaging::KCapsGetSmspList |
				    RMobileSmsMessaging::KCapsSetSmspList;
	ASSERT_BITS_SET(smsCaps.iSmsControl, expectedMinCaps, KNoUnwantedBits, 
		_L("RMobileSmsMessaging::GetCaps returned with incorrect caps."));
		
	//
	// TEST END
	//

    StartCleanup();
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0031::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0031");
	}



CCTSYIntegrationTestSmsMessaging0032::CCTSYIntegrationTestSmsMessaging0032(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0032::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0032::~CCTSYIntegrationTestSmsMessaging0032()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0032::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0032
 * @SYMFssID BA/CTSY/SMSM-0032
 * @SYMTestCaseDesc Cancel receiving an SMS message.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - 
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify request is cancelled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer, KMainPhone, KMainSmsMessaging);
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Request to receive an SMS message with RMobileSmsMessaging::ReceiveMessage
	TExtEtelRequestStatus receiveSmsRequestStatus(smsMessaging,EMobileSmsMessagingReceiveMessage);
	CleanupStack::PushL(receiveSmsRequestStatus);
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receivedSmsAttributes;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receivedSmsAttributesPckg(receivedSmsAttributes);
	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	smsMessaging.ReceiveMessage(receiveSmsRequestStatus, gsmMsg, receivedSmsAttributesPckg);
	
	// Cancel the request with CancelAsyncRequest( EMobileSmsMessagingReceiveMessage ) 
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingReceiveMessage);

	// Check RMobileSmsMessaging::ReceiveMessage completes with KErrCancel
	ASSERT_EQUALS(WaitForRequestWithTimeOut(receiveSmsRequestStatus, ETimeVeryLong),
		KErrNone, _L("RMobilePhone::CancelAsyncRequest returned with error."));	
	ASSERT_TRUE(receiveSmsRequestStatus.Int() == KErrNone || receiveSmsRequestStatus.Int() == KErrCancel, 
		_L("RMobilePhone::CancelAsyncRequest returned with error."));	
	
	
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(1);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0032::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0032");
	}



CCTSYIntegrationTestSmsMessaging0033::CCTSYIntegrationTestSmsMessaging0033(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0033::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0033::~CCTSYIntegrationTestSmsMessaging0033()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0033::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0033
 * @SYMFssID BA/CTSY/SMSM-0033
 * @SYMTestCaseDesc Attempt to send  an SMS message that is too long.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Error returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned.
 *
 * @return - TVerdict code
 * 
 * $CTSYProblem - The board resets when passing a messsage to LTSY which is too long.  No checking is done by the CTSY.
 * 	No error is returned from LTSY.  The board just reboots.
 * 
 * 
 */
	{

	//
	// SET UP
	//
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer, KMainPhone, KMainSmsMessaging);
	
	// Ensure SMS bearer is RMobileSmsMessaging::ESmsBearerCircuitPreferred 
	RMobileSmsMessaging::TMobileSmsBearer bearer;
	TExtEtelRequestStatus setBearerStatusRequest(smsMessaging, EMobileSmsMessagingSetMoSmsBearer);
	CleanupStack::PushL(setBearerStatusRequest);
	smsMessaging.SetMoSmsBearer(setBearerStatusRequest, RMobileSmsMessaging::ESmsBearerCircuitPreferred);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(setBearerStatusRequest, ETimeLong), KErrNone, 
			_L("RMobileSmsMessaging::SetMoSmsBearer timed-out"));

	// Check RMobileSmsMessaging::GetMoSmsBearer returns KErrNotSupported
	TInt error = smsMessaging.GetMoSmsBearer(bearer);
	CHECK_EQUALS_L(error, KErrNotSupported,_L("RMobileSmsMessaging::GetMoSmsBearer returned with error."));
	

	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Attempt to send an SMS message that is 206 characters with RMobileSmsMessaging::SendMessage 
	TExtEtelRequestStatus sendSmsRequestStatus(smsMessaging, EMobileSmsMessagingSendMessage);
	CleanupStack::PushL(sendSmsRequestStatus);

	TBuf8<206> text(_L8("This is a 206 character message.  It is going to fill up the space with XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"));

	//Read the destination and Service Centre numbers from ini file
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 sendSmsAttributes;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg sendSmsAttributesPckg(sendSmsAttributes);	
	GetSmsSendAttributesL(sendSmsAttributes);

	// Check error is returned.  
    // Due to LTSY crashing when a message of this length is passed, the rest of 
    // this test case cannot be executed. If the LTSY is fixed, execute the code below.
    /*
    smsMessaging.SendMessage(sendSmsRequestStatus, text, sendSmsAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendSmsRequestStatus, ETimeMedium),
		KErrNone, _L("RMobileSmsMessaging::SendMessage timed out."));
	ASSERT_TRUE(sendSmsRequestStatus.Int() != KErrNone, 
		_L("RMobileSmsMessaging::SendMessage completed with incorrect status"));
    */

	//
	// TEST END
	//

    StartCleanup();
	
    /* 	setBearerStatusRequest
		sendSmsRequestStatus */
    CleanupStack::PopAndDestroy(2,&setBearerStatusRequest);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0033::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0033");
	}



CCTSYIntegrationTestSmsMessaging0034::CCTSYIntegrationTestSmsMessaging0034(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0034::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0034::~CCTSYIntegrationTestSmsMessaging0034()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0034::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0034
 * @SYMFssID BA/CTSY/SMSM-0034
 * @SYMTestCaseDesc Resume SMS Reception
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrNone is returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNone is returned
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer, KMainPhone, KMainSmsMessaging);
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Resume SMS reception with RMobileSmsMessaging::ResumeSmsReception 
	TExtEtelRequestStatus resumeReceptionRequestStatus(smsMessaging, EMobileSmsMessagingResumeSmsReception);
	CleanupStack::PushL(resumeReceptionRequestStatus);
	smsMessaging.ResumeSmsReception(resumeReceptionRequestStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(resumeReceptionRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::ResumeSmsReception timed out."));
	ASSERT_EQUALS(resumeReceptionRequestStatus.Int(), KErrNone, 
		_L("RMobileSmsMessaging::ResumeSmsReception completed with incorrect status"));
	
	
	//
	// TEST END
	//

    StartCleanup();
	
	// resumeReceptionRequestStatus
	CleanupStack::PopAndDestroy(1);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0034::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0034");
	}



CCTSYIntegrationTestSmsMessaging0035::CCTSYIntegrationTestSmsMessaging0035(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSmsMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestSmsMessaging0035::GetTestStepName());
	}

CCTSYIntegrationTestSmsMessaging0035::~CCTSYIntegrationTestSmsMessaging0035()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestSmsMessaging0035::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-SMSM-0035
 * @SYMFssID BA/CTSY/SMSM-0035
 * @SYMTestCaseDesc Cancel resume SMS reception
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrNone is returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNone or KErrCancel returned
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobileSmsMessaging &smsMessaging = iEtelSessionMgr.GetSmsMessagingL(KMainServer, KMainPhone, KMainSmsMessaging);
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Resume SMS reception with RMobileSmsMessaging::ResumeSmsReception 
	TExtEtelRequestStatus resumeReceiveSmsRequestStatus(smsMessaging, EMobileSmsMessagingResumeSmsReception);
	CleanupStack::PushL(resumeReceiveSmsRequestStatus);
	smsMessaging.ResumeSmsReception(resumeReceiveSmsRequestStatus);
	
	// Cancel request with RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingResumeSmsReception)  
	smsMessaging.CancelAsyncRequest(EMobileSmsMessagingResumeSmsReception);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(resumeReceiveSmsRequestStatus, ETimeMedium),
			KErrNone, _L("RMobileSmsMessaging::ResumeSmsReception timed out."));
	ASSERT_TRUE(resumeReceiveSmsRequestStatus.Int() == KErrNone || resumeReceiveSmsRequestStatus.Int() == KErrCancel, 
		_L("RMobileSmsMessaging::ResumeSmsReception completed with incorrect status"));

	
	
	//
	// TEST END
	//

    StartCleanup();
	
	// resumeReceiveSmsRequestStatus
	CleanupStack::PopAndDestroy(1);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestSmsMessaging0035::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestSmsMessaging0035");
	}
