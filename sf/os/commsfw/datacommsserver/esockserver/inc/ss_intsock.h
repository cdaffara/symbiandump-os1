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
//

#if !defined(__SS_INTSOCK_H__)
#define __SS_INTSOCK_H__

#define SYMBIAN_NETWORKING_UPS

#include <es_enum.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_internal.h>
#endif

#include <e32base.h>
#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include "ss_flowrequest.h"
#include <es_mbufif.h>

#ifdef SYMBIAN_NETWORKING_UPS
#include <comms-infras/ss_platsec_apiext.h>		// ASockSubSessionPlatsecApiExt
#endif //SYMBIAN_NETWORKING_UPS

class CWaitForMBufs;
class CProtocolBase;
class ProtocolManager;
class CInternalSocketImpl;

namespace ESock
{

class AMessage
{
public:
	virtual void AcquireMessage ( AMessage* aMessage ) =0;

    virtual TInt ReadDes(TInt aSrcParamIndex,TDes8 &aDes,TInt anOffset=0) = 0;
	virtual TInt ReadInt(TInt aSrcParamIndex) = 0;
	virtual TInt ReadMBuf(TInt aSrcParamIndex, RMBufChain& aBufChain) =0;
	virtual void InitMBuf(TInt aParamIndex) =0;
    virtual TInt WriteDes(TInt aDstParamIndex,const TDesC8& aDes,TInt anOffset=0) = 0;
    virtual TInt WriteMBuf(TInt aDstParamIndex,RMBufChain& aBufChain) = 0;

	virtual void CompleteMessage(TInt anError) = 0;
	virtual TBool IsNull (TInt aParamIndex) =0;

#ifdef SYMBIAN_NETWORKING_UPS
    virtual TInt GetProcessAndThreadId(TProcessId& /*aProcessId*/, TThreadId& /*aThreadId*/) const;
#endif	
		
	virtual ~AMessage() {}
};

NONSHARABLE_CLASS(ASocket) : public Messages::ASimpleNodeIdBase,
                             public MSessionControlNotify, public MSessionDataNotify,
                             public AIPCFlowRequester
#ifdef SYMBIAN_NETWORKING_UPS
                             , private ASockSubSessionPlatsecApiExt
#endif
/**
Represents binder-facing part of socket.
Implements ESOCK client socket binder and flow specific handling.
It's meant to be agregated with the apropriet client-facing part to implement
full socket functionality
@see CSocket
@internalTechnology
*/
	{
	friend class ::ProtocolManager;

protected:
	enum TSocketState
		{
		ESStateNull=-1,
		ESStateCreated=0,
		ESStateOpeningActive,
		ESStateOpeningPassive,
		ESStateOpen,
		ESStateConnected,
		ESStateDisconnected,
		ESStateClosing,
		ESStateShuttingDown,
		ESStateDead,
		ESStateError,
		ESStateAccepting,
		ESStateBinding
		};

protected:
    //Messages::ANode
	virtual void ReceivedL(
		const Messages::TRuntimeCtxId& aSender,
		const Messages::TNodeId& aRecipient,
		Messages::TSignatureBase& aMessage
		);
	//messages
	void BindToL(const TCFDataClient::TBindTo& aBindTo);

public:
	virtual ~ASocket();

	virtual const Messages::RNodeInterface* ServiceProvider() const;
	TBool GetFlowAndSCPR(Messages::TNodeId& aFlow, Messages::TNodeId& aSCPR) const;

	TInt GetConnectionSocketInfo(TConnectionSocketInfo& aInfo) const;

	// Request service routines.
	void ConnectL(TBool aConnectData);
	void ShutdownL(RSocket::TShutdown aType, TBool aDisconnectData);
	virtual TBool CloseSocket();
	void ListenL(TInt aQlen, TBool aConnectionData);
	void Accept();

	void AutoBind();
	void BindL();
	void RemoteNameL();
	void LocalNameL();
	void SetLocalName();

	void SetSockOptionL(TInt aOptionName, TInt aOptionLength, TInt aOptionLevel);
	void GetSockOptionL(TInt aOptionName, TInt aOptionLength, TInt aOptionLevel, TInt aWriteBack);
	void IoctlL(TInt aOptionName, TInt aOptionLength, TInt aOptionLevel, TBool aReadOption);
	void GetDisconnectDataL( ) const;
//
	void SendL(TInt aXferLenArg, TInt aAddrArg, TInt aSendByteCount, TInt aSendFlags, TBool aUseMBufs);
	void RecvL(TInt aXferLenArg, TInt aAddrArg, TInt aReadByteCount, TInt aReadFlags, TBool aUseMBufs, TInt aRecvOneOrMore);
//
	inline void CancelSend();
	inline void CancelRecv();
	void CancelConnect();
	void CancelAccept();
	void CancelIoctl();
	void CancelAll();

	void GetInfoL();

	void CommunicateOwner();

	TPtr8 IoctlBuffer();

	//	SSP Upcalls - from MSessionControlNotify
	void NewData(TUint aCount);
    void CanSend();
    void ConnectComplete();
	void ConnectComplete(const TDesC8& aConnectData);
    void ConnectComplete(CSubConnectionFlowBase& anSSP);
	void ConnectComplete(CSubConnectionFlowBase& anSSP,const TDesC8& aConnectData);
	void CanClose(MSessionControlNotify::TDelete aDelete=MSessionControlNotify::EDelete);
    void CanClose(const TDesC8& aDisconnectData,MSessionControlNotify::TDelete aDelete=MSessionControlNotify::EDelete);
	TInt Error(TInt anError,TUint anOperationMask);
	void Disconnect( );
	void Disconnect(TDesC8& aDisconnectData);
	void IoctlComplete(TDesC8* aBuf);
    void SetLocalNameComplete();
	void DisconnectFromListener(CSubConnectionFlowBase& aSSP);

	// Utitlity functions for testing state and capabilities
	inline TUint IsConnectionOriented() const;
	inline TUint IsConnectionLess()const;
	inline TUint IsDataGram() const;
	inline TUint IsStream() const;
	inline TUint IsPseudoStream() const;
	inline TUint IsReliable() const;
	inline TUint DeliversInOrder() const;
	inline TUint IsMessageBased() const;
	inline TUint CanSendUrgentData() const;
	inline TUint CanSendConnectData() const;
	inline TUint CanSendDisconnectData() const;
	inline TUint CanBroadcast() const;
	inline TUint SupportsMultiPoint() const;
	inline TUint SupportsQOS() const;
	inline TUint IsWriteOnly() const;
	inline TUint IsReadOnly() const;
	inline TUint SupportsGracefulClose() const;
	inline TUint CanReconnect() const;
	inline TUint SupportsPeek() const;
	inline TUint RequiresOwnerInfo() const;
	inline TUint StateCanProcessData() const;

	virtual void InitiateDestruction();
	inline void SetFlowRequestPending(TBool aPending);

	enum 
        { 
        KNoXferLen = -1, 
        KNoAddrArg = -1, 
        KWriteNoAddrArg = -2    // for legacy compatibility reasons we tolerate ESoWrite on a non-connected datagram socket  
        };

protected:
   	explicit ASocket(TInt aSocketType);
	void Create(TServerProtocolDesc* aServiceInfo);

	TBool CheckRunningAndSetReturn();
	// Utility functions for handling blocked messages.
	enum EBlockingFlags
	 {EBlockedClose=0x01,
	 EBlockedConnect=0x02,
	 EBlockedIoctl=0x04,
	 EBlockedRead=0x08, 
	 EBlockedWrite=0x10,
	 EWriteFlowedOff=0x20,
	EReadStopped=0x40,
	 EWriteStopped=0x80
	, EBlockedSetLocalName=0x100
						 };

	inline void SetBlockedReadFlag();
	inline void SetBlockedWriteFlag();
	inline void SetBlockedCloseFlag();
	inline void SetBlockedIoctlFlag();
	inline void SetBlockedConnectFlag();

	inline void SetBlockedRead();
	inline void SetBlockedWrite();
	inline void SetBlockedClose();
	inline void SetBlockedIoctl();
	inline void SetBlockedConnect();
	inline void SetBlockedSetLocalName();
	inline TBool IsBlockedSetLocalName() const;
	void CompleteSetLocalName(TInt anErr);

	virtual void SetClosing() =0;

	inline TBool IsBlockedRead() const;
	inline TBool IsBlockedWrite() const;
	inline TBool IsBlockedConnect() const;
	inline TBool IsBlockedClose() const;
	inline TBool IsBlockedIoctl() const;
	virtual TBool IsClosing() =0;

	TBool CompleteRead(TInt anError);
	TBool CompleteWrite(TInt anError);
	void CompleteConnect(TInt anErr);
	TBool CompleteClose(TInt anErr);
	void CompleteIoctl(TInt anErr);

	virtual void DontCompleteCurrentRequest() = 0;
	virtual ASocket* InitiateAcceptingSocket() = 0;
	virtual ASocket* GetAcceptingSocket() = 0;
	virtual void DoCompleteAccept();
	void AcceptSetupL(const ASocket& aParentSocket, CSubConnectionFlowBase& aFlow);

	virtual void PanicSocketClient(TESockPanic aPanic) =0;
	virtual void SetReturn(TInt aReturnValue) const = 0;

	inline TSocketState State() const;
	inline void SetState(TSocketState aState);

	virtual TDes8* BorrowTemporaryBuffer(TInt aSize) = 0;
    virtual TDes8* BorrowTemporaryBufferL(TInt aSize) = 0;

	TInt DrainStreamProtocol(AMessage* aMsg);
	void DoRecv(TBool aInitialRequest);
	TInt FillStreamProtocol(AMessage* aMsg);
	void DoSend(TBool aInitialRequest);

    TUint SelectConditionsReady();
	TBool CheckReadStopped();
	void TryToCompleteSelectIoctl();
	void DoError(TInt aError, TUint aOperationMask, TBool aDeleteInterface = ETrue);
    inline void ClearErrorIfNotFatal();

	TInt RequestAsyncMBufAllocation(TInt aSignalType, TUint aSize);


	enum TSocketMessage
		{
		ESocketCurrentMessage,
		ESocketReadMessage,
		ESocketWriteMessage,
		ESocketCloseMessage,
		ESocketIoCtlMessage,
		ESocketConnectMessage
		,ESocketSetLocalNameMessage

		};

    void ReadParamL(TSocketMessage aMessage, TInt aSrcParamIndex,TDes8 &aDes,TInt anOffset=0);
    void ReadParamL(TSocketMessage aMessage, TInt aSrcParamIndex, RMBufChain& aBufChain);
	void WriteParamL(TSocketMessage aMessage, TInt aDstParamIndex,const TDesC8& aDes,TInt anOffset=0);
	void WriteParamL(TSocketMessage aMessage, TInt aDstParamIndex, RMBufChain& aBufChain);


	AMessage* GetUserMessage( TSocketMessage aMessage ) const;
	void SetUserMessage ( TSocketMessage aMessageType, AMessage* aMessage );

	virtual void GetOwnerInfo(TProcessId& aProcId, TSoOwnerInfo& aInfo, TThreadId& aThreadId) = 0;
    virtual TInt SecurityCheck() = 0;

	virtual void CompleteFlowRequestMessage(TInt err) =0;
	inline TBool FlowRequestPending();

#ifdef SYMBIAN_NETWORKING_UPS
	TInt GetProcessAndThreadId(TProcessId& aProcessId, TThreadId& aThreadId) const;
#endif

protected:
	MSessionControl* iSSP; // NULL-ness well controlled
	AMessage*  iCurrentMsg;
	TSockXfrLength iXferLength;		// used to write transfer lengths back to client; kept in scope with socket to avoid repeated construction cost
private:
	MSessionData* iSSPData;
	MFlowBinderControl* iFlowBinder;
	Messages::RNodeInterface iServiceProvider;

	TProtocolDesc* iProtocolInfo;
    TInt        iSocketType;


	TSocketState iState;
	TInt iDataAvailable;
	TInt iSendOffset;
	TInt iRecOffset;
	TInt iSendByteCount;
	TInt iRecByteCount;
	TInt iSendFlags;
	TInt iRecvFlags;

	TSockAddr iRecvFromAddr;
//
	TUint iBlockedOperations;

	TInt iError;
    TUint iErrorOperationMask;     // stored aOperationMask from last MSessionControlNotify::Error() call


	TUint iOptions;

	enum { KMaxStreamChunk = 32 * 1024 };
//
	TInt iRecBufSize;
	TInt iSendBufSize;
	RMBufChain iSendData;
	HBufC8* iDisconnectData;
	TInt iDisconnectDataError;
	RMBufChain iDatagramTail;
//
	CWaitForMBufs* iAllocAsync;
//
	CCirBuf<TAcceptQEntry>* iAcceptQ;	//lint -esym(613, ASocket::iAcceptQ)	// NULL-ness well controlled
	CCirBuf<TAcceptQEntry>* iNextAcceptQ;	//lint -esym(613, CSocket::iAcceptQ)	// NULL-ness well controlled
	TInt        iSelectFlags;

	TSecurityPolicy iTransferSecPolicy;     // used for security check during ProtocolManager::TransferSocketL()

	TBool iIsBound:1;
	TBool iRecvOneOrMore:1;
    TBool iSecTransferEnabled:1;  // flag, indicating that iTransferSecPolicy is initialized and socket transfer is enabled
	TBool iIsFlowRequestPending:1;
	TBool iSendUseMBufs:1;
	TBool iRecvUseMBufs:1;
	TInt8 iSendXferLenIdx;
	TInt8 iRecvXferLenIdx;
	TInt8 iSendToAddrIdx;
	TInt8 iRecvFromAddrIdx;
    TInt8 iBlockOnPeekReadCnt; // recursion level counter for NewData().

	AMessage* iReadMsg;
	AMessage* iWriteMsg;
	AMessage* iBlockedCloseMsg;
	AMessage* iBlockedConnectMsg;
	AMessage* iBlockedIoctlMsg;
	AMessage* iBlockedSetLocalNameMsg;
	
	TSocketState iNextState; // Next state to enter after binding state exits.
	TBool        iConnectData; // Used to indicate if connect data is present.

	
	RMBufAllocator iAllocator; // To speed up the RMBufChain allocation
	};

inline TUint ASocket::IsConnectionOriented()  const
	{
	return !(iProtocolInfo->iServiceInfo & KSIConnectionLess);
	}

inline TUint ASocket::IsConnectionLess() const
	{
	return (iProtocolInfo->iServiceInfo & KSIConnectionLess);
	}

inline TUint ASocket::IsDataGram() const
	{
	return (iProtocolInfo->iServiceInfo & KSIDatagram);
	}

inline TUint ASocket::IsStream() const
	{
	return (iProtocolInfo->iServiceInfo & KSIStreamBased);
	}

inline TUint ASocket::IsPseudoStream() const
	{
	return (iProtocolInfo->iServiceInfo & KSIPseudoStream);
	}

inline TUint ASocket::IsReliable() const
	{
	return (iProtocolInfo->iServiceInfo & KSIReliable);
	}

inline TUint ASocket::CanReconnect() const
	{
	return (iProtocolInfo->iServiceInfo & KSICanReconnect);
	}

inline TUint ASocket::SupportsPeek() const
	{
	return (iProtocolInfo->iServiceInfo & KSIPeekData);
	}

inline TUint ASocket::DeliversInOrder() const
	{
	return (iProtocolInfo->iServiceInfo & KSIInOrder);
	}

inline TUint ASocket::IsMessageBased() const
	{
	return (iProtocolInfo->iServiceInfo & KSIMessageBased);
	}

inline TUint ASocket::CanSendUrgentData() const
	{
	return (iProtocolInfo->iServiceInfo & KSIUrgentData);
	}

inline TUint ASocket::CanSendConnectData() const
	{
	return (iProtocolInfo->iServiceInfo & KSIConnectData);
	}

inline TUint ASocket::CanSendDisconnectData() const
	{
	return (iProtocolInfo->iServiceInfo & KSIDisconnectData);
	}

inline TUint ASocket::CanBroadcast() const
	{
	return (iProtocolInfo->iServiceInfo & KSIBroadcast);
	}

inline TUint ASocket::SupportsMultiPoint() const
	{
	return (iProtocolInfo->iServiceInfo & KSIMultiPoint);
	}

inline TUint ASocket::SupportsQOS() const
	{
	return (iProtocolInfo->iServiceInfo & KSIQOS);
	}

inline TUint ASocket::IsWriteOnly() const
	{
	return (iProtocolInfo->iServiceInfo & KSIWriteOnly);
	}

inline TUint ASocket::IsReadOnly() const
	{
	return (iProtocolInfo->iServiceInfo & KSIReadOnly);
	}

inline TUint ASocket::SupportsGracefulClose() const
	{
	return (iProtocolInfo->iServiceInfo & KSIGracefulClose);
	}

inline TUint ASocket::RequiresOwnerInfo() const
	{
	return (iProtocolInfo->iServiceInfo & KSIRequiresOwnerInfo);
	}

inline void ASocket::SetBlockedReadFlag()
	{
	__ASSERT_DEBUG(!IsBlockedRead(),Fault(ETwoReads));
	iBlockedOperations|=EBlockedRead;
	}

inline void ASocket::SetBlockedWriteFlag()
	{
	__ASSERT_DEBUG(!IsBlockedWrite(),Fault(ETwoWrites));
	iBlockedOperations|=EBlockedWrite;
	}

inline void ASocket::SetBlockedConnectFlag()
	{
	__ASSERT_DEBUG(!IsBlockedConnect(),Fault(ETwoConnects));
	iBlockedOperations|=EBlockedConnect;
	}

inline void ASocket::SetBlockedCloseFlag()
	{
	__ASSERT_DEBUG(!IsBlockedClose(),Fault(ETwoClose));
	iBlockedOperations|=EBlockedClose;
	}

inline void ASocket::SetBlockedIoctlFlag()
	{
	__ASSERT_DEBUG(!IsBlockedIoctl(),Fault(ETwoIoctl));
	iBlockedOperations|=EBlockedIoctl;
	}
	
inline void ASocket::SetBlockedSetLocalName()
    {
    iBlockedOperations |= EBlockedSetLocalName;
    iBlockedSetLocalNameMsg->AcquireMessage( iCurrentMsg );
    }
    
 inline TBool ASocket::IsBlockedSetLocalName() const
	{
	return iBlockedOperations & EBlockedSetLocalName;
	}


inline void ASocket::SetBlockedRead()
	{
	SetBlockedReadFlag ();
	}

inline void ASocket::SetBlockedWrite()
	{
	SetBlockedWriteFlag ();
	}

inline void ASocket::SetBlockedClose()
	{
	SetBlockedCloseFlag ();
	iBlockedCloseMsg->AcquireMessage(iCurrentMsg);
	}

inline void ASocket::SetBlockedIoctl()
	{
	SetBlockedIoctlFlag ();
	iBlockedIoctlMsg->AcquireMessage(iCurrentMsg);
	}

inline void ASocket::SetBlockedConnect()
	{
	SetBlockedConnectFlag ();
	iBlockedConnectMsg->AcquireMessage(iCurrentMsg);
	}


inline TBool ASocket::IsBlockedClose() const
	{return iBlockedOperations&EBlockedClose;}

inline TBool ASocket::IsBlockedConnect() const
	{return iBlockedOperations&EBlockedConnect;}

inline TBool ASocket::IsBlockedIoctl() const
	{return iBlockedOperations&EBlockedIoctl;}

inline TBool ASocket::IsBlockedRead() const
	{return iBlockedOperations&EBlockedRead;}

inline TBool ASocket::IsBlockedWrite() const
	{return iBlockedOperations&EBlockedWrite;}

void ASocket::CancelSend()
	{ CompleteWrite(KErrCancel); }

void ASocket::CancelRecv()
	{ CompleteRead(KErrCancel); }

inline void ASocket::ClearErrorIfNotFatal()
	{
	if (State() != ESStateError)
		{
		iError = KErrNone;
		}
	};


inline ASocket::TSocketState ASocket::State() const
	{
	return iState;
	}

inline void ASocket::SetState(TSocketState aState)
	{
	iState = aState;
	// If we've requested notification of available MBufs for a Recv or Send
	// operation and now are entering some inappropriate state then we cancel this
	if(iAllocAsync && iState != ESStateConnected && iState != ESStateOpen)
		{
		delete iAllocAsync;
		iAllocAsync = NULL;
		}
	}

inline void ASocket::SetFlowRequestPending(TBool aPending)
	{
	iIsFlowRequestPending = aPending;
	}

inline TBool ASocket::FlowRequestPending()
	{
	return iIsFlowRequestPending;
	}

}  //namespace ESock

#endif
// __SS_INTSOCK_H__

