// rconn.cpp
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

#undef SYMBIAN_ENABLE_SPLIT_HEADERS // Stopgap to handle TConnectionInfoV2 being moved in latest TB92

#include <fshell/ioutils.h>
#include <es_sock.h>
#include <es_enum.h>
#include <nifvar.h>
#include <commdbconnpref.h>

using namespace IoUtils;

class CCmdRconn : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdRconn();
private:
	CCmdRconn();
	void ListConnectionsL();
	void StartConnectionL();
	void StopConnectionL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RSocketServ iSocketSession;
	RConnection iConnection;
	enum 
		{
		EList, EStart, EStop
		} iOperation;
	TUint iIapId;
	TUint iNetworkId;
	};


CCommandBase* CCmdRconn::NewLC()
	{
	CCmdRconn* self = new(ELeave) CCmdRconn();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdRconn::~CCmdRconn()
	{
	iConnection.Close();
	iSocketSession.Close();
	}

CCmdRconn::CCmdRconn()
	{
	}

const TDesC& CCmdRconn::Name() const
	{
	_LIT(KName, "rconn");	
	return KName;
	}

#define CASE_RETURN_LIT(XXX) case XXX: { _LIT(_KLit, #XXX); return &_KLit; }
#define DEFAULT_RETURN_LIT(XXX) default: { _LIT(_KLit, XXX); return &_KLit; }

const TDesC* StringifyConnectionType(TConnectionType aConnectionType)
	{
	enum TConnectionType_NotPresentInTB92
		{
		EConnectionCDMA = 2500,
		EConnectionCDMA20001xRTT,
		EConnectionCDMA20001xRTTDO,
		EConnectionCDMA20001xRTTDV,
		EConnectionCDMA20003xRTT,
		};
		
	switch (aConnectionType)
		{
		CASE_RETURN_LIT(EConnectionGeneric);
		CASE_RETURN_LIT(EConnectionCSD);
		CASE_RETURN_LIT(EConnectionGPRS);
		CASE_RETURN_LIT(EConnectionGPRSR97);
		CASE_RETURN_LIT(EConnectionGPRSR99);
		CASE_RETURN_LIT(EConnectionGPRSRel4);
		CASE_RETURN_LIT(EConnectionGPRSRel5);
		CASE_RETURN_LIT(EConnectionCDMA);
		CASE_RETURN_LIT(EConnectionCDMA20001xRTT);
		CASE_RETURN_LIT(EConnectionCDMA20001xRTTDO);
		CASE_RETURN_LIT(EConnectionCDMA20001xRTTDV);
		CASE_RETURN_LIT(EConnectionCDMA20003xRTT);
		CASE_RETURN_LIT(EConnectionEthernet);
		CASE_RETURN_LIT(EConnectionWLAN);
		CASE_RETURN_LIT(EConnectionBTPAN);
		DEFAULT_RETURN_LIT("Unknown");
		}
	}

void CCmdRconn::DoRunL()
	{
	LeaveIfErr(iSocketSession.Connect(), _L("Couldn't open socket server session"));
	LeaveIfErr(iConnection.Open(iSocketSession), _L("Couldn't open connection handle"));

	if (!iArguments.IsPresent(0)) iOperation = EList;
	switch (iOperation)
		{
	case EList:
		ListConnectionsL();
		break;
	case EStart:
		StartConnectionL();
		break;
	case EStop:
		StopConnectionL();
		break;
		}
	}

void CCmdRconn::ListConnectionsL()
	{
	TUint numConnections;
	LeaveIfErr(iConnection.EnumerateConnections(numConnections), _L("Couldn't enumerate connections"));

	if (numConnections <= 0)
		{
		Write(_L("No connections currently up.\r\n"));
		}
	else
		{
		IoUtils::CTextFormatter* formatter = CTextFormatter::NewLC(Stdout());
		IoUtils::CTextBuffer* buf = IoUtils::CTextBuffer::NewLC(0x100);
		buf->AppendFormatL(_L("IAP\tNetwork\tType\r\n"));
		for (TInt i = 1; i <= numConnections; ++i)
			{
			TConnectionInfoV2Buf connectionInfoBuf;
			LeaveIfErr(iConnection.GetConnectionInfo(i, connectionInfoBuf), _L("Couldn't get connection info (%d of %d)"), i, numConnections);
			TConnectionInfoV2& info = connectionInfoBuf();
			buf->AppendFormatL(_L("%d\t%d\t%S\r\n"), info.iIapId, info.iNetId, StringifyConnectionType(info.ConnectionType()));
			}
		formatter->TabulateL(0, 2, buf->Descriptor());
		Write(formatter->Descriptor());
		CleanupStack::PopAndDestroy(2, formatter);
		}
	}

void CCmdRconn::StartConnectionL()
	{
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

void CCmdRconn::StopConnectionL()
	{
	TConnectionInfo connectionInfo;
	if (iIapId > 0)
		{
		connectionInfo.iIapId = iIapId;
		}
	if (iNetworkId > 0)
		{
		connectionInfo.iNetId = iNetworkId;
		}
	TPckgC<TConnectionInfo> connectionInfoPckg(connectionInfo);
	LeaveIfErr(iConnection.Attach(connectionInfoPckg, RConnection::EAttachTypeNormal), _L("Unable to attach to connection"));
	LeaveIfErr(iConnection.Stop(), _L("Unable to stop connection"));
	}

void CCmdRconn::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptIapId, "iap");
	aOptions.AppendUintL(iIapId, KOptIapId);

	_LIT(KOptNetworkId, "network");
	aOptions.AppendUintL(iNetworkId, KOptNetworkId);
	}

void CCmdRconn::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgOperation, "operation");
	aArguments.AppendEnumL((TInt&)iOperation, KArgOperation);
	}

EXE_BOILER_PLATE(CCmdRconn)

