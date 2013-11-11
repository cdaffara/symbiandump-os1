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
//

#include "BTTestHelpers.h"
#include "BTTestConsole.h"

_LIT(KNotDefined,				"-");

_LIT(KConnIdle,					"Idle        ");
_LIT(KConnListening,			"Listening   ");
_LIT(KConnListeningAccept,		"Lstn[Accept]");
_LIT(KConnAccepting,			"Accepting   ");
_LIT(KConnConnecting,			"Connecting  ");
_LIT(KConnConnected,			"Connected   ");
_LIT(KConnDisconnected,			"Disconnected");
_LIT(KConnServerDisconnected,	"Serv Disc   ");
_LIT(KConnFailed,				"Failed      ");

_LIT(KRoleMaster,				"M");
_LIT(KRoleSlave,				"S");

_LIT(KModeActive,				"Active");
_LIT(KRoleHold,					"Hold  ");
_LIT(KRoleSniff,				"Sniff ");
_LIT(KRolePark,					"Park  ");
_LIT(KRoleScatter,				"Scatr ");
_LIT(KModeExplicitActive,		"ExpAct");

_LIT(KDataNone,					" --- ");
_LIT(KDataDialog1,				"Rx/Tx");
_LIT(KDataDialog2,				"Tx/Rx");
_LIT(KDataFailed,				"FAIL ");
_LIT(KDataSend,					"Send ");
_LIT(KDataRecv,					"Recv ");
_LIT(KDataRecvAny,				"RecvA");
_LIT(KDataSendReceive,			"BiDi ");
_LIT(KDataSendOS,				"Tx OS");
_LIT(KDataRecvOS,				"Rx OS");

const TDesC& TConnProfile::StateName(TConnState aState)
	{
	switch(aState)
		{
		case EConnIdle:
			return KConnIdle;
			
		case EConnListening:
			return KConnListening;
			
		case EConnListeningAccept:
			return KConnListeningAccept;
			
		case EConnAccepting:
			return KConnAccepting;
			
		case EConnConnecting:
			return KConnConnecting;
			
		case EConnConnected:
			return KConnConnected;
			
		case EConnDisconnected:
			return KConnDisconnected;
			
		case EConnServerDisconnected:
			return KConnServerDisconnected;
			
		case EConnFailed:
			return KConnFailed;
			
		default:
			break;
		};
		
	return KNullDesC;	
	}

const TDesC& TConnProfile::RoleName(TConnRole aState)
	{
	switch(aState)
		{
		case ENone:
			return KNotDefined;
			
		case EMaster:
			return KRoleMaster;
			
		case ESlave:
			return KRoleSlave;
			
		default:
			break;
		};
		
	return KNullDesC;	
	}

const TDesC& TConnProfile::ModeName(TBTLinkMode aState)
	{
	switch(aState)
		{
		case EActiveMode:
			return KModeActive;
			
		case EHoldMode:
			return KRoleHold;

		case ESniffMode:
			return KRoleSniff;
			
		case EParkMode:
			return KRolePark;

		case EScatterMode:
			return KRoleScatter;
			
		default:
			break;
		};
		
	return KNullDesC;	
	}

const TDesC& TConnProfile::ModeName(TConnProfile::TConnMode aState)
	{
	switch(aState)
		{
		case EActive:
			return KModeActive;
			
		case EHold:
			return KRoleHold;

		case ESniff:
			return KRoleSniff;
			
		case EPark:
			return KRolePark;
		
		case EExplicitActive:
			return KModeExplicitActive;

		default:
			break;
		};
		
	return KNullDesC;	
	}

const TDesC& TConnProfile::DataStateName(TDataState aState)
	{
	switch(aState)
		{
		case EDataIdle:
			return KDataNone;
			
		case EDataDialog1:
			return KDataDialog1;
			
		case EDataDialog2:
			return KDataDialog2;
			
		case EDataFailed:
			return KDataFailed;
			
		case EDataSend:
			return KDataSend;
			
		case EDataRecv:
			return KDataRecv;

		case EDataRecvAny:
			return KDataRecvAny;
			
		case EDataSendOneShot:
			return KDataSendOS;
			
		case EDataRecvOneShot:
			return KDataRecvOS;

		case EDataSendReceive:
			return KDataSendReceive;
			
		default:
			break;
		};
		
	return KNullDesC;	
	}


CBTTestConnectionNotifier* CBTTestConnectionNotifier::NewL(CBTTestConnection & aParent)
	{
	CBTTestConnectionNotifier* self = NewLC(aParent);
	CleanupStack::Pop(); // self
	return self;
	}

CBTTestConnectionNotifier* CBTTestConnectionNotifier::NewLC(CBTTestConnection & aParent)
	{
	CBTTestConnectionNotifier* self = new (ELeave) CBTTestConnectionNotifier(aParent);
	CleanupStack::PushL(self);
	return self;
	}

CBTTestConnectionNotifier::~CBTTestConnectionNotifier()
	{
	}

CBTTestConnectionNotifier::CBTTestConnectionNotifier(CBTTestConnection & aParent)
  : CActive(CActive::EPriorityStandard), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}

void CBTTestConnectionNotifier::CancelRequests()
	{
	Cancel();
	}

void CBTTestConnectionNotifier::DoCancel()
	{
	iParent.CancelBasebandNotifyRequest();
	}

void CBTTestConnectionNotifier::RequestNotifications(TUint32 aNotifications, RBTPhysicalLinkAdapter & aPhyAdapter)
	{
	iEvent = iNotification;
	aPhyAdapter.NotifyNextBasebandChangeEvent(iEvent, iStatus, aNotifications);

	SetActive();
	}


void CBTTestConnectionNotifier::RunL()
	{
	TBTBasebandEventNotification notification = iEvent();
	iParent.OutputOneShotEventNotification(notification);
	}


//
//				CBTTestConnection Class Methods					//
//
//
//				Public functions								//
//
CBTTestConnection* CBTTestConnection::NewL(CBTTestConsole* aConsole, RSocketServ& aSocketServer)
	{
	CBTTestConnection* self = NewLC(aConsole, aSocketServer);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CBTTestConnection* CBTTestConnection::NewLC(CBTTestConsole * aConsole, RSocketServ & aSocketServer)
	{
	CBTTestConnection* self = new (ELeave) CBTTestConnection(aConsole, aSocketServer);
	CleanupStack::PushL(self);
	return self;
	}

CBTTestConnection::CBTTestConnection(CBTTestConsole * aConsole, RSocketServ & aSocketServer)
: iParent(aConsole),
  iSocketServer(aSocketServer),
  iNotifier(*this),
  iSyncLinkHandler(*this),
  iPhyLinkAdapter(),
  iEventNotificationQueue(_FOFF(TBTQueuedBasebandEventNotification, iLink)),
  iCurrentIOCTL(EDontCare),
  iTailDigit(-1),
  iTotalRecvByte(0),
  iAutoSniff(EFalse)
	{
	}

void CBTTestConnection::ConstructL()
	{
	iCustomSendRecvBuffer.CreateL(0);
	}

CBTTestConnection::~CBTTestConnection()
	{
	delete iBTSocket;
	delete iBTPhyLinks;
	delete iBTSyncLink;
	iNotifier.CancelRequests();
	iPhyLinkAdapter.Close();
	delete iMetrics;
	delete iMetricsConsole;
	ClearEventStore();
	}

TInt CBTTestConnection::ConfigureLink(TBool aUseIoctl)
	{
	iConfigPkg() = (iParent->L2CapAPIConfig());
	TInt rerr;
	if(aUseIoctl)
		{
		rerr = iBTSocket->Ioctl(KL2CAPUpdateChannelConfigIoctl, &iConfigPkg, KSolBtL2CAP); 
		}
	else
		{
		rerr = iBTSocket->SetOpt(KL2CAPUpdateChannelConfig, KSolBtL2CAP, iConfigPkg);
		}
		
	return rerr;
	}

TInt CBTTestConnection::CreateListeningSocket(TUint16 aPort, TDesC16 const& aProtocolName, TBTServiceSecurity & aSecurity, TBool aUseDefaultConfig)
	{
	TInt rerr = KErrNone;
	
	TProtocolDesc iProtocolInfo;

	User::LeaveIfError(iSocketServer.FindProtocol(aProtocolName, iProtocolInfo));  //Qualified
	TRAP(rerr, iBTSocket = CBluetoothSocket::NewL(*this, iSocketServer, iProtocolInfo.iSockType,iProtocolInfo.iProtocol));

	if(rerr == KErrNone)
		{
		iPeerSocketAddr.SetBTAddr(TBTDevAddr(0));
		iPeerSocketAddr.SetPort(aPort);
		iPeerSocketAddr.SetSecurity(aSecurity);

		rerr = iBTSocket->Bind(iPeerSocketAddr);
		if(rerr == KErrNone)
			{
			rerr = iBTSocket->Listen(5);
			if(rerr == KErrNone)
				{
#ifdef _DEBUG
				TInt value = 0;
				TInt err = iBTSocket->GetOpt(KL2CAPVersion1_2, KSolBtL2CAP, value);
#else
				// For non debug build this condition can not be tested.  Assume L2CAP is v1.2
				TInt err = KErrNone;
#endif
				if(!aUseDefaultConfig && err == KErrNone)
					{
					TL2CapConfigPkg pkg(iParent->L2CapAPIConfig());
					rerr = iBTSocket->SetOpt(KL2CAPUpdateChannelConfig, KSolBtL2CAP, pkg);
					}
				}
			}
		}
	iProfile.iState = (rerr == KErrNone) ? TConnProfile::EConnListening :
	                                       TConnProfile::EConnFailed;
	iProfile.iErrorCode = rerr;	

	return rerr;		
	}

TInt CBTTestConnection::Accept(CBluetoothSocket& aListeningSocket)
	{
	TInt rerr = KErrNone;
	TRAP(rerr, iBTSocket = CBluetoothSocket::NewL(*this, iSocketServer));
	if(rerr == KErrNone)
		{
		rerr = aListeningSocket.Accept(*iBTSocket);
		}

	iProfile.iState = (rerr == KErrNone) ? TConnProfile::EConnAccepting :
	                                       TConnProfile::EConnFailed;
	iProfile.iErrorCode = rerr;

	return rerr;
	}

TInt CBTTestConnection::ConnectToPeer(TBTDevAddr &aAddr, TUint16 aPort, TDesC16 const& aProtocolName, TBTServiceSecurity & aSecurity, TBool aUseDefaultConfig)
	{
	TInt rerr = KErrNone;
	
	TProtocolDesc iProtocolInfo;

	rerr = iSocketServer.FindProtocol(aProtocolName, iProtocolInfo);
	if(rerr == KErrNone)
		{
		TRAP(rerr, iBTSocket = CBluetoothSocket::NewL(*this, iSocketServer, iProtocolInfo.iSockType,
			                                                                 iProtocolInfo.iProtocol));
		if(rerr == KErrNone)
			{
			iPeerSocketAddr.SetBTAddr(aAddr);
			iPeerSocketAddr.SetPort(aPort);
			iPeerSocketAddr.SetSecurity(aSecurity);

#ifdef _DEBUG
			TInt value = 0;
			TInt err = iBTSocket->GetOpt(KL2CAPVersion1_2, KSolBtL2CAP, value);
#else
			// For non debug build this condition can not be tested.  Assume L2CAP is v1.2
			TInt err = KErrNone;
#endif
			if(!aUseDefaultConfig && err == KErrNone)
				{
				TL2CapConfigPkg pkg(iParent->L2CapAPIConfig());
				rerr = iBTSocket->SetOpt(KL2CAPUpdateChannelConfig, KSolBtL2CAP, pkg);
				}
			}
		}
	if(rerr == KErrNone)
		{
		rerr = iBTSocket->Connect(iPeerSocketAddr);
		}

	iProfile.iState = (rerr == KErrNone) ? TConnProfile::EConnConnecting :
	                                       TConnProfile::EConnFailed;
	iProfile.iErrorCode = rerr;		

	return rerr;
	}
	
TInt CBTTestConnection::SendMTUIoctl(TBool aIsIncomingMTU, TUint16 aNewValue)
	{
	i16bitIoctlBuffer() = aNewValue;
	return iBTSocket->Ioctl((aIsIncomingMTU ? KL2CAPIncomingMTUIoctl : KL2CAPOutgoingMTUIoctl), &i16bitIoctlBuffer, KSolBtL2CAP);
	}

TInt CBTTestConnection::SendPretendIncomingSduQFullIoctl(TBool aPretendIncomingSduQFull)
	{
	iBoolIoctlBuffer() = aPretendIncomingSduQFull;
	return iBTSocket->Ioctl(KL2CAPPretendIncomingSduQFull, &iBoolIoctlBuffer, KSolBtL2CAP);
	}

TInt CBTTestConnection::GetChannelMode(TL2CapChannelMode& aMode)
	{
	return iBTSocket->GetOpt(KL2CAPNegotiatedChannelMode, KSolBtL2CAP, reinterpret_cast<TInt&>(aMode));
	}

TInt CBTTestConnection::GetLocalPSM(TL2CAPPort& aPsm)
	{
	TPckg<TL2CAPPort> buf(aPsm);
	return iBTSocket->GetOpt(KL2CAPLocalPSM, KSolBtL2CAP, buf);
	}

TInt CBTTestConnection::RecvOneShotData()
	{
	TInt rerr = KErrNone;
	if(iProfile.iIsStreamProtocol)
		{
		rerr = iBTSocket->RecvOneOrMore(iRecvBuffer, 0, iRecvBufferLength);
		}
	else
		{
		rerr = iBTSocket->Read(iRecvBuffer);
		}
	return rerr;
	}
	
TInt CBTTestConnection::SendOneShotData(TUint16 aDataSize)
	{
	TInt rerr = KErrNone;
	//fill the data with 0123456789012... sequences
	iCustomSendRecvBuffer.Zero();	
	iCustomSendRecvBuffer.ReAlloc(aDataSize);
	for(TInt i =0 ;i<aDataSize;i++)
		{
		iCustomSendRecvBuffer.Append(i%10);
		}
	rerr = iBTSocket->Write(iCustomSendRecvBuffer);
	return rerr;
	}
	
TInt CBTTestConnection::RecvContinuationData(TUint16 aDataSize)
	{
	TInt rerr =0;
	
	iCustomSendRecvBuffer.Zero();	
	rerr = iCustomSendRecvBuffer.ReAlloc(aDataSize);
	if(rerr == KErrNone)
		{
		/*
		In function Recv(iCustomSendRecvBuffer, KSockReadContinuation, iRecvBufferLength);
		iCustomSendRecvBuffer: 	A descriptor for the information being sent to the remote Bluetooth device. 
		KSockReadContinuation:	Bluetooth information flags. When it's set to KSockReadContinuation, 
								Read from datagram in a stream-like fashion(not discarding tails,continuation).
		iRecvBufferLength:		A length indicating how much data is left in the buffer.
		*/
		rerr = iBTSocket->Recv(iCustomSendRecvBuffer, KSockReadContinuation, iRecvBufferLength);
		}
	return rerr;
	}

TInt CBTTestConnection::SetLocalModemStatus(TUint8 aModemStatus)
	{
	TPckgBuf<TUint8> pkg(aModemStatus);
	iParent->test.Printf(_L("\nSetLocalModemStatus: %x"), aModemStatus);
	return iBTSocket->SetOpt(KRFCOMMLocalModemStatus, KSolBtRFCOMM, pkg);
	}
	
TInt CBTTestConnection::RequestRFCommConfChgInd()
	{
	// Cancel any outstanding IOCTLs
	__ASSERT_ALWAYS(iBTSocket, Panic(EBTConsoleNoSocketForSocketBasedConnection));
	iBTSocket->CancelIoctl();
	
	TInt rerr = iBTSocket->Ioctl(KRFCOMMConfigChangeIndicationIoctl, NULL, KSolBtRFCOMM); 
	if(rerr == KErrNone)
		{
		iCurrentIOCTL = ERFCOMMConfigInd;
		}
	return rerr;	
	}

TInt CBTTestConnection::ConnectUsingBAP(TBTDevAddr &aAddr)
	{
	TInt rerr = KErrNone;
	iProfile.iIsSocketBased = EFalse;
	TRAP(rerr, iBTPhyLinks = CBluetoothPhysicalLinks::NewL(*this, iSocketServer));
	if(rerr == KErrNone)
		{
		iProfile.iRemoteBTAddr.SetBTAddr(aAddr);
		rerr = iBTPhyLinks->CreateConnection(aAddr);
		}
		
	return rerr;	
	}

void CBTTestConnection::UpdateProfileAcceptComplete()
	{
	TInt rerr = KErrNone;
	
	iBTSocket->RemoteName(iProfile.iRemoteBTAddr);
	iProfile.iState = TConnProfile::EConnConnected;
	TBTDevAddr addr = iProfile.iRemoteBTAddr.BTAddr();

	TInt rValue = 0;
	rerr = iBTSocket->GetOpt(KL2CAPOutboundMTUForBestPerformance, KSolBtL2CAP, rValue);
	if(rerr == KErrNone)
		{
		iProfile.iSendingMTUSize = rValue;

		rerr = iBTSocket->GetOpt(KL2CAPInboundMTU, KSolBtL2CAP, rValue);
		if(rerr == KErrNone)
			{
			iProfile.iRecvMTUSize = rValue;
			
			iParent->CreatePhyRecord(addr);
			RequestNotification(0xffffffff);
			}
		}

	iProfile.iErrorCode = rerr;	
	}


void CBTTestConnection::AcceptComplete(TInt result)
	{
	if(result)
		{
		iParent->test.Printf(_L("\nAccept Completed with error: %d"), result);
		iProfile.iErrorCode = result;	
		if(iProfile.iAccepterIx >= 0 && iParent->GetConn(iProfile.iAccepterIx))
			{
			(iParent->GetConn(iProfile.iAccepterIx))->iProfile.iState = TConnProfile::EConnFailed;
			(iParent->GetConn(iProfile.iAccepterIx))->iProfile.iErrorCode = result;
			}
		}
	else
		{
		if(iProfile.iAccepterIx >= 0 && iParent->GetConn(iProfile.iAccepterIx))
			{
			(iParent->GetConn(iProfile.iAccepterIx))->UpdateProfileAcceptComplete();
			(iParent->GetConn(iProfile.iAccepterIx))->iProfile.iIsStreamProtocol = iProfile.iIsStreamProtocol;
			}
		}
	iProfile.iState = TConnProfile::EConnListening;
	iParent->Draw();
	}

void CBTTestConnection::ConnectComplete(TInt result)
	{
	TInt rerr = KErrNone;
	
	if(result)
		{
		iParent->test.Printf(_L("\nConnect Completed with error: %d"), result);
		iProfile.iState = TConnProfile::EConnFailed;
		iProfile.iErrorCode = result;	
		}
	else
		{
		iProfile.iState = TConnProfile::EConnConnected;
		iBTSocket->RemoteName(iProfile.iRemoteBTAddr);
		TBTDevAddr addr = iProfile.iRemoteBTAddr.BTAddr();

		TInt rValue = 0;
		rerr = iBTSocket->GetOpt(KL2CAPOutboundMTUForBestPerformance, KSolBtL2CAP, rValue);
		if(rerr == KErrNone)
			{
			iProfile.iSendingMTUSize = rValue;

			rerr = iBTSocket->GetOpt(KL2CAPInboundMTU, KSolBtL2CAP, rValue);
			if(rerr == KErrNone)
				{
				iProfile.iRecvMTUSize = rValue;
				TRAP_IGNORE(iMetrics = CBluetoothPhysicalLinkMetrics::NewL(*this, iSocketServer, addr));
				iParent->CreatePhyRecord(addr);
				RequestNotification(0xffffffff);
				}
			}
		iProfile.iErrorCode = rerr;	
		}
	iParent->Draw();
	}

TInt CBTTestConnection::SetupPhysAdapter()
	{
	TBTDevAddr addr = iProfile.iRemoteBTAddr.BTAddr();
	iParent->CreatePhyRecord(addr);
	TInt rerr = iPhyLinkAdapter.Open(iSocketServer, addr);
	if(rerr == KErrNone)
		{
		iNotifier.CancelRequests();
		iNotifier.RequestNotifications(0xffffffff, iPhyLinkAdapter);
		}
	return rerr;
	}

void CBTTestConnection::ConnectBAPComplete(TInt result)
	{
	if(result)
		{
		iParent->test.Printf(_L("\nConnect BAP Completed with error: %d"), result);
		iProfile.iState = TConnProfile::EConnFailed;
		iProfile.iErrorCode = result;
		}
	else
		{
		iProfile.iState = TConnProfile::EConnConnected;
		TBTDevAddr addr = iProfile.iRemoteBTAddr.BTAddr();
		
		TInt rerr = SetupPhysAdapter();
		if(rerr == KErrNone)
			{
			TRAP_IGNORE(iMetrics = CBluetoothPhysicalLinkMetrics::NewL(*this, iSocketServer, addr));
			}
		else
			{
			iProfile.iErrorCode = rerr;	
			}
		}
	iParent->Draw();
	}

TInt CBTTestConnection::SendEchoRequest(TInt aPayloadSize)
	{
	TInt rerr = KErrNone;
	if(iProfile.iIsSocketBased)
		{
		if (aPayloadSize == 0)
			{
			rerr = iBTSocket->Ioctl(KL2CAPEchoRequestIoctl, NULL, KSolBtL2CAP);
			}
		else
			{
			iEchoDataBuffer.Fill(0x28, aPayloadSize);
			iEchoDataBuffer.SetLength(aPayloadSize);	
			rerr = iBTSocket->Ioctl(KL2CAPEchoRequestIoctl, &iEchoDataBuffer, KSolBtL2CAP);
			}
		}
	return rerr;
	}

void CBTTestConnection::CancelAccept()
	{
	if(iProfile.iAccepterIx >= 0 && iParent->GetConn(iProfile.iAccepterIx))
		{
		(iParent->GetConn(iProfile.iAccepterIx))->iProfile.iState = TConnProfile::EConnFailed;
		}
	iBTSocket->CancelAccept();
	iProfile.iState = TConnProfile::EConnListening;
	}

void CBTTestConnection::CancelConnect()
	{
	if(iProfile.iIsSocketBased)
		{
		iBTSocket->CancelConnect();
		}
	else
		{
		iBTPhyLinks->CancelCreateConnection();
		}
	}

TInt CBTTestConnection::ShutdownSocket(RSocket::TShutdown aShutdownType)
	{
	TInt rerr = KErrNone;
	if(iProfile.iIsSocketBased)
		{
		__ASSERT_ALWAYS(iBTSocket, Panic(EBTConsoleNoSocketForSocketBasedConnection));
		rerr = iBTSocket->Shutdown(aShutdownType);
		iProfile.iState = (rerr == KErrNone) ? TConnProfile::EConnDisconnected :
		                                       TConnProfile::EConnFailed;
		iProfile.iErrorCode = rerr;		
		}
	else
		{
		Panic(EBTConsoleShutdownCalledOnNonCBluetoothSocket);
		}
	
	return rerr;	
	}
	
TInt CBTTestConnection::IncreaseRTXTimer(TUint aTime)
	{
	TInt rerr = KErrNone;
	if(iProfile.iIsSocketBased)
		{
		__ASSERT_ALWAYS(iBTSocket, Panic(EBTConsoleNoSocketForSocketBasedConnection));
		rerr = iBTSocket->SetOpt(KL2CAPRTXTimer, KSolBtL2CAP, aTime);
		iProfile.iErrorCode = rerr;		
		}
	else
		{
		Panic(EBTConsoleShutdownCalledOnNonCBluetoothSocket);
		}
	
	return rerr;	
	}

TInt CBTTestConnection::DisconnectLink()
	{
	TInt rerr = KErrNone;
	if(!iProfile.iIsSocketBased)
		{
		if(iProfile.iIsSCO)
			{
			rerr = iBTSyncLink->Disconnect();			
			}
		else
			{
			rerr = iBTPhyLinks->Disconnect(iProfile.iRemoteBTAddr.BTAddr());
			}

		iProfile.iState = (rerr == KErrNone) ? TConnProfile::EConnDisconnected :
		                                       TConnProfile::EConnFailed;
		iProfile.iErrorCode = rerr;		
		}
	else
		{
		Panic(EBTConsoleDisconnectCalledOnCBluetoothSocket);
		}
	
	return rerr;	
	}

void CBTTestConnection::RequestNotification(TUint32 aNotification)
	{
	TInt rerr = KErrNone;
	if(iProfile.iIsSocketBased)
		{
		rerr = KErrNotFound;
		if(iBTSocket)
			{
			rerr = iBTSocket->ActivateBasebandEventNotifier(aNotification);
			}
		}

	if(rerr != KErrNone)
		{
		iParent->test.Printf(_L("Request Notification [%d], Error: %d\n"), aNotification, rerr);
		}
	}

void CBTTestConnection::OutputOneShotEventNotification(TBTBasebandEventNotification & aNotification)
	{
	iNotifier.RequestNotifications(0xffffffff, iPhyLinkAdapter);
	OutputEventNotification(aNotification);
	}
	
void CBTTestConnection::OutputEventNotification(TBTBasebandEventNotification & aNotification)
	{
	TConnProfile * profile = iParent->FindPhyRecord(iProfile.iRemoteBTAddr.BTAddr());
	if(!profile)
		{
		return;
		}

	// Store the event.
	StoreEvent(aNotification);

	// Find associated PHY.
	if(ENotifyMaster & aNotification.EventType())
		{
		profile->iRole = TConnProfile::EMaster;
		}
	if(ENotifySlave & aNotification.EventType())
		{
		profile->iRole = TConnProfile::ESlave;
		}

	if(ENotifyActiveMode & aNotification.EventType())
		{
		profile->iMode2 = TConnProfile::EActive;

		if(iParent->iLPMRequestsIfActive == EFalse )
			{
			RemoveLPMRequest();	
			}
		}

	if(ENotifySniffMode & aNotification.EventType())
		{
		profile->iMode2 = TConnProfile::ESniff;
		}
	if(ENotifyParkMode & aNotification.EventType())
		{
		profile->iMode2 = TConnProfile::EPark;
		}
	if(ENotifyHoldMode & aNotification.EventType())
		{
		profile->iMode2 = TConnProfile::EHold;
		}

	if(ENotifyMaxSlots1 & aNotification.EventType())
		{
		profile->iMaxSlots = 1;
		}
	if(ENotifyMaxSlots3 & aNotification.EventType())
		{
		profile->iMaxSlots = 3;
		}
	if(ENotifyMaxSlots5 & aNotification.EventType())
		{
		profile->iMaxSlots = 5;
		}

	TUint16 packetTypes = 0;
	TBool packetEvent = EFalse;

	if(ENotifyPacketsDM1 & aNotification.EventType())
		{
		packetTypes |= EPacketsDM1;
		packetEvent = ETrue;
		}
	if(ENotifyPacketsDM3 & aNotification.EventType())
		{
		packetTypes |= EPacketsDM3;
		packetEvent = ETrue;
		}
	if(ENotifyPacketsDM5 & aNotification.EventType())
		{
		packetTypes |= EPacketsDM5;
		packetEvent = ETrue;
		}
	if(ENotifyPacketsDH1 & aNotification.EventType())
		{
		packetTypes |= EPacketsDH1;
		packetEvent = ETrue;
		}
	if(ENotifyPacketsDH3 & aNotification.EventType())
		{
		packetTypes |= EPacketsDH3;
		packetEvent = ETrue;
		}
	if(ENotifyPacketsDH5 & aNotification.EventType())
		{
		packetTypes |= EPacketsDH5;
		packetEvent = ETrue;
		}
	if(packetEvent)
		{
		profile->iPackets = packetTypes;
		}
/*
	if(EPacketsTypeHV1 & aNotification.EventType())
		{
		iParent->test.Printf(_L("SCO HV1 packet type now supported."));
		}
	if(EPacketsTypeHV2 & aNotification.EventType())
		{
		iParent->test.Printf(_L("SCO HV2 packet type now supported."));
		}
	if(EPacketsTypeHV3 & aNotification.EventType())
		{
		iParent->test.Printf(_L("SCO HV3 packet type now supported."));
		}

	if(ENotifyAuthenticationComplete & aNotification.EventType())
		{
		iParent->test.Printf(_L("Authentification complete."));
		}
*/
	if(ENotifyEncryptionChangeOn & aNotification.EventType())
		{
		profile->iEncryption = ETrue;
		}
	if(ENotifyEncryptionChangeOff & aNotification.EventType())
		{
		profile->iEncryption = EFalse;
		}

	if(ENotifyPhysicalLinkUp & aNotification.EventType())
		{
		profile->iState = TConnProfile::EConnConnected;
		}
	if(ENotifyPhysicalLinkDown & aNotification.EventType())
		{
		profile->iState = TConnProfile::EConnDisconnected;
		}
	if(ENotifyPhysicalLinkError & aNotification.EventType())
		{
		profile->iState = TConnProfile::EConnFailed;
		}
	iParent->Draw();
	}

void CBTTestConnection::CancelBasebandNotifyRequest()
	{
	iPhyLinkAdapter.CancelNextBasebandChangeEventNotifier();
	}


TInt CBTTestConnection::ChangeSupportedPacketTypes(TUint16 aPacketTypes)
	{
	TInt rerr;

	if(iProfile.iIsSocketBased)
		{
		rerr = KErrNotFound;
		if(iBTSocket)
			{
			rerr = iBTSocket->RequestChangeSupportedPacketTypes(aPacketTypes);
			}
		}
	else
		{
		rerr = iPhyLinkAdapter.RequestChangeSupportedPacketTypes(aPacketTypes);
		}

	iParent->test.Printf(_L("Change Supported Packet Request [%d], Error: %d\n"), aPacketTypes, rerr);
	return rerr;
	}


TInt CBTTestConnection::RoleChangeAllowed(TBool aAllowed)
	{
	TInt rerr;
	if(aAllowed)
		{
		iParent->test.Printf(_L("Accept role change requests"));
		if(iProfile.iIsSocketBased)
			{
			rerr = KErrNotFound;
			if(iBTSocket)
				{
				rerr = iBTSocket->AllowRoleSwitch();
				}
			}
		else
			{
			rerr = iPhyLinkAdapter.AllowRoleSwitch();
			}
		}
	else
		{
		iParent->test.Printf(_L("Prevent role change requests"));
		if(iProfile.iIsSocketBased)
			{
			rerr = KErrNotFound;
			if(iBTSocket)
				{
				rerr = iBTSocket->PreventRoleSwitch();
				}
			}
		else
			{
			rerr = iPhyLinkAdapter.PreventRoleSwitch();
			}
		}
	iParent->test.Printf(_L("Role Change Allowed Request, Error: %d\n"), rerr);
	return rerr;
	}


TInt CBTTestConnection::RequestRoleChange(TBTBasebandRole aRole)
	{
	TInt rerr;
	if(aRole == EMaster)
		{
		iParent->test.Printf(_L("Request Role Change to Master\n"));
		if(iProfile.iIsSocketBased)
			{
			rerr = KErrNotFound;
			if(iBTSocket)
				{
				rerr = iBTSocket->RequestMasterRole();
				}
			}
		else
			{
			rerr = iPhyLinkAdapter.RequestMasterRole();
			}
		}	
	else
		{
		iParent->test.Printf(_L("Request Role Change to Slave\n"));
		if(iProfile.iIsSocketBased)
			{
			rerr = KErrNotFound;
			if(iBTSocket)
				{
				rerr = iBTSocket->RequestSlaveRole();
				}
			}
		else
			{
			rerr = iPhyLinkAdapter.RequestSlaveRole();
			}
		}
	iParent->test.Printf(_L("Role Change Request, Error: %d\n"), rerr);
	return rerr;
	}
 	
TInt CBTTestConnection::RequestAuthentication()
 	{
 	TInt rerr;
 	iParent->test.Printf(_L("Requesting Authentication\n"));
 	if(iProfile.iIsSocketBased)
 		{
 		rerr = KErrNotFound;
 		iParent->test.Printf(_L("Can't request authentication on a socket based connection\n"));
 		}
 	else
 		{
 		rerr = iPhyLinkAdapter.Authenticate();
 		}
 	iParent->test.Printf(_L("Authentication Request, Error: %d\n"), rerr);
 	return rerr;
 	}

void CBTTestConnection::StartACLData()
	{
	iTime.HomeTime();
	iDataDialogCount = 0;
	ContinueDialog();
	}
	
void CBTTestConnection::StopACLData()
	{
	iBTSocket->CancelSend();
	iBTSocket->CancelRecv();
	}


TBool CBTTestConnection::SendComplete(TInt aIndex, TInt aStatus)
	{
	if(aStatus != KErrNone)
		{
		iParent->test.Printf(_L("\nSend[%d] Completed with Error: %d.\n"), aIndex, aStatus);
		return EFalse;
		}
	return ETrue;
	}

TBool CBTTestConnection::RecvComplete(TInt aIndex, TInt aStatus, TInt aDataErr)
	{
	TBool rCode = ETrue;
	if(aStatus != KErrNone)
		{
		iParent->test.Printf(_L("\nRevc[%d] Completed with Error: %d.\n"), aIndex, aStatus);
		rCode = EFalse;
		}
	else
		{
		if(aDataErr == KErrCompletion)
			{
			iDataDialogCount++;
			}
		else
			{
			if(aDataErr != KErrNone)
				{
				iParent->test.Printf(_L("\nRevc[%d] Completed. Data Error.\n"), aIndex);
				rCode = EFalse;
				}
			}
		}
	return rCode;
	}


TInt CBTTestConnection::RequestLPM(TConnProfile::TConnMode aLPM)
	{
	TInt rerr = KErrNone;

	switch(aLPM)
		{
		case TConnProfile::EActive:
			if(iProfile.iIsSocketBased)
				{
				__ASSERT_DEBUG(iBTSocket, Panic(EBTConsoleNoSocketForSocketBasedConnection));
				rerr = iBTSocket->CancelLowPowerModeRequester();
				}
			else
				{
				rerr = iPhyLinkAdapter.CancelLowPowerModeRequester();
				}
			break;

		case TConnProfile::ESniff:
			if(iProfile.iIsSocketBased)
				{
				__ASSERT_DEBUG(iBTSocket, Panic(EBTConsoleNoSocketForSocketBasedConnection));
				rerr = iBTSocket->ActivateSniffRequester();
				}
			else
				{
				rerr = iPhyLinkAdapter.ActivateSniffRequester();
				}
			break;

		case TConnProfile::EPark:
			if(iProfile.iIsSocketBased)
				{
				__ASSERT_DEBUG(iBTSocket, Panic(EBTConsoleNoSocketForSocketBasedConnection));
				rerr = iBTSocket->ActivateParkRequester();
				}
			else
				{
				rerr = iPhyLinkAdapter.ActivateParkRequester();
				}
			break;

		case TConnProfile::EExplicitActive:
			if(iProfile.iIsSocketBased)
				{
				Panic(EBTConsoleActivateActiveRequesterCalledOnCBluetoothSocket);
				}
			else
				{
				rerr = iPhyLinkAdapter.ActivateActiveRequester();
				}
			break;

		default:
			iParent->test.Printf(_L("Request for mode %S not handled."), &TConnProfile::ModeName(aLPM));
			break;	
		};
	iParent->test.Printf(_L("%S Request Issued. Error: %d"), &TConnProfile::ModeName(aLPM), rerr);
	return rerr;
	}

TInt CBTTestConnection::RemoveLPMRequest()
	{
	TInt rerr = KErrNone;
		
	if(iProfile.iIsSocketBased)
		{
		__ASSERT_DEBUG(iBTSocket, Panic(EBTConsoleNoSocketForSocketBasedConnection));
		rerr = iBTSocket->CancelLowPowerModeRequester();
		}
	else
		{
		rerr = iPhyLinkAdapter.CancelLowPowerModeRequester();
		}
		
	if(rerr == KErrNone)
		{
		iProfile.iMode2 = TConnProfile::EActive;
		}

	iParent->test.Printf(_L("Low Power Mode Request Cleared. Error: %d"), rerr);
	return rerr;
	}
	
	
TInt CBTTestConnection::SupportLPM(TBTLinkMode aLPM) 
	{
	TInt rerr = KErrNone;
	if(iProfile.iIsSocketBased)
		{
		rerr = KErrNotFound;
		if(iBTSocket)
			{
			rerr = iBTSocket->AllowLowPowerModes(aLPM);
			}
		}
	else
		{
		rerr = iPhyLinkAdapter.AllowLowPowerModes(aLPM);
		}

	iParent->test.Printf(_L("Request to support %S. Error: %d"), &TConnProfile::ModeName(aLPM), rerr);
	return rerr;
	}


TInt CBTTestConnection::PreventLPM(TBTLinkMode aLPM) 
	{
	TInt rerr = KErrNone;
	if(iProfile.iIsSocketBased)
		{
		rerr = KErrNotFound;
		if(iBTSocket)
			{
			rerr = iBTSocket->PreventLowPowerModes(aLPM);
			}
		}
	else
		{
		rerr = iPhyLinkAdapter.PreventLowPowerModes(aLPM);
		}
	iParent->test.Printf(_L("Request to prevent %S. Error: %d"), &TConnProfile::ModeName(aLPM), rerr);
	return rerr;
	}

TInt CBTTestConnection::KillAllLinks(TInt /*aReason*/)
	{
	TInt rerr = KErrNone;
	if(!iBTPhyLinks)
		{
		TRAP(rerr, iBTPhyLinks = CBluetoothPhysicalLinks::NewL(*this, iSocketServer));
		}

	if(rerr == KErrNone)
		{
		rerr = iBTPhyLinks->DisconnectAll();
		}
		
	return rerr;
	}

TInt CBTTestConnection::KillLink(TInt /*aReason*/)
	{
	TInt rerr = KErrNone;
	if(!iBTPhyLinks)
		{
		TRAP(rerr, iBTPhyLinks = CBluetoothPhysicalLinks::NewL(*this, iSocketServer));
		}
	if(rerr == KErrNone)
		{
		rerr = iBTPhyLinks->Disconnect(iProfile.iRemoteBTAddr.BTAddr());
		}
		
	return rerr;
	}

TInt CBTTestConnection::ToggleAutoSniff()
	{
	TInt rerr = KErrNone;

	iAutoSniff = !iAutoSniff;
	
	rerr = iBTSocket->SetAutomaticSniffMode(iAutoSniff);
	if (!rerr)
		{
		iProfile.iAutoSniffActive = iAutoSniff;
		}

		
	return rerr;
	}

TInt CBTTestConnection::PassiveSCO()
	{
	TInt rerr = KErrNone;
	iProfile.iIsSocketBased = EFalse;
	if(!iBTSyncLink)
		{
		TRAP(rerr, iBTSyncLink = CBluetoothSynchronousLink::NewL(iSyncLinkHandler, iSocketServer));
		}
	if(rerr == KErrNone)
		{	                                               
		rerr = iBTSyncLink->AcceptConnection();
		}

	iProfile.iIsSCO = ETrue;
	iProfile.iState = (rerr == KErrNone) ? TConnProfile::EConnAccepting :
	                                       TConnProfile::EConnFailed;
	iProfile.iErrorCode = rerr;		

	return rerr;
	}

TInt CBTTestConnection::ActiveSCO(TBTDevAddr& aAddr)
	{
	TInt rerr = KErrNone;
	iProfile.iIsSocketBased = EFalse;
	if(!iBTSyncLink)
		{
		TRAP(rerr, iBTSyncLink = CBluetoothSynchronousLink::NewL(iSyncLinkHandler, iSocketServer));
		}
	if(rerr == KErrNone)
		{
		rerr = iBTSyncLink->SetupConnection(aAddr);
		iProfile.iRemoteBTAddr.SetBTAddr(aAddr);
		}

	iProfile.iIsSCO = ETrue;
	iProfile.iState = (rerr == KErrNone) ? TConnProfile::EConnConnecting :
	                                       TConnProfile::EConnFailed;
	iProfile.iErrorCode = rerr;		

	return rerr;
	}

TInt CBTTestConnection::PassiveESCO()
	{
	TInt rerr = KErrNone;
	iProfile.iIsSocketBased = EFalse;
	if(!iBTSyncLink)
		{
		TRAP(rerr, iBTSyncLink = CBluetoothSynchronousLink::NewL(iSyncLinkHandler, iSocketServer));
		}
	if(rerr == KErrNone)
		{ 
		rerr = iBTSyncLink->AcceptConnection(TBTSyncPackets(TBTSyncPackets::ESyncAnySCOPacket | 
		                                                    TBTSyncPackets::ESyncAnyESCOPacket));		                                               
		}

	iProfile.iIsSCO = ETrue;
	iProfile.iState = (rerr == KErrNone) ? TConnProfile::EConnAccepting :
	                                       TConnProfile::EConnFailed;
	iProfile.iErrorCode = rerr;		

	return rerr;
	}

TInt CBTTestConnection::ActiveESCO(TBTDevAddr& aAddr)
	{
	TInt rerr = KErrNone;
	iProfile.iIsSocketBased = EFalse;
	if(!iBTSyncLink)
		{
		TRAP(rerr, iBTSyncLink = CBluetoothSynchronousLink::NewL(iSyncLinkHandler, iSocketServer));
		}
	if(rerr == KErrNone)
		{
		iBTSyncLink->SetupConnection(aAddr,TBTSyncPackets(TBTSyncPackets::ESyncAnyESCOPacket));
		iProfile.iRemoteBTAddr.SetBTAddr(aAddr);
		}

	iProfile.iIsSCO = ETrue;
	iProfile.iState = (rerr == KErrNone) ? TConnProfile::EConnConnecting :
	                                       TConnProfile::EConnFailed;
	iProfile.iErrorCode = rerr;		

	return rerr;
	}
	
void CBTTestConnection::StoreEvent(TBTBasebandEventNotification & aEvent)
	{
	TBTQueuedBasebandEventNotification * e = new TBTQueuedBasebandEventNotification(aEvent);
	iEventNotificationQueue.AddLast(*e);
	}

TBool CBTTestConnection::PrintEvents(TInt aCurrentEvent, TInt aNumEvents)
	{
	iParent->DrawConnections();
	iParent->test.Printf(_L("\n\n ----------- Event List -----------\n"));
	
	TInt count = 0;
	
	if(iEventNotificationQueue.IsEmpty())
		{
		return ETrue;
		}

	TSglQueIter<TBTQueuedBasebandEventNotification> iter(iEventNotificationQueue);
	while (iter && count < (aCurrentEvent + aNumEvents))
		{
		TBTQueuedBasebandEventNotification * e = iter++;
		if(count++ < aCurrentEvent)
			{
			continue;
			}

		if(ENotifyMaster & e->EventType())
			{
			iParent->test.Printf(_L("Role changed to MASTER."));
			}
		if(ENotifySlave & e->EventType())
			{
			iParent->test.Printf(_L("Role changed to SLAVE."));
			}

		if(ENotifyActiveMode & e->EventType())
			{
			iParent->test.Printf(_L("Mode changed to ACTIVE."));
			}
		if(ENotifySniffMode & e->EventType())
			{
			iParent->test.Printf(_L("Mode changed to SNIFF."));
			}
		if(ENotifyParkMode & e->EventType())
			{
			iParent->test.Printf(_L("Mode changed to PARK."));
			}
		if(ENotifyHoldMode & e->EventType())
			{
			iParent->test.Printf(_L("Mode changed to HOLD."));
			}

		if(ENotifyMaxSlots1 & e->EventType())
			{
			iParent->test.Printf(_L("Max Slots changed to 1."));
			}
		if(ENotifyMaxSlots3 & e->EventType())
			{
			iParent->test.Printf(_L("Max Slots changed to 3."));
			}
		if(ENotifyMaxSlots5 & e->EventType())
			{
			iParent->test.Printf(_L("Max Slots changed to 5."));
			}

		if(ENotifyPacketsDM1 & e->EventType() ||
		   ENotifyPacketsDM3 & e->EventType() ||
		   ENotifyPacketsDM5 & e->EventType() ||	
		   ENotifyPacketsDH1 & e->EventType() ||	
		   ENotifyPacketsDH3 & e->EventType() ||	
		   ENotifyPacketsDH5 & e->EventType())
			{
			iParent->test.Printf(_L("Packet Types now supported: "));

			if(ENotifyPacketsDM1 & e->EventType())
				{
				iParent->test.Printf(_L("DM1 "));				
				}
			if(ENotifyPacketsDM3 & e->EventType())
				{
				iParent->test.Printf(_L("DM3 "));
				}
			if(ENotifyPacketsDM5 & e->EventType())
				{
				iParent->test.Printf(_L("DM5 "));
				}
			if(ENotifyPacketsDH1 & e->EventType())
				{
				iParent->test.Printf(_L("DH1 "));
				}
			if(ENotifyPacketsDH3 & e->EventType())
				{
				iParent->test.Printf(_L("DH3 "));
				}
			if(ENotifyPacketsDH5 & e->EventType())
				{
				iParent->test.Printf(_L("DH5 "));
				}
			}

		if(ENotifyAuthenticationComplete & e->EventType())
			{
			iParent->test.Printf(_L("Authentification complete."));
			}

		if(ENotifyEncryptionChangeOn & e->EventType())
			{
			iParent->test.Printf(_L("Encryption Enabled."));
			}
		if(ENotifyEncryptionChangeOff & e->EventType())
			{
			iParent->test.Printf(_L("Encryption Disabled."));
			}

		if(ENotifyPhysicalLinkUp & e->EventType())
			{
			iParent->test.Printf(_L("Physical Link Up."));
			}
		if(ENotifyPhysicalLinkDown & e->EventType())
			{
			iParent->test.Printf(_L("Physical Link Down."));
			}
		if(ENotifyPhysicalLinkError & e->EventType())
			{
			iParent->test.Printf(_L("Physical Link Error."));
			}
		
 		if(ENotifySynchronousLinkUp & e->EventType())
 			{
 			iParent->test.Printf(_L("Synchronous Link Up."));
 			}
 
 		if(ENotifySynchronousLinkDown & e->EventType())
 			{
 			iParent->test.Printf(_L("Synchronous Link Down."));
 			}
   
 		if(ENotifySynchronousLinkError & e->EventType())
 			{
 			iParent->test.Printf(_L("Synchronous Link Error."));
 			}

		iParent->test.Printf(_L("  - Error Code: %d\n"), e->ErrorCode());
		}

	if(iter)
		{
		return (count < (aCurrentEvent + aNumEvents));
		}
	else
		{
		return ETrue;
		}
	}

void CBTTestConnection::ClearEventStore()
	{
	TSglQueIter<TBTQueuedBasebandEventNotification> iter(iEventNotificationQueue);
	while (iter)
		{
		TBTQueuedBasebandEventNotification * e = iter++;
		iEventNotificationQueue.Remove(*e);
		delete e;
		}
	}

void CBTTestConnection::HandleConnectCompleteL(TInt aErr)
	{
	ConnectComplete(aErr);
	}

void CBTTestConnection::HandleAcceptCompleteL(TInt aErr)
	{
	AcceptComplete(aErr);
	}

void CBTTestConnection::HandleShutdownCompleteL(TInt aErr)
	{
	iParent->test.Printf(_L("Shutdown Complete. Error[%d]."), aErr);
	}

void CBTTestConnection::HandleSendCompleteL(TInt aErr)
	{
	TBool ok = ETrue;
	switch(iProfile.iData)
		{
		case TConnProfile::EDataIdle:
			if(aErr == KErrCancel)
				{
				iParent->test.Printf(_L("Send Cancelled. Data[%d]."), iDataDialogCount);
				}
			else
				{
				__DEBUGGER();
				}
			break;
			
		case TConnProfile::EDataDialog1:
		case TConnProfile::EDataDialog2:
		case TConnProfile::EDataSend:
		case TConnProfile::EDataSendReceive:

			ok = SendComplete(iDataDialogCount++, aErr);

			if(aErr == KErrDisconnected)
				{
				iProfile.iState = TConnProfile::EConnServerDisconnected;
				}

			if(ok) 
				{
				if (iProfile.iData == TConnProfile::EDataSendReceive)
					{
					DoSend(TUint8(iDataDialogCount&0xff));
					}
				else
					{
					ContinueDialog();					
					}
				}
			else
				{
				iProfile.iData = TConnProfile::EDataFailed;
				}
			break;

		case TConnProfile::EDataSendOneShot:
			iParent->test.Printf(_L("\nOne Shot Send Completed with Error: %d.\n"), aErr);
			iProfile.iData = TConnProfile::EDataIdle;
			break;
					
		case TConnProfile::EDataRecv:
		case TConnProfile::EDataRecvOneShot:
			__DEBUGGER();
			break;
		case TConnProfile::ERecvContinuationData:
			__DEBUGGER();
			break;
		default:
			iParent->test.Printf(_L("\nData Send Completed invalid state [%d], Error [%d]\n"), 
			                     iProfile.iData, aErr);
			break;		
		};
	}
	
void CBTTestConnection::HandleReceiveCompleteL(TInt aErr)
	{
	switch(iProfile.iData)
		{
		case TConnProfile::EDataIdle:
			if(aErr == KErrCancel)
				{
				iParent->test.Printf(_L("Recv Cancelled. Data[%d]."), iDataDialogCount);
				}
			else
				{
				__DEBUGGER();
				}
			break;
			
		case TConnProfile::EDataDialog1:
		case TConnProfile::EDataDialog2:
		case TConnProfile::EDataRecv:
		case TConnProfile::EDataRecvAny:
		case TConnProfile::EDataSendReceive:
			{
			TBool ok = RecvComplete(iDataDialogCount, aErr, CheckRecvData());
			
			if(aErr == KErrDisconnected)
				{
				iProfile.iState = TConnProfile::EConnServerDisconnected;
				}

			if(ok) 
				{
				TInt len = iRecvBuffer.Length();
//				iParent->test.Printf(_L("\nRecv Completed.  Length[%d], Value[%d]\n"), len, iRecvBuffer[0]);

				if(!(iDataDialogCount & 0xff))
					{
					TTime temp;
					temp.HomeTime();
					TTimeIntervalMicroSeconds diff = temp.MicroSecondsFrom(iTime);

					iParent->test.Printf(_L("\nRecv data %d"), diff.Int64());
					iTime.HomeTime();
					}

				if (iProfile.iData == TConnProfile::EDataSendReceive)
					{
					DoRecv();
					}
				else
					{
					ContinueDialog();					
					}
				}
			else
				{
				iProfile.iData = TConnProfile::EDataFailed;
				}
			}
			break;

		case TConnProfile::EDataRecvOneShot:
			{
			iParent->test.Printf(_L("\nOne Shot Recv Completed with Error: %d.\n"), aErr);
			if(aErr == KErrNone)
				{
				TInt bufLen = iRecvBuffer.Length();
				if((bufLen - 1)%10 != iRecvBuffer[bufLen - 1])
					{
					iParent->test.Printf(_L("\nData Error: Invalid Data Payload. Recv Byte 0[%d], Recv End %d[%d]\n"),
		                                    iRecvBuffer[0], bufLen, iRecvBuffer[bufLen - 1]);
					}
				else
					{
					iParent->test.Printf(_L("\nOne Shot Recv Completed.  Length[%d], Value[%d]\n"), bufLen, iRecvBuffer[0]);
					}
				iProfile.iData = TConnProfile::EDataIdle;
				}
			}
			break;
						
		case TConnProfile::EDataSend:
		case TConnProfile::EDataSendOneShot:
			__DEBUGGER();
			break;
		case TConnProfile::ERecvContinuationData:
			{
			iParent->test.Printf(_L("\nData Recv Completed with Error: %d."), aErr);
			if(aErr == KErrNone)
				{
				iParent->test.Printf(_L("\nThe Recv Data Sequence is: \n"));
				TInt len = iCustomSendRecvBuffer.Length();
				for(TInt i = 0;i<len;i++)
					{
					iParent->test.Printf(_L("%d"),iCustomSendRecvBuffer[i]);
					}
				iTotalRecvByte += len;

				iParent->test.Printf(_L("\n\n%x bytes recieved this time.\n"),len);

				iParent->test.Printf(_L("%x bytes recieved so far.\n"),iTotalRecvByte);

				iParent->test.Printf(_L("%x bytes left in buffer.\n"),iRecvBufferLength[0]);

				iParent->test.Printf(_L("%x bytes in total.\n"),iRecvBufferLength[0]+iTotalRecvByte);
				/*
				iTailDigit == -1 when receiving data for the first time,
				Otherwise, it equals to the last digit from previous receive 
				*/
				if(iTailDigit != -1)
					{
					if((iTailDigit+1)%10 == iCustomSendRecvBuffer[0])
						{
						iParent->test.Printf(_L("\nData are continuous.\n"));
						}
					else
						{
						iParent->test.Printf(_L("\nData are lost during transfer.\nContinuation is broken!"));
						}
					}
				if (iRecvBufferLength[0] == 0)
					{
					iTailDigit = -1;
					iTotalRecvByte = 0;
					}
				else
					{
					iTailDigit = iCustomSendRecvBuffer[len - 1];
					}
					
				//if there is anything left in the buffer to receive
				if(iRecvBufferLength[0] != 0)
					RecvContinuationData(len);
				//buffer is empty, stop receiving
				else
					iProfile.iData = TConnProfile::EDataIdle;
				}
			break;
			}
		default:
			iParent->test.Printf(_L("\nData Recv Completed invalid state [%d], Error [%d]\n"), 
			                     iProfile.iData, aErr);
			break;		
		};
	}


void CBTTestConnection::HandleIoctlCompleteL(TInt aErr)
	{
	switch(iCurrentIOCTL)
		{
		case ERFCOMMConfigInd:
			{
			iParent->test.Printf(_L("RFCOMM Config Ind Ioctl Complete. Error[%d].\n"), aErr);

			TUint8 modemStatus = 0;
			TPckgBuf<TUint8> pkg(modemStatus);
			TInt err = iBTSocket->GetOpt(KRFCOMMLocalModemStatus, KSolBtRFCOMM, pkg);
			modemStatus = pkg();
			iParent->test.Printf(_L("\nLocalModemStatus: Err = %d, Status = %x"), err, modemStatus);
			}
			break;

		default:	
			iParent->test.Printf(_L("Ioctl Complete. Error[%d]."), aErr);
			break;
		};
	}

void CBTTestConnection::HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification)
	{
	if(aErr == KErrNone)
		{
		OutputEventNotification(aEventNotification);
		}
	else
		{
		iParent->test.Printf(_L("Event Notifier Complete. Error[%d]."), aErr);
		}
	}

// MBluetoothPhysicalLinksNotifier definitions
void CBTTestConnection::HandleCreateConnectionCompleteL(TInt aErr)
	{
	ConnectBAPComplete(aErr);
	}

void CBTTestConnection::HandleDisconnectCompleteL(TInt aErr)
	{
	iParent->test.Printf(_L("Disconnect Complete. Error[%d]."), aErr);
	}

void CBTTestConnection::HandleDisconnectAllCompleteL(TInt aErr)
	{
	iParent->test.Printf(_L("Disconnect All Complete. Error[%d]."), aErr);
	}

// MBluetoothSynchronousLinkNotifier definitions
void CBTTestConnection::HandleSyncSetupConnectionCompleteL(TInt aErr)
	{
	if(aErr != KErrNone)
		{
		iParent->test.Printf(_L("\nSCO(eSCO) Connection Completed with error: %d"), aErr);
		iProfile.iState = TConnProfile::EConnFailed;
		iProfile.iErrorCode = aErr;
		}
	else
		{
		iProfile.iState = TConnProfile::EConnConnected;
		}
	iParent->Draw();
	}
	
void CBTTestConnection::HandleSyncDisconnectionCompleteL(TInt aErr)
	{
	iParent->test.Printf(_L("HandleSyncDisconnectionCompleteL. Error[%d]."), aErr);
	}

void CBTTestConnection::HandleSyncAcceptConnectionCompleteL(TInt aErr)
	{
	if(aErr != KErrNone)
		{
		iParent->test.Printf(_L("\nSCO(eSCO) Accept Completed with error: %d"), aErr);
		iProfile.iState = TConnProfile::EConnFailed;
		iProfile.iErrorCode = aErr;
		}
	else
		{
		iBTSyncLink->RemoteName(iProfile.iRemoteBTAddr);
		iProfile.iState = TConnProfile::EConnConnected;
		}
	iParent->Draw();
	}

void CBTTestConnection::HandleSyncSendCompleteL(TInt aErr)
	{
	iParent->test.Printf(_L("HandleSyncSendCompleteL. Error[%d]."), aErr);	
	}

void CBTTestConnection::HandleSyncReceiveCompleteL(TInt aErr)
	{
	iParent->test.Printf(_L("HandleSyncReceiveCompleteL. Error[%d]."), aErr);	
	}

void CBTTestConnection::MbplmoRssiChanged(TInt8 aRssi)
	{
	iMetricsConsole->Printf(_L("RSSI: %d\n"), aRssi);
	}

void CBTTestConnection::MbplmoLinkQualityChanged(TUint8 aLinkQuality)
	{
	iMetricsConsole->Printf(_L("Link Quality: %d\n"), aLinkQuality);
	}

void CBTTestConnection::MbplmoFailedContactCounterChanged(TUint16 aFailedContactCounter)
	{
	iMetricsConsole->Printf(_L("Failed Contact Counter: %d\n"), aFailedContactCounter);
	}

void CBTTestConnection::MbplmoTransmitPowerLevelChanged(TInt8 aTransmitPowerLevel)
	{
	iMetricsConsole->Printf(_L("Transmit Power Level: %d\n"), aTransmitPowerLevel);
	}

void CBTTestConnection::MbplmoError(TInt aError)
	{
	iMetricsConsole->Printf(_L("Error received: %d\n"), aError);
	}

TInt CBTTestConnection::CheckRecvData()
	{
	TInt rerr = KErrCompletion;

	if(iProfile.iIsStreamProtocol)
		{
		TInt newLength = iStreamRecvBuffer.Length() + iRecvBuffer.Length();
		if(newLength > iProfile.iRecvMTUSize)
			{
			iParent->test.Printf(_L("\nData Error: Stream Buffer error. Expected[Actual] Size = %d[%d]"),
			                     iProfile.iRecvMTUSize, newLength);
			iStreamRecvBuffer.Zero();
			rerr = KErrOverflow;
			}
		else
			{
			iStreamRecvBuffer.Append(iRecvBuffer);

			if(newLength == iProfile.iRecvMTUSize)
				{
				iRecvBuffer = iStreamRecvBuffer;
				iStreamRecvBuffer.Zero();
				}
			else
				{
				rerr = KErrNone;
				}
			}
		}

	// Skip data checks if we're doing '/dev/null' style receive.
	if (iProfile.iData != TConnProfile::EDataRecvAny && iProfile.iData != TConnProfile::EDataSendReceive)
		{
		// Check that the packet is the expected length,
		// encoded in bytes 1 and 2 of the received packet
		if(rerr == KErrCompletion)
			{
			TUint16 expectedLength = 0;
			expectedLength  = iRecvBuffer[2];        // Lo byte of expected length in byte 2
			expectedLength += iRecvBuffer[1] << 8;   // Hi byte of expected length in byte 1
			if (expectedLength != iRecvBuffer.Length())
				{
				iParent->test.Printf(_L("\nData Error: Invalid Data Payload. Expected length %d received length %d"),
				                        expectedLength, iRecvBuffer.Length());
				if (iRecvBuffer.Length() > expectedLength)
					{
					rerr = KErrOverflow;
					}
				else
					{
					rerr = KErrUnderflow;
					}
				}
		
			// Check that the final byte is same as first byte
			if(iRecvBuffer[0] != iRecvBuffer[iRecvBuffer.Length() - 1])
				{
				iParent->test.Printf(_L("\nData Error: Invalid Data Payload. Expected[Recv Byte 0, Recv End N] %d[%d, %d]"),
				                        (iDataDialogCount&0xff), iRecvBuffer[0], iRecvBuffer[iRecvBuffer.Length() - 1]);
				rerr = KErrCorrupt;
				}
			}
		
		// Check that the packet received is in the correct order
		if(rerr == KErrCompletion)
			{
			if(iRecvBuffer[0] != (iDataDialogCount&0xff))
				{
				switch(iProfile.iData)
					{
					case TConnProfile::EDataDialog1:
					case TConnProfile::EDataDialog2:
						iParent->test.Printf(_L("\nData Error: Packet OOS. Recv[Expected] %d[%d]"), iRecvBuffer[0], (iDataDialogCount&0xff));
						rerr = KErrTotalLossOfPrecision;
						break;
											
					case TConnProfile::EDataRecv:
						iParent->test.Printf(_L("\nData Error: Packet OOS. Recv[Expected] %d[%d]"), iRecvBuffer[0], (iDataDialogCount&0xff));
						iParent->test.Printf(_L("\nPacket [%d] lost/flushed, next expected [%d]"), (iDataDialogCount&0xff), (iRecvBuffer[0] + 1));
						iDataDialogCount = iRecvBuffer[0];
						break;
						
					default:
						__DEBUGGER();
						break;
					};
				}
			}
		} // !TConnProfile::EDataRecvAny
	return rerr;
	}


void CBTTestConnection::ContinueDialog()	
	{
	switch(iProfile.iData)
		{
		case TConnProfile::EDataIdle:
			break;
			
		case TConnProfile::EDataDialog1:
		case TConnProfile::EDataDialog2:
			if((iDataDialogCount + iProfile.iData) & 0x1)
				{
				DoRecv();
				}
			else
				{
				DoSend(TUint8(iDataDialogCount&0xff));
				}				
			break;

		case TConnProfile::EDataSend:
			DoSend(TUint8(iDataDialogCount&0xff));
			if(!(iDataDialogCount & 0xff))
				{
				TTime temp;
				temp.HomeTime();
				TTimeIntervalMicroSeconds diff = temp.MicroSecondsFrom(iTime);	

				iParent->test.Printf(_L("\nWriting data %d"), diff.Int64());
				iTime.HomeTime();
				}
			break;
			
		case TConnProfile::EDataRecv:
		case TConnProfile::EDataRecvAny:
			DoRecv();
			break;

		case TConnProfile::EDataSendReceive:
			DoRecv();
			DoSend(TUint8(iDataDialogCount&0xff));
			break;

		default:
			iParent->test.Printf(_L("\nContinueDialog: Invalid data state [%d]"), iProfile.iData);
			__DEBUGGER();
			break;		
		};
	}	

void CBTTestConnection::DoSend(TUint8 aVal)
	{
	// Zero-fill the packet. Then set the first and last bytes to a number
	// starting from zero and incrementing with each packet. This enables
	// checking that they arrive in order. The 2nd and 3rd bytes are set
	// to the high and low bytes of the length of the packet, respectively
	//
	iSendBuffer.FillZ(iProfile.iSendingMTUSize);
	iSendBuffer[0] = aVal;
	iSendBuffer[1] = (iProfile.iSendingMTUSize & 0xFF00) >> 8;   // Hi byte of packet size
	iSendBuffer[2] = (iProfile.iSendingMTUSize & 0x00FF);        // Lo byte of packet size
	iSendBuffer[iProfile.iSendingMTUSize - 1] = aVal;
	TInt rerr = iBTSocket->Write(iSendBuffer);
	if(rerr != KErrNone)
		{
		// This is called both on send and receive completion, so if it's actually receive
		// being completed, we may still have a previous outstanding send.
		if (!(iProfile.iData == TConnProfile::EDataSendReceive && rerr == KErrInUse))
			{
			iProfile.iData = TConnProfile::EDataFailed;
			iParent->test.Printf(_L("\nData Send Error: Value[%d], Error[%d]."), aVal, rerr);
			}
		}
	}

void CBTTestConnection::DoRecv()
	{
	TInt rerr = KErrNone;
	if(iProfile.iIsStreamProtocol)
		{
		rerr = iBTSocket->RecvOneOrMore(iRecvBuffer, 0, iRecvBufferLength);
		}
	else
		{
		rerr = iBTSocket->Read(iRecvBuffer);
		}
	
	if(rerr != KErrNone)
		{
		// This is called both on send and receive completion, so if it's actually send
		// being completed, we may still have a previous outstanding recv.
		if (!(iProfile.iData == TConnProfile::EDataSendReceive && rerr == KErrInUse))
			{
			iProfile.iData = TConnProfile::EDataFailed;
			iParent->test.Printf(_L("\nData Recv Error: Error[%d]."), rerr);
			}
		}
	}

void CBTTestConnection::Panic(TBTTestConnectionPanic aPanic)
	{
	_LIT(KBTTestConnectionPanicName, "Bluetooth Test Console Panic");
	User::Panic(KBTTestConnectionPanicName, aPanic);
	}

void CBTTestConnection::StartPlmDisplayL()
	{
	if (iMetricsConsole)
		{
		User::Leave(KErrAlreadyExists);
		}
	if (!iMetrics)
		{
		User::Leave(KErrNotReady);
		}
	iMetricsConsole = Console::NewL(_L("PLM"), TSize(KConsFullScreen, KConsFullScreen));
	iMetrics->SubscribeRssi();
	iMetrics->SubscribeLinkQuality();
	iMetrics->SubscribeFailedContactCounter();
	iMetrics->SubscribeTransmitPowerLevel();
	}

void CBTTestConnection::StopPlmDisplayL()
	{
	if (!iMetricsConsole)
	{
		User::Leave(KErrArgument);
		}
	iMetrics->Cancel();
	delete iMetricsConsole;
	iMetricsConsole = NULL;
	}

TInt CBTTestConnection::PrintBTPhyConnections()
	{
	TInt rerr = KErrNone;
	if(!iBTPhyLinks)
		{
		TRAP(rerr, iBTPhyLinks = CBluetoothPhysicalLinks::NewL(*this, iSocketServer));
		}

	if(rerr == KErrNone)
		{
		TUint aMaxNumber = 10;
		RBTDevAddrArray aBTDevAddrArray(aMaxNumber);
		
		rerr = iBTPhyLinks->Enumerate(aBTDevAddrArray,aMaxNumber);
		TInt arraySize = aBTDevAddrArray.Count();
		iParent->test.Printf(_L("\nNo of Connected Links: %d\nSource Address:\n"),arraySize);
		if(rerr == KErrNone && arraySize)
			{
			// No Error and we have something to print!
			TBuf<20> buf;
			for(TInt i=0;i<arraySize;i++)
				{
				aBTDevAddrArray[i].GetReadable(buf);
				iParent->test.Printf(_L("\n%S"),&buf);
				}
			}
		}
	return rerr;
	}

TInt CBTTestConnection::GetRemoteName(TNameEntry& aNameEntry)
	{
	
	RHostResolver resolver;
	TInt err = resolver.Open(iSocketServer, KBTAddrFamily, KBTLinkManager);
	if (err==KErrNone)
		{
		TInquirySockAddr address(iPeerSocketAddr);
		address.SetBTAddr(iPeerSocketAddr.BTAddr());
		address.SetAction(KHostResName|KHostResIgnoreCache);
		err = resolver.GetByAddress(address, aNameEntry);
		}
	resolver.Close();	
	return err;
	}

// ----------------------------------------------------------------------------
//							DEBUG METHODS
// ----------------------------------------------------------------------------
#ifdef _DEBUG
// This is copied from L2Types.h
//enum TL2CapChannelMode
//	{
//	EL2CAPBasicMode 			= 0x00,
//	EL2CAPRetransmissionMode 	= 0x01,
//	EL2CAPFlowControlMode 		= 0x02,
//	};

class TL2DataPlaneConfig
	{
public:
	TL2CapChannelMode iLinkMode;
	TUint8 iTxWindowSize;
	TUint8 iMaxTransmit;
	TUint16 iRetransmissionTimeout;
	TUint16 iMonitorTimeout;
	TUint16 iMaximumPDUSize;
	TUint8 iPriority;
	TUint8 iOutboundQueueSize;
	TUint16 iFlushTimeout;
	TUint16 iMaxOutgoingMTU;
	TUint16 iMaxIncomingMTU;
	};

typedef TPckgBuf<TL2DataPlaneConfig> TL2DataPlaneConfigPkg;


TInt CBTTestConnection::DataPlaneConfig()
	{

	TL2DataPlaneConfig conf;
	TL2DataPlaneConfigPkg pckg(conf);

	TInt rerr = iBTSocket->GetOpt(KL2CAPDataPlaneConfig, KSolBtL2CAP, pckg);
	if(rerr == KErrNone)
		{
		conf = pckg();
		iParent->test.Printf(_L("\n\n ***** Data Plane Configuration *****\n"));
		iParent->test.Printf(_L("Priority            - %d\n"), conf.iPriority);
		iParent->test.Printf(_L("Outbound Queue Size - %d\n"), conf.iOutboundQueueSize);
		iParent->test.Printf(_L("Flush Timeout       - %d\n"), conf.iFlushTimeout);
		iParent->test.Printf(_L("Max Outgoing MTU    - %d\n"), conf.iMaxOutgoingMTU);
		iParent->test.Printf(_L("Max Incoming MTU    - %d\n"), conf.iMaxIncomingMTU);

		iParent->test.Printf(_L("Link Mode           - %d\n"), conf.iLinkMode);
		iParent->test.Printf(_L("Tx Window Size      - %d\n"), conf.iTxWindowSize);
		iParent->test.Printf(_L("Max # Retransmit    - %d\n"), conf.iMaxTransmit);
		iParent->test.Printf(_L("Retransmit Timeout  - %d\n"), conf.iRetransmissionTimeout);
		iParent->test.Printf(_L("Monitor Timeout     - %d\n"), conf.iMonitorTimeout);
		iParent->test.Printf(_L("Max PDU Size        - %d\n"), conf.iMaximumPDUSize);
		iParent->test.Printf(_L(" ************************************\n\n"));
		}
	
	TPckgBuf<TInt> getOptBuf;
	TInt getOptValue = 0;
	if(rerr == KErrNone)
		{
		rerr = iBTSocket->GetOpt(KL2CAPNegotiatedOutboundMTU, KSolBtL2CAP, getOptBuf);
		if(rerr == KErrNone)
			{
			getOptValue = getOptBuf();
			iParent->test.Printf(_L("GetOpt Outgoing MTU (Negotiated)    - %d\n"), getOptValue);
			}
		}
	
	if(rerr == KErrNone)
		{
		rerr = iBTSocket->GetOpt(KL2CAPOutboundMTUForBestPerformance, KSolBtL2CAP, getOptBuf);
		if(rerr == KErrNone)
			{
			getOptValue = getOptBuf();
			iParent->test.Printf(_L("GetOpt Outgoing MTU (Best Performance)     - %d\n"), getOptValue);
			}
		}
	
	if(rerr == KErrNone)
		{
		rerr = iBTSocket->GetOpt(KL2CAPInboundMTU, KSolBtL2CAP, getOptBuf);
		if(rerr == KErrNone)
			{
			getOptValue = getOptBuf();
			iParent->test.Printf(_L("GetOpt Incoming MTU	 - %d\n"), getOptValue);
			}
		}

	if(rerr == KErrNone)
		{
		rerr = iBTSocket->GetOpt(KL2CAPGetMaxOutboundMTU, KSolBtL2CAP, getOptBuf);
		if(rerr == KErrNone)
			{
			getOptValue = getOptBuf();
			iParent->test.Printf(_L("GetOpt Max Outgoing MTU - %d\n"), getOptValue);
			}
		}

	if(rerr == KErrNone)
		{
		rerr = iBTSocket->GetOpt(KL2CAPGetMaxInboundMTU, KSolBtL2CAP, getOptBuf);
		if(rerr == KErrNone)
			{
			getOptValue = getOptBuf();
			iParent->test.Printf(_L("GetOpt Max Incoming MTU - %d\n"), getOptValue);
			}
		}
		
	if(rerr == KErrNone)
		{
		rerr = iBTSocket->GetOpt(KL2CAPRTXTimer, KSolBtL2CAP, getOptBuf);
		if(rerr == KErrNone)
			{
			getOptValue = getOptBuf();
			iParent->test.Printf(_L("GetOpt RTXTimerDuration - %d\n"), getOptValue);
			}
		}
	
	if(rerr == KErrNone)
		{
		rerr = iBTSocket->GetOpt(KL2CAPERTXTimer, KSolBtL2CAP, getOptBuf);
		if(rerr == KErrNone)
			{
			getOptValue = getOptBuf();
			iParent->test.Printf(_L("GetOpt ERTXTimerDuration - %d\n"), getOptValue);
			}
		}

	return rerr;
	}

TInt CBTTestConnection::IssueManualFlush()
	{
	TInt msgID = 0;
	TInt rerr = iBTSocket->GetOpt(KL2CAPDebugFlush, KSolBtL2CAP, msgID);

	iParent->test.Printf(_L("Manual Flush Request: MSG ID[%d], Error[%d]"), msgID, rerr);
	return rerr;			
	}
#endif

TConnProfile::TConnProfile()
: iPhyLinkIx(-2),
  iState(EConnIdle),
  iRole(ENone),
  iPackets(EPacketsDM1 | EPacketsDM3 | EPacketsDM5 | EPacketsDH1 | EPacketsDH3 | EPacketsDH5),
  iMaxSlots(5),
  iMode2(TConnProfile::EActive),
  iModeAllowed(ESniffMode | EParkMode | EHoldMode),
  iSwitchAllowed(ETrue),
  iEncryption(EFalse),
  iData(EDataIdle),
  iAccepterIx(-1),
  iIsSocketBased(ETrue),
  iErrorCode(KErrNone),
  iSendingMTUSize(0),
  iRecvMTUSize(0),
  iIsStreamProtocol(EFalse),
  iIsSCO(EFalse),
  iAutoSniffActive(EFalse)
	{}



