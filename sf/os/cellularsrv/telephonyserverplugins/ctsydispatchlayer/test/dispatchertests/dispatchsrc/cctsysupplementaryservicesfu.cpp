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
 @file The TEFUnit test suite for SupplementaryServicesControl in the Common TSY.
*/

#include "cctsysupplementaryservicesfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/ltsy/mltsydispatchsupplementaryservicesinterface.h>
#include "mockltsyindicatorids.h"
#include <mmretrieve.h>
#include <ctsy/rmmcustomapi.h>
#include "cctsyactiveretriever.h"
#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>

CTestSuite* CCTsySupplementaryServicesFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	//add use-case tests
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUseCase0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUseCase0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUseCase0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUseCase0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUseCase0005L);
  
	//add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUnit0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsySupplementaryServicesFU, TestUnit0008L);

	END_SUITE;
	}


//
// Use-case tests
//

/**
 * Compare two RMobilePhone::TMobileAddress objects.
 * 
 * @param aAddr1 the first object.
 * @param aAddr2 the second object.
 * @return ETrue iff the two object are the same.
 * 
 * */ 
TBool operator ==(const RMobilePhone::TMobileAddress& aAddr1, 
		const RMobilePhone::TMobileAddress& aAddr2)
	{
	if(aAddr1.iTypeOfNumber != aAddr2.iTypeOfNumber)
		{
		return EFalse;
		}
	if(aAddr1.iNumberPlan != aAddr2.iNumberPlan)
		{
		return EFalse;
		}
	if(aAddr1.iTelNumber != aAddr2.iTelNumber)
		{
		return EFalse;
		}
	return ETrue;
	}

/**
 * Compare two RMobilePhone::TMobilePhoneCFInfoEntryV1 objects.
 * 
 * @param aEntry1 the first object.
 * @param aEntry2 the second object.
 * @return ETrue iff the two object are the same.
 * 
 * */ 
TBool operator ==(const RMobilePhone::TMobilePhoneCFInfoEntryV1& aEntry1, 
		const RMobilePhone::TMobilePhoneCFInfoEntryV1& aEntry2)
	{
	if(aEntry1.iCondition!=aEntry2.iCondition)
		{
		return EFalse;
		}
	if(aEntry1.iServiceGroup!=aEntry2.iServiceGroup)
		{
		return EFalse;
		}
	if(aEntry1.iStatus!=aEntry2.iStatus)
		{
		return EFalse;
		}
	if(!(aEntry1.iNumber==aEntry2.iNumber))
		{
		return EFalse;
		}
	if(aEntry1.iTimeout!=aEntry2.iTimeout)
		{
		return EFalse;
		}
	
	return ETrue;
	}

/**
 * Compare two RMobilePhone::TMobilePhoneCWInfoEntryV1 objects.
 * 
 * @param aEntry1 the first object.
 * @param aEntry2 the second object.
 * @return ETrue iff the two object are the same.
 * 
 * */ 
TBool operator ==(const RMobilePhone::TMobilePhoneCWInfoEntryV1& aEntry1, 
		const RMobilePhone::TMobilePhoneCWInfoEntryV1& aEntry2)
	{
	if(aEntry1.iStatus!=aEntry2.iStatus)
		{
		return EFalse;
		}
	if(aEntry1.iServiceGroup!=aEntry2.iServiceGroup)
		{
		return EFalse;
		}
	return ETrue;
	}

/**
 * Compare two RMobilePhone::TMobilePhoneCWInfoEntryV1 objects.
 * 
 * @param aEntry1 the first object.
 * @param aEntry2 the second object.
 * @return ETrue iff the two object are the same.
 * 
 * */ 
TBool operator ==(const RMobilePhone::TMobilePhoneCBInfoEntryV1& aEntry1, 
		const RMobilePhone::TMobilePhoneCBInfoEntryV1& aEntry2)
	{
	if(aEntry1.iStatus!=aEntry2.iStatus)
		{
		return EFalse;
		}
	if(aEntry1.iCondition!=aEntry2.iCondition)
		{
		return EFalse;
		}
	if(aEntry1.iServiceGroup!=aEntry2.iServiceGroup)
		{
		return EFalse;
		}
	return ETrue;
	}

/**
 * Compare two CMobilePhoneReadOnlyList objects.
 * 
 * @param aList1 the first object.
 * @param aList2 the second object.
 * @return ETrue iff the two object are the same.
 * 
 * */ 
template<class T>
TBool operator ==(const CMobilePhoneReadOnlyList<T>& aList1, 
		const CMobilePhoneReadOnlyList<T>& aList2)
	{
	TInt size1 = aList1.Enumerate();
	TInt size2 = aList2.Enumerate();
	if(size1 != size2)
		{
		return EFalse;
		}
	for(TInt i = 0; i < size1; ++i)
		{
		if(!(aList1.GetEntryL(i) == aList2.GetEntryL(i)))
			{
			return EFalse;
			}
		}
	return ETrue;
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UC0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for notify, set and get the call forwarding
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCallForwardingActive, RMobilePhone::NotifyCallForwardingStatusChange, RMobilePhone::SetCallForwardingStatus, CRetrieveMobilePhoneCFList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySupplementaryServicesFU::TestUseCase0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);	

	 
	// post NotifyCallForwardingActive and NotifyCallForwardingStatusChange 
	TRequestStatus notifyCallForwardingActiveStatus;
	RMobilePhone::TMobileService service;
	RMobilePhone::TMobilePhoneCFActive active;
	iPhone.NotifyCallForwardingActive(notifyCallForwardingActiveStatus,service,active);
	
	TRequestStatus notifyCallForwardingStatusChangeStatus;
	RMobilePhone::TMobilePhoneCFCondition notifyCondition;
	iPhone.NotifyCallForwardingStatusChange(notifyCallForwardingStatusChangeStatus,notifyCondition);

	// prepare ltsy to accept and answer the SetCallForwardingStatus
	RMobilePhone::TMobilePhoneCFCondition setCondition = RMobilePhone::ECallForwardingNoReply;
	RMobilePhone::TMobilePhoneCFChangeV1  callForwardinginfo;
	callForwardinginfo.iAction = RMobilePhone::EServiceActionRegister;
	callForwardinginfo.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	callForwardinginfo.iNumber.iNumberPlan   = RMobilePhone::EIsdnNumberPlan;
	_LIT(KTelNumber,"+74951000000");
	callForwardinginfo.iNumber.iTelNumber.Copy(KTelNumber);
	callForwardinginfo.iServiceGroup = RMobilePhone::EVoiceService;
	callForwardinginfo.iTimeout = 20;
	TMockLtsyData5<RMobilePhone::TMobilePhoneCFCondition, 
	               RMobilePhone::TMobileService,
	               RMobilePhone::TMobilePhoneServiceAction,
	               TInt,
	               TBuf<RMobilePhone::KMaxMobileTelNumberSize> > mockCompleteSetData(setCondition,
	            		   callForwardinginfo.iServiceGroup,
	            		   callForwardinginfo.iAction,
	            		   callForwardinginfo.iTimeout,
	            		   callForwardinginfo.iNumber.iTelNumber);
	data.Close();
	mockCompleteSetData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesSetCallForwardingStatus::KLtsyDispatchSupplementaryServicesSetCallForwardingStatusApiId, data, KErrNone);
	
	// call to RMobilePhone::SetCallForwardingStatus
	TRequestStatus requestStatusSet;
	iPhone.SetCallForwardingStatus(requestStatusSet, setCondition, callForwardinginfo);
	ASSERT_EQUALS(KRequestPending, notifyCallForwardingActiveStatus.Int());
	ASSERT_EQUALS(KRequestPending, notifyCallForwardingStatusChangeStatus.Int());
	ASSERT_EQUALS(KRequestPending, requestStatusSet.Int());

	// complete NotifyCallForwardingStatusChange
	RMobilePhone::TMobilePhoneCFCondition completeCondition = RMobilePhone::ECallForwardingNoReply;
	TMockLtsyData1<RMobilePhone::TMobilePhoneCFCondition> mockCompleteNotifyCFStatusChangeData(completeCondition);
	data.Close();
	mockCompleteNotifyCFStatusChangeData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyCallForwardingStatusChangeIndId, KErrNone, data);
	
	//Wait for NotifyCallForwardingStatusChange
	User::WaitForRequest(notifyCallForwardingStatusChangeStatus);
	ASSERT_EQUALS(KErrNone, notifyCallForwardingStatusChangeStatus.Int());
	ASSERT_EQUALS(KRequestPending, requestStatusSet.Int());
	ASSERT_EQUALS(notifyCondition, completeCondition);
	
	// complete SetCallForwardingStatus
	iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesSetCallForwardingStatus::KLtsyDispatchSupplementaryServicesSetCallForwardingStatusApiId, KErrNone);
	
	//Wait for SetCallForwardingStatus
	User::WaitForRequest(requestStatusSet);
	ASSERT_EQUALS(KErrNone, requestStatusSet.Int());
	
	// complete notify NotifyCallForwardingActive
	RMobilePhone::TMobileService completeService = callForwardinginfo.iServiceGroup;
	RMobilePhone::TMobilePhoneCFActive completeActive = RMobilePhone::ECFConditionalActive;
	TMockLtsyData2<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneCFActive> mockCompleteNotifyCFAData(completeService,completeActive);
	data.Close();
	mockCompleteNotifyCFAData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyCallForwardingActiveIndId, KErrNone, data);
	
	//Wait for NotifyCallForwardingActive
	User::WaitForRequest(notifyCallForwardingActiveStatus);
	ASSERT_EQUALS(KErrNone, notifyCallForwardingActiveStatus.Int());
	ASSERT_EQUALS(service, completeService);
	ASSERT_EQUALS(active, completeActive);
	
	// prepare ltsy to accept the CRetrieveMobilePhoneCFList::Start
	
	//CFilteringActiveScheduler does not need to be used, this should be changed
	//to a listretrieverao type class
	CFilteringActiveScheduler* scheduler = new (ELeave) CFilteringActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	RMobilePhone::TMobilePhoneCFCondition getCondition = RMobilePhone::ECallForwardingBusy;
	RMobilePhone::TMobileService getServiceGroup = RMobilePhone::EVoiceService;
	TMockLtsyData2<RMobilePhone::TMobilePhoneCFCondition, RMobilePhone::TMobileService> mockCompleteGetData(getCondition,getServiceGroup);
	data.Close();
	mockCompleteGetData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesGetCallForwardingStatus::KLtsyDispatchSupplementaryServicesGetCallForwardingStatusApiId, data, KErrNone);	

	// Get the call forwarding list
	TRequestStatus requestStatusGet;
	RMobilePhone::TMobilePhoneCFCondition condition = getCondition;
	RMobilePhone::TMobileService serviceGroup = getServiceGroup;
	CRetrieveMobilePhoneCFList* asyncRetrieveMobilePhoneCFList =
	                                       CRetrieveMobilePhoneCFList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCFList);
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCFList);
	CleanupStack::PushL(activeRetriever);
	scheduler->AddRetrieverL(*activeRetriever);
	asyncRetrieveMobilePhoneCFList->Start(activeRetriever->iStatus, condition, serviceGroup);
	activeRetriever->Activate();
	
	// Complete get call forwarding list
	CMobilePhoneCFList* completeCFList = CMobilePhoneCFList::NewL();
	CleanupStack::PushL(completeCFList);
	RMobilePhone::TMobilePhoneCFInfoEntryV1	entryCFInfoEntry;
	entryCFInfoEntry.iTimeout              = 20;
	entryCFInfoEntry.iCondition            = RMobilePhone::ECallForwardingUnconditional;
	entryCFInfoEntry.iServiceGroup         = RMobilePhone::EVoiceService;
	entryCFInfoEntry.iStatus               = RMobilePhone::ECallForwardingStatusNotProvisioned;
	entryCFInfoEntry.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	entryCFInfoEntry.iNumber.iNumberPlan   = RMobilePhone::EIsdnNumberPlan;
	entryCFInfoEntry.iNumber.iTelNumber.Copy(KTelNumber);
	completeCFList->AddEntryL(entryCFInfoEntry);
	entryCFInfoEntry.iTimeout              = 15;
	entryCFInfoEntry.iCondition            = RMobilePhone::ECallForwardingNoReply;
	entryCFInfoEntry.iServiceGroup         = RMobilePhone::EServiceUnspecified;
	entryCFInfoEntry.iStatus               = RMobilePhone::ECallForwardingStatusActive;
	entryCFInfoEntry.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	entryCFInfoEntry.iNumber.iNumberPlan   = RMobilePhone::ENationalNumberPlan;
	_LIT(KTelNumber2,"123456789");
	entryCFInfoEntry.iNumber.iTelNumber.Copy(KTelNumber2);
	completeCFList->AddEntryL(entryCFInfoEntry);
	CBufBase* completeCFListBuf = completeCFList->StoreLC();
	data.Create(completeCFListBuf->Ptr(0));
    iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesGetCallForwardingStatus::KLtsyDispatchSupplementaryServicesGetCallForwardingStatusApiId, KErrNone, data);

	//Wait for CRetrieveMobilePhoneCFList::Start
	scheduler->StartScheduler();
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	CMobilePhoneCFList *list = asyncRetrieveMobilePhoneCFList->RetrieveListL();
	ASSERT_EQUALS(*completeCFList,*list);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this); // completeCFListBuf, completeCFList, activeRetriever, asyncRetrieveMobilePhoneCFList, scheduler, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UC0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for send a network service request string (with and without FDN check)
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendNetworkServiceRequest, RMobilePhone::SendNetworkServiceRequestNoFdnCheck
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySupplementaryServicesFU::TestUseCase0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);	

	// prepare ltsy to accept and answer the SendNetworkServiceRequest
	_LIT(KMsg1,"SendNetworkServiceRequest1.1");
	TName expServiceString(KMsg1);
 	TMockLtsyData1<TName > expLtsyData(expServiceString);
    expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestApiId, data, KErrNone);
	
	// Send Network Service Request
	TRequestStatus sendNetworkServiceRequestStatus;
	iPhone.SendNetworkServiceRequest(sendNetworkServiceRequestStatus,expServiceString);
	ASSERT_EQUALS(KRequestPending, sendNetworkServiceRequestStatus.Int());

	// complete notify SendNetworkServiceRequest
	iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestApiId, KErrNone);
	
	//Wait for NotifyCallForwardingActive
	User::WaitForRequest(sendNetworkServiceRequestStatus);
	ASSERT_EQUALS(KErrNone, sendNetworkServiceRequestStatus.Int());

	// prepare ltsy to accept and answer the SendNetworkServiceRequestNoFdnCheck
	_LIT(KMsg2,"SendNetworkServiceRequest1.2");
	TName expServiceStringNoFDN(KMsg2);
 	TMockLtsyData1<TName > expLtsyDataNoFDN(expServiceStringNoFDN);
 	expLtsyDataNoFDN.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiId, data, KErrNone);
	
	// Send Network Service Request
	TRequestStatus sendNetworkServiceRequestNoFDNStatus;
	iPhone.SendNetworkServiceRequestNoFdnCheck(sendNetworkServiceRequestNoFDNStatus,expServiceStringNoFDN);
	ASSERT_EQUALS(KRequestPending, sendNetworkServiceRequestNoFDNStatus.Int());

	// complete notify SendNetworkServiceRequestNoFDNCheck
	iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiId, KErrNone);

	//Wait for SendNetworkServiceRequestNoFDNCheck
	User::WaitForRequest(sendNetworkServiceRequestNoFDNStatus);
	ASSERT_EQUALS(KErrNone, sendNetworkServiceRequestNoFDNStatus.Int());
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UC0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for send a USSD request to the network
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::ReceiveMessage, RMobileUssdMessaging::SendMessage, RMobileUssdMessaging::SendRelease
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySupplementaryServicesFU::TestUseCase0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);	
	RMobileUssdMessaging ussdMessaging;
	ASSERT_EQUALS(KErrNone, ussdMessaging.Open(iPhone));
	CleanupClosePushL(ussdMessaging);

	// Request CTSY to notify for USSD message
	RMobileUssdMessaging::TGsmUssdMessageData recvData;
	RMobileUssdMessaging::TMobileUssdAttributesV1 recvAttr; 
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg recvAttrPckg(recvAttr);
	TRequestStatus receiveMessageStatus;
	ussdMessaging.ReceiveMessage(receiveMessageStatus,recvData,recvAttrPckg);
	ASSERT_EQUALS(KRequestPending, receiveMessageStatus.Int());
	
	
	// prepare ltsy to accept the SendMessage
	_LIT8(KMsg1,"SendMessage1.1");
	RMobileUssdMessaging::TGsmUssdMessageData expSendData(KMsg1);
	RMobileUssdMessaging::TMobileUssdAttributesV1 expSendAttr;
	expSendAttr.iDcs = 50;
	expSendAttr.iFormat = RMobileUssdMessaging::EFormatUnspecified;
	expSendAttr.iType = RMobileUssdMessaging::EUssdMORequest;
	expSendAttr.iFlags = RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageType | RMobileUssdMessaging::KUssdMessageDcs;
 	TMockLtsyData2<RMobileUssdMessaging::TGsmUssdMessageData, RMobileUssdMessaging::TMobileUssdAttributesV1> expLtsySendData(expSendData, expSendAttr);
 	expLtsySendData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesSendUssdMessage::KLtsyDispatchSupplementaryServicesSendUssdMessageApiId, data, KErrNone);
	
	// Send USSD Message
	TRequestStatus sendMessageStatus;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg sendAttrPckg(expSendAttr);
	ussdMessaging.SendMessage(sendMessageStatus,expSendData,sendAttrPckg);
	ASSERT_EQUALS(KRequestPending, sendMessageStatus.Int());
	ASSERT_EQUALS(KRequestPending, receiveMessageStatus.Int());

	// complete SendMessage
	iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesSendUssdMessage::KLtsyDispatchSupplementaryServicesSendUssdMessageApiId, KErrNone);
	
	//Wait for SendMessage
	User::WaitForRequest(sendMessageStatus);
	ASSERT_EQUALS(KErrNone, sendMessageStatus.Int());
	ASSERT_EQUALS(KRequestPending, receiveMessageStatus.Int());

	// complete ReceiveMessage
	_LIT8(KMsg2,"ReceiveMessage1.2");
	TDesC8* messagePtr = const_cast<TDesC8*>(&KMsg2);
	RMobileUssdMessaging::TMobileUssdAttributesV1 compRecvAttr;
	compRecvAttr.iDcs = 16;
	compRecvAttr.iFormat = RMobileUssdMessaging::EFormatUnspecified;
	compRecvAttr.iType = RMobileUssdMessaging::EUssdMTReply;
	compRecvAttr.iFlags = RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageType | RMobileUssdMessaging::KUssdMessageDcs;
	TMockLtsyData2<TDesC8*, RMobileUssdMessaging::TMobileUssdAttributesV1> mockCompleteReceiveMessageData(messagePtr,compRecvAttr);
	data.Close();
	mockCompleteReceiveMessageData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesReceiveUssdMessageIndId, KErrNone, data);

	//Wait for ReceiveMessage
	User::WaitForRequest(receiveMessageStatus);
	ASSERT_EQUALS(KErrNone, receiveMessageStatus.Int());
	ASSERT_TRUE(recvData == *messagePtr);
	ASSERT_EQUALS(recvAttr.iDcs, compRecvAttr.iDcs);
	ASSERT_EQUALS(recvAttr.iFormat, compRecvAttr.iFormat);
	ASSERT_EQUALS(recvAttr.iType, compRecvAttr.iType);
	ASSERT_EQUALS(recvAttr.iFlags, compRecvAttr.iFlags);

	// prepare ltsy to accept the SendRelease
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesSendUssdRelease::KLtsyDispatchSupplementaryServicesSendUssdReleaseApiId, KErrNone);
	
	// Send USSD Release
	TRequestStatus sendReleaseStatus;
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendSSRequest;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg sendSSRequestPckg(sendSSRequest);
	ussdMessaging.SendRelease(sendReleaseStatus,sendSSRequestPckg);
	ASSERT_EQUALS(KRequestPending, sendReleaseStatus.Int());

	// complete SendRelease
	RMobilePhone::TMobilePhoneSendSSRequestV3 compSendSSRequest;
	_LIT(KMsg3,"SendRelease1.3");
	compSendSSRequest.iAdditionalInfo = KMsg3;
	compSendSSRequest.iOpCode = 14;
 	TMockLtsyData1<RMobilePhone::TMobilePhoneSendSSRequestV3> expLtsySendRelease(compSendSSRequest);
	data.Close();
 	expLtsySendRelease.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesSendUssdRelease::KLtsyDispatchSupplementaryServicesSendUssdReleaseApiId, KErrNone, data);
	
	//Wait for SendRelease
	User::WaitForRequest(sendReleaseStatus);
	ASSERT_EQUALS(KErrNone, sendReleaseStatus.Int());
	// CTSY defect:  defect id 380202 in the component tests. SendRelease does not propogate the return values into ETEL  
	//ASSERT_EQUALS(compSendSSRequest.iAdditionalInfo, sendSSRequest.iAdditionalInfo);
	//ASSERT_EQUALS(compSendSSRequest.iOpCode, sendSSRequest.iOpCode);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // ussdMessaging, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UC0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for notify, set and get the call barring
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallBarringStatus, RMobilePhone::NotifyCallBarringStatusChange, CRetrieveMobilePhoneCBList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySupplementaryServicesFU::TestUseCase0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);	

	// prepare ltsy to accept the CRetrieveMobilePhoneCBList::Start
	
	//CFilteringActiveScheduler does not need to be used, this should be changed
	//to a listretrieverao type class
	CFilteringActiveScheduler* scheduler = new (ELeave) CFilteringActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	RMobilePhone::TMobilePhoneCBCondition getCondition = RMobilePhone::EBarOutgoingInternational;
	TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> mockCompleteGetData(getCondition);
	data.Close();
	mockCompleteGetData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesGetCallBarringStatus::KLtsyDispatchSupplementaryServicesGetCallBarringStatusApiId, data, KErrNone);	

	// Get the call barring list
	TRequestStatus requestStatusGet;
	RMobilePhone::TMobilePhoneCBCondition condition = getCondition;
	CRetrieveMobilePhoneCBList* asyncRetrieveMobilePhoneCBList = CRetrieveMobilePhoneCBList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCBList);
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCBList);
	CleanupStack::PushL(activeRetriever);
	scheduler->AddRetrieverL(*activeRetriever);
	asyncRetrieveMobilePhoneCBList->Start(activeRetriever->iStatus, condition);
	activeRetriever->Activate();
	
	// Complete get call barring list
	CMobilePhoneCBList* completeCBList = CMobilePhoneCBList::NewL();
	CleanupStack::PushL(completeCBList);
	RMobilePhone::TMobilePhoneCBInfoEntryV1	entryCBInfoEntry;
	entryCBInfoEntry.iCondition = RMobilePhone::EBarOutgoingInternational;
	entryCBInfoEntry.iStatus = RMobilePhone::ECallBarringStatusActive;
	entryCBInfoEntry.iServiceGroup = RMobilePhone::EFaxService;
	completeCBList->AddEntryL(entryCBInfoEntry);
	entryCBInfoEntry.iCondition = RMobilePhone::EBarIncomingRoaming;
	entryCBInfoEntry.iStatus = RMobilePhone::ECallBarringStatusNotProvisioned;
	entryCBInfoEntry.iServiceGroup = RMobilePhone::EAllDataTele;
	completeCBList->AddEntryL(entryCBInfoEntry);
	CBufBase* completeCBListBuf = completeCBList->StoreLC();
	data.Create(completeCBListBuf->Ptr(0));
    iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesGetCallBarringStatus::KLtsyDispatchSupplementaryServicesGetCallBarringStatusApiId, KErrNone, data);

	//Wait for CRetrieveMobilePhoneCBList::Start
	scheduler->StartScheduler();
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	CMobilePhoneCBList *list = asyncRetrieveMobilePhoneCBList->RetrieveListL();
	ASSERT_EQUALS(*completeCBList,*list);

	// post NotifyCallBarringStatusChange 
	TRequestStatus notifyCallBarringStatusChangeStatus;
	RMobilePhone::TMobilePhoneCBCondition notifyCondition; 
	iPhone.NotifyCallBarringStatusChange(notifyCallBarringStatusChangeStatus, notifyCondition);
	ASSERT_EQUALS(KRequestPending, notifyCallBarringStatusChangeStatus.Int());

	// prepare ltsy to accept and answer the SetCallBarringStatus
	RMobilePhone::TMobilePhoneCBCondition setCondition = RMobilePhone::EBarIncomingRoaming;
	RMobilePhone::TMobilePhoneCBChangeV1 setInfo;
	setInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	setInfo.iServiceGroup = RMobilePhone::EPadAccess;
	_LIT(KPassword,"1234");
	setInfo.iPassword = KPassword;
	TMockLtsyData2<RMobilePhone::TMobilePhoneCBCondition, RMobilePhone::TMobilePhoneCBChangeV1> mockCompleteSetData(setCondition,setInfo);
	data.Close();
	mockCompleteSetData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesSetCallBarringStatus::KLtsyDispatchSupplementaryServicesSetCallBarringStatusApiId, data, KErrNone);
	
	// call to RMobilePhone::SetCallBarringStatus
	TRequestStatus requestStatusSet;
	iPhone.SetCallBarringStatus(requestStatusSet, setCondition, setInfo);
	ASSERT_EQUALS(KRequestPending, notifyCallBarringStatusChangeStatus.Int());
	ASSERT_EQUALS(KRequestPending, requestStatusSet.Int());

	// complete SetCallBarringStatus
	iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesSetCallBarringStatus::KLtsyDispatchSupplementaryServicesSetCallBarringStatusApiId, KErrNone);
	
	//Wait for SetCallBarringStatus
	User::WaitForRequest(requestStatusSet);
	ASSERT_EQUALS(KRequestPending, notifyCallBarringStatusChangeStatus.Int());
	ASSERT_EQUALS(KErrNone, requestStatusSet.Int());
	
	// complete NotifyCallBarringStatusChange
	RMobilePhone::TMobilePhoneCBCondition completeCondition = RMobilePhone::EBarIncomingRoaming;
	TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> mockCompleteNotifyCallBarringStatusChangeData(completeCondition);
	data.Close();
	mockCompleteNotifyCallBarringStatusChangeData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyCallBarringStatusChangeIndId, KErrNone, data);
	
	//Wait for NotifyCallBarringStatusChange
	User::WaitForRequest(notifyCallBarringStatusChangeStatus);
	ASSERT_EQUALS(KErrNone, notifyCallBarringStatusChangeStatus.Int());
	ASSERT_EQUALS(notifyCondition, completeCondition);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this); // completeCBListBuf, completeCBList, activeRetriever, asyncRetrieveMobilePhoneCBList, scheduler, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UC0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for notify, set and get the call waiting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetCallWaitingStatus, RMobilePhone::NotifyCallWaitingStatusChange, CRetrieveMobilePhoneCWList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySupplementaryServicesFU::TestUseCase0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);	

	// prepare ltsy to accept the CRetrieveMobilePhoneCWList::Start
	
	//CFilteringActiveScheduler does not need to be used, this should be changed
	//to a listretrieverao type class
	CFilteringActiveScheduler* scheduler = new (ELeave) CFilteringActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesGetCallWaitingStatus::KLtsyDispatchSupplementaryServicesGetCallWaitingStatusApiId, KErrNone);	

	// Get the call waiting list
	TRequestStatus requestStatusGet;
	CRetrieveMobilePhoneCWList* asyncRetrieveMobilePhoneCWList = CRetrieveMobilePhoneCWList::NewL(iPhone);    
	CleanupStack::PushL(asyncRetrieveMobilePhoneCWList);
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*asyncRetrieveMobilePhoneCWList);
	CleanupStack::PushL(activeRetriever);
	scheduler->AddRetrieverL(*activeRetriever);
	asyncRetrieveMobilePhoneCWList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	
	// Complete get call waiting list
	CMobilePhoneCWList* completeCWList = CMobilePhoneCWList::NewL();
	CleanupStack::PushL(completeCWList);
	RMobilePhone::TMobilePhoneCWInfoEntryV1	entryCWInfoEntry;
	entryCWInfoEntry.iStatus = RMobilePhone::ECallWaitingStatusActive;
	entryCWInfoEntry.iServiceGroup = RMobilePhone::EFaxService;
	completeCWList->AddEntryL(entryCWInfoEntry);
	entryCWInfoEntry.iStatus = RMobilePhone::ECallWaitingStatusNotProvisioned;
	entryCWInfoEntry.iServiceGroup = RMobilePhone::EAllDataTele;
	completeCWList->AddEntryL(entryCWInfoEntry);
	CBufBase* completeCWListBuf = completeCWList->StoreLC();
	data.Create(completeCWListBuf->Ptr(0));
    iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesGetCallWaitingStatus::KLtsyDispatchSupplementaryServicesGetCallWaitingStatusApiId, KErrNone, data);

	//Wait for CRetrieveMobilePhoneCWList::Start
	scheduler->StartScheduler();
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	CMobilePhoneCWList *list = asyncRetrieveMobilePhoneCWList->RetrieveListL();
	ASSERT_EQUALS(*completeCWList,*list);

	// post NotifyCallWaitingStatusChange
	TRequestStatus notifyCallWaitingStatusChangeStatus;
    RMobilePhone::TMobilePhoneCWInfoEntryV1 notifyResult;
    RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg notifyResultPckg(notifyResult);
	iPhone.NotifyCallWaitingStatusChange(notifyCallWaitingStatusChangeStatus, notifyResultPckg);
	ASSERT_EQUALS(KRequestPending, notifyCallWaitingStatusChangeStatus.Int());

	// prepare ltsy to accept and answer the SetCallWaitingStatus
    RMobilePhone::TMobileService setService = RMobilePhone::EPadAccess;
    RMobilePhone::TMobilePhoneServiceAction setAction = RMobilePhone::EServiceActionActivate;
	TMockLtsyData2<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneServiceAction> mockCompleteSetData(setService,setAction );
	data.Close();
	mockCompleteSetData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesSetCallWaitingStatus::KLtsyDispatchSupplementaryServicesSetCallWaitingStatusApiId, data, KErrNone);
	
	// call to RMobilePhone::SetCallWaitingStatus
	TRequestStatus requestStatusSet;
	iPhone.SetCallWaitingStatus(requestStatusSet, setService, setAction);
	ASSERT_EQUALS(KRequestPending, notifyCallWaitingStatusChangeStatus.Int());
	ASSERT_EQUALS(KRequestPending, requestStatusSet.Int());

	// complete SetCallWaitingStatus
	iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesSetCallWaitingStatus::KLtsyDispatchSupplementaryServicesSetCallWaitingStatusApiId, KErrNone);
	
	//Wait for SetCallWaitingStatus
	User::WaitForRequest(requestStatusSet);
	ASSERT_EQUALS(KRequestPending, notifyCallWaitingStatusChangeStatus.Int());
	ASSERT_EQUALS(KErrNone, requestStatusSet.Int());
	
	// complete NotifyCallWaitingStatusChange
	RMobilePhone::TMobileService completeService = RMobilePhone::EAllTele;
    RMobilePhone::TMobilePhoneCWStatus completeStatus = RMobilePhone::ECallWaitingStatusNotAvailable;
	TMockLtsyData2<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneCWStatus> mockCompleteNotifyCallWaitingStatusChangeData(completeService, completeStatus);
	data.Close();
	mockCompleteNotifyCallWaitingStatusChangeData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyCallWaitingStatusChangeIndId, KErrNone, data);
	
	//Wait for NotifyCallWaitingStatusChange
	User::WaitForRequest(notifyCallWaitingStatusChangeStatus);
	ASSERT_EQUALS(KErrNone, notifyCallWaitingStatusChangeStatus.Int());
	ASSERT_EQUALS(notifyResult.iStatus, completeStatus);
	ASSERT_EQUALS(notifyResult.iServiceGroup, completeService);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this); // completeCBListBuf, completeCBList, activeRetriever, asyncRetrieveMobilePhoneCBList, scheduler, data, this
	}

//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetSSPassword
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetSSPassword
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySupplementaryServicesFU::TestUnit0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	// Prepare LTSY to answer RMobilePhone::SetSSPassword
	_LIT(KOldPassword,"oldPswd");
	_LIT(KNewPassword,"newPswd");
	RMobilePhone::TMobilePhonePasswordChangeV2 pwdChange;
	pwdChange.iOldPassword.Copy(KOldPassword);
	pwdChange.iNewPassword.Copy(KNewPassword);
	pwdChange.iVerifiedPassword.Copy(KNewPassword);
	TPckg<RMobilePhone::TMobilePhonePasswordChangeV2> password(pwdChange);
	MLtsyDispatchSupplementaryServicesSetSsPassword::TServiceType service = MLtsyDispatchSupplementaryServicesSetSsPassword::ECallBarring;
	TMockLtsyData2<RMobilePhone::TMobilePhonePasswordChangeV2, MLtsyDispatchSupplementaryServicesSetSsPassword::TServiceType> ssData(pwdChange, service);
	ssData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesSetSsPassword::KLtsyDispatchSupplementaryServicesSetSsPasswordApiId, data, KErrNone);

	// complete notify SetSSPassword
	iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesSetSsPassword::KLtsyDispatchSupplementaryServicesSetSsPasswordApiId, KErrNone);

	// Send Network Service Request
	TRequestStatus setSSPasswordStatus;
	iPhone.SetSSPassword(setSSPasswordStatus,password,service);
	ASSERT_EQUALS(KRequestPending, setSSPasswordStatus.Int());
	
	//Wait for SetSSPassword
	User::WaitForRequest(setSSPasswordStatus);
	ASSERT_EQUALS(KErrNone, setSSPasswordStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}
	
	
	
/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::ClearCallBlackList
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ClearCallBlackList
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySupplementaryServicesFU::TestUnit0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	// Prepare LTSY to answer RMmCustomAPI::ClearCallBlackList
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesClearBlacklist::KLtsyDispatchSupplementaryServicesClearBlacklistApiId, KErrNone);

	// complete ClearCallBlackList request
	iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesClearBlacklist::KLtsyDispatchSupplementaryServicesClearBlacklistApiId, KErrNone);

	// Send Network Service Request
	TRequestStatus clearBlacklistStatus;
	customApi.ClearCallBlackList(clearBlacklistStatus);
	ASSERT_EQUALS(KRequestPending, clearBlacklistStatus.Int());
	
	//Wait for ClearCallBlackList
	User::WaitForRequest(clearBlacklistStatus);
	ASSERT_EQUALS(KErrNone, clearBlacklistStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // customApi, data, this
	
	}
	
	



/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::GetDiagnosticInfo
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetDiagnosticInfo
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySupplementaryServicesFU::TestUnit0003L()
	{
	// available causes, details in 3gpp 24.008 
	static const TUint8 KDiagnosticInfoBarredWithCUG = 0x81;
	static const TUint8 KDiagnosticInfoBarredNoCUG = 0x82;
	static const TUint8 KDiagnosticInfoBarredUnknownCUG = 0x83;
	static const TUint8 KDiagnosticInfoBarredIncompatibleCUG = 0x84;
	static const TUint8 KDiagnosticInfoBarredFailureCUG = 0x85;
	static const TUint8 KDiagnosticInfoBarredClirNotSubscribed = 0x86;
	static const TUint8 KDiagnosticInfoBarredCCBSPossible = 0x87;
	static const TUint8 KDiagnosticInfoBarredCCBSNotPossible = 0x88;
	static const TUint8 KDiagnosticInfoBarredWrong = 0x89;
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	TInt callId = 1;
	DriverDialCallL(callId, RMobilePhone::EVoiceService);

	RMobileLine mobileLine;
	CleanupClosePushL(mobileLine);
	RMobileCall mobileCall;
	CleanupClosePushL(mobileCall);
	ClientDialCallL(mobileLine, mobileCall, RMobilePhone::EVoiceService);

	// Getting the call information
	RMobileCall::TMobileCallInfoV1 callInfo;
	RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(callInfo);
	ASSERT_EQUALS(KErrNone, mobileCall.GetMobileCallInfo(callInfoPckg));
	ASSERT_EQUALS(callId , callInfo.iCallId);
	
	// Check GetDiagnosticInfo when no set was invoke
	ASSERT_EQUALS(KErrNone, customApi.GetDiagnosticInfo(callInfo.iCallName));
	
	// Set the diagnostic octet to "Outgoing calls barred within CUG"
	TRequestStatus terminatedStatus;
	TUint8 cause = KDiagnosticInfoBarredWithCUG;
	TMockLtsyData2<TInt, TUint8> diagData(callInfo.iCallId, cause);
	diagData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(terminatedStatus);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId, KErrNone, data);
	User::WaitForRequest(terminatedStatus);

	// Check GetDiagnosticInfo 
	ASSERT_EQUALS(KErrDiagnosticInfoBarredWithCUG, customApi.GetDiagnosticInfo(callInfo.iCallName));

	// Set the diagnostic octet to "No CUG selected"
	cause = KDiagnosticInfoBarredNoCUG;
	diagData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(terminatedStatus);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId, KErrNone, data);
	User::WaitForRequest(terminatedStatus);

	// Check GetDiagnosticInfo 
	ASSERT_EQUALS(KErrDiagnosticInfoBarredNoCUG, customApi.GetDiagnosticInfo(callInfo.iCallName));

	// Set the diagnostic octet to "Unknown CUG index"
	cause = KDiagnosticInfoBarredUnknownCUG;
	diagData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(terminatedStatus);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId, KErrNone, data);
	User::WaitForRequest(terminatedStatus);

	// Check GetDiagnosticInfo 
	ASSERT_EQUALS(KErrDiagnosticInfoBarredUnknownCUG, customApi.GetDiagnosticInfo(callInfo.iCallName));

	// Set the diagnostic octet to "CUG index incompatible with requested basic service"
	cause = KDiagnosticInfoBarredIncompatibleCUG;
	diagData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(terminatedStatus);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId, KErrNone, data);
	User::WaitForRequest(terminatedStatus);

	// Check GetDiagnosticInfo 
	ASSERT_EQUALS(KErrDiagnosticInfoBarredIncompatibleCUG, customApi.GetDiagnosticInfo(callInfo.iCallName));

	// Set the diagnostic octet to "CUG call failure, unspecified"
	cause = KDiagnosticInfoBarredFailureCUG;
	diagData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(terminatedStatus);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId, KErrNone, data);
	User::WaitForRequest(terminatedStatus);

	// Check GetDiagnosticInfo 
	ASSERT_EQUALS(KErrDiagnosticInfoBarredFailureCUG, customApi.GetDiagnosticInfo(callInfo.iCallName));

	// Set the diagnostic octet to "CLIR not subscribed"
	cause = KDiagnosticInfoBarredClirNotSubscribed;
	diagData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(terminatedStatus);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId, KErrNone, data);
	User::WaitForRequest(terminatedStatus);

	// Check GetDiagnosticInfo 
	ASSERT_EQUALS(KErrDiagnosticInfoBarredClirNotSubscribed, customApi.GetDiagnosticInfo(callInfo.iCallName));

	// Set the diagnostic octet to "CCBS possible"
	cause = KDiagnosticInfoBarredCCBSPossible;
	diagData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(terminatedStatus);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId, KErrNone, data);
	User::WaitForRequest(terminatedStatus);

	// Check GetDiagnosticInfo 
	ASSERT_EQUALS(KErrDiagnosticInfoBarredCCBSPossible, customApi.GetDiagnosticInfo(callInfo.iCallName));

	// Set the diagnostic octet to "CCBS not possible"
	cause = KDiagnosticInfoBarredCCBSNotPossible;
	diagData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(terminatedStatus);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId, KErrNone, data);
	User::WaitForRequest(terminatedStatus);

	// Check GetDiagnosticInfo 
	ASSERT_EQUALS(KErrDiagnosticInfoBarredCCBSNotPossible, customApi.GetDiagnosticInfo(callInfo.iCallName));

	// Set the diagnostic octet to wrong value
	cause = KDiagnosticInfoBarredWrong;
	diagData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(terminatedStatus);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId, KErrNone, data);
	User::WaitForRequest(terminatedStatus);

	// Check GetDiagnosticInfo 
	ASSERT_EQUALS(KErrNone, customApi.GetDiagnosticInfo(callInfo.iCallName));

	AssertMockLtsyStatusL();
	
	TInt hangUpCause = KErrGsmCCNormalCallClearing;
	TMockLtsyData2<TInt, TInt> mockData2(callId, hangUpCause);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId, data);
	
	CleanupStack::PopAndDestroy(5, this); // mobileCall, mobileLine, customApi, data, this
	
	}
	


/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::NotifySsNetworkEvent
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifySsNetworkEvent
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySupplementaryServicesFU::TestUnit0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	// A. forward mode notification 
	// register the notification
	TRequestStatus requestStatus;
	RMmCustomAPI::TSsTypeAndMode typeAndMode;
	RMmCustomAPI::TSsInfo info;
	customApi.NotifySsNetworkEvent(requestStatus, typeAndMode, info);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

	// complete NotifySsNetworkEvent
	RMmCustomAPI::TSsForwMode compForwardInfo = RMmCustomAPI::ESsIncCallForwToC;
	RMmCustomAPI::TSsTypeAndMode compTypeAndMode;
	compTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotProvisioned;
	compTypeAndMode.iSsType = RMmCustomAPI::ESsForwNoReach; 
	
	data.Close();
	TMockLtsyData2<RMmCustomAPI::TSsTypeAndMode, RMmCustomAPI::TSsForwMode> compData1(compTypeAndMode, compForwardInfo);
	compData1.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventForwardModeIndId, KErrNone, data);

	// Check Notification
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compForwardInfo, info.iForwMode);
	ASSERT_EQUALS(compTypeAndMode.iSsMode, typeAndMode.iSsMode);
	ASSERT_EQUALS(compTypeAndMode.iSsType, typeAndMode.iSsType);
	
	// B. call waiting notification 
	// register the notification
	customApi.NotifySsNetworkEvent(requestStatus, typeAndMode, info);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

	// complete NotifySsNetworkEvent
	TBool compCallWaitingInfo = ETrue;
	compTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotRegistered;
	
	data.Close();
	TMockLtsyData2<RMmCustomAPI::TSsMode, TBool> compData2(compTypeAndMode.iSsMode, compCallWaitingInfo);
	compData2.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventCallWaitingIndId, KErrNone, data);

	// Check Notification
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compCallWaitingInfo, info.iCallWait);
	ASSERT_EQUALS(compTypeAndMode.iSsMode, typeAndMode.iSsMode);
	ASSERT_EQUALS(RMmCustomAPI::ESsCallWaiting, typeAndMode.iSsType);
	
	// C. hold mode notification 
	// register the notification
	customApi.NotifySsNetworkEvent(requestStatus, typeAndMode, info);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

	// complete NotifySsNetworkEvent
	RMmCustomAPI::TSsHoldMode compHoldMode = RMmCustomAPI::ESsHoldResume;
	compTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotAvailable;
	compTypeAndMode.iSsType = RMmCustomAPI::ESsBarrOutInterExcHome; 
	
	data.Close();
	TMockLtsyData2<RMmCustomAPI::TSsTypeAndMode, RMmCustomAPI::TSsHoldMode> compData3(compTypeAndMode, compHoldMode);
	compData3.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventHoldModeIndId, KErrNone, data);

	// Check Notification
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compHoldMode, info.iCallHold);
	ASSERT_EQUALS(compTypeAndMode.iSsMode, typeAndMode.iSsMode);
	ASSERT_EQUALS(compTypeAndMode.iSsType, typeAndMode.iSsType);
	
	// D. Confrence notification 
	// register the notification
	customApi.NotifySsNetworkEvent(requestStatus, typeAndMode, info);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

	// complete NotifySsNetworkEvent
	TBool compConf = EFalse;
	compTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotAvailable;
	compTypeAndMode.iSsType = RMmCustomAPI::ESsBarrOutInterExcHome; 
	
	data.Close();
	TMockLtsyData2<RMmCustomAPI::TSsTypeAndMode, TBool> compData4(compTypeAndMode, compConf);
	compData4.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventConfrenceIndId, KErrNone, data);

	// Check Notification
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compConf, info.iConfInd);
	ASSERT_EQUALS(compTypeAndMode.iSsMode, typeAndMode.iSsMode);
	ASSERT_EQUALS(compTypeAndMode.iSsType, typeAndMode.iSsType);
	
	// E. Cug notification 
	// register the notification
	customApi.NotifySsNetworkEvent(requestStatus, typeAndMode, info);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

	// complete NotifySsNetworkEvent
	TUint16 compCugIndex = 181;
	compTypeAndMode.iSsMode = RMmCustomAPI::ESsModeActive;
	compTypeAndMode.iSsType = RMmCustomAPI::ESsIncomingBarrServ; 
	
	data.Close();
	TMockLtsyData2<RMmCustomAPI::TSsTypeAndMode, TUint16> compData5(compTypeAndMode, compCugIndex);
	compData5.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventCugIndId, KErrNone, data);

	// Check Notification
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compCugIndex, info.iCugIndex);
	ASSERT_EQUALS(compTypeAndMode.iSsMode, typeAndMode.iSsMode);
	ASSERT_EQUALS(compTypeAndMode.iSsType, typeAndMode.iSsType);
	
	// F. Clir Suppression notification 
	// register the notification
	customApi.NotifySsNetworkEvent(requestStatus, typeAndMode, info);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

	// complete NotifySsNetworkEvent
	TBool compClir = EFalse;
	compTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotProvisioned;
	
	data.Close();
	TMockLtsyData2<RMmCustomAPI::TSsMode, TBool> compData6(compTypeAndMode.iSsMode, compClir);
	compData6.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventClirSuppressionIndId, KErrNone, data);

	// Check Notification
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compClir, info.iClirSuppReject);
	ASSERT_EQUALS(compTypeAndMode.iSsMode, typeAndMode.iSsMode);
	ASSERT_EQUALS(RMmCustomAPI::ESsClir, typeAndMode.iSsType);
	
	// G. Ect notification 
	// register the notification
	customApi.NotifySsNetworkEvent(requestStatus, typeAndMode, info);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

	// complete NotifySsNetworkEvent
	RMmCustomAPI::TSsEctState compEctState = RMmCustomAPI::ESsEctAlerting;
	RMmCustomAPI::TSsChoice compEctChoice = RMmCustomAPI::ESsPresRestrictedAddress;
	_LIT(KRemoteAddress,"74951000123");
	TBuf<RMmCustomAPI::KRemoteAddressSize> compRemoteAddress(KRemoteAddress); 
	compTypeAndMode.iSsMode = RMmCustomAPI::ESsModeActive;
	compTypeAndMode.iSsType = RMmCustomAPI::ESsIncomingBarrServ; 
	
	data.Close();
	TMockLtsyData4<RMmCustomAPI::TSsTypeAndMode, RMmCustomAPI::TSsEctState, 
	                  RMmCustomAPI::TSsChoice, TBuf<RMmCustomAPI::KRemoteAddressSize> > compData7(compTypeAndMode, compEctState, compEctChoice, compRemoteAddress);
	compData7.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventEctCallStateIndId, KErrNone, data);

	// Check Notification
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compEctState, info.iEctCallState);
	ASSERT_EQUALS(compEctChoice, info.iChoice);
	ASSERT_EQUALS(compRemoteAddress, info.iRemoteAddress);
	ASSERT_EQUALS(compTypeAndMode.iSsMode, typeAndMode.iSsMode);
	ASSERT_EQUALS(compTypeAndMode.iSsType, typeAndMode.iSsType);
	
	// H. General notification 
	// register the notification
	customApi.NotifySsNetworkEvent(requestStatus, typeAndMode, info);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

	// complete NotifySsNetworkEvent
	compTypeAndMode.iSsMode = RMmCustomAPI::ESsModeNotActive;
	compTypeAndMode.iSsType = RMmCustomAPI::ESsColr; 
	
	data.Close();
	TMockLtsyData1<RMmCustomAPI::TSsTypeAndMode> compData8(compTypeAndMode);
	compData8.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventIndId, KErrNone, data);

	// Check Notification
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compTypeAndMode.iSsMode, typeAndMode.iSsMode);
	ASSERT_EQUALS(compTypeAndMode.iSsType, typeAndMode.iSsType);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // customApi, data, this
	
	}
	

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SsAdditionalInfoNotification
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SsAdditionalInfoNotification
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySupplementaryServicesFU::TestUnit0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	// register the notification
	TRequestStatus requestStatus;
	RMmCustomAPI::TSsAdditionalInfo additionalInfo;
	customApi.SsAdditionalInfoNotification(requestStatus, additionalInfo);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

	// complete SsAdditionalInfoNotification
	RMmCustomAPI::TSsAdditionalInfo compAdditionalInfo;
	compAdditionalInfo.iOperationCode = 74;
	_LIT8(KAdditionalInfo, "Additional Info");
	compAdditionalInfo.iAdditionalInfo = KAdditionalInfo;
	TMockLtsyData1<RMmCustomAPI::TSsAdditionalInfo> compData(compAdditionalInfo);
	compData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyAdditionalInfoIndId, KErrNone, data);

	// Check Notification
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compAdditionalInfo.iOperationCode, additionalInfo.iOperationCode);
	ASSERT_EQUALS(compAdditionalInfo.iAdditionalInfo, additionalInfo.iAdditionalInfo);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // customApi, data, this
	
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMmCustomAPI::SsRequestCompleteNotification
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SsRequestCompleteNotification
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySupplementaryServicesFU::TestUnit0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	// register the notification
	TRequestStatus requestStatus;
	TInt status;
	customApi.SsRequestCompleteNotification(requestStatus, status);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());

	// complete SsAdditionalInfoNotification with an arbitrary status value (6812)
	TInt compStatus = 6812;
	TMockLtsyData1<TInt> compData(compStatus);
	compData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifyRequestCompleteIndId, KErrNone, data);

	// Check Notification
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(compStatus, status);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // customApi, data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifySendNetworkServiceRequest()
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifySendNetworkServiceRequest()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySupplementaryServicesFU::TestUnit0007L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenPhoneL();
	
	TRequestStatus reqStatus;
	
	//test the Notify in the KErrNone case
	
	RMobilePhone::TMobilePhoneNotifySendSSOperation operation = RMobilePhone::ESendSSReturnResult;
	_LIT(KAddInfo,"ADDINFO");
	

	TDesC* additionalInfoPtr = const_cast<TDesC*>(&KAddInfo);
	TMockLtsyData2<RMobilePhone::TMobilePhoneNotifySendSSOperation, TDesC*> notifySendNetworkServiceRequestCompData(operation,additionalInfoPtr);
	notifySendNetworkServiceRequestCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifySendNetworkServiceRequestIndId, KErrNone, data);
	data.Close();
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendSsRequest;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg sendSsRequestPckg(sendSsRequest);
	iPhone.NotifySendNetworkServiceRequest(reqStatus, operation, sendSsRequestPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(operation, static_cast<RMobilePhone::TMobilePhoneNotifySendSSOperation>(sendSsRequest.iOpCode));
	ASSERT_TRUE(KAddInfo() == sendSsRequest.iAdditionalInfo);
		
	AssertMockLtsyStatusL();

	//test the Notify in the KErrGeneral case
	notifySendNetworkServiceRequestCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifySendNetworkServiceRequestIndId, KErrGeneral, data);
	data.Close();

	iPhone.NotifySendNetworkServiceRequest(reqStatus, operation, sendSsRequestPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrGeneral);

	AssertMockLtsyStatusL();

	//test the case where a indicator is sent up on an operation not being notified of.

	//send up a indicator on a Operation we are not waiting on. 
	operation = RMobilePhone::ESendSSInvoke;
	notifySendNetworkServiceRequestCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifySendNetworkServiceRequestIndId, KErrNone, data);
	data.Close();

	//send up a indicator on a Operation we are waiting on. 
	operation = RMobilePhone::ESendSSReturnResult;
	notifySendNetworkServiceRequestCompData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSupplementaryServicesNotifySendNetworkServiceRequestIndId, KErrNone, data);
	data.Close();

	RMobilePhone::TMobilePhoneSendSSRequestV3 sendSsRequest2;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg sendSsRequestPckg2(sendSsRequest2);
	iPhone.NotifySendNetworkServiceRequest(reqStatus, operation, sendSsRequestPckg2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(), KErrNone);
	ASSERT_EQUALS(operation, static_cast<RMobilePhone::TMobilePhoneNotifySendSSOperation>(sendSsRequest2.iOpCode));
	ASSERT_TRUE(KAddInfo() == sendSsRequest2.iAdditionalInfo);
	
	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-SUPPLEMENTARYSERVICES-UN0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for send a USSD request to the network without FDN check
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySupplementaryServicesFU::TestUnit0008L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);	
	RMobileUssdMessaging ussdMessaging;
	ASSERT_EQUALS(KErrNone, ussdMessaging.Open(iPhone));
	CleanupClosePushL(ussdMessaging);

	
	// prepare ltsy to accept the SendMessage
	_LIT8(KMsg1,"SendMessage1.1");
	RMobileUssdMessaging::TGsmUssdMessageData expSendData(KMsg1);
	RMobileUssdMessaging::TMobileUssdAttributesV1 expSendAttr;
	expSendAttr.iDcs = 50;
	expSendAttr.iFormat = RMobileUssdMessaging::EFormatUnspecified;
	expSendAttr.iType = RMobileUssdMessaging::EUssdMORequest;
	expSendAttr.iFlags = RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageType | RMobileUssdMessaging::KUssdMessageDcs;
 	TMockLtsyData2<RMobileUssdMessaging::TGsmUssdMessageData, RMobileUssdMessaging::TMobileUssdAttributesV1> expLtsySendData(expSendData, expSendAttr);
 	expLtsySendData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck::KLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheckApiId, data, KErrNone);
	
	// Send USSD Message
	TRequestStatus sendMessageStatus;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg sendAttrPckg(expSendAttr);
	ussdMessaging.SendMessageNoFdnCheck(sendMessageStatus,expSendData,sendAttrPckg);
	ASSERT_EQUALS(KRequestPending, sendMessageStatus.Int());

	// complete SendMessage
	iMockLTSY.CompleteL(MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck::KLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheckApiId, KErrNone);
	
	//Wait for SendMessage
	User::WaitForRequest(sendMessageStatus);
	ASSERT_EQUALS(KErrNone, sendMessageStatus.Int());


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // ussdMessaging, data, this
	}
