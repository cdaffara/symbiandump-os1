/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Utility functions of smsprot
* contains the class definition of CSmsStackTestUtils and 3 active objects for receiving and
* enumerating sms messages.
* 
*
*/

/**
 @file
*/

#ifndef SMSSTACKTESTUTILITIES_H
#define SMSSTACKTESTUTILITIES_H

#include <e32test.h>
#include <es_sock.h>
#include <mmlist.h>
#include <gsmupdu.h>
#include <smsuact.h>
#include <smsuaddr.H>
#include <test/testexecuteserverbase.h>
#include <logwrapconst.h>
#include <logsmspdudata.h>
#include <commsdat_partner.h>

class CTestStep;
class RFs;
class CSmsMessage;
class CLogEvent;
class CSmsPduDatabase;
class CTestConfig;
class CTestConfigSection;

const TInt KActiveObjectPriority = CActive::EPriorityStandard;
_LIT(KGmsSmsConfigFileDir,"sms");
_LIT(KSimTsyConfigFileName,"c:\\config.txt");


class CSmsStackTestUtils : public CBase
	{
	public:
		//Factory
		IMPORT_C static CSmsStackTestUtils* NewL(CTestStep* aTestStep, RFs& aFs);
		IMPORT_C ~CSmsStackTestUtils();

		///SmsProt.Prt Test Utils, more below
		IMPORT_C void GetLogEventL(CLogEvent& aLogEvent, TInt aLogServerId);
		IMPORT_C TLogId AddLogEventL(CSmsMessage& aSmsMessage,TLogSmsPduData& aSmsPDUData);
 		IMPORT_C void DisableLogging();
 		IMPORT_C void EnableLogging();		
		IMPORT_C void SendSmsAndChangeBearerL(CSmsMessage* aSms, RSocket& aSocket, RMobileSmsMessaging::TMobileSmsBearer aBearer);

		///GSMU.dll Test Utils
		IMPORT_C CSmsMessage* CreateSmsMessageLC(CSmsPDU::TSmsPDUType aType, const TDesC& aDestination, const TDesC& aMessage);
		CArrayFixFlat<TGsmSms>* EncodeMessageLC(CSmsMessage& aMessage) const;

		///Config File Utils
		IMPORT_C void CopyConfigFileL(const TDesC& aFromSmsConfigFile, CSmsPduDatabase* aPduDb=NULL);
		IMPORT_C void AddSmsRxL(CTestConfig& aConfigFile, TInt aTestNumber, CSmsMessage& aMessage, const TDesC& aServiceCenter) const;
		IMPORT_C void AddSmsTxL(CTestConfig& aConfigFile, TInt aTestNumber, CSmsMessage& aMessage, const TDesC& aServiceCenter, TInt aMsgRef = 0, CSmsMessage* aSubmitReport = NULL, TInt aExpectedError = KErrNone) const;
		IMPORT_C CTestConfigSection& GetSectionL(CTestConfig& aConfig, TInt aTestNumber) const;

		///PDU utils
		IMPORT_C HBufC8* ConvertToHexLC(const TDesC8& aDes) const;
		IMPORT_C TSmsFirstOctet FirstDeliverOctet(TBool aMMS, TBool aRP, TBool aUDHI, TBool aSRI);
		IMPORT_C void DeliverWithoutUserDataL(TGsmSms& aPdu, TSmsFirstOctet aFO, const TGsmSmsTelNumber& aOA, TSmsProtocolIdentifier aPID, TSmsDataCodingScheme aDCS, const TTime& aSCTS, TInt aUTCOffset);
		IMPORT_C void AppendUserDataL(TGsmSms& aPdu, const CSmsUserData& aUD, TInt aUDL = KErrNotFound);
		IMPORT_C void PrintAndTestDecodeL(const TGsmSms& aPdu, TSmsDataCodingScheme aExpectedDCS, TSmsProtocolIdentifier aExpectedPID, const CSmsUserData& aExpectedUD, TBool aIsMobileTerminated, TInt aExpectedError);
		IMPORT_C void CompareUDL(const CSmsPDU& aPDU, const CSmsUserData& aUD);
		IMPORT_C void ComparePIDL(const CSmsPDU& aPDU, TSmsProtocolIdentifier aPID);
		IMPORT_C void CompareDCSL(const CSmsPDU& aPdu, TSmsDataCodingScheme aDCS);

		///Socket utils
		IMPORT_C void OpenSmsSocketLC(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddr& aSmsAddr);
		IMPORT_C void OpenSmsSocketLC(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddrFamily aFamily);
		IMPORT_C void OpenSmsSocketL(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddr& aSmsAddr);
		IMPORT_C void OpenSmsSocketL(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddrFamily aFamily);

		///SMS Parameter utils
		IMPORT_C TInt StoreParamsL(CMobilePhoneSmspList& aSmspList, RSocket& aSocket, TBool aCancelAfterRequest);
		IMPORT_C TInt RetrieveParamsL(CMobilePhoneSmspList& aSmspList, RSocket& aSocket, TBool aCancelAfterRequest);
		IMPORT_C void MakeParametersReadRequestL(RSocket& aSocket);
		IMPORT_C void StoreParamsLeaveIfErrorL(CMobilePhoneSmspList& aSmspList,RSocket& aSocket);
		IMPORT_C TInt TestParameters(CMobilePhoneSmspList& aLeft,CMobilePhoneSmspList& aRight);
		IMPORT_C TInt TestPhoneNumbers(const RMobilePhone::TMobileAddress& aLeftNumber,const RMobilePhone::TMobileAddress& aRightNumber);
		IMPORT_C void PrintSmspEntry(RMobileSmsMessaging::TMobileSmspEntryV1& aEntryToTsy);
        IMPORT_C void PrintSmspList(CMobilePhoneSmspList& aSmspList);

		//Unit Tests
		//TSmsServiceCenterTimeStamp
		IMPORT_C void CreateTSmsServiceCenterTimeStampL();
		IMPORT_C void DecodeTSmsServiceCenterTimeStampL();
		IMPORT_C void EncodeTSmsServiceCenterTimeStampL();

		//TSmsValidityPerdiod
		IMPORT_C void CreateTSmsValidityPeriodL();
		IMPORT_C void DecodeTSmsValidityPeriodL();
		IMPORT_C void EncodeTSmsValidityPeriodL();
		
		IMPORT_C void ChangeBearerL(RMobileSmsMessaging::TMobileSmsBearer aBearer);
		IMPORT_C void GetBearerL(RMobileSmsMessaging::TMobileSmsBearer& aBearer);
		
	private:
		CSmsStackTestUtils(CTestStep* aTestStep, RFs& aFs);
		void ConstructL();
	     
	public:
		CTestStep* iTestStep;
		RFs& iFs;
		CCnvCharacterSetConverter* iCharConv;
	};

class CTestGetSmsList : public CSmsuActiveBase
/**
 *  This active object is used to enumerate the messages from
 *  the sms message store on a socket
 */
	{
public:
	IMPORT_C static CTestGetSmsList* NewL(TInt aPriority, RSocketServ& aSocketServer, CSmsStackTestUtils& iTestUtils);
	void Start();//not used
	IMPORT_C void GetSmsList(TRequestStatus& aStatus);
	TInt iRemainingMessages;
	TPckgBuf<TUint> iMessageCount;
protected:
	IMPORT_C CTestGetSmsList(TInt aPriority, RSocketServ& aSocketServer, CSmsStackTestUtils& iTestUtils);
	void ConstructL();
	void DoRunL();
	void DoCancel();
protected:
	RSocketServ& iSocketServer;
	RSocket iSocket;
	enum TTestGetSmsListState
		{
		EEnuming,
		EReading
		} iState;

	CSmsStackTestUtils& iTestUtils;
	};

class CTestReceiveMessages : public CSmsuActiveBase
/**
 *  This active object is used receive sms messages
 *  from a socket
 *  
 */
	{
public:
	IMPORT_C static CTestReceiveMessages* NewL(TInt aPriority, RSocketServ& aSocketServer, CSmsStackTestUtils& aTestUtils);
	void Start();
	IMPORT_C void Receive(TRequestStatus& aStatus, TInt aNumberOfMessages);
	IMPORT_C void Receive();
	TInt NumberReceived();
protected:
	IMPORT_C CTestReceiveMessages(TInt aPriority, RSocketServ& aSocketServer, CSmsStackTestUtils& aTestUtils);
	void ConstructL();
	void DoRunL();
	void DoCancel();
protected:
	RSocketServ& iSocketServer;
	RSocket iSocket;
	TPckgBuf<TUint> iMessageBuf;
	TInt iNumberOfReceivedMessages;
	TInt iNumberOfMessagesToReceive;
	enum TTestReceiveMessagesState
		{
		EReceiving,
		EReceived
		} iState;
	CSmsStackTestUtils& iTestUtils;
	};

class CTestReceiveEnumerateMessages : public CSmsuActiveBase
/**
 *  This class defines an active object use to control 2 active objects
 *  CTestReceiveMessages and CTestGetSmsList
 */
	{
public:
	IMPORT_C static CTestReceiveEnumerateMessages* NewL(TInt aPriority, RSocketServ& aSocketServer, CSmsStackTestUtils& aTestUtils);
	IMPORT_C void Start(TInt aEnumExpected, TInt aRecvExpected, TInt aRecvInitial);
	IMPORT_C ~CTestReceiveEnumerateMessages();
protected:
	IMPORT_C CTestReceiveEnumerateMessages(TInt aPriority, RSocketServ& aSocketServer, CSmsStackTestUtils& aTestUtils);
//	TInt RunError(TInt aError);
	void ConstructL();
	void DoRunL();
	void DoCancel();
	void Complete(TInt);
protected:
	CTestReceiveMessages* iReceiver;
	CTestGetSmsList* iEnumerator;
	RSocketServ& iSocketServer;
	TInt iEnumExpected, iRecvExpected , iRecvInitial;
	enum TTestReceiveEnumerateMessagesState
		{
		ERecvInitial,
		EEnumerating,
		ESecondReceiving,
		ETryingToReceiveOneMore
		} iState;
	CSmsStackTestUtils& iTestUtils;
	};

#endif  // SMSSTACKTESTUTILITIES_H
