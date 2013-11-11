// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// L2CAP Host resolver.
// Implements Inquiry and Name lookup
// 
//

#ifndef L2HOSTRESOLVER_H
#define L2HOSTRESOLVER_H


#include <bt_sock.h>
#include <es_prot.h>
#include <bluetooth/hcicommandqueue.h>
#include <bluetooth/hcicommandqueueclient.h>
#include <bluetooth/hci/hcitypes.h>
#include <bluetooth/hci/hciconsts.h>

// BT1.1: Could make these runtime configurable.

// HCI Inquiry length time, in 1.28 seconds.
static const TInt KInquiryLength		= 10;
// Upper limit to number of results we can handle
static const TInt KInquiryMaxResults	= 00;

// Seconds between flushes
static const TInt KFlushTimeoutSecs	= 5;
// Number of flushes required to actually clear a record
static const TInt KRecordDeathAge		= 120;	// 10 mins


// Maximum flushes before the a device disappears to inquires
static const TInt KRecordStaleAge		= 10; // 50 secs
// Maximum flushes before the cache is set to stale (needing refresh)
static const TInt KCacheStaleAge		= -1; // -1 == always needs refresh

// While doing inquiry, set of watchdog every second
static const TInt KInquiryWatchdogPeriod = 1;
// Immidiately fetch the first 3 names
static const TInt KImmediateNameFetch = 3;

// Page Timeout for name request, we'll initially use this for each one
static const TUint16 KMinNamePageTimeout = 0x1200; // 0x1200 = 3 secs
// Amount to increase pagetimeout, everytime a lookup fails
static const TUint16 KNamePageTimeoutIncrement = 0x140; // 0x140 = .2 sec
/** Max Page Timeout for name request, we'll consider using (ACL connects
are blocked while doing name lookup, so mustn't be too high!) **/
static const TUint16 KMaxNamePageTimeout = 0x2500; // 0x2500 = 6 secs

// Max times we'll try to get a single name while an inquiry pending
static const TInt KMaxNameLookupAttemptsDuringInquiry = 1;
// Max number of times to try to get a name
static const TInt KMaxNameLookupAttempts = 4;



class CBTInquiryMgr;
class CBTInqResultRef;
class CLinkMgrProtocol;


enum TBTInqResultValid
	{
	EBluetoothAddr 					= 0x001, //000000000001
	EBluetoothCoD		 			= 0x008, //000000001000
	EBluetoothPageScanRepMode		= 0x080, //000010000000
	EBluetoothPageScanMode          = 0x100, //000100000000
	EBluetoothClockOffSet			= 0x200, //001000000000
	EBluetoothRssi					= 0x400, //010000000000
	EBluetoothEir					= 0x800, //100000000000
	EBluetoothJuice			 =  EBluetoothCoD | 
								EBluetoothPageScanRepMode | 
								EBluetoothPageScanMode | 
								EBluetoothClockOffSet
	};


/**
	BT Inquiry Result record.
	Ref counted object, deletes itself when no more CBTInqResultRef objects
	point to it. Aggregates the TInquiryLogEntry type.
**/

NONSHARABLE_CLASS(CBTInqResultRecord) : public CBase
	{
friend class CBTInquiryMgr;
friend class CBTInqResultRef;

public:
	TInquiryLogEntry& LogEntry();
	void SetName(const TDesC8& aName);
	const TDesC8& Name() const;
	TBool IsNameRequestPending() const;
	void SetNamePending(TBool aBool);
	TBool IsNameRefreshRequested() const;
	void SetNameRefreshRequested(TBool aBool);
	TBool IsNameComplete() const;
	void SetNameComplete(TBool aBool);
	TBool IsExplicitNameRequest() const;
	void SetExplicitNameRequest(TBool aBool);
	TBool IsNameValid() const;
	void SetNameValid(TBool aBool);
	void SetNameLookupResultCode(TInt aResultCode);
	TBool HaveNameLookupResult() const;
	TInt NameLookupResultCode() const;
	TInt NameLookupAttempts() const;
	TInt IncFlushes();
	void GetInquirySockAddr(TInquirySockAddr& aAddr);
	TInt GetEir(TNameRecord& aNameRec, TBool aIgnoreCachedName);
	TInt GetName(TNameRecord& aNameRec);
	TInt AddIAC(TUint aIAC);
	TBool HasRespondedToIAC(TUint aIAC);
	TInt NumberOfIACsRespondedTo();
	void ClearIACs();
	inline TBool IsJuiceFromHCI() { return (EBluetoothJuice == (iJuiceFromHCIMask & EBluetoothJuice)); };//for the moment demand all
	inline TBool IsPageScanModeFromHCI() { return iJuiceFromHCIMask & EBluetoothPageScanMode; };
	inline TBool IsPageScanRepModeFromHCI() { return iJuiceFromHCIMask & EBluetoothPageScanRepMode; };
	inline TBool IsCoDFromHCI() { return iJuiceFromHCIMask & EBluetoothCoD; };
	inline TBool IsClockOffsetFromHCI() { return iJuiceFromHCIMask & EBluetoothClockOffSet; }
	inline TBool IsEirPresent() { return iJuiceFromHCIMask & EBluetoothEir; }
	inline TExtendedInquiryResponseDataCodec& Codec() { return iCodec; }
	
private:
	enum TNameStatus
		{
		ENamePending = 1,
		ENameRefreshRequested = 2,
		ENameComplete = 4,
		ENameExplicitRequest = 8, // An explicit name lookup has been requested, so don't put this to the back of the queue
		};

	~CBTInqResultRecord();
	// Interface for CBTInquiryMgr
	CBTInqResultRecord(const TBTDevAddr& aAddr);
	// Interface for CBTInqResultRef
	void Open();
	void Close();

	// Number of CBTInqResultRef objects pointing to this result
	TInt				iRefCount;
	// The actual inquiry log entry. It contains EIR data, RSSI value and old style TInquiryLogEntry.
	TInquiryLogEntryWithEir	iEntry;
	// The device name's state
	TInt				iNameStatus; //Bit flag
	// The result of name lookup, of >0 if one hasn't been done
	TInt				iNameLookupResultCode;
	// The device's name (if known), it can be from EIR or a Remote Name Request
	TBuf8<KHCIRemoteDeviceNameMaxLength>	iName;
	// Number of attempts made at getting name.
	TInt				iNameLookupAttempts;
	// Flush cycles since last seen (lower == newer).
	TInt				iFlushes;
	// IACs this device has responded to
	RArray<TUint>		iIACs;
	// Has inquiry log entry been populated by inquiry?
	TUint16				iJuiceFromHCIMask;
	// Has this entry been found during the current, ongoing inquiry?
	TBool				iFoundDuringCurrentInquiry;
	
	TExtendedInquiryResponseDataCodec iCodec;
	};

/**
	Inquiry result reference.
	References a CBTInqResultRecord.
**/
NONSHARABLE_CLASS(CBTInqResultRef) : public CBase
	{
public:
	CBTInqResultRef(CBTInqResultRef& aRef);
	CBTInqResultRef(CBTInqResultRecord& aRec);
	~CBTInqResultRef();
	CBTInqResultRecord& Result() const;

private:
	// The pointer to actual CBTInqResultRecord
	CBTInqResultRecord& iRecord;
public:
	// List to go on a view, in CBTInqResultSet
	TDblQueLink			iLink;
	};

/**
	BT Inquiry Result Set.
	A view onto a set of the current inquiry results.
**/
NONSHARABLE_CLASS(CBTInqResultSet) : public CBase
	{
public:
	CBTInqResultSet();
	~CBTInqResultSet();
	void Reset();
	CBTInqResultRef* Add(CBTInqResultRecord& aRec);
	CBTInqResultRef* FindEntry(const TBTDevAddr& aAddr);
	CBTInqResultRef* NextResult();
	CBTInqResultRef* CurrentResult();
	void ReturnToFirstResult();
	TBool IsEmpty();
	void MoveToback(CBTInqResultRef& aRef);
private:
	typedef TDblQue<CBTInqResultRef> TResultRefQue;
	typedef TDblQueIter<CBTInqResultRef> TResultQueIter;
	TResultRefQue	iResultRefs;
	TResultQueIter	iNextRefIter;
	CBTInqResultRef* iCurrentResult;
	};

NONSHARABLE_STRUCT(TInquiryCacheAge)
	{
public:
	TUint iIAC;
	TInt iCacheAge;
	};

/**
	BT Host resolver.
	Represents one client host resolver session.
	Only implements GetByAddress, as this is all the
	BT API supports.
**/
NONSHARABLE_CLASS(CBTHostResolver) : public CHostResolvProvdBase
	{
public:
	enum TInquiryStatus
		{
		EInquiryReady,
		EInquiring,
		EInquiryComplete
		};
	enum TNameLookupMode
		{
		EDontGetNames,
		EDoGetNames
		};
	
	enum TRequestState
		{
		EIdle,
		EInquiry,
		ENameLookup,
		ESetLocalName,
		EGetLocalName, 
		EError	// LC added
		};

	CBTHostResolver(CBTInquiryMgr& aInquiryMgr);
	~CBTHostResolver();
	// From CHostResolvProvdBase
	void GetByName(TNameRecord& aName);
	void GetByAddress(TNameRecord& aName);
	void SetHostName(TDes& aNameBuf);
	void GetHostName(TDes& aNameBuf);
// Need this to allow instantiation.  Doesn't seem to be callable though!
// Simply stub it, as we don't actually care...
	virtual TInt SetOption(TUint aLevel, TUint aName, const TDesC8& aOption);

	// From CResolverProvdBase
	void CancelCurrentOperation();

	// IquiryMgr interface
	void InquiryResult(CBTInqResultRecord& aResult);
	void NameLookupResult(TInt aErr, const TBTDevAddr& aAddr, const TDesC8& aName);
	void InquiryComplete(TInt aErr);
	void SetLocalNameComplete(TInt aErr);
	void GetLocalNameComplete(TInt aErr, const TDesC8& aName);

	TUint GetIAC() const;
	inline static TInt LinkOffset() {return _FOFF(CBTHostResolver, iLink);}

	//From CResolverProvdBase
	TInt SecurityCheck(MProvdSecurityChecker *aSecurityChecker);
private:
	void TryToCompleteRequest();
	void CompleteRequest(TInt aErr);
	void CompleteCurrentOperation();

	CBTInquiryMgr&		iInquiryMgr;
	// The record we're currently retreiving
	TNameRecord*		iNameRecord;
	// the buffer we get host name into
	TDes*				iHostNameBuf;
	
	// Current user request.
	TRequestState		iRequestState;
	// Status of the on-going inquiry process
	TInquiryStatus		iInquiryStatus;
	// Get names or not?
	TNameLookupMode		iNameLookupMode;
	// Error code... Only valid when iInquiryStatus == EInquiryComplete
	TInt				iInqCompletionCode;
	// Set of devices found, to be returned to client
	CBTInqResultSet		iResults;
	// Member of Q in InquiryMgr.
	TDblQueLink iLink;
	//Mixin providing security checking, This is not an owned variable.
	MProvdSecurityChecker* iSecurityChecker;
	TInquirySockAddr	iSockAddr;
	};

#ifdef CONNECTION_PREEMPTS_INQUIRY
class CConnectingStatusSubscriber;
#endif

/**
	BT Inquiry Manager.
	Singleton object that manages host resolvers, inquiries, and
	name lookups.
**/
NONSHARABLE_CLASS(CBTInquiryMgr) : public CBase, public MHCICommandQueueClient
	{
public:
	// Interface for CL2CAPProtocol
	~CBTInquiryMgr();
	static CBTInquiryMgr* NewL(CLinkMgrProtocol& aProtocol);
	CBTHostResolver* NewHostResolverL();

	void SetHCICommandQueue(MHCICommandQueue& aCommandQueue);
	void ClearHCICommandQueue();

	void ClockOffsetResult(const TBTDevAddr& aAddr, TBasebandTime aClockOffset);
	void CoDResult(const TBTDevAddr& aConn, TUint aCoD);

	void SetLocalNameComplete(TInt aErr);
	void GetLocalNameComplete(TInt aErr, const TDesC8& aName);
	void Suspend();
	void Resume();
	void SetInquiryMode();
	

#ifdef _DEBUG
	void IncrementHRCount() {++iNumHRs;};
	void DecrementHRCount() {--iNumHRs;};
#endif

	// Interface for CBTHostResolver
	void StartInquiry(CBTHostResolver& aResolver, TUint aIAC, TBool aIgnoreCache);
	void LookupName(CBTHostResolver& aResolver, const TBTDevAddr& aAddr, TBool aIgnoreCache, TBool aExplicitNameRequest);
	void ClearCache();
	TInt SetLocalName(const TDesC8& aName);
	TInt GetLocalName();
	CBTInqResultRef* FindExistingCacheEntry(const TBTDevAddr& aAddr);
	CBTInqResultRef* AddEntryToCache(const TBTDevAddr& aAddr);
	CBTInqResultRef* AddEntryWithJuiceToCache(const TInquiryLogEntry& aEntry);
	CBTInqResultRef* AddEntryWithCoDToCache(const TBTDevAddr& aAddr, const TUint aCoD);
	CBTInqResultRef* AddEntryWithClockOffsetToCache(const TBTDevAddr& aAddr, const TBasebandTime aClockOffset);
	const TDesC8* DeviceNameFromCache(const TBTDevAddr& aAddr);
	CBTInqResultRecord* BasebandParametersFromCache(const TBTDevAddr& aAddr);
	void DeletingHostResolver();

		enum THWState
		{
		EIdle = 0,
		EInquiry,
		ENameLookup,
		EConnecting,	// can't do anything really!
//		EConnected		// can't do anything ?
		EOff,
		ECancellingForNewIAC,
		};

	THWState HWState(){ return iHWState; }
	void SetHWState(THWState aState);
	void PublishStatus();

	void CompleteCommands(TInt aErr);
	TBool IsExtendedInquiryResponseSupported();

	// commands sent to the controller -- called from external classes
	void ReadRemoteNameL(const TBTDevAddr& aAddr);

private: // Handling logical inquiry events
	void WriteInquiryModeComplete(TBool aSucceeded);
	void InquiryResult(TInt aErr,const TInquiryLogEntry& aEntry);
	void InquiryComplete(TInt aErr, TUint8 aNumResponses);
	void RemoteNameResult(TInt aErr, const TBTDevAddr& aAddr, const TBTDeviceName8& aBuf);
	void RemoteHostSupportedFeatures(TInt aErr, const TBTDevAddr& aAddr, const TUint64& aHostSupportedFeatures);

private: // from MHCICommandQueueClient
	virtual void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	virtual void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);

private: // HCI event handling functions
	// basic types
	void CommandCompleteEvent(const THCIEventBase& aEvent);
	void CommandStatusEvent(const THCIEventBase& aEvent, const CHCICommandBase& aCommand);

	// first class events
	void InquiryResultEvent(const THCIEventBase& aEvent);
	void InquiryResultWithRSSIEvent(const THCIEventBase& aEvent);
	void ExtendedInquiryResultEvent(const THCIEventBase& aEvent);
	void InquiryCompleteEvent(const THCIEventBase& aEvent);
	void RemoteNameReqCompleteEvent(const THCIEventBase& aEvent);
	void RemoteHostSupportedFeaturesNotificationEvent(const THCIEventBase& aEvent);

	// command complete events
	void WriteInquiryModeOpcode(THCIErrorCode aHciErr, const THCIEventBase& aEvent);
	void ReadLocalNameOpcode(THCIErrorCode aHciErr, const THCIEventBase& aEvent);
	void InquiryCancelOpcode(THCIErrorCode aHciErr, const THCIEventBase& aEvent);

private:
	CBTInquiryMgr(CLinkMgrProtocol& aProtocol);
	void ConstructL();

	MHCICommandQueue& CommandQueue() const;

	// commands sent to the controller
	void CancelInquiryL();
	void CancelRemoteNameL(const TBTDevAddr& aAddr);
	void StartInquiryL(TUint aIAC, TUint8 aLength, TUint8 aNumResponses);
	void WriteInquiryModeL(TUint8 aInquiryMode);
	void LookupNameL(const TInquiryLogEntry& aEntry);
	void ReadLocalNameL();

	void DoInquiry();
	void DoNameLookup(TBool aInquiryComplete);
	void HandleRemoteNameResult(TInt aErr, CBTInqResultRef& aRef, const TBTDeviceName8& aBuf);
	void TryToInterruptInquiryForNameLookup();
	void UpdateNotifiers(const TBTDevAddr& aAddr, const TDesC8& aName);
	void ClearCurrentInquiryResults();
	TInt CancelHardwareInquiry();
	TInt StartHardwareInquiry();

	static TInt InquiryWatchdog(TAny* aPtr);
	static TInt Flush(TAny* aPtr);
	void DoFlush();
	void EnableFlusher();

	TInt CacheAge(TUint aIAC) const;
	void SetCacheAge(TUint aIAC, TInt aAge);
	
	TUint RequestedInquiryIAC();
	
private:
	MHCICommandQueue*   iCommandQueue;

	// Singleton owner of us
	CLinkMgrProtocol& iLinkMgrProtocol;

	// What the hardware is currently up to (as far as we know)
	THWState		iHWState;
	// IAC of the current inquiry
	TUint			iCurrentInquiryIAC;
	// Number of times we've interrupted the current inquiry
	TInt			iInquiryInteruptions;
	// Time since the last inquiry, per IAC
	RArray<TInquiryCacheAge> iCacheAge;
	// Timer to age records and the cache itself
	CPeriodic*		iFlusher;
	// Results since watchdog last went off
	TInt			iResultCount;
	// Number of watchdogs seeing 0 devices
	TInt			iInquirySilenceCount;
	// Number of name requests pending
	TInt			iPendingNameRequests;
	// True is there are new page requests pending
	TInt			iNewPageRequestsPending;
	// Current name request pagetimeout
	TUint16			iNamePageTimeout;
	// Current Inquiry Mode
	TUint8			iInquiryMode;
	// Pending Inquiry Mode
	TUint8			iPendingInquiryMode;

	CBTInqResultSet					iCurrentResults;
	TDblQue<CBTHostResolver>		iHRs;

	TBool			iReportedInquiryState; // what did we last tell the Inquiry state P&S key
#ifdef CONNECTION_PREEMPTS_INQUIRY
	// Monitors if a ACL connection is currently being established.
	CConnectingStatusSubscriber*	iConnectingStatus;
#endif
	
#ifdef _DEBUG
	TInt							iNumHRs; // no Count() in que's - handy to have
#endif
	};

#ifdef CONNECTION_PREEMPTS_INQUIRY
// *******************************************************************
// ACL Connecting status subscriber
// *******************************************************************
NONSHARABLE_CLASS(CConnectingStatusSubscriber) : public CActive
	{
public:
	static CConnectingStatusSubscriber* NewL(CBTInquiryMgr& aInquiryMgr); 
	~CConnectingStatusSubscriber();

private:
	CConnectingStatusSubscriber(CBTInquiryMgr& aInquiryMgr);
	void ConstructL();
	void Subscribe();
	
	void RunL();
	void DoCancel();

	CBTInquiryMgr& iParent;
	RProperty iProperty;
	};
#endif	// CONNECTION_PREEMPTS_INQUIRY

#endif
