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

#ifndef __TE_SMSINTER_H_
#define __TE_SMSINTER_H_

#include "TE_smsinterbase.h"


class CSms : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvTextFormated7BitEMS : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvTextFormated8BitEMS : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvTextFormated16BitEMS : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsEnumerateReadWriteAndDeleteEMSOnSIM : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvPreDefSoundEMS : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvUserDefSoundEMS : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsEnumerateReadWriteAndDeleteEMSOnPhone : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvPicturesEMS : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvPreDefAnimation : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvUserDefAnimation : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvUserPromptEMS : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvObjectDistributionEMS : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvManyEMSObjects : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvEMSOn8BitPort : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendRecvEMSOn16BitPort : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestFormattingSegmentation : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestNumberOfMessages : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestCreateSocket : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsPublicMethods : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendAndReceive7bitMsgs : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendAndReceive8bitMsgs : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendAndReceive16bitMsgs : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendAndReceiveConcatenatedMsgs : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestStringMatchingIncomingMsg : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendAndReceiveDifferentIEIs : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendAndReceiveBinaryData : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestOOM : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestSendAndReceive1byteMsg : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestSmsWapEmailNotification : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestSendAndReceive159bytesMsg : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestSocketConnectTwice : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestSocketBinding : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestEnumerateAndReceiveMessages : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestEnumerateAndDeleteMessages : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsEnumerateAndWriteAndDeleteMessages : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsPDUSlotStoreWithEnumeration : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsDeleteMessagesWithoutEnumeration : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSmartMessaging : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsRetrieveAndStoreSmsParams : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestRetrieveAndStoreSmsParamsOOM : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSmsStoreMsgOfDifferentClasses : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsDuplicatePdusOnStore : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestMaxNumberOfPdusOnStore : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsDeletingMessageInEmptyStore : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsConcatenatedMessages : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsOutOfMemoryWrite : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestOutOfMemoryRead : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsOutOfMemoryDelete : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsSendSMS2Port0000 : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestReceiveLongClass2 : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestSendAndReceiveConcatenated1 : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestSendAndReceiveConcatenated2 : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestBindingToApplicationPort : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestSendAndReceiveAndDeleteClass2Msgs : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsPublishandSubscribeTest : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsTestSendingReceivingViaAppPortPublishAndSubscribe : public CSmsInterTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


#endif // __TE_SMSINTER_H_
