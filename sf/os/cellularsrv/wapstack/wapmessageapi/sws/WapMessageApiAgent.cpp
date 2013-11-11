// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "WapMessageApiAgentTraces.h"
#endif

#include "WapMessageApiAgent.h"
#include "WapMsgUtils.h"
#include "CLWSPPduHandler.h"
#include <es_wsms.h>
#include <wapmsgerr.h>

CWapAsyncCallBack::CWapAsyncCallBack( const TCallBack& aCallBack, TInt aPriority )
:   CActive( aPriority ), iCallBack( aCallBack )
	{
	CActiveScheduler::Add( this );
	}

CWapAsyncCallBack::~CWapAsyncCallBack()
	{
	Cancel();
	}

void CWapAsyncCallBack::CallBack()
	{
	if  ( !IsActive() )
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete( status, KErrNone );
		SetActive();
		}
	}

void CWapAsyncCallBack::RunL()
	{
	iCallBack.CallBack();
	}

void CWapAsyncCallBack::DoCancel()
	{
	// Empty - request already completed in CallBack()
	}

//
//CWapMessageApiAgent class Method
//
CWapMessageApiAgent::CWapMessageApiAgent(TWapMessageType aMessageType):iMessageType(aMessageType)
/**
Constuctor.
@internalComponent
@released since v8.0
@param aMessageType the message type that handled by this agent
*/
	{
	}

CWapMessageApiAgent::~CWapMessageApiAgent()
/**
Destrutor
@internalComponent.
@released since v8.0
*/
	{
	TInt count=iBearers.Count();
	for (TInt i=0; i<count; i++)
		{
		iBearers[i]->Cancel();
		iBearers[i]->CleanUpData();
		}
	iBearers.ResetAndDestroy();
	iSocketServ.Close();
	if (iTimeoutTimer)
		{
		iTimeoutTimer->Cancel();		
		delete iTimeoutTimer;
		}
	if (iAsyncReadCompletion)
		{
		delete iAsyncReadCompletion;
		}
	}

TInt CWapMessageApiAgent::CompleteReading(TAny* aAgent)
	{
	CWapMessageApiAgent* agent = reinterpret_cast<CWapMessageApiAgent*>(aAgent);
	return agent->DoCompleteReading();
	}

TInt CWapMessageApiAgent::DoCompleteReading()
	{
	iRequestActive=EFalse;
	User::RequestComplete(iRequestStatus, iLastReadingError);
	return KErrNone;
	}
void CWapMessageApiAgent::ConstructL()
/**
Second Phase Constructor
@internalComponent.
@released since v8.0
*/
	{
	iTimeoutTimer = CTimeOutTimer::NewL(*this);
	TCallBack callback = TCallBack(CompleteReading, this);
	iAsyncReadCompletion = new (ELeave) CWapAsyncCallBack(callback, CActive::EPriorityHigh);
	User::LeaveIfError(iSocketServ.Connect());
 	}

CActiveSocket* CWapMessageApiAgent::GetActiveSocketByBearer(Wap::TBearer aBearer)
/**
To Get the CActiveSocket from the Array by Bearer type. 
@internalComponent.
@released since v8.0
@param aBearer (in)the bearer type of the CActiveSocket
@returns the pointer of CAtiveSocket
*/
	{
	TInt count=iBearers.Count();
	for (TInt i=0; i<count; i++)
		{
		Wap::TBearer bearer=iBearers[i]->GetBearerType();
		if (bearer==aBearer)
			{
			return iBearers[i];
			}
		else
			{
			if ((aBearer>=Wap::ESMS7 && aBearer<=Wap::EWAPSMS) && (bearer>=Wap::ESMS7 && bearer<=Wap::EWAPSMS))
				{
				return iBearers[i];
				}
			}
		}
	return NULL;
	}

CActiveSocket* CWapMessageApiAgent::GetActiveSocketByStatus(TWapMessageState aState)
/**
To Get the CActiveSocket from the Array by Data State. 
@internalComponent.
@released since v8.0
@param aState (in)the Data state of the CActiveSocket
@returns the pointer of CAtiveSocket
*/
	{
	TInt count=iBearers.Count();
	for (TInt i=0; i<count; i++)
		{
		if (iBearers[i]->GetDataState()==aState)
			{
			return iBearers[i];
			}
		}
	return NULL;
	}

void CWapMessageApiAgent::TimerExpired()
/**
To handle the receive time out.
@internalComponent.
@released since v8.0
*/
	{
	TInt count=iBearers.Count();
	for (TInt i=0; i<count; i++)
		{
		iBearers[i]->Cancel();
		iBearers[i]->CleanUpData();
		}
	iRequestActive=EFalse;
	User::RequestComplete(iRequestStatus, KErrTimedOut);
	}

TInt CWapMessageApiAgent::GetLocalPort(Wap::TPort& aLocalPort)
/**
To Get local port of the last received packet
@internalComponent.
@released since v8.0
@param aLocalPort (out)the reference of the localPort.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (!iIsOpen)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_GETLOCALPORT_1, "CWapMessageApiAgent::GetLocalPort: Trying to GetLocalPort on unconnected API.");
        return KErrNotReady;
		}
	CActiveSocket* sock=GetActiveSocketByBearer(iLastPduBearer);
	if (!sock)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_GETLOCALPORT_2, "CWapMessageApiAgent::GetLocalPort: Can not find Last Bearer.");
        return Wap::EBearerError;
		}
	sock->GetLocalPort(aLocalPort);
	return KErrNone;
	}

TInt CWapMessageApiAgent::GetLocalAddress(HBufC8*& aLocalHost)
/**
To Get local Address of the last received Packet
@internalComponent.
@released since v8.0
@param aLocalHost (out)the reference of the localHost.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (!iIsOpen)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_GETLOCALADDRESS_1, "CWapMessageApiAgent::GetLocalAddress: Trying to GetLocalAddress on unconnected API.");
		return KErrNotReady;
		}
	CActiveSocket* sock=GetActiveSocketByBearer(iLastPduBearer);
	if (!sock)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_GETLOCALADDRESS_2, "CWapMessageApiAgent::GetLocalAddress: Can not find Last Bearer");
		return Wap::EBearerError;
		}
	TSockAddr& localHost=sock->GetLocalAddress();
	TInt err=KErrNone;
	TRAP(err, aLocalHost=localHost.AllocL())
	if (err)
		{
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_GETLOCALADDRESS_3, "CWapMessageApiAgent::GetLocalAddress: Alloc Memory Err=%d", err);
		}
	return err;
	}

TInt CWapMessageApiAgent::GetBearer(Wap::TBearer& aBearer)
/**
To Get Bearer of the last received packet
@internalComponent.
@released since v8.0
@param aBearer (out)the reference of the Bearer.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (!iIsOpen)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_GETBEARER_1, "CWapMessageApiAgent::GetBearer: Trying to GetBearer on unconnected API.");
		return KErrNotReady;
		}
	aBearer=iLastPduBearer;
	return KErrNone;
	}

TInt CWapMessageApiAgent::GetServerAddress(HBufC8*& aServerHost)
/**
To Get Server Host of the last received packet
@internalComponent.
@released since v8.0
@param aServerHost (out)the reference of the Server Host.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (!iIsOpen)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_GETSERVERADDRESS_1, "CWapMessageApiAgent::GetServerAddress: Trying to GetServerAddress on unconnected API.");
		return KErrNotReady;
		}
	CActiveSocket* sock=GetActiveSocketByBearer(iLastPduBearer);
	if (!sock)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_GETSERVERADDRESS_2, "CWapMessageApiAgent::GetServerAddress: Can not find last error.");
		return Wap::EBearerError;
		}
	return sock->GetRemoteAddress(aServerHost);
	}

void CWapMessageApiAgent::CancelRequest()
/**
To Cancel the client's request
@internalComponent.
@released since v8.0
*/
	{
	if (iIsOpen)
		{
		TInt count=iBearers.Count();
		for (TInt i=0; i<count; i++)
			{
			iBearers[i]->Cancel();
			iBearers[i]->CleanUpData();
			}
		iTimeoutTimer->Cancel();
		if (iRequestActive)
			{
			iRequestActive=EFalse;
			iAsyncReadCompletion->Cancel();
			User::RequestComplete(iRequestStatus, KErrCancel);
			}
		}
	}

TInt CWapMessageApiAgent::Connect(Wap::TBearer aBearer, Wap::TPort aPort, TBool aSecure)
/**
Opening an endpoint that can be used to listen for subsequent incoming datagrams.
@internalComponent.
@released since v8.0
@param aBearer (In) the bearer to listen on
@param aPort (In) the port to listen on.
@param aSecure (In) security flag indicates whether WTLS will be used or not (Not Supported)
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TInetAddr inetAddr( KInetAddrAny, 0);
	return (Connect(aBearer, aPort, aSecure, inetAddr));
	}

TInt CWapMessageApiAgent::Connect(Wap::TBearer aBearer, Wap::TPort aPort, TBool /*aSecure*/, TInetAddr /*aInetAddr*/)
/**
Opening an endpoint that can be used to listen for subsequent incoming datagrams.
@internalComponent.
@released since v8.0
@param aBearer (In) the bearer to listen on
@param aPort (In) the port to listen on.
@param aSecure (In) security flag indicates whether WTLS will be used or not (Not Supported)
@param aInetAddr (In) Not In Use.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TInt ret=KErrNone;
	if (!iIsOpen)
		{
		TRAP(ret, CActiveSocket::NewL(iSocketServ, iBearers, aBearer, iMessageType, this, aPort))
		if (ret!=KErrNone)
			{
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_1, "CWapMessageApiAgent::Connect: CActiveSocket Instantiate err=%d.",ret);
			return ret;
			}
		iIsOpen=ETrue;
		}
	else
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_2, "CWapMessageApiAgent::Connect:Trying Connect twice to the API.");
		ret=KErrInUse;
		}
	return ret;
	}

TInt CWapMessageApiAgent::Connect(Wap::TBearer aBearer, Wap::TPort aPort, TBool /*aSecure*/, TInt aSocketServHandle, RConnection* aConnection)
/**
Opening an endpoint that can be used to listen for subsequent incoming datagrams.
@internalComponent.
@released since v8.0
@param aBearer (In) the bearer to listen on
@param aPort (In) the port to listen on.
@param aSecure (In) security flag indicates whether WTLS will be used or not (Not Supported)
@param aSocketServHandle (In) The Socket Server ID.
@param aConnection (In) The RConnection that shared with the client.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (aSocketServHandle==0 || !aConnection)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_1_1, "CWapMessageApiAgent::Connect:Parameter Error");
		return KErrArgument;
		}
	TInt ret=KErrNone;
	if (!iIsOpen)
		{
		iSocketServ.SetHandle(aSocketServHandle);
		TRAP(ret, CActiveSocket::NewL(iSocketServ, iBearers, aBearer, iMessageType, this, aPort, aConnection))
		if (ret!=KErrNone)
			{
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_1_2, "CWapMessageApiAgent::Connect: CActiveSocket Instantiate err=%d.", ret);
			return ret;
			}
		iIsOpen=ETrue;
		}
	else
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_1_3, "CWapMessageApiAgent::Connect:Trying to Connect twice to the API.");
		ret=KErrInUse;
		}
	return ret;
	}

TInt CWapMessageApiAgent::Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TBool /*aSecure*/, TInetAddr /*aInetAddr*/)
/**
Opens a socket which is to be used only with a single, named remote host.
@internalComponent.
@released since v8.0
@param aRemoteHost (In) the name of the remote host to connect.
@param aRemotePort (In) the port of the remote host to connect.
@param aBearer (In) the bearer to be used
@param aSecure (In) security flag indicates whether WTLS will be used or not (Not Supported)
@param aInetAddr (In) Not In Use
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (aBearer==Wap::EAll)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_2_1, "CWapMessageApiAgent::SendWspMessage:Connect to EAll Error");
		return Wap::EBearerError;
		}
	TInt ret=KErrNone;
	if (!iIsOpen)
		{
		TSockAddr remoteAddr;
		TRAP(ret, CSWSWapMsgUtils::BuildAddrL(remoteAddr, aBearer, aRemoteHost, aRemotePort))
		if (ret!=KErrNone)
			{
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_2_2, "CWapMessageApiAgent::Connect: BuildAddress err=%d.", ret);
			return ret;
			}
		TRAP(ret, CActiveSocket::NewL(iSocketServ, iBearers, aBearer, iMessageType, this, remoteAddr))
		if (ret!=KErrNone)
			{
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_2_3, "CWapMessageApiAgent::Connect: CActiveSocket Instantiate err=%d.", ret);
			return ret;
			}
		iIsOpen=ETrue;
		}
	else
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_2_4, "CWapMessageApiAgent::Connect:Trying to Connect twice to the API.");
		ret=KErrInUse;
		}
	return ret;
	}

TInt CWapMessageApiAgent::Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TBool aSecure)
/**
Opens a socket which is to be used only with a single, named remote host.
@internalComponent.
@released since v8.0
@param aRemoteHost (In) the name of the remote host to connect.
@param aRemotePort (In) the port of the remote host to connect.
@param aBearer (In) the bearer to be used
@param aSecure (In) security flag indicates whether WTLS will be used or not (Not Supported)
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TInetAddr inetAddr( KInetAddrAny, 0);
	return (Connect(aRemoteHost, aRemotePort, aBearer, aSecure, inetAddr));
	}

TInt CWapMessageApiAgent::Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TBool /*aSecure*/, TInt aSocketServHandle, RConnection* aConnection)
/**
Opens a socket which is to be used only with a single, named remote host.
@internalComponent.
@released since v8.0
@param aRemoteHost (In) the name of the remote host to connect.
@param aRemotePort (In) the port of the remote host to connect.
@param aBearer (In) the bearer to be used
@param aSecure (In) security flag indicates whether WTLS will be used or not (Not Supported)
@param aSocketServHandle (In) The Socket Server ID.
@param aConnection (In) The RConnection that shared with the client.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (aBearer==Wap::EAll)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_3_1, "CWapMessageApiAgent::SendWspMessage:Connect to EAll Error");
		return Wap::EBearerError;
		}
	if (aSocketServHandle==0 || !aConnection)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_3_2, "CWapMessageApiAgent::Connect:Parameter Error");		
		return KErrArgument;
		}
	TInt ret=KErrNone;
	if (!iIsOpen)
		{
		iSocketServ.SetHandle(aSocketServHandle);
		TSockAddr remoteAddr;
		TRAP(ret, CSWSWapMsgUtils::BuildAddrL(remoteAddr, aBearer, aRemoteHost, aRemotePort))
		if (ret!=KErrNone)
			{
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_3_3, "CWapMessageApiAgent::Connect: BuildAddress err=%d.",ret);
			return ret;
			}
		TRAP(ret, CActiveSocket::NewL(iSocketServ, iBearers, aBearer, iMessageType, this, remoteAddr, aConnection))
		if (ret!=KErrNone)
			{
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_3_4, "CWapMessageApiAgent::Connect: CActiveSocket Instantiate err=%d.",ret);
			return ret;
			}
		iIsOpen=ETrue;
		}
	else
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPMESSAGEAPIAGENT_CONNECT_3_5, "CWapMessageApiAgent::Connect:Connect to a connected API.");
		ret=KErrInUse;
		}
	return ret;
	}
//
//CWspMessageApiAgent class Methods
//
CWspMessageApiAgent* CWspMessageApiAgent::NewL()
/**
Static NEWL()
@internalComponent
@released since v8.0
@returns CWspMessageApiAgent instance.
*/
	{
	CWspMessageApiAgent* me = new(ELeave) CWspMessageApiAgent();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

CWspMessageApiAgent::CWspMessageApiAgent():CWapMessageApiAgent(EWapWsp)
/**
Constructor
@internalComponent
@released since v8.0
*/
	{
	}

CWspMessageApiAgent::~CWspMessageApiAgent()
/**
Destructor
@internalComponent
@released since v8.0
*/
	{
	}

void CWspMessageApiAgent::ConstructL()
/**
Second Phase Constructor
@internalComponent
@released since v8.0
*/
	{
	CWapMessageApiAgent::ConstructL();
	}

TInt CWspMessageApiAgent::SendWspMessage(Wap::TBearer aBearer, const TDesC8& aRemoteHost, Wap::TPort aRemotePort, TUint aMethod, const TDesC& aURI, const TDesC8& aReqHeaders, const TDesC8& aReqBody, TUint8 aTransactionId)
/**
Send Wsp Message to a remote host
@internalComponent
@released since v8.0
@param aBearer (in) the bearer to be used
@param aRemoteHost (in) The remote host to be sent.
@param aRemotePort (in) The remote port to be sent.
@param aMethod (in) The method to be invoked.
@param aURI (in) The destination URI.
@param aReqHeaders (in) The Wsp header of WSP message.
@param aReqBody (in) The Wsp body of WSP message.
@param aTransactionId (in) The transaction ID of WSP message.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (!iIsOpen)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_SENDWSPMESSAGE_1, "CWapMessageApiAgent::SendWspMessage:Tryig to send to unconnected API");
		return KErrNotReady;
		}
	if (aBearer==Wap::EAll)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_SENDWSPMESSAGE_2, "CWapMessageApiAgent::SendWspMessage:Send to EAll Error");
		return Wap::EBearerError;
		}
	HBufC8* sendBuf=NULL;
	TInt err=KErrNone;
	TSockAddr remoteAddr;
	TRAP(err, CSWSWapMsgUtils::BuildAddrL(remoteAddr, aBearer, aRemoteHost, aRemotePort))
	if (err!=KErrNone)
		{
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_SENDWSPMESSAGE_3, "CWapMessageApiAgent::SendWspMessage: BuildAddress err=%d.", err);
		return err;
		}
	TRAP(err, CCLWSPPduHandler::PackWSPPduL(sendBuf, TWSPPduType(aMethod), aURI, aReqHeaders, aReqBody, aTransactionId))
	if (err!=KErrNone)
		{
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_SENDWSPMESSAGE_4, "CWapMessageApiAgent::SendWspMessage:Pack Wsp Packet Error=%d", err);
		return err;
		}
	CActiveSocket* sendSock=GetActiveSocketByBearer(aBearer);
	if (sendSock)
		{
		if (aBearer==Wap::ESMS||aBearer==Wap::EWAPSMS)
			{
			TWapSmsDataCodingScheme codingScheme = EWapSms8BitDCS;
			sendSock->Socket().SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,codingScheme);
			}
		TRequestStatus status;
		sendSock->Socket().SendTo(*sendBuf, remoteAddr, 0, status);
		User::WaitForRequest(status);
		err=status.Int();
		}
	else
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_SENDWSPMESSAGE_5, "CWapMessageApiAgent::SendWspMessage: Can not find Bearer");
		err=Wap::EBearerError;
		}
	delete sendBuf;
	return err;
	}

TInt CWspMessageApiAgent::SendWspMessage(TUint aMethod, const TDesC& aURI, const TDesC8& aReqHeaders, const TDesC8& aReqBody, TUint8 aTransactionId)
/**
Send Wsp Message to a remote host for fulluSpecified Interface
@internalComponent
@released since v8.0
@param aMethod (in) The method to be invoked.
@param aURI (in) The destination URI.
@param aReqHeaders (in) The Wsp header of WSP message.
@param aReqBody (in) The Wsp body of WSP message.
@param aTransactionId (in) The transaction ID of WSP message.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (!iIsOpen)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_SENDWSPMESSAGE_1_1, "CWapMessageApiAgent::SendWspMessage:Tryig to send to unconnected API");
		return KErrNotReady;
		}
	if (!iBearers[0])
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_SENDWSPMESSAGE_1_2, "CWapMessageApiAgent::SendWspMessage:No Bearer");
		return Wap::EBearerError;
		}
	HBufC8* sendBuf=NULL;
	TInt err=KErrNone;
	TRAP(err, CCLWSPPduHandler::PackWSPPduL(sendBuf, TWSPPduType(aMethod), aURI, aReqHeaders, aReqBody, aTransactionId))
	if (err!=KErrNone)
		{
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_SENDWSPMESSAGE_1_3, "CWapMessageApiAgent::SendWspMessage:Pack Wsp Packet Error=%d",err);
		return err;
		}
	Wap::TBearer bearer=iBearers[0]->GetBearerType();
	if (bearer==Wap::ESMS||bearer==Wap::EWAPSMS)
		{
		TWapSmsDataCodingScheme codingScheme = EWapSms8BitDCS;
		iBearers[0]->Socket().SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,codingScheme);
		}
	TSockAddr& remoteAddr=iBearers[0]->GetRemoteAddress();
	TRequestStatus status;
	iBearers[0]->Socket().SendTo(*sendBuf, remoteAddr, 0, status);
	User::WaitForRequest(status);
	delete sendBuf;
	return status.Int();
	}

TInt CWspMessageApiAgent::ReceiveWspMessage(TDes8& aWspHeaders, TDes8& aWspBody, TPckgBuf<TUint8>& aTransactionIdPckg, TWSPStatus& aWspStatus, TRequestStatus& aReqStatus, TUint32 aTimeout)
/**
Receive the Wsp Message to a remote host for fulluSpecified Interface
@internalComponent
@released since v8.0
@param aWspHeaders (out) The Wsp header to be received
@param aWspBody (out) The Wsp body to be received.
@param aTransactionIdPckg (out) The received transaction ID buffer
@param aWspStatus (out) The Wsp status for Method Result.
@param aReqStatus (out) The request status of the WAP message API
@param aTimeout (in) The timer out value.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TRequestStatus* reqStatus=NULL;
	if (!iIsOpen)
		{
		reqStatus=&aReqStatus;
		User::RequestComplete(reqStatus, KErrNotReady);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_RECEIVEWSPMESSAGE_1, "CWspMessageApiAgent::ReceiveWspMessage: Trying to recevive from unconnected API");
		return KErrNotReady;
		}
	if (iRequestActive)
		{
		reqStatus=&aReqStatus;
		User::RequestComplete(reqStatus, KErrInUse);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_RECEIVEWSPMESSAGE_2, "CWspMessageApiAgent::ReceiveWspMessage:Outstanding Receive exist");
		return KErrInUse;
		}
	//Record the client buffer
	iClientReqHeaders=&aWspHeaders;
	iClientReqBody=&aWspBody;
	iClientTransactionId=(TUint8*)aTransactionIdPckg.Ptr();
	iClientWspStatus=&aWspStatus;
	//if there are data are being reading
	CActiveSocket* readingSock=GetActiveSocketByStatus(EReading);
	if (readingSock)
		{
		iRequestStatus=&aReqStatus;
		*iRequestStatus = KRequestPending;
		iRequestActive=ETrue;
		iLastReadingError=readingSock->GetWspData(*iClientReqHeaders, *iClientReqBody, *iClientTransactionId, *iClientWspStatus);
		if (iLastReadingError==KErrNone)
			{
			iLastPduBearer=readingSock->GetBearerType();
			}
		iAsyncReadCompletion->CallBack();
		return iLastReadingError;
		}
	//if there are pending data.
	CActiveSocket* pendingSock=GetActiveSocketByStatus(EPendingData);
	if (pendingSock)
		{
		iRequestStatus=&aReqStatus;
		*iRequestStatus = KRequestPending;
		iRequestActive=ETrue;
		iLastReadingError=pendingSock->GetWspData(*iClientReqHeaders, *iClientReqBody, *iClientTransactionId, *iClientWspStatus);
		if (iLastReadingError==KErrNone)
			{
			iLastPduBearer=pendingSock->GetBearerType();
			}
		iAsyncReadCompletion->CallBack();
		return iLastReadingError;
		}
	// Issue request in Idle socket
	CActiveSocket* sock=GetActiveSocketByStatus(EIdle);
	if (sock)
		{
		iRequestStatus=&aReqStatus;
		*iRequestStatus = KRequestPending;
		iRequestActive=ETrue;
		for (TInt i=0; i<iBearers.Count(); i++)
			{
			if (iBearers[i]->GetDataState()==EIdle)
				{
				iBearers[i]->AwaitRecvDataSize();
				}
			}
		if (aTimeout>0)
			{
			iTimeoutTimer->After(aTimeout);
			}
		}
	return KErrNone;
	}

void CWspMessageApiAgent::Notification(TInt aMessage, const TDesC8& aData)
/**
Notification of PDU size or PDU data
@internalComponent
@released since v8.0
@param aMessage (in) The Event Type received by the ActiveSocket
@param aData (in)The Event received by the ActiveSocket.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_NOTIFICATION_1, "CWspMessageApiAgent::Notification is called");
	TWapNotificationInfo& info=*(reinterpret_cast<TWapNotificationInfo*>(const_cast<TUint8*>(aData.Ptr())));
	Wap::TBearer bearer=info.iBearer;
	CActiveSocket* currentSocket=GetActiveSocketByBearer(bearer);
	ASSERT(currentSocket!=NULL);
	if (!currentSocket)
		{
		return;
		}
	TInt err=info.iError;		
	switch (TWapNotificationEvent(aMessage))
		{
	case EPduLengthReceived:
			{
			if (err!=KErrNone)
				{
                OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_NOTIFICATION_2, "CWspMessageApiAgent::Notification PDU length err status:%d", err);
				currentSocket->CleanUpData();
				if (iRequestActive)
					{
					iTimeoutTimer->Cancel();
					User::RequestComplete(iRequestStatus, err);
					iRequestActive=EFalse;
					}
				}
			else
				{
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_NOTIFICATION_3, "CWspMessageApiAgent::Notification PDU length is received");
				if ((err=currentSocket->Receive())!=KErrNone)
					{
					currentSocket->CleanUpData();
					if (iRequestActive)
						{
						iTimeoutTimer->Cancel();
						User::RequestComplete(iRequestStatus, err);
						iRequestActive=EFalse;
						}
					}
				}
			break;
			}
	case EPduReceived:
			{
			CActiveSocket* sock=GetActiveSocketByStatus(EReading);
			if (err!=KErrNone)
				{
                OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_NOTIFICATION_4, "CWspMessageApiAgent::Notification PDU data is received with Err status:%d", err);
				currentSocket->CleanUpData();
				if (iRequestActive && !sock)
					{
					iTimeoutTimer->Cancel();
					User::RequestComplete(iRequestStatus, err);
					iRequestActive=EFalse;
					}
				}
			else
				{
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_NOTIFICATION_5, "CWspMessageApiAgent::Notification PDU data is received");
				if (!sock && iRequestActive)
					{
					iTimeoutTimer->Cancel();
					iRequestActive=EFalse;
					TRAP(err, currentSocket->UnpackPduToWspDataL())
					if (err)
						{
						currentSocket->CleanUpData();
						User::RequestComplete(iRequestStatus, err);
						return;
						}
					err=currentSocket->GetWspData(*iClientReqHeaders, *iClientReqBody, *iClientTransactionId, *iClientWspStatus);
					if (err==KErrNone)
						iLastPduBearer=bearer;
					User::RequestComplete(iRequestStatus, err);
					}
				}
			break;
			}
	default:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGEAPIAGENT_NOTIFICATION_6, "CWspMessageApiAgent::Notification() Unknown Event From Bearer");
		;	
		}
	}

//
//CWdpMessageApiAgent class Method
//
CWdpMessageApiAgent* CWdpMessageApiAgent::NewL()
/**
The static funtion to new a wdp message API agent
@internalComponent
@released since v8.0
@returns the CWdpMessageApiAgent instance
*/
	{
	CWdpMessageApiAgent* me = new(ELeave) CWdpMessageApiAgent();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

CWdpMessageApiAgent::CWdpMessageApiAgent():CWapMessageApiAgent(EWapWdp)
/**
Constructor
@internalComponent
@released since v8.0
*/
	{
	}

CWdpMessageApiAgent::~CWdpMessageApiAgent()
/**
Destructor
@internalComponent
@released since v8.0
*/
	{
	}

void CWdpMessageApiAgent::ConstructL()
/**
Second Phase Constructor
@internalComponent
@released since v8.0
*/
	{
	CWapMessageApiAgent::ConstructL();
	}

TInt CWdpMessageApiAgent::ReceiveWdpMessage(TDes8& aRemoteHost, Wap::TPort& aRemotePort, TDes8& aBuffer, TBool& aTruncated, TRequestStatus& aReqStatus, TUint32 aTimeout)
/**
Receive WDP message from a remote host
@internalComponent
@released since v8.0
@param aRemoteHost (out) the remote host from which the WDP is sent
@param aRemotePort (out) the remote port from which the WDP is sent
@param aBuffer (out) the buffer to contain the received the WDP pdu
@param aTruncated (out) the flag to show if the WDP PDU is trucated ot not
@param aReqStatus (out) the client request status.
@param aTimeOut (in) the time out value
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TRequestStatus* reqStatus=NULL;
	TInt err=KErrNone;
	if (!iIsOpen)
		{
		reqStatus=&aReqStatus;
		User::RequestComplete(reqStatus, KErrNotReady);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGEAPIAGENT_RECEIVEWDPMESSAGE_1, "CWdpMessageApiAgent::ReceiveWdpMessage: Recv From unconnected API");
		return KErrNotReady;
		}
	//if the length has been read
	CActiveSocket* sock=GetActiveSocketByStatus(EGotLength);
	CActiveSocket* sock1=GetActiveSocketByStatus(EReading);
	if (iRequestActive || (!sock && !sock1))
		{
		reqStatus=&aReqStatus;
		User::RequestComplete(reqStatus, KErrInUse);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGEAPIAGENT_RECEIVEWDPMESSAGE_2, "CWdpMessageApiAgent::ReceiveWdpMessage:in wrong state");
		return KErrInUse;
		}
	if (sock1)
		{
		Wap::TBearer bearer=sock1->GetBearerType();
		TSockAddr& remoteAddr=sock1->GetRemoteAddress();
		TRAP(err, CSWSWapMsgUtils::AnalyseAddrL(remoteAddr, bearer, *iClientRemoteHost, *iClientRemotePort))
		if (err)
			{
			reqStatus=&aReqStatus;
			User::RequestComplete(reqStatus, err);
			return err;
			}
		iRequestStatus=&aReqStatus;
		*iRequestStatus = KRequestPending;
		iRequestActive=ETrue;
		iLastReadingError=sock1->GetPduData(*iClientPduBuffer, *iClientTruncated);
		if (!iLastReadingError)
			{
			iLastPduBearer=bearer;
			}
		iAsyncReadCompletion->CallBack();
		return iLastReadingError;
		}
	if (sock)
		{
		if ((err=sock->Receive())==KErrNone)
			{
			iClientPduBuffer=&aBuffer;
			iClientRemoteHost=&aRemoteHost;
			iClientRemotePort=&aRemotePort;
			iClientTruncated=&aTruncated;
			iRequestStatus=&aReqStatus;
			*iRequestStatus = KRequestPending;
			iRequestActive=ETrue;		
			if (aTimeout)
				{
				iTimeoutTimer->After(aTimeout);
				}
			}
		else
			{
			reqStatus=&aReqStatus;
			User::RequestComplete(reqStatus, err);
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGEAPIAGENT_RECEIVEWDPMESSAGE_3, "CWdpMessageApiAgent::ReceiveWdpMessage:err=%d",err);
			return err;
			}
		}
	return KErrNone;
	}

TInt CWdpMessageApiAgent::AwaitRecvDataSize(TPckg<TUint16>& aDataSizePckg, TRequestStatus& aReqStatus)
/**
Receive WDP message PDU length from a remote host
@internalComponent
@released since v8.0
@param aDataSizePckg (out) the length of the PDU
@param aReqStatus (out) the client request status.
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TRequestStatus* reqStatus=NULL;
	if (!iIsOpen)
		{
		reqStatus=&aReqStatus;
		User::RequestComplete(reqStatus, KErrNotReady);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGEAPIAGENT_AWAITRECVDATASIZE_1, "CWdpMessageApiAgent::AwaitRecvDataSize:Wait For Data length From unconnected API");
		return KErrNotReady;
		}
	//if the length has been read or there is some data pending, then error completion
	CActiveSocket* sock=GetActiveSocketByStatus(EGotLength);
	CActiveSocket* sock1=GetActiveSocketByStatus(ERequestingData);
	CActiveSocket* sock2=GetActiveSocketByStatus(EReading);
	if (iRequestActive||sock||sock1||sock2)
		{
		reqStatus=&aReqStatus;
		User::RequestComplete(reqStatus, KErrInUse);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGEAPIAGENT_AWAITRECVDATASIZE_2, "CWdpMessageApiAgent::AwaitRecvDataSize:in wrong state");
		return KErrInUse;
		}
	//if there is a pending length, then get it
	sock=GetActiveSocketByStatus(EPendingLength);
	if (sock)
		{
		iClientDataSize=&aDataSizePckg;
		iRequestStatus=&aReqStatus;
		*iRequestStatus = KRequestPending;
		iRequestActive=ETrue;
		TUint16 length=(TUint16)sock->GetPduSize();
		TPckg<TUint16> des(length);
		iClientDataSize->Copy(des);
		iLastReadingError=KErrNone;
		iAsyncReadCompletion->CallBack();
		return KErrNone;
		}
	//if there is some idle socket, then wait for length
	sock=GetActiveSocketByStatus(EIdle);
	if (sock)
		{
		iRequestActive=ETrue;
		iClientDataSize=&aDataSizePckg;
		iRequestStatus=&aReqStatus;
		*iRequestStatus = KRequestPending;
		//Wait for length on the idle socket
		for (TInt i=0; i<iBearers.Count(); i++)
			{
			if (iBearers[i]->GetDataState()==EIdle)
				{
				iBearers[i]->AwaitRecvDataSize();
				}
			}
		}
	return KErrNone;
	}

TInt CWdpMessageApiAgent::SendWdpMessage(const TDesC8& aBuffer, const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer)
/**
Send WDP message to a remote host
@internalComponent
@released since v8.0
@param aBuffer (in) the data to be sent
@param aRemoteHost (in) the remote host to be sent
@param aRemotePort (in) the remote port to be sent
@param aBearer (in) the Bearer to be used
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (!iIsOpen)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGEAPIAGENT_SENDWDPMESSAGE_1, "CWdpMessageApiAgent::SendWdpMessage:Send WDP to unconnected API");
		return KErrNotReady;
		}
	if (aBearer==Wap::EAll)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGEAPIAGENT_SENDWDPMESSAGE_2, "CWdpMessageApiAgent::SendWdpMessage:Wrong Bearer");
		return Wap::EBearerError;
		}
	TSockAddr remoteAddr;
	TInt err=KErrNone;
	TRAP(err, CSWSWapMsgUtils::BuildAddrL(remoteAddr, aBearer, aRemoteHost, aRemotePort))
	if (err)
		{
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGEAPIAGENT_SENDWDPMESSAGE_3, "CWdpMessageApiAgent::SendWdpMessage:BuildAddrL Err=%d", err);
		return err;
		}
	CActiveSocket* sendSock=GetActiveSocketByBearer(aBearer);
	if (sendSock)
		{
		if (aBearer==Wap::ESMS||aBearer==Wap::EWAPSMS)
			{
			TWapSmsDataCodingScheme codingScheme = EWapSms8BitDCS;
			sendSock->Socket().SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,codingScheme);
			}
		TRequestStatus status;
		sendSock->Socket().SendTo(aBuffer, remoteAddr, 0, status);
		User::WaitForRequest(status);
		err=status.Int();
		}
	else
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGEAPIAGENT_SENDWDPMESSAGE_4, "CWdpMessageApiAgent::SendWdpMessage:No Bearer");
		err=Wap::EBearerError;
		}
	return err;
	}

TInt CWdpMessageApiAgent::SendWdpMessage(const TDesC8& aBuffer)
/**
Send WDP message to a remote host
@internalComponent
@released since v8.0
@param aBuffer (in) the data to be sent
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (!iIsOpen)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGEAPIAGENT_SENDWDPMESSAGE_1_1, "CWdpMessageApiAgent::SendWdpMessage:Send WDP to unconnected API");
		return KErrNotReady;
		}
	if (!iBearers[0])
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGEAPIAGENT_SENDWDPMESSAGE_1_2, "CWdpMessageApiAgent::SendWdpMessage:No Bearer");
		return Wap::EBearerError;
		}
	Wap::TBearer bearer=iBearers[0]->GetBearerType();
	if (bearer==Wap::ESMS||bearer==Wap::EWAPSMS)
		{
		TWapSmsDataCodingScheme codingScheme = EWapSms8BitDCS;
		iBearers[0]->Socket().SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,codingScheme);
		}
	TSockAddr& remoteAddr=iBearers[0]->GetRemoteAddress();
	TRequestStatus status;
	iBearers[0]->Socket().SendTo(aBuffer, remoteAddr, 0, status);
	User::WaitForRequest(status);
	return status.Int();
	}

void CWdpMessageApiAgent::Notification(TInt aMessage, const TDesC8& aData)
/**
Notification from the ActiveSocket
@internalComponent
@released since v8.0
@param aMessage (in) the Event Type
@param aData (in) the Event
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TWapNotificationInfo& info=*(reinterpret_cast<TWapNotificationInfo*>(const_cast<TUint8*>(aData.Ptr())));
	Wap::TBearer bearer=info.iBearer;
	CActiveSocket* currentSocket=GetActiveSocketByBearer(bearer);
	ASSERT(currentSocket!=NULL);
	if (!currentSocket)
		{
		return;
		}
	TInt err=info.iError;		
	switch (TWapNotificationEvent(aMessage))
		{
	case EPduLengthReceived:
			{
			CActiveSocket* sock1=GetActiveSocketByStatus(ERequestingData);
			CActiveSocket* sock2=GetActiveSocketByStatus(EGotLength);
			CActiveSocket* sock3=GetActiveSocketByStatus(EReading);
			if (!sock1 && !sock2 && !sock3 && iRequestActive)
				// Read pdu length from this socket
				{
				iRequestActive=EFalse;
				if (err==KErrNone)
					{
					TUint16 length=(TUint16)currentSocket->GetPduSize();
					TPckg<TUint16> des(length);
					iClientDataSize->Copy(des);	
					}
				else
					{
					currentSocket->CleanUpData();
					}
				User::RequestComplete(iRequestStatus, err);
				}
			break;
			}
	case EPduReceived:
			{
			ASSERT(iRequestActive);
			if (iRequestActive)
				{
				// Read pdu from this socket
				iRequestActive=EFalse;
				iTimeoutTimer->Cancel();
				if (err==KErrNone)
					{
					TSockAddr& remoteAddr=currentSocket->GetRemoteAddress();
					TRAP(err, CSWSWapMsgUtils::AnalyseAddrL(remoteAddr, bearer, *iClientRemoteHost, *iClientRemotePort))
					if (!err)
						{
						err=currentSocket->GetPduData(*iClientPduBuffer, *iClientTruncated);
						if (!err)
							iLastPduBearer=bearer;
						}
					else
						{
						currentSocket->SetDataState(EReading);
						}
					}
				else
					{
					currentSocket->CleanUpData();
					}
				User::RequestComplete(iRequestStatus, err);
				}
			}
		break;
	default:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGEAPIAGENT_NOTIFICATION_1, "CWdpMessageApiAgent::Notification() Unknown Event From Bearer");
		;
		}
	}
