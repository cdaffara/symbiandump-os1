// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @internalAll
*/


#ifndef __SIMSMSMESS_H_
#define __SIMSMSMESS_H_

#include <etelmm.h>
#include <et_phone.h>

#include "CSimPhone.h"
#include "csimtimer.h"

#include "CSimSmsStore.h"
#include "utils.h"

const TUint KMaxSmsPduLength=168;
typedef TBuf8<KMaxSmsPduLength> TSmsPdu;

const TUint KMaxSmsScaLength=100;
typedef TBuf8<KMaxSmsScaLength> TSmsSca;

const TUint KMaxSmsDestinationLength=50;
typedef TBuf8<KMaxSmsDestinationLength> TSmsDestination;

const TUint KMaxSmsExpectedErrorLength=10;
typedef TBuf8<KMaxSmsExpectedErrorLength> TSmsExpectedError;


struct TSmsTxParametersGsm
{
	TInt iFlags;
	RMobileSmsMessaging::TMobileSmsGsmTpdu iPdu;
	TSmsSca iSca;
	TInt iRef;
	RMobileSmsMessaging::TMobileSmsGsmTpdu iSubmitReport;
	TInt iExpectedError;	
};


struct TSmsRxParametersGsm
{
	RMobileSmsMessaging::TMobileSmsGsmTpdu iPdu;
	RMobileSmsMessaging::TMobileSmsGsmTpdu iDeliveryReport;
	TSmsSca iSca;	
};


struct TConstraintEntry
{
	TInt iIpcCnt;			// <The Number of times an IPC(now only sendMessage suported)before incoming messages are simulated
	TInt iRxCnt;			// <The number of times sms messages are going to be simulated
};


class CSimSmsStore;
class CSimPhone;
class CSimSmsMessaging : public CSubSessionExtBase, public MTimerCallBack
	{
friend class CSimSmsStore;

public:
	static CSimSmsMessaging* NewL(CSimPhone* aPhone);
	CSimSmsMessaging(CSimPhone* aPhone);
	~CSimSmsMessaging();

	static void CloseSmsObj(TAny* aObj);
	const CTestConfigSection* CfgFileSection();

	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
// MTelObjectTSY pure virtual
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual void Init();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);

// MSubSessionExtBaseTSY pure virtual
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	
	void ReloadConfigurationSettingsL();
	
protected:
	void ConstructL();

private:
	enum TSmsRxEvent
		{
		ESmsEventPostedRxReq,
		ESmsEventRxTimer,
		ESmsEventRxAckNack,
		ESmsEventResume
		};

	enum TSmsRxState
		{
		ESmsRxStateIdle,
		ESmsRxStateWaitingForSmsRx,
		ESmsRxStateWaitingForNetworkAckNackResponse,
		ESmsRxStateWaitingForAckNack,
		ESmsRxStateSuspend,
		ESmsRxStateWaitingForNetworkResumeResponse,
		ESmsRxStateWaitingToStart
		} iRxState;
		
	enum TSmsTxEvent
		{
		ESmsEventSendReq,
		ESmsEventSubmitReportReceived
		};

	enum TSmsTxState
		{
		ESmsTxStateIdle,
		ESmsTxStateWaitingForSubmitReport
		} iTxState;

	TInt HandleExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	
	void FindAndCreateSmsStoresL(CSimPhone* aPhone);
	void FindAndCreateRxAttributesL();
	void FindAndCreateTxAttributesL();
	void FindAndCreateConstraints();
	void FindAndCreateSmsParamsL();

	TInt ReceiveMessageL(const TTsyReqHandle aTsyReqHandle,TDes8* aSmsPdu,TDes8* aSmsAttrib);
	void ReceiveMessageCancel();
	void SendMessageCancel();
	TInt GetCaps(const TTsyReqHandle aReqHandle,TDes8* aPckg1);
	TInt EnumerateMessagingStores(TTsyReqHandle aReqHandle,TDes8* aPckg);
	TInt GetMessageStoreInfo(TTsyReqHandle aReqHandle,TDes8* aPckg1, TDes8* aPckg2);
	TInt GetReceiveMode(const TTsyReqHandle aReqHandle,TDes8* aPckg);
	TInt SetReceiveMode(const TTsyReqHandle aReqHandle,TDes8* aPckg);
	TInt AckSmsStored(const TTsyReqHandle aReqHandle,TDes8* aMsg,TDes8* aFullFlag);
	TInt NackSmsStored(const TTsyReqHandle aReqHandle,TDes8* aMsg,TDes8* aRpCausePckg);
	void AckNackCancel();
	TInt ResumeSmsReception(const TTsyReqHandle aReqHandle);
	void ResumeSmsReceptionCancel();
	TInt SendMessageL(const TTsyReqHandle aTsyReqHandle,TDes8* aPckg1, TDes8* aPckg2);
	TInt GetSmspListPhase1(const TTsyReqHandle aTsyReqHandle,
                                            TDes8* aParam1,TDes8* aParam2);
	void GetSmspListPhase1L(TTsyReqHandle aTsyReqHandle,RMobilePhone::TClientId& aClientId,TInt& aBufSize);
	TInt GetSmspListPhase2(const TTsyReqHandle aTsyReqHandle,
                                            TDes8* aParam1,TDes8* aParam2);
	void GetSmspListCancel();
	TInt StoreSmspList(const TTsyReqHandle aTsyReqHandle,TDes8* aPckg);
	void StoreSmspListCancel();	
	void CompletePendingReq();
	void CompleteTxPendingReq(TInt aError);
	TInt AttemptSmsRxComplete();
	void StartSmsMtTimer();
	void TimerCallBack(TInt aId);
	void PopulateSmsRxAttrib(const TDesC8& aAsciiScaAddr,RMobileSmsMessaging::TMobileSmsReceiveAttributesV1* aAttrib);

	void PopulateSmsTxAttrib(RMobileSmsMessaging::TMobileSmsSendAttributesV1* aAttrib);
	
	void PopulateSmsTxAttrib(RMobileSmsMessaging::TMobileSmsAttributesV1* aAttrib);

	void RecordSca(const TDesC8& aAsciiScaAddr, RMobileSmsMessaging::TMobileSmspEntryV1& aEntry);	
	void RecordDestination(const TDesC8& aAsciiScaAddr, RMobileSmsMessaging::TMobileSmspEntryV1& aEntry);
	void PopulateSmspListFromConfigFile();
	TBool IpcMatch();
	TInt ActionRxEventUnstoredClientAck(TSmsRxEvent aRxEvent);
	TInt ActionRxEventUnstoredPhoneAck(TSmsRxEvent aRxEvent);
	TInt ActionRxEventStored(TSmsRxEvent aRxEvent);
	TInt ActionTxEvent(TSmsTxEvent aTxEvent);
	
	TBool CanStartRxEvent() const;
	TBool ConstraintEllapsed() const;

	void LogRequest(const TBool aEntering, TInt aIpc, TInt aError);

#ifdef _DEBUG
	void LogTMobileSmsAttributesV1(const RMobileSmsMessaging::TMobileSmsAttributesV1& aSmsAttributesV1);
    void AppendTonToBuffer(TDes8& aBuffer, const RMobilePhone::TMobileTON& aTon);
    void AppendNpiToBuffer(TDes8& aBuffer, const RMobilePhone::TMobileNPI& aNpi);
#endif
    	
	HBufC8* PduToAscii(TDesC8& aSmsPdu);
	void DumpPdu(const TDesC8& aText, TDesC8& aSmsPdu, HBufC8* aPduInAscii = 0);
	
	void Reset();
	void InitializeL();
	
private:
	
	CSimTimer* iRxTimer;
	CSimTimer* iTxTimer;
	CSimTimer* iSmspTimer;
	TTsyReqHandle iSmsRxReqHandle;	
	TTsyReqHandle iSmsTxReqHandle;
	TTsyReqHandle iSmspReqHandle;
	TTsyReqHandle iPendingReqHandle;	// < Request handle for pending Ack, Nack or Resume Completion.
	
	CSimPhone* iPhone;
	CArrayFixFlat<CSimSmsStore*>* iSmsStores;
	
	CArrayFixFlat<TSmsRxParametersGsm>* iSmsRxParameterListGsm;	// < Contains the values associated with the SmsRx tag for GSM
		
	TInt iSmsRxPeriod;					// < The period between simulating received SMS messages.
	TInt iAckNackCompletePause;			// < The pause before Ack/Nack requests are completed.
	TInt iResumeCompletePause;			// < The pause before Resume requests are completed.
	TBool iSmsStoreFull;				// < Flag to indicate that last Ack/NAck received stated that the store was full.
	TInt iSmsRxCnt;						// < Count of the number of SMS messages received.
	TBool iSmsRxReqOutstanding;			// < Flags if ReceiveMessage has been posted by the client
	TDes8* iSmsRxPdu;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1* iSmsRxAttrib; // <Attributes to be return to the client On ReceiveMessage completion

	CArrayFixFlat<TSmsTxParametersGsm>* iSmsTxParametersListGsm;			// < Contains the values associated with the Smstx tag 

	RMobileSmsMessaging::TMobileSmsSendAttributesV1* iSmsTxAttrib; // <Attributes to be return to the client On SendMessage completion

	TInt iSmsTxCnt;							// < Count the number times RSmsMessaging::SendMessage. completes	
	TInt iSmsTxPause;						// < The period between the layer 3 send request and the submit report reception
	RMobileSmsMessaging::TMobileSmsControlCaps iSmsControlCaps;	// < The control capabilities of messaging(TMobileSmsControlCaps)

	RMobileSmsMessaging::TMobileSmsReceiveMode iSmsReceiveMode;	// < The control capabilities of messaging(TMobileSmsControlCaps)
	
	//constraint
	TInt iCurrentConstraint;				// < The index in the iConstraints Array currently in use
	TInt iConstraintRxCnt;					//	< The number of SmsMessages received under 1 constraint
	RArray<TConstraintEntry> iConstraints;	// < An array of constraints

	//smsp
	CArrayPtrFlat<CListReadAllAttempt>* iSmspReadAll;	// < A list of the oustanding two phase batch read requests.
	CMobilePhoneSmspList* iSmspEntries;					// < A pointer to the SMSpList array.
	TInt iSmspBatchPause;								// < The pause value used to delay the completion of a batch SMS Store manipulation request.
	TBool iSmspBusy;									// < Used as a semaphore to avoid store/get at the same time.

	RMobileSmsMessaging::TMobileSmsModeCaps iSmsModeCaps;	// < The mode capabilities of messaging(TMobileSmsControlCaps)	
	TInt iSmsRxStartDelay;
	TBool	iSmsReceiveModeSet;
	TUint8	iLastTxMsgId[2];	
	};

#endif // __SIMSMSMESS_H_
