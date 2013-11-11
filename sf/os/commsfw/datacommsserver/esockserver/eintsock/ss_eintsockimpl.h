// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// eintsockimpl.h
// 
//

/**
 @file
 @internalTechnology 
*/

#ifndef __EINTSOCKIMPL_H__
#define __EINTSOCKIMPL_H__

#include <e32std.h>
#include <es_prot.h>
#include <ss_std.h>
#include <es_mbuf.h>
#include "ss_eintsock_msgs.h"

#include <comms-infras/ss_intsock.h>

class CInternalServiceResolver;	// not implemented as no-one wants it at the moment
class CInternalConnection;		// not implemented, as too dodgy to bypass multiplexing in esock without some serious thought

class RInternalSocket;
class TSockXferLength;
class TTransportUserStorage;

using namespace ESock;

// This is not really a C type class. 
NONSHARABLE_CLASS(CIntSocketMessage) : public CBase, public AMessage
	{
	public:
		virtual void AcquireMessage ( AMessage* aMessage );
		inline void SetMessage ( TEIntSockOpMsgWrapper& aMessage );
		
	private:
		virtual TInt ReadDes(TInt aSrcParamIndex,TDes8 &aDes,TInt anOffset=0 );
		virtual TInt ReadInt(TInt aSrcParamIndex);
		virtual TInt ReadMBuf(TInt aSrcParamIndex, RMBufChain& aBufChain);
		virtual void InitMBuf(TInt aParamIndex);
		virtual TInt WriteDes(TInt aDstParamIndex,const TDesC8& aDes,TInt anOffset=0);    
		virtual TInt WriteMBuf(TInt aDstParamIndex,RMBufChain& aBufChain);    

		virtual void CompleteMessage(TInt anError);
		virtual TBool IsNull (TInt aParamIndex);
		
		inline TEIntSockOpMsgWrapper& Message ();

	private:
		TEIntSockOpMsgWrapper iMessage;
	};


// A simple default security checker
class TDefaultSecurityChecker : public MProvdSecurityChecker
	{
	public:
		TInt CheckPolicy (const TSecurityPolicy& /*aPolicy*/, const char * /*aDiagnostic*/)
	      {
	      return KErrNone;
	      }
	};

// This class is needed for synchronous open
NONSHARABLE_CLASS(CAsyncWaiter) : public CActive
	{
	public:
		static CAsyncWaiter* New ();
		~CAsyncWaiter ();
		
		TInt StartAndWait ();
		
	protected:
		CAsyncWaiter ();
		
		// CActive
		void RunL ();
		void DoCancel ();		
	};


NONSHARABLE_CLASS(CInternalSockSubSession) : public CBase, public MProvdSecurityChecker
	{
public:
	class TParameters
		{
	public:
		TParameters(CommsFW::TWorkerId aClientWorkerId, CommsFW::MCommsTransportSender* aSender, TTransportUserStorage& aStorage, const Messages::TNodeId& aServerCookie, TRequestWrapper* aRequest /*TWorkerId aClientWorkerId, CommsFW::TWorkerId aServerWorkerId*/);
	public:
		TTransportUserStorage& iStorage;
		CommsFW::TWorkerId iClientWorkerId;
		Messages::TNodeId iServerCookie;

        // Used by the client for subsequent calls. Presumes client remains in same thread as when opened
        // RInternalSocket must be Transfer()'d if client thread changes
        CommsFW::MCommsTransportSender* iSender;
        TRequestWrapper* 			iRequest;
		};

	IMPORT_C static void ProcessMessage(const Den::TWorkerTransportMsg& aMsg, TRequestWrapper& aRequest, TTransportUserStorage& aStorage, CommsFW::TWorkerId aPeerWorkerId);
	IMPORT_C static void OnPeerDeath(CommsFW::TWorkerId aPeer, TTransportUserStorage& aStorage);
	IMPORT_C static void Shutdown(TBool aAlreadyDead, TTransportUserStorage& aStorage);

	inline CommsFW::TWorkerId ClientWorkerId() const;
	inline CommsFW::TWorkerId ServerWorkerId() const;
	CommsFW::MCommsTransportSender* Sender() const;
	void SetSender(CommsFW::MCommsTransportSender* aSender);

	inline TInt SendReceiveMsg(Den::TWorkerTransportMsg& aMsg);
	inline void SendMsg(Den::TWorkerTransportMsg& aMsg);
	static TInt SendReceiveMsg(Den::TWorkerTransportMsg& aMsg, CommsFW::MCommsTransportSender* aSender, const Messages::TNodeId& aServerCookie);
	static void SendMsg(Den::TWorkerTransportMsg& aMsg, CommsFW::MCommsTransportSender* aSender, const Messages::TNodeId& aServerCookie);
	static TInt SendReceiveMsgAsync( Den::TWorkerTransportMsg& aMsg, CommsFW::MCommsTransportSender* aSender, const Messages::TNodeId& aServerCookie );
	
	virtual void FinalCompleteAllBlockedMessages(TInt aResult) = 0;

	virtual TInt CheckPolicy(const TSecurityPolicy& /*aPolicy*/, const char* /*aDiagnostic*/);
	static TInt LinkOffset();
protected:
	typedef TDblQue<CInternalSockSubSession> TSubSessions;
	class TGlobals
		{
	public:
		TGlobals();
		TSubSessions iSubSessions;
		};

	CInternalSockSubSession(const TParameters& aParams);
	void ConstructL(const TParameters& aParams);

	static TGlobals* Globals(TTransportUserStorage& aStorage);

	static TSubSessions& SubSessions(TTransportUserStorage& aStorage);
	static void AddSubsession(TTransportUserStorage& aStorage, CInternalSockSubSession* aSubSession);
	static void ProcessMessageL(const Den::TWorkerTransportMsg& aMsg, TRequestWrapper& aRequest, TTransportUserStorage& aStorage, CommsFW::TWorkerId aPeerWorkerId);
	static TInt PerformOperationL(const TEIntSockMsg& aMsg, TRequestWrapper& aRequest);
	
private:
	TInt iAccessCount;
	CommsFW::TWorkerId iClientWorkerId;
//	TWorkerId iServerWorkerId;
	Messages::TNodeId iServerCookie;
	CommsFW::MCommsTransportSender* iSender;	// presumes client remains in same thread as when opened - safe for now
protected:
	TDblQueLink iLink;
	CWorkerThread* iOwnerThread;
	};


NONSHARABLE_CLASS(CInternalSocketImpl) : public CInternalSockSubSession, public ASocket
/**
Implementation of RInternalSocket
*/
	{
	typedef CInternalSockSubSession inherited;
	friend class CInternalSockSubSession;
	friend class ProtocolManager;
private:
	// Various bits to create a new internal socket in different ways	
	static CInternalSocketImpl* NewL ( const CInternalSockSubSession::TParameters& aParams );
	static CInternalSocketImpl* NewL ( const CInternalSockSubSession::TParameters& aParams, TServerProtocolDesc* aServiceInfo, CProtocolBase* aProt );
	~CInternalSocketImpl();

	virtual void FinalCompleteAllBlockedMessages(TInt aResult);
	
	void SetImpl ( CInternalSocketImpl*& aImpl );

	virtual TDes8* BorrowTemporaryBuffer(TInt aSize);
    virtual TDes8* BorrowTemporaryBufferL(TInt aSize);
private:
	CInternalSocketImpl(const CInternalSockSubSession::TParameters& aParams, TUint aSockType);
	void ConstructL(const TParameters& aParams, TServerProtocolDesc* aServiceInfo, CProtocolBase* aProt);
	void InitUserMessageL ( TSocketMessage aMessage );
	
	void SendL ( TSockMess aWriteFunction, TEIntSockOpMsgWrapper& aMsg );
	void RecvL ( TSockMess aReadFunction, TEIntSockOpMsgWrapper& aMsg );	
	void CancelOpen ();
	
	// From ASocket
	// ----------------
	virtual void SetClosing();
	virtual TBool IsClosing();

	virtual void DontCompleteCurrentRequest();
	
	virtual ASocket* InitiateAcceptingSocket();
    virtual ASocket* GetAcceptingSocket();
    
	virtual void SetReturn(TInt aReturnValue) const;

	virtual void GetOwnerInfo(TProcessId& aProcId, TSoOwnerInfo& aInfo, TThreadId& aThreadId);
    virtual TInt SecurityCheck();
	
    virtual void CompleteFlowRequestMessage(TInt aErr);
    
    virtual void InitiateDestruction();
    virtual TBool CloseSocket();
    virtual void PanicSocketClient(TESockPanic aPanic);
	// --------------
	
	TEIntSockOpMsgWrapper& GetMessage (TSocketMessage aMessage);
	TBool ShouldCompleteCurrentRequest () const;
	inline TInt Return () const;
	void AdoptFlowRequest ( TRequestWrapper& aRequest );
	void SetCurrentMessage ( TEIntSockOpMsgWrapper& aIntSockMsg );	
	void CompleteCurrentRequest ( TBool aValue );
	
private:
	TBool 				iComplete:1;
	TBool				iClosing:1;
	mutable TInt		iReturn;
	
	TRequestWrapper		iFlowRequest;
	CInternalSocketImpl** iImpl; // This will hold the reference to the pointer that RInternalSocket holds.
								 // It will be set when the the internal socket is actuall created. in case of
								 // an error it will be NULL and socket will do a self deletion. 
	CProtocolBase* iProtocol;
	};

#include "ss_eintsockimpl.inl"

#endif // __EINTSOCKIMPL_H__

