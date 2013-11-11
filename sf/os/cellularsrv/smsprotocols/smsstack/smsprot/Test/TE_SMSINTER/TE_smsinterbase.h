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

#ifndef _TE_SMSINTERBASE_H_
#define _TE_SMSINTERBASE_H_

#include <e32std.h>
#include <e32des8.h>
#include <fbs.h>

#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

#include <smsuaddr.h>
#include <gsmumsg.h>

#include "smspdudbitems.h"
#include "smsstacktestutilities.h"


const TInt KActiveSenderPriority = CActive::EPriorityStandard;
const TInt KActiveReceiverPriority = CActive::EPriorityStandard;
const TInt KStopIfKeyPressedPriority = CActive::EPriorityUserInput;

const TInt  KMaxNumberOfCharsFor7BitSms = 160;
const TInt  KMaxNumberOfCharsFor8BitSms = 140;
const TUint KSocketMessageSlots = 16;   // Override the default value of 8


class TTestCase
	{
public:
	TTestCase();
	TTestCase(const TDesC& aMsg, const TDesC& aMatch);
	TTestCase(const TDesC& aMsg, CSmsInformationElement::TSmsInformationElementIdentifier aId);
	TTestCase(const TDesC& aMsg, TSmsFirstOctet::TSmsStatusReportRequest aSRR);
	TTestCase(const TDesC& aMsg, TSmsDataCodingScheme::TSmsIndicationType aIndicationType,
	TSmsDataCodingScheme::TSmsDCSBits7To4 aBits7To4, TSmsDataCodingScheme::TSmsIndicationState iIndicationState);
	TTestCase(const TDesC& aMsg);
	const TTestCase& operator=(const TTestCase& aTest);
	void SetSmsClass(TSmsDataCodingScheme::TSmsClass aClass);
	void SetValidityPeriod(const TTimeIntervalMinutes& aTimeInterval);
	void SetRecvForever();
private:
	void CommonInit();
public:
	TSmsAddrFamily iMatchType;
	TPtrC iMsg;
	TPtrC iMatch;
	CSmsInformationElement::TSmsInformationElementIdentifier iIdentifierMatch;

	TBool iTestSmsClass;
	TSmsDataCodingScheme::TSmsClass iSmsClass;
	TBool iTestValidityPeriod;
	TTimeIntervalMinutes iValidityPeriod;
	TBool iTestForever;
	TBool iTestIndicators;
	TSmsDataCodingScheme::TSmsIndicationType iIndicationType;	// for ESmsAddrMessageIndication
	TSmsDataCodingScheme::TSmsDCSBits7To4 iBits7To4;			// for ESmsAddrMessageIndication
	TSmsDataCodingScheme::TSmsIndicationState iIndicationState; // for ESmsAddrMessageIndication
	};


class CActiveReceiver : public CActive
	{
public:
	enum { EPriority = KActiveReceiverPriority };
public:
	static CActiveReceiver* NewL(RSocketServ& aSocketServer,
                                 RFs& aFs);
	virtual void AddTestL(const TTestCase& aTest);
	virtual void StartTestsL();
	virtual void StopIfKeyPressedL();
protected:
	CActiveReceiver(RSocketServ& aSocketServer, RFs& aFs);
	RSocketServ& iSocketServer;
	RFs& iFs;
	};

class CActiveSender : public CActive
	{
public:
	enum { EPriority = KActiveSenderPriority };
public:
	static CActiveSender* NewL(RSocketServ& aSocketServer,
                               RFs& aFs,
                               const TDesC& aTelephoneNumber,
                               const TDesC& aServiceCenterNumber);
	virtual void StartSendingL();
	virtual void StopIfKeyPressedL();
protected:
	CActiveSender(RSocketServ& aSocketServer, RFs& aFs);
	RSocketServ& iSocketServer;
	RFs& iFs;
	};


class CSmsInterTestStep : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	void ReadPduL( TBuf8<64> aSectionName, RPointerArray<CSmsPduDbMessage>& aArray );
	void TestCreateSocketL();
	void TestPublicMethodsL( const RPointerArray<CSmsPduDbMessage>& aArray );
	CSmsMessage* CreateAndSetMessageWithOneIEL( const RPointerArray<CSmsPduDbMessage>& aArray, TInt aArrayIndex );
	CSmsMessage* CreateAndSetMessageWithOneIEOnPhoneL( const RPointerArray<CSmsPduDbMessage>& aArray, TInt aArrayIndex );
	HBufC8* CreatePictureBufferL(const CFbsBitmap& aBitmap);
	TBool CompareEmsElementL(const CEmsInformationElement& aOriginalElement,RPointerArray<const CEmsInformationElement>& aOtherElementsArray);
	void CompareEmsElementsL(CSmsMessage& aOriginalMessage,CSmsMessage& aOtherMessage);
	void SendRecvTextFormated7BitEMSL();
	void SendRecvTextFormated8BitEMSL();
	void SendRecvTextFormated16BitEMSL();
	void EnumerateReadWriteAndDeleteEMSOnSIML( const RPointerArray<CSmsPduDbMessage>& aArray );
	void SendRecvPreDefSoundEMSL();
	void SendRecvUserDefSoundEMSL();
	void EnumerateReadWriteAndDeleteEMSOnPhoneL( const RPointerArray<CSmsPduDbMessage>& aArray );
	void SendRecvPicturesEMSL();
	void SendRecvPreDefAnimationL();
	void SendRecvUserDefAnimationL();
	void SendRecvUserPromptEMSL();
	void SendRecvObjectDistributionEMSL();
	void SendRecvManyEMSObjectsL();
	void SendRecvEMSOn8BitPortL();
	void SendRecvEMSOn16BitPortL();
	void TestFormattingSegmentationL();
	void TestNumberOfMessagesL();
	TBool DoSingleTestCaseL( const TDesC8& aSection, TInt aCount );
	void EnumerateReadWriteAndDeleteEMSOnSIML();
	void EnumerateReadWriteAndDeleteEMSOnPhoneL();
	CSmsMessage* CreateAndSetMessageL( const RPointerArray<CSmsPduDbMessage>& aArray, TInt aArrayIndex );
	void TestSendAndReceive7bitMsgsL( TPtrC8 aPdu,
											  const RPointerArray<CSmsPduDbMessage>& aArray,
											  const CArrayFixFlat<TInt>* aCharArray,
											  const CArrayFixFlat<TInt>* aMessageLengthArray );
	void TestSendAndReceive8bitMsgsL( const RPointerArray<CSmsPduDbMessage>& aArray,
											  const CArrayFixFlat<TInt>* aMessageLengthArray );
	void TestSendAndReceive16bitMsgsL( const RPointerArray<CSmsPduDbMessage>& aArray,
											   const CArrayFixFlat<TInt>* aMessageLengthArray );
	void TestSendAndReceiveConcatenatedMsgsL( const RPointerArray<CSmsPduDbMessage>& aArray,
													  const CArrayFixFlat<TInt>* aAlphabetArray );
	void TestStringMatchingIncomingMsgL();
	void TestSendAndReceiveDifferentIEIsL( const RPointerArray<CSmsPduDbMessage>& aArray,
												   const CArrayFixFlat<TInt>* aAlphabetArray,
												   const CArrayFixFlat<TInt>* aInformationElementArray,
												   const CArrayFixFlat<TInt>* aIeiDataArray );
	void TestSendAndReceiveBinaryDataL( const CArrayFixFlat<TInt>* aMessageLengthArray );
	void TestOOML();
	void TestSmsWapEmailNotificationL();
	void TestSendAndReceive159bytesMsgL();
	void TestSendAndReceive1byteMsgL();
	void TestSocketConnectTwiceL();
	void TestSocketBindingL();
	void TestEnumerateAndReceiveMessagesL();
	void TestEnumerateAndDeleteMessagesL();
	void TestEnumerateAndWriteAndDeleteMessagesL( const RPointerArray<CSmsPduDbMessage>& aArray );
	TInt TestParametersL(CMobilePhoneSmspList& aLeft,CMobilePhoneSmspList& aRight);
	TInt TestPhoneNumbers(const RMobilePhone::TMobileAddress& aLeftNumber,const RMobilePhone::TMobileAddress& aRightNumber);
	void TestRetrieveAndStoreSmsParamsL(
								RMobileSmsMessaging::TMobileSmspStoreValidParams aSmspEntryValidParams,
								RMobilePhone::TMobileTON aSmspEntryScTypeOfNumber,
								RMobilePhone::TMobileNPI aSmspEntryScNumberPlan,
								const TDesC8& aSmspEntryScTelNumber );
	void TestPDUSlotStoreWithEnumerationL( const RPointerArray<CSmsPduDbMessage>& aArray );
	void TestDeleteMessagesWithoutEnumerationL( const RPointerArray<CSmsPduDbMessage>& aArray );
	void TestRetrieveAndStoreSmsParamsOOML();
	void TestSmsStoreMsgOfDifferentClassesL( const RPointerArray<CSmsPduDbMessage>& aArray,
													 const CArrayFixFlat<TInt>* aSmsStorageArray,
													 const CArrayFixFlat<TInt>* aSmsClassArray );
	void TestDuplicatePdusOnStoreL( const RPointerArray<CSmsPduDbMessage>& aArray );
	void TestMaxNumberOfPdusOnStoreL();
	void TestDeletingMessageInEmptyStoreL( RPointerArray<CSmsPduDbMessage> aArray );
	void TestConcatenatedMessagesL( const RPointerArray<CSmsPduDbMessage>& aArray );
	void TestOutOfMemoryWriteL( const RPointerArray<CSmsPduDbMessage>& aArray );
	void TestOutOfMemoryReadL();
	void TestOutOfMemoryDeleteL( const RPointerArray<CSmsPduDbMessage>& aArray );
	void TestSmartMessagingL( const RPointerArray<CSmsPduDbMessage>& aArray,
									  const CArrayFixFlat<TInt>* aIeiDataArray,
									  const CArrayFixFlat<TInt>* aInformationElementArray,
									  const CArrayFixFlat<TInt>* aAlphabetArray );
	void TestSendSMS2Port0000L( const RPointerArray<CSmsPduDbMessage>& aArray );
	void TestReceiveLongClass2L();
	void TestSendAndReceiveConcatenated1L();
	void TestSendAndReceiveConcatenated2L();
	void TestBindingToApplicationPortL();
	void TestSendAndReceiveAndDeleteClass2MsgsL();
	void PublishandSubscribeTestL();
	void TestSendingReceivingViaAppPortPublishAndSubscribeL();
	void EnumerateAndWriteAndDeleteMessagesL();
	void PDUSlotStoreWithEnumerationL();
	void DeleteMessagesWithoutEnumerationL();
	void SmartMessagingL();
	void RetrieveAndStoreSmsParamsL();
	void DeletingMessageInEmptyStoreL();
	void SmsStoreMsgOfDifferentClassesL();
	void DuplicatePdusOnStoreL();
	void ConcatenatedMessagesL();
	void OutOfMemoryWriteL();
	void OutOfMemoryDeleteL();
	void PublicMethodsL();
	void SendAndReceive7bitMsgsL();
	void SendAndReceive8bitMsgsL();
	void SendAndReceive16bitMsgsL();
	void SendAndReceiveConcatenatedMsgsL();
	void SendAndReceiveDifferentIEIsL();
	void SendAndReceiveBinaryDataL();
	void SendSMS2Port0000L();

	void Connect2EsockLC(RSocketServ& aSocketServer);
	
	CSmsMessage* CreateSmsMessageL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aType = CSmsPDU::ESmsSubmit);
	CSmsMessage* CreateSmsWithStatusReportReqL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet);
	void OpenSmsSocketMatchTextLC(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddrFamily aFamily, TDesC8* aTextMatch);
	void SendSimpleSmsL(RSocket& aSocket);
	void SendSmsL(const CSmsMessage* aSms, RSocket& aSocket);
	void SendSmsErrorL(CSmsMessage* aSms, RSocket& aSocket);
	void SendSmsDontCheckReturnValueL(CSmsMessage* aSms, RSocket& aSocket);
	void SendCommandSmsL(CSmsMessage* aSms, RSocket& aSocket);
	void SendSmsCancelL(CSmsMessage* aSms, RSocket& aSocket1, RSocket& aSocket2);
	void SendAndRecvAndCompareMessageIEIL(const TDesC& aUserData,
												  const TDesC8& aIEIOctets,
												  RSocket& aSocket,
												  TSmsDataCodingScheme::TSmsAlphabet aSmsAlphabet,
												  CSmsInformationElement::TSmsInformationElementIdentifier aIEI);
	void WaitForRecvL(RSocket& aSocket);
	TBool TimedWaitForRecvL(RSocket& aSocket, TUint aDelay);
	CSmsMessage* RecvSmsL(RSocket& aSocket, TInt aIoctl = KIoctlReadMessageSucceeded);
	CSmsMessage* RecvSmsFailedL(RSocket& aSocket);
	void RecvStatusReportL(TSmsServiceCenterAddress& aRecipientNumber, RSocket& aSocket);
	void ReadSmsStoreL(RSocket& aSocket, RPointerArray<CSmsMessage>& aMessages);
	TInt MakeReadSmsStoreRequestL(RSocket& aSocket);
	void WriteSmsToSimL(CSmsMessage& aSms, RSocket& aSocket, TDes8* aDesc=NULL);
	TInt CancelWriteSmsToSimL(CSmsMessage& aSms, RSocket& aSocket, TInt aDelay);
	void WriteMessagesUntilStoreComesFullL(RPointerArray<CSmsMessage>& aMessages, const CSmsMessage::TMobileSmsStorage& aStorage, RSocket& aSocket);
	void WriteSmsLeaveIfErrorL(const CSmsMessage& aSms, RSocket& aSocket);
	TInt DeleteSmsL(const CSmsMessage& aSms, RSocket& aSocket);
	void DeleteSmsLeaveIfErrorL(const CSmsMessage& aSms, RSocket& aSocket);
	void TestSmsContentsL(CSmsMessage* aSms, const TDesC& aDes);
	void TestSmsContentsL(CSmsMessage* aSms, const CSmsBufferBase& aSmsBuffer );
	void TestSmsListsL(RPointerArray<CSmsMessage>& aLeft,RPointerArray<CSmsMessage>& aRight);
	void DoSendSmsL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, RSocket& aSocket);
	void SendSms7BitL(const TDesC& aDes, RSocket& aSocket);
	void SendSms8BitL(const TDesC& aDes, RSocket& aSocket);
	void SendSms16BitL(const TDesC& aDes, RSocket& aSocket);
	void DoSendAndRecvSmsL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, RSocket& aSocket);
	void SendAndRecvSms7BitL(const TDesC& aDes, RSocket& aSocket);
	void SendAndRecvSms8BitL(const TDesC& aDes, RSocket& aSocket);
	void SendAndRecvSmsBinaryL(const TDesC& aDes, RSocket& aSocket);
	void SendAndRecvSms16BitL(const TDesC16& aDes, RSocket& aSocket);
	void FillDes(TDes& aDes,TInt aLength);
	void SendAndRecvSms7BitL(TInt aLength, RSocket& aSocket);
	void BinaryFillDes(TDes& aDes, TInt aLength);
	void SendAndRecvSms8BitL(TInt aLength, RSocket& aSocket);
	void SendAndRecvSmsBinaryL(TInt aLength, RSocket& aSocket);
	void SendAndRecvSms16BitL(TInt aLength, RSocket& aSocket);
	void TestMessageContentsL(CSmsMessage* aSms, const TTestCase& aTestCase);
	void SetIndicatorL(const TTestCase& aTestCase, CSmsMessage* aSms);
	void SendAndRecvTestMessageL(const TTestCase& aTestCase, RSocket& aSocket);
	void SendTestMessageL(const TTestCase& aTestCase, RSocket& aSocket);
	TInt WriteReadAndDeleteMessageL(const TTestCase& aTestCase, const CSmsMessage::TMobileSmsStorage& aSmsStorage, RSocket& aSocket);
	void SendMessagesUntilKeyPressL(RSocketServ& aSocketServer);
	void PrintMessageL(const CSmsMessage* aSms);

	
protected:
	TSmsServiceCenterAddress iTelephoneNumber ;
	TSmsServiceCenterAddress iServiceCenterNumber ;
	CSmsStackTestUtils* iSmsStackTestUtils;
	RFs iFs;
	
private:
	CActiveScheduler*  iScheduler;
	};

#endif // _TE_SMSINTERBASE_H_
