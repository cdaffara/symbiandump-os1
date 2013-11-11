// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// eintsockimpl.inl
// @file
// @internalTechnology
// 
//


inline TInt CInternalSocketImpl::Return () const
	{
	return iReturn;		
	}


inline CommsFW::TWorkerId CInternalSockSubSession::ClientWorkerId() const
	{
	return iClientWorkerId;
	}

inline CommsFW::TWorkerId CInternalSockSubSession::ServerWorkerId() const
	{
	return iServerCookie.Thread();
	}

inline CommsFW::MCommsTransportSender* CInternalSockSubSession::Sender() const
	{
	return iSender;
	}

inline TInt CInternalSockSubSession::SendReceiveMsg(Den::TWorkerTransportMsg& aMsg)
	{
	return SendReceiveMsg(aMsg, Sender(), iServerCookie);
	}

inline void CInternalSockSubSession::SendMsg(Den::TWorkerTransportMsg& aMsg)
	{
	SendMsg(aMsg, Sender(), iServerCookie);
	}


inline void CInternalSockSubSession::SetSender(CommsFW::MCommsTransportSender* aSender)
	{
	iSender = aSender;
	}

inline TInt CInternalSockSubSession::CheckPolicy(const TSecurityPolicy& /*aPolicy*/, const char* /*aDiagnostic*/)
	{
	// With our current architecture we cannot meaningfully support security checking: protocols are by design unaware
	// of client identity and so unable validate such requests from another protocol. In principle the request could
	// get passed all the way back to the real CSocket (presuming that it ever corresponded to a client request) but 
	// the protocol would have to preserve the origin of data through to its bottom layer, eg the NIF for the IP stack,
	// which is currently done
	return KErrNone;
	}

inline CInternalSockSubSession::TGlobals* CInternalSockSubSession::Globals(TTransportUserStorage& aStorage)
	{
	return reinterpret_cast<TGlobals*>(aStorage.Ptr());
	}

inline TInt CInternalSockSubSession::LinkOffset()
	{
	return _FOFF(CInternalSockSubSession, iLink);
	}

inline void CIntSocketMessage::SetMessage ( TEIntSockOpMsgWrapper& aMessage )
	{
	iMessage = aMessage;		
	}

inline TEIntSockOpMsgWrapper& CIntSocketMessage::Message ()
	{
	return iMessage;	
	}


