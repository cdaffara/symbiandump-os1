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

#if !defined(__SS_SOCK_H__)
#define __SS_SOCK_H__

#include <comms-infras/ss_intsock.h>

#define SYMBIAN_NETWORKING_UPS

#ifdef SYMBIAN_NETWORKING_UPS
#include <comms-infras/ss_platsec_apiext.h>		// ASockSubSessionPlatsecApiExt
#endif //SYMBIAN_NETWORKING_UPS

//#include <es_prot.h>
#include <es_enum.h>
#include <e32base.h>
#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include "ss_flowrequest.h"

class CWaitForMBufs;
class CProtocolBase;
class ProtocolManager;
class CInternalSocketImpl;

namespace ESock
{

NONSHARABLE_CLASS(CSocketMessage) : public CBase, public AMessage 
	{
	public:
	virtual void AcquireMessage ( AMessage* aMessage );
	inline void SetMessage ( Den::RSafeMessage& aMessage );
	inline void ProcessedMessage ();


	private:
    virtual TInt ReadDes(TInt aSrcParamIndex,TDes8 &aDes,TInt anOffset=0);
	virtual TInt ReadInt(TInt aSrcParamIndex);
	virtual TInt ReadMBuf(TInt aSrcParamIndex, RMBufChain& aBufChain);
	virtual void InitMBuf(TInt aParamIndex);
    virtual TInt WriteDes(TInt aDstParamIndex,const TDesC8& aDes,TInt anOffset=0);    
    virtual TInt WriteMBuf(TInt aDstParamIndex,RMBufChain& aBufChain);    

	virtual void CompleteMessage(TInt anError);

	virtual TBool IsNull (TInt aParamIndex);

#ifdef SYMBIAN_NETWORKING_UPS
    TInt GetProcessAndThreadId(TProcessId& /*aProcessId*/, TThreadId& /*aThreadId*/) const;
	inline const Den::RSafeMessage& Message() const;
#endif	
	
	inline Den::RSafeMessage& Message ();
	private:
		Den::RSafeMessage iMessage;   // Not owned
	};

NONSHARABLE_CLASS(CSocket) : public CSockSubSession, public ASocket
/**
Represents client-facing part of socket.
Implements ESOCK client socket sub-session specific handling.
Deals with RMessage2, player and session properties
@see ASocket
@internalTechnology
*/
	{
	friend class ::ProtocolManager;
	friend class ::Nif;
	friend class ::CInternalSocketImpl;	// need access to TSocketState enum - for esock "internal sockets"
	typedef CSockSubSession inherited;

public:
	static CSocket* NewLC(TServerProtocolDesc* aServiceInfo, CSockSession* aSession, CPlayer* aPlayer, CProtocolBase* aProt, const TSubSessionUniqueId aSubSessionUniqueId, TInt aSocketType);
	virtual ~CSocket();
	virtual Den::TSubSessInfo Type() const
		{
		return TCFSubSessInfo(TCFSubSessInfo::ESocket);
		}

	virtual const Messages::RNodeInterface* ServiceProvider() const;

	// Request service routines.
	void ConnectL();
	void ShutdownL();
	TBool CloseSocket();
	void ListenL();

	void SetSockOptionL();
	void GetSockOptionL();
	void IoctlL();
	void ReferenceL();

	virtual void ProcessMessageL();

	virtual void InitiateDestruction();
	
	void AdoptFlowRequestMessage(const RMessage2& aFlowRequestMessage)
		{
		iFlowRequestMessage.Adopt(static_cast<Den::RSafeMessage&>(const_cast<RMessage2&>(aFlowRequestMessage)));
		}
	void CompleteFlowRequestMessage(TInt err);

private:
   	CSocket(CSockSession* aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId, TInt aSocketType);


	virtual void SetClosing();
	virtual TBool IsClosing();

	virtual void DontCompleteCurrentRequest();

	virtual ASocket* InitiateAcceptingSocket();
    virtual ASocket* GetAcceptingSocket();
  
	virtual void PanicSocketClient(TESockPanic aPanic);
	virtual void SetReturn(TInt aReturnValue) const;

	virtual void GetOwnerInfo(TProcessId& aProcId, TSoOwnerInfo& aInfo, TThreadId& aThreadId);
    virtual TInt SecurityCheck();

	void FinalCompleteAllBlockedMessages(TInt aResult);

	void ConstructL (CProtocolBase* aProtocol);
	void InitUserMessageL ( TSocketMessage aMessage );

	virtual TDes8* BorrowTemporaryBuffer(TInt aSize);
    virtual TDes8* BorrowTemporaryBufferL(TInt aSize);
private:
	Den::RSafeMessage iFlowRequestMessage;	
    };

// ---------------------------------------------
inline void CSocketMessage::SetMessage ( Den::RSafeMessage& aMessage )
	{
	iMessage.Duplicate(aMessage);
	}

inline void CSocketMessage::ProcessedMessage ()
    {
    // Resets the handle to reflect the fact that the message has been processed
    iMessage.ResetHandle();
    }
inline Den::RSafeMessage& CSocketMessage::Message ()
	{
	return iMessage;
	}

inline const Den::RSafeMessage& CSocketMessage::Message() const
	{
	return iMessage;
	}

}  //namespace ESock

#endif
// __SS_SOCK_H__
