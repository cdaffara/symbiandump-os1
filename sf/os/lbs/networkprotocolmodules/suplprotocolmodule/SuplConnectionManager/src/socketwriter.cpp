// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
 @deprecated
*/
#include "suplconnectionmanager.h"
#include "socketwriter.h"
#include "socketreader.h"
#include <lbs/lbshostsettings.h>

#include "suplmessagebase.h"

#include "supldevloggermacros.h"
#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>
#include <commdbconnpref.h>

// Timeout timer
const TInt KTimeoutTime = 15; // Seconds, will convert to TTimeIntervalSeconds.

CSocketWriterBase::CSocketWriterBase(const TLbsHostSettingsId& aSlpId,
									 TInt aCallbackIndex,
	   	  							 MSuplSocketObserver& aSocketObserver,
	   	  							 RSocketServ& aSocketServer,
	   	  							 const TLbsHostSettingsSupl& aHostSettings) :
    CActive(EPriorityStandard), iState(ENone), iSocketServ(aSocketServer), iObserver(aSocketObserver), 
    iCallbackId(aCallbackIndex), iSessionCount(0), iHostSettings(aHostSettings), iSlpSettings(aSlpId), iQueue(5)  
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::CSocketWriterBase() Begin\n");
	CActiveScheduler::Add(this);
	SUPLLOG(ELogP1, "CSocketWriterBase::CSocketWriterBase() End\n");
	}

CSocketWriterBase::~CSocketWriterBase()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::~CSocketWriterBase() Begin\n");
	Cancel();
	iResolver.Close();
	iSocket.Close();
	iConnection.Close();
	
	delete iTimeout;
	SUPLLOG(ELogP1, "CSocketWriterBase::~CSocketWriterBase() End\n");
	}

void CSocketWriterBase::BaseConstructL()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::BaseConstructL() Begin\n");
	User::LeaveIfError(iConnection.Open(iSocketServ));
	
	iTimeout = CLbsCallbackTimer::NewL(*this);
	SUPLLOG(ELogP1, "CSocketWriterBase::BaseConstructL() End\n");
	}

void CSocketWriterBase::Connect(const TSuplLocalSessionId aSessionId)
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::Connect() Begin\n");
	if (iSessionCount == 0)
		{
		++iSessionCount;
		iLastSessionId = aSessionId;
		
		if (iState == ENone)
			{
			ConnectConnection();
			}
		else if(iState != EConnectingConnection)
			{
			DoConnectSocket();
			}
		}
	else
		{
		++iSessionCount;

		TInetAddr addr;
		iSocket.LocalName(addr);
		iObserver.Connected(aSessionId, addr);
		}

	SUPLLOG(ELogP1, "CSocketWriterBase::Connect() End\n");
	}

void CSocketWriterBase::Disconnect()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::Disconnect() Begin\n");
	if (--iSessionCount == 0)
		{
		// Stop the timer incase it is active
		StopTimer();

		// Call the derived class disconnect 
		DoDisconnect();
		
		// Close the socket
		iSocket.Close();
		
		// Inform connection manager of shutdown to socket writer
		TBool cleanup = EFalse;
		iObserver.ConnectionError(MSuplSocketObserver::EDisconnected, iCallbackId, iLastSessionId, cleanup);
		if (cleanup)
			{
			delete this;
			}
		}
	SUPLLOG(ELogP1, "CSocketWriterBase::Disconnect() End\n");
	}

void CSocketWriterBase::SendMessageL(CSuplMessageBase* aMessage, 
									 const TSuplLocalSessionId& aSessionId)

	{
	SUPLLOG(ELogP1, "CSocketWriterBase::SendMessageL() Begin\n");
	iLastSessionId = aSessionId;
	CleanupStack::PushL(aMessage); // As we clean it up
	
	if ((iState != EIdleConnected) && (iState != ESendingMessage))
		{
		SUPLLOG(ELogP1, "CSocketWriterBase::SendMessageL() Leave: KErrNotReady\n");
		User::Leave(KErrNotReady);
		}

	// Encode the message
	HBufC8* buffer = HBufC8::NewLC(KSuplMaxMessageLength);
	TPtr8 ptr(buffer->Des());
	TInt err = aMessage->EncodeToL(ptr);
	if (err != KErrNone)
		{
		ASSERT(EFalse);
		SUPLLOG(ELogP1, "CSocketWriterBase::SendMessageL() Leave: KErrGeneral\n");
		User::Leave(KErrGeneral);
		}
	
	// Add to queue
	iQueue.AppendL(buffer);
	
	CheckQueue();
	
	CleanupStack::Pop(buffer);
	CleanupStack::PopAndDestroy(aMessage);
	SUPLLOG(ELogP1, "CSocketWriterBase::SendMessageL() End\n");
	}

TLbsHostSettingsId CSocketWriterBase::HostSettingsId()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::HostSettingsId() Begin\n");
	SUPLLOG(ELogP1, "CSocketWriterBase::HostSettingsId() End\n");
	return iSlpSettings;
	}

void CSocketWriterBase::ResolveHost()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::ResolveHost() Begin\n");
	iResolver.Open(iSocketServ, KAfInet, KProtocolInetTcp, iConnection);
	
	TBuf8<256> hostname8;
	iHostSettings.GetHostNameAddress(hostname8);
	TBuf16<256> hostname16;
	hostname16.Copy(hostname8);
	iResolver.GetByName(hostname16, iNameEntry, iStatus);
	
	iState = EResolvingSlpAddress;
	StartTimer();
	SetActive();
	SUPLLOG(ELogP1, "CSocketWriterBase::ResolveHost() End\n");
	}

void CSocketWriterBase::ConnectConnection()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::ConnectConnection() Begin\n");
	// Override defaults
	TInt ap;
	TLbsHostSettingsSupl::TLbsConnectionType connectionType;
	iHostSettings.GetConnectionPoint(ap, connectionType);
	
	if (connectionType == TLbsHostSettingsSupl::ELbsConnectionTypeInvalid)
		{
	    SUPLLOG(ELogP1, "connectionType is TLbsHostSettingsSupl::ELbsConnectionTypeInvalid\n");

		iConnection.Start(iStatus);
		}
	else
		{
		TCommDbConnPref prefs;
		prefs.SetIapId(ap);
		prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
		SUPLLOG2(ELogP1, "iConnection.Start(ap=%d)\n", ap);
		iConnection.Start(prefs, iStatus);
		}

	iState = EConnectingConnection;
	StartTimer();
	SetActive();
	SUPLLOG(ELogP1, "CSocketWriterBase::ConnectConnection() End\n");
	}

void CSocketWriterBase::RunL()
	{
	SUPLLOG2(ELogP1, "CSocketWriterBase::RunL() Begin, iState: %d\n", iState );
	StopTimer();
	
	TBool cleanup = EFalse;
	
	switch (iState)
		{
	case EConnectingConnection:
		{
		if (iStatus == KErrNone)
			{
			ResolveHost();
			}
		else
			{
			SUPLLOG2(ELogP1, "CSocketWriterBase::RunL() Connection Error(KErrCouldNotConnect) iStatus = %d\n", iStatus);
			iObserver.ConnectionError(KErrCouldNotConnect, iCallbackId, iLastSessionId, cleanup);
			}
		break;
		} // case

		case EResolvingSlpAddress:
			{
			if (iStatus == KErrNone)
				{
				// Fill in the actual iAddr
				iAddr = TInetAddr::Cast(iNameEntry().iAddr);
				TInt port = 0;
				iHostSettings.GetPortId(port);
				iAddr.SetPort(port);
				
				DoConnectSocket();
				} // if
			else
				{
				SUPLLOG(ELogP1, "CSocketWriterBase::RunL() Connection Error(KErrNotFound)\n");
				iObserver.ConnectionError(KErrNotFound, iCallbackId, iLastSessionId, cleanup);
				}
			break;
			} // case
		case EConnectingSocket:
			{
			if (iStatus == KErrNone)
				{
				// Start the reader
				TRAPD(err, DoStartReaderL());
				if (err == KErrNone)
					{
					iState = EIdleConnected;
					
					TInetAddr addr;
					iSocket.LocalName(addr);
					iObserver.Connected(iLastSessionId, addr);
					
					SelfComplete();
					}
				else
					{
					SUPLLOG(ELogP1, "CSocketWriterBase::RunL() Connection Error(KErrCouldNotConnect,1)\n");
					iObserver.ConnectionError(KErrCouldNotConnect, iCallbackId, iLastSessionId, cleanup);
					}
				}
			else
				{
				SUPLLOG(ELogP1, "CSocketWriterBase::RunL() Connection Error(KErrCouldNotConnect,2)\n");
				iObserver.ConnectionError(KErrCouldNotConnect, iCallbackId, iLastSessionId, cleanup);
				}
			break;
			}
		case ESendingMessage:
			{
			// Remove the message that was sent
			HBufC8* buf = iQueue.At(0); 
			delete buf;
			iQueue.Delete(0);
			
			if (iStatus == KErrNone)
				{
				iState = EIdleConnected;
				SelfComplete();
				} // if
			else
				{
				SUPLLOG(ELogP1, "CSocketWriterBase::RunL() Connection Error(EFailedToSend)\n");
				iObserver.ConnectionError(MSuplSocketObserver::EFailedToSend, iCallbackId, iLastSessionId, cleanup);
				} // else
			break;
			}
		case EIdleConnected:
			{
			CheckQueue();
			
			break;
			}
		default:
			{
			// Nothing to do
			break;
			}
		} // switch
	
	if (cleanup)
		{
		delete this;
		}
	SUPLLOG(ELogP1, "CSocketWriterBase::RunL() End\n");
	}

void CSocketWriterBase::DoCancel()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::DoCancel() Begin\n");
	switch(iState)
		{
		case EResolvingSlpAddress:
			iResolver.Cancel();
			break;
		case EConnectingConnection:
			{
			//Unfortunatelly there is no currently a cancel method for the RConnection::Start.
			//This is a bug in the OS (see CR0028 for details). 
			//As a workaround we use the following code.
			iConnection.Close();
			TInt err = iConnection.Open(iSocketServ);
			__ASSERT_DEBUG(KErrNone==err, User::Invariant());
			}
			break;
		case EConnectingSocket:
			iSocket.CancelConnect();
			break;
		case ESendingMessage:
			iSocket.CancelWrite();
			break;
		default:
			//Intentionally left blank
			break;
		}
	SUPLLOG(ELogP1, "CSocketWriterBase::DoCancel() End\n");
	}

TInt CSocketWriterBase::RunError(TInt aError)
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::RunError() Begin\n");
	__ASSERT_DEBUG(EFalse, User::Invariant());
	
	SUPLLOG(ELogP1, "CSocketWriterBase::RunError() End\n");
	return aError;
	}

void CSocketWriterBase::CheckQueue()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::CheckQueue() Begin\n");
	if (!IsActive() && (iQueue.Count() > 0))
		{
		DoSendMessage(*(iQueue.At(0)));
		}
	SUPLLOG(ELogP1, "CSocketWriterBase::CheckQueue() End\n");
	}

void CSocketWriterBase::DoConnect()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::DoConnect() Begin\n");
	SUPLLOG(ELogP1, "CSocketWriterBase::DoConnect() End\n");
	}

void CSocketWriterBase::SelfComplete()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::SelfComplete() Begin\n");
	TRequestStatus* temp = &iStatus;
	User::RequestComplete(temp, KErrNone);
	
	StartTimer();
	SetActive();
	SUPLLOG(ELogP1, "CSocketWriterBase::SelfComplete() End\n");
	}

void CSocketWriterBase::StartTimer()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::StartTimer() Begin\n");
	TTimeIntervalSeconds timeout = KTimeoutTime; 
	iTimeout->EventAfter(timeout, 1);
	SUPLLOG(ELogP1, "CSocketWriterBase::StartTimer() End\n");
	}

void CSocketWriterBase::StopTimer()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::StopTimer() Begin\n");
	iTimeout->Cancel();
	SUPLLOG(ELogP1, "CSocketWriterBase::StopTimer() End\n");
	}

void CSocketWriterBase::OnTimerEventL(TInt 
#ifdef _DEBUG									  
									 aTimerId
#endif
									  )
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::OnTimerEventL() Begin\n");
	__ASSERT_DEBUG(aTimerId == 1, User::Invariant());
	
	// Server has not responded. The error will result in the connection being closed
	TBool cleanup = EFalse;
	iObserver.ConnectionError(MSuplSocketObserver::ETimeOut, iCallbackId, iLastSessionId, cleanup);
	if (cleanup)
		{
		delete this;
		}
	SUPLLOG(ELogP1, "CSocketWriterBase::OnTimerEventL() End\n");
	}

TInt CSocketWriterBase::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::OnTimerEventL() Begin\n");
	__ASSERT_DEBUG(EFalse, User::Invariant());
	
	SUPLLOG(ELogP1, "CSocketWriterBase::OnTimerEventL() End\n");
	return aError;
	}

TInt CSocketWriterBase::CallbackId()
	{
	SUPLLOG(ELogP1, "CSocketWriterBase::CallbackId() Begin\n");
	SUPLLOG(ELogP1, "CSocketWriterBase::CallbackId() End\n");
	return iCallbackId;
	}



/**
 * CSocketWriter
 */
CSocketWriter* CSocketWriter::NewL(const TLbsHostSettingsId& aSlpId, 
								   TInt aCallbackIndex,
								   MSuplSocketObserver& aSocketObserver,
								   RSocketServ& aSocketServer,
								   TLbsHostSettingsSupl& aHostSettings)
	{
	SUPLLOG(ELogP1, "CSocketWriter::NewL() Begin\n");
	CSocketWriter* self = new(ELeave) CSocketWriter(aSlpId, aCallbackIndex, aSocketObserver, aSocketServer, aHostSettings);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	
	SUPLLOG(ELogP1, "CSocketWriter::NewL() End\n");
	return self;
	}

CSocketWriter::~CSocketWriter()
	{
	SUPLLOG(ELogP1, "CSocketWriter::~CSocketWriter() Begin\n");
	delete iSocketReader;
	SUPLLOG(ELogP1, "CSocketWriter::~CSocketWriter() End\n");
	}

CSocketWriter::CSocketWriter(const TLbsHostSettingsId& aSlpId,
							 TInt aCallbackIndex,
							 MSuplSocketObserver& aSocketObserver,
			   	  			 RSocketServ& aSocketServer,
			   	  			 TLbsHostSettingsSupl& aHostSettings) :
    CSocketWriterBase(aSlpId, aCallbackIndex, aSocketObserver, aSocketServer, aHostSettings)
	{
	SUPLLOG(ELogP1, "CSocketWriter::CSocketWriter() Begin\n");
	SUPLLOG(ELogP1, "CSocketWriter::CSocketWriter() End\n");
	}

void CSocketWriter::DoConnectSocket()
	{
	SUPLLOG(ELogP1, "CSocketWriter::DoConnectSocket() Begin\n");
	TInt err = iSocket.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp, iConnection); 
	if (err == KErrNone)
		{
		iSocket.Connect(iAddr, iStatus);
		
		iState = EConnectingSocket;
		StartTimer();
		SetActive();
		}
	else
		{
		SUPLLOG(ELogP1, "CSocketWriter::DoConnectSocket() Error: ESocketError\n");
		TBool cleanup = EFalse;
		iObserver.ConnectionError(MSuplSocketObserver::ESocketError, iCallbackId, iLastSessionId, cleanup);
		if (cleanup)
			{
			delete this;
			}
		}
	SUPLLOG(ELogP1, "CSocketWriter::DoConnectSocket() End\n");
	}

void CSocketWriter::DoDisconnect()
	{
	SUPLLOG(ELogP1, "CSocketWriter::DoDisconnect() Begin\n");
	if(IsActive())
		{
		Cancel();
		}
	delete iSocketReader;
	iSocketReader = NULL;
	if((iState == EResolvingSlpAddress) || (iState ==EConnectingConnection))
		{
		iState = ENone;
		}

	SUPLLOG(ELogP1, "CSocketWriter::DoDisconnect() End\n");
	}
	
void CSocketWriter::DoSendMessage(TDesC8& aMessage)
	{
	SUPLLOG(ELogP1, "CSocketWriter::DoSendMessage() Begin\n");
	iState = ESendingMessage;
	
	iSocket.Write(aMessage, iStatus);
	StartTimer();
	SetActive();
	SUPLLOG(ELogP1, "CSocketWriter::DoSendMessage() End\n");
	}

void CSocketWriter::DoStartReaderL()
	{
	SUPLLOG(ELogP1, "CSocketWriter::DoStartReaderL() Begin\n");
	iSocketReader = CSocketReader::NewL(iSocket, iObserver, iCallbackId);
	SUPLLOG(ELogP1, "CSocketWriter::DoStartReaderL() End\n");
	}

/**
 * CSecureSocketWriter
 */
CSecureSocketWriter* CSecureSocketWriter::NewL(const TLbsHostSettingsId& aSlpId, 
								   		 	   TInt aCallbackIndex,
								   		 	   MSuplSocketObserver& aSocketObserver,
								   		 	   RSocketServ& aSocketServer,
								   		 	   TLbsHostSettingsSupl& aHostSettings,
								   	  		   const CSuplSessionRecord::TServiceType& aServiceType)
	{
	SUPLLOG(ELogP1, "CSecureSocketWriter::NewL() Begin\n");
	CSecureSocketWriter* self = new(ELeave) CSecureSocketWriter(aSlpId, aCallbackIndex, aSocketObserver, aSocketServer, aHostSettings, aServiceType);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	
	SUPLLOG(ELogP1, "CSecureSocketWriter::NewL() End\n");
	return self;
	}

CSecureSocketWriter::~CSecureSocketWriter()
	{
	SUPLLOG(ELogP1, "CSecureSocketWriter::~CSecureSocketWriter() Begin\n");
	delete iSocketReader;
	delete iSecureSocket;
	SUPLLOG(ELogP1, "CSecureSocketWriter::~CSecureSocketWriter() End\n");
	}

CSecureSocketWriter::CSecureSocketWriter(const TLbsHostSettingsId& aSlpId,
							 			 TInt aCallbackIndex,
							 			 MSuplSocketObserver& aSocketObserver,
							 			 RSocketServ& aSocketServer,
							 			 TLbsHostSettingsSupl& aHostSettings,
							   	  		 const CSuplSessionRecord::TServiceType& aServiceType) :
    CSocketWriterBase(aSlpId, aCallbackIndex, aSocketObserver, aSocketServer, aHostSettings),
    iSecureSocket(NULL), iServiceType(aServiceType)
	{
	SUPLLOG(ELogP1, "CSecureSocketWriter::CSecureSocketWriter() Begin\n");
	SUPLLOG(ELogP1, "CSecureSocketWriter::CSecureSocketWriter() End\n");
	}

void CSecureSocketWriter::RunL()
	{
	TBool cleanup = EFalse;
	
	SUPLLOG2(ELogP1, "CSecureSocketWriter::RunL() Begin, iState: %d\n", iState);
	switch (iState)
		{
		case EConnectingSocket:
			{
			StopTimer();
			if (iStatus == KErrNone)
				{
				StartSecureSocketL();
				}
			else
				{
				SUPLLOG(ELogP1, "CSecureSocketWriter::RunL() Error(KErrCoundNotConnect)\n");
				iObserver.ConnectionError(KErrCouldNotConnect, iCallbackId, iLastSessionId, cleanup);
				}		
			break;
			}
		case ETlsHandshaking:
			{
			StopTimer();
			if (iStatus == KErrNone)
				{
				// Check the certificate if we are running in TLS_ACA mode
				TBool isSecure = EFalse;
				TRAPD(err, isSecure = CheckTlsSecureL());
				if ((err == KErrNone) && isSecure)
					{
					TRAP(err, DoStartReaderL());
					if (err == KErrNone)
						{
						iState = EIdleConnected;

						TInetAddr addr;
						iSocket.LocalName(addr);
						iObserver.Connected(iLastSessionId, addr);

						SelfComplete();
						}
					else
						{
						SUPLLOG(ELogP1, "CSecureSocketWriter::RunL() Error(KErrCoundNotConnect)\n");
						iObserver.ConnectionError(KErrCouldNotConnect, iCallbackId, iLastSessionId, cleanup);
						}
					}
				else
					{
					SUPLLOG(ELogP1, "CSecureSocketWriter::RunL() Error(EFailedCertCheck)\n");
					iObserver.ConnectionError(MSuplSocketObserver::EFailedCertCheck, iCallbackId, iLastSessionId, cleanup);
					}
				}
			else
				{
				SUPLLOG(ELogP1, "CSecureSocketWriter::RunL() Error(EFailedToHandshake)\n");
				iObserver.ConnectionError(MSuplSocketObserver::EFailedToHandshake, iCallbackId, iLastSessionId, cleanup);
				}
			break;
			}
		default:
			{
			CSocketWriterBase::RunL();
			break;
			}
		}
	
	if (cleanup)
		{
		delete this;
		}
	SUPLLOG(ELogP1, "CSecureSocketWriter::RunL() End\n");
	}


void CSecureSocketWriter::DoCancel()
	{
	switch(iState)
		{
		case ETlsHandshaking:
			__ASSERT_DEBUG(iSecureSocket!=NULL, User::Invariant());
			if(iSecureSocket)
				{
				iSecureSocket->CancelHandshake();
				}
			break;
		default:
			CSocketWriterBase::DoCancel();
			break;
		}
	}

void CSecureSocketWriter::DoConnectSocket()
	{
	SUPLLOG(ELogP1, "CSecureSocketWriter::DoConnectSocket() Begin\n");
	TInt err = iSocket.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp, iConnection); 
	if (err == KErrNone)
		{
		iSocket.Connect(iAddr, iStatus);
		
		iState = EConnectingSocket;
		StartTimer();
		SetActive();
		}
	else
		{
		SUPLLOG(ELogP1, "CSecureSocketWriter::DoConnectSocket() Error(ESocketError)\n");
		TBool cleanup = EFalse;
		iObserver.ConnectionError(MSuplSocketObserver::ESocketError, iCallbackId, iLastSessionId, cleanup);
		if (cleanup)
			{
			delete this;
			}
		}
	SUPLLOG(ELogP1, "CSecureSocketWriter::DoConnectSocket() End\n");
	}

void CSecureSocketWriter::DoDisconnect()
	{
	SUPLLOG(ELogP1, "CSecureSocketWriter::DoDisconnect() Begin\n");
	if(IsActive())
		{
		Cancel();
		}

	delete iSocketReader;
	iSocketReader = NULL;
	
	// If we cant resovle the host we wont have a secure connection yet
	if (iSecureSocket != NULL)
		{
		iSecureSocket->Close();
		}
	if((iState == EResolvingSlpAddress) || (iState ==EConnectingConnection))
		{
		iState = ENone;
		}
	SUPLLOG(ELogP1, "CSecureSocketWriter::DoDisconnect() End\n");
	}
	
void CSecureSocketWriter::DoSendMessage(TDesC8& aMessage)
	{
	SUPLLOG(ELogP1, "CSecureSocketWriter::DoSendMessage() Begin\n");
	iState = ESendingMessage;
	
	iSecureSocket->Send(aMessage, iStatus);
	StartTimer();
	SetActive();
	SUPLLOG(ELogP1, "CSecureSocketWriter::DoSendMessage() End\n");
	}

void CSecureSocketWriter::DoStartReaderL()
	{
	SUPLLOG(ELogP1, "CSecureSocketWriter::DoStartReaderL() Begin\n");
	iSocketReader = CSecureSocketReader::NewL(*iSecureSocket, iObserver, iCallbackId);
	SUPLLOG(ELogP1, "CSecureSocketWriter::DoStartReaderL() End\n");
	}

void CSecureSocketWriter::StartSecureSocketL()
	{
	SUPLLOG(ELogP1, "CSecureSocketWriter::StartSecureSocketL() Begin\n");
	_LIT(KSSLProtocol,"TLS1.0");
	
 	iState = ETlsHandshaking;
	
	// Create the secure socket
	iSecureSocket = CSecureSocket::NewL(iSocket, KSSLProtocol());
	iSecureSocket->FlushSessionCache();

	// Do the handshake
	iSecureSocket->StartClientHandshake(iStatus);
	StartTimer();
	SetActive();
	SUPLLOG(ELogP1, "CSecureSocketWriter::StartSecureSocketL() End\n");
	}

TBool CSecureSocketWriter::CheckTlsSecureL()
	{
	SUPLLOG(ELogP1, "CSecureSocketWriter::CheckTlsSecureL() Begin\n");
	TBool ret = ETrue;
	
	// Get the auth mode to check
	TLbsHostSettingsSupl::TAuthModes authMode;
	if (iServiceType == CSuplSessionRecord::EServiceMtlr)
		{
		iHostSettings.GetAuthModesMTLR(authMode);
		}
	else
		{
		iHostSettings.GetAuthModesMOLR(authMode);
		}
	
	// If we are using AuthAcaTls then we need to check that the certificate is from the
	// provisioned host. We dont bother with the check if we will accept AuthTLs as we simply
	// fall back to that if we fail anyway.
	if (((authMode & TLbsHostSettingsSupl::EAuthAcaTls) == TLbsHostSettingsSupl::EAuthAcaTls) && 
		((authMode & TLbsHostSettingsSupl::EAuthTls)  != TLbsHostSettingsSupl::EAuthTls))
		{
		ret = EFalse;

		// Get server name from certificate
		const CX509Certificate* cert = iSecureSocket->ServerCert();
		if (cert != NULL)
			{
			HBufC* commonName = cert->SubjectName().DisplayNameL();
			
			// Host provisioned
			TBuf8<256> hostname8;
			iHostSettings.GetHostNameAddress(hostname8);
			TBuf16<256> hostname16;
			hostname16.Copy(hostname8);

			if (commonName->CompareF(hostname16) == 0)
				{
				// Certificate is from provisioned host
				ret = ETrue;
				}
			else
				{
				SUPLLOG(ELogP1, "CSecureSocketWriter::CheckTlsSecureL() Error(FailedACATest)\n");
				}
			}
		}
	
	SUPLLOG(ELogP1, "CSecureSocketWriter::CheckTlsSecureL() End\n");

	return ret;
	}
