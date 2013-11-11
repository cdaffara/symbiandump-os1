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

#ifndef BTCOMM_H__
#define BTCOMM_H__

#include <es_sock.h>
#include <cs_port.h>

#include <bt_sock.h>
#include <btmanclient.h>
#include <btdefcommport.h>


static const TInt KBTCOMMRecvBufferLength=1024;
static const TInt KBTCOMMSendBufferLength=1024;
static const TInt KBTCOMMCircularBufferLength=4096;
static const TInt KBTCOMMCircularBufferHighWatermark=3072;
static const TInt KBTCOMMCircularBufferLowWatermark=2048;

const TUint KCommLowUnit=0;
const TInt KBtcommShutdownTimer=6000000; //2 secs



//  CONCRETE FACTORY CLASSES FOR BTMODEM CSY

#define SERIAL_DESCRIPTION _L("BT RFComm Serial Emulation")
#define SERIAL_NAME _S("BTCOMM")

// Concrete factory makes the CPort which does the work
// This uses the "highlander pattern".  ie there can only
// be one factory. Note that the one factory instance 
// can make many CPort instances however.
class CBTPortStateFactory;
NONSHARABLE_CLASS(CBTPortFactory) : public CSerial
	{
public:
	~CBTPortFactory();
	static CBTPortFactory* NewL();
	virtual CPort* NewPortL(const TUint aUnit);
	virtual void Info(TSerialInfo& aSerialInfo);
	inline CBTPortStateFactory* GetPortStateFactory() {return iPortStateFactory;};
protected:
	//From CSerial
    virtual TSecurityPolicy PortPlatSecCapability(TUint aPort) const;
private:
	void ConstructL();
	CBTPortFactory();
private:
	CBTPortStateFactory* iPortStateFactory;
	};


class CBTPortProxy;
class CBTPortLocker;
class CBTPortReader;
class CBTPortWriter;
NONSHARABLE_CLASS(CBTPort) : public CPort
/**
	Concrete implementation of the BTComm CSY.
	This is in effect the server of the RComm client requests.
*/
	{
	friend class CBTPortProxy;
public:
	static CBTPort* NewL(TUint aUint,CBTPortStateFactory* aFactory);
public:
	virtual void StartRead(const TAny* aClientBuffer,TInt aLength);
	virtual void ReadCancel();
	virtual TInt QueryReceiveBuffer(TInt& aLength) const;
	virtual void ResetBuffers(TUint aFlags);
	virtual void StartWrite(const TAny* aClientBuffer,TInt aLength);
	virtual void WriteCancel();
	virtual void Break(TInt aTime);
	virtual void BreakCancel();
	virtual TInt GetConfig(TDes8& aPackage) const;
	virtual TInt SetConfig(const TDesC8& aPackage);
	virtual TInt SetServerConfig(const TDesC8& aPackage);
	virtual TInt GetServerConfig(TDes8& aPackage);
	virtual TInt GetCaps(TDes8& aPackage);
	virtual TInt GetSignals(TUint& aSignals);
	virtual TInt SetSignalsToMark(TUint aSignals);
	virtual TInt SetSignalsToSpace(TUint aSignals);
	virtual TInt GetReceiveBufferLength(TInt& aLength) const;
	virtual TInt SetReceiveBufferLength(TInt aLength);
    virtual ~CBTPort();
	virtual void Destruct();
	virtual void FreeMemory();
	// extra functions introduced by v6 c32
	virtual void NotifySignalChange(TUint aSignalMask);
	virtual void NotifySignalChangeCancel();
	virtual void NotifyConfigChange();
	virtual void NotifyConfigChangeCancel();
	virtual void NotifyFlowControlChange();
	virtual void NotifyFlowControlChangeCancel();
	virtual void NotifyBreak();
	virtual void NotifyBreakCancel();
	virtual void NotifyDataAvailable();
	virtual void NotifyDataAvailableCancel();
	virtual void NotifyOutputEmpty();
	virtual void NotifyOutputEmptyCancel();
	virtual TInt GetFlowControlStatus(TFlowControl& aFlowControl);
	virtual TInt GetRole(TCommRole& aRole);
	virtual TInt SetRole(TCommRole aRole);
	//
private:
    CBTPort();
	void ConstructL(TUint aUnit,CBTPortStateFactory *aFactory);
	void DestructNow(); //called by CBTPortproxy to signal shutdown
private:
	TCommRole			iRole;
	TCommConfigV01		iConfig;
	TCommCapsV02		iCaps;
	CBTPortProxy		*iPortProxy;
	};

NONSHARABLE_CLASS(TReadTerminator)
	{
public:
	TInt	iTerminatorCount; //Number of terminator characters
	TText8	iTerminatorChars[KConfigMaxTerminators];
	TInt    iNextCharToFindIdx; // index in the array of the next char in the sequence
								// to search for
	};

class CBTTimerSimple;
class CBTPortBuffer;
class TBTPortState;
class CSockServConnector;
NONSHARABLE_CLASS(CBTPortProxy) : public CActive
// Context class in state pattern.
// Friendship is extended to this class so that it 
// can see the CPort instance private data.
	{
	friend class CBTPort;
	friend class CBTPortLocker;
	friend class CBTPortWriter;
	friend class CBTPortReader;
	friend class CBTTimerSimple;
	friend class TBTPortDefaultState;
	friend class TBTPortCommonBaseState;
	friend class TBTPortStateIdle;
	friend class TBTPortStateLoadingProtocol;
	friend class TBTPortStateDiscovering;
	friend class TBTPortStateSDPConnected;
	friend class TBTPortStateSDPServiceQuery;
	friend class TBTPortStateSDPAttributeListRetrieved;
	friend class TBTPortStateConnectionSecuritySetup;
	friend class TBTPortStateConnecting;
	friend class TBTPortStateOpen;
	friend class TBTPortStateClosing;
	friend class TBTPortErrorState;
	friend class TBTPortStateServiceIDListRetrieved;
public:
	static CBTPortProxy* NewL(TUint32 aPortNo, CBTPort *aParent,CBTPortStateFactory *aFactory);
	~CBTPortProxy();
	inline void SetState(TBTPortState& aState){ iState=&aState;};
	void MoveToErrorState();
	// from CActive
	virtual void RunL();
	virtual void DoCancel();
	// interface of proxy
	void Read(const TAny* aClientBuffer,TInt aLength);
	void ReadCancel();
	void Write(const TAny* aClientBuffer,TInt aLength);
	void WriteCancel();
	TInt QueryReceiveBuffer(TInt &aLength);
	TInt GetReceiveBufferLength(TInt &aLength);
	void ResetRxBuffer();
	void Close();
private:
	CBTPortProxy(TUint32 aPortNo);
	void InitL(CBTPort *aParent,CBTPortStateFactory *aFactory);
	void DestructContext();
	// context locker AO
	void StartLocker();
	void DoLockedAction();
	TBool IsLockerOn();
	void StopLocker();
	// context writer AO
	void StartWriter();
	void DoWriteCompleted(TInt aError);
	void StopWriter();
	// context reader AO
	void StartReader();
	void DoReadCompleted(TInt aError);
	void StopReader();
	// To be called by the active friendly states
	void DoWriteCancel();
	// void DoReadCancel(); not needed since read cancels are not propagated to the underlying socket
	void StartShutdownTimerL();
	void CancelShutDownTimer();
	void ShutdownAlarm();
	// To be called by the Open state only
	// void SetReadCancelPending(); not needed since read cancels are not propagated to the underlying socket
	void SetWriteCancelPending();
	TBool IsWriteCancelPending();
	// TBool IsReadCancelPending(); not needed since read cancels are not propagated to the underlying socket
	TBool ReadInBufferLowWatermarkReached();
	TBool ReadInBufferHighWatermarkReached();
	// To be called by Closing and SDP Discovering states only
	TBool IsNetDbInUse();
	void SetNetDbInUse();
	void SetNetDbNotInUse();
private:
	TInt					iLastError;
	TBTPortState*			iState;
	CBTPort*				iPort;
	TUint32					iPortNo;
	CBTPortStateFactory*	iPortStateFactory;
    CBTPortLocker*			iPortLocker;
	CBTPortReader*			iPortReader;
	CBTPortWriter*			iPortWriter;
	CSockServConnector*		iSockServConnector;
	RSocketServ				iSockServ;
	RSocket					iSocket;
	CBTPortBuffer*			iCircularReadBuf;
	HBufC8*					iSendBuf;
	TPtr8					iSendBufPtr;
	HBufC8*					iReadBuf;
	TPtr8					iReadBufPtr;

							// temp FIX
	HBufC8*					iReadOutBuf; // to be used for reading from the circular buf to the client
	
	TBool					iMoreSendsToCome;
	TAny*					iClientWritePtr;
	TInt					iClientWriteLength;
	TInt					iClientLengthWrittenSoFar;
	TAny*					iClientReadPtr;
	TInt					iClientReadLength;
	TInt					iClientRemainderToRead;
	TInt					iClientWriteOffset;
	TBool					iClientReadOneOrMore;
	RNetDatabase			iNetDatabase;
	TNameEntry				iLog;
	TUint					iRemoteDevAddr;
	TBool					iTerminatedReads;
	TReadTerminator			iTerm;
	TInt					iReadTerminatorOffset;

	// TBool					iReadCancelationPending; not needed because the read cancel is not propagated to the underlying socket
	TBool					iWriteCancelationPending;
	TRfcommSockAddr			iAddr;
	TBuf8<512>				iSDPResult;
	TBuf8<512>				iSDPServRecordHandle;
	TBuf8<255>				iSDPRequest;
	TUint8					iRemoteRfcommPortNumber;
	TBTDevAddr				iBdaddr;
	TBool					iNetDbInUse;

	RBTRegServ				iRegServ;
	RBTCommPortSettings		iPortSettings;
	TBTCommPortSettings   	iDefaultService;
	CBTTimerSimple*			iShutdownTimer;

#ifdef _DEBUG
	TInt					iReadsPending; //for debugging calls to the reader
#endif
};

#endif
