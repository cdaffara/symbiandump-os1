/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/**
 @file te_TransportStep.h
*/

#if (!defined TE_TRANSPORTSTEP_H)
#define TE_TRANSPORTSTEP_H

#include <cflog.h>
#include <elements/metatype.h>
#include "te_TransportServer.h"
#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>
#include <elements/cftransport.h>
#include <elements/cftransportmsg.h>
#include <in_sock.h>
#include <cfthread.h>
#include <elements/interfacetable.h>
#include "transportSpecificDefs.h"
#include <es_sock.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include <elements/nm_interfaces.h>
#include <elements/nm_signals.h>

using namespace NetInterfaces;
using namespace CommsFW;


_LIT(KMutex, "JournalMutex");
const TUint16 KBadWorkerId = -1;
const TUint16 KMaxWorkerId = 1;
const TInt KMaxPeers=2;

typedef TInt TPeerId;
//typedef TInt TWorkerId;
typedef CWorkerThreadRegister<TWorkerThreadDataBase, CommsFW::KMaxWorkerId>  TWorkerRegister;
typedef CWorkerThreadDataGlobals<TWorkerThreadDataBase, CommsFW::KMaxWorkerId> TWorkerGlobals;

// large enough for a big blob msg
typedef TBuf<1000> TLarge;
// Small msg
typedef TBuf<2> TSmall;
// large enough to split msg but not blob
typedef TBuf<20> TNormal;

typedef TBuf8<1000> TLarge8;
// Small msg
typedef TBuf8<2> TSmall8;
// large enough to split msg but not blob
typedef TBuf8<20> TNormal8;

extern TInt journal[KTotalThreads][KJournalSlots];
extern TInt KTimeInterval;




/////////////////////////////////////////////////////////////////////////////////////////////////
/*********************** Classes and Interfaces For Object Dispatch ****************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

/*
  These are the interfaces that the transport messages will be dispatched
 */


enum TTestItfSet { EMBasicItfId = 1066, EMLinkItfId = 1067, EMBlob = 1068};

class MBasicItf
// a basic interface for message dispatch of transport test messages
	{
public:
	// API functions
	virtual void SimpleDispatch(TInt aId) = 0;
	virtual void NullDispatch(TInt aId) = 0;
	enum KMemberFns { ESimpleDispatch, ENullDispatch, EHookDispatch, EListener, EDeregListener };
    static const TTestItfSet KInterfaceId = EMBasicItfId;
	};


class MLinkItf
	{
public:
	// API functions
	virtual void StartRespL(TInt aError) = 0;
	virtual void LargeDispatchThread1(const TDesC& aBuf) = 0;
	virtual void LargeDispatchThread2(const TDesC& aBuf)= 0;
	virtual void ProvisionRequest(TPtrC8& aMetaParams) = 0;

	enum KMemberFns { EStartResp, ELargeDispatch1, ELargeDispatch2, EProvisionRequest};
    static const TTestItfSet KInterfaceId = EMLinkItfId;
	};

class RBasicItf : public Messages::RClientInterface
	{
public:
	RBasicItf();
	void SimpleDispatch(TInt a1);
	void NullDispatch(TInt a1);
	};

class RLinkItf : public RBasicItf
	{
public:
	RLinkItf();
	void StartResp(TInt aError);
	void DispatchThread1(const TDesC& aBuf);
	void DispatchThread2(const TDesC& aBuf);
	void ProvisionRequestL(const Meta::SMetaData& aParams);
	};

class TLargeMessage: public Meta::SMetaData
	{
public:
	enum { KItfId = 1666, KFn1Id = 22 };
	DATA_VTABLE
	TLarge iBuf;
	};

class TNormalMessage: public Meta::SMetaData
	{
public:
	enum { KItfId = 1667, KFn1Id = 23 };
	DATA_VTABLE
	TNormal iBuf;
	};

///////////////////////////////////////////////////////////////////////////////////////////////////
/******************************** CLASSES USED FOR RECIEVING *************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////

class CItfTestImpl : public CBase,
				     public MBasicItf,
				     public MLinkItf,
					 public NetInterfaces::TInterfaceControl,
					 public ITFHIERARCHY_2(CItfTestImpl, MBasicItf, MLinkItf)

// Class used as a dispatcher for TCFExternBase Messages
// this class will MBasic and MLink message types and dispatch them
 	{
public:
	typedef ITFHIERARCHY_2(CItfTestImpl, MBasicItf, MLinkItf) TIfStaticFetcherNearestInHierarchy;

public:
	CItfTestImpl();
	void SetJournalPosition(TInt aJournalPosition);
	void ReturnInterfacePtrL(MBasicItf*& aInterface);
	void ReturnInterfacePtrL(MLinkItf*& aInterface);

	// From MBasicItf
	virtual void SimpleDispatch(TInt aId);
	virtual void NullDispatch(TInt aId);

	// From MLinkItf
	virtual void StartRespL(TInt aError);
	virtual void LargeDispatchThread1(const TDesC& aBuf);
	virtual void LargeDispatchThread2(const TDesC& aBuf);
	virtual void ProvisionRequest(TPtrC8& aMetaParams);

	void OpenMutexAndWait(RMutex& aMutex);
	void SignalMutexAndClose(RMutex& aMutex);


	public :
	TInt iThread1State;
	TInt iThread2State;
	TInt iJournalPosition;
	};

/////////////////////////////////////////////////////////////////////////////////////////////////
/*************************** Structs For holding general data **********************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

enum JournalCodes
// when the individual test threads/peers complete their messages they will set the results in a journal that
// is accessable to the main test thread for comparison at the end, these are the journal codes
	{
	/*
	Message recieved correctly
	*/
	EMessageRecieved = 200,

	/*
	Blob has been received correctly by reciever, contents of blob is accurate
	*/
	EBlobRecievedCorrectly = 201,

	/*
	No message has been recieved by the reciever thread
	*/
	ENoMessageRecieved = 202,

	/*
	Blob has been recieved but the information it contains is inaccurate
	*/
	EBlobRecievedContentsIncorrect = 203,

	/*
	This is the case where a message that was corrupt or incorrectly posted is recieved,
	this should not happen
	*/
	EBadMessageRecieved = 204,

	/*
	Used in Exit array if one of the Transport Thread contains a memory Leak
	*/
	EMemoryLeak = 300,

	/*
	Used in Exit array if one of the Transport Thread contains a memory Leak
	*/
	ENoMemoryLeak = 301,
	};

struct TGlobalInfo
// A global struct containing different types of cookie that are used throughout the tests
	{
	TGlobalInfo()
		{
		//TCFLegacyMessagePacker::PackForPostage(2, iBadCookie);
		iBadWorkerId = ::KMaxWorkerId + 1;
		}

	TWorkerId iThread1LegacyWorkerId;
	TWorkerId iThread2LegacyWorkerId;
	//TCFMessage2 iThread1LegacyCookie;
	//TCFMessage2 iThread2LegacyCookie;
	TWorkerId iBadWorkerId;
	
	Messages::TNodeId iRxLegacyCookie;
	Messages::TNodeId iDispatchCookie;
	Messages::TNodeId iDispatchCookie2;
	Messages::TNodeId iThread1DispatchCookie;
	Messages::TNodeId iThread2DispatchCookie;
	Messages::TNodeId iTransportHookCookie;
	Messages::TNodeId iTransportHookCookieClient;
	Meta::CMetaDataVirtualCtorInPlace* iVCtor;
	};

GLREF_C TGlobalInfo globalInfo;

class TWorkerParams
//class that contains information about a peer, is passed to CCommsTransport
	{
public:
	TWorkerParams(const RCFChannelPair& aChannel, TWorkerGlobals& aThreadGlobals, TWorkerId aWorkerId, TWorkerId aPeerId)
		: iThreadGlobals(aThreadGlobals),
	   	 iWorkerId(aWorkerId),
		  iPeerId(aPeerId)
		{
		aChannel.GetRecvMsgQueues(iInQueues);
		aChannel.GetSendMsgQueues(iOutQueues);
		}

	TWorkerGlobals& iThreadGlobals;
	RCFChannelPair::TMsgQueues iOutQueues;
	RCFChannelPair::TMsgQueues iInQueues;
	TWorkerId iWorkerId;
	TWorkerId iPeerId;
	};

////////////////////////////////////////////////////////////////////////////////////////
//******************************* BASIC MESSAGES *************************************//
////////////////////////////////////////////////////////////////////////////////////////

class TCFBaseImpl : public Messages::TSignalBase
// Basic transport message type
	{
public :
	TCFBaseImpl();
	virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
	virtual void Error(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient, TInt aError);
	};


class TSimpleDispatch : public TCFBaseImpl
// A simple message dispatch
	{
public:
	DECLARE_MVIP_CTR(TSimpleDispatch)
	inline TSimpleDispatch();
	inline TSimpleDispatch(TInt aId, TNormalMessage& aMsg);
	virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);

public:
	DATA_VTABLE
	TInt iId;
	TNormalMessage iMsg;
	};


class TNullDispatch : public TCFBaseImpl
// A null dispatching message
	{
public:
	DECLARE_MVIP_CTR(TNullDispatch)
	inline TNullDispatch();
	inline TNullDispatch(TInt aId, TNormalMessage& aMsg);
	virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);

public:
	DATA_VTABLE
	TInt iId;
	TNormalMessage iMsg;
	};

////////////////////////////////////////////////////////////////////////////////////////
//******************************* LINK MESSAGES **************************************//
////////////////////////////////////////////////////////////////////////////////////////

class TStartRespMsg : public Messages::TSignalBase
	{
public:
	DECLARE_MVIP_CTR(TStartRespMsg)
	inline TStartRespMsg();
	inline TStartRespMsg(TInt aError);

	virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
	virtual void Error(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient, TInt aError);

public:
	DATA_VTABLE
	TInt iError;
	};


//*** TLargeDispatchThread1

class TLargeDispatchThread1 : public TCFBaseImpl
// This is a large message that is not send as a Blob and needs to be chipped into
// seperate parts by the transport mechanism in order to sent. This message is used on
// peers that have the ability to send and recieve transport test messages
	{
public:
	DECLARE_MVIP_CTR(TLargeDispatchThread1)
	inline TLargeDispatchThread1();
	inline TLargeDispatchThread1(const TDesC& aBuf);
	virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);

public:
	DATA_VTABLE
	TSmall iBuf;
	};


//*** TLargeDispatchThread2

class TLargeDispatchThread2 : public TCFBaseImpl
// This is a large message that is not send as a Blob and needs to be chipped into
// seperate parts by the transport mechanism in order to sent. This message is used on
// peers that have the ability to send and recieve transport test messages
	{
public:
	DECLARE_MVIP_CTR(TLargeDispatchThread2)
	inline TLargeDispatchThread2();
	inline TLargeDispatchThread2(const TDesC& aBuf);
	virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);

public:
	DATA_VTABLE
	TSmall iBuf;
	};


//*** TProvisionRequestMsg

class TProvisionRequestMsg : public TCFBaseImpl
// A simple message that sends a Large Blob message
	{
public:
	DECLARE_MVIP_CTR(TProvisionRequestMsg)
	inline TProvisionRequestMsg();
	inline TProvisionRequestMsg(const Meta::SMetaData& iParams);
	virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
public:
	DATA_VTABLE
	TLarge8 iParams;
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//****************** Active object to stop the Scheduler after a set amount of time *******************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class CSchedulerStopper : public CActive
// An active object that when started will stop the active scheduler
// after a time specified by the test ini file
{
public :
	~CSchedulerStopper();
	static CSchedulerStopper* NewL();
	static CSchedulerStopper* NewLC();
	virtual void RunL();
	void StartL();
	void Cancel();
private :
	virtual void DoCancel();
	CSchedulerStopper();
	void ConstructL();
	RTimer iTimer;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//******************************* Base Class for Transport across Threads *********************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CTransport : public CBase
// Base class that will enable a peer to transport messages between Threads or between
// two peers in the same thread
	{
public :
	void ConstructL(TWorkerParams& aParams, Meta::CMetaDataVirtualCtorInPlace* aVCtor = globalInfo.iVCtor);
	~CTransport();
	void OpenSemaphores(RSemaphore& aSem1, RSemaphore& aSem2);
	void OpenMutexAndWait(RMutex& aMutex);
	void SignalMutexAndClose(RMutex& aMutex);
	static RHeap& __START_HEAPCHECK();
	static void __END_HEAPCHECK(RHeap& aHeap);


protected:
	TWorkerRegister* iThreadRegister;
	CCommsTransport* iTransport;
	TWorkerId iPeerId;
	CActiveScheduler* iActSched;
	TInt iJournalCounter;
	CSchedulerStopper* iStopper;
	CCFTransportHooks* iHook;
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//******************************* Classes used for recieving messages *********************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class CRecieve : public CTransport, public MLegacyMessageReceiver
//NET-COMNF-FWORK-770001, NET-COMNF-FWORK-770003
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
	static CRecieve* NewLC(TWorkerParams& aParams);
private :
	void DoItL();
	// From MLegacyMessageReciever
	virtual void DispatchL(const TCFMessage& aMessage, TWorkerId /*aSenderId*/);
	virtual void OnDispatchLeave(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/, TInt /*aFirstDispatchLeaveReason*/)
		{
		}
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//******************************** Class used for DISPATCH RECIEVING **********************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class CSimpleDispatchRecieve : public CTransport, MLegacyMessageReceiver
//NET-COMNF-FWORK-770006
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
	static CSimpleDispatchRecieve* NewLC(TWorkerParams& aParams, Meta::CMetaDataVirtualCtorInPlace* aVCtor);
	~CSimpleDispatchRecieve();
	virtual void DispatchL(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/);

private :
	void DoItL(Meta::CMetaDataVirtualCtorInPlace* aVCtor);

private :
	CItfTestImpl* iClientItfTest;
	CItfTestImpl* iClientNarrowTest;
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//*************************** RECIEVER THAT HALF DROPS TRANSPORT TO PEER  *****************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class CDropTransportRecieve : public CTransport, public MLegacyMessageReceiver
//Will be used as entry point for threads who act as a reciever but half drop transport to peer
//NET-COMNF-FWORK-770019
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
private :
	void DoItL();
	static CDropTransportRecieve* NewLC(TWorkerParams& aParams);
	virtual void DispatchL(const TCFMessage& aMessage, TWorkerId /*aSenderId*/);
	virtual void OnDispatchLeave(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/, TInt /*aFirstDispatchLeaveReason*/);
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//********************************** Class used for EMPTY RECIEVING ***********************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class CEmptyRecieve : public CTransport, public MLegacyMessageReceiver
//Will be used as entry point for threads act as a reciever but do not dispatch any messages
//NET-COMNF-FWORK-770014
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
	~CEmptyRecieve();
private :
	void DoItL();
	static CEmptyRecieve* NewLC(TWorkerParams& aParams);
	virtual void DispatchL(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/);
	virtual void OnDispatchLeave(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/, TInt /*aFirstDispatchLeaveReason*/);

private:
	CSchedulerStopper* iStopper;
	};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//****************************** RECIEVER THAT LEAVES DURING DIAPATCH *********************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class CLeavingRecieve : public CTransport, public MLegacyMessageReceiver
//NET-COMNF-FWORK-770020
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
private :
	void DoItL();
	static CLeavingRecieve* NewLC(TWorkerParams& aParams);
	virtual void DispatchL(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/);
	virtual void OnDispatchLeave(const TCFMessage& aMessage, TWorkerId /*aSenderId*/, TInt aFirstDispatchLeaveReason);

	};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************** Class used for UNREGISTERED LEGACY Recieving ********************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class CUnregisteredRecieve : public CTransport, public MLegacyMessageReceiver
//NET-COMNF-FWORK-770015
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
	~CUnregisteredRecieve();
private:
	void DoItL();
	static CUnregisteredRecieve* NewLC(TWorkerParams& aParams);
	virtual void DispatchL(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/);

private :
	CSchedulerStopper* iStopper;
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//******************** Class used for DEREGISTERED DISPATCH recieving *********************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class CDeregDispatchRecieve : public CTransport
//Will be used as the reciever for a simple one way DEREGISTERED object dispatch
//NET-COMNF-FWORK-770017
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
	static CDeregDispatchRecieve* NewLC(TWorkerParams& aParams,Meta::CMetaDataVirtualCtorInPlace* aVCtor);
	~CDeregDispatchRecieve();
private :
	void DoItL(Meta::CMetaDataVirtualCtorInPlace* aVCtor);
	CItfTestImpl* iClientItfTest;
	CSchedulerStopper* iStopper;
	};



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//******************************** Class used for Blocked RECIEVER ************************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBusyReciever : public CTransport, public MLegacyMessageReceiver
//NET-COMNF-FWORK-770005, NET-COMNF-FWORK-770004
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
private :
	void DoItL(TInt aTimeInterval);
	virtual void DispatchL(const TCFMessage& aMessage, TWorkerId /*aSenderId*/);
	~CBusyReciever();
	static CBusyReciever* NewLC(TWorkerParams& aParams);
private :
 	CSchedulerStopper* iStopper;
	};


//////////////////////////////////////////////////////////////////////////////////////////////////////
//************************************ Classes for Sending data ************************************//
//////////////////////////////////////////////////////////////////////////////////////////////////////

class CSend : public CTransport
//Will be used as entry point fot threads that will only send messages
//NET-COMNF-FWORK-770015, NET-COMNF-FWORK-770001, NET-COMNF-FWORK-770003,NET-COMNF-FWORK-770016,
//NET-COMNF-FWORK-770018, NET-COMNF-FWORK-770019, NET-COMNF-FWORK-770020
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
private:
	void DoItL();
	static CSend* NewLC(TWorkerParams& aParams);
	};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//******************************* Class for SENDING DISPATCH MSGS *********************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////

class CSimpleDispatchSend : public CTransport
//Will be used as entry point fot threads that will send simple Dispatch messages
//NET-COMNF-FWORK-770017, NET-COMNF-FWORK-770006
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
	static CSimpleDispatchSend* NewLC(TWorkerParams& aParams);
private :
	void DoItL();
	};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//***************************** Class for SENDING A MSG on a BAD COOKIE ***************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////

class CSendBadCookie : public CTransport
//Will be used as entry point for threads that send cookies with bad data
//NET-COMNF-FWORK-770014
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
private :
	void DoItL();
	static CSendBadCookie* NewLC(TWorkerParams& aParams);
	};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//****************** Class for SENDING/RECIEVING lEGACY MSGS IN SAME THREAD ***********************//
/////////////////////////////////////////////////////////////////////////////////////////////////////

class COneThreadSendRecieve : public CBase, public MLegacyMessageReceiver
//Will be used as entry point for threads that send and recieve in one thread
	{
public :
	void ConstructL(TWorkerParams& aParams);
	~COneThreadSendRecieve();
	void Send();
	virtual void COneThreadSendRecieve::DispatchL(const TCFMessage& aMessage, TWorkerId /*aSenderId*/);

public :
	static COneThreadSendRecieve* NewLC(TWorkerParams& aParams);
	virtual void OnDispatchLeave(TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/, TInt /*aFirstDispatchLeaveReason*/);
	void RegisterInterfaces();
	void DeregisterInterfaces();
	void DropTransport();
	void SendDispatch();
private :
	TWorkerRegister* iThreadRegister;
	CCommsTransport* iTransport;
	TWorkerId iPeerId;
	TInt iJournalCounter;
	CItfTestImpl* iClientItfTest;
	CItfTestImpl* iDispatchTestItf;
	};




/////////////////////////////////////////////////////////////////////////////////////////////////////
//************************* Class for SENDING/RECIEVING lEGACY MESSAGES ***************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////

class CSendRecieve : public CTransport, public MLegacyMessageReceiver
//Will be used as entry point for threads that send and recieve
//NET-COMNF-FWORK-770007
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
private:
	static CSendRecieve* NewLC(TWorkerParams& aParams);
	void DoItL();
	void Send();
	virtual void DispatchL(const TCFMessage& aMessage, TWorkerId /*aSenderId*/);
	virtual void OnDispatchLeave(const TCFMessage& /*aMessage*/, TWorkerId /*aSenderId*/, TInt /*aFirstDispatchLeaveReason*/);
	void RegisterInterfaces();
	void DeregisterInterfaces();
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//****************************** Class for OOM Deadlocking Dispatch *******************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////


class COOMDispatchSendRecieve : public CTransport
//NET-COMNF-FWORK-770012, NET-COMNF-FWORK-770013
	{
public :
	static void EntryPoint(TWorkerParams* aParams);
	~COOMDispatchSendRecieve();
private :
	static COOMDispatchSendRecieve* NewLC(TWorkerParams& aParams, Meta::CMetaDataVirtualCtorInPlace* aVCtor);
	void DoItL(Meta::CMetaDataVirtualCtorInPlace* aVCtor);
	void RegisterInterfacesL(Meta::CMetaDataVirtualCtorInPlace* aVCtor);
	void DeregisterInterfaces(Meta::CMetaDataVirtualCtorInPlace* aVCtor);
	void Send();

private :
	CItfTestImpl* iClientItfTest;
	CSchedulerStopper* iStopper;
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//******************************* struct for storing config data **********************************//
/////////////////////////////////////////////////////////////////////////////////////////////////////


struct TConfigData
//Struct containing all the data from the .ini concerning the peers
	{
public :
	TConfigData(TPeerId aPeerId, TWorkerId aWorkerId)
		:
		iPeerId(aPeerId), iWorkerId(aWorkerId)
		{
		}
 	TPeerId iPeerId;
	TWorkerId iWorkerId;
 	};


///////////////////////////////////////////////////////////////////////////////////////////////////
//******************************** Test Step Class Declarations *********************************//
///////////////////////////////////////////////////////////////////////////////////////////////////
class RCommsBufPondOp;
class CTransportTestStep : public CTestStep
//The base test for thread transport test steps
	{
public :
	virtual ~CTransportTestStep();
	virtual TVerdict doTestStepPreampleL();
	virtual TVerdict doTestStepL()=0;
	void InitDataL();
	void CreateChannelPairsL();
	void CreateWorkerParams(TWorkerGlobals* aThreadGlobals);
	RCFThread* CreateThreadsL();
	void RunThreadsL(TThreadFunction aThreadFunction1, TThreadFunction aThreadFunction2, TThreadPriority aPriority1,TThreadPriority aPriority2, const TDesC &aThreadName1, const TDesC &aThreadName2, const TInt aThreadMaxHeap1 = 1048576, const TInt aThreadMaxHeap2 = 1048576);
	void RunOOMThreadsL(TThreadFunction aThreadFunction1, TThreadFunction aThreadFunction2, const TDesC &aThreadName1, const TDesC &aThreadName2, const TInt aThreadMaxHeap1 = 1048576, const TInt aThreadMaxHeap2 = 1048576);
	void CloseChannelPairs();
	void OpenMutexAndWait(RMutex& aMutex);
	void SignalMutexAndClose(RMutex& aMutex);
	void VerifyCSend();
	void VerifyTwoWaySend(TInt aCode);
	void VerifyHeapChecks();

public :
	TInt iTasks;
	RCFChannelPair iChannelPairs[2];
protected :
	TConfigData* iConfigDataList[KMaxPeers];
	TInt SemCount;
#ifdef SYMBIAN_ZERO_COPY_NETWORKING	
	void CreateCommsBufPondLC(RCommsBufPondOp& aPond);
#else
	CMBufManager* CreateCMBufManagerLC();
#endif // SYMBIAN_ZERO_COPY_NETWORKING	
	};


class CTransportTestStep1_1 : public CTransportTestStep
 	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep1_2 : public CTransportTestStep
 	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep1_3 : public CTransportTestStep
 	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep1_4 : public CTransportTestStep
 	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep1_5 : public CTransportTestStep
 	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep1_6 : public CTransportTestStep
 	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep1_7 : public CTransportTestStep
 	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep2_1 : public CTransportTestStep
 	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep3_5 : public CTransportTestStep
 	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep3_6 : public CTransportTestStep
 	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep4_1 : public CTransportTestStep
	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep5_1 : public CTransportTestStep
 	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep5_3 : public CTransportTestStep
 	{
 	virtual TVerdict doTestStepL();
 	};

class CTransportTestStep7_1 : public CTransportTestStep
	{
 	virtual TVerdict doTestStepL();
 	};

 class CTransportTestStep7_2 : public CTransportTestStep
	{
 	virtual TVerdict doTestStepL();
 	};

#endif

