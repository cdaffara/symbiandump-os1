// tcpcsy.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <c32comm.h>
#include "panic.h"
#include "tcpcsy.h"
#include "tcpcsylog.h"


//
// CTcpPort.
//

TInt CTcpPort::LinkOffset()
	{
	return _FOFF(CTcpPort, iLink);
	}

CTcpPort* CTcpPort::NewLC(CTcpPortFactory& aFactory, CConfig::TMode aMode, TUint aProtocolFamily, TUint aProtocol, const TInetAddr& aAddress, RSocketServ& aSocketServ)
	{
	CTcpPort* self = new(ELeave) CTcpPort(aFactory);
	CleanupClosePushL(*self);
	self->ConstructL(aMode, aProtocolFamily, aProtocol, aAddress, aSocketServ);
	return self;
	}

CTcpPort::CTcpPort(CTcpPortFactory& aFactory)
	: iFactory(aFactory)
	{
	LOG("Created new CTcpPort object 0x%08x", this);
	}

void CTcpPort::ConstructL(CConfig::TMode aMode, TUint aProtocolFamily, TUint aProtocol, const TInetAddr& aAddress, RSocketServ& aSocketServ)
	{
	iInetAddr = aAddress;

	if (aMode == CConfig::EActive)
		{
		iConnector = CConnector::NewL(aSocketServ, iSocket, aProtocolFamily, aProtocol, iInetAddr, *this);
		}
	else
		{
		iListener = CListener::NewL(aSocketServ, iSocket, aProtocolFamily, aProtocol, iInetAddr, *this);
		}

	iReader = CReader::NewL(iSocket, *this);
	iWriter = CWriter::NewL(iSocket, *this);
	}

CTcpPort::~CTcpPort()
	{
	delete iWriter;
	delete iReader;
	delete iConnector;
	if (iSocketConnected)
		{
		TRequestStatus status;
		iSocket.Shutdown(RSocket::ENormal, status);
		User::WaitForRequest(status);
		iSocket.Close();
		}
	iLink.Deque();
	}

void CTcpPort::StartRead(const TAny*, TInt aLength)
	{
	LOG("CTcpPort::StartRead this=0x%08x, aLength=%d", this, aLength);

	if (iError)
		{
		ReadError(iError);
		}
	else if (aLength == 0)
		{
		ReadCompleted(KErrNone);
		}
	else
		{
		if (aLength < 0)
			{
			iReader->StartRead(-aLength, CReader::EOneOrMore);
			}
		else
			{
			iReader->StartRead(aLength, CReader::EFull);
			}
		}
	}

void CTcpPort::ReadCancel()
	{
	LOG("CTcpPort::ReadCancel this=0x%08x", this);
	iReader->Abort();
	ReadError(KErrCancel);
	}

TInt CTcpPort::QueryReceiveBuffer(TInt& aLength) const
	{
	iReader->GetBufferLength(aLength);
	return KErrNone;
	}

void CTcpPort::ResetBuffers(TUint)
	{
	// Doesn't really have much meaning for the TCPCSY as reads and writes are mapped directly onto RSocket calls.
	}

void CTcpPort::StartWrite(const TAny* aClientBuffer, TInt aLength)
	{
	LOG("CTcpPort::StartWrite this=0x%08x, aClientBuff=0x%08x, aLength=%d", this, aClientBuffer, aLength);
	
	TInt err = KErrNone;

	if (iError)
		{
		err = iError;
		}
	else if (aLength > 0)
		{
		if (iWriteBuf.MaxLength() < aLength)
			{
			err = iWriteBuf.ReAlloc(aLength);
			}

		if (err == KErrNone)
			{
			iWriteBuf.Zero();
			err = IPCRead(aClientBuffer, iWriteBuf, 0);
			if (err == KErrNone)
				{
				iWriter->StartWrite(iWriteBuf);
				}
			}
		}

	if (err || (aLength == 0))
		{
		WriteComplete(err);
		}
	}

void CTcpPort::WriteCancel()
	{
	LOG("CTcpPort::WriteCancel this=0x%08x", this);
	iWriter->Abort();
	WriteComplete(KErrCancel);
	}

void CTcpPort::Break(TInt)
	{
	}

void CTcpPort::BreakCancel()
	{
	}

TInt CTcpPort::GetConfig(TDes8&) const
	{
	return KErrNone;
	}

TInt CTcpPort::SetConfig(const TDesC8&)
	{
	return KErrNone;
	}

TInt CTcpPort::GetCaps(TDes8&)
	{
	return KErrNone;
	}

TInt CTcpPort::SetServerConfig(const TDesC8&)
	{
	return KErrNone;
	}

TInt CTcpPort::GetServerConfig(TDes8&)
	{
	return KErrNone;
	}

TInt CTcpPort::GetSignals(TUint&)
	{
	return KErrNone;
	}

TInt CTcpPort::SetSignalsToMark(TUint)
	{
	return KErrNone;
	}

TInt CTcpPort::SetSignalsToSpace(TUint)
	{
	return KErrNone;
	}

TInt CTcpPort::GetReceiveBufferLength(TInt&) const
	{
	return KErrNone;
	}

TInt CTcpPort::SetReceiveBufferLength(TInt)
	{
	return KErrNone;
	}

void CTcpPort::Destruct()
	{
	delete this;
	}

void CTcpPort::NotifySignalChange(TUint)
	{
	}

void CTcpPort::NotifySignalChangeCancel()
	{
	}


void CTcpPort::NotifyConfigChange()
	{
	}

void CTcpPort::NotifyConfigChangeCancel()
	{
	}

void CTcpPort::NotifyFlowControlChange()
	{
	}

void CTcpPort::NotifyFlowControlChangeCancel()
	{
	}

void CTcpPort::NotifyBreak()
	{
	}

void CTcpPort::NotifyBreakCancel()
	{
	}

void CTcpPort::NotifyDataAvailable()
	{
	}

void CTcpPort::NotifyDataAvailableCancel()
	{
	}

void CTcpPort::NotifyOutputEmpty()
	{
	}

void CTcpPort::NotifyOutputEmptyCancel()
	{
	}

TInt CTcpPort::GetFlowControlStatus(TFlowControl&)
	{
	return KErrNotSupported;
	}

TInt CTcpPort::GetRole(TCommRole& aRole)
	{
	aRole = iRole;
	return KErrNone;
	}

TInt CTcpPort::SetRole(TCommRole aRole)
	{
	iRole = aRole;
	return KErrNone;
	}

void CTcpPort::FreeMemory()
	{
	}

void CTcpPort::ConnectionComplete(TInt aError)
	{
	LOG("CTcpPort::ConnectionComplete this=0x%08x, aError=%d", this, aError);
	if (aError == KErrNone)
		{
		iSocketConnected = ETrue;
		iReader->SocketConnected();
		iWriter->SocketConnected();
		}
	else
		{
		iError = aError;
		}
	}

void CTcpPort::ListenComplete(TInt aError)
	{
	LOG("CTcpPort::ListenComplete this=0x%08x, aError=%d", this, aError);
	if (aError == KErrNone)
		{
		iReader->SocketConnected();
		iWriter->SocketConnected();
		}
	else
		{
		iError = aError;
		}
	}

void CTcpPort::ReadComplete(const TDesC8& aData)
	{
	LOG("CTcpPort::ReadComplete this=0x%08x", this);
	TInt err = IPCWrite(NULL, aData, 0);
	ReadCompleted(KErrNone);
	}

void CTcpPort::ReadError(TInt aError)
	{
	LOG("CTcpPort::ReadError this=0x%08x, aError=%d", this, aError);
	ReadCompleted(aError);
	}

void CTcpPort::WriteComplete(TInt aError)
	{
	LOG("CTcpPort::WriteComplete this=0x%08x, aError=%d", this, aError);
	WriteCompleted(aError);
	}



//
// CTcpPortFactory.
//

CTcpPortFactory* CTcpPortFactory::NewL()
	{
	CTcpPortFactory* self = new(ELeave)CTcpPortFactory;
	CleanupClosePushL(*self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPort* CTcpPortFactory::NewPortL(const TUint aUnit)
	{
	TInetAddr inetAddr;
	iConfig->GetAddressL(aUnit, inetAddr);
	CTcpPort* port = CTcpPort::NewLC(*this, iConfig->ModeL(aUnit), iConfig->ProtocolFamilyL(aUnit), iConfig->ProtocolL(aUnit), inetAddr, iSocketServ);
	TBuf<10> name;
	name.Num(aUnit);
	port->SetNameL(&name);
	CleanupStack::Pop(port);
	iPorts.AddLast(*port);
	return port;
	}

void CTcpPortFactory::Info(TSerialInfo& aSerialInfo)
/**
 * This method fills information into the passed structure.  It is required for factory objects.
 *
 * @param	aSerialInfo		-	a reference to the structure to fill in.
 *
 * @return	None
 */
	{
	aSerialInfo.iDescription = KCsyName;
	aSerialInfo.iName = KCsyName;
	iConfig->UnitRange(aSerialInfo.iLowUnit, aSerialInfo.iHighUnit);
	}

CTcpPortFactory::CTcpPortFactory()
	: iPorts(CTcpPort::LinkOffset())
	{
	}

void CTcpPortFactory::ConstructL()
	{
#ifdef __FLOG_ACTIVE	
	
#ifdef LOG_CSY_EVENTS
#pragma message("General logging enabled")
	(void) iEventLogger.Connect();
	iEventLogger.SetLogTags(KDebugSubSystem(), KDebugCategoryEvents());
#endif // LOG_CSY_EVENTS

#ifdef LOG_CSY_TX
#pragma message("Tx logging enabled")
	(void) iTxLogger.Connect();
	iTxLogger.SetLogTags(KDebugSubSystem(), KDebugCategoryTx());
#endif // LOG_CSY_TX

#ifdef LOG_CSY_RX
#pragma message("Rx logging enabled")
	(void) iRxLogger.Connect();
	iRxLogger.SetLogTags(KDebugSubSystem(), KDebugCategoryRx());
#endif // LOG_CSY_RX

#endif // __FLOG_ACTIVE

	SetNameL(&(KCsyName()));
	iConfig = CConfig::NewL(*this);
	User::LeaveIfError(iSocketServ.Connect());
	}
	
CTcpPortFactory::~CTcpPortFactory()
	{
	delete iConfig;
	iSocketServ.Close();

#ifdef __FLOG_ACTIVE

#ifdef LOG_CSY_EVENTS
	iEventLogger.Close();
#endif // LOG_CSY_EVENTS

#ifdef LOG_CSY_TX
	iTxLogger.Close();
#endif // LOG_CSY_TX

#ifdef LOG_CSY_RX
	iRxLogger.Close();
#endif // LOG_CSY_RX

#endif // __FLOG_ACTIVE
	}
	
TSecurityPolicy CTcpPortFactory::PortPlatSecCapability(TUint /*aPort*/) const
	{
	return TSecurityPolicy(ECapabilityNetworkServices);
	}

extern "C"
	{
	IMPORT_C CSerial * LibEntry(void);
	}


EXPORT_C CSerial* LibEntry(void)
	{
	return CTcpPortFactory::NewL();
	}

void Panic(TTcpCsyPanicReason aReason)
	{
	_LIT(KCategory, "tcpcsy");
	User::Panic(KCategory(), aReason);
	}
