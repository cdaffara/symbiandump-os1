// tcpcsy.h
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

#ifndef __TCPCSY_H__
#define __TCPCSY_H__


#include <cs_port.h>
#include "config.h"
#include "reader.h"
#include "writer.h"
#include "connector.h"
#include "listener.h"
#include "tcpcsylog.h"


//
// Constants.
//

_LIT(KCsyName, "tcpcsy");


//
// Forward declarations.
//
class CTcpPort;


//
// Class Definitions.
//


class CTcpPortFactory : public CSerial
	{
public:
    static CTcpPortFactory* NewL();
    ~CTcpPortFactory();
private:	// From CSerial.
	virtual CPort * NewPortL(const TUint aUnit);
	virtual void Info(TSerialInfo &aSerialInfo);
	virtual TSecurityPolicy PortPlatSecCapability(TUint aPort) const;
private:
	CTcpPortFactory();
	void ConstructL();
private:
	RSocketServ iSocketServ;
	CConfig* iConfig;
	TDblQue<CTcpPort> iPorts;
public:
#ifdef __FLOG_ACTIVE

#ifdef LOG_CSY_EVENTS
	RFileLogger iEventLogger;
#endif // LOG_CSY_EVENTS

#ifdef LOG_CSY_TX
	RFileLogger iTxLogger;
#endif // LOG_CSY_TX

#ifdef LOG_CSY_RX
	RFileLogger iRxLogger;
#endif // LOG_CSY_RX

#endif // __FLOG_ACTIVE
	};


class CTcpPort : public CPort, public MConnectorObserver, public MListenerObserver, public MReaderObserver, public MWriterObserver
	{
public:
	static TInt LinkOffset();
	static CTcpPort* NewLC(CTcpPortFactory& aFactory, CConfig::TMode aMode, TUint aProtocolFamily, TUint aProtocol, const TInetAddr& aAddress, RSocketServ& aSocketServ);
public:	// From CPort.
	virtual void StartRead(const TAny* aClientBuffer, TInt aLength);
	virtual void ReadCancel();
	virtual TInt QueryReceiveBuffer(TInt& aLength) const;
	virtual void ResetBuffers(TUint aFlags);
	virtual void StartWrite(const TAny* aClientBuffer, TInt aLength);
	virtual void WriteCancel();
	virtual void Break(TInt aTime);
	virtual void BreakCancel();
	virtual TInt GetConfig(TDes8& aDes) const;
	virtual TInt SetConfig(const TDesC8& aDes);
	virtual TInt SetServerConfig(const TDesC8& aDes);
	virtual TInt GetServerConfig(TDes8& aDes);
	virtual TInt GetCaps(TDes8& aDes);
	virtual TInt GetSignals(TUint& aSignals);
	virtual TInt SetSignalsToMark(TUint aSignals);
	virtual TInt SetSignalsToSpace(TUint aSignals);
	virtual TInt GetReceiveBufferLength(TInt& aLength) const;
	virtual TInt SetReceiveBufferLength(TInt aSignals);
	virtual void Destruct();
	virtual void FreeMemory();
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
public: // From MConnectorObserver.
	virtual void ConnectionComplete(TInt aError);
public: // From MListenerObserver.
	virtual void ListenComplete(TInt aError);
public: // From MReaderObserver.
	virtual void ReadComplete(const TDesC8& aData);
	virtual void ReadError(TInt aError);
public: // From MWriterObserver.
	virtual void WriteComplete(TInt aError);
private:
	virtual ~CTcpPort();
	CTcpPort(CTcpPortFactory& aFactory);
	void ConstructL(CConfig::TMode aMode, TUint aProtocolFamily, TUint aProtocol, const TInetAddr& aAddress, RSocketServ& aSocketServ);
private:
	TDblQueLink iLink;
	CTcpPortFactory& iFactory;
	RSocket iSocket;
	TBool iSocketConnected;
	CConnector* iConnector;
	CListener* iListener;
	CReader* iReader;
	CWriter* iWriter;
	RBuf8 iWriteBuf;
	TInt iError;
	TInetAddr iInetAddr;
	TCommRole iRole;
	};

#endif // __TCPCSY_H__
