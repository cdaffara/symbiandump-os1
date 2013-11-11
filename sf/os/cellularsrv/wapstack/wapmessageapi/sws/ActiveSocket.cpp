// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ActiveSocketTraces.h"
#endif

#include "ActiveSocket.h"
#include <es_wsms.h>
#include "CLWSPPduHandler.h"
#include "wapmsgerr.h"
#include <wap_sock.h>
#include "WapMessageApiAgent.h"
#include "WapMsgUtils.h"

using namespace Wap;

void CActiveSocket::NewL(RSocketServ& aSocketServ, RPointerArray<CActiveSocket>& aActiveSockets, Wap::TBearer aBearer, TWapMessageType aType, MProgressNotify* aNotify, Wap::TPort aLocalPort, RConnection* aConnection)
/**
The static new function instanciates corresponding Bearers in terms of the input bearer type.
This function is used by Bound Wap APIs which listen the incoming packet to a specific port. 
@internalComponent
@released
@since v8.0
@param aSocketServ the shared RSocketServ instance used in the Wap messaging API which owns this bearer
@param aActiveSockets the bearer array used in the Wap messaging API which owns the bearer
@param aBearer  the bearer to listen on (use EAll for all bearers)
@param aType  the type of the wap message that will received
@param aNotify  the instance to be notified when a wap message is received
@param aLocalPort  the port to listen on
@param aConnection the shared connection from Wap messaging API client
*/
	{
	//Instanciate the corresponding
	switch(aBearer)
		{
		case Wap::ESMS7:
		case Wap::ESMS:
		case Wap::EWAPSMS7:
		case Wap::EWAPSMS:
			{
			CActiveSocket* me = new(ELeave) CActiveSocketSMS(aSocketServ, aNotify, aBearer, aLocalPort);
			CleanupStack::PushL(me);
			me->ConstructL(aType);
			aActiveSockets.AppendL(me);
			CleanupStack::Pop(me);
			break;
			}
		case Wap::EIP:
			{
			CActiveSocket* me = new(ELeave) CActiveSocketUDP(aSocketServ, aNotify, aBearer, aLocalPort, aConnection);
			CleanupStack::PushL(me);
			me->ConstructL(aType);
			aActiveSockets.AppendL(me);
			CleanupStack::Pop(me);
			break;
			}
		case Wap::EAll:
			{
			CActiveSocket* me = new(ELeave) CActiveSocketUDP(aSocketServ, aNotify, Wap::EIP, aLocalPort, aConnection);
			CleanupStack::PushL(me);
			me->ConstructL(aType);          
			CActiveSocket* me1 = new(ELeave) CActiveSocketSMS(aSocketServ, aNotify, Wap::ESMS, aLocalPort);
			CleanupStack::PushL(me1);
			me1->ConstructL(aType);
			aActiveSockets.ReserveL(2); // pre-allocate the memory                                           
			aActiveSockets.AppendL(me1);
			CleanupStack::Pop(me1);
			aActiveSockets.AppendL(me);
			CleanupStack::Pop(me);
			break;
			}
		default:
		    {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CACTIVESOCKET_NEWL_1, "CActiveSocket::NewL Unknown Bearer Type");
			User::Leave(Wap::EBearerError);
		    }
		}
	}

void CActiveSocket::NewL(RSocketServ& aSocketServ, RPointerArray<CActiveSocket>& aActiveSockets, Wap::TBearer aBearer, TWapMessageType aType, MProgressNotify* aNotify, const TSockAddr& aRemoteAddr, RConnection* aConnection)
/**
The static new function instanciates corresponding Bearers in terms of the input bearer type.
This function is used by Fully specified Wap APIs which will open a socket with a single, named remote host. 
@internalComponent
@released
@since v8.0
@param aSocketServ the shared RSocketServ instance used in the Wap messaging API which owns this bearer
@param aActiveSockets the bearer array used in the Wap messaging API which owns the bearer
@param aBearer the bearer to listen on (use EAll for all bearers)
@param aType the type of the wap message that will received
@param aNotify the instance to be notified when a wap message is received
@param aRemoteAddr the remote host to be communicate with
@param aConnection the shared connection from Wap messaging API client
*/
	{
	//Instanciate the corresponding
	switch(aBearer)
		{
		case Wap::ESMS7:
		case Wap::ESMS:
		case Wap::EWAPSMS7:
		case Wap::EWAPSMS:
			{
			CActiveSocket* me = new(ELeave) CActiveSocketSMS(aSocketServ, aNotify, aBearer, aRemoteAddr);
			CleanupStack::PushL(me);
			me->ConstructL(aType);
            aActiveSockets.AppendL(me);
			CleanupStack::Pop(me);
			break;
			}
		case Wap::EIP:
			{
			CActiveSocket* me = new(ELeave) CActiveSocketUDP(aSocketServ, aNotify, aBearer, aRemoteAddr, aConnection);
			CleanupStack::PushL(me);
			me->ConstructL(aType);
            aActiveSockets.AppendL(me);
			CleanupStack::Pop(me);
			break;
			}
		case Wap::EAll:
			{
			CActiveSocket* me = new(ELeave) CActiveSocketUDP(aSocketServ, aNotify, aBearer, aRemoteAddr, aConnection);
			CleanupStack::PushL(me);
			me->ConstructL(aType);
			CActiveSocket* me1 = new(ELeave) CActiveSocketSMS(aSocketServ, aNotify, aBearer, aRemoteAddr);
			CleanupStack::PushL(me1);
			me1->ConstructL(aType);
            aActiveSockets.ReserveL(2); // pre-allocate the memory    
			aActiveSockets.AppendL(me1);
            CleanupStack::Pop(me1);
            aActiveSockets.AppendL(me);
            CleanupStack::Pop(me);
			break;
			}
		default:
		    {
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CACTIVESOCKET_NEWL_1_1, "CActiveSocket::NewL Unknown Bearer Type");
			User::Leave(Wap::EBearerError);
		    }
		}
	}

CActiveSocket::CActiveSocket(RSocketServ& aSocketServ, Wap::TBearer aBearerType, MProgressNotify* aNotify, Wap::TPort aLocalPort)
:CActive(EPriorityStandard), iLocalAddr(0), iSocketServ(aSocketServ), iBearerType(aBearerType), iLocalPort(aLocalPort), iSocketState(ESocketIdle),iNotify(aNotify), iBuf(0,0), iRxlength(0), iBufCon(0,0)
/**
Constructor of bearer base class for Bound Wap APIs
@internalComponent
@released
@since v8.0
*/
	{
	CActiveScheduler::Add(this);
	}

CActiveSocket::CActiveSocket(RSocketServ& aSocketServ, Wap::TBearer aBearerType, MProgressNotify* aNotify, const TSockAddr& aRemoteAddr, Wap::TPort aLocalPort): CActive(EPriorityStandard), 
iRemoteAddr(aRemoteAddr), iLocalAddr(0), iSocketServ(aSocketServ), iBearerType(aBearerType), iLocalPort(aLocalPort), iSocketState(ESocketIdle), iNotify(aNotify), iBuf(0,0), iRxlength(0), iBufCon(0,0)
/**
Constructor of bearer base class for fully specified Wap APIs
@internalComponent
@released
@since v8.0
*/
	{
	CActiveScheduler::Add(this);
	}

CActiveSocket::~CActiveSocket()
/**
Destructor
@internalComponent
@released
@since v8.0
*/
	{
	Cancel();
	iSocket.Close();
	if (iMessageRecord) 
		{
		delete iMessageRecord;
		}
	}

void CActiveSocket::ConstructL(TWapMessageType aType)
/**
Second Phase Constructor
@internalComponent
@released
@param aType the type of Wap message which is received. 
@since v8.0
*/
	{
	iMessageRecord=CWapMessageRecord::NewL(aType);
	}

RSocket& CActiveSocket::Socket()
/**
To get the RSocket instance ownd by this bearer
@internalComponent
@released
@since v8.0
@returns the reference of the RSocket instance.
*/
	{
	return iSocket;
	}

Wap::TBearer CActiveSocket::GetBearerType()
/**
To get the bearer type of this bearer
@internalComponent
@released
@since v8.0
@returns the bearer type
*/
	{
	return iBearerType;
	}

TSockAddr& CActiveSocket::GetLocalAddress()
/**
To get the local address of this bearer
@internalComponent
@released
@since v8.0
@returns the lcoal address instance
*/
	{
	iSocket.LocalName(iLocalAddr);
	return iLocalAddr;
	}

TSockAddr& CActiveSocket::GetRemoteAddress()
/**
To get the remote address of the last received packet
@internalComponent
@released
@since v8.0
@returns the remote address instance
*/
	{
	return iRemoteAddr;
	}

TInt CActiveSocket::GetRemoteAddress(HBufC8*& aAddr)
/**
To get the remote address of the last received packet
@internalComponent
@released
@since v8.0
@param aAddr the remote host name
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TRAPD(err, aAddr=iRemoteAddr.AllocL())
	if (err==KErrNone)
		{
		Wap::TPort port;
		TPtr8 des=aAddr->Des();
		TRAP(err, CSWSWapMsgUtils::AnalyseAddrL(iRemoteAddr, iBearerType, des, port))
		}
	else
		{
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CACTIVESOCKET_GETREMOTEADDRESS_1, "CActiveSocket::GetRemoteAddress: Alloc Memory Err=%d", err);
		}
	return err;
	}

TInt CActiveSocket::GetLocalPort(Wap::TPort& aLocalPort)
/**
To get the lcoal port of this bearer
@internalComponent
@released
@since v8.0
@param aLocalPort the local port of this bearer
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	GetLocalAddress();
	aLocalPort=Wap::TPort(iLocalAddr.Port());
	return KErrNone;
	}

TUint32 CActiveSocket::GetPduSize()
/**
To get the received Wdp Pdu length
@internalComponent
@released
@since v8.0
@returns the length of the received Wdp pdu.
*/
	{
	return iMessageRecord->GetPduSize();
	}

TWapMessageState CActiveSocket::GetDataState()
/**
To get the state of the data that is being received
@internalComponent
@released
@since v8.0
@returns the state of the data that is being received
*/
	{
	return iMessageRecord->GetDataState();
	}

void CActiveSocket::SetDataState(TWapMessageState aState)
/**
To set the state of the data that is being received
@internalComponent
@released
@since v8.0
@param aState the state of the data that is being received
*/
	{
	iMessageRecord->SetDataState(aState);
	}

TInt CActiveSocket::GetPduData(TDes8& aBuffer, TBool& aTruncated)
/**
To get the received Wdp pdu.
@internalComponent
@released
@since v8.0
@param aBuffer the buffer to read the received WDP pdu
@param aTruncated the flag to represent if the data has be truncated or not
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iMessageRecord->GetPduData(aBuffer, aTruncated);
	}

TInt CActiveSocket::GetWspData(TDes8& aWspHeader, TDes8& aWspBody, TUint8& aTransactionId, TWSPStatus& aStatus)
/**
To get the received Wsp header, body, tranaction ID and Wsp status.
@internalComponent
@released
@since v8.0
@param aWspHeader the buffer to read the received Wsp header
@param aWspBody the buffer to read the received Wsp body
@param aTransactionId the transaction ID of the received Wsp Message
@param aStatus the Wsp status of  the received Wsp Message
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	return iMessageRecord->GetWspData(aWspHeader, aWspBody, aTransactionId, aStatus);
	}
void CActiveSocket::UnpackPduToWspDataL()
/**
To extract the Wsp header, body, transaction ID and status from the received WDP pdu
@internalComponent
@released
@since v8.0
*/
	{
	iMessageRecord->UnpackPduToWspDataL();
	}

void CActiveSocket::CleanUpData()
	{
	TPtr8 zero(0,0);
	iBuf.Set(zero);
	iBufCon.Set(zero);
	iMessageRecord->CleanUpData();
	}

/** SMS active socket
*/
CActiveSocketSMS::CActiveSocketSMS(RSocketServ& aSocketServ, MProgressNotify* aNotify, Wap::TBearer aBearer, Wap::TPort aLocalPort)
:CActiveSocket(aSocketServ, aBearer, aNotify, aLocalPort)
/**
Constructor of SMS bearer for Bound Wap APIs
@internalComponent
@released
@since v8.0
*/
	{
	}

CActiveSocketSMS::CActiveSocketSMS(RSocketServ& aSocketServ, MProgressNotify* aNotify, Wap::TBearer aBearer, const TSockAddr& aRemoteAddr)
:CActiveSocket(aSocketServ, aBearer, aNotify, aRemoteAddr, (Wap::TPort)EWapPortUnspecified)
/**
Constructor of SMS bearer for fully specified Wap APIs
@internalComponent
@released
@since v8.0
*/
	{
	}

CActiveSocketSMS::~CActiveSocketSMS() 
/**
Destructor of SMS bearer for fully specified Wap APIs
@internalComponent
@released
@since v8.0
*/
	{
	}

void CActiveSocketSMS::ConstructL(TWapMessageType aType)
/**
Second Phase Constructor
@internalComponent
@released
@param aType the type of Wap message which is received.
@since v8.0
*/
	{
	CActiveSocket::ConstructL(aType);
	User::LeaveIfError(iSocket.Open(iSocketServ, KWAPSMSAddrFamily, KSockDatagram, KWAPSMSDatagramProtocol));
	User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionNewStyleClient,KWapSmsOptionLevel, 0));
	TWapAddr wapAddr;
	wapAddr.SetWapPort(TWapPortNumber(iLocalPort));
	TInt err=iSocket.Bind(wapAddr);
	if (err==KErrInUse)
		{
		User::Leave(Wap::EPortAlreadyBound);
		}
	}

TInt CActiveSocketSMS::AwaitRecvDataSize()
/**
Read the received Wdp pdu length.
@internalComponent
@released
@since v8.0
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TPckgBuf<TUint32>* length=iMessageRecord->GetPduSizeRef();
	iSocket.Ioctl(KSOGetLength, iStatus, length, KSolWapProv);
	iMessageRecord->SetDataState(ERequestingLength);
	iSocketState=ESocketWaitingForLength;
	SetActive();
	return KErrNone;
	}

TInt CActiveSocketSMS::Receive()
/**
Read the received Wdp pdu.
@internalComponent
@released
@since v8.0
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TInt err=KErrNone;
	TRAP(err, iMessageRecord->CreatePduBufferL(EFalse))
	if (err!=KErrNone)
		{
		return err;
		}
	HBufC8*& pdu=iMessageRecord->GetPduPtr();
	iBuf.Set(pdu->Des());
	iSocket.RecvFrom(iBuf, iRemoteAddr, 0, iStatus);
	iMessageRecord->SetDataState(ERequestingData);
	iSocketState=ESocketWaitingForData;
	SetActive();
	return KErrNone;
	}

void CActiveSocketSMS::RunL() 
/**
Overload the CActive virtual methods
@internalComponent
@released
@since v8.0
*/
	{
	switch (iSocketState)
		{
	case ESocketWaitingForLength:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CACTIVESOCKETSMS_RUNL_1, "CActiveSocketSMS::RunL() ESocketWaitingForLength");
			iMessageRecord->SetDataState(EPendingLength);
			iSocketState=ESocketIdle;
			TWapNotificationInfo info(iBearerType, iStatus.Int());
			TWapNotificationInfoBuf infoBuf(info);
			iNotify->Notification(EPduLengthReceived, infoBuf);
			break;
			}
	case ESocketWaitingForData:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CACTIVESOCKETSMS_RUNL_2, "CActiveSocketSMS::RunL() ESocketWaitingForData");
			iMessageRecord->SetDataState(EPendingData);
			iSocketState=ESocketIdle;
			TWapNotificationInfo info(iBearerType, iStatus.Int());
			TWapNotificationInfoBuf infoBuf(info);
			iNotify->Notification(EPduReceived, infoBuf);
			iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0);
			break;
			}
	default:
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CACTIVESOCKETSMS_RUNL_3, "CActiveSocketSMS::RunL() Unknown State");
		}
	}
void CActiveSocketSMS::DoCancel() 
/**
Overload the CActive virtual methods
@internalComponent
@released
@since v8.0
*/
	{
	switch (iSocketState)
		{
	case ESocketWaitingForLength:
			{
			iSocket.CancelIoctl();
			break;
			}
	case ESocketWaitingForData:
			{
			iSocket.CancelRecv();
			break;
			}
	default:
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CACTIVESOCKETSMS_DOCANCEL_1, "CActiveSocketSMS::DoCancel() Unknown State");	
		}
	}

//
// UDP active socket
//
CActiveSocketUDP::CActiveSocketUDP(RSocketServ& aSocketServ, MProgressNotify* aNotify, Wap::TBearer aBearer, Wap::TPort aLocalPort, RConnection* aConnection)
:CActiveSocket(aSocketServ, aBearer, aNotify, aLocalPort),iConnection(aConnection)
/**
Constructor for Bound Wap APIs
@internalComponent
@released
@since v8.0
*/
	{
	}
	
CActiveSocketUDP::CActiveSocketUDP(RSocketServ& aSocketServ, MProgressNotify* aNotify, Wap::TBearer aBearer, const TSockAddr& aRemoteAddr, RConnection* aConnection)
:CActiveSocket(aSocketServ, aBearer, aNotify, aRemoteAddr,0), iConnection(aConnection)
/**
Constructor for FullySpec Wap APIs
@internalComponent
@released
@since v8.0
*/
	{
	}

void CActiveSocketUDP::ConstructL(TWapMessageType aType)
/**
Second Phase constructor
@internalComponent
@released
@since v8.0
*/
	{
	CActiveSocket::ConstructL(aType);
	if (!iConnection)
		{
		User::LeaveIfError(iSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolInetUdp));
		}
	else
		{
		User::LeaveIfError(iSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolInetUdp, *iConnection));
		}
	TInetAddr inetAddr(iLocalPort);
	TInt err=iSocket.Bind(inetAddr);
	if (err==KErrInUse)
		{
		User::Leave(Wap::EPortAlreadyBound);
		}
	}

CActiveSocketUDP::~CActiveSocketUDP() 
/**
Destructor
@internalComponent
@released
@since v8.0
*/
	{
	}

TInt CActiveSocketUDP::AwaitRecvDataSize()
/**
Wait for Pdu data size
@internalComponent
@released
@since v8.0
*/
	{
	iRxlength=0;
	TRAPD(err, iMessageRecord->CreatePduBufferL(ETrue))
	if (err!=KErrNone)
		{
		return err;
		}
	HBufC8*& pdu=iMessageRecord->GetPduPtr();
	iBuf.Set(pdu->Des());
	iSocket.RecvFrom(iBuf, iRemoteAddr, 0, iStatus, iRxlength);
	iMessageRecord->SetDataState(ERequestingLength);
	iSocketState=ESocketWaitingForLength;
	SetActive();
	return KErrNone;	
	}

TInt CActiveSocketUDP::Receive()
/**
Receive the pdu
@internalComponent
@released
@since v8.0
*/
	{
	if(iMessageRecord->GetDataState()==EContinuous)
		{
		TRAPD(err, iMessageRecord->CreatePduBufferL(ETrue))
		if (err!=KErrNone)
			{
			return err;
			}
		HBufC8*& pdu=iMessageRecord->GetPduPtr();
		iBuf.Set(pdu->Des());
		iBuf.SetLength(iBuf.Length()+1);
		iBufCon.Set(&iBuf[iBuf.Length()-1],0,iBuf.MaxLength()-iBuf.Length());
		iBuf.SetLength(iBuf.Length()+iRxlength()-1);
		iSocket.RecvFrom(iBufCon, iRemoteAddr, KSockReadContinuation, iStatus,iRxlength);
		iMessageRecord->SetDataState(ERequestingData);
		iSocketState=ESocketWaitingForData;
		SetActive();	
		}
	else
		{
		iMessageRecord->SetDataState(ERequestingData);
		iSocketState=ESocketWaitingForData;
		iStatus = KRequestPending;
		SetActive();
		TRequestStatus* reqStatus=&iStatus;
		User::RequestComplete(reqStatus, KErrNone);
		}
	return KErrNone;
	}

void CActiveSocketUDP::RunL()
/**
RunL()
@internalComponent
@released
@since v8.0
*/
	{
	switch (iSocketState)
		{
	case ESocketWaitingForLength:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CACTIVESOCKETUDP_RUNL_1, "CActiveSocketUDP::RunL() ESocketWaitingForLength");        
			iMessageRecord->SetPduSize(iBuf.Length()+ iRxlength());
			if(iRxlength() > 0)
				{
				iMessageRecord->SetDataState(EContinuous);
				}
			else 
				{
				iMessageRecord->SetDataState(EPendingLength);
				}
			iSocketState=ESocketIdle;
			TWapNotificationInfo info(iBearerType, iStatus.Int());
			TWapNotificationInfoBuf infoBuf(info);
			iNotify->Notification(EPduLengthReceived, infoBuf);
			break;
			}
	case ESocketWaitingForData:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CACTIVESOCKETUDP_RUNL_2, "CActiveSocketUDP::RunL() ESocketWaitingForData");
			iMessageRecord->SetDataState(EPendingData);
			iSocketState=ESocketIdle;
			TWapNotificationInfo info(iBearerType, iStatus.Int());
			TWapNotificationInfoBuf infoBuf(info);
			iNotify->Notification(EPduReceived, infoBuf);
			break;
			}
	default:
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CACTIVESOCKETUDP_RUNL_3, "CActiveSocketUDP::RunL() Unknown State");
		break;
		}
	}

void CActiveSocketUDP::DoCancel()
/**
Cancel the outstanding request on UDP bearer
@internalComponent
@released
@since v8.0
*/
	{
	switch (iSocketState)
		{
	case ESocketWaitingForLength:
			{
			iSocket.CancelRecv();
			break;
			}
	case ESocketWaitingForData:
			{
			break;
			}
	default:
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CACTIVESOCKETUDP_DOCANCEL_1, "CActiveSocketUDP::DoCancel() Unknown State");	
		}
	}

//
//CWapMessageRecord
//
CWapMessageRecord* CWapMessageRecord::NewL(TWapMessageType aType)
/**
The static funtion to instanciate the Pdu data record
@internalComponent
@released
@since v8.0
@param aType the type of Wap message which is received.
@returns the data record instance.
*/
	{
	CWapMessageRecord* me;
	if (aType==EWapWsp)
		{
		me = new(ELeave) CWspMessageRecord();
		}
	else
		{
		me = new(ELeave) CWdpMessageRecord();
		}
	return me;
	}

CWapMessageRecord::CWapMessageRecord()
/**
Constructor
@internalComponent
@released
@since v8.0
*/
	{
	}

CWapMessageRecord::~CWapMessageRecord()
/**
Destructor
@internalComponent
@released
@since v8.0
*/
	{
	if (iPdu)
		{
		delete iPdu;
		}
	}

TWapMessageState CWapMessageRecord::GetDataState()
/**
To get the state of the data that is being received
@internalComponent
@released
@since v8.0
returns the state of the data that is being received
*/
	{
	return iState;
	}

void CWapMessageRecord::SetDataState(TWapMessageState aState)
/**
To set the state of the data that is being received
@internalComponent
@released
@since v8.0
@param the state of the data that is being received
*/
	{
	iState=aState;
	}

TPckgBuf<TUint32>* CWapMessageRecord::GetPduSizeRef()
/**
To get the buffer which is used to contain the received data length
@internalComponent
@released
@since v8.0
@returns the pointer to the buffer length
*/
	{
	return &iDataLength;
	}

void CWapMessageRecord::SetPduSize(TUint32 aLength)
	{
	iDataLength=aLength;
	}

TUint32 CWapMessageRecord::GetPduSize()
/**
To get the received wdp pdu length
@internalComponent
@released
@since v8.0
@returns the received wdp pdu length
*/
	{
	iState=EGotLength;
	return iDataLength();
	}

HBufC8*& CWapMessageRecord::GetPduPtr()
/**
To get the received wdp pdu.
@internalComponent
@released
@since v8.0
@returns the pointer the received wdp buffer
*/
	{
	return iPdu;
	}

void CWapMessageRecord::CreatePduBufferL(TBool aFixLengthFlag)
/**
create the wdp pdu buffer according to the length
@internalComponent
@released
@since v8.0
*/
	{
	if (iState==EContinuous)
		{
		iPdu = iPdu->ReAllocL(iDataLength());
		}
	else if (!aFixLengthFlag)
		{
		iPdu=HBufC8::NewL(iDataLength());
		}
	else
		{
		iPdu=HBufC8::NewL(KMaxUdpBearerDataBufferLength);
		}
	}

void CWapMessageRecord::CleanUpData()
/**
clean up the receive buffer.
@internalComponent
@released
@since v8.0
*/
	{
	if (iPdu)
		{
		delete iPdu;
		iPdu=NULL;
		}
	iDataLength.FillZ();
	iState=EIdle;
	}

//
//CWspMessageRecord methods
//
CWspMessageRecord::CWspMessageRecord()
/**
Constructor
@internalComponent
@released
@since v8.0
*/
	{
	}

CWspMessageRecord::~CWspMessageRecord()
/**
Destructor
@internalComponent
@released
@since v8.0
*/
	{
	if (iWspHeader)
		{
		delete iWspHeader;
		}
	if (iWspBody)
		{
		delete iWspBody;
		}
	}

void CWspMessageRecord::UnpackPduToWspDataL()
/**
To unpack the received wdp pdu to wsp message.
@internalComponent
@released
@since v8.0
*/
	{
	TWSPPduType type;
	CCLWSPPduHandler::UnpackWSPPduL(iPdu, type, iWspHeader, iWspBody, iTransactionId, iWspStatus);
	}

TInt CWspMessageRecord::GetWspData(TDes8& aWspHeader, TDes8& aWspBody, TUint8& aTransactionId, TWSPStatus& aWspStatus)
/**
To read the wsp message from the buffer
@internalComponent
@released
@since v8.0
@param aWspHeader the buffer to contain the wsp header
@param aWspBody the buffer to contain the wsp body
@param iTransactionId the received transaction ID
@param aWspStatus the received wsp status
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (!iWspHeader && !iWspBody)
		{
		// if no data, should not be here at all
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWSPMESSAGERECORD_GETWSPDATA_1, "CWspMessageRecord::GetWspData() No Data Available");
		CleanUpData();
		return KErrBadDescriptor;
		}
	TInt ret=KErrNone;
	//Copy the transaction ID
	aTransactionId=iTransactionId;
	aWspStatus=iWspStatus;
	//Copy the header
	TInt bufferLength;
	if (iWspHeader)
		{
		bufferLength=aWspHeader.MaxLength();
		TPtrC8 remainHeader=iWspHeader->Mid(iHeaderOffset);
		//Client Header buffer is not long enough
		if (bufferLength<remainHeader.Length())
			{
			aWspHeader.Copy(remainHeader.Ptr(), bufferLength);
			iHeaderOffset+=bufferLength;
			iState=EReading;
			ret=Wap::EMoreData;
			}
		else
			{
			aWspHeader.Copy(remainHeader);
			iHeaderOffset=0;
			delete iWspHeader;
			iWspHeader=NULL;
			}
		}
	if (iWspBody)
		{
		//Copy the Body
		bufferLength=aWspBody.MaxLength();
		TPtrC8 remainBody=iWspBody->Mid(iBodyOffset);

		//Client Header buffer is not long enough
		if (bufferLength<remainBody.Length())
			{
			aWspBody.Copy(remainBody.Ptr(), bufferLength);
			iBodyOffset+=bufferLength;
			iState=EReading;
			ret=Wap::EMoreData;
			}
		else
			{
			aWspBody.Copy(remainBody);
			iBodyOffset=0;
			delete iWspBody;
			iWspBody=NULL;
			}
		}
	//All Wsp Data has been read.
	if (ret==KErrNone)
		{
		delete iPdu;
		iPdu=NULL;
		iDataLength.FillZ();
		iState=EIdle;
		}
	return ret;
	}

TInt CWspMessageRecord::GetPduData(TDes8& /*aBuffer*/, TBool& /*aTruncated*/)
/**
Not supported
@internalComponent
@released
@since v8.0
*/
	{//Should not be used
	return KErrNotSupported;
	}

void CWspMessageRecord::CleanUpData()
/**
To clean up the wsp related data
@internalComponent
@released
@since v8.0
*/
	{
	CWapMessageRecord::CleanUpData();
	if (iWspHeader)
		{
		delete iWspHeader;
		iWspHeader=NULL;
		}
	if (iWspBody)
		{
		delete iWspBody;
		iWspBody=NULL;
		}
	iTransactionId=0;
	iHeaderOffset=0;
	iBodyOffset=0;
	}
//
//CWspMessageRecord method
//
CWdpMessageRecord::CWdpMessageRecord()
/**
Constructor
@internalComponent
@released
@since v8.0
*/
	{
	}

CWdpMessageRecord::~CWdpMessageRecord()
/**
Destructor
@internalComponent
@released
@since v8.0
*/
	{
	}

TInt CWdpMessageRecord::GetPduData(TDes8& aBuffer, TBool& aTruncated)
/**
To read the received Wdp pdu
@internalComponent
@released
@since v8.0
@param aBuffer the buffer to contain the received wdp pdu
@param aTruncated the flag to represent if the data is truncated or not
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	if (!iPdu)
		{
		//Should not be here at all
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWDPMESSAGERECORD_GETPDUDATA_1, "CWdpMessageRecord::GetPduData No Data Available");
		CleanUpData();
		return KErrBadDescriptor;
		}
	TInt ret=KErrNone;
	TInt bufLength=aBuffer.MaxLength();
	TPtrC8 remainPdu=iPdu->Mid(iPduOffset);
	if (bufLength<remainPdu.Length())
		{
		aBuffer.Copy(remainPdu.Ptr(), bufLength);
		iPduOffset+=bufLength;
		iState=EReading;
		aTruncated=ETrue;
		ret=Wap::EMoreData;
		}
	else
		{
		aBuffer.Copy(remainPdu);
		aTruncated=EFalse;
		CleanUpData();
		}
	return ret;
	}

void CWdpMessageRecord::UnpackPduToWspDataL()
/**
Not supported
@internalComponent
@released
@since v8.0
*/
	{
	//Should not be used
	User::Leave(KErrNotSupported);
	}

TInt CWdpMessageRecord::GetWspData(TDes8& /*aWspHeader*/, TDes8& /*aWspBody*/, TUint8& /*aTransactionId*/, TWSPStatus& /*aWspStatus*/)
/**
Not supported
@internalComponent
@released
@since v8.0
*/
	{
	//Should not be used
	return KErrNotSupported;
	}

void CWdpMessageRecord::CleanUpData()
/**
To clean up the wdp related data
@internalComponent
@released
@since v8.0
*/
	{
	CWapMessageRecord::CleanUpData();
	iPduOffset=0;
	}
