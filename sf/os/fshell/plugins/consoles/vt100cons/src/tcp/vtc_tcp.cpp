// vtc_tcp.cpp
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32std.h>
#include <e32cons.h>
#include <c32comm.h>
#include <e32math.h>
#include <es_sock.h>
#include <in_sock.h>
#include <commdbconnpref.h>
#include <fshell/consoleextensions.h>
#include <fshell/vtc_base.h>
#include <fshell/vtc_controller.h>

const TInt KUninitialized = -1;

class TPortConfig
	{
public:
	TPortConfig();
public:
	TName iHost;
	TInt iPort;
	TInt iIapId;
	TInt iNetworkId;
	TInt iProtocolId;
	TInt iImplicit;
	};

TPortConfig::TPortConfig()
	: iPort(8080), iIapId(KUninitialized), iNetworkId(KUninitialized), iProtocolId(KProtocolInetTcp), iImplicit(0)
	{
	}
	
NONSHARABLE_CLASS(CVtcTcpConsole) : public CVtcConsoleBase
	{
public:
	CVtcTcpConsole();
	~CVtcTcpConsole();
private: // From CVtcSerialConsole.
	virtual void ConstructL(const TDesC& aTitle);
private: // From MConsoleOutput.
	virtual TInt Output(const TDesC8& aDes);
private: // From MConsoleInput.
	virtual void Input(TDes8& aDes, TRequestStatus& aStatus);
	virtual void CancelInput(TRequestStatus& aStatus);
private:
	TInt ConfigurePort(const TDesC& aConfig);
	TInt ReadConfig(const TDesC& aConfigDes, TPortConfig& aConfig);
	TInt Accept(const TPortConfig& aConfig);
	TInt Connect(const TPortConfig& aConfig);
	
	TInt Query(TRequestStatus& stat, TRefByValue<const TDesC> aFmt, ...);
	void CancelQuery();
	void CleanupQuery();

private:
	RSocketServ iSocketServ;
	RConnection iConnection;
	RSocket iListeningSocket;
	RSocket iClientSocket;
	TSockXfrLength iSockXfrLength;
	RNotifier iNotifier;
	TInt iQueryResult;
	};


CVtcTcpConsole::CVtcTcpConsole()
	{
	}

CVtcTcpConsole::~CVtcTcpConsole()
	{
	iNotifier.Close();
	
	iListeningSocket.Close();
	iClientSocket.Close();
	iConnection.Close();
	iSocketServ.Close();
	}

class TOverflowTruncate : public TDes16Overflow
	{
public:
	virtual void Overflow(TDes16&) {}
	};
	
TInt CVtcTcpConsole::Query(TRequestStatus& stat, TRefByValue<const TDesC> aFmt, ...)
	{
	TOverflowTruncate overflow;
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, list, &overflow);

	if (UnderlyingConsole())
		{
		Message(EInformation, buf);
		// when using a console, the accept can be cancelled by hitting ctrl-c
		return KErrNotSupported;
		}
	else
		{
		TInt err = KErrNone;
		if (iNotifier.Handle() == KNullHandle)
			{
			err = iNotifier.Connect();
			}
		if (err == KErrNone)
			{
			iNotifier.Notify(_L("vt100tcpcons"), buf, _L("OK"), _L("Cancel"), iQueryResult, stat);
			}
		return err;
		}
	}
	
void CVtcTcpConsole::CancelQuery()
	{
	if (iNotifier.Handle())
		{
		iNotifier.NotifyCancel(); // Annoyingly this doesn't appear to be implemented, so we can't auto-dismiss the dialog.
		iNotifier.Close();
		}
	}
	
void CVtcTcpConsole::ConstructL(const TDesC& aTitle)
	{
	User::LeaveIfError(ConfigurePort(aTitle));
	CVtcConsoleBase::ConstructL(aTitle);
	}

TInt CVtcTcpConsole::ReadConfig(const TDesC& aConfigDes, TPortConfig& aConfig)
	{
	_LIT(KKeywordHost, "host");
	_LIT(KKeywordPort, "port");
	_LIT(KKeywordIapId, "iap");
	_LIT(KKeywordNetworkId, "network");
	_LIT(KKeywordProtocolId, "protocol");
	_LIT(KKeywordDebug, "debug");
	_LIT(KKeywordImplicit, "implicit");

	TInt err = KErrNone;
	TLex lex(aConfigDes);
	while (!lex.Eos())
		{
		TPtrC keyword;
		TPtrC valueDes;
		err = ReadKeywordValuePair(lex, keyword, valueDes);
		if (err != KErrNone)
			{
			break;
			}

		if (keyword == KKeywordHost)
			{
			aConfig.iHost = valueDes;
			}
		else
			{
			TInt valueInt;
			TLex lex(valueDes);
			err = lex.Val(valueInt);

			if (keyword == KKeywordPort)
				{
				aConfig.iPort = valueInt;
				}
			else if (keyword == KKeywordIapId)
				{
				aConfig.iIapId = valueInt;
				}
			else if (keyword == KKeywordNetworkId)
				{
				aConfig.iNetworkId = valueInt;
				}
			else if (keyword == KKeywordProtocolId)
				{
				aConfig.iProtocolId = valueInt;
				}
			else if (keyword == KKeywordDebug)
				{
				SetDebug(valueInt);
				}
			else if (keyword == KKeywordImplicit)
				{
				aConfig.iImplicit = valueInt;
				}
			}
		}

	return err;
	}

TInt CVtcTcpConsole::ConfigurePort(const TDesC& aConfig)
	{
	TPortConfig portConfig;
	TInt err = ReadConfig(aConfig, portConfig);
	if (err == KErrNone)
		{
		err = iSocketServ.Connect();
		}
	if ((err == KErrNone) && !portConfig.iImplicit)
		{
		err = iConnection.Open(iSocketServ);
		if (err == KErrNone)
			{
			Message(EDebug, _L("Starting connection..."));
			TCommDbConnPref prefs;
			if (portConfig.iIapId == KUninitialized)
				{
				prefs.SetDialogPreference(ECommDbDialogPrefPrompt);
				}
			else
				{
				prefs.SetIapId(portConfig.iIapId);
				prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
				}
			err = iConnection.Start(prefs);
			if (err)
				{
				Message(EError, _L("Connection failed (%d)"), err);
				}
			}
		}
	if (err == KErrNone)
		{
		if (portConfig.iHost.Length() > 0)
			{
			err = Connect(portConfig);
			}
		else
			{
			err = Accept(portConfig);
			}			
		}

	if (err == KErrNone)
		{
		iClientSocket.SetOpt(KSoTcpNoDelay, KSolInetTcp, ETrue);	// Ignore error (may not be supported).
		Message(EDebug, _L("Connected."));
		}

	if (err != KErrNone)
		{
		iClientSocket.Close();
		iListeningSocket.Close();
		iConnection.Close();
		iSocketServ.Close();
		}

	return err;
	}
	
TInt CVtcTcpConsole::Connect(const TPortConfig& aConfig)
	{
	Message(EDebug, _L("Connecting to %S:%u"), &aConfig.iHost, aConfig.iPort);
	TInetAddr addr(aConfig.iPort);
	TInt err = addr.Input(aConfig.iHost);
	if (err)
		{
		RHostResolver resolver;
		if (aConfig.iImplicit)
			{
			err = (resolver.Open(iSocketServ, KAfInet, aConfig.iProtocolId));
			}
		else
			{
			err = (resolver.Open(iSocketServ, KAfInet, aConfig.iProtocolId, iConnection));
			}
		if (err == KErrNone)
			{
			TNameEntry nameEntry;
			err = resolver.GetByName(aConfig.iHost, nameEntry);
			if (err == KErrNone)
				{
				addr.SetAddress(TInetAddr::Cast(nameEntry().iAddr).Address());
				}
			resolver.Close();
			}
		}
	if (err == KErrNone)
		{
		if (aConfig.iImplicit)
			{
			err = iClientSocket.Open(iSocketServ, KAfInet, KSockStream, aConfig.iProtocolId);
			}
		else
			{
			err = iClientSocket.Open(iSocketServ, KAfInet, KSockStream, aConfig.iProtocolId, iConnection);
			}
		}
	if (err == KErrNone)
		{
		TRequestStatus status;
		iClientSocket.Connect(addr, status);
		User::WaitForRequest(status);
		err = status.Int();
		}
	if (err != KErrNone)
		{
		Message(EError, _L("Connection to %S:%u failed (%d)"), &aConfig.iHost, aConfig.iPort, err);
		}
	return err;
	}
	
TInt CVtcTcpConsole::Accept(const TPortConfig& aConfig)
	{
	TInt err = KErrNone;
	Message(EDebug, _L("Opening listening socket..."));
	if (aConfig.iImplicit)
		{
		err = iListeningSocket.Open(iSocketServ, KAfInet, KSockStream, aConfig.iProtocolId);
		}
	else
		{
		err = iListeningSocket.Open(iSocketServ, KAfInet, KSockStream, aConfig.iProtocolId, iConnection);
		}
	if (err == KErrNone)
		{
		TInetAddr addr(aConfig.iPort);
		Message(EDebug, _L("Binding listening socket..."));
		err = iListeningSocket.Bind(addr);
		if (err)
			{
			Message(EError, _L("Bind failed (%d)"), err);
			}
		}
	if (err == KErrNone)
		{
		err = iListeningSocket.Listen(1);
		}
	if (err == KErrNone)
		{
		err = iClientSocket.Open(iSocketServ);
		}
	if (err == KErrNone)
		{
		// Find our local IP address.
		TInetAddr addr;
		addr.SetAddress(KInetAddrAny);
		TPckgBuf<TSoInetIfQuery> query;
		query().iDstAddr = addr;
		TInt err2 = iListeningSocket.GetOpt(KSoInetIfQueryByDstAddr, KSolInetIfQuery, query);
		TInt qerr;
		
		TRequestStatus queryStatus;
		if (err2 == KErrNone)
			{
			TBuf<128> addrName;
			query().iSrcAddr.Output(addrName);
			qerr = Query(queryStatus, _L("Listening on %S:%d..."), &addrName, aConfig.iPort);
			}
		else
			{
			qerr = Query(queryStatus, _L("Listening on port %d..."), aConfig.iPort);
			}

		// Start the accept.
		TRequestStatus acceptStatus;
		iListeningSocket.Accept(iClientSocket, acceptStatus);

		if (qerr == KErrNone)
			{
			User::WaitForRequest(acceptStatus, queryStatus);
			if (acceptStatus != KRequestPending)
				{
				CancelQuery();
				User::WaitForRequest(queryStatus);
				}
			else
				{
				if (iQueryResult == 1)
					{
					if (acceptStatus == KRequestPending)
						{
						iListeningSocket.CancelAccept();
						}
					User::WaitForRequest(acceptStatus);
					}
				else
					{
					User::WaitForRequest(acceptStatus);
					}
				CancelQuery();
				}
			}
		else
			{
			User::WaitForRequest(acceptStatus);
			}

		err = acceptStatus.Int();
		if (err)
			{
			Message(EError, _L("Accept failed (%d)"), err);
			}
		else
			{
			TInetAddr remote;
			iClientSocket.RemoteName(remote);
			TBuf<128> addrName;
			remote.Output(addrName);
			Message(EInformation, _L("Remote %S connected."), &addrName);
			}
		}
	return err;
	}
	
TInt CVtcTcpConsole::Output(const TDesC8& aDes)
	{
	TRequestStatus status;
	iClientSocket.Write(aDes, status);
	User::WaitForRequest(status);
	return status.Int();
	}

void CVtcTcpConsole::Input(TDes8& aDes, TRequestStatus& aStatus)
	{
	iClientSocket.RecvOneOrMore(aDes, 0, aStatus, iSockXfrLength);
	}

void CVtcTcpConsole::CancelInput(TRequestStatus&)
	{
	iClientSocket.CancelRead();
	}

EXPORT_C TAny* NewConsole()
	{
	return new CVtcTcpConsole;
	}

