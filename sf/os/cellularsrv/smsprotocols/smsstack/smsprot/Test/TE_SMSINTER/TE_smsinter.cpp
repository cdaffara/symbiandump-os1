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

#include <e32std.h>

#include "TE_smsinter.h"


TVerdict CSmsSendRecvTextFormated7BitEMS::doTestStepL()
	{
	SendRecvTextFormated7BitEMSL();

	return TestStepResult();
	} // CSmsSendRecvTextFormated7BitEMS::doTestStepL


TVerdict CSmsSendRecvTextFormated8BitEMS::doTestStepL()
	{
	SendRecvTextFormated8BitEMSL();

	return TestStepResult();
	} // CSmsSendRecvTextFormated8BitEMS::doTestStepL


TVerdict CSmsSendRecvTextFormated16BitEMS::doTestStepL()
	{
	SendRecvTextFormated16BitEMSL();
	
	return TestStepResult();
	} // CSmsSendRecvTextFormated16BitEMS::doTestStepL


TVerdict CSmsEnumerateReadWriteAndDeleteEMSOnSIM::doTestStepL()
	{
	EnumerateReadWriteAndDeleteEMSOnSIML();
	
	return TestStepResult();
	} // CSmsEnumerateReadWriteAndDeleteEMSOnSIM::doTestStepL


TVerdict CSmsSendRecvPreDefSoundEMS::doTestStepL()
	{
	SendRecvPreDefSoundEMSL();
	
	return TestStepResult();
	} // CSmsSendRecvPreDefSoundEMS::doTestStepL


TVerdict CSmsSendRecvUserDefSoundEMS::doTestStepL()
	{
	SendRecvUserDefSoundEMSL();
	
	return TestStepResult();
	} // CSmsSendRecvUserDefSoundEMS::doTestStepL


TVerdict CSmsEnumerateReadWriteAndDeleteEMSOnPhone::doTestStepL()
	{
	EnumerateReadWriteAndDeleteEMSOnPhoneL();
	
	return TestStepResult();
	} // CSmsEnumerateReadWriteAndDeleteEMSOnPhone::doTestStepL


TVerdict CSmsSendRecvPicturesEMS::doTestStepL()
	{
	SendRecvPicturesEMSL();
	
	return TestStepResult();
	} // CSmsSendRecvPicturesEMS::doTestStepL


TVerdict CSmsSendRecvPreDefAnimation::doTestStepL()
	{
	SendRecvPreDefAnimationL();
	
	return TestStepResult();
	} // CSmsSendRecvPreDefAnimation::doTestStepL


TVerdict CSmsSendRecvUserDefAnimation::doTestStepL()
	{
	SendRecvUserDefAnimationL();
	
	return TestStepResult();
	} // CSmsSendRecvUserDefAnimation::doTestStepL


TVerdict CSmsSendRecvUserPromptEMS::doTestStepL()
	{
	SendRecvUserPromptEMSL();
	
	return TestStepResult();
	} // CSmsSendRecvUserPromptEMS::doTestStepL


TVerdict CSmsSendRecvObjectDistributionEMS::doTestStepL()
	{
	SendRecvObjectDistributionEMSL();
	
	return TestStepResult();
	} // CSmsSendRecvObjectDistributionEMS::doTestStepL


TVerdict CSmsSendRecvManyEMSObjects::doTestStepL()
	{
	SendRecvManyEMSObjectsL();
	
	return TestStepResult();
	} // CSmsSendRecvManyEMSObjects::doTestStepL


TVerdict CSmsSendRecvEMSOn8BitPort::doTestStepL()
	{
	SendRecvEMSOn8BitPortL();
	
	return TestStepResult();
	} // CSmsSendRecvEMSOn8BitPort::doTestStepL


TVerdict CSmsSendRecvEMSOn16BitPort::doTestStepL()
	{
	SendRecvEMSOn16BitPortL();
	
	return TestStepResult();
	} // CSmsSendRecvEMSOn16BitPort::doTestStepL


TVerdict CSmsTestFormattingSegmentation::doTestStepL()
	{
	TestFormattingSegmentationL();
	
	return TestStepResult();
	} // CSmsTestFormattingSegmentation::doTestStepL


TVerdict CSmsTestNumberOfMessages::doTestStepL()
	{
	TestNumberOfMessagesL();
	
	return TestStepResult();
	} // CSmsTestNumberOfMessages::doTestStepL


TVerdict CSmsTestCreateSocket::doTestStepL()
	{
	TestCreateSocketL();
	
	return TestStepResult();
	} // CSmsTestCreateSocket::doTestStepL


TVerdict CSmsPublicMethods::doTestStepL()
	{
	PublicMethodsL();
	
	return TestStepResult();
	} // CSmsPublicMethods::doTestStepL


TVerdict CSmsSendAndReceive7bitMsgs::doTestStepL()
	{
	SendAndReceive7bitMsgsL();
	
	return TestStepResult();
	} // CSmsSendAndReceive7bitMsgs::doTestStepL


TVerdict CSmsSendAndReceive8bitMsgs::doTestStepL()
	{
	SendAndReceive8bitMsgsL();
	
	return TestStepResult();
	} // CSmsSendAndReceive8bitMsgs::doTestStepL


TVerdict CSmsSendAndReceive16bitMsgs::doTestStepL()
	{
	SendAndReceive16bitMsgsL();
	
	return TestStepResult();
	} // CSmsSendAndReceive16bitMsgs::doTestStepL


TVerdict CSmsSendAndReceiveConcatenatedMsgs::doTestStepL()
	{
	SendAndReceiveConcatenatedMsgsL();
	
	return TestStepResult();
	} // CSmsSendAndReceiveConcatenatedMsgs::doTestStepL


TVerdict CSmsTestStringMatchingIncomingMsg::doTestStepL()
	{
	TestStringMatchingIncomingMsgL();
	
	return TestStepResult();
	} // CSmsTestStringMatchingIncomingMsg::doTestStepL


TVerdict CSmsSendAndReceiveDifferentIEIs::doTestStepL()
	{
	SendAndReceiveDifferentIEIsL();
	
	return TestStepResult();
	} // CSmsSendAndReceiveDifferentIEIs::doTestStepL


TVerdict CSmsSendAndReceiveBinaryData::doTestStepL()
	{
	SendAndReceiveBinaryDataL();
	
	return TestStepResult();
	} // CSmsSendAndReceiveBinaryData::doTestStepL


TVerdict CSmsTestOOM::doTestStepL()
	{
	TestOOML();
	
	return TestStepResult();
	} // CSmsTestOOM::doTestStepL


TVerdict CSmsTestSendAndReceive1byteMsg::doTestStepL()
	{
	TestSendAndReceive1byteMsgL();
	
	return TestStepResult();
	} // CSmsTestSendAndReceive1byteMsg::doTestStepL


TVerdict CSmsTestSmsWapEmailNotification::doTestStepL()
	{
	TestSmsWapEmailNotificationL();
	
	return TestStepResult();
	} // CSmsTestSmsWapEmailNotification::doTestStepL


TVerdict CSmsTestSendAndReceive159bytesMsg::doTestStepL()
	{
	TestSendAndReceive159bytesMsgL();
	
	return TestStepResult();
	} // CSmsTestSendAndReceive159bytesMsg::doTestStepL


TVerdict CSmsTestSocketConnectTwice::doTestStepL()
	{
	TestSocketConnectTwiceL();
	
	return TestStepResult();
	} // CSmsTestSocketConnectTwice::doTestStepL


TVerdict CSmsTestSocketBinding::doTestStepL()
	{
	TestSocketBindingL();
	
	return TestStepResult();
	} // CSmsTestSocketBinding::doTestStepL


TVerdict CSmsTestEnumerateAndReceiveMessages::doTestStepL()
	{
	TestEnumerateAndReceiveMessagesL();
	
	return TestStepResult();
	} // CSmsTestEnumerateAndReceiveMessages::doTestStepL


TVerdict CSmsTestEnumerateAndDeleteMessages::doTestStepL()
	{
	TestEnumerateAndDeleteMessagesL();
	
	return TestStepResult();
	} // CSmsTestEnumerateAndDeleteMessages::doTestStepL


TVerdict CSmsEnumerateAndWriteAndDeleteMessages::doTestStepL()
	{
	EnumerateAndWriteAndDeleteMessagesL();
	
	return TestStepResult();
	} // CSmsEnumerateAndWriteAndDeleteMessages::doTestStepL


TVerdict CSmsPDUSlotStoreWithEnumeration::doTestStepL()
	{
	PDUSlotStoreWithEnumerationL();
	
	return TestStepResult();
	} // CSmsPDUSlotStoreWithEnumeration::doTestStepL


TVerdict CSmsDeleteMessagesWithoutEnumeration::doTestStepL()
	{
	DeleteMessagesWithoutEnumerationL();
	
	return TestStepResult();
	} // CSmsDeleteMessagesWithoutEnumeration::doTestStepL


TVerdict CSmsSmartMessaging::doTestStepL()
	{
	SmartMessagingL();
	
	return TestStepResult();
	} // CSmsSmartMessaging::doTestStepL


TVerdict CSmsRetrieveAndStoreSmsParams::doTestStepL()
	{
	RetrieveAndStoreSmsParamsL();
	
	return TestStepResult();
	} // CSmsRetrieveAndStoreSmsParams::doTestStepL


TVerdict CSmsTestRetrieveAndStoreSmsParamsOOM::doTestStepL()
	{
	TestRetrieveAndStoreSmsParamsOOML();
	
	return TestStepResult();
	} // CSmsTestRetrieveAndStoreSmsParamsOOM::doTestStepL


TVerdict CSmsSmsStoreMsgOfDifferentClasses::doTestStepL()
	{
	SmsStoreMsgOfDifferentClassesL();
	
	return TestStepResult();
	} // CSmsSmsStoreMsgOfDifferentClasses::doTestStepL


TVerdict CSmsDuplicatePdusOnStore::doTestStepL()
	{
	DuplicatePdusOnStoreL();
	
	return TestStepResult();
	} // CSmsDuplicatePdusOnStore::doTestStepL


TVerdict CSmsTestMaxNumberOfPdusOnStore::doTestStepL()
	{
	TestMaxNumberOfPdusOnStoreL();

	return TestStepResult();
	} // CSmsTestMaxNumberOfPdusOnStore::doTestStepL


TVerdict CSmsDeletingMessageInEmptyStore::doTestStepL()
	{
	DeletingMessageInEmptyStoreL();
	
	return TestStepResult();
	} // CSmsDeletingMessageInEmptyStore::doTestStepL


TVerdict CSmsConcatenatedMessages::doTestStepL()
	{
	ConcatenatedMessagesL();
	
	return TestStepResult();
	} // CSmsConcatenatedMessages::doTestStepL


TVerdict CSmsOutOfMemoryWrite::doTestStepL()
	{
	OutOfMemoryWriteL();
	
	return TestStepResult();
	} // CSmsOutOfMemoryWrite::doTestStepL


TVerdict CSmsTestOutOfMemoryRead::doTestStepL()
	{
	TestOutOfMemoryReadL();
	
	return TestStepResult();
	} // CSmsTestOutOfMemoryRead::doTestStepL


TVerdict CSmsOutOfMemoryDelete::doTestStepL()
	{
	OutOfMemoryDeleteL();
	
	return TestStepResult();
	} // CSmsOutOfMemoryDelete::doTestStepL


TVerdict CSmsSendSMS2Port0000::doTestStepL()
	{
	SendSMS2Port0000L();
	
	return TestStepResult();
	} // CSmsSendSMS2Port0000::doTestStepL


TVerdict CSmsTestReceiveLongClass2::doTestStepL()
	{
	TestReceiveLongClass2L();
	
	return TestStepResult();
	} // CSmsTestReceiveLongClass2::doTestStepL


TVerdict CSmsTestSendAndReceiveConcatenated1::doTestStepL()
	{
	TestSendAndReceiveConcatenated1L();
	
	return TestStepResult();
	} // CSmsTestSendAndReceiveConcatenated1::doTestStepL


TVerdict CSmsTestSendAndReceiveConcatenated2::doTestStepL()
	{
	TestSendAndReceiveConcatenated2L();
	
	return TestStepResult();
	} // CSmsTestSendAndReceiveConcatenated2::doTestStepL


TVerdict CSmsTestBindingToApplicationPort::doTestStepL()
	{
	TestBindingToApplicationPortL();
	
	return TestStepResult();
	} // CSmsTestBindingToApplicationPort::doTestStepL


TVerdict CSmsTestSendAndReceiveAndDeleteClass2Msgs::doTestStepL()
	{
	TestSendAndReceiveAndDeleteClass2MsgsL();
	
	return TestStepResult();
	} // CSmsTestSendAndReceiveAndDeleteClass2Msgs::doTestStepL


TVerdict CSmsPublishandSubscribeTest::doTestStepL()
	{
	PublishandSubscribeTestL();
	
	return TestStepResult();
	} // CSmsPublishandSubscribeTest::doTestStepL


TVerdict CSmsTestSendingReceivingViaAppPortPublishAndSubscribe::doTestStepL()
	{
	TestSendingReceivingViaAppPortPublishAndSubscribeL();
	
	return TestStepResult();
	} // CSmsTestSendingReceivingViaAppPortPublishAndSubscribe::doTestStepL

