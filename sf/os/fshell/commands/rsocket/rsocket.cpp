// rsocket.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <es_sock.h>
#include <es_enum.h>
#include <in_sock.h>
#include <nifvar.h>
#include <commdbconnpref.h>

using namespace IoUtils;

class CCmdRsocket : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdRsocket();
private:
	CCmdRsocket();
	void StartConnectionL();
	void TcpConnectL();
	void TcpDisconnect();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RSocketServ iSocketSession;
	RConnection iConnection;
	RSocket iSocket;
	enum 
		{
		EConnect, EListen
		} iOperation;
	HBufC* iHost;
	TUint iPort;
	TUint iIapId;
	TUint iNetworkId;
	TBool iVerbose;
	};


CCommandBase* CCmdRsocket::NewLC()
	{
	CCmdRsocket* self = new(ELeave) CCmdRsocket();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdRsocket::~CCmdRsocket()
	{
	delete iHost;
	iSocket.Close();
	iConnection.Close();
	iSocketSession.Close();
	}

CCmdRsocket::CCmdRsocket()
	{
	}

const TDesC& CCmdRsocket::Name() const
	{
	_LIT(KName, "rsocket");	
	return KName;
	}

void CCmdRsocket::DoRunL()
	{
	LeaveIfErr(iSocketSession.Connect(), _L("Couldn't open socket server session"));
	LeaveIfErr(iConnection.Open(iSocketSession), _L("Couldn't open connection handle"));

	switch (iOperation)
		{
		case EConnect:
			TcpConnectL();
			break;
		case EListen:
			LeaveIfErr(KErrNotSupported, _L("Listen operation not yet implemented"));
			break;
		};
	}

void CCmdRsocket::StartConnectionL()
	{
	if (iVerbose)
		{
		Write(_L("Starting RConnection...\r\n"));
		}

	TCommDbConnPref prefs;
	if (iIapId > 0)
		{
		prefs.SetIapId(iIapId);
		prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
		}
	if (iNetworkId > 0)
		{
		prefs.SetNetId(iNetworkId);
		}
	LeaveIfErr(iConnection.Start(prefs), _L("Failed to start connection"));
	}

void CCmdRsocket::TcpConnectL()
	{
	StartConnectionL();

	TInetAddr addr(iPort);
	TInt err = addr.Input(*iHost);
	if (err)
		{
		if (iVerbose)
			{
			Write(_L("Resolving name...\r\n"));
			}
		RHostResolver resolver;
		LeaveIfErr(resolver.Open(iSocketSession, KAfInet, KProtocolInetTcp, iConnection), _L("Unable to open host resolver"));
		CleanupClosePushL(resolver);
		TNameEntry nameEntry;
		LeaveIfErr(resolver.GetByName(*iHost, nameEntry), _L("Unable to resolve name \"%S\""), iHost);
		addr.SetAddress(TInetAddr::Cast(nameEntry().iAddr).Address());
		CleanupStack::PopAndDestroy(&resolver);
		}

	if (iVerbose)
		{
		Write(_L("Opening socket...\r\n"));
		}
	LeaveIfErr(iSocket.Open(iSocketSession, KAfInet, KSockStream, KProtocolInetTcp, iConnection), _L("Unable to open TCP socket"));

	if (iVerbose)
		{
		Write(_L("Connecting socket...\r\n"));
		}
	TRequestStatus status;
	iSocket.Connect(addr, status);
	User::WaitForRequest(status);
	LeaveIfErr(status.Int(), _L("Unable to connect to %S : %u"), iHost, iPort);

	TcpDisconnect();
	}

void CCmdRsocket::TcpDisconnect()
	{
	if (iVerbose)
		{
		Write(_L("Shutting down socket...\r\n"));
		}
	TRequestStatus status;
	iSocket.Shutdown(RSocket::ENormal, status);
	User::WaitForRequest(status);
	}

void CCmdRsocket::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgOperation, "operation");
	aArguments.AppendEnumL((TInt&)iOperation, KArgOperation);

	_LIT(KOptHost, "host");
	aArguments.AppendStringL(iHost, KOptHost);

	_LIT(KOptPort, "port");
	aArguments.AppendUintL(iPort, KOptPort);
	}

void CCmdRsocket::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);

	_LIT(KOptIapId, "iap");
	aOptions.AppendUintL(iIapId, KOptIapId);

	_LIT(KOptNetworkId, "network");
	aOptions.AppendUintL(iNetworkId, KOptNetworkId);
	}


EXE_BOILER_PLATE(CCmdRsocket)

