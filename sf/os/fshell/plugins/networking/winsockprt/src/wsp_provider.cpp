// wsp_provider.cpp
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//


#include "wsp_provider.h"
#include "wsp_factory.h"
#include "wsp_socket.h"
#include "wsp_panic.h"
#include "wsp_log.h"


//
// Constants.
//

const TInt KReceiveBufferSize = 4096;


//
// CWinSockProviderBase.
//

CWinSockProviderBase::CWinSockProviderBase(RWin32Factory& aWin32Factory)
	: iWin32Factory(aWin32Factory), iWinSockWriteBuffer(NULL, 0)
	{
	}

CWinSockProviderBase::~CWinSockProviderBase()
	{
	delete iSendSocketWrapper;
	delete iReceiveSocketWrapper;
	delete iReceiveBuffer;
	delete iSendBuffer;
	iWin32Socket.Close();
	}

void CWinSockProviderBase::ConstructL()
	{
	iSendSocketWrapper = CWin32SocketWrapper::NewL(*this, iWin32Socket);
	iReceiveSocketWrapper = CWin32SocketWrapper::NewL(*this, iWin32Socket);
	iReceiveBuffer = HBufC8::NewL(KReceiveBufferSize);
	iWinSockWriteBuffer.Set(iReceiveBuffer->Des());
	}

TInt CWinSockProviderBase::FillSendBuffer(const TDesC8& aDataToSend)
	{
	if (iSendBuffer)
		{
		if (iSendBuffer->Des().MaxLength() < aDataToSend.Length())
			{
			iSendBuffer = iSendBuffer->ReAlloc(aDataToSend.Length());
			if (iSendBuffer == NULL)
				{
				return KErrNoMemory;
				}
			}
		*iSendBuffer = aDataToSend;
		}
	else
		{
		iSendBuffer = aDataToSend.Alloc();
		if (iSendBuffer == NULL)
			{
			return KErrNoMemory;
			}
		}
	return KErrNone;
	}

TInt CWinSockProviderBase::SetRemName(TSockAddr& anAddr)
	{
	WSP_LOG(WspLog::Write(_L("CWinSockProviderBase::SetRemName")));
	iRemoteAddr = anAddr;
	return KErrNone;
	}

void CWinSockProviderBase::LocalName(TSockAddr& anAddr)const
	{
	WSP_LOG(WspLog::Write(_L("CWinSockProviderBase::LocalName")));
	iWin32Socket.GetSocketName(static_cast<TInetAddr&>(anAddr)); // Throw away error code.
	}

TInt CWinSockProviderBase::SetLocalName(TSockAddr& anAddr)
	{
	WSP_LOG(WspLog::Write(_L("CWinSockProviderBase::SetLocalName")));
	return iWin32Socket.Bind(anAddr);
	}

void CWinSockProviderBase::AutoBind( void )
	{
	WSP_LOG(WspLog::Write(_L("CWinSockProviderBase::AutoBind")));
	}

TInt CWinSockProviderBase::GetOption(TUint /*aLevel*/, TUint /*aName*/, TDes8& /*anOption*/)const
	{
	return KErrNotSupported;
	}

TInt CWinSockProviderBase::SetOption(TUint /*aLevel*/, TUint /*aName*/, const TDesC8& /*anOption*/)
	{
	return KErrNotSupported;
	}

void CWinSockProviderBase::Ioctl(TUint /*aLevel*/, TUint /*aName*/, TDes8* /*anOption*/)
	{
	iSocket->Error(KErrNotSupported, MSocketNotify::EErrorIoctl);
	}

void CWinSockProviderBase::CancelIoctl(TUint /*aLevel*/, TUint /*aName*/)
	{
	}

TInt CWinSockProviderBase::PassiveOpen(TUint /*aQue*/, const TDesC8& /*aConnectionData*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedActivePassiveOpenWithConnectionData));
	return 0;
	}

void CWinSockProviderBase::Shutdown(TCloseType /*option*/, const TDesC8& /*aDisconnectData*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedActiveShutdownWithConnectionData));
	}

void CWinSockProviderBase::ActiveOpen(const TDesC8& /*aConnectionData*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedActiveOpenCallWithConnectionData));
	}

void CWinSockProviderBase::Shutdown(TCloseType aType)
	{
	WSP_LOG(WspLog::Write(_L("CWinSockProviderBase::Shutdown")));
	if (aType != EImmediate)
		{
		iSocket->CanClose();
		}
	}


//
// CWinSockTcpProvider.
//

CWinSockTcpProvider* CWinSockTcpProvider::NewL(RWin32Factory& aWin32Factory)
	{
	CWinSockTcpProvider* self = new(ELeave) CWinSockTcpProvider(aWin32Factory);
	CleanupStack::PushL(self);
	self->ConstructL(RWin32Socket::ETcp);
	CleanupStack::Pop(self);
	return self;
	}

CWinSockTcpProvider* CWinSockTcpProvider::NewBlankLC(RWin32Factory& aWin32Factory)
	{
	CWinSockTcpProvider* self = new(ELeave) CWinSockTcpProvider(aWin32Factory);
	CleanupStack::PushL(self);
	self->ConstructL(RWin32Socket::EBlank);
	return self;
	}

CWinSockTcpProvider::CWinSockTcpProvider(RWin32Factory& aWin32Factory)
	: CWinSockProviderBase(aWin32Factory)
	{
	}

void CWinSockTcpProvider::ConstructL(RWin32Socket::TType aType)
	{
	User::LeaveIfError(iWin32Socket.Open(iWin32Factory, aType));
	iConnectSocketWrapper = CWin32SocketWrapper::NewL(*this, iWin32Socket);
	iAcceptSocketWrapper = CWin32SocketWrapper::NewL(*this, iWin32Socket);
	CWinSockProviderBase::ConstructL();
	}

CWinSockTcpProvider::~CWinSockTcpProvider()
	{
	delete iConnectSocketWrapper;
	delete iAcceptSocketWrapper;
	delete iAcceptSap;
	}

void CWinSockTcpProvider::RemName(TSockAddr& anAddr) const
	{
	WSP_LOG(WspLog::Write(_L("CWinSockTcpProvider::RemName")));
	iWin32Socket.GetPeerName(static_cast<TInetAddr&>(anAddr)); // Throw away error code.
	}

TUint CWinSockTcpProvider::Write(const TDesC8& aDesc, TUint /*options*/, TSockAddr* /*anAddr*/)
	{
	WSP_LOG(WspLog::Printf(_L("CWinSockTcpProvider::Write: writing %d bytes"), aDesc.Length()));
	WSP_RAW_LOG(WspLog::WriteRawOutbound(aDesc));
	if (iSendSocketWrapper->IsActive())
		{
		return 0;
		}

	TInt err = FillSendBuffer(aDesc);
	if (err)
		{
		iSocket->Error(err, MSocketNotify::EErrorSend);
		return 0;
		}
	iSendSocketWrapper->Send(*iSendBuffer);
	return iSendBuffer->Length();
	}

void CWinSockTcpProvider::GetData(TDes8& aDesc, TUint /*aOptions*/, TSockAddr* /*anAddr*/)
	{
	WSP_LOG(WspLog::Write(_L("CWinSockTcpProvider::GetData")));
	TInt esockBufferSize = aDesc.Length();
	if (esockBufferSize >= iEsockReadBuffer.Length())
		{
		WSP_LOG(WspLog::Printf(_L("\tCopying all %d bytes up to ESock"), iEsockReadBuffer.Length()));
		aDesc.Copy(iEsockReadBuffer);
		StartReceive();
		}
	else
		{
		WSP_LOG(WspLog::Printf(_L("\tESock buffer size: %d, available data: %d"), esockBufferSize, iEsockReadBuffer.Length()));
		aDesc.Copy(iEsockReadBuffer.Left(esockBufferSize));
		iEsockReadBuffer.Set(iEsockReadBuffer.Right(iEsockReadBuffer.Length() - esockBufferSize));
		}

	WSP_RAW_LOG(WspLog::WriteRawInbound(aDesc));
	}

void CWinSockTcpProvider::Start()
	{
	if (iListeningSap)
		{
		iListeningSap->HandleListeningSapStarted();
		iListeningSap = NULL;
		}
	}

void CWinSockTcpProvider::ActiveOpen()
	{
	WSP_LOG(WspLog::Write(_L("CWinSockTcpProvider::ActiveOpen")));
	TInt err = iConnectSocketWrapper->Connect(iRemoteAddr);
	if (err)
		{
		iSocket->Error(err, MSocketNotify::EErrorConnect);
		}
	}

TInt CWinSockTcpProvider::PassiveOpen(TUint aQue)
	{
	WSP_LOG(WspLog::Write(_L("CWinSockTcpProvider::PassiveOpen")));
	TRAPD(err, DoPassiveOpenL(aQue));
	return err;
	}

TInt CWinSockTcpProvider::GetOption(TUint aLevel, TUint aName, TDes8& anOption) const
	{
	if (aLevel == KSolInetTcp)
		{
		switch (aName)
			{
			case KSoTcpNoDelay:
				{
				BOOL bOptVal = FALSE;
				TInt length = sizeof(BOOL);
				TInt err = iWin32Socket.GetOption(IPPROTO_TCP, TCP_NODELAY, (char*)&bOptVal, &length);
				if (err == KErrNone)
					{
					TPckg<TInt> intPackage(bOptVal);
					anOption.Copy(intPackage);
					}
				return err;
				}
			}
		}

	return CWinSockProviderBase::GetOption(aLevel, aName, anOption);
	}

TInt CWinSockTcpProvider::SetOption(TUint aLevel, TUint aName, const TDesC8& anOption)
	{
	if (aLevel == KSolInetTcp)
		{
		switch (aName)
			{
			case KSoTcpNoDelay:
				{
				TPckgC<TInt> intPackage(0);
				intPackage.Set(anOption);
				BOOL bOptVal = intPackage();
				return iWin32Socket.SetOption(IPPROTO_TCP, TCP_NODELAY, (char*)&bOptVal, sizeof(BOOL));
				}
			}
		}

	return CWinSockProviderBase::SetOption(aLevel, aName, anOption);
	}

void CWinSockTcpProvider::HandleWin32SocketCompletion(TRequestType aRequestType, TInt aError)
	{
	switch (aRequestType)
		{
		case MWin32SocketObserver::EConnect:
			{
			WSP_LOG(WspLog::Printf(_L("CWinSockTcpProvider::HandleWin32SocketCompletion: EConnect, error: %d"), aError));
			if (aError)
				{
				iSocket->Error(aError, MSocketNotify::EErrorConnect);
				}
			else
				{
				StartReceive();
				if (iSocket)
					{
					iSocket->ConnectComplete();
					}
				}
			break;
			}
		case MWin32SocketObserver::ESend:
			{
			WSP_LOG(WspLog::Printf(_L("CWinSockTcpProvider::HandleWin32SocketCompletion: ESend, error: %d"), aError));
			if (iSocket)
				{
				if (aError)
					{
					iSocket->Error(aError, MSocketNotify::EErrorSend);
					}
				else
					{
					iSocket->CanSend();
					}
				}
			break;
			}
		case MWin32SocketObserver::EReceive:
			{
			WSP_LOG(WspLog::Printf(_L("CWinSockTcpProvider::HandleWin32SocketCompletion: EReceive, error: %d"), aError));
			if (aError)
				{
				if (iSocket)
					{
					iSocket->Error(aError, MSocketNotify::EErrorRecv);
					}
				}
			else
				{
				iEsockReadBuffer.Set(iWinSockWriteBuffer);
				TInt receivedBytes = iEsockReadBuffer.Length();
				WSP_LOG(WspLog::Printf(_L("\t %d bytes received"), receivedBytes));
				if (iSocket)
					{
					if (receivedBytes)
						{
						iSocket->NewData(receivedBytes);
						}
					else
						{
						iSocket->NewData(KNewDataEndofData);
						}
					}
				}
			break;
			}
		case MWin32SocketObserver::EAccept:
			{
			WSP_LOG(WspLog::Printf(_L("CWinSockTcpProvider::HandleWin32SocketCompletion: EAccept, error: %d"), aError));
			iAcceptSap->StartReceive();
			CWinSockProviderBase& acceptSap = *iAcceptSap;
			iAcceptSap = NULL;	// This pointer will be reused when our HandleListeningSapStarted gets, which may be as a result of the next CSocket::ConnectComplete call.
			iSocket->ConnectComplete(acceptSap);
			break;
			}
		case MWin32SocketObserver::ESendTo:
		case MWin32SocketObserver::EReceiveFrom:
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtInvalidTcpSocketRequestType));
			}
		}
	}

void CWinSockTcpProvider::DoPassiveOpenL(TUint aQue)
	{
	__ASSERT_DEBUG(iAcceptSap == NULL, Panic(EWinSockPrtCWinSockTcpProviderDoPassiveOpenLAcceptSapNotNull));
	CWinSockTcpProvider* acceptSap = CWinSockTcpProvider::NewBlankLC(iWin32Factory);
	acceptSap->SetListeningSap(*this);
	User::LeaveIfError(iWin32Socket.Listen(aQue));
	User::LeaveIfError(iAcceptSocketWrapper->Accept(acceptSap->iWin32Socket));
	CleanupStack::Pop(acceptSap);
	iAcceptSap = acceptSap;
	}

void CWinSockTcpProvider::StartReceive()
	{
	iWinSockWriteBuffer.Zero();
	iReceiveSocketWrapper->Receive(iWinSockWriteBuffer);
	}

void CWinSockTcpProvider::SetListeningSap(CWinSockTcpProvider& aListeningSap)
	{
	__ASSERT_DEBUG(iListeningSap == NULL, Panic(EWinSockPrtCWinSockTcpProviderSetListeningSapAlreadySet));
	iListeningSap = &aListeningSap;
	}

void CWinSockTcpProvider::HandleListeningSapStarted()
	{
	__ASSERT_DEBUG(iAcceptSap == NULL, Panic(EWinSockPrtCWinSockTcpProviderStartAcceptSapNotNull));
	CWinSockTcpProvider* acceptSap = CWinSockTcpProvider::NewBlankLC(iWin32Factory);
	acceptSap->SetListeningSap(*this);
	User::LeaveIfError(iAcceptSocketWrapper->Accept(acceptSap->iWin32Socket));
	CleanupStack::Pop(acceptSap);
	iAcceptSap = acceptSap;
	}


//
// CWinSockUdpProvider.
//

CWinSockUdpProvider* CWinSockUdpProvider::NewL(RWin32Factory& aWin32Factory)
	{
	CWinSockUdpProvider* self = new(ELeave) CWinSockUdpProvider(aWin32Factory);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

TInt CWinSockUdpProvider::SetLocalName(TSockAddr& anAddr)
	{
	TInt err = CWinSockProviderBase::SetLocalName(anAddr);
	if (err == KErrNone)
		{
		iReceiveSocketWrapper->ReceiveFrom(iWinSockWriteBuffer, iReceiveFromAddress);
		}
	return err;
	}

void CWinSockUdpProvider::RemName(TSockAddr& anAddr) const
	{
	anAddr = iRemoteAddr;
	}

TUint CWinSockUdpProvider::Write(const TDesC8& aDesc, TUint /*options*/, TSockAddr* anAddr)
	{
	WSP_LOG(WspLog::Printf(_L("CWinSockUdpProvider::Write: writing %d bytes"), aDesc.Length()));
	WSP_RAW_LOG(WspLog::WriteRawOutbound(aDesc));
	if (iSendSocketWrapper->IsActive())
		{
		return 0;
		}
	TInt err = FillSendBuffer(aDesc);
	if (err)
		{
		iSocket->Error(err, MSocketNotify::EErrorSend);
		return 0;
		}
	if (anAddr)
		{
		iSendSocketWrapper->SendTo(*iSendBuffer, *anAddr);
		}
	else
		{
		iSendSocketWrapper->SendTo(*iSendBuffer, iRemoteAddr);
		}
	return iSendBuffer->Length();
	}

void CWinSockUdpProvider::GetData(TDes8 &aDesc, TUint /*aOptions*/, TSockAddr* anAddr)
	{
	WSP_LOG(WspLog::Write(_L("CWinSockUdpProvider::GetData")));
	TInt esockBufferSize = aDesc.Length();
	if (esockBufferSize >= iEsockReadBuffer.Length())
		{
		WSP_LOG(WspLog::Printf(_L("\tCopying all %d bytes up to ESock"), iEsockReadBuffer.Length()));
		aDesc.Copy(iEsockReadBuffer);
		}
	else
		{
		WSP_LOG(WspLog::Printf(_L("\tESock buffer size: %d, available data: %d"), esockBufferSize, iEsockReadBuffer.Length()));
		aDesc.Copy(iEsockReadBuffer.Left(esockBufferSize)); // Throw away remainder.
		}

	WSP_RAW_LOG(WspLog::WriteRawInbound(aDesc));

	if (anAddr)
		{
		anAddr->Copy(iReceiveFromAddress);
		}
	iWinSockWriteBuffer.Zero();
	iReceiveSocketWrapper->ReceiveFrom(iWinSockWriteBuffer, iReceiveFromAddress);
	}

void CWinSockUdpProvider::Start()
	{
	}

void CWinSockUdpProvider::ActiveOpen(void)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtCWinSockUdpProviderActiveOpenUnexpectedCall));
	}

TInt CWinSockUdpProvider::PassiveOpen(TUint /*aQue*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtCWinSockUdpProviderPassiveOpenUnexpectedCall));
	return 0;
	}

void CWinSockUdpProvider::HandleWin32SocketCompletion(TRequestType aRequestType, TInt aError)
	{
	switch (aRequestType)
		{
		case MWin32SocketObserver::ESendTo:
			{
			WSP_LOG(WspLog::Printf(_L("CWinSockUdpProvider::HandleWin32SocketCompletion: ESendTo, error: %d"), aError));
			if (aError)
				{
				iSocket->Error(aError, MSocketNotify::EErrorSend);
				}
			else
				{
				iSocket->CanSend();
				}
			break;
			}
		case MWin32SocketObserver::EReceiveFrom:
			{
			WSP_LOG(WspLog::Printf(_L("CWinSockUdpProvider::HandleWin32SocketCompletion: EReceiveFrom, error: %d"), aError));
			if (aError)
				{
				iSocket->Error(aError, MSocketNotify::EErrorRecv);
				}
			else
				{
				iEsockReadBuffer.Set(iWinSockWriteBuffer);
				WSP_LOG(WspLog::Printf(_L("\t %d bytes received"), iEsockReadBuffer.Length()));
				iSocket->NewData(1);
				}
			break;
			}
		case MWin32SocketObserver::EConnect:
		case MWin32SocketObserver::ESend:
		case MWin32SocketObserver::EReceive:
		case MWin32SocketObserver::EAccept:
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtInvalidUdpSocketRequestType));
			}
		}
	}

CWinSockUdpProvider::~CWinSockUdpProvider()
	{
	}

CWinSockUdpProvider::CWinSockUdpProvider(RWin32Factory& aWin32Factory)
	: CWinSockProviderBase(aWin32Factory)
	{
	}

void CWinSockUdpProvider::ConstructL()
	{
	User::LeaveIfError(iWin32Socket.Open(iWin32Factory, RWin32Socket::EUdp));
	CWinSockProviderBase::ConstructL();
	}
