// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the avdtp transport session base class
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TRANSPORTSESSION_H
#define TRANSPORTSESSION_H

#include <es_mbuf.h>
#include <bt_sock.h>
#include <bluetoothav.h>
#include "avdtpAllocators.h"

class CUserPlaneTransportSession;
class CTransportSession;
class CTransportChannel;
class CAvdtpProtocol;
class CAvdtpSAP;
class CAVStream;

NONSHARABLE_STRUCT(TTransportBinding)
	{
	CUserPlaneTransportSession* iSession;
	CTransportChannel* iChannel;
	};


// starting off life as RMBufChain - move to RAvdtpMessage later..
// but actually it's only muxchannel that needs to do stuff!
// so might be best for the pool to reatin just mbufchains and
// for muxchannel to prepend mux data later
static const TUint KDefaultPacketPool = 8;

NONSHARABLE_CLASS(RPacketPool)
	{
public:
	RPacketPool();
	TInt Create(TUint aSize, TBool aBalk);
	TInt Add(RMBufChain& aChain, TInt& aWrappedChainSize);
	TInt Remove(RMBufChain& aReturnedMBufChain);
	TInt SetCeiling(TInt aCeiling);
	void Close();
	TInt Resize(TUint aSize);
	inline TBool CanBalk() const;
	inline TInt Count() const;
	inline void SetBalking(TBool aNewBalkSetting);
private:
	inline TInt OldestIndex() const;
	TInt AllocatePool(TUint aSize);
	void AllocatePoolL(TUint aSize);
	void CreateEntryL();
	void DestroyEntry(TUint aIndex);
	
private:
	RArray<RMBufChain>		iPool;
	TUint					iNewestIndex; // the position at which last inbound chain assigned
	TUint					iOldestIndex;
	TBool					iBalk;	// if false, wrapping occurs
	};

inline TBool RPacketPool:: CanBalk() const
	{
	return iBalk;
	}
	
inline TInt RPacketPool::Count() const
	{
	return iPool.Count();
	}
	
inline void RPacketPool::SetBalking(TBool aNewBalkSetting)
	{
	iBalk = aNewBalkSetting;
	}

NONSHARABLE_CLASS(CTransportSession) : public CBase
/**
	Intended for derivation by all stream bearers
	Where there is clear generality the base class provides implementations
	However this class can't begin to think what to do on some downcalls so
	it mandates that derivers implement these
	
*/
	{
public:
	virtual TInt ActiveOpen()=0;
	virtual TInt SetOption(TUint aLevel, TUint aName, const TDesC8 &aOption);
	virtual TInt GetOption(TUint aLevel, TUint aName, TDes8& aOption) const;
	virtual void Ioctl(TUint aLevel, TUint aName, const TDesC8* aOption);
	virtual void CancelIoctl(TUint aLevel, TUint aName);
	virtual void Start();
	virtual void Shutdown();
	virtual void FastShutdown();
	virtual TInt Send(RMBufChain& aData, TUint aOptions, TSockAddr* aAddr)=0;
	virtual TInt GetData(RMBufChain& aData)=0;
	void Ready(const TBTDevAddr& aRemoteAddr); //addr passed cos possibly passive
	inline TBTDevAddr RemoteAddress() const;
	inline TAvdtpTransportSessionType SessionType() const;
	inline void SetRemoteAddress(const TBTDevAddr& aSockAddr);
	
	void LocalName(TAvdtpSockAddr& aAddr) const;
	void RemName(TAvdtpSockAddr& aAddr) const;
	TInt SetLocalName(TAvdtpSockAddr& aAddr);
	TInt SetRemoteName(TAvdtpSockAddr& aAddr);
	~CTransportSession();
	
protected:
	CTransportSession(CAvdtpProtocol& aProtocol,
					  CAvdtpSAP& aSAP,
					  TAvdtpTransportSessionType aType);
	void ConstructL();
	inline TSEID SEID() const;
	inline void SetSEID(TSEID aSEID);
protected:
	CAvdtpSAP&			iSAP; // our (parent) SAP
	CAvdtpProtocol&		iProtocol;
protected:
	enum TAvdtpSAPShutdownReceived
		{
		ENone,
		ENormal,
		EImmediate,
		};
	virtual void DoShutdown()=0;
	TAvdtpSAPShutdownReceived	iSAPShutdown;
private:
	// private to stop derivers playing around with session type later
	TAvdtpSockAddr		iAddress;
	};
	

NONSHARABLE_CLASS(CUserPlaneTransportSession) : public CTransportSession 
	{
public:
	inline TTSID TSID() const;
	inline TSEID SEID() const;
	void ChannelError(TInt aError);	// informs stream, which may call back into error
	virtual void StreamError(TInt aError);	// default just passes to sap
	virtual void StreamReleased();
	virtual TInt Send(RMBufChain& aData, TUint aOptions, TSockAddr* aAddr)=0;
	virtual TInt GetData(RMBufChain& aData);
	virtual void CanSend()=0;
	virtual void ReceivePacketLost();
	
//	void NewData(TUint aCount);
	void NewData(RMBufChain& aChain);
	virtual TInt ActiveOpen();
protected:	
	CUserPlaneTransportSession(CAvdtpProtocol& aProtocol,
					  CAvdtpSAP& aSAP,
					  TAvdtpTransportSessionType aType,
					  CAVStream& aStream);
	void ConstructL();
	~CUserPlaneTransportSession();
	void LeaveTransportChannel();
	
	virtual TInt DoActiveOpen()=0;	// could bring this into this class
	virtual void DoShutdown();
protected:
	RTSID				iTSID;
	RPacketPool			iReceivePool;
	RPacketPool			iSendPool;
	CAVStream*			iStream; // the stream we're part of, may become NULL
	CTransportChannel*	iTransportChannel; //non-owned
	TBool				iTransportChannelBlocked;
	TInt				iRemainingDataInChannel;
	};


inline TAvdtpTransportSessionType CTransportSession::SessionType() const
	{
	return iAddress.Session();
	}
	

inline TSEID CTransportSession::SEID() const
	{
	return iAddress.SEID();
	}

inline void CTransportSession::SetSEID(TSEID aSEID)
	{
	iAddress.SetSEID(aSEID);
	}

inline TSEID CUserPlaneTransportSession::SEID() const
	{
	return CTransportSession::SEID();
	}

inline TTSID CUserPlaneTransportSession::TSID() const
	{
	return iTSID.TSID();
	}

inline TBTDevAddr CTransportSession::RemoteAddress() const
	{
	return iAddress.BTAddr();
	}
	
inline void CTransportSession::SetRemoteAddress(const TBTDevAddr& aDevAddr)
	{
	iAddress.SetBTAddr(aDevAddr);
	}
	
#endif //TRANSPORTSESSION_H
